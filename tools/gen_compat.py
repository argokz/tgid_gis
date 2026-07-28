"""Генерация слоя совместимости: представления в форме прежних public.nodes
и public.linesobj поверх схемы net.

Представления создаются в схеме net под именами v_nodes / v_linesobj —
это позволяет сверить их с оригиналом, ничего не переименовывая.
Переключение приложения — отдельный шаг (sql/040_switch_to_net.sql).
"""
import argparse
import json
import os

import psycopg2

# Колонки, которых в net нет: восстанавливаем из геометрии или подставляем
# то же значение, что было в public.
NODE_EXPR = {
    'x': 'ST_X({t}.geom) * 100.0',
    'y': '-ST_Y({t}.geom) * 100.0',
    'shape': '{t}.geom',
    # В public.nodes.removed встречаются только 0 и 1, поэтому
    # восстановление из removed_at точное.
    'removed': 'CASE WHEN {t}.removed_at IS NULL THEN 0 ELSE 1 END',
    'idremoved': 'NULL::int',
    'fileid': '{t}.fragment_id',
    'coords': "''::text",
    'globalid': 'NULL::text',
    'gistable': 'NULL::text',
    'sync': 'NULL::boolean',
    'gis': 'NULL::boolean',
    'sync_tgid': 'false',
    'id_old': '0',
}

LINE_EXPR = dict(NODE_EXPR)
LINE_EXPR.update({
    'shape': '{t}.geom',
    'coords': '{t}.coords_legacy',
    'nodeid1': '{t}.node_from_src',
    'nodeid2': '{t}.node_to_src',
    # Именно исходный fileid линии, а не фрагмент начального узла:
    # представление обязано повторять прежнее поведение.
    'fileid': '{t}.fileid_src',
})


def legacy_columns(cur, table):
    cur.execute("""SELECT column_name, data_type
                   FROM information_schema.columns
                   WHERE table_schema = 'public' AND table_name = %s
                   ORDER BY ordinal_position""", (table,))
    return cur.fetchall()


def net_columns(cur, table):
    cur.execute("""SELECT column_name FROM information_schema.columns
                   WHERE table_schema = 'net' AND table_name = %s""", (table,))
    return {r[0] for r in cur.fetchall()}


def build_view(cur, view_name, legacy_table, targets, expr):
    cols = legacy_columns(cur, legacy_table)
    branches = []
    for t in targets:
        have = net_columns(cur, t)
        parts = []
        for name, dtype in cols:
            if name in expr:
                parts.append('%s AS %s' % (expr[name].format(t=t), name))
            elif name in have:
                parts.append('%s.%s' % (t, name))
            else:
                parts.append('NULL::%s AS %s' % (dtype, name))
        branches.append('SELECT\n    %s\nFROM net.%s %s'
                        % (',\n    '.join(parts), t, t))

    return ('CREATE OR REPLACE VIEW net.%s AS\n%s;\n'
            % (view_name, '\nUNION ALL\n'.join(branches)))


HEADER = """-- СГЕНЕРИРОВАНО tools/gen_compat.py
--
-- Слой совместимости: net.v_nodes и net.v_linesobj повторяют форму
-- прежних public.nodes и public.linesobj, но читают из схемы net.
--
-- Создаются РЯДОМ с оригиналом, ничего не переименовывая: это позволяет
-- построчно сверить их с public до какого-либо переключения
-- (tools/check_compat.py). Переключение — отдельный шаг.
--
-- Колонки, которых в net больше нет, восстанавливаются:
--   x, y      — из геометрии (сантиметры, ось Y инвертирована)
--   shape     — это и есть geom
--   coords    — вычисляемая колонка coords_legacy
--   removed   — всегда 0: удалённые в net помечены removed_at
--               и в представление не попадают
"""


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--mapping', required=True)
    ap.add_argument('--out', default='sql/030_compat_views.sql')
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

    node_tables = [e['target'] for e in m['class_node']] + ['node_plain']
    line_tables = [e['target'] for e in m['class_line']] + ['line_plain']

    sql = HEADER
    sql += '\n' + build_view(cur, 'v_nodes', 'nodes', node_tables, NODE_EXPR)
    sql += '\n' + build_view(cur, 'v_linesobj', 'linesobj', line_tables,
                             LINE_EXPR)

    os.makedirs(os.path.dirname(args.out), exist_ok=True)
    with open(args.out, 'w', encoding='utf-8') as f:
        f.write(sql)
    print('-> %s (узлов %d ветвей, линий %d ветвей)'
          % (args.out, len(node_tables), len(line_tables)))

    conn.close()


if __name__ == '__main__':
    main()
