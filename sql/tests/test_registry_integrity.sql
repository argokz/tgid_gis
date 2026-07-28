-- Проверка ядра модели: реестр наполняется триггерами, а внешний ключ
-- от линии к узлу реально запрещает удалить используемый узел.
\set ON_ERROR_STOP on

BEGIN;

INSERT INTO net.fragment (id, name) VALUES (999, 'test');

-- 1. Вставка узла должна автоматически создать запись в реестре.
INSERT INTO net.heat_chamber (id, fragment_id, geom)
VALUES (1001, 999, ST_SetSRID(ST_Point(-2426.11, -1036.23), 9998));

DO $$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM net.node_reg
                   WHERE id = 1001 AND kind = 'heat_chamber') THEN
        RAISE EXCEPTION 'ПРОВАЛ: триггер не создал запись в net.node_reg';
    END IF;
END $$;

INSERT INTO net.heat_chamber (id, fragment_id, geom)
VALUES (1002, 999, ST_SetSRID(ST_Point(-2431.62, -1078.37), 9998));

-- 2. Линия ссылается на реестр — вставка должна пройти.
INSERT INTO net.pipe_section (id, fragment_id, node_from, node_to, geom)
VALUES (2001, 999, 1001, 1002,
        ST_SetSRID(ST_MakeLine(ST_Point(-2426.11, -1036.23),
                               ST_Point(-2431.62, -1078.37)), 9998));

-- 3. Ссылка на несуществующий узел должна быть отвергнута.
DO $$
BEGIN
    BEGIN
        INSERT INTO net.pipe_section (id, fragment_id, node_from, node_to, geom)
        VALUES (2002, 999, 1001, 999999,
                ST_SetSRID(ST_MakeLine(ST_Point(0, 0), ST_Point(1, 1)), 9998));
        RAISE EXCEPTION 'ПРОВАЛ: принята линия с несуществующим узлом';
    EXCEPTION WHEN foreign_key_violation THEN
        NULL;  -- ожидаемо
    END;
END $$;

-- 4. Удаление узла, на котором висит линия, должно быть запрещено.
DO $$
BEGIN
    BEGIN
        DELETE FROM net.heat_chamber WHERE id = 1001;
        RAISE EXCEPTION 'ПРОВАЛ: удалён узел, используемый линией';
    EXCEPTION WHEN foreign_key_violation THEN
        NULL;  -- ожидаемо
    END;
END $$;

-- 5. После удаления линии узел удаляется, реестр чистится триггером.
DELETE FROM net.pipe_section WHERE id = 2001;
DELETE FROM net.heat_chamber WHERE id = 1001;

DO $$
BEGIN
    IF EXISTS (SELECT 1 FROM net.node_reg WHERE id = 1001) THEN
        RAISE EXCEPTION 'ПРОВАЛ: триггер не убрал запись из net.node_reg';
    END IF;
END $$;

-- 6. id выдаётся общей последовательностью — он уникален между таблицами.
INSERT INTO net.heat_chamber (fragment_id, geom)
VALUES (999, ST_SetSRID(ST_Point(0, 0), 9998));
INSERT INTO net.manhole (fragment_id, geom)
VALUES (999, ST_SetSRID(ST_Point(0, 0), 9998));

DO $$
DECLARE n int;
BEGIN
    SELECT count(*) INTO n FROM (
        SELECT id FROM net.heat_chamber
        UNION ALL SELECT id FROM net.manhole
    ) s;
    IF n <> (SELECT count(DISTINCT id) FROM (
                SELECT id FROM net.heat_chamber
                UNION ALL SELECT id FROM net.manhole) s2) THEN
        RAISE EXCEPTION 'ПРОВАЛ: id не уникален между объектными таблицами';
    END IF;
END $$;

ROLLBACK;

\echo 'ВСЕ ПРОВЕРКИ ЦЕЛОСТНОСТИ ПРОЙДЕНЫ'
