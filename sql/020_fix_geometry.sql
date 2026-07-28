-- НЕОБЯЗАТЕЛЬНЫЙ шаг: починка геометрии, унаследованной из public.
--
-- Все перечисленные дефекты присутствуют в исходных данных: конвертер их
-- не порождает, а честно переносит. Применять осознанно — ST_MakeValid
-- меняет геометрию объекта.
--
-- Проверить, что будет затронуто, до применения:
--     SELECT count(*) FROM net.building WHERE NOT ST_IsValid(geom);

\set ON_ERROR_STOP on

BEGIN;

-- 1. Самопересекающиеся полигоны зданий: 23 в building, 11 в building_tu.
--    Источник — public.zdaniya_2 и public.zdaniya_tu, там ровно столько же.
--    ST_MakeValid разрезает самопересечение, площадь сохраняется.
CREATE TEMP TABLE _fixed_poly AS
SELECT 'building' AS tbl, id, ST_Area(geom) AS area_before FROM net.building
WHERE NOT ST_IsValid(geom)
UNION ALL
SELECT 'building_tu', id, ST_Area(geom) FROM net.building_tu
WHERE NOT ST_IsValid(geom);

UPDATE net.building SET geom = ST_MakeValid(geom) WHERE NOT ST_IsValid(geom);
UPDATE net.building_tu SET geom = ST_MakeValid(geom) WHERE NOT ST_IsValid(geom);

-- 2. Вырожденные линии нулевой длины (оба конца в одной точке).
--    Геометрию починить нельзя — это ошибка данных, поэтому только помечаем.
INSERT INTO net.conversion_reject (src_table, src_id, reason, detail)
SELECT 'net.pipe_section', src_id, 'линия нулевой длины: концы совпадают',
       jsonb_build_object('node_from', node_from, 'node_to', node_to)
FROM net.pipe_section
WHERE ST_NPoints(geom) < 3 AND ST_Length(geom) = 0;

-- Контроль: после починки невалидных полигонов остаться не должно.
DO $$
DECLARE n int;
BEGIN
    SELECT count(*) INTO n FROM (
        SELECT geom FROM net.building
        UNION ALL SELECT geom FROM net.building_tu
    ) s WHERE NOT ST_IsValid(geom);
    IF n > 0 THEN
        RAISE EXCEPTION 'осталось % невалидных полигонов', n;
    END IF;
END $$;

COMMIT;

\echo 'Геометрия починена'
