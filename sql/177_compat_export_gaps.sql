-- Десять имён, нужных экспорту фрагмента gid8, но не резолвившихся.
--
-- Найдено не инвентарём, а чтением исходников: export_fragment.cpp
-- строит запросы динамически (FROM %1, .arg(br_text(tn))) с полным
-- списком из 34 типов объектов фрагмента. db_consumers.py и его
-- предшественники такие места не видят — имя приходит не литералом
-- после FROM/JOIN, а подставляется в рантайме. Проверено запросом
-- каждого из 34 имён при search_path gid8: 24 резолвились, 10 нет.
--
-- Группа 1 — четыре класса оборудования ИТП, заведённые в net пустыми
-- по решению заказчика (шаг 41): consumptRegulators, pressDropRegulators,
-- regulArmatures (net.armature_control, исходное имя regularmatures),
-- reverseValves. Данных нет, но представления для чтения — были нужны,
-- их не сделали, потому что при пустых таблицах некому было пожаловаться.
--
-- Отображение id/lineid проверено на населённом классе с тем же
-- происхождением (net.regulator_press, 509 строк): "id" объекта — это
-- сохранённый исходный id самой линии (у net — subtype_src_id),
-- "lineid" в плоской модели был ссылкой на ту же линию (у net — id).
-- Не предположено, а сверено: compat.pressregulators уже использует
-- ровно это соответствие через net.v_pressregulators.
--
-- Группа 2 — пять узловых таблиц, пустых или почти пустых, убранных
-- на чердак как "неиспользуемые": overgroundNodes, pavilions,
-- undergroundNodes, uninstalledNodes (0 строк все), WDOdevices
-- (3385 строк, но только 10 соответствуют существующему узлу — разбор
-- в docs/14). internalNodes ушла на чердак ещё раньше и тоже пуста.
-- Все пять экспорт фрагмента запрашивает по имени. Представления —
-- прямой проброс к attic.<имя>: данные там не менялись, риска нет,
-- восстанавливается только видимость.

BEGIN;

CREATE SCHEMA IF NOT EXISTS compat;

-- Группа 1: пустые классы ИТП, id/lineid по проверенному правилу.
CREATE OR REPLACE VIEW compat.consumptregulators AS
SELECT
    o.subtype_src_id AS id,
    o.id              AS lineid,
    o.nodeid,
    o.regconsmean, o.workattrid, o.hydroresopen, o.hydroresclose,
    o.regvalvecap, o.relatleakage, o.plumsconsumption, o.opc,
    o.deltah, o.regulatorstateid
FROM net.regulator_consumption o
WHERE o.subtype_src_id IS NOT NULL;

CREATE OR REPLACE VIEW compat.pressdropregulators AS
SELECT
    o.subtype_src_id AS id,
    o.id              AS lineid,
    o.nodeid,
    o.pressdropmean, o.regvalvehydrores, o.consthroughregvalve,
    o.thrustdropmean, o.regvalverelcap, o.maxleakageclosevalve,
    o.consdrip, o.workattrid, o.deltah, o.regulatorstateid
FROM net.regulator_pressdrop o
WHERE o.subtype_src_id IS NOT NULL;

CREATE OR REPLACE VIEW compat.regularmatures AS
SELECT
    o.subtype_src_id AS id,
    o.id              AS lineid,
    o.nodeid,
    o.regpdmean, o.diametercondit, o.name, o.regarmtype, o.regarmsign,
    o.damperopendeg, o.rotationcount, o.gatecontrol, o.clue,
    o.thrustcollar, o.relleakage, o.opc, o.h, o.deltah, o.deltaq, o.q,
    o.damperarmaturestateid
FROM net.armature_control o
WHERE o.subtype_src_id IS NOT NULL;

CREATE OR REPLACE VIEW compat.reversevalves AS
SELECT
    o.subtype_src_id AS id,
    o.id              AS lineid,
    o.hydroresopen, o.hydroresclose, o.locinstall, o.standardrevvalveid,
    o.standardid, o.tubeid, o.pipelinelength, o.diameterinternal,
    o.tuberoughness, o.rescoeffssum, o.revvalvecap, o.relatleakage,
    o.stateid
FROM net.valve_reverse o
WHERE o.subtype_src_id IS NOT NULL;

COMMIT;

-- Группа 2: прямой проброс к чердаку, только если исходной таблицы
-- ещё нет ни в одной схеме (чтобы не затенить будущую настоящую миграцию).
DO $$
DECLARE
    t text;
BEGIN
    FOREACH t IN ARRAY ARRAY[
        'overgroundnodes', 'pavilions', 'undergroundnodes',
        'uninstallednodes', 'wdodevices', 'internalnodes'
    ]
    LOOP
        IF to_regclass('attic.' || quote_ident(t)) IS NOT NULL
           AND to_regclass('compat.' || quote_ident(t)) IS NULL THEN
            EXECUTE format(
                'CREATE VIEW compat.%I AS SELECT * FROM attic.%I', t, t);
        END IF;
    END LOOP;
END $$;

SELECT count(*) AS predstavleniy_v_compat
FROM pg_class c JOIN pg_namespace n ON n.oid = c.relnamespace
WHERE n.nspname = 'compat' AND c.relkind = 'v';
