-- Пустые неиспользуемые таблицы убираются на чердак.
--
-- Отбор: ноль строк И ни одного упоминания имени в коде движка sety,
-- в SQL репозитория и в исходниках Qt-клиента. Проверка «используется»
-- в tools/public_inventory.py стоит РАНЬШЕ проверки «пусто» именно
-- поэтому: таблица бывает пустой сейчас, но приложение в неё пишет.
-- В первой версии классификатора порядок был обратный, и 20 таких
-- таблиц попали в кандидаты на удаление.
--
-- Данных нет, терять нечего — но DROP необратим, а SET SCHEMA нет.
-- Схема attic не входит в search_path, поэтому забытое обращение
-- упадёт с внятной ошибкой, а не продолжит молча работать.
--
-- Список явный: миграцию продуктовой БД читают глазами.
--
-- Откат: sql/127_attic_rollback.sql.

BEGIN;

CREATE SCHEMA IF NOT EXISTS attic;

DO $$
DECLARE
    t       text;
    n_move  int := 0;
    n_skip  int := 0;
    n_rows  bigint;
BEGIN
    FOREACH t IN ARRAY ARRAY[
        'act',
        'administrativedistricts',
        'bridge_crossing',
        'brigades',
        'calculatedloads',
        'calculatedloadsfact',
        'calculation_iznos',
        'calculations',
        'capital2',
        'capital2_dokumenty',
        'capital2_meropriyatiya',
        'channelcoverconstructions',
        'channels',
        'channeltypes',
        'defect',
        'defectchannel',
        'defectdocuments',
        'defectkamera',
        'defectmeropr',
        'defectopis',
        'defectsforshurfy',
        'defecttube',
        'defekt',
        'defekt2',
        'defekt2_dokumenty',
        'defekt2_meropriyatiya',
        'deployedtempgraphsfact',
        'dgu',
        'dgudocuments',
        'districts',
        'docdiagosv',
        'docdiagvskr',
        'docdiagvyr',
        'dolzhnosti_es',
        'dolzhnosti_korrozia',
        'drenazhnyy_kran',
        'drenazhnyy_truboprovod',
        'duker',
        'edv',
        'edvdocuments',
        'electrodocuments',
        'electrodocumentsist',
        'electrodocumentspr',
        'faktory_riska_truboprovoda',
        'gilza_es',
        'gilza_esdocuments',
        'granitsy_vertikalnyh_kompensatorov',
        'gruzob',
        'gruzobdocuments',
        'heatlosesfact',
        'heatlosessourcefact',
        'heatlosessourcelist',
        'heatlosessourcemonthsfact',
        'heatpipesectionsharness',
        'heatpipesectionsharnessfact',
        'heatpoint',
        'heatsysobjmaterials',
        'indikator_korrozii',
        'indikator_korrozii_po_godam',
        'internalnodes',
        'ispravl',
        'istochnik_elektrosnabzheniya',
        'istochniki_teplosnabzheniya',
        'iznos',
        'kabelnyy_kanal_es',
        'kabelnyy_kanal_esdocuments',
        'kamera_opuska_ili_podema',
        'kanal',
        'kapremont_elementy',
        'kapremont_kolodtsy_opory_nadzemnyy',
        'kapremont_kolodtsy_opory_nadzemnyy2',
        'kapremont_kolodtsy_opory_podzemnyy',
        'kapremont_kolodtsy_opory_podzemnyy2',
        'kapremont_meropriyatiya',
        'kapremont_planiruemye_meropriyatiya_trubopr_izolyatsiya_oborudo',
        'kapremont_trubopr_izol',
        'kolodtsy',
        'kompensator',
        'kontrol_tehnicheskogo_sostoyaniya',
        'kontrol_tehnicheskogo_sostoyaniya_dokumenty',
        'liniya_elektroperedach',
        'link_elist_diz',
        'link_elist_grpod',
        'link_elist_trans',
        'list_opres_node1',
        'list_opres_node2',
        'losesbyfillingfact',
        'materialcharacteristics',
        'materialcharacteristicsfact',
        'mufta',
        'muftadocuments',
        'nagruzki_po_elevatoram',
        'nalichie_vblizi_kommunikacij_for_shurfy',
        'nasosnye_stantsii',
        'opora_es',
        'opora_esdocuments',
        'opora_skolzyaschaya',
        'opres',
        'opresacts',
        'opresdeployed',
        'opresdocuments',
        'opresmeropr',
        'opressovka_otvetstvennye',
        'opressovka_sektsioniruyuschaya_zadvizhka',
        'opressovka_uchastki_remonta',
        'osmotr',
        'osmotrdeployed',
        'osmotrdocuments',
        'owners',
        'pavilion',
        'perehlesty_setey',
        'peremychki',
        'pipesections',
        'plan_remont',
        'podpis',
        'povrezhdennyielementfordefect',
        'pribor',
        'prichiny_narusheniya_organizacionnye_for_defect',
        'prichinypovrezhdeniafordefect',
        'priemnik_elektrosnabzheniya',
        'realconsumerdocuments1',
        'realconsumerdocuments2',
        'realconsumerdocuments3',
        'regulatortypes',
        'remont',
        'remont2',
        'remont2deployed',
        'remontcapitalchannel',
        'remontcapitalkamera',
        'remontcapitaltube',
        'remontchannel',
        'remontdocuments',
        'remontinvestchannel',
        'remontinvestkamera',
        'remontinvesttube',
        'remontkamera',
        'remonttube',
        'responsibles',
        'responsibles_es',
        'responsibles_korrozia',
        'shield',
        'shurf',
        'shurf2',
        'shurf2_dokumenty',
        'shurf2_meropriyatiya',
        'shurfdocuments',
        'shurfy',
        'soputstvuiushchieprichinyfordefect',
        'sortlinesforuchastok',
        'sortnodesforuchastok',
        'sostkonstruktsiikameryfordefect',
        'sostkonstruktsiikanalafordefect',
        'spisok_kamer_drenazh',
        'standartequipment',
        'streets',
        'sysdiagrams',
        'tankbatteries',
        'temp_line',
        'temp_node',
        'tip_kamery',
        'tip_otklyucheniya_zapornoy_armatury',
        'tip_perekrytiya_kamery',
        'tkamera',
        'tkameradocuments',
        'towns',
        'transf',
        'transfdocuments',
        'tstrp',
        'tubes',
        'ulitsy_almaty',
        'uzel_kanala',
        'vertikalnyy_podem_opusk_truprovoda',
        'vid_ispytani',
        'vidy_elementov_for_shurfy',
        'visualmetriccontrol',
        'vladeltsy_es',
        'vneshny_vid',
        'vozdushnik',
        'zapornaya_armatura'
    ]
    LOOP
        IF NOT EXISTS (
            SELECT 1 FROM pg_class c
            JOIN pg_namespace ns ON ns.oid = c.relnamespace
            WHERE ns.nspname = 'public' AND c.relname = t AND c.relkind = 'r')
        THEN
            n_skip := n_skip + 1;
            CONTINUE;
        END IF;

        -- Ещё раз проверяем пустоту уже в транзакции: между построением
        -- списка и запуском скрипта в таблицу могли что-то записать.
        EXECUTE format('SELECT count(*) FROM public.%I', t) INTO n_rows;
        IF n_rows <> 0 THEN
            RAISE WARNING 'public.% уже не пуста (% строк) — оставлена', t, n_rows;
            n_skip := n_skip + 1;
            CONTINUE;
        END IF;

        EXECUTE format('ALTER TABLE public.%I SET SCHEMA attic', t);
        n_move := n_move + 1;
    END LOOP;

    RAISE NOTICE 'убрано пустых таблиц: %, пропущено: %', n_move, n_skip;
END $$;

COMMIT;

SELECT count(*) AS ostalos_v_public
FROM pg_class c JOIN pg_namespace ns ON ns.oid = c.relnamespace
WHERE ns.nspname = 'public' AND c.relkind = 'r';
