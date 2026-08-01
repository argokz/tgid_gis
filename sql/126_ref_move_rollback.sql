-- Откат 125_ref_move.sql: справочники возвращаются в public.
--
-- Возвращаются ВСЕ таблицы ref, а не только перенесённые 124-м скриптом:
-- разделить их по происхождению после переноса уже нельзя, а держать
-- отдельный журнал ради отката не стоит. Если нужен частичный откат,
-- перечислите таблицы вручную.
--
-- 27 справочников, которые до 124-го скрипта существовали в двух копиях,
-- этот откат вернёт в public в единственном экземпляре — то есть в
-- состояние ПОСЛЕ дедупликации, а не до неё. Это и правильно: вторая
-- копия была дефектом.

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
        WHERE ns.nspname = 'ref' AND c.relkind = 'r'
        ORDER BY c.relname
    LOOP
        IF EXISTS (
            SELECT 1 FROM pg_class c2
            JOIN pg_namespace n2 ON n2.oid = c2.relnamespace
            WHERE n2.nspname = 'public' AND c2.relname = t.relname)
        THEN
            RAISE WARNING 'public.% занято — таблица оставлена в ref',
                          t.relname;
            CONTINUE;
        END IF;

        EXECUTE format('ALTER TABLE ref.%I SET SCHEMA public', t.relname);
        n := n + 1;
    END LOOP;

    RAISE NOTICE 'возвращено справочников в public: %', n;
END $$;

COMMIT;
