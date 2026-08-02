-- Представление fragments для gid8: три таблицы обратно в одну строку.
--
-- Фрагмент разложен на net.fragment (паспорт), net.fragment_system
-- (параметры по пяти системам) и net.fragment_month (температуры по
-- двенадцати месяцам) — обоснование в docs/15. Приложение gid8
-- обращается к плоской fragments из 109 файлов, поэтому здесь она
-- собирается обратно.
--
-- Обратная запись раскладывается INSTEAD OF-триггером. Это обязательно:
-- старый интерфейс создаёт фрагменты, правит их паспорта и удаляет их.

BEGIN;

CREATE SEQUENCE IF NOT EXISTS net.fragment_id_seq AS integer;

DO $do$
DECLARE
    max_id integer;
    seq_value integer;
BEGIN
    SELECT COALESCE(max(id), 0) INTO max_id FROM net.fragment;
    SELECT last_value INTO seq_value FROM net.fragment_id_seq;

    IF GREATEST(max_id, seq_value) > 0 THEN
        PERFORM setval('net.fragment_id_seq', GREATEST(max_id, seq_value), true);
    ELSE
        PERFORM setval('net.fragment_id_seq', 1, false);
    END IF;
END $do$;

ALTER TABLE net.fragment ALTER COLUMN id
    SET DEFAULT nextval('net.fragment_id_seq');

CREATE OR REPLACE VIEW compat.fragments AS
SELECT
    f.id,
    f.name,
    f.name_sys,
    f.nasel_point,
    f.sezon,
    f.year,
    f.t_or,
    f.t_vr,
    f.t_vnew,
    f.tx,
    f.tx_leto,
    f.tip_qgvs,
    f.begin_year,
    f.end_year,
    f.tn_god,
    f.tg_god,
    f.tn_god_leto,
    f.tg_god_leto,
    f.a,
    f.year_audit,
    y1.tvn AS tvn_1,
    y2.tvn AS tvn_2,
    y3.tvn AS tvn_3,
    y4.tvn AS tvn_4,
    y5.tvn AS tvn_5,
    y1.g1n_sys AS g1n_sys_1,
    y2.g1n_sys AS g1n_sys_2,
    y3.g1n_sys AS g1n_sys_3,
    y4.g1n_sys AS g1n_sys_4,
    y5.g1n_sys AS g1n_sys_5,
    y1.g2n_sys AS g2n_sys_1,
    y2.g2n_sys AS g2n_sys_2,
    y3.g2n_sys AS g2n_sys_3,
    y4.g2n_sys AS g2n_sys_4,
    y5.g2n_sys AS g2n_sys_5,
    y1.gnz_sys AS gnz_sys_1,
    y2.gnz_sys AS gnz_sys_2,
    y3.gnz_sys AS gnz_sys_3,
    y4.gnz_sys AS gnz_sys_4,
    y5.gnz_sys AS gnz_sys_5,
    y1.qn_pot AS qn_pot_1,
    y2.qn_pot AS qn_pot_2,
    y3.qn_pot AS qn_pot_3,
    y4.qn_pot AS qn_pot_4,
    y5.qn_pot AS qn_pot_5,
    y1.qn_gv_pot AS qn_gv_pot_1,
    y2.qn_gv_pot AS qn_gv_pot_2,
    y3.qn_gv_pot AS qn_gv_pot_3,
    y4.qn_gv_pot AS qn_gv_pot_4,
    y5.qn_gv_pot AS qn_gv_pot_5,
    y1.q1n_tp AS q1n_tp_1,
    y2.q1n_tp AS q1n_tp_2,
    y3.q1n_tp AS q1n_tp_3,
    y4.q1n_tp AS q1n_tp_4,
    y5.q1n_tp AS q1n_tp_5,
    y1.q2n_tp AS q2n_tp_1,
    y2.q2n_tp AS q2n_tp_2,
    y3.q2n_tp AS q2n_tp_3,
    y4.q2n_tp AS q2n_tp_4,
    y5.q2n_tp AS q2n_tp_5,
    y1.t1n_pot AS t1n_pot_1,
    y2.t1n_pot AS t1n_pot_2,
    y3.t1n_pot AS t1n_pot_3,
    y4.t1n_pot AS t1n_pot_4,
    y5.t1n_pot AS t1n_pot_5,
    y1.t2n_pot AS t2n_pot_1,
    y2.t2n_pot AS t2n_pot_2,
    y3.t2n_pot AS t2n_pot_3,
    y4.t2n_pot AS t2n_pot_4,
    y5.t2n_pot AS t2n_pot_5,
    y1.t1n_sys AS t1n_sys_1,
    y2.t1n_sys AS t1n_sys_2,
    y3.t1n_sys AS t1n_sys_3,
    y4.t1n_sys AS t1n_sys_4,
    y5.t1n_sys AS t1n_sys_5,
    y1.t2n_sys AS t2n_sys_1,
    y2.t2n_sys AS t2n_sys_2,
    y3.t2n_sys AS t2n_sys_3,
    y4.t2n_sys AS t2n_sys_4,
    y5.t2n_sys AS t2n_sys_5,
    m1.tn_long AS tn_long_1,
    m2.tn_long AS tn_long_2,
    m3.tn_long AS tn_long_3,
    m4.tn_long AS tn_long_4,
    m5.tn_long AS tn_long_5,
    m6.tn_long AS tn_long_6,
    m7.tn_long AS tn_long_7,
    m8.tn_long AS tn_long_8,
    m9.tn_long AS tn_long_9,
    m10.tn_long AS tn_long_10,
    m11.tn_long AS tn_long_11,
    m12.tn_long AS tn_long_12,
    m1.tgr_long AS tgr_long_1,
    m2.tgr_long AS tgr_long_2,
    m3.tgr_long AS tgr_long_3,
    m4.tgr_long AS tgr_long_4,
    m5.tgr_long AS tgr_long_5,
    m6.tgr_long AS tgr_long_6,
    m7.tgr_long AS tgr_long_7,
    m8.tgr_long AS tgr_long_8,
    m9.tgr_long AS tgr_long_9,
    m10.tgr_long AS tgr_long_10,
    m11.tgr_long AS tgr_long_11,
    m12.tgr_long AS tgr_long_12,
    m1.tpod_long AS tpod_long_1,
    m2.tpod_long AS tpod_long_2,
    m3.tpod_long AS tpod_long_3,
    m4.tpod_long AS tpod_long_4,
    m5.tpod_long AS tpod_long_5,
    m6.tpod_long AS tpod_long_6,
    m7.tpod_long AS tpod_long_7,
    m8.tpod_long AS tpod_long_8,
    m9.tpod_long AS tpod_long_9,
    m10.tpod_long AS tpod_long_10,
    m11.tpod_long AS tpod_long_11,
    m12.tpod_long AS tpod_long_12,
    m1.tn_fakt AS tn_fakt_1,
    m2.tn_fakt AS tn_fakt_2,
    m3.tn_fakt AS tn_fakt_3,
    m4.tn_fakt AS tn_fakt_4,
    m5.tn_fakt AS tn_fakt_5,
    m6.tn_fakt AS tn_fakt_6,
    m7.tn_fakt AS tn_fakt_7,
    m8.tn_fakt AS tn_fakt_8,
    m9.tn_fakt AS tn_fakt_9,
    m10.tn_fakt AS tn_fakt_10,
    m11.tn_fakt AS tn_fakt_11,
    m12.tn_fakt AS tn_fakt_12,
    m1.tgr_fakt AS tgr_fakt_1,
    m2.tgr_fakt AS tgr_fakt_2,
    m3.tgr_fakt AS tgr_fakt_3,
    m4.tgr_fakt AS tgr_fakt_4,
    m5.tgr_fakt AS tgr_fakt_5,
    m6.tgr_fakt AS tgr_fakt_6,
    m7.tgr_fakt AS tgr_fakt_7,
    m8.tgr_fakt AS tgr_fakt_8,
    m9.tgr_fakt AS tgr_fakt_9,
    m10.tgr_fakt AS tgr_fakt_10,
    m11.tgr_fakt AS tgr_fakt_11,
    m12.tgr_fakt AS tgr_fakt_12,
    m1.tpod_fakt AS tpod_fakt_1,
    m2.tpod_fakt AS tpod_fakt_2,
    m3.tpod_fakt AS tpod_fakt_3,
    m4.tpod_fakt AS tpod_fakt_4,
    m5.tpod_fakt AS tpod_fakt_5,
    m6.tpod_fakt AS tpod_fakt_6,
    m7.tpod_fakt AS tpod_fakt_7,
    m8.tpod_fakt AS tpod_fakt_8,
    m9.tpod_fakt AS tpod_fakt_9,
    m10.tpod_fakt AS tpod_fakt_10,
    m11.tpod_fakt AS tpod_fakt_11,
    m12.tpod_fakt AS tpod_fakt_12,
    f.phone_manager,
    (f.removed_at IS NOT NULL)::int AS removed,
    NULL::int AS idremoved,
    f.id_old
FROM net.fragment f
LEFT JOIN net.fragment_month m1 ON m1.fragment_id = f.id AND m1.month = 1
LEFT JOIN net.fragment_month m2 ON m2.fragment_id = f.id AND m2.month = 2
LEFT JOIN net.fragment_month m3 ON m3.fragment_id = f.id AND m3.month = 3
LEFT JOIN net.fragment_month m4 ON m4.fragment_id = f.id AND m4.month = 4
LEFT JOIN net.fragment_month m5 ON m5.fragment_id = f.id AND m5.month = 5
LEFT JOIN net.fragment_month m6 ON m6.fragment_id = f.id AND m6.month = 6
LEFT JOIN net.fragment_month m7 ON m7.fragment_id = f.id AND m7.month = 7
LEFT JOIN net.fragment_month m8 ON m8.fragment_id = f.id AND m8.month = 8
LEFT JOIN net.fragment_month m9 ON m9.fragment_id = f.id AND m9.month = 9
LEFT JOIN net.fragment_month m10 ON m10.fragment_id = f.id AND m10.month = 10
LEFT JOIN net.fragment_month m11 ON m11.fragment_id = f.id AND m11.month = 11
LEFT JOIN net.fragment_month m12 ON m12.fragment_id = f.id AND m12.month = 12
LEFT JOIN net.fragment_system y1 ON y1.fragment_id = f.id AND y1.sys_no = 1
LEFT JOIN net.fragment_system y2 ON y2.fragment_id = f.id AND y2.sys_no = 2
LEFT JOIN net.fragment_system y3 ON y3.fragment_id = f.id AND y3.sys_no = 3
LEFT JOIN net.fragment_system y4 ON y4.fragment_id = f.id AND y4.sys_no = 4
LEFT JOIN net.fragment_system y5 ON y5.fragment_id = f.id AND y5.sys_no = 5;

COMMENT ON VIEW compat.fragments IS
    'Плоский вид фрагмента для gid8: net.fragment + fragment_system '
    '(5 систем) + fragment_month (12 месяцев). Запись раскладывается триггером.';


CREATE OR REPLACE FUNCTION compat.fragments_write() RETURNS trigger
LANGUAGE plpgsql
SET search_path = pg_catalog, net, compat
AS $fn$
DECLARE
    row_data jsonb;
    n integer;
BEGIN
    IF TG_OP = 'DELETE' THEN
        -- В старой БД удаление фрагмента не удаляло объекты сети. В новой
        -- модели внешние ключи защищают их, поэтому повторяем видимое для
        -- пользователя поведение безопасным мягким удалением.
        UPDATE net.fragment
        SET removed_at = COALESCE(removed_at, clock_timestamp())
        WHERE id = OLD.id;
        RETURN OLD;
    END IF;

    IF TG_OP = 'INSERT' THEN
        INSERT INTO net.fragment (
            id, name, name_sys, nasel_point, sezon, "year", removed_at,
            t_or, t_vr, t_vnew, tx, tx_leto, tip_qgvs,
            begin_year, end_year, tn_god, tg_god, tn_god_leto,
            tg_god_leto, a, year_audit, phone_manager, id_old
        ) VALUES (
            COALESCE(NEW.id, nextval('net.fragment_id_seq')),
            COALESCE(NEW.name, ''), NEW.name_sys, NEW.nasel_point,
            NEW.sezon, NEW.year,
            CASE WHEN COALESCE(NEW.removed, 0) <> 0
                 THEN clock_timestamp() ELSE NULL END,
            NEW.t_or, NEW.t_vr, NEW.t_vnew, NEW.tx, NEW.tx_leto,
            NEW.tip_qgvs, NEW.begin_year, NEW.end_year, NEW.tn_god,
            NEW.tg_god, NEW.tn_god_leto, NEW.tg_god_leto, NEW.a,
            NEW.year_audit, NEW.phone_manager, NEW.id_old
        )
        RETURNING id INTO NEW.id;
    ELSE
        UPDATE net.fragment SET
            name = COALESCE(NEW.name, ''),
            name_sys = NEW.name_sys,
            nasel_point = NEW.nasel_point,
            sezon = NEW.sezon,
            "year" = NEW.year,
            removed_at = CASE WHEN COALESCE(NEW.removed, 0) <> 0
                              THEN COALESCE(removed_at, clock_timestamp())
                              ELSE NULL END,
            t_or = NEW.t_or,
            t_vr = NEW.t_vr,
            t_vnew = NEW.t_vnew,
            tx = NEW.tx,
            tx_leto = NEW.tx_leto,
            tip_qgvs = NEW.tip_qgvs,
            begin_year = NEW.begin_year,
            end_year = NEW.end_year,
            tn_god = NEW.tn_god,
            tg_god = NEW.tg_god,
            tn_god_leto = NEW.tn_god_leto,
            tg_god_leto = NEW.tg_god_leto,
            a = NEW.a,
            year_audit = NEW.year_audit,
            phone_manager = NEW.phone_manager,
            id_old = NEW.id_old
        WHERE id = OLD.id;
        NEW.id := OLD.id;
    END IF;

    row_data := to_jsonb(NEW);

    FOR n IN 1..5 LOOP
        INSERT INTO net.fragment_system (
            fragment_id, sys_no, g1n_sys, g2n_sys, gnz_sys,
            q1n_tp, q2n_tp, qn_gv_pot, qn_pot,
            t1n_pot, t1n_sys, t2n_pot, t2n_sys, tvn
        ) VALUES (
            NEW.id, n,
            (row_data ->> ('g1n_sys_' || n))::double precision,
            (row_data ->> ('g2n_sys_' || n))::double precision,
            (row_data ->> ('gnz_sys_' || n))::double precision,
            (row_data ->> ('q1n_tp_' || n))::double precision,
            (row_data ->> ('q2n_tp_' || n))::double precision,
            (row_data ->> ('qn_gv_pot_' || n))::double precision,
            (row_data ->> ('qn_pot_' || n))::double precision,
            (row_data ->> ('t1n_pot_' || n))::double precision,
            (row_data ->> ('t1n_sys_' || n))::double precision,
            (row_data ->> ('t2n_pot_' || n))::double precision,
            (row_data ->> ('t2n_sys_' || n))::double precision,
            (row_data ->> ('tvn_' || n))::double precision
        )
        ON CONFLICT (fragment_id, sys_no) DO UPDATE SET
            g1n_sys = EXCLUDED.g1n_sys,
            g2n_sys = EXCLUDED.g2n_sys,
            gnz_sys = EXCLUDED.gnz_sys,
            q1n_tp = EXCLUDED.q1n_tp,
            q2n_tp = EXCLUDED.q2n_tp,
            qn_gv_pot = EXCLUDED.qn_gv_pot,
            qn_pot = EXCLUDED.qn_pot,
            t1n_pot = EXCLUDED.t1n_pot,
            t1n_sys = EXCLUDED.t1n_sys,
            t2n_pot = EXCLUDED.t2n_pot,
            t2n_sys = EXCLUDED.t2n_sys,
            tvn = EXCLUDED.tvn;
    END LOOP;

    FOR n IN 1..12 LOOP
        INSERT INTO net.fragment_month (
            fragment_id, month, tgr_fakt, tgr_long,
            tn_fakt, tn_long, tpod_fakt, tpod_long
        ) VALUES (
            NEW.id, n,
            (row_data ->> ('tgr_fakt_' || n))::double precision,
            (row_data ->> ('tgr_long_' || n))::double precision,
            (row_data ->> ('tn_fakt_' || n))::double precision,
            (row_data ->> ('tn_long_' || n))::double precision,
            (row_data ->> ('tpod_fakt_' || n))::double precision,
            (row_data ->> ('tpod_long_' || n))::double precision
        )
        ON CONFLICT (fragment_id, month) DO UPDATE SET
            tgr_fakt = EXCLUDED.tgr_fakt,
            tgr_long = EXCLUDED.tgr_long,
            tn_fakt = EXCLUDED.tn_fakt,
            tn_long = EXCLUDED.tn_long,
            tpod_fakt = EXCLUDED.tpod_fakt,
            tpod_long = EXCLUDED.tpod_long;
    END LOOP;

    RETURN NEW;
END $fn$;

DROP TRIGGER IF EXISTS fragments_write ON compat.fragments;
CREATE TRIGGER fragments_write
INSTEAD OF INSERT OR UPDATE OR DELETE ON compat.fragments
FOR EACH ROW EXECUTE FUNCTION compat.fragments_write();

COMMIT;

SELECT count(*) AS strok, count(*) FILTER (WHERE tn_long_1 IS NOT NULL) AS s_mesyacami
FROM compat.fragments;
