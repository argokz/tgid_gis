-- Аудит создания новых объектов из Qt, QGIS и других SQL-клиентов.

\set ON_ERROR_STOP on

BEGIN;

ALTER TABLE meta.object_change_log
    DROP CONSTRAINT IF EXISTS object_change_log_operation_check;

ALTER TABLE meta.object_change_log
    ADD CONSTRAINT object_change_log_operation_check
    CHECK (operation IN ('insert', 'update', 'archive', 'restore'));

CREATE OR REPLACE FUNCTION net.log_object_insert()
RETURNS trigger
LANGUAGE plpgsql
AS $$
DECLARE
    new_values jsonb := to_jsonb(NEW) - 'geom';
    inserted_values jsonb;
BEGIN
    SELECT coalesce(
               jsonb_object_agg(
                   e.key,
                   jsonb_build_object('old', NULL, 'new', e.value)),
               '{}'::jsonb)
      INTO inserted_values
      FROM jsonb_each(new_values) AS e(key, value)
     WHERE e.value <> 'null'::jsonb
       AND e.key NOT IN ('row_version', 'updated_at', 'updated_by');

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
        'insert',
        NEW.updated_at,
        session_user,
        nullif(current_setting('application_name', true), ''),
        inserted_values);

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
            'DROP TRIGGER IF EXISTS object_insert_log ON net.%I',
            r.table_name);
        EXECUTE format(
            'CREATE TRIGGER object_insert_log
             AFTER INSERT ON net.%I
             FOR EACH ROW EXECUTE FUNCTION net.log_object_insert()',
            r.table_name);
    END LOOP;
END
$$;

INSERT INTO meta.schema_version (version, description)
VALUES (4, 'Создание точечных объектов и аудит INSERT')
ON CONFLICT (version) DO NOTHING;

COMMIT;
