-- Объекты сети, перенесённые в net, убираются из public на чердак.
--
-- Каждая таблица сверена с её объектной таблицей net перед переносом.
-- Результат сверки записан здесь, чтобы при разборе не пришлось
-- повторять её заново.
--
-- Полностью перенесены (в net нет ни одной строки без пары):
--
--   ugol_povorota_truboprovoda  25 214 -> net.pipe_turn            25 214
--   lyuki                       18 392 -> net.manhole              18 392
--   opora                        3 611 -> net.support               3 611
--   perehod_diametra                 4 -> net.diameter_change            4
--   truby_rekonstruiruemye          44 -> net.pipe_reconstructed_point 5
--                                        + ..._line                    39
--
-- Перенесены частично, остальное мертво в самом источнике:
--
--   wdodevices                   3 385 -> net.node_metering_device      10
--
--     Из 3 375 неперенесённых 2 845 не имеют узла вовсе, а у остальных
--     530 узел не существует. Проверено на НЕТРОНУТОЙ исходной БД
--     запросом самого приложения: оно видело те же 10 строк из 3 385.
--     Это мёртвые записи, а не потеря миграции.
--
-- Структура заведена, данные не переносились по решению заказчика:
--
--   bypass                           0 -> net.bypass
--   reversevalves                    0 -> net.valve_reverse
--   regularmatures                   0 -> net.armature_control
--   consumptregulators               7 -> net.regulator_consumption
--   pressdropregulators             13 -> net.regulator_pressdrop
--
--     У последних двух в источнике 7 и 13 строк. Они относятся к
--     оборудованию без концов; решение «структуру завести, данные не
--     тянуть» принято заказчиком. Здесь они сохраняются на чердаке —
--     единственная копия.
--
-- Схема attic не входит в search_path: забытое обращение упадёт
-- с внятной ошибкой. Откат — sql/127_attic_rollback.sql.

BEGIN;

CREATE SCHEMA IF NOT EXISTS attic;

DO $$
DECLARE
    t       text;
    n_move  int := 0;
    n_skip  int := 0;
BEGIN
    FOREACH t IN ARRAY ARRAY[
        'bypass',
        'consumptregulators',
        'lyuki',
        'opora',
        'perehod_diametra',
        'pressdropregulators',
        'regularmatures',
        'reversevalves',
        'truby_rekonstruiruemye',
        'ugol_povorota_truboprovoda',
        'wdodevices'
    ]
    LOOP
        IF to_regclass('public.' || quote_ident(t)) IS NULL THEN
            n_skip := n_skip + 1;
            CONTINUE;
        END IF;
        EXECUTE format('ALTER TABLE public.%I SET SCHEMA attic', t);
        n_move := n_move + 1;
    END LOOP;

    RAISE NOTICE 'убрано перенесённых объектов: %, пропущено: %',
                 n_move, n_skip;
END $$;

COMMIT;

SELECT count(*) AS tablic_v_public
FROM pg_class c JOIN pg_namespace n ON n.oid = c.relnamespace
WHERE n.nspname = 'public' AND c.relkind = 'r';
