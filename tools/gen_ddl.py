"""Генерация DDL целевой схемы net из converter/mapping.json.

Результат — sql/010_net_schema.sql. Файл идемпотентен (CREATE IF NOT EXISTS),
но по умолчанию рассчитан на создание схемы с нуля.
"""
import argparse
import json
import os

HEADER = """-- СГЕНЕРИРОВАНО tools/gen_ddl.py — править руками не нужно.
-- Целевая схема ТГИД: одна таблица — один тип объекта.
--
-- Идентификаторы всех объектов выдаёт общая последовательность net.obj_id_seq,
-- поэтому id уникален глобально. Реестры net.node_reg / net.line_reg
-- существуют ради настоящих внешних ключей от линий к узлам.

CREATE SCHEMA IF NOT EXISTS net;

CREATE SEQUENCE IF NOT EXISTS net.obj_id_seq AS bigint START 1;

-- Фрагменты (расчётные схемы). Переносится из public.fragments.
CREATE TABLE IF NOT EXISTS net.fragment (
    id           int PRIMARY KEY,
    name         text NOT NULL,
    name_sys     text,
    nasel_point  text,
    sezon        text,
    year         text,
    removed_at   timestamptz
);

-- Реестр точечных объектов. Наполняется триггерами объектных таблиц.
CREATE TABLE IF NOT EXISTS net.node_reg (
    id   bigint PRIMARY KEY,
    kind text NOT NULL,
    tbl  text NOT NULL
);
CREATE INDEX IF NOT EXISTS node_reg_kind_idx ON net.node_reg (kind);

-- Реестр линейных объектов.
CREATE TABLE IF NOT EXISTS net.line_reg (
    id   bigint PRIMARY KEY,
    kind text NOT NULL,
    tbl  text NOT NULL
);
CREATE INDEX IF NOT EXISTS line_reg_kind_idx ON net.line_reg (kind);

-- Линии, оба конца которых не разрешаются в узлы. Геометрию построить нельзя.
-- Хранятся отдельно, чтобы не терять данные и не засорять рабочую сеть.
CREATE TABLE IF NOT EXISTS net.line_orphan (
    id           bigint PRIMARY KEY,
    src_table    text NOT NULL,
    nodeid1      int,
    nodeid2      int,
    reason       text NOT NULL,
    payload      jsonb NOT NULL
);

-- Триггерные функции синхронизации реестров.
CREATE OR REPLACE FUNCTION net.reg_node_sync() RETURNS trigger
LANGUAGE plpgsql AS $$
BEGIN
    IF TG_OP = 'INSERT' THEN
        INSERT INTO net.node_reg (id, kind, tbl)
        VALUES (NEW.id, TG_ARGV[0], TG_TABLE_NAME);
        RETURN NEW;
    ELSE
        DELETE FROM net.node_reg WHERE id = OLD.id;
        RETURN OLD;
    END IF;
END $$;

CREATE OR REPLACE FUNCTION net.reg_line_sync() RETURNS trigger
LANGUAGE plpgsql AS $$
BEGIN
    IF TG_OP = 'INSERT' THEN
        INSERT INTO net.line_reg (id, kind, tbl)
        VALUES (NEW.id, TG_ARGV[0], TG_TABLE_NAME);
        RETURN NEW;
    ELSE
        DELETE FROM net.line_reg WHERE id = OLD.id;
        RETURN OLD;
    END IF;
END $$;
"""

# information_schema.data_type -> тип в DDL
TYPE_MAP = {
    'integer': 'int',
    'bigint': 'bigint',
    'smallint': 'smallint',
    'double precision': 'double precision',
    'real': 'real',
    'numeric': 'numeric',
    'boolean': 'boolean',
    'date': 'date',
    'text': 'text',
    'character varying': 'text',
    'character': 'text',
    'timestamp without time zone': 'timestamp',
    'timestamp with time zone': 'timestamptz',
    'time without time zone': 'time',
    'uuid': 'uuid',
    'jsonb': 'jsonb',
    'json': 'jsonb',
    'bytea': 'bytea',
    'ARRAY': 'text[]',
    'USER-DEFINED': 'text',
}

RESERVED = {
    'year', 'name', 'end', 'order', 'group', 'user', 'default', 'check',
    'column', 'table', 'all', 'and', 'or', 'not', 'null', 'offset', 'limit',
}


def ident(name):
    if name in RESERVED or not name.replace('_', '').isalnum() or name[0].isdigit():
        return '"%s"' % name
    return name


def col_ddl(name, meta):
    t = TYPE_MAP.get(meta['data_type'])
    if t is None:
        t = 'text'
    return '    %-34s %s' % (ident(name), t)


def table_ddl(e, geom_type, base_cols, base_types, is_line):
    lines = []
    lines.append('\n-- %s  <-  public.%s  (%s строк)' % (
        e['target'], e['source'], e['rows']))
    lines.append('CREATE TABLE IF NOT EXISTS net.%s (' % e['target'])
    lines.append("    id                                 bigint PRIMARY KEY "
                 "DEFAULT nextval('net.obj_id_seq')")
    lines.append('  , fragment_id                        int '
                 'REFERENCES net.fragment(id)')
    if is_line:
        lines.append('  , node_from                          bigint NOT NULL '
                     'REFERENCES net.node_reg(id) ON DELETE RESTRICT')
        lines.append('  , node_to                            bigint NOT NULL '
                     'REFERENCES net.node_reg(id) ON DELETE RESTRICT')
    lines.append('  , geom                               geometry(%s, 9998) '
                 'NOT NULL' % geom_type)
    lines.append('  , removed_at                         timestamptz')
    lines.append('  , src_id                             int')

    # Общие поля базовой таблицы (nodes / linesobj)
    for c in base_cols:
        if c in ('id', 'fileid', 'nodeid1', 'nodeid2'):
            continue
        lines.append('  , ' + col_ddl(c, base_types[c]).lstrip())

    # Собственные поля подтипа
    for c in e['columns']:
        if c in base_cols or c in ('id', 'fileid'):
            continue
        lines.append('  , ' + col_ddl(c, e['column_types'][c]).lstrip())

    lines.append(');')
    lines.append('CREATE INDEX IF NOT EXISTS %s_geom_idx ON net.%s '
                 'USING gist (geom);' % (e['target'], e['target']))
    lines.append('CREATE INDEX IF NOT EXISTS %s_frag_idx ON net.%s '
                 '(fragment_id) WHERE removed_at IS NULL;'
                 % (e['target'], e['target']))
    lines.append('CREATE INDEX IF NOT EXISTS %s_src_idx ON net.%s (src_id);'
                 % (e['target'], e['target']))
    if is_line:
        lines.append('CREATE INDEX IF NOT EXISTS %s_from_idx ON net.%s '
                     '(node_from);' % (e['target'], e['target']))
        lines.append('CREATE INDEX IF NOT EXISTS %s_to_idx ON net.%s '
                     '(node_to);' % (e['target'], e['target']))
    reg = 'line' if is_line else 'node'
    lines.append(
        'DROP TRIGGER IF EXISTS %s_reg ON net.%s;\n'
        'CREATE TRIGGER %s_reg AFTER INSERT OR DELETE ON net.%s\n'
        "    FOR EACH ROW EXECUTE FUNCTION net.reg_%s_sync('%s');"
        % (e['target'], e['target'], e['target'], e['target'],
           reg, e['target']))
    return '\n'.join(lines)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--mapping', required=True)
    ap.add_argument('--schema', required=True)
    ap.add_argument('--out', required=True)
    args = ap.parse_args()

    with open(args.mapping, encoding='utf-8') as f:
        m = json.load(f)
    with open(args.schema, encoding='utf-8') as f:
        d = json.load(f)

    base_types = {}
    for c in d['columns']:
        base_types.setdefault(c['table_name'], {})[c['column_name']] = {
            'data_type': c['data_type'],
            'max_length': c['character_maximum_length'],
        }

    out = [HEADER]

    out.append('\n-- ============ ТОЧЕЧНЫЕ ОБЪЕКТЫ ============')
    node_base = m['base']['nodes']['columns']
    for e in m['node_types']:
        out.append(table_ddl(e, 'Point', node_base, base_types['nodes'], False))

    # Узлы без подтипа
    plain_node = {'target': 'node_plain', 'source': 'nodes', 'rows': 'остаток',
                  'columns': [], 'column_types': {}}
    out.append(table_ddl(plain_node, 'Point', node_base,
                         base_types['nodes'], False))

    out.append('\n-- ============ ЛИНЕЙНЫЕ ОБЪЕКТЫ ============')
    line_base = m['base']['linesobj']['columns']
    for e in m['line_types']:
        out.append(table_ddl(e, 'LineString', line_base,
                             base_types['linesobj'], True))

    plain_line = {'target': 'line_plain', 'source': 'linesobj',
                  'rows': 'остаток', 'columns': [], 'column_types': {}}
    out.append(table_ddl(plain_line, 'LineString', line_base,
                         base_types['linesobj'], True))

    out.append('\n-- ============ ПОЛИГОНАЛЬНЫЕ СЛОИ ============')
    for e in m['polygon_types']:
        lines = ['\n-- %s  <-  public.%s  (%s строк)'
                 % (e['target'], e['source'], e['rows']),
                 'CREATE TABLE IF NOT EXISTS net.%s (' % e['target'],
                 "    id       bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')",
                 '  , geom     geometry(Geometry, 9998) NOT NULL',
                 '  , src_id   int']
        for c in e['columns']:
            if c in ('id', 'shape'):
                continue
            lines.append('  , ' + col_ddl(c, e['column_types'][c]).lstrip())
        lines.append(');')
        lines.append('CREATE INDEX IF NOT EXISTS %s_geom_idx ON net.%s '
                     'USING gist (geom);' % (e['target'], e['target']))
        out.append('\n'.join(lines))

    os.makedirs(os.path.dirname(args.out), exist_ok=True)
    with open(args.out, 'w', encoding='utf-8') as f:
        f.write('\n'.join(out) + '\n')
    print('-> %s' % args.out)


if __name__ == '__main__':
    main()
