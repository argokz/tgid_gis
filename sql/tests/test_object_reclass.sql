-- Проверка атомарной смены класса узла и линии. Данные откатываются.

\set ON_ERROR_STOP on

BEGIN;
SET LOCAL application_name = 'tgid_test_object_reclass';

DO $$
DECLARE
    test_fragment integer;
    first_node bigint;
    second_node bigint;
    test_line bigint;
    colliding_line bigint;
    node_version bigint;
    line_version bigint;
    colliding_line_version bigint;
    result_version bigint;
    copied integer;
    result_is_node boolean;
    conflict_seen boolean := false;
    original_node_geom geometry;
    original_line_geom geometry;
BEGIN
    SELECT id INTO test_fragment
      FROM net.fragment WHERE removed_at IS NULL ORDER BY id LIMIT 1;
    IF test_fragment IS NULL THEN
        RAISE EXCEPTION 'Нет активного фрагмента для теста';
    END IF;

    INSERT INTO net.connect_node (fragment_id, geom)
    VALUES (
        test_fragment,
        ST_SetSRID(ST_MakePoint(730000, 4800000), 9998))
    RETURNING id, row_version, geom
      INTO first_node, node_version, original_node_geom;
    INSERT INTO net.connect_node (fragment_id, geom)
    VALUES (
        test_fragment,
        ST_SetSRID(ST_MakePoint(730100, 4800000), 9998))
    RETURNING id INTO second_node;
    INSERT INTO net.pipe_section (fragment_id, node_from, node_to)
    VALUES (test_fragment, first_node, second_node)
    RETURNING id, row_version, geom
      INTO test_line, line_version, original_line_geom;

    SELECT node.id
      INTO colliding_line
      FROM net.node_reg node
     WHERE NOT EXISTS (
               SELECT 1 FROM net.line_reg line WHERE line.id = node.id)
     ORDER BY node.id
     LIMIT 1;
    IF colliding_line IS NULL THEN
        RAISE EXCEPTION 'Нет свободного ID узла для проверки совпадения реестров';
    END IF;
    INSERT INTO net.pipe_section (id, fragment_id, node_from, node_to)
    VALUES (colliding_line, test_fragment, first_node, second_node)
    RETURNING row_version INTO colliding_line_version;

    SELECT new_version, copied_fields, object_is_node
      INTO result_version, copied, result_is_node
      FROM net.reclass_object(
          'connect_node', first_node, node_version, 'node_plain');
    IF result_version <> node_version + 1 OR copied <= 0
       OR NOT result_is_node THEN
        RAISE EXCEPTION 'Некорректный результат смены класса узла';
    END IF;
    IF NOT EXISTS (
        SELECT 1 FROM net.node_plain
         WHERE id = first_node AND ST_Equals(geom, original_node_geom))
       OR EXISTS (SELECT 1 FROM net.connect_node WHERE id = first_node)
       OR NOT EXISTS (
           SELECT 1 FROM net.node_reg
            WHERE id = first_node AND tbl = 'node_plain')
       OR NOT EXISTS (
           SELECT 1 FROM net.pipe_section
            WHERE id = test_line AND node_from = first_node) THEN
        RAISE EXCEPTION 'Смена класса узла нарушила данные или топологию';
    END IF;

    BEGIN
        PERFORM * FROM net.reclass_object(
            'node_plain', first_node, node_version, 'connect_node');
    EXCEPTION WHEN OTHERS THEN
        conflict_seen := SQLERRM LIKE '%CONFLICT:%';
    END;
    IF NOT conflict_seen THEN
        RAISE EXCEPTION 'Устаревшая версия узла не вызвала CONFLICT';
    END IF;

    SELECT new_version
      INTO line_version
      FROM net.reclass_object(
          'pipe_section', test_line, line_version, 'line_plain');
    IF NOT EXISTS (
        SELECT 1 FROM net.line_plain
         WHERE id = test_line
           AND node_from = first_node AND node_to = second_node
           AND ST_Equals(geom, original_line_geom))
       OR EXISTS (SELECT 1 FROM net.pipe_section WHERE id = test_line)
       OR NOT EXISTS (
           SELECT 1 FROM net.line_reg
            WHERE id = test_line AND tbl = 'line_plain') THEN
        RAISE EXCEPTION 'Смена класса линии нарушила данные или топологию';
    END IF;

    PERFORM * FROM net.reclass_object(
        'pipe_section', colliding_line, colliding_line_version, 'line_plain');
    IF NOT EXISTS (
        SELECT 1 FROM net.line_plain WHERE id = colliding_line)
       OR NOT EXISTS (
           SELECT 1 FROM net.node_reg WHERE id = colliding_line)
       OR NOT EXISTS (
           SELECT 1 FROM net.line_reg
            WHERE id = colliding_line AND tbl = 'line_plain') THEN
        RAISE EXCEPTION 'Совпадающий ID узла ошибочно определил тип линии';
    END IF;

    IF NOT EXISTS (
        SELECT 1 FROM meta.object_change_log
         WHERE schema_name = 'net'
           AND table_name IN ('node_plain', 'line_plain')
           AND object_id IN (first_node, test_line)
           AND operation = 'reclass'
           AND changed_fields ? 'class') THEN
        RAISE EXCEPTION 'Смена класса не записана в аудит';
    END IF;
END
$$;

ROLLBACK;
