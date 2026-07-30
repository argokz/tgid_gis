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

-- 1. Самопересекающиеся полигоны нормализует сам конвертер через
--    ST_MakeValid + ST_Multi. Здесь оставлена повторная идемпотентная
--    страховка для старых переходных БД и новой раздельной схемы слоёв.
DO $$
DECLARE t text;
BEGIN
  FOREACH t IN ARRAY ARRAY['building', 'building_tu', 'building_tu_area',
                           'building_consumer_area']
  LOOP
    IF to_regclass('net.' || t) IS NOT NULL THEN
      EXECUTE format(
        'UPDATE net.%I
         SET geom = ST_Multi(ST_CollectionExtract(ST_MakeValid(geom), 3))
         WHERE GeometryType(geom) IN (''POLYGON'', ''MULTIPOLYGON'')
           AND NOT ST_IsValid(geom)', t);
    END IF;
  END LOOP;
END $$;

-- 2. Вырожденные линии нулевой длины (оба конца в одной точке).
--    Геометрию починить нельзя. Такие строки нельзя оставлять в GIS-слое:
--    сохраняем полный payload в line_orphan и удаляем из pipe_section.
INSERT INTO net.conversion_reject (src_table, src_id, reason, detail)
SELECT 'net.pipe_section', src_id, 'линия нулевой длины: концы совпадают',
       jsonb_build_object('node_from', node_from, 'node_to', node_to)
FROM net.pipe_section
WHERE ST_NPoints(geom) < 3 AND ST_Length(geom) = 0;

INSERT INTO net.line_orphan
    (id, src_table, nodeid1, nodeid2, reason, payload)
SELECT id, 'net.pipe_section', node_from_src, node_to_src,
       'невалидная линия нулевой длины: концы совпадают',
       to_jsonb(p) - 'geom'
FROM net.pipe_section p
WHERE ST_NPoints(geom) < 3 AND ST_Length(geom) = 0
ON CONFLICT (id) DO UPDATE
SET src_table = EXCLUDED.src_table,
    nodeid1 = EXCLUDED.nodeid1,
    nodeid2 = EXCLUDED.nodeid2,
    reason = EXCLUDED.reason,
    payload = EXCLUDED.payload;

DELETE FROM net.pipe_section
WHERE ST_NPoints(geom) < 3 AND ST_Length(geom) = 0;

COMMIT;

\echo 'Геометрия починена'
