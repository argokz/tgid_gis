#include <QString>
#include <QDateTime>
#include <QtSql>

#include <db/db.h>

#include "heatSystem.h"

bool getCT(QSqlDatabase &db, C_heatSystem &d)
{
    QSqlQuery query(db);
    query.setForwardOnly(true);

    bool ret = false;

    QString q = QString("SELECT TOP 1 * FROM heatSystem");

    if (!is_MSSQL()) {
        q = QString("SELECT * FROM heatSystem LIMIT 1");
    }

    if (query_exec(db, query, q)) {
        if  (query.next()) {
            d.name = query.value("name").toString();
            d.nasel_point = query.value("nasel_point").toString();
            d.seasonID = query.value("seasonID").toInt();
            d.year = query.value("year").toString();
            d.t_or = query.value("t_or").toDouble();
            d.t_vr = query.value("t_vr").toDouble();
            d.t_vnew = query.value("t_vnew").toDouble();
            d.tx = query.value("tx").toDouble();
            d.tx_leto = query.value("tx_leto").toDouble();
            d.tip_Qgvs = query.value("tip_Qgvs").toInt();
            d.begin_year = query.value("begin_year").toDateTime();
            d.end_year = query.value("end_year").toDateTime();
            d.tn_god = query.value("tn_god").toDouble();
            d.tg_god = query.value("tg_god").toDouble();
            d.tn_god_leto = query.value("tn_god_leto").toDouble();
            d.tg_god_leto = query.value("tg_god_leto").toDouble();
            d.a = query.value("a").toDouble();
            d.year_audit = query.value("year_audit").toDateTime();
            d.tvn[1-1] = query.value("tvn_1").toDouble();
            d.tvn[2-1] = query.value("tvn_2").toDouble();
            d.tvn[3-1] = query.value("tvn_3").toDouble();
            d.tvn[4-1] = query.value("tvn_4").toDouble();
            d.tvn[5-1] = query.value("tvn_5").toDouble();
            d.G1n_sys[1-1] = query.value("G1n_sys_1").toDouble();
            d.G1n_sys[2-1] = query.value("G1n_sys_2").toDouble();
            d.G1n_sys[3-1] = query.value("G1n_sys_3").toDouble();
            d.G1n_sys[4-1] = query.value("G1n_sys_4").toDouble();
            d.G1n_sys[5-1] = query.value("G1n_sys_5").toDouble();
            d.G2n_sys[1-1] = query.value("G2n_sys_1").toDouble();
            d.G2n_sys[2-1] = query.value("G2n_sys_2").toDouble();
            d.G2n_sys[3-1] = query.value("G2n_sys_3").toDouble();
            d.G2n_sys[4-1] = query.value("G2n_sys_4").toDouble();
            d.G2n_sys[5-1] = query.value("G2n_sys_5").toDouble();
            d.Gnz_sys[1-1] = query.value("Gnz_sys_1").toDouble();
            d.Gnz_sys[2-1] = query.value("Gnz_sys_2").toDouble();
            d.Gnz_sys[3-1] = query.value("Gnz_sys_3").toDouble();
            d.Gnz_sys[4-1] = query.value("Gnz_sys_4").toDouble();
            d.Gnz_sys[5-1] = query.value("Gnz_sys_5").toDouble();
            d.Qn_pot[1-1] = query.value("Qn_pot_1").toDouble();
            d.Qn_pot[2-1] = query.value("Qn_pot_2").toDouble();
            d.Qn_pot[3-1] = query.value("Qn_pot_3").toDouble();
            d.Qn_pot[4-1] = query.value("Qn_pot_4").toDouble();
            d.Qn_pot[5-1] = query.value("Qn_pot_5").toDouble();
            d.Qn_gv_pot[1-1] = query.value("Qn_gv_pot_1").toDouble();
            d.Qn_gv_pot[2-1] = query.value("Qn_gv_pot_2").toDouble();
            d.Qn_gv_pot[3-1] = query.value("Qn_gv_pot_3").toDouble();
            d.Qn_gv_pot[4-1] = query.value("Qn_gv_pot_4").toDouble();
            d.Qn_gv_pot[5-1] = query.value("Qn_gv_pot_5").toDouble();
            d.Q1n_tp[1-1] = query.value("Q1n_tp_1").toDouble();
            d.Q1n_tp[2-1] = query.value("Q1n_tp_2").toDouble();
            d.Q1n_tp[3-1] = query.value("Q1n_tp_3").toDouble();
            d.Q1n_tp[4-1] = query.value("Q1n_tp_4").toDouble();
            d.Q1n_tp[5-1] = query.value("Q1n_tp_5").toDouble();
            d.Q2n_tp[1-1] = query.value("Q2n_tp_1").toDouble();
            d.Q2n_tp[2-1] = query.value("Q2n_tp_2").toDouble();
            d.Q2n_tp[3-1] = query.value("Q2n_tp_3").toDouble();
            d.Q2n_tp[4-1] = query.value("Q2n_tp_4").toDouble();
            d.Q2n_tp[5-1] = query.value("Q2n_tp_5").toDouble();
            d.t1n_pot[1-1] = query.value("t1n_pot_1").toDouble();
            d.t1n_pot[2-1] = query.value("t1n_pot_2").toDouble();
            d.t1n_pot[3-1] = query.value("t1n_pot_3").toDouble();
            d.t1n_pot[4-1] = query.value("t1n_pot_4").toDouble();
            d.t1n_pot[5-1] = query.value("t1n_pot_5").toDouble();
            d.t2n_pot[1-1] = query.value("t2n_pot_1").toDouble();
            d.t2n_pot[2-1] = query.value("t2n_pot_2").toDouble();
            d.t2n_pot[3-1] = query.value("t2n_pot_3").toDouble();
            d.t2n_pot[4-1] = query.value("t2n_pot_4").toDouble();
            d.t2n_pot[5-1] = query.value("t2n_pot_5").toDouble();
            d.t1n_sys[1-1] = query.value("t1n_sys_1").toDouble();
            d.t1n_sys[2-1] = query.value("t1n_sys_2").toDouble();
            d.t1n_sys[3-1] = query.value("t1n_sys_3").toDouble();
            d.t1n_sys[4-1] = query.value("t1n_sys_4").toDouble();
            d.t1n_sys[5-1] = query.value("t1n_sys_5").toDouble();
            d.t2n_sys[1-1] = query.value("t2n_sys_1").toDouble();
            d.t2n_sys[2-1] = query.value("t2n_sys_2").toDouble();
            d.t2n_sys[3-1] = query.value("t2n_sys_3").toDouble();
            d.t2n_sys[4-1] = query.value("t2n_sys_4").toDouble();
            d.t2n_sys[5-1] = query.value("t2n_sys_5").toDouble();
            d.tn_long[1-1] = query.value("tn_long_1").toDouble();
            d.tn_long[2-1] = query.value("tn_long_2").toDouble();
            d.tn_long[3-1] = query.value("tn_long_3").toDouble();
            d.tn_long[4-1] = query.value("tn_long_4").toDouble();
            d.tn_long[5-1] = query.value("tn_long_5").toDouble();
            d.tn_long[6-1] = query.value("tn_long_6").toDouble();
            d.tn_long[7-1] = query.value("tn_long_7").toDouble();
            d.tn_long[8-1] = query.value("tn_long_8").toDouble();
            d.tn_long[9-1] = query.value("tn_long_9").toDouble();
            d.tn_long[10-1] = query.value("tn_long_10").toDouble();
            d.tn_long[11-1] = query.value("tn_long_11").toDouble();
            d.tn_long[12-1] = query.value("tn_long_12").toDouble();
            d.tgr_long[1-1] = query.value("tgr_long_1").toDouble();
            d.tgr_long[2-1] = query.value("tgr_long_2").toDouble();
            d.tgr_long[3-1] = query.value("tgr_long_3").toDouble();
            d.tgr_long[4-1] = query.value("tgr_long_4").toDouble();
            d.tgr_long[5-1] = query.value("tgr_long_5").toDouble();
            d.tgr_long[6-1] = query.value("tgr_long_6").toDouble();
            d.tgr_long[7-1] = query.value("tgr_long_7").toDouble();
            d.tgr_long[8-1] = query.value("tgr_long_8").toDouble();
            d.tgr_long[9-1] = query.value("tgr_long_9").toDouble();
            d.tgr_long[10-1] = query.value("tgr_long_10").toDouble();
            d.tgr_long[11-1] = query.value("tgr_long_11").toDouble();
            d.tgr_long[12-1] = query.value("tgr_long_12").toDouble();
            d.tpod_long[1-1] = query.value("tpod_long_1").toDouble();
            d.tpod_long[2-1] = query.value("tpod_long_2").toDouble();
            d.tpod_long[3-1] = query.value("tpod_long_3").toDouble();
            d.tpod_long[4-1] = query.value("tpod_long_4").toDouble();
            d.tpod_long[5-1] = query.value("tpod_long_5").toDouble();
            d.tpod_long[6-1] = query.value("tpod_long_6").toDouble();
            d.tpod_long[7-1] = query.value("tpod_long_7").toDouble();
            d.tpod_long[8-1] = query.value("tpod_long_8").toDouble();
            d.tpod_long[9-1] = query.value("tpod_long_9").toDouble();
            d.tpod_long[10-1] = query.value("tpod_long_10").toDouble();
            d.tpod_long[11-1] = query.value("tpod_long_11").toDouble();
            d.tpod_long[12-1] = query.value("tpod_long_12").toDouble();
            d.tn_fakt[1-1] = query.value("tn_fakt_1").toDouble();
            d.tn_fakt[2-1] = query.value("tn_fakt_2").toDouble();
            d.tn_fakt[3-1] = query.value("tn_fakt_3").toDouble();
            d.tn_fakt[4-1] = query.value("tn_fakt_4").toDouble();
            d.tn_fakt[5-1] = query.value("tn_fakt_5").toDouble();
            d.tn_fakt[6-1] = query.value("tn_fakt_6").toDouble();
            d.tn_fakt[7-1] = query.value("tn_fakt_7").toDouble();
            d.tn_fakt[8-1] = query.value("tn_fakt_8").toDouble();
            d.tn_fakt[9-1] = query.value("tn_fakt_9").toDouble();
            d.tn_fakt[10-1] = query.value("tn_fakt_10").toDouble();
            d.tn_fakt[11-1] = query.value("tn_fakt_11").toDouble();
            d.tn_fakt[12-1] = query.value("tn_fakt_12").toDouble();
            d.tgr_fakt[1-1] = query.value("tgr_fakt_1").toDouble();
            d.tgr_fakt[2-1] = query.value("tgr_fakt_2").toDouble();
            d.tgr_fakt[3-1] = query.value("tgr_fakt_3").toDouble();
            d.tgr_fakt[4-1] = query.value("tgr_fakt_4").toDouble();
            d.tgr_fakt[5-1] = query.value("tgr_fakt_5").toDouble();
            d.tgr_fakt[6-1] = query.value("tgr_fakt_6").toDouble();
            d.tgr_fakt[7-1] = query.value("tgr_fakt_7").toDouble();
            d.tgr_fakt[8-1] = query.value("tgr_fakt_8").toDouble();
            d.tgr_fakt[9-1] = query.value("tgr_fakt_9").toDouble();
            d.tgr_fakt[10-1] = query.value("tgr_fakt_10").toDouble();
            d.tgr_fakt[11-1] = query.value("tgr_fakt_11").toDouble();
            d.tgr_fakt[12-1] = query.value("tgr_fakt_12").toDouble();
            d.tpod_fakt[1-1] = query.value("tpod_fakt_1").toDouble();
            d.tpod_fakt[2-1] = query.value("tpod_fakt_2").toDouble();
            d.tpod_fakt[3-1] = query.value("tpod_fakt_3").toDouble();
            d.tpod_fakt[4-1] = query.value("tpod_fakt_4").toDouble();
            d.tpod_fakt[5-1] = query.value("tpod_fakt_5").toDouble();
            d.tpod_fakt[6-1] = query.value("tpod_fakt_6").toDouble();
            d.tpod_fakt[7-1] = query.value("tpod_fakt_7").toDouble();
            d.tpod_fakt[8-1] = query.value("tpod_fakt_8").toDouble();
            d.tpod_fakt[9-1] = query.value("tpod_fakt_9").toDouble();
            d.tpod_fakt[10-1] = query.value("tpod_fakt_10").toDouble();
            d.tpod_fakt[11-1] = query.value("tpod_fakt_11").toDouble();
            d.tpod_fakt[12-1] = query.value("tpod_fakt_12").toDouble();
            d.phone_manager = query.value("phone_manager").toString();
            d.removed = query.value("removed").toInt();
            d.idRemoved = query.value("idRemoved").toInt();
            d.Central_Meridian = query.value("Central_Meridian").toDouble();
            d.Latitude_Of_Origin = query.value("Latitude_Of_Origin").toDouble();
            d.False_Easting = query.value("False_Easting").toInt();
            d.False_Northing = query.value("False_Northing").toInt();
            d.Scale_Factor = query.value("Scale_Factor").toDouble();
            d.Angle = query.value("Angle").toDouble();
            d.dx = query.value("dx").toDouble();
            d.dy = query.value("dy").toDouble();
            ret = true;
        }
    }
    return ret;
}
