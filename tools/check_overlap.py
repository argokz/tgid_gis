"""Проверка допущения «объект принадлежит ровно одному подтипу».

Печатает распределение числа подтипов на объект и самые частые сочетания.
"""
import argparse
import json
import os

import psycopg2


def build_union(entries, link):
    return ' union all '.join(
        "select %s as oid, '%s' as t from public.%s where %s is not null"
        % (link, e['target'], e['source'], link) for e in entries)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--mapping', required=True)
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', type=int, default=int(os.environ.get('TGID_PORT', 5440)))
    ap.add_argument('--db', default=os.environ.get('TGID_DB', 'almatygid'))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    args = ap.parse_args()

    with open(args.mapping, encoding='utf-8') as f:
        m = json.load(f)

    conn = psycopg2.connect(host=args.host, port=args.port, dbname=args.db,
                            user=args.user, password=args.password)
    cur = conn.cursor()

    for label, key, link in (('УЗЛЫ', 'node_types', 'nodeid'),
                             ('ЛИНИИ', 'line_types', 'lineid')):
        u = build_union(m[key], link)
        print('\n===== %s =====' % label)

        cur.execute('with u as (%s) '
                    'select cnt, count(*) from '
                    '(select oid, count(*) cnt from u group by oid) z '
                    'group by cnt order by cnt' % u)
        print('подтипов на объект -> сколько объектов')
        for cnt, n in cur.fetchall():
            print('  %2d -> %d' % (cnt, n))

        cur.execute("with u as (%s), "
                    "g as (select oid, string_agg(t, '+' order by t) combo, "
                    "count(*) c from u group by oid) "
                    "select combo, count(*) from g where c > 1 "
                    "group by combo order by 2 desc limit 15" % u)
        rows = cur.fetchall()
        if rows:
            print('частые сочетания:')
            for combo, n in rows:
                print('  %-60s %d' % (combo, n))

    conn.close()


if __name__ == '__main__':
    main()
