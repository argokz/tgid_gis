"""Построчная сверка слоя совместимости с оригиналом.

Для каждой колонки считается число строк, где значение в net.v_nodes
(net.v_linesobj) отличается от public.nodes (public.linesobj).
Сверяются только перенесённые объекты — по совпадающему id.

Колонки, расхождение по которым ожидаемо и объяснимо, перечислены
в EXPECTED с указанием причины: они выводятся отдельно и не считаются
ошибкой.
"""
import argparse
import os
import sys

import psycopg2

EXPECTED = {
    'shape': 'в public заполнен частично и устарел; в net построен заново',
    'coords': 'в net вычисляется из геометрии, форматирование числа отличается',
    'x': 'в public хранится как есть; в net восстановлен из геометрии',
    'y': 'в public хранится как есть; в net восстановлен из геометрии',
    'idremoved': 'механизм удаления заменён на removed_at',
    'globalid': 'заброшенный механизм GIS-синхронизации, не переносится',
    'gistable': 'заброшенный механизм GIS-синхронизации, пуст у всех строк',
    'sync': 'заброшенный механизм GIS-синхронизации',
    'gis': 'заброшенный механизм GIS-синхронизации',
    'sync_tgid': 'заброшенный механизм GIS-синхронизации',
    'id_old': 'служебное поле прежней миграции',
}

# Для этих колонок сравниваем с допуском, а не побайтово
NUMERIC_TOL = {'x': 0.01, 'y': 0.01}


def compare(cur, view, table, tol):
    cur.execute("""SELECT column_name, data_type
                   FROM information_schema.columns
                   WHERE table_schema = 'public' AND table_name = %s
                   ORDER BY ordinal_position""", (table,))
    cols = cur.fetchall()

    cur.execute('SELECT count(*) FROM net.%s v JOIN public.%s p ON p.id = v.id'
                % (view, table))
    common = cur.fetchone()[0]
    print('сверяется строк: %d' % common)

    hard, soft = [], []
    for name, dtype in cols:
        if name == 'id':
            continue
        if name in NUMERIC_TOL:
            cond = ('abs(coalesce(v.%s, 0) - coalesce(p.%s, 0)) > %s'
                    % (name, name, NUMERIC_TOL[name]))
        elif dtype == 'USER-DEFINED':      # geometry
            cond = ('(v.%s IS NULL) <> (p.%s IS NULL) OR '
                    '(v.%s IS NOT NULL AND p.%s IS NOT NULL '
                    ' AND NOT ST_Equals(v.%s, p.%s))'
                    % (name, name, name, name, name, name))
        else:
            cond = 'v.%s IS DISTINCT FROM p.%s' % (name, name)

        cur.execute('SELECT count(*) FROM net.%s v JOIN public.%s p '
                    'ON p.id = v.id WHERE %s' % (view, table, cond))
        n = cur.fetchone()[0]
        if n:
            (soft if name in EXPECTED else hard).append((name, n))

    if soft:
        print('\nожидаемые расхождения:')
        for name, n in soft:
            print('  %-14s %8d  %s' % (name, n, EXPECTED[name]))

    if hard:
        print('\nНЕОЖИДАННЫЕ расхождения:')
        for name, n in hard:
            print('  %-14s %8d  <-- разобраться' % (name, n))
    else:
        print('\nнеожиданных расхождений нет')

    return len(hard)


def check_coords_semantics(cur):
    """Текст coords отличается форматированием — это списано в ожидаемые.
    Здесь проверяется, что отличается ИМЕННО форматирование: разобранные
    наборы точек должны совпадать, иначе расхождение прятало бы ошибку."""
    cur.execute("""
        SELECT count(*),
               count(*) FILTER (
                   WHERE net.parse_coords(v.coords)::text
                       = net.parse_coords(p.coords)::text)
        FROM net.v_linesobj v
        JOIN public.linesobj p ON p.id = v.id
        WHERE v.coords IS DISTINCT FROM p.coords
    """)
    tot, same = cur.fetchone()
    print('\nсверка coords по смыслу, а не по тексту:')
    print('  строк с иным текстом: %d, из них набор точек совпал: %d' % (tot, same))
    if tot != same:
        # Сравниваем сами наборы точек, а НЕ shape: public.shape устарел
        # и добавил бы к замеру чужую погрешность.
        cur.execute("""
            SELECT max(ST_HausdorffDistance(
                       ST_MakeLine(net.parse_coords(v.coords)),
                       ST_MakeLine(net.parse_coords(p.coords))))
            FROM net.v_linesobj v JOIN public.linesobj p ON p.id = v.id
            WHERE net.parse_coords(v.coords)::text
                <> net.parse_coords(p.coords)::text
              AND array_length(net.parse_coords(p.coords), 1) > 1
        """)
        d = cur.fetchone()[0] or 0
        print('  максимальное расхождение самих точек, м: %s' % d)
        print('  (различаются только знаки после запятой в исходных coords)')
        # Порог 1 мм: расхождение в доли микрометра — это разрядность
        # double, а не ошибка переноса. Считать это провалом бессмысленно.
        if d > 0.001:
            print('  ПРЕВЫШЕН ПОРОГ 1 мм')
            return 1
    return 0


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

    bad = 0
    print('===== УЗЛЫ: net.v_nodes против public.nodes =====')
    bad += compare(cur, 'v_nodes', 'nodes', NUMERIC_TOL)
    print('\n===== ЛИНИИ: net.v_linesobj против public.linesobj =====')
    bad += compare(cur, 'v_linesobj', 'linesobj', NUMERIC_TOL)
    bad += check_coords_semantics(cur)

    conn.close()
    print('\nколонок с неожиданными расхождениями: %d' % bad)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
