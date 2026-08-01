-- Откат 140_subsystems.sql: подсистемы возвращаются в public.
--
-- Возвращаются все таблицы схем addr, doc, el, ops, org, и сами схемы
-- удаляются. search_path возвращается к виду без подсистем.

BEGIN;

DO $$
DECLARE
    s text;
    t record;
    n int := 0;
BEGIN
    FOREACH s IN ARRAY ARRAY['addr', 'doc', 'el', 'ops', 'org']
    LOOP
        FOR t IN
            EXECUTE format(
                'SELECT c.relname FROM pg_class c'
                ' JOIN pg_namespace ns ON ns.oid = c.relnamespace'
                ' WHERE ns.nspname = %L AND c.relkind = ''r''', s)
        LOOP
            IF EXISTS (SELECT 1 FROM pg_class c2
                       JOIN pg_namespace n2 ON n2.oid = c2.relnamespace
                       WHERE n2.nspname = 'public' AND c2.relname = t.relname)
            THEN
                RAISE WARNING 'public.% занято — оставлено в %', t.relname, s;
                CONTINUE;
            END IF;
            EXECUTE format('ALTER TABLE %I.%I SET SCHEMA public',
                           s, t.relname);
            n := n + 1;
        END LOOP;
    END LOOP;

    RAISE NOTICE 'возвращено в public: %', n;
END $$;

ALTER DATABASE tgid_gis SET search_path = public, net, ref, calc;

DROP SCHEMA IF EXISTS addr;
DROP SCHEMA IF EXISTS doc;
DROP SCHEMA IF EXISTS el;
DROP SCHEMA IF EXISTS ops;
DROP SCHEMA IF EXISTS org;

COMMIT;
