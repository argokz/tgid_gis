-- Паспорт участка ПТС для старого gid8 поверх единственного объекта net.pipe_section.
--
-- В исходной БД public.pipesections была пустой, а её поля частично дублировали
-- public.heatpipesections. Поэтому отдельную таблицу с копиями участков не создаём:
-- недостающие паспортные поля хранятся на каноническом объекте, а gid8 получает
-- прежний контракт через compat.pipesections.

BEGIN;

ALTER TABLE net.pipe_section
    ADD COLUMN IF NOT EXISTS diamuslov integer,
    ADD COLUMN IF NOT EXISTS pipelength double precision,
    ADD COLUMN IF NOT EXISTS standart text,
    ADD COLUMN IF NOT EXISTS nalichie_vblizi_kommunikacijid integer,
    ADD COLUMN IF NOT EXISTS harakter_gruntaid integer,
    ADD COLUMN IF NOT EXISTS poverhnost_nad_trassojid integer,
    ADD COLUMN IF NOT EXISTS podtoplenie_do_truby integer,
    ADD COLUMN IF NOT EXISTS zanos_kanala_gruntomid integer,
    ADD COLUMN IF NOT EXISTS stroitelnye_konstrukcii_kanalaid integer,
    ADD COLUMN IF NOT EXISTS sostoyanie_stroitelnyh_konstrukcij_kanalaid integer,
    ADD COLUMN IF NOT EXISTS vnutrennee_sostoyanie_kanalaid integer,
    ADD COLUMN IF NOT EXISTS konstrukciya_drenazhnogo_ustrojstvaid integer,
    ADD COLUMN IF NOT EXISTS sostoyanie_teplovoj_izolyacii_podachaid integer,
    ADD COLUMN IF NOT EXISTS sostoyanie_teplovoj_izolyacii_obratkaid integer,
    ADD COLUMN IF NOT EXISTS sostoyanie_naruzhnogo_pokrytiya_podachaid integer,
    ADD COLUMN IF NOT EXISTS sostoyanie_naruzhnogo_pokrytiya_obratkaid integer,
    ADD COLUMN IF NOT EXISTS sostoyanie_protivokorrozionnogo_pokrytiya_podachaid integer,
    ADD COLUMN IF NOT EXISTS sostoyanie_protivokorrozionnogo_pokrytiya_obratkaid integer,
    ADD COLUMN IF NOT EXISTS nalichie_korrozii_podachaid integer,
    ADD COLUMN IF NOT EXISTS nalichie_korrozii_obratkaid integer,
    ADD COLUMN IF NOT EXISTS zashemid integer,
    ADD COLUMN IF NOT EXISTS sost_podvid integer,
    ADD COLUMN IF NOT EXISTS sost_oporid integer,
    ADD COLUMN IF NOT EXISTS nalich_obrklid integer,
    ADD COLUMN IF NOT EXISTS sootv_nadpid integer,
    ADD COLUMN IF NOT EXISTS nalich_tabl_regid integer,
    ADD COLUMN IF NOT EXISTS nalich_tabl_parid integer,
    ADD COLUMN IF NOT EXISTS int_korid integer,
    ADD COLUMN IF NOT EXISTS sostoyanie_metalla_truboprovodaid integer,
    ADD COLUMN IF NOT EXISTS tol1 double precision,
    ADD COLUMN IF NOT EXISTS tol2 double precision,
    ADD COLUMN IF NOT EXISTS glubina_kor double precision,
    ADD COLUMN IF NOT EXISTS razmery_kor double precision,
    ADD COLUMN IF NOT EXISTS document_analiz_vlazhnost text,
    ADD COLUMN IF NOT EXISTS document_analiz_korrozia text,
    ADD COLUMN IF NOT EXISTS document_potenzial text,
    ADD COLUMN IF NOT EXISTS document_analiz_vytyazhka text,
    ADD COLUMN IF NOT EXISTS dokument_analiz_prob_na_vlazhnost text,
    ADD COLUMN IF NOT EXISTS dokument_analiz_prob_na_korrozijnuyu_aktivnost text,
    ADD COLUMN IF NOT EXISTS dokument_raznost_elektricheskih_potencialov text,
    ADD COLUMN IF NOT EXISTS dokument_analiz_vodyanoj_vytyazhki text,
    ADD COLUMN IF NOT EXISTS dokument_chertezh_objekta_kontrolya text,
    ADD COLUMN IF NOT EXISTS len_tube double precision,
    ADD COLUMN IF NOT EXISTS len_izol double precision,
    ADD COLUMN IF NOT EXISTS asfaltirovanie double precision,
    ADD COLUMN IF NOT EXISTS len_channel double precision,
    ADD COLUMN IF NOT EXISTS zamena_kanala_procent double precision,
    ADD COLUMN IF NOT EXISTS zamena_kompensatorov integer,
    ADD COLUMN IF NOT EXISTS rekonstrukciya_kamery_nachalnogo_uzla integer,
    ADD COLUMN IF NOT EXISTS rekonstrukciya_kamery_konechnogo_uzla integer,
    ADD COLUMN IF NOT EXISTS ustanovka_i_zamena_zadvizhek integer,
    ADD COLUMN IF NOT EXISTS id_old integer DEFAULT 0;

-- У старого pipeSections был собственный id. Так как таблица была пустой,
-- устойчивым совместимым идентификатором становится id того же сетевого объекта.
-- Техническое заполнение не является пользовательской правкой 84 тысяч объектов,
-- поэтому не раздуваем meta.object_change_log служебными событиями миграции.
ALTER TABLE net.pipe_section DISABLE TRIGGER object_touch;

UPDATE net.pipe_section
   SET pipesectionid = id::integer
 WHERE pipesectionid IS NULL;

-- Начальные значения двух старых полей берём из уже перенесённой гидравлики.
UPDATE net.pipe_section
   SET diamuslov = diametercondit::integer
 WHERE diamuslov IS NULL
   AND diametercondit IS NOT NULL;

UPDATE net.pipe_section
   SET pipelength = pipesectlength
 WHERE pipelength IS NULL
   AND pipesectlength IS NOT NULL;

ALTER TABLE net.pipe_section ENABLE TRIGGER object_touch;

CREATE OR REPLACE FUNCTION net.pipe_section_legacy_id_sync()
RETURNS trigger
LANGUAGE plpgsql
AS $$
BEGIN
    IF NEW.pipesectionid IS NULL THEN
        IF NEW.id > 2147483647 THEN
            RAISE EXCEPTION 'pipe_section id % не помещается в старый integer pipeSectionID', NEW.id;
        END IF;
        NEW.pipesectionid := NEW.id::integer;
    END IF;
    RETURN NEW;
END;
$$;

DROP TRIGGER IF EXISTS pipe_section_legacy_id_sync ON net.pipe_section;
CREATE TRIGGER pipe_section_legacy_id_sync
BEFORE INSERT ON net.pipe_section
FOR EACH ROW EXECUTE FUNCTION net.pipe_section_legacy_id_sync();

CREATE OR REPLACE VIEW compat.pipesections AS
SELECT
    id,
    name,
    node_from_src AS nodeid1,
    node_to_src AS nodeid2,
    magistral,
    magistralsite,
    distsite,
    tubingtypeid,
    firstpicdatehp,
    lasttransdate,
    lastisoldate,
    diamuslov,
    pipelength,
    standart,
    inventnumber,
    tubetypeid,
    objecttypeid,
    sreda,
    press,
    temperature,
    spoksluzhbirash,
    ressurs,
    pusk,
    organizationid,
    elektrich,
    transportelekricht,
    ponezial,
    floodintensitygwid,
    floodintensityfwid,
    floodintensitytwid,
    floodintensitybwid,
    vodootved,
    slozhokluzh,
    otkluzhgkal,
    avariivipoln,
    pesehod,
    zhddorogi,
    ludiproklad,
    uzherbludi,
    uzherbsity,
    vnesniivid,
    sostoborudovania,
    vnesnkorrozia,
    vnunrenkorrozia,
    sostkonstrukz,
    kategorii,
    nalichie_vblizi_kommunikacijid,
    harakter_gruntaid,
    poverhnost_nad_trassojid,
    podtoplenie_do_truby,
    zanos_kanala_gruntomid,
    stroitelnye_konstrukcii_kanalaid,
    sostoyanie_stroitelnyh_konstrukcij_kanalaid,
    vnutrennee_sostoyanie_kanalaid,
    konstrukciya_drenazhnogo_ustrojstvaid,
    sostoyanie_teplovoj_izolyacii_podachaid,
    sostoyanie_teplovoj_izolyacii_obratkaid,
    sostoyanie_naruzhnogo_pokrytiya_podachaid,
    sostoyanie_naruzhnogo_pokrytiya_obratkaid,
    sostoyanie_protivokorrozionnogo_pokrytiya_podachaid,
    sostoyanie_protivokorrozionnogo_pokrytiya_obratkaid,
    nalichie_korrozii_podachaid,
    nalichie_korrozii_obratkaid,
    zashemid,
    sost_podvid,
    sost_oporid,
    nalich_obrklid,
    sootv_nadpid,
    nalich_tabl_regid,
    nalich_tabl_parid,
    int_korid,
    sostoyanie_metalla_truboprovodaid,
    tol1,
    tol2,
    glubina_kor,
    razmery_kor,
    document_analiz_vlazhnost,
    document_analiz_korrozia,
    document_potenzial,
    document_analiz_vytyazhka,
    dokument_analiz_prob_na_vlazhnost,
    dokument_analiz_prob_na_korrozijnuyu_aktivnost,
    dokument_raznost_elektricheskih_potencialov,
    dokument_analiz_vodyanoj_vytyazhki,
    dokument_chertezh_objekta_kontrolya,
    diametercondit,
    diameterexternal,
    diameterinternal,
    wallthickness,
    pipesectlength,
    isolmaterialid,
    isolationtypeid,
    isolthickness,
    externmaterialid,
    externcoverthick,
    anticorrmaterialid,
    len_tube,
    len_izol,
    asfaltirovanie,
    len_channel,
    zamena_kanala_procent,
    zamena_kompensatorov,
    rekonstrukciya_kamery_nachalnogo_uzla,
    rekonstrukciya_kamery_konechnogo_uzla,
    ustanovka_i_zamena_zadvizhek,
    id_old
FROM net.pipe_section;

COMMENT ON VIEW compat.pipesections IS
    'Старый паспорт участка gid8 без дублирования объекта: 1:1 с net.pipe_section.';

-- В старой модели DELETE удалял отдельную строку паспорта, но не саму линию.
-- Теперь это один объект, поэтому автоматическое удаление через простое view
-- было бы опасным: оно физически удалило бы net.pipe_section. Перехватываем его.
CREATE OR REPLACE FUNCTION compat.pipesections_safe_delete()
RETURNS trigger
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN OLD;
END;
$$;

DROP TRIGGER IF EXISTS pipesections_safe_delete ON compat.pipesections;
CREATE TRIGGER pipesections_safe_delete
INSTEAD OF DELETE ON compat.pipesections
FOR EACH ROW EXECUTE FUNCTION compat.pipesections_safe_delete();

COMMIT;
