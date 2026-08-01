-- Результаты расчёта переезжают из public в схему calc.
--
-- Цель — «одна БД net, ничего старого»: в public не должно остаться
-- ни одной таблицы ТГИД. Результаты режимов не относятся к объектам
-- сети и не место им ни в net, ни в ref — для них своя схема calc.
--
-- Переносится ALTER TABLE ... SET SCHEMA: вместе с таблицей переезжают
-- её индексы, ограничения и принадлежащие колонкам последовательности,
-- данные остаются на месте физически. Операция обратима — откат в
-- 121_calc_rollback.sql.
--
-- Старое приложение gid8 обращается к public.*_out напрямую. Если БД
-- должна и дальше его обслуживать, поверх calc ставятся представления:
-- 122_calc_compat.sql. В продуктовой БД они не нужны.

BEGIN;

CREATE SCHEMA IF NOT EXISTS calc;

COMMENT ON SCHEMA calc IS
    'Результаты гидравлических и тепловых расчётов. Заполняется движком '
    'sety, читается отчётами и пьезометрическими графиками. Объекты сети '
    'здесь не хранятся — они в net.';

DO $$
DECLARE
    t   record;
    n   int := 0;
BEGIN
    FOR t IN
        SELECT c.relname
        FROM pg_class c
        JOIN pg_namespace ns ON ns.oid = c.relnamespace
        WHERE ns.nspname = 'public'
          AND c.relkind = 'r'
          AND (c.relname LIKE '%\_out'
               OR c.relname IN ('calculation', 'numberedcalculations'))
        ORDER BY c.relname
    LOOP
        EXECUTE format('ALTER TABLE public.%I SET SCHEMA calc', t.relname);
        n := n + 1;
    END LOOP;

    RAISE NOTICE 'перенесено таблиц в calc: %', n;

    IF n = 0 THEN
        RAISE NOTICE 'в public таблиц результатов не найдено — '
                     'вероятно, перенос уже выполнен';
    END IF;
END $$;

COMMIT;

-- Проверка: в public не должно остаться таблиц результатов.
SELECT count(*) AS ostalos_v_public
FROM pg_class c
JOIN pg_namespace ns ON ns.oid = c.relnamespace
WHERE ns.nspname = 'public'
  AND c.relkind = 'r'
  AND (c.relname LIKE '%\_out' OR c.relname IN ('calculation'));

SELECT count(*) AS tablic_v_calc
FROM pg_class c
JOIN pg_namespace ns ON ns.oid = c.relnamespace
WHERE ns.nspname = 'calc' AND c.relkind = 'r';
