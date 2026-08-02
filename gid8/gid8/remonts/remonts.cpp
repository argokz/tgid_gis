#include <QtGui>
#include <set>
#include <mystd.h>

QString set_to_text(std::set<int>& set_i)
{
    QString ss, s = "";

    for (auto& it : set_i) {
        if (s != "") s += ",";
        ss = QString("%1").arg(it);
        s += ss;
    }
    return s;
}

void set_ms_rs_condition(QString& qq, std::set<int>& m_set_ms, std::set<int>& m_set_rs)
{
    QString ss, ms_rs_condition = "";

    if (m_set_ms.size() > 0 || m_set_rs.size() > 0) {
        ms_rs_condition += "AND (";

        if (m_set_ms.size() > 0) {
            QString s1 = set_to_text(m_set_ms);
            ss = QString(" hpss.magistralSite IN (%1)").arg(s1);
            ms_rs_condition += ss;
        }
        if (m_set_rs.size() > 0) {
            if (m_set_ms.size() > 0) {
                ms_rs_condition += " OR ";
            }

            QString s1 = set_to_text(m_set_rs);
            ss = QString(" hpss.distSite IN (%1)").arg(s1);
            ms_rs_condition += ss;
        }
        ms_rs_condition += ")";
    }
    qq.replace("$and_condition2$", ms_rs_condition);
}

void sezon_condition(QString &q, int m_sezon_korrozia, QDateTime m_sezon_date1, QDateTime m_sezon_date2)
{
    QString season_condition = " IS NULL OR 1=1";

    QString ds1 = m_sezon_date1.toString("yyyyMMdd");
    QString ds2 = m_sezon_date2.toString("yyyyMMdd");

    if (m_sezon_korrozia > 0) {
        season_condition = QString(" BETWEEN '%1' AND '%2' ").arg(ds1, ds2);
    }

    q.replace("$season_condition$", season_condition);
}

QString table_to_q_for_remont(QString fn)
{
//    QString qq = readQ(m_cxema->m_ado, fn, "", m_cxema->m_par);
    QString qq = readFile(fn);
    std::set<int> m_set_ms;
    std::set<int> m_set_rs;
    int m_sezon_korrozia = 2023;

    QDateTime m_sezon_date1;
    QDateTime m_sezon_date2;

    set_ms_rs_condition(qq, m_set_ms, m_set_rs);
    sezon_condition(qq, m_sezon_korrozia, m_sezon_date1, m_sezon_date2);

    return qq;
}
