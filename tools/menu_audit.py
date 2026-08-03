"""Реестр команд меню gid8: меню -> действие -> слот -> SQL.

Проверка «все ли команды переведены на новую БД» до сих пор велась по
блокам в docs/18, то есть по записям о проделанной работе. Это проверка
намерения, а не результата: команду можно закрыть в документе и не
тронуть в коде, а можно тронуть код и забыть про соседнюю команду того
же меню. Инструмент строит список независимо — от разметки меню к коду,
а не от журнала шагов.

Источники:
  gidrMenu.cpp   — порядок и вложенность меню (addMenu / addAction);
  gidrAction.cpp — объявления aXxx = addTrigger|addToggle(..., SLOT(onYyy()));
  всё дерево     — тело слота void Gidr::onYyy(...) { ... }.

Состояние слота:
  ПУСТО     — тела нет либо в нём нет ни одного оператора (заглушка);
  ЗАГЛУШКА  — тело только сообщает, что команда не реализована;
  СТАРОЕ    — в SQL тела есть имена дореформенной модели;
  NET       — в SQL тела есть net.* / tbl_sql(), старых имён нет;
  БЕЗ SQL   — код есть, прямого SQL в теле нет (обычно вызов диалога).

«БЕЗ SQL» не значит «не трогает БД»: запрос может быть в вызываемом
диалоге. Инструмент показывает границу своей видимости, а не выдаёт её
за отсутствие проблемы.

    python tools/menu_audit.py                  # весь реестр
    python tools/menu_audit.py --status СТАРОЕ  # только требующие правки
    python tools/menu_audit.py --menu Ремонты   # одно меню
"""
import argparse
import collections
import io
import os
import re
import sys

BASE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GID8 = os.path.join(BASE, 'gid8', 'gid8')
VIEW = os.path.join(GID8, 'gidview')

# Имена дореформенной модели. Держим отдельно от списка классов net:
# отсутствие имени в этом списке не делает запрос правильным, поэтому
# список именно «что точно старое», а не «что точно новое».
OLD_NAMES = [
    'nodes', 'linesobj', 'heatsources', 'realconsumers',
    'generalizedconsumers', 'connectnodes', 'heatchambers', 'pumpstations',
    'refillnodes', 'threewayvalves', 'dampers', 'diaphragms', 'elevators',
    'heatexchangers', 'heatpipesections', 'pumps', 'systemradiators',
    'airheaters', 'pressregulators', 'localhydroresistances2',
    'setpressnodes', 'wdodevices', 'trps', 'buildingentries',
    'sortnodesforuchastok', 'deployeddirections', 'opresdeployed',
    'overgroundnodes', 'pavilions', 'undergroundnodes', 'uninstallednodes',
    'internalnodes', 'bypass', 'consumptregulators', 'pressdropregulators',
    'regularmatures', 'reversevalves',
]
RE_OLD = re.compile(r'\b(?:from|join|into|update)\s+(%s)\b'
                    % '|'.join(OLD_NAMES), re.I)
RE_NET = re.compile(r'\bnet\.|tbl_sql\s*\(', re.I)
RE_SQL = re.compile(r'\b(select|insert\s+into|update|delete\s+from)\b', re.I)

RE_ACTION = re.compile(
    r'(a[A-Za-z0-9_]+)\s*=\s*add(?:Trigger|Toggle|ToggleGroup)\s*\((.*?)\)\s*;',
    re.S)
RE_SLOT_IN = re.compile(r'SLOT\s*\(\s*(on[A-Za-z0-9_]+)')
RE_STR = re.compile(r'(?:tr\s*\(\s*)?"((?:[^"\\]|\\.)*)"')

RE_MENU_NEW = re.compile(r'(\w+)\s*=\s*(?:\w+)\s*->\s*addMenu\s*\(\s*(?:tr\s*\(\s*)?"([^"]*)"')
RE_MENU_ADD = re.compile(r'(\w+)\s*->\s*addAction\s*\(\s*[\w.]*?(a[A-Za-z0-9_]+)')

# Признаки тела-заглушки: команда объявлена, но сообщает о нереализованности.
RE_STUB = re.compile(r'не\s*реализ|not\s*implement|заглушк|TODO', re.I)


def read(path):
    return io.open(path, encoding='utf-8', errors='replace').read()


def walk_sources():
    for dp, dn, fs in os.walk(GID8):
        for fn in fs:
            if fn.endswith(('.cpp', '.h')):
                yield os.path.join(dp, fn)


def parse_actions():
    """aXxx -> (подпись, слот)."""
    txt = read(os.path.join(VIEW, 'gidrAction.cpp'))
    out = {}
    for m in RE_ACTION.finditer(txt):
        name, args = m.group(1), m.group(2)
        label = ''
        s = RE_STR.search(args)
        if s:
            label = s.group(1)
        slot = RE_SLOT_IN.search(args)
        out[name] = (label, slot.group(1) if slot else '')
    return out


def parse_menu():
    """Порядок команд: [(путь меню, aXxx)] в порядке разметки."""
    txt = read(os.path.join(VIEW, 'gidrMenu.cpp'))
    names = {}          # переменная меню -> подпись
    parent = {}         # переменная меню -> родитель
    order = []
    for line in txt.splitlines():
        m = RE_MENU_NEW.search(line)
        if m:
            var, label = m.group(1), m.group(2)
            names[var] = label
            owner = re.search(r'=\s*(\w+)\s*->\s*addMenu', line)
            if owner and owner.group(1) in names:
                parent[var] = owner.group(1)
            continue
        m = RE_MENU_ADD.search(line)
        if m:
            var, act = m.group(1), m.group(2)
            path, seen = [], set()
            cur = var
            while cur in names and cur not in seen:
                seen.add(cur)
                path.append(names[cur])
                cur = parent.get(cur)
            order.append((' / '.join(reversed(path)) or '?', act))
    return order


def body_of(txt, start):
    """Тело функции от позиции открывающей скобки, по балансу скобок."""
    i = txt.find('{', start)
    if i < 0:
        return ''
    depth, j = 0, i
    while j < len(txt):
        if txt[j] == '{':
            depth += 1
        elif txt[j] == '}':
            depth -= 1
            if depth == 0:
                return txt[i + 1:j]
        j += 1
    return txt[i + 1:]


def index_slots():
    """onXxx -> (файл, тело)."""
    out = {}
    for p in walk_sources():
        txt = read(p)
        # Открывающая скобка не обязана быть на строке сигнатуры: в
        # gidrSlot.cpp между ними стоит комментарий с русской подписью
        # команды. Требование `{` на той же строке давало 303 «не
        # найден» из 387 — то есть инструмент отчитывался о ненайденном
        # коде, который лежит строкой ниже.
        for m in re.finditer(
                r'\bvoid\s+\w+::\s*(on[A-Za-z0-9_]+)\s*\([^)]*\)\s*'
                r'(?:const\s*)?(?://[^\n]*)?\s*\{', txt):
            name = m.group(1)
            if name in out:
                continue
            out[name] = (os.path.relpath(p, BASE), body_of(txt, m.end() - 1))
    return out


def strip_comments(s):
    s = re.sub(r'/\*.*?\*/', ' ', s, flags=re.S)
    s = re.sub(r'//[^\n]*', ' ', s)
    return s


def resolve_map(names):
    """имя -> схема, в которой оно резолвится при search_path gid8.

    Список «старых» имён сам по себе не признак дефекта: часть из них
    легально переехала в ref (deployeddirections) или net и резолвится
    правильно. Дефект — это имя, которое резолвится в compat, то есть
    держится только слоем совместимости. Поэтому решает не список, а
    фактический резолвинг в живой БД.
    """
    try:
        import psycopg2
    except ImportError:
        return {}
    pw = os.environ.get('PGPASSWORD')
    if not pw:
        return {}
    try:
        c = psycopg2.connect(host=os.environ.get('TGID_HOST', 'localhost'),
                             port=os.environ.get('TGID_PORT', '5440'),
                             dbname=os.environ.get('TGID_DB', 'tgid_gis'),
                             user=os.environ.get('TGID_USER', 'postgres'),
                             password=pw)
    except Exception:
        return {}
    cur = c.cursor()
    cur.execute('SET search_path = compat, public, net, ref, calc, meta, '
                'addr, doc, el, ops, org')
    out = {}
    for n in names:
        cur.execute("SELECT nspname FROM pg_class k "
                    "JOIN pg_namespace s ON s.oid=k.relnamespace "
                    "WHERE k.oid = to_regclass(%s)", (n,))
        r = cur.fetchone()
        out[n] = r[0] if r else None
    c.close()
    return out


RESOLVED = {}


def classify(body):
    code = strip_comments(body)
    if not code.strip():
        return 'ПУСТО', ''
    if not re.search(r'[;{]', code):
        return 'ПУСТО', ''
    if RE_STUB.search(code) and len(code.strip()) < 400:
        return 'ЗАГЛУШКА', ''
    hits = sorted(set(m.group(1).lower() for m in RE_OLD.finditer(code)))
    # Дефект — только то, что держится на compat либо не резолвится вовсе.
    compat = [n for n in hits if RESOLVED.get(n, 'compat') == 'compat']
    lost = [n for n in hits if n in RESOLVED and RESOLVED[n] is None]
    if lost:
        return 'НЕТ ТАКОЙ', ', '.join(lost)
    if compat:
        return 'COMPAT', ', '.join('%s' % n for n in compat)
    if hits:
        return 'NET', 'через %s' % ', '.join(
            '%s->%s' % (n, RESOLVED[n]) for n in hits)
    if RE_NET.search(code):
        return 'NET', ''
    if RE_SQL.search(code):
        return 'SQL?', 'запрос без явных имён (динамика)'
    return 'БЕЗ SQL', ''


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--status', help='показать только это состояние')
    ap.add_argument('--menu', help='подстрока пути меню')
    ap.add_argument('--full', action='store_true', help='построчный реестр')
    args = ap.parse_args()

    global RESOLVED
    RESOLVED = resolve_map(OLD_NAMES)
    if not RESOLVED:
        print('ВНИМАНИЕ: нет связи с БД (нужен PGPASSWORD) — имена не '
              'проверены на резолвинг, все совпадения показаны как COMPAT.\n')

    actions = parse_actions()
    order = parse_menu()
    slots = index_slots()

    rows, seen = [], set()
    for path, act in order:
        if act in seen:
            continue
        seen.add(act)
        label, slot = actions.get(act, ('', ''))
        if not slot:
            rows.append((path, act, label, '', 'НЕТ СЛОТА', '', ''))
            continue
        f, body = slots.get(slot, ('', None))
        if body is None:
            rows.append((path, act, label, slot, 'НЕ НАЙДЕН', '', ''))
            continue
        st, note = classify(body)
        rows.append((path, act, label, slot, st, note, f))

    if args.menu:
        rows = [r for r in rows if args.menu.lower() in r[0].lower()]
    if args.status:
        rows = [r for r in rows if r[4] == args.status.upper()]

    cnt = collections.Counter(r[4] for r in rows)
    per_menu = collections.defaultdict(collections.Counter)
    for r in rows:
        per_menu[r[0].split(' / ')[0]][r[4]] += 1

    print('Команд меню: %d\n' % len(rows))
    print('Состояние слотов:')
    for st, n in cnt.most_common():
        print('  %-10s %4d' % (st, n))

    print('\nПо меню верхнего уровня:')
    bad_order = ['COMPAT', 'НЕТ ТАКОЙ', 'ПУСТО', 'ЗАГЛУШКА', 'НЕТ СЛОТА', 'НЕ НАЙДЕН']
    for m in sorted(per_menu, key=lambda x: -sum(per_menu[x][b] for b in bad_order)):
        c = per_menu[m]
        bad = sum(c[b] for b in bad_order)
        print('  %-28s всего %3d | требуют внимания %3d %s'
              % (m[:28], sum(c.values()), bad,
                 '(' + ', '.join('%s %d' % (b, c[b]) for b in bad_order if c[b]) + ')' if bad else ''))

    if args.full or args.status or args.menu:
        print('\n%-26s %-24s %-10s %s' % ('МЕНЮ', 'СЛОТ', 'СОСТОЯНИЕ', 'ПОДПИСЬ / ЗАМЕЧАНИЕ'))
        for path, act, label, slot, st, note, f in rows:
            print('%-26s %-24s %-10s %s%s'
                  % (path[:26], slot[:24], st, label[:40],
                     '  <- ' + note if note else ''))
    return 0


if __name__ == '__main__':
    sys.stdout.reconfigure(encoding='utf-8')
    sys.exit(main())
