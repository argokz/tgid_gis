\set ON_ERROR_STOP on

BEGIN;

DO $$
DECLARE
    fragment_id integer;
    test_node_from bigint;
    test_node_to bigint;
    source_line bigint;
    split_result record;
    original_archived boolean;
    first_length double precision;
    second_length double precision;
BEGIN
    SELECT id INTO fragment_id
      FROM net.fragment
     WHERE removed_at IS NULL
     ORDER BY id
     LIMIT 1;
    IF fragment_id IS NULL THEN
        RAISE EXCEPTION 'Для теста нужен активный фрагмент';
    END IF;

    INSERT INTO net.connect_node (fragment_id, geom)
    VALUES (
        fragment_id,
        ST_SetSRID(ST_MakePoint(1000.0, 1000.0), 9998)
    ) RETURNING id INTO test_node_from;
    INSERT INTO net.connect_node (fragment_id, geom)
    VALUES (
        fragment_id,
        ST_SetSRID(ST_MakePoint(1100.0, 1000.0), 9998)
    ) RETURNING id INTO test_node_to;

    INSERT INTO net.pipe_section (
        fragment_id, node_from, node_to, pipesectlength
    )
    VALUES (fragment_id, test_node_from, test_node_to, 100.0)
    RETURNING id INTO source_line;

    BEGIN
        PERFORM * FROM net.split_line(
            'pipe_section', source_line, 2, 1050.0, 1000.0
        );
        RAISE EXCEPTION 'Ожидался конфликт row_version';
    EXCEPTION
        WHEN OTHERS THEN
            IF position('CONFLICT:' IN SQLERRM) = 0 THEN
                RAISE;
            END IF;
    END;

    SELECT * INTO split_result
      FROM net.split_line(
          'pipe_section', source_line, 1, 1050.0, 1000.0
      );

    SELECT removed_at IS NOT NULL
      INTO original_archived
      FROM net.pipe_section
     WHERE id = source_line;
    IF NOT coalesce(original_archived, false) THEN
        RAISE EXCEPTION 'Исходная линия не архивирована';
    END IF;
    IF split_result.node_id IS NULL
       OR NOT split_result.node_created
       OR split_result.first_line_id IS NULL
       OR split_result.second_line_id IS NULL THEN
        RAISE EXCEPTION 'Функция не вернула созданные идентификаторы';
    END IF;

    SELECT pipesectlength INTO first_length
     FROM net.pipe_section
     WHERE id = split_result.first_line_id
       AND node_from = test_node_from
       AND node_to = split_result.node_id
       AND removed_at IS NULL;
    SELECT pipesectlength INTO second_length
     FROM net.pipe_section
     WHERE id = split_result.second_line_id
       AND node_from = split_result.node_id
       AND node_to = test_node_to
       AND removed_at IS NULL;
    IF first_length IS NULL OR second_length IS NULL
       OR abs(first_length - 50.0) > 0.000001
       OR abs(second_length - 50.0) > 0.000001 THEN
        RAISE EXCEPTION
            'Длина разделена неверно: % и %', first_length, second_length;
    END IF;
    IF abs(split_result.split_fraction - 0.5) > 0.000001 THEN
        RAISE EXCEPTION
            'Неверная доля разрезания: %', split_result.split_fraction;
    END IF;
END
$$;

ROLLBACK;
