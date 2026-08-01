-- Представления совместимости над calc для старого приложения gid8.
--
-- Нужны ТОЛЬКО переходной БД, которая обслуживает и старый gid8, и новый
-- контур. В продуктовой БД этот скрипт не запускается: там public пуст.
--
-- Представление над одной таблицей без агрегатов PostgreSQL обновляет
-- автоматически, поэтому INSTEAD OF-триггеры здесь не нужны: старое
-- приложение продолжает и читать, и писать.

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
        WHERE ns.nspname = 'calc' AND c.relkind = 'r'
        ORDER BY c.relname
    LOOP
        -- Если в public уже есть объект с таким именем, пропускаем:
        -- значит перенос не выполнялся или выполнен наполовину.
        IF EXISTS (SELECT 1 FROM pg_class c2
                   JOIN pg_namespace n2 ON n2.oid = c2.relnamespace
                   WHERE n2.nspname = 'public' AND c2.relname = t.relname)
        THEN
            RAISE NOTICE 'public.% уже существует — пропущено', t.relname;
            CONTINUE;
        END IF;

        EXECUTE format(
            'CREATE VIEW public.%I AS SELECT * FROM calc.%I',
            t.relname, t.relname);
        n := n + 1;
    END LOOP;

    RAISE NOTICE 'создано представлений совместимости: %', n;
END $$;

COMMIT;
