-- Проверка ядра модели: реестр наполняется триггерами, а внешний ключ
-- от линии к узлу реально запрещает удалить используемый узел.
\set ON_ERROR_STOP on

BEGIN;

-- Идентификаторы берём из последовательности, а не константами:
-- id объектов сохраняются из public, поэтому 1001 и 2001 в рабочей БД
-- уже заняты, и тест с константами падал на нарушении уникальности.
CREATE TEMP TABLE _ids AS
SELECT nextval('net.obj_id_seq') AS n1,
       nextval('net.obj_id_seq') AS n2,
       nextval('net.obj_id_seq') AS n3,
       nextval('net.obj_id_seq') AS l1,
       (SELECT coalesce(max(id), 0) + 1000 FROM net.fragment) AS frag;

INSERT INTO net.fragment (id, name)
SELECT frag, 'проверка целостности' FROM _ids;

-- 1. Вставка узла должна автоматически создать запись в реестре.
INSERT INTO net.heat_chamber (id, fragment_id, geom)
VALUES ((SELECT n1 FROM _ids), (SELECT frag FROM _ids), ST_SetSRID(ST_Point(-2426.11, -1036.23), 9998));

DO $$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM net.node_reg
                   WHERE id = (SELECT n1 FROM _ids) AND kind = 'heat_chamber') THEN
        RAISE EXCEPTION 'ПРОВАЛ: триггер не создал запись в net.node_reg';
    END IF;
END $$;

INSERT INTO net.heat_chamber (id, fragment_id, geom)
VALUES ((SELECT n2 FROM _ids), (SELECT frag FROM _ids), ST_SetSRID(ST_Point(-2431.62, -1078.37), 9998));

-- 2. Линия ссылается на реестр — вставка должна пройти.
INSERT INTO net.pipe_section (id, fragment_id, node_from, node_to, geom)
VALUES ((SELECT l1 FROM _ids), (SELECT frag FROM _ids), (SELECT n1 FROM _ids), (SELECT n2 FROM _ids),
        ST_SetSRID(ST_MakeLine(ST_Point(-2426.11, -1036.23),
                               ST_Point(-2431.62, -1078.37)), 9998));

-- 3. Ссылка на несуществующий узел должна быть отвергнута.
DO $$
DECLARE rejected boolean := false;
BEGIN
    BEGIN
        INSERT INTO net.pipe_section (id, fragment_id, node_from, node_to, geom)
        VALUES ((SELECT l1 FROM _ids) + 500000, (SELECT frag FROM _ids), (SELECT n1 FROM _ids), 999999999,
                ST_SetSRID(ST_MakeLine(ST_Point(0, 0), ST_Point(1, 1)), 9998));
    EXCEPTION WHEN foreign_key_violation OR raise_exception THEN
        rejected := true;  -- FK либо более ранняя проверка топологии
    END;
    IF NOT rejected THEN
        RAISE EXCEPTION 'ПРОВАЛ: принята линия с несуществующим узлом';
    END IF;
END $$;

-- 4. Удаление узла, на котором висит линия, должно быть запрещено.
DO $$
DECLARE rejected boolean := false;
BEGIN
    BEGIN
        DELETE FROM net.heat_chamber WHERE id = (SELECT n1 FROM _ids);
    EXCEPTION WHEN foreign_key_violation OR raise_exception THEN
        rejected := true;  -- FK либо защитный триггер архива/топологии
    END;
    IF NOT rejected THEN
        RAISE EXCEPTION 'ПРОВАЛ: удалён узел, используемый линией';
    END IF;
END $$;

-- 5. После удаления линии узел удаляется, реестр чистится триггером.
DELETE FROM net.pipe_section WHERE id = (SELECT l1 FROM _ids);
DELETE FROM net.heat_chamber WHERE id = (SELECT n1 FROM _ids);

DO $$
BEGIN
    IF EXISTS (SELECT 1 FROM net.node_reg WHERE id = (SELECT n1 FROM _ids)) THEN
        RAISE EXCEPTION 'ПРОВАЛ: триггер не убрал запись из net.node_reg';
    END IF;
END $$;

-- 6. id выдаётся общей последовательностью — он уникален между таблицами.
INSERT INTO net.heat_chamber (fragment_id, geom)
SELECT frag, ST_SetSRID(ST_Point(0, 0), 9998) FROM _ids;
INSERT INTO net.consumer_real (fragment_id, geom)
SELECT frag, ST_SetSRID(ST_Point(0, 0), 9998) FROM _ids;

DO $$
DECLARE n int; d int;
BEGIN
    SELECT count(*), count(DISTINCT id) INTO n, d FROM (
        SELECT id FROM net.heat_chamber
        UNION ALL SELECT id FROM net.consumer_real
    ) s;
    IF n <> d THEN
        RAISE EXCEPTION 'ПРОВАЛ: id не уникален между объектными таблицами';
    END IF;
END $$;

-- 7. Аспект привязан к реестру: узел-потребитель может быть ещё и узлом
--    задания давления, но ссылка на несуществующий узел недопустима.
INSERT INTO net.consumer_general (id, fragment_id, geom)
VALUES ((SELECT n3 FROM _ids), (SELECT frag FROM _ids), ST_SetSRID(ST_Point(1, 1), 9998));
INSERT INTO net.node_press_setting (node_id) VALUES ((SELECT n3 FROM _ids));

DO $$
BEGIN
    BEGIN
        INSERT INTO net.node_press_setting (node_id) VALUES (999999998);
        RAISE EXCEPTION 'ПРОВАЛ: аспект принят для несуществующего узла';
    EXCEPTION WHEN foreign_key_violation THEN
        NULL;  -- ожидаемо
    END;
END $$;

ROLLBACK;

\echo 'ВСЕ ПРОВЕРКИ ЦЕЛОСТНОСТИ ПРОЙДЕНЫ'
