-- Единые правила топологии для линий, создаваемых из Qt, QGIS или SQL.

\set ON_ERROR_STOP on

BEGIN;

CREATE OR REPLACE FUNCTION net.validate_line_topology()
RETURNS trigger
LANGUAGE plpgsql
AS $$
DECLARE
    from_table text;
    to_table text;
    from_fragment int;
    to_fragment int;
    from_source_id int;
    to_source_id int;
    from_geom geometry;
    to_geom geometry;
    from_active boolean;
    to_active boolean;
BEGIN
    IF NEW.node_from IS NULL OR NEW.node_to IS NULL THEN
        RAISE EXCEPTION 'Для линии должны быть заданы оба конечных узла';
    END IF;
    IF NEW.node_from = NEW.node_to THEN
        RAISE EXCEPTION 'Начальный и конечный узлы линии должны различаться';
    END IF;

    SELECT tbl INTO from_table
      FROM net.node_reg WHERE id = NEW.node_from;
    SELECT tbl INTO to_table
      FROM net.node_reg WHERE id = NEW.node_to;
    IF from_table IS NULL OR to_table IS NULL THEN
        RAISE EXCEPTION 'Один из конечных узлов отсутствует в net.node_reg';
    END IF;

    EXECUTE format(
        'SELECT fragment_id, src_id, geom, removed_at IS NULL
           FROM net.%I WHERE id = $1',
        from_table)
    INTO from_fragment, from_source_id, from_geom, from_active
    USING NEW.node_from;
    EXECUTE format(
        'SELECT fragment_id, src_id, geom, removed_at IS NULL
           FROM net.%I WHERE id = $1',
        to_table)
    INTO to_fragment, to_source_id, to_geom, to_active
    USING NEW.node_to;

    IF from_geom IS NULL OR to_geom IS NULL
       OR NOT coalesce(from_active, false)
       OR NOT coalesce(to_active, false) THEN
        RAISE EXCEPTION 'Конечный узел линии не найден или находится в архиве';
    END IF;
    IF from_fragment IS DISTINCT FROM to_fragment THEN
        RAISE EXCEPTION 'Конечные узлы относятся к разным фрагментам';
    END IF;
    IF NEW.fragment_id IS NULL THEN
        NEW.fragment_id := from_fragment;
    ELSIF NEW.fragment_id IS DISTINCT FROM from_fragment THEN
        RAISE EXCEPTION 'Фрагмент линии не совпадает с фрагментом узлов';
    END IF;
    IF ST_Equals(from_geom, to_geom) THEN
        RAISE EXCEPTION 'Конечные узлы имеют одинаковые координаты';
    END IF;

    IF NEW.geom IS NULL THEN
        NEW.geom := ST_MakeLine(from_geom, to_geom);
    ELSIF ST_GeometryType(NEW.geom) <> 'ST_LineString'
          OR ST_NPoints(NEW.geom) < 2 THEN
        RAISE EXCEPTION 'Геометрия линии должна содержать минимум две точки';
    ELSE
        NEW.geom := ST_SetPoint(NEW.geom, 0, from_geom);
        NEW.geom := ST_SetPoint(
            NEW.geom, ST_NPoints(NEW.geom) - 1, to_geom);
    END IF;

    NEW.node_from_src := from_source_id;
    NEW.node_to_src := to_source_id;
    NEW.fileid_src := NEW.fragment_id;
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
                  AND a.attname = 'node_from'
                  AND NOT a.attisdropped)
           AND EXISTS (
               SELECT 1 FROM pg_attribute a
                WHERE a.attrelid = c.oid
                  AND a.attname = 'node_to'
                  AND NOT a.attisdropped)
    LOOP
        EXECUTE format(
            'DROP TRIGGER IF EXISTS line_topology ON net.%I',
            r.table_name);
        EXECUTE format(
            'CREATE TRIGGER line_topology
             BEFORE INSERT OR UPDATE OF
                 node_from, node_to, fragment_id, geom, removed_at
             ON net.%I
             FOR EACH ROW EXECUTE FUNCTION net.validate_line_topology()',
            r.table_name);
    END LOOP;
END
$$;

INSERT INTO meta.schema_version (version, description)
VALUES (5, 'Проверка топологии при создании и изменении линий')
ON CONFLICT (version) DO NOTHING;

COMMIT;
