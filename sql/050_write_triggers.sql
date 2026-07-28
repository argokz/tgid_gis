-- СГЕНЕРИРОВАНО tools/gen_write_triggers.py
--
-- INSTEAD OF триггеры делают слой совместимости доступным для записи.
-- Приложение продолжает выполнять привычные UPDATE nodes SET x=..., y=...
-- и UPDATE nodes SET removed=1, а строки уходят в объектные таблицы net.
--
-- Геометрия пересчитывается из x/y при каждой записи, поэтому она остаётся
-- единственным источником истины: рассинхронизации, как между прежними
-- coords и shape, возникнуть не может.

CREATE OR REPLACE VIEW net.v_nodes_geom AS
SELECT id, geom FROM net.connect_node UNION ALL SELECT id, geom FROM net.consumer_general UNION ALL SELECT id, geom FROM net.heat_chamber UNION ALL SELECT id, geom FROM net.heat_source UNION ALL SELECT id, geom FROM net.pump_station UNION ALL SELECT id, geom FROM net.consumer_real UNION ALL SELECT id, geom FROM net.refill_node UNION ALL SELECT id, geom FROM net.valve_3way UNION ALL SELECT id, geom FROM net.node_plain;

CREATE OR REPLACE VIEW net.v_linesobj_topo AS
SELECT id, node_from, node_to FROM net.air_heater UNION ALL SELECT id, node_from, node_to FROM net.damper UNION ALL SELECT id, node_from, node_to FROM net.diaphragm UNION ALL SELECT id, node_from, node_to FROM net.elevator UNION ALL SELECT id, node_from, node_to FROM net.heat_exchanger UNION ALL SELECT id, node_from, node_to FROM net.pipe_section UNION ALL SELECT id, node_from, node_to FROM net.local_resistance UNION ALL SELECT id, node_from, node_to FROM net.regulator_press UNION ALL SELECT id, node_from, node_to FROM net.pump UNION ALL SELECT id, node_from, node_to FROM net.radiator UNION ALL SELECT id, node_from, node_to FROM net.line_plain;

-- Геометрия линии по её концам и промежуточным точкам.
CREATE OR REPLACE FUNCTION net.build_line_geom(n_from bigint, n_to bigint,
                                               coords text)
RETURNS geometry
LANGUAGE sql STABLE SET search_path = pg_catalog, public, net AS $$
    SELECT ST_SetSRID(ST_MakeLine(
        ARRAY[(SELECT geom FROM net.v_nodes_geom WHERE id = n_from)]
        || net.parse_coords(coords, 100.0)
        || ARRAY[(SELECT geom FROM net.v_nodes_geom WHERE id = n_to)]), 9998);
$$;

-- Пересчёт геометрии существующей линии при смене промежуточных точек.
CREATE OR REPLACE FUNCTION net.rebuild_line_geom(line_id bigint, coords text)
RETURNS geometry
LANGUAGE sql STABLE SET search_path = pg_catalog, public, net AS $$
    SELECT net.build_line_geom(l.node_from, l.node_to, coords)
    FROM (SELECT node_from, node_to FROM net.v_linesobj_topo
          WHERE id = line_id) l;
$$;

CREATE OR REPLACE FUNCTION net.v_nodes_update() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
DECLARE
    t text;
    g geometry;
BEGIN
    SELECT tbl INTO t FROM net.node_reg WHERE id = OLD.id;
    IF t IS NULL THEN
        RAISE EXCEPTION 'объект % не найден в реестре node_reg', OLD.id;
    END IF;

    -- Геометрию считаем ЗДЕСЬ и передаём параметром: внутри динамического
    -- запроса запись NEW недоступна, там видны только $1, $2, ...
    g := ST_SetSRID(ST_Point(NEW.x / 100.0, -NEW.y / 100.0), 9998);

    EXECUTE format(
        'UPDATE net.%I SET
                geom = $3,
                removed_at = $2,
                internalnodeid = $4,
                externalcodeid = $5,
                externalnodename = $6,
                externalsignid = $7,
                addressid = $8,
                geomarktoptube = $9,
                geomarknodearea = $10,
                calcpressflow = $11,
                calcpressret = $12,
                picdate = $13,
                lastrepairdate = $14,
                displaysign = $15,
                archivechangedate = $16,
                operatorid = $17,
                nodename = $18,
                scheme = $19,
                memo = $20,
                gpscoords = $21,
                belonghn = $22,
                passport = $23,
                inventnumber = $24,
                pipelinesign = $25,
                nodetypeid = $26,
                isbusy = $27,
                isloaded = $28,
                belongmagistralsite = $29,
                belongdistsite = $30,
                organizationid = $31,
                magistralsite = $32,
                distsite = $33,
                registnumber = $34
         WHERE id = $1', t)
    USING OLD.id,
          CASE WHEN NEW.removed <> 0 THEN now() ELSE NULL END,
          g,
          NEW.internalnodeid,
            NEW.externalcodeid,
            NEW.externalnodename,
            NEW.externalsignid,
            NEW.addressid,
            NEW.geomarktoptube,
            NEW.geomarknodearea,
            NEW.calcpressflow,
            NEW.calcpressret,
            NEW.picdate,
            NEW.lastrepairdate,
            NEW.displaysign,
            NEW.archivechangedate,
            NEW.operatorid,
            NEW.nodename,
            NEW.scheme,
            NEW.memo,
            NEW.gpscoords,
            NEW.belonghn,
            NEW.passport,
            NEW.inventnumber,
            NEW.pipelinesign,
            NEW.nodetypeid,
            NEW.isbusy,
            NEW.isloaded,
            NEW.belongmagistralsite,
            NEW.belongdistsite,
            NEW.organizationid,
            NEW.magistralsite,
            NEW.distsite,
            NEW.registnumber;

    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_nodes_upd ON net.v_nodes;
CREATE TRIGGER v_nodes_upd INSTEAD OF UPDATE ON net.v_nodes
    FOR EACH ROW EXECUTE FUNCTION net.v_nodes_update();


CREATE OR REPLACE FUNCTION net.v_nodes_delete() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
DECLARE
    t text;
BEGIN
    SELECT tbl INTO t FROM net.node_reg WHERE id = OLD.id;
    IF t IS NULL THEN
        RETURN OLD;
    END IF;
    EXECUTE format('DELETE FROM net.%I WHERE id = $1', t) USING OLD.id;
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_nodes_del ON net.v_nodes;
CREATE TRIGGER v_nodes_del INSTEAD OF DELETE ON net.v_nodes
    FOR EACH ROW EXECUTE FUNCTION net.v_nodes_delete();


CREATE OR REPLACE FUNCTION net.v_nodes_insert() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    -- Тип объекта в момент вставки ещё не известен: прежняя модель
    -- создавала строку в базовой таблице, а тип задавала следующим
    -- запросом в таблицу-подтип. Поэтому объект попадает в *_plain,
    -- а смену класса выполняет net.reclass_node / net.reclass_line.
    IF NEW.id IS NULL THEN
        NEW.id := nextval('net.obj_id_seq');
    END IF;

    INSERT INTO net.node_plain (id, fragment_id, geom, removed_at, internalnodeid, externalcodeid, externalnodename, externalsignid, addressid, geomarktoptube, geomarknodearea, calcpressflow, calcpressret, picdate, lastrepairdate, displaysign, archivechangedate, operatorid, nodename, scheme, memo, gpscoords, belonghn, passport, inventnumber, pipelinesign, nodetypeid, isbusy, isloaded, belongmagistralsite, belongdistsite, organizationid, magistralsite, distsite, registnumber)
    VALUES (NEW.id, NEW.fileid, ST_SetSRID(ST_Point(NEW.x / 100.0, -NEW.y / 100.0), 9998),
            CASE WHEN NEW.removed <> 0 THEN now() ELSE NULL END,
            NEW.internalnodeid, NEW.externalcodeid, NEW.externalnodename, NEW.externalsignid, NEW.addressid, NEW.geomarktoptube, NEW.geomarknodearea, NEW.calcpressflow, NEW.calcpressret, NEW.picdate, NEW.lastrepairdate, NEW.displaysign, NEW.archivechangedate, NEW.operatorid, NEW.nodename, NEW.scheme, NEW.memo, NEW.gpscoords, NEW.belonghn, NEW.passport, NEW.inventnumber, NEW.pipelinesign, NEW.nodetypeid, NEW.isbusy, NEW.isloaded, NEW.belongmagistralsite, NEW.belongdistsite, NEW.organizationid, NEW.magistralsite, NEW.distsite, NEW.registnumber);

    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_nodes_ins ON net.v_nodes;
CREATE TRIGGER v_nodes_ins INSTEAD OF INSERT ON net.v_nodes
    FOR EACH ROW EXECUTE FUNCTION net.v_nodes_insert();

CREATE OR REPLACE FUNCTION net.v_linesobj_update() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
DECLARE
    t text;
    g geometry;
BEGIN
    SELECT tbl INTO t FROM net.line_reg WHERE id = OLD.id;
    IF t IS NULL THEN
        RAISE EXCEPTION 'объект % не найден в реестре line_reg', OLD.id;
    END IF;

    -- Геометрию считаем ЗДЕСЬ и передаём параметром: внутри динамического
    -- запроса запись NEW недоступна, там видны только $1, $2, ...
    g := net.rebuild_line_geom(OLD.id, NEW.coords);

    EXECUTE format(
        'UPDATE net.%I SET
                geom = $3,
                removed_at = $2,
                externalsignlineid = $4,
                location = $5,
                hydrores = $6,
                organizationid = $7,
                registnum = $8,
                firstpicdate = $9,
                lastmaintdate = $10,
                displaysign = $11,
                archivechangedate = $12,
                operatorid = $13,
                typ = $14,
                internalnodeid = $15
         WHERE id = $1', t)
    USING OLD.id,
          CASE WHEN NEW.removed <> 0 THEN now() ELSE NULL END,
          g,
          NEW.externalsignlineid,
            NEW.location,
            NEW.hydrores,
            NEW.organizationid,
            NEW.registnum,
            NEW.firstpicdate,
            NEW.lastmaintdate,
            NEW.displaysign,
            NEW.archivechangedate,
            NEW.operatorid,
            NEW.typ,
            NEW.internalnodeid;

    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_linesobj_upd ON net.v_linesobj;
CREATE TRIGGER v_linesobj_upd INSTEAD OF UPDATE ON net.v_linesobj
    FOR EACH ROW EXECUTE FUNCTION net.v_linesobj_update();


CREATE OR REPLACE FUNCTION net.v_linesobj_delete() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
DECLARE
    t text;
BEGIN
    SELECT tbl INTO t FROM net.line_reg WHERE id = OLD.id;
    IF t IS NULL THEN
        RETURN OLD;
    END IF;
    EXECUTE format('DELETE FROM net.%I WHERE id = $1', t) USING OLD.id;
    RETURN OLD;
END $fn$;

DROP TRIGGER IF EXISTS v_linesobj_del ON net.v_linesobj;
CREATE TRIGGER v_linesobj_del INSTEAD OF DELETE ON net.v_linesobj
    FOR EACH ROW EXECUTE FUNCTION net.v_linesobj_delete();


CREATE OR REPLACE FUNCTION net.v_linesobj_insert() RETURNS trigger
LANGUAGE plpgsql SET search_path = pg_catalog, public, net AS $fn$
BEGIN
    -- Тип объекта в момент вставки ещё не известен: прежняя модель
    -- создавала строку в базовой таблице, а тип задавала следующим
    -- запросом в таблицу-подтип. Поэтому объект попадает в *_plain,
    -- а смену класса выполняет net.reclass_node / net.reclass_line.
    IF NEW.id IS NULL THEN
        NEW.id := nextval('net.obj_id_seq');
    END IF;

    INSERT INTO net.line_plain (id, fragment_id, geom, removed_at, node_from, node_to, node_from_src, node_to_src, fileid_src, externalsignlineid, location, hydrores, organizationid, registnum, firstpicdate, lastmaintdate, displaysign, archivechangedate, operatorid, typ, internalnodeid)
    VALUES (NEW.id, (SELECT fragment_id FROM net.node_plain WHERE id = NEW.nodeid1 UNION ALL SELECT fragment_id FROM net.consumer_real WHERE id = NEW.nodeid1 LIMIT 1), net.build_line_geom(NEW.nodeid1, NEW.nodeid2, NEW.coords),
            CASE WHEN NEW.removed <> 0 THEN now() ELSE NULL END, NEW.nodeid1, NEW.nodeid2, NEW.nodeid1, NEW.nodeid2, NEW.fileid,
            NEW.externalsignlineid, NEW.location, NEW.hydrores, NEW.organizationid, NEW.registnum, NEW.firstpicdate, NEW.lastmaintdate, NEW.displaysign, NEW.archivechangedate, NEW.operatorid, NEW.typ, NEW.internalnodeid);

    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS v_linesobj_ins ON net.v_linesobj;
CREATE TRIGGER v_linesobj_ins INSTEAD OF INSERT ON net.v_linesobj
    FOR EACH ROW EXECUTE FUNCTION net.v_linesobj_insert();
