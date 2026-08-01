-- Подсистемы переезжают из public в свои схемы.
--
-- СГЕНЕРИРОВАНО tools/gen_subsystems.py. Списки явные: миграцию
-- продуктовой БД читают глазами, и она не должна зависеть от
-- того, как сегодня написана регулярка в генераторе.
--
-- Правило отбора (docs/15): перепроектировать заново — только то,
-- что рисует карта или читает расчёт. Эти подсистемы не рисуются
-- и не считаются, поэтому переносятся как есть, без изменения
-- структуры.
--
-- search_path расширяется новыми схемами: обращения по имени без
-- схемы обязаны продолжать работать, в том числе из старого
-- приложения. Без этого перенос был бы не реорганизацией, а
-- поломкой.
--
-- Откат: sql/141_subsystems_rollback.sql.

BEGIN;

CREATE SCHEMA IF NOT EXISTS addr;
CREATE SCHEMA IF NOT EXISTS doc;
CREATE SCHEMA IF NOT EXISTS el;
CREATE SCHEMA IF NOT EXISTS ops;
CREATE SCHEMA IF NOT EXISTS org;

-- addr: 20 таблиц
DO $$
DECLARE t text; n int := 0;
BEGIN
    FOREACH t IN ARRAY ARRAY[
        'addresses',
        'buildingentries',
        'funktsionalnoe_naznachenie_zdaniy_i_sooruzheniy',
        'geodz',
        'obekty',
        'spisok_ulits',
        'spisok_ulits_staryy',
        'spisok_zhilyh_massivov',
        'sposob_vvoda_v_zdanie',
        'tablitsa1',
        'tip_kladbischa',
        'tip_massiva',
        'ulitsy',
        'vvod_v_zdanie',
        'vvody_v_zdanie',
        'zdaniya_2',
        'zdaniya_potrebiteley',
        'zdaniya_tu',
        'zhile',
        'zhile1'
    ]
    LOOP
        IF EXISTS (SELECT 1 FROM pg_class c JOIN pg_namespace ns ON ns.oid = c.relnamespace
                   WHERE ns.nspname = 'public' AND c.relname = t AND c.relkind = 'r') THEN
            EXECUTE format('ALTER TABLE public.%I SET SCHEMA addr', t);
            n := n + 1;
        END IF;
    END LOOP;
    RAISE NOTICE 'в addr перенесено: %', n;
END $$;

-- doc: 7 таблиц
DO $$
DECLARE t text; n int := 0;
BEGIN
    FOREACH t IN ARRAY ARRAY[
        'audit_group_comments',
        'help_old',
        'oshibki_vstavki',
        'passports',
        'passportsdocs',
        'tehnicheskie_usloviya',
        'texts'
    ]
    LOOP
        IF EXISTS (SELECT 1 FROM pg_class c JOIN pg_namespace ns ON ns.oid = c.relnamespace
                   WHERE ns.nspname = 'public' AND c.relname = t AND c.relkind = 'r') THEN
            EXECUTE format('ALTER TABLE public.%I SET SCHEMA doc', t);
            n := n + 1;
        END IF;
    END LOOP;
    RAISE NOTICE 'в doc перенесено: %', n;
END $$;

-- el: 6 таблиц
DO $$
DECLARE t text; n int := 0;
BEGIN
    FOREACH t IN ARRAY ARRAY[
        'energosistemy',
        'marki_kabeley_es',
        'marki_transformatorov_es',
        'tipy_istochnikov_elektricheskih_setey',
        'tipy_lep',
        'tipy_priemnikov_elektricheskih_setey'
    ]
    LOOP
        IF EXISTS (SELECT 1 FROM pg_class c JOIN pg_namespace ns ON ns.oid = c.relnamespace
                   WHERE ns.nspname = 'public' AND c.relname = t AND c.relkind = 'r') THEN
            EXECUTE format('ALTER TABLE public.%I SET SCHEMA el', t);
            n := n + 1;
        END IF;
    END LOOP;
    RAISE NOTICE 'в el перенесено: %', n;
END $$;

-- ops: 75 таблиц
DO $$
DECLARE t text; n int := 0;
BEGIN
    FOREACH t IN ARRAY ARRAY[
        'agressivnost_setevoy_vody',
        'defekt_elementy',
        'defekt_truby',
        'diag',
        'faktiznos_1_vneshnii_vid',
        'faktiznos_2_kat_narush',
        'faktiznos_3_sost_obor',
        'faktriska_10_stroykonstr',
        'faktriska_11_vodootvedenie',
        'faktriska_12_slozhnost_otkl',
        'faktriska_13_otkl_potrebiteli',
        'faktriska_14_avar_vosst_raboty',
        'faktriska_15_peshehod',
        'faktriska_16_avto',
        'faktriska_17_skoplenie_zhit',
        'faktriska_18_uscherb_nas',
        'faktriska_19_uscherb_infrastr',
        'faktriska_4_elekt',
        'faktriska_5_transport',
        'faktriska_6_potencial',
        'faktriska_7_vneshkorroz',
        'faktriska_8_vnutkorroz',
        'faktriska_9_podtoplenie',
        'harakter_grunta',
        'izolyatsiya_truby',
        'kapitalnyy_remont',
        'kapremont_planiruemye_meropriyatiya_kolodtsy_opory',
        'kapremont_truboprovod_oborudovanie_izolyatsiya',
        'kapremont_uchastki_remonta',
        'kapremont_uchastok_teploprovoda_ishodnyy',
        'kapremont_uchastok_teploprovoda_posle_remonta',
        'konstruktsiya_drenazhnogo_ustroystva',
        'konstruktsiya_pokrovnogo_sloya_izolyatsii',
        'material_pokrovnogo_sloya',
        'material_teploizolyatsionnyy',
        'namechennye_meropriyatiya_po_elementam',
        'namechennye_meropriyatiya_po_izolyatsionnoy_konstruktsii',
        'namechennye_meropriyatiya_po_kanalu',
        'namechennye_meropriyatiya_po_teploprovodu',
        'ochered_opressovok',
        'opressovka_defekt',
        'opressovka_meropriyatiya',
        'opressovka_meropriyatiya_po_ustraneniyu_povrezhdeniy',
        'opressovka_povrezhdeniya',
        'opressovki_uchastok_ocheredi',
        'otsenka_korroziynogo_protsessa',
        'pasport_uchastka_ms',
        'pasport_uchastka_rs',
        'perspektivnye_truby',
        'plity_perekrytiya_teplovyh_kamer',
        'prichiny_korrozii',
        'prichiny_razryva_stenki_truby',
        'prichiny_razryva_svarnogo_shva',
        'prichiny_svischa',
        'remont_izolyatsionnoy_konstruktsii',
        'remont_po_vosstanovleniyu_kanala',
        'shurf_elementy',
        'shurf_korroziynaya_aktivnost',
        'shurf_potentsialy_truba_zemlya',
        'shurf_truby',
        'shurf_vlazhnost_grunta',
        'shurf_vodnaya_vytyazhka',
        'sostoyanie_izolyatsionnogo_materiala',
        'sostoyanie_konstruktsii_kanala',
        'sostoyanie_pokrovnogo_sloya_izolyatsii',
        'sostoyanie_protivokorrozionnogo_pokrytiya',
        'sostoyanie_teploizolyatsionnoy_konstruktsii',
        'tip_izolyatsii',
        'tolschinometr',
        'uchastok_ms',
        'uchastok_rs',
        'vid_korrozii',
        'vidy_elementov',
        'vidy_trub',
        'vosstanovitelnye_raboty_shurf'
    ]
    LOOP
        IF EXISTS (SELECT 1 FROM pg_class c JOIN pg_namespace ns ON ns.oid = c.relnamespace
                   WHERE ns.nspname = 'public' AND c.relname = t AND c.relkind = 'r') THEN
            EXECUTE format('ALTER TABLE public.%I SET SCHEMA ops', t);
            n := n + 1;
        END IF;
    END LOOP;
    RAISE NOTICE 'в ops перенесено: %', n;
END $$;

-- org: 18 таблиц
DO $$
DECLARE t text; n int := 0;
BEGIN
    FOREACH t IN ARRAY ARRAY[
        'dolzhnosti',
        'factorymanufacturers',
        'familii_masterov_ao__astana_teplotranzit',
        'nachalniki_uchastkov',
        'organizatsii',
        'organizatsii_obsluzhivayuschie',
        'organizatsii_podryadchiki',
        'organizatsii_proektnye',
        'organizatsii_vladeltsy',
        'password',
        'passwords',
        'prinadlezhnost',
        'proektnaya_organizatsiya',
        'rabotniki',
        'rayon_ekspluatatsii',
        'serviceorganizations',
        'uchastki_ekspluatatsii',
        'zavod_izgotovitel'
    ]
    LOOP
        IF EXISTS (SELECT 1 FROM pg_class c JOIN pg_namespace ns ON ns.oid = c.relnamespace
                   WHERE ns.nspname = 'public' AND c.relname = t AND c.relkind = 'r') THEN
            EXECUTE format('ALTER TABLE public.%I SET SCHEMA org', t);
            n := n + 1;
        END IF;
    END LOOP;
    RAISE NOTICE 'в org перенесено: %', n;
END $$;

-- Порядок поиска — свойство БД: его видят все клиенты.
ALTER DATABASE tgid_gis SET search_path = public, net, ref, calc, addr, doc, el, ops, org;

COMMIT;

SELECT count(*) AS ostalos_v_public FROM pg_class c
JOIN pg_namespace n ON n.oid = c.relnamespace
WHERE n.nspname = 'public' AND c.relkind = 'r';
