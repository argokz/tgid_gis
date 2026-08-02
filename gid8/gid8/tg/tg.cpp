#include <QSqlQuery>
#include <map>

#include <db/db.h>

#include "tg.h"



static std::map<long, std::map<int, TG> > map_TG_n;
static std::map<long, std::map<double, TG> > map_TG;
static std::map<long, TLeto > map_TG_leto;

bool readTG(QSqlDatabase &db, QSqlQuery &query)
{
    map_TG_leto.clear();
    QString q = "SELECT id, temperDWflowSummer, temperDWretSummer FROM heatSources";

    if (query_exec(db, query, q)) {
        if (query.next()) {
            int id = query.value(0).toInt();

            TLeto tl;

            tl.t1 = query.value("temperDWflowSummer").toDouble();
            tl.t2 = query.value("temperDWretSummer").toDouble();
            tl.tn1 = -30;
            tl.tn2 = 8;

            map_TG_leto[id] = tl;
        }
    }

    q = "SELECT * FROM deployedTempGraphs ORDER BY hSourceID, tn desc";

    map_TG.clear();
    map_TG_n.clear();
    
    long hSourceIDold = -1;
    long hSourceID_old = -1;

    if (query_exec(db, query, q)) {
        int i = 0;
        while (query.next()) {
            TG tg;
            int id = query.value("id").toInt();
            int hSourceID = query.value("hSourceID").toInt();

            if (hSourceID != hSourceID_old) {
                i = 0;
                hSourceID_old = hSourceID;
            }

            tg.tn = query.value("tn").toDouble();
            tg.Q_otn = query.value("Q_otn").toDouble();
            tg.t1 = query.value("t1").toDouble();
            tg.t2 = query.value("t2").toDouble();
            tg.t3 = query.value("t3").toDouble();
            tg.tv = query.value("tv").toDouble();
            tg.t_bn = query.value("t_bn").toDouble();
            tg.tg = query.value("tg").toDouble();
            
            if (hSourceID != hSourceIDold) {
                map_TG_leto[hSourceID].tn1 = tg.tn;
                hSourceIDold = hSourceID;
            }
            map_TG_leto[hSourceID].tn2 = tg.tn;
            map_TG[hSourceID][tg.tn] = tg;
            map_TG_n[hSourceID][i++] = tg;
        }
    }
    return map_TG.size() > 0;
}

bool getTG_tn1_tn2(long hSourceID, double &tn1, double &tn2)
{
    tn1 = -32, tn2 = 8;
    std::map<long, TLeto >::const_iterator it = map_TG_leto.find(hSourceID);
    if (it != map_TG_leto.end()) {
      tn1 = it->second.tn1;
      tn2 = it->second.tn2;
      return true;
    }
    return false;
}

bool getTG(long hSourceID, double tn, double &t1, double &t2, double &t3, double &tv)
{

    double tn1, tn2;
    getTG_tn1_tn2(hSourceID, tn1, tn2);


    if (tn > tn2+0.001 && tn > tn1 + 0.001) {
      std::map<long, TLeto >::const_iterator it = map_TG_leto.find(hSourceID);
      
      t1 = t2 = t3 = tv = it->second.t1;
      return true;
    }


    std::map<long, std::map<double, TG> >::const_iterator it = map_TG.find(hSourceID);

    double t1_0 = 0;
    double t2_0 = 0;
    double t3_0 = 0;
    double tv_0 = 0;
    double tn0 = 0;


    if (it != map_TG.end()) {
        std::map<double, TG>::const_iterator it2 = it->second.begin();
        
        for (; it2 != it->second.end(); it2++) {
            double tn1 = it2->first;

            double t1_1 = it2->second.t1;
            double t2_1 = it2->second.t2;
            double t3_1 = it2->second.t3;
            double tv_1 = it2->second.tv;

            if (tn <= tn1) {
                it2++;
                if (it2 != it->second.end()) {
                    double t1_2 = it2->second.t1;
                    double t2_2 = it2->second.t2;
                    double t3_2 = it2->second.t3;
                    double tv_2 = it2->second.tv;

                    t1 = t1_1 + (t1_2 - t1_1)*(tn - tn1);
                    t2 = t2_1 + (t2_2 - t2_1)*(tn - tn1);
                    t3 = t3_1 + (t3_2 - t3_1)*(tn - tn1);
                    tv = tv_1 + (tv_2 - tv_1)*(tn - tn1);
                }
                else {
                    t1 = t1_1 + (t1_0 - t1_1)*(tn - tn0);
                    t2 = t2_1 + (t2_0 - t2_1)*(tn - tn0);
                    t3 = t3_1 + (t3_0 - t3_1)*(tn - tn0);
                    tv = tv_1 + (tv_0 - tv_1)*(tn - tn0);
                }
                return true;
            }

            t1_0 = t1_1;
            t2_0 = t2_1;
            t3_0 = t3_1;
            tv_0 = tv_1;
            tn0 = tn;
        }
    }
    return false;
}


// Количество точек в ТГ

int getTG_n_col(long hSourceID)
{
    auto it = map_TG_n.find(hSourceID);
    if (it != map_TG_n.end()) {
        return it->second.size();
    }
    return 0;
}



bool getTG_n(long hSourceID, int n, double &tn, double &t1, double &t2, double &t3, double &tv)
{
    tn = t1 = t2 = t3 = tv = 0;
    
    auto it = map_TG_n.find(hSourceID);
    if (it != map_TG_n.end()) {
        auto it2 = it->second.find(n);
        if (it2 != it->second.end()) {
            tn = it2->second.tn;
            t1 = it2->second.t1;
            t2 = it2->second.t2;
            t3 = it2->second.t3;
            tv = it2->second.tv;
            return true;
        }
    }
    return false;
}
