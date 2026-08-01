-- Проверка механизма «скопировано, но помечено».
--
-- Смысл: пока решение по спорному объекту не принято, обе версии лежат
-- в базе, объект помечен, а выбор конвертера можно переиграть одной
-- командой без пересборки схемы.
\set ON_ERROR_STOP on

BEGIN;

CREATE TEMP TABLE _t AS
SELECT v.obj_id,
       max(v.src_id) FILTER (WHERE v.chosen)     AS id_vybran,
       max(v.src_id) FILTER (WHERE NOT v.chosen) AS id_otvergnut,
       max(v.target)                             AS target
FROM net.object_variant v
WHERE v.obj_kind = 'node' AND v.src_table = 'generalizedconsumers'
GROUP BY v.obj_id
HAVING count(*) = 2
   AND count(*) FILTER (WHERE v.chosen) = 1
LIMIT 1;

-- 1. Обе версии на месте, объект помечен.
DO $$
DECLARE n int;
BEGIN
    SELECT count(*) INTO n FROM net.object_variant
    WHERE obj_id = (SELECT obj_id FROM _t);
    IF n <> 2 THEN
        RAISE EXCEPTION 'ПРОВАЛ: сохранено % версий вместо 2', n;
    END IF;

    IF NOT (SELECT needs_review FROM net.node_reg
            WHERE id = (SELECT obj_id FROM _t)) THEN
        RAISE EXCEPTION 'ПРОВАЛ: объект не помечен как спорный';
    END IF;
END $$;

-- 2. Значение в объекте совпадает с выбранной версией, а не с отвергнутой.
DO $$
DECLARE v_obj text; v_sel text; v_rej text;
BEGIN
    SELECT c.calchldep::text INTO v_obj
    FROM net.consumer_general c WHERE c.id = (SELECT obj_id FROM _t);

    SELECT payload ->> 'calchldep' INTO v_sel FROM net.object_variant
    WHERE obj_id = (SELECT obj_id FROM _t) AND chosen;
    SELECT payload ->> 'calchldep' INTO v_rej FROM net.object_variant
    WHERE obj_id = (SELECT obj_id FROM _t) AND NOT chosen;

    IF v_obj IS DISTINCT FROM v_sel THEN
        RAISE EXCEPTION 'ПРОВАЛ: в объекте %, в выбранной версии %', v_obj, v_sel;
    END IF;
    RAISE NOTICE 'выбрано %, отвергнуто %', v_sel, v_rej;
END $$;

-- 3. Переигрываем решение: принимаем отвергнутую версию.
SELECT net.apply_variant((SELECT id_otvergnut FROM _t), 'generalizedconsumers');

DO $$
DECLARE v_obj text; v_rej text;
BEGIN
    SELECT c.calchldep::text INTO v_obj
    FROM net.consumer_general c WHERE c.id = (SELECT obj_id FROM _t);
    SELECT payload ->> 'calchldep' INTO v_rej FROM net.object_variant
    WHERE src_id = (SELECT id_otvergnut FROM _t);

    IF v_obj IS DISTINCT FROM v_rej THEN
        RAISE EXCEPTION 'ПРОВАЛ: после принятия варианта в объекте %, ожидалось %',
              v_obj, v_rej;
    END IF;

    IF (SELECT needs_review FROM net.node_reg
        WHERE id = (SELECT obj_id FROM _t)) THEN
        RAISE EXCEPTION 'ПРОВАЛ: пометка не снята после принятия решения';
    END IF;

    IF (SELECT count(*) FROM net.object_variant
        WHERE obj_id = (SELECT obj_id FROM _t) AND chosen) <> 1 THEN
        RAISE EXCEPTION 'ПРОВАЛ: выбранной должна остаться ровно одна версия';
    END IF;
END $$;

-- 4. Геометрия и id при этом не менялись.
DO $$
DECLARE n int;
BEGIN
    SELECT count(*) INTO n FROM net.consumer_general c
    JOIN attic.nodes_legacy o ON o.id = c.src_id
    WHERE c.id = (SELECT obj_id FROM _t)
      AND abs(ST_X(c.geom) - o.x / 100.0) < 0.001
      AND abs(ST_Y(c.geom) + o.y / 100.0) < 0.001;
    IF n <> 1 THEN
        RAISE EXCEPTION 'ПРОВАЛ: геометрия изменилась';
    END IF;
END $$;

ROLLBACK;

\echo 'ПОМЕТКА И ПЕРЕИГРОВКА РЕШЕНИЯ РАБОТАЮТ'
