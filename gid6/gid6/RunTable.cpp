#include "stdafx.h"
#include "gid6.h"
#include "Cxema.h"

#include "graph2.h"
#include "ado.h"
#include "win.h"

#include "MainFrm.h"

#include "gid6doc.h"
#include "gidrview.h"

#include "virtualnodelist.h"
#include "VirtualDlg.h"
#include "virtlistView.h"
#include "VirtualTable.h"

#include "Ultimate Grid/VirtualTable2.h"
#include "NewPassport/CWebViewTable.h"
#include "NewPassport/WebPageVirtualTable.h"

int getOutID(CAdoFile* ado, int fileID);

#include "adolist.h"

CString getGidAdoName();
CAdoFile* getAdo(const char* name);
CDockablePane* getDockablePane(int nID);


//-----------------------------------------------
// Смотреть таблицу table
//-----------------------------------------------

int RunTableDialog(CWnd* wnd, CAdoFile* ado, const TCHAR* table, const TCHAR* query, const TCHAR* title)
{
    if (!ado) return false;

    CAdoListData* nd = new CAdoListData();

    CString q;

    if (query && query[0]) {
        q = query;
    }
    else {
        q.Format("SELECT * FROM %s", transl(table));
    }

    if (nd->initQpat(ado, q) > 0) {
        nd->setGeo(table);

#if 1
        CString tit = title && title[0] ? title : table;

        CVirtualDlg dlg(wnd, nd, -1, tit, true);
        if (dlg.DoModal() == IDOK) {
            return dlg.m_id;
        }
#else
        CVirtualTable* a = (CVirtualTable*)getDockablePane(IDD_PTS_VIRTUAL_TABLE);
        if (a) {
            a->Init0(nd);
            BOOL delay = TRUE;
            a->ShowPane(TRUE, delay, TRUE);
        }
#endif
    }

    return 0;
}
#if 0

int RunTableNiz(CWnd* wnd, CAdoFile* ado, const TCHAR* table, const TCHAR* query, const TCHAR* title)
{
    if (!ado) return false;

    CAdoListData* nd = new CAdoListData();

    CString q;

    if (query && query[0]) {
        q = query;
    }
    else {
        q.Format("SELECT * FROM %s", transl(table));
    }

    if (nd->initQpat(ado, q) > 0) {
        nd->setGeo(table);

        CVirtualTable* a = (CVirtualTable*)getDockablePane(IDD_PTS_VIRTUAL_TABLE);
        if (a) {
            a->Init0(nd);
            a->ShowPane(TRUE, FALSE, TRUE);
        }
    }

    return 0;
}

#endif



bool CGidrView::RunTableDB(const char* database, const TCHAR* table, const TCHAR* query, int nom, VIRT_DATA typ, CString add_text, const char * title)
{
    CAdoListData* nd = new CAdoListData();
    CAdoFile* ado = getAdo(database);

    if (!ado) return false;

    CString q;

    if (query && query[0]) {
        q = query;
    }
    else {
        q.Format("SELECT * FROM %s", transl(table));
    }

    viewQ2(ado, database, q, transl(table), table, typ, add_text, title);

    return true;
}



bool CGidrView::RunTableMap(const char* database, const TCHAR* table, const TCHAR* query, int nom)
{
    return RunTableDB(database, table, query, nom, virt_data_geo, "", "");
}

bool CGidrView::RunTableTU(const char* database, const TCHAR* table, const TCHAR* query, int nom)
{
    return RunTableDB(database, table, query, nom, virt_data_tu, "", "");
}


bool CGidrView::RunTableAdo(CAdoFile* ado, const TCHAR* table, const TCHAR* query, int nom, VIRT_DATA typ, CString add_text, const char * title)
{
//    CAdoFile* ado = getAdo(getGidAdoName());

    if (ado) {
        return RunTableDB(ado->m_schema, table, query, nom, typ, add_text, title);
    }
    return false;
}



bool CGidrView::RunTable(const TCHAR* table, const TCHAR* query, int nom, VIRT_DATA typ, CString add_text, const char * title)
{
    CAdoFile* ado = getAdo(getGidAdoName());

    return RunTableAdo(ado, table, query, nom, typ, add_text, title);

/*

    if (ado) {
        return RunTableDB(ado->m_schema, table, query, nom, typ, add_text, title);
    }
    return false;
*/
}


void RunTableNew(CWnd* wnd, const char* tn, CAdoListData* nd)
{
    CVirtualTable* a = (CVirtualTable*)getDockablePane(IDD_PTS_VIRTUAL_TABLE);
    if (a) {
        a->Init0(nd);
        BOOL delay = TRUE;
        a->ShowPane(TRUE, delay, TRUE);
    }
}


void RunTableNodes(CWnd* wnd, int i, CGraph2* graph)
{
    CString tn = nodef[i].tableMySQL;

    CAdoListData* nd = new CAdoListData();

    CAdoFile* ado = getAdo(getGidAdoName());

    if (ado) {
        nd->initNode(graph, ado, tn, nodef[i].typ);
        RunTableNew(wnd, nodef[i].table, nd);
    }
}

void RunTableLines(CWnd* wnd, int i, CGraph2* graph)
{
    CString tn = linef[i].tableMySQL;

    CAdoListData* nd = new CAdoListData();

    CAdoFile* ado = getAdo(getGidAdoName());

    if (ado) {
        nd->initLine(graph, ado, tn, linef[i].typ);

        RunTableNew(wnd, linef[i].table, nd);
    }
}



void RunTableNodesOut(CWnd* wnd, const char* tn, CGraph2* graph)
{
    //  CString tn = linef[i].tableMySQL;

    CAdoListData* nd = new CAdoListData();

    CAdoFile* ado = getAdo(getGidAdoName());

    if (ado) {
        nd->initNodeOut(graph, ado, tn, 1);

        RunTableNew(wnd, tn, nd);
    }
}


void RunTableLinesOut(CWnd* wnd, const char* tn, CGraph2* graph)
{
    //  CString tn = linef[i].tableMySQL;

    CAdoListData* nd = new CAdoListData();

    CAdoFile* ado = getAdo(getGidAdoName());

    if (ado) {
        nd->initLineOut(graph, ado, tn, 1);

        RunTableNew(wnd, tn, nd);
    }
}

CString readQ(CAdoFile* ado, const char* tn, const char* tn2, int fileID);

void vydMsRsMag(CGraph2* graph, int m_ms, int m_rs, int m_mag)
{
    if (m_ms <= 0 && m_rs <= 0 && m_mag <= 0) return;

    CMapGraph::iterator p = graph->map_node.begin();
    while (p != graph->map_node.end()) {
        CNode2* n = p->second;
        for (CLINE2* ll = n->lines; ll; ll = next(ll)) {
            CLine2* l = bline(ll);

            if (l->line.mag != 0 && l->line.mag == m_mag || l->line.ms != 0 && l->line.ms == m_ms || l->line.rs != 0 && l->line.rs == m_rs) {
                bline(l)->line.isPjezo = true;
                where(l)->node.isPjezo = true;
                other(l)->node.isPjezo = true;
            }
        }
        p++;
    }
}

bool create_vyd_line_table_new_simple(CAdoFile * ado, CGraph2 * graph, CString & tn);
bool create_vyd_line_table_new_all(CAdoFile * ado, CGraph2 * graph, CString & tn, int fileID);
bool create_vyd_node_table_BIG(CAdoFile* ado, CGraph2* graph, CString& tn, bool is_big);
bool create_vyd_node_table_BIG_all(CAdoFile* ado, CGraph2* graph, CString& tn, bool is_big, int fileID);


void CGidrView::viewTable2(const char* db, const char* tn, const char* tn2, const char* caption, VIRT_DATA typ, bool all)
{
    CAdoFile* ado = getAdo(db);
    if (!ado) return;

    int fileID = 0;
    CString vyd, s, s_order;

    if (m_cxema->m_cur_file) {
        fileID = m_cxema->m_cur_file->id;
    }

    m_cxema->m_graph->save_pjezo();
    vydMsRsMag(m_cxema->m_graph, m_ms, m_rs, m_mag);

    bool isMark = m_cxema->m_graph->isMark();

    if (/*isMark && */(virt_data_line == typ || virt_data_node == typ || virt_data_line_out == typ || virt_data_node_out == typ)) {
        CString fn;
        fn.Format("%ssql\\%s.sql", argpath(), tn);
        CString q = readFile(fn);

        if (q != "") {
            if (isMark) {
                if (virt_data_line == typ || virt_data_line_out == typ) {
                    create_vyd_line_table_new_simple(ado, m_cxema->m_graph, vyd);

                    s.Format("join %s vyd ON vyd.id2=l.id WHERE", vyd);
                    q.Replace("WHERE n1.fileID=$fileID$ AND", s);
                    s_order.Format(" ORDER BY vyd.id1");
                    q += s_order;
                }
                else if (virt_data_node == typ || virt_data_node_out == typ) {
                    create_vyd_node_table_BIG(ado, m_cxema->m_graph, vyd, false);

                    s.Format("join %s vyd ON vyd.id2=n.id WHERE", vyd);
                    q.Replace("WHERE n.fileID=$fileID$ AND", s);
                    s_order.Format(" ORDER BY vyd.id1");
                    q += s_order;
                }
                if (virt_data_node_out == typ || virt_data_line_out == typ || virt_data_unknown_out == typ) {
                    int calculationID = getOutID(ado, fileID);
                    s.Format("%d", calculationID);
                    q.Replace("$calculationID$", s);
                }
            }
            else {
                if (virt_data_line == typ || virt_data_line_out == typ) {
                    create_vyd_line_table_new_all(ado, m_cxema->m_graph, vyd, fileID);

                    s.Format("join %s vyd ON vyd.id2=l.id WHERE", vyd);
                    q.Replace("WHERE n1.fileID=$fileID$ AND", s);
                    s_order.Format(" ORDER BY vyd.id1");
                    q += s_order;
                }
                else if (virt_data_node == typ || virt_data_node_out == typ) {
                    create_vyd_node_table_BIG_all(ado, m_cxema->m_graph, vyd, false, fileID);

                    s.Format("join %s vyd ON vyd.id2=n.id WHERE", vyd);
                    q.Replace("WHERE n.fileID=$fileID$ AND", s);
                    s_order.Format(" ORDER BY vyd.id1");
                    q += s_order;
                }
                if (virt_data_node_out == typ || virt_data_line_out == typ || virt_data_unknown_out == typ) {
                    int calculationID = getOutID(ado, fileID);
                    s.Format("%d", calculationID);
                    q.Replace("$calculationID$", s);
                }
            }

            viewQ2(ado, ado->m_schema, q, tn2, tn2, typ, "", caption);
            m_cxema->m_graph->restore_pjezo();

            return;
        }
    }

    if (!curFile() && (virt_data_node == typ || virt_data_line == typ || virt_data_node_out == typ || virt_data_line_out == typ || virt_data_tg == typ)) {
        AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK|MB_ICONINFORMATION);
        m_cxema->m_graph->restore_pjezo();
        return;
    }

    CString q = readQ(ado, tn, tn2, fileID);
    if (m_cxema->m_cur_file) {
        int n_p = m_cxema->m_graph->map_node.size();

        if (n_p < 1000)  {
            if (virt_data_line == typ || virt_data_line_out == typ) {
                create_vyd_line_table_new_all(ado, m_cxema->m_graph, vyd, fileID);
                s.Format("join %s vyd ON vyd.id2=l.id WHERE n1.fileID=", vyd);
                q.Replace("WHERE n1.fileID=", s);
                s_order.Format(" ORDER BY vyd.id1");
                q += s_order;
            }
            else if (virt_data_node == typ || virt_data_node_out == typ) {
                create_vyd_node_table_BIG_all(ado, m_cxema->m_graph, vyd, false, fileID);

                s.Format("join %s vyd ON vyd.id2=n.id WHERE n.fileID=", vyd);
                q.Replace("WHERE n.fileID=", s);
                s_order.Format(" ORDER BY vyd.id1");
                q += s_order;
            }
        }


        fileID = m_cxema->m_cur_file->id;
        if (virt_data_node_out == typ || virt_data_line_out == typ || virt_data_unknown_out == typ) {
            int calculationID = getOutID(ado, fileID);
            s.Format("%d", calculationID);
            q.Replace("$calculationID$", s);
        }
    }

    if (m_parent_id > 0) {
        s.Format("internalNodeID=%d", m_parent_id);
        q.Replace("internalNodeID IS NULL", s);
    }

    viewQ2(ado, ado->m_schema, q, tn2, tn2, typ, "", caption, all);
    m_cxema->m_graph->restore_pjezo();


}



CString readSql(const char* path, const char* fn, bool include, char* xn, int& nline, int& ntab, char* ist);

#include "Ultimate Grid/ADOSampleView.h"

void CGidrView::viewQ2(CAdoFile* ado1, const char* db, const char* qq, const char* tn, const char* head, VIRT_DATA typ, CString add_text, const char *title, bool all)
{
    CString q = qq;

    log1(q);

    void split_sql_expressions(CString text, std::list<CString> &list_q);

    std::list<CString> list_q;

    split_sql_expressions(q, list_q);

    if (list_q.size() > 1) {
        q = list_q.back();
        list_q.pop_back();

        for (auto& qq : list_q) {
            ado1->Execute(qq);
        }
    }

    

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
    Cgid6App* theApp = (Cgid6App*)AfxGetApp();

    //    CAdoFile *ado = getAdo(getGidAdoName());
    CAdoFile* ado = getAdo(db);
    if (!ado) return;
//    CAdoFile* ado = ado1;

    CMainFrame* mf = (CMainFrame*)AfxGetMainWnd();
    {
        POSITION pos = pDoc->GetFirstViewPosition();
        while (pos != NULL)
        {
            CWebViewTable* pView = (CWebViewTable*)pDoc->GetNextView(pos);
            if (pView->IsKindOf(RUNTIME_CLASS(CWebViewTable))) {
                if (pView->mSql == q && pView->tableName == tn) {
                    mf->setActiveView(pView);
                    return;
                }
            }
        }
    }
    pDoc->m_view_head = head;
    pDoc->m_view_color_col = -1;
    //    pDoc->m_view_color_col = 28;

    pDoc->m_view_typ = typ;
    pDoc->m_view_db = db;
    pDoc->m_view_tn = tn;

    if (title && title[0]) {
        pDoc->m_view_caption = title;
    }
    else {
        pDoc->m_view_caption = tn;
    }
    pDoc->m_view_sql = q;
    pDoc->m_view_add_text = add_text;
    pDoc->m_view_connect = ado->getConnect();
    pDoc->m_view_all = all;

    pDoc->m_view_is_edit = true;
    pDoc->m_view_is_add = true;
    pDoc->m_view_is_del = true;

    CFrameWnd* pWndSecondChild;
    CMultiDocTemplate* pDocTemplate;

    pDocTemplate = theApp->GetDocTemplate(DOCTEMPLATE_TABLE_NEW);
    if (pDocTemplate != NULL)
    {
        pWndSecondChild = pDocTemplate->CreateNewFrame(pDoc, NULL);
        ASSERT(pWndSecondChild != NULL);
        pDocTemplate->InitialUpdateFrame(pWndSecondChild, pDoc);
    }
}


void CGidrView::viewQ2key(const char* db, const char* q, const char* tn, const char* head, VIRT_DATA typ, CString add_text, CString key_name, int key_value)
{
    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
    Cgid6App* theApp = (Cgid6App*)AfxGetApp();

    //    CAdoFile *ado = getAdo(getGidAdoName());
    CAdoFile* ado = getAdo(db);
    if (!ado) return;

    pDoc->m_view_head = head;
    pDoc->m_view_color_col = -1;
    //    pDoc->m_view_color_col = 28;

    pDoc->m_view_typ = typ;
    pDoc->m_view_db = db;
    pDoc->m_view_tn = tn;
    pDoc->m_view_caption = tn;
    pDoc->m_view_sql = q;
    pDoc->m_view_add_text = add_text;
    pDoc->m_view_all = false;
    pDoc->m_view_connect = ado->getConnect();

    pDoc->m_view_is_edit = true;
    pDoc->m_view_is_add = true;
    pDoc->m_view_is_del = true;

    CFrameWnd* pWndSecondChild;
    CMultiDocTemplate* pDocTemplate;

    POSITION pos = pDoc->GetFirstViewPosition();
    while (pos != NULL)
    {
        CADOSampleView* pView = (CADOSampleView*)pDoc->GetNextView(pos);
        if (pView->IsKindOf(RUNTIME_CLASS(CADOSampleView))) {
            CMDIFrameWnd* pFrame = (CMDIFrameWnd*)theApp->m_pMainWnd;
            pFrame->MDIActivate(pView->GetParent());
#if 0
            AfxMessageBox(_TR("Закройте предыдущю таблицу"), MB_OK|MB_ICONINFORMATION);
            //            pView->init(pDoc->m_data);
            return;
#endif
        }
    }

    pDocTemplate = theApp->GetDocTemplate(DOCTEMPLATE_TABLE2);
    if (pDocTemplate != NULL)
    {
        pWndSecondChild = pDocTemplate->CreateNewFrame(pDoc, NULL);
        ASSERT(pWndSecondChild != NULL);
        pDocTemplate->InitialUpdateFrame(pWndSecondChild, pDoc);
    }
}


static CString old_tn = "", old_q = "", old_title = "";
static bool old_is_head = false;

//void CGidrView::RunTableNiz(const char *db, const char *q, const char *tn, VIRT_DATA typ)
int CGidrView::RunTableNiz(CAdoFile* ado, const TCHAR* tn0, const TCHAR* q0, const TCHAR* title0, bool refresh, bool is_head, VIRT_DATA typ)
{
//    VIRT_DATA typ = _typ;

    CString tn = tn0, q = q0, title = title0;

//    CMainFrame* mf = (CMainFrame*)AfxGetMainWnd();

//    if (mf) {     
//        mf->m_wndVirtualTable2.ShowPane(TRUE, FALSE, FALSE);
//    }

    CVirtualTable2* a = (CVirtualTable2*)getDockablePane(IDD_PTS_VIRTUAL_TABLE_NIZ);
    //WebPageVirtualTable* a = (WebPageVirtualTable*)getDockablePane(IDD_PTS_VIRTUAL_TABLE_NIZ1);

    if (tn != "") {
        old_tn = tn;
        old_q = q;
        old_title = title;
        old_is_head = is_head;
    }
    else {
        tn = old_tn;
        q = old_q;
        title = old_title;
        is_head = old_is_head;
    }

    if (a) {

//        a->ShowPane(TRUE, FALSE, FALSE);
        
        if (a->init(ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, title, is_head, typ)) {
            if (refresh) {
                BOOL delay = TRUE;
                a->ShowPane(TRUE, delay, TRUE);
            }
            a->Invalidate();

        }
        else {
            if (refresh) {
                BOOL delay = TRUE;
                a->ShowPane(FALSE, delay, FALSE);
            }
        }
    }

    m_bottom_info.q = q0;
    m_bottom_info.title = title0;
    m_bottom_info.tn = tn0;
    m_bottom_info.typ = typ;


    return 0;
}

#include "join2.h"

CString getJoinQ(CAdoFile* ado, const char* tn)
{
    Join2 j;
    int n1 = j.initJoin(ado, tn);

    j.allJoin(ado, tn, n1);

    int n2 = j.addJoin("", ado, n1, "lineID", "lines", "id", NULL, false);

    CString q = j.getQ();

    return q;
}

int CGidrView::CloseTableNizCod(int code)
{

    if (m_niz_code != code) return 0;

    CVirtualTable2* a = (CVirtualTable2*)getDockablePane(IDD_PTS_VIRTUAL_TABLE_NIZ);

    if (a) {
        BOOL delay = TRUE;
        a->ShowPane(FALSE, delay, FALSE);
    }
    return 0;
}


int CGidrView::RunTableNizCod(int code)
{
    CAdoFile* ado = getAdo(getGidAdoName());

    CTime date1, date2;

    SetButtomInfo(code, 0, false, date1, date2);
    if (m_bottom_info.q != "") {
        m_niz_code = code;
        RunTableNiz(ado, m_bottom_info.tn, m_bottom_info.q, m_bottom_info.title, true, true, m_bottom_info.typ);
    }
    else {
        CVirtualTable2* a = (CVirtualTable2*)getDockablePane(IDD_PTS_VIRTUAL_TABLE_NIZ);

        if (a) {
            BOOL delay = TRUE;
            a->ShowPane(FALSE, delay, FALSE);
            m_niz_code = -1;
        }
    }

    return 0;
}
