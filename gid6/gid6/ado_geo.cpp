#include "stdafx.h"
#include "gid6.h"

#include "ado.h"
#include "gid6doc.h"
#include "gidrview.h"
#include "MMenu.h"
#include "MainFrm.h"
#include "gid6Doc.h"

#include "electro.h"

CString getAdoError();

bool ado_execute(CAdoFile* ado, const char* q, long* affected);
long ado_executeInsert(CAdoFile* ado, const char* q, long* affected);

bool delObject(CAdoFile* ado, const char* tn, int id);

#include "CNagr2.h"
#include "CTu2.h"

#include "Ultimate Grid\TableDialog.h"

#include "ado2.h"

void copy_korrozia(CAdoFile* ado, int y1)
{
    CString q;
    long affected;


    q.Format(

        "UPDATE i\n"
        "SET \n"
        //"i.tekuschiy_nomer = NULL,\n"
        "i.nomer_indikatora_korrozii = NULL,\n"
        "i.truboprovod = NULL,\n"
        "i.kolichestvo_plastin_v_sborke = NULL,\n"
        "i.sredniy_ves_plastiny_pri_ustanovke__g = NULL,\n"
        "i.radius_krugloy_plastiny__mm = NULL,\n"
        "i.radius_vtulki__mm = NULL,\n"
        "i.tolschina_plastiny__mm = NULL,\n"
        "i.data_ustanovki = NULL,\n"
        "i.data_izvlecheniya = NULL,\n"
        //"i.kolichestvo_dney_ispytaniy = NULL,\n"
        "i.sredniy_ves_plastiny_posle_ispytaniy__g = NULL,\n"
        "i.poterya_massy_srednyaya_pri_kislotnoy_obraboke__g = NULL,\n"
        "i.srednyaya_skorost_korrozii__mm_god = NULL,\n"
        "i.otsenka_korrozionnogo_protsessa = NULL,\n"
        "i.agressivnost_setevoy_vody = NULL,\n"
        "i.vneshniy_vid_plastin = NULL,\n"
        "i.data_planirovaniya = NULL,\n"
        "i.primechanie = NULL,\n"
        "i.stateIndID = NULL\n"

        "FROM indikator_korrozii i\n"
        "JOIN indikator_korrozii_po_godam i2 ON i2.id_i=i.id \n"
        "WHERE YEAR(i2.data_planirovaniya)=%d OR YEAR(i2.data_ustanovki)=%d\n"
        , y1, y1
    );

    ado->Execute(q, &affected);


    q.Format(

        "UPDATE i\n"
        "SET \n"
        "i.sostoyanie = i2.sostoyanie,\n"


        //"i.tekuschiy_nomer = i2.tekuschiy_nomer,\n"
        "i.nomer_indikatora_korrozii = i2.nomer_indikatora_korrozii,\n"
        "i.truboprovod = i2.truboprovod,\n"
        "i.kolichestvo_plastin_v_sborke = i2.kolichestvo_plastin_v_sborke,\n"
        "i.sredniy_ves_plastiny_pri_ustanovke__g = i2.sredniy_ves_plastiny_pri_ustanovke__g,\n"
        "i.radius_krugloy_plastiny__mm = i2.radius_krugloy_plastiny__mm,\n"
        "i.radius_vtulki__mm = i2.radius_vtulki__mm,\n"
        "i.tolschina_plastiny__mm = i2.tolschina_plastiny__mm,\n"
        "i.data_ustanovki = i2.data_ustanovki,\n"
        "i.data_izvlecheniya = i2.data_izvlecheniya,\n"
        //"i.kolichestvo_dney_ispytaniy = i2.kolichestvo_dney_ispytaniy,\n"
        "i.sredniy_ves_plastiny_posle_ispytaniy__g = i2.sredniy_ves_plastiny_posle_ispytaniy__g,\n"
        "i.poterya_massy_srednyaya_pri_kislotnoy_obraboke__g = i2.poterya_massy_srednyaya_pri_kislotnoy_obraboke__g,\n"
        "i.srednyaya_skorost_korrozii__mm_god = i2.srednyaya_skorost_korrozii__mm_god,\n"
        "i.otsenka_korrozionnogo_protsessa = i2.otsenka_korrozionnogo_protsessa,\n"
        "i.agressivnost_setevoy_vody = i2.agressivnost_setevoy_vody,\n"
        "i.vneshniy_vid_plastin = i2.vneshniy_vid_plastin,\n"
        "i.data_planirovaniya = i2.data_planirovaniya,\n"
        "i.primechanie = i2.primechanie,\n"
        "i.stateIndID = i2.stateIndID\n"

        "FROM indikator_korrozii i\n"
        "JOIN indikator_korrozii_po_godam i2 ON i2.id_i=i.id \n"
        "WHERE YEAR(i2.data_planirovaniya)=%d OR YEAR(i2.data_ustanovki)=%d\n"
        , y1, y1
    );

    ado->Execute(q, &affected);
}

#include "PropertyGrid\test\PropGridDlg.h"


int get_minuts(CString vre)
{
    int h, m;

    if (sscanf(vre, "%d:%d", &h, &m) == 2) {
        return h * 60 + m;
    }
    return 0;
}

void CGidrView::setDefectForRemonts()
{
    int n_p = 0;
    double nagr = 0;

    auto p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* pp = p->second;
        if (pp->node.isPjezo && (pp->node.typ == TIP_PR || pp->node.typ == TIP_PO)) {
            n_p++;

            double nagr_p = pp->node.Qot + pp->node.Qgvs + pp->node.Qvent;
            nagr += nagr_p;
        }
        p++;
    }

    if (m_prop_grid && ::IsWindow(m_prop_grid->m_hWnd)) {

        COleDateTime date1, date2;
        CString s_t1, s_t2;

        bool b1 = m_prop_grid->GetNameValue("data_nachala_remonta", date1);
        bool b2 = m_prop_grid->GetNameValue("data_zaversheniya_remonta", date2);

//        bool b3 = m_prop_grid->GetNameValue("vremiaNachalaRemonta", s_t1);
//        bool b4 = m_prop_grid->GetNameValue("vremiaZaversheniaRemonta", s_t2);

        double t1 = 0, t2 = 0;

//        if (b1 && b2 && b3 && b4) {
        if (b1 && b2) {
//            int m1 = get_minuts(s_t1);
//            int m2 = get_minuts(s_t2);
            int m1 = 0;
            int m2 = 0;


            double dt = ((date2 - date1) * 24. * 60. + m2 - m1) / 60. * nagr;
            m_prop_grid->SetNameValue("kolichestvo_nedootpushchennoj_teplovoj_energii", (double)dt);
        }

        m_prop_grid->SetNameValue("kolichestvo_otklyuchennyh_potrebitelej", n_p);
    }
}



void CGidrView::setDefect()
{
    if (m_node_pick1 && m_node_pick2) {
        m_node_pick1->node.m_graph->reset();
        m_node_pick1->node.m_graph->link2(m_node_pick1, m_node_pick2, false);
//        m_node_pick1->node.m_graph->link(m_node_pick1, m_node_pick2);

        int n_p = 0;
        double nagr = 0;

        auto p = m_cxema->m_graph->map_node.begin();

        while (p != m_cxema->m_graph->map_node.end()) {
            CNode2* pp = p->second;
            if (pp->node.isPjezo && (pp->node.typ == TIP_PR || pp->node.typ == TIP_PO)) {
                n_p++;

                double nagr_p = pp->node.Qot + pp->node.Qgvs + pp->node.Qvent;
                nagr += nagr_p;
            }
            p++;
        }

        if (m_prop_grid && ::IsWindow(m_prop_grid->m_hWnd)) {

            COleDateTime date1, date2;
            CString s_t1, s_t2;

            bool b1 = m_prop_grid->GetNameValue("data_nachala_remonta", date1);
            bool b2 = m_prop_grid->GetNameValue("data_zaversheniya_remonta", date2);

            bool b3 = m_prop_grid->GetNameValue("vremiaNachalaRemonta", s_t1);
            bool b4 = m_prop_grid->GetNameValue("vremiaZaversheniaRemonta", s_t2);

            double t1 = 0, t2 = 0;

            if (b1 && b2 && b3 && b4) {
                int m1 = get_minuts(s_t1);
                int m2 = get_minuts(s_t2);

                double dt = ((date2 - date1) * 24. * 60. + m2 - m1) / 60. * nagr;
                m_prop_grid->SetNameValue("kolichestvo_nedootpushchennoj_teplovoj_energii", (double)dt);
            }

            m_prop_grid->SetNameValue("kolichestvo_otklyuchennyh_potrebitelej", n_p);
        }
        Invalidate();
    }
}


int CGidrView::GeoInfo(const char* tn, int ID, bool autodelete)
{
    CPropertyGrid* prop = nullptr;
    int ret = 0;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (kls) {

        if (kls->nazv == "defect" || kls->nazv == "shurfy") {
            CGeoObject* geo = kls->getGeoObjectById(ID);

            if (geo) {
                CNode2* node;
                CLINE2* line;

                if (findNodeLine(geo->cl.first(), &node, &line, D5) && line) {
                    m_linePTS_ID = bline(line)->line.pipeSectionID;
                    Invalidate();
                }
            }
        }


        CDC* m_dc;
        if ((m_dc = BeginPaint()) != NULL) {
            m_geofile->DrawObj(m_dc, this, kls, kls->GetNom2(ID), true, true, true, false, kls->no_mas);
            m_is_geo = true;
            EndPaint(m_dc);
        }

        if (kls->nazv.CompareNoCase("Zdaniya_potrebiteley") == 0) {
            CNagr2 dlg(this, kls->m_ado->m_type_of_net, kls->m_ado->m_schema, kls->m_ado->getConnect(), tn, ID);
            dlg.DoModal();
            Invalidate();
        }
        else if (kls->nazv.CompareNoCase("zdaniya_tu") == 0) {
            CTu2 dlg(this, kls->m_ado->m_type_of_net, kls->m_ado->m_schema, kls->m_ado->getConnect(), tn, ID);
            if (dlg.DoModal() == IDOK) {
                CGeoObject* geo = kls->getGeoObjectById(ID);
                if (geo) {
                    geo->cl.color = (dlg.m_zav_exist && !dlg.m_dei_exist) ? getGeoColor(2000 + (1 << (4-1)) ) : getGeoColor(2001);
//                    geo->cl.color = (dlg.m_zav_exist && !dlg.m_dei_exist) ? getGeoColor(1000) : getGeoColor(2004);
                }
            }
            Invalidate();
        }
        /*
                else if (kls->nazv.CompareNoCase("defekt112") == 0) {
                    CAdoFile* ado = kls->m_ado;
                    CString tn = "defekt_elementy";
                    CString q;
                    int n_ret = 1;
                    q.Format("SELECT * FROM defekt_elementy WHERE defekt=%d", ID);
                    CTableDialog dlg(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, n_ret, virt_data_edit_add_del);
                    dlg.SetKey("defekt", ID);

                    dlg.DoModal();
                    Invalidate();
                }
        */
        else if (kls->nazv.CompareNoCase("defect") == 0) {
            prop = ado_info(this, kls->m_ado, tn, ID, kls->RusName(), TP_GEO, NULL, autodelete);
            m_prop_grid = prop;

            CString q;
            vector<CString> vv;

            q.Format("SELECT nodeID1, nodeID2 FROM %s WHERE id=%d", kls->nazv, ID);

            if (readTableValues(kls->m_ado, q, 2, vv)) {
                m_node_pick1 = m_cxema->m_graph->find(atoi(vv[0]));
                m_node_pick2 = m_cxema->m_graph->find(atoi(vv[1]));
                //                m_cxema->m_graph->link(m_node_pick1, m_node_pick2);
                setDefect();
            }

            //            ret = (int) ado_info(this, kls->m_ado, tn, ID, kls->RusName(), 3, NULL, autodelete);
        }
        else if (kls->nazv.CompareNoCase("indikator_korrozii") == 0) {
            copy_korrozia(kls->m_ado, m_sezon_korrozia);

            ret = (int)ado_info(this, kls->m_ado, tn, ID, kls->RusName(), TP_GEO, NULL, autodelete);
        }
        else {
            ret = (int)ado_info(this, kls->m_ado, tn, ID, kls->RusName(), TP_GEO, NULL, autodelete);
        }

        return ret;
    }

    return 0;
}



bool CGeoFile::updateObjField(const char* tn, int id, const char* fn, int value)
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (kls) {
        CString q;

        q.Format("UPDATE %s SET %s=%d WHERE ID=%d", tn, fn, value, id);

        long affected;

        if (ado_execute(kls->m_ado, q, &affected) && affected > 0) {
        }
        else {
            return false;
        }
    }

    return true;
}



bool CGeoFile::updateObj(const char* tn, int id, const CCoordList& cl)
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (kls) {
        CString str = cl.getWKT(kls->loc, kls->m_ado->m_type_of_net);
        CString q;

        //    q.Format("UPDATE `%s` SET shape=%s WHERE ID=%d", tn, str, id);
        q.Format("UPDATE %s SET shape=%s WHERE ID=%d", tn, str, id);

        long affected;

        if (ado_execute(kls->m_ado, q, &affected) && affected > 0) {
            CGeoObject* geo = kls->getGeoObjectById(id);

            if (geo) {
                geo->cl = cl;
                geo->rect = cl.GetFRect();
            }
        }
        else {
            return false;
        }
    }

    return true;
}


bool CGidrView::updateObj(const char* tn, int id, const CCoordList& cl)
{
    if (m_geofile->updateObj(tn, id, cl)) {
        Invalidate();
    }
    else {
        AfxMessageBox(getAdoError(), MB_OK | MB_ICONINFORMATION);
    }

    return true;
}

int addGeoAdd(CAdoFile *ado, const CString &tn, int id);
int addGeoDelete(CAdoFile *ado, const CString &tn, int id);



bool CGeoFile::delGeoObj(const char* tn0, int id)
{
    bool ret = false;

    CString tn = tn0;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (kls) {
        if (delObject(kls->m_ado, tn, id)) {

            CGeoObject* geo = kls->getGeoObjectById(id);
            if (geo) {
                delete geo;
            }

            int n = kls->GetNom2(id);
            if (n > 0) {
                kls->geo4.erase(kls->geo4.begin() + (n - 1));
            }

            addGeoDelete(kls->m_ado, tn, id);

            ret = true;
        }
        else {
            if (tn == EL_LINE) {
                AfxMessageBox("Невозможно удалить линию электропередач, пока существуют привязанные к ней объекты", MB_OK | MB_ICONINFORMATION);
            }
            else if (tn == EL_PRIEM) {
                if (kls->m_ado->getError().Find(EL_LINE) != -1) {
                    AfxMessageBox("Невозможно удалить приемник электроснабжения, пока существует привязанная к нему линия электропередач", MB_OK | MB_ICONINFORMATION);
                }
                else {
                    AfxMessageBox("Невозможно удалить приемник электроснабжения, пока существуют привязанные к нему объекты", MB_OK | MB_ICONINFORMATION);
                }
            }
            else if (tn == EL_IST) {
                if (kls->m_ado->getError().Find(EL_LINE) != -1) {
                    AfxMessageBox("Невозможно удалить источник электроснабжения, пока существует привязанная к нему линия электропередач", MB_OK | MB_ICONINFORMATION);
                }
                else {
                    AfxMessageBox("Невозможно удалить приемник электроснабжения, пока существуют привязанные к нему объекты", MB_OK | MB_ICONINFORMATION);
                }
            }
            else if (tn == "indikator_korrozii") {
                if (kls->m_ado->getError().Find("indikator_korrozii_po_godam") != -1) {
                    AfxMessageBox("Невозможно удалить Индикатор коррозии, пока существуют записи в Журнале истории", MB_OK | MB_ICONINFORMATION);
                }
            }
            else if (tn == "shurfy") {
                if (kls->m_ado->getError().Find("defect") != -1) {
                    AfxMessageBox("Невозможно удалить Шурф, пока существуют связанные с ним Нарушения", MB_OK | MB_ICONINFORMATION);
                }
                else {
                    CString q;
                    q.Format("DELETE FROM faktory_riska_truboprovoda WHERE objID=%d AND obj_type_faktory_riskaID=1", id);
                    kls->m_ado->Execute(q);
                }
            }
            else {
                AfxMessageBox(kls->m_ado->getError(), MB_OK | MB_ICONINFORMATION);
            }
        }
    }

    return ret;
}


bool CGidrView::delGeoObj(const char* tn, int id)
{
    if (!strcmp(tn, "indikator_korrozii")) {
        return korrozia_del(tn, id);
    }

    bool b = m_geofile->delGeoObj(tn, id);

    if (b) {
        Invalidate();
    }
    return false;
}

/**

int CGeoFile::createObj2Electro(const char* tn, CCoordList& cl, map<CString, COleVariant>& m_map_geo_value)
{
    CString q, q2;
    long affected;
    CNode2* node = NULL;
    CLINE2* line = NULL;
    q2.Format("");
    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (kls && kls->m_ado) {

        if (isPoint(kls->loc)) {
            CFPoint pt = cl.first();
            CGidrView* view = getView();

            if (view) {
            }
        }

        CString str = cl.getWKT(kls->loc, kls->m_ado->m_type_of_net);
        q.Format("INSERT INTO %s (shape) VALUES (%s)", tn, str);

        auto executeQuery = [&](CString q)-> int
        {
            long idv = ado_executeInsert(kls->m_ado, q, &affected);
            if (idv) {
                if (kls->codlabel_color != "") {
                    cl.is_color = true;
                    cl.color = getGeoColor(1);
                }

                CGeoObject* g = new CGeoObject(idv, cl);
                kls->geo4.push_back(g);

                return kls->geo4.size();
            }
            else
                return 0;
        };

        int size = executeQuery(q);
        if (!q2.IsEmpty())
            executeQuery(q2);
        return size;
    }
    return 0;
}

*/

int isElPoint(const char* tn1);
int isElEnd(const char* tn1);

double getLengthLine(CLINE2* ll, BOOL ismap);


CNode2* find_next11(CGraph2* graph, CNode2* n1, CNode2* n2, CNode2* n0, double& dl)
{
    //  n1->isPjezo = true;

    if (inc(n1) != 2) return n1;
    //  if (n1->namePS != "") return n1;
    if (n1->isKam()) return n1;

    for (CLINE2* l = n1->lines; l; l = next(l)) {
        CNode2* o = other(l);
        if (o != n2 && o != n0) {
            //      bline(l)->isPjezo = true;

            dl += getLengthLine(l, false);

            return find_next11(graph, o, n1, n0, dl);
        }
    }
    return NULL;
}



void get_dop_s(const map<CString, COleVariant>& m_map_geo_value, CString& dop_p1, CString& dop_p2, CString codlabel, CString& label)
{
    dop_p1 = "";
    dop_p2 = "";

    for (auto it : m_map_geo_value) {
        dop_p1 += ",";
        dop_p2 += ",";
        dop_p1 += it.first;
        dop_p2 += "'";

        CString s = CCrack::strVARIANT2(it.second);

        dop_p2 += s;
        dop_p2 += "'";

        if (codlabel == it.first) {
            label = it.second;
        }
    }
}

int color_open_close(int stateID);

int CGeoFile::createObj2(const char* tn, CCoordList& cl, map<CString, COleVariant>& m_map_geo_value1)
{
    CString label = "";
    CString dop_p1 = "", dop_p2 = "";
    int id_opr = 0;
    CString q, q2;
    long affected;
    CNode2* node = NULL;
    CLINE2* line = NULL;
    q2.Format("");

    int po = 0;

    map<CString, COleVariant> m_map_geo_value = m_map_geo_value1;

    CGidrView* view = getView();
    if (!view) return 0;


    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (kls && kls->m_ado) {

        if (isPoint(kls->loc) && !isElPoint(tn) && !isElEnd(tn)) {
            CFPoint pt = cl.first();

            id_opr = view->m_id_opr;

            view->findNodeLine(pt, &node, &line, D5);

            if (line) {
                MySQLColumns* col = kls->m_ado->findMySQLColumns(kls->nazv, "lineID");
                if (!col) {
                    line = NULL;
                }
            }
            if (node) {
                MySQLColumns* col = kls->m_ado->findMySQLColumns(kls->nazv, "nodeID");
                if (!col) {
                    node = NULL;
                }
            }
        }

        if (strcmp("defect", tn) == 0) {
            if (view->m_id_opr > 0) {
                m_map_geo_value["opresID"] = (long)view->m_id_opr;
                m_map_geo_value["remontTypeID"] = 2L;
            }
            else if (view->m_id_osmotr2 > 0) {
                m_map_geo_value["osmotrID"] = (long)view->m_id_osmotr2;
                m_map_geo_value["remontTypeID"] = 6L;
            }
            else {
                auto it = m_map_geo_value.find("remontTypeID");
                if (it == m_map_geo_value.end()) {
                    m_map_geo_value["remontTypeID"] = 1L;
                }
            }
            m_map_geo_value["stateID"] = 1L;
        }


        CString str = cl.getWKT(kls->loc, kls->m_ado->m_type_of_net);
        if (node) {
            q.Format("INSERT INTO %s (nodeID, shape) VALUES (%d, %s)", tn, node->id, str);
        }
        else if (line && (kls->m_ado->findMySQLColumns(tn, "priznak_truboprovoda") || strcmp("indikator_korrozii", tn) == 0)) {
            int nomP = bline(line)->line.nomP;
            int nomO = bline(line)->line.nomO;

            CNode2* n1 = where(line);
            CNode2* n2 = other(line);

            double dl1 = 0, dl2 = 0;

            CNode2* nn1 = n1->isKam() ? n1 : find_next11(n1->node.m_graph, n1, n2, n1, dl1);
            CNode2* nn2 = n2->isKam() ? n2 : find_next11(n2->node.m_graph, n2, n1, n2, dl2);

            CFPoint p = cl.first();

            CString kod0 = "", uzel0 = "";
            double dl1_w = rasst(line, p);
            double dl2_w = getLengthLine(line, false) - dl1_w;

            dl1 += dl1_w;
            dl2 += dl2_w;

            double dl;

            if (dl1 < dl2) {
                dl = dl1;
                if (nn1) {
                    kod0 = nn1->getKod();
                    uzel0 = nn1->node.name;
                }
            }
            else {
                dl = dl2;
                if (nn2) {
                    kod0 = nn2->getKod();
                    uzel0 = nn2->node.name;
                }
            }

            if (strcmp("indikator_korrozii", tn) == 0) {
                m_map_geo_value["kod_rs_blizhayshey_kamery"] = kod0;
                m_map_geo_value["blizhayshaya_kamera"] = uzel0;

                CString dls;
                dls.Format("%g", dl);

                m_map_geo_value["rasstoyanie_do_kamery__m"] = dls;
            }

            get_dop_s(m_map_geo_value, dop_p1, dop_p2, kls->codlabel, label);


            if ((nomP != nomO && nomP != -1 && nomO != -1) || (nomP == nomO)) {
                CMMenu menu(AfxGetMainWnd(), IDD_MENU);
                menu.lst.Add(_TR("Подающий"));
                menu.lst.Add(_TR("Обратный"));
                if ((nomP == nomO) && strcmp("indikator_korrozii", tn) != 0) {
                    menu.lst.Add(_TR("Общий"));
                    menu.lst.Add(_TR("Подающий + Обратный"));
                }

                CString priznakTruboprovoda = "priznak_truboprovoda";
                if (strcmp("indikator_korrozii", tn) == 0)
                    priznakTruboprovoda = "truboprovod";
                if (menu.DoModal() == IDOK) {

                    switch (menu.m_Index) {
                    case 0: q.Format("INSERT INTO %s (lineID, shape, %s%s) VALUES (%d, %s, %d%s)", tn, priznakTruboprovoda, dop_p1, nomP, str, 2, dop_p2);
                        po = 2;
                        break;
                    case 1: q.Format("INSERT INTO %s (lineID, shape, %s%s) VALUES (%d, %s, %d%s)", tn, priznakTruboprovoda, dop_p1, nomO, str, 3, dop_p2);
                        po = 3;
                        break;
                    case 2: q.Format("INSERT INTO %s (lineID, shape, %s%s) VALUES (%d, %s, %d%s)", tn, priznakTruboprovoda, dop_p1, nomO, str, 1, dop_p2);
                        po = 1;
                        break;
                    case 3: q.Format("INSERT INTO %s (lineID, shape, %s%s) VALUES (%d, %s, %d%s)", tn, priznakTruboprovoda, dop_p1, nomP, str, 2, dop_p2);
                        po = 0;
                        q2.Format("INSERT INTO %s (lineID, shape, %s%s) VALUES (%d, %s, %d%s)", tn, priznakTruboprovoda, dop_p1, nomO, str, 3, dop_p2);
                        break;
                    }
                }
                else {
                    return false;
                }
            }
            else
            {
                int id = bline(line)->line.nomP > 0 ? bline(line)->line.nomP : bline(line)->line.nomO;
                int type = bline(line)->line.nomP > 0 ? 2 : 3;

                CString priznakTruboprovoda = "priznak_truboprovoda";
                if (strcmp("indikator_korrozii", tn) == 0)
                    priznakTruboprovoda = "truboprovod";

                q.Format("INSERT INTO %s (lineID, shape, %s%s) VALUES (%d, %s, %d%s)", tn, priznakTruboprovoda, dop_p1, id, str, type, dop_p2);
            }

            /*else if (line) {
                int id = bline(line)->line.nomP > 0 ? bline(line)->line.nomP : bline(line)->line.nomO;
                q.Format("INSERT INTO %s (lineID, shape) VALUES (%d, %s)", tn, id, str);
            }*/
        }
        else {
            get_dop_s(m_map_geo_value, dop_p1, dop_p2, kls->codlabel, label);
            q.Format("INSERT INTO %s (shape%s) VALUES (%s%s)", tn, dop_p1, str, dop_p2);
        }

        auto executeQuery = [&](CString q, CString tn)-> int
        {
            long idv = ado_executeInsert(kls->m_ado, q, &affected);
            if (idv) {
                addGeoAdd(kls->m_ado, tn, idv);
                
                if (kls->codlabel_color != "") {
                    cl.is_color = true;
                    cl.color = getGeoColor(1);
                    if (tn == "indikator_korrozii") {
                        cl.color = getGeoColor(4);
                    }
                    cl.po = po;
                }

                if (kls->codlabel_color2 != "") {
                    cl.is_color2 = true;
                    cl.color2 = getGeoColor(1);
                    cl.po = po;

                    int get_colorID2();
                    int get_colorID1();

                    cl.color = getGeoColor(get_colorID1());
                    cl.color2 = getGeoColor(get_colorID2());
                }

                if (kls->label_stateID) {
                    CString val;
                    cl.stateID = 1;
                    cl.color = color_open_close(cl.stateID);
                }

                CGeoObject* g = new CGeoObject(idv, cl);
                if (!g) return 0;
                g->text00000 = label;

                kls->geo4.push_back(g);

                return kls->geo4.size();
            }
            else
                return 0;
        };

        int size = executeQuery(q, tn);
        if (!q2.IsEmpty()) {
            executeQuery(q2, tn);
        }
        return size;
    }
    return 0;
}



int copyZdan(const char* name1, int id, const char* name2)
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(transl(name1));
    if (!kls) return -1;

    Klassif* kls2 = m_geofile->m_kl_list->findKlN(transl(name2));
    if (!kls2) return -1;


    int id2 = -1;

    CAdoFile* ado = kls->m_ado;

    CGeoObject* geo = kls->getGeoObjectById(id);

    if (!geo)  return -1;


    CString q;
    q.Format("SELECT * FROM %s WHERE ID=%d", name1, id);

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            CCoordList cl = geo->cl;

            CString gr = ado->readStr(transl("Город"));
            CString mk = ado->readStr(transl("ID_ADR_MAS"));
            CString st = ado->readStr(transl("STREET_NAM"));
            CString dm = ado->readStr(transl("NUMBER_1"));

            if (st == "" && dm == "") {
                gr = ado->readStr(transl("Город"));
                mk = ado->readStr(transl("Микрорайон"));
                st = ado->readStr(transl("Улица"));
                dm = ado->readStr(transl("Дом"));
            }

            q.Format("INSERT INTO %s (%s,%s,%s,%s,shape) VALUES ('%s','%s','%s','%s', %s)",
                transl(name2),
                transl("Город"),
                transl("Микрорайон"),
                transl("Улица"),
                transl("Дом"),
                gr, mk, st, dm,
                cl.getWKT(kls->loc, kls->m_ado->m_type_of_net));

            long affected;
            id2 = ado_executeInsert(kls2->m_ado, q, &affected);
            if (id2) {
                CGeoObject* g = new CGeoObject(id2, cl);
                kls2->geo4.push_back(g);
            }
        }
        ado->closeTable();
    }
    return id2;
}
