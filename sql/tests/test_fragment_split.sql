-- Сверка разложения фрагмента: каждое значение из attic.fragments
-- обязано найтись в net.fragment / fragment_system / fragment_month.
--
-- Проверяются ВСЕ 132 параметра каждого фрагмента, а не выборка:
-- потеря одного столбца при разложении выглядит точно так же, как
-- успех, если смотреть на пару значений.
--
--   psql -d tgid_gis -v ON_ERROR_STOP=1 -f sql/tests/test_fragment_split.sql

DO $$
DECLARE
    g        text;
    i        int;
    bad      bigint;
    checked  int := 0;
BEGIN
    -- Параметры по системам: 12 групп × 5 систем.
    FOREACH g IN ARRAY ARRAY['tvn', 'g1n_sys', 'g2n_sys', 'gnz_sys',
                             'qn_pot', 'qn_gv_pot', 'q1n_tp', 'q2n_tp',
                             't1n_pot', 't2n_pot', 't1n_sys', 't2n_sys']
    LOOP
        FOR i IN 1..5 LOOP
            EXECUTE format($q$
                SELECT count(*) FROM attic.fragments s
                JOIN net.fragment f ON f.id = s.id
                JOIN net.fragment_system y
                  ON y.fragment_id = f.id AND y.sys_no = %s
                WHERE s.%I IS DISTINCT FROM y.%I
            $q$, i, g || '_' || i, g) INTO bad;

            IF bad <> 0 THEN
                RAISE EXCEPTION 'системы: % (система %) расходится в % строках',
                                g, i, bad;
            END IF;
            checked := checked + 1;
        END LOOP;
    END LOOP;

    -- Температуры по месяцам: 6 групп × 12 месяцев.
    FOREACH g IN ARRAY ARRAY['tn_long', 'tgr_long', 'tpod_long',
                             'tn_fakt', 'tgr_fakt', 'tpod_fakt']
    LOOP
        FOR i IN 1..12 LOOP
            EXECUTE format($q$
                SELECT count(*) FROM attic.fragments s
                JOIN net.fragment f ON f.id = s.id
                JOIN net.fragment_month m
                  ON m.fragment_id = f.id AND m.month = %s
                WHERE s.%I IS DISTINCT FROM m.%I
            $q$, i, g || '_' || i, g) INTO bad;

            IF bad <> 0 THEN
                RAISE EXCEPTION 'месяцы: % (месяц %) расходится в % строках',
                                g, i, bad;
            END IF;
            checked := checked + 1;
        END LOOP;
    END LOOP;

    -- Скалярные поля паспорта.
    FOREACH g IN ARRAY ARRAY['name', 'name_sys', 'nasel_point', 'sezon',
                             'year', 't_or', 't_vr', 't_vnew', 'tx',
                             'tx_leto', 'tip_qgvs', 'begin_year',
                             'end_year', 'tn_god', 'tg_god', 'tn_god_leto',
                             'tg_god_leto', 'a', 'year_audit',
                             'phone_manager', 'id_old']
    LOOP
        EXECUTE format($q$
            SELECT count(*) FROM attic.fragments s
            JOIN net.fragment f ON f.id = s.id
            WHERE s.%I IS DISTINCT FROM f.%I
        $q$, g, g) INTO bad;

        IF bad <> 0 THEN
            RAISE EXCEPTION 'паспорт: % расходится в % строках', g, bad;
        END IF;
        checked := checked + 1;
    END LOOP;

    RAISE NOTICE 'сверено значений на фрагмент: % — расхождений нет',
                 checked;
END $$;
