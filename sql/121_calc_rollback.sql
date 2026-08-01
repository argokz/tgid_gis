-- Откат 120_calc_schema.sql: таблицы результатов возвращаются в public.
--
-- Если поверх calc были поставлены представления совместимости
-- (122_calc_compat.sql), их нужно снять ДО отката — иначе имя занято.

BEGIN;

DO $$
DECLARE
    t record;
    n int := 0;
BEGIN
    -- Сначала убираем представления совместимости, если они есть:
    -- иначе ALTER TABLE ... SET SCHEMA public упрётся в занятое имя.
    FOR t IN
        SELECT c.relname
        FROM pg_class c
        JOIN pg_namespace ns ON ns.oid = c.relnamespace
        WHERE ns.nspname = 'public'
          AND c.relkind = 'v'
          AND (c.relname LIKE '%\_out'
               OR c.relname IN ('calculation', 'numberedcalculations'))
    LOOP
        EXECUTE format('DROP VIEW public.%I', t.relname);
    END LOOP;

    FOR t IN
        SELECT c.relname
        FROM pg_class c
        JOIN pg_namespace ns ON ns.oid = c.relnamespace
        WHERE ns.nspname = 'calc' AND c.relkind = 'r'
        ORDER BY c.relname
    LOOP
        EXECUTE format('ALTER TABLE calc.%I SET SCHEMA public', t.relname);
        n := n + 1;
    END LOOP;

    RAISE NOTICE 'возвращено таблиц в public: %', n;
END $$;

DROP SCHEMA IF EXISTS calc;

COMMIT;
