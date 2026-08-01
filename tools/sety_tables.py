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
    # Имя без схемы разрешается по search_path. Наследие — это не
    # «имя написано без схемы», а «имя разрешается в public». После
    # переезда таблиц в calc/ref те же запросы уже целевые, и считать
    # их наследием — значит показывать работу, которой нет.
    # Порядок тот же, что в search_path базы.
    ORDER = ('public', 'net', 'ref', 'calc', 'meta')
    qualified, bare = {}, {}
    for sch, tab, typ in cur.fetchall():
        qualified['%s.%s' % (sch, tab)] = (sch, typ)
        if sch not in ORDER:
            continue
        prev = bare.get(tab)
        if prev is None or ORDER.index(sch) < ORDER.index(prev[0]):
            bare[tab] = (sch, typ)
    conn.close()

    target, legacy, unknown = {}, {}, {}
    resolved = {}
    for name, files in hits.items():
        if '.' in name:
            info = qualified.get(name)
        else:
            info = bare.get(name)
        if info is None:
            unknown[name] = files
            continue
        resolved[name] = info
        if info[0] == 'public':
            legacy[name] = files
        else:
            target[name] = files

    def show(title, d, detail=False):
        print('\n%s: %d' % (title, len(d)))
        for name in sorted(d, key=lambda n: (-len(d[n]), n)):
            t = ''
            if detail:
                sch, typ = resolved[name]
                t = ' -> %s [%s]' % (
                    sch, 'представление' if 'VIEW' in typ else 'таблица')
            fl = sorted(d[name])
            more = '' if len(fl) <= 3 else ' +%d' % (len(fl) - 3)
            print('  %-26s%-22s %s%s'
                  % (name, t, ', '.join(fl[:3]), more))

    show('целевые (разрешаются в net/ref/calc/meta)', target, detail=True)
    show('НАСЛЕДИЕ — разрешаются в public', legacy, detail=True)
    show('не сопоставлено со схемой (импорты Python и т.п.)', unknown)

    print('\nИтого имён: %d, из них наследия: %d'
          % (len(hits), len(legacy)))
    return 0


if __name__ == '__main__':
    sys.exit(main())
