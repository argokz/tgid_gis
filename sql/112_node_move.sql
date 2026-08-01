-- Атомарное перемещение узла с перестроением подключённых линий.

\set ON_ERROR_STOP on

BEGIN;

CREATE OR REPLACE FUNCTION net.annotate_geometry_change()
RETURNS trigger
LANGUAGE plpgsql
AS $$
BEGIN
    IF OLD.geom IS NOT DISTINCT FROM NEW.geom THEN
        RETURN NULL;
    END IF;
    UPDATE meta.object_change_log
       SET changed_fields = changed_fields || jsonb_build_object(
               'geom', jsonb_build_object(
                   'old', CASE WHEN OLD.geom IS NULL THEN NULL
                               ELSE ST_AsText(ST_Envelope(OLD.geom), 3) END,
                   'new', CASE WHEN NEW.geom IS NULL THEN NULL
                               ELSE ST_AsText(ST_Envelope(NEW.geom), 3) END,
                   'srid', coalesce(ST_SRID(NEW.geom), ST_SRID(OLD.geom))))
     WHERE schema_name = TG_TABLE_SCHEMA
       AND table_name = TG_TABLE_NAME
       AND object_id = NEW.id
       AND row_version = NEW.row_version
       AND operation = 'update';
    RETURN NULL;
END
$$;

DO $$
DECLARE
    r record;
BEGIN
    FOR r IN
        SELECT c.relname AS table_name
          FROM pg_class c
          JOIN pg_namespace n ON n.oid = c.relnamespace
         WHERE n.nspname = 'net'
           AND c.relkind IN ('r', 'p')
           AND EXISTS (
               SELECT 1 FROM pg_attribute a
                WHERE a.attrelid = c.oid
                  AND a.attname = 'geom'
                  AND NOT a.attisdropped)
           AND EXISTS (
               SELECT 1 FROM pg_attribute a
                WHERE a.attrelid = c.oid
                  AND a.attname = 'row_version'
                  AND NOT a.attisdropped)
    LOOP
        EXECUTE format(
            'DROP TRIGGER IF EXISTS geometry_change_audit ON net.%I',
            r.table_name);
        EXECUTE format(
            'CREATE TRIGGER geometry_change_audit
             AFTER UPDATE OF geom ON net.%I
             FOR EACH ROW EXECUTE FUNCTION net.annotate_geometry_change()',
            r.table_name);
    END LOOP;
END
$$;

DROP FUNCTION IF EXISTS net.move_node(
    text, bigint, bigint, double precision, double precision
);

CREATE FUNCTION net.move_node(
    p_table text,
    p_node_id bigint,
    p_expected_version bigint,
    p_x double precision,
    p_y double precision
)
RETURNS TABLE (
    new_version bigint,
    connected_lines integer
)
LANGUAGE plpgsql
SET search_path = pg_catalog, public, net
AS $$
DECLARE
    registered_table text;
    layer_geometry_type text;
    layer_srid integer;
    layer_editable boolean;
    node_geom geometry;
    node_version bigint;
    node_removed_at timestamptz;
    moved_geom geometry;
    table_lines bigint;
    r record;
BEGIN
    IF p_table IS NULL OR p_table !~ '^[a-z_][a-z0-9_]*$' THEN
        RAISE EXCEPTION 'Недопустимое имя таблицы узла';
    END IF;
    IF p_node_id <= 0 OR p_expected_version <= 0
       OR p_x::text IN ('Infinity', '-Infinity', 'NaN')
       OR p_y::text IN ('Infinity', '-Infinity', 'NaN') THEN
        RAISE EXCEPTION 'Некорректные параметры перемещения узла';
    END IF;

    SELECT tbl
      INTO registered_table
      FROM net.node_reg
     WHERE id = p_node_id;
    IF registered_table IS NULL THEN
        RAISE EXCEPTION 'Узел % отсутствует в net.node_reg', p_node_id;
    END IF;
    IF registered_table IS DISTINCT FROM p_table THEN
        RAISE EXCEPTION
            'Узел % относится к net.%, а не к net.%',
            p_node_id, registered_table, p_table;
    END IF;

    SELECT upper(geometry_type), srid, is_editable
      INTO layer_geometry_type, layer_srid, layer_editable
      FROM meta.layer_catalog
     WHERE schema_name = 'net'
       AND table_name = p_table;
    IF layer_geometry_type IS DISTINCT FROM 'POINT'
       OR layer_srid <= 0
       OR NOT coalesce(layer_editable, false) THEN
        RAISE EXCEPTION 'Слой net.% нельзя перемещать', p_table;
    END IF;

    EXECUTE format(
        'SELECT geom, row_version, removed_at
           FROM net.%I
          WHERE id = $1
          FOR UPDATE',
        p_table)
    INTO node_geom, node_version, node_removed_at
    USING p_node_id;

    IF node_geom IS NULL THEN
        RAISE EXCEPTION 'Узел % не найден или не имеет геометрии', p_node_id;
    END IF;
    IF node_removed_at IS NOT NULL THEN
        RAISE EXCEPTION 'Архивный узел % нельзя перемещать', p_node_id;
    END IF;
    IF node_version IS DISTINCT FROM p_expected_version THEN
        RAISE EXCEPTION
            'CONFLICT: версия узла % изменилась: ожидалась %, текущая %',
            p_node_id, p_expected_version, node_version;
    END IF;

    moved_geom := ST_SetSRID(ST_MakePoint(p_x, p_y), layer_srid);
    IF ST_Equals(node_geom, moved_geom) THEN
        RAISE EXCEPTION 'Новое положение узла совпадает с текущим';
    END IF;

    connected_lines := 0;
    FOR r IN
        SELECT c.relname AS table_name
          FROM pg_class c
          JOIN pg_namespace n ON n.oid = c.relnamespace
         WHERE n.nspname = 'net'
           AND c.relkind IN ('r', 'p')
           AND EXISTS (
               SELECT 1 FROM pg_attribute a
                WHERE a.attrelid = c.oid
                  AND a.attname = 'node_from'
                  AND NOT a.attisdropped)
           AND EXISTS (
               SELECT 1 FROM pg_attribute a
                WHERE a.attrelid = c.oid
                  AND a.attname = 'node_to'
                  AND NOT a.attisdropped)
    LOOP
        EXECUTE format(
            'SELECT count(*) FROM net.%I
              WHERE (node_from = $1 OR node_to = $1)
                AND removed_at IS NULL',
            r.table_name)
        INTO table_lines
        USING p_node_id;
        connected_lines := connected_lines + table_lines::integer;
    END LOOP;

    EXECUTE format(
        'UPDATE net.%I
            SET geom = $1
          WHERE id = $2
            AND row_version = $3
            AND removed_at IS NULL
        RETURNING row_version',
        p_table)
    INTO new_version
    USING moved_geom, p_node_id, p_expected_version;

    IF new_version IS NULL THEN
        RAISE EXCEPTION 'CONFLICT: узел был изменён во время перемещения';
    END IF;
    RETURN NEXT;
END
$$;

COMMENT ON FUNCTION net.move_node(
    text, bigint, bigint, double precision, double precision
) IS
    'Перемещает активный узел с контролем версии; триггеры обновляют линии';

INSERT INTO meta.schema_version (version, description)
VALUES (11, 'Атомарное перемещение узлов с перестроением линий')
ON CONFLICT (version) DO NOTHING;

COMMIT;
