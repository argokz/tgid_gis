-- Каталог слоёв и готовность к QGIS / ArcGIS Pro.
-- Применять на БД, где уже есть схема net после конвертера.
-- Не создаёт compat-представления nodes/linesobj.

CREATE SCHEMA IF NOT EXISTS meta;
CREATE SCHEMA IF NOT EXISTS ref;

CREATE TABLE IF NOT EXISTS meta.layer_catalog (
    id              serial PRIMARY KEY,
    schema_name     text NOT NULL DEFAULT 'net',
    table_name      text NOT NULL,
    geometry_type   text NOT NULL,
    srid            int  NOT NULL DEFAULT 9998,
    display_name_ru text NOT NULL,
    is_editable     boolean NOT NULL DEFAULT true,
    kind            text NOT NULL DEFAULT 'object',
    sort_order      int NOT NULL DEFAULT 100,
    UNIQUE (schema_name, table_name)
);

CREATE TABLE IF NOT EXISTS meta.spatial_ref_system (
    srid integer PRIMARY KEY,
    auth_name varchar(256),
    auth_srid integer,
    srtext varchar(2048),
    proj4text varchar(2048)
);

CREATE TABLE IF NOT EXISTS meta.schema_version (
    version integer PRIMARY KEY,
    description text NOT NULL,
    applied_at timestamptz NOT NULL DEFAULT clock_timestamp()
);

INSERT INTO meta.schema_version (version, description)
VALUES (1, 'Чистая объектная GIS-схема net/ref/meta')
ON CONFLICT (version) DO NOTHING;

COMMENT ON TABLE meta.layer_catalog IS
    'Слои для QGIS и ArcGIS Pro: каждая строка = одна объектная таблица со своей geom.';

TRUNCATE meta.layer_catalog;
INSERT INTO meta.layer_catalog (table_name, geometry_type, display_name_ru, kind, sort_order)
SELECT
    c.relname,
    upper(coalesce(g.type, 'GEOMETRY')),
    c.relname,
    'object',
    100
FROM pg_class c
JOIN pg_namespace n ON n.oid = c.relnamespace
JOIN geometry_columns g
  ON g.f_table_schema = n.nspname
 AND g.f_table_name = c.relname
 AND g.f_geometry_column = 'geom'
WHERE n.nspname = 'net'
  AND c.relkind = 'r'
ORDER BY c.relname;

-- WGS84-представления для веб / части Online-сценариев.
-- Редактирование в Pro/QGIS — по net.<table>.geom (SRID 9998).
DO $$
DECLARE
    r record;
    has_removed boolean;
    where_sql text;
BEGIN
    FOR r IN SELECT table_name FROM meta.layer_catalog LOOP
        SELECT EXISTS (
            SELECT 1 FROM information_schema.columns
            WHERE table_schema = 'net'
              AND table_name = r.table_name
              AND column_name = 'removed_at'
        ) INTO has_removed;

        where_sql := CASE WHEN has_removed THEN 'WHERE t.removed_at IS NULL' ELSE '' END;

        EXECUTE format(
            'CREATE OR REPLACE VIEW net.%I AS
             SELECT t.*, ST_Transform(t.geom, 4326) AS geom_4326
             FROM net.%I t %s',
            r.table_name || '_wgs84',
            r.table_name,
            where_sql
        );
    END LOOP;
END $$;
