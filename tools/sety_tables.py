"""Какие таблицы движок sety трогает и какие из них ещё наследие.

Требование «одна БД net, ничего старого» проверяется не на глаз:
запросы разбросаны по 103 файлам. Здесь собираются все имена таблиц из
FROM / JOIN / INSERT / UPDATE / DELETE и делятся на три группы:

  net.* и ref.*   — целевые, ничего делать не надо;
  наследие        — то, что придётся перенести или переписать;
  неизвестно      — имена, которые не удалось сопоставить со схемой.

Принадлежность проверяется по живой БД, а не по списку в голове.

    python tools/sety_tables.py --db tgid_gis
"""
import argparse
import collections
import io
import os
import re
import sys

import psycopg2

SETY_DIR = r'H:\projects\tgid-app-new\gid8\python\sety\sety'

# Имена после FROM/JOIN/INTO/UPDATE. Подзапросы '(' отсекаются.
# Отрицательный просмотр вперёд отсекает питоновские "from X import":
# без него в список таблиц попадали все модули движка.
RE_TAB = re.compile(
    r'\b(?:from|join|insert\s+into|update|delete\s+from)\s+'
    r'([A-Za-z_][\w]*(?:\.[A-Za-z_][\w]*)?)'
    r'(?!\s+import\b)',
    re.I)

# Служебные слова, попадающие в захват по ошибке
SKIP = {'select', 'values', 'set', 'where', 'dual', 'lateral'}


def scan(root):
    hits = collections.defaultdict(set)
    for dirpath, dirnames, files in os.walk(root):
        dirnames[:] = [d for d in dirnames if d != '__pycache__']
        for fn in files:
            if not fn.endswith('.py'):
                continue
            p = os.path.join(dirpath, fn)
            # Часть файлов движка не в UTF-8 — читаем терпимо: нас
            # интересуют только латинские имена таблиц.
            try:
                text = io.open(p, encoding='utf-8', errors='replace').read()
            except OSError:
                continue
            rel = os.path.relpath(p, root)
            for m in RE_TAB.finditer(text):
                name = m.group(1).lower()
                if name in SKIP:
                    continue
                hits[name].add(rel)
    return hits


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--db', required=True)
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', default=os.environ.get('TGID_PORT', '5440'))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    ap.add_argument('--sety', default=SETY_DIR)
    args = ap.parse_args()

    hits = scan(args.sety)

    conn = psycopg2.connect(host=args.host, port=args.port, dbname=args.db,
                            user=args.user, password=args.password)
    cur = conn.cursor()
    cur.execute("""
        SELECT table_schema, table_name, table_type
        FROM information_schema.tables
        WHERE table_schema NOT IN ('pg_catalog', 'information_schema')
    """)
    kind = {}
    for sch, tab, typ in cur.fetchall():
        kind['%s.%s' % (sch, tab)] = typ
        kind.setdefault(tab, ('%s|%s' % (sch, typ)))
    conn.close()

    target, legacy, unknown = {}, {}, {}
    for name, files in hits.items():
        if name.startswith(('net.', 'ref.', 'meta.', 'calc.')):
            target[name] = files
            continue
        key = kind.get(name) or kind.get('public.' + name)
        if key is None:
            unknown[name] = files
        else:
            legacy[name] = files

    def show(title, d, with_type=False):
        print('\n%s: %d' % (title, len(d)))
        for name in sorted(d, key=lambda n: (-len(d[n]), n)):
            t = ''
            if with_type:
                k = kind.get('public.' + name) or kind.get(name) or ''
                t = ' [%s]' % ('представление' if 'VIEW' in str(k) else 'таблица')
            fl = sorted(d[name])
            more = '' if len(fl) <= 3 else ' +%d' % (len(fl) - 3)
            print('  %-28s%s  %s%s' % (name, t, ', '.join(fl[:3]), more))

    show('целевые (net/ref/meta/calc)', target)
    show('НАСЛЕДИЕ — переписать или перенести', legacy, with_type=True)
    show('не сопоставлено со схемой', unknown)

    print('\nИтого имён: %d, из них наследия: %d'
          % (len(hits), len(legacy)))
    return 0


if __name__ == '__main__':
    sys.exit(main())
