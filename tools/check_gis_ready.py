"""Проверка пригодности слоёв net для внешних ГИС (QGIS, GeoServer).

QGIS требует от таблицы-слоя: зарегистрированную геометрию с корректным SRID,
первичный ключ целого типа, пространственный индекс и валидную геометрию.
"""
import argparse
import os

import psycopg2


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', type=int, default=int(os.environ.get('TGID_PORT', 5440)))
    ap.add_argument('--db', default=os.environ.get('TGID_DB', 'almatygid'))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    args = ap.parse_args()

    conn = psycopg2.connect(host=args.host, port=args.port, dbname=args.db,
                            user=args.user, password=args.password)
    cur = conn.cursor()

    cur.execute("""
        SELECT g.f_table_name, g.type, g.srid,
               EXISTS (SELECT 1 FROM pg_index i
                       JOIN pg_class c ON c.oid = i.indrelid
                       JOIN pg_namespace ns ON ns.oid = c.relnamespace
                       WHERE ns.nspname = 'net' AND c.relname = g.f_table_name
                         AND i.indisprimary) AS has_pk,
               EXISTS (SELECT 1 FROM pg_indexes
                       WHERE schemaname = 'net' AND tablename = g.f_table_name
                         AND indexdef LIKE '%gist%') AS has_gist
        FROM geometry_columns g
        WHERE g.f_table_schema = 'net'
        ORDER BY g.f_table_name
    """)
    rows = cur.fetchall()

    print('%-26s %-12s %6s %6s %6s %10s %8s' % (
        'СЛОЙ', 'ГЕОМЕТРИЯ', 'SRID', 'PK', 'GIST', 'ОБЪЕКТОВ', 'БИТЫХ'))
    problems = 0
    total = 0
    for name, gtype, srid, pk, gist in rows:
        cur.execute('SELECT count(*), count(*) FILTER '
                    '(WHERE NOT ST_IsValid(geom)) FROM net.%s' % name)
        n, bad = cur.fetchone()
        total += n
        ok = pk and gist and srid == 9998 and bad == 0
        if not ok:
            problems += 1
        print('%-26s %-12s %6s %6s %6s %10d %8d %s' % (
            name, gtype, srid, 'да' if pk else 'НЕТ',
            'да' if gist else 'НЕТ', n, bad, '' if ok else '  <-- проблема'))

    cur.execute("SELECT count(*) FROM spatial_ref_sys WHERE srid = 9998 "
                "AND proj4text IS NOT NULL AND proj4text <> ''")
    srid_ok = cur.fetchone()[0]

    print('\nслоёв: %d, объектов с геометрией: %d' % (len(rows), total))
    print('SRID 9998 описан в spatial_ref_sys: %s' % ('да' if srid_ok else 'НЕТ'))
    print('слоёв с проблемами: %d' % problems)

    conn.close()
    return 1 if problems or not srid_ok else 0


if __name__ == '__main__':
    raise SystemExit(main())
