-- Рабочие таблицы дефектов, шурфов и ремонтов для старого gid8.
--
-- Эти таблицы были пустыми во время конвертации и поэтому попали в attic.
-- Пустота не означает, что таблица не нужна: gid8 создаёт в них новые акты,
-- дефекты и мероприятия. Данные не копируются, таблицы вместе с owned sequence
-- возвращаются в предметные схемы.

BEGIN;

DO $$
DECLARE
    table_name text;
    moved integer := 0;
BEGIN
    FOREACH table_name IN ARRAY ARRAY[
        'defectchannel',
        'defectkamera',
        'defectmeropr',
        'defectsforshurfy',
        'defecttube',
        'indikator_korrozii',
        'indikator_korrozii_po_godam',
        'list_opres_node1',
        'list_opres_node2',
        'nalichie_vblizi_kommunikacij_for_shurfy',
        'povrezhdennyielementfordefect',
        'prichiny_narusheniya_organizacionnye_for_defect',
        'prichinypovrezhdeniafordefect',
        'remont',
        'remontcapitalchannel',
        'remontcapitalkamera',
        'remontcapitaltube',
        'remontchannel',
        'remontinvestchannel',
        'remontinvestkamera',
        'remontinvesttube',
        'remontkamera',
        'remonttube',
        'shurf',
        'shurfy',
        'soputstvuiushchieprichinyfordefect',
        'sortlinesforuchastok',
        'sortnodesforuchastok',
        'sostkonstruktsiikameryfordefect',
        'sostkonstruktsiikanalafordefect',
        'vidy_elementov_for_shurfy',
        'zapornaya_armatura'
    ]
    LOOP
        IF to_regclass('attic.' || quote_ident(table_name)) IS NOT NULL
           AND to_regclass('ops.' || quote_ident(table_name)) IS NULL THEN
            EXECUTE format('ALTER TABLE attic.%I SET SCHEMA ops', table_name);
            moved := moved + 1;
        END IF;
    END LOOP;
    RAISE NOTICE 'возвращено таблиц эксплуатации в ops: %', moved;
END;
$$;

DO $$
DECLARE
    table_name text;
    moved integer := 0;
BEGIN
    FOREACH table_name IN ARRAY ARRAY[
        'vid_ispytani',
        'vneshny_vid'
    ]
    LOOP
        IF to_regclass('attic.' || quote_ident(table_name)) IS NOT NULL
           AND to_regclass('ref.' || quote_ident(table_name)) IS NULL THEN
            EXECUTE format('ALTER TABLE attic.%I SET SCHEMA ref', table_name);
            moved := moved + 1;
        END IF;
    END LOOP;
    RAISE NOTICE 'возвращено справочников эксплуатации в ref: %', moved;
END;
$$;

DO $$
DECLARE
    table_name text;
    moved integer := 0;
BEGIN
    FOREACH table_name IN ARRAY ARRAY[
        'responsibles',
        'responsibles_es',
        'responsibles_korrozia'
    ]
    LOOP
        IF to_regclass('attic.' || quote_ident(table_name)) IS NOT NULL
           AND to_regclass('org.' || quote_ident(table_name)) IS NULL THEN
            EXECUTE format('ALTER TABLE attic.%I SET SCHEMA org', table_name);
            moved := moved + 1;
        END IF;
    END LOOP;
    RAISE NOTICE 'возвращено таблиц ответственных в org: %', moved;
END;
$$;

COMMIT;
