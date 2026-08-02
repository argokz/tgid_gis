-- Запись через карточки ИТП + недостающая net.reclass_line.
--
-- Найдено проверкой вставки: триггеры линейных подтипов из
-- sql/055_subtype_views.sql вызывают net.reclass_line(bigint, text),
-- которой в живой БД не было (есть только reclass_node и
-- reclass_object). INSERT в net.v_pressregulators и любой другой
-- линейный подтип падал с "function net.reclass_line does not exist".
--
-- Здесь:
-- 1) net.reclass_line — симметрично reclass_node, но по net.line_reg;
-- 2) INSTEAD OF INSERT/UPDATE/DELETE на пять представлений шага 178
--    (bypass, consumptregulators, pressdropregulators, regularmatures,
--    reversevalves) по проверенному образцу v_pressregulators:
--    INSERT — reclass в класс + UPDATE атрибутов подтипа,
--    DELETE — снятие типа обратно в line_plain (объект не удаляется).

BEGIN;

CREATE OR REPLACE FUNCTION net.reclass_line(p_id bigint, p_target text)
RETURNS void
LANGUAGE plpgsql
SET search_path = pg_catalog, public, net
AS $function$
DECLARE
    src  text;
    cols text;
BEGIN
    SELECT tbl INTO src FROM net.line_reg WHERE id = p_id;
    IF src IS NULL THEN
        RAISE EXCEPTION 'Линия % не найдена в реестре', p_id;
    END IF;
    IF src = p_target THEN
        RETURN;
    END IF;

    SELECT string_agg(quote_ident(a.column_name), ', ')
    INTO cols
    FROM information_schema.columns a
    JOIN information_schema.columns b
      ON b.table_schema = 'net' AND b.table_name = p_target
     AND b.column_name = a.column_name
    WHERE a.table_schema = 'net' AND a.table_name = src
      AND a.is_generated = 'NEVER' AND b.is_generated = 'NEVER';

    EXECUTE format('INSERT INTO net.%I (%s) SELECT %s FROM net.%I WHERE id = $1',
                   p_target, cols, cols, src) USING p_id;
    EXECUTE format('DELETE FROM net.%I WHERE id = $1', src) USING p_id;
END $function$;

-- bypass
CREATE OR REPLACE FUNCTION net.v_bypass_ins() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    IF NEW.lineid IS NULL THEN
        RAISE EXCEPTION 'bypass: не указан lineid';
    END IF;
    PERFORM net.reclass_line(NEW.lineid, 'bypass');
    UPDATE net.bypass SET subtype_src_id = NEW.id,
        nodeid = NEW.nodeid,
        standardid = NEW.standardid,
        standardtubelink = NEW.standardtubelink,
        length = NEW.length,
        diameterinternal = NEW.diameterinternal,
        tuberoughness = NEW.tuberoughness,
        rescoeffssum = NEW.rescoeffssum,
        locinstall = NEW.locinstall,
        h = NEW.h, deltah = NEW.deltah, deltaq = NEW.deltaq, q = NEW.q,
        regulatorstateid = NEW.regulatorstateid,
        pipelinesignid = NEW.pipelinesignid
    WHERE id = NEW.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_bypass_ins ON net.v_bypass;
CREATE TRIGGER v_bypass_ins INSTEAD OF INSERT ON net.v_bypass
    FOR EACH ROW EXECUTE FUNCTION net.v_bypass_ins();

CREATE OR REPLACE FUNCTION net.v_bypass_upd() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    UPDATE net.bypass SET subtype_src_id = NEW.id,
        nodeid = NEW.nodeid,
        standardid = NEW.standardid,
        standardtubelink = NEW.standardtubelink,
        length = NEW.length,
        diameterinternal = NEW.diameterinternal,
        tuberoughness = NEW.tuberoughness,
        rescoeffssum = NEW.rescoeffssum,
        locinstall = NEW.locinstall,
        h = NEW.h, deltah = NEW.deltah, deltaq = NEW.deltaq, q = NEW.q,
        regulatorstateid = NEW.regulatorstateid,
        pipelinesignid = NEW.pipelinesignid
    WHERE id = OLD.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_bypass_upd ON net.v_bypass;
CREATE TRIGGER v_bypass_upd INSTEAD OF UPDATE ON net.v_bypass
    FOR EACH ROW EXECUTE FUNCTION net.v_bypass_upd();

CREATE OR REPLACE FUNCTION net.v_bypass_del() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    PERFORM net.reclass_line(OLD.lineid, 'line_plain');
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_bypass_del ON net.v_bypass;
CREATE TRIGGER v_bypass_del INSTEAD OF DELETE ON net.v_bypass
    FOR EACH ROW EXECUTE FUNCTION net.v_bypass_del();

-- consumptregulators
CREATE OR REPLACE FUNCTION net.v_consumptregulators_ins() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    IF NEW.lineid IS NULL THEN
        RAISE EXCEPTION 'consumptregulators: не указан lineid';
    END IF;
    PERFORM net.reclass_line(NEW.lineid, 'regulator_consumption');
    UPDATE net.regulator_consumption SET subtype_src_id = NEW.id,
        nodeid = NEW.nodeid,
        regconsmean = NEW.regconsmean,
        workattrid = NEW.workattrid,
        hydroresopen = NEW.hydroresopen,
        hydroresclose = NEW.hydroresclose,
        regvalvecap = NEW.regvalvecap,
        relatleakage = NEW.relatleakage,
        plumsconsumption = NEW.plumsconsumption,
        opc = NEW.opc,
        deltah = NEW.deltah,
        regulatorstateid = NEW.regulatorstateid
    WHERE id = NEW.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_consumptregulators_ins ON net.v_consumptregulators;
CREATE TRIGGER v_consumptregulators_ins INSTEAD OF INSERT ON net.v_consumptregulators
    FOR EACH ROW EXECUTE FUNCTION net.v_consumptregulators_ins();

CREATE OR REPLACE FUNCTION net.v_consumptregulators_upd() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    UPDATE net.regulator_consumption SET subtype_src_id = NEW.id,
        nodeid = NEW.nodeid,
        regconsmean = NEW.regconsmean,
        workattrid = NEW.workattrid,
        hydroresopen = NEW.hydroresopen,
        hydroresclose = NEW.hydroresclose,
        regvalvecap = NEW.regvalvecap,
        relatleakage = NEW.relatleakage,
        plumsconsumption = NEW.plumsconsumption,
        opc = NEW.opc,
        deltah = NEW.deltah,
        regulatorstateid = NEW.regulatorstateid
    WHERE id = OLD.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_consumptregulators_upd ON net.v_consumptregulators;
CREATE TRIGGER v_consumptregulators_upd INSTEAD OF UPDATE ON net.v_consumptregulators
    FOR EACH ROW EXECUTE FUNCTION net.v_consumptregulators_upd();

CREATE OR REPLACE FUNCTION net.v_consumptregulators_del() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    PERFORM net.reclass_line(OLD.lineid, 'line_plain');
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_consumptregulators_del ON net.v_consumptregulators;
CREATE TRIGGER v_consumptregulators_del INSTEAD OF DELETE ON net.v_consumptregulators
    FOR EACH ROW EXECUTE FUNCTION net.v_consumptregulators_del();

-- pressdropregulators
CREATE OR REPLACE FUNCTION net.v_pressdropregulators_ins() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    IF NEW.lineid IS NULL THEN
        RAISE EXCEPTION 'pressdropregulators: не указан lineid';
    END IF;
    PERFORM net.reclass_line(NEW.lineid, 'regulator_pressdrop');
    UPDATE net.regulator_pressdrop SET subtype_src_id = NEW.id,
        nodeid = NEW.nodeid,
        pressdropmean = NEW.pressdropmean,
        regvalvehydrores = NEW.regvalvehydrores,
        consthroughregvalve = NEW.consthroughregvalve,
        thrustdropmean = NEW.thrustdropmean,
        regvalverelcap = NEW.regvalverelcap,
        maxleakageclosevalve = NEW.maxleakageclosevalve,
        consdrip = NEW.consdrip,
        workattrid = NEW.workattrid,
        deltah = NEW.deltah,
        regulatorstateid = NEW.regulatorstateid
    WHERE id = NEW.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_pressdropregulators_ins ON net.v_pressdropregulators;
CREATE TRIGGER v_pressdropregulators_ins INSTEAD OF INSERT ON net.v_pressdropregulators
    FOR EACH ROW EXECUTE FUNCTION net.v_pressdropregulators_ins();

CREATE OR REPLACE FUNCTION net.v_pressdropregulators_upd() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    UPDATE net.regulator_pressdrop SET subtype_src_id = NEW.id,
        nodeid = NEW.nodeid,
        pressdropmean = NEW.pressdropmean,
        regvalvehydrores = NEW.regvalvehydrores,
        consthroughregvalve = NEW.consthroughregvalve,
        thrustdropmean = NEW.thrustdropmean,
        regvalverelcap = NEW.regvalverelcap,
        maxleakageclosevalve = NEW.maxleakageclosevalve,
        consdrip = NEW.consdrip,
        workattrid = NEW.workattrid,
        deltah = NEW.deltah,
        regulatorstateid = NEW.regulatorstateid
    WHERE id = OLD.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_pressdropregulators_upd ON net.v_pressdropregulators;
CREATE TRIGGER v_pressdropregulators_upd INSTEAD OF UPDATE ON net.v_pressdropregulators
    FOR EACH ROW EXECUTE FUNCTION net.v_pressdropregulators_upd();

CREATE OR REPLACE FUNCTION net.v_pressdropregulators_del() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    PERFORM net.reclass_line(OLD.lineid, 'line_plain');
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_pressdropregulators_del ON net.v_pressdropregulators;
CREATE TRIGGER v_pressdropregulators_del INSTEAD OF DELETE ON net.v_pressdropregulators
    FOR EACH ROW EXECUTE FUNCTION net.v_pressdropregulators_del();

-- regularmatures (net.armature_control)
CREATE OR REPLACE FUNCTION net.v_regularmatures_ins() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    IF NEW.lineid IS NULL THEN
        RAISE EXCEPTION 'regularmatures: не указан lineid';
    END IF;
    PERFORM net.reclass_line(NEW.lineid, 'armature_control');
    UPDATE net.armature_control SET subtype_src_id = NEW.id,
        nodeid = NEW.nodeid,
        regpdmean = NEW.regpdmean,
        diametercondit = NEW.diametercondit,
        name = NEW.name,
        regarmtype = NEW.regarmtype,
        regarmsign = NEW.regarmsign,
        damperopendeg = NEW.damperopendeg,
        rotationcount = NEW.rotationcount,
        gatecontrol = NEW.gatecontrol,
        clue = NEW.clue,
        thrustcollar = NEW.thrustcollar,
        relleakage = NEW.relleakage,
        opc = NEW.opc,
        h = NEW.h, deltah = NEW.deltah, deltaq = NEW.deltaq, q = NEW.q,
        damperarmaturestateid = NEW.damperarmaturestateid
    WHERE id = NEW.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_regularmatures_ins ON net.v_regularmatures;
CREATE TRIGGER v_regularmatures_ins INSTEAD OF INSERT ON net.v_regularmatures
    FOR EACH ROW EXECUTE FUNCTION net.v_regularmatures_ins();

CREATE OR REPLACE FUNCTION net.v_regularmatures_upd() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    UPDATE net.armature_control SET subtype_src_id = NEW.id,
        nodeid = NEW.nodeid,
        regpdmean = NEW.regpdmean,
        diametercondit = NEW.diametercondit,
        name = NEW.name,
        regarmtype = NEW.regarmtype,
        regarmsign = NEW.regarmsign,
        damperopendeg = NEW.damperopendeg,
        rotationcount = NEW.rotationcount,
        gatecontrol = NEW.gatecontrol,
        clue = NEW.clue,
        thrustcollar = NEW.thrustcollar,
        relleakage = NEW.relleakage,
        opc = NEW.opc,
        h = NEW.h, deltah = NEW.deltah, deltaq = NEW.deltaq, q = NEW.q,
        damperarmaturestateid = NEW.damperarmaturestateid
    WHERE id = OLD.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_regularmatures_upd ON net.v_regularmatures;
CREATE TRIGGER v_regularmatures_upd INSTEAD OF UPDATE ON net.v_regularmatures
    FOR EACH ROW EXECUTE FUNCTION net.v_regularmatures_upd();

CREATE OR REPLACE FUNCTION net.v_regularmatures_del() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    PERFORM net.reclass_line(OLD.lineid, 'line_plain');
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_regularmatures_del ON net.v_regularmatures;
CREATE TRIGGER v_regularmatures_del INSTEAD OF DELETE ON net.v_regularmatures
    FOR EACH ROW EXECUTE FUNCTION net.v_regularmatures_del();

-- reversevalves
CREATE OR REPLACE FUNCTION net.v_reversevalves_ins() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    IF NEW.lineid IS NULL THEN
        RAISE EXCEPTION 'reversevalves: не указан lineid';
    END IF;
    PERFORM net.reclass_line(NEW.lineid, 'valve_reverse');
    UPDATE net.valve_reverse SET subtype_src_id = NEW.id,
        hydroresopen = NEW.hydroresopen,
        hydroresclose = NEW.hydroresclose,
        locinstall = NEW.locinstall,
        standardrevvalveid = NEW.standardrevvalveid,
        standardid = NEW.standardid,
        tubeid = NEW.tubeid,
        pipelinelength = NEW.pipelinelength,
        diameterinternal = NEW.diameterinternal,
        tuberoughness = NEW.tuberoughness,
        rescoeffssum = NEW.rescoeffssum,
        revvalvecap = NEW.revvalvecap,
        relatleakage = NEW.relatleakage,
        stateid = NEW.stateid
    WHERE id = NEW.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_reversevalves_ins ON net.v_reversevalves;
CREATE TRIGGER v_reversevalves_ins INSTEAD OF INSERT ON net.v_reversevalves
    FOR EACH ROW EXECUTE FUNCTION net.v_reversevalves_ins();

CREATE OR REPLACE FUNCTION net.v_reversevalves_upd() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    UPDATE net.valve_reverse SET subtype_src_id = NEW.id,
        hydroresopen = NEW.hydroresopen,
        hydroresclose = NEW.hydroresclose,
        locinstall = NEW.locinstall,
        standardrevvalveid = NEW.standardrevvalveid,
        standardid = NEW.standardid,
        tubeid = NEW.tubeid,
        pipelinelength = NEW.pipelinelength,
        diameterinternal = NEW.diameterinternal,
        tuberoughness = NEW.tuberoughness,
        rescoeffssum = NEW.rescoeffssum,
        revvalvecap = NEW.revvalvecap,
        relatleakage = NEW.relatleakage,
        stateid = NEW.stateid
    WHERE id = OLD.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_reversevalves_upd ON net.v_reversevalves;
CREATE TRIGGER v_reversevalves_upd INSTEAD OF UPDATE ON net.v_reversevalves
    FOR EACH ROW EXECUTE FUNCTION net.v_reversevalves_upd();

CREATE OR REPLACE FUNCTION net.v_reversevalves_del() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    PERFORM net.reclass_line(OLD.lineid, 'line_plain');
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_reversevalves_del ON net.v_reversevalves;
CREATE TRIGGER v_reversevalves_del INSTEAD OF DELETE ON net.v_reversevalves
    FOR EACH ROW EXECUTE FUNCTION net.v_reversevalves_del();

COMMIT;
