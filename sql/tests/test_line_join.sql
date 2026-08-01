\set ON_ERROR_STOP on

BEGIN;

DO $$
DECLARE
    test_fragment integer;
    node_left bigint;
    node_middle bigint;
    node_right bigint;
    line_left bigint;
    line_right bigint;
    join_result record;
    joined_length double precision;
    joined_geom_length double precision;
    archived_sources integer;
    middle_archived boolean;
BEGIN
    SELECT id INTO test_fragment
      FROM net.fragment
     WHERE removed_at IS NULL
     ORDER BY id
     LIMIT 1;
    IF test_fragment IS NULL THEN
        RAISE EXCEPTION 'Для теста нужен активный фрагмент';
    END IF;

    INSERT INTO net.connect_node (fragment_id, geom)
    VALUES (test_fragment, ST_SetSRID(ST_MakePoint(2000, 2000), 9998))
    RETURNING id INTO node_left;
    INSERT INTO net.connect_node (fragment_id, geom)
    VALUES (test_fragment, ST_SetSRID(ST_MakePoint(2040, 2000), 9998))
    RETURNING id INTO node_middle;
    INSERT INTO net.connect_node (fragment_id, geom)
    VALUES (test_fragment, ST_SetSRID(ST_MakePoint(2100, 2000), 9998))
    RETURNING id INTO node_right;

    INSERT INTO net.pipe_section (
        fragment_id, node_from, node_to, pipesectlength, location
    ) VALUES (test_fragment, node_left, node_middle, 40, 'join-test')
    RETURNING id INTO line_left;
    INSERT INTO net.pipe_section (
        fragment_id, node_from, node_to, pipesectlength, location
    ) VALUES (test_fragment, node_middle, node_right, 60, 'join-test')
    RETURNING id INTO line_right;

    BEGIN
        PERFORM * FROM net.join_lines(
            'pipe_section', line_left, 2, line_right, 1
        );
        RAISE EXCEPTION 'Ожидался конфликт row_version';
    EXCEPTION
        WHEN OTHERS THEN
            IF position('CONFLICT:' IN SQLERRM) = 0 THEN
                RAISE;
            END IF;
    END;

    SELECT * INTO join_result
      FROM net.join_lines(
          'pipe_section', line_left, 1, line_right, 1
      );

    SELECT pipesectlength, ST_Length(geom)
      INTO joined_length, joined_geom_length
      FROM net.pipe_section
     WHERE id = join_result.joined_line_id
       AND node_from = node_left
       AND node_to = node_right
       AND removed_at IS NULL;
    IF joined_length IS NULL OR abs(joined_length - 100) > 0.000001
       OR abs(joined_geom_length - 100) > 0.000001 THEN
        RAISE EXCEPTION
            'Неверная объединённая длина: атрибут %, геометрия %',
            joined_length, joined_geom_length;
    END IF;

    SELECT count(*) INTO archived_sources
      FROM net.pipe_section
     WHERE id IN (line_left, line_right)
       AND removed_at IS NOT NULL;
    IF archived_sources <> 2 THEN
        RAISE EXCEPTION 'Исходные участки не архивированы';
    END IF;
    SELECT removed_at IS NOT NULL INTO middle_archived
      FROM net.connect_node WHERE id = node_middle;
    IF NOT coalesce(middle_archived, false)
       OR join_result.archived_node_id <> node_middle THEN
        RAISE EXCEPTION 'Общий узел не архивирован';
    END IF;
END
$$;

ROLLBACK;
