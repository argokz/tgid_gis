#include "stdafx.h"
#include "ado.h"

#include "graph2.h"
#include "geofile.h"

#include "gidrView.h"

#include "resource.h"
#include "TaskProgressDlg.h"
#include "debugapi.h"

#include "InputDia.h"



void copyPTS(CAdoFile* ado)
{
    long affected;
    CString par = "0";

    CGidrView* pView = getView();
    if (pView) {
        par = pView->m_cxema->m_par;
    }

    CString q;

    q.Format(
        "DISABLE TRIGGER PipeSections_UPDATE ON pipeSections;\n"

        "UPDATE ps1 \n"
        "SET inventNumber=hps.inventNumber,\n"
        "    objectTypeID=hps.objectTypeID,\n"
        "    Sreda=hps.Sreda,\n"
        "    press=hps.press,\n"
        "    temperature=hps.temperature,\n"
        "    SpokSluzhbiRash=hps.SpokSluzhbiRash,\n"
        "    ressurs=hps.ressurs,\n"
        "    pusk=hps.pusk,\n"
        "    organizationID=hps.organizationID,\n"
        "    Elektrich=hps.Elektrich,\n"
        "    TransportElekricht=hps.TransportElekricht,\n"
        "    Ponezial=hps.Ponezial,\n"
        "    floodIntensityGWid=hps.floodIntensityGWid,\n"
        "    floodIntensityFWid=hps.floodIntensityFWid,\n"
        "    floodIntensityTWid=hps.floodIntensityTWid,\n"
        "    floodIntensityBWid=hps.floodIntensityBWid,\n"
        "    Vodootved=hps.Vodootved,\n"
        "    SlozhOkluzh=hps.SlozhOkluzh,\n"
        "    OtkluzhGkal=hps.OtkluzhGkal,\n"
        "    AvariiVipoln=hps.AvariiVipoln,\n"
        "    Pesehod=hps.Pesehod,\n"
        "    ZhdDorogi=hps.ZhdDorogi,\n"
        "    LudiProklad=hps.LudiProklad,\n"
        "    UzherbLudi=hps.UzherbLudi,\n"
        "    UzherbSity=hps.UzherbSity,\n"
        "    VnesniiVid=hps.VnesniiVid,\n"
        "    SostOborudovania=hps.SostOborudovania,\n"
        "    VnesnKorrozia=hps.VnesnKorrozia,\n"
        "    VnunrenKorrozia=hps.VnunrenKorrozia,\n"
        "    SostKonstrukz=hps.SostKonstrukz,\n"
        "    Kategorii=hps.Kategorii\n"
        "FROM pipeSections ps1\n"
        "JOIN sortLinesForUchastok srt ON srt.pipeSectionID = ps1.id\n"
        "JOIN linesobj l ON l.id=srt.lineID\n"
        "JOIN heatPipeSections hps ON hps.lineID=l.id\n"
        "JOIN nodes n1 ON n1.id=l.nodeID1\n"
        "JOIN nodes n2 ON n2.id=l.nodeID2\n"
        "WHERE n1.fileID in (%s);\n"

        "ENABLE TRIGGER PipeSections_UPDATE ON pipeSections;\n"
        , par);

    if (!ado->Execute(q, &affected)) {
        AfxMessageBox(ado->getError(), MB_OK | MB_ICONINFORMATION);
    }
}


CString getTemp(CAdoFile* ado)
{
    CString tn = "temp_table";

    if (ado->m_type_of_net == TYPE_OF_NET_MSSQL) {
        //        int r = rand();
        //        tn = "temp_table";
//        tn = "##temp_table";
        tn = "temp_table";
        // Вернуть после того как Астана избавиться от 12 версии
//        tn = "##temp_table";
        //        tn.Format("#temp%d", r);

        CString user = "0";

        CGidrView* pView = getView();
        if (pView) {
            transl(pView->GetDocument()->m_user_gid);
        }

        //        tn.Format("temp_table_%s", user);
    }
    return tn;
}

bool create_vyd_line_table0(CAdoFile* ado, CGraph2* graph, CString tn)
{
    CWaitCursor wait;
    bool ret = false;
    long affected;
    CString sql;


    sql.Format("DROP TABLE %s", tn);
    ado->Execute(sql, &affected);

    sql.Format("CREATE TEMPORARY TABLE %s (ID INT NOT NULL, PRIMARY KEY (ID))", tn);
    ado->Execute(sql, &affected);

    CMapGraph::const_iterator p;

    CAdoInsert ins(ado, tn, "id");

    bool is_vyd = false;
    p = graph->map_node.begin();
    while (p != graph->map_node.end()) {
        CNode2* n = p->second;
        for (CLINE2* l = n->lines; l; l = next(l)) {
            if (IsBegin(l) && bline(l)->line.isPjezo) {
                if (bline(l)->line.nomP > 0) {
                    if (ins.ado_insert_int1(bline(l)->line.nomP)) ret = true;
                }

                if (bline(l)->line.nomO > 0 && bline(l)->line.nomP != bline(l)->line.nomO) {
                    if (ins.ado_insert_int1(bline(l)->line.nomO)) ret = true;
                }
            }
        }
        p++;
    }
    if (ins.ado_insert()) ret = true;
    return ret;
}

bool create_vyd_line_tuple(CAdoFile* ado, CGraph2* graph, CString& q)
{
    CWaitCursor wait;
    CString s;
    q = "";

    CMapGraph::const_iterator p;

    bool is_vyd = false;
    p = graph->map_node.begin();
    while (p != graph->map_node.end()) {
        CNode2* n = p->second;
        for (CLINE2* l = n->lines; l; l = next(l)) {
            if (IsBegin(l) && bline(l)->line.isPjezo) {
                if (bline(l)->line.nomP > 0) {
                    s.Format("%d", bline(l)->line.nomP);
                    if (q != "") q += ",";
                    q += s;
                }
                if (bline(l)->line.nomO > 0 && bline(l)->line.nomP != bline(l)->line.nomO) {
                    s.Format("%d", bline(l)->line.nomO);
                    if (q != "") q += ",";
                    q += s;
                }
            }
        }
        p++;
    }
    return true;
}


bool create_vyd_line_table(CAdoFile* ado, CGraph2* graph, CString& tn)
{
    tn = getTemp(ado);

    return create_vyd_line_table0(ado, graph, tn);
}

bool create_vyd_node_table0(CAdoFile* ado, CGraph2* graph, CString tn)
{
    CWaitCursor wait;
    bool ret = false;
    long affected;
    CString sql;

    CAdoInsert ins(ado, tn, "id");

    sql.Format("DROP TABLE %s", tn);
    if (!ado->Execute(sql, &affected)) {
        //        AfxMessageBox(sql + "\n" + ado->getError(), MB_OK|MB_ICONINFORMATION);
    }

    sql.Format("CREATE TEMPORARY TABLE %s (ID INT NOT NULL, PRIMARY KEY (ID))", tn);
    if (!ado->Execute(sql, &affected)) {
        AfxMessageBox(sql + "\n" + ado->getError(), MB_OK | MB_ICONINFORMATION);
    }

    CMapGraph::const_iterator p;

    bool is_vyd = false;
    p = graph->map_node.begin();
    while (p != graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.isPjezo) {
            if (ins.ado_insert_int1(n->id)) ret = true;
        }
        p++;
    }
    if (ins.ado_insert()) ret = true;
    return ret;
}


bool create_vyd_node_tuple(CAdoFile* ado, CGraph2* graph, CString& q)
{
    CWaitCursor wait;
    CString s;
    q = "";

    CMapGraph::const_iterator p;

    bool is_vyd = false;
    p = graph->map_node.begin();
    while (p != graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.isPjezo) {
            s.Format("%d", n->id);
            if (q != "") q += ",";
            q += s;
        }
        p++;
    }
    return true;
}


bool create_vyd_node_table(CAdoFile* ado, CGraph2* graph, CString& tn)
{
    tn = getTemp(ado);
    return create_vyd_node_table0(ado, graph, tn);
}

bool getVydRectAllMSRS(CGraph2* graph, CFRect& rect0, int mag_typ, int mag_id);

int makeGeoTemp(CGraph2* graph, Klassif* kls, CString& vyd)
{
    CString sql;
    CFRect rectAll;
    long affected;
    CFRect rect, r;
    bool ret;
    int qq = 0;
    int mag_typ = 0;
    int mag_id = 0;



    if (!getVydRectAllMSRS(graph, rectAll, mag_typ, mag_id)) {
        return 0;
    }

    CAdoFile* ado = getAdo(getPsAdoName());
    if (ado) {
        vyd = getTemp(ado);

        sql.Format("DROP TABLE %s", vyd);
        ado->Execute(sql, &affected);

        sql.Format("CREATE TEMPORARY TABLE %s (id int)", vyd);
        ado->Execute(sql, &affected);

        sql.Format("SELECT * FROM %s", kls->nazv);

        //        if (ado->openTable0(sql)) {

//        m_geofile->m_kls000 = kls;

        int nn = kls->NObj();

        CAdoInsert ins(ado, vyd, "id");

        for (int i = 1; i <= nn; i++) {
            m_geofile->GetFRect(kls, i, rect);

            if (!r.IntersectRect(rect, rectAll)) continue;

            CMapGraph::iterator p = graph->map_node.begin();

            while (p != graph->map_node.end()) {
                CNode2* n = p->second;

                for (CLINE2* l = n->lines; l; l = next(l)) {
                    if (bline(l)->line.isPjezo && IsBegin(l)) {

                        CCoordList cl;
                        m_geofile->GetCoord(kls, i, cl);

                        //                double md = 5*masx;
                        double md = LEN100;
                        CFPoint pt = cl.first();

                        double len = getLength(l, pt);

                        if (len < md) {
                            kls->geo4[i - 1]->mark = 1;
                            if (ins.ado_insert_int1(kls->geo4[i - 1]->nom)) ret = true;

                            qq++;

                            goto QUIT;
                        }
                    }
                }
                p++;
            }
            if (ins.ado_insert()) ret = true;

        QUIT:
            qq;
            //    qq ++;
        }
        //            ado->closeTable();
        //        }
    }
    return qq;
}


void copyVydGraph_new(CGraph2* graph1, CGraph2* graph2, bool is_po = false);
void sort_line_rs_new(CGraph2* graph, list<CNode2*>& list_sort, list<CLINE2*>& st_l, int fileID = -1);
bool isPSstrict(CNode2* node);



bool create_vyd_line_table0_new(CAdoFile* ado, CGraph2* graph, CString tn)
{
    CWaitCursor wait;
    bool ret = false;
    long affected;
    CString sql;

    sql.Format("DROP TABLE %s", tn);
    ado->Execute(sql, &affected);

    //    sql.Format("CREATE TEMPORARY TABLE %s (ID1 INT NOT NULL, ID2 INT NOT NULL, PRIMARY KEY (ID1))", tn);
    sql.Format("CREATE TEMPORARY TABLE %s (ID1 INT NOT NULL, ID2 INT NOT NULL, PRIMARY KEY (ID1))", tn, tn);
    ado->Execute(sql, &affected);

    CGraph2 graph2(NULL);
    list<CNode2*> list_sort;
    list<CLINE2*> st_l;

    copyVydGraph_new(graph, &graph2, true);
    sort_line_rs_new(&graph2, list_sort, st_l);

    CNode2* n1_old = nullptr, * n2_old = nullptr;
    CNode2* n_old = nullptr;

    list <list<int> > list_ut;
    list<int> list_ut_min;

    int nn = 1;

    CAdoInsert ins(ado, tn, "id1,id2");

    graph->init_find_line_nom();

    for (auto p : st_l) {
        CLINE2* l = p;

        CLINE2* lP = graph->find_line_nom(bline(l)->line.nomP);
        CLINE2* lO = graph->find_line_nom(bline(l)->line.nomO);

        if (lP || lO) {
            CNode2* n1 = where(l);
            CNode2* n2 = other(l);

            if (n1_old && (isPSstrict(n1) || n1 != n2_old)) {
                for (auto it : list_ut_min) {
                    if (ins.ado_insert_int2(nn++, it)) ret = true;
                }

                list_ut_min.clear();
            }

            list_ut_min.push_back(lP ? bline(lP)->line.nomP : bline(lO)->line.nomO);

            n1_old = n1;
            n2_old = n2;
        }
    }

    for (auto it : list_ut_min) {
        if (ins.ado_insert_int2(nn++, it)) ret = true;
    }
    if (ins.ado_insert()) ret = true;

    return ret;
}



bool create_vyd_line_table_new(CAdoFile* ado, CGraph2* graph, CString& tn)
{
    tn = getTemp(ado);

    return create_vyd_line_table0_new(ado, graph, tn);
}



/*
Простая сортировка без ПТС
*/



bool create_vyd_line_table0_new_simple(CAdoFile* ado, CGraph2* graph, CString tn)
{
    CWaitCursor wait;
    bool ret = false;
    long affected;
    CString sql;

    sql.Format("DROP TABLE %s", tn);
    ado->Execute(sql, &affected);

    sql.Format("CREATE TEMPORARY TABLE %s (ID1 INT NOT NULL, ID2 INT NOT NULL, PRIMARY KEY (ID1))", tn, tn);
    ado->Execute(sql, &affected);

    CGraph2 graph2(NULL);
    list<CNode2*> list_sort;
    list<CLINE2*> st_l;

    copyVydGraph_new(graph, &graph2, true);
    sort_line_rs_new(&graph2, list_sort, st_l);

    CNode2* n1_old = nullptr, * n2_old = nullptr;
    CNode2* n_old = nullptr;

    list <list<int> > list_ut;
    list<int> list_ut_min;

    int nn = 1;

    CAdoInsert ins(ado, tn, "id1,id2");

    graph->init_find_line_nom();

    for (auto p : st_l) {
        CLINE2* l = p;

        CLINE2* lP = graph->find_line_nom(bline(l)->line.nomP);
        CLINE2* lO = graph->find_line_nom(bline(l)->line.nomO);

        if (lP || lO) {
            CNode2* n1 = where(l);
            CNode2* n2 = other(l);

            //        if (n1_old && (isPSstrict(n1) || n1 != n2_old)) {
            if (n1_old && (n1 != n2_old)) {
                for (auto it : list_ut_min) {
                    if (ins.ado_insert_int2(nn++, it)) ret = true;
                }

                list_ut_min.clear();
            }

            list_ut_min.push_back(lP ? bline(lP)->line.nomP : bline(lO)->line.nomO);

            n1_old = n1;
            n2_old = n2;
        }
    }

    for (auto it : list_ut_min) {
        if (ins.ado_insert_int2(nn++, it)) ret = true;
    }
    if (ins.ado_insert()) ret = true;

    return ret;
}

bool create_vyd_line_table_new_simple(CAdoFile* ado, CGraph2* graph, CString& tn)
{
    tn = getTemp(ado);

    return create_vyd_line_table0_new_simple(ado, graph, tn);
}


void copyVydGraph_fileID(CGraph2* graph1, CGraph2* graph2, bool is_po, int fileID)
{
    CMapGraph::iterator p = graph1->map_node.begin();
    while (p != graph1->map_node.end()) {
        CNode2* n = p->second;
        //    if (n->node.isPjezo) {
        if (n->node.fileID == fileID) {
            CNode2* n2 = graph2->find_ins(n->id);
            if (n2) {
                n2->node = n->node;
            }
        }
        p++;
    }

    p = graph1->map_node.begin();

    while (p != graph1->map_node.end()) {
        CNode2* n = p->second;

        //    if (n->node.isPjezo) {
        if (n->node.fileID == fileID) {
            CNode2* n2 = graph2->find(n->id);

            if (n2) {
                for (CLINE2* l = n->lines; l; l = next(l)) {
                    //          if (bline(l)->line.isPjezo && IsBegin(l) && (!is_po || (bline(l)->line.nomP != -1 || bline(l)->line.nomO != -1) )  ) {
//                    if (IsBegin(l) && (!is_po || (bline(l)->line.nomP != -1 || bline(l)->line.nomO != -1))) {
                    if (IsBegin(l)) {
                        CNode2* o = other(l);
                        CNode2* o2 = graph2->find(o->id);
                        if (o2) {
                            CCoordList cl;

                            CLINE2* ll = graph2->insert_line(n2, o2, cl, true);
                            if (ll) {
                                bline(ll)->line = bline(l)->line;
                                //                bline(ll)->line.isPjezo = false;
                            }
                        }
                    }
                }
            }
        }
        p++;
    }
}



bool create_vyd_line_table0_new_all(CAdoFile* ado, CGraph2* graph, CString tn, int fileID)
{
    CWaitCursor wait;
    bool ret = false;
    long affected;
    CString sql;

    sql.Format("DROP TABLE %s", tn);
    ado->Execute(sql, &affected);

    sql.Format("CREATE TEMPORARY TABLE %s (ID1 INT NOT NULL, ID2 INT NOT NULL, PRIMARY KEY (ID1))", tn, tn);
    ado->Execute(sql, &affected);

    CGraph2 graph2(NULL);
    list<CNode2*> list_sort;
    list<CLINE2*> st_l;

    //    copyVydGraph_new(graph, &graph2, true);

    copyVydGraph_fileID(graph, &graph2, false, fileID);
    sort_line_rs_new(&graph2, list_sort, st_l);

    CNode2* n1_old = nullptr, * n2_old = nullptr;
    CNode2* n_old = nullptr;

    list <list<int> > list_ut;
    list<int> list_ut_min;

    int nn = 1;

    graph->init_find_line_nom();

    CAdoInsert ins(ado, tn, "id1,id2");

    for (auto p : st_l) {
        CLINE2* l = p;

        CLINE2* lP = graph->find_line_nom(bline(l)->line.nomP);
        CLINE2* lO = graph->find_line_nom(bline(l)->line.nomO);

        if (lP || lO) {
            CNode2* n1 = where(l);
            CNode2* n2 = other(l);

            //        if (n1_old && (isPSstrict(n1) || n1 != n2_old)) {
            if (n1_old && (n1 != n2_old)) {
                for (auto it : list_ut_min) {
                    if (ins.ado_insert_int2(nn++, it)) ret = true;
                }

                list_ut_min.clear();
            }

            list_ut_min.push_back(lP ? bline(lP)->line.nomP : bline(lO)->line.nomO);
            if (lP && lO && bline(lP)->line.nomP != bline(lP)->line.nomO) list_ut_min.push_back(bline(lO)->line.nomO);


            n1_old = n1;
            n2_old = n2;
        }
    }

    for (auto it : list_ut_min) {
        if (ins.ado_insert_int2(nn++, it)) ret = true;
    }
    if (ins.ado_insert()) ret = true;
    return ret;
}

bool create_vyd_line_table_new_all(CAdoFile* ado, CGraph2* graph, CString& tn, int fileID)
{
    tn = getTemp(ado);

    return create_vyd_line_table0_new_all(ado, graph, tn, fileID);
}


bool get_first_last(CGraph2* graph, list<int>& list_ut_min, CNode2*& n_first, CNode2*& n_last)
{
    CNode2* n1_old = nullptr, * n2_old = nullptr;
    n_first = nullptr;
    n_last = nullptr;

    if (list_ut_min.size() == 2) {
        CLINE2* ll[2];
        int i = 0;
        for (auto it : list_ut_min) {
            ll[i] = graph->find_line_nom(it);
            i++;
        }
        if (ll[0] == ll[1]) {
            CNode2* n1 = where(ll[0]);
            CNode2* n2 = other(ll[0]);

            if (bline(ll[0])->line.nomO == -1) {
                bline(ll[0])->line.nomO = bline(ll[1])->line.nomO;
            }
            if (bline(ll[0])->line.nomP == -1) {
                bline(ll[0])->line.nomP = bline(ll[1])->line.nomP;
            }
            auto it = list_ut_min.rbegin();
            list_ut_min.remove(*it);

            n_first = n1;
            n_last = n2;

            return true;
        }
    }



    auto it_last = list_ut_min.rbegin();

    CLINE2* l_old = nullptr;

    for (auto it : list_ut_min) {
        CLINE2* l = graph->find_line_nom(it);
        if (l && l == l_old) {
            CNode2* n1 = where(l);
            CNode2* n2 = other(l);
            n_first = n1;
            n_last = n2;
            list_ut_min.remove(it);
            break;
        }
        if (l && l != l_old) {
            l_old = l;
            CNode2* n1 = where(l);
            CNode2* n2 = other(l);

            if (n1_old && n2_old) {
                if (!n_first) {
                    if (n1 == n1_old || n2 == n1_old) n_first = n2_old;
                    if (n1 == n2_old || n2 == n2_old) n_first = n1_old;
                }

                if (it == *it_last) { //the last iteration
                    if (n1 == n1_old || n1 == n2_old) n_last = n2;
                    if (n2 == n1_old || n2 == n2_old) n_last = n1;
                }
            }

            if (it == *it_last && (!n_first || !n_last)) { //the last iteration
                n_first = n1;
                n_last = n2;
            }


            n1_old = n1;
            n2_old = n2;
        }
    }
    return true;
}


bool get_first_last_cl(CGraph2* graph, list<int>& list_ut_min, CNode2*& n_first, CNode2*& n_last, CCoordList& cl)
{
    CNode2* n1_old = nullptr, * n2_old = nullptr;
    n_first = nullptr;
    n_last = nullptr;

    auto it_last = list_ut_min.rbegin();

    CLINE2* l_old = nullptr;

    for (auto it : list_ut_min) {
        CLINE2* l = graph->find_line_nom(it);
        if (l && l == l_old) {
            CNode2* n1 = where(l);
            CNode2* n2 = other(l);
            n_first = n1;
            n_last = n2;
            list_ut_min.remove(it);
            break;
        }
        if (l && l != l_old) {
            CNode2* n1 = where(l);
            CNode2* n2 = other(l);

            CCoordList cl1 = bline(l)->getCLall();

            cl.insert(cl.end(), cl1.begin(), cl1.end());
            cl.push_back(CFPoint(C_SPR, C_SPR));

            if (n1_old && n2_old) {
                if (!n_first) {
                    if (n1 == n1_old || n2 == n1_old) n_first = n2_old;
                    if (n1 == n2_old || n2 == n2_old) n_first = n1_old;
                }

                if (it == *it_last) { //the last iteration
                    if (n1 == n1_old || n1 == n2_old) n_last = n2;
                    if (n2 == n1_old || n2 == n2_old) n_last = n1;
                }
            }

            if (it == *it_last && (!n_first || !n_last)) { //the last iteration
                n_first = n1;
                n_last = n2;
            }

            n1_old = n1;
            n2_old = n2;
        }
    }

    return true;
}



CLINE2* get_max(CGraph2* graph, list<int>& list_ut_min, double& dlP, double& dlO, double& qP, double& qO)
{
    CLINE2* lmax = nullptr;
    double len_max = -1;

    dlP = 0;
    dlO = 0;
    qP = 0;
    qO = 0;

    for (auto it : list_ut_min) {
        CLINE2* ll = graph->find_line_nom(it);
        if (ll) {
            double len = bline(ll)->line.pod.dl;

            dlP += bline(ll)->line.pod.dl;
            dlO += bline(ll)->line.obr.dl;
            qP += bline(ll)->line.pod.v;
            qO += bline(ll)->line.obr.v;

            if (len > len_max) {
                lmax = ll;
            }
        }
    }

    return lmax;
}

struct BigLine
{
    int n_maxP;
    int n_maxO;
    CNode2* n_first;
    CNode2* n_last;
    double lengthP;
    double lengthO;
    list<int> list_l;
};


bool copy_ut(CGraph2* graph, CString tn, int& nn, list<int>& list_ut_min, BigLine& bigline)
{
    CString sql;
    //    long affected;
    bool ret = false;
    double len_max = -1;

    CNode2* n_first = nullptr, * n_last = nullptr;

    get_first_last(graph, list_ut_min, n_first, n_last);

    int n_maxP = -1;
    int n_big_max = -1;

    double lengthP = 0, lengthO = 0;

    bigline.list_l.clear();

    for (auto it : list_ut_min) {
        CLINE2* ll = graph->find_line_nom(it);
        if (ll) {
            double lenP = bline(ll)->line.pod.dl;
            double lenO = bline(ll)->line.obr.dl;

            lengthP += lenP;
            lengthO += lenO;

            if (lenP > len_max || lenO > len_max) {
                n_maxP = it;
            }
            bigline.list_l.push_back(bline(ll)->line.idP2);
            bigline.list_l.push_back(bline(ll)->line.idO2);
        }
    }

    //    if (n_first == n_last) {
    //        lengthP /=2;
    //        lengthO /= 2;
    //    }

    if (n_maxP > 0 && n_first && n_last) {
        bigline.n_maxP = n_maxP;
        bigline.n_first = n_first;
        bigline.n_last = n_last;
        bigline.lengthP = lengthP;
        bigline.lengthO = lengthO;
    }
    return ret;
}

bool create_vyd_line_table0_BIG(CAdoFile* ado, CGraph2* graph, CString tn)
{
    CWaitCursor wait;
    bool ret = false;
    long affected;
    CString sql;
    BigLine bigline;
    list<BigLine> bigline_list;

    sql.Format("DROP TABLE %s", tn);
    ado->Execute(sql, &affected);

    sql.Format("CREATE TEMPORARY TABLE %s (ID1 INT NOT NULL, ID2 INT NOT NULL, N1 INT NOT NULL, N2 INT NOT NULL, lengthP FLOAT, lengthO FLOAT, fileID int, PRIMARY KEY (ID1)"
        ")", tn);
    ado->Execute(sql, &affected);

    CGraph2 graph2(NULL);
    list<CNode2*> list_sort;
    list<CLINE2*> st_l;

    copyVydGraph_new(graph, &graph2, true);
    sort_line_rs_new(&graph2, list_sort, st_l);

    CNode2* n1_old = nullptr, * n2_old = nullptr;
    CNode2* n_old = nullptr;

    list<list<int> > list_ut;
    list<int> list_ut_min;

    int nn = 1;

    graph->init_find_line_nom();

    for (auto p : st_l) {
        CLINE2* l = p;

        CLINE2* lP = graph->find_line_nom(bline(l)->line.nomP);
        CLINE2* lO = graph->find_line_nom(bline(l)->line.nomO);

        if (lP || lO) {
            CNode2* n1 = where(l);
            CNode2* n2 = other(l);

            if (n1_old && (isPSstrict(n1) || n1 != n2_old)) {
                ret = copy_ut(graph, tn, nn, list_ut_min, bigline);
                bigline_list.push_back(bigline);

                CNode2* nn1 = graph->find(bigline.n_first->id);
                CNode2* nn2 = graph->find(bigline.n_last->id);

                list_ut_min.clear();

                n2_old = other(l);
            }

            if (lP) {
                list_ut_min.push_back(bline(lP)->line.nomP);
            }
            if (lO && (lO != lP || bline(lP)->line.nomP != bline(lO)->line.nomO)) {
                list_ut_min.push_back(bline(lO)->line.nomO);
            }


            //            list_ut_min.push_back(lP ? bline(lP)->line.nomP : bline(lO)->line.nomO);



            n1_old = n1;
            n2_old = n2;
        }
    }
    if (list_ut_min.size() > 0) {
        ret = copy_ut(graph, tn, nn, list_ut_min, bigline);
        bigline_list.push_back(bigline);
    }

    CNode2* n1 = nullptr;
    CNode2* n2 = nullptr;



    auto pp = bigline_list.rbegin();

    for (auto p = bigline_list.begin(); p != bigline_list.end(); p++) {
        if (&*pp != &*p) {

            //                    auto p_next = *(&p + 1);
            //                    list<BigLine>::iterator it = *p;

            auto p_next = std::next(p, 1);

            if (p->n_first == p_next->n_first || p->n_first == p_next->n_last) {
                CNode2* nn = p->n_first; p->n_first = p->n_last; p->n_last = nn;
            }
        }

        if (p->n_last == n2) {
            CNode2* nn = p->n_first; p->n_first = p->n_last; p->n_last = nn;
        }

        n1 = p->n_first;
        n2 = p->n_last;
    }

    CAdoInsert ins(ado, tn, "id1,id2,n1,n2,lengthP,lengthO,fileID");

    int nnn = 1;

    for (auto& p : bigline_list) {
        CString sql;

        if (isPSstrict(p.n_first) && isPSstrict(p.n_last)) {
            sql.Format("%d,%d,%d,%d,%g,%g,%d",
                nnn++, p.n_maxP, p.n_first->id, p.n_last->id, p.lengthP, p.lengthO, p.n_first->node.fileID);

            if (ins.ado_insert(sql)) ret = true;
        }
    }
    if (ins.ado_insert()) ret = true;

    return ret;
}

bool create_vyd_line_table_BIG(CAdoFile* ado, CGraph2* graph, CString& tn)
{
    tn = getTemp(ado);

    return create_vyd_line_table0_BIG(ado, graph, tn);
}



bool create_vyd_node_table0_BIG(CAdoFile* ado, CGraph2* graph, CString tn, bool is_big)
{
    CWaitCursor wait;
    bool ret = false;
    long affected;
    CString sql;

    sql.Format("DROP TABLE %s", tn);
    ado->Execute(sql, &affected);

    sql.Format("CREATE TEMPORARY TABLE %s (ID1 INT NOT NULL, ID2 INT NOT NULL, fileID int, PRIMARY KEY (ID1))", tn);
    ado->Execute(sql, &affected);

    CGraph2 graph2(NULL);
    list<CNode2*> list_sort;
    list<CLINE2*> st_l;

    copyVydGraph_new(graph, &graph2, true);
    sort_line_rs_new(&graph2, list_sort, st_l);
    CNode2* n1_old = nullptr, * n2_old = nullptr;
    CNode2* n_old = nullptr;

    list<list<int> > list_ut;
    list<int> list_ut_min;

    int nn = 1;

    auto it_first = list_sort.begin();
    auto it_last = list_sort.rbegin();

    CAdoInsert ins(ado, tn, "id1,id2,fileID");

    for (auto p : list_sort) {
        CNode2* n = graph->find(p->id);

        //        if (isPSstrict(p) || p == *it_first || p == *it_last) {

        if (isPSstrict(p) || !is_big) {
            sql.Format("%d,%d,%d", nn++, n->id, n->node.fileID);
            if (ins.ado_insert(sql)) ret = true;
        }
    }
    if (ins.ado_insert()) ret = true;
    return ret;
}

bool create_vyd_node_table_BIG(CAdoFile* ado, CGraph2* graph, CString& tn, bool is_big)
{
    tn = getTemp(ado);

    return create_vyd_node_table0_BIG(ado, graph, tn, is_big);
}


bool create_vyd_node_table0_BIG_all(CAdoFile* ado, CGraph2* graph, CString tn, bool is_big, int fileID)
{
    CWaitCursor wait;
    bool ret = false;
    long affected;
    CString sql;

    sql.Format("DROP TABLE %s", tn);
    ado->Execute(sql, &affected);

    sql.Format("CREATE TEMPORARY TABLE %s (ID1 INT NOT NULL, ID2 INT NOT NULL, fileID int, PRIMARY KEY (ID1))", tn, tn, tn);
    ado->Execute(sql, &affected);

    CGraph2 graph2(NULL);
    list<CNode2*> list_sort;
    list<CLINE2*> st_l;

    //    copyVydGraph_new(graph, &graph2, true);

    copyVydGraph_fileID(graph, &graph2, false, fileID);
    
    sort_line_rs_new(&graph2, list_sort, st_l);
    CNode2* n1_old = nullptr, * n2_old = nullptr;
    CNode2* n_old = nullptr;

    list<list<int> > list_ut;
    list<int> list_ut_min;

    int nn = 1;

    auto it_first = list_sort.begin();
    auto it_last = list_sort.rbegin();

    CAdoInsert ins(ado, tn, "id1,id2,fileID");

    for (auto p : list_sort) {
        CNode2* n = graph->find(p->id);

        //        if (isPSstrict(p) || p == *it_first || p == *it_last) {

        if (isPSstrict(p) || !is_big) {
            sql.Format("%d,%d,%d", nn++, n->id, n->node.fileID);
            if (ins.ado_insert(sql)) ret = true;
        }
    }
    if (ins.ado_insert()) ret = true;
    return ret;
}

bool create_vyd_node_table_BIG_all(CAdoFile* ado, CGraph2* graph, CString& tn, bool is_big, int fileID)
{
    tn = getTemp(ado);

    return create_vyd_node_table0_BIG_all(ado, graph, tn, is_big, fileID);
}



CString int_to_s(int n)
{
    if (n == 0) return "NULL";

    CString s;
    s.Format("%d", n);

    return s;
}


#include "ps_alma.h"

int viewUchastok(CGraph2* m_graph, tuple<int, int> msrs, int type /*1 - Магистраль 2 Распредсеть 3 Магистраль и распредсеть*/, bool is_only_big_pipe);


#if 0

// Сортировка участков ПТС с удалением

void insertSortLinesToDb2(CAdoFile* ado, CGraph2* graph, CString tn, bool ms, map<int, int>& map_big, int id)
{
    BigLine bigline;
    list<BigLine> bigline_list;
    CGraph2 graph2(NULL);
    list<CNode2*> list_sort;
    list<CLINE2*> st_l;
    bool ret;

    CString sql;
    long affected;

    if (ms) {
        sql.Format("DELETE FROM pipeSections WHERE id IN (SELECT pipeSectionID FROM heatPipeSections hps WHERE hps.magistralSite=%d)", id);
        ret = ado->Execute(sql, &affected);

        sql.Format("DELETE FROM %s WHERE uchastok_ms_id=%d", tn, id);
        ret = ado->Execute(sql, &affected);
    }
    else {
        sql.Format("DELETE FROM pipeSections WHERE id IN (SELECT pipeSectionID FROM heatPipeSections hps WHERE hps.distSite=%d)", id);
        ret = ado->Execute(sql, &affected);

        sql.Format("DELETE FROM %s WHERE uchastok_rs_id=%d", tn, id);
        ret = ado->Execute(sql, &affected);
    }
    graph->reset();

    if (ms)
        viewUchastok(graph, make_tuple(id, 0), 1, false);
    else
        viewUchastok(graph, make_tuple(0, id), 2, false);


    copyVydGraph_new(graph, &graph2);
    sort_line_rs_new(&graph2, list_sort, st_l);

    CNode2* n1_old = nullptr, * n2_old = nullptr;
    CNode2* n_old = nullptr;

    list<list<int> > list_ut;
    list<int> list_ut_min;

    int nn = 1;

    for (auto p : st_l) {
        CLINE2* l = p;

        CLINE2* lP = graph->find_line_nom(bline(l)->line.nomP);
        CLINE2* lO = graph->find_line_nom(bline(l)->line.nomO);

        if (lP || lO) {

            CNode2* n1 = where(l);
            CNode2* n2 = other(l);

            if (n1_old && (isPSstrict(n1) || n1 != n2_old)) {
                ret = copy_ut(graph, tn, nn, list_ut_min, bigline);
                bigline_list.push_back(bigline);
                list_ut_min.clear();
            }

            if (lP) {
                list_ut_min.push_back(bline(lP)->line.nomP);
            }
            if (lO && (lO != lP || bline(lP)->line.nomP != bline(lO)->line.nomO)) {
                list_ut_min.push_back(bline(lO)->line.nomO);
            }

            n1_old = n1;
            n2_old = n2;
        }
    }
    if (list_ut_min.size() > 0) {
        ret = copy_ut(graph, tn, nn, list_ut_min, bigline);
        bigline_list.push_back(bigline);
    }
    if (bigline_list.size() > 0)
    {
        CNode2* n1 = nullptr;
        CNode2* n2 = nullptr;

        auto pp = bigline_list.rbegin();

        for (auto p = bigline_list.begin(); p != bigline_list.end(); p++) {
            if (&*pp != &*p) {
                auto p_next = std::next(p, 1);

                if (p->n_first == p_next->n_first || p->n_first == p_next->n_last) {
                    CNode2* nn = p->n_first; p->n_first = p->n_last; p->n_last = nn;
                }
            }

            if (p->n_last == n2) {
                CNode2* nn = p->n_first; p->n_first = p->n_last; p->n_last = nn;
            }

            n1 = p->n_first;
            n2 = p->n_last;
        }

        int nnn = 1;

        for (auto& p : bigline_list) {
            CString sql;
            long affected;

            if (isPSstrict(p.n_first) && isPSstrict(p.n_last)) {

                CLINE2* line = graph->find_line_nom(p.n_maxP);

                if (line) {
                    CLine* l = &bline(line)->line;

                    CString ms = int_to_s(l->ms);
                    CString rs = int_to_s(l->rs);
                    CString mag = int_to_s(l->mag);
                    CString tubingTypeID = int_to_s(l->tubingTypeID);
                    int DiamUslov = l->pod.diam_usl;
                    double length = p.lengthP;

                    if (l->nomP == -1) {
                        DiamUslov = l->obr.diam_usl;
                        length = p.lengthO;
                    }

                    sql.Format("INSERT INTO pipeSections (nodeID1,nodeID2,pipeLength,magistral,magistralSite,distSite,tubingTypeID, DiamUslov) VALUES (%d,%d,%g,%s,%s,%s,%s,%d)",
                        p.n_first->id, p.n_last->id, length, mag, ms, rs, tubingTypeID, DiamUslov);
                }
                else {
                    sql.Format("INSERT INTO pipeSections (nodeID1,nodeID2,pipeLength) VALUES (%d,%d,%g)",
                        p.n_first->id, p.n_last->id, p.lengthP);
                }

                int pipeSectionID = ado->ExecuteInsert(sql, &affected);

                if (affected != 1) {
                    AfxMessageBox(sql, MB_OK | MB_ICONINFORMATION);
                }

                for (auto& pp : p.list_l) {
                    if (pp > 0) {
                        map_big[pp] = pipeSectionID;
                    }
                }

                CString column_name = ms ? "uchastok_ms_id" : "uchastok_rs_id";


                double length = p.lengthP > 0 ? p.lengthP : p.lengthO;


                sql.Format("INSERT INTO %s (orderID, pipeSectionID, lineID, nodeID1, nodeID2, totalLength, fileID, %s) VALUES (%d,%d,%d,%d,%d,%g,%d,%d)",
                    tn, column_name, nnn++, pipeSectionID, p.n_maxP, p.n_first->id, p.n_last->id, length, p.n_first->node.fileID, id);

                if (ado->Execute(sql, &affected) && affected) {
                    ret = true;
                }


                CLINE2* l = graph->find_line_nom(p.n_maxP);

                if (l && bline(l)->line.nomP != -1 && bline(l)->line.nomO != -1 && bline(l)->line.nomP != bline(l)->line.nomO) {

                    double length = p.lengthO;

                    //                    sql.Format("INSERT INTO %s (orderID, pipeSectionID, lineID, nodeID1, nodeID2, totalLength, fileID, %s) VALUES (%d,%d,%d,%d,%d,%g,%d,%d)",
                    //                        tn, column_name, nnn++, pipeSectionID, bline(l)->line.nomO, p.n_first->id, p.n_last->id, length, p.n_first->node.fileID, id);

                    //                    if (ado->Execute(sql, &affected) && affected) {
                    //                        ret = true;
                    //                    }

                    CString h;
                    h.Format("orderID, pipeSectionID, lineID, nodeID1, nodeID2, totalLength, fileID, %s", column_name);

                    sql.Format("%d,%d,%d,%d,%d,%g,%d,%d",
                        nnn++, pipeSectionID, bline(l)->line.nomO, p.n_first->id, p.n_last->id, length, p.n_first->node.fileID, id);

                    if (ins.ado_insert(tn, h, sql)) ret = true;
                }
            }
            if (ins.ado_insert()) ret = true;
        }
    }
    graph2.clear();
}

void set_map_big(CAdoFile* ado, CGraph2* graph, map<int, int>& map_big)
{
    graph->init_find_line_nom_hps();

    long affected;
    CString sql;
#if 0
    CString tn = getTemp(ado);

    sql.Format("DROP TABLE %s", tn);
    ado->Execute(sql, &affected);

    sql.Format("CREATE TEMPORARY TABLE %s (ID1, ID2)", );
    ado->Execute(sql, &affected);
    CString sql0 = "";
    sql.Format("INSERT INTO %s (ID1, ID2) VALUES ", );

    for (auto p : map_big) {
        CString ss;
        if (sql0 != "") sql0 += ",";
        ss.Format("(%d,%d)", p.first, p.second);
        sql0 += ss;
    }

    sql += sql0;

    if (ado->Execute(sql, &affected)) {
        for (auto p : map_big) {
            CString sql;
            long affected;

            CLINE2* l = graph->find_line_nom_hps(p.first);

            if (l) {
                bline(l)->line.pipeSectionID = p.second;
            }
        }
    }
#endif
    for (auto p : map_big) {
        CString sql;
        long affected;

        CLINE2* l = graph->find_line_nom_hps(p.first);
        sql.Format("UPDATE heatPipeSections SET pipeSectionID=%d WHERE id=%d", p.second, p.first);

        if (ado->Execute(sql, &affected)) {
            if (l) {
                bline(l)->line.pipeSectionID = p.second;
            }
        }
    }
}



#else


// Сортировка учатков ПТС без удаления (попытка)

void insertSortLinesToDb2(CAdoFile* ado, CGraph2* graph, CString tn, bool ms, map<int, int>& map_big, int id)
{
    BigLine bigline;
    list<BigLine> bigline_list;
    CGraph2 graph2(NULL);
    list<CNode2*> list_sort;
    list<CLINE2*> st_l;
    bool ret;

    CString sql;
    long affected;

    if (ms) {
        sql.Format("DELETE FROM %s WHERE uchastok_ms_id=%d", tn, id);
    }
    else {
        sql.Format("DELETE FROM %s WHERE uchastok_rs_id=%d", tn, id);
    }
    ret = ado->Execute(sql, &affected);
    graph->reset();

    if (ms)
        viewUchastok(graph, make_tuple(id, 0), 1, false);
    else
        viewUchastok(graph, make_tuple(0, id), 2, false);

    copyVydGraph_new(graph, &graph2);
    sort_line_rs_new(&graph2, list_sort, st_l);

    CNode2* n1_old = nullptr, * n2_old = nullptr;
    CNode2* n_old = nullptr;

    list<list<int> > list_ut;
    list<int> list_ut_min;

    int nn = 1;

    graph->init_find_line_nom();

    for (auto p : st_l) {
        CLINE2* l = p;

        CLINE2* lP = graph->find_line_nom(bline(l)->line.nomP);
        CLINE2* lO = graph->find_line_nom(bline(l)->line.nomO);

        if (lP || lO) {

            CNode2* n1 = where(l);
            CNode2* n2 = other(l);

            if (n1_old && (isPSstrict(n1) || n1 != n2_old)) {
                ret = copy_ut(graph, tn, nn, list_ut_min, bigline);
                bigline_list.push_back(bigline);
                list_ut_min.clear();
            }

            if (lP) {
                list_ut_min.push_back(bline(lP)->line.nomP);
            }
            if (lO && (lO != lP || bline(lP)->line.nomP != bline(lO)->line.nomO)) {
                list_ut_min.push_back(bline(lO)->line.nomO);
            }

            n1_old = n1;
            n2_old = n2;
        }
    }
    if (list_ut_min.size() > 0) {
        ret = copy_ut(graph, tn, nn, list_ut_min, bigline);
        bigline_list.push_back(bigline);
    }
    if (bigline_list.size() > 0)
    {
        CNode2* n1 = nullptr;
        CNode2* n2 = nullptr;

        auto pp = bigline_list.rbegin();

        for (auto p = bigline_list.begin(); p != bigline_list.end(); p++) {
            if (&*pp != &*p) {
                auto p_next = std::next(p, 1);

                if (p->n_first == p_next->n_first || p->n_first == p_next->n_last) {
                    CNode2* nn = p->n_first; p->n_first = p->n_last; p->n_last = nn;
                }
            }

            if (p->n_last == n2) {
                CNode2* nn = p->n_first; p->n_first = p->n_last; p->n_last = nn;
            }

            n1 = p->n_first;
            n2 = p->n_last;
        }

        int nnn = 1;

        CString column_name = ms ? "uchastok_ms_id" : "uchastok_rs_id";

        CString h;
        h.Format("orderID, pipeSectionID, lineID, nodeID1, nodeID2, totalLength, fileID, %s", column_name);


        CAdoInsert ins_sort(ado, tn, h);
        //        CAdoInsert ins_pipe(ado, "pipeSections", "nodeID1,nodeID2,pipeLength,magistral,magistralSite,distSite,tubingTypeID, DiamUslov");

        for (auto& p : bigline_list) {
            CString sql;

            if (isPSstrict(p.n_first) && isPSstrict(p.n_last)) {

                CLINE2* line = graph->find_line_nom(p.n_maxP);

                int pipeSectionID = 0;


                if (line) {
                    CLine* l = &bline(line)->line;

                    CString ms_id = int_to_s(l->ms);
                    CString rs_id = int_to_s(l->rs);
                    CString mag_id = int_to_s(l->mag);
                    CString tubingTypeID = int_to_s(l->tubingTypeID);
                    int DiamUslov = l->pod.diam_usl;
                    double length = p.lengthP;

                    if (l->nomP == -1) {
                        DiamUslov = l->obr.diam_usl;
                        length = p.lengthO;
                    }

                    pipeSectionID = l->pipeSectionID;

                    if (pipeSectionID == 0) {
                        CString sql;
                        sql.Format("INSERT INTO pipeSections (nodeID1,nodeID2,pipeLength,magistral,magistralSite,distSite,tubingTypeID, DiamUslov) VALUES (%d,%d,%g,%s,%s,%s,%s,%d)",
                            p.n_first->id, p.n_last->id, length, mag_id, ms_id, rs_id, tubingTypeID, DiamUslov);

                        pipeSectionID = ado->ExecuteInsert(sql, &affected);

                        if (pipeSectionID > 0) {

                            CString ss = "", s;
                            for (auto& pp : p.list_l) {
                                if (pp > 0) {
                                    if (ss != "") ss += ",";
                                    s.Format("%d", pp);
                                    ss += s;
                                }
                            }

                            sql.Format("UPDATE heatPipeSections SET pipeSectionID=%d WHERE id IN (%s)", pipeSectionID, ss);

                            if (ado->Execute(sql, &affected)) {

                                for (auto& pp : p.list_l) {
                                    if (pp > 0) {
                                        CLINE2* line = graph->find_line_nom_hps(pp);
                                        if (line) {
                                            bline(line)->line.pipeSectionID = pipeSectionID;
                                        }
                                    }
                                }
                            }
                        }
                    }



                    for (auto& pp : p.list_l) {
                        if (pp > 0) {
                            map_big[pp] = pipeSectionID;
                        }
                    }

                    CString sql2;
                    sql.Format("%d,%d,%d,%d,%d,%g,%d,%d",
                        nnn++, pipeSectionID, p.n_maxP, p.n_first->id, p.n_last->id, length, p.n_first->node.fileID, id);

                    if (ins_sort.ado_insert(sql)) {
                        ret = true;
                    }

                    {
                        CLINE2* l = graph->find_line_nom(p.n_maxP);

                        if (l && bline(l)->line.nomP != -1 && bline(l)->line.nomO != -1 && bline(l)->line.nomP != bline(l)->line.nomO) {

                            double length = p.lengthO;

                            sql.Format("%d,%d,%d,%d,%d,%g,%d,%d",
                                nnn++, pipeSectionID, bline(l)->line.nomO, p.n_first->id, p.n_last->id, length, p.n_first->node.fileID, id);

                            if (ins_sort.ado_insert(sql)) ret = true;
                        }
                    }
                }
            }
            //            if (ins_sort.ado_insert()) ret = true;
        }
        if (ins_sort.ado_insert()) ret = true;
    }
    graph2.clear();
}

void set_map_big(CAdoFile* ado, CGraph2* graph, map<int, int>& map_big)
{
}


#endif



template <class T>
void insertSortLinesToDb(CAdoFile* ado, CGraph2* graph, Percent2& pc, CString tn, int* ii, int* sz, bool ms, map<int, T> map_Iter)
{
    graph->init_find_line_nom();
    graph->init_find_line_nom_hps();

    map<int, int> map_big;

    bool condition = map_Iter.begin() != map_Iter.end();
    int nn = 1;
    map<int, T>::const_iterator it = map_Iter.begin();
    //auto it = ms? ps_alma.map_UMS.begin():ps_alma.map_URS.begin();
    for (; it != map_Iter.end(); it++) {
        insertSortLinesToDb2(ado, graph, tn, ms, map_big, it->second.id);
        pc.SetValue((*ii) * 100. / *sz);
        (*ii)++;
    }

    set_map_big(ado, graph, map_big);
}

void sortNodeDb(FILE* f, CAdoFile* ado, CGraph2* graph, bool ms, int id)
{
    CGraph2 graph2(NULL);
    list<CNode2*> list_sort;
    list<CLINE2*> st_l;

    CString sql;
    long affected;

    CString tn = "sortNodesForUchastok";

    if (ms) {
        sql.Format("DELETE FROM %s WHERE uchastok_ms_id=%d", tn, id);
    }
    else {
        sql.Format("DELETE FROM %s WHERE uchastok_rs_id=%d", tn, id);
    }

    OutputDebugStringA("first exec in sortNodeDb \n");
    ado->Execute(sql, &affected);

    graph->reset();

    OutputDebugStringA(" viewUchastok start sortNodeDb \n");
    char myBuff[80];

    sprintf(myBuff, " size before viewUchastok %d\n", graph->map_node.size());
    OutputDebugString(myBuff);
    if (ms) {

        sprintf(myBuff, " viewUchastok ms %d\n", id);
        OutputDebugString(myBuff);
        viewUchastok(graph, make_tuple(id, 0), 1, false);
    }
    else {

        sprintf(myBuff, " viewUchastok rs %d\n", id);
        OutputDebugString(myBuff);
        viewUchastok(graph, make_tuple(0, id), 2, false);
    }
    sprintf(myBuff, " size after viewUchastok %d\n", graph->map_node.size());
    OutputDebugString(myBuff);
    OutputDebugStringA(" copyGraph start sortNodeDb \n");
    copyVydGraph_new(graph, &graph2, true);

    OutputDebugStringA(" sort start sortNodeDb \n");
    sort_line_rs_new(&graph2, list_sort, st_l);


    CNode2* n1_old = nullptr, * n2_old = nullptr;
    CNode2* n_old = nullptr;

    list<list<int> > list_ut;
    list<int> list_ut_min;

    auto it_first = list_sort.begin();
    auto it_last = list_sort.rbegin();

    bool ret = false;

    CString h;
    if (ms) {
        h = "orderID, nodeID, uchastok_ms_id";
    }
    else {
        h = "orderID, nodeID, uchastok_rs_id";
    }

    CAdoInsert ins(ado, tn, h);

    for (auto p : list_sort) {
        CNode2* n = graph->find(p->id);

        if (isPSstrict(p)) {
            sql.Format("%d,%d, %d", nn++, n->id, id);
            if (ins.ado_insert(sql)) ret = true;
        }
    }
    if (ins.ado_insert()) ret = true;
}



void CGidrView::sortNode()
{
    //    FILE* f = fopen("e:\\dima.log", "a");
    FILE* f = nullptr;

    CAdoFile* ado = m_cxema->m_ado;
    CGraph2* graph = m_cxema->m_graph;


    Percent2 pc;
    int ii = 0;
    int sz = ps_alma.map_UMS.size() + ps_alma.map_URS.size();

    {
        map<int, UMS>::const_iterator it = ps_alma.map_UMS.begin();

        int nn = 1;

        for (; it != ps_alma.map_UMS.end(); it++) {
            sortNodeDb(f, ado, graph, true, it->second.id);
            OutputDebugStringA("__progress in sortNode 1 \n");
            pc.SetValue(ii * 100. / sz);
            ii++;
        }
    }

    OutputDebugStringA("__sortNode 1 cycle end \n");

    {
        map<int, URS>::const_iterator it = ps_alma.map_URS.begin();

        for (; it != ps_alma.map_URS.end(); it++) {

            sortNodeDb(f, ado, graph, false, it->second.id);
            OutputDebugStringA("__progress in sortNode 2 \n");
            pc.SetValue(ii * 100. / sz);
            ii++;
        }
    }
    OutputDebugStringA("__sortNode end \n");
    //    fclose(f);
    graph->reset();
}

void CGidrView::sortLine()
{
    CString tn = "sortLinesForUchastok";
    CString sql;
    long affected;
    bool ret;

    CAdoFile* ado = m_cxema->m_ado;

    sql.Format("DELETE FROM iznos");
    ret = ado->Execute(sql, &affected);

    sql =
        "UPDATE hps\n"
        "SET pipeSectionID=NULL\n"
        "FROM heatPipeSections hps\n"
        "WHERE pipeSectionID NOT IN \n"
        "(\n"
        "SELECT id FROM pipeSections\n"
        ")\n";

    ret = ado->Execute(sql, &affected);

    CGraph2* graph = m_cxema->m_graph;

    Percent2 pc;

    int ii = 0;
    int sz = ps_alma.map_UMS.size() + ps_alma.map_URS.size();

    insertSortLinesToDb(ado, graph, pc, tn, &ii, &sz, false, ps_alma.map_URS);
    insertSortLinesToDb(ado, graph, pc, tn, &ii, &sz, true, ps_alma.map_UMS);

    graph->reset();

    copyPTS(ado);

}

void CGidrView::OnCreateSortNode()
{
    //    m_cxema->m_graph->clear();
    //    Invalidate();

    //    return;

    sortNode();
    sortLine();

    AfxMessageBox("Готово", MB_OK | MB_ICONINFORMATION);
}
