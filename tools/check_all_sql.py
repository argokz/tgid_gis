"""Прогон всех SQL-файлов приложения против БД: что сломается после миграции.

Файлы проверяются через EXPLAIN — это ловит отсутствующие таблицы и колонки,
не выполняя сам запрос и ничего не меняя.

Смысл: слой совместимости обязан покрывать не только чтение схемы
(sql3/us.sql, ut.sql), но и отчёты, выгрузки и справочники — иначе
«приложение работает» неправда.

    python tools/check_all_sql.py --root H:/projects/.../gid6/gidr --db tgid_gis
"""
import argparse
import os
import re
import sys

import psycopg2

# Подстановки под плейсхолдеры, которые приложение заменяет в рантайме.
SUBST = [
    (r'\$user_gid\$', "'Администратор'"),
    (r'\$fileID\$', '1'),
    (r'\$id\$', '1'),
    (r'\$calculationID\$', '1'),
    (r'\$nodeID\$', '1'),
    (r'\$lineID\$', '1'),
]

# Файл пропускается, если после подстановок остались незакрытые параметры.
UNRESOLVED = re.compile(r'\$\w+\$|%\d|\{\w+\}|@\w+|\?')


def read(path):
    for enc in ('utf-8', 'cp1251'):
        try:
            with open(path, encoding=enc) as f:
                return f.read()
        except UnicodeDecodeError:
            continue
    return None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--root', required=True)
    ap.add_argument('--db', default='tgid_gis')
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', type=int, default=int(os.environ.get('TGID_PORT', 5440)))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    ap.add_argument('--only-net', action='store_true',
                    help='только файлы, обращающиеся к nodes/linesobj')
    ap.add_argument('--show', type=int, default=25)
    ap.add_argument('--dump', help='записать список «файл -> ошибка» для сравнения БД')
    args = ap.parse_args()

    files = []
    for dirpath, _, names in os.walk(args.root):
        for n in names:
            if n.lower().endswith('.sql'):
                files.append(os.path.join(dirpath, n))
    files.sort()

    conn = psycopg2.connect(host=args.host, port=args.port, dbname=args.db,
                            user=args.user, password=args.password)
    conn.autocommit = False

    total = skipped = ok = failed = 0
    errors = {}
    per_file = {}

    for path in files:
        sql = read(path)
        if not sql or not sql.strip():
            continue
        low = sql.lower()
        if args.only_net and 'nodes' not in low and 'linesobj' not in low:
            continue
        total += 1

        for pat, val in SUBST:
            sql = re.sub(pat, val, sql, flags=re.I)

        if UNRESOLVED.search(sql):
            skipped += 1
            continue
        if not low.lstrip().startswith(('select', 'with')):
            skipped += 1
            continue

        cur = conn.cursor()
        try:
            cur.execute('EXPLAIN ' + sql)
            ok += 1
            per_file[os.path.relpath(path, args.root)] = 'OK'
        except Exception as e:
            failed += 1
            msg = str(e).split('\n')[0].strip()
            rel = os.path.relpath(path, args.root)
            errors.setdefault(msg, []).append(rel)
            per_file[rel] = msg
        finally:
            conn.rollback()
            cur.close()

    conn.close()

    print('файлов рассмотрено: %d' % total)
    print('  пропущено (параметры/не SELECT): %d' % skipped)
    print('  разобрано успешно:               %d' % ok)
    print('  С ОШИБКОЙ:                       %d' % failed)

    if errors:
        print('\nошибки по видам:')
        for msg, paths in sorted(errors.items(), key=lambda kv: -len(kv[1])):
            print('\n  [%d файлов] %s' % (len(paths), msg[:110]))
            for p in paths[:args.show]:
                print('      %s' % p)
            if len(paths) > args.show:
                print('      ... ещё %d' % (len(paths) - args.show))

    if args.dump:
        os.makedirs(os.path.dirname(args.dump) or '.', exist_ok=True)
        with open(args.dump, 'w', encoding='utf-8') as f:
            for k in sorted(per_file):
                f.write('%s\t%s\n' % (k, per_file[k]))
        print('\n-> %s' % args.dump)

    return 1 if failed else 0


if __name__ == '__main__':
    sys.exit(main())
