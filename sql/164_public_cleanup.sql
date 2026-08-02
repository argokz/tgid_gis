-- Последняя партия public: справочники в ref, остальное на чердак.
--
-- Разбор каждой таблицы — docs/14, раздел «Разбор семи оставшихся».
-- Две записи там пришлось исправить: обе я сначала оценил по имени.
--
-- klapany__ventili__zapornye (17 731) — НЕ объекты сети. В таблице нет
--   ни узла, ни линии, ни геометрии, ни фрагмента: наименование, Ду, Ру,
--   материал корпуса, рабочая среда, тип присоединения и ЦЕНА. Это
--   прайс-каталог запорной арматуры. Место — ref, класс в net не нужен.
--
-- table (73) — не мусор, вопреки имени: kotelnye, kod_istochnika,
--   nagruzka_otoplenie, nagruzka_ventilyatsiya, nagruzka_par,
--   nagruzka_goryachaya_voda. Это нагрузки котельных. Переносится в ref
--   с осмысленным именем kotelnye_nagruzki: имя "table" ломает любой
--   запрос без кавычек, потому что это ключевое слово SQL.
--
-- magistrali (14) — дубль ref.magistrals (14): те же магистрали
--   (М-1, М-2, М-3, М-3А...), русские имена колонок против английских,
--   разные диапазоны id. Авторитетной остаётся ref.magistrals, она уже
--   в модели; русская версия сохраняется на чердаке до решения
--   заказчика, какая из двух верна.
--
-- vyd (51), tab (1) — рабочие выборки, ни в движке, ни в SQL
--   репозитория не упоминаются. На чердак; удалять без ответа
--   заказчика нельзя, копия одна.
--
-- После этого в public остаётся только spatial_ref_sys — служебная
-- таблица PostGIS, и два её представления geometry_columns и
-- geography_columns.
--
-- Откат: sql/125_ref_move_rollback.sql, sql/127_attic_rollback.sql.

BEGIN;

CREATE SCHEMA IF NOT EXISTS ref;
CREATE SCHEMA IF NOT EXISTS attic;

DO $$
BEGIN
    IF to_regclass('public.klapany__ventili__zapornye') IS NOT NULL
       AND to_regclass('ref.klapany__ventili__zapornye') IS NULL THEN
        ALTER TABLE public.klapany__ventili__zapornye SET SCHEMA ref;
        RAISE NOTICE 'прайс-каталог арматуры перенесён в ref';
    END IF;

    IF to_regclass('public.table') IS NOT NULL
       AND to_regclass('ref.kotelnye_nagruzki') IS NULL THEN
        ALTER TABLE public."table" SET SCHEMA ref;
        ALTER TABLE ref."table" RENAME TO kotelnye_nagruzki;
        RAISE NOTICE 'table -> ref.kotelnye_nagruzki';
    END IF;

    IF to_regclass('public.magistrali') IS NOT NULL
       AND to_regclass('attic.magistrali') IS NULL THEN
        ALTER TABLE public.magistrali SET SCHEMA attic;
        RAISE NOTICE 'magistrali (дубль) убрана на чердак';
    END IF;

    IF to_regclass('public.vyd') IS NOT NULL
       AND to_regclass('attic.vyd') IS NULL THEN
        ALTER TABLE public.vyd SET SCHEMA attic;
    END IF;

    IF to_regclass('public.tab') IS NOT NULL
       AND to_regclass('attic.tab') IS NULL THEN
        ALTER TABLE public.tab SET SCHEMA attic;
    END IF;
END $$;

COMMENT ON TABLE ref.kotelnye_nagruzki IS
    'Нагрузки котельных: отопление, вентиляция, пар, горячая вода. '
    'В исходной БД называлась public."table" — имя, требующее кавычек '
    'в каждом запросе.';

COMMENT ON TABLE ref.klapany__ventili__zapornye IS
    'Прайс-каталог запорной арматуры: типоразмеры, материалы, цены. '
    'Объектом сети не является — ни узла, ни геометрии в ней нет.';

COMMIT;

SELECT c.relname, c.relkind
FROM pg_class c JOIN pg_namespace n ON n.oid = c.relnamespace
WHERE n.nspname = 'public' AND c.relkind IN ('r', 'v')
ORDER BY 1;
