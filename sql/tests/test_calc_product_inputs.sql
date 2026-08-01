-- Минимальный контракт гидравлического ядра на чистой продуктовой БД.
-- Здесь намеренно нет net.v_nodes/net.v_<подтип>: sety читает тонкие
-- слои карты и физические объектные/extra-таблицы.

DO $$
DECLARE
    t text;
    n bigint;
BEGIN
    FOREACH t IN ARRAY ARRAY[
        'heatsystem', 'specexpends', 'calctemperatures', 'varcoefficients'
    ]
    LOOP
        IF to_regclass(format('ref.%I', t)) IS NULL THEN
            RAISE EXCEPTION 'нет обязательного расчётного справочника ref.%', t;
        END IF;
        EXECUTE format('SELECT count(*) FROM ref.%I', t) INTO n;
        IF n = 0 THEN
            RAISE EXCEPTION 'обязательный расчётный справочник ref.% пуст', t;
        END IF;
    END LOOP;

    IF to_regclass('net.v_map_nodes') IS NULL
       OR to_regclass('net.v_map_lines') IS NULL THEN
        RAISE EXCEPTION 'нет продуктовых тонких слоёв карты для sety';
    END IF;

    IF NOT EXISTS (
        SELECT 1 FROM information_schema.columns
        WHERE table_schema = 'net' AND table_name = 'v_map_nodes'
          AND column_name = 'geomarktoptube'
    ) OR NOT EXISTS (
        SELECT 1 FROM information_schema.columns
        WHERE table_schema = 'net' AND table_name = 'v_map_nodes'
          AND column_name = 'geomarknodearea'
    ) THEN
        RAISE EXCEPTION 'v_map_nodes не отдаёт геодезические отметки для sety';
    END IF;

    -- Рабочая миграционная БД пока хранит attic и может сохранять net.v_*
    -- для старых клиентов. В чистом продукте attic не экспортируется и
    -- переходных представлений быть не должно.
    IF to_regnamespace('attic') IS NULL THEN
        FOREACH t IN ARRAY ARRAY[
            'v_nodes', 'v_linesobj', 'v_heatsources',
            'v_generalizedconsumers', 'v_realconsumers', 'v_connectnodes'
        ]
        LOOP
            IF to_regclass(format('net.%I', t)) IS NOT NULL THEN
                RAISE EXCEPTION 'в продукт вернулось переходное представление net.%', t;
            END IF;
        END LOOP;
    END IF;

    FOREACH t IN ARRAY ARRAY[
        'calculation', 'pt_out', 'ut_out', 'us_out', 'ns_out', 'rs_out'
    ]
    LOOP
        IF to_regclass(format('calc.%I', t)) IS NULL THEN
            RAISE EXCEPTION 'нет таблицы результата calc.%', t;
        END IF;
    END LOOP;
END $$;
