-- compat.wdodevices: с проброса на чердак — на целевую таблицу net.
--
-- Целевая модель (docs/02-target-model.md) предписывала wdodevices
-- перенести дочерней таблицей net.node_metering_device; таблица создана
-- ещё в sql/010_net_schema.sql и содержит ровно 10 живых строк (src_id
-- 481–490, узел 14522). Остальные 3 375 строк attic.wdodevices мёртвые
-- (2 845 без узла, у 530 узел не существует) — разбор в docs/14, шаг 12.
--
-- compat.wdodevices до сих пор показывал все 3 385 строк чердака и был
-- невписываемым: b5.cpp (построение внутренней схемы, блок #if 1)
-- делает insertIntoDatabase("WDOdevices", {nodeID, externalSignID}),
-- что после ухода таблицы на чердак падало бы ошибкой "нельзя изменять
-- представление".
--
-- Здесь: представление над net.node_metering_device в старой форме
-- (id/nodeid/externalsignid/wdo/hydrores) + INSTEAD OF на запись.
-- id = COALESCE(src_id, id): старые строки отдают прежние 481–490
-- (экспорт фрагмента не заметит разницы), новые — реальный net id,
-- чтобы RETURNING id у insertIntoDatabase не был NULL.

BEGIN;

DROP VIEW IF EXISTS compat.wdodevices;
CREATE VIEW compat.wdodevices AS
SELECT
    COALESCE(m.src_id, m.id)::int AS id,
    m.node_id                     AS nodeid,
    m.externalsignid,
    m.wdo,
    m.hydrores
FROM net.node_metering_device m;

CREATE OR REPLACE FUNCTION compat.wdodevices_write() RETURNS trigger
LANGUAGE plpgsql
SET search_path = pg_catalog, net, compat
AS $fn$
BEGIN
    IF TG_OP = 'INSERT' THEN
        INSERT INTO net.node_metering_device
            (node_id, src_id, externalsignid, wdo, hydrores)
        VALUES
            (NEW.nodeid,
             NULLIF(NEW.id, 0),
             NEW.externalsignid, NEW.wdo, NEW.hydrores)
        RETURNING COALESCE(src_id, id)::int,
                  node_id, externalsignid, wdo, hydrores
        INTO NEW.id, NEW.nodeid, NEW.externalsignid, NEW.wdo, NEW.hydrores;
        RETURN NEW;
    ELSIF TG_OP = 'UPDATE' THEN
        UPDATE net.node_metering_device
        SET node_id        = NEW.nodeid,
            externalsignid = NEW.externalsignid,
            wdo            = NEW.wdo,
            hydrores       = NEW.hydrores
        WHERE COALESCE(src_id, id) = OLD.id;
        RETURN NEW;
    ELSE
        DELETE FROM net.node_metering_device
        WHERE COALESCE(src_id, id) = OLD.id;
        RETURN OLD;
    END IF;
END;
$fn$;

DROP TRIGGER IF EXISTS wdodevices_write ON compat.wdodevices;
CREATE TRIGGER wdodevices_write
INSTEAD OF INSERT OR UPDATE OR DELETE ON compat.wdodevices
FOR EACH ROW EXECUTE FUNCTION compat.wdodevices_write();

COMMIT;

SELECT count(*) AS strok_v_predstavlenii FROM compat.wdodevices;
