#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
//#include "DbTable.h"
#include "InputDia.h"
#include "Input2.h"
#include "gidrView.h"

#include "ado.h"
#include "win.h"

#include "jsmn.h"

#include "electro.h"

#include "PropertyGrid\test\PropGridDlg.h"
#include "Ultimate Grid\TableDialog.h"

bool is_water_line(const char* tn);
bool is_water_node(const char* tn);

void get_google_coord(CFPoint pt, double& x, double& y);

CNode2* find_next(CGraph2* graph, CNode2* n1, CNode2* n2, CNode2* n0);

bool is_onlypts(const char* tn);
bool is_onlyline(const char* tn);
CString getOnlygeo(const char* tn);

int isElPoint(const char* tn1);
CString Elevation(CFPoint pt);
void OsmInfo(CFPoint pt);


CFPoint GetProjection(CFPoint p, CFPoint p1, CFPoint p2);
void getProject(CLINE2* line, CFPoint point, CCoordList& m_NP, int loc);
bool isOpres(CLINE2* line, int m_id_opr);

static int n_EL_IST = -1;
static int n_EL_PRIEM = -1;



double tri_h(double xx, double yy, int n, const double *x, const double *y, const double *h)
{
    double total_weight = 0;
    double interpolated_height = 0;

//    for point in known_points :
    for (int i = 0; i < n; i++) {
        double dist = hypot(xx - x[i], yy - y[i]);

        if (dist == 0) {
            return h[i];
        }
//            distance((x, y), (point[0], point[1]));
        double weight = 1. / dist; // # Простейший вариант веса, можно уточнить по необходимости
        total_weight += weight;
        interpolated_height += weight * h[i] ;
    }

    interpolated_height /= total_weight;

    return interpolated_height;
}


int get_4_point(CGeoList  &geo4, CFPoint pt, int *min_i_list, bool check_angle)
{
    int n = geo4.size();
    double min_dist_old = 0;
    
    int kk = 0;

    for (int k = 0; k < 4; k++) {
        double min_dist = 1e10;
        int min_i = 0;
        min_i_list[k] = -1;

        for (int i = 0; i < n; i++) {
            CFPoint pt2 = geo4[i]->rect.TopLeft();

            double h = atof(geo4[i]->text00000);

            if (geo4[i]->text00000 != "" && h != 0) {
                double dist = Length(pt, pt2);

                if (dist < min_dist && dist > min_dist_old) {

                    double alpha = atan2(pt2.x - pt.x, pt2.y - pt.y);

                    bool yes = true;

                    if (check_angle) {
                        for (int j = 0; j < k; j++) {
                            CFPoint pt3 = geo4[min_i_list[j]]->rect.TopLeft();
                            double beta = atan2(pt3.x - pt.x, pt3.y - pt.y);

                            if (fabs(beta - alpha) < 1.) {
                                yes = false;
                            }
                        }
                    }

                    if (yes) {
                        min_dist = dist;
                        min_i = geo4[i]->nom;
                    }
                }
            }
        }
        min_i_list[k] = min_i;
        kk = k;

        min_dist_old = min_dist;
    }
    return kk;
}




CString CGidrView::Elevation2(CFPoint pt)
{
    Klassif *kls = m_geofile->m_kl_list->findKlN("gps");

    if (!kls) return false;

//    int n = kls->geo4.size();

/*


    int min_i_list[10];

    double min_dist_old = 0;

    for (int k = 0; k < 4; k++) {
        double min_dist = 1e10;
        int min_i = 0;
        
        for (int i = 0; i < n; i++) {
            CFPoint pt2 = kls->geo4[i]->rect.TopLeft();

            double h = atof(kls->geo4[i]->text00000);

            if (kls->geo4[i]->text00000 != "" && h != 0) {
                double dist = Length(pt, pt2);

                if (dist < min_dist && dist > min_dist_old) {
                    min_dist = dist;
                    min_i = kls->geo4[i]->nom;
                }
            }
        }
        min_i_list[k] = min_i;
        min_dist_old = min_dist;
    }
*/

    int min_i_list[10];

    int kk = get_4_point(kls->geo4, pt, min_i_list, true);

    if (kk < 4) {
        kk = get_4_point(kls->geo4, pt, min_i_list, false);
    }

    if (kk > 4) {
        kk = 4;
    }

    CString ss = "";
    CString s, q;

    for (int i = 0; i < kk; i++) {
        if (ss != "") ss += ",";
        s.Format("%d", min_i_list[i]);
        ss += s;
    }

    q.Format("select id, h, shape.STX as x, shape.STY as y from %s where id in (%s) order by id", kls->nazv, ss);

    CAdoFile *ado = kls->m_ado;

    double h[10];
    double x[10];
    double y[10];

    int i = 0;
    if (ado->openTable0(q)) {
         while ( !ado->isEOF() ) {
            int id = ado->read_long(0);
            h[i] = ado->read_double(1);
            x[i] = ado->read_double(2);
            y[i] = ado->read_double(3);
            i++;
            ado->MoveNext();
        }
    }

    double xx = pt.x/100.;
    double yy = -pt.y/100.;

    double interpolated_height = tri_h(xx, yy, kk, x, y, h);

    ss.Format("%.2f", interpolated_height);

    return ss;
}


void find_line_p(const CLINE2* line, CFPoint p, CFPoint& point1, CFPoint& point2)
{
    CFPoint p1 = where(line)->node.coord;
    CFPoint p2 = other(line)->node.coord;

    CLine2* l = bline(line);

    CCoordList cl, m_NP;

    cl.push_back(p1);
    CCoordList::const_iterator it = l->line.m_NP.begin();
    for (; it != l->line.m_NP.end(); ++it) {
        cl.push_back(*it);
    }
    cl.push_back(p2);

    point1 = cl.GetProjection(p, p1, p2);

    double dd = cl.GetProjection(p);
    CString str;
    str.Format("%g", dd);

    double h = hypot(p1.x - p2.x, p1.y - p2.y);
    double mas_otn = 1;
    double m5 = 500;

    point2 = CFPoint(p.x + (p2.x - p1.x) * m5 / h / mas_otn, p.y + (p2.y - p1.y) * m5 / mas_otn / h);
}

bool CGidrView::findNodeLine(const CFPoint& pt, CNode2** node, CLINE2** line, double m5)
{
    return findNodeLine2(pt, node, line, m5);
}


bool CGidrView::findNodeLine2(const CFPoint& pt, CNode2** node, CLINE2** line, double m5)
{
    CFPoint point;
    point.x = okr(pt.x, m_dX);
    point.y = okr(pt.y, m_dX);

    *node = m_cxema->m_graph->find(m_parent_id, pt, geom.masx * m5 / mas_otn);
    *line = m_cxema->m_graph->findLine(m_parent_id, pt, geom.masx * m5 / mas_otn);

    if (*node && IsRezhim(*node)) {
        *node = NULL;
    }
    if (*line && IsRezhim(*line)) {
        *line = NULL;
    }

    if (*node || *line) return true;

    return false;
}

bool CGidrView::findNodeLineUSA(const CFPoint& pt, CNode2** node, CLINE2** line, double m5)
{
    CFPoint point;
    point.x = okr(pt.x, m_dX);
    point.y = okr(pt.y, m_dX);

    *node = m_cxema->m_graph->find(m_parent_id, pt, m5);
    *line = m_cxema->m_graph->findLine(m_parent_id, pt, m5);

    if (*node && IsRezhim(*node)) {
        *node = NULL;
    }
    if (*line && IsRezhim(*line)) {
        *line = NULL;
    }

    if (*node || *line) return true;

    return false;
}

bool canInst(CFPoint pt, double masx, Klassif* m_kls, CLINE2* line, bool is_select)
{
    if (is_onlyline(m_kls->nazv)) {
        if (!line) {
            CString str;
            str.Format("%s можно размещать только на участке", m_kls->RusName());
            AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
            return false;
        }
        else {
            if (is_select && !bline(line)->line.isPjezo) {
               CString str;
               str.Format("%s нельзя размещать на этом участке", m_kls->RusName());
               AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
               return false;
            }
        }
    }

    if (is_onlypts(m_kls->nazv)) {
        if (!line || bline(line)->line.pipeSectionID <= 0) {
            CString str;
            str.Format("%s можно размещать только на участке ПТС", m_kls->RusName());
            AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
            return false;
        }
    }


    CString sg = getOnlygeo(m_kls->nazv);
    if (sg != "") {
        int n = m_geofile->findXY3(pt, D5 * masx, 3, sg);
        if (!n) {
            Klassif* kls = m_geofile->m_kl_list->findKlN(sg);
            if (kls) {
                sg = kls->RusName();
            }

            CString str;
            str.Format("%s можно размещать только на %s", m_kls->RusName(), sg);
            AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
            return false;
        }
    }

    return true;
}


bool CGidrView::find(const CFPoint& pt)
{
    CFPoint point;
    CNode2* node;
    CLINE2* line;
    CString str, text;
    CDC* m_dc;
    bool ret = false;

    point.x = okr(pt.x, m_dX);
    point.y = okr(pt.y, m_dX);

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();


    findNodeLine(pt, &node, &line, D5);

    //    CString http_add_geo_point(const char *tn, double mas, double x, double y);
    //    CString ss = http_add_geo_point("?", masx, pt.x, pt.y);


    if (regim == R_INIT) {
        switch (m_nRegim) {
        case ID_GOOGLE_ELEVATION:
        {
            Klassif *kls = m_geofile->m_kl_list->findKlN("gps");

            if (!kls) {
                kls = m_geofile->m_kl_list->findKlN("gis");
            }
            if (kls) {
                CString s2 = Elevation2(pt);

                CInputDialog inD(this, _TR("Геодезическая отметка"), _TR("Геодезическая отметка"), s2);
                inD.DoModal();
            }
            else {
                CString s2 = Elevation(pt);

                CInputDialog inD(this, _TR("Геодезическая отметка"), _TR("Геодезическая отметка"), s2);
                inD.DoModal();
            }
        }
        break;

        case ID_ELEVATION2:
        {
            CString s2 = Elevation2(pt);

            CInputDialog inD(this, _TR("Геодезическая отметка"), _TR("Геодезическая отметка"), s2);
            inD.DoModal();
        }
        break;
        
        
        case ID_OPENSTREETMAP_INFO:
            OsmInfo(pt);
            break;
        case ID_MARK_REV:
            if (line) {
                sw_p(line);
                InvalidateG();
            }
            break;

        case ID_MS_NODE_1:
            if (node) setMsNode(node, 1);
            break;
        case ID_MS_NODE_2:
            if (node) setMsNode(node, 2);
            break;


        case ID_OPR_NODE_1:
            if (node) setOprNode(node, 1);
            break;
        case ID_OPR_NODE_2:
            if (node) setOprNode(node, 2);
            break;


            //        case ID_SET_UCH_RS1:
            //            if (line) SetUchRs1(line);
            //            break;

        case ID_PAS_KAM:
            if (node) pasport_US(node);
            break;

        case ID_PAS_VV:
            pasport_VV(pt);
            break;

        case ID_REMONT_OTKL_POTR:
            //        AfxMessageBox("!", MB_OK|MB_ICONINFORMATION);
            if (line) {
                /*
                int     n = m_geofile->findXY3(point, D5*masx, 1, STR_REMONT_DEFECT);
                if (!n) n = m_geofile->findXY3(point, D5*masx, 1, STR_REMONT_SHURF );
                if (!n) n = m_geofile->findXY3(point, D5*masx, 1, STR_REMONT_OSMOTR);
                if (!n) n = m_geofile->findXY3(point, D5*masx, 1, STR_REMONT_KORROZ);
                if (!n) n = m_geofile->findXY3(point, D5*masx, 1, STR_REMONT_ISPYT);

                if (n) {
                */
                PopupOtklPotr(line);
                //          }
            }


            //        m_nRegim = ID_INFO;
            break;


        case ID_MOVE_LINE_END:
            if (!isEditGid()) return false;
            if (line) {
                CNode2* n1 = where(line);
                CNode2* n2 = other(line);

                double d1 = Length(pt, n1->node.coord);
                double d2 = Length(pt, n2->node.coord);

                m_nRegim = ID_MOVE_LINE_END_NEXT;
                m_line = line;
                m_n1 = (d1 < d2) ? n1 : n2;
                BeginDrawNode(m_n1);
            }
            break;
        case ID_MOVE_LINE_END_NEXT:
            EndDrawNode();
            m_nRegim = ID_MOVE_LINE_END;
            if (!isEditGid()) return false;

            if (node && node != where(m_line) && node != other(m_line)) {
                if (node->node.fileID != where(m_line)->node.fileID) {
                    AfxMessageBox(_TR("Узлы находятся в разных схемах"), MB_OK|MB_ICONINFORMATION);
                }
                else {
                    str.Format(_TR("Присоединить %s к узлу?"), bline(m_line)->getTable());
                    if (AfxMessageBox(str, MB_YESNO) == IDYES) {

                        if (moveLineEnd(m_n1, node, m_line)) {
                            GetDocument()->SetModifiedFlag(true);
                            InvalidateG();
                        }
                    }
                }
            }
            break;

        case ID_NEW_GEO:
        {
            if (m_kls && isEditGeo(m_kls->nazv)) {
                bool is_select = m_id_osmotr2 > 0;

                if (!canInst(pt, geom.masx, m_kls, line, is_select)) return false;

                if (m_kls->loc == LOC6 && line) { // По участку
                    getProject(line, point, m_NP, 6);

                    int nn = m_geofile->createObj(m_kls->nazv, m_NP, m_map_geo_value);

                    if (nn) {
                        CNode2* n1 = where(line);
                        CNode2* n2 = other(line);

                        CNode2* nn1 = n1->typPS() > 0 ? n1 : find_next(n1->node.m_graph, n1, n2, n1);
                        CNode2* nn2 = n2->typPS() > 0 ? n2 : find_next(n2->node.m_graph, n2, n1, n2);

                        regim = R_INIT;
                        m_NP.clear();
                        m_nRegim = ID_INFO;

                        GeoInfo(m_kls->nazv, m_kls->geo4[nn - 1]->nom, true);
                        m_nRegim = ID_INFO;

                        int sz_new = m_kls->geo4.size();

                        if (nn + 1 == sz_new) {
                            GeoInfo(m_kls->nazv, m_kls->geo4[nn + 1 - 1]->nom, true);
                        }
                    }
                    break;
                }
                else if (isPoint(m_kls->loc) && line) { // По участку
                    getProject(line, point, m_NP, 1);
                    int nn = m_geofile->createObj(m_kls->nazv, m_NP, m_map_geo_value);
                    if (nn) {
                        GeoInfo(m_kls->nazv, m_kls->geo4[nn - 1]->nom, true);
                        int sz_new = m_kls->geo4.size();

                        if (nn+1 == sz_new) {
                            GeoInfo(m_kls->nazv, m_kls->geo4[nn+1-1]->nom, true);
                        }
                    }
                    regim = R_INIT;
                    m_nRegim = ID_INFO;
                    m_NP.clear();
                    InvalidateG();
                    break;
                }
                else if (isPoint(m_kls->loc) && node) { // По узлу
                    m_NP.clear();
                    m_NP.push_back(node->node.coord);

                    int nn = m_geofile->createObj(m_kls->nazv, m_NP, m_map_geo_value);
                    regim = R_INIT;
                    m_nRegim = ID_INFO;
                    m_NP.clear();
                    InvalidateG();
                    if (nn) GeoInfo(m_kls->nazv, m_kls->geo4[nn - 1]->nom, true);
                    break;
                }
                //              else if (isPoint(m_kls->loc)) {
                else if (m_kls->loc == LOC1 || m_kls->loc == LOC11) {


                    m_NP.push_back(point);

                    int nn = m_geofile->createObj(m_kls->nazv, m_NP, m_map_geo_value);

                    regim = R_INIT;
                    m_nRegim = ID_INFO;
                    m_NP.clear();

                    InvalidateG();
                    if (nn) GeoInfo(m_kls->nazv, m_kls->geo4[nn - 1]->nom, true);
                    break;
                }

                if (m_kls->loc == LOC1 || isElPoint(m_kls->nazv)) {
                    m_NP.push_back(point);

                    int nn = m_geofile->createObj(m_kls->nazv, m_NP, m_map_geo_value);

                    regim = R_INIT;
                    m_nRegim = ID_INFO;
                    m_NP.clear();

                    InvalidateG();
                    if (nn) GeoInfo(m_kls->nazv, m_kls->geo4[nn - 1]->nom, true);
                    break;
                }

                if (m_kls->nazv == EL_LINE) {
                    int n = m_geofile->findXY3(point, D5 * geom.masx, 1, EL_IST);
                    if (n) {
                        Klassif* kls1 = m_geofile->m_kl_list->findKlN(EL_IST);
                        if (kls1) {
                            int id = kls1->GetNom2(n);
                            if (id >= 1) {
                                n_EL_IST = n;
                                CCoordList clL = kls1->geo4[id - 1]->cl;
                                point = clL.first();
                            }
                        }
                    }
                    else {
                        AfxMessageBox(_TR("Линия передач должна начинаться от источника электроснабжения"), MB_OK|MB_ICONINFORMATION);
                        return false;
                    }
                }

                if (is_water_line(m_kls->nazv)) {
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
//                                n_EL_IST = n;
                                CCoordList clL = kls1->geo4[id - 1]->cl;
                                point = clL.first();
                            }
                        }
                    }
                    else {
                        CString str;
                        str.Format("%s должна начинаться от узла водоснабжения", m_kls->RusName());
                        AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
                        return false;
                    }
                }

                regim = R_BEGIN_DRAW_LINE;
                if (m_kls->loc == LOC6) regim = R_BEGIN_DRAW_TYPE;
                setTypeLine(m_kls->typ);
                m_pt1 = m_pt2 = point;
                m_NP.push_back(m_pt1);

                if ((m_dc = BeginPaint()) != NULL) {
                    XDraw(m_dc, false);
                    XDraw(m_dc, true);
                    DrawRect(m_dc, point);
                    EndPaint(m_dc);
                }
            }
        }
        break;

        case ID_REMONT_KAPITAL:
            //    case ID_REMONT_OPRESSOVKA  :
        {
            if (!line) {
                AfxMessageBox(_TR("Не выбран участок"), MB_OK|MB_ICONINFORMATION);
                break;
            }

            //        CString fn = STR_REMONT;

            //        if (m_nRegim == ID_REMONT_KAPITAL) {
            CString fn = STR_REMONT_KAPITAL;
            //        }
            //          if (m_nRegim == ID_REMONT_OPRESSOVKA) {
            //            fn = STR_REMONT_OPRESSOVKA;
            //          }

            m_kls = m_geofile->m_kl_list->findKlN(fn);

            //          if (m_kls && isPoint(m_kls->loc) && isEditGeo(m_kls->nazv)) {
            if (m_kls && isEditGeo(m_kls->nazv)) {

                getProject(line, pt, m_NP, 1);
                m_NP = bline(line)->getCLall();
                int n;

                if (m_nRegim == ID_REMONT_KAPITAL) {
                    n = m_geofile->createObjKapital(m_kls->nazv, m_NP, line);
                }

                if (n) {
                    GeoInfo(m_kls->nazv, m_kls->geo4[n - 1]->nom, true);
                }
                //            m_geofile->createZhurnal(STR_REMONT, m_NP, line);

                regim = R_INIT;
                m_nRegim = ID_INFO;
                m_NP.clear();

                InvalidateG();
            }
        }
        break;


        //      case ID_REMONT :
//      case ID_OPRES_ZADV:
        case ID_REMONT_DEFECT_OPR:
        case ID_REMONT_DEFECT:
        case ID_REMONT_DEFECT2:
        case ID_REMONT_SHURF:
        case ID_REMONT_SHURF2:
        case ID_REMONT_CONTROL_TU:
        case ID_REMONT_OSMOTR:
        case ID_REMONT_KORROZ:
        case ID_REMONT_ISPYT:
        {
            if (!line) {
                AfxMessageBox(_TR("Не выбран участок"), MB_OK|MB_ICONINFORMATION);
                break;
            }
            /*
                        if (m_nRegim == ID_REMONT_DEFECT_OPR && bline(line)->line.m_opressovka <= 0) {
                            AfxMessageBox(_TR("На участке не задана опрессовка"), MB_OK|MB_ICONINFORMATION);
                            break;
                        }
            */

            if (m_nRegim == ID_REMONT_DEFECT_OPR && !isOpres(line, m_id_opr)) {
                AfxMessageBox(_TR("На участке не задана опрессовка"), MB_OK|MB_ICONINFORMATION);
                break;
            }


            /*
                        if (m_nRegim == ID_REMONT_DEFECT_REMONT2 && isRemont2(line, m_id_remont2)) {
                            AfxMessageBox(_TR("На участке не задан ремонт"), MB_OK|MB_ICONINFORMATION);
                            break;
                        }
            */


            CString fn = STR_REMONT_DEFECT;

            switch (m_nRegim) {
            case ID_OPRES_ZADV:        fn = "Опрессовка секционирующая задвижка"; break;
            case ID_REMONT_DEFECT:     fn = STR_REMONT_DEFECT;     break;
            case ID_REMONT_DEFECT2:    fn = STR_REMONT_DEFECT2;    break;
            case ID_REMONT_DEFECT_OPR: fn = "Опрессовка дефект";   break;
            case ID_REMONT_SHURF:      fn = STR_REMONT_SHURF;      break;
            case ID_REMONT_SHURF2:     fn = STR_REMONT_SHURF2;     break;
            case ID_REMONT_CONTROL_TU: fn = STR_REMONT_CONTROL_TU; break;
            case ID_REMONT_OSMOTR:     fn = STR_REMONT_OSMOTR;     break;
            case ID_REMONT_KORROZ:     fn = STR_REMONT_KORROZ;     break;
            case ID_REMONT_ISPYT:      fn = STR_REMONT_ISPYT;      break;
            case ID_REMONT_KAPITAL:    fn = STR_REMONT_KAPITAL;    break;
            }

            m_kls = m_geofile->m_kl_list->findKlN(transl(fn));

            if (!m_kls) {
                CString str;
                str.Format(_TR("Нет данных по %s"), fn);
                AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
            }

            if (m_kls && isPoint(m_kls->loc)) {
                if (isEditGeo(m_kls->nazv)) {
                    getProject(line, pt, m_NP, m_kls->loc);

                    map<CString, COleVariant> map_geo_value;

                    int n = m_geofile->createObjRemont(m_kls->nazv, m_NP, line, map_geo_value);

                    if (n) {
                        GeoInfo(m_kls->nazv, m_kls->geo4[n - 1]->nom, true);
                    }
                    //            m_geofile->createZhurnal(STR_REMONT, m_NP, line);

                    regim = R_INIT;
                    m_nRegim = ID_INFO;
                    m_NP.clear();

                    InvalidateG();
                }
                else {
                    CString str;
                    str.Format(_TR("Редактирование таблицы %s не разрешено"), m_kls->RusName());
                    AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
                }
            }
        }
        break;

        case ID_NAPR:
        case ID_NAPR_OPRES:
        case ID_NAPR_OSMOTR2:
        case ID_NAPR_2:
        case ID_NAPR_REMONT2:
            if (!node && line && m_napr1) {
                node = where(line) == m_napr1 ? other(line) : where(line);
            }
            if (node) {
                if (m_napr1 != NULL) {
                    m_napr1->node.m_graph->link(m_napr1, node);
                    redrawOnlyGid();
                }
                m_napr1 = node;
                node->node.isPjezo = true;
                m_cxema->m_graph->list_pjezo_min.push_back(node->id);
                if ((m_dc = BeginPaint()) != NULL) {
                    drawNode0(m_dc, node, false);
                    EndPaint(m_dc);
                }
            }
            break;

        case ID_NAPR2:
            if (!node && line && m_napr1) {
                node = where(line) == m_napr1 ? other(line) : where(line);
            }
            if (node) {
                if (m_napr1 != NULL) {
                    if (m_napr1->node.m_graph != node->node.m_graph) {
                        double len;
                        CNode2* find_zn0(CCxema * m_cxema, CNode2 * node, double& length);


                        m_napr1 = find_zn0(m_cxema, m_napr1, len);
                        if (len > 10) {
                            m_napr1 = 0;
                        }
                    }
                    if (m_napr1) {
                        m_napr1->node.m_graph->link2(m_napr1, node, false);
                        redrawOnlyGid();
                    }
                    m_napr1 = NULL;

                }
                else {
                    m_napr1 = node;
                }
                node->node.isPjezo = true;
                m_cxema->m_graph->list_pjezo_min.push_back(node->id);
                if ((m_dc = BeginPaint()) != NULL) {
                    drawNode0(m_dc, node, false);
                    EndPaint(m_dc);
                }
            }
            break;


        case ID_NODE:
            addNode(point);
            break;
        case ID_LINE:
            if (!isEditGid()) return false;
            if (isPot()) return false;
            m_n1 = node;
            m_n2 = NULL;
            if (m_n1) {
                regim = R_BEGIN_DRAW_LINE;
                m_pt1 = m_pt2 = m_n1->node.coord;
                m_NP.push_back(m_pt1);
                InvalidateG();
            }
            break;

        case ID_TRIO:
            AfxMessageBox(_TR("Временно удалено 501"), MB_OK|MB_ICONINFORMATION);
            break;

        case ID_NODE_NEOTR: neotr_node(node, point); break;
        case ID_ZN: Zn(node);    break;

        case ID_ADD_CITY: addCity(pt); break;

        case ID_VYD:  vydel(pt); break;
        case ID_VYD4:  vydel4(node, line); break;

        case ID_DEL:  del(pt);   break;

        case ID_REMONT_DEL: del_remont(pt);   break;
        case ID_REMONT_INFO: info_remont(pt);  break;
        case ID_KAPREMONT_INFO: info_kapremont(pt);  break;

        case ID_REMONT_KAPITAL_DOB: dob_kapital(pt); break;
            //    case ID_REMONT_OPRESSOVKA_DOB : dob_opressovka(pt); break;

        case ID_INFO:  ret = info(pt);  break;
        case ID_TU_INFO:  ret = tu_info(pt);  break;
        case ID_TU_NEW:  ret = tu_new(pt);  break;
        case ID_TU_DEL:  ret = tu_del(pt);  break;


        case ID_PTS_KARTA:  ret = pts_karta(pt);  break;
        
        case ID_PTS_INFO:  ret = pts_info(pt);  break;
        case ID_PTS_MOVE:  ret = pts_move(pt);  break;
        case ID_PTS_DEL:   ret = pts_del(pt);  break;

            //      case ID_PTS_DEL:  ret = pts_del(pt, m_kls_for_del);  break;

        case ID_INFO_TGID:  ret = info_TGID(pt);  break;
        case ID_WINCC_INFO: wincc_info(pt); break;
        case ID_INFO_GID:  ret = info_gid(pt);  break;
            //    case ID_MOVE_TO :  move_to(pt);  break;

        case ID_GEO_INFO: ret = geoInfo(pt); break;

        case ID_NODE_MOVE:
            if (!isEditPS()) return false;
            if (node) {
                m_n1 = node;
                m_nRegim = ID_NODE_MOVE_NEXT;
                BeginDrawNode(m_n1);
            }
            else if (line) {
                m_nRegim = ID_NODE_MOVE;
                ism(line);
            }
            break;

        case ID_NODE_MOVE1:
            if (!isEditGid()) return false;
            if (node) {
                m_n1 = node;
                m_nRegim = ID_NODE_MOVE1_NEXT;

                BeginDrawNode(m_n1);
            }
            else if (line) {
                ism(line);
                m_nRegim = ID_NODE_MOVE1;
            }
            break;

        case ID_NODE_MOVE_NEXT:
        {
            if (node && node != m_n1 && node->node.m_graph == m_n1->node.m_graph) {
                str.Format(_TR("Слишком близко к %s %s"), node->getTable(), node->getName());
                AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
                m_nRegim = ID_NODE_MOVE;
                break;
            }
            EndDrawNode();
            if (!isEditPS()) return false;
            setXY(m_n1, point);
            m_nRegim = ID_NODE_MOVE;
            GetDocument()->SetModifiedFlag(true);
            InvalidateG();
        }
        break;

        case ID_NODE_MOVE1_NEXT:
            //        {
            //            AfxMessageBox("Временно удалено 502", MB_OK|MB_ICONINFORMATION);   // разбить узел
            //        }

            /// Исправляю <<<

            EndDrawNode();
            if (!isEditGid()) return false;
            if (m_n1->node.externalSignID == 1)
                setXY1(m_n1, point);  // подача обратка
            else
                setXY(m_n1, point);

            m_nRegim = ID_NODE_MOVE1;
            GetDocument()->SetModifiedFlag(true);
            InvalidateG();

            /// >>> Исправляю

            break;


        case ID_COPY_AKT:
            copyAkt(m_n_akt, point);
            break;

        case ID_RENAME:
            if (!isEditGid()) return false;
            if (node) renameNode(node);
            else if (line) renameLine(bline(line));
            break;

        case ID_KAM:
        case ID_KAM2:

            if (node && node->node.typ != TIP_PO && node->node.internalNodeID == 0) {
                if (!node->node.isP) {
                    if (isEditGid()) {
                        if (AfxMessageBox(_TR("Создать схему?"), MB_YESNO) != IDYES) return false;
                        if (!initCxema(node)) {
                            return false;;
                        }
                    }
                    else {
                        break;
                    }
                }
                node->node.isP = true;
                m_parent_id = node->id;
                old_masx = geom.masx;
                old_masy = geom.masy;
                old_m_bx = geom.m_bx;
                old_m_by = geom.m_by;
                old_rect = m_rectBorder;

                EndDrawNode();

                CFRect rect = m_cxema->m_graph->getBorder(m_parent_id);
                SetBorder(rect);
                ChMas(rect);
            }
            Invalidate();
            break;

        case ID_SWAP:
            if (!isEditGid()) return false;
            if (isPot()) return false;
            if (line) {
                swap(line);
            }
            break;

        case ID_PODP_YES_NO:
            if (!isEditGid()) return false;
            if (isPot()) return false;
            if (line || node) {
                Podp(node, bline(line));
                GetDocument()->SetModifiedFlag(true);
                InvalidateG();
            }
            break;

        case ID_OPEN_CLOSE:
            if (!isEditGid()) return false;
            if (node) {
                Otkr(node);
            }
            else if (line) {
                Otkr(bline(line));
            }
            break;

        case ID_DEL_MAG_UZEL:
            //      if (node) MagUzelDel(node);
            break;

        case  ID_SET_UCH_MS:
            if (line && bline(line)->line.isPjezo) {
                bline(line)->line.isPjezo2 = !bline(line)->line.isPjezo2;
                InvalidateG();
            }
            break;

        case ID_TEXT:
        {
            if (!isEditGid()) return false;
            if (!curFile()) {
                AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK|MB_ICONINFORMATION);
            }
            else {
                CString text;
                CInputDialog inD(this, _TR("Исходный текст"), _TR("Введите текст"), text);

                if (inD.DoModal() == IDOK) {
                    if (m_cxema->addText(point, inD.m_strInput, m_parent_id, m_cxema->m_cur_file->id)) {
                        GetDocument()->SetModifiedFlag(true);
                        InvalidateG();
                    }
                }
            }
        }
        break;
        /*
        case ID_TEXT_OPR :
        {
        if (!isEditGid()) return false;
        CString text;
        CInputDialog inD(this, _TR("Исходный текст"), _TR("Введите текст"), text);

        if (inD.DoModal() == IDOK) {
        if (m_cxema->addText(point, inD.m_strInput)) {
        GetDocument()->SetModifiedFlag(true);
        InvalidateG();
        }
        }
        }
        break;
        */


        case ID_BEGIN_CANAL_1:
            if (!line) {
                AfxMessageBox("Точка должна быть на участке");
            }
            else {
                m_pt1_canal = pt;
                m_l_canal = line;
                m_nRegim = ID_BEGIN_CANAL_2;
                Invalidate();
                AfxMessageBox("Введите вторую точку на участке");
            }
            break;
        case ID_BEGIN_CANAL_2:
            if (line != m_l_canal) {
                AfxMessageBox("Точка должна быть на том же участке");
            }
            else {
                m_pt2_canal = pt;
                m_nRegim = ID_INFO;

                CString text = AfxGetApp()->GetProfileString(szSection, "canal_width", "1");
                
                CInputDialog inD(this, _TR("Ширина канала"), _TR("Введите ширину канала"), text);
                if (inD.DoModal() == IDOK) {
                    text = inD.m_strInput;
                    double width = atof(text);
                    if (width > 0) {
                        AfxGetApp()->WriteProfileString(szSection, "canal_width", text);
                        CCoordList cl = bline(line)->line.m_NP;

                        cl.push_front(where(line)->node.coord);
                        cl.push_back(other(line)->node.coord);

                        cl = cl.subline(m_pt1_canal, m_pt2_canal, D5 * geom.masx);

                        cl = cl.createBuffer(width*100./2);
                        int n = m_geofile->createObj("kanal", cl, m_map_geo_value);
                        if (n) {
                            GeoInfo(m_kls->nazv, m_kls->geo4[n - 1]->nom, true);
                        }
                        Invalidate();
                    }
                }
                Invalidate();
            }
            break;

        }
    }
    else if (regim == R_BEGIN_DRAW_TYPE && m_nRegim == ID_NEW_GEO) {
        m_NP.push_back(point);

        int nn = 0;

        if (m_geofile) nn = m_geofile->createObj(m_kls->nazv, m_NP, m_map_geo_value);

        regim = R_INIT;
        m_nRegim = ID_INFO;
        m_NP.clear();
        GetDocument()->SetModifiedFlag(true);
        if (nn) GeoInfo(m_kls->nazv, m_kls->geo4[nn - 1]->nom, true);
        InvalidateG();
    }
    else if (regim == R_BEGIN_DRAW_LINE && (m_nRegim == ID_NEW_GEO)) {
        if (isLine(m_kls->loc) || m_kls->loc == LOC3) {

            if (m_kls->nazv == EL_LINE) // Создать Линию электропередач от нач до конца
            {
                int n = m_geofile->findXY3(point, D5 * geom.masx, 1, EL_PRIEM);
                if (n) {
                    Klassif* kls1 = m_geofile->m_kl_list->findKlN(EL_PRIEM);
                    if (kls1) {
                        int id = kls1->GetNom2(n);
                        if (id >= 1) {
                            CCoordList clL = kls1->geo4[id - 1]->cl;
                            point = clL.first();
                            m_NP.push_back(point);
                            if (m_geofile) {
                                nn = m_geofile->createObj(m_kls->nazv, m_NP, m_map_geo_value);
                                //                m_geofile->write(m_kls->nazv, nn, _TR("Наименование источника"), n_EL_IST);
                                //                m_geofile->write(m_kls->nazv, nn, _TR("Наименование приемника"), n);
                                //                m_geofile->write(m_kls->nazv, nn, _TR("Протяженность, м"), m_NP.Length());
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
            }


            if (is_water_line(m_kls->nazv)) // Создать участок водопровода
            {
                if (createWaterLine(m_kls, point)) return true;
            }

            if ((m_dc = BeginPaint()) != NULL) {
                XDraw(m_dc, false);
                if (m_NP.size() > 0) m_pt1 = m_NP.back();
                m_pt2 = point;
                m_NP.push_back(point);
                DrawRect(m_dc, point);
                XDraw(m_dc, true);
                EndPaint(m_dc);
            }
            m_pt1 = point;
        }
    }
    else if (regim == R_BEGIN_MOVE_POINT6_2) {
        regim = R_INIT;
        m_NP.clear();
        m_NP.push_back(m_pt1);
        m_NP.push_back(point);

        regim = R_INIT;
        m_nRegim = ID_ISM_GEO;

        updateObj();
        m_nRegim = ID_INFO;
        GetDocument()->SetModifiedFlag(true);
        InvalidateG();
    }
    else if (regim == R_BEGIN_MOVE_POINT1) {
        if (!canInst(pt, geom.masx, m_kls, line, false)) return false;

        if (is_water_node(m_kls->nazv)) {
            updateWaterNode(m_kls, point); 
            return true;
        }

        regim = R_INIT;

        bool cr_el = updateEl(m_kls, point, 6);

        m_NP.clear();

        if (!cr_el) {
            if (line) { // По участку
                getProject(line, point, m_NP, 1);
            }
            else {
                m_NP.push_back(point);
            }

            m_CurLine = line;
            m_nRegim = ID_ISM_GEO;

            updateObj();

            m_nRegim = ID_INFO;

            GetDocument()->SetModifiedFlag(true);
            InvalidateG();
        }
        
    }
    else if (regim == R_BEGIN_MOVE_POINT6) {
        if (!canInst(pt, geom.masx, m_kls, line, false)) return false;

        bool cr_el = updateEl(m_kls, point, 6);
        if (!cr_el) {
            if (line) { // По участку
                m_NP.clear();
                getProject(line, point, m_NP, 6);

                regim = R_INIT;
                m_nRegim = ID_ISM_GEO;
                m_CurLine = line;

                updateObj();

                m_nRegim = ID_INFO;

                GetDocument()->SetModifiedFlag(true);
                InvalidateG();
            }
            else {
                regim = R_BEGIN_MOVE_POINT6_2;
                setTypeLine(m_kls->typ);
                m_pt1 = m_pt2 = point;
                m_NP.push_back(m_pt1);

                if ((m_dc = BeginPaint()) != NULL) {
                    XDraw(m_dc, false);
                    XDraw(m_dc, true);
                    DrawRect(m_dc, point);
                    EndPaint(m_dc);
                }
            }
        }
    }
    else if (regim == R_ALSECO_PICK) {
        if (m_prop_grid && ::IsWindow(m_prop_grid->m_hWnd)) {
            CString zdan = ZDANIYA_2;

            int n = m_geofile->findXY3(point, D5 * geom.masx, LOC3, zdan);
            if (n) {
                Klassif* kls = m_geofile->m_kl_list->findKlN(zdan);
                if (kls) {
                    bool readAlseco(CAdoFile *ado, CString qq, double &otop_z_el, double &otop_z_be, double &otop_n, double &gvs_op, double &gvs_oo, double &gvs_pr, double &gvs_sm, double &gvs_ps, double &gvs_pw, double &vent);

                    CString qq;
                    qq.Format("%d", n);

                    double otop_z_el, otop_z_be, otop_n, gvs_op, gvs_oo, gvs_pr, gvs_sm, gvs_ps, gvs_pw, vent;

                    readAlseco(kls->m_ado, qq, otop_z_el, otop_z_be, otop_n, gvs_op, gvs_oo, gvs_pr, gvs_sm, gvs_ps, gvs_pw, vent);

                    //                int id = kls->GetNom2(n);
                    //                if (id >= 1) {
                    CString q;
                    q.Format("SELECT  id_adr_mas, street_nam, number_1 FROM %s WHERE id=%d", kls->nazv, n);

                    vector<CString> vv;

                    if (readTableValues(kls->m_ado, q, 3, vv)) {
                        CString msg, name = "";

                        m_prop_grid->GetNameValue("name", name);


                        CString s1 = vv[0];
                        CString s2 = vv[1];
                        CString s3 = vv[2];
                        
                        if (s1 != "") s1 += " ";
                        s1 += s2;
                        if (s1 != "") s1 += " ";
                        s1 += s3;

                        if (name == "") name = s1;


                        CString alseco_text(double otop_z_el, double otop_z_be, double otop_n, double gvs_op, double gvs_oo, double gvs_pr, double gvs_sm, double gvs_ps, double gvs_pw, double vent);


                        CString ss = alseco_text(otop_z_el, otop_z_be, otop_n, gvs_op, gvs_oo, gvs_pr, gvs_sm, gvs_ps, gvs_pw, vent);

                        msg.Format("%s\nНагрузки ГКал/ч:\n%sСохранить нагрузки в потребитель?",
                            name, ss);


//                        bool saveAlseco(CPropertyGrid* wnd, CString m_prop_table, CString m_prop_field, CString name, double otop_z, double otop_n, double gvs_op, double gvs_oo, double gvs_pr, double gvs_sm, double gvs_ps, double gvs_pw, double vent)
                        bool saveAlseco(CPropertyGrid* wnd, CString m_prop_table, CString m_prop_field, CString name, double otop_z_el, double otop_z_be, double otop_n, double gvs_op, double gvs_oo, double gvs_pr, double gvs_sm, double gvs_ps, double gvs_pw, double vent);

                        if (AfxMessageBox(msg, MB_YESNO) == IDYES) {
                            saveAlseco(m_prop_grid, m_prop_table, m_prop_field, name, otop_z_el, otop_z_be, otop_n, gvs_op, gvs_oo, gvs_pr, gvs_sm, gvs_ps, gvs_pw, vent);

                            CString kod, name;

                            m_prop_grid->GetNameValue("externalCodeID", kod);
                            m_prop_grid->GetNameValue("externalNodeName", name);

                            CString qq;

                            qq.Format("UPDATE zdaniya_2 SET potrebitel=NULL WHERE potrebitel='%s %s'", kod, name);
                            kls->m_ado->Execute(qq);
                            
                            qq.Format("UPDATE zdaniya_2 SET potrebitel='%s %s' WHERE id=%d", kod, name, n);
                            kls->m_ado->Execute(qq);

                            m_prop_grid->Invalidate(FALSE);
                            m_prop_grid->UpdateWindow();
                        }
                    }
                    else {
                        AfxMessageBox("Ошибка 2");
                    }
                }
            }
            else {
                AfxMessageBox("Здание не нашел. Повторите команду \"Выберите здание с нагрузками\"");
            }
        }
        regim = R_INIT;
        m_nRegim = ID_INFO;
    }

    else if (regim == R_NODE_PICK || regim == R_NODE_PICK1 || regim == R_NODE_PICK2) {
        if (node) {
            if (m_prop_grid && ::IsWindow(m_prop_grid->m_hWnd)) {
                m_prop_grid->SetNameValue(m_prop_field, node->id);
                if (m_field_for_dist != "") {
                    double dis = Length(pt, m_pt_object);
                    dis = trunc(dis * 10 + 5) / 10;

                    m_prop_grid->SetNameValue(m_field_for_dist, dis);
                    m_field_for_dist = "";
                }
                m_prop_grid->Invalidate(FALSE);
                m_prop_grid->UpdateWindow();
                if (regim == R_NODE_PICK1) m_node_pick1 = node;
                if (regim == R_NODE_PICK2) m_node_pick2 = node;

/*
                if (m_node_pick1 && m_node_pick2) {
//                    m_cxema->m_graph->link(m_node_pick1, m_node_pick2);
                    m_node_pick1->node.m_graph->reset();
                    m_node_pick1->node.m_graph->link2(m_node_pick1, m_node_pick2, true);
                }
                Invalidate();
*/
                setDefect();
            }
            regim = R_INIT;
            m_nRegim = ID_INFO;
        }
    }
    else if (regim == R_NODE_ADD) {
        if (node) {
            if (m_table_node && ::IsWindow(m_table_node->m_hWnd)) {
                m_table_node->proc(12345, (void*)node, (void*)m_prop_id, 0, 0);
            }
            regim = R_INIT;
            m_nRegim = ID_INFO;
        }
    }

    else if (regim == R_BEGIN_DRAW_LINE && m_nRegim == ID_LINE && m_n1) {
        if (node || m_n2) {
            m_NP.pop_front();

            regim = R_INIT;
            m_nRegim = ID_INFO;

            if (m_n2) {
                m_nRegim = ID_NODE_NEOTR;
                setXY(m_n2, point);

                CString par;
                par.Format("%d", m_n2->node.fileID);
                CWaitCursor wait;

                CAdoFile* ado = getAdo(getPsAdoName());

                if (ado) {
                    set<int>  v;
                    v.insert(m_n2->node.fileID);
                    m_cxema->m_graph->clearFile(v);
                    m_cxema->openGid(ado, par, pDoc->m_user_gid);
                    Invalidate();
                }
            }
            else {
                addLine(m_n1, node, m_NP);
            }
            GetDocument()->SetModifiedFlag(true);
            m_n1 = m_n2 = NULL;
            m_NP.clear();
            InvalidateG();
            Invalidate();
        }
        else {
            if ((m_dc = BeginPaint()) != NULL) {
                XDraw(m_dc, false);
                if (m_NP.size() > 0) m_pt1 = m_NP.back();
                m_pt2 = point;
                m_NP.push_back(point);
                DrawRect(m_dc, point);
                XDraw(m_dc, true);
                EndPaint(m_dc);
            }
            m_pt1 = point;
        }
    }
    else if (regim == R_BEGIN_DRAW_TEXT1) {
        regim = R_BEGIN_DRAW_TEXT2;
        m_pt1 = m_pt2 = pt;
    }
    else if (regim == R_BEGIN_DRAW_TEXT2) {
        regim = R_INIT;
        m_nRegim = ID_INFO;
        m_pt2 = pt;
        //      createText(m_pt1, m_pt2, m_strText);
        m_geofile->createText(m_pt1, m_pt2, m_strText);
        Invalidate();

    }

    return ret;
}
