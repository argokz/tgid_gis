-- Чтение и запись старого pipeSections должны работать через один net.pipe_section.
BEGIN;

DO $$
DECLARE
    net_count bigint;
    compat_count bigint;
    missing_links bigint;
    test_id bigint;
    old_name text;
    test_name text := 'compat pipeSections test';
BEGIN
    SELECT count(*) INTO net_count FROM net.pipe_section;
    SELECT count(*) INTO compat_count FROM compat.pipesections;

    IF net_count <> compat_count THEN
        RAISE EXCEPTION 'pipesections count mismatch: net %, compat %', net_count, compat_count;
    END IF;

    SELECT count(*)
      INTO missing_links
      FROM compat.heatpipesections h
      JOIN compat.linesobj l ON l.id = h.lineid
 LEFT JOIN compat.pipesections p ON p.id = h.pipesectionid
     WHERE l.removed = 0
       AND p.id IS NULL;

    IF missing_links <> 0 THEN
        RAISE EXCEPTION 'heatpipesections without pipeSections passport: %', missing_links;
    END IF;

    SELECT id, name
      INTO test_id, old_name
      FROM compat.pipesections
     ORDER BY id
     LIMIT 1;

    IF test_id IS NULL THEN
        RAISE EXCEPTION 'compat.pipesections is empty';
    END IF;

    UPDATE compat.pipesections
       SET name = test_name,
           dokument_chertezh_objekta_kontrolya = 'test-document'
     WHERE id = test_id;

    IF NOT EXISTS (
        SELECT 1
          FROM net.pipe_section
         WHERE id = test_id
           AND name = test_name
           AND dokument_chertezh_objekta_kontrolya = 'test-document'
    ) THEN
        RAISE EXCEPTION 'write through compat.pipesections did not reach net.pipe_section';
    END IF;

    DELETE FROM compat.pipesections WHERE id = test_id;

    IF NOT EXISTS (SELECT 1 FROM net.pipe_section WHERE id = test_id) THEN
        RAISE EXCEPTION 'legacy passport DELETE removed canonical net.pipe_section';
    END IF;

    RAISE NOTICE 'compat.pipesections: % rows, read/write/safe delete OK', compat_count;
END;
$$;

ROLLBACK;
