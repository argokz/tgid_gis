-- Откат 126_attic.sql и 128_attic_empty.sql: всё с чердака обратно в public.
--
-- Возвращаются ВСЕ таблицы attic сразу: разделить их по происхождению
-- после переноса нельзя, а вести журнал ради отката не стоит. Для
-- частичного отката перечислите таблицы вручную.

BEGIN;

DO $$
DECLARE
    t record;
    n int := 0;
BEGIN
    FOR t IN
        SELECT c.relname
        FROM pg_class c
        JOIN pg_namespace ns ON ns.oid = c.relnamespace
        WHERE ns.nspname = 'attic' AND c.relkind = 'r'
        ORDER BY c.relname
    LOOP
        IF EXISTS (
            SELECT 1 FROM pg_class c2
            JOIN pg_namespace n2 ON n2.oid = c2.relnamespace
            WHERE n2.nspname = 'public' AND c2.relname = t.relname)
        THEN
            RAISE WARNING 'public.% занято — таблица оставлена на чердаке',
                          t.relname;
            CONTINUE;
        END IF;

        EXECUTE format('ALTER TABLE attic.%I SET SCHEMA public', t.relname);
        n := n + 1;
    END LOOP;

    RAISE NOTICE 'возвращено из attic в public: %', n;
END $$;

COMMIT;
