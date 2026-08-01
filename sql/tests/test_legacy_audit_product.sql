-- Старый журнал нужен двум перенесённым прикладным таблицам и должен входить
-- в чистый продукт без пользовательских объектов в public.
\set ON_ERROR_STOP on

BEGIN;

SET LOCAL client_min_messages = warning;

DO $$
DECLARE
    before_count bigint;
    after_count bigint;
    test_id bigint;
BEGIN
    IF to_regclass('meta.audit_log') IS NULL
       OR to_regclass('meta.changed_object') IS NULL
       OR to_regprocedure('meta.log_changes()') IS NULL
       OR to_regprocedure('meta.rollback_change(integer)') IS NULL
       OR to_regprocedure('meta.rollback_group(uuid)') IS NULL THEN
        RAISE EXCEPTION 'Подсистема исходного аудита перенесена не полностью';
    END IF;

    IF to_regclass('public.audit_log') IS NOT NULL
       OR to_regclass('public.changed_object') IS NOT NULL
       OR to_regprocedure('public.log_changes()') IS NOT NULL THEN
        RAISE EXCEPTION 'Объекты исходного аудита остались в public';
    END IF;

    IF EXISTS (
        SELECT 1
        FROM pg_trigger t
        JOIN pg_class c ON c.oid = t.tgrelid
        JOIN pg_namespace n ON n.oid = c.relnamespace
        JOIN pg_proc p ON p.oid = t.tgfoid
        JOIN pg_namespace pn ON pn.oid = p.pronamespace
        WHERE NOT t.tgisinternal
          AND n.nspname IN (
              'net', 'ref', 'meta', 'calc', 'addr', 'doc', 'el', 'ops', 'org')
          AND pn.nspname = 'public') THEN
        RAISE EXCEPTION 'Продуктовый триггер зависит от функции public';
    END IF;

    SELECT id INTO test_id FROM addr.zdaniya_tu ORDER BY id LIMIT 1;
    IF test_id IS NULL THEN
        RAISE EXCEPTION 'Нет строки для функциональной проверки аудита';
    END IF;

    SELECT count(*) INTO before_count FROM meta.audit_log;
    UPDATE addr.zdaniya_tu
       SET kommentariy = coalesce(kommentariy, '') || ' [audit test]'
     WHERE id = test_id;
    SELECT count(*) INTO after_count FROM meta.audit_log;

    IF after_count <> before_count + 1 THEN
        RAISE EXCEPTION 'Триггер аудита не записал изменение: % -> %',
            before_count, after_count;
    END IF;
END
$$;

ROLLBACK;

SELECT 'test_legacy_audit_product: ok' AS result;
