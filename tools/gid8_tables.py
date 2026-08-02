"""К каким таблицам обращается старое приложение gid8.

Перед тем как переводить 583 файла C++ на новую схему, надо знать объём:
какие имена таблиц там встречаются и во что они теперь разрешаются.
Тот же приём, что и с движком расчёта (tools/sety_tables.py), но
источники другие: .cpp/.h с SQL внутри строк и отдельные .sql-файлы,
которые приложение читает с диска.

Наследие — это имя, которое разрешается в public. Имя, попадающее в
net/ref/calc и подобные, уже целевое: search_path базы приводит его
куда надо без правки кода.

    python tools/gid8_tables.py --db tgid_gis --root ../gid8
"""
import argparse
import collections
import io
import os
import re
import sys

import psycopg2

RE_TAB = re.compile(
    r'\b(?:from|join|insert\s+into|update|delete\s+from)\s+'
    r'([A-Za-z_][\w]*(?:\.[A-Za-z_][\w]*)?)'
    r'(?!\s*\()',
    re.I)

SKIP = {'select', 'values', 'set', 'where', 'dual', 'lateral', 'only',
        'table', 'into'}
EXT = ('.cpp', '.h', '.hpp', '.sql', '.ui')


def scan(root):
    hits = collections.defaultdict(set)
    for dirpath, dirnames, files in os.walk(root):
        dirnames[:] = [d for d in dirnames
                       if d not in ('.git', 'build', 'debug', 'release',
                                    '__pycache__')]
        for fn in files:
            if not fn.lower().endswith(EXT):
                continue
            p = os.path.join(dirpath, fn)
            try:
                with io.open(p, encoding='utf-8', errors='replace') as f:
                    text = f.read()
            except OSError:
                continue
            rel = os.path.relpath(p, root)
            for m in RE_TAB.finditer(text):
                name = m.group(1).lower()
                if name in SKIP or name.isdigit():
                    continue
                hits[name].add(rel)
    return hits


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--db', required=True)
    ap.add_argument('--root', required=True, help='каталог gid8')
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', default=os.environ.get('TGID_PORT', '5440'))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    ap.add_argument('--limit', type=int, default=25)
    args = ap.parse_args()

    if not os.path.isdir(args.root):
        print('нет каталога %s' % args.root)
        return 2

    hits = scan(args.root)

    conn = psycopg2.connect(host=args.host, port=args.port, dbname=args.db,
                            user=args.user, password=args.password)
    cur = conn.cursor()
    cur.execute("""
        SELECT table_schema, table_name
        FROM information_schema.tables
        WHERE table_schema NOT IN ('pg_catalog', 'information_schema')""")
    # compat идёт первой: именно такой порядок задаёт gid8
    # при подключении (gid8/db/db.cpp).
    ORDER = ('compat', 'public', 'net', 'ref', 'calc', 'meta',
             'addr', 'doc', 'el', 'ops', 'org')
    bare, qualified = {}, set()
    for sch, tab in cur.fetchall():
        qualified.add('%s.%s' % (sch, tab))
        if sch not in ORDER:
            continue
        prev = bare.get(tab)
        if prev is None or ORDER.index(sch) < ORDER.index(prev):
            bare[tab] = sch
    conn.close()

    legacy, target, unknown = {}, {}, {}
    where = {}
    for name, files in hits.items():
        if '.' in name:
            sch = name.split('.')[0] if name in qualified else None
        else:
            sch = bare.get(name)
        if sch is None:
            unknown[name] = files
        elif sch == 'public':
            legacy[name] = files
            where[name] = sch
        else:
            target[name] = files
            where[name] = sch

    def show(title, d, n):
        print('\n%s: %d' % (title, len(d)))
        for name in sorted(d, key=lambda x: (-len(d[x]), x))[:n]:
            fl = sorted(d[name])
            more = '' if len(fl) <= 2 else '  +%d' % (len(fl) - 2)
            print('  %-28s %-6s %s%s'
                  % (name, where.get(name, ''), ', '.join(fl[:2]), more))

    show('НАСЛЕДИЕ — разрешаются в public', legacy, args.limit)
    show('целевые — разрешаются в net/ref/calc/...', target, args.limit)
    show('не найдены в БД (возможно, уже удалены)', unknown, args.limit)

    print('\nимён всего: %d; наследия: %d; целевых: %d; неизвестных: %d'
          % (len(hits), len(legacy), len(target), len(unknown)))
    print('файлов с обращениями: %d'
          % len({f for s in hits.values() for f in s}))
    return 0


if __name__ == '__main__':
    sys.exit(main())
