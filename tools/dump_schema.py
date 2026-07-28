"""Выгрузка структуры БД ТГИД в JSON для анализа и генерации конвертера.

Запуск:
    python tools/dump_schema.py --out docs/schema/almatygid.json
"""
import argparse
import json
import os
import sys

import psycopg2
import psycopg2.extras


def q(cur, sql, args=None):
    # Без vars — иначе psycopg2 трактует % в LIKE-шаблонах как плейсхолдер.
    if args is None:
        cur.execute(sql)
    else:
        cur.execute(sql, args)
    return [dict(r) for r in cur.fetchall()]


def dump(conn):
    cur = conn.cursor(cursor_factory=psycopg2.extras.RealDictCursor)

    tables = q(cur, """
        select c.relname as name,
               c.relkind as kind,
               c.reltuples::bigint as est_rows,
               pg_total_relation_size(c.oid) as total_bytes,
               obj_description(c.oid) as comment
        from pg_class c
        join pg_namespace n on n.oid = c.relnamespace
        where n.nspname = 'public' and c.relkind in ('r', 'v', 'm', 'p')
        order by c.relname
    """)

    columns = q(cur, """
        select table_name, column_name, ordinal_position, data_type,
               character_maximum_length, numeric_precision, numeric_scale,
               is_nullable, column_default
        from information_schema.columns
        where table_schema = 'public'
        order by table_name, ordinal_position
    """)

    col_comments = q(cur, """
        select c.relname as table_name, a.attname as column_name,
               col_description(c.oid, a.attnum) as comment
        from pg_class c
        join pg_namespace n on n.oid = c.relnamespace
        join pg_attribute a on a.attrelid = c.oid
        where n.nspname = 'public' and a.attnum > 0 and not a.attisdropped
          and col_description(c.oid, a.attnum) is not null
    """)

    constraints = q(cur, """
        select con.conname as name,
               rel.relname as table_name,
               con.contype as type,
               pg_get_constraintdef(con.oid) as definition,
               confrel.relname as ref_table
        from pg_constraint con
        join pg_class rel on rel.oid = con.conrelid
        join pg_namespace n on n.oid = rel.relnamespace
        left join pg_class confrel on confrel.oid = con.confrelid
        where n.nspname = 'public'
        order by rel.relname, con.contype, con.conname
    """)

    indexes = q(cur, """
        select tablename as table_name, indexname as name, indexdef as definition
        from pg_indexes
        where schemaname = 'public'
        order by tablename, indexname
    """)

    geom_columns = q(cur, """
        select f_table_name as table_name, f_geometry_column as column_name,
               coord_dimension, srid, type
        from geometry_columns
        where f_table_schema = 'public'
        order by f_table_name
    """)

    triggers = q(cur, """
        select c.relname as table_name, t.tgname as name,
               pg_get_triggerdef(t.oid) as definition
        from pg_trigger t
        join pg_class c on c.oid = t.tgrelid
        join pg_namespace n on n.oid = c.relnamespace
        where n.nspname = 'public' and not t.tgisinternal
        order by c.relname, t.tgname
    """)

    routines = q(cur, """
        select p.proname as name,
               pg_get_function_identity_arguments(p.oid) as args,
               pg_get_functiondef(p.oid) as definition
        from pg_proc p
        join pg_namespace n on n.oid = p.pronamespace
        where n.nspname = 'public' and p.prokind = 'f'
          and p.proname not like 'st\\_%' and p.proname not like '\\_st\\_%'
          and p.proname not like 'postgis%' and p.proname not like 'geometry%'
          and p.proname not like 'geography%' and p.proname not like 'box%'
        order by p.proname
    """)

    views = q(cur, """
        select table_name as name, view_definition as definition
        from information_schema.views
        where table_schema = 'public'
        order by table_name
    """)

    sequences = q(cur, """
        select sequence_name as name from information_schema.sequences
        where sequence_schema = 'public' order by sequence_name
    """)

    # Точные счётчики строк — только для таблиц, где оценка не нулевая,
    # иначе полный count по 646 таблицам занимает минуты.
    exact_counts = {}
    for t in tables:
        if t['kind'] != 'r':
            continue
        cur.execute('select count(*) as n from public.%s' % quote_ident(t['name']))
        exact_counts[t['name']] = cur.fetchone()['n']

    return {
        'tables': tables,
        'columns': columns,
        'column_comments': col_comments,
        'constraints': constraints,
        'indexes': indexes,
        'geometry_columns': geom_columns,
        'triggers': triggers,
        'routines': routines,
        'views': views,
        'sequences': sequences,
        'exact_counts': exact_counts,
    }


def quote_ident(name):
    return '"' + name.replace('"', '""') + '"'


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--host', default=os.environ.get('TGID_HOST', 'localhost'))
    ap.add_argument('--port', type=int, default=int(os.environ.get('TGID_PORT', 5440)))
    ap.add_argument('--db', default=os.environ.get('TGID_DB', 'almatygid'))
    ap.add_argument('--user', default=os.environ.get('TGID_USER', 'postgres'))
    ap.add_argument('--password', default=os.environ.get('PGPASSWORD', ''))
    ap.add_argument('--out', required=True)
    args = ap.parse_args()

    conn = psycopg2.connect(host=args.host, port=args.port, dbname=args.db,
                            user=args.user, password=args.password)
    try:
        data = dump(conn)
    finally:
        conn.close()

    os.makedirs(os.path.dirname(args.out), exist_ok=True)
    with open(args.out, 'w', encoding='utf-8') as f:
        json.dump(data, f, ensure_ascii=False, indent=1)

    print('tables=%d columns=%d fk=%d geom=%d -> %s' % (
        len(data['tables']), len(data['columns']),
        sum(1 for c in data['constraints'] if c['type'] == 'f'),
        len(data['geometry_columns']), args.out))


if __name__ == '__main__':
    sys.exit(main())
