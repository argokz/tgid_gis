-- Проверка новых классов оборудования: объект обязан попадать в реестр
-- линий и уходить из него при удалении. Пустая таблица со всеми
-- триггерами бесполезна, если триггеры не срабатывают.
--
-- Всё внутри транзакции с откатом: тест не оставляет следов.
--
--   psql -d tgid_gis -v ON_ERROR_STOP=1 -f sql/tests/test_itp_classes.sql

BEGIN;

DO $$
DECLARE
    cls        text;
    frag       int;
    n_from     bigint;
    n_to       bigint;
    new_id     bigint;
    in_reg     int;
    checked    int := 0;
BEGIN
    SELECT id INTO frag FROM net.fragment ORDER BY id LIMIT 1;
    IF frag IS NULL THEN
        RAISE EXCEPTION 'нет ни одного фрагмента — тест невозможен';
    END IF;

    SELECT id INTO n_from FROM net.node_reg
    WHERE NOT removed AND fragment_id = frag ORDER BY id LIMIT 1;
    SELECT id INTO n_to FROM net.node_reg
    WHERE NOT removed AND fragment_id = frag AND id <> n_from
    ORDER BY id LIMIT 1;

    IF n_from IS NULL OR n_to IS NULL THEN
        RAISE EXCEPTION 'во фрагменте % меньше двух узлов', frag;
    END IF;

    FOREACH cls IN ARRAY ARRAY['bypass', 'valve_reverse', 'armature_control',
                               'regulator_consumption', 'regulator_pressdrop']
    LOOP
        -- id не задаём: он обязан прийти из DEFAULT nextval('obj_id_seq'),
        -- как у остальных классов. Если умолчания нет, вставка упадёт —
        -- это и проверяется.
        EXECUTE format(
            'INSERT INTO net.%I (fragment_id, node_from, node_to)'
            ' VALUES ($1, $2, $3) RETURNING id', cls)
        INTO new_id USING frag, n_from, n_to;

        SELECT count(*) INTO in_reg FROM net.line_reg WHERE id = new_id;
        IF in_reg <> 1 THEN
            RAISE EXCEPTION 'net.% : после вставки в line_reg % строк, ждали 1',
                            cls, in_reg;
        END IF;

        EXECUTE format('DELETE FROM net.%I WHERE id = $1', cls) USING new_id;

        SELECT count(*) INTO in_reg FROM net.line_reg WHERE id = new_id;
        IF in_reg <> 0 THEN
            RAISE EXCEPTION 'net.% : после удаления в line_reg осталось % строк',
                            cls, in_reg;
        END IF;

        checked := checked + 1;
    END LOOP;

    RAISE NOTICE 'классов проверено: % — вставка и удаление синхронны '
                 'с net.line_reg', checked;
END $$;

ROLLBACK;
