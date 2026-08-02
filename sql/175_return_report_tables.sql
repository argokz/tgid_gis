-- Таблицы подсистемы отчётов возвращаются с чердака.
--
-- Их использует python/poteriNewPg — выгрузки в Excel. На чердак они
-- попали как «пустые и никем не используемые»: проверка «используется»
-- смотрела движок расчёта, SQL репозитория и Qt-клиент, но ни
-- исходники gid8, ни подсистему отчётов. Это уже второй такой случай
-- после sql/174 — признак того, что критерий «используется» был
-- заужен с самого начала.
--
--   heatlosesfact, heatlosessourcefact, heatlosessourcemonthsfact,
--   losesbyfillingfact      — фактические тепловые потери, в ref рядом
--                             с нормативными heatloses*
--   heatpipesectionsharness — привязка участков, туда же
--   tankbatteries           — баки-аккумуляторы
--   heatpoint               — тепловые пункты
--   streets                 — улицы, в addr к остальным адресам

BEGIN;

DO $$
DECLARE
    t text; n int := 0;
BEGIN
    FOREACH t IN ARRAY ARRAY[
        'heatlosesfact', 'heatlosessourcefact', 'heatlosessourcemonthsfact',
        'losesbyfillingfact', 'heatpipesectionsharness', 'tankbatteries',
        'heatpoint'
    ]
    LOOP
        IF to_regclass('attic.' || quote_ident(t)) IS NOT NULL
           AND to_regclass('ref.' || quote_ident(t)) IS NULL THEN
            EXECUTE format('ALTER TABLE attic.%I SET SCHEMA ref', t);
            n := n + 1;
        END IF;
    END LOOP;

    IF to_regclass('attic.streets') IS NOT NULL
       AND to_regclass('addr.streets') IS NULL THEN
        ALTER TABLE attic.streets SET SCHEMA addr;
        n := n + 1;
    END IF;

    RAISE NOTICE 'возвращено таблиц отчётов: %', n;
END $$;

COMMIT;
