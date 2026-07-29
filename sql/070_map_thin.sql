-- Тонкие представления для отрисовки карты (~15 колонок).
--
-- Полные us_net / ut_net отдают 67 / 56 колонок и тянут результаты
-- расчёта. Отрисовке карты достаточно id, типа, геометрии/координат,
-- подписи и пары визуальных признаков.
--
-- Важно: removed_at IS NULL фильтруется ВНУТРИ каждой ветви UNION —
-- иначе планировщик не использует частичные индексы (см. docs/03).
--
-- Фильтр фрагмента дописывается снаружи:
--   SELECT ... FROM net.v_map_nodes n WHERE TRUE AND n.fileID IN (...)
--   SELECT ... FROM net.v_map_lines n1 WHERE TRUE AND n1.fileID IN (...)
--
--   psql -d tgid_gis -f sql/070_map_thin.sql
--   python tools/bench_map.py --db tgid_gis --fragments 80

\set ON_ERROR_STOP on

CREATE OR REPLACE VIEW net.v_map_nodes AS
SELECT * FROM (
    SELECT src_id AS id, fragment_id AS fileid,
           ST_X(geom) * 100.0 AS x, -ST_Y(geom) * 100.0 AS y,
           'PR'::text AS type_txt, displaysign, externalnodename, nodename,
           externalcodeid, internalnodeid, externalsignid,
           0 AS removed, id AS id2, geom, 'consumer_real'::text AS class_table
      FROM net.consumer_real WHERE removed_at IS NULL
    UNION ALL
    SELECT src_id, fragment_id, ST_X(geom)*100.0, -ST_Y(geom)*100.0,
           'PO', displaysign, externalnodename, nodename,
           externalcodeid, internalnodeid, externalsignid,
           0, id, geom, 'consumer_general'
      FROM net.consumer_general WHERE removed_at IS NULL
    UNION ALL
    SELECT src_id, fragment_id, ST_X(geom)*100.0, -ST_Y(geom)*100.0,
           'IS', displaysign, externalnodename, nodename,
           externalcodeid, internalnodeid, externalsignid,
           0, id, geom, 'heat_source'
      FROM net.heat_source WHERE removed_at IS NULL
    UNION ALL
    SELECT src_id, fragment_id, ST_X(geom)*100.0, -ST_Y(geom)*100.0,
           'HS', displaysign, externalnodename, nodename,
           externalcodeid, internalnodeid, externalsignid,
           0, id, geom, 'pump_station'
      FROM net.pump_station WHERE removed_at IS NULL
    UNION ALL
    SELECT src_id, fragment_id, ST_X(geom)*100.0, -ST_Y(geom)*100.0,
           'C3', displaysign, externalnodename, nodename,
           externalcodeid, internalnodeid, externalsignid,
           0, id, geom, 'valve_3way'
      FROM net.valve_3way WHERE removed_at IS NULL
    UNION ALL
    SELECT src_id, fragment_id, ST_X(geom)*100.0, -ST_Y(geom)*100.0,
           'US2', displaysign, externalnodename, nodename,
           externalcodeid, internalnodeid, externalsignid,
           0, id, geom, 'connect_node'
      FROM net.connect_node WHERE removed_at IS NULL
    UNION ALL
    SELECT src_id, fragment_id, ST_X(geom)*100.0, -ST_Y(geom)*100.0,
           'US', displaysign, externalnodename, nodename,
           externalcodeid, internalnodeid, externalsignid,
           0, id, geom, 'heat_chamber'
      FROM net.heat_chamber WHERE removed_at IS NULL
    UNION ALL
    SELECT src_id, fragment_id, ST_X(geom)*100.0, -ST_Y(geom)*100.0,
           'US', displaysign, externalnodename, nodename,
           externalcodeid, internalnodeid, externalsignid,
           0, id, geom, 'refill_node'
      FROM net.refill_node WHERE removed_at IS NULL
    UNION ALL
    SELECT src_id, fragment_id, ST_X(geom)*100.0, -ST_Y(geom)*100.0,
           'US', displaysign, externalnodename, nodename,
           externalcodeid, internalnodeid, externalsignid,
           0, id, geom, 'node_plain'
      FROM net.node_plain WHERE removed_at IS NULL
) u;

CREATE OR REPLACE VIEW net.v_map_lines AS
SELECT * FROM (
    SELECT src_id AS id, fragment_id AS fileid,
           node_from_src AS nodeid1, node_to_src AS nodeid2,
           'UT'::text AS type_txt, displaysign,
           diametercondit, pipesectlength, magistral,
           coords_legacy AS coords, 0 AS removed, id AS id2,
           externalsignlineid, geom, 'pipe_section'::text AS class_table
      FROM net.pipe_section WHERE removed_at IS NULL
    UNION ALL
    SELECT src_id, fragment_id, node_from_src, node_to_src,
           'ZD', displaysign, diametercondit,
           NULL::double precision, NULL::integer,
           coords_legacy, 0, id, externalsignlineid, geom, 'damper'
      FROM net.damper WHERE removed_at IS NULL
    UNION ALL
    SELECT src_id, fragment_id, node_from_src, node_to_src,
           'HC', displaysign, NULL::double precision,
           NULL::double precision, NULL::integer,
           coords_legacy, 0, id, externalsignlineid, geom, 'pump'
      FROM net.pump WHERE removed_at IS NULL
    UNION ALL
    SELECT src_id, fragment_id, node_from_src, node_to_src,
           NULL::text, displaysign, NULL::double precision,
           NULL::double precision, NULL::integer,
           coords_legacy, 0, id, externalsignlineid, geom, 'line_plain'
      FROM net.line_plain WHERE removed_at IS NULL
) u;
