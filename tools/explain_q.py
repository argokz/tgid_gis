"""EXPLAIN ANALYZE для файла запроса с дописанным фильтром фрагмента."""
import argparse
import os
import re

import psycopg2


def read(path):
    for enc in ('utf-8', 'cp1251'):
        try:
            with open(path, encoding=enc) as f:
                return f.read()
        except UnicodeDecodeError:
            continue
    raise SystemExit('кодировка не определена: %s' % path)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--sql', required=True)
    ap.add_argument('--alias', default='n')
    ap.add_argument('--fragments', default='80')
    ap.add_argument('--grep', default='', help='показать только строки с этим текстом')
    ap.add_argument('--top', type=int, default=25)
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', type=int, default=int(os.environ.get('TGID_PORT', 5440)))
    ap.add_argument('--db', default=os.environ.get('TGID_DB', 'almatygid'))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    args = ap.parse_args()

    sql = read(args.sql)
    if args.fragments:
        sql += ' AND %s.fileID IN (%s)' % (args.alias, args.fragments)

    conn = psycopg2.connect(host=args.host, port=args.port, dbname=args.db,
                            user=args.user, password=args.password)
    cur = conn.cursor()
    cur.execute('EXPLAIN (ANALYZE, BUFFERS) ' + sql)
    lines = [r[0] for r in cur.fetchall()]

    if args.grep:
        lines = [l for l in lines if args.grep in l]

    # Самые дорогие узлы плана — по actual time
    scored = []
    for l in lines:
        m = re.search(r'actual time=[\d.]+\.\.([\d.]+) rows=(\d+)', l)
        if m:
            scored.append((float(m.group(1)), l.strip()[:150]))
    scored.sort(reverse=True)

    print('--- самые дорогие узлы плана ---')
    for t, l in scored[:args.top]:
        print('%9.1f мс  %s' % (t, l))

    for l in lines:
        if l.startswith(('Planning', 'Execution', ' Planning', ' Execution')):
            print(l)

    conn.close()


if __name__ == '__main__':
    main()
