-- Приведение переходных GIS-слоёв к актуальной модели «один тип геометрии —
-- одна таблица». Миграция нужна для баз, созданных до разделения смешанных
-- слоёв в 010_net_schema.sql. На новой чистой сборке она идемпотентна.

\set ON_ERROR_STOP on

BEGIN;

CREATE SCHEMA IF NOT EXISTS attic;

CREATE TABLE IF NOT EXISTS meta.geometry_repair_log (
    id             bigserial PRIMARY KEY,
    table_name     text NOT NULL,
    object_id      bigint NOT NULL,
    source_id      bigint,
    reason         text NOT NULL,
    method         text NOT NULL,
    original_geom  geometry(Geometry, 9998) NOT NULL,
    repaired_geom  geometry(Geometry, 9998),
    repaired_at    timestamptz NOT NULL DEFAULT clock_timestamp(),
    UNIQUE (table_name, object_id, method)
);

INSERT INTO meta.geometry_repair_log (
    table_name, object_id, source_id, reason, method,
    original_geom, repaired_geom)
SELECT 'net.building', id, src_id, ST_IsValidReason(geom),
       'ST_Multi(ST_CollectionExtract(ST_MakeValid(geom), 3))',
       geom,
       ST_Multi(ST_CollectionExtract(ST_MakeValid(geom), 3))
FROM net.building
WHERE NOT ST_IsValid(geom)
ON CONFLICT (table_name, object_id, method) DO NOTHING;

UPDATE net.building
SET geom = ST_Multi(ST_CollectionExtract(ST_MakeValid(geom), 3))
WHERE NOT ST_IsValid(geom);

-- ALTER TYPE запрещён, пока представление/триггер зависит от geom. Удаляем их
-- только в старой generic-схеме; повторный запуск уже готовую БД не затрагивает.
DO $$
BEGIN
    IF EXISTS (
        SELECT 1 FROM geometry_columns
        WHERE f_table_schema = 'net'
          AND f_table_name = 'building'
          AND upper(type) <> 'MULTIPOLYGON')
    THEN
        DROP VIEW IF EXISTS net.building_wgs84;
        DROP TRIGGER IF EXISTS geometry_change_audit ON net.building;
        ALTER TABLE net.building
            ALTER COLUMN geom TYPE geometry(MultiPolygon, 9998)
            USING ST_Multi(
                ST_CollectionExtract(ST_MakeValid(geom), 3));
    END IF;
END
$$;

DROP VIEW IF EXISTS net.building_consumer_wgs84;
DROP VIEW IF EXISTS net.building_tu_wgs84;
DROP VIEW IF EXISTS net.pipe_reconstructed_wgs84;

CREATE OR REPLACE FUNCTION pg_temp.tgid_split_mixed_layer(
    source_name text,
    target_name text,
    source_types text[],
    target_type text,
    geometry_expression text)
RETURNS bigint
LANGUAGE plpgsql
AS $$
DECLARE
    target_rows bigint;
    expression_with_alias text;
BEGIN
    IF to_regclass(format('net.%I', source_name)) IS NULL THEN
        RETURN 0;
    END IF;

    IF to_regclass(format('net.%I', target_name)) IS NULL THEN
        EXECUTE format(
            'CREATE TABLE net.%I (
                 LIKE net.%I
                 INCLUDING DEFAULTS
                 INCLUDING CONSTRAINTS
                 INCLUDING STORAGE
                 INCLUDING COMMENTS)',
            target_name, source_name);
    ELSE
        EXECUTE format('SELECT count(*) FROM net.%I', target_name)
           INTO target_rows;
        IF target_rows <> 0 THEN
            RAISE EXCEPTION
                'Целевой слой net.% уже заполнен (% строк)',
                target_name, target_rows;
        END IF;
    END IF;

    expression_with_alias := replace(geometry_expression, 'geom', 's.geom');
    EXECUTE format(
        'INSERT INTO net.%I
         SELECT s.*
         FROM net.%I s
         WHERE GeometryType(s.geom) = ANY ($1)
           AND NOT ST_IsEmpty(%s)',
        target_name, source_name, expression_with_alias)
    USING source_types;

    GET DIAGNOSTICS target_rows = ROW_COUNT;

    EXECUTE format(
        'ALTER TABLE net.%I
         ALTER COLUMN geom TYPE geometry(%s, 9998)
         USING (%s)::geometry(%s, 9998)',
        target_name, target_type, geometry_expression, target_type);

    RETURN target_rows;
END
$$;

DO $$
DECLARE
    source_count bigint;
    moved_count bigint;
    rejected_count bigint;
BEGIN
    IF to_regclass('net.building_consumer') IS NOT NULL THEN
        moved_count :=
            pg_temp.tgid_split_mixed_layer(
                'building_consumer', 'building_consumer_point',
                ARRAY['POINT'], 'Point', 'geom')
          + pg_temp.tgid_split_mixed_layer(
                'building_consumer', 'building_consumer_line',
                ARRAY['LINESTRING', 'MULTILINESTRING'], 'MultiLineString',
                'ST_Multi(ST_CollectionExtract(geom, 2))')
          + pg_temp.tgid_split_mixed_layer(
                'building_consumer', 'building_consumer_area',
                ARRAY['POLYGON', 'MULTIPOLYGON'], 'MultiPolygon',
                'ST_Multi(ST_CollectionExtract(ST_MakeValid(geom), 3))');

        SELECT count(*) INTO source_count FROM net.building_consumer;
        IF moved_count <> source_count THEN
            RAISE EXCEPTION
                'building_consumer: перенесено %, ожидалось %',
                moved_count, source_count;
        END IF;
    END IF;

    IF to_regclass('net.building_tu') IS NOT NULL THEN
        INSERT INTO meta.geometry_repair_log (
            table_name, object_id, source_id, reason, method,
            original_geom, repaired_geom)
        SELECT 'net.building_tu', id, src_id, ST_IsValidReason(geom),
               'ST_MakeValid before geometry-type split', geom,
               ST_MakeValid(geom)
        FROM net.building_tu
        WHERE NOT ST_IsValid(geom)
        ON CONFLICT (table_name, object_id, method) DO NOTHING;

        INSERT INTO net.conversion_reject (src_table, src_id, reason, detail)
        SELECT 'net.building_tu', s.src_id,
               'полигон после ST_MakeValid не содержит площадной геометрии',
               (to_jsonb(s) - 'geom') || jsonb_build_object(
                   'object_id', s.id,
                   'geometry_ewkt', ST_AsEWKT(s.geom),
                   'make_valid_type', GeometryType(ST_MakeValid(s.geom)))
        FROM net.building_tu s
        WHERE GeometryType(s.geom) IN ('POLYGON', 'MULTIPOLYGON')
          AND ST_IsEmpty(
              ST_CollectionExtract(ST_MakeValid(s.geom), 3))
          AND NOT EXISTS (
              SELECT 1
              FROM net.conversion_reject r
              WHERE r.src_table = 'net.building_tu'
                AND r.src_id IS NOT DISTINCT FROM s.src_id
                AND r.reason =
                    'полигон после ST_MakeValid не содержит площадной геометрии');

        moved_count :=
            pg_temp.tgid_split_mixed_layer(
                'building_tu', 'building_tu_line',
                ARRAY['LINESTRING', 'MULTILINESTRING'], 'MultiLineString',
                'ST_Multi(ST_CollectionExtract(geom, 2))')
          + pg_temp.tgid_split_mixed_layer(
                'building_tu', 'building_tu_area',
                ARRAY['POLYGON', 'MULTIPOLYGON'], 'MultiPolygon',
                'ST_Multi(ST_CollectionExtract(ST_MakeValid(geom), 3))');

        SELECT count(*) INTO source_count FROM net.building_tu;
        SELECT count(*) INTO rejected_count
        FROM net.building_tu s
        WHERE GeometryType(s.geom) IN ('POLYGON', 'MULTIPOLYGON')
          AND ST_IsEmpty(
              ST_CollectionExtract(ST_MakeValid(s.geom), 3));
        IF moved_count + rejected_count <> source_count THEN
            RAISE EXCEPTION
                'building_tu: перенесено % + отклонено %, ожидалось %',
                moved_count, rejected_count, source_count;
        END IF;
    END IF;

    IF to_regclass('net.pipe_reconstructed') IS NOT NULL THEN
        moved_count :=
            pg_temp.tgid_split_mixed_layer(
                'pipe_reconstructed', 'pipe_reconstructed_point',
                ARRAY['POINT'], 'Point', 'geom')
          + pg_temp.tgid_split_mixed_layer(
                'pipe_reconstructed', 'pipe_reconstructed_line',
                ARRAY['LINESTRING', 'MULTILINESTRING'], 'MultiLineString',
                'ST_Multi(ST_CollectionExtract(geom, 2))');

        SELECT count(*) INTO source_count FROM net.pipe_reconstructed;
        IF moved_count <> source_count THEN
            RAISE EXCEPTION
                'pipe_reconstructed: перенесено %, ожидалось %',
                moved_count, source_count;
        END IF;
    END IF;
END
$$;

DO $$
DECLARE
    source_name text;
    attic_name text;
BEGIN
    FOREACH source_name IN ARRAY ARRAY[
        'building_consumer', 'building_tu', 'pipe_reconstructed'
    ] LOOP
        IF to_regclass(format('net.%I', source_name)) IS NULL THEN
            CONTINUE;
        END IF;
        attic_name := source_name || '_mixed_legacy';
        IF to_regclass(format('attic.%I', attic_name)) IS NOT NULL THEN
            RAISE EXCEPTION 'Резервная таблица attic.% уже существует', attic_name;
        END IF;
        EXECUTE format('ALTER TABLE net.%I SET SCHEMA attic', source_name);
        EXECUTE format('ALTER TABLE attic.%I RENAME TO %I',
                       source_name, attic_name);
    END LOOP;
END
$$;

DO $$
DECLARE
    target_name text;
BEGIN
    FOREACH target_name IN ARRAY ARRAY[
        'building_consumer_point', 'building_consumer_line',
        'building_consumer_area', 'building_tu_line', 'building_tu_area',
        'pipe_reconstructed_point', 'pipe_reconstructed_line'
    ] LOOP
        IF to_regclass(format('net.%I', target_name)) IS NULL THEN
            RAISE EXCEPTION 'Не создан обязательный слой net.%', target_name;
        END IF;
        IF NOT EXISTS (
            SELECT 1 FROM pg_constraint
            WHERE conrelid = format('net.%I', target_name)::regclass
              AND contype = 'p')
        THEN
            EXECUTE format(
                'ALTER TABLE net.%I ADD CONSTRAINT %I PRIMARY KEY (id)',
                target_name, target_name || '_pkey');
        END IF;
        EXECUTE format(
            'CREATE UNIQUE INDEX IF NOT EXISTS %I ON net.%I (src_id)',
            target_name || '_src_uidx', target_name);
        EXECUTE format(
            'CREATE INDEX IF NOT EXISTS %I ON net.%I USING gist (geom)',
            target_name || '_geom_idx', target_name);
    END LOOP;
END
$$;

DO $$
DECLARE
    layer_name text;
    target_type text;
BEGIN
    FOR layer_name, target_type IN
        SELECT * FROM (VALUES
            ('building_inlet', 'Point'),
            ('building_inlet_line', 'Point'),
            ('diameter_change', 'Point'),
            ('manhole', 'Point'),
            ('pipe_turn', 'Point'),
            ('pressure_test_defect', 'Point'),
            ('repair_section', 'LineString'),
            ('support', 'Point')
        ) AS expected(layer_name, target_type)
    LOOP
        IF EXISTS (
            SELECT 1 FROM geometry_columns
            WHERE f_table_schema = 'net'
              AND f_table_name = layer_name
              AND upper(type) = 'GEOMETRY')
        THEN
            EXECUTE format('DROP VIEW IF EXISTS net.%I',
                           layer_name || '_wgs84');
            EXECUTE format(
                'DROP TRIGGER IF EXISTS geometry_change_audit ON net.%I',
                layer_name);
            EXECUTE format(
                'ALTER TABLE net.%I
                 ALTER COLUMN geom TYPE geometry(%s, 9998)
                 USING geom::geometry(%s, 9998)',
                layer_name, target_type, target_type);
        END IF;
    END LOOP;
END
$$;

DELETE FROM meta.layer_catalog
WHERE schema_name = 'net'
  AND table_name IN (
      'building_consumer', 'building_tu', 'pipe_reconstructed');

DELETE FROM meta.field_catalog
WHERE table_schema = 'net'
  AND table_name IN (
      'building_consumer', 'building_tu', 'pipe_reconstructed');

INSERT INTO meta.schema_version (version, description)
VALUES (17, 'Типизация и разделение смешанных GIS-слоёв')
ON CONFLICT (version) DO NOTHING;

COMMIT;
