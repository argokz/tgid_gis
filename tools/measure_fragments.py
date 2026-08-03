"""Замер чтения схемы: один фрагмент против всех сразу.

Открытие схемы в gid8 — это два тяжёлых запроса: узлы (sql3/us.sql) и
участки (sql3/ut.sql), в оба подставляется список фрагментов. Мерить
их надо ровно так, как это делает программа, иначе вывод получится про
что-то другое.

Первый прогон каждого запроса всегда медленнее (холодный кэш страниц),
поэтому делается прогрев и берётся медиана нескольких повторов, а не
одно измерение.

    python tools/measure_fragments.py
    python tools/measure_fragments.py --repeat 5
"""
import argparse
import io
import os
import statistics
import sys
import time

import psycopg2

TOOLS = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(TOOLS)

SEARCH_PATH = ('compat, public, net, ref, calc, meta, addr, doc, el, '
               'ops, org')


def read_sql(build_dir, name):
    p = os.path.join(build_dir, 'sql3', name)
    if not os.path.exists(p):
        raise SystemExit('нет файла %s' % p)
    # Файлы лежат в cp1251 (наследие старого интерфейса); имена
    # отношений латиницей, поэтому читаем терпимо.
    return io.open(p, encoding='utf-8', errors='replace').read()


def timed(cur, sql, repeat):
    times, rows = [], 0
    for i in range(repeat + 1):          # +1 — прогрев, в счёт не идёт
        t0 = time.perf_counter()
        cur.execute(sql)
        got = cur.fetchall()
        dt = time.perf_counter() - t0
        if i:
            times.append(dt)
            rows = len(got)
    return statistics.median(times), rows


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--db', default=os.environ.get('TGID_DB', 'tgid_gis'))
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', default=os.environ.get('TGID_PORT', '5440'))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    ap.add_argument('--build', default=r'H:\build\gid8-audit')
    ap.add_argument('--repeat', type=int, default=3)
    args = ap.parse_args()

    conn = psycopg2.connect(host=args.host, port=args.port, dbname=args.db,
                            user=args.user, password=args.password)
    cur = conn.cursor()
    cur.execute('SET search_path = %s' % SEARCH_PATH)

    cur.execute('SELECT id FROM fragments WHERE removed = 0 ORDER BY id')
    frags = [r[0] for r in cur.fetchall()]
    if not frags:
        raise SystemExit('нет фрагментов')

    us = read_sql(args.build, 'us.sql')
    ut = read_sql(args.build, 'ut.sql')

    # Как строит сам gid8: подстановка списка в условие по fileID.
    def with_frags(sql, ids):
        par = ','.join(str(x) for x in ids)
        return sql.replace('$$', 'AND n1.fileID IN (%s)' % par) \
                  .replace('@@', '') + \
            ('' if 'fileID IN' in sql else '')

    print('фрагментов в базе: %d' % len(frags))
    print('повторов на замер: %d (плюс прогрев)\n' % args.repeat)

    cases = [('один фрагмент (%d)' % frags[0], [frags[0]]),
             ('все фрагменты', frags)]

    for title, ids in cases:
        par = ','.join(str(x) for x in ids)
        print('--- %s' % title)
        for name, base, cond in (
                ('узлы (us.sql)', us, 'n.fileID IN (%s)' % par),
                ('участки (ut.sql)', ut, 'n1.fileID IN (%s)' % par)):
            sql = 'SELECT count(*) FROM (%s AND %s) _x' % (
                base.rstrip().rstrip(';'), cond)
            try:
                dt, _ = timed(cur, sql, args.repeat)
                cur.execute(sql)
                n = cur.fetchone()[0]
                print('  %-18s %7.0f мс   строк %d' % (name, dt * 1000, n))
            except Exception as e:
                conn.rollback()
                print('  %-18s не выполнен: %s'
                      % (name, str(e).splitlines()[0][:70]))
        print()

    conn.close()
    return 0


if __name__ == '__main__':
    sys.stdout.reconfigure(encoding='utf-8')
    sys.exit(main())
