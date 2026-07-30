-- Журнал всех изменений объектных GIS-таблиц.
--
-- Триггер из 080_concurrency.sql уже вызывается для UPDATE из Qt, QGIS и psql.
-- Здесь функция расширяется: версия строки и технические поля обновляются как
-- раньше, а изменённые атрибуты записываются в meta.object_change_log.

\set ON_ERROR_STOP on

BEGIN;

CREATE SCHEMA IF NOT EXISTS meta;

CREATE TABLE IF NOT EXISTS meta.object_change_log (
    id               bigserial PRIMARY KEY,
    schema_name      text NOT NULL,
    table_name       text NOT NULL,
    object_id        bigint NOT NULL,
    row_version      bigint NOT NULL,
    operation        text NOT NULL
                     CHECK (operation IN ('update', 'archive', 'restore')),
    changed_at       timestamptz NOT NULL DEFAULT clock_timestamp(),
    changed_by       text NOT NULL,
    application_name text,
    changed_fields   jsonb NOT NULL DEFAULT '{}'::jsonb
);

CREATE INDEX IF NOT EXISTS object_change_log_object_idx
    ON meta.object_change_log
       (schema_name, table_name, object_id, row_version DESC);

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
                  AND a.attname = 'removed_at'
                  AND NOT a.attisdropped)
    LOOP
        EXECUTE format(
            'CREATE INDEX IF NOT EXISTS %I ON net.%I (removed_at DESC)
             WHERE removed_at IS NOT NULL',
            left(r.table_name, 45) || '_archived_idx',
            r.table_name);
    END LOOP;
END
$$;

CREATE OR REPLACE FUNCTION net.touch_object_row()
RETURNS trigger
LANGUAGE plpgsql
AS $$
DECLARE
    old_values jsonb := to_jsonb(OLD) - 'geom';
    new_values jsonb;
    differences jsonb;
    action_name text := 'update';
BEGIN
    NEW.row_version := OLD.row_version + 1;
    NEW.updated_at := clock_timestamp();
    NEW.updated_by := session_user;
    new_values := to_jsonb(NEW) - 'geom';

    IF old_values ->> 'removed_at' IS NULL
       AND new_values ->> 'removed_at' IS NOT NULL THEN
        action_name := 'archive';
    ELSIF old_values ->> 'removed_at' IS NOT NULL
          AND new_values ->> 'removed_at' IS NULL THEN
        action_name := 'restore';
    END IF;

    SELECT coalesce(
               jsonb_object_agg(
                   coalesce(n.key, o.key),
                   jsonb_build_object('old', o.value, 'new', n.value)),
               '{}'::jsonb)
      INTO differences
      FROM jsonb_each(old_values) AS o(key, value)
      FULL JOIN jsonb_each(new_values) AS n(key, value) USING (key)
     WHERE o.value IS DISTINCT FROM n.value
       AND coalesce(n.key, o.key) NOT IN
           ('row_version', 'updated_at', 'updated_by');

    INSERT INTO meta.object_change_log (
        schema_name,
        table_name,
        object_id,
        row_version,
        operation,
        changed_at,
        changed_by,
        application_name,
        changed_fields)
    VALUES (
        TG_TABLE_SCHEMA,
        TG_TABLE_NAME,
        (new_values ->> 'id')::bigint,
        NEW.row_version,
        action_name,
        NEW.updated_at,
        session_user,
        nullif(current_setting('application_name', true), ''),
        differences);

    RETURN NEW;
END
$$;

INSERT INTO meta.schema_version (version, description)
VALUES (3, 'Журнал, архивирование и восстановление GIS-объектов')
ON CONFLICT (version) DO NOTHING;

COMMIT;
