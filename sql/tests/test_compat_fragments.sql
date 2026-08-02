-- compat.fragments обязана отдавать то же, что исходная таблица.
--
-- Представление собирает 156 колонок обратно из net.fragment,
-- fragment_system (5 систем) и fragment_month (12 месяцев). Ошибка в
-- любом из 132 соединений по индексу выглядит как успех, если смотреть
-- на пару значений, поэтому сверяются все колонки подряд.
--
--   psql -d tgid_gis -v ON_ERROR_STOP=1 -f sql/tests/test_compat_fragments.sql

DO $$
DECLARE
    c        record;
    bad      bigint;
    checked  int := 0;
BEGIN
    IF to_regclass('attic.fragments') IS NULL THEN
        RAISE NOTICE 'attic.fragments нет — сверять не с чем';
        RETURN;
    END IF;

    FOR c IN
        SELECT column_name FROM information_schema.columns
        WHERE table_schema = 'attic' AND table_name = 'fragments'
          -- removed/idremoved в net не переносились: у объекта свой
          -- признак удаления removed_at.
          AND column_name NOT IN ('removed', 'idremoved')
        ORDER BY ordinal_position
    LOOP
        EXECUTE format($q$
            SELECT count(*) FROM attic.fragments s
            JOIN compat.fragments v ON v.id = s.id
            WHERE s.%I IS DISTINCT FROM v.%I
        $q$, c.column_name, c.column_name) INTO bad;

        IF bad <> 0 THEN
            RAISE EXCEPTION 'колонка % расходится в % строках',
                            c.column_name, bad;
        END IF;
        checked := checked + 1;
    END LOOP;

    RAISE NOTICE 'сверено колонок: % — compat.fragments совпадает '
                 'с исходной таблицей', checked;
END $$;
