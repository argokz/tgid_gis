-- Параметрические справочники расчёта переезжают в ref,
-- пустые объектные таблицы — на чердак.
--
-- Отбор: всё, что движок читает как параметр (нормы тепловых потерь,
-- температурные графики, коэффициенты, магистрали, нагрузки, источники),
-- относится к справочным данным, а не к объектам сети. Место таких
-- таблиц — ref; обращения по имени без схемы продолжают работать через
-- search_path базы.
--
-- Пустые объектные таблицы (overgroundnodes, pavilions, undergroundnodes,
-- uninstallednodes) в запросах движка не встречаются: имена нашлись
-- только в каталоге колонок w_data.py, который описывает состав полей,
-- а не выполняет запросы. Они уходят на чердак, где обращение к ним
-- упадёт заметно, если такое место всё же найдётся.
--
-- НЕ трогаются, требуют решения:
--   spatial_ref_sys              — служебная таблица PostGIS;
--   setpressnodes                — есть net.node_press_setting, но связь
--                                  subtype_src_id не заполнена;
--   klapany__ventili__zapornye   — 17 731 объект без класса в net;
--   magistrali / magistrals      — пара по 14 строк, похоже на дубль;
--   tab, table, vyd              — имена без смысла.
--
-- Откат: sql/125_ref_move_rollback.sql (для ref),
--        sql/127_attic_rollback.sql (для чердака).

BEGIN;

CREATE SCHEMA IF NOT EXISTS ref;
CREATE SCHEMA IF NOT EXISTS attic;

DO $$
DECLARE
    t      text;
    n_ref  int := 0;
    n_att  int := 0;
BEGIN
    FOREACH t IN ARRAY ARRAY[
        'airgroundmonthtemperatures',
        'calcschemes',
        'corrosionindicators',
        'deployeddirections',
        'gvsloadgraphs',
        'heating_seasons',
        'heatloses',
        'heatlosesmain',
        'heatlosesmainfact',
        'heatlosessource',
        'heatlosessourcemonths',
        'hsourceworkmonthgraphs',
        'istochniki_tepla',
        'kotelnye',
        'localhydroresistances',
        'losesbyfilling',
        'magistrals',
        'nagruzki',
        'normtemperaturesgraphs',
        'prisoedinennaya_nagruzka_istochnikov',
        'prisoedinennaya_nagruzka_istochnikov3',
        'qualityregtempgraphs',
        'raspredseti',
        'teplo',
        'tip_truboprovoda',
        'trps'
    ]
    LOOP
        IF to_regclass('public.' || quote_ident(t)) IS NOT NULL
           AND to_regclass('ref.' || quote_ident(t)) IS NULL THEN
            EXECUTE format('ALTER TABLE public.%I SET SCHEMA ref', t);
            n_ref := n_ref + 1;
        END IF;
    END LOOP;

    FOREACH t IN ARRAY ARRAY[
        'overgroundnodes',
        'pavilions',
        'undergroundnodes',
        'uninstallednodes'
    ]
    LOOP
        IF to_regclass('public.' || quote_ident(t)) IS NOT NULL
           AND to_regclass('attic.' || quote_ident(t)) IS NULL THEN
            EXECUTE format('ALTER TABLE public.%I SET SCHEMA attic', t);
            n_att := n_att + 1;
        END IF;
    END LOOP;

    RAISE NOTICE 'в ref: %, на чердак: %', n_ref, n_att;
END $$;

COMMIT;

SELECT count(*) AS tablic_v_public
FROM pg_class c JOIN pg_namespace n ON n.oid = c.relnamespace
WHERE n.nspname = 'public' AND c.relkind = 'r';
