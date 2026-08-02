-- Запись старого плоского fragments должна раскладываться в новую модель.
-- Тест полностью откатывается и не оставляет данных.
\set ON_ERROR_STOP on

BEGIN;

DO $do$
DECLARE
    new_fragment_id integer;
    child_count integer;
BEGIN
    INSERT INTO compat.fragments (name, tvn_1, tn_long_12)
    VALUES ('__gid8_compat_write_test__', 17.25, -31.5)
    RETURNING id INTO new_fragment_id;

    IF new_fragment_id IS NULL THEN
        RAISE EXCEPTION 'INSERT не вернул id фрагмента';
    END IF;

    SELECT count(*) INTO child_count
    FROM net.fragment_system WHERE fragment_id = new_fragment_id;
    IF child_count <> 5 THEN
        RAISE EXCEPTION 'ожидалось 5 систем, получено %', child_count;
    END IF;

    SELECT count(*) INTO child_count
    FROM net.fragment_month WHERE fragment_id = new_fragment_id;
    IF child_count <> 12 THEN
        RAISE EXCEPTION 'ожидалось 12 месяцев, получено %', child_count;
    END IF;

    UPDATE compat.fragments
    SET name = '__gid8_compat_updated__', tvn_1 = 18.5, tn_long_12 = -32.75
    WHERE id = new_fragment_id;

    IF NOT EXISTS (
        SELECT 1
        FROM compat.fragments
        WHERE id = new_fragment_id
          AND name = '__gid8_compat_updated__'
          AND tvn_1 = 18.5
          AND tn_long_12 = -32.75
    ) THEN
        RAISE EXCEPTION 'UPDATE плоского представления не сохранился';
    END IF;

    DELETE FROM compat.fragments WHERE id = new_fragment_id;

    IF NOT EXISTS (
        SELECT 1 FROM net.fragment
        WHERE id = new_fragment_id AND removed_at IS NOT NULL
    ) THEN
        RAISE EXCEPTION 'DELETE не выполнил мягкое удаление';
    END IF;
END $do$;

ROLLBACK;
