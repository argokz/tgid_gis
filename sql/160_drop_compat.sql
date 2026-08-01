-- Снятие слоя совместимости: представления ТГИД уходят из public.
--
-- ЭТО ТОЧКА НЕВОЗВРАТА ДЛЯ СТАРОГО ПРИЛОЖЕНИЯ gid8. Оно обращается к
-- public.nodes, public.linesobj и представлениям подтипов напрямую и
-- без них не запустится. Решение о полном переходе принято заказчиком.
--
-- Новый Qt-клиент и движок расчёта sety от них не зависят: чтение и
-- запись переведены на net (docs/14, шаги 9 и 10). Единственное
-- оставшееся упоминание public — setpressnodes в старых ветках
-- read_gid.py и read_zn.py, которые на переведённой БД не выполняются.
--
-- Проверено перед удалением:
--   * ни одно представление, правило или ограничение в БД от них
--     не зависит (pg_depend по pg_rewrite — пусто);
--   * триггеров INSTEAD OF на них нет: запись обрабатывают триггеры
--     на net.v_*, а эти представления — простые обёртки.
--
-- Служебные представления PostGIS geometry_columns и geography_columns
-- не трогаются: они не наши.
--
-- Откат: sql/161_compat_restore.sql — снимок определений, снятый
-- непосредственно перед удалением.
--
-- DROP без CASCADE намеренно: если зависимость всё же обнаружится,
-- скрипт обязан упасть, а не тащить за собой чужие объекты.

BEGIN;

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
          AND c.relkind = 'v'
          AND c.relname NOT IN ('geometry_columns', 'geography_columns')
        ORDER BY c.relname
    LOOP
        EXECUTE format('DROP VIEW public.%I', t.relname);
        n := n + 1;
    END LOOP;

    RAISE NOTICE 'снято представлений совместимости: %', n;
END $$;

COMMIT;

-- В public должны остаться только служебные представления PostGIS.
SELECT count(*) AS predstavleniy_v_public
FROM pg_class c JOIN pg_namespace n ON n.oid = c.relnamespace
WHERE n.nspname = 'public' AND c.relkind = 'v';
