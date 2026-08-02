#include "stdafx.h"
#include "ado.h"


struct TG {
  double tn;
  double Q_otn;
  double t1;
  double t2;
  double t3;
  double tv;
  double t_bn;
  double tg;
};

struct TLeto {
  double t1, t2;
  double tn1, tn2;

};

static map<long, map<int, TG> > map_TG_n;
static map<long, map<double, TG> > map_TG;
static map<long, TLeto > map_TG_leto;


bool readTG(CAdoFile *ado)
{
    CString q;

    map_TG_leto.clear();
    q.Format("SELECT id, temperDWflowSummer, temperDWretSummer FROM heatSources");

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long(0);
            TLeto tl;

            tl.t1 = ado->read_double("temperDWflowSummer");
            tl.t2 = ado->read_double("temperDWretSummer");
            tl.tn1 = -30;
            tl.tn2 = 8;

            map_TG_leto[id] = tl;

            ado->MoveNext();
        }
        ado->closeTable();
    }

    q = "SELECT * FROM deployedTempGraphs ORDER BY hSourceID, tn desc";

    map_TG.clear();
    map_TG_n.clear();
    
    long hSourceIDold = -1;
    long hSourceID_old = -1;

    if (ado->openTable0(q)) {
        int i = 0;
        while (!ado->isEOF()) {
            TG tg;
            long id = ado->read_long("id");
            long hSourceID = ado->read_long("hSourceID");

            if (hSourceID != hSourceID_old) {
                i = 0;
                hSourceID_old = hSourceID;
            }


            tg.tn = ado->read_double("tn");
            tg.Q_otn = ado->read_double("Q_otn");
            tg.t1 = ado->read_double("t1");
            tg.t2 = ado->read_double("t2");
            tg.t3 = ado->read_double("t3");
            tg.tv = ado->read_double("tv");
            tg.t_bn = ado->read_double("t_bn");
            tg.tg = ado->read_double("tg");
            
            if (hSourceID != hSourceIDold) {
                map_TG_leto[hSourceID].tn1 = tg.tn;
                hSourceIDold = hSourceID;
            }
            map_TG_leto[hSourceID].tn2 = tg.tn;
            map_TG[hSourceID][tg.tn] = tg;
            map_TG_n[hSourceID][i++] = tg;

            ado->MoveNext();
        }
        ado->closeTable();
    }
    return map_TG.size() > 0;
}

bool getTG_tn1_tn2(long hSourceID, double &tn1, double &tn2)
{
    tn1 = -32, tn2 = 8;
    map<long, TLeto >::const_iterator it = map_TG_leto.find(hSourceID);
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
      map<long, TLeto >::const_iterator it = map_TG_leto.find(hSourceID);
      
      t1 = t2 = t3 = tv = it->second.t1;
      return true;
    }


    map<long, map<double, TG> >::const_iterator it = map_TG.find(hSourceID);

    double t1_0 = 0;
    double t2_0 = 0;
    double t3_0 = 0;
    double tv_0 = 0;
    double tn0 = 0;


    if (it != map_TG.end()) {
        map<double, TG>::const_iterator it2 = it->second.begin();
        
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
