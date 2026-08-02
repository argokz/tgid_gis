-- net.v_* для пустых ИТП-классов и bypass.
--
-- Шаг 64 завёл только compat.* (чтение для экспорта). Шаг 65 переводит
-- динамический резолвер gid8 (tbl_sql) на net.v_*: compat остаётся
-- тонкой обёрткой SELECT * FROM net.v_*, чтобы search_path gid8 и
-- прямой путь давали одно и то же.
--
-- INSTEAD OF не добавляем: таблицы пусты по решению заказчика, а состав
-- колонок совпадает с уже проверенным правилом id/lineid из
-- sql/177_compat_export_gaps.sql. Запись через карточки для этих
-- типов — отдельный шаг вместе с наполнением.

\set ON_ERROR_STOP on

BEGIN;

DROP VIEW IF EXISTS net.v_consumptregulators CASCADE;
CREATE VIEW net.v_consumptregulators AS
SELECT
    o.subtype_src_id AS id,
    o.id              AS lineid,
    o.nodeid,
    o.regconsmean, o.workattrid, o.hydroresopen, o.hydroresclose,
    o.regvalvecap, o.relatleakage, o.plumsconsumption, o.opc,
    o.deltah, o.regulatorstateid
FROM net.regulator_consumption o
WHERE o.subtype_src_id IS NOT NULL;

DROP VIEW IF EXISTS net.v_pressdropregulators CASCADE;
CREATE VIEW net.v_pressdropregulators AS
SELECT
    o.subtype_src_id AS id,
    o.id              AS lineid,
    o.nodeid,
    o.pressdropmean, o.regvalvehydrores, o.consthroughregvalve,
    o.thrustdropmean, o.regvalverelcap, o.maxleakageclosevalve,
    o.consdrip, o.workattrid, o.deltah, o.regulatorstateid
FROM net.regulator_pressdrop o
WHERE o.subtype_src_id IS NOT NULL;

DROP VIEW IF EXISTS net.v_regularmatures CASCADE;
CREATE VIEW net.v_regularmatures AS
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

DROP VIEW IF EXISTS net.v_reversevalves CASCADE;
CREATE VIEW net.v_reversevalves AS
SELECT
    o.subtype_src_id AS id,
    o.id              AS lineid,
    o.hydroresopen, o.hydroresclose, o.locinstall, o.standardrevvalveid,
    o.standardid, o.tubeid, o.pipelinelength, o.diameterinternal,
    o.tuberoughness, o.rescoeffssum, o.revvalvecap, o.relatleakage,
    o.stateid
FROM net.valve_reverse o
WHERE o.subtype_src_id IS NOT NULL;

-- bypass: в экспорте запрашивается как подтип линии; объектная таблица
-- уже есть (sql/130). Представление — тот же контракт id/lineid.
DROP VIEW IF EXISTS net.v_bypass CASCADE;
CREATE VIEW net.v_bypass AS
SELECT
    o.subtype_src_id AS id,
    o.id              AS lineid,
    o.nodeid,
    o.standardid, o.standardtubelink, o.length, o.diameterinternal,
    o.tuberoughness, o.rescoeffssum, o.locinstall,
    o.h, o.deltah, o.deltaq, o.q, o.regulatorstateid, o.pipelinesignid
FROM net.bypass o
WHERE o.subtype_src_id IS NOT NULL;

-- compat — тонкие обёртки: старый search_path gid8 без пересборки.
CREATE SCHEMA IF NOT EXISTS compat;

CREATE OR REPLACE VIEW compat.consumptregulators AS
SELECT * FROM net.v_consumptregulators;

CREATE OR REPLACE VIEW compat.pressdropregulators AS
SELECT * FROM net.v_pressdropregulators;

CREATE OR REPLACE VIEW compat.regularmatures AS
SELECT * FROM net.v_regularmatures;

CREATE OR REPLACE VIEW compat.reversevalves AS
SELECT * FROM net.v_reversevalves;

CREATE OR REPLACE VIEW compat.bypass AS
SELECT * FROM net.v_bypass;

COMMIT;
