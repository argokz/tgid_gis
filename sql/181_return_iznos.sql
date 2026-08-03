-- Таблицы расчёта износа (aIznos / OnWear) возвращаются с чердака в ops.
--
-- Попали в attic как пустые; gid8 пишет в них новые расчёты износа
-- участков (gid6/remont_gid.cpp::OnWear → ps.CALCULATION_IZNOS / ps.iznos).
-- Пустая таблица, в которую пишет приложение, выглядит «неиспользуемой»
-- только до первого запуска команды «Износ оборудования».

BEGIN;

DO $$
DECLARE
    t text;
    n int := 0;
BEGIN
    FOREACH t IN ARRAY ARRAY[
        'calculation_iznos',
        'iznos'
    ]
    LOOP
        IF to_regclass('attic.' || quote_ident(t)) IS NOT NULL
           AND to_regclass('ops.' || quote_ident(t)) IS NULL THEN
            EXECUTE format('ALTER TABLE attic.%I SET SCHEMA ops', t);
            n := n + 1;
        END IF;
    END LOOP;
    RAISE NOTICE 'возвращено таблиц износа в ops: %', n;
END $$;

COMMIT;
