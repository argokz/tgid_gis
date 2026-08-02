"""Кто обращается к каждому отношению БД — одна инвентаризация на всех.

Три прежних инструмента (sety_tables.py, gid8_tables.py,
public_inventory.py) видят каждый свою часть кода. Из-за этого таблицы
трижды уезжали на чердак как «неиспользуемые» и возвращались обратно
(шаги 58, 60). Здесь все потребители обходятся одним проходом, а вывод
строится от отношения к коду, а не от кода к отношению: вопрос всегда
один — «кто это трогает».

    python tools/db_consumers.py --db tgid_gis
    python tools/db_consumers.py --db tgid_gis --table nodes
    python tools/db_consumers.py --db tgid_gis --json docs/schema/db_consumers.json

Свидетельства разной силы и не смешиваются:

  sql    — имя стоит после FROM / JOIN / INSERT INTO / UPDATE / DELETE;
  упом.  — имя встречается в файле просто словом: переменная, .ui,
           шаблон запроса, имя слоя в настройках;
  дин.   — имя таблицы подставляется в запрос на ходу (%s, %1, «+»).
           Такие места статически не разбираются, поэтому вывод «никто
           не обращается» действителен только вместе с их числом.

Имя без схемы разрешается по search_path того потребителя, который его
написал: у gid8 первой идёт compat, у остальных — порядок самой БД.
Поэтому одно и то же `nodes` у gid8 и у движка расчёта — разные
отношения, и считаются они порознь.
"""
import argparse
import collections
import io
import json
import os
import re
import sys

import psycopg2

TOOLS = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(TOOLS)
BASE = os.path.dirname(REPO)

# Порядок поиска самой БД (pg_db_role_setting) и порядок gid8, который
# тот задаёт себе сам в gid8/db/db.cpp рядом с set application_name.
DB_PATH = ('public', 'net', 'ref', 'calc', 'meta', 'addr', 'doc', 'el',
           'ops', 'org')
GID8_PATH = ('compat',) + DB_PATH

# Потребители: имя, каталог, порядок поиска. Каталог сборки включён
# намеренно — приложение читает внешние .sql оттуда, а не из репозитория,
# и там может остаться имя, которого в исходниках уже нет.
CONSUMERS = [
    ('sety',      os.path.join(BASE, 'gid8', 'python', 'sety', 'sety'), DB_PATH),
    ('gid8',      os.path.join(REPO, 'gid8'),                           GID8_PATH),
    ('gid8-сбор', r'H:\build\gid8-tgid-gis-20260802',                   GID8_PATH),
    ('gid6',      os.path.join(BASE, 'gid6'),                           DB_PATH),
    ('gid6-репо', os.path.join(REPO, 'gid6'),                           DB_PATH),
    ('tgid_app',  os.path.join(REPO, 'tgid_app'),                       DB_PATH),
    ('potr5',     os.path.join(BASE, 'potr5'),                          DB_PATH),
    ('sql',       os.path.join(REPO, 'sql'),                            DB_PATH),
    ('converter', os.path.join(REPO, 'converter'),                      DB_PATH),
    ('tools',     TOOLS,                                                DB_PATH),
]

EXT = ('.py', '.cpp', '.cxx', '.cc', '.c', '.h', '.hpp', '.sql', '.ui',
       '.ps1', '.json')
SKIP_DIRS = {'.git', '.svn', 'build', 'debug', 'release', '__pycache__',
             'node_modules', '.venv', 'venv', 'x64', 'Win32'}

# Имя после FROM/JOIN/... Отрицательный просмотр вперёд отсекает
# питоновское "from X import": без него в таблицы попадали модули движка.
RE_SQL = re.compile(
    r'\b(from|join|insert\s+into|update|delete\s+from|truncate(?:\s+table)?)'
    r'\s+("?[A-Za-z_]\w*"?(?:\s*\.\s*"?[A-Za-z_]\w*"?)?)'
    r'(?!\s*\()(?!\s+import\b)',
    re.I)

# FROM и JOIN встречаются и в английской прозе комментариев («read from
# the table»), поэтому от них требуется соседство с глаголом запроса.
# INSERT/UPDATE/DELETE/TRUNCATE опознают себя сами.
RE_SQLCTX = re.compile(
    r'\b(?:select|insert|update|delete|with|merge|exec\w*|prepare|'
    r'setquery|setquerystring)\b', re.I)
CTX_WINDOW = 400

# То же место, но вместо имени — подстановка. Считаем, не разбирая.
RE_DYN = re.compile(
    r'\b(?:from|join|insert\s+into|update|delete\s+from)\s*'
    r'(?:%[sd1-9]|\{|\+|"\s*\+|\'\s*\+|\$)',
    re.I)

# Имена, которые код создаёт сам: временные таблицы и представления.
# Их отсутствие в БД — не пропажа.
RE_MADE = re.compile(
    r'\bcreate\s+(?:or\s+replace\s+)?(?:global\s+|local\s+)?'
    r'(?:temp\w*\s+)?(?:unlogged\s+)?(?:materialized\s+)?'
    r'(?:table|view)\s+(?:if\s+not\s+exists\s+)?'
    r'("?[A-Za-z_#]\w*"?(?:\.\s*"?[A-Za-z_]\w*"?)?)',
    re.I)

RE_WORD = re.compile(r'[A-Za-z_]\w{2,}')

# Служебные слова, попадающие в захват после FROM/JOIN по ошибке
SKIP_NAMES = {'select', 'values', 'set', 'where', 'dual', 'lateral', 'only',
              'into', 'unnest', 'generate_series', 'as', 'on',
              'and', 'or', 'not', 'null', 'exists', 'case', 'when', 'then',
              'left', 'right', 'inner', 'outer', 'full', 'cross', 'order',
              'group', 'having', 'limit', 'offset', 'union', 'all',
              'import', 'os', 'sys', 're', 'math', 'time', 'datetime',
              'typing', 'collections', 'pathlib'}

# Слова английской прозы. Применяются только к именам, которых нет в БД:
# если такое слово окажется настоящим отношением, оно разрешится раньше
# и до этого списка не дойдёт.
STOP_EN = {
    'a', 'an', 'the', 'this', 'that', 'these', 'those', 'it', 'its', 'our',
    'your', 'their', 'his', 'her', 'them', 'there', 'here', 'now', 'then',
    'each', 'any', 'both', 'some', 'such', 'same', 'other', 'another',
    'one', 'two', 'three', 'first', 'last', 'next', 'previous', 'new',
    'old', 'current', 'main', 'real', 'top', 'above', 'below', 'more',
    'most', 'less', 'least', 'many', 'much', 'which', 'what', 'who',
    'where', 'why', 'how', 'data', 'file', 'files', 'line', 'lines',
    'list', 'item', 'items', 'value', 'values', 'name', 'names', 'type',
    'code', 'user', 'users', 'text', 'size', 'count', 'class', 'object',
    'objects', 'view', 'views', 'index', 'key', 'keys', 'page', 'form',
    'field', 'fields', 'loss', 'warning', 'error', 'errors', 'reading',
    'writing', 'result', 'results', 'source', 'target', 'string', 'number',
    'point', 'points', 'position', 'memory', 'buffer', 'stream', 'input',
    'output', 'python', 'pyqt', 'qt', 'collection', 'array', 'vector',
    'map', 'model', 'widget', 'window', 'dialog', 'menu', 'button',
    'propertie', 'properties', 'property', 'method', 'function', 'client',
    'server', 'database', 'db', 'query', 'queries', 'row', 'rows',
    'column', 'columns', 'record', 'records', 'left', 'right', 'start',
    'end', 'begin', 'here_doc', 'self', 'cls', 'none', 'true', 'false'}

# Схемы, которые не наши: их имена в отчёте — шум.
SYSTEM_SCHEMAS = {'pg_catalog', 'information_schema', 'pg_toast'}
# Служебные объекты PostGIS живут в public, но переносить их некуда.
POSTGIS = {'geometry_columns', 'geography_columns', 'spatial_ref_sys',
           'raster_columns', 'raster_overviews'}
# Потребители, которые сами и переносят: их обращение к чердаку — это
# скрипт переноса, а не работающее приложение.
REPO_SIDE = {'sql', 'tools', 'converter'}


def norm(name):
    """`"Net" . "Nodes"` -> `net.nodes`."""
    return name.replace('"', '').replace(' ', '').lower()


def scan(root):
    """Один каталог: имена в SQL, слова-упоминания, свои таблицы, дин."""
    sql = collections.defaultdict(set)     # имя -> файлы
    words = collections.defaultdict(set)   # слово -> файлы
    made = set()
    dyn = collections.Counter()
    nfiles = 0
    for dirpath, dirnames, files in os.walk(root):
        dirnames[:] = [d for d in dirnames if d not in SKIP_DIRS]
        for fn in files:
            if not fn.lower().endswith(EXT):
                continue
            p = os.path.join(dirpath, fn)
            # Часть файлов старых интерфейсов не в UTF-8. Читаем терпимо:
            # имена отношений всё равно латиницей.
            try:
                text = io.open(p, encoding='utf-8', errors='replace').read()
            except OSError:
                continue
            nfiles += 1
            rel = os.path.relpath(p, root)
            for m in RE_SQL.finditer(text):
                name = norm(m.group(2))
                if name in SKIP_NAMES or name.isdigit():
                    continue
                if m.group(1).lower() in ('from', 'join') and not \
                        RE_SQLCTX.search(text, max(0, m.start() - CTX_WINDOW),
                                         m.start()):
                    continue
                sql[name].add(rel)
            for m in RE_MADE.finditer(text):
                made.add(norm(m.group(1)))
            n = len(RE_DYN.findall(text))
            if n:
                dyn[rel] = n
            for w in set(x.lower() for x in RE_WORD.findall(text)):
                words[w].add(rel)
    return {'sql': sql, 'words': words, 'made': made, 'dyn': dyn,
            'files': nfiles}


def relations(cur):
    """Все отношения БД: (схема, имя) -> вид и оценка числа строк."""
    cur.execute("""
        SELECT n.nspname, c.relname, c.relkind, c.reltuples::bigint
        FROM pg_class c JOIN pg_namespace n ON n.oid = c.relnamespace
        WHERE c.relkind IN ('r', 'p', 'v', 'm', 'f')
          AND n.nspname NOT IN ('pg_catalog', 'information_schema')
          AND n.nspname NOT LIKE 'pg_%'""")
    rel = {}
    for sch, tab, kind, tuples in cur.fetchall():
        rel[(sch, tab)] = {'kind': kind,
                           'rows': None if kind in ('v', 'f') else max(tuples, 0)}
    return rel


def db_side(cur, rel):
    """Обращения из самой БД: они кодом не видны, а таблицу держат.

    Представление, внешний ключ, тело функции и каталог слоёв — четыре
    способа зависеть от таблицы, не написав про неё ни строчки в
    приложении. Из-за них «к таблице никто не ходит» бывает неправдой.
    """
    dep = collections.defaultdict(lambda: collections.defaultdict(set))

    cur.execute("""
        SELECT DISTINCT sn.nspname, sc.relname, dn.nspname, dv.relname
        FROM pg_depend d
        JOIN pg_rewrite r ON r.oid = d.objid
        JOIN pg_class dv ON dv.oid = r.ev_class
        JOIN pg_namespace dn ON dn.oid = dv.relnamespace
        JOIN pg_class sc ON sc.oid = d.refobjid
        JOIN pg_namespace sn ON sn.oid = sc.relnamespace
        WHERE d.classid = 'pg_rewrite'::regclass
          AND d.refclassid = 'pg_class'::regclass
          AND d.deptype = 'n' AND dv.oid <> sc.oid
          AND sn.nspname NOT IN ('pg_catalog', 'information_schema')""")
    for sn, sc, dn, dv in cur.fetchall():
        dep[(sn, sc)]['бд:предст.'].add('%s.%s' % (dn, dv))

    cur.execute("""
        SELECT fn.nspname, fl.relname, cn.nspname, cl.relname
        FROM pg_constraint c
        JOIN pg_class cl ON cl.oid = c.conrelid
        JOIN pg_namespace cn ON cn.oid = cl.relnamespace
        JOIN pg_class fl ON fl.oid = c.confrelid
        JOIN pg_namespace fn ON fn.oid = fl.relnamespace
        WHERE c.contype = 'f'""")
    for fn, fl, cn, cl in cur.fetchall():
        dep[(fn, fl)]['бд:ключ'].add('%s.%s' % (cn, cl))

    cur.execute("""
        SELECT n.nspname, p.proname, p.prosrc
        FROM pg_proc p JOIN pg_namespace n ON n.oid = p.pronamespace
        WHERE n.nspname NOT IN ('pg_catalog', 'information_schema')
          AND p.prosrc IS NOT NULL""")
    bare = resolver(rel, DB_PATH)
    for sch, proc, src in cur.fetchall():
        for m in RE_SQL.finditer(src):
            name = norm(m.group(2))
            if name in SKIP_NAMES:
                continue
            key = tuple(name.split('.', 1)) if '.' in name \
                else (bare.get(name), name)
            if key[0] and key in rel:
                dep[key]['бд:функция'].add('%s.%s' % (sch, proc))

    for src, cols in (('meta.layer_catalog', ('schema_name', 'table_name')),
                      ('meta.field_catalog', ('table_schema', 'table_name'))):
        cur.execute("SELECT to_regclass(%s)", (src,))
        if cur.fetchone()[0] is None:
            continue
        cur.execute('SELECT DISTINCT %s, %s FROM %s' % (cols + (src,)))
        for sch, tab in cur.fetchall():
            if (sch, tab) in rel:
                dep[(sch, tab)]['бд:каталог'].add(src)
    return dep


def resolver(rel, path):
    """Имя без схемы -> схема, по порядку поиска потребителя."""
    bare = {}
    for (sch, tab) in rel:
        if sch not in path:
            continue
        prev = bare.get(tab)
        if prev is None or path.index(sch) < path.index(prev):
            bare[tab] = sch
    return bare


KIND_RU = {'r': 'таблица', 'p': 'секц.', 'v': 'предст.', 'm': 'матпредст.',
           'f': 'внешн.'}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--db', default=os.environ.get('TGID_DB', 'tgid_gis'))
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', default=os.environ.get('TGID_PORT', '5440'))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    ap.add_argument('--table', default='',
                    help='показать потребителей одного отношения')
    ap.add_argument('--json', default='')
    ap.add_argument('--limit', type=int, default=20)
    args = ap.parse_args()

    conn = psycopg2.connect(host=args.host, port=args.port, dbname=args.db,
                            user=args.user, password=args.password)
    cur = conn.cursor()
    rel = relations(cur)
    dep = db_side(cur, rel)
    conn.close()

    # (схема, имя) -> потребитель -> {'sql': [...], 'упом.': n}
    use = collections.defaultdict(lambda: collections.defaultdict(dict))
    legacy = collections.defaultdict(lambda: collections.defaultdict(set))
    missing = collections.defaultdict(lambda: collections.defaultdict(set))
    stat = []
    seen_names = set(tab for _, tab in rel)
    schemas = set(sch for sch, _ in rel)

    bare_of = {c: resolver(rel, path) for c, _, path in CONSUMERS}

    for cname, root, path in CONSUMERS:
        if not os.path.isdir(root):
            stat.append((cname, root, None, 0, 0, 0))
            continue
        got = scan(root)
        bare = bare_of[cname]
        nsql = 0
        for name, files in got['sql'].items():
            if '.' in name:
                sch, tab = name.split('.', 1)
                key = (sch, tab) if (sch, tab) in rel else None
            else:
                sch = bare.get(name)
                key = (sch, name) if sch else None
            if key is None:
                sch = name.split('.', 1)[0] if '.' in name else ''
                if (name not in got['made'] and not name.startswith('#')
                        and name not in STOP_EN
                        and sch not in SYSTEM_SCHEMAS
                        and (not sch or sch in schemas)):
                    missing[name][cname] = files
                continue
            nsql += 1
            use[key][cname]['sql'] = sorted(files)
            if key[0] == 'public' and key[1] not in POSTGIS:
                legacy[key][cname] = files
        # Упоминания разрешаются тем же порядком поиска: слово из кода
        # значит то же отношение, что и в запросе того же потребителя.
        for w, files in got['words'].items():
            if w not in seen_names:
                continue
            sch = bare.get(w)
            if sch is None:
                continue
            use[(sch, w)][cname].setdefault('sql', [])
            use[(sch, w)][cname]['упом.'] = len(files)
        stat.append((cname, root, got['files'], nsql,
                     sum(got['dyn'].values()), len(got['dyn'])))

    if args.table:
        want = args.table.lower()
        keys = sorted(k for k in rel
                      if k[1] == want or '%s.%s' % k == want)
        if not keys:
            print('в БД нет отношения «%s»' % args.table)
            return 1
        for key in keys:
            sch, tab = key
            info = rel[key]
            print('\n%s.%s [%s%s]' % (
                sch, tab, KIND_RU.get(info['kind'], info['kind']),
                '' if info['rows'] is None else ', ~%d строк' % info['rows']))
            by = use.get(key, {})
            for cname in sorted(by):
                ev = by[cname]
                files = ev.get('sql') or []
                more = '' if len(files) <= 4 else ' +%d' % (len(files) - 4)
                print('  %-11s sql:%-4d упом.:%-4d %s%s'
                      % (cname, len(files), ev.get('упом.', 0),
                         ', '.join(files[:4]), more))
            for kind, who in sorted(dep.get(key, {}).items()):
                w = sorted(who)
                more = '' if len(w) <= 4 else ' +%d' % (len(w) - 4)
                print('  %-11s %d: %s%s'
                      % (kind, len(w), ', '.join(w[:4]), more))
            if not by and key not in dep:
                print('  никто из тех, кого видно; помнить про динамический'
                      ' SQL и QGIS')
        return 0

    print('%-10s %7s %7s %7s %7s' % ('источник', 'файлов', 'sql-имён',
                                     'динам.', 'файлов'))
    for cname, root, nfiles, nsql, ndyn, ndynf in stat:
        if nfiles is None:
            print('%-10s   нет каталога %s' % (cname, root))
            continue
        print('%-10s %7d %8d %7d %7d' % (cname, nfiles, nsql, ndyn, ndynf))

    # Затенение: отношение, до которого ни у одного потребителя не
    # доходит очередь по search_path, «не используется» не потому, что
    # не нужно, а потому, что его имя занято схемой выше.
    shadow = {}
    for (sch, tab) in rel:
        vis = set()
        for cname, _, path in CONSUMERS:
            if sch in path and bare_of[cname].get(tab) == sch:
                vis.add(cname)
        shadow[(sch, tab)] = vis

    # Чердак сторожит ошибку шагов 58 и 60: таблица уехала, а обращение
    # осталось. Обращения из sql/tools/converter не в счёт — это сами
    # скрипты переноса, они и должны про чердак знать.
    attic = []
    for key, by in use.items():
        if key[0] != 'attic':
            continue
        who = {c: e['sql'] for c, e in by.items()
               if e.get('sql') and c not in REPO_SIDE}
        if who or dep.get(key):
            attic.append((key, who))
    print('\nна чердаке, но обращения остались: %d' % len(attic))
    for (sch, tab), who in sorted(attic, key=lambda x: -sum(
            len(f) for f in x[1].values())):
        parts = ['%s:%d' % (c, len(f)) for c, f in sorted(who.items())]
        parts += ['%s:%d' % (k, len(v))
                  for k, v in sorted(dep.get((sch, tab), {}).items())]
        print('  %-40s %s' % (tab, ', '.join(parts)))

    pub = sorted(legacy.items(), key=lambda x: -sum(len(f) for f in x[1].values()))
    print('\nнаследие — обращения разрешаются в public: %d' % len(pub))
    for (sch, tab), by in pub:
        print('  %-40s %s' % (tab, ', '.join('%s:%d' % (c, len(f))
                                             for c, f in sorted(by.items()))))

    # Отношение без единого обращения: ни из кода, ни из самой БД.
    # Представления отделены от таблиц: производное представление,
    # которое никто не читает, — это слой для внешнего ГИС, а не потеря.
    dead_tab, dead_view, shadowed = [], [], []
    for key, info in sorted(rel.items()):
        sch, tab = key
        if sch in ('attic', 'compat') or tab in POSTGIS:
            continue
        by = use.get(key, {})
        if any(e.get('sql') or e.get('упом.') for e in by.values()):
            continue
        if dep.get(key):
            continue
        if not shadow[key]:
            shadowed.append((key, info))
        elif info['kind'] in ('v', 'm'):
            dead_view.append((key, info))
        else:
            dead_tab.append((key, info))

    print('\nникто не обращается, таблицы — кандидаты на чердак: %d'
          % len(dead_tab))
    for (sch, tab), info in sorted(dead_tab,
                                   key=lambda x: -(x[1]['rows'] or 0))[:args.limit]:
        print('  %-12s %-40s ~%d строк' % (sch, tab, info['rows'] or 0))
    if len(dead_tab) > args.limit:
        print('  ... ещё %d, полный список в --json' % (len(dead_tab) - args.limit))

    print('\nникто не обращается, представления — вероятно, слои ГИС: %d'
          % len(dead_view))
    for (sch, tab), info in dead_view[:args.limit]:
        print('  %-12s %-40s' % (sch, tab))
    if len(dead_view) > args.limit:
        print('  ... ещё %d' % (len(dead_view) - args.limit))

    print('\nимя занято схемой выше — судить нельзя: %d' % len(shadowed))
    for (sch, tab), info in shadowed[:args.limit]:
        print('  %-12s %-40s' % (sch, tab))

    print('\nв БД не найдено (и код их не создаёт): %d' % len(missing))
    for name in sorted(missing, key=lambda n: -sum(len(f) for f in missing[n].values()))[:args.limit]:
        by = missing[name]
        print('  %-40s %s' % (name, ', '.join('%s:%d' % (c, len(f))
                                              for c, f in sorted(by.items()))))

    if args.json:
        out = {'relations': [], 'missing': {}, 'consumers': [
            {'name': c, 'root': r, 'files': f, 'sql_names': s,
             'dynamic': d, 'dynamic_files': df}
            for c, r, f, s, d, df in stat]}
        for key, info in sorted(rel.items()):
            sch, tab = key
            by = use.get(key, {})
            out['relations'].append({
                'schema': sch, 'name': tab, 'kind': info['kind'],
                'rows': info['rows'],
                'visible_to': sorted(shadow[key]),
                'consumers': {c: {'sql': e.get('sql', []),
                                  'mentions': e.get('упом.', 0)}
                              for c, e in sorted(by.items())}})
        for name, by in sorted(missing.items()):
            out['missing'][name] = {c: sorted(f) for c, f in sorted(by.items())}
        os.makedirs(os.path.dirname(args.json) or '.', exist_ok=True)
        with io.open(args.json, 'w', encoding='utf-8') as f:
            json.dump(out, f, ensure_ascii=False, indent=1)
        print('\nполная карта: %s' % args.json)
    return 0


if __name__ == '__main__':
    sys.exit(main())
