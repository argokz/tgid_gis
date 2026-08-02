#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
//#include "DbTable.h"
#include "InputDia.h"
#include "Input2.h"
#include "gidrView.h"
#include "graph.h"
#include "mmenu.h"
#include "ado.h"
#include <fstream>
#include "win.h"


#if 0
    CString q, str, fStr;

    if (!graph->isMark()) {
        fStr.Format("%ssql\\objects\\%s.sql", argpath(), kls->nazv);
        ifstream f(fStr);
        if (f.good())
        {
            fStr.Format("objects\\%s", kls->nazv);
            q = readQ(kls->m_ado, fStr, "", 0);
        }
        else
        {
            CString params = getParams(kls->m_ado, kls->nazv);
            q.Format("SELECT %s FROM [%s]", params, kls->nazv);
        }
        viewQ2(kls->m_database, q, kls->nazv, virt_data_geo);
        return;
    }

    CString vyd = "";

    int qq = makeGeoTemp(graph, kls, vyd);

    if (qq > 0) {
        fStr.Format("%ssql\\objects\\%s.sql", argpath(), kls->nazv);
        ifstream f(fStr);
        if (f.good())
        {
            fStr.Format("objects\\%s", kls->nazv);
            q = readQ(kls->m_ado, fStr, "", 0);
            q.Format("%s WHERE tn.id IN (SELECT id from %s)", q, vyd);
        }
        else
        {
            CString params = getParams(kls->m_ado, kls->nazv);
            q.Format("SELECT %s FROM %s WHERE ID IN (SELECT id from %s)", params, kls->nazv, vyd);
        }
        viewQ2(kls->m_database, q, kls->nazv, virt_data_geo);

    }
    else {
        str.Format(_TR("Объекты типа \"%s\" не найдены на выделенных участках"), kls->RusName());
        AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
    }
#endif


CString getPSTable(int n);
CString readQ(CAdoFile *ado, const char *tn, const char *tn2, int fileID);
CString readQ(CAdoFile * ado, const char* tn, const char* tn2, const char* fileID);

bool create_vyd_line_table_new(CAdoFile *ado, CGraph2 *graph, CString &tn);
bool create_vyd_line_table_BIG(CAdoFile *ado, CGraph2 *graph, CString &tn);
bool create_vyd_node_table_BIG(CAdoFile *ado, CGraph2 *graph, CString &tn, bool is_big);
bool create_vyd_line_table_new_simple(CAdoFile* ado, CGraph2* graph, CString& tn);

void init_pts_list_file(const char * fn1);
void get_map_pts(map<CString, Klassif*> & _map_pts);

void CGidrView::viewUchPTS()
{
    CAdoFile *ado = getAdo(getGidAdoName());
    if (ado) {
        CString vyd;
        CString name = _TR("Участки ПТС");

        CGraph2* graph = m_cxema->m_graph;

        if (create_vyd_line_table_BIG(ado, graph, vyd)) {
            CString q = readQ(ado, "UT_BIG", name, 0);
            if (q != "") {
                q.Replace("$vyd$", vyd);
                viewQ2(ado, ado->m_schema, q, "pipeSections", name, virt_data_line_big, "", name);
            }
        }
    }
}

void CGidrView::OnViewDbPts()
{
    CGraph2 *graph = m_cxema->m_graph;

    CMMenu menu(this, IDD_MENU3, _TR("Выберите таблицу объекта с исходной информацией"));

//    menu.Add(_TR("Тепловые пункты"), (void*) -6);

    if (m_cxema->m_graph->isMark()) {
//        menu.Add(_TR("Трубопроводы"), (void*)0);
//        menu.Add(_TR("Участки ПТС"), (void*) -3);
        menu.Add(_TR("Участки ПТС"), (void*) -5);
        menu.Add(_TR("Узлы ПТС"), (void*) -4);
    }

    map<CString, Klassif*> map_pts;

    init_pts_list_file("pts");
    get_map_pts(map_pts);

    for (auto it : map_pts) {
        menu.Add(it.first, (void*) it.second);
    }

    if (menu.DoModal() == IDOK) {
        int id = (int) menu.getV();

        if (id == 0) {  // Участки
            CAdoFile *ado = getAdo(getGidAdoName());
            if (ado) {
                CString vyd;

                
//                if (create_vyd_line_table(ado, graph, vyd)) {
                if (create_vyd_line_table_new_simple(ado, graph, vyd)) {
                    CString q = readQ(ado, "UT_vyd", _TR("Участки теплопровода"), 0);
                    if (q != "") {
                        q.Replace("$vyd$", vyd);
                        viewQ2(ado, ado->m_schema, q, "heatPipeSections", ST_UT, virt_data_line, "", ST_UT);
                    }
                }
            }
        }
        else if (id == -3) {  // Участки ПТС
//            AfxMessageBox("Участки ПТС", MB_OK|MB_ICONINFORMATION);

            CAdoFile *ado = getAdo(getGidAdoName());
            if (ado) {
                CString vyd;
                if (create_vyd_line_table_new(ado, graph, vyd)) {
                    CString q = readQ(ado, "UT", _TR("Участки ПТС"), 0);
                    if (q != "") {
//                        q.Replace("$vyd$", vyd);
                        CString ss;
//                        ss.Format("l.id IN (SELECT ID from %s)", vyd);
//                        q.Replace("n1.fileID=0", ss);


//                        qq.Format(" JOIN %s vyd ON vyd.id2=l.id ORDER BY vyd.id1", vyd);


                        CString qq;
                        qq.Format(" JOIN %s vyd ON vyd.id2=l.id WHERE ", vyd);
                        q.Replace("WHERE n1.fileID=0 AND ", qq);
                        qq.Format(" ORDER BY vyd.id1 ", vyd);

                        q += qq;
                        viewQ2(ado, ado->m_schema, q, "pipeSections", ST_UT, virt_data_line_big, "", ST_UT);
                    }
                }
            }
        }

        else if (id == -6) {  // Новый документ
            void Run(CWnd * win, const char* cmd, const char* prtn, int id);

            CString cmd;
            CString tmpName;
            Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

            cmd.Format("\"%s\" \"%s\\_new\\w.py\" -rdbms MsSql -server %s -database %s -port %d -user %s -password %s  -run  -fragments %s", 
                python_exe(), argpath(), pDoc->m_IP, pDoc->m_bd_gid, pDoc->m_port, pDoc->m_user, pDoc->m_password, m_cxema->m_par);

            CInputDialog inD(AfxGetMainWnd(), "", "?", cmd);
            inD.DoModal();

            Run(this, cmd, argpath_2() + "protocol.txt", ID_END_PYTHON);
        }

        else if (id == -5) {  // Участки ПТС
            viewUchPTS();
        }
        else if (id == -4) { // Узлы ПТС
            CAdoFile *ado = getAdo(getGidAdoName());
            if (ado) {
                CString vyd;
                if (create_vyd_node_table_BIG(ado, graph, vyd, true)) {
                    CString name = _TR("Узлы ПТС");
                    CString q = readQ(ado, "US_BIG", name, 0);
                    if (q != "") {
                        q.Replace("$vyd$", vyd);
                        viewQ2(ado, ado->m_schema, q, "nodes", name, virt_data_node, "", name);
                    }
                }
            }
        }
        else if (id == -2) {
            //      RunGeoTableKapital(graph, STR_REMONT_KAPITAL);
            //      AfxMessageBox("!", MB_OK|MB_ICONINFORMATION);
        }
//        else if (id == -1) {
        else {
            Klassif *kls = (Klassif *) menu.getV();
            if (kls) RunGeoTable(kls);
        }
    }
}

int makeGeoTemp(CGraph2 *graph, Klassif *kls, CString &vyd);

CString getParams(CAdoFile *ado, const char *tn);

void CGidrView::RunGeoTable(CGraph2 *graph, Klassif *kls)
{
    CString q, str, fStr;

    if (!kls) {
//        str.Format("%s не найден, подключите Геобазу", tn0);
//        AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
        return;
    }

    if (!graph->isMark()) {

        fStr.Format("%ssql\\objects\\%s.sql", argpath(), kls->nazv);
        ifstream f(fStr);
        if (f.good())
        {
            fStr.Format("objects\\%s", kls->nazv);
            q = readQ(kls->m_ado, fStr, "", 0);
        }
        else
        {
            CString params = getParams(kls->m_ado, kls->nazv);
            q.Format("SELECT %s FROM [%s]", params, kls->nazv);
        }
        viewQ2(kls->m_ado, kls->m_database, q, kls->nazv, kls->nazv, virt_data_geo, "", kls->RusName());
        return;
    }

    CString vyd = "";

    int qq = makeGeoTemp(graph, kls, vyd);

    if (qq > 0) {
        fStr.Format("%ssql\\objects\\%s.sql", argpath(), kls->nazv);
        ifstream f(fStr);
        if (f.good())
        {
            fStr.Format("objects\\%s", kls->nazv);
            q = readQ(kls->m_ado, fStr, "", 0);
            q.Format("%s WHERE tn.id IN (SELECT id from %s)", q, vyd);
        }
        else
        {
            CString params = getParams(kls->m_ado, kls->nazv);
            q.Format("SELECT %s FROM %s WHERE ID IN (SELECT id from %s)", params, kls->nazv, vyd);
        }
        viewQ2(kls->m_ado, kls->m_database, q, kls->nazv, kls->nazv, virt_data_geo, "", kls->RusName());
    }
    else {
        str.Format(_TR("Объекты типа \"%s\" не найдены на выделенных участках"), kls->RusName());
        AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
    }
}



#include "join2.h"

CString getTabQ0(CAdoFile* ado, const char * tn)

{
    Join2 j;
    int n0 = j.initJoin(ado, tn, true, "[nodeID][lineID]");
    j.allJoin(ado, tn, n0);
    CString q = j.getQ();
//    CString par = j.getParam_shape("obj");

    return q;
}


#if 0

CString getGeoQ0(Klassif* kls)
{
    CAdoFile* ado = kls->m_ado;
    if (!ado) return "";

    CString q, par;
    getTabQ(ado, kls->nazv, q, par);

    CString qq;

    qq.Format(
"SELECT \n"
" obj.id,\n"
" n1.nodeName AS nodeName1,\n"
" n2.nodeName AS nodeName2,\n"
//" n1.externalNodeName AS exnodeName1,\n"
//" n2.externalNodeName AS exnodeName2,\n"
//" --COUNT(obj.id) cnt\n"
//" l.id AS l_id,\n"
//" l.shape.STDistance(obj.shape) AS leng, \n"
" %s \n"
" FROM (%s\n) obj\n"
" LEFT JOIN linesobj l ON l.shape.STDistance(obj.shape) < 0.3\n"
" LEFT JOIN heatPipeSections hps ON hps.lineID=l.id\n"
" LEFT JOIN pipeSections pipe ON pipe.id=hps.pipeSectionID\n"
" LEFT JOIN sortLinesForUchastok srt ON srt.pipeSectionID=pipe.id\n"
" LEFT JOIN nodes n1 ON n1.id=pipe.nodeID1\n"
" LEFT JOIN nodes n2 ON n2.id=pipe.nodeID2\n",

 par, q);

 return qq;
}

#endif



bool getTabQ(CAdoFile* ado, const char * tn, CString &q, CString &par)
{
    Join2 j;
    int n0 = j.initJoin(ado, tn, true, "[nodeID][lineID]");
    j.allJoin(ado, tn, n0);
    q = j.getQ_shape();
    par = j.getParam_shape("obj");

    return true;
}


CString getTemp(CAdoFile* ado);


CString getGeoQ(Klassif* kls, bool isMark)
{
    CAdoFile* ado = kls->m_ado;
    if (!ado) return "";


    CGidrView* pView = getView();
    CString fragments_nums = pView->m_cxema->getParAll();

    CString q, par;
    getTabQ(ado, kls->nazv, q, par);

//    CString temp_table = "temp_table";
    CString temp_table = getTemp(ado);

    CString qq;

    double dist = 0.3;

    if (kls->nazv == "kolodtsy" || kls->nazv == "kompensator") {
        dist = 4.0;
    }

    qq.Format(

" SELECT obj_id as '№', nodeName1, nodeName2, %s\n"
" FROM \n"
" (\n"
" SELECT *\n"
" FROM (\n"
"   SELECT \n"
"   obj.id AS obj_id,\n"
//"   n1.nodeName AS nodeName1,\n"
//"   n2.nodeName AS nodeName2,\n"
" IIF (n1.nodeName is NULL or n1.nodeName = '' or n1.nodeName = ' ',n1.externalNodeName, n1.nodeName) as 'nodeName1',\n"
" IIF (n2.nodeName is NULL or n2.nodeName = '' or n2.nodeName = ' ',n2.externalNodeName, n2.nodeName) as 'nodeName2',\n"

"   %s,\n"
//"   ROW_NUMBER() OVER(PARTITION BY obj.id ORDER  BY obj.id) AS __row_id,\n"
"     ROW_NUMBER() OVER(PARTITION BY obj.id ORDER  BY obj.shape.STPointN(1).STDistance(l.shape)) AS __row_id,\n"
"   srt.orderID AS srt_orderID,\n"
"   l.id AS __l_id\n"
"   FROM (%s\n) obj\n"
"   LEFT JOIN linesobj l ON l.shape.STDistance(obj.shape) < %f\n"
"   LEFT JOIN heatPipeSections hps ON hps.lineID=l.id\n"
"   LEFT JOIN pipeSections pipe ON pipe.id=hps.pipeSectionID\n"
"   LEFT JOIN sortLinesForUchastok srt ON srt.pipeSectionID=pipe.id\n"
"   LEFT JOIN nodes n1 ON n1.id=pipe.nodeID1\n"
"   LEFT JOIN nodes n2 ON n2.id=pipe.nodeID2\n"
//"  JOIN ##temp_table vyd ON vyd.id2=l.id\n"
"  JOIN %s vyd ON vyd.id2=l.id\n"
"   ) _T1\n"
"  WHERE __row_id=1\n"
"  ) obj\n"
, par , par, q, dist, temp_table);



 if (!isMark) {

    qq.Format(

"SELECT * FROM (\n"

        
"SELECT \n"
" obj.id,\n"
" n1.nodeName AS nodeName1,\n"
" n2.nodeName AS nodeName2,\n"
//" n1.externalNodeName AS exnodeName1,\n"
//" n2.externalNodeName AS exnodeName2,\n"
//" --COUNT(obj.id) cnt\n"
//" l.id AS l_id,\n"
//" l.shape.STDistance(obj.shape) AS leng, \n"
//" ROW_NUMBER() OVER(PARTITION BY obj.id ORDER  BY obj.id) AS __row_id,"
" ROW_NUMBER() OVER(PARTITION BY obj.id ORDER  BY obj.shape.STPointN(1).STDistance(l.shape)) AS __row_id,\n"

" %s \n"
" FROM (%s\n) obj\n"
" LEFT JOIN linesobj l ON l.shape.STDistance(obj.shape) < %f\n"
" LEFT JOIN heatPipeSections hps ON hps.lineID=l.id\n"
" LEFT JOIN pipeSections pipe ON pipe.id=hps.pipeSectionID\n"
" LEFT JOIN sortLinesForUchastok srt ON srt.pipeSectionID=pipe.id\n"
" LEFT JOIN nodes n1 ON n1.id=pipe.nodeID1\n"
" LEFT JOIN nodes n2 ON n2.id=pipe.nodeID2\n"
" WHERE n1.fileID IN (%s)\n"
") __ WHERE __row_id=1"
,

 par, q, dist, fragments_nums);

 }

// CInputDialog inD(AfxGetMainWnd(), _TR(""), _TR(""), qq);
// inD.DoModal();

    return qq;
}

CString getGeoQ0(Klassif* kls)
{
    CAdoFile* ado = kls->m_ado;
    if (!ado) return "";

    CString q, par;
    getTabQ(ado, kls->nazv, q, par);

    CString qq;

    qq.Format(
        "SELECT \n"
        " obj.id,\n"
        " %s \n"
        " FROM (%s\n) obj\n",

        par, q);

    return qq;
}




CString getGeoQGraph(Klassif* kls, CGraph2 *graph, bool sort)
{
    CAdoFile* ado = kls->m_ado;
    if (!ado) return "";

    CString qq = getGeoQ(kls, graph->isMark());
    if (qq == "") return "";

    if (graph->isMark()) {
        CString vyd, q2;

        if (create_vyd_line_table_new(ado, graph, vyd)) {
            if (sort) {
                q2.Format(" JOIN %s vyd ON vyd.id2=l.id\n"
                           " ORDER BY srt.orderID", vyd);
            }
            else {
                q2.Format(" JOIN %s vyd ON vyd.id2=l.id", vyd);
            }

            q2 = "";

            if (sort) {
                  q2 = " ORDER BY srt_orderID";
            }

            qq += q2;
        }
    }
    return qq;

}


// Таблица с начальным и конечным узла


void CGidrView::RunGeoTableNew(Klassif* kls)
{
    if (!kls) {
        return;
    }

    CAdoFile* ado = kls->m_ado;
    if (!ado) return;

    CString qq = "";

    MySQLColumns* col = kls->m_ado->findMySQLColumns(kls->nazv, "lineID");
    if (!col) {
        qq = getGeoQ0(kls);
    }
    else {
        qq = getGeoQGraph(kls, m_cxema->m_graph, true);
    }

    TypMenu idMenu = m_idMenu;
    m_idMenu = TYP_MENU_REZHIM;

    RunTableDB(kls->m_ado->m_schema, kls->nazv, qq, -1, virt_data_geo, "", kls->RusName());

    m_idMenu = idMenu;

    return;
}

CString CGidrView::qGeoTable(Klassif *kls)
{
    CString q, str, fStr, fStr2;

    if (!kls) {
        return "";
    }

    CGraph2 *graph = m_cxema->m_graph; 

//    CString tn = kls->nazv;
    fStr.Format("%ssql\\objects\\%s.sql", argpath(), kls->nazv);
    fStr2.Format("objects\\%s", kls->nazv);

    if (!IsFile(fStr)) {
        fStr.Format("%ssql\\objects\\defect\\%s.sql", argpath(), kls->nazv);
        fStr2.Format("objects\\defect\\%s", kls->nazv);
    }

    if (!IsFile(fStr)) {
         MySQLColumns* col = kls->m_ado->findMySQLColumns(kls->nazv, "lineID");
         if (!col) {
             q = getGeoQ0(kls);
         }
         else {
             q = getGeoQGraph(kls, m_cxema->m_graph, true);
         }
        return q;
    }


    q = readQ(kls->m_ado, fStr2, "", m_cxema->m_par);

    q.Replace("$and_condition$", "");
    q.Replace("$j$", "");
    q.Replace("$order$", "");

    CString fragments_nums = m_cxema->getParAll();
    CString ss;
    ss.Format(" AND n1.fileID IN (%s)", fragments_nums);

    q.Replace("$AND_N1FILEID$", ss);

    if (!graph->isMark()) {
        q.Replace("$JOIN_GEO$", "");
        q.Replace("$SORT_GEO$", "");
        return q;
    }

    bool sort = true;

    CString vyd, q2;

    if (create_vyd_line_table_new(kls->m_ado, graph, vyd)) {
        q2.Format(" JOIN %s vyd ON vyd.id2=l.id", vyd);
        q.Replace("$JOIN_GEO$", q2);

        if (sort) {
            q2 = " ORDER BY srt_orderID";
            q.Replace("$SORT_GEO$", q2);
        }
        else {
            q.Replace("$SORT_GEO$", "");
        }

    }

    return q;
}

void CGidrView::RunGeoTable(Klassif *kls)
{
    CString q, str, fStr, fStr2;
///    Klassif *kls = m_geofile->m_kl_list->findKlN(transl(tn));
//    Klassif *kls = m_geofile->m_kl_list->findKlN(tn0);

    if (!kls) {
//        str.Format("%s не найден, подключите Геобазу", tn0);
//        AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
        return;
    }

    CGraph2* graph = m_cxema->m_graph;

    CString getGeoSelect(Klassif *kls, CGraph2 *graph);
    q = getGeoSelect(kls, graph);

//    viewQ2(kls->m_ado, kls->m_database, q, kls->nazv, kls->nazv, virt_data_geo, "", kls->RusName());
    viewQ2(kls->m_ado, kls->m_database, q, kls->nazv, "", virt_data_geo, "", kls->RusName());

    return;


//    CString tn = kls->nazv;
    fStr.Format("%ssql\\objects\\%s.sql", argpath(), kls->nazv);
    fStr2.Format("objects\\%s", kls->nazv);

    if (!IsFile(fStr)) {
        fStr.Format("%ssql\\objects\\defect\\%s.sql", argpath(), kls->nazv);
        fStr2.Format("objects\\defect\\%s", kls->nazv);
    }

    if (!IsFile(fStr)) {
        RunGeoTableNew(kls);
        return;
    }


    q = readQ(kls->m_ado, fStr2, "", m_cxema->m_par);

    q.Replace("$and_condition$", "");
    q.Replace("$j$", "");
    q.Replace("$order$", "");

    CString fragments_nums = m_cxema->getParAll();
    CString ss;
    ss.Format(" AND n1.fileID IN (%s)", fragments_nums);

    q.Replace("$AND_N1FILEID$", ss);

    if (!graph->isMark()) {
        q.Replace("$JOIN_GEO$", "");
        q.Replace("$SORT_GEO$", "");
        viewQ2(kls->m_ado, kls->m_database, q, kls->nazv, kls->nazv, virt_data_geo, "", kls->RusName());
        return;
    }

    bool sort = true;

    CString vyd, q2;

    if (create_vyd_line_table_new(kls->m_ado, graph, vyd)) {
        q2.Format(" JOIN %s vyd ON vyd.id2=l.id", vyd);
        q.Replace("$JOIN_GEO$", q2);

        if (sort) {
            q2 = " ORDER BY srt_orderID";
            q.Replace("$SORT_GEO$", q2);
        }
        else {
            q.Replace("$SORT_GEO$", "");
        }

        viewQ2(kls->m_ado, kls->m_database, q, kls->nazv, kls->nazv, virt_data_geo, "", kls->RusName());
    }


#if 0

    CString vyd = "";

    int qq = makeGeoTemp(graph, kls, vyd);

    if (qq > 0) {
        CString q2;
        q2.Format("SELECT * FROM (%s\n) _QQQ WHERE l_id IN (SELECT id from %s)", q, vyd);
        q = q2;

        viewQ2(kls->m_database, q, kls->RusName(), kls->RusName(), virt_data_geo, "");
    }
    else {
        str.Format(_TR("Объекты типа \"%s\" не найдены на выделенных участках"), kls->RusName());
        AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
    }
#endif
}




// Таблица без начального и конечного узла

void CGidrView::RunGeoTable0(Klassif* kls)
{
    if (!kls) {
        return;
    }

    CAdoFile* ado = kls->m_ado;
    if (!ado) return;

    CString qq = getGeoQ0(kls);

    TypMenu idMenu = m_idMenu;
    m_idMenu = TYP_MENU_REZHIM;

    RunTableDB(kls->m_ado->m_schema, kls->nazv, qq, -1, virt_data_geo, "", "");

    m_idMenu = idMenu;

    return;
}

