-- Атомарное соединение двух последовательных трубопроводных участков.

\set ON_ERROR_STOP on

BEGIN;

ALTER TABLE meta.layer_catalog
    ADD COLUMN IF NOT EXISTS can_join boolean NOT NULL DEFAULT false;

UPDATE meta.layer_catalog
   SET can_join = table_name IN ('pipe_section', 'line_plain')
 WHERE schema_name = 'net';

COMMENT ON COLUMN meta.layer_catalog.can_join IS
    'Разрешено атомарное соединение участков через net.join_lines';

DROP FUNCTION IF EXISTS net.join_lines(
    text, bigint, bigint, bigint, bigint
);

CREATE FUNCTION net.join_lines(
    p_table text,
    p_first_id bigint,
    p_first_version bigint,
    p_second_id bigint,
    p_second_version bigint
)
RETURNS TABLE (
    joined_line_id bigint,
    archived_node_id bigint
)
LANGUAGE plpgsql
SET search_path = pg_catalog, public, net
AS $$
DECLARE
    line_a_id bigint;
    line_b_id bigint;
    version_a_expected bigint;
    version_b_expected bigint;
    registered_a text;
    registered_b text;
    layer_geometry_type text;
    layer_editable boolean;
    layer_can_join boolean;
    fragment_a integer;
    fragment_b integer;
    from_a bigint;
    to_a bigint;
    from_b bigint;
    to_b bigint;
    geom_a geometry;
    geom_b geometry;
    version_a bigint;
    version_b bigint;
    removed_a timestamptz;
    removed_b timestamptz;
    shared_node bigint;
    shared_table text;
    shared_removed timestamptz;
    outer_a bigint;
    outer_b bigint;
    oriented_a geometry;
    oriented_b geometry;
    joined_geom geometry;
    connected_count bigint := 0;
    table_count bigint;
    attributes_equal boolean;
    copy_columns text;
    copy_values text;
    has_length boolean;
    length_a double precision;
    length_b double precision;
    joined_length double precision;
    affected bigint;
    r record;
BEGIN
    IF p_table IS NULL OR p_table !~ '^[a-z_][a-z0-9_]*$' THEN
        RAISE EXCEPTION 'Недопустимое имя таблицы линии';
    END IF;
    IF p_first_id <= 0 OR p_second_id <= 0
       OR p_first_id = p_second_id
       OR p_first_version <= 0 OR p_second_version <= 0 THEN
        RAISE EXCEPTION 'Некорректные параметры соединения линий';
    END IF;

    IF p_first_id < p_second_id THEN
        line_a_id := p_first_id;
        line_b_id := p_second_id;
        version_a_expected := p_first_version;
        version_b_expected := p_second_version;
    ELSE
        line_a_id := p_second_id;
        line_b_id := p_first_id;
        version_a_expected := p_second_version;
        version_b_expected := p_first_version;
    END IF;

    SELECT tbl INTO registered_a FROM net.line_reg WHERE id = line_a_id;
    SELECT tbl INTO registered_b FROM net.line_reg WHERE id = line_b_id;
    IF registered_a IS DISTINCT FROM p_table
       OR registered_b IS DISTINCT FROM p_table THEN
        RAISE EXCEPTION
            'Обе линии должны существовать в net.%', p_table;
    END IF;

    SELECT upper(geometry_type), is_editable, can_join
      INTO layer_geometry_type, layer_editable, layer_can_join
      FROM meta.layer_catalog
     WHERE schema_name = 'net'
       AND table_name = p_table;
    IF layer_geometry_type IS DISTINCT FROM 'LINESTRING'
       OR NOT coalesce(layer_editable, false)
       OR NOT coalesce(layer_can_join, false) THEN
        RAISE EXCEPTION 'Слой net.% нельзя соединять', p_table;
    END IF;

    EXECUTE format(
        'SELECT fragment_id, node_from, node_to, geom, row_version, removed_at
           FROM net.%I WHERE id = $1 FOR UPDATE', p_table)
    INTO fragment_a, from_a, to_a, geom_a, version_a, removed_a
    USING line_a_id;
    EXECUTE format(
        'SELECT fragment_id, node_from, node_to, geom, row_version, removed_at
           FROM net.%I WHERE id = $1 FOR UPDATE', p_table)
    INTO fragment_b, from_b, to_b, geom_b, version_b, removed_b
    USING line_b_id;

    IF geom_a IS NULL OR geom_b IS NULL THEN
        RAISE EXCEPTION 'Одна из линий не найдена или не имеет геометрии';
    END IF;
    IF removed_a IS NOT NULL OR removed_b IS NOT NULL THEN
        RAISE EXCEPTION 'Архивные линии соединять нельзя';
    END IF;
    IF version_a IS DISTINCT FROM version_a_expected
       OR version_b IS DISTINCT FROM version_b_expected THEN
        RAISE EXCEPTION
            'CONFLICT: версии линий изменились; ожидались % и %, текущие % и %',
            version_a_expected, version_b_expected, version_a, version_b;
    END IF;
    IF fragment_a IS DISTINCT FROM fragment_b THEN
        RAISE EXCEPTION 'Линии относятся к разным фрагментам';
    END IF;

    IF from_a = from_b OR from_a = to_b THEN
        shared_node := from_a;
    ELSIF to_a = from_b OR to_a = to_b THEN
        shared_node := to_a;
    ELSE
        RAISE EXCEPTION 'Линии не имеют общего узла';
    END IF;
    IF ((from_a = from_b OR from_a = to_b)::integer
        + (to_a = from_b OR to_a = to_b)::integer) <> 1 THEN
        RAISE EXCEPTION 'Линии должны иметь ровно один общий узел';
    END IF;

    outer_a := CASE WHEN from_a = shared_node THEN to_a ELSE from_a END;
    outer_b := CASE WHEN from_b = shared_node THEN to_b ELSE from_b END;
    IF outer_a = outer_b THEN
        RAISE EXCEPTION 'Соединение создаёт замкнутую линию';
    END IF;

    SELECT tbl INTO shared_table
      FROM net.node_reg WHERE id = shared_node;
    IF shared_table IS DISTINCT FROM 'connect_node' THEN
        RAISE EXCEPTION
            'Общий узел % должен иметь класс connect_node', shared_node;
    END IF;
    SELECT removed_at INTO shared_removed
      FROM net.connect_node
     WHERE id = shared_node
     FOR UPDATE;
    IF shared_removed IS NOT NULL THEN
        RAISE EXCEPTION 'Общий узел уже находится в архиве';
    END IF;

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
                AND removed_at IS NULL', r.table_name)
        INTO table_count
        USING shared_node;
        connected_count := connected_count + table_count;
    END LOOP;
    IF connected_count <> 2 THEN
        RAISE EXCEPTION
            'Общий узел % связан с % активными линиями вместо двух',
            shared_node, connected_count;
    END IF;

    EXECUTE format(
        'SELECT (to_jsonb(a) - ARRAY[
                    ''id'', ''geom'', ''node_from'', ''node_to'',
                    ''node_from_src'', ''node_to_src'', ''fileid_src'',
                    ''src_id'', ''subtype_src_id'', ''removed_at'',
                    ''row_version'', ''updated_at'', ''updated_by'',
                    ''coords_legacy'', ''pipesectlength''
                ]::text[])
                =
                (to_jsonb(b) - ARRAY[
                    ''id'', ''geom'', ''node_from'', ''node_to'',
                    ''node_from_src'', ''node_to_src'', ''fileid_src'',
                    ''src_id'', ''subtype_src_id'', ''removed_at'',
                    ''row_version'', ''updated_at'', ''updated_by'',
                    ''coords_legacy'', ''pipesectlength''
                ]::text[])
           FROM net.%1$I a, net.%1$I b
          WHERE a.id = $1 AND b.id = $2', p_table)
    INTO attributes_equal
    USING line_a_id, line_b_id;
    IF NOT coalesce(attributes_equal, false) THEN
        RAISE EXCEPTION
            'Бизнес-атрибуты участков различаются; автоматическое соединение запрещено';
    END IF;

    oriented_a := CASE
        WHEN from_a = outer_a THEN geom_a ELSE ST_Reverse(geom_a)
    END;
    oriented_b := CASE
        WHEN from_b = shared_node THEN geom_b ELSE ST_Reverse(geom_b)
    END;
    joined_geom := ST_MakeLine(oriented_a, oriented_b);
    IF ST_GeometryType(joined_geom) <> 'ST_LineString'
       OR ST_NPoints(joined_geom) < 2
       OR ST_Length(joined_geom) <= 0 THEN
        RAISE EXCEPTION 'Не удалось построить объединённую геометрию';
    END IF;

    SELECT EXISTS (
        SELECT 1
          FROM pg_class c
          JOIN pg_namespace n ON n.oid = c.relnamespace
          JOIN pg_attribute a ON a.attrelid = c.oid
         WHERE n.nspname = 'net'
           AND c.relname = p_table
           AND a.attname = 'pipesectlength'
           AND NOT a.attisdropped
    ) INTO has_length;
    IF has_length THEN
        EXECUTE format(
            'SELECT pipesectlength::double precision
               FROM net.%I WHERE id = $1', p_table)
        INTO length_a USING line_a_id;
        EXECUTE format(
            'SELECT pipesectlength::double precision
               FROM net.%I WHERE id = $1', p_table)
        INTO length_b USING line_b_id;
        joined_length := CASE
            WHEN length_a IS NULL AND length_b IS NULL THEN NULL
            ELSE coalesce(length_a, 0.0) + coalesce(length_b, 0.0)
        END;
    END IF;

    SELECT
        string_agg(format('%I', a.attname), ', ' ORDER BY a.attnum),
        string_agg(format('source.%I', a.attname), ', ' ORDER BY a.attnum)
      INTO copy_columns, copy_values
      FROM pg_class c
      JOIN pg_namespace n ON n.oid = c.relnamespace
      JOIN pg_attribute a ON a.attrelid = c.oid
     WHERE n.nspname = 'net'
       AND c.relname = p_table
       AND a.attnum > 0
       AND NOT a.attisdropped
       AND a.attgenerated = ''
       AND a.attidentity = ''
       AND a.attname NOT IN (
           'id', 'geom', 'node_from', 'node_to',
           'node_from_src', 'node_to_src', 'fileid_src',
           'src_id', 'subtype_src_id', 'removed_at',
           'row_version', 'updated_at', 'updated_by',
           'coords_legacy', 'pipesectlength'
       );
    IF copy_columns IS NULL THEN
        RAISE EXCEPTION 'Не найдены копируемые поля net.%', p_table;
    END IF;

    IF has_length THEN
        EXECUTE format(
            'INSERT INTO net.%1$I (
                 %2$s, pipesectlength, node_from, node_to, geom
             )
             SELECT %3$s, $5, $3, $4, $6
               FROM net.%1$I source
              WHERE source.id = $1
             RETURNING id', p_table, copy_columns, copy_values)
        INTO joined_line_id
        USING line_a_id, line_b_id, outer_a, outer_b,
              joined_length, joined_geom;
    ELSE
        EXECUTE format(
            'INSERT INTO net.%1$I (%2$s, node_from, node_to, geom)
             SELECT %3$s, $3, $4, $5
               FROM net.%1$I source
              WHERE source.id = $1
             RETURNING id', p_table, copy_columns, copy_values)
        INTO joined_line_id
        USING line_a_id, line_b_id, outer_a, outer_b, joined_geom;
    END IF;

    EXECUTE format(
        'UPDATE net.%I SET removed_at = clock_timestamp()
          WHERE id = $1 AND row_version = $2 AND removed_at IS NULL', p_table)
    USING line_a_id, version_a_expected;
    GET DIAGNOSTICS affected = ROW_COUNT;
    IF affected <> 1 THEN
        RAISE EXCEPTION 'CONFLICT: первая линия изменилась при соединении';
    END IF;
    EXECUTE format(
        'UPDATE net.%I SET removed_at = clock_timestamp()
          WHERE id = $1 AND row_version = $2 AND removed_at IS NULL', p_table)
    USING line_b_id, version_b_expected;
    GET DIAGNOSTICS affected = ROW_COUNT;
    IF affected <> 1 THEN
        RAISE EXCEPTION 'CONFLICT: вторая линия изменилась при соединении';
    END IF;

    UPDATE net.connect_node
       SET removed_at = clock_timestamp()
     WHERE id = shared_node
       AND removed_at IS NULL;
    GET DIAGNOSTICS affected = ROW_COUNT;
    IF affected <> 1 THEN
        RAISE EXCEPTION 'Не удалось архивировать общий узел';
    END IF;
    archived_node_id := shared_node;

    RETURN NEXT;
END
$$;

COMMENT ON FUNCTION net.join_lines(text, bigint, bigint, bigint, bigint) IS
    'Соединяет два одинаковых участка через connect_node и архивирует исходные строки';

INSERT INTO meta.schema_version (version, description)
VALUES (10, 'Атомарное соединение двух трубопроводных участков')
ON CONFLICT (version) DO NOTHING;

COMMIT;
