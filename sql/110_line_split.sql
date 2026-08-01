-- Атомарное разрезание линейного объекта в объектной модели net.

\set ON_ERROR_STOP on

BEGIN;

ALTER TABLE meta.layer_catalog
    ADD COLUMN IF NOT EXISTS can_split boolean NOT NULL DEFAULT false;

UPDATE meta.layer_catalog
   SET can_split = table_name IN ('pipe_section', 'line_plain')
 WHERE schema_name = 'net';

COMMENT ON COLUMN meta.layer_catalog.can_split IS
    'Разрешено атомарное разрезание линии через net.split_line';

DROP FUNCTION IF EXISTS net.split_line(
    text, bigint, bigint, double precision, double precision
);

CREATE OR REPLACE FUNCTION net.split_line(
    p_table text,
    p_line_id bigint,
    p_expected_version bigint,
    p_x double precision,
    p_y double precision
)
RETURNS TABLE (
    node_id bigint,
    node_created boolean,
    first_line_id bigint,
    second_line_id bigint,
    split_fraction double precision
)
LANGUAGE plpgsql
SET search_path = pg_catalog, public, net
AS $$
DECLARE
    registered_table text;
    layer_geometry_type text;
    layer_editable boolean;
    layer_can_split boolean;
    line_fragment integer;
    line_from bigint;
    line_to bigint;
    line_geom geometry;
    line_version bigint;
    line_removed_at timestamptz;
    input_point geometry;
    split_point geometry;
    first_geom geometry;
    second_geom geometry;
    copy_columns text;
    first_values text;
    second_values text;
    affected bigint;
BEGIN
    IF p_table IS NULL OR p_table !~ '^[a-z_][a-z0-9_]*$' THEN
        RAISE EXCEPTION 'Недопустимое имя таблицы линии';
    END IF;
    IF p_line_id <= 0 OR p_expected_version <= 0
       OR p_x::text IN ('Infinity', '-Infinity', 'NaN')
       OR p_y::text IN ('Infinity', '-Infinity', 'NaN') THEN
        RAISE EXCEPTION 'Некорректные параметры разрезания линии';
    END IF;

    SELECT tbl
      INTO registered_table
      FROM net.line_reg
     WHERE id = p_line_id;
    IF registered_table IS NULL THEN
        RAISE EXCEPTION 'Линия % отсутствует в net.line_reg', p_line_id;
    END IF;
    IF registered_table IS DISTINCT FROM p_table THEN
        RAISE EXCEPTION
            'Линия % относится к net.%, а не к net.%',
            p_line_id, registered_table, p_table;
    END IF;

    SELECT upper(geometry_type), is_editable, can_split
      INTO layer_geometry_type, layer_editable, layer_can_split
      FROM meta.layer_catalog
     WHERE schema_name = 'net'
       AND table_name = p_table;
    IF layer_geometry_type IS DISTINCT FROM 'LINESTRING'
       OR NOT coalesce(layer_editable, false)
       OR NOT coalesce(layer_can_split, false) THEN
        RAISE EXCEPTION 'Слой net.% нельзя разрезать', p_table;
    END IF;

    EXECUTE format(
        'SELECT fragment_id, node_from, node_to, geom, row_version, removed_at
           FROM net.%I
          WHERE id = $1
          FOR UPDATE',
        p_table
    )
    INTO line_fragment, line_from, line_to, line_geom,
         line_version, line_removed_at
    USING p_line_id;

    IF line_geom IS NULL THEN
        RAISE EXCEPTION 'Линия % не найдена или не имеет геометрии', p_line_id;
    END IF;
    IF line_removed_at IS NOT NULL THEN
        RAISE EXCEPTION 'Архивную линию % нельзя разрезать', p_line_id;
    END IF;
    IF line_version IS DISTINCT FROM p_expected_version THEN
        RAISE EXCEPTION
            'CONFLICT: версия линии % изменилась: ожидалась %, текущая %',
            p_line_id, p_expected_version, line_version;
    END IF;
    IF ST_GeometryType(line_geom) <> 'ST_LineString'
       OR ST_NPoints(line_geom) < 2
       OR ST_IsEmpty(line_geom) THEN
        RAISE EXCEPTION 'Некорректная геометрия линии %', p_line_id;
    END IF;

    input_point := ST_SetSRID(ST_MakePoint(p_x, p_y), ST_SRID(line_geom));
    split_fraction := ST_LineLocatePoint(line_geom, input_point);
    IF split_fraction <= 0.000001 OR split_fraction >= 0.999999 THEN
        RAISE EXCEPTION 'Точка разрезания слишком близка к концу линии';
    END IF;

    split_point := ST_LineInterpolatePoint(line_geom, split_fraction);
    first_geom := ST_LineSubstring(line_geom, 0.0, split_fraction);
    second_geom := ST_LineSubstring(line_geom, split_fraction, 1.0);
    IF ST_NPoints(first_geom) < 2 OR ST_NPoints(second_geom) < 2
       OR ST_Length(first_geom) <= 0 OR ST_Length(second_geom) <= 0 THEN
        RAISE EXCEPTION 'Разрезание создаёт вырожденную геометрию';
    END IF;

    SELECT id2
      INTO node_id
      FROM net.v_map_nodes
     WHERE fileid = line_fragment
       AND ST_DWithin(geom, split_point, 0.001)
     ORDER BY ST_Distance(geom, split_point), id2
     LIMIT 1;
    node_created := node_id IS NULL;
    IF node_created THEN
        INSERT INTO net.connect_node (fragment_id, geom)
        VALUES (line_fragment, split_point)
        RETURNING id INTO node_id;
    END IF;

    SELECT
        string_agg(format('%I', a.attname), ', ' ORDER BY a.attnum),
        string_agg(
            CASE
                WHEN a.attname = 'pipesectlength' THEN
                    format(
                        'CASE WHEN %1$I IS NULL THEN NULL ELSE %1$I * $5 END',
                        a.attname)
                ELSE format('%I', a.attname)
            END,
            ', ' ORDER BY a.attnum),
        string_agg(
            CASE
                WHEN a.attname = 'pipesectlength' THEN
                    format(
                        'CASE WHEN %1$I IS NULL THEN NULL ELSE %1$I * (1.0 - $5) END',
                        a.attname)
                ELSE format('%I', a.attname)
            END,
            ', ' ORDER BY a.attnum)
      INTO copy_columns, first_values, second_values
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
           'row_version', 'updated_at', 'updated_by'
       );

    IF copy_columns IS NULL THEN
        RAISE EXCEPTION 'Не найдены копируемые поля net.%', p_table;
    END IF;

    EXECUTE format(
        'INSERT INTO net.%1$I (%2$s, node_from, node_to, geom)
         SELECT %3$s, $2, $3, $4
           FROM net.%1$I
          WHERE id = $1
         RETURNING id',
        p_table, copy_columns, first_values)
    INTO first_line_id
    USING p_line_id, line_from, node_id, first_geom, split_fraction;

    EXECUTE format(
        'INSERT INTO net.%1$I (%2$s, node_from, node_to, geom)
         SELECT %3$s, $2, $3, $4
           FROM net.%1$I
          WHERE id = $1
         RETURNING id',
        p_table, copy_columns, second_values)
    INTO second_line_id
    USING p_line_id, node_id, line_to, second_geom, split_fraction;

    EXECUTE format(
        'UPDATE net.%I
            SET removed_at = clock_timestamp()
          WHERE id = $1
            AND row_version = $2
            AND removed_at IS NULL',
        p_table)
    USING p_line_id, p_expected_version;
    GET DIAGNOSTICS affected = ROW_COUNT;
    IF affected <> 1 THEN
        RAISE EXCEPTION 'CONFLICT: линия была изменена во время разрезания';
    END IF;

    RETURN NEXT;
END
$$;

COMMENT ON FUNCTION net.split_line(
    text, bigint, bigint, double precision, double precision
) IS
    'Создаёт соединительный узел и две линии, архивируя исходную строку';

INSERT INTO meta.schema_version (version, description)
VALUES (9, 'Атомарное разрезание линейных объектов')
ON CONFLICT (version) DO NOTHING;

COMMIT;
