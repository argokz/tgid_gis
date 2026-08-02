#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
//#include "DbTable.h"
#include "InputDia.h"
#include "Input2.h"
#include "gidrView.h"


bool is_water_node(const char* tn);

bool CGidrView::createWaterLine(Klassif* m_kls, CFPoint point)
{
    char fn[1024];
    void get_water_node(set<CString>& _set_water_node);
    set<CString> set_water_node;
    get_water_node(set_water_node);

    int n = m_geofile->findXY_list(point, D5 * geom.masx, 1, set_water_node, fn);
    if (n) {
        Klassif* kls1 = m_geofile->m_kl_list->findKlN(fn);
        if (kls1) {
            int id = kls1->GetNom2(n);

            if (id >= 1) {
                CCoordList clL = kls1->geo4[id - 1]->cl;
                point = clL.first();
                m_NP.push_back(point);
                if (m_geofile) {
                    nn = m_geofile->createObj(m_kls->nazv, m_NP, m_map_geo_value);
                }
                regim = R_INIT;
                m_nRegim = ID_INFO;
                m_NP.clear();
                GetDocument()->SetModifiedFlag(true);
                InvalidateG();
                if (nn) GeoInfo(m_kls->nazv, m_kls->geo4[nn - 1]->nom, true);
                return true;
            }
        }
    }
    return false;
}



bool CGidrView::updateWaterLine(Klassif* m_kls)
{
    CFPoint p1 = m_NP.first();
    CFPoint p2 = m_NP.last();

    void get_water_node(set<CString>& _set_water_node);
    set<CString> set_water_node;
    get_water_node(set_water_node);

    char fn[1024];

    int n1 = m_geofile->findXY_list(p1, D5 * geom.masx, 1, set_water_node, fn);
    if (n1) {
        Klassif* kls1 = m_geofile->m_kl_list->findKlN(fn);
        if (kls1) {
            int id = kls1->GetNom2(n1);
            if (id >= 1) {
                CCoordList clL = kls1->geo4[id - 1]->cl;
                CFPoint p = clL.first();

                m_NP.pop_front();
                m_NP.push_front(p);
            }
        }
    }

    int n2 = m_geofile->findXY_list(p2, D5 * geom.masx, 1, set_water_node, fn);
    if (n2) {
        Klassif* kls1 = m_geofile->m_kl_list->findKlN(fn);
        if (kls1) {
            int id = kls1->GetNom2(n2);
            if (id >= 1) {
                CCoordList clL = kls1->geo4[id - 1]->cl;
                CFPoint p = clL.first();

                m_NP.pop_back();
                m_NP.push_back(p);
            }
        }
    }
    return true;
}


bool CGidrView::updateWaterNode(Klassif* m_kls, CFPoint point)
{
    if (!is_water_node(m_kls->nazv)) return false;

    void get_water_line(set<CString>&_set_water_line);
    set<CString> set_water_line;
    map<CString, set<int> > set_out;
    get_water_line(set_water_line);

    CFPoint p_old = m_NP.first();

    bool mv = false;

    bool b = m_geofile->findXY_end_list_exact(p_old, set_water_line, set_out);
    if (b) {
        for (auto& it : set_out) {
            CString tn = it.first;
            CString q;

            Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
            if (kls) {
                set<int> set1;
                set<int> set2;

                for (auto& i : it.second) {
                    CCoordList cl;
                    kls->geo4[i - 1]->cl;
                    CFPoint p1 = kls->geo4[i - 1]->cl.first();
                    CFPoint p2 = kls->geo4[i - 1]->cl.last();
                    if (Length(p1, p_old) < 1e-5) {
                        kls->geo4[i - 1]->cl.pop_front();
                        kls->geo4[i - 1]->cl.push_front(point);
                        set1.insert(kls->geo4[i - 1]->nom);
                    }
                    if (Length(p2, p_old) < 1e-5) {
                        kls->geo4[i - 1]->cl.pop_back();
                        kls->geo4[i - 1]->cl.push_back(point);
                        set2.insert(kls->geo4[i - 1]->nom);
                    }

                    kls->geo4[i - 1]->rect = kls->geo4[i - 1]->cl.GetFRect();
                }
                CString set_to_text(set<int>&set_i);

                if (!set1.empty()) {
                    q.Format("UPDATE %s SET shape=dbo.insert_point_to_line(shape, geometry::STPointFromText('POINT(%.2lf %.2lf)', 0), 1) WHERE id IN (%s)", tn, point.x / 100, -point.y / 100, set_to_text(set1));
                    kls->m_ado->Execute(q);
                    mv = true;
                }

                if (!set2.empty()) {
                    q.Format("UPDATE %s SET shape=dbo.insert_point_to_line(shape, geometry::STPointFromText('POINT(%.2lf %.2lf)', 0), -1) WHERE id IN (%s)", tn, point.x / 100, -point.y / 100, set_to_text(set2));
                    kls->m_ado->Execute(q);
                    mv = true;
                }
            }
        }
    }

    m_NP.clear();
    m_NP.push_back(point);

    m_nRegim = ID_ISM_GEO;

    updateObj();

    m_NP.clear();
    m_nRegim = ID_INFO;

    GetDocument()->SetModifiedFlag(true);
    InvalidateG();

    return true;
}
