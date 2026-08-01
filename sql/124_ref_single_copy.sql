-- Справочники: одна копия вместо двух.
--
-- 098_core_references.sql копировал нужные справочники из public в ref,
-- чтобы собрать чистый продуктовый дамп. Оригиналы при этом оставались
-- в public, и в переходной БД справочник существовал в двух экземплярах.
-- Внешние ключи net смотрят на ref, а приложение и движок читают public:
-- пока копии совпадают, разницы не видно, но любая правка справочника
-- разводит их молча.
--
-- Здесь public-копии удаляются. Предварительно проверено:
--   * все 27 таблиц ref совпадают с public построчно (EXCEPT даёт 0);
--   * ни один внешний ключ не ссылается на public-копии.
--
-- Обращения по имени без схемы продолжают работать: порядок поиска
-- задан на уровне БД (см. ниже), и после удаления public-копии имя
-- разрешается в ref.
--
-- DROP без CASCADE намеренно: если на таблицу опирается представление
-- или ограничение, скрипт обязан упасть, а не тихо снести зависимости.

BEGIN;

-- Порядок поиска — свойство БД, а не отдельного клиента: так его видят
-- и движок sety, и Qt-клиент, и psql, и старое приложение. Имя берём
-- динамически: в чистой сборке база называется иначе.
DO $configure_database$
BEGIN
    EXECUTE format(
        'ALTER DATABASE %I SET search_path = public, net, ref, calc',
        current_database());
END
$configure_database$;

DO $$
DECLARE
    t        record;
    n_drop   int := 0;
    n_skip   int := 0;
    n_diff   bigint;
BEGIN
    FOR t IN
        SELECT c.relname
        FROM pg_class c
        JOIN pg_namespace ns ON ns.oid = c.relnamespace
        WHERE ns.nspname = 'ref' AND c.relkind = 'r'
        ORDER BY c.relname
    LOOP
        IF NOT EXISTS (
            SELECT 1 FROM pg_class c2
            JOIN pg_namespace n2 ON n2.oid = c2.relnamespace
            WHERE n2.nspname = 'public' AND c2.relname = t.relname
              AND c2.relkind = 'r')
        THEN
            CONTINUE;   -- дубля нет, удалять нечего
        END IF;

        -- Ещё раз, уже в транзакции: удаляем только точный дубль.
        EXECUTE format(
            'SELECT count(*) FROM ('
            '  SELECT * FROM public.%I EXCEPT SELECT * FROM ref.%I'
            '  UNION ALL'
            '  SELECT * FROM ref.%I EXCEPT SELECT * FROM public.%I) x',
            t.relname, t.relname, t.relname, t.relname)
        INTO n_diff;

        IF n_diff <> 0 THEN
            RAISE WARNING 'public.% и ref.% РАЗЛИЧАЮТСЯ (% строк) — '
                          'оставлено как есть, нужен разбор',
                          t.relname, t.relname, n_diff;
            n_skip := n_skip + 1;
            CONTINUE;
        END IF;

        EXECUTE format('DROP TABLE public.%I', t.relname);
        n_drop := n_drop + 1;
    END LOOP;

    RAISE NOTICE 'удалено дублей из public: %, оставлено на разбор: %',
                 n_drop, n_skip;
END $$;

COMMIT;

SELECT count(*) AS dublei_ostalos
FROM pg_class c
JOIN pg_namespace ns ON ns.oid = c.relnamespace
WHERE ns.nspname = 'public' AND c.relkind = 'r'
  AND c.relname IN (SELECT c2.relname FROM pg_class c2
                    JOIN pg_namespace n2 ON n2.oid = c2.relnamespace
                    WHERE n2.nspname = 'ref' AND c2.relkind = 'r');
