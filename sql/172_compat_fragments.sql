-- Представление fragments для gid8: три таблицы обратно в одну строку.
--
-- Фрагмент разложен на net.fragment (паспорт), net.fragment_system
-- (параметры по пяти системам) и net.fragment_month (температуры по
-- двенадцати месяцам) — обоснование в docs/15. Приложение gid8
-- обращается к плоской fragments из 109 файлов, поэтому здесь она
-- собирается обратно.
--
-- Представление только для чтения: обратная сборка 132 колонок из
-- строк дочерних таблиц однозначно не разбирается, а править параметры
-- фрагмента из gid8 не требуется.

BEGIN;

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
    '(5 систем) + fragment_month (12 месяцев). Только чтение.';

COMMIT;

SELECT count(*) AS strok, count(*) FILTER (WHERE tn_long_1 IS NOT NULL) AS s_mesyacami
FROM compat.fragments;
