-- Сохраняем старый журнал изменений как отдельную подсистему продукта.
-- Две перенесённые таблицы addr/doc всё ещё используют log_changes(); без
-- переноса функции продуктовый дамп нельзя восстановить без схемы public.

\set ON_ERROR_STOP on

BEGIN;

CREATE SCHEMA IF NOT EXISTS meta;

DO $$
BEGIN
    IF to_regclass('public.audit_log') IS NOT NULL
       AND to_regclass('meta.audit_log') IS NULL THEN
        ALTER TABLE public.audit_log SET SCHEMA meta;
    END IF;

    IF to_regclass('public.changed_object') IS NOT NULL
       AND to_regclass('meta.changed_object') IS NULL THEN
        ALTER TABLE public.changed_object SET SCHEMA meta;
    END IF;

    IF to_regprocedure('public.log_changes()') IS NOT NULL
       AND to_regprocedure('meta.log_changes()') IS NULL THEN
        ALTER FUNCTION public.log_changes() SET SCHEMA meta;
    END IF;

    IF to_regprocedure('public.rollback_change(integer)') IS NOT NULL
       AND to_regprocedure('meta.rollback_change(integer)') IS NULL THEN
        ALTER FUNCTION public.rollback_change(integer) SET SCHEMA meta;
    END IF;

    IF to_regprocedure('public.rollback_group(uuid)') IS NOT NULL
       AND to_regprocedure('meta.rollback_group(uuid)') IS NULL THEN
        ALTER FUNCTION public.rollback_group(uuid) SET SCHEMA meta;
    END IF;
END
$$;

-- В старых функциях имена audit_log/rollback_change и таблиц отката не
-- квалифицированы схемой. Фиксированный search_path сохраняет их поведение и
-- не зависит от настроек конкретного клиента QGIS/Qt.
DO $$
DECLARE
    function_name text;
BEGIN
    FOREACH function_name IN ARRAY ARRAY[
        'log_changes()',
        'rollback_change(integer)',
        'rollback_group(uuid)'
    ] LOOP
        IF to_regprocedure('meta.' || function_name) IS NOT NULL THEN
            EXECUTE format(
                'ALTER FUNCTION meta.%s SET search_path = '
                'pg_catalog, public, meta, net, ref, calc, addr, doc, el, ops, org',
                function_name);
        END IF;
    END LOOP;
END
$$;

COMMENT ON TABLE meta.audit_log IS
    'Исходный журнал ТГИД, сохранённый для addr/doc и операций отката';
COMMENT ON TABLE meta.changed_object IS
    'Исходный список изменённых объектов ТГИД';

INSERT INTO meta.schema_version (version, description)
VALUES (18, 'Перенос исходного журнала и функций аудита из public в meta')
ON CONFLICT (version) DO NOTHING;

COMMIT;
