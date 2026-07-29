-- СГЕНЕРИРОВАНО tools/gen_subtype_views.py
--
-- Слой совместимости для таблиц-подтипов. Представления создаются
-- в схеме net под именами v_<исходная таблица>, оригиналы не трогаются.
-- Подстановка вместо public.* — отдельным шагом (sql/060_switch_subtypes.sql).


-- connectnodes -> net.connect_node
-- DROP обязателен: CREATE OR REPLACE VIEW не умеет менять состав
-- и порядок колонок, а они меняются при правке карты типов.
DROP VIEW IF EXISTS net.v_connectnodes CASCADE;
CREATE VIEW net.v_connectnodes AS
SELECT o.subtype_src_id AS id,
       o.id             AS nodeid,
       o.connectid
FROM net.connect_node o
UNION ALL
-- Строки, не ставшие объектом: дубли и «проигравшие» чужому классу.
-- Узел может иметь строки сразу в двух подтипах — старая модель это
-- допускала, и расчётное ядро читает обе. Без этой части выборка
-- вернула бы меньше строк, чем на исходной БД.
SELECT (v.payload ->> 'id')::int AS id,
       v.obj_id                  AS nodeid,
       (v.payload ->> 'connectid')::integer
FROM net.object_variant v
WHERE v.src_table = 'connectnodes' AND NOT v.chosen;

CREATE OR REPLACE FUNCTION net.v_connectnodes_ins() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    IF NEW.nodeid IS NULL THEN
        RAISE EXCEPTION 'connectnodes: не указан nodeid';
    END IF;
    -- Тип объекта задаётся именно этой вставкой: переносим его в класс.
    PERFORM net.reclass_node(NEW.nodeid, 'connect_node');
    UPDATE net.connect_node SET subtype_src_id = NEW.id,
        connectid = NEW.connectid
    WHERE id = NEW.nodeid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_connectnodes_ins ON net.v_connectnodes;
CREATE TRIGGER v_connectnodes_ins INSTEAD OF INSERT ON net.v_connectnodes
    FOR EACH ROW EXECUTE FUNCTION net.v_connectnodes_ins();

CREATE OR REPLACE FUNCTION net.v_connectnodes_upd() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    UPDATE net.connect_node SET subtype_src_id = NEW.id,
        connectid = NEW.connectid
    WHERE id = OLD.nodeid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_connectnodes_upd ON net.v_connectnodes;
CREATE TRIGGER v_connectnodes_upd INSTEAD OF UPDATE ON net.v_connectnodes
    FOR EACH ROW EXECUTE FUNCTION net.v_connectnodes_upd();

CREATE OR REPLACE FUNCTION net.v_connectnodes_del() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    -- Удаление строки подтипа означает снятие типа, а не удаление объекта.
    PERFORM net.reclass_node(OLD.nodeid, 'node_plain');
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_connectnodes_del ON net.v_connectnodes;
CREATE TRIGGER v_connectnodes_del INSTEAD OF DELETE ON net.v_connectnodes
    FOR EACH ROW EXECUTE FUNCTION net.v_connectnodes_del();


-- generalizedconsumers -> net.consumer_general
-- DROP обязателен: CREATE OR REPLACE VIEW не умеет менять состав
-- и порядок колонок, а они меняются при правке карты типов.
DROP VIEW IF EXISTS net.v_generalizedconsumers CASCADE;
CREATE VIEW net.v_generalizedconsumers AS
SELECT o.subtype_src_id AS id,
       o.id             AS nodeid,
       o.quarter,
       o.maxbuildingheight,
       o.accumcoeff,
       o.hydromodesignid,
       o.specexpendid,
       o.calctemperatureid,
       o.gvsloadgraphid,
       o.varcoeffid,
       o.normhlosflow,
       o.normhlosret,
       o.calchldep,
       o.calcinternhddep,
       o.adjcalchldep,
       o.adjcalchddep,
       o.perspcalchldep,
       o.perspcalcinternhddep,
       o.calctempdep,
       o.connectionschemeid,
       o.mixfactcoeffdep,
       o.calchlindep,
       o.calcinternhdindep,
       o.adjcalchlindep,
       o.adjcalcinternhdindep,
       o.perspcalchlindep,
       o.perspcalcinternhdindep,
       o.calctempindep,
       o.mixfactcoeffindep,
       o.calchlventil,
       o.adjcalchlventil,
       o.perspcalchlventil,
       o.calchlcond,
       o.adjcalchlcond,
       o.perspcalchlcond,
       o.calchlclosesys,
       o.calchlopensysflow,
       o.calchlopensysret,
       o.adjcalchlclosesys,
       o.adjcalchlopensysflow,
       o.adjcalchlopensysret,
       o.perspcalchlclosesys,
       o.perspcalchlopensysflow,
       o.perspcalchlopensysret,
       o.expendhwpart,
       o.calctempha,
       o.calchlparall,
       o.internhdparall,
       o.adjcalchlparall,
       o.adjinternhdparall,
       o.perspcalchlparall,
       o.perspinternhdparall,
       o.schemeparallid,
       o.calchlgvsparall,
       o.adjcalchlgvsparall,
       o.perspcalchlgvsparall,
       o.hourirregcoeffparall,
       o.avghlcompparall,
       o.temprecircpipeparall,
       o.calctemphrparall,
       o.calctemphwdoparall,
       o.amrdepparall,
       o.calchlmix,
       o.internhdmix,
       o.adjcalchlmix,
       o.adjinternhdmix,
       o.perspcalchlmix,
       o.perspinternhdmix,
       o.schememixid,
       o.calchlgvsmix,
       o.adjcalchlgvsmix,
       o.perspcalchlgvsmix,
       o.hourirregcoeffmix,
       o.avghlcompmix,
       o.temprecircpipemix,
       o.calctemphrmix,
       o.calctemphwdomix,
       o.amrdepmix,
       o.calchlconseq,
       o.internhdconseq,
       o.adjcalchlconseq,
       o.adjinternhdconseq,
       o.perspcalchlconseq,
       o.perspinternhdconseq,
       o.schemeconseqid,
       o.calchlgvsconseq,
       o.adjcalchlgvsconseq,
       o.perspcalchlgvsconseq,
       o.hourirregcoeffconseq,
       o.avghlcompconseq,
       o.temprecircpipeconseq,
       o.calctemphrconseq,
       o.calctemphwdoconseq,
       o.amrdepconseq,
       o.calchlpreon,
       o.internhdpreon,
       o.adjcalchlpreon,
       o.adjinternhdpreon,
       o.perspcalchlpreon,
       o.perspinternhdpreon,
       o.schemepreonid,
       o.calchlgvspreon,
       o.adjcalchlgvspreon,
       o.perspcalchlgvspreon,
       o.hourirregcoeffpreon,
       o.avghlcomppreon,
       o.temprecircpipepreon,
       o.calctemphrpreon,
       o.calctemphwdopreon,
       o.amrdeppreon,
       o.avghlgvsopensysflow,
       o.avghlgvsopensysret,
       o.adjavghlgvsopensysflow,
       o.adjavghlgvsopensysret,
       o.perspavghlgvsopensysflow,
       o.perspavghlgvsopensysret,
       o.hourirregcoeffopen,
       o.avghlcompopen,
       o.temprecircpipeopen,
       o.calctemphwdoopen,
       o.pdvalveinstalllocid,
       o.setpdonregulator,
       o.setleakageflow,
       o.setleakageret,
       o.wemeteringdeviceid,
       o.volwaterhs,
       o.volwatervs,
       o.minthrustds,
       o.hydroresclosesys,
       o.hydroresclosesyssummer,
       o.closesyscalcsignid,
       o.hydroreswdoflow,
       o.calcsignopensysflowid,
       o.hydroreswdoret,
       o.calcsignreswdoretid,
       o.calcsignsetloadclosesys,
       o.calcsignsetloadopensysflow,
       o.calcsignsetloadopensysret,
       o.hydrothrustin,
       o.hydrothrustout,
       o.calcexpenddep,
       o.calcexpendindep,
       o.calcexpendventil,
       o.calcexpendcond,
       o.calcexpendhwflow,
       o.calcexpendhwret,
       o.calcexpendrecircopen,
       o.calcexpendhwparall,
       o.calcexpendhwmix,
       o.calcexpendhwconseq,
       o.calcexpendhwpreon,
       o.calcthrustinwsd,
       o.consumerstateid,
       o.name
FROM net.consumer_general o
UNION ALL
-- Строки, не ставшие объектом: дубли и «проигравшие» чужому классу.
-- Узел может иметь строки сразу в двух подтипах — старая модель это
-- допускала, и расчётное ядро читает обе. Без этой части выборка
-- вернула бы меньше строк, чем на исходной БД.
SELECT (v.payload ->> 'id')::int AS id,
       v.obj_id                  AS nodeid,
       (v.payload ->> 'quarter')::integer,
       (v.payload ->> 'maxbuildingheight')::double precision,
       (v.payload ->> 'accumcoeff')::double precision,
       (v.payload ->> 'hydromodesignid')::integer,
       (v.payload ->> 'specexpendid')::integer,
       (v.payload ->> 'calctemperatureid')::integer,
       (v.payload ->> 'gvsloadgraphid')::integer,
       (v.payload ->> 'varcoeffid')::integer,
       (v.payload ->> 'normhlosflow')::double precision,
       (v.payload ->> 'normhlosret')::double precision,
       (v.payload ->> 'calchldep')::double precision,
       (v.payload ->> 'calcinternhddep')::double precision,
       (v.payload ->> 'adjcalchldep')::double precision,
       (v.payload ->> 'adjcalchddep')::double precision,
       (v.payload ->> 'perspcalchldep')::double precision,
       (v.payload ->> 'perspcalcinternhddep')::double precision,
       (v.payload ->> 'calctempdep')::double precision,
       (v.payload ->> 'connectionschemeid')::integer,
       (v.payload ->> 'mixfactcoeffdep')::double precision,
       (v.payload ->> 'calchlindep')::double precision,
       (v.payload ->> 'calcinternhdindep')::double precision,
       (v.payload ->> 'adjcalchlindep')::double precision,
       (v.payload ->> 'adjcalcinternhdindep')::double precision,
       (v.payload ->> 'perspcalchlindep')::double precision,
       (v.payload ->> 'perspcalcinternhdindep')::double precision,
       (v.payload ->> 'calctempindep')::double precision,
       (v.payload ->> 'mixfactcoeffindep')::double precision,
       (v.payload ->> 'calchlventil')::double precision,
       (v.payload ->> 'adjcalchlventil')::double precision,
       (v.payload ->> 'perspcalchlventil')::double precision,
       (v.payload ->> 'calchlcond')::double precision,
       (v.payload ->> 'adjcalchlcond')::double precision,
       (v.payload ->> 'perspcalchlcond')::double precision,
       (v.payload ->> 'calchlclosesys')::double precision,
       (v.payload ->> 'calchlopensysflow')::double precision,
       (v.payload ->> 'calchlopensysret')::double precision,
       (v.payload ->> 'adjcalchlclosesys')::double precision,
       (v.payload ->> 'adjcalchlopensysflow')::double precision,
       (v.payload ->> 'adjcalchlopensysret')::double precision,
       (v.payload ->> 'perspcalchlclosesys')::double precision,
       (v.payload ->> 'perspcalchlopensysflow')::double precision,
       (v.payload ->> 'perspcalchlopensysret')::double precision,
       (v.payload ->> 'expendhwpart')::double precision,
       (v.payload ->> 'calctempha')::double precision,
       (v.payload ->> 'calchlparall')::double precision,
       (v.payload ->> 'internhdparall')::double precision,
       (v.payload ->> 'adjcalchlparall')::double precision,
       (v.payload ->> 'adjinternhdparall')::double precision,
       (v.payload ->> 'perspcalchlparall')::double precision,
       (v.payload ->> 'perspinternhdparall')::double precision,
       (v.payload ->> 'schemeparallid')::integer,
       (v.payload ->> 'calchlgvsparall')::double precision,
       (v.payload ->> 'adjcalchlgvsparall')::double precision,
       (v.payload ->> 'perspcalchlgvsparall')::double precision,
       (v.payload ->> 'hourirregcoeffparall')::double precision,
       (v.payload ->> 'avghlcompparall')::double precision,
       (v.payload ->> 'temprecircpipeparall')::double precision,
       (v.payload ->> 'calctemphrparall')::double precision,
       (v.payload ->> 'calctemphwdoparall')::double precision,
       (v.payload ->> 'amrdepparall')::double precision,
       (v.payload ->> 'calchlmix')::double precision,
       (v.payload ->> 'internhdmix')::double precision,
       (v.payload ->> 'adjcalchlmix')::double precision,
       (v.payload ->> 'adjinternhdmix')::double precision,
       (v.payload ->> 'perspcalchlmix')::double precision,
       (v.payload ->> 'perspinternhdmix')::double precision,
       (v.payload ->> 'schememixid')::integer,
       (v.payload ->> 'calchlgvsmix')::double precision,
       (v.payload ->> 'adjcalchlgvsmix')::double precision,
       (v.payload ->> 'perspcalchlgvsmix')::double precision,
       (v.payload ->> 'hourirregcoeffmix')::double precision,
       (v.payload ->> 'avghlcompmix')::double precision,
       (v.payload ->> 'temprecircpipemix')::double precision,
       (v.payload ->> 'calctemphrmix')::double precision,
       (v.payload ->> 'calctemphwdomix')::double precision,
       (v.payload ->> 'amrdepmix')::double precision,
       (v.payload ->> 'calchlconseq')::double precision,
       (v.payload ->> 'internhdconseq')::double precision,
       (v.payload ->> 'adjcalchlconseq')::double precision,
       (v.payload ->> 'adjinternhdconseq')::double precision,
       (v.payload ->> 'perspcalchlconseq')::double precision,
       (v.payload ->> 'perspinternhdconseq')::double precision,
       (v.payload ->> 'schemeconseqid')::integer,
       (v.payload ->> 'calchlgvsconseq')::double precision,
       (v.payload ->> 'adjcalchlgvsconseq')::double precision,
       (v.payload ->> 'perspcalchlgvsconseq')::double precision,
       (v.payload ->> 'hourirregcoeffconseq')::double precision,
       (v.payload ->> 'avghlcompconseq')::double precision,
       (v.payload ->> 'temprecircpipeconseq')::double precision,
       (v.payload ->> 'calctemphrconseq')::double precision,
       (v.payload ->> 'calctemphwdoconseq')::double precision,
       (v.payload ->> 'amrdepconseq')::double precision,
       (v.payload ->> 'calchlpreon')::double precision,
       (v.payload ->> 'internhdpreon')::double precision,
       (v.payload ->> 'adjcalchlpreon')::double precision,
       (v.payload ->> 'adjinternhdpreon')::double precision,
       (v.payload ->> 'perspcalchlpreon')::double precision,
       (v.payload ->> 'perspinternhdpreon')::double precision,
       (v.payload ->> 'schemepreonid')::integer,
       (v.payload ->> 'calchlgvspreon')::double precision,
       (v.payload ->> 'adjcalchlgvspreon')::double precision,
       (v.payload ->> 'perspcalchlgvspreon')::double precision,
       (v.payload ->> 'hourirregcoeffpreon')::double precision,
       (v.payload ->> 'avghlcomppreon')::double precision,
       (v.payload ->> 'temprecircpipepreon')::double precision,
       (v.payload ->> 'calctemphrpreon')::double precision,
       (v.payload ->> 'calctemphwdopreon')::double precision,
       (v.payload ->> 'amrdeppreon')::double precision,
       (v.payload ->> 'avghlgvsopensysflow')::double precision,
       (v.payload ->> 'avghlgvsopensysret')::double precision,
       (v.payload ->> 'adjavghlgvsopensysflow')::double precision,
       (v.payload ->> 'adjavghlgvsopensysret')::double precision,
       (v.payload ->> 'perspavghlgvsopensysflow')::double precision,
       (v.payload ->> 'perspavghlgvsopensysret')::double precision,
       (v.payload ->> 'hourirregcoeffopen')::double precision,
       (v.payload ->> 'avghlcompopen')::double precision,
       (v.payload ->> 'temprecircpipeopen')::double precision,
       (v.payload ->> 'calctemphwdoopen')::double precision,
       (v.payload ->> 'pdvalveinstalllocid')::integer,
       (v.payload ->> 'setpdonregulator')::double precision,
       (v.payload ->> 'setleakageflow')::double precision,
       (v.payload ->> 'setleakageret')::double precision,
       (v.payload ->> 'wemeteringdeviceid')::integer,
       (v.payload ->> 'volwaterhs')::double precision,
       (v.payload ->> 'volwatervs')::double precision,
       (v.payload ->> 'minthrustds')::double precision,
       (v.payload ->> 'hydroresclosesys')::double precision,
       (v.payload ->> 'hydroresclosesyssummer')::double precision,
       (v.payload ->> 'closesyscalcsignid')::integer,
       (v.payload ->> 'hydroreswdoflow')::double precision,
       (v.payload ->> 'calcsignopensysflowid')::integer,
       (v.payload ->> 'hydroreswdoret')::double precision,
       (v.payload ->> 'calcsignreswdoretid')::integer,
       (v.payload ->> 'calcsignsetloadclosesys')::integer,
       (v.payload ->> 'calcsignsetloadopensysflow')::integer,
       (v.payload ->> 'calcsignsetloadopensysret')::integer,
       (v.payload ->> 'hydrothrustin')::double precision,
       (v.payload ->> 'hydrothrustout')::double precision,
       (v.payload ->> 'calcexpenddep')::double precision,
       (v.payload ->> 'calcexpendindep')::double precision,
       (v.payload ->> 'calcexpendventil')::double precision,
       (v.payload ->> 'calcexpendcond')::double precision,
       (v.payload ->> 'calcexpendhwflow')::double precision,
       (v.payload ->> 'calcexpendhwret')::double precision,
       (v.payload ->> 'calcexpendrecircopen')::double precision,
       (v.payload ->> 'calcexpendhwparall')::double precision,
       (v.payload ->> 'calcexpendhwmix')::double precision,
       (v.payload ->> 'calcexpendhwconseq')::double precision,
       (v.payload ->> 'calcexpendhwpreon')::double precision,
       (v.payload ->> 'calcthrustinwsd')::integer,
       (v.payload ->> 'consumerstateid')::integer,
       (v.payload ->> 'name')::text
FROM net.object_variant v
WHERE v.src_table = 'generalizedconsumers' AND NOT v.chosen;

CREATE OR REPLACE FUNCTION net.v_generalizedconsumers_ins() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    IF NEW.nodeid IS NULL THEN
        RAISE EXCEPTION 'generalizedconsumers: не указан nodeid';
    END IF;
    -- Тип объекта задаётся именно этой вставкой: переносим его в класс.
    PERFORM net.reclass_node(NEW.nodeid, 'consumer_general');
    UPDATE net.consumer_general SET subtype_src_id = NEW.id,
        quarter = NEW.quarter,
        maxbuildingheight = NEW.maxbuildingheight,
        accumcoeff = NEW.accumcoeff,
        hydromodesignid = NEW.hydromodesignid,
        specexpendid = NEW.specexpendid,
        calctemperatureid = NEW.calctemperatureid,
        gvsloadgraphid = NEW.gvsloadgraphid,
        varcoeffid = NEW.varcoeffid,
        normhlosflow = NEW.normhlosflow,
        normhlosret = NEW.normhlosret,
        calchldep = NEW.calchldep,
        calcinternhddep = NEW.calcinternhddep,
        adjcalchldep = NEW.adjcalchldep,
        adjcalchddep = NEW.adjcalchddep,
        perspcalchldep = NEW.perspcalchldep,
        perspcalcinternhddep = NEW.perspcalcinternhddep,
        calctempdep = NEW.calctempdep,
        connectionschemeid = NEW.connectionschemeid,
        mixfactcoeffdep = NEW.mixfactcoeffdep,
        calchlindep = NEW.calchlindep,
        calcinternhdindep = NEW.calcinternhdindep,
        adjcalchlindep = NEW.adjcalchlindep,
        adjcalcinternhdindep = NEW.adjcalcinternhdindep,
        perspcalchlindep = NEW.perspcalchlindep,
        perspcalcinternhdindep = NEW.perspcalcinternhdindep,
        calctempindep = NEW.calctempindep,
        mixfactcoeffindep = NEW.mixfactcoeffindep,
        calchlventil = NEW.calchlventil,
        adjcalchlventil = NEW.adjcalchlventil,
        perspcalchlventil = NEW.perspcalchlventil,
        calchlcond = NEW.calchlcond,
        adjcalchlcond = NEW.adjcalchlcond,
        perspcalchlcond = NEW.perspcalchlcond,
        calchlclosesys = NEW.calchlclosesys,
        calchlopensysflow = NEW.calchlopensysflow,
        calchlopensysret = NEW.calchlopensysret,
        adjcalchlclosesys = NEW.adjcalchlclosesys,
        adjcalchlopensysflow = NEW.adjcalchlopensysflow,
        adjcalchlopensysret = NEW.adjcalchlopensysret,
        perspcalchlclosesys = NEW.perspcalchlclosesys,
        perspcalchlopensysflow = NEW.perspcalchlopensysflow,
        perspcalchlopensysret = NEW.perspcalchlopensysret,
        expendhwpart = NEW.expendhwpart,
        calctempha = NEW.calctempha,
        calchlparall = NEW.calchlparall,
        internhdparall = NEW.internhdparall,
        adjcalchlparall = NEW.adjcalchlparall,
        adjinternhdparall = NEW.adjinternhdparall,
        perspcalchlparall = NEW.perspcalchlparall,
        perspinternhdparall = NEW.perspinternhdparall,
        schemeparallid = NEW.schemeparallid,
        calchlgvsparall = NEW.calchlgvsparall,
        adjcalchlgvsparall = NEW.adjcalchlgvsparall,
        perspcalchlgvsparall = NEW.perspcalchlgvsparall,
        hourirregcoeffparall = NEW.hourirregcoeffparall,
        avghlcompparall = NEW.avghlcompparall,
        temprecircpipeparall = NEW.temprecircpipeparall,
        calctemphrparall = NEW.calctemphrparall,
        calctemphwdoparall = NEW.calctemphwdoparall,
        amrdepparall = NEW.amrdepparall,
        calchlmix = NEW.calchlmix,
        internhdmix = NEW.internhdmix,
        adjcalchlmix = NEW.adjcalchlmix,
        adjinternhdmix = NEW.adjinternhdmix,
        perspcalchlmix = NEW.perspcalchlmix,
        perspinternhdmix = NEW.perspinternhdmix,
        schememixid = NEW.schememixid,
        calchlgvsmix = NEW.calchlgvsmix,
        adjcalchlgvsmix = NEW.adjcalchlgvsmix,
        perspcalchlgvsmix = NEW.perspcalchlgvsmix,
        hourirregcoeffmix = NEW.hourirregcoeffmix,
        avghlcompmix = NEW.avghlcompmix,
        temprecircpipemix = NEW.temprecircpipemix,
        calctemphrmix = NEW.calctemphrmix,
        calctemphwdomix = NEW.calctemphwdomix,
        amrdepmix = NEW.amrdepmix,
        calchlconseq = NEW.calchlconseq,
        internhdconseq = NEW.internhdconseq,
        adjcalchlconseq = NEW.adjcalchlconseq,
        adjinternhdconseq = NEW.adjinternhdconseq,
        perspcalchlconseq = NEW.perspcalchlconseq,
        perspinternhdconseq = NEW.perspinternhdconseq,
        schemeconseqid = NEW.schemeconseqid,
        calchlgvsconseq = NEW.calchlgvsconseq,
        adjcalchlgvsconseq = NEW.adjcalchlgvsconseq,
        perspcalchlgvsconseq = NEW.perspcalchlgvsconseq,
        hourirregcoeffconseq = NEW.hourirregcoeffconseq,
        avghlcompconseq = NEW.avghlcompconseq,
        temprecircpipeconseq = NEW.temprecircpipeconseq,
        calctemphrconseq = NEW.calctemphrconseq,
        calctemphwdoconseq = NEW.calctemphwdoconseq,
        amrdepconseq = NEW.amrdepconseq,
        calchlpreon = NEW.calchlpreon,
        internhdpreon = NEW.internhdpreon,
        adjcalchlpreon = NEW.adjcalchlpreon,
        adjinternhdpreon = NEW.adjinternhdpreon,
        perspcalchlpreon = NEW.perspcalchlpreon,
        perspinternhdpreon = NEW.perspinternhdpreon,
        schemepreonid = NEW.schemepreonid,
        calchlgvspreon = NEW.calchlgvspreon,
        adjcalchlgvspreon = NEW.adjcalchlgvspreon,
        perspcalchlgvspreon = NEW.perspcalchlgvspreon,
        hourirregcoeffpreon = NEW.hourirregcoeffpreon,
        avghlcomppreon = NEW.avghlcomppreon,
        temprecircpipepreon = NEW.temprecircpipepreon,
        calctemphrpreon = NEW.calctemphrpreon,
        calctemphwdopreon = NEW.calctemphwdopreon,
        amrdeppreon = NEW.amrdeppreon,
        avghlgvsopensysflow = NEW.avghlgvsopensysflow,
        avghlgvsopensysret = NEW.avghlgvsopensysret,
        adjavghlgvsopensysflow = NEW.adjavghlgvsopensysflow,
        adjavghlgvsopensysret = NEW.adjavghlgvsopensysret,
        perspavghlgvsopensysflow = NEW.perspavghlgvsopensysflow,
        perspavghlgvsopensysret = NEW.perspavghlgvsopensysret,
        hourirregcoeffopen = NEW.hourirregcoeffopen,
        avghlcompopen = NEW.avghlcompopen,
        temprecircpipeopen = NEW.temprecircpipeopen,
        calctemphwdoopen = NEW.calctemphwdoopen,
        pdvalveinstalllocid = NEW.pdvalveinstalllocid,
        setpdonregulator = NEW.setpdonregulator,
        setleakageflow = NEW.setleakageflow,
        setleakageret = NEW.setleakageret,
        wemeteringdeviceid = NEW.wemeteringdeviceid,
        volwaterhs = NEW.volwaterhs,
        volwatervs = NEW.volwatervs,
        minthrustds = NEW.minthrustds,
        hydroresclosesys = NEW.hydroresclosesys,
        hydroresclosesyssummer = NEW.hydroresclosesyssummer,
        closesyscalcsignid = NEW.closesyscalcsignid,
        hydroreswdoflow = NEW.hydroreswdoflow,
        calcsignopensysflowid = NEW.calcsignopensysflowid,
        hydroreswdoret = NEW.hydroreswdoret,
        calcsignreswdoretid = NEW.calcsignreswdoretid,
        calcsignsetloadclosesys = NEW.calcsignsetloadclosesys,
        calcsignsetloadopensysflow = NEW.calcsignsetloadopensysflow,
        calcsignsetloadopensysret = NEW.calcsignsetloadopensysret,
        hydrothrustin = NEW.hydrothrustin,
        hydrothrustout = NEW.hydrothrustout,
        calcexpenddep = NEW.calcexpenddep,
        calcexpendindep = NEW.calcexpendindep,
        calcexpendventil = NEW.calcexpendventil,
        calcexpendcond = NEW.calcexpendcond,
        calcexpendhwflow = NEW.calcexpendhwflow,
        calcexpendhwret = NEW.calcexpendhwret,
        calcexpendrecircopen = NEW.calcexpendrecircopen,
        calcexpendhwparall = NEW.calcexpendhwparall,
        calcexpendhwmix = NEW.calcexpendhwmix,
        calcexpendhwconseq = NEW.calcexpendhwconseq,
        calcexpendhwpreon = NEW.calcexpendhwpreon,
        calcthrustinwsd = NEW.calcthrustinwsd,
        consumerstateid = NEW.consumerstateid,
        name = NEW.name
    WHERE id = NEW.nodeid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_generalizedconsumers_ins ON net.v_generalizedconsumers;
CREATE TRIGGER v_generalizedconsumers_ins INSTEAD OF INSERT ON net.v_generalizedconsumers
    FOR EACH ROW EXECUTE FUNCTION net.v_generalizedconsumers_ins();

CREATE OR REPLACE FUNCTION net.v_generalizedconsumers_upd() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    UPDATE net.consumer_general SET subtype_src_id = NEW.id,
        quarter = NEW.quarter,
        maxbuildingheight = NEW.maxbuildingheight,
        accumcoeff = NEW.accumcoeff,
        hydromodesignid = NEW.hydromodesignid,
        specexpendid = NEW.specexpendid,
        calctemperatureid = NEW.calctemperatureid,
        gvsloadgraphid = NEW.gvsloadgraphid,
        varcoeffid = NEW.varcoeffid,
        normhlosflow = NEW.normhlosflow,
        normhlosret = NEW.normhlosret,
        calchldep = NEW.calchldep,
        calcinternhddep = NEW.calcinternhddep,
        adjcalchldep = NEW.adjcalchldep,
        adjcalchddep = NEW.adjcalchddep,
        perspcalchldep = NEW.perspcalchldep,
        perspcalcinternhddep = NEW.perspcalcinternhddep,
        calctempdep = NEW.calctempdep,
        connectionschemeid = NEW.connectionschemeid,
        mixfactcoeffdep = NEW.mixfactcoeffdep,
        calchlindep = NEW.calchlindep,
        calcinternhdindep = NEW.calcinternhdindep,
        adjcalchlindep = NEW.adjcalchlindep,
        adjcalcinternhdindep = NEW.adjcalcinternhdindep,
        perspcalchlindep = NEW.perspcalchlindep,
        perspcalcinternhdindep = NEW.perspcalcinternhdindep,
        calctempindep = NEW.calctempindep,
        mixfactcoeffindep = NEW.mixfactcoeffindep,
        calchlventil = NEW.calchlventil,
        adjcalchlventil = NEW.adjcalchlventil,
        perspcalchlventil = NEW.perspcalchlventil,
        calchlcond = NEW.calchlcond,
        adjcalchlcond = NEW.adjcalchlcond,
        perspcalchlcond = NEW.perspcalchlcond,
        calchlclosesys = NEW.calchlclosesys,
        calchlopensysflow = NEW.calchlopensysflow,
        calchlopensysret = NEW.calchlopensysret,
        adjcalchlclosesys = NEW.adjcalchlclosesys,
        adjcalchlopensysflow = NEW.adjcalchlopensysflow,
        adjcalchlopensysret = NEW.adjcalchlopensysret,
        perspcalchlclosesys = NEW.perspcalchlclosesys,
        perspcalchlopensysflow = NEW.perspcalchlopensysflow,
        perspcalchlopensysret = NEW.perspcalchlopensysret,
        expendhwpart = NEW.expendhwpart,
        calctempha = NEW.calctempha,
        calchlparall = NEW.calchlparall,
        internhdparall = NEW.internhdparall,
        adjcalchlparall = NEW.adjcalchlparall,
        adjinternhdparall = NEW.adjinternhdparall,
        perspcalchlparall = NEW.perspcalchlparall,
        perspinternhdparall = NEW.perspinternhdparall,
        schemeparallid = NEW.schemeparallid,
        calchlgvsparall = NEW.calchlgvsparall,
        adjcalchlgvsparall = NEW.adjcalchlgvsparall,
        perspcalchlgvsparall = NEW.perspcalchlgvsparall,
        hourirregcoeffparall = NEW.hourirregcoeffparall,
        avghlcompparall = NEW.avghlcompparall,
        temprecircpipeparall = NEW.temprecircpipeparall,
        calctemphrparall = NEW.calctemphrparall,
        calctemphwdoparall = NEW.calctemphwdoparall,
        amrdepparall = NEW.amrdepparall,
        calchlmix = NEW.calchlmix,
        internhdmix = NEW.internhdmix,
        adjcalchlmix = NEW.adjcalchlmix,
        adjinternhdmix = NEW.adjinternhdmix,
        perspcalchlmix = NEW.perspcalchlmix,
        perspinternhdmix = NEW.perspinternhdmix,
        schememixid = NEW.schememixid,
        calchlgvsmix = NEW.calchlgvsmix,
        adjcalchlgvsmix = NEW.adjcalchlgvsmix,
        perspcalchlgvsmix = NEW.perspcalchlgvsmix,
        hourirregcoeffmix = NEW.hourirregcoeffmix,
        avghlcompmix = NEW.avghlcompmix,
        temprecircpipemix = NEW.temprecircpipemix,
        calctemphrmix = NEW.calctemphrmix,
        calctemphwdomix = NEW.calctemphwdomix,
        amrdepmix = NEW.amrdepmix,
        calchlconseq = NEW.calchlconseq,
        internhdconseq = NEW.internhdconseq,
        adjcalchlconseq = NEW.adjcalchlconseq,
        adjinternhdconseq = NEW.adjinternhdconseq,
        perspcalchlconseq = NEW.perspcalchlconseq,
        perspinternhdconseq = NEW.perspinternhdconseq,
        schemeconseqid = NEW.schemeconseqid,
        calchlgvsconseq = NEW.calchlgvsconseq,
        adjcalchlgvsconseq = NEW.adjcalchlgvsconseq,
        perspcalchlgvsconseq = NEW.perspcalchlgvsconseq,
        hourirregcoeffconseq = NEW.hourirregcoeffconseq,
        avghlcompconseq = NEW.avghlcompconseq,
        temprecircpipeconseq = NEW.temprecircpipeconseq,
        calctemphrconseq = NEW.calctemphrconseq,
        calctemphwdoconseq = NEW.calctemphwdoconseq,
        amrdepconseq = NEW.amrdepconseq,
        calchlpreon = NEW.calchlpreon,
        internhdpreon = NEW.internhdpreon,
        adjcalchlpreon = NEW.adjcalchlpreon,
        adjinternhdpreon = NEW.adjinternhdpreon,
        perspcalchlpreon = NEW.perspcalchlpreon,
        perspinternhdpreon = NEW.perspinternhdpreon,
        schemepreonid = NEW.schemepreonid,
        calchlgvspreon = NEW.calchlgvspreon,
        adjcalchlgvspreon = NEW.adjcalchlgvspreon,
        perspcalchlgvspreon = NEW.perspcalchlgvspreon,
        hourirregcoeffpreon = NEW.hourirregcoeffpreon,
        avghlcomppreon = NEW.avghlcomppreon,
        temprecircpipepreon = NEW.temprecircpipepreon,
        calctemphrpreon = NEW.calctemphrpreon,
        calctemphwdopreon = NEW.calctemphwdopreon,
        amrdeppreon = NEW.amrdeppreon,
        avghlgvsopensysflow = NEW.avghlgvsopensysflow,
        avghlgvsopensysret = NEW.avghlgvsopensysret,
        adjavghlgvsopensysflow = NEW.adjavghlgvsopensysflow,
        adjavghlgvsopensysret = NEW.adjavghlgvsopensysret,
        perspavghlgvsopensysflow = NEW.perspavghlgvsopensysflow,
        perspavghlgvsopensysret = NEW.perspavghlgvsopensysret,
        hourirregcoeffopen = NEW.hourirregcoeffopen,
        avghlcompopen = NEW.avghlcompopen,
        temprecircpipeopen = NEW.temprecircpipeopen,
        calctemphwdoopen = NEW.calctemphwdoopen,
        pdvalveinstalllocid = NEW.pdvalveinstalllocid,
        setpdonregulator = NEW.setpdonregulator,
        setleakageflow = NEW.setleakageflow,
        setleakageret = NEW.setleakageret,
        wemeteringdeviceid = NEW.wemeteringdeviceid,
        volwaterhs = NEW.volwaterhs,
        volwatervs = NEW.volwatervs,
        minthrustds = NEW.minthrustds,
        hydroresclosesys = NEW.hydroresclosesys,
        hydroresclosesyssummer = NEW.hydroresclosesyssummer,
        closesyscalcsignid = NEW.closesyscalcsignid,
        hydroreswdoflow = NEW.hydroreswdoflow,
        calcsignopensysflowid = NEW.calcsignopensysflowid,
        hydroreswdoret = NEW.hydroreswdoret,
        calcsignreswdoretid = NEW.calcsignreswdoretid,
        calcsignsetloadclosesys = NEW.calcsignsetloadclosesys,
        calcsignsetloadopensysflow = NEW.calcsignsetloadopensysflow,
        calcsignsetloadopensysret = NEW.calcsignsetloadopensysret,
        hydrothrustin = NEW.hydrothrustin,
        hydrothrustout = NEW.hydrothrustout,
        calcexpenddep = NEW.calcexpenddep,
        calcexpendindep = NEW.calcexpendindep,
        calcexpendventil = NEW.calcexpendventil,
        calcexpendcond = NEW.calcexpendcond,
        calcexpendhwflow = NEW.calcexpendhwflow,
        calcexpendhwret = NEW.calcexpendhwret,
        calcexpendrecircopen = NEW.calcexpendrecircopen,
        calcexpendhwparall = NEW.calcexpendhwparall,
        calcexpendhwmix = NEW.calcexpendhwmix,
        calcexpendhwconseq = NEW.calcexpendhwconseq,
        calcexpendhwpreon = NEW.calcexpendhwpreon,
        calcthrustinwsd = NEW.calcthrustinwsd,
        consumerstateid = NEW.consumerstateid,
        name = NEW.name
    WHERE id = OLD.nodeid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_generalizedconsumers_upd ON net.v_generalizedconsumers;
CREATE TRIGGER v_generalizedconsumers_upd INSTEAD OF UPDATE ON net.v_generalizedconsumers
    FOR EACH ROW EXECUTE FUNCTION net.v_generalizedconsumers_upd();

CREATE OR REPLACE FUNCTION net.v_generalizedconsumers_del() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    -- Удаление строки подтипа означает снятие типа, а не удаление объекта.
    PERFORM net.reclass_node(OLD.nodeid, 'node_plain');
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_generalizedconsumers_del ON net.v_generalizedconsumers;
CREATE TRIGGER v_generalizedconsumers_del INSTEAD OF DELETE ON net.v_generalizedconsumers
    FOR EACH ROW EXECUTE FUNCTION net.v_generalizedconsumers_del();


-- heatchambers -> net.heat_chamber
-- DROP обязателен: CREATE OR REPLACE VIEW не умеет менять состав
-- и порядок колонок, а они меняются при правке карты типов.
DROP VIEW IF EXISTS net.v_heatchambers CASCADE;
CREATE VIEW net.v_heatchambers AS
SELECT o.subtype_src_id AS id,
       o.id             AS nodeid,
       o.name,
       o.slotscount,
       o.slotsweight,
       o.internalheight,
       o.internalwidth,
       o.internallength,
       o.wallmaterial,
       o.stairscount,
       o.stoparmaturecount,
       o.ballvalvescount,
       o.airventscount
FROM net.heat_chamber o
UNION ALL
-- Строки, не ставшие объектом: дубли и «проигравшие» чужому классу.
-- Узел может иметь строки сразу в двух подтипах — старая модель это
-- допускала, и расчётное ядро читает обе. Без этой части выборка
-- вернула бы меньше строк, чем на исходной БД.
SELECT (v.payload ->> 'id')::int AS id,
       v.obj_id                  AS nodeid,
       (v.payload ->> 'name')::text,
       (v.payload ->> 'slotscount')::integer,
       (v.payload ->> 'slotsweight')::text,
       (v.payload ->> 'internalheight')::double precision,
       (v.payload ->> 'internalwidth')::double precision,
       (v.payload ->> 'internallength')::double precision,
       (v.payload ->> 'wallmaterial')::text,
       (v.payload ->> 'stairscount')::integer,
       (v.payload ->> 'stoparmaturecount')::integer,
       (v.payload ->> 'ballvalvescount')::integer,
       (v.payload ->> 'airventscount')::integer
FROM net.object_variant v
WHERE v.src_table = 'heatchambers' AND NOT v.chosen;

CREATE OR REPLACE FUNCTION net.v_heatchambers_ins() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    IF NEW.nodeid IS NULL THEN
        RAISE EXCEPTION 'heatchambers: не указан nodeid';
    END IF;
    -- Тип объекта задаётся именно этой вставкой: переносим его в класс.
    PERFORM net.reclass_node(NEW.nodeid, 'heat_chamber');
    UPDATE net.heat_chamber SET subtype_src_id = NEW.id,
        name = NEW.name,
        slotscount = NEW.slotscount,
        slotsweight = NEW.slotsweight,
        internalheight = NEW.internalheight,
        internalwidth = NEW.internalwidth,
        internallength = NEW.internallength,
        wallmaterial = NEW.wallmaterial,
        stairscount = NEW.stairscount,
        stoparmaturecount = NEW.stoparmaturecount,
        ballvalvescount = NEW.ballvalvescount,
        airventscount = NEW.airventscount
    WHERE id = NEW.nodeid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_heatchambers_ins ON net.v_heatchambers;
CREATE TRIGGER v_heatchambers_ins INSTEAD OF INSERT ON net.v_heatchambers
    FOR EACH ROW EXECUTE FUNCTION net.v_heatchambers_ins();

CREATE OR REPLACE FUNCTION net.v_heatchambers_upd() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    UPDATE net.heat_chamber SET subtype_src_id = NEW.id,
        name = NEW.name,
        slotscount = NEW.slotscount,
        slotsweight = NEW.slotsweight,
        internalheight = NEW.internalheight,
        internalwidth = NEW.internalwidth,
        internallength = NEW.internallength,
        wallmaterial = NEW.wallmaterial,
        stairscount = NEW.stairscount,
        stoparmaturecount = NEW.stoparmaturecount,
        ballvalvescount = NEW.ballvalvescount,
        airventscount = NEW.airventscount
    WHERE id = OLD.nodeid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_heatchambers_upd ON net.v_heatchambers;
CREATE TRIGGER v_heatchambers_upd INSTEAD OF UPDATE ON net.v_heatchambers
    FOR EACH ROW EXECUTE FUNCTION net.v_heatchambers_upd();

CREATE OR REPLACE FUNCTION net.v_heatchambers_del() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    -- Удаление строки подтипа означает снятие типа, а не удаление объекта.
    PERFORM net.reclass_node(OLD.nodeid, 'node_plain');
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_heatchambers_del ON net.v_heatchambers;
CREATE TRIGGER v_heatchambers_del INSTEAD OF DELETE ON net.v_heatchambers
    FOR EACH ROW EXECUTE FUNCTION net.v_heatchambers_del();


-- heatsources -> net.heat_source
-- DROP обязателен: CREATE OR REPLACE VIEW не умеет менять состав
-- и порядок колонок, а они меняются при правке карты типов.
DROP VIEW IF EXISTS net.v_heatsources CASCADE;
CREATE VIEW net.v_heatsources AS
SELECT o.subtype_src_id AS id,
       o.id             AS nodeid,
       o.sourcename,
       o.name,
       o.stateid,
       o.hsourcetypeid,
       o.hsourceid,
       o.hsourcepower,
       o.hsourcepowerinst,
       o.hseasonbegindate,
       o.hseasonenddate,
       o.hsourcecode,
       o.temperdwflowsummer,
       o.temperdwretsummer,
       o.specvolhs,
       o.specvolvent,
       o.expenddwnorm1,
       o.expenddwnorm2,
       o.expenddwnorm3,
       o.expenddwnorm4,
       o.expenddwnorm5,
       o.managerphone,
       o.controllerphone,
       o.powerset,
       o.poweravailable,
       o.heighttubemark,
       o.heightareamark,
       o.repairworks,
       o.t1_summer,
       o.t2_summer,
       o.name_tg,
       o.heatloscalcyear,
       o.graphtypeid,
       o.tn_1,
       o.tn_5,
       o.tvn_r,
       o.t1_r,
       o.t2_r,
       o.t3_r,
       o.q_r,
       o.t1_2r,
       o.t1_4r,
       o.tvb_tr,
       o.uf,
       o.t2_2r,
       o.q_gv,
       o.tg_r,
       o.tx_r,
       o.t2_gv,
       o.pr,
       o.g1,
       o.g2,
       o.t_gv1,
       o.deployedtempgraphid,
       o.v,
       o.date_on,
       o.name_exe,
       o.name_manager,
       o.dt2_co,
       o.t2pod_parl,
       o.dt2v_sm_noavm,
       o.dt2v_sm_avm,
       o.t1pod_sm,
       o.t1pod_posll,
       o.dt2v_posl,
       o.length_1,
       o.length_2,
       o.length_3,
       o.length_4,
       o.length_5,
       o.length_6,
       o.length_7,
       o.length_8,
       o.length_9,
       o.length_10,
       o.length_11,
       o.length_12,
       o.t_1,
       o.t_2,
       o.t_3,
       o.t_4,
       o.t_5,
       o.t_6,
       o.t_7,
       o.t_8,
       o.t_9,
       o.t_10,
       o.t_11,
       o.t_12,
       o.t1_1,
       o.t1_2,
       o.t1_3,
       o.t1_4,
       o.t1_5,
       o.t1_6,
       o.t1_7,
       o.t1_8,
       o.t1_9,
       o.t1_10,
       o.t1_11,
       o.t1_12,
       o.t2_1,
       o.t2_2,
       o.t2_3,
       o.t2_4,
       o.t2_5,
       o.t2_6,
       o.t2_7,
       o.t2_8,
       o.t2_9,
       o.t2_10,
       o.t2_11,
       o.t2_12,
       o.length_1_fakt,
       o.length_2_fakt,
       o.length_3_fakt,
       o.length_4_fakt,
       o.length_5_fakt,
       o.length_6_fakt,
       o.length_7_fakt,
       o.length_8_fakt,
       o.length_9_fakt,
       o.length_10_fakt,
       o.length_11_fakt,
       o.length_12_fakt,
       o.t_1_fakt,
       o.t_2_fakt,
       o.t_3_fakt,
       o.t_4_fakt,
       o.t_5_fakt,
       o.t_6_fakt,
       o.t_7_fakt,
       o.t_8_fakt,
       o.t_9_fakt,
       o.t_10_fakt,
       o.t_11_fakt,
       o.t_12_fakt,
       o.t1_1_fakt,
       o.t1_2_fakt,
       o.t1_3_fakt,
       o.t1_4_fakt,
       o.t1_5_fakt,
       o.t1_6_fakt,
       o.t1_7_fakt,
       o.t1_8_fakt,
       o.t1_9_fakt,
       o.t1_10_fakt,
       o.t1_11_fakt,
       o.t1_12_fakt,
       o.t2_1_fakt,
       o.t2_2_fakt,
       o.t2_3_fakt,
       o.t2_4_fakt,
       o.t2_5_fakt,
       o.t2_6_fakt,
       o.t2_7_fakt,
       o.t2_8_fakt,
       o.t2_9_fakt,
       o.t2_10_fakt,
       o.t2_11_fakt,
       o.t2_12_fakt,
       o.gpod_1_fakt,
       o.gpod_2_fakt,
       o.gpod_3_fakt,
       o.gpod_4_fakt,
       o.gpod_5_fakt,
       o.gpod_6_fakt,
       o.gpod_7_fakt,
       o.gpod_8_fakt,
       o.gpod_9_fakt,
       o.gpod_10_fakt,
       o.gpod_11_fakt,
       o.gpod_12_fakt,
       o.ggvs_pribor_1_fakt,
       o.ggvs_pribor_2_fakt,
       o.ggvs_pribor_3_fakt,
       o.ggvs_pribor_4_fakt,
       o.ggvs_pribor_5_fakt,
       o.ggvs_pribor_6_fakt,
       o.ggvs_pribor_7_fakt,
       o.ggvs_pribor_8_fakt,
       o.ggvs_pribor_9_fakt,
       o.ggvs_pribor_10_fakt,
       o.ggvs_pribor_11_fakt,
       o.ggvs_pribor_12_fakt,
       o.gakt_tex_1_fakt,
       o.gakt_tex_2_fakt,
       o.gakt_tex_3_fakt,
       o.gakt_tex_4_fakt,
       o.gakt_tex_5_fakt,
       o.gakt_tex_6_fakt,
       o.gakt_tex_7_fakt,
       o.gakt_tex_8_fakt,
       o.gakt_tex_9_fakt,
       o.gakt_tex_10_fakt,
       o.gakt_tex_11_fakt,
       o.gakt_tex_12_fakt,
       o.gakt_avarija_1_fakt,
       o.gakt_avarija_2_fakt,
       o.gakt_avarija_3_fakt,
       o.gakt_avarija_4_fakt,
       o.gakt_avarija_5_fakt,
       o.gakt_avarija_6_fakt,
       o.gakt_avarija_7_fakt,
       o.gakt_avarija_8_fakt,
       o.gakt_avarija_9_fakt,
       o.gakt_avarija_10_fakt,
       o.gakt_avarija_11_fakt,
       o.gakt_avarija_12_fakt,
       o.id_old
FROM net.heat_source o
UNION ALL
-- Строки, не ставшие объектом: дубли и «проигравшие» чужому классу.
-- Узел может иметь строки сразу в двух подтипах — старая модель это
-- допускала, и расчётное ядро читает обе. Без этой части выборка
-- вернула бы меньше строк, чем на исходной БД.
SELECT (v.payload ->> 'id')::int AS id,
       v.obj_id                  AS nodeid,
       (v.payload ->> 'sourcename')::text,
       (v.payload ->> 'name')::text,
       (v.payload ->> 'stateid')::integer,
       (v.payload ->> 'hsourcetypeid')::integer,
       (v.payload ->> 'hsourceid')::integer,
       (v.payload ->> 'hsourcepower')::double precision,
       (v.payload ->> 'hsourcepowerinst')::double precision,
       (v.payload ->> 'hseasonbegindate')::date,
       (v.payload ->> 'hseasonenddate')::date,
       (v.payload ->> 'hsourcecode')::text,
       (v.payload ->> 'temperdwflowsummer')::double precision,
       (v.payload ->> 'temperdwretsummer')::double precision,
       (v.payload ->> 'specvolhs')::double precision,
       (v.payload ->> 'specvolvent')::double precision,
       (v.payload ->> 'expenddwnorm1')::double precision,
       (v.payload ->> 'expenddwnorm2')::double precision,
       (v.payload ->> 'expenddwnorm3')::double precision,
       (v.payload ->> 'expenddwnorm4')::double precision,
       (v.payload ->> 'expenddwnorm5')::double precision,
       (v.payload ->> 'managerphone')::integer,
       (v.payload ->> 'controllerphone')::integer,
       (v.payload ->> 'powerset')::double precision,
       (v.payload ->> 'poweravailable')::double precision,
       (v.payload ->> 'heighttubemark')::integer,
       (v.payload ->> 'heightareamark')::integer,
       (v.payload ->> 'repairworks')::double precision,
       (v.payload ->> 't1_summer')::double precision,
       (v.payload ->> 't2_summer')::double precision,
       (v.payload ->> 'name_tg')::text,
       (v.payload ->> 'heatloscalcyear')::text,
       (v.payload ->> 'graphtypeid')::integer,
       (v.payload ->> 'tn_1')::double precision,
       (v.payload ->> 'tn_5')::double precision,
       (v.payload ->> 'tvn_r')::double precision,
       (v.payload ->> 't1_r')::double precision,
       (v.payload ->> 't2_r')::double precision,
       (v.payload ->> 't3_r')::double precision,
       (v.payload ->> 'q_r')::double precision,
       (v.payload ->> 't1_2r')::double precision,
       (v.payload ->> 't1_4r')::double precision,
       (v.payload ->> 'tvb_tr')::double precision,
       (v.payload ->> 'uf')::double precision,
       (v.payload ->> 't2_2r')::double precision,
       (v.payload ->> 'q_gv')::double precision,
       (v.payload ->> 'tg_r')::double precision,
       (v.payload ->> 'tx_r')::double precision,
       (v.payload ->> 't2_gv')::double precision,
       (v.payload ->> 'pr')::integer,
       (v.payload ->> 'g1')::double precision,
       (v.payload ->> 'g2')::double precision,
       (v.payload ->> 't_gv1')::double precision,
       (v.payload ->> 'deployedtempgraphid')::integer,
       (v.payload ->> 'v')::double precision,
       (v.payload ->> 'date_on')::double precision,
       (v.payload ->> 'name_exe')::text,
       (v.payload ->> 'name_manager')::text,
       (v.payload ->> 'dt2_co')::double precision,
       (v.payload ->> 't2pod_parl')::double precision,
       (v.payload ->> 'dt2v_sm_noavm')::double precision,
       (v.payload ->> 'dt2v_sm_avm')::double precision,
       (v.payload ->> 't1pod_sm')::double precision,
       (v.payload ->> 't1pod_posll')::double precision,
       (v.payload ->> 'dt2v_posl')::double precision,
       (v.payload ->> 'length_1')::integer,
       (v.payload ->> 'length_2')::integer,
       (v.payload ->> 'length_3')::integer,
       (v.payload ->> 'length_4')::integer,
       (v.payload ->> 'length_5')::integer,
       (v.payload ->> 'length_6')::integer,
       (v.payload ->> 'length_7')::integer,
       (v.payload ->> 'length_8')::integer,
       (v.payload ->> 'length_9')::integer,
       (v.payload ->> 'length_10')::integer,
       (v.payload ->> 'length_11')::integer,
       (v.payload ->> 'length_12')::integer,
       (v.payload ->> 't_1')::double precision,
       (v.payload ->> 't_2')::double precision,
       (v.payload ->> 't_3')::double precision,
       (v.payload ->> 't_4')::double precision,
       (v.payload ->> 't_5')::double precision,
       (v.payload ->> 't_6')::double precision,
       (v.payload ->> 't_7')::double precision,
       (v.payload ->> 't_8')::double precision,
       (v.payload ->> 't_9')::double precision,
       (v.payload ->> 't_10')::double precision,
       (v.payload ->> 't_11')::double precision,
       (v.payload ->> 't_12')::double precision,
       (v.payload ->> 't1_1')::double precision,
       (v.payload ->> 't1_2')::double precision,
       (v.payload ->> 't1_3')::double precision,
       (v.payload ->> 't1_4')::double precision,
       (v.payload ->> 't1_5')::double precision,
       (v.payload ->> 't1_6')::double precision,
       (v.payload ->> 't1_7')::double precision,
       (v.payload ->> 't1_8')::double precision,
       (v.payload ->> 't1_9')::double precision,
       (v.payload ->> 't1_10')::double precision,
       (v.payload ->> 't1_11')::double precision,
       (v.payload ->> 't1_12')::double precision,
       (v.payload ->> 't2_1')::double precision,
       (v.payload ->> 't2_2')::double precision,
       (v.payload ->> 't2_3')::double precision,
       (v.payload ->> 't2_4')::double precision,
       (v.payload ->> 't2_5')::double precision,
       (v.payload ->> 't2_6')::double precision,
       (v.payload ->> 't2_7')::double precision,
       (v.payload ->> 't2_8')::double precision,
       (v.payload ->> 't2_9')::double precision,
       (v.payload ->> 't2_10')::double precision,
       (v.payload ->> 't2_11')::double precision,
       (v.payload ->> 't2_12')::double precision,
       (v.payload ->> 'length_1_fakt')::integer,
       (v.payload ->> 'length_2_fakt')::integer,
       (v.payload ->> 'length_3_fakt')::integer,
       (v.payload ->> 'length_4_fakt')::integer,
       (v.payload ->> 'length_5_fakt')::integer,
       (v.payload ->> 'length_6_fakt')::integer,
       (v.payload ->> 'length_7_fakt')::integer,
       (v.payload ->> 'length_8_fakt')::integer,
       (v.payload ->> 'length_9_fakt')::integer,
       (v.payload ->> 'length_10_fakt')::integer,
       (v.payload ->> 'length_11_fakt')::integer,
       (v.payload ->> 'length_12_fakt')::integer,
       (v.payload ->> 't_1_fakt')::double precision,
       (v.payload ->> 't_2_fakt')::double precision,
       (v.payload ->> 't_3_fakt')::double precision,
       (v.payload ->> 't_4_fakt')::double precision,
       (v.payload ->> 't_5_fakt')::double precision,
       (v.payload ->> 't_6_fakt')::double precision,
       (v.payload ->> 't_7_fakt')::double precision,
       (v.payload ->> 't_8_fakt')::double precision,
       (v.payload ->> 't_9_fakt')::double precision,
       (v.payload ->> 't_10_fakt')::double precision,
       (v.payload ->> 't_11_fakt')::double precision,
       (v.payload ->> 't_12_fakt')::double precision,
       (v.payload ->> 't1_1_fakt')::double precision,
       (v.payload ->> 't1_2_fakt')::double precision,
       (v.payload ->> 't1_3_fakt')::double precision,
       (v.payload ->> 't1_4_fakt')::double precision,
       (v.payload ->> 't1_5_fakt')::double precision,
       (v.payload ->> 't1_6_fakt')::double precision,
       (v.payload ->> 't1_7_fakt')::double precision,
       (v.payload ->> 't1_8_fakt')::double precision,
       (v.payload ->> 't1_9_fakt')::double precision,
       (v.payload ->> 't1_10_fakt')::double precision,
       (v.payload ->> 't1_11_fakt')::double precision,
       (v.payload ->> 't1_12_fakt')::double precision,
       (v.payload ->> 't2_1_fakt')::double precision,
       (v.payload ->> 't2_2_fakt')::double precision,
       (v.payload ->> 't2_3_fakt')::double precision,
       (v.payload ->> 't2_4_fakt')::double precision,
       (v.payload ->> 't2_5_fakt')::double precision,
       (v.payload ->> 't2_6_fakt')::double precision,
       (v.payload ->> 't2_7_fakt')::double precision,
       (v.payload ->> 't2_8_fakt')::double precision,
       (v.payload ->> 't2_9_fakt')::double precision,
       (v.payload ->> 't2_10_fakt')::double precision,
       (v.payload ->> 't2_11_fakt')::double precision,
       (v.payload ->> 't2_12_fakt')::double precision,
       (v.payload ->> 'gpod_1_fakt')::double precision,
       (v.payload ->> 'gpod_2_fakt')::double precision,
       (v.payload ->> 'gpod_3_fakt')::double precision,
       (v.payload ->> 'gpod_4_fakt')::double precision,
       (v.payload ->> 'gpod_5_fakt')::double precision,
       (v.payload ->> 'gpod_6_fakt')::double precision,
       (v.payload ->> 'gpod_7_fakt')::double precision,
       (v.payload ->> 'gpod_8_fakt')::double precision,
       (v.payload ->> 'gpod_9_fakt')::double precision,
       (v.payload ->> 'gpod_10_fakt')::double precision,
       (v.payload ->> 'gpod_11_fakt')::double precision,
       (v.payload ->> 'gpod_12_fakt')::double precision,
       (v.payload ->> 'ggvs_pribor_1_fakt')::double precision,
       (v.payload ->> 'ggvs_pribor_2_fakt')::double precision,
       (v.payload ->> 'ggvs_pribor_3_fakt')::double precision,
       (v.payload ->> 'ggvs_pribor_4_fakt')::double precision,
       (v.payload ->> 'ggvs_pribor_5_fakt')::double precision,
       (v.payload ->> 'ggvs_pribor_6_fakt')::double precision,
       (v.payload ->> 'ggvs_pribor_7_fakt')::double precision,
       (v.payload ->> 'ggvs_pribor_8_fakt')::double precision,
       (v.payload ->> 'ggvs_pribor_9_fakt')::double precision,
       (v.payload ->> 'ggvs_pribor_10_fakt')::double precision,
       (v.payload ->> 'ggvs_pribor_11_fakt')::double precision,
       (v.payload ->> 'ggvs_pribor_12_fakt')::double precision,
       (v.payload ->> 'gakt_tex_1_fakt')::double precision,
       (v.payload ->> 'gakt_tex_2_fakt')::double precision,
       (v.payload ->> 'gakt_tex_3_fakt')::double precision,
       (v.payload ->> 'gakt_tex_4_fakt')::double precision,
       (v.payload ->> 'gakt_tex_5_fakt')::double precision,
       (v.payload ->> 'gakt_tex_6_fakt')::double precision,
       (v.payload ->> 'gakt_tex_7_fakt')::double precision,
       (v.payload ->> 'gakt_tex_8_fakt')::double precision,
       (v.payload ->> 'gakt_tex_9_fakt')::double precision,
       (v.payload ->> 'gakt_tex_10_fakt')::double precision,
       (v.payload ->> 'gakt_tex_11_fakt')::double precision,
       (v.payload ->> 'gakt_tex_12_fakt')::double precision,
       (v.payload ->> 'gakt_avarija_1_fakt')::double precision,
       (v.payload ->> 'gakt_avarija_2_fakt')::double precision,
       (v.payload ->> 'gakt_avarija_3_fakt')::double precision,
       (v.payload ->> 'gakt_avarija_4_fakt')::double precision,
       (v.payload ->> 'gakt_avarija_5_fakt')::double precision,
       (v.payload ->> 'gakt_avarija_6_fakt')::double precision,
       (v.payload ->> 'gakt_avarija_7_fakt')::double precision,
       (v.payload ->> 'gakt_avarija_8_fakt')::double precision,
       (v.payload ->> 'gakt_avarija_9_fakt')::double precision,
       (v.payload ->> 'gakt_avarija_10_fakt')::double precision,
       (v.payload ->> 'gakt_avarija_11_fakt')::double precision,
       (v.payload ->> 'gakt_avarija_12_fakt')::double precision,
       (v.payload ->> 'id_old')::integer
FROM net.object_variant v
WHERE v.src_table = 'heatsources' AND NOT v.chosen;

CREATE OR REPLACE FUNCTION net.v_heatsources_ins() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    IF NEW.nodeid IS NULL THEN
        RAISE EXCEPTION 'heatsources: не указан nodeid';
    END IF;
    -- Тип объекта задаётся именно этой вставкой: переносим его в класс.
    PERFORM net.reclass_node(NEW.nodeid, 'heat_source');
    UPDATE net.heat_source SET subtype_src_id = NEW.id,
        sourcename = NEW.sourcename,
        name = NEW.name,
        stateid = NEW.stateid,
        hsourcetypeid = NEW.hsourcetypeid,
        hsourceid = NEW.hsourceid,
        hsourcepower = NEW.hsourcepower,
        hsourcepowerinst = NEW.hsourcepowerinst,
        hseasonbegindate = NEW.hseasonbegindate,
        hseasonenddate = NEW.hseasonenddate,
        hsourcecode = NEW.hsourcecode,
        temperdwflowsummer = NEW.temperdwflowsummer,
        temperdwretsummer = NEW.temperdwretsummer,
        specvolhs = NEW.specvolhs,
        specvolvent = NEW.specvolvent,
        expenddwnorm1 = NEW.expenddwnorm1,
        expenddwnorm2 = NEW.expenddwnorm2,
        expenddwnorm3 = NEW.expenddwnorm3,
        expenddwnorm4 = NEW.expenddwnorm4,
        expenddwnorm5 = NEW.expenddwnorm5,
        managerphone = NEW.managerphone,
        controllerphone = NEW.controllerphone,
        powerset = NEW.powerset,
        poweravailable = NEW.poweravailable,
        heighttubemark = NEW.heighttubemark,
        heightareamark = NEW.heightareamark,
        repairworks = NEW.repairworks,
        t1_summer = NEW.t1_summer,
        t2_summer = NEW.t2_summer,
        name_tg = NEW.name_tg,
        heatloscalcyear = NEW.heatloscalcyear,
        graphtypeid = NEW.graphtypeid,
        tn_1 = NEW.tn_1,
        tn_5 = NEW.tn_5,
        tvn_r = NEW.tvn_r,
        t1_r = NEW.t1_r,
        t2_r = NEW.t2_r,
        t3_r = NEW.t3_r,
        q_r = NEW.q_r,
        t1_2r = NEW.t1_2r,
        t1_4r = NEW.t1_4r,
        tvb_tr = NEW.tvb_tr,
        uf = NEW.uf,
        t2_2r = NEW.t2_2r,
        q_gv = NEW.q_gv,
        tg_r = NEW.tg_r,
        tx_r = NEW.tx_r,
        t2_gv = NEW.t2_gv,
        pr = NEW.pr,
        g1 = NEW.g1,
        g2 = NEW.g2,
        t_gv1 = NEW.t_gv1,
        deployedtempgraphid = NEW.deployedtempgraphid,
        v = NEW.v,
        date_on = NEW.date_on,
        name_exe = NEW.name_exe,
        name_manager = NEW.name_manager,
        dt2_co = NEW.dt2_co,
        t2pod_parl = NEW.t2pod_parl,
        dt2v_sm_noavm = NEW.dt2v_sm_noavm,
        dt2v_sm_avm = NEW.dt2v_sm_avm,
        t1pod_sm = NEW.t1pod_sm,
        t1pod_posll = NEW.t1pod_posll,
        dt2v_posl = NEW.dt2v_posl,
        length_1 = NEW.length_1,
        length_2 = NEW.length_2,
        length_3 = NEW.length_3,
        length_4 = NEW.length_4,
        length_5 = NEW.length_5,
        length_6 = NEW.length_6,
        length_7 = NEW.length_7,
        length_8 = NEW.length_8,
        length_9 = NEW.length_9,
        length_10 = NEW.length_10,
        length_11 = NEW.length_11,
        length_12 = NEW.length_12,
        t_1 = NEW.t_1,
        t_2 = NEW.t_2,
        t_3 = NEW.t_3,
        t_4 = NEW.t_4,
        t_5 = NEW.t_5,
        t_6 = NEW.t_6,
        t_7 = NEW.t_7,
        t_8 = NEW.t_8,
        t_9 = NEW.t_9,
        t_10 = NEW.t_10,
        t_11 = NEW.t_11,
        t_12 = NEW.t_12,
        t1_1 = NEW.t1_1,
        t1_2 = NEW.t1_2,
        t1_3 = NEW.t1_3,
        t1_4 = NEW.t1_4,
        t1_5 = NEW.t1_5,
        t1_6 = NEW.t1_6,
        t1_7 = NEW.t1_7,
        t1_8 = NEW.t1_8,
        t1_9 = NEW.t1_9,
        t1_10 = NEW.t1_10,
        t1_11 = NEW.t1_11,
        t1_12 = NEW.t1_12,
        t2_1 = NEW.t2_1,
        t2_2 = NEW.t2_2,
        t2_3 = NEW.t2_3,
        t2_4 = NEW.t2_4,
        t2_5 = NEW.t2_5,
        t2_6 = NEW.t2_6,
        t2_7 = NEW.t2_7,
        t2_8 = NEW.t2_8,
        t2_9 = NEW.t2_9,
        t2_10 = NEW.t2_10,
        t2_11 = NEW.t2_11,
        t2_12 = NEW.t2_12,
        length_1_fakt = NEW.length_1_fakt,
        length_2_fakt = NEW.length_2_fakt,
        length_3_fakt = NEW.length_3_fakt,
        length_4_fakt = NEW.length_4_fakt,
        length_5_fakt = NEW.length_5_fakt,
        length_6_fakt = NEW.length_6_fakt,
        length_7_fakt = NEW.length_7_fakt,
        length_8_fakt = NEW.length_8_fakt,
        length_9_fakt = NEW.length_9_fakt,
        length_10_fakt = NEW.length_10_fakt,
        length_11_fakt = NEW.length_11_fakt,
        length_12_fakt = NEW.length_12_fakt,
        t_1_fakt = NEW.t_1_fakt,
        t_2_fakt = NEW.t_2_fakt,
        t_3_fakt = NEW.t_3_fakt,
        t_4_fakt = NEW.t_4_fakt,
        t_5_fakt = NEW.t_5_fakt,
        t_6_fakt = NEW.t_6_fakt,
        t_7_fakt = NEW.t_7_fakt,
        t_8_fakt = NEW.t_8_fakt,
        t_9_fakt = NEW.t_9_fakt,
        t_10_fakt = NEW.t_10_fakt,
        t_11_fakt = NEW.t_11_fakt,
        t_12_fakt = NEW.t_12_fakt,
        t1_1_fakt = NEW.t1_1_fakt,
        t1_2_fakt = NEW.t1_2_fakt,
        t1_3_fakt = NEW.t1_3_fakt,
        t1_4_fakt = NEW.t1_4_fakt,
        t1_5_fakt = NEW.t1_5_fakt,
        t1_6_fakt = NEW.t1_6_fakt,
        t1_7_fakt = NEW.t1_7_fakt,
        t1_8_fakt = NEW.t1_8_fakt,
        t1_9_fakt = NEW.t1_9_fakt,
        t1_10_fakt = NEW.t1_10_fakt,
        t1_11_fakt = NEW.t1_11_fakt,
        t1_12_fakt = NEW.t1_12_fakt,
        t2_1_fakt = NEW.t2_1_fakt,
        t2_2_fakt = NEW.t2_2_fakt,
        t2_3_fakt = NEW.t2_3_fakt,
        t2_4_fakt = NEW.t2_4_fakt,
        t2_5_fakt = NEW.t2_5_fakt,
        t2_6_fakt = NEW.t2_6_fakt,
        t2_7_fakt = NEW.t2_7_fakt,
        t2_8_fakt = NEW.t2_8_fakt,
        t2_9_fakt = NEW.t2_9_fakt,
        t2_10_fakt = NEW.t2_10_fakt,
        t2_11_fakt = NEW.t2_11_fakt,
        t2_12_fakt = NEW.t2_12_fakt,
        gpod_1_fakt = NEW.gpod_1_fakt,
        gpod_2_fakt = NEW.gpod_2_fakt,
        gpod_3_fakt = NEW.gpod_3_fakt,
        gpod_4_fakt = NEW.gpod_4_fakt,
        gpod_5_fakt = NEW.gpod_5_fakt,
        gpod_6_fakt = NEW.gpod_6_fakt,
        gpod_7_fakt = NEW.gpod_7_fakt,
        gpod_8_fakt = NEW.gpod_8_fakt,
        gpod_9_fakt = NEW.gpod_9_fakt,
        gpod_10_fakt = NEW.gpod_10_fakt,
        gpod_11_fakt = NEW.gpod_11_fakt,
        gpod_12_fakt = NEW.gpod_12_fakt,
        ggvs_pribor_1_fakt = NEW.ggvs_pribor_1_fakt,
        ggvs_pribor_2_fakt = NEW.ggvs_pribor_2_fakt,
        ggvs_pribor_3_fakt = NEW.ggvs_pribor_3_fakt,
        ggvs_pribor_4_fakt = NEW.ggvs_pribor_4_fakt,
        ggvs_pribor_5_fakt = NEW.ggvs_pribor_5_fakt,
        ggvs_pribor_6_fakt = NEW.ggvs_pribor_6_fakt,
        ggvs_pribor_7_fakt = NEW.ggvs_pribor_7_fakt,
        ggvs_pribor_8_fakt = NEW.ggvs_pribor_8_fakt,
        ggvs_pribor_9_fakt = NEW.ggvs_pribor_9_fakt,
        ggvs_pribor_10_fakt = NEW.ggvs_pribor_10_fakt,
        ggvs_pribor_11_fakt = NEW.ggvs_pribor_11_fakt,
        ggvs_pribor_12_fakt = NEW.ggvs_pribor_12_fakt,
        gakt_tex_1_fakt = NEW.gakt_tex_1_fakt,
        gakt_tex_2_fakt = NEW.gakt_tex_2_fakt,
        gakt_tex_3_fakt = NEW.gakt_tex_3_fakt,
        gakt_tex_4_fakt = NEW.gakt_tex_4_fakt,
        gakt_tex_5_fakt = NEW.gakt_tex_5_fakt,
        gakt_tex_6_fakt = NEW.gakt_tex_6_fakt,
        gakt_tex_7_fakt = NEW.gakt_tex_7_fakt,
        gakt_tex_8_fakt = NEW.gakt_tex_8_fakt,
        gakt_tex_9_fakt = NEW.gakt_tex_9_fakt,
        gakt_tex_10_fakt = NEW.gakt_tex_10_fakt,
        gakt_tex_11_fakt = NEW.gakt_tex_11_fakt,
        gakt_tex_12_fakt = NEW.gakt_tex_12_fakt,
        gakt_avarija_1_fakt = NEW.gakt_avarija_1_fakt,
        gakt_avarija_2_fakt = NEW.gakt_avarija_2_fakt,
        gakt_avarija_3_fakt = NEW.gakt_avarija_3_fakt,
        gakt_avarija_4_fakt = NEW.gakt_avarija_4_fakt,
        gakt_avarija_5_fakt = NEW.gakt_avarija_5_fakt,
        gakt_avarija_6_fakt = NEW.gakt_avarija_6_fakt,
        gakt_avarija_7_fakt = NEW.gakt_avarija_7_fakt,
        gakt_avarija_8_fakt = NEW.gakt_avarija_8_fakt,
        gakt_avarija_9_fakt = NEW.gakt_avarija_9_fakt,
        gakt_avarija_10_fakt = NEW.gakt_avarija_10_fakt,
        gakt_avarija_11_fakt = NEW.gakt_avarija_11_fakt,
        gakt_avarija_12_fakt = NEW.gakt_avarija_12_fakt,
        id_old = NEW.id_old
    WHERE id = NEW.nodeid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_heatsources_ins ON net.v_heatsources;
CREATE TRIGGER v_heatsources_ins INSTEAD OF INSERT ON net.v_heatsources
    FOR EACH ROW EXECUTE FUNCTION net.v_heatsources_ins();

CREATE OR REPLACE FUNCTION net.v_heatsources_upd() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    UPDATE net.heat_source SET subtype_src_id = NEW.id,
        sourcename = NEW.sourcename,
        name = NEW.name,
        stateid = NEW.stateid,
        hsourcetypeid = NEW.hsourcetypeid,
        hsourceid = NEW.hsourceid,
        hsourcepower = NEW.hsourcepower,
        hsourcepowerinst = NEW.hsourcepowerinst,
        hseasonbegindate = NEW.hseasonbegindate,
        hseasonenddate = NEW.hseasonenddate,
        hsourcecode = NEW.hsourcecode,
        temperdwflowsummer = NEW.temperdwflowsummer,
        temperdwretsummer = NEW.temperdwretsummer,
        specvolhs = NEW.specvolhs,
        specvolvent = NEW.specvolvent,
        expenddwnorm1 = NEW.expenddwnorm1,
        expenddwnorm2 = NEW.expenddwnorm2,
        expenddwnorm3 = NEW.expenddwnorm3,
        expenddwnorm4 = NEW.expenddwnorm4,
        expenddwnorm5 = NEW.expenddwnorm5,
        managerphone = NEW.managerphone,
        controllerphone = NEW.controllerphone,
        powerset = NEW.powerset,
        poweravailable = NEW.poweravailable,
        heighttubemark = NEW.heighttubemark,
        heightareamark = NEW.heightareamark,
        repairworks = NEW.repairworks,
        t1_summer = NEW.t1_summer,
        t2_summer = NEW.t2_summer,
        name_tg = NEW.name_tg,
        heatloscalcyear = NEW.heatloscalcyear,
        graphtypeid = NEW.graphtypeid,
        tn_1 = NEW.tn_1,
        tn_5 = NEW.tn_5,
        tvn_r = NEW.tvn_r,
        t1_r = NEW.t1_r,
        t2_r = NEW.t2_r,
        t3_r = NEW.t3_r,
        q_r = NEW.q_r,
        t1_2r = NEW.t1_2r,
        t1_4r = NEW.t1_4r,
        tvb_tr = NEW.tvb_tr,
        uf = NEW.uf,
        t2_2r = NEW.t2_2r,
        q_gv = NEW.q_gv,
        tg_r = NEW.tg_r,
        tx_r = NEW.tx_r,
        t2_gv = NEW.t2_gv,
        pr = NEW.pr,
        g1 = NEW.g1,
        g2 = NEW.g2,
        t_gv1 = NEW.t_gv1,
        deployedtempgraphid = NEW.deployedtempgraphid,
        v = NEW.v,
        date_on = NEW.date_on,
        name_exe = NEW.name_exe,
        name_manager = NEW.name_manager,
        dt2_co = NEW.dt2_co,
        t2pod_parl = NEW.t2pod_parl,
        dt2v_sm_noavm = NEW.dt2v_sm_noavm,
        dt2v_sm_avm = NEW.dt2v_sm_avm,
        t1pod_sm = NEW.t1pod_sm,
        t1pod_posll = NEW.t1pod_posll,
        dt2v_posl = NEW.dt2v_posl,
        length_1 = NEW.length_1,
        length_2 = NEW.length_2,
        length_3 = NEW.length_3,
        length_4 = NEW.length_4,
        length_5 = NEW.length_5,
        length_6 = NEW.length_6,
        length_7 = NEW.length_7,
        length_8 = NEW.length_8,
        length_9 = NEW.length_9,
        length_10 = NEW.length_10,
        length_11 = NEW.length_11,
        length_12 = NEW.length_12,
        t_1 = NEW.t_1,
        t_2 = NEW.t_2,
        t_3 = NEW.t_3,
        t_4 = NEW.t_4,
        t_5 = NEW.t_5,
        t_6 = NEW.t_6,
        t_7 = NEW.t_7,
        t_8 = NEW.t_8,
        t_9 = NEW.t_9,
        t_10 = NEW.t_10,
        t_11 = NEW.t_11,
        t_12 = NEW.t_12,
        t1_1 = NEW.t1_1,
        t1_2 = NEW.t1_2,
        t1_3 = NEW.t1_3,
        t1_4 = NEW.t1_4,
        t1_5 = NEW.t1_5,
        t1_6 = NEW.t1_6,
        t1_7 = NEW.t1_7,
        t1_8 = NEW.t1_8,
        t1_9 = NEW.t1_9,
        t1_10 = NEW.t1_10,
        t1_11 = NEW.t1_11,
        t1_12 = NEW.t1_12,
        t2_1 = NEW.t2_1,
        t2_2 = NEW.t2_2,
        t2_3 = NEW.t2_3,
        t2_4 = NEW.t2_4,
        t2_5 = NEW.t2_5,
        t2_6 = NEW.t2_6,
        t2_7 = NEW.t2_7,
        t2_8 = NEW.t2_8,
        t2_9 = NEW.t2_9,
        t2_10 = NEW.t2_10,
        t2_11 = NEW.t2_11,
        t2_12 = NEW.t2_12,
        length_1_fakt = NEW.length_1_fakt,
        length_2_fakt = NEW.length_2_fakt,
        length_3_fakt = NEW.length_3_fakt,
        length_4_fakt = NEW.length_4_fakt,
        length_5_fakt = NEW.length_5_fakt,
        length_6_fakt = NEW.length_6_fakt,
        length_7_fakt = NEW.length_7_fakt,
        length_8_fakt = NEW.length_8_fakt,
        length_9_fakt = NEW.length_9_fakt,
        length_10_fakt = NEW.length_10_fakt,
        length_11_fakt = NEW.length_11_fakt,
        length_12_fakt = NEW.length_12_fakt,
        t_1_fakt = NEW.t_1_fakt,
        t_2_fakt = NEW.t_2_fakt,
        t_3_fakt = NEW.t_3_fakt,
        t_4_fakt = NEW.t_4_fakt,
        t_5_fakt = NEW.t_5_fakt,
        t_6_fakt = NEW.t_6_fakt,
        t_7_fakt = NEW.t_7_fakt,
        t_8_fakt = NEW.t_8_fakt,
        t_9_fakt = NEW.t_9_fakt,
        t_10_fakt = NEW.t_10_fakt,
        t_11_fakt = NEW.t_11_fakt,
        t_12_fakt = NEW.t_12_fakt,
        t1_1_fakt = NEW.t1_1_fakt,
        t1_2_fakt = NEW.t1_2_fakt,
        t1_3_fakt = NEW.t1_3_fakt,
        t1_4_fakt = NEW.t1_4_fakt,
        t1_5_fakt = NEW.t1_5_fakt,
        t1_6_fakt = NEW.t1_6_fakt,
        t1_7_fakt = NEW.t1_7_fakt,
        t1_8_fakt = NEW.t1_8_fakt,
        t1_9_fakt = NEW.t1_9_fakt,
        t1_10_fakt = NEW.t1_10_fakt,
        t1_11_fakt = NEW.t1_11_fakt,
        t1_12_fakt = NEW.t1_12_fakt,
        t2_1_fakt = NEW.t2_1_fakt,
        t2_2_fakt = NEW.t2_2_fakt,
        t2_3_fakt = NEW.t2_3_fakt,
        t2_4_fakt = NEW.t2_4_fakt,
        t2_5_fakt = NEW.t2_5_fakt,
        t2_6_fakt = NEW.t2_6_fakt,
        t2_7_fakt = NEW.t2_7_fakt,
        t2_8_fakt = NEW.t2_8_fakt,
        t2_9_fakt = NEW.t2_9_fakt,
        t2_10_fakt = NEW.t2_10_fakt,
        t2_11_fakt = NEW.t2_11_fakt,
        t2_12_fakt = NEW.t2_12_fakt,
        gpod_1_fakt = NEW.gpod_1_fakt,
        gpod_2_fakt = NEW.gpod_2_fakt,
        gpod_3_fakt = NEW.gpod_3_fakt,
        gpod_4_fakt = NEW.gpod_4_fakt,
        gpod_5_fakt = NEW.gpod_5_fakt,
        gpod_6_fakt = NEW.gpod_6_fakt,
        gpod_7_fakt = NEW.gpod_7_fakt,
        gpod_8_fakt = NEW.gpod_8_fakt,
        gpod_9_fakt = NEW.gpod_9_fakt,
        gpod_10_fakt = NEW.gpod_10_fakt,
        gpod_11_fakt = NEW.gpod_11_fakt,
        gpod_12_fakt = NEW.gpod_12_fakt,
        ggvs_pribor_1_fakt = NEW.ggvs_pribor_1_fakt,
        ggvs_pribor_2_fakt = NEW.ggvs_pribor_2_fakt,
        ggvs_pribor_3_fakt = NEW.ggvs_pribor_3_fakt,
        ggvs_pribor_4_fakt = NEW.ggvs_pribor_4_fakt,
        ggvs_pribor_5_fakt = NEW.ggvs_pribor_5_fakt,
        ggvs_pribor_6_fakt = NEW.ggvs_pribor_6_fakt,
        ggvs_pribor_7_fakt = NEW.ggvs_pribor_7_fakt,
        ggvs_pribor_8_fakt = NEW.ggvs_pribor_8_fakt,
        ggvs_pribor_9_fakt = NEW.ggvs_pribor_9_fakt,
        ggvs_pribor_10_fakt = NEW.ggvs_pribor_10_fakt,
        ggvs_pribor_11_fakt = NEW.ggvs_pribor_11_fakt,
        ggvs_pribor_12_fakt = NEW.ggvs_pribor_12_fakt,
        gakt_tex_1_fakt = NEW.gakt_tex_1_fakt,
        gakt_tex_2_fakt = NEW.gakt_tex_2_fakt,
        gakt_tex_3_fakt = NEW.gakt_tex_3_fakt,
        gakt_tex_4_fakt = NEW.gakt_tex_4_fakt,
        gakt_tex_5_fakt = NEW.gakt_tex_5_fakt,
        gakt_tex_6_fakt = NEW.gakt_tex_6_fakt,
        gakt_tex_7_fakt = NEW.gakt_tex_7_fakt,
        gakt_tex_8_fakt = NEW.gakt_tex_8_fakt,
        gakt_tex_9_fakt = NEW.gakt_tex_9_fakt,
        gakt_tex_10_fakt = NEW.gakt_tex_10_fakt,
        gakt_tex_11_fakt = NEW.gakt_tex_11_fakt,
        gakt_tex_12_fakt = NEW.gakt_tex_12_fakt,
        gakt_avarija_1_fakt = NEW.gakt_avarija_1_fakt,
        gakt_avarija_2_fakt = NEW.gakt_avarija_2_fakt,
        gakt_avarija_3_fakt = NEW.gakt_avarija_3_fakt,
        gakt_avarija_4_fakt = NEW.gakt_avarija_4_fakt,
        gakt_avarija_5_fakt = NEW.gakt_avarija_5_fakt,
        gakt_avarija_6_fakt = NEW.gakt_avarija_6_fakt,
        gakt_avarija_7_fakt = NEW.gakt_avarija_7_fakt,
        gakt_avarija_8_fakt = NEW.gakt_avarija_8_fakt,
        gakt_avarija_9_fakt = NEW.gakt_avarija_9_fakt,
        gakt_avarija_10_fakt = NEW.gakt_avarija_10_fakt,
        gakt_avarija_11_fakt = NEW.gakt_avarija_11_fakt,
        gakt_avarija_12_fakt = NEW.gakt_avarija_12_fakt,
        id_old = NEW.id_old
    WHERE id = OLD.nodeid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_heatsources_upd ON net.v_heatsources;
CREATE TRIGGER v_heatsources_upd INSTEAD OF UPDATE ON net.v_heatsources
    FOR EACH ROW EXECUTE FUNCTION net.v_heatsources_upd();

CREATE OR REPLACE FUNCTION net.v_heatsources_del() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    -- Удаление строки подтипа означает снятие типа, а не удаление объекта.
    PERFORM net.reclass_node(OLD.nodeid, 'node_plain');
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_heatsources_del ON net.v_heatsources;
CREATE TRIGGER v_heatsources_del INSTEAD OF DELETE ON net.v_heatsources
    FOR EACH ROW EXECUTE FUNCTION net.v_heatsources_del();


-- pumpstations -> net.pump_station
-- DROP обязателен: CREATE OR REPLACE VIEW не умеет менять состав
-- и порядок колонок, а они меняются при правке карты типов.
DROP VIEW IF EXISTS net.v_pumpstations CASCADE;
CREATE VIEW net.v_pumpstations AS
SELECT o.subtype_src_id AS id,
       o.id             AS nodeid,
       o.name,
       o.purpose,
       o.capacity,
       o.pumpcountflow,
       o.pumpcountret,
       o.state,
       o.stateid,
       o.heighttubemark,
       o.heightareamark
FROM net.pump_station o
UNION ALL
-- Строки, не ставшие объектом: дубли и «проигравшие» чужому классу.
-- Узел может иметь строки сразу в двух подтипах — старая модель это
-- допускала, и расчётное ядро читает обе. Без этой части выборка
-- вернула бы меньше строк, чем на исходной БД.
SELECT (v.payload ->> 'id')::int AS id,
       v.obj_id                  AS nodeid,
       (v.payload ->> 'name')::text,
       (v.payload ->> 'purpose')::text,
       (v.payload ->> 'capacity')::double precision,
       (v.payload ->> 'pumpcountflow')::integer,
       (v.payload ->> 'pumpcountret')::integer,
       (v.payload ->> 'state')::text,
       (v.payload ->> 'stateid')::integer,
       (v.payload ->> 'heighttubemark')::double precision,
       (v.payload ->> 'heightareamark')::double precision
FROM net.object_variant v
WHERE v.src_table = 'pumpstations' AND NOT v.chosen;

CREATE OR REPLACE FUNCTION net.v_pumpstations_ins() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    IF NEW.nodeid IS NULL THEN
        RAISE EXCEPTION 'pumpstations: не указан nodeid';
    END IF;
    -- Тип объекта задаётся именно этой вставкой: переносим его в класс.
    PERFORM net.reclass_node(NEW.nodeid, 'pump_station');
    UPDATE net.pump_station SET subtype_src_id = NEW.id,
        name = NEW.name,
        purpose = NEW.purpose,
        capacity = NEW.capacity,
        pumpcountflow = NEW.pumpcountflow,
        pumpcountret = NEW.pumpcountret,
        state = NEW.state,
        stateid = NEW.stateid,
        heighttubemark = NEW.heighttubemark,
        heightareamark = NEW.heightareamark
    WHERE id = NEW.nodeid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_pumpstations_ins ON net.v_pumpstations;
CREATE TRIGGER v_pumpstations_ins INSTEAD OF INSERT ON net.v_pumpstations
    FOR EACH ROW EXECUTE FUNCTION net.v_pumpstations_ins();

CREATE OR REPLACE FUNCTION net.v_pumpstations_upd() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    UPDATE net.pump_station SET subtype_src_id = NEW.id,
        name = NEW.name,
        purpose = NEW.purpose,
        capacity = NEW.capacity,
        pumpcountflow = NEW.pumpcountflow,
        pumpcountret = NEW.pumpcountret,
        state = NEW.state,
        stateid = NEW.stateid,
        heighttubemark = NEW.heighttubemark,
        heightareamark = NEW.heightareamark
    WHERE id = OLD.nodeid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_pumpstations_upd ON net.v_pumpstations;
CREATE TRIGGER v_pumpstations_upd INSTEAD OF UPDATE ON net.v_pumpstations
    FOR EACH ROW EXECUTE FUNCTION net.v_pumpstations_upd();

CREATE OR REPLACE FUNCTION net.v_pumpstations_del() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    -- Удаление строки подтипа означает снятие типа, а не удаление объекта.
    PERFORM net.reclass_node(OLD.nodeid, 'node_plain');
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_pumpstations_del ON net.v_pumpstations;
CREATE TRIGGER v_pumpstations_del INSTEAD OF DELETE ON net.v_pumpstations
    FOR EACH ROW EXECUTE FUNCTION net.v_pumpstations_del();


-- realconsumers -> net.consumer_real
-- DROP обязателен: CREATE OR REPLACE VIEW не умеет менять состав
-- и порядок колонок, а они меняются при правке карты типов.
DROP VIEW IF EXISTS net.v_realconsumers CASCADE;
CREATE VIEW net.v_realconsumers AS
SELECT o.subtype_src_id AS id,
       o.id             AS nodeid,
       o.name,
       o.consumerstateid,
       o.automdegid,
       o.sectconsumercode,
       o.schemenum,
       o.buildheight,
       o.accumcoeff,
       o.specexpendid,
       o.calctemperatureid,
       o.gvsloadgraphid,
       o.varcoeffid,
       o.calchldep,
       o.calchlindep,
       o.relloadfacade,
       o.calcinternhd,
       o.contcalchldep,
       o.contcalchlindep,
       o.contrelloadfacade,
       o.continternhd,
       o.perspcalchldep,
       o.perspcalchlindep,
       o.persprelloadfacade,
       o.perspinternhd,
       o.calchlventil,
       o.expendhwpart,
       o.contcalchlventil,
       o.perspcalchlventil,
       o.avghlcond,
       o.contavghlcond,
       o.perspavghlcond,
       o.avghlclosesys,
       o.avghlopensysflow,
       o.avghlopensysret,
       o.contavghlclosesys,
       o.contavghlopensysflow,
       o.contavghlopensysret,
       o.perspavghlclose,
       o.perspavghlopenflow,
       o.contavghlopenret,
       o.avghlgvsopenflow,
       o.avghlgvsopenret,
       o.avghlgvscloseparall,
       o.avghlgvsclosemix,
       o.avghlgvscloseconseq,
       o.avghlgvsclosepreon,
       o.contavghlgvsopenflow,
       o.contavghlgvsopenret,
       o.contavghlgvscloseparall,
       o.contavghlgvsclosemix,
       o.contavghlgvscloseconseq,
       o.contavghlgvsclosepreon,
       o.perspavghlgvsopenflow,
       o.perspavghlgvsopenret,
       o.perspavghlgvscloseparall,
       o.perspavghlgvsclosemix,
       o.perspavghlgvscloseconseq,
       o.perspavghlgvsclosepreon,
       o.hydromodesignid,
       o.mixfactcoeff,
       o.hourirregcoeff,
       o.circhlosopen,
       o.temprecircpipe,
       o.setleakageflow,
       o.setleakageret,
       o.wemeteringdeviceid,
       o.volwaterhs,
       o.volwatervs,
       o.hydroresclosesys,
       o.hydroresclosesyssummer,
       o.closesyscalcsignid,
       o.hydroreswdoflow,
       o.calcsignopensysflowid,
       o.hydroreswdoret,
       o.calcsignreswdoretid,
       o.calcsignsetloadclosesys,
       o.calcsignsetloadopensysflow,
       o.calcsignsetloadopensysret,
       o.hydrothrustin,
       o.hydrothrustout,
       o.calcexpenddep,
       o.calcexpendindep,
       o.calcexpendventil,
       o.calcexpendcond,
       o.calcexpendhwflow,
       o.calcexpendhwret,
       o.calcexpendrecircopen,
       o.calcexpendhwparall,
       o.calcexpendhwmix,
       o.calcexpendhwconseq,
       o.calcexpendhwpreon,
       o.throtstagesignid,
       o.diameterthrotdiaph,
       o.diameterelevnozzle,
       o.temperchartsignid,
       o.calcsignres,
       o.calcsignhl,
       o.parallheaterscount1,
       o.parallheaterscount2,
       o.parallheaterscountindep,
       o.calcthrustloshs,
       o.calcthrustlosah,
       o.calcthrustlosac,
       o.calcthrustlosflow,
       o.calcthrustlosflowcirc,
       o.calcthrustinwdo,
       o.calcthrustlosheaters1,
       o.calcthrustlosheaters2,
       o.pdvalveinstalllocid,
       o.setpdonregulator,
       o.calcferdiametersignid,
       o.calctemphr,
       o.calctempvs,
       o.calctemphwdo,
       o.responsibleid,
       o.contractnumber,
       o.stopvalvetypeid,
       o.meterdevworksign,
       o.meterdevsafety,
       o.meterdevstampnum,
       o.isolationtype,
       o.buildingtypeid,
       o.heatsourceptsid,
       o.heatpointid,
       o.countusergv,
       o.countbusinessconsumers,
       o.area,
       o.buildingvolume,
       o.basementvolume,
       o.builtinvolume,
       o.reducebuildingvolume,
       o.countfloors,
       o.builtyear,
       o.streetid,
       o.housenumber,
       o.note_1,
       o.note_2,
       o.note_pts,
       o.note_regime,
       o.b23,
       o.b28,
       o.b32,
       o.b20,
       o.b14,
       o.b38,
       o.b35,
       o.b36,
       o.zd7,
       o.zd8,
       o.zd27,
       o.zd28,
       o.zd36,
       o.zd38,
       o.zd39,
       o.zd48,
       o.zd49,
       o.zd50,
       o.elevatornuminst,
       o.diameternozzle
FROM net.consumer_real o
UNION ALL
-- Строки, не ставшие объектом: дубли и «проигравшие» чужому классу.
-- Узел может иметь строки сразу в двух подтипах — старая модель это
-- допускала, и расчётное ядро читает обе. Без этой части выборка
-- вернула бы меньше строк, чем на исходной БД.
SELECT (v.payload ->> 'id')::int AS id,
       v.obj_id                  AS nodeid,
       (v.payload ->> 'name')::text,
       (v.payload ->> 'consumerstateid')::integer,
       (v.payload ->> 'automdegid')::integer,
       (v.payload ->> 'sectconsumercode')::text,
       (v.payload ->> 'schemenum')::text,
       (v.payload ->> 'buildheight')::double precision,
       (v.payload ->> 'accumcoeff')::double precision,
       (v.payload ->> 'specexpendid')::integer,
       (v.payload ->> 'calctemperatureid')::integer,
       (v.payload ->> 'gvsloadgraphid')::integer,
       (v.payload ->> 'varcoeffid')::integer,
       (v.payload ->> 'calchldep')::double precision,
       (v.payload ->> 'calchlindep')::double precision,
       (v.payload ->> 'relloadfacade')::double precision,
       (v.payload ->> 'calcinternhd')::double precision,
       (v.payload ->> 'contcalchldep')::double precision,
       (v.payload ->> 'contcalchlindep')::double precision,
       (v.payload ->> 'contrelloadfacade')::double precision,
       (v.payload ->> 'continternhd')::double precision,
       (v.payload ->> 'perspcalchldep')::double precision,
       (v.payload ->> 'perspcalchlindep')::double precision,
       (v.payload ->> 'persprelloadfacade')::double precision,
       (v.payload ->> 'perspinternhd')::double precision,
       (v.payload ->> 'calchlventil')::double precision,
       (v.payload ->> 'expendhwpart')::double precision,
       (v.payload ->> 'contcalchlventil')::double precision,
       (v.payload ->> 'perspcalchlventil')::double precision,
       (v.payload ->> 'avghlcond')::double precision,
       (v.payload ->> 'contavghlcond')::double precision,
       (v.payload ->> 'perspavghlcond')::double precision,
       (v.payload ->> 'avghlclosesys')::double precision,
       (v.payload ->> 'avghlopensysflow')::double precision,
       (v.payload ->> 'avghlopensysret')::double precision,
       (v.payload ->> 'contavghlclosesys')::double precision,
       (v.payload ->> 'contavghlopensysflow')::double precision,
       (v.payload ->> 'contavghlopensysret')::double precision,
       (v.payload ->> 'perspavghlclose')::double precision,
       (v.payload ->> 'perspavghlopenflow')::double precision,
       (v.payload ->> 'contavghlopenret')::double precision,
       (v.payload ->> 'avghlgvsopenflow')::double precision,
       (v.payload ->> 'avghlgvsopenret')::double precision,
       (v.payload ->> 'avghlgvscloseparall')::double precision,
       (v.payload ->> 'avghlgvsclosemix')::double precision,
       (v.payload ->> 'avghlgvscloseconseq')::double precision,
       (v.payload ->> 'avghlgvsclosepreon')::double precision,
       (v.payload ->> 'contavghlgvsopenflow')::double precision,
       (v.payload ->> 'contavghlgvsopenret')::double precision,
       (v.payload ->> 'contavghlgvscloseparall')::double precision,
       (v.payload ->> 'contavghlgvsclosemix')::double precision,
       (v.payload ->> 'contavghlgvscloseconseq')::double precision,
       (v.payload ->> 'contavghlgvsclosepreon')::double precision,
       (v.payload ->> 'perspavghlgvsopenflow')::double precision,
       (v.payload ->> 'perspavghlgvsopenret')::double precision,
       (v.payload ->> 'perspavghlgvscloseparall')::double precision,
       (v.payload ->> 'perspavghlgvsclosemix')::double precision,
       (v.payload ->> 'perspavghlgvscloseconseq')::double precision,
       (v.payload ->> 'perspavghlgvsclosepreon')::double precision,
       (v.payload ->> 'hydromodesignid')::integer,
       (v.payload ->> 'mixfactcoeff')::double precision,
       (v.payload ->> 'hourirregcoeff')::double precision,
       (v.payload ->> 'circhlosopen')::double precision,
       (v.payload ->> 'temprecircpipe')::double precision,
       (v.payload ->> 'setleakageflow')::double precision,
       (v.payload ->> 'setleakageret')::double precision,
       (v.payload ->> 'wemeteringdeviceid')::integer,
       (v.payload ->> 'volwaterhs')::double precision,
       (v.payload ->> 'volwatervs')::double precision,
       (v.payload ->> 'hydroresclosesys')::double precision,
       (v.payload ->> 'hydroresclosesyssummer')::double precision,
       (v.payload ->> 'closesyscalcsignid')::integer,
       (v.payload ->> 'hydroreswdoflow')::double precision,
       (v.payload ->> 'calcsignopensysflowid')::integer,
       (v.payload ->> 'hydroreswdoret')::double precision,
       (v.payload ->> 'calcsignreswdoretid')::integer,
       (v.payload ->> 'calcsignsetloadclosesys')::integer,
       (v.payload ->> 'calcsignsetloadopensysflow')::integer,
       (v.payload ->> 'calcsignsetloadopensysret')::integer,
       (v.payload ->> 'hydrothrustin')::double precision,
       (v.payload ->> 'hydrothrustout')::double precision,
       (v.payload ->> 'calcexpenddep')::double precision,
       (v.payload ->> 'calcexpendindep')::double precision,
       (v.payload ->> 'calcexpendventil')::double precision,
       (v.payload ->> 'calcexpendcond')::double precision,
       (v.payload ->> 'calcexpendhwflow')::double precision,
       (v.payload ->> 'calcexpendhwret')::double precision,
       (v.payload ->> 'calcexpendrecircopen')::double precision,
       (v.payload ->> 'calcexpendhwparall')::double precision,
       (v.payload ->> 'calcexpendhwmix')::double precision,
       (v.payload ->> 'calcexpendhwconseq')::double precision,
       (v.payload ->> 'calcexpendhwpreon')::double precision,
       (v.payload ->> 'throtstagesignid')::integer,
       (v.payload ->> 'diameterthrotdiaph')::double precision,
       (v.payload ->> 'diameterelevnozzle')::double precision,
       (v.payload ->> 'temperchartsignid')::integer,
       (v.payload ->> 'calcsignres')::integer,
       (v.payload ->> 'calcsignhl')::integer,
       (v.payload ->> 'parallheaterscount1')::integer,
       (v.payload ->> 'parallheaterscount2')::integer,
       (v.payload ->> 'parallheaterscountindep')::integer,
       (v.payload ->> 'calcthrustloshs')::double precision,
       (v.payload ->> 'calcthrustlosah')::double precision,
       (v.payload ->> 'calcthrustlosac')::double precision,
       (v.payload ->> 'calcthrustlosflow')::double precision,
       (v.payload ->> 'calcthrustlosflowcirc')::double precision,
       (v.payload ->> 'calcthrustinwdo')::double precision,
       (v.payload ->> 'calcthrustlosheaters1')::double precision,
       (v.payload ->> 'calcthrustlosheaters2')::double precision,
       (v.payload ->> 'pdvalveinstalllocid')::integer,
       (v.payload ->> 'setpdonregulator')::double precision,
       (v.payload ->> 'calcferdiametersignid')::integer,
       (v.payload ->> 'calctemphr')::double precision,
       (v.payload ->> 'calctempvs')::double precision,
       (v.payload ->> 'calctemphwdo')::double precision,
       (v.payload ->> 'responsibleid')::integer,
       (v.payload ->> 'contractnumber')::text,
       (v.payload ->> 'stopvalvetypeid')::integer,
       (v.payload ->> 'meterdevworksign')::integer,
       (v.payload ->> 'meterdevsafety')::integer,
       (v.payload ->> 'meterdevstampnum')::text,
       (v.payload ->> 'isolationtype')::double precision,
       (v.payload ->> 'buildingtypeid')::integer,
       (v.payload ->> 'heatsourceptsid')::integer,
       (v.payload ->> 'heatpointid')::integer,
       (v.payload ->> 'countusergv')::integer,
       (v.payload ->> 'countbusinessconsumers')::integer,
       (v.payload ->> 'area')::double precision,
       (v.payload ->> 'buildingvolume')::double precision,
       (v.payload ->> 'basementvolume')::double precision,
       (v.payload ->> 'builtinvolume')::double precision,
       (v.payload ->> 'reducebuildingvolume')::double precision,
       (v.payload ->> 'countfloors')::integer,
       (v.payload ->> 'builtyear')::integer,
       (v.payload ->> 'streetid')::integer,
       (v.payload ->> 'housenumber')::text,
       (v.payload ->> 'note_1')::text,
       (v.payload ->> 'note_2')::text,
       (v.payload ->> 'note_pts')::text,
       (v.payload ->> 'note_regime')::text,
       (v.payload ->> 'b23')::double precision,
       (v.payload ->> 'b28')::double precision,
       (v.payload ->> 'b32')::double precision,
       (v.payload ->> 'b20')::double precision,
       (v.payload ->> 'b14')::double precision,
       (v.payload ->> 'b38')::double precision,
       (v.payload ->> 'b35')::double precision,
       (v.payload ->> 'b36')::double precision,
       (v.payload ->> 'zd7')::integer,
       (v.payload ->> 'zd8')::integer,
       (v.payload ->> 'zd27')::integer,
       (v.payload ->> 'zd28')::integer,
       (v.payload ->> 'zd36')::integer,
       (v.payload ->> 'zd38')::integer,
       (v.payload ->> 'zd39')::integer,
       (v.payload ->> 'zd48')::integer,
       (v.payload ->> 'zd49')::integer,
       (v.payload ->> 'zd50')::integer,
       (v.payload ->> 'elevatornuminst')::integer,
       (v.payload ->> 'diameternozzle')::double precision
FROM net.object_variant v
WHERE v.src_table = 'realconsumers' AND NOT v.chosen;

CREATE OR REPLACE FUNCTION net.v_realconsumers_ins() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    IF NEW.nodeid IS NULL THEN
        RAISE EXCEPTION 'realconsumers: не указан nodeid';
    END IF;
    -- Тип объекта задаётся именно этой вставкой: переносим его в класс.
    PERFORM net.reclass_node(NEW.nodeid, 'consumer_real');
    UPDATE net.consumer_real SET subtype_src_id = NEW.id,
        name = NEW.name,
        consumerstateid = NEW.consumerstateid,
        automdegid = NEW.automdegid,
        sectconsumercode = NEW.sectconsumercode,
        schemenum = NEW.schemenum,
        buildheight = NEW.buildheight,
        accumcoeff = NEW.accumcoeff,
        specexpendid = NEW.specexpendid,
        calctemperatureid = NEW.calctemperatureid,
        gvsloadgraphid = NEW.gvsloadgraphid,
        varcoeffid = NEW.varcoeffid,
        calchldep = NEW.calchldep,
        calchlindep = NEW.calchlindep,
        relloadfacade = NEW.relloadfacade,
        calcinternhd = NEW.calcinternhd,
        contcalchldep = NEW.contcalchldep,
        contcalchlindep = NEW.contcalchlindep,
        contrelloadfacade = NEW.contrelloadfacade,
        continternhd = NEW.continternhd,
        perspcalchldep = NEW.perspcalchldep,
        perspcalchlindep = NEW.perspcalchlindep,
        persprelloadfacade = NEW.persprelloadfacade,
        perspinternhd = NEW.perspinternhd,
        calchlventil = NEW.calchlventil,
        expendhwpart = NEW.expendhwpart,
        contcalchlventil = NEW.contcalchlventil,
        perspcalchlventil = NEW.perspcalchlventil,
        avghlcond = NEW.avghlcond,
        contavghlcond = NEW.contavghlcond,
        perspavghlcond = NEW.perspavghlcond,
        avghlclosesys = NEW.avghlclosesys,
        avghlopensysflow = NEW.avghlopensysflow,
        avghlopensysret = NEW.avghlopensysret,
        contavghlclosesys = NEW.contavghlclosesys,
        contavghlopensysflow = NEW.contavghlopensysflow,
        contavghlopensysret = NEW.contavghlopensysret,
        perspavghlclose = NEW.perspavghlclose,
        perspavghlopenflow = NEW.perspavghlopenflow,
        contavghlopenret = NEW.contavghlopenret,
        avghlgvsopenflow = NEW.avghlgvsopenflow,
        avghlgvsopenret = NEW.avghlgvsopenret,
        avghlgvscloseparall = NEW.avghlgvscloseparall,
        avghlgvsclosemix = NEW.avghlgvsclosemix,
        avghlgvscloseconseq = NEW.avghlgvscloseconseq,
        avghlgvsclosepreon = NEW.avghlgvsclosepreon,
        contavghlgvsopenflow = NEW.contavghlgvsopenflow,
        contavghlgvsopenret = NEW.contavghlgvsopenret,
        contavghlgvscloseparall = NEW.contavghlgvscloseparall,
        contavghlgvsclosemix = NEW.contavghlgvsclosemix,
        contavghlgvscloseconseq = NEW.contavghlgvscloseconseq,
        contavghlgvsclosepreon = NEW.contavghlgvsclosepreon,
        perspavghlgvsopenflow = NEW.perspavghlgvsopenflow,
        perspavghlgvsopenret = NEW.perspavghlgvsopenret,
        perspavghlgvscloseparall = NEW.perspavghlgvscloseparall,
        perspavghlgvsclosemix = NEW.perspavghlgvsclosemix,
        perspavghlgvscloseconseq = NEW.perspavghlgvscloseconseq,
        perspavghlgvsclosepreon = NEW.perspavghlgvsclosepreon,
        hydromodesignid = NEW.hydromodesignid,
        mixfactcoeff = NEW.mixfactcoeff,
        hourirregcoeff = NEW.hourirregcoeff,
        circhlosopen = NEW.circhlosopen,
        temprecircpipe = NEW.temprecircpipe,
        setleakageflow = NEW.setleakageflow,
        setleakageret = NEW.setleakageret,
        wemeteringdeviceid = NEW.wemeteringdeviceid,
        volwaterhs = NEW.volwaterhs,
        volwatervs = NEW.volwatervs,
        hydroresclosesys = NEW.hydroresclosesys,
        hydroresclosesyssummer = NEW.hydroresclosesyssummer,
        closesyscalcsignid = NEW.closesyscalcsignid,
        hydroreswdoflow = NEW.hydroreswdoflow,
        calcsignopensysflowid = NEW.calcsignopensysflowid,
        hydroreswdoret = NEW.hydroreswdoret,
        calcsignreswdoretid = NEW.calcsignreswdoretid,
        calcsignsetloadclosesys = NEW.calcsignsetloadclosesys,
        calcsignsetloadopensysflow = NEW.calcsignsetloadopensysflow,
        calcsignsetloadopensysret = NEW.calcsignsetloadopensysret,
        hydrothrustin = NEW.hydrothrustin,
        hydrothrustout = NEW.hydrothrustout,
        calcexpenddep = NEW.calcexpenddep,
        calcexpendindep = NEW.calcexpendindep,
        calcexpendventil = NEW.calcexpendventil,
        calcexpendcond = NEW.calcexpendcond,
        calcexpendhwflow = NEW.calcexpendhwflow,
        calcexpendhwret = NEW.calcexpendhwret,
        calcexpendrecircopen = NEW.calcexpendrecircopen,
        calcexpendhwparall = NEW.calcexpendhwparall,
        calcexpendhwmix = NEW.calcexpendhwmix,
        calcexpendhwconseq = NEW.calcexpendhwconseq,
        calcexpendhwpreon = NEW.calcexpendhwpreon,
        throtstagesignid = NEW.throtstagesignid,
        diameterthrotdiaph = NEW.diameterthrotdiaph,
        diameterelevnozzle = NEW.diameterelevnozzle,
        temperchartsignid = NEW.temperchartsignid,
        calcsignres = NEW.calcsignres,
        calcsignhl = NEW.calcsignhl,
        parallheaterscount1 = NEW.parallheaterscount1,
        parallheaterscount2 = NEW.parallheaterscount2,
        parallheaterscountindep = NEW.parallheaterscountindep,
        calcthrustloshs = NEW.calcthrustloshs,
        calcthrustlosah = NEW.calcthrustlosah,
        calcthrustlosac = NEW.calcthrustlosac,
        calcthrustlosflow = NEW.calcthrustlosflow,
        calcthrustlosflowcirc = NEW.calcthrustlosflowcirc,
        calcthrustinwdo = NEW.calcthrustinwdo,
        calcthrustlosheaters1 = NEW.calcthrustlosheaters1,
        calcthrustlosheaters2 = NEW.calcthrustlosheaters2,
        pdvalveinstalllocid = NEW.pdvalveinstalllocid,
        setpdonregulator = NEW.setpdonregulator,
        calcferdiametersignid = NEW.calcferdiametersignid,
        calctemphr = NEW.calctemphr,
        calctempvs = NEW.calctempvs,
        calctemphwdo = NEW.calctemphwdo,
        responsibleid = NEW.responsibleid,
        contractnumber = NEW.contractnumber,
        stopvalvetypeid = NEW.stopvalvetypeid,
        meterdevworksign = NEW.meterdevworksign,
        meterdevsafety = NEW.meterdevsafety,
        meterdevstampnum = NEW.meterdevstampnum,
        isolationtype = NEW.isolationtype,
        buildingtypeid = NEW.buildingtypeid,
        heatsourceptsid = NEW.heatsourceptsid,
        heatpointid = NEW.heatpointid,
        countusergv = NEW.countusergv,
        countbusinessconsumers = NEW.countbusinessconsumers,
        area = NEW.area,
        buildingvolume = NEW.buildingvolume,
        basementvolume = NEW.basementvolume,
        builtinvolume = NEW.builtinvolume,
        reducebuildingvolume = NEW.reducebuildingvolume,
        countfloors = NEW.countfloors,
        builtyear = NEW.builtyear,
        streetid = NEW.streetid,
        housenumber = NEW.housenumber,
        note_1 = NEW.note_1,
        note_2 = NEW.note_2,
        note_pts = NEW.note_pts,
        note_regime = NEW.note_regime,
        b23 = NEW.b23,
        b28 = NEW.b28,
        b32 = NEW.b32,
        b20 = NEW.b20,
        b14 = NEW.b14,
        b38 = NEW.b38,
        b35 = NEW.b35,
        b36 = NEW.b36,
        zd7 = NEW.zd7,
        zd8 = NEW.zd8,
        zd27 = NEW.zd27,
        zd28 = NEW.zd28,
        zd36 = NEW.zd36,
        zd38 = NEW.zd38,
        zd39 = NEW.zd39,
        zd48 = NEW.zd48,
        zd49 = NEW.zd49,
        zd50 = NEW.zd50,
        elevatornuminst = NEW.elevatornuminst,
        diameternozzle = NEW.diameternozzle
    WHERE id = NEW.nodeid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_realconsumers_ins ON net.v_realconsumers;
CREATE TRIGGER v_realconsumers_ins INSTEAD OF INSERT ON net.v_realconsumers
    FOR EACH ROW EXECUTE FUNCTION net.v_realconsumers_ins();

CREATE OR REPLACE FUNCTION net.v_realconsumers_upd() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    UPDATE net.consumer_real SET subtype_src_id = NEW.id,
        name = NEW.name,
        consumerstateid = NEW.consumerstateid,
        automdegid = NEW.automdegid,
        sectconsumercode = NEW.sectconsumercode,
        schemenum = NEW.schemenum,
        buildheight = NEW.buildheight,
        accumcoeff = NEW.accumcoeff,
        specexpendid = NEW.specexpendid,
        calctemperatureid = NEW.calctemperatureid,
        gvsloadgraphid = NEW.gvsloadgraphid,
        varcoeffid = NEW.varcoeffid,
        calchldep = NEW.calchldep,
        calchlindep = NEW.calchlindep,
        relloadfacade = NEW.relloadfacade,
        calcinternhd = NEW.calcinternhd,
        contcalchldep = NEW.contcalchldep,
        contcalchlindep = NEW.contcalchlindep,
        contrelloadfacade = NEW.contrelloadfacade,
        continternhd = NEW.continternhd,
        perspcalchldep = NEW.perspcalchldep,
        perspcalchlindep = NEW.perspcalchlindep,
        persprelloadfacade = NEW.persprelloadfacade,
        perspinternhd = NEW.perspinternhd,
        calchlventil = NEW.calchlventil,
        expendhwpart = NEW.expendhwpart,
        contcalchlventil = NEW.contcalchlventil,
        perspcalchlventil = NEW.perspcalchlventil,
        avghlcond = NEW.avghlcond,
        contavghlcond = NEW.contavghlcond,
        perspavghlcond = NEW.perspavghlcond,
        avghlclosesys = NEW.avghlclosesys,
        avghlopensysflow = NEW.avghlopensysflow,
        avghlopensysret = NEW.avghlopensysret,
        contavghlclosesys = NEW.contavghlclosesys,
        contavghlopensysflow = NEW.contavghlopensysflow,
        contavghlopensysret = NEW.contavghlopensysret,
        perspavghlclose = NEW.perspavghlclose,
        perspavghlopenflow = NEW.perspavghlopenflow,
        contavghlopenret = NEW.contavghlopenret,
        avghlgvsopenflow = NEW.avghlgvsopenflow,
        avghlgvsopenret = NEW.avghlgvsopenret,
        avghlgvscloseparall = NEW.avghlgvscloseparall,
        avghlgvsclosemix = NEW.avghlgvsclosemix,
        avghlgvscloseconseq = NEW.avghlgvscloseconseq,
        avghlgvsclosepreon = NEW.avghlgvsclosepreon,
        contavghlgvsopenflow = NEW.contavghlgvsopenflow,
        contavghlgvsopenret = NEW.contavghlgvsopenret,
        contavghlgvscloseparall = NEW.contavghlgvscloseparall,
        contavghlgvsclosemix = NEW.contavghlgvsclosemix,
        contavghlgvscloseconseq = NEW.contavghlgvscloseconseq,
        contavghlgvsclosepreon = NEW.contavghlgvsclosepreon,
        perspavghlgvsopenflow = NEW.perspavghlgvsopenflow,
        perspavghlgvsopenret = NEW.perspavghlgvsopenret,
        perspavghlgvscloseparall = NEW.perspavghlgvscloseparall,
        perspavghlgvsclosemix = NEW.perspavghlgvsclosemix,
        perspavghlgvscloseconseq = NEW.perspavghlgvscloseconseq,
        perspavghlgvsclosepreon = NEW.perspavghlgvsclosepreon,
        hydromodesignid = NEW.hydromodesignid,
        mixfactcoeff = NEW.mixfactcoeff,
        hourirregcoeff = NEW.hourirregcoeff,
        circhlosopen = NEW.circhlosopen,
        temprecircpipe = NEW.temprecircpipe,
        setleakageflow = NEW.setleakageflow,
        setleakageret = NEW.setleakageret,
        wemeteringdeviceid = NEW.wemeteringdeviceid,
        volwaterhs = NEW.volwaterhs,
        volwatervs = NEW.volwatervs,
        hydroresclosesys = NEW.hydroresclosesys,
        hydroresclosesyssummer = NEW.hydroresclosesyssummer,
        closesyscalcsignid = NEW.closesyscalcsignid,
        hydroreswdoflow = NEW.hydroreswdoflow,
        calcsignopensysflowid = NEW.calcsignopensysflowid,
        hydroreswdoret = NEW.hydroreswdoret,
        calcsignreswdoretid = NEW.calcsignreswdoretid,
        calcsignsetloadclosesys = NEW.calcsignsetloadclosesys,
        calcsignsetloadopensysflow = NEW.calcsignsetloadopensysflow,
        calcsignsetloadopensysret = NEW.calcsignsetloadopensysret,
        hydrothrustin = NEW.hydrothrustin,
        hydrothrustout = NEW.hydrothrustout,
        calcexpenddep = NEW.calcexpenddep,
        calcexpendindep = NEW.calcexpendindep,
        calcexpendventil = NEW.calcexpendventil,
        calcexpendcond = NEW.calcexpendcond,
        calcexpendhwflow = NEW.calcexpendhwflow,
        calcexpendhwret = NEW.calcexpendhwret,
        calcexpendrecircopen = NEW.calcexpendrecircopen,
        calcexpendhwparall = NEW.calcexpendhwparall,
        calcexpendhwmix = NEW.calcexpendhwmix,
        calcexpendhwconseq = NEW.calcexpendhwconseq,
        calcexpendhwpreon = NEW.calcexpendhwpreon,
        throtstagesignid = NEW.throtstagesignid,
        diameterthrotdiaph = NEW.diameterthrotdiaph,
        diameterelevnozzle = NEW.diameterelevnozzle,
        temperchartsignid = NEW.temperchartsignid,
        calcsignres = NEW.calcsignres,
        calcsignhl = NEW.calcsignhl,
        parallheaterscount1 = NEW.parallheaterscount1,
        parallheaterscount2 = NEW.parallheaterscount2,
        parallheaterscountindep = NEW.parallheaterscountindep,
        calcthrustloshs = NEW.calcthrustloshs,
        calcthrustlosah = NEW.calcthrustlosah,
        calcthrustlosac = NEW.calcthrustlosac,
        calcthrustlosflow = NEW.calcthrustlosflow,
        calcthrustlosflowcirc = NEW.calcthrustlosflowcirc,
        calcthrustinwdo = NEW.calcthrustinwdo,
        calcthrustlosheaters1 = NEW.calcthrustlosheaters1,
        calcthrustlosheaters2 = NEW.calcthrustlosheaters2,
        pdvalveinstalllocid = NEW.pdvalveinstalllocid,
        setpdonregulator = NEW.setpdonregulator,
        calcferdiametersignid = NEW.calcferdiametersignid,
        calctemphr = NEW.calctemphr,
        calctempvs = NEW.calctempvs,
        calctemphwdo = NEW.calctemphwdo,
        responsibleid = NEW.responsibleid,
        contractnumber = NEW.contractnumber,
        stopvalvetypeid = NEW.stopvalvetypeid,
        meterdevworksign = NEW.meterdevworksign,
        meterdevsafety = NEW.meterdevsafety,
        meterdevstampnum = NEW.meterdevstampnum,
        isolationtype = NEW.isolationtype,
        buildingtypeid = NEW.buildingtypeid,
        heatsourceptsid = NEW.heatsourceptsid,
        heatpointid = NEW.heatpointid,
        countusergv = NEW.countusergv,
        countbusinessconsumers = NEW.countbusinessconsumers,
        area = NEW.area,
        buildingvolume = NEW.buildingvolume,
        basementvolume = NEW.basementvolume,
        builtinvolume = NEW.builtinvolume,
        reducebuildingvolume = NEW.reducebuildingvolume,
        countfloors = NEW.countfloors,
        builtyear = NEW.builtyear,
        streetid = NEW.streetid,
        housenumber = NEW.housenumber,
        note_1 = NEW.note_1,
        note_2 = NEW.note_2,
        note_pts = NEW.note_pts,
        note_regime = NEW.note_regime,
        b23 = NEW.b23,
        b28 = NEW.b28,
        b32 = NEW.b32,
        b20 = NEW.b20,
        b14 = NEW.b14,
        b38 = NEW.b38,
        b35 = NEW.b35,
        b36 = NEW.b36,
        zd7 = NEW.zd7,
        zd8 = NEW.zd8,
        zd27 = NEW.zd27,
        zd28 = NEW.zd28,
        zd36 = NEW.zd36,
        zd38 = NEW.zd38,
        zd39 = NEW.zd39,
        zd48 = NEW.zd48,
        zd49 = NEW.zd49,
        zd50 = NEW.zd50,
        elevatornuminst = NEW.elevatornuminst,
        diameternozzle = NEW.diameternozzle
    WHERE id = OLD.nodeid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_realconsumers_upd ON net.v_realconsumers;
CREATE TRIGGER v_realconsumers_upd INSTEAD OF UPDATE ON net.v_realconsumers
    FOR EACH ROW EXECUTE FUNCTION net.v_realconsumers_upd();

CREATE OR REPLACE FUNCTION net.v_realconsumers_del() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    -- Удаление строки подтипа означает снятие типа, а не удаление объекта.
    PERFORM net.reclass_node(OLD.nodeid, 'node_plain');
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_realconsumers_del ON net.v_realconsumers;
CREATE TRIGGER v_realconsumers_del INSTEAD OF DELETE ON net.v_realconsumers
    FOR EACH ROW EXECUTE FUNCTION net.v_realconsumers_del();


-- refillnodes -> net.refill_node
-- DROP обязателен: CREATE OR REPLACE VIEW не умеет менять состав
-- и порядок колонок, а они меняются при правке карты типов.
DROP VIEW IF EXISTS net.v_refillnodes CASCADE;
CREATE VIEW net.v_refillnodes AS
SELECT o.subtype_src_id AS id,
       o.id             AS nodeid,
       o.externalsignid,
       o.refillexpend,
       o.wdo,
       o.refillloss,
       o.diameterinternal,
       o.watervolup,
       o.watervoldown,
       o.watervolupset,
       o.potscount,
       o.potssumvol,
       o.potworkingsign,
       o.chargeexpend,
       o.dischargeexpend,
       o.setpressret
FROM net.refill_node o
UNION ALL
-- Строки, не ставшие объектом: дубли и «проигравшие» чужому классу.
-- Узел может иметь строки сразу в двух подтипах — старая модель это
-- допускала, и расчётное ядро читает обе. Без этой части выборка
-- вернула бы меньше строк, чем на исходной БД.
SELECT (v.payload ->> 'id')::int AS id,
       v.obj_id                  AS nodeid,
       (v.payload ->> 'externalsignid')::integer,
       (v.payload ->> 'refillexpend')::double precision,
       (v.payload ->> 'wdo')::double precision,
       (v.payload ->> 'refillloss')::double precision,
       (v.payload ->> 'diameterinternal')::double precision,
       (v.payload ->> 'watervolup')::double precision,
       (v.payload ->> 'watervoldown')::double precision,
       (v.payload ->> 'watervolupset')::double precision,
       (v.payload ->> 'potscount')::integer,
       (v.payload ->> 'potssumvol')::double precision,
       (v.payload ->> 'potworkingsign')::text,
       (v.payload ->> 'chargeexpend')::double precision,
       (v.payload ->> 'dischargeexpend')::double precision,
       (v.payload ->> 'setpressret')::integer
FROM net.object_variant v
WHERE v.src_table = 'refillnodes' AND NOT v.chosen;

CREATE OR REPLACE FUNCTION net.v_refillnodes_ins() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    IF NEW.nodeid IS NULL THEN
        RAISE EXCEPTION 'refillnodes: не указан nodeid';
    END IF;
    -- Тип объекта задаётся именно этой вставкой: переносим его в класс.
    PERFORM net.reclass_node(NEW.nodeid, 'refill_node');
    UPDATE net.refill_node SET subtype_src_id = NEW.id,
        externalsignid = NEW.externalsignid,
        refillexpend = NEW.refillexpend,
        wdo = NEW.wdo,
        refillloss = NEW.refillloss,
        diameterinternal = NEW.diameterinternal,
        watervolup = NEW.watervolup,
        watervoldown = NEW.watervoldown,
        watervolupset = NEW.watervolupset,
        potscount = NEW.potscount,
        potssumvol = NEW.potssumvol,
        potworkingsign = NEW.potworkingsign,
        chargeexpend = NEW.chargeexpend,
        dischargeexpend = NEW.dischargeexpend,
        setpressret = NEW.setpressret
    WHERE id = NEW.nodeid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_refillnodes_ins ON net.v_refillnodes;
CREATE TRIGGER v_refillnodes_ins INSTEAD OF INSERT ON net.v_refillnodes
    FOR EACH ROW EXECUTE FUNCTION net.v_refillnodes_ins();

CREATE OR REPLACE FUNCTION net.v_refillnodes_upd() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    UPDATE net.refill_node SET subtype_src_id = NEW.id,
        externalsignid = NEW.externalsignid,
        refillexpend = NEW.refillexpend,
        wdo = NEW.wdo,
        refillloss = NEW.refillloss,
        diameterinternal = NEW.diameterinternal,
        watervolup = NEW.watervolup,
        watervoldown = NEW.watervoldown,
        watervolupset = NEW.watervolupset,
        potscount = NEW.potscount,
        potssumvol = NEW.potssumvol,
        potworkingsign = NEW.potworkingsign,
        chargeexpend = NEW.chargeexpend,
        dischargeexpend = NEW.dischargeexpend,
        setpressret = NEW.setpressret
    WHERE id = OLD.nodeid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_refillnodes_upd ON net.v_refillnodes;
CREATE TRIGGER v_refillnodes_upd INSTEAD OF UPDATE ON net.v_refillnodes
    FOR EACH ROW EXECUTE FUNCTION net.v_refillnodes_upd();

CREATE OR REPLACE FUNCTION net.v_refillnodes_del() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    -- Удаление строки подтипа означает снятие типа, а не удаление объекта.
    PERFORM net.reclass_node(OLD.nodeid, 'node_plain');
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_refillnodes_del ON net.v_refillnodes;
CREATE TRIGGER v_refillnodes_del INSTEAD OF DELETE ON net.v_refillnodes
    FOR EACH ROW EXECUTE FUNCTION net.v_refillnodes_del();


-- threewayvalves -> net.valve_3way
-- DROP обязателен: CREATE OR REPLACE VIEW не умеет менять состав
-- и порядок колонок, а они меняются при правке карты типов.
DROP VIEW IF EXISTS net.v_threewayvalves CASCADE;
CREATE VIEW net.v_threewayvalves AS
SELECT o.subtype_src_id AS id,
       o.id             AS nodeid,
       o.structure,
       o.state,
       o.purpose,
       o.calcmixfactcoeff,
       o.calcthrustlos,
       o.expendcharstraight,
       o.expendcharvert,
       o.authority,
       o.calccapdeviation,
       o.type,
       o.capacity,
       o.regulator,
       o.maxpd,
       o.diameternomin,
       o.calcexpendnodemix,
       o.pumpstationname
FROM net.valve_3way o
UNION ALL
-- Строки, не ставшие объектом: дубли и «проигравшие» чужому классу.
-- Узел может иметь строки сразу в двух подтипах — старая модель это
-- допускала, и расчётное ядро читает обе. Без этой части выборка
-- вернула бы меньше строк, чем на исходной БД.
SELECT (v.payload ->> 'id')::int AS id,
       v.obj_id                  AS nodeid,
       (v.payload ->> 'structure')::text,
       (v.payload ->> 'state')::text,
       (v.payload ->> 'purpose')::text,
       (v.payload ->> 'calcmixfactcoeff')::double precision,
       (v.payload ->> 'calcthrustlos')::double precision,
       (v.payload ->> 'expendcharstraight')::text,
       (v.payload ->> 'expendcharvert')::text,
       (v.payload ->> 'authority')::double precision,
       (v.payload ->> 'calccapdeviation')::integer,
       (v.payload ->> 'type')::text,
       (v.payload ->> 'capacity')::double precision,
       (v.payload ->> 'regulator')::text,
       (v.payload ->> 'maxpd')::double precision,
       (v.payload ->> 'diameternomin')::double precision,
       (v.payload ->> 'calcexpendnodemix')::double precision,
       (v.payload ->> 'pumpstationname')::text
FROM net.object_variant v
WHERE v.src_table = 'threewayvalves' AND NOT v.chosen;

CREATE OR REPLACE FUNCTION net.v_threewayvalves_ins() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    IF NEW.nodeid IS NULL THEN
        RAISE EXCEPTION 'threewayvalves: не указан nodeid';
    END IF;
    -- Тип объекта задаётся именно этой вставкой: переносим его в класс.
    PERFORM net.reclass_node(NEW.nodeid, 'valve_3way');
    UPDATE net.valve_3way SET subtype_src_id = NEW.id,
        structure = NEW.structure,
        state = NEW.state,
        purpose = NEW.purpose,
        calcmixfactcoeff = NEW.calcmixfactcoeff,
        calcthrustlos = NEW.calcthrustlos,
        expendcharstraight = NEW.expendcharstraight,
        expendcharvert = NEW.expendcharvert,
        authority = NEW.authority,
        calccapdeviation = NEW.calccapdeviation,
        type = NEW.type,
        capacity = NEW.capacity,
        regulator = NEW.regulator,
        maxpd = NEW.maxpd,
        diameternomin = NEW.diameternomin,
        calcexpendnodemix = NEW.calcexpendnodemix,
        pumpstationname = NEW.pumpstationname
    WHERE id = NEW.nodeid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_threewayvalves_ins ON net.v_threewayvalves;
CREATE TRIGGER v_threewayvalves_ins INSTEAD OF INSERT ON net.v_threewayvalves
    FOR EACH ROW EXECUTE FUNCTION net.v_threewayvalves_ins();

CREATE OR REPLACE FUNCTION net.v_threewayvalves_upd() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    UPDATE net.valve_3way SET subtype_src_id = NEW.id,
        structure = NEW.structure,
        state = NEW.state,
        purpose = NEW.purpose,
        calcmixfactcoeff = NEW.calcmixfactcoeff,
        calcthrustlos = NEW.calcthrustlos,
        expendcharstraight = NEW.expendcharstraight,
        expendcharvert = NEW.expendcharvert,
        authority = NEW.authority,
        calccapdeviation = NEW.calccapdeviation,
        type = NEW.type,
        capacity = NEW.capacity,
        regulator = NEW.regulator,
        maxpd = NEW.maxpd,
        diameternomin = NEW.diameternomin,
        calcexpendnodemix = NEW.calcexpendnodemix,
        pumpstationname = NEW.pumpstationname
    WHERE id = OLD.nodeid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_threewayvalves_upd ON net.v_threewayvalves;
CREATE TRIGGER v_threewayvalves_upd INSTEAD OF UPDATE ON net.v_threewayvalves
    FOR EACH ROW EXECUTE FUNCTION net.v_threewayvalves_upd();

CREATE OR REPLACE FUNCTION net.v_threewayvalves_del() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    -- Удаление строки подтипа означает снятие типа, а не удаление объекта.
    PERFORM net.reclass_node(OLD.nodeid, 'node_plain');
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_threewayvalves_del ON net.v_threewayvalves;
CREATE TRIGGER v_threewayvalves_del INSTEAD OF DELETE ON net.v_threewayvalves
    FOR EACH ROW EXECUTE FUNCTION net.v_threewayvalves_del();


-- airheaters -> net.air_heater
-- DROP обязателен: CREATE OR REPLACE VIEW не умеет менять состав
-- и порядок колонок, а они меняются при правке карты типов.
DROP VIEW IF EXISTS net.v_airheaters CASCADE;
CREATE VIEW net.v_airheaters AS
SELECT o.subtype_src_id AS id,
       o.id             AS lineid,
       o.faninstall,
       o.scheme,
       o.airheaterscount,
       o.rowscount,
       o.storescount,
       o.airheatertype,
       o.contamincoeff,
       o.presscoeff,
       o.location
FROM net.air_heater o
UNION ALL
-- Строки, не ставшие объектом: дубли и «проигравшие» чужому классу.
-- Узел может иметь строки сразу в двух подтипах — старая модель это
-- допускала, и расчётное ядро читает обе. Без этой части выборка
-- вернула бы меньше строк, чем на исходной БД.
SELECT (v.payload ->> 'id')::int AS id,
       v.obj_id                  AS lineid,
       (v.payload ->> 'faninstall')::text,
       (v.payload ->> 'scheme')::text,
       (v.payload ->> 'airheaterscount')::double precision,
       (v.payload ->> 'rowscount')::double precision,
       (v.payload ->> 'storescount')::double precision,
       (v.payload ->> 'airheatertype')::text,
       (v.payload ->> 'contamincoeff')::double precision,
       (v.payload ->> 'presscoeff')::double precision,
       (v.payload ->> 'location')::text
FROM net.object_variant v
WHERE v.src_table = 'airheaters' AND NOT v.chosen;

CREATE OR REPLACE FUNCTION net.v_airheaters_ins() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    IF NEW.lineid IS NULL THEN
        RAISE EXCEPTION 'airheaters: не указан lineid';
    END IF;
    -- Тип объекта задаётся именно этой вставкой: переносим его в класс.
    PERFORM net.reclass_line(NEW.lineid, 'air_heater');
    UPDATE net.air_heater SET subtype_src_id = NEW.id,
        faninstall = NEW.faninstall,
        scheme = NEW.scheme,
        airheaterscount = NEW.airheaterscount,
        rowscount = NEW.rowscount,
        storescount = NEW.storescount,
        airheatertype = NEW.airheatertype,
        contamincoeff = NEW.contamincoeff,
        presscoeff = NEW.presscoeff,
        location = NEW.location
    WHERE id = NEW.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_airheaters_ins ON net.v_airheaters;
CREATE TRIGGER v_airheaters_ins INSTEAD OF INSERT ON net.v_airheaters
    FOR EACH ROW EXECUTE FUNCTION net.v_airheaters_ins();

CREATE OR REPLACE FUNCTION net.v_airheaters_upd() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    UPDATE net.air_heater SET subtype_src_id = NEW.id,
        faninstall = NEW.faninstall,
        scheme = NEW.scheme,
        airheaterscount = NEW.airheaterscount,
        rowscount = NEW.rowscount,
        storescount = NEW.storescount,
        airheatertype = NEW.airheatertype,
        contamincoeff = NEW.contamincoeff,
        presscoeff = NEW.presscoeff,
        location = NEW.location
    WHERE id = OLD.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_airheaters_upd ON net.v_airheaters;
CREATE TRIGGER v_airheaters_upd INSTEAD OF UPDATE ON net.v_airheaters
    FOR EACH ROW EXECUTE FUNCTION net.v_airheaters_upd();

CREATE OR REPLACE FUNCTION net.v_airheaters_del() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    -- Удаление строки подтипа означает снятие типа, а не удаление объекта.
    PERFORM net.reclass_line(OLD.lineid, 'line_plain');
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_airheaters_del ON net.v_airheaters;
CREATE TRIGGER v_airheaters_del INSTEAD OF DELETE ON net.v_airheaters
    FOR EACH ROW EXECUTE FUNCTION net.v_airheaters_del();


-- dampers -> net.damper
-- DROP обязателен: CREATE OR REPLACE VIEW не умеет менять состав
-- и порядок колонок, а они меняются при правке карты типов.
DROP VIEW IF EXISTS net.v_dampers CASCADE;
CREATE VIEW net.v_dampers AS
SELECT o.subtype_src_id AS id,
       o.id             AS lineid,
       o.dispatcherswitch,
       o.diametercondit,
       o.partdempopen,
       o.name,
       o.standarddamplink,
       o.turncount,
       o.gatecontrol,
       o.clue,
       o.thrustcollar,
       o.relatleakage,
       o.opc,
       o.damperarmaturestateid
FROM net.damper o
UNION ALL
-- Строки, не ставшие объектом: дубли и «проигравшие» чужому классу.
-- Узел может иметь строки сразу в двух подтипах — старая модель это
-- допускала, и расчётное ядро читает обе. Без этой части выборка
-- вернула бы меньше строк, чем на исходной БД.
SELECT (v.payload ->> 'id')::int AS id,
       v.obj_id                  AS lineid,
       (v.payload ->> 'dispatcherswitch')::text,
       (v.payload ->> 'diametercondit')::double precision,
       (v.payload ->> 'partdempopen')::double precision,
       (v.payload ->> 'name')::text,
       (v.payload ->> 'standarddamplink')::integer,
       (v.payload ->> 'turncount')::integer,
       (v.payload ->> 'gatecontrol')::integer,
       (v.payload ->> 'clue')::integer,
       (v.payload ->> 'thrustcollar')::integer,
       (v.payload ->> 'relatleakage')::double precision,
       (v.payload ->> 'opc')::text,
       (v.payload ->> 'damperarmaturestateid')::integer
FROM net.object_variant v
WHERE v.src_table = 'dampers' AND NOT v.chosen;

CREATE OR REPLACE FUNCTION net.v_dampers_ins() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    IF NEW.lineid IS NULL THEN
        RAISE EXCEPTION 'dampers: не указан lineid';
    END IF;
    -- Тип объекта задаётся именно этой вставкой: переносим его в класс.
    PERFORM net.reclass_line(NEW.lineid, 'damper');
    UPDATE net.damper SET subtype_src_id = NEW.id,
        dispatcherswitch = NEW.dispatcherswitch,
        diametercondit = NEW.diametercondit,
        partdempopen = NEW.partdempopen,
        name = NEW.name,
        standarddamplink = NEW.standarddamplink,
        turncount = NEW.turncount,
        gatecontrol = NEW.gatecontrol,
        clue = NEW.clue,
        thrustcollar = NEW.thrustcollar,
        relatleakage = NEW.relatleakage,
        opc = NEW.opc,
        damperarmaturestateid = NEW.damperarmaturestateid
    WHERE id = NEW.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_dampers_ins ON net.v_dampers;
CREATE TRIGGER v_dampers_ins INSTEAD OF INSERT ON net.v_dampers
    FOR EACH ROW EXECUTE FUNCTION net.v_dampers_ins();

CREATE OR REPLACE FUNCTION net.v_dampers_upd() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    UPDATE net.damper SET subtype_src_id = NEW.id,
        dispatcherswitch = NEW.dispatcherswitch,
        diametercondit = NEW.diametercondit,
        partdempopen = NEW.partdempopen,
        name = NEW.name,
        standarddamplink = NEW.standarddamplink,
        turncount = NEW.turncount,
        gatecontrol = NEW.gatecontrol,
        clue = NEW.clue,
        thrustcollar = NEW.thrustcollar,
        relatleakage = NEW.relatleakage,
        opc = NEW.opc,
        damperarmaturestateid = NEW.damperarmaturestateid
    WHERE id = OLD.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_dampers_upd ON net.v_dampers;
CREATE TRIGGER v_dampers_upd INSTEAD OF UPDATE ON net.v_dampers
    FOR EACH ROW EXECUTE FUNCTION net.v_dampers_upd();

CREATE OR REPLACE FUNCTION net.v_dampers_del() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    -- Удаление строки подтипа означает снятие типа, а не удаление объекта.
    PERFORM net.reclass_line(OLD.lineid, 'line_plain');
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_dampers_del ON net.v_dampers;
CREATE TRIGGER v_dampers_del INSTEAD OF DELETE ON net.v_dampers
    FOR EACH ROW EXECUTE FUNCTION net.v_dampers_del();


-- diaphragms -> net.diaphragm
-- DROP обязателен: CREATE OR REPLACE VIEW не умеет менять состав
-- и порядок колонок, а они меняются при правке карты типов.
DROP VIEW IF EXISTS net.v_diaphragms CASCADE;
CREATE VIEW net.v_diaphragms AS
SELECT o.subtype_src_id AS id,
       o.id             AS lineid,
       o.throtdiaphloc,
       o.diameterinternal,
       o.consinstdiaphcount,
       o.entrymark,
       o.stateid
FROM net.diaphragm o
UNION ALL
-- Строки, не ставшие объектом: дубли и «проигравшие» чужому классу.
-- Узел может иметь строки сразу в двух подтипах — старая модель это
-- допускала, и расчётное ядро читает обе. Без этой части выборка
-- вернула бы меньше строк, чем на исходной БД.
SELECT (v.payload ->> 'id')::int AS id,
       v.obj_id                  AS lineid,
       (v.payload ->> 'throtdiaphloc')::text,
       (v.payload ->> 'diameterinternal')::double precision,
       (v.payload ->> 'consinstdiaphcount')::integer,
       (v.payload ->> 'entrymark')::text,
       (v.payload ->> 'stateid')::integer
FROM net.object_variant v
WHERE v.src_table = 'diaphragms' AND NOT v.chosen;

CREATE OR REPLACE FUNCTION net.v_diaphragms_ins() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    IF NEW.lineid IS NULL THEN
        RAISE EXCEPTION 'diaphragms: не указан lineid';
    END IF;
    -- Тип объекта задаётся именно этой вставкой: переносим его в класс.
    PERFORM net.reclass_line(NEW.lineid, 'diaphragm');
    UPDATE net.diaphragm SET subtype_src_id = NEW.id,
        throtdiaphloc = NEW.throtdiaphloc,
        diameterinternal = NEW.diameterinternal,
        consinstdiaphcount = NEW.consinstdiaphcount,
        entrymark = NEW.entrymark,
        stateid = NEW.stateid
    WHERE id = NEW.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_diaphragms_ins ON net.v_diaphragms;
CREATE TRIGGER v_diaphragms_ins INSTEAD OF INSERT ON net.v_diaphragms
    FOR EACH ROW EXECUTE FUNCTION net.v_diaphragms_ins();

CREATE OR REPLACE FUNCTION net.v_diaphragms_upd() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    UPDATE net.diaphragm SET subtype_src_id = NEW.id,
        throtdiaphloc = NEW.throtdiaphloc,
        diameterinternal = NEW.diameterinternal,
        consinstdiaphcount = NEW.consinstdiaphcount,
        entrymark = NEW.entrymark,
        stateid = NEW.stateid
    WHERE id = OLD.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_diaphragms_upd ON net.v_diaphragms;
CREATE TRIGGER v_diaphragms_upd INSTEAD OF UPDATE ON net.v_diaphragms
    FOR EACH ROW EXECUTE FUNCTION net.v_diaphragms_upd();

CREATE OR REPLACE FUNCTION net.v_diaphragms_del() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    -- Удаление строки подтипа означает снятие типа, а не удаление объекта.
    PERFORM net.reclass_line(OLD.lineid, 'line_plain');
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_diaphragms_del ON net.v_diaphragms;
CREATE TRIGGER v_diaphragms_del INSTEAD OF DELETE ON net.v_diaphragms
    FOR EACH ROW EXECUTE FUNCTION net.v_diaphragms_del();


-- elevators -> net.elevator
-- DROP обязателен: CREATE OR REPLACE VIEW не умеет менять состав
-- и порядок колонок, а они меняются при правке карты типов.
DROP VIEW IF EXISTS net.v_elevators CASCADE;
CREATE VIEW net.v_elevators AS
SELECT o.subtype_src_id AS id,
       o.id             AS lineid,
       o.elevatortype,
       o.elevatornuminst,
       o.diameternozzle,
       o.entrymark,
       o.diameterchamber,
       o.length,
       o.diameterinletflange,
       o.diameteroutletflange,
       o.diametersuctionpipe,
       o.material,
       o.stateid
FROM net.elevator o
UNION ALL
-- Строки, не ставшие объектом: дубли и «проигравшие» чужому классу.
-- Узел может иметь строки сразу в двух подтипах — старая модель это
-- допускала, и расчётное ядро читает обе. Без этой части выборка
-- вернула бы меньше строк, чем на исходной БД.
SELECT (v.payload ->> 'id')::int AS id,
       v.obj_id                  AS lineid,
       (v.payload ->> 'elevatortype')::integer,
       (v.payload ->> 'elevatornuminst')::integer,
       (v.payload ->> 'diameternozzle')::double precision,
       (v.payload ->> 'entrymark')::text,
       (v.payload ->> 'diameterchamber')::double precision,
       (v.payload ->> 'length')::double precision,
       (v.payload ->> 'diameterinletflange')::double precision,
       (v.payload ->> 'diameteroutletflange')::double precision,
       (v.payload ->> 'diametersuctionpipe')::double precision,
       (v.payload ->> 'material')::text,
       (v.payload ->> 'stateid')::integer
FROM net.object_variant v
WHERE v.src_table = 'elevators' AND NOT v.chosen;

CREATE OR REPLACE FUNCTION net.v_elevators_ins() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    IF NEW.lineid IS NULL THEN
        RAISE EXCEPTION 'elevators: не указан lineid';
    END IF;
    -- Тип объекта задаётся именно этой вставкой: переносим его в класс.
    PERFORM net.reclass_line(NEW.lineid, 'elevator');
    UPDATE net.elevator SET subtype_src_id = NEW.id,
        elevatortype = NEW.elevatortype,
        elevatornuminst = NEW.elevatornuminst,
        diameternozzle = NEW.diameternozzle,
        entrymark = NEW.entrymark,
        diameterchamber = NEW.diameterchamber,
        length = NEW.length,
        diameterinletflange = NEW.diameterinletflange,
        diameteroutletflange = NEW.diameteroutletflange,
        diametersuctionpipe = NEW.diametersuctionpipe,
        material = NEW.material,
        stateid = NEW.stateid
    WHERE id = NEW.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_elevators_ins ON net.v_elevators;
CREATE TRIGGER v_elevators_ins INSTEAD OF INSERT ON net.v_elevators
    FOR EACH ROW EXECUTE FUNCTION net.v_elevators_ins();

CREATE OR REPLACE FUNCTION net.v_elevators_upd() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    UPDATE net.elevator SET subtype_src_id = NEW.id,
        elevatortype = NEW.elevatortype,
        elevatornuminst = NEW.elevatornuminst,
        diameternozzle = NEW.diameternozzle,
        entrymark = NEW.entrymark,
        diameterchamber = NEW.diameterchamber,
        length = NEW.length,
        diameterinletflange = NEW.diameterinletflange,
        diameteroutletflange = NEW.diameteroutletflange,
        diametersuctionpipe = NEW.diametersuctionpipe,
        material = NEW.material,
        stateid = NEW.stateid
    WHERE id = OLD.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_elevators_upd ON net.v_elevators;
CREATE TRIGGER v_elevators_upd INSTEAD OF UPDATE ON net.v_elevators
    FOR EACH ROW EXECUTE FUNCTION net.v_elevators_upd();

CREATE OR REPLACE FUNCTION net.v_elevators_del() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    -- Удаление строки подтипа означает снятие типа, а не удаление объекта.
    PERFORM net.reclass_line(OLD.lineid, 'line_plain');
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_elevators_del ON net.v_elevators;
CREATE TRIGGER v_elevators_del INSTEAD OF DELETE ON net.v_elevators
    FOR EACH ROW EXECUTE FUNCTION net.v_elevators_del();


-- heatexchangers -> net.heat_exchanger
-- DROP обязателен: CREATE OR REPLACE VIEW не умеет менять состав
-- и порядок колонок, а они меняются при правке карты типов.
DROP VIEW IF EXISTS net.v_heatexchangers CASCADE;
CREATE VIEW net.v_heatexchangers AS
SELECT o.subtype_src_id AS id,
       o.id             AS lineid,
       o.heatexchtype,
       o.heatexchcode,
       o.location,
       o.stateid
FROM net.heat_exchanger o
UNION ALL
-- Строки, не ставшие объектом: дубли и «проигравшие» чужому классу.
-- Узел может иметь строки сразу в двух подтипах — старая модель это
-- допускала, и расчётное ядро читает обе. Без этой части выборка
-- вернула бы меньше строк, чем на исходной БД.
SELECT (v.payload ->> 'id')::int AS id,
       v.obj_id                  AS lineid,
       (v.payload ->> 'heatexchtype')::text,
       (v.payload ->> 'heatexchcode')::integer,
       (v.payload ->> 'location')::text,
       (v.payload ->> 'stateid')::integer
FROM net.object_variant v
WHERE v.src_table = 'heatexchangers' AND NOT v.chosen;

CREATE OR REPLACE FUNCTION net.v_heatexchangers_ins() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    IF NEW.lineid IS NULL THEN
        RAISE EXCEPTION 'heatexchangers: не указан lineid';
    END IF;
    -- Тип объекта задаётся именно этой вставкой: переносим его в класс.
    PERFORM net.reclass_line(NEW.lineid, 'heat_exchanger');
    UPDATE net.heat_exchanger SET subtype_src_id = NEW.id,
        heatexchtype = NEW.heatexchtype,
        heatexchcode = NEW.heatexchcode,
        location = NEW.location,
        stateid = NEW.stateid
    WHERE id = NEW.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_heatexchangers_ins ON net.v_heatexchangers;
CREATE TRIGGER v_heatexchangers_ins INSTEAD OF INSERT ON net.v_heatexchangers
    FOR EACH ROW EXECUTE FUNCTION net.v_heatexchangers_ins();

CREATE OR REPLACE FUNCTION net.v_heatexchangers_upd() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    UPDATE net.heat_exchanger SET subtype_src_id = NEW.id,
        heatexchtype = NEW.heatexchtype,
        heatexchcode = NEW.heatexchcode,
        location = NEW.location,
        stateid = NEW.stateid
    WHERE id = OLD.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_heatexchangers_upd ON net.v_heatexchangers;
CREATE TRIGGER v_heatexchangers_upd INSTEAD OF UPDATE ON net.v_heatexchangers
    FOR EACH ROW EXECUTE FUNCTION net.v_heatexchangers_upd();

CREATE OR REPLACE FUNCTION net.v_heatexchangers_del() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    -- Удаление строки подтипа означает снятие типа, а не удаление объекта.
    PERFORM net.reclass_line(OLD.lineid, 'line_plain');
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_heatexchangers_del ON net.v_heatexchangers;
CREATE TRIGGER v_heatexchangers_del INSTEAD OF DELETE ON net.v_heatexchangers
    FOR EACH ROW EXECUTE FUNCTION net.v_heatexchangers_del();


-- heatpipesections -> net.pipe_section
-- DROP обязателен: CREATE OR REPLACE VIEW не умеет менять состав
-- и порядок колонок, а они меняются при правке карты типов.
DROP VIEW IF EXISTS net.v_heatpipesections CASCADE;
CREATE VIEW net.v_heatpipesections AS
SELECT o.subtype_src_id AS id,
       o.id             AS lineid,
       o.pipesectionid,
       o.pipesectstateidflow,
       o.pipesectstateidret,
       o.standardid,
       o.standardtubelink,
       o.tubescount,
       o.diameterinternal,
       o.diametercondit,
       o.diameterexternal,
       o.wallthickness,
       o.pipesectlength,
       o.tuberoughness,
       o.locallosesshare,
       o.localressum,
       o.varcoeffidflow,
       o.varcoeffidret,
       o.calcheatlossignid,
       o.tubingtypeid,
       o.piperemonttypeid,
       o.channelid,
       o.constrchanwidth,
       o.constrchanheight,
       o.heattestscoeff,
       o.signnumwork,
       o.isolmaterialid,
       o.isolthickness,
       o.isolmaterialhccoeff,
       o.pipelinelayingdepth,
       o.isolhtcoeffabove,
       o.isolhtcoeffunder,
       o.airgroundhtcoeffunder,
       o.groundhccoeff,
       o.pipelineaxesdist,
       o.damagenum,
       o.lasttransdate,
       o.lastisoldate,
       o.repairdatecapital,
       o.picdatecapital,
       o.repairdatemaint,
       o.picdatemaint,
       o.repairdateplantp,
       o.firstpicdatehp,
       o.lastmaintdatehp,
       o.sectexploitperiod,
       o.buildingconstrstateid,
       o.specdamagecoeff,
       o.specdamagecoeff2,
       o.powcabinstcount10,
       o.powcabinstcount5,
       o.powcabinstcount3,
       o.powcabinstcount1,
       o.powcabinstcount0,
       o.powcabinterscount,
       o.gasecpcount10,
       o.gasecpcount5,
       o.gasecpcount3,
       o.gasecpcount1,
       o.gasecpcount0,
       o.gasecpinterscount,
       o.eltranspcount10,
       o.eltranspcount5,
       o.eltranspcount3,
       o.eltranspcount1,
       o.eltranspcount0,
       o.eltranspinterscount,
       o.potentialdifflevelid,
       o.corrosiondegoutid,
       o.corrosiondeginid,
       o.floodintensitygwid,
       o.floodintensityfwid,
       o.floodintensitytwid,
       o.floodintensitybwid,
       o.jointstightnessinfr,
       o.chanconstrfract,
       o.projsoldeviation,
       o.constrbearreduce,
       o.fixedsuppdestr,
       o.sectwaterdumpid,
       o.breakcomplexityid,
       o.breakconsumvolid,
       o.erwdifficultyid,
       o.tubingpedestrianid,
       o.tubingwayid,
       o.populdamageid,
       o.infrastrdamageid,
       o.hydratestsdate,
       o.sectexpend,
       o.sectthrustloses,
       o.heattestsdate,
       o.temperoutair,
       o.temperground,
       o.expenddwflow,
       o.expenddwret,
       o.tempercoolflow,
       o.tempercoolret,
       o.temperdwflow,
       o.temperdwret,
       o.opc,
       o.tubecharactid,
       o.tubetypeid,
       o.tubematerial,
       o.tempermax,
       o.factorymanufid,
       o.externmaterialid,
       o.isolationtypeid,
       o.externcoverthick,
       o.anticorrmaterialid,
       o.magistralsite,
       o.distsite,
       o.exploitreg,
       o.net,
       o.magistral,
       o.exploitsite,
       o.nettype,
       o.crimpingquesite,
       o.h,
       o.deltah,
       o.deltaq,
       o.q,
       o.primechanie,
       o.name,
       o.inventnumber,
       o.objecttypeid,
       o.sreda,
       o.press,
       o.temperature,
       o.spoksluzhbirash,
       o.ressurs,
       o.pusk,
       o.organizationid,
       o.elektrich,
       o.transportelekricht,
       o.ponezial,
       o.vodootved,
       o.slozhokluzh,
       o.otkluzhgkal,
       o.avariivipoln,
       o.pesehod,
       o.zhddorogi,
       o.ludiproklad,
       o.uzherbludi,
       o.uzherbsity,
       o.vnesniivid,
       o.sostoborudovania,
       o.vnesnkorrozia,
       o.vnunrenkorrozia,
       o.sostkonstrukz,
       o.kategorii,
       o.mestn
FROM net.pipe_section o
UNION ALL
-- Строки, не ставшие объектом: дубли и «проигравшие» чужому классу.
-- Узел может иметь строки сразу в двух подтипах — старая модель это
-- допускала, и расчётное ядро читает обе. Без этой части выборка
-- вернула бы меньше строк, чем на исходной БД.
SELECT (v.payload ->> 'id')::int AS id,
       v.obj_id                  AS lineid,
       (v.payload ->> 'pipesectionid')::integer,
       (v.payload ->> 'pipesectstateidflow')::integer,
       (v.payload ->> 'pipesectstateidret')::integer,
       (v.payload ->> 'standardid')::integer,
       (v.payload ->> 'standardtubelink')::integer,
       (v.payload ->> 'tubescount')::integer,
       (v.payload ->> 'diameterinternal')::double precision,
       (v.payload ->> 'diametercondit')::double precision,
       (v.payload ->> 'diameterexternal')::double precision,
       (v.payload ->> 'wallthickness')::double precision,
       (v.payload ->> 'pipesectlength')::double precision,
       (v.payload ->> 'tuberoughness')::double precision,
       (v.payload ->> 'locallosesshare')::double precision,
       (v.payload ->> 'localressum')::double precision,
       (v.payload ->> 'varcoeffidflow')::integer,
       (v.payload ->> 'varcoeffidret')::integer,
       (v.payload ->> 'calcheatlossignid')::integer,
       (v.payload ->> 'tubingtypeid')::integer,
       (v.payload ->> 'piperemonttypeid')::integer,
       (v.payload ->> 'channelid')::integer,
       (v.payload ->> 'constrchanwidth')::double precision,
       (v.payload ->> 'constrchanheight')::double precision,
       (v.payload ->> 'heattestscoeff')::double precision,
       (v.payload ->> 'signnumwork')::integer,
       (v.payload ->> 'isolmaterialid')::integer,
       (v.payload ->> 'isolthickness')::double precision,
       (v.payload ->> 'isolmaterialhccoeff')::double precision,
       (v.payload ->> 'pipelinelayingdepth')::double precision,
       (v.payload ->> 'isolhtcoeffabove')::double precision,
       (v.payload ->> 'isolhtcoeffunder')::double precision,
       (v.payload ->> 'airgroundhtcoeffunder')::double precision,
       (v.payload ->> 'groundhccoeff')::double precision,
       (v.payload ->> 'pipelineaxesdist')::double precision,
       (v.payload ->> 'damagenum')::integer,
       (v.payload ->> 'lasttransdate')::date,
       (v.payload ->> 'lastisoldate')::date,
       (v.payload ->> 'repairdatecapital')::date,
       (v.payload ->> 'picdatecapital')::date,
       (v.payload ->> 'repairdatemaint')::date,
       (v.payload ->> 'picdatemaint')::date,
       (v.payload ->> 'repairdateplantp')::date,
       (v.payload ->> 'firstpicdatehp')::date,
       (v.payload ->> 'lastmaintdatehp')::date,
       (v.payload ->> 'sectexploitperiod')::integer,
       (v.payload ->> 'buildingconstrstateid')::integer,
       (v.payload ->> 'specdamagecoeff')::double precision,
       (v.payload ->> 'specdamagecoeff2')::double precision,
       (v.payload ->> 'powcabinstcount10')::integer,
       (v.payload ->> 'powcabinstcount5')::integer,
       (v.payload ->> 'powcabinstcount3')::integer,
       (v.payload ->> 'powcabinstcount1')::integer,
       (v.payload ->> 'powcabinstcount0')::integer,
       (v.payload ->> 'powcabinterscount')::integer,
       (v.payload ->> 'gasecpcount10')::integer,
       (v.payload ->> 'gasecpcount5')::integer,
       (v.payload ->> 'gasecpcount3')::integer,
       (v.payload ->> 'gasecpcount1')::integer,
       (v.payload ->> 'gasecpcount0')::integer,
       (v.payload ->> 'gasecpinterscount')::integer,
       (v.payload ->> 'eltranspcount10')::integer,
       (v.payload ->> 'eltranspcount5')::integer,
       (v.payload ->> 'eltranspcount3')::integer,
       (v.payload ->> 'eltranspcount1')::integer,
       (v.payload ->> 'eltranspcount0')::integer,
       (v.payload ->> 'eltranspinterscount')::integer,
       (v.payload ->> 'potentialdifflevelid')::integer,
       (v.payload ->> 'corrosiondegoutid')::integer,
       (v.payload ->> 'corrosiondeginid')::integer,
       (v.payload ->> 'floodintensitygwid')::integer,
       (v.payload ->> 'floodintensityfwid')::integer,
       (v.payload ->> 'floodintensitytwid')::integer,
       (v.payload ->> 'floodintensitybwid')::integer,
       (v.payload ->> 'jointstightnessinfr')::integer,
       (v.payload ->> 'chanconstrfract')::integer,
       (v.payload ->> 'projsoldeviation')::integer,
       (v.payload ->> 'constrbearreduce')::integer,
       (v.payload ->> 'fixedsuppdestr')::integer,
       (v.payload ->> 'sectwaterdumpid')::integer,
       (v.payload ->> 'breakcomplexityid')::integer,
       (v.payload ->> 'breakconsumvolid')::integer,
       (v.payload ->> 'erwdifficultyid')::integer,
       (v.payload ->> 'tubingpedestrianid')::integer,
       (v.payload ->> 'tubingwayid')::integer,
       (v.payload ->> 'populdamageid')::integer,
       (v.payload ->> 'infrastrdamageid')::integer,
       (v.payload ->> 'hydratestsdate')::date,
       (v.payload ->> 'sectexpend')::double precision,
       (v.payload ->> 'sectthrustloses')::double precision,
       (v.payload ->> 'heattestsdate')::date,
       (v.payload ->> 'temperoutair')::double precision,
       (v.payload ->> 'temperground')::double precision,
       (v.payload ->> 'expenddwflow')::double precision,
       (v.payload ->> 'expenddwret')::double precision,
       (v.payload ->> 'tempercoolflow')::double precision,
       (v.payload ->> 'tempercoolret')::double precision,
       (v.payload ->> 'temperdwflow')::double precision,
       (v.payload ->> 'temperdwret')::double precision,
       (v.payload ->> 'opc')::text,
       (v.payload ->> 'tubecharactid')::integer,
       (v.payload ->> 'tubetypeid')::integer,
       (v.payload ->> 'tubematerial')::text,
       (v.payload ->> 'tempermax')::double precision,
       (v.payload ->> 'factorymanufid')::integer,
       (v.payload ->> 'externmaterialid')::integer,
       (v.payload ->> 'isolationtypeid')::integer,
       (v.payload ->> 'externcoverthick')::double precision,
       (v.payload ->> 'anticorrmaterialid')::integer,
       (v.payload ->> 'magistralsite')::integer,
       (v.payload ->> 'distsite')::integer,
       (v.payload ->> 'exploitreg')::integer,
       (v.payload ->> 'net')::text,
       (v.payload ->> 'magistral')::integer,
       (v.payload ->> 'exploitsite')::integer,
       (v.payload ->> 'nettype')::integer,
       (v.payload ->> 'crimpingquesite')::integer,
       (v.payload ->> 'h')::double precision,
       (v.payload ->> 'deltah')::double precision,
       (v.payload ->> 'deltaq')::double precision,
       (v.payload ->> 'q')::double precision,
       (v.payload ->> 'primechanie')::text,
       (v.payload ->> 'name')::text,
       (v.payload ->> 'inventnumber')::text,
       (v.payload ->> 'objecttypeid')::integer,
       (v.payload ->> 'sreda')::integer,
       (v.payload ->> 'press')::double precision,
       (v.payload ->> 'temperature')::double precision,
       (v.payload ->> 'spoksluzhbirash')::integer,
       (v.payload ->> 'ressurs')::integer,
       (v.payload ->> 'pusk')::integer,
       (v.payload ->> 'organizationid')::integer,
       (v.payload ->> 'elektrich')::integer,
       (v.payload ->> 'transportelekricht')::integer,
       (v.payload ->> 'ponezial')::integer,
       (v.payload ->> 'vodootved')::integer,
       (v.payload ->> 'slozhokluzh')::integer,
       (v.payload ->> 'otkluzhgkal')::integer,
       (v.payload ->> 'avariivipoln')::integer,
       (v.payload ->> 'pesehod')::integer,
       (v.payload ->> 'zhddorogi')::integer,
       (v.payload ->> 'ludiproklad')::integer,
       (v.payload ->> 'uzherbludi')::integer,
       (v.payload ->> 'uzherbsity')::integer,
       (v.payload ->> 'vnesniivid')::integer,
       (v.payload ->> 'sostoborudovania')::integer,
       (v.payload ->> 'vnesnkorrozia')::integer,
       (v.payload ->> 'vnunrenkorrozia')::integer,
       (v.payload ->> 'sostkonstrukz')::integer,
       (v.payload ->> 'kategorii')::integer,
       (v.payload ->> 'mestn')::text
FROM net.object_variant v
WHERE v.src_table = 'heatpipesections' AND NOT v.chosen;

CREATE OR REPLACE FUNCTION net.v_heatpipesections_ins() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    IF NEW.lineid IS NULL THEN
        RAISE EXCEPTION 'heatpipesections: не указан lineid';
    END IF;
    -- Тип объекта задаётся именно этой вставкой: переносим его в класс.
    PERFORM net.reclass_line(NEW.lineid, 'pipe_section');
    UPDATE net.pipe_section SET subtype_src_id = NEW.id,
        pipesectionid = NEW.pipesectionid,
        pipesectstateidflow = NEW.pipesectstateidflow,
        pipesectstateidret = NEW.pipesectstateidret,
        standardid = NEW.standardid,
        standardtubelink = NEW.standardtubelink,
        tubescount = NEW.tubescount,
        diameterinternal = NEW.diameterinternal,
        diametercondit = NEW.diametercondit,
        diameterexternal = NEW.diameterexternal,
        wallthickness = NEW.wallthickness,
        pipesectlength = NEW.pipesectlength,
        tuberoughness = NEW.tuberoughness,
        locallosesshare = NEW.locallosesshare,
        localressum = NEW.localressum,
        varcoeffidflow = NEW.varcoeffidflow,
        varcoeffidret = NEW.varcoeffidret,
        calcheatlossignid = NEW.calcheatlossignid,
        tubingtypeid = NEW.tubingtypeid,
        piperemonttypeid = NEW.piperemonttypeid,
        channelid = NEW.channelid,
        constrchanwidth = NEW.constrchanwidth,
        constrchanheight = NEW.constrchanheight,
        heattestscoeff = NEW.heattestscoeff,
        signnumwork = NEW.signnumwork,
        isolmaterialid = NEW.isolmaterialid,
        isolthickness = NEW.isolthickness,
        isolmaterialhccoeff = NEW.isolmaterialhccoeff,
        pipelinelayingdepth = NEW.pipelinelayingdepth,
        isolhtcoeffabove = NEW.isolhtcoeffabove,
        isolhtcoeffunder = NEW.isolhtcoeffunder,
        airgroundhtcoeffunder = NEW.airgroundhtcoeffunder,
        groundhccoeff = NEW.groundhccoeff,
        pipelineaxesdist = NEW.pipelineaxesdist,
        damagenum = NEW.damagenum,
        lasttransdate = NEW.lasttransdate,
        lastisoldate = NEW.lastisoldate,
        repairdatecapital = NEW.repairdatecapital,
        picdatecapital = NEW.picdatecapital,
        repairdatemaint = NEW.repairdatemaint,
        picdatemaint = NEW.picdatemaint,
        repairdateplantp = NEW.repairdateplantp,
        firstpicdatehp = NEW.firstpicdatehp,
        lastmaintdatehp = NEW.lastmaintdatehp,
        sectexploitperiod = NEW.sectexploitperiod,
        buildingconstrstateid = NEW.buildingconstrstateid,
        specdamagecoeff = NEW.specdamagecoeff,
        specdamagecoeff2 = NEW.specdamagecoeff2,
        powcabinstcount10 = NEW.powcabinstcount10,
        powcabinstcount5 = NEW.powcabinstcount5,
        powcabinstcount3 = NEW.powcabinstcount3,
        powcabinstcount1 = NEW.powcabinstcount1,
        powcabinstcount0 = NEW.powcabinstcount0,
        powcabinterscount = NEW.powcabinterscount,
        gasecpcount10 = NEW.gasecpcount10,
        gasecpcount5 = NEW.gasecpcount5,
        gasecpcount3 = NEW.gasecpcount3,
        gasecpcount1 = NEW.gasecpcount1,
        gasecpcount0 = NEW.gasecpcount0,
        gasecpinterscount = NEW.gasecpinterscount,
        eltranspcount10 = NEW.eltranspcount10,
        eltranspcount5 = NEW.eltranspcount5,
        eltranspcount3 = NEW.eltranspcount3,
        eltranspcount1 = NEW.eltranspcount1,
        eltranspcount0 = NEW.eltranspcount0,
        eltranspinterscount = NEW.eltranspinterscount,
        potentialdifflevelid = NEW.potentialdifflevelid,
        corrosiondegoutid = NEW.corrosiondegoutid,
        corrosiondeginid = NEW.corrosiondeginid,
        floodintensitygwid = NEW.floodintensitygwid,
        floodintensityfwid = NEW.floodintensityfwid,
        floodintensitytwid = NEW.floodintensitytwid,
        floodintensitybwid = NEW.floodintensitybwid,
        jointstightnessinfr = NEW.jointstightnessinfr,
        chanconstrfract = NEW.chanconstrfract,
        projsoldeviation = NEW.projsoldeviation,
        constrbearreduce = NEW.constrbearreduce,
        fixedsuppdestr = NEW.fixedsuppdestr,
        sectwaterdumpid = NEW.sectwaterdumpid,
        breakcomplexityid = NEW.breakcomplexityid,
        breakconsumvolid = NEW.breakconsumvolid,
        erwdifficultyid = NEW.erwdifficultyid,
        tubingpedestrianid = NEW.tubingpedestrianid,
        tubingwayid = NEW.tubingwayid,
        populdamageid = NEW.populdamageid,
        infrastrdamageid = NEW.infrastrdamageid,
        hydratestsdate = NEW.hydratestsdate,
        sectexpend = NEW.sectexpend,
        sectthrustloses = NEW.sectthrustloses,
        heattestsdate = NEW.heattestsdate,
        temperoutair = NEW.temperoutair,
        temperground = NEW.temperground,
        expenddwflow = NEW.expenddwflow,
        expenddwret = NEW.expenddwret,
        tempercoolflow = NEW.tempercoolflow,
        tempercoolret = NEW.tempercoolret,
        temperdwflow = NEW.temperdwflow,
        temperdwret = NEW.temperdwret,
        opc = NEW.opc,
        tubecharactid = NEW.tubecharactid,
        tubetypeid = NEW.tubetypeid,
        tubematerial = NEW.tubematerial,
        tempermax = NEW.tempermax,
        factorymanufid = NEW.factorymanufid,
        externmaterialid = NEW.externmaterialid,
        isolationtypeid = NEW.isolationtypeid,
        externcoverthick = NEW.externcoverthick,
        anticorrmaterialid = NEW.anticorrmaterialid,
        magistralsite = NEW.magistralsite,
        distsite = NEW.distsite,
        exploitreg = NEW.exploitreg,
        net = NEW.net,
        magistral = NEW.magistral,
        exploitsite = NEW.exploitsite,
        nettype = NEW.nettype,
        crimpingquesite = NEW.crimpingquesite,
        h = NEW.h,
        deltah = NEW.deltah,
        deltaq = NEW.deltaq,
        q = NEW.q,
        primechanie = NEW.primechanie,
        name = NEW.name,
        inventnumber = NEW.inventnumber,
        objecttypeid = NEW.objecttypeid,
        sreda = NEW.sreda,
        press = NEW.press,
        temperature = NEW.temperature,
        spoksluzhbirash = NEW.spoksluzhbirash,
        ressurs = NEW.ressurs,
        pusk = NEW.pusk,
        organizationid = NEW.organizationid,
        elektrich = NEW.elektrich,
        transportelekricht = NEW.transportelekricht,
        ponezial = NEW.ponezial,
        vodootved = NEW.vodootved,
        slozhokluzh = NEW.slozhokluzh,
        otkluzhgkal = NEW.otkluzhgkal,
        avariivipoln = NEW.avariivipoln,
        pesehod = NEW.pesehod,
        zhddorogi = NEW.zhddorogi,
        ludiproklad = NEW.ludiproklad,
        uzherbludi = NEW.uzherbludi,
        uzherbsity = NEW.uzherbsity,
        vnesniivid = NEW.vnesniivid,
        sostoborudovania = NEW.sostoborudovania,
        vnesnkorrozia = NEW.vnesnkorrozia,
        vnunrenkorrozia = NEW.vnunrenkorrozia,
        sostkonstrukz = NEW.sostkonstrukz,
        kategorii = NEW.kategorii,
        mestn = NEW.mestn
    WHERE id = NEW.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_heatpipesections_ins ON net.v_heatpipesections;
CREATE TRIGGER v_heatpipesections_ins INSTEAD OF INSERT ON net.v_heatpipesections
    FOR EACH ROW EXECUTE FUNCTION net.v_heatpipesections_ins();

CREATE OR REPLACE FUNCTION net.v_heatpipesections_upd() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    UPDATE net.pipe_section SET subtype_src_id = NEW.id,
        pipesectionid = NEW.pipesectionid,
        pipesectstateidflow = NEW.pipesectstateidflow,
        pipesectstateidret = NEW.pipesectstateidret,
        standardid = NEW.standardid,
        standardtubelink = NEW.standardtubelink,
        tubescount = NEW.tubescount,
        diameterinternal = NEW.diameterinternal,
        diametercondit = NEW.diametercondit,
        diameterexternal = NEW.diameterexternal,
        wallthickness = NEW.wallthickness,
        pipesectlength = NEW.pipesectlength,
        tuberoughness = NEW.tuberoughness,
        locallosesshare = NEW.locallosesshare,
        localressum = NEW.localressum,
        varcoeffidflow = NEW.varcoeffidflow,
        varcoeffidret = NEW.varcoeffidret,
        calcheatlossignid = NEW.calcheatlossignid,
        tubingtypeid = NEW.tubingtypeid,
        piperemonttypeid = NEW.piperemonttypeid,
        channelid = NEW.channelid,
        constrchanwidth = NEW.constrchanwidth,
        constrchanheight = NEW.constrchanheight,
        heattestscoeff = NEW.heattestscoeff,
        signnumwork = NEW.signnumwork,
        isolmaterialid = NEW.isolmaterialid,
        isolthickness = NEW.isolthickness,
        isolmaterialhccoeff = NEW.isolmaterialhccoeff,
        pipelinelayingdepth = NEW.pipelinelayingdepth,
        isolhtcoeffabove = NEW.isolhtcoeffabove,
        isolhtcoeffunder = NEW.isolhtcoeffunder,
        airgroundhtcoeffunder = NEW.airgroundhtcoeffunder,
        groundhccoeff = NEW.groundhccoeff,
        pipelineaxesdist = NEW.pipelineaxesdist,
        damagenum = NEW.damagenum,
        lasttransdate = NEW.lasttransdate,
        lastisoldate = NEW.lastisoldate,
        repairdatecapital = NEW.repairdatecapital,
        picdatecapital = NEW.picdatecapital,
        repairdatemaint = NEW.repairdatemaint,
        picdatemaint = NEW.picdatemaint,
        repairdateplantp = NEW.repairdateplantp,
        firstpicdatehp = NEW.firstpicdatehp,
        lastmaintdatehp = NEW.lastmaintdatehp,
        sectexploitperiod = NEW.sectexploitperiod,
        buildingconstrstateid = NEW.buildingconstrstateid,
        specdamagecoeff = NEW.specdamagecoeff,
        specdamagecoeff2 = NEW.specdamagecoeff2,
        powcabinstcount10 = NEW.powcabinstcount10,
        powcabinstcount5 = NEW.powcabinstcount5,
        powcabinstcount3 = NEW.powcabinstcount3,
        powcabinstcount1 = NEW.powcabinstcount1,
        powcabinstcount0 = NEW.powcabinstcount0,
        powcabinterscount = NEW.powcabinterscount,
        gasecpcount10 = NEW.gasecpcount10,
        gasecpcount5 = NEW.gasecpcount5,
        gasecpcount3 = NEW.gasecpcount3,
        gasecpcount1 = NEW.gasecpcount1,
        gasecpcount0 = NEW.gasecpcount0,
        gasecpinterscount = NEW.gasecpinterscount,
        eltranspcount10 = NEW.eltranspcount10,
        eltranspcount5 = NEW.eltranspcount5,
        eltranspcount3 = NEW.eltranspcount3,
        eltranspcount1 = NEW.eltranspcount1,
        eltranspcount0 = NEW.eltranspcount0,
        eltranspinterscount = NEW.eltranspinterscount,
        potentialdifflevelid = NEW.potentialdifflevelid,
        corrosiondegoutid = NEW.corrosiondegoutid,
        corrosiondeginid = NEW.corrosiondeginid,
        floodintensitygwid = NEW.floodintensitygwid,
        floodintensityfwid = NEW.floodintensityfwid,
        floodintensitytwid = NEW.floodintensitytwid,
        floodintensitybwid = NEW.floodintensitybwid,
        jointstightnessinfr = NEW.jointstightnessinfr,
        chanconstrfract = NEW.chanconstrfract,
        projsoldeviation = NEW.projsoldeviation,
        constrbearreduce = NEW.constrbearreduce,
        fixedsuppdestr = NEW.fixedsuppdestr,
        sectwaterdumpid = NEW.sectwaterdumpid,
        breakcomplexityid = NEW.breakcomplexityid,
        breakconsumvolid = NEW.breakconsumvolid,
        erwdifficultyid = NEW.erwdifficultyid,
        tubingpedestrianid = NEW.tubingpedestrianid,
        tubingwayid = NEW.tubingwayid,
        populdamageid = NEW.populdamageid,
        infrastrdamageid = NEW.infrastrdamageid,
        hydratestsdate = NEW.hydratestsdate,
        sectexpend = NEW.sectexpend,
        sectthrustloses = NEW.sectthrustloses,
        heattestsdate = NEW.heattestsdate,
        temperoutair = NEW.temperoutair,
        temperground = NEW.temperground,
        expenddwflow = NEW.expenddwflow,
        expenddwret = NEW.expenddwret,
        tempercoolflow = NEW.tempercoolflow,
        tempercoolret = NEW.tempercoolret,
        temperdwflow = NEW.temperdwflow,
        temperdwret = NEW.temperdwret,
        opc = NEW.opc,
        tubecharactid = NEW.tubecharactid,
        tubetypeid = NEW.tubetypeid,
        tubematerial = NEW.tubematerial,
        tempermax = NEW.tempermax,
        factorymanufid = NEW.factorymanufid,
        externmaterialid = NEW.externmaterialid,
        isolationtypeid = NEW.isolationtypeid,
        externcoverthick = NEW.externcoverthick,
        anticorrmaterialid = NEW.anticorrmaterialid,
        magistralsite = NEW.magistralsite,
        distsite = NEW.distsite,
        exploitreg = NEW.exploitreg,
        net = NEW.net,
        magistral = NEW.magistral,
        exploitsite = NEW.exploitsite,
        nettype = NEW.nettype,
        crimpingquesite = NEW.crimpingquesite,
        h = NEW.h,
        deltah = NEW.deltah,
        deltaq = NEW.deltaq,
        q = NEW.q,
        primechanie = NEW.primechanie,
        name = NEW.name,
        inventnumber = NEW.inventnumber,
        objecttypeid = NEW.objecttypeid,
        sreda = NEW.sreda,
        press = NEW.press,
        temperature = NEW.temperature,
        spoksluzhbirash = NEW.spoksluzhbirash,
        ressurs = NEW.ressurs,
        pusk = NEW.pusk,
        organizationid = NEW.organizationid,
        elektrich = NEW.elektrich,
        transportelekricht = NEW.transportelekricht,
        ponezial = NEW.ponezial,
        vodootved = NEW.vodootved,
        slozhokluzh = NEW.slozhokluzh,
        otkluzhgkal = NEW.otkluzhgkal,
        avariivipoln = NEW.avariivipoln,
        pesehod = NEW.pesehod,
        zhddorogi = NEW.zhddorogi,
        ludiproklad = NEW.ludiproklad,
        uzherbludi = NEW.uzherbludi,
        uzherbsity = NEW.uzherbsity,
        vnesniivid = NEW.vnesniivid,
        sostoborudovania = NEW.sostoborudovania,
        vnesnkorrozia = NEW.vnesnkorrozia,
        vnunrenkorrozia = NEW.vnunrenkorrozia,
        sostkonstrukz = NEW.sostkonstrukz,
        kategorii = NEW.kategorii,
        mestn = NEW.mestn
    WHERE id = OLD.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_heatpipesections_upd ON net.v_heatpipesections;
CREATE TRIGGER v_heatpipesections_upd INSTEAD OF UPDATE ON net.v_heatpipesections
    FOR EACH ROW EXECUTE FUNCTION net.v_heatpipesections_upd();

CREATE OR REPLACE FUNCTION net.v_heatpipesections_del() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    -- Удаление строки подтипа означает снятие типа, а не удаление объекта.
    PERFORM net.reclass_line(OLD.lineid, 'line_plain');
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_heatpipesections_del ON net.v_heatpipesections;
CREATE TRIGGER v_heatpipesections_del INSTEAD OF DELETE ON net.v_heatpipesections
    FOR EACH ROW EXECUTE FUNCTION net.v_heatpipesections_del();


-- localhydroresistances2 -> net.local_resistance
-- DROP обязателен: CREATE OR REPLACE VIEW не умеет менять состав
-- и порядок колонок, а они меняются при правке карты типов.
DROP VIEW IF EXISTS net.v_localhydroresistances2 CASCADE;
CREATE VIEW net.v_localhydroresistances2 AS
SELECT o.subtype_src_id AS id,
       o.id             AS lineid,
       o.name_mest,
       o.s_mest,
       o.k_mest,
       o.sum_mest
FROM net.local_resistance o
UNION ALL
-- Строки, не ставшие объектом: дубли и «проигравшие» чужому классу.
-- Узел может иметь строки сразу в двух подтипах — старая модель это
-- допускала, и расчётное ядро читает обе. Без этой части выборка
-- вернула бы меньше строк, чем на исходной БД.
SELECT (v.payload ->> 'id')::int AS id,
       v.obj_id                  AS lineid,
       (v.payload ->> 'name_mest')::text,
       (v.payload ->> 's_mest')::double precision,
       (v.payload ->> 'k_mest')::integer,
       (v.payload ->> 'sum_mest')::double precision
FROM net.object_variant v
WHERE v.src_table = 'localhydroresistances2' AND NOT v.chosen;

CREATE OR REPLACE FUNCTION net.v_localhydroresistances2_ins() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    IF NEW.lineid IS NULL THEN
        RAISE EXCEPTION 'localhydroresistances2: не указан lineid';
    END IF;
    -- Тип объекта задаётся именно этой вставкой: переносим его в класс.
    PERFORM net.reclass_line(NEW.lineid, 'local_resistance');
    UPDATE net.local_resistance SET subtype_src_id = NEW.id,
        name_mest = NEW.name_mest,
        s_mest = NEW.s_mest,
        k_mest = NEW.k_mest,
        sum_mest = NEW.sum_mest
    WHERE id = NEW.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_localhydroresistances2_ins ON net.v_localhydroresistances2;
CREATE TRIGGER v_localhydroresistances2_ins INSTEAD OF INSERT ON net.v_localhydroresistances2
    FOR EACH ROW EXECUTE FUNCTION net.v_localhydroresistances2_ins();

CREATE OR REPLACE FUNCTION net.v_localhydroresistances2_upd() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    UPDATE net.local_resistance SET subtype_src_id = NEW.id,
        name_mest = NEW.name_mest,
        s_mest = NEW.s_mest,
        k_mest = NEW.k_mest,
        sum_mest = NEW.sum_mest
    WHERE id = OLD.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_localhydroresistances2_upd ON net.v_localhydroresistances2;
CREATE TRIGGER v_localhydroresistances2_upd INSTEAD OF UPDATE ON net.v_localhydroresistances2
    FOR EACH ROW EXECUTE FUNCTION net.v_localhydroresistances2_upd();

CREATE OR REPLACE FUNCTION net.v_localhydroresistances2_del() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    -- Удаление строки подтипа означает снятие типа, а не удаление объекта.
    PERFORM net.reclass_line(OLD.lineid, 'line_plain');
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_localhydroresistances2_del ON net.v_localhydroresistances2;
CREATE TRIGGER v_localhydroresistances2_del INSTEAD OF DELETE ON net.v_localhydroresistances2
    FOR EACH ROW EXECUTE FUNCTION net.v_localhydroresistances2_del();


-- pressregulators -> net.regulator_press
-- DROP обязателен: CREATE OR REPLACE VIEW не умеет менять состав
-- и порядок колонок, а они меняются при правке карты типов.
DROP VIEW IF EXISTS net.v_pressregulators CASCADE;
CREATE VIEW net.v_pressregulators AS
SELECT o.subtype_src_id AS id,
       o.id             AS lineid,
       o.nodeid,
       o.valvehydroresopen,
       o.valvehydroresclose,
       o.regvalverelcap,
       o.relleakage,
       o.consdrip,
       o.workattrid,
       o.deltah,
       o.regulatorstateid,
       o.h,
       o.pipelinesignid
FROM net.regulator_press o
UNION ALL
-- Строки, не ставшие объектом: дубли и «проигравшие» чужому классу.
-- Узел может иметь строки сразу в двух подтипах — старая модель это
-- допускала, и расчётное ядро читает обе. Без этой части выборка
-- вернула бы меньше строк, чем на исходной БД.
SELECT (v.payload ->> 'id')::int AS id,
       v.obj_id                  AS lineid,
       (v.payload ->> 'nodeid')::integer,
       (v.payload ->> 'valvehydroresopen')::double precision,
       (v.payload ->> 'valvehydroresclose')::double precision,
       (v.payload ->> 'regvalverelcap')::integer,
       (v.payload ->> 'relleakage')::double precision,
       (v.payload ->> 'consdrip')::double precision,
       (v.payload ->> 'workattrid')::integer,
       (v.payload ->> 'deltah')::double precision,
       (v.payload ->> 'regulatorstateid')::integer,
       (v.payload ->> 'h')::double precision,
       (v.payload ->> 'pipelinesignid')::integer
FROM net.object_variant v
WHERE v.src_table = 'pressregulators' AND NOT v.chosen;

CREATE OR REPLACE FUNCTION net.v_pressregulators_ins() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    IF NEW.lineid IS NULL THEN
        RAISE EXCEPTION 'pressregulators: не указан lineid';
    END IF;
    -- Тип объекта задаётся именно этой вставкой: переносим его в класс.
    PERFORM net.reclass_line(NEW.lineid, 'regulator_press');
    UPDATE net.regulator_press SET subtype_src_id = NEW.id,
        nodeid = NEW.nodeid,
        valvehydroresopen = NEW.valvehydroresopen,
        valvehydroresclose = NEW.valvehydroresclose,
        regvalverelcap = NEW.regvalverelcap,
        relleakage = NEW.relleakage,
        consdrip = NEW.consdrip,
        workattrid = NEW.workattrid,
        deltah = NEW.deltah,
        regulatorstateid = NEW.regulatorstateid,
        h = NEW.h,
        pipelinesignid = NEW.pipelinesignid
    WHERE id = NEW.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_pressregulators_ins ON net.v_pressregulators;
CREATE TRIGGER v_pressregulators_ins INSTEAD OF INSERT ON net.v_pressregulators
    FOR EACH ROW EXECUTE FUNCTION net.v_pressregulators_ins();

CREATE OR REPLACE FUNCTION net.v_pressregulators_upd() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    UPDATE net.regulator_press SET subtype_src_id = NEW.id,
        nodeid = NEW.nodeid,
        valvehydroresopen = NEW.valvehydroresopen,
        valvehydroresclose = NEW.valvehydroresclose,
        regvalverelcap = NEW.regvalverelcap,
        relleakage = NEW.relleakage,
        consdrip = NEW.consdrip,
        workattrid = NEW.workattrid,
        deltah = NEW.deltah,
        regulatorstateid = NEW.regulatorstateid,
        h = NEW.h,
        pipelinesignid = NEW.pipelinesignid
    WHERE id = OLD.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_pressregulators_upd ON net.v_pressregulators;
CREATE TRIGGER v_pressregulators_upd INSTEAD OF UPDATE ON net.v_pressregulators
    FOR EACH ROW EXECUTE FUNCTION net.v_pressregulators_upd();

CREATE OR REPLACE FUNCTION net.v_pressregulators_del() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    -- Удаление строки подтипа означает снятие типа, а не удаление объекта.
    PERFORM net.reclass_line(OLD.lineid, 'line_plain');
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_pressregulators_del ON net.v_pressregulators;
CREATE TRIGGER v_pressregulators_del INSTEAD OF DELETE ON net.v_pressregulators
    FOR EACH ROW EXECUTE FUNCTION net.v_pressregulators_del();


-- pumps -> net.pump
-- DROP обязателен: CREATE OR REPLACE VIEW не умеет менять состав
-- и порядок колонок, а они меняются при правке карты типов.
DROP VIEW IF EXISTS net.v_pumps CASCADE;
CREATE VIEW net.v_pumps AS
SELECT o.subtype_src_id AS id,
       o.id             AS lineid,
       o.offreason,
       o.pumpstationid,
       o.number,
       o.thrust,
       o.standardpumpid,
       o.parallagregcount,
       o.drivetypeid,
       o.rotordiametertypeid,
       o.standardemid,
       o.r0,
       o.r1,
       o.r2,
       o.e0,
       o.e1,
       o.e2,
       o.k0,
       o.k1,
       o.k2,
       o.r0_z,
       o.r1_z,
       o.r2_z,
       o.e0_z,
       o.e1_z,
       o.e2_z,
       o.k0_z,
       o.k1_z,
       o.k2_z,
       o.rotorrotspeedset,
       o.rotordiameterset,
       o.lastpumpreplacedate,
       o.lastemreplacedate,
       o.lastagregreplacedate,
       o.repaircountpump,
       o.replacecountagreg,
       o.repaircountem,
       o.opc,
       o.stateid
FROM net.pump o
UNION ALL
-- Строки, не ставшие объектом: дубли и «проигравшие» чужому классу.
-- Узел может иметь строки сразу в двух подтипах — старая модель это
-- допускала, и расчётное ядро читает обе. Без этой части выборка
-- вернула бы меньше строк, чем на исходной БД.
SELECT (v.payload ->> 'id')::int AS id,
       v.obj_id                  AS lineid,
       (v.payload ->> 'offreason')::integer,
       (v.payload ->> 'pumpstationid')::text,
       (v.payload ->> 'number')::text,
       (v.payload ->> 'thrust')::double precision,
       (v.payload ->> 'standardpumpid')::integer,
       (v.payload ->> 'parallagregcount')::integer,
       (v.payload ->> 'drivetypeid')::integer,
       (v.payload ->> 'rotordiametertypeid')::integer,
       (v.payload ->> 'standardemid')::integer,
       (v.payload ->> 'r0')::double precision,
       (v.payload ->> 'r1')::double precision,
       (v.payload ->> 'r2')::double precision,
       (v.payload ->> 'e0')::double precision,
       (v.payload ->> 'e1')::double precision,
       (v.payload ->> 'e2')::double precision,
       (v.payload ->> 'k0')::double precision,
       (v.payload ->> 'k1')::double precision,
       (v.payload ->> 'k2')::double precision,
       (v.payload ->> 'r0_z')::double precision,
       (v.payload ->> 'r1_z')::double precision,
       (v.payload ->> 'r2_z')::double precision,
       (v.payload ->> 'e0_z')::double precision,
       (v.payload ->> 'e1_z')::double precision,
       (v.payload ->> 'e2_z')::double precision,
       (v.payload ->> 'k0_z')::double precision,
       (v.payload ->> 'k1_z')::double precision,
       (v.payload ->> 'k2_z')::double precision,
       (v.payload ->> 'rotorrotspeedset')::double precision,
       (v.payload ->> 'rotordiameterset')::double precision,
       (v.payload ->> 'lastpumpreplacedate')::date,
       (v.payload ->> 'lastemreplacedate')::date,
       (v.payload ->> 'lastagregreplacedate')::date,
       (v.payload ->> 'repaircountpump')::integer,
       (v.payload ->> 'replacecountagreg')::integer,
       (v.payload ->> 'repaircountem')::integer,
       (v.payload ->> 'opc')::text,
       (v.payload ->> 'stateid')::integer
FROM net.object_variant v
WHERE v.src_table = 'pumps' AND NOT v.chosen;

CREATE OR REPLACE FUNCTION net.v_pumps_ins() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    IF NEW.lineid IS NULL THEN
        RAISE EXCEPTION 'pumps: не указан lineid';
    END IF;
    -- Тип объекта задаётся именно этой вставкой: переносим его в класс.
    PERFORM net.reclass_line(NEW.lineid, 'pump');
    UPDATE net.pump SET subtype_src_id = NEW.id,
        offreason = NEW.offreason,
        pumpstationid = NEW.pumpstationid,
        number = NEW.number,
        thrust = NEW.thrust,
        standardpumpid = NEW.standardpumpid,
        parallagregcount = NEW.parallagregcount,
        drivetypeid = NEW.drivetypeid,
        rotordiametertypeid = NEW.rotordiametertypeid,
        standardemid = NEW.standardemid,
        r0 = NEW.r0,
        r1 = NEW.r1,
        r2 = NEW.r2,
        e0 = NEW.e0,
        e1 = NEW.e1,
        e2 = NEW.e2,
        k0 = NEW.k0,
        k1 = NEW.k1,
        k2 = NEW.k2,
        r0_z = NEW.r0_z,
        r1_z = NEW.r1_z,
        r2_z = NEW.r2_z,
        e0_z = NEW.e0_z,
        e1_z = NEW.e1_z,
        e2_z = NEW.e2_z,
        k0_z = NEW.k0_z,
        k1_z = NEW.k1_z,
        k2_z = NEW.k2_z,
        rotorrotspeedset = NEW.rotorrotspeedset,
        rotordiameterset = NEW.rotordiameterset,
        lastpumpreplacedate = NEW.lastpumpreplacedate,
        lastemreplacedate = NEW.lastemreplacedate,
        lastagregreplacedate = NEW.lastagregreplacedate,
        repaircountpump = NEW.repaircountpump,
        replacecountagreg = NEW.replacecountagreg,
        repaircountem = NEW.repaircountem,
        opc = NEW.opc,
        stateid = NEW.stateid
    WHERE id = NEW.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_pumps_ins ON net.v_pumps;
CREATE TRIGGER v_pumps_ins INSTEAD OF INSERT ON net.v_pumps
    FOR EACH ROW EXECUTE FUNCTION net.v_pumps_ins();

CREATE OR REPLACE FUNCTION net.v_pumps_upd() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    UPDATE net.pump SET subtype_src_id = NEW.id,
        offreason = NEW.offreason,
        pumpstationid = NEW.pumpstationid,
        number = NEW.number,
        thrust = NEW.thrust,
        standardpumpid = NEW.standardpumpid,
        parallagregcount = NEW.parallagregcount,
        drivetypeid = NEW.drivetypeid,
        rotordiametertypeid = NEW.rotordiametertypeid,
        standardemid = NEW.standardemid,
        r0 = NEW.r0,
        r1 = NEW.r1,
        r2 = NEW.r2,
        e0 = NEW.e0,
        e1 = NEW.e1,
        e2 = NEW.e2,
        k0 = NEW.k0,
        k1 = NEW.k1,
        k2 = NEW.k2,
        r0_z = NEW.r0_z,
        r1_z = NEW.r1_z,
        r2_z = NEW.r2_z,
        e0_z = NEW.e0_z,
        e1_z = NEW.e1_z,
        e2_z = NEW.e2_z,
        k0_z = NEW.k0_z,
        k1_z = NEW.k1_z,
        k2_z = NEW.k2_z,
        rotorrotspeedset = NEW.rotorrotspeedset,
        rotordiameterset = NEW.rotordiameterset,
        lastpumpreplacedate = NEW.lastpumpreplacedate,
        lastemreplacedate = NEW.lastemreplacedate,
        lastagregreplacedate = NEW.lastagregreplacedate,
        repaircountpump = NEW.repaircountpump,
        replacecountagreg = NEW.replacecountagreg,
        repaircountem = NEW.repaircountem,
        opc = NEW.opc,
        stateid = NEW.stateid
    WHERE id = OLD.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_pumps_upd ON net.v_pumps;
CREATE TRIGGER v_pumps_upd INSTEAD OF UPDATE ON net.v_pumps
    FOR EACH ROW EXECUTE FUNCTION net.v_pumps_upd();

CREATE OR REPLACE FUNCTION net.v_pumps_del() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    -- Удаление строки подтипа означает снятие типа, а не удаление объекта.
    PERFORM net.reclass_line(OLD.lineid, 'line_plain');
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_pumps_del ON net.v_pumps;
CREATE TRIGGER v_pumps_del INSTEAD OF DELETE ON net.v_pumps
    FOR EACH ROW EXECUTE FUNCTION net.v_pumps_del();


-- systemradiators -> net.radiator
-- DROP обязателен: CREATE OR REPLACE VIEW не умеет менять состав
-- и порядок колонок, а они меняются при правке карты типов.
DROP VIEW IF EXISTS net.v_systemradiators CASCADE;
CREATE VIEW net.v_systemradiators AS
SELECT o.subtype_src_id AS id,
       o.id             AS lineid,
       o.name,
       o.type,
       o.count,
       o.totalequivsurface,
       o.stateid
FROM net.radiator o
UNION ALL
-- Строки, не ставшие объектом: дубли и «проигравшие» чужому классу.
-- Узел может иметь строки сразу в двух подтипах — старая модель это
-- допускала, и расчётное ядро читает обе. Без этой части выборка
-- вернула бы меньше строк, чем на исходной БД.
SELECT (v.payload ->> 'id')::int AS id,
       v.obj_id                  AS lineid,
       (v.payload ->> 'name')::text,
       (v.payload ->> 'type')::text,
       (v.payload ->> 'count')::text,
       (v.payload ->> 'totalequivsurface')::text,
       (v.payload ->> 'stateid')::integer
FROM net.object_variant v
WHERE v.src_table = 'systemradiators' AND NOT v.chosen;

CREATE OR REPLACE FUNCTION net.v_systemradiators_ins() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    IF NEW.lineid IS NULL THEN
        RAISE EXCEPTION 'systemradiators: не указан lineid';
    END IF;
    -- Тип объекта задаётся именно этой вставкой: переносим его в класс.
    PERFORM net.reclass_line(NEW.lineid, 'radiator');
    UPDATE net.radiator SET subtype_src_id = NEW.id,
        name = NEW.name,
        type = NEW.type,
        count = NEW.count,
        totalequivsurface = NEW.totalequivsurface,
        stateid = NEW.stateid
    WHERE id = NEW.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_systemradiators_ins ON net.v_systemradiators;
CREATE TRIGGER v_systemradiators_ins INSTEAD OF INSERT ON net.v_systemradiators
    FOR EACH ROW EXECUTE FUNCTION net.v_systemradiators_ins();

CREATE OR REPLACE FUNCTION net.v_systemradiators_upd() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    UPDATE net.radiator SET subtype_src_id = NEW.id,
        name = NEW.name,
        type = NEW.type,
        count = NEW.count,
        totalequivsurface = NEW.totalequivsurface,
        stateid = NEW.stateid
    WHERE id = OLD.lineid;
    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_systemradiators_upd ON net.v_systemradiators;
CREATE TRIGGER v_systemradiators_upd INSTEAD OF UPDATE ON net.v_systemradiators
    FOR EACH ROW EXECUTE FUNCTION net.v_systemradiators_upd();

CREATE OR REPLACE FUNCTION net.v_systemradiators_del() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    -- Удаление строки подтипа означает снятие типа, а не удаление объекта.
    PERFORM net.reclass_line(OLD.lineid, 'line_plain');
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_systemradiators_del ON net.v_systemradiators;
CREATE TRIGGER v_systemradiators_del INSTEAD OF DELETE ON net.v_systemradiators
    FOR EACH ROW EXECUTE FUNCTION net.v_systemradiators_del();
