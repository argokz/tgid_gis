-- Проверка пути записи через каноническое представление net.v_nodes.
-- Представления public снимаются после полного перехода, поэтому тест
-- проверяет тот же INSTEAD OF-механизм непосредственно в схеме net.
\set ON_ERROR_STOP on

BEGIN;

-- 1. Перемещение узла: приложение пишет x/y, геометрия обязана пересчитаться.
CREATE TEMP TABLE _n AS
SELECT node.id, node.x, node.y
  FROM net.v_nodes node
 WHERE node.removed = 0
   AND NOT EXISTS (
       SELECT 1
         FROM net.v_linesobj line
        WHERE line.removed = 0
          AND (line.nodeid1 = node.id OR line.nodeid2 = node.id)
   )
 ORDER BY node.id
 LIMIT 1;

UPDATE net.v_nodes
SET x = (SELECT x FROM _n) + 5000,
    y = (SELECT y FROM _n) + 7000
WHERE id = (SELECT id FROM _n);

DO $$
DECLARE gx double precision; gy double precision; ex double precision; ey double precision;
BEGIN
    SELECT ST_X(g.geom), ST_Y(g.geom) INTO gx, gy
    FROM net.v_nodes_geom g WHERE g.id = (SELECT id FROM _n);

    SELECT (x + 5000) / 100.0, -(y + 7000) / 100.0 INTO ex, ey FROM _n;

    IF abs(gx - ex) > 0.001 OR abs(gy - ey) > 0.001 THEN
        RAISE EXCEPTION 'ПРОВАЛ: геометрия не пересчиталась (% %) вместо (% %)',
              gx, gy, ex, ey;
    END IF;
END $$;

-- Обратное чтение через представление должно вернуть то же самое.
DO $$
DECLARE vx double precision; ex double precision;
BEGIN
    SELECT n.x INTO vx FROM net.v_nodes n WHERE n.id = (SELECT id FROM _n);
    SELECT x + 5000 INTO ex FROM _n;
    IF abs(vx - ex) > 0.01 THEN
        RAISE EXCEPTION 'ПРОВАЛ: чтение вернуло x=% вместо %', vx, ex;
    END IF;
END $$;

-- 2. Удаление так, как это делает приложение: removed=1, а не DELETE.
UPDATE net.v_nodes SET removed = 1 WHERE id = (SELECT id FROM _n);

DO $$
DECLARE n int;
BEGIN
    SELECT count(*) INTO n FROM net.v_nodes
    WHERE id = (SELECT id FROM _n) AND removed = 1;
    IF n <> 1 THEN
        RAISE EXCEPTION 'ПРОВАЛ: пометка removed=1 не сохранилась';
    END IF;
END $$;

-- 3. Восстановление: приложение снимает пометку (cxema/undo_gid.cpp:262).
UPDATE net.v_nodes SET removed = 0 WHERE id = (SELECT id FROM _n);

DO $$
DECLARE n int;
BEGIN
    SELECT count(*) INTO n FROM net.v_nodes
    WHERE id = (SELECT id FROM _n) AND removed = 0;
    IF n <> 1 THEN
        RAISE EXCEPTION 'ПРОВАЛ: снятие пометки removed не сработало';
    END IF;
END $$;

-- 4. Создание нового узла через представление.
INSERT INTO net.v_nodes (fileid, externalnodename, x, y, externalsignid, removed)
VALUES ((SELECT min(id) FROM net.fragment), 'проверочный узел',
        -242611, 103623, 1, 0);

DO $$
DECLARE nid bigint; k text;
BEGIN
    SELECT id INTO nid FROM net.v_nodes
    WHERE externalnodename = 'проверочный узел';
    IF nid IS NULL THEN
        RAISE EXCEPTION 'ПРОВАЛ: вставка не создала узел';
    END IF;

    SELECT kind INTO k FROM net.node_reg WHERE id = nid;
    IF k <> 'node_plain' THEN
        RAISE EXCEPTION 'ПРОВАЛ: новый узел попал в % вместо node_plain', k;
    END IF;

    IF NOT EXISTS (SELECT 1 FROM net.node_plain
                   WHERE id = nid
                     AND abs(ST_X(geom) - (-2426.11)) < 0.001
                     AND abs(ST_Y(geom) - (-1036.23)) < 0.001) THEN
        RAISE EXCEPTION 'ПРОВАЛ: геометрия нового узла построена неверно';
    END IF;
END $$;

-- 5. Реестр обязан остаться согласованным с объектными таблицами.
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

\echo 'ПУТЬ ЗАПИСИ РАБОТАЕТ: перемещение, удаление, восстановление, вставка'
