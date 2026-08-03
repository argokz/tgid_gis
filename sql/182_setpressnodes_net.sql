-- Устранение раздвоения setPressNodes.
--
-- Найдено реестром меню (tools/menu_audit.py), а не инвентарём: в БД
-- одновременно жили ДВА источника уставок давления —
--   public.setpressnodes      5740 строк, настоящая таблица;
--   net.node_press_setting    5733 строки, целевая модель.
-- Какой из них увидит потребитель, зависело от того, чей search_path
-- применён: у gid8 первым идёт compat -> он читал net; у sety, potr5 и
-- инструментов схемы compat в пути нет -> они читали public. То есть
-- уставка, сохранённая из программы, движку расчёта была не видна.
-- Это не «лишняя таблица», а расхождение входных данных расчёта.
--
-- Разница в 7 строк — сироты: их nodeid (526291, 543113, 551822,
-- 559097, 566279, 574620, 582642) не существует ни в одном узле net,
-- поэтому конвертер их и не перенёс. Соединение с узлами отсекает их в
-- обоих вариантах: выборка sety до и после — 5561 строка, суммы
-- pressFlow/pressRet совпадают до последнего знака. Перенос безопасен
-- не по рассуждению, а по замеру.
--
-- Семантика представления НЕ меняется: как и в compat, «id» — это
-- src_id. Соблазнительный COALESCE(src_id, id), применённый в 179 для
-- wdodevices, здесь дал бы коллизии: id тут обычный serial 1..5733, а
-- src_id доходит до 7825, диапазоны пересекаются. Строки, вставленные
-- напрямую в net (например из QGIS) без src_id, представление по-
-- прежнему не покажет — это известное свойство, отмеченное в docs, а
-- не то, что чинится подменой ключа.

BEGIN;

-- 1. Каноническое представление в net — там, где его достанет любой
--    потребитель: схема net есть в search_path и БД, и клиента.
CREATE OR REPLACE VIEW net.v_setpressnodes AS
SELECT o.src_id            AS id,
       o.node_id           AS nodeid,
       o.pressflow,
       o.pressret,
       o.fragment_resultid,
       o.file_result,
       o.kod_m,
       o.uzel_m,
       o.fileid
FROM net.node_press_setting o
WHERE o.src_id IS NOT NULL;

-- 2. Запись. Представление простое и было бы автообновляемым, но
--    триггеры ставим явно по образцу остальных net.v_* (три штуки):
--    единый способ записи важнее экономии на трёх функциях, иначе
--    следующий читающий код не поймёт, почему одно представление
--    пишется само, а соседнее через триггер.
CREATE OR REPLACE FUNCTION net.v_setpressnodes_ins() RETURNS trigger
LANGUAGE plpgsql AS $$
BEGIN
    INSERT INTO net.node_press_setting
        (node_id, src_id, pressflow, pressret, fragment_resultid,
         file_result, kod_m, uzel_m, fileid)
    VALUES (NEW.nodeid, NEW.id, NEW.pressflow, NEW.pressret,
            NEW.fragment_resultid, NEW.file_result, NEW.kod_m,
            NEW.uzel_m, NEW.fileid);
    RETURN NEW;
END $$;

CREATE OR REPLACE FUNCTION net.v_setpressnodes_upd() RETURNS trigger
LANGUAGE plpgsql AS $$
BEGIN
    UPDATE net.node_press_setting
       SET node_id           = NEW.nodeid,
           pressflow         = NEW.pressflow,
           pressret          = NEW.pressret,
           fragment_resultid = NEW.fragment_resultid,
           file_result       = NEW.file_result,
           kod_m             = NEW.kod_m,
           uzel_m            = NEW.uzel_m,
           fileid            = NEW.fileid
     WHERE src_id = OLD.id;
    RETURN NEW;
END $$;

CREATE OR REPLACE FUNCTION net.v_setpressnodes_del() RETURNS trigger
LANGUAGE plpgsql AS $$
BEGIN
    DELETE FROM net.node_press_setting WHERE src_id = OLD.id;
    RETURN OLD;
END $$;

DROP TRIGGER IF EXISTS v_setpressnodes_ins ON net.v_setpressnodes;
CREATE TRIGGER v_setpressnodes_ins INSTEAD OF INSERT ON net.v_setpressnodes
    FOR EACH ROW EXECUTE FUNCTION net.v_setpressnodes_ins();
DROP TRIGGER IF EXISTS v_setpressnodes_upd ON net.v_setpressnodes;
CREATE TRIGGER v_setpressnodes_upd INSTEAD OF UPDATE ON net.v_setpressnodes
    FOR EACH ROW EXECUTE FUNCTION net.v_setpressnodes_upd();
DROP TRIGGER IF EXISTS v_setpressnodes_del ON net.v_setpressnodes;
CREATE TRIGGER v_setpressnodes_del INSTEAD OF DELETE ON net.v_setpressnodes
    FOR EACH ROW EXECUTE FUNCTION net.v_setpressnodes_del();

-- 3. compat становится тонкой обёрткой, как и все остальные его
--    представления (проверено: compat.heatpipesections/linesobj/nodes —
--    ровно SELECT ... FROM net.v_*). Одно определение, одно место правки.
DROP VIEW IF EXISTS compat.setpressnodes;
CREATE VIEW compat.setpressnodes AS SELECT * FROM net.v_setpressnodes;

COMMIT;

-- 4. Источник убирается на чердак, а не удаляется: правило проекта —
--    изъятое сохраняем. После этого имя setpressnodes больше не
--    резолвится в public ни у кого, и «тихого» чтения устаревшей копии
--    возникнуть не может.
BEGIN;
ALTER TABLE IF EXISTS public.setpressnodes SET SCHEMA attic;
COMMIT;

SELECT (SELECT count(*) FROM net.v_setpressnodes)  AS v_setpressnodes,
       (SELECT count(*) FROM compat.setpressnodes) AS compat_setpressnodes,
       (SELECT count(*) FROM attic.setpressnodes)  AS attic_ishodnik,
       to_regclass('public.setpressnodes')         AS ostalos_v_public;
