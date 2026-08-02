-- Восемь таблиц эксплуатации возвращаются с чердака в ops.
--
-- Они попали на чердак как «пустые и никем не используемые»: движок
-- расчёта их действительно не читает. Но их использует приложение gid8
-- — ремонты, опрессовки, осмотры, дефекты, факторы риска:
--
--   remont2, remont2deployed   gid8/property/prop_fun2_progr.cpp,
--                              gid8/remonts/opres.cpp
--   opres, opresdeployed       gid8/remonts/opres.cpp
--   osmotr, osmotrdeployed     gid8/remonts/opres.cpp
--   defect                     gid8/geo/geo_open.cpp
--   faktory_riska_truboprovoda gid8/remonts/opres.cpp
--
-- Проверка «используется» в public_inventory.py смотрела код движка,
-- SQL репозитория и Qt-клиента, но не исходники gid8 — оттуда и
-- пропуск. Пустая таблица, в которую приложение пишет, выглядит
-- неиспользуемой ровно до первого запуска.
--
-- Место — ops: это подсистема эксплуатации, туда же ушли остальные
-- ремонты и шурфы.

BEGIN;

DO $$
DECLARE
    t text;
    n int := 0;
BEGIN
    FOREACH t IN ARRAY ARRAY[
        'remont2', 'remont2deployed',
        'opres', 'opresdeployed',
        'osmotr', 'osmotrdeployed',
        'defect', 'faktory_riska_truboprovoda'
    ]
    LOOP
        IF to_regclass('attic.' || quote_ident(t)) IS NOT NULL
           AND to_regclass('ops.' || quote_ident(t)) IS NULL THEN
            EXECUTE format('ALTER TABLE attic.%I SET SCHEMA ops', t);
            n := n + 1;
        END IF;
    END LOOP;
    RAISE NOTICE 'возвращено в ops: %', n;
END $$;

COMMIT;
