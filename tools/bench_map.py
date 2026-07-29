"""Замер тонкого представления карты против полного us_net / ut_net.

Тонкие views содержат geom (для QGIS). Для сравнения с приложением
по умолчанию выбираются текстовые coords / x,y — без передачи geometry.

    python tools/bench_map.py --db tgid_gis --fragments 80
    python tools/bench_map.py --db tgid_gis --fragments 80 --with-geom
    python tools/bench_map.py --report docs/schema/map_thin_bench.json
"""
import argparse
import json
import os
import statistics
import sys
import time

import psycopg2


def load_env(path='.env'):
    if not os.path.isfile(path):
        return
    with open(path, encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#') or '=' not in line:
                continue
            k, v = line.split('=', 1)
            os.environ.setdefault(k.strip(), v.strip().strip('"').strip("'"))


def read_sql(path):
    for enc in ('utf-8', 'cp1251'):
        try:
            with open(path, encoding=enc) as f:
                return f.read()
        except UnicodeDecodeError:
            continue
    raise SystemExit('не удалось прочитать %s' % path)


def timed(cur, sql, runs):
    times = []
    ncols = nrows = None
    for _ in range(runs):
        t0 = time.time()
        cur.execute(sql)
        rows = cur.fetchall()
        times.append(time.time() - t0)
        nrows = len(rows)
        ncols = len(cur.description)
    return {
        'rows': nrows,
        'cols': ncols,
        'min_ms': round(min(times) * 1000, 1),
        'median_ms': round(statistics.median(times) * 1000, 1),
    }


NODE_THIN = """
SELECT id, fileid, x, y, type_txt, displaysign, externalnodename,
       nodename, externalcodeid, internalnodeid, externalsignid,
       removed, id2, class_table{geom}
FROM net.v_map_nodes n
WHERE TRUE
"""

LINE_THIN = """
SELECT id, fileid, nodeid1, nodeid2, type_txt, displaysign,
       diametercondit, pipesectlength, magistral, coords,
       removed, id2, externalsignlineid, class_table{geom}
FROM net.v_map_lines n1
WHERE TRUE
"""


def main():
    load_env()
    ap = argparse.ArgumentParser()
    ap.add_argument('--db', default=os.environ.get('TGID_DB', 'tgid_gis'))
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', type=int,
                    default=int(os.environ.get('TGID_PORT', 5440)))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    ap.add_argument('--fragments', default='80')
    ap.add_argument('--runs', type=int, default=3)
    ap.add_argument('--us', default='sql/sql3/us_net.sql')
    ap.add_argument('--ut', default='sql/sql3/ut_net.sql')
    ap.add_argument('--with-geom', action='store_true',
                    help='включать колонку geom (дороже по сети)')
    ap.add_argument('--report', default='')
    args = ap.parse_args()

    conn = psycopg2.connect(host=args.host, port=args.port, dbname=args.db,
                            user=args.user, password=args.password)
    cur = conn.cursor()

    cur.execute("""
        SELECT count(*) FROM pg_class c
        JOIN pg_namespace n ON n.oid = c.relnamespace
        WHERE n.nspname = 'net' AND c.relname IN ('v_map_nodes', 'v_map_lines')
    """)
    if cur.fetchone()[0] < 2:
        print('ПРОВАЛ: сначала примените sql/070_map_thin.sql')
        return 1

    frag = args.fragments
    g = ', geom' if args.with_geom else ''
    us = read_sql(args.us) + (' AND n.fileID IN (%s)' % frag)
    ut = read_sql(args.ut) + (' AND n1.fileID IN (%s)' % frag)
    # removed уже отфильтрован внутри view (removed_at IS NULL в ветвях)
    node_thin = (NODE_THIN.format(geom=g)
                 + (' AND n.fileID IN (%s)' % frag))
    line_thin = (LINE_THIN.format(geom=g)
                 + (' AND n1.fileID IN (%s)' % frag))

    report = {
        'db': args.db,
        'fragments': frag,
        'runs': args.runs,
        'with_geom': args.with_geom,
        'nodes_full': timed(cur, us, args.runs),
        'nodes_thin': timed(cur, node_thin, args.runs),
        'lines_full': timed(cur, ut, args.runs),
        'lines_thin': timed(cur, line_thin, args.runs),
    }

    for kind in ('nodes', 'lines'):
        full = report['%s_full' % kind]
        thin = report['%s_thin' % kind]
        speed = (full['min_ms'] / thin['min_ms']) if thin['min_ms'] else None
        report['%s_speedup' % kind] = round(speed, 2) if speed else None
        print('%s  full: %5d строк, %2d кол, %7.1f мс'
              % (kind, full['rows'], full['cols'], full['min_ms']))
        print('%s  thin: %5d строк, %2d кол, %7.1f мс  (x%.2f)'
              % (kind, thin['rows'], thin['cols'], thin['min_ms'],
                 speed or 0))

    conn.close()
    if args.report:
        os.makedirs(os.path.dirname(args.report) or '.', exist_ok=True)
        with open(args.report, 'w', encoding='utf-8') as f:
            json.dump(report, f, ensure_ascii=False, indent=2)
        print('отчёт:', args.report)
    return 0


if __name__ == '__main__':
    sys.exit(main())
