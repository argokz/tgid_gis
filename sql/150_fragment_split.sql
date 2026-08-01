-- Фрагмент как объект: три таблицы вместо 156 колонок.
--
-- СГЕНЕРИРОВАНО tools/gen_fragment_split.py по фактическому
-- составу public.fragments. Руками не правят: опечатка в имени
-- вроде tpod_fakt_11 молча потеряла бы столбец данных.
--
-- Обоснование разложения — docs/15-fragment-and-subsystems.md.
-- Коротко: 132 из 156 колонок это массивы, выписанные плоско,
-- а net.fragment не имеет геометрии и слоем карты не является.
--
-- Откат: sql/151_fragment_split_rollback.sql.

BEGIN;

-- 1. Недостающие скалярные поля паспорта фрагмента.
ALTER TABLE net.fragment ADD COLUMN IF NOT EXISTS t_or               double precision;
ALTER TABLE net.fragment ADD COLUMN IF NOT EXISTS t_vr               double precision;
ALTER TABLE net.fragment ADD COLUMN IF NOT EXISTS t_vnew             double precision;
ALTER TABLE net.fragment ADD COLUMN IF NOT EXISTS tx                 double precision;
ALTER TABLE net.fragment ADD COLUMN IF NOT EXISTS tx_leto            double precision;
ALTER TABLE net.fragment ADD COLUMN IF NOT EXISTS tip_qgvs           text;
ALTER TABLE net.fragment ADD COLUMN IF NOT EXISTS begin_year         date;
ALTER TABLE net.fragment ADD COLUMN IF NOT EXISTS end_year           date;
ALTER TABLE net.fragment ADD COLUMN IF NOT EXISTS tn_god             double precision;
ALTER TABLE net.fragment ADD COLUMN IF NOT EXISTS tg_god             double precision;
ALTER TABLE net.fragment ADD COLUMN IF NOT EXISTS tn_god_leto        double precision;
ALTER TABLE net.fragment ADD COLUMN IF NOT EXISTS tg_god_leto        double precision;
ALTER TABLE net.fragment ADD COLUMN IF NOT EXISTS a                  double precision;
ALTER TABLE net.fragment ADD COLUMN IF NOT EXISTS year_audit         date;
ALTER TABLE net.fragment ADD COLUMN IF NOT EXISTS phone_manager      text;
ALTER TABLE net.fragment ADD COLUMN IF NOT EXISTS id_old             integer;


-- Параметры по системам теплоснабжения: 5 строк вместо 60 колонок.
CREATE TABLE IF NOT EXISTS net.fragment_system (
    fragment_id  integer NOT NULL REFERENCES net.fragment(id) ON DELETE CASCADE,
    sys_no       smallint NOT NULL CHECK (sys_no BETWEEN 1 AND 5),
    g1n_sys      double precision,
    g2n_sys      double precision,
    gnz_sys      double precision,
    q1n_tp       double precision,
    q2n_tp       double precision,
    qn_gv_pot    double precision,
    qn_pot       double precision,
    t1n_pot      double precision,
    t1n_sys      double precision,
    t2n_pot      double precision,
    t2n_sys      double precision,
    tvn          double precision,
    PRIMARY KEY (fragment_id, sys_no)
);

-- Температуры по месяцам: 12 строк вместо 72 колонок.
CREATE TABLE IF NOT EXISTS net.fragment_month (
    fragment_id  integer NOT NULL REFERENCES net.fragment(id) ON DELETE CASCADE,
    month        smallint NOT NULL CHECK (month BETWEEN 1 AND 12),
    tgr_fakt     double precision,
    tgr_long     double precision,
    tn_fakt      double precision,
    tn_long      double precision,
    tpod_fakt    double precision,
    tpod_long    double precision,
    PRIMARY KEY (fragment_id, month)
);

-- 2. Перенос данных из public.fragments.

UPDATE net.fragment f SET
    t_or = s.t_or,
    t_vr = s.t_vr,
    t_vnew = s.t_vnew,
    tx = s.tx,
    tx_leto = s.tx_leto,
    tip_qgvs = s.tip_qgvs,
    begin_year = s.begin_year,
    end_year = s.end_year,
    tn_god = s.tn_god,
    tg_god = s.tg_god,
    tn_god_leto = s.tn_god_leto,
    tg_god_leto = s.tg_god_leto,
    a = s.a,
    year_audit = s.year_audit,
    phone_manager = s.phone_manager,
    id_old = s.id_old
FROM public.fragments s
WHERE s.id = f.id;

INSERT INTO net.fragment_system (fragment_id, sys_no, g1n_sys, g2n_sys, gnz_sys, q1n_tp, q2n_tp, qn_gv_pot, qn_pot, t1n_pot, t1n_sys, t2n_pot, t2n_sys, tvn)
SELECT f.id, v.sys_no,
    CASE v.sys_no WHEN 1 THEN s.g1n_sys_1 WHEN 2 THEN s.g1n_sys_2 WHEN 3 THEN s.g1n_sys_3 WHEN 4 THEN s.g1n_sys_4 WHEN 5 THEN s.g1n_sys_5 END AS g1n_sys,
    CASE v.sys_no WHEN 1 THEN s.g2n_sys_1 WHEN 2 THEN s.g2n_sys_2 WHEN 3 THEN s.g2n_sys_3 WHEN 4 THEN s.g2n_sys_4 WHEN 5 THEN s.g2n_sys_5 END AS g2n_sys,
    CASE v.sys_no WHEN 1 THEN s.gnz_sys_1 WHEN 2 THEN s.gnz_sys_2 WHEN 3 THEN s.gnz_sys_3 WHEN 4 THEN s.gnz_sys_4 WHEN 5 THEN s.gnz_sys_5 END AS gnz_sys,
    CASE v.sys_no WHEN 1 THEN s.q1n_tp_1 WHEN 2 THEN s.q1n_tp_2 WHEN 3 THEN s.q1n_tp_3 WHEN 4 THEN s.q1n_tp_4 WHEN 5 THEN s.q1n_tp_5 END AS q1n_tp,
    CASE v.sys_no WHEN 1 THEN s.q2n_tp_1 WHEN 2 THEN s.q2n_tp_2 WHEN 3 THEN s.q2n_tp_3 WHEN 4 THEN s.q2n_tp_4 WHEN 5 THEN s.q2n_tp_5 END AS q2n_tp,
    CASE v.sys_no WHEN 1 THEN s.qn_gv_pot_1 WHEN 2 THEN s.qn_gv_pot_2 WHEN 3 THEN s.qn_gv_pot_3 WHEN 4 THEN s.qn_gv_pot_4 WHEN 5 THEN s.qn_gv_pot_5 END AS qn_gv_pot,
    CASE v.sys_no WHEN 1 THEN s.qn_pot_1 WHEN 2 THEN s.qn_pot_2 WHEN 3 THEN s.qn_pot_3 WHEN 4 THEN s.qn_pot_4 WHEN 5 THEN s.qn_pot_5 END AS qn_pot,
    CASE v.sys_no WHEN 1 THEN s.t1n_pot_1 WHEN 2 THEN s.t1n_pot_2 WHEN 3 THEN s.t1n_pot_3 WHEN 4 THEN s.t1n_pot_4 WHEN 5 THEN s.t1n_pot_5 END AS t1n_pot,
    CASE v.sys_no WHEN 1 THEN s.t1n_sys_1 WHEN 2 THEN s.t1n_sys_2 WHEN 3 THEN s.t1n_sys_3 WHEN 4 THEN s.t1n_sys_4 WHEN 5 THEN s.t1n_sys_5 END AS t1n_sys,
    CASE v.sys_no WHEN 1 THEN s.t2n_pot_1 WHEN 2 THEN s.t2n_pot_2 WHEN 3 THEN s.t2n_pot_3 WHEN 4 THEN s.t2n_pot_4 WHEN 5 THEN s.t2n_pot_5 END AS t2n_pot,
    CASE v.sys_no WHEN 1 THEN s.t2n_sys_1 WHEN 2 THEN s.t2n_sys_2 WHEN 3 THEN s.t2n_sys_3 WHEN 4 THEN s.t2n_sys_4 WHEN 5 THEN s.t2n_sys_5 END AS t2n_sys,
    CASE v.sys_no WHEN 1 THEN s.tvn_1 WHEN 2 THEN s.tvn_2 WHEN 3 THEN s.tvn_3 WHEN 4 THEN s.tvn_4 WHEN 5 THEN s.tvn_5 END AS tvn
FROM public.fragments s
JOIN net.fragment f ON f.id = s.id
CROSS JOIN generate_series(1, 5) AS v(sys_no)
ON CONFLICT (fragment_id, sys_no) DO NOTHING;

INSERT INTO net.fragment_month (fragment_id, month, tgr_fakt, tgr_long, tn_fakt, tn_long, tpod_fakt, tpod_long)
SELECT f.id, v.month,
    CASE v.month WHEN 1 THEN s.tgr_fakt_1 WHEN 2 THEN s.tgr_fakt_2 WHEN 3 THEN s.tgr_fakt_3 WHEN 4 THEN s.tgr_fakt_4 WHEN 5 THEN s.tgr_fakt_5 WHEN 6 THEN s.tgr_fakt_6 WHEN 7 THEN s.tgr_fakt_7 WHEN 8 THEN s.tgr_fakt_8 WHEN 9 THEN s.tgr_fakt_9 WHEN 10 THEN s.tgr_fakt_10 WHEN 11 THEN s.tgr_fakt_11 WHEN 12 THEN s.tgr_fakt_12 END AS tgr_fakt,
    CASE v.month WHEN 1 THEN s.tgr_long_1 WHEN 2 THEN s.tgr_long_2 WHEN 3 THEN s.tgr_long_3 WHEN 4 THEN s.tgr_long_4 WHEN 5 THEN s.tgr_long_5 WHEN 6 THEN s.tgr_long_6 WHEN 7 THEN s.tgr_long_7 WHEN 8 THEN s.tgr_long_8 WHEN 9 THEN s.tgr_long_9 WHEN 10 THEN s.tgr_long_10 WHEN 11 THEN s.tgr_long_11 WHEN 12 THEN s.tgr_long_12 END AS tgr_long,
    CASE v.month WHEN 1 THEN s.tn_fakt_1 WHEN 2 THEN s.tn_fakt_2 WHEN 3 THEN s.tn_fakt_3 WHEN 4 THEN s.tn_fakt_4 WHEN 5 THEN s.tn_fakt_5 WHEN 6 THEN s.tn_fakt_6 WHEN 7 THEN s.tn_fakt_7 WHEN 8 THEN s.tn_fakt_8 WHEN 9 THEN s.tn_fakt_9 WHEN 10 THEN s.tn_fakt_10 WHEN 11 THEN s.tn_fakt_11 WHEN 12 THEN s.tn_fakt_12 END AS tn_fakt,
    CASE v.month WHEN 1 THEN s.tn_long_1 WHEN 2 THEN s.tn_long_2 WHEN 3 THEN s.tn_long_3 WHEN 4 THEN s.tn_long_4 WHEN 5 THEN s.tn_long_5 WHEN 6 THEN s.tn_long_6 WHEN 7 THEN s.tn_long_7 WHEN 8 THEN s.tn_long_8 WHEN 9 THEN s.tn_long_9 WHEN 10 THEN s.tn_long_10 WHEN 11 THEN s.tn_long_11 WHEN 12 THEN s.tn_long_12 END AS tn_long,
    CASE v.month WHEN 1 THEN s.tpod_fakt_1 WHEN 2 THEN s.tpod_fakt_2 WHEN 3 THEN s.tpod_fakt_3 WHEN 4 THEN s.tpod_fakt_4 WHEN 5 THEN s.tpod_fakt_5 WHEN 6 THEN s.tpod_fakt_6 WHEN 7 THEN s.tpod_fakt_7 WHEN 8 THEN s.tpod_fakt_8 WHEN 9 THEN s.tpod_fakt_9 WHEN 10 THEN s.tpod_fakt_10 WHEN 11 THEN s.tpod_fakt_11 WHEN 12 THEN s.tpod_fakt_12 END AS tpod_fakt,
    CASE v.month WHEN 1 THEN s.tpod_long_1 WHEN 2 THEN s.tpod_long_2 WHEN 3 THEN s.tpod_long_3 WHEN 4 THEN s.tpod_long_4 WHEN 5 THEN s.tpod_long_5 WHEN 6 THEN s.tpod_long_6 WHEN 7 THEN s.tpod_long_7 WHEN 8 THEN s.tpod_long_8 WHEN 9 THEN s.tpod_long_9 WHEN 10 THEN s.tpod_long_10 WHEN 11 THEN s.tpod_long_11 WHEN 12 THEN s.tpod_long_12 END AS tpod_long
FROM public.fragments s
JOIN net.fragment f ON f.id = s.id
CROSS JOIN generate_series(1, 12) AS v(month)
ON CONFLICT (fragment_id, month) DO NOTHING;

COMMIT;

-- Проверка: строк-потомков должно быть ровно по числу фрагментов.
SELECT (SELECT count(*) FROM net.fragment)          AS fragmentov,
       (SELECT count(*) FROM net.fragment_system)   AS strok_sistem,
       (SELECT count(*) FROM net.fragment_month)    AS strok_mesyacev;
