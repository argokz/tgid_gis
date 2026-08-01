-- Интеграционная проверка net.move_node. Все тестовые данные откатываются.

\set ON_ERROR_STOP on

BEGIN;

SET LOCAL application_name = 'tgid_test_node_move';

DO $$
DECLARE
    test_fragment integer;
    first_node bigint;
    second_node bigint;
    test_line bigint;
    node_version bigint;
    line_version bigint;
    moved_version bigint;
    connected integer;
    old_x double precision := 700000.0;
    old_y double precision := 4800000.0;
    new_x double precision := 700012.5;
    new_y double precision := 4800007.25;
    actual_point geometry;
    actual_line geometry;
    conflict_seen boolean := false;
BEGIN
    SELECT id INTO test_fragment
      FROM net.fragment
     WHERE removed_at IS NULL
     ORDER BY id
     LIMIT 1;
    IF test_fragment IS NULL THEN
        RAISE EXCEPTION 'Нет активного фрагмента для теста';
    END IF;

    INSERT INTO net.connect_node (fragment_id, geom, memo)
    VALUES (
        test_fragment,
        ST_SetSRID(ST_MakePoint(old_x, old_y), 9998),
        'test_node_move:first')
    RETURNING id, row_version INTO first_node, node_version;

    INSERT INTO net.connect_node (fragment_id, geom, memo)
    VALUES (
        test_fragment,
        ST_SetSRID(ST_MakePoint(old_x + 100.0, old_y), 9998),
        'test_node_move:second')
    RETURNING id INTO second_node;

    INSERT INTO net.pipe_section (fragment_id, node_from, node_to)
    VALUES (test_fragment, first_node, second_node)
    RETURNING id, row_version INTO test_line, line_version;

    SELECT new_version, connected_lines
      INTO moved_version, connected
      FROM net.move_node(
          'connect_node', first_node, node_version, new_x, new_y);

    IF moved_version <> node_version + 1 THEN
        RAISE EXCEPTION
            'Версия узла не увеличилась: было %, стало %',
            node_version, moved_version;
    END IF;
    IF connected <> 1 THEN
        RAISE EXCEPTION 'Ожидалась одна подключённая линия, получено %', connected;
    END IF;

    SELECT geom INTO actual_point
      FROM net.connect_node WHERE id = first_node;
    IF NOT ST_Equals(
        actual_point, ST_SetSRID(ST_MakePoint(new_x, new_y), 9998)) THEN
        RAISE EXCEPTION 'Координаты узла не обновились';
    END IF;

    SELECT geom, row_version INTO actual_line, line_version
      FROM net.pipe_section WHERE id = test_line;
    IF NOT ST_Equals(
        ST_StartPoint(actual_line),
        ST_SetSRID(ST_MakePoint(new_x, new_y), 9998)) THEN
        RAISE EXCEPTION 'Начало подключённой линии не перестроено';
    END IF;
    IF line_version <= 1 THEN
        RAISE EXCEPTION 'Версия подключённой линии не увеличилась';
    END IF;

    BEGIN
        PERFORM * FROM net.move_node(
            'connect_node', first_node, node_version, new_x + 1.0, new_y);
    EXCEPTION WHEN OTHERS THEN
        conflict_seen := SQLERRM LIKE '%CONFLICT:%';
    END;
    IF NOT conflict_seen THEN
        RAISE EXCEPTION 'Устаревшая версия узла не вызвала CONFLICT';
    END IF;

    IF NOT EXISTS (
        SELECT 1 FROM meta.object_change_log
         WHERE schema_name = 'net'
           AND table_name = 'connect_node'
           AND object_id = first_node
           AND operation = 'update'
           AND changed_fields ? 'geom') THEN
        RAISE EXCEPTION 'Перемещение узла не записано в аудит';
    END IF;
END
$$;

ROLLBACK;
