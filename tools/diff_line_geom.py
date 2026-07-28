"""Разбор линий, у которых построенная геометрия разошлась с прежним public.shape."""
import argparse
import json
import os

import psycopg2


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--mapping', required=True)
    ap.add_argument('--limit', type=int, default=10)
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', type=int, default=int(os.environ.get('TGID_PORT', 5440)))
    ap.add_argument('--db', default=os.environ.get('TGID_DB', 'almatygid'))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    args = ap.parse_args()

    with open(args.mapping, encoding='utf-8') as f:
        m = json.load(f)
    tables = [e['target'] for e in m['class_line']] + ['line_plain']
    union = ' UNION ALL '.join('SELECT src_id, geom FROM net.%s' % t
                               for t in tables)

    conn = psycopg2.connect(host=args.host, port=args.port, dbname=args.db,
                            user=args.user, password=args.password)
    cur = conn.cursor()

    cur.execute("""
        SELECT count(*),
               count(*) FILTER (WHERE ST_NPoints(u.geom) <> ST_NPoints(l.shape)),
               count(*) FILTER (WHERE ST_NPoints(u.geom) = ST_NPoints(l.shape)),
               max(ST_HausdorffDistance(u.geom, l.shape))
        FROM (%s) u JOIN public.linesobj l ON l.id = u.src_id
        WHERE l.shape IS NOT NULL
          AND ST_HausdorffDistance(u.geom, l.shape) >= 0.001
    """ % union)
    tot, diff_np, same_np, maxd = cur.fetchone()
    print('расхождений: %d' % tot)
    print('  из них с другим числом вершин: %d' % diff_np)
    print('  с тем же числом вершин:        %d' % same_np)
    print('  максимальное расхождение, м:   %.3f' % (maxd or 0))

    print('\nраспределение по величине расхождения:')
    cur.execute("""
        SELECT width_bucket(ST_HausdorffDistance(u.geom, l.shape),
                            0, 100, 5) b,
               count(*), min(ST_HausdorffDistance(u.geom, l.shape)),
               max(ST_HausdorffDistance(u.geom, l.shape))
        FROM (%s) u JOIN public.linesobj l ON l.id = u.src_id
        WHERE l.shape IS NOT NULL
          AND ST_HausdorffDistance(u.geom, l.shape) >= 0.001
        GROUP BY b ORDER BY b
    """ % union)
    for b, n, mn, mx in cur.fetchall():
        print('  корзина %s: %5d шт, от %.3f до %.3f м' % (b, n, mn, mx))

    print('\nпримеры:')
    cur.execute("""
        SELECT l.id, ST_NPoints(u.geom), ST_NPoints(l.shape),
               round(ST_HausdorffDistance(u.geom, l.shape)::numeric, 2),
               length(l.coords), left(l.coords, 70)
        FROM (%s) u JOIN public.linesobj l ON l.id = u.src_id
        WHERE l.shape IS NOT NULL
          AND ST_HausdorffDistance(u.geom, l.shape) >= 0.001
        ORDER BY ST_HausdorffDistance(u.geom, l.shape) DESC
        LIMIT %d
    """ % (union, args.limit))
    print('%10s %5s %5s %10s %7s  %s' % (
        'id', 'нов', 'стар', 'расх,м', 'len', 'coords'))
    for r in cur.fetchall():
        print('%10s %5s %5s %10s %7s  %s' % r)

    conn.close()


if __name__ == '__main__':
    main()
