-- Запись через представления net: UPDATE обязан доходить и до
-- объектной таблицы класса, и до строк extra_*.
--
-- Расчёт переведён с public.generalizedconsumers на
-- net.v_generalizedconsumers. Прогон расчёта этого не проверяет: путь
-- записи включается не всегда. Поэтому проверка отдельная.
--
-- Всё в транзакции с откатом.
--
--   psql -d tgid_gis -v ON_ERROR_STOP=1 -f sql/tests/test_net_view_update.sql

BEGIN;

DO $$
DECLARE
    id_cls   bigint;
    n        int;
BEGIN
    -- 1. Строка из объектной таблицы класса.
    SELECT o.id INTO id_cls FROM net.consumer_general o LIMIT 1;
    IF id_cls IS NULL THEN
        RAISE EXCEPTION 'net.consumer_general пуста — проверка невозможна';
    END IF;

    UPDATE net.v_generalizedconsumers AS gc
       SET normhlosflow = 98765
     WHERE gc.nodeid = id_cls;

    SELECT count(*) INTO n FROM net.consumer_general
     WHERE id = id_cls AND normhlosflow = 98765;
    IF n <> 1 THEN
        RAISE EXCEPTION 'запись в класс не дошла: строк %', n;
    END IF;

    -- 2. Строка из extra_*: ИЗВЕСТНЫЙ ДЕФЕКТ.
    --
    -- Триггер net.v_generalizedconsumers_upd выполняет
    --   UPDATE net.consumer_general ... WHERE id = OLD.nodeid
    -- то есть адресует строку по УЗЛУ. У строки из extra_* поле nodeid
    -- указывает на тот же узел, что и у строки класса, поэтому правка
    -- дубля либо не находит ничего, либо попадает в строку класса.
    -- В саму extra_* она не доходит никогда.
    --
    -- Дефект существовал и до перевода записи на net: представление
    -- public.generalizedconsumers — обёртка над тем же net.v_*.
    --
    -- Не исправляется здесь намеренно: что должна делать правка дубля —
    -- вопрос, связанный с разбором спорных объектов (docs/07). Тест
    -- фиксирует поведение, чтобы оно не считалось нормой.
    SELECT count(*) INTO n FROM net.extra_generalizedconsumers;
    IF n > 0 THEN
        RAISE WARNING 'строк-дублей, недоступных для правки: % '
                      '(generalizedconsumers)', n;
    END IF;
    SELECT count(*) INTO n FROM net.extra_realconsumers;
    IF n > 0 THEN
        RAISE WARNING 'строк-дублей, недоступных для правки: % '
                      '(realconsumers)', n;
    END IF;

    -- 3. То же для потребителей реальных — их правит dross1.
    SELECT o.id INTO id_cls FROM net.consumer_real o LIMIT 1;
    UPDATE net.v_realconsumers AS rc
       SET b23 = 12345
     WHERE rc.nodeid = id_cls;

    SELECT count(*) INTO n FROM net.consumer_real
     WHERE id = id_cls AND b23 = 12345;
    IF n <> 1 THEN
        RAISE EXCEPTION 'запись в consumer_real не дошла: строк %', n;
    END IF;

    RAISE NOTICE 'запись через представления net доходит до объектных '
                 'таблиц классов; строки extra_* правке недоступны '
                 '(см. предупреждения выше)';
END $$;

ROLLBACK;
