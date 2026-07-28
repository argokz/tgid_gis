-- Проверка смены класса объекта.
--
-- Сценарий приложения: сначала создаётся строка в nodes (объект попадает
-- в node_plain), затем строка в таблице-подтипе — и только она задаёт тип.
-- Перенос обязан сохранить id и не разорвать ссылки линий на этот узел.
\set ON_ERROR_STOP on

BEGIN;

-- Берём узел, на который ссылается хотя бы одна линия: именно на нём
-- проверяется, что внешние ключи переживают перенос.
CREATE TEMP TABLE _t AS
SELECT p.node_from AS nid, p.id AS lid
FROM net.pipe_section p
JOIN net.node_reg r ON r.id = p.node_from
WHERE r.tbl = 'node_plain'
LIMIT 1;

DO $$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM _t) THEN
        RAISE EXCEPTION 'нет подходящего узла для проверки';
    END IF;
END $$;

-- Перенос node_plain -> consumer_real
SELECT net.reclass_node((SELECT nid FROM _t), 'consumer_real');

DO $$
DECLARE k text; n int;
BEGIN
    SELECT kind INTO k FROM net.node_reg WHERE id = (SELECT nid FROM _t);
    IF k <> 'consumer_real' THEN
        RAISE EXCEPTION 'ПРОВАЛ: реестр показывает % вместо consumer_real', k;
    END IF;

    SELECT count(*) INTO n FROM net.consumer_real WHERE id = (SELECT nid FROM _t);
    IF n <> 1 THEN
        RAISE EXCEPTION 'ПРОВАЛ: строка не появилась в consumer_real';
    END IF;

    SELECT count(*) INTO n FROM net.node_plain WHERE id = (SELECT nid FROM _t);
    IF n <> 0 THEN
        RAISE EXCEPTION 'ПРОВАЛ: строка осталась в node_plain';
    END IF;

    -- Главное: линия по-прежнему ссылается на этот узел.
    SELECT count(*) INTO n FROM net.pipe_section
    WHERE id = (SELECT lid FROM _t) AND node_from = (SELECT nid FROM _t);
    IF n <> 1 THEN
        RAISE EXCEPTION 'ПРОВАЛ: ссылка линии на узел потеряна';
    END IF;
END $$;

-- Геометрия должна пережить перенос без изменений.
DO $$
DECLARE n int;
BEGIN
    SELECT count(*) INTO n FROM net.consumer_real c
    JOIN public.nodes_legacy o ON o.id = c.id
    WHERE c.id = (SELECT nid FROM _t)
      AND abs(ST_X(c.geom) - o.x / 100.0) < 0.001
      AND abs(ST_Y(c.geom) + o.y / 100.0) < 0.001;
    IF n <> 1 THEN
        RAISE EXCEPTION 'ПРОВАЛ: геометрия изменилась при переносе';
    END IF;
END $$;

-- Обратный перенос должен работать так же.
SELECT net.reclass_node((SELECT nid FROM _t), 'node_plain');

DO $$
DECLARE k text; n int;
BEGIN
    SELECT kind INTO k FROM net.node_reg WHERE id = (SELECT nid FROM _t);
    IF k <> 'node_plain' THEN
        RAISE EXCEPTION 'ПРОВАЛ: обратный перенос не выполнен, класс %', k;
    END IF;
    SELECT count(*) INTO n FROM net.pipe_section
    WHERE id = (SELECT lid FROM _t) AND node_from = (SELECT nid FROM _t);
    IF n <> 1 THEN
        RAISE EXCEPTION 'ПРОВАЛ: ссылка линии потеряна при обратном переносе';
    END IF;
END $$;

-- Реестр обязан остаться согласованным.
DO $$
DECLARE r bigint; t bigint;
BEGIN
    SELECT count(*) INTO r FROM net.node_reg;
    SELECT count(*) INTO t FROM net.v_nodes_geom;
    IF r <> t THEN
        RAISE EXCEPTION 'ПРОВАЛ: реестр % против таблиц %', r, t;
    END IF;
END $$;

ROLLBACK;

\echo 'СМЕНА КЛАССА РАБОТАЕТ: id, геометрия и ссылки линий сохраняются'
