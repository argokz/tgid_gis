-- Сохранение топологии при перемещении и архивировании узлов.

\set ON_ERROR_STOP on

BEGIN;

CREATE OR REPLACE FUNCTION net.propagate_node_geometry()
RETURNS trigger
LANGUAGE plpgsql
AS $$
DECLARE
    r record;
BEGIN
    IF ST_Equals(OLD.geom, NEW.geom) THEN
        RETURN NEW;
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
            'UPDATE net.%I
                SET geom = CASE
                    WHEN node_from = $1
                    THEN ST_SetPoint(geom, 0, $2)
                    ELSE ST_SetPoint(geom, ST_NPoints(geom) - 1, $2)
                END
              WHERE (node_from = $1 OR node_to = $1)
                AND removed_at IS NULL',
            r.table_name)
        USING NEW.id, NEW.geom;
    END LOOP;
    RETURN NEW;
END
$$;

CREATE OR REPLACE FUNCTION net.prevent_archiving_connected_node()
RETURNS trigger
LANGUAGE plpgsql
AS $$
DECLARE
    r record;
    connected boolean;
BEGIN
    IF OLD.removed_at IS NOT NULL OR NEW.removed_at IS NULL THEN
        RETURN NEW;
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
            'SELECT EXISTS (
                 SELECT 1 FROM net.%I
                  WHERE (node_from = $1 OR node_to = $1)
                    AND removed_at IS NULL)',
            r.table_name)
        INTO connected
        USING NEW.id;
        IF connected THEN
            RAISE EXCEPTION
                'Нельзя архивировать узел %: существует активная линия в net.%',
                NEW.id, r.table_name;
        END IF;
    END LOOP;
    RETURN NEW;
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
                  AND a.attname = 'fragment_id'
                  AND NOT a.attisdropped)
           AND EXISTS (
               SELECT 1 FROM pg_attribute a
                WHERE a.attrelid = c.oid
                  AND a.attname = 'removed_at'
                  AND NOT a.attisdropped)
           AND NOT EXISTS (
               SELECT 1 FROM pg_attribute a
                WHERE a.attrelid = c.oid
                  AND a.attname = 'node_from'
                  AND NOT a.attisdropped)
           AND EXISTS (
               SELECT 1 FROM pg_attribute a
                WHERE a.attrelid = c.oid
                  AND a.attname = 'geom'
                  AND NOT a.attisdropped
                  AND postgis_typmod_type(a.atttypmod) = 'Point')
    LOOP
        EXECUTE format(
            'DROP TRIGGER IF EXISTS node_geometry_propagate ON net.%I',
            r.table_name);
        EXECUTE format(
            'CREATE TRIGGER node_geometry_propagate
             AFTER UPDATE OF geom ON net.%I
             FOR EACH ROW EXECUTE FUNCTION net.propagate_node_geometry()',
            r.table_name);
        EXECUTE format(
            'DROP TRIGGER IF EXISTS node_archive_guard ON net.%I',
            r.table_name);
        EXECUTE format(
            'CREATE TRIGGER node_archive_guard
             BEFORE UPDATE OF removed_at ON net.%I
             FOR EACH ROW
             EXECUTE FUNCTION net.prevent_archiving_connected_node()',
            r.table_name);
    END LOOP;
END
$$;

INSERT INTO meta.schema_version (version, description)
VALUES (6, 'Перенос концов линий и защита архивирования связанных узлов')
ON CONFLICT (version) DO NOTHING;

COMMIT;
