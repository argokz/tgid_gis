-- Справочники переезжают из public в ref.
--
-- Список явный, а не вычисляемый по маске: миграция продуктовой БД
-- должна быть читаемой при разборе, а не зависеть от эвристики,
-- которая завтра классифицирует иначе.
--
-- Отбор: таблицы вида «id + наименование» из карты
-- tools/public_inventory.py, плюс справочники, которые движок читает
-- явно, но у которых колонок больше (deployedtempgraphs, lookup1..6,
-- external*).
--
-- ALTER TABLE ... SET SCHEMA не ломает ни внешние ключи, ни
-- представления: и те и другие держат таблицу по OID, а не по имени.
-- Обращения по имени без схемы продолжают работать через search_path
-- базы (public, net, ref, calc).
--
-- Откат: sql/125_ref_move_rollback.sql.

BEGIN;

CREATE SCHEMA IF NOT EXISTS ref;

DO $$
DECLARE
    t       text;
    n_move  int := 0;
    n_skip  int := 0;
BEGIN
    FOREACH t IN ARRAY ARRAY[
        'administrativnyy_rayon',
        'anticorrmaterials',
        'armaturetypes',
        'automdegs',
        'axialcompensatortypes',
        'ballvalvetypes',
        'baza_dannyh_po_adresnym_spravkam',
        'breakcomplexities',
        'breakconsumvols',
        'buildingconstrstates',
        'buildingconstructions',
        'buildingtypes',
        'calcferdiametersigns',
        'calcheatlosessigns',
        'capacity_types',
        'channelstates',
        'channeltype',
        'characteristictypes',
        'cities',
        'closesyscalcsigns',
        'compensatortypes',
        'connectionschemes',
        'connectiontypes',
        'constructionoverlaptypes',
        'constructiontypes',
        'consumerstates',
        'controlconditions',
        'controlorganizations',
        'controlservices',
        'controlstages',
        'coolanttypes',
        'corrosiondegreesin',
        'corrosiondegreesout',
        'corrosionprocessmarks',
        'da_net_informacii',
        'damperarmaturestates',
        'defectmeroprtype',
        'defectopistype',
        'defectstates',
        'defecttypes',
        'deployedtempgraphs',
        'designtypes',
        'dgupurpose',
        'directions',
        'docremonttypes',
        'documenttypes',
        'drenageperformances',
        'dreniruemyj_truboprovod',
        'drivetypes',
        'edvpurpos',
        'electroprotectdevices',
        'erwdifficulties',
        'externalmaterials',
        'externalsignline',
        'externalsigns',
        'floodingbeforetubes',
        'floodintensitiesbw',
        'floodintensitiesfw',
        'floodintensitiesgw',
        'floodintensitiestw',
        'gidroizolyacionnaya_konstrukciya',
        'graphtypes',
        'groundhumidities',
        'groundwatercauses',
        'grupurpose',
        'gvsparallschemes',
        'harakter_grunta_shurf',
        'heatcarriers',
        'heatpointtypes',
        'heatsourcetypes',
        'holes',
        'hydromodesigns',
        'indicatorstates',
        'infrastrdamages',
        'int_kor',
        'isolationtypes',
        'konstrukciya_drenazhnogo_ustrojstva',
        'konstruktsiya_perekrytiya_kanala',
        'listptstables',
        'locations',
        'locationtypes',
        'lookup1',
        'lookup2',
        'lookup3',
        'lookup4',
        'lookup5',
        'lookup6',
        'material_kamery_opuska_i_pavilonov_setey_teplosnabzh',
        'material_obektov_setey_teplosnabzheniya',
        'materialtypes',
        'materialy_i_mekhanizmy',
        'mestopovrezhdenija',
        'mikrorayony',
        'months',
        'nalichie_korrozii_shurf',
        'nalichie_vblizi_kommunikacij',
        'naznachenie_vskr',
        'naznachenie_zapornoy_armatury_setey_teplosnabzheniya',
        'netwateraggressivenesses',
        'obj_type_faktory_riska',
        'object_types',
        'objecttypes',
        'objecttypes2',
        'objekt_opressovochnogo_nasosa',
        'operators',
        'oporatypes',
        'opres_types',
        'osmotr_sostoyanie',
        'overlaptypes',
        'ownerorganizationtypes',
        'pdvalveinstalllocs',
        'pipelinesigns',
        'piperemonttypes',
        'pipesectionsstates',
        'populdamages',
        'potentialdifflevels',
        'poverhnost_nad_trassoj',
        'povrezhdennyielement',
        'pr_ots_lookup',
        'prichiny_narusheniya_organizacionnye',
        'prichinypovrezhdenia',
        'prtable',
        'purposetypes',
        'radialcompensatorforms',
        'regulatorstates',
        'remont_kamery',
        'remont_kanala',
        'remontcapitalchanneltypes',
        'remontcapitaltubetypes',
        'remontcat',
        'remontchanneltypes',
        'remontdocumenttypes',
        'remontinvestchanneltypes',
        'remontinvesttubetypes',
        'remontizolkonstruktsii',
        'remontkanalkameraspisok',
        'remonttruboprovodaspisok',
        'remonttubetypes',
        'remonttypes',
        'roadsurfacetypes',
        'rotordiametertypes',
        'schemeconnections',
        'sectwaterdumps',
        'setloadclosesyscalcsigns',
        'shema_gvs',
        'shema_nagr',
        'shurfstates',
        'shutdowntypes',
        'signnumworks',
        'sootv_lookup',
        'soputstvuiushchieprichiny',
        'sost_konstr',
        'sost_lookup',
        'sost_oborud',
        'sostkonstruktsiikamery',
        'sostkonstruktsiikanala',
        'sostnaruzhnoipoverkhnosti',
        'sostoyanie_metalla_truboprovoda',
        'sostoyanie_naruzhnogo_pokrytiya',
        'sostoyanie_opres',
        'sostoyanie_protivokorrozionnogo_pokrytiya_shurf',
        'sostoyanie_shurfa',
        'sostoyanie_stroitelnyh_konstrukcij_kanala',
        'sostoyanie_teplovoj_izolyacii',
        'sostteploizol',
        'sostvnutrenneipoverkhnosti',
        'spisokelementov',
        'sposob_ispolneniya_obektov_setey_teplosnabzheniya',
        'sredstvodiag',
        'statedefect',
        'stateindicator',
        'stateinds',
        'stateremont2',
        'statetu',
        'statustypes',
        'stopvalvetypes',
        'stroitelnye_konstrukcii_kanala',
        'subdivisions',
        'temperaturechartsigns',
        'throtstagesigns',
        'tip_armatury_setey_teplosnabzheniya',
        'tip_kanala_setey_teplosnabzheniya',
        'tip_kompensatora_setey_teplosnabzheniya',
        'tip_opory',
        'tip_opory_setey_teplosnabzheniya',
        'tip_soedineniya_truboprovoda_setey_teplosnabzheniya',
        'tip_vertikalnogo_povorota_setey_teplosnabzheniya',
        'tippoverhnosti',
        'tippovrezhdenia',
        'transfpurpos',
        'transftypes',
        'tubecharacteristics',
        'tubeisolations',
        'tubetypes',
        'tubingpedestrians',
        'tubingways',
        'typeedv',
        'typeofwork',
        'ustrojstva_vodootvedeniya',
        'valvepurpose',
        'vid_narusheniya',
        'vid_rabot',
        'vidy_dokumentov_osmotra',
        'vidy_dokumentov_shurfa',
        'vidy_elementov_shurf',
        'vnutrennee_sostoyanie_kanala',
        'wemeteringdevices',
        'workattributes',
        'yesorno',
        'yesorno12',
        'yesorno123',
        'zanos_kanala_gruntom'
    ]
    LOOP
        IF NOT EXISTS (
            SELECT 1 FROM pg_class c
            JOIN pg_namespace ns ON ns.oid = c.relnamespace
            WHERE ns.nspname = 'public' AND c.relname = t AND c.relkind = 'r')
        THEN
            n_skip := n_skip + 1;
            CONTINUE;   -- уже перенесена или не существует
        END IF;

        IF EXISTS (
            SELECT 1 FROM pg_class c
            JOIN pg_namespace ns ON ns.oid = c.relnamespace
            WHERE ns.nspname = 'ref' AND c.relname = t)
        THEN
            RAISE WARNING 'ref.% уже существует — public-копия оставлена, '
                          'нужен разбор', t;
            n_skip := n_skip + 1;
            CONTINUE;
        END IF;

        EXECUTE format('ALTER TABLE public.%I SET SCHEMA ref', t);
        n_move := n_move + 1;
    END LOOP;

    RAISE NOTICE 'перенесено справочников в ref: %, пропущено: %',
                 n_move, n_skip;
END $$;

COMMIT;

SELECT count(*) AS tablic_v_ref
FROM pg_class c JOIN pg_namespace ns ON ns.oid = c.relnamespace
WHERE ns.nspname = 'ref' AND c.relkind = 'r';
