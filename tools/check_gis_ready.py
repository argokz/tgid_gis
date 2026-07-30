"""Проверка пригодности слоёв net для внешних ГИС (QGIS, GeoServer).

QGIS требует от таблицы-слоя: зарегистрированную геометрию с корректным SRID,
первичный ключ целого типа, пространственный индекс и валидную геометрию.
"""
import argparse
import os

import psycopg2
from psycopg2 import sql


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
        SELECT g.f_table_name, g.f_geometry_column, g.type, g.srid,
               EXISTS (SELECT 1 FROM pg_index i
                       JOIN pg_attribute pa
                         ON pa.attrelid = i.indrelid
                        AND pa.attnum = ANY(i.indkey)
                       WHERE i.indrelid = c.oid
                         AND i.indisprimary
                         AND pa.attname = 'id'
                         AND pa.atttypid IN ('int4'::regtype, 'int8'::regtype)
                         AND array_length(i.indkey, 1) = 1) AS has_integer_pk,
               EXISTS (SELECT 1 FROM pg_index i
                       JOIN pg_class ic ON ic.oid = i.indexrelid
                       JOIN pg_am am ON am.oid = ic.relam
                       JOIN pg_attribute ga
                         ON ga.attrelid = i.indrelid
                        AND ga.attnum = ANY(i.indkey)
                       WHERE i.indrelid = c.oid
                         AND am.amname = 'gist'
                         AND ga.attname = g.f_geometry_column) AS has_gist,
               a.attnotnull AS geom_not_null
        FROM geometry_columns g
        JOIN pg_namespace n ON n.nspname = g.f_table_schema
        JOIN pg_class c
          ON c.relnamespace = n.oid
         AND c.relname = g.f_table_name
         AND c.relkind IN ('r', 'p')
        JOIN pg_attribute a
          ON a.attrelid = c.oid
         AND a.attname = g.f_geometry_column
         AND NOT a.attisdropped
        WHERE g.f_table_schema = 'net'
        ORDER BY g.f_table_name, g.f_geometry_column
    """)
    rows = cur.fetchall()

    print('%-26s %-12s %6s %6s %6s %6s %10s %8s  %s' % (
        'СЛОЙ', 'ГЕОМЕТРИЯ', 'SRID', 'PK', 'GIST', 'NN',
        'ОБЪЕКТОВ', 'БИТЫХ', 'ФАКТИЧЕСКИЕ ТИПЫ / ПРОБЛЕМЫ'))
    problems = 0
    total = 0
    for name, geom_column, gtype, srid, pk, gist, not_null in rows:
        cur.execute(
            sql.SQL("""
                SELECT count(*),
                       count(*) FILTER (WHERE {geom} IS NULL),
                       count(*) FILTER (
                           WHERE {geom} IS NOT NULL
                             AND NOT ST_IsValid({geom})),
                       array_agg(DISTINCT GeometryType({geom}))
                           FILTER (WHERE {geom} IS NOT NULL)
                FROM {schema}.{table}
            """).format(
                geom=sql.Identifier(geom_column),
                schema=sql.Identifier('net'),
                table=sql.Identifier(name),
            )
        )
        n, nulls, bad, actual_types = cur.fetchone()
        actual_types = actual_types or []
        total += n
        reasons = []
        if not pk:
            reasons.append('нет целочисленного PK id')
        if not gist:
            reasons.append('нет GiST по геометрии')
        if not not_null or nulls:
            reasons.append('геометрия допускает/содержит NULL')
        if srid != 9998:
            reasons.append('SRID != 9998')
        if bad:
            reasons.append('невалидная геометрия')
        if gtype.upper() == 'GEOMETRY':
            reasons.append('тип geometry не конкретизирован')
        if len(actual_types) > 1:
            reasons.append('смешанные типы')
        if (actual_types and gtype.upper() != 'GEOMETRY'
                and any(t.upper() != gtype.upper() for t in actual_types)):
            reasons.append('фактический тип не совпадает с DDL')
        ok = not reasons
        if not ok:
            problems += 1
        type_text = ','.join(actual_types) if actual_types else '-'
        suffix = '' if ok else '  <-- ' + '; '.join(reasons)
        print('%-26s %-12s %6s %6s %6s %6s %10d %8d  %s%s' % (
            name, gtype, srid, 'да' if pk else 'НЕТ',
            'да' if gist else 'НЕТ', 'да' if not_null else 'НЕТ',
            n, bad, type_text, suffix))

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
