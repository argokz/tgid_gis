"""Генерация DDL целевой схемы net из converter/mapping.json -> sql/010_net_schema.sql."""
import argparse
import json
import os

HEADER = """-- СГЕНЕРИРОВАНО tools/gen_ddl.py — править руками не нужно.
-- Целевая схема ТГИД: одна таблица — один тип объекта.
--
-- id всех объектов выдаёт общая последовательность net.obj_id_seq, поэтому он
-- уникален глобально. Реестры net.node_reg / net.line_reg нужны ради настоящих
-- внешних ключей от линий и дочерних таблиц к узлам.

CREATE SCHEMA IF NOT EXISTS net;
CREATE SEQUENCE IF NOT EXISTS net.obj_id_seq AS bigint START 1;

CREATE TABLE IF NOT EXISTS net.fragment (
    id          int PRIMARY KEY,
    name        text NOT NULL,
    name_sys    text,
    nasel_point text,
    sezon       text,
    "year"      text,
    removed_at  timestamptz
);

CREATE TABLE IF NOT EXISTS net.node_reg (
    id bigint PRIMARY KEY, kind text NOT NULL, tbl text NOT NULL);
CREATE INDEX IF NOT EXISTS node_reg_kind_idx ON net.node_reg (kind);

CREATE TABLE IF NOT EXISTS net.line_reg (
    id bigint PRIMARY KEY, kind text NOT NULL, tbl text NOT NULL);
CREATE INDEX IF NOT EXISTS line_reg_kind_idx ON net.line_reg (kind);

-- Линии, концы которых не разрешаются в узлы: геометрию построить нельзя.
CREATE TABLE IF NOT EXISTS net.line_orphan (
    id        bigint PRIMARY KEY,
    src_table text NOT NULL,
    nodeid1   int,
    nodeid2   int,
    reason    text NOT NULL,
    payload   jsonb NOT NULL
);

-- Объекты, отвергнутые конвертером, и причины — для разбора после миграции.
CREATE TABLE IF NOT EXISTS net.conversion_reject (
    id        bigserial PRIMARY KEY,
    src_table text NOT NULL,
    src_id    bigint,
    reason    text NOT NULL,
    detail    jsonb
);

-- Триггеры реестра устроены так, чтобы объект можно было ПЕРЕНЕСТИ из одной
-- объектной таблицы в другую, не разрывая внешние ключи от линий.
--
-- INSERT обновляет запись реестра (ON CONFLICT), а DELETE удаляет её только
-- если реестр всё ещё указывает на эту таблицу. Тогда перенос «вставить
-- в новую таблицу, удалить из старой» оставляет строку реестра на месте,
-- и ссылки net.line.node_from не ломаются.
CREATE OR REPLACE FUNCTION net.reg_node_sync() RETURNS trigger
LANGUAGE plpgsql AS $$
BEGIN
    IF TG_OP = 'INSERT' THEN
        INSERT INTO net.node_reg (id, kind, tbl)
        VALUES (NEW.id, TG_ARGV[0], TG_TABLE_NAME)
        ON CONFLICT (id) DO UPDATE
            SET kind = EXCLUDED.kind, tbl = EXCLUDED.tbl;
        RETURN NEW;
    END IF;
    DELETE FROM net.node_reg
    WHERE id = OLD.id AND tbl = TG_TABLE_NAME;
    RETURN OLD;
END $$;

CREATE OR REPLACE FUNCTION net.reg_line_sync() RETURNS trigger
LANGUAGE plpgsql AS $$
BEGIN
    IF TG_OP = 'INSERT' THEN
        INSERT INTO net.line_reg (id, kind, tbl)
        VALUES (NEW.id, TG_ARGV[0], TG_TABLE_NAME)
        ON CONFLICT (id) DO UPDATE
            SET kind = EXCLUDED.kind, tbl = EXCLUDED.tbl;
        RETURN NEW;
    END IF;
    DELETE FROM net.line_reg
    WHERE id = OLD.id AND tbl = TG_TABLE_NAME;
    RETURN OLD;
END $$;

-- Перенос объекта в другой класс: общие колонки копируются, остальные
-- получают значения по умолчанию. Порядок важен — сначала вставка
-- (она обновит реестр), потом удаление (реестр уже указывает на новую
-- таблицу, поэтому строка реестра уцелеет).
CREATE OR REPLACE FUNCTION net.reclass_node(p_id bigint, p_target text)
RETURNS void
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $$
DECLARE
    src  text;
    cols text;
BEGIN
    SELECT tbl INTO src FROM net.node_reg WHERE id = p_id;
    IF src IS NULL THEN
        RAISE EXCEPTION 'узел % не найден в реестре', p_id;
    END IF;
    IF src = p_target THEN
        RETURN;
    END IF;

    SELECT string_agg(quote_ident(a.column_name), ', ')
    INTO cols
    FROM information_schema.columns a
    JOIN information_schema.columns b
      ON b.table_schema = 'net' AND b.table_name = p_target
     AND b.column_name = a.column_name
    WHERE a.table_schema = 'net' AND a.table_name = src
      AND a.is_generated = 'NEVER' AND b.is_generated = 'NEVER';

    EXECUTE format('INSERT INTO net.%I (%s) SELECT %s FROM net.%I WHERE id = $1',
                   p_target, cols, cols, src) USING p_id;
    EXECUTE format('DELETE FROM net.%I WHERE id = $1', src) USING p_id;
END $$;
"""

TYPE_MAP = {
    'integer': 'int', 'bigint': 'bigint', 'smallint': 'smallint',
    'double precision': 'double precision', 'real': 'real', 'numeric': 'numeric',
    'boolean': 'boolean', 'date': 'date', 'text': 'text',
    'character varying': 'text', 'character': 'text',
    'timestamp without time zone': 'timestamp',
    'timestamp with time zone': 'timestamptz', 'time without time zone': 'time',
    'uuid': 'uuid', 'jsonb': 'jsonb', 'json': 'jsonb', 'bytea': 'bytea',
    'ARRAY': 'text[]', 'USER-DEFINED': 'text',
}

RESERVED = {'year', 'name', 'end', 'order', 'group', 'user', 'default', 'check',
            'column', 'table', 'all', 'and', 'or', 'not', 'null', 'offset',
            'limit', 'references', 'primary', 'foreign', 'constraint', 'from',
            'select', 'where', 'desc', 'asc', 'union', 'do', 'to', 'in', 'is'}


def ident(n):
    if n in RESERVED or not n.replace('_', '').isalnum() or n[0].isdigit():
        return '"%s"' % n
    return n


def coltype(meta):
    return TYPE_MAP.get(meta['data_type'], 'text')


def idx(target, name, expr, extra=''):
    return ('CREATE INDEX IF NOT EXISTS %s_%s_idx ON net.%s %s%s;'
            % (target, name, target, expr, extra))


def reg_trigger(target, reg):
    return ('DROP TRIGGER IF EXISTS %s_reg ON net.%s;\n'
            'CREATE TRIGGER %s_reg AFTER INSERT OR DELETE ON net.%s\n'
            "    FOR EACH ROW EXECUTE FUNCTION net.reg_%s_sync('%s');"
            % (target, target, target, target, reg, target))


CONCURRENCY_COLUMNS = [
    '  , row_version  bigint NOT NULL DEFAULT 1',
    '  , updated_at   timestamptz NOT NULL DEFAULT clock_timestamp()',
    '  , updated_by   text NOT NULL DEFAULT current_user',
]


def emit_class(e, base_cols, base_types, is_line):
    t = e['target']
    L = ['\n-- %s  <-  public.%s  (%s строк, %s)'
         % (t, e['source'], e['rows'], e['category']),
         'CREATE TABLE IF NOT EXISTS net.%s (' % t,
         "    id           bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')",
         '  , fragment_id  int REFERENCES net.fragment(id)']
    if is_line:
        L += ['  , node_from    bigint NOT NULL REFERENCES net.node_reg(id) '
              'ON DELETE RESTRICT',
              '  , node_to      bigint NOT NULL REFERENCES net.node_reg(id) '
              'ON DELETE RESTRICT',
              # Прежние public.id концов. Нужны слою совместимости, чтобы
              # не джойнить карту соответствия на каждом чтении.
              '  , node_from_src int',
              '  , node_to_src   int',
              # Собственный fileid линии. Заполнен лишь у 22 из 80 тыс.
              # строк и ненадёжен — приложение фильтрует по фрагменту
              # начального узла. Но слой совместимости обязан отдавать
              # ровно то, что было, поэтому исходное значение сохраняем.
              '  , fileid_src    int']
    L += ['  , geom         geometry(%s, 9998) NOT NULL' % e['geometry'],
          '  , removed_at   timestamptz',
          # NULL у объектов, созданных уже после миграции.
          # Уникальный индекс это допускает: в PostgreSQL NULL не конфликтуют.
          '  , src_id       int']
    L += CONCURRENCY_COLUMNS

    seen = set()
    for c in base_cols:
        if c in ('id', 'fileid', 'nodeid1', 'nodeid2') or c in seen:
            continue
        seen.add(c)
        L.append('  , %-32s %s' % (ident(c), coltype(base_types[c])))
    for c in e['columns']:
        if c in seen or c in ('id', 'fileid'):
            continue
        seen.add(c)
        L.append('  , %-32s %s' % (ident(c), coltype(e['column_types'][c])))
    if is_line:
        # Текстовый coords нужен приложению, но вычислять его на каждом
        # чтении дорого — замер показал 330 мс из 380 на 6.8 тыс. строк.
        # GENERATED STORED считает его при записи. В отличие от прежнего
        # public.linesobj.coords эта копия не может разойтись с геометрией:
        # PostgreSQL пересчитывает её при любом изменении geom.
        L.append('  , coords_legacy text GENERATED ALWAYS AS '
                 '(net.geom_to_coords(geom, 100.0)) STORED')
    L.append(');')

    L += [idx(t, 'geom', 'USING gist (geom)'),
          idx(t, 'frag', '(fragment_id)', ' WHERE removed_at IS NULL'),
          'CREATE UNIQUE INDEX IF NOT EXISTS %s_src_uidx ON net.%s (src_id);'
          % (t, t)]
    if is_line:
        L += [idx(t, 'from', '(node_from)'), idx(t, 'to', '(node_to)')]
    L.append(reg_trigger(t, 'line' if is_line else 'node'))
    return '\n'.join(L)


def emit_layer(e):
    t = e['target']
    geometry_type = {
        'GEOMETRY': 'Geometry',
        'POINT': 'Point',
        'LINESTRING': 'LineString',
        'POLYGON': 'Polygon',
        'MULTIPOINT': 'MultiPoint',
        'MULTILINESTRING': 'MultiLineString',
        'MULTIPOLYGON': 'MultiPolygon',
    }.get((e.get('geometry') or 'Geometry').upper(),
          e.get('geometry') or 'Geometry')
    L = ['\n-- %s  <-  public.%s  (%s строк, самостоятельный слой)'
         % (t, e['source'], e['rows']),
         'CREATE TABLE IF NOT EXISTS net.%s (' % t,
         "    id      bigint PRIMARY KEY DEFAULT nextval('net.obj_id_seq')",
         '  , geom    geometry(%s, 9998) NOT NULL' % geometry_type,
         '  , src_id  int']
    L += CONCURRENCY_COLUMNS
    seen = set()
    for c in e['columns']:
        if c in seen:
            continue
        seen.add(c)
        L.append('  , %-32s %s' % (ident(c), coltype(e['column_types'][c])))
    L += [');', idx(t, 'geom', 'USING gist (geom)'),
          'CREATE UNIQUE INDEX IF NOT EXISTS %s_src_uidx ON net.%s (src_id);'
          % (t, t)]
    return '\n'.join(L)


def emit_child(e):
    t = e['target']
    L = ['\n-- %s  <-  public.%s  (%s строк, %s к узлу)'
         % (t, e['source'], e['rows'], e['category']),
         'CREATE TABLE IF NOT EXISTS net.%s (' % t,
         '    id      bigserial PRIMARY KEY',
         '  , node_id bigint NOT NULL REFERENCES net.node_reg(id) ON DELETE CASCADE',
         '  , src_id  int']
    seen = set()
    for c in e['columns']:
        if c in seen:
            continue
        seen.add(c)
        L.append('  , %-32s %s' % (ident(c), coltype(e['column_types'][c])))
    L += [');', idx(t, 'node', '(node_id)')]
    if e['category'] == 'aspect':
        L.append('CREATE UNIQUE INDEX IF NOT EXISTS %s_node_uidx '
                 'ON net.%s (node_id);' % (t, t))
    return '\n'.join(L)


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
            'data_type': c['data_type']}

    out = [HEADER]
    nb = m['base']['nodes']['columns']
    lb = m['base']['linesobj']['columns']

    out.append('\n-- ============ ТОЧЕЧНЫЕ КЛАССЫ ============')
    for e in m['class_node']:
        out.append(emit_class(e, nb, base_types['nodes'], False))
    out.append(emit_class(
        {'target': 'node_plain', 'source': 'nodes', 'rows': 'остаток',
         'category': 'class_node', 'geometry': 'Point',
         'columns': [], 'column_types': {}}, nb, base_types['nodes'], False))

    out.append('\n-- ============ ЛИНЕЙНЫЕ КЛАССЫ ============')
    for e in m['class_line']:
        out.append(emit_class(e, lb, base_types['linesobj'], True))
    out.append(emit_class(
        {'target': 'line_plain', 'source': 'linesobj', 'rows': 'остаток',
         'category': 'class_line', 'geometry': 'LineString',
         'columns': [], 'column_types': {}}, lb, base_types['linesobj'], True))

    out.append('\n-- ============ САМОСТОЯТЕЛЬНЫЕ СЛОИ ============')
    for e in m['layer']:
        out.append(emit_layer(e))

    out.append('\n-- ============ АСПЕКТЫ И ДОЧЕРНИЕ ТАБЛИЦЫ ============')
    for e in m['aspect'] + m['child']:
        out.append(emit_child(e))

    os.makedirs(os.path.dirname(args.out), exist_ok=True)
    with open(args.out, 'w', encoding='utf-8') as f:
        f.write('\n'.join(out) + '\n')
    print('-> %s' % args.out)


if __name__ == '__main__':
    main()
