-- Оптимистическая блокировка и отметка автора для всех объектных GIS-таблиц.
--
-- Любой UPDATE, выполненный из Qt, QGIS или psql, увеличивает row_version.
-- Qt обновляет строку с условием WHERE id = ? AND row_version = ?, поэтому
-- чужое изменение между чтением и сохранением обнаруживается без перезаписи.

\set ON_ERROR_STOP on

BEGIN;

CREATE SCHEMA IF NOT EXISTS meta;

CREATE TABLE IF NOT EXISTS meta.schema_version (
    version integer PRIMARY KEY,
    description text NOT NULL,
    applied_at timestamptz NOT NULL DEFAULT clock_timestamp()
);

INSERT INTO meta.schema_version (version, description)
VALUES (2, 'Оптимистическая блокировка GIS-объектов')
ON CONFLICT (version) DO NOTHING;

CREATE OR REPLACE FUNCTION net.touch_object_row()
RETURNS trigger
LANGUAGE plpgsql
AS $$
BEGIN
    NEW.row_version := OLD.row_version + 1;
    NEW.updated_at := clock_timestamp();
    NEW.updated_by := current_user;
    RETURN NEW;
END
$$;

DO $$
DECLARE
    r record;
BEGIN
    FOR r IN
        SELECT DISTINCT c.relname AS table_name
        FROM pg_class c
        JOIN pg_namespace n ON n.oid = c.relnamespace
        JOIN pg_attribute a
          ON a.attrelid = c.oid
         AND a.attname = 'geom'
         AND NOT a.attisdropped
        WHERE n.nspname = 'net'
          AND c.relkind IN ('r', 'p')
          AND a.atttypid = 'geometry'::regtype
        ORDER BY c.relname
    LOOP
        EXECUTE format(
            'ALTER TABLE net.%I
               ADD COLUMN IF NOT EXISTS row_version bigint NOT NULL DEFAULT 1,
               ADD COLUMN IF NOT EXISTS updated_at timestamptz NOT NULL
                   DEFAULT clock_timestamp(),
               ADD COLUMN IF NOT EXISTS updated_by text NOT NULL
                   DEFAULT current_user',
            r.table_name);

        EXECUTE format(
            'DROP TRIGGER IF EXISTS object_touch ON net.%I',
            r.table_name);
        EXECUTE format(
            'CREATE TRIGGER object_touch
             BEFORE UPDATE ON net.%I
             FOR EACH ROW EXECUTE FUNCTION net.touch_object_row()',
            r.table_name);
    END LOOP;
END
$$;

COMMIT;

