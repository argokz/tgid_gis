// Распредсеть при создании
//

#include "stdafx.h"
#include "gid6.h"
#include "PtsAlmaM2.h"

#include "gid6Doc.h"

#include "gidrview.h"
#include <WINDOWSX.H>
#include "ado.h"

#include "win.h"

#include "wm_user.h"


//#include "mags.h"
#include "ps_alma.h"
//#include "DialogNew2.h"

#include "TaskProgressDlg.h"
#include<tuple> // for tuple 
using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CGeoFile* m_geofile;

CDockablePane* getDockablePane(int nID);


static HTREEITEM m_hItem = 0;
static HTREEITEM m_hItem0 = 0;

/////////////////////////////////////////////////////////////////////////////
// CPtsAlmaM21 property page

//IMPLEMENT_DYNCREATE(CPtsAlmaM2, CPtsTab)

void findMs(int mag);
void findRs(int mag);



CPtsAlmaM2::CPtsAlmaM2(const char* cap, int is_mag)
//CPtsAlmaM2::CPtsAlmaM2() 
{
    //{{AFX_DATA_INIT(CPtsAlmaM2)
    //}}AFX_DATA_INIT
  //  m_dbf = NULL;

    m_kl_list = NULL;
    m_bInit = FALSE;
    m_uzel1 = "";
    m_uzel2 = "";

    m_is_edit = FALSE;

    m_hi = 0;

    m_id = 0;
    m_id2 = 0;

    m_raion = 0;
    m_nachal = 0;
    m_nach = 0;
    m_magistr = 0;

    //  CString cap = "";
    //  int is_mag = 1;

    m_cap = cap;
    //  m_psp.dwFlags |= PSP_USETITLE;
    //  m_psp.pszTitle = m_cap;
    m_mag = is_mag;
}

CPtsAlmaM2::~CPtsAlmaM2()
{
}

void CPtsAlmaM2::DoDataExchange(CDataExchange* pDX)
{
    CPaneDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CPtsAlmaM2)
    DDX_Control(pDX, IDC_LIST1, m_list_uzel1);
    DDX_Control(pDX, IDC_LIST2, m_list_uzel2);
    DDX_Control(pDX, IDC_TREE, m_ctrlTree);
    DDX_LBString(pDX, IDC_LIST1, m_uzel1);
    DDX_LBString(pDX, IDC_LIST2, m_uzel2);
    DDX_LBIndex(pDX, IDC_LIST1, m_Index1);
    DDX_LBIndex(pDX, IDC_LIST2, m_Index2);


    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPtsAlmaM2, CPaneDialog)
    //{{AFX_MSG_MAP(CPtsAlmaM2)
  //  ON_COMMAND(ID_VYD, OnVyd)
  //  ON_WM_CREATE()

    ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
    ON_COMMAND(ID_PASSPORT, OnPassport)   // Старый паспорт
    ON_COMMAND(ID_PASSPORT2, OnPassport2) // Новый паспорт 2020
    ON_COMMAND(ID_EDIT, OnPopupEdit)
    ON_COMMAND(ID_EDIT_MAG, OnPopupEditMag)
    ON_COMMAND(ID_EDIT_GRAPH, OnEditGraph)

    ON_COMMAND(ID_NODE_PTS, OnNodePTS)
    ON_COMMAND(ID_NODE_PTS_UT, OnNodePTSUt)

    ON_COMMAND(ID_EDIT_GRAPH_MAG, OnEditGraphMag)
    ON_COMMAND(ID_MOVE2, OnMove2)
    ON_COMMAND(ID_MOVE_MAG, OnMoveMag)
    ON_COMMAND(ID_MOVE_NACH, OnMoveNach)

    ON_COMMAND(ID_OSMOTR_NEW, OnOsmotrNew)
    ON_COMMAND(ID_OSMOTR_LAST, OnOsmotrLast)
    ON_COMMAND(ID_OSMOTR_HIST, OnOsmotrHist)
    ON_COMMAND(ID_OSMOTR_HIST_NACH, OnOsmotrHistNach)

    ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeListUzel1)
    ON_LBN_SELCHANGE(IDC_LIST2, OnSelchangeListUzel2)

    ON_WM_SIZE()

    ON_WM_CONTEXTMENU()
    ON_MESSAGE(WM_USER_ADO_UPDATED, OnAdoUpdated)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPtsAlmaM2 message handlers

void CPtsAlmaM2::OnOsmotrNew()
{
    CGidrView* pView = getView();
    if (!pView) return;

    CGraph2* graph = pView->m_cxema->m_graph;
    vydMSRS(m_id2, false);
    if (!graph->isMark()) {
        AfxMessageBox(_TR("Участок не описан"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    int id_ue = 0, nachalnik = 0;
    CString str = m_ctrlTree.GetItemText(m_hi);

    HTREEITEM hItem = m_ctrlTree.GetParentItem(m_hi);
    if (hItem) {
        id_ue = m_ctrlTree.GetItemData(hItem);

        for (auto& it : ps_alma.map_UE) {
            UE ue = it.second;
            if (ue.id == id_ue) {
                nachalnik = ue.nach;
            }
        }
    }

    CString s;
    s.Format("Осмотр %s", str);

    pView->SaveOpresNew("osmotr", "Осмотр", "name", s, "", nachalnik, pView->m_id_osmotr2);
}

void CPtsAlmaM2::OnOsmotrLast()
{
}

void CPtsAlmaM2::OnOsmotrHist()
{
}

void CPtsAlmaM2::OnOsmotrHistNach()
{
}


void CPtsAlmaM2::OnSize(UINT nType, int cx, int cy)
{
    CPaneDialog::OnSize(nType, cx, cy);

    if (!m_list_uzel2.m_hWnd) return;

    CRect sz, sz1, sz2, sz3;

    GetClientRect(sz);

    CRect rectClient;
    GetClientRect(rectClient);

    int h = sz.bottom;
    int w = sz.right;


    /*

      sz1 = sz; sz1.bottom = h/2;
      sz2 = sz; sz2.top = sz1.bottom;
      sz2.bottom -= 70;

      sz3 = sz; sz3.top = sz2.bottom;
    */


    sz1 = sz; sz1.bottom = h / 2;
    sz2 = sz; sz2.top = sz1.bottom;
    sz2.bottom = h * 3 / 4;

    sz3 = sz; sz3.top = sz2.bottom;


    m_ctrlTree.MoveWindow(sz1);
    m_list_uzel1.MoveWindow(sz2);
    m_list_uzel2.MoveWindow(sz3);

}

int num_MSRS2(const char* s)
{
    vector<CString> match;
    CString ss;

    int l = regex_match(_TR("УТ.?([0-9+])"), s, match);
    if (l >= 2) {
        CString t1 = match[1];
        return atoi(t1);
    }

    return 0;
}


CString num_MSRS(const char* s)
{
    vector<CString> match;
    CString ss;

    int l = regex_match("^Т?М[ \\-]*([0-9]+)(.*)$", s, match);
    if (l >= 3) {
        CString t1 = match[1];
        CString t2 = match[2];

        int m = atoi(t1);

        ss.Format("%05d%s", m, t2);
        return ss;
    }

    l = regex_match(_TR("п.№\\s*([0-9]+)(.*)$"), s, match);
    if (l >= 3) {
        CString t1 = match[1];
        CString t2 = match[2];

        int m = atoi(t1);

        ss.Format("%05d%s", m, t2);
        return ss;
    }


    /*
      l = regex_match("^ТМ ([0-9]+)(.*)$", s, match);
      if (l >= 3) {
        CString t1 = match[1];
        CString t2 = match[2];

        int m = atoi(t1);

        ss.Format("%05d%s", m, t2);
        return ss;
      }
    */


    l = regex_match(_TR("УТ ([0-9\\.LR]+)\\(([0-9]+)\\)"), s, match);
    if (l >= 3) {
        CString t2 = match[1];
        CString t1 = match[2];

        int m = atoi(t1);

        ss.Format("%05d%s", m, t2);
        return ss;
    }


    return s;
}

/**

struct lessMSRS : std::binary_function <const CString&, const CString&, bool>
{
    result_type operator() ( const CString &arg1, const CString & arg2) const
    {
        CString n1 = num_MSRS(arg1);
        CString n2 = num_MSRS(arg2);

        if (n1 == n2 && arg1 != arg2) {
            return arg1 < arg2;
        }

        return n1 < n2;
    }
};

*/


struct lessMSRS5
{
    using result_type = bool;
    using first_argument_type = const CString&;
    using second_argument_type = const CString&;
    bool operator() (const CString& arg1, const CString& arg2) const;
};

bool lessMSRS5::operator() (const CString& arg1, const CString& arg2) const
{
    CString n1 = num_MSRS(arg1);
    CString n2 = num_MSRS(arg2);


    CString ss;

    ss.Format("%s\n%s,\n%s\n%s", n1, arg1, n2, arg2);

    //    if (arg1 == "т/тр от УТ 19(7)  до ул.Желтоксан,32" || arg2 == "т / тр от УТ 19(7)  до ул.Желтоксан, 32") {
    //        AfxMessageBox(ss, MB_OK|MB_ICONINFORMATION);
    //    }


    if (n1 == n2 && arg1 != arg2) {
        return arg1 < arg2;
    }

    return n1 < n2;
}


struct lessMSRS8
{
    using result_type = bool;
    using first_argument_type = const CString&;
    using second_argument_type = const CString&;
    bool operator() (const pair<int, CString>& arg1, const pair<int, CString>& arg2) const;
};


bool lessMSRS8::operator() (const pair<int, CString>& arg1, const pair<int, CString>& arg2) const
{
    if (arg1.first != arg2.first) return arg1.first < arg2.first;

    CString n1 = num_MSRS(arg1.second);
    CString n2 = num_MSRS(arg2.second);


    CString ss;

    ss.Format("%s\n%s,\n%s\n%s", n1, arg1.second, n2, arg2.second);

    if (n1 == n2 && arg1.second != arg2.second) {
        return arg1.second < arg2.second;
    }

    return n1 < n2;
}




static map<int, UMSRS> mapMSRS;


void initMSRE(map<int, UMSRS>& mapMSRS)
{
    UMSRS ms;
    mapMSRS.clear();

    int id = 0;

    {
        map<int, UMS>::const_iterator it = ps_alma.map_UMS.begin();

        for (; it != ps_alma.map_UMS.end(); it++) {
            UMS rs = it->second;

            ms.id = rs.id;
            ms.id2 = id;
            ms.name = rs.name;
            ms.opis = rs.opis;
            //      ms.kod = rs.kod;
            //      ms.re = rs.re;
            ms.ue = rs.ue;
            ms.kod1 = rs.kod1;      ms.uzel1 = rs.uzel1;
            ms.kod2 = rs.kod2;      ms.uzel2 = rs.uzel2;

            //      ms.nach = rs.nach;
            ms.mag = RT_MS;

            mapMSRS[id] = ms;
            id++;
        }
    }

    {
        map<int, URS>::const_iterator it = ps_alma.map_URS.begin();

        for (; it != ps_alma.map_URS.end(); it++) {
            URS rs = it->second;

            ms.id = rs.id;
            ms.id2 = id;
            //      ms.name = rs.name;
            ms.opis = rs.opis;
            //      ms.kod = rs.kod;
            //      ms.re = rs.re;
            ms.ue = rs.ue;

            ms.kod1 = rs.kod1;      ms.uzel1 = rs.uzel1;
            ms.kod2 = rs.kod2;      ms.uzel2 = rs.uzel2;

            //      ms.nach = rs.nach;
            ms.mag = RT_RS;

            mapMSRS[id] = ms;
            id++;
        }
    }
}


#if 0
static void initNach1(CTreeCtrl& m_ctrlTree, HTREEITEM i1, BOOL is_mag)
{



    //  id_map = 0;

    CGidrView* pView = getView();
    if (!pView) return;
    CCxema* cxema = pView->m_cxema;

    map<CString, UMSRS> mapMS;

    map<int, UMSRS>::const_iterator it = mapMSRS.begin();

    for (; it != mapMSRS.end(); it++) {
        UMSRS rs = it->second;
        if (is_mag == rs.mag) {
            CString s = rs.opis;
            if (s == "") s = rs.name;

            mapMS[s] = rs;
        }
    }

    map<CString, UE> map3;

    {
        map<int, UE>::const_iterator it = ps_alma.map_UE.begin();

        for (; it != ps_alma.map_UE.end(); it++) {

            UE ue = it->second;

            CString re = "";
            CString nach = "";

            map<int, NACH>::const_iterator itN = ps_alma.map_NACH.find(ue.nach);
            if (itN != ps_alma.map_NACH.end()) {
                nach = itN->second.name;
            }

            map<int, CString>::const_iterator itR = ps_alma.map_RE.find(ue.re);
            if (itR != ps_alma.map_RE.end()) {
                re = itR->second;
            }

            map<CString, UMSRS>::const_iterator it2 = mapMS.begin();
            for (; it2 != mapMS.end(); it2++) {
                UMSRS rs = it2->second;
                if (rs.ue == ue.id) {
                    CString s = nach;

                    map3[s] = ue;
                    break;
                }
            }
        }
    }

    map<CString, UE>::const_iterator it2 = map3.begin();

    for (; it2 != map3.end(); it2++) {
        CString s = it2->first;
        UE ue = it2->second;

        HTREEITEM i = m_ctrlTree.InsertItem(s, i1);

        m_ctrlTree.SetItemData(i, (DWORD)ue.id);
    }
}
#else

static void initNach1(CTreeCtrl& m_tree, HTREEITEM m_nach, int m_mag)
{

    HTREEITEM i0 = 0;

    //    HTREEITEM m_nach = m_tree.InsertItem(_TR("Начальники"), i0);

    map<pair<int, CString>, UMSRS, lessMSRS8> mapMSRS;

    int id = 0;

    for (auto& it : ps_alma.map_UMS) {
        UMS rs = it.second;
        UMSRS ms;

        ms.id = rs.id;
        ms.id2 = id;
        ms.name = rs.name;
        ms.opis = rs.opis;
        //      ms.kod = rs.kod;
        //      ms.re = rs.re;
        ms.ue = rs.ue;
        ms.kod1 = rs.kod1;      ms.uzel1 = rs.uzel1;
        ms.kod2 = rs.kod2;      ms.uzel2 = rs.uzel2;

        //      ms.nach = rs.nach;
        ms.mag = RT_MS;

        mapMSRS[pair<int, CString>(1, ms.opis)] = ms;
        id++;
    }


    for (auto& it : ps_alma.map_URS) {
        URS rs = it.second;
        UMSRS ms;

        ms.id = rs.id;
        ms.id2 = id;
        //      ms.name = rs.name;
        ms.opis = rs.opis;
        //      ms.kod = rs.kod;
        //      ms.re = rs.re;
        ms.ue = rs.ue;

        ms.kod1 = rs.kod1;      ms.uzel1 = rs.uzel1;
        ms.kod2 = rs.kod2;      ms.uzel2 = rs.uzel2;

        //      ms.nach = rs.nach;
        ms.mag = RT_RS;

        mapMSRS[pair<int, CString>(2, ms.opis)] = ms;
        id++;
    }

    map<CString, NACH> map_nach;

    for (auto& it : ps_alma.map_NACH) {
        map_nach[it.second.name] = it.second;
    }

    for (auto& it : map_nach) {
        CString nach = it.second.name;


        //        CString ss;
        //        ss.Format("%d %s", );

        HTREEITEM i1 = m_tree.InsertItem(nach, m_nach);

        for (auto& it1 : ps_alma.map_UE) {
            UE ue = it1.second;
            if (ue.nach == it.second.id) {
                for (auto& it3 : mapMSRS) {
                    UMSRS rs = it3.second;
                    if (rs.ue == ue.id && rs.mag == m_mag) {
                        //                        CString ss;
                        //                        ss.Format("%d %d %d %s", ue.id, rs.id, rs.id2, rs.opis);
                        HTREEITEM i2 = m_tree.InsertItem(rs.opis, i1);
                        m_tree.SetItemData(i1, ue.id);
                        m_tree.SetItemData(i2, rs.id2);
                    }
                }
            }
        }
    }
}
#endif



int isMagMsRs(int ue, int ms_rs)
{
    map<int, UMSRS>::const_iterator it = mapMSRS.begin();

    for (; it != mapMSRS.end(); it++) {
        UMSRS rs = it->second;
        if (rs.ue == ue) {
            if (rs.mag == ms_rs) return true;
        }
    }
    return false;
}


int isUeMsRs(int re, int ms_rs)
{
    map<int, UE>::const_iterator it = ps_alma.map_UE.begin();

    for (; it != ps_alma.map_UE.end(); it++) {
        UE ue = it->second;
        if (ue.re == re) {
            if (isMagMsRs(ue.id, ms_rs)) return true;
        }
    }
    return false;
}


static void initRe(CTreeCtrl& m_ctrlTree, HTREEITEM i1, BOOL is_mag)
{
    //  id_map = 0;

    CGidrView* pView = getView();
    if (!pView) return;
    CCxema* cxema = pView->m_cxema;


    map<CString, map<int, CString> > map2;

    map<int, CString>::const_iterator it2 = ps_alma.map_RE.begin();

    for (; it2 != ps_alma.map_RE.end(); it2++) {
        CString s = it2->second;

        if (isUeMsRs(it2->first, is_mag)) {
            map2[s][it2->first] = s;
        }
    }

    map<CString, map<int, CString> >::const_iterator it3 = map2.begin();

    for (; it3 != map2.end(); it3++) {

        map<int, CString> ::const_iterator it4 = it3->second.begin();

        for (; it4 != it3->second.end(); it4++) {
            HTREEITEM i = m_ctrlTree.InsertItem(it4->second, i1);
            m_ctrlTree.SetItemData(i, (DWORD)it4->first);
        }
    }



#if 0

    map<int, CString>::const_iterator it2 = ps_alma.map_RE.begin();

    for (; it2 != ps_alma.map_RE.end(); it2++) {
        CString s = it2->second;

        if (isUeMsRs(it2->first, is_mag)) {
            HTREEITEM i = m_ctrlTree.InsertItem(s, i1);
            m_ctrlTree.SetItemData(i, (DWORD)it2->first);
        }
    }
#endif
}


static void initMagistr(CTreeCtrl& m_ctrlTree, HTREEITEM i1)
{
    map<CString, UMS, lessMSRS5> map2_1;

    CGidrView* pView = getView();
    if (!pView) return;
    CCxema* cxema = pView->m_cxema;


    map<int, UMS>::const_iterator it = ps_alma.map_UMS.begin();

    for (; it != ps_alma.map_UMS.end(); it++) {
        UMS rs = it->second;
        map2_1[rs.opis] = rs;
    }

    map<CString, UMAG, lessMSRS5> mag_mag;

    map<int, UMAG>::const_iterator it1 = ps_alma.mapMAG.begin();

    for (; it1 != ps_alma.mapMAG.end(); it1++) {
        UMAG mag = it1->second;
        mag_mag[mag.name] = mag;
    }

    map<CString, UMAG, lessMSRS5>::const_iterator itU = mag_mag.begin();

    for (; itU != mag_mag.end(); itU++) {
        UMAG mag = itU->second;
        HTREEITEM i = m_ctrlTree.InsertItem(mag.opis, i1);
        m_ctrlTree.SetItemData(i, (DWORD)mag.id);

        map<CString, UMS, lessMSRS5>::const_iterator it2 = map2_1.begin();
        for (; it2 != map2_1.end(); it2++) {
            UMS ms = it2->second;
            if (ms.mag == mag.id) {
                HTREEITEM i1 = m_ctrlTree.InsertItem(ms.opis, i);
                m_ctrlTree.SetItemData(i1, (DWORD)ms.id);
            }
        }
    }
}


static void initMag2(CTreeCtrl& m_ctrlTree, HTREEITEM i1, int id, BOOL is_mag)
{

    map<CString, UMSRS, lessMSRS5> map2_2;
    map<int, UMSRS>::const_iterator it = mapMSRS.begin();

    for (; it != mapMSRS.end(); it++) {
        UMSRS rs = it->second;
        map2_2[rs.opis] = rs;
    }

    map<CString, UMSRS, lessMSRS5>::const_iterator it2 = map2_2.begin();
    for (; it2 != map2_2.end(); it2++) {
        UMSRS rs = it2->second;

        map<int, UE>::const_iterator itUE = ps_alma.map_UE.find(rs.ue);
        if (itUE != ps_alma.map_UE.end()) {
            UE ue = itUE->second;
            if (ue.id == id) {

                if (isMagMsRs(ue.id, is_mag)) {
                    HTREEITEM i = m_ctrlTree.InsertItem(rs.opis, i1);

                    m_ctrlTree.SetItemData(i, rs.id2);
                }
            }
        }
    }
}


static bool isLinePjezo(const CNode2* n)
{
    for (CLINE2* l = n->lines; l; l = next(l)) {
        if (bline(l)->line.isPjezo) return true;
    }
    return false;
}

int viewUchastok(CGraph2* m_graph, tuple<int, int> msrs, int type /*1 - Магистраль 2 Распредсеть 3 Магистраль и распредсеть*/, bool is_only_big_pipe)
{
    CMapGraph::const_iterator p = m_graph->map_node.begin();

    int nn = 0;

    while (p != m_graph->map_node.end()) {
        CNode2* n = p->second;

        if (n->node.coord.x != 0 || n->node.coord.y != 0) {

            CLINE2* l;
            for (l = n->lines; l; l = next(l)) {
                CLine2* ll = bline(l);
                bool condition;
                switch (type)
                {
                case 1:
                    condition = ll->line.ms == get<0>(msrs);
                case 2:
                    condition = ll->line.rs == get<1>(msrs);
                case 3:
                    condition = ll->line.ms == get<0>(msrs) && get<0>(msrs) > 0 || ll->line.rs == get<1>(msrs) && get<1>(msrs) > 0;
                }

                bool r = IsRezhim(ll);

                if (condition && (ll->line.pipeSectionID > 0 || !is_only_big_pipe) && !r) {
                    ll->line.isPjezo = 1;
                    ll->line.isPjezo = 1;
                    where(l)->node.isPjezo = 1;
                    other(l)->node.isPjezo = 1;
                    nn++;
                }
            }
        }
        p++;
    }
    return nn;
}


int vydMsRs(CGraph2* m_graph, int ms, int rs)
{
    CMapGraph::const_iterator p = m_graph->map_node.begin();

    int nn = 0;

    while (p != m_graph->map_node.end()) {
        CNode2* n = p->second;

        if (n->node.coord.x != 0 || n->node.coord.y != 0) {

            map<string, string>::const_iterator it;

            CLINE2* l;
            for (l = n->lines; l; l = next(l)) {
                CLine2* ll = bline(l);

                if (ll->line.ms == ms && ms > 0 || ll->line.rs == rs && rs > 0) {
                    ll->line.isPjezo = true;
                    ll->line.isPjezo = true;
                    where(l)->node.isPjezo = true;
                    other(l)->node.isPjezo = true;
                    nn++;
                }
            }
        }
        p++;
    }
    return nn;
}

int vydMsRsN(CGraph2* m_graph, int ms_rs, int rs, bool is_only_big_pipe)
{
    if (ms_rs == 0) {
        return viewUchastok(m_graph, make_tuple(rs, 0), 1, is_only_big_pipe);
    }
    if (ms_rs == 1) {
        return viewUchastok(m_graph, make_tuple(0, rs), 2, is_only_big_pipe);
    }
    return 0;
}


/*
int vydRsN(CGraph2 *m_graph, const char *kod)
{
  CMapGraph::const_iterator p = m_graph->map_node.begin();

  int n = 0;

  while (p != m_graph->map_node.end()) {
    if (p->coord.x != 0 || p->coord.y != 0) {

      map<string, string>::const_iterator it;

      CLINE2 *l;
      for (l = p->lines; l; l = next(l)) {
        CLine *ll = bline(l);

        if (ll->kod0 == kod) {
          ll->isPjezo = 1;
          ll->isPjezo = 1;
          where(l)->isPjezo = 1;
          other(l)->isPjezo = 1;
          n++;
        }
      }
    }
    p++;
  }
  return n;
}
*/

void copyVydGraphAll(CGraph2* graph1, CGraph2* graph2, list<CNode*>& list_sort, list<CLINE2*>& list_l, int mag_typ, int mag_id);
CString getPSTable(int n);

void copyVydGraph_new(CGraph2* graph1, CGraph2* graph2, bool is_po = false);
void sort_line_rs_new(CGraph2* graph, list<CNode2*>& list_sort, list<CLINE2*>& st_l, int fileID = -1);

// Определяет, что это узел ПТС

/*
bool isPS_old(CNode2 *node)
{
    return node->typPS() > 0 && node->typPS() != PS_NEUST || node->node.name.Find(_TR("УТ-")) == 0;
//    return node->typPS() > 0 && node->typPS() != PS_NEUST || inc(node) != 2 || node->node.name.Find(_TR("УТ-")) == 0;
}
*/

// Определяет, что это узел ПТС или угловой


bool isPS(CNode2* node)
{
//    return true;
    //    return node->typPS() > 0 || node->node.namePS != "" || node->node.typ == TIP_PR;
        /// !!!!!!!!!!!!!


        //    return node->typPS() > 0 && node->typPS() != PS_NEUST || node->node.name.Find(_TR("УТ-")) == 0  || inc(node) != 2;
        //    return node->typPS() > 0 || node->node.name.Find(_TR("УТ-")) == 0 || inc(node) != 2 || node->node.namePS != "";

    int inc1 = inc_pod(node);
    if (inc1 == 0) {
        inc1 = inc_obr(node);
    }

    int incP = inc_pod(node);
    int incO = inc_obr(node);

    inc1 = max(incP, incO);
    inc1 = inc(node);

    CLINE2* l;
    int rs = 0;
    int ms = 0;

    inc1 = 0;

    for (l = node->lines; l; l = next(l)) {
        if (bline(l)->line.typ == TIP_UT) {
            inc1++;
        }
    }


    set<int> set_ms;
    set<int> set_rs;

    for (l = node->lines; l; l = next(l)) {
        set_ms.insert(bline(l)->line.ms);
        set_rs.insert(bline(l)->line.rs);
    }

    if (set_ms.size() + set_rs.size() > 2) inc1 = 3;


    bool lin1 = false;
    bool lin2 = false;

    for (l = node->lines; l; l = next(l)) {
        if (bline(l)->line.nomP == -1 || bline(l)->line.nomO == -1) {
            lin1 = true;
        }
        if (bline(l)->line.nomP != -1 && bline(l)->line.nomO != -1) {
            lin2 = true;
        }
    }
    if (lin1 && lin2) inc1 = 3;

    if (inc1 == 2) {
        CNode2* nn1 = nullptr, * nn2 = nullptr;
        for (l = node->lines; l; l = next(l)) {
            if (bline(l)->line.nomP == -1 || bline(l)->line.nomO != -1) {
                nn1 = other(l);
            }
            if (bline(l)->line.nomP != -1 && bline(l)->line.nomO == -1) {
                nn2 = other(l);
            }
        }
        if (nn1 && nn2 && nn1 == nn2) {
            inc1 = 1;
        }

        for (l = node->lines; l; l = next(l)) {
            if (bline(l)->line.nomP != -1 && bline(l)->line.nomO != -1 && bline(l)->line.nomP != bline(l)->line.nomO) {
                inc1 = 3;
            }
        }
    }

    return (node->typPS() > 0 && node->typPS() != PS_NEUST) || node->node.namePS != "" || inc1 != 2;

    //    return node->typPS() > 0 && node->typPS() != PS_NEUST || inc(node) != 2 || node->node.name.Find(_TR("УТ-")) == 0;
}


bool isPSstrict(CNode2* node)
{
//    return true;
    //    return node->typPS() > 0 && node->typPS() != PS_NEUST;
    //    return node->typPS() > 0;

//    return node->typPS() > 0 || node->node.namePS != "";
    /// !!!!!!!!!!!!!


    return isPS(node);
}


void fillNodeList(CGraph2* graph, CListBox* m_list_uzel1, map<CString, const CNode2*>& map_node, list<const CNode2*>* list_all_node)
{
    //  return;

    CGraph2 graph2(NULL);

    list<CNode2*> list_sort;
    list<CLINE2*> list_l;

    copyVydGraph_new(graph, &graph2, true);
    sort_line_rs_new(&graph2, list_sort, list_l);

    //  copyVydGraphAll(graph, &graph2, list_sort, list_l, 1, mag.id);

    CNode2* n_old = NULL;

    list<CNode2*>::iterator p = list_sort.begin();
    while (p != list_sort.end()) {
        CNode2* nn = *p;
        p++;

        CString str = PSName(nn);

        if (nn->typPS() > 0 && nn->typPS() != PS_NEUST) {
            str += " " + getPSTable(nn->typPS());
        }
        if (n_old != NULL && !graph2.Link(nn, n_old)) {
            if (m_list_uzel1) {
                //                m_list_uzel1->AddString("---------");
            }
        }

        if (list_all_node) {
            CNode2* n0 = graph->find(nn->id);

            if (n0) {
                list_all_node->push_back(n0);
            }
        }

        //        if (isPSstrict(nn) || n_old == NULL || p == list_sort.end()) {
        if (isPSstrict(nn)) {
            if (1) {
                if (m_list_uzel1) {
                    m_list_uzel1->AddString(str);
                }
                CNode2* node = graph->find(nn->id);
                if (node) map_node[str] = node;
            }
        }

        n_old = nn;
    }
}


#if 0
void fillNodeLineList(CGraph2* graph, list<const CNode2*>* list_all_node)
{
    CGraph2 graph2(NULL);

    list<CNode2*> list_sort;
    list<CLINE2*> list_l;

    copyVydGraph_new(graph, &graph2, true);
    sort_line_rs_new(&graph2, list_sort, list_l);

    CNode2* n_old = NULL;

    bool first = true;

    list<CNode2*>::iterator p = list_sort.begin();
    while (p != list_sort.end()) {
        CNode2* nn = *p;
        p++;

        if (n_old != NULL && !graph2.Link(nn, n_old)) {
            //      m_list_uzel1.AddString("---------");
            first = true;
            list_all_node->push_back(NULL);
        }

        if (isPS(nn) || n_old == NULL || p == list_sort.end()) {
            if (list_all_node) {
                CNode2* n0 = graph->find(nn->id);

                if (n0) {
                    list_all_node->push_back(n0);
                }
            }
        }

        first = false;
        n_old = nn;
    }
}
#endif


void fillLineList(CGraph2* graph, list<CLINE2*>& list_l)
{
    CGraph2 graph2(NULL);

    list<CNode2*> list_sort;

    copyVydGraph_new(graph, &graph2, true);
    sort_line_rs_new(&graph2, list_sort, list_l);
}

CString readQ(CAdoFile* ado, const char* tn, const char* tn2, int fileID);
bool create_vyd_line_table_BIG(CAdoFile* ado, CGraph2* graph, CString& tn);


int vydMagN(CGraph2* m_graph, int id);
int vydNach(CGraph2* m_graph, int id);


void CPtsAlmaM2::vydMSRS(int id, bool is_only_big_pipe)
{
    CGidrView* pView = getView();
    if (!pView) return;

    m_list_uzel1.ResetContent();
    m_list_uzel2.ResetContent();

    CGraph2* graph = pView->m_cxema->m_graph;

    graph->reset();

    if (m_mag == 0) {
        viewUchastok(graph, make_tuple(id, 0), 1, is_only_big_pipe);
    }
    else {
        viewUchastok(graph, make_tuple(0, id), 2, is_only_big_pipe);
    }
    pView->Invalidate();

    map_node.clear();
    fillNodeList(graph, &m_list_uzel1, map_node, NULL);

    if (m_mag == 0) {
        pView->setMagNomer(0, m_id2, 0);
    }
    else if (m_mag == 1) {
        pView->setMagNomer(0, 0, m_id2);
    }


    /*
      UMSRS *ms = ps_alma.getMSRS(kod);

      CNode *m_node_first = pView->m_cxema->findAll(ms->kod1, ms->uzel1);
      CNode *m_node_last = pView->m_cxema->findAll(ms->kod2, ms->uzel2);;
      CString str = "";

      if (m_node_first) {
        str.Format("%-*s %s", N3, m_node_first->node_name.kod, m_node_first->node_name.name);
        m_list_uzel2.AddString(str);
        map_node[str] = m_node_first;
      }

      if (m_node_last) {
        str.Format("%-*s %s", N3, m_node_last->node_name.kod, m_node_last->node_name.name);
        map_node[str] = m_node_last;
      }
    */


    map_line.clear();

    CAdoFile* ado = getAdo(getGidAdoName());
    if (ado) {
        CString vyd;
        CString name = _TR("Участки ПТС");
        if (create_vyd_line_table_BIG(ado, graph, vyd)) {
            CString q = readQ(ado, "UT_BIG", name, 0);
            if (q != "") {
                q.Replace("$vyd$", vyd);

                if (ado->openTable0(q)) {

                    int i = 0;

                    while (!ado->isEOF()) {
                        CString ss;
                        int id = ado->read_long(0);
                        CString s1 = ado->readStr(2);
                        CString s2 = ado->readStr(3);
                        ss.Format("%s - %s", s1, s2);
                        m_list_uzel2.AddString(ss);
                        map_line[i++] = id;

                        ado->MoveNext();
                    }
                    ado->closeTable();
                }
            }
        }
    }











}

void CPtsAlmaM2::vydMag(int id)
{
    CGidrView* pView = getView();
    if (!pView) return;

    m_list_uzel1.ResetContent();
    m_list_uzel2.ResetContent();


    CGraph2* graph = pView->m_cxema->m_graph;

    graph->reset();
    vydMagN(graph, id);
    pView->Invalidate();

    map_node.clear();
    fillNodeList(graph, &m_list_uzel1, map_node, NULL);

    /*

      UMSRS *ms = ps_alma.getMSRS(kod);

      CNode *m_node_first = pView->m_cxema->findAll(ms->kod1, ms->uzel1);
      CNode *m_node_last = pView->m_cxema->findAll(ms->kod2, ms->uzel2);;

      CString str = "";

      if (m_node_first) {
        str.Format("%-*s %s", N3, m_node_first->node_name.kod, m_node_first->node_name.name);
        m_list_uzel2.AddString(str);
        map_node[str] = m_node_first;
      }

      if (m_node_last) {
        str.Format("%-*s %s", N3, m_node_last->node_name.kod, m_node_last->node_name.name);
        map_node[str] = m_node_last;
      }
    */
}




static void initUE(CTreeCtrl& m_ctrlTree, HTREEITEM i1, int id, int is_mag)
{
    CGidrView* pView = getView();
    if (!pView) return;

    map<CString, map<int, UE> > map2;

    map<int, UE>::const_iterator it = ps_alma.map_UE.begin();
    for (; it != ps_alma.map_UE.end(); it++) {
        UE ue = it->second;
        if (ue.re == id) {
            if (isMagMsRs(ue.id, is_mag)) {
                map2[ue.nomer][it->first] = ue;
            }
        }
    }


    map<CString, map<int, UE> >::const_iterator it3 = map2.begin();

    for (; it3 != map2.end(); it3++) {

        map<int, UE> ::const_iterator it4 = it3->second.begin();

        for (; it4 != it3->second.end(); it4++) {
            HTREEITEM i = m_ctrlTree.InsertItem(it3->first, i1);
            m_ctrlTree.SetItemData(i, (DWORD)it4->second.id);
        }
    }




#if 0
    map<int, UE>::const_iterator it = ps_alma.map_UE.begin();
    for (; it != ps_alma.map_UE.end(); it++) {
        UE ue = it->second;
        if (ue.re == id) {
            if (isMagMsRs(ue.id, is_mag)) {
                HTREEITEM i = m_ctrlTree.InsertItem(ue.nomer, i1);
                m_ctrlTree.SetItemData(i, (DWORD)ue.id);
            }
        }
    }
#endif
}

//BOOL CPtsAlmaM2::OnInitDialog() 
//{
//  CPaneDialog::OnInitDialog();

double getScreenMas();

static bool is_font = false;
static int m_tree_font = 18;

void setTreeFont(CTreeCtrl* m_ctrlTree, CFont* m_Font)
{
    LOGFONT lf;

    m_ctrlTree->GetFont()->GetLogFont(&lf);

    //   lf.lfHeight *= getScreenMas(); // make font larger (check for -ve)
    //   lf.lfHeight *= 1.5; // make font larger (check for -ve)
    int dd = 5;

    //   if (lf.lfHeight < 0) 
    //        lf.lfHeight -= dd;
    //   else 
    //        lf.lfHeight += dd;

    //    if (!is_font) {
    //        m_tree_font *= getScreenMas();
    //        is_font = true;
    COLORREF color;
    CFont font;
    font.CreatePointFont(120, _T("Tahoma"));
    font.GetLogFont(&lf);
    font.DeleteObject();

    LoadFont2("PanelFont", lf, color);
    //    }
    //    lf.lfHeight = m_tree_font;

    lf.lfItalic = 0;

    m_Font->CreateFontIndirect(&lf);
    m_ctrlTree->SetFont(m_Font);
}


//int CPtsAlmaM2::OnCreate(LPCREATESTRUCT lpCreateStruct)
LRESULT CPtsAlmaM2::HandleInitDialog(WPARAM, LPARAM)
{

    UpdateData(FALSE);

    HTREEITEM i0 = 0;

    m_imgList.Create(IDB_TREE, 16, 1, RGB(0, 128, 128));

    setTreeFont(&m_ctrlTree, &m_Font);

    m_ctrlTree.SetImageList(&m_imgList, TVSIL_NORMAL);

    m_ctrlTree.SetItemData(TVI_ROOT, (DWORD)0);

    //  i0 = m_ctrlTree.InsertItem(_TR("Магистрали"), i0);


    //  m_nachal = m_ctrlTree.InsertItem(_TR("Магистрали1"), i0);
    //  m_nach = m_ctrlTree.InsertItem(_TR("Начальники магистралей"), i0);

    //  SetWindowText(m_cap);

    //  CTabCtrl *tab = GetTabControl();
    //  if (tab) {
    //  }

    UpdateData(TRUE);

    return TRUE;
}


void CPtsAlmaM2::Click(HTREEITEM ht_hItem)
{
    HTREEITEM hItemChild = 0;
    HTREEITEM hItem1 = 0;
    HTREEITEM hItem2 = 0;
    HTREEITEM hItem3 = 0;

    DWORD dwpos = GetMessagePos();

    hItemChild = m_ctrlTree.GetChildItem(ht_hItem);
    hItem1 = m_ctrlTree.GetParentItem(ht_hItem);
    hItem2 = m_ctrlTree.GetParentItem(hItem1);
    hItem3 = m_ctrlTree.GetParentItem(hItem2);

    if (ht_hItem) {
        CDocument* pDocument = (CDocument*)m_ctrlTree.GetItemData(ht_hItem);
        m_hItem = ht_hItem;
        if (hItem2 == m_nachal) {  // Выбрана распредсеть
            m_id = m_ctrlTree.GetItemData(ht_hItem);
            setU(m_id);
        }


        m_ctrlTree.Select(ht_hItem, TVGN_CARET);
        m_ctrlTree.Select(ht_hItem, TVGN_DROPHILITE);

        CString str = m_ctrlTree.GetItemText(ht_hItem);

        if (!hItemChild) {
            if (ht_hItem == m_nachal) {
                //                initNach1(m_ctrlTree, m_nachal, m_mag);
            }
            if (ht_hItem == m_raion) {
                initRe(m_ctrlTree, m_raion, m_mag);
            }
            if (ht_hItem == m_magistr) {
                initMagistr(m_ctrlTree, m_magistr);
            }
            else if (hItem1 == m_nachal) {
                int id = m_ctrlTree.GetItemData(ht_hItem);
                m_id = id;
                setU(m_id);
                initMag2(m_ctrlTree, ht_hItem, id, m_mag);
            }
            else if (hItem1 == m_raion) {
                int id = m_ctrlTree.GetItemData(ht_hItem);
                initUE(m_ctrlTree, ht_hItem, id, m_mag);
            }
            else if (hItem2 == m_raion) {
                int id = m_ctrlTree.GetItemData(ht_hItem);
                m_id = id;
                setU(m_id);
                initMag2(m_ctrlTree, ht_hItem, id, m_mag);
            }
        }

        CGidrView* pView = getView();
        if (pView) {
            if (hItem2 == m_nachal || hItem3 == m_raion || hItem2 == m_magistr && m_magistr != NULL) { // МС РС

                int id = (int)m_ctrlTree.GetItemData(ht_hItem);
                UMSRS* msrs = ps_alma.getMsRs(id);

                if (msrs) {
                    m_id2 = msrs->id;
                    vydMSRS(msrs->id, false);

                    pView->m_ms_kod = 0;
                    pView->m_rs_kod = 0;
                    pView->m_magistral_kod = 0;
                }
            }

            if (hItem1 == m_magistr && m_magistr != NULL) { // Магистраль
                int id = m_ctrlTree.GetItemData(ht_hItem);
                m_id2 = id;
                vydMag(id);

                pView->m_ms_kod = 0;
                pView->m_rs_kod = 0;
                pView->m_magistral_kod = 0;
            }
        }
    }
}



BOOL CPtsAlmaM2::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    // TODO: Add your specialized code here and/or call the base class

    LPNMHDR lpnmh = (LPNMHDR)lParam;
    TVHITTESTINFO ht = { 0 };

    HTREEITEM hItemChild = 0;
    HTREEITEM hItem1 = 0;
    HTREEITEM hItem2 = 0;
    HTREEITEM hItem3 = 0;


    if ((lpnmh->code == NM_CLICK) || (lpnmh->code == NM_RCLICK)) {

        DWORD dwpos = GetMessagePos();

        // include <windowsx.h> and <windows.h> header files
        ht.pt.x = GET_X_LPARAM(dwpos);
        ht.pt.y = GET_Y_LPARAM(dwpos);
        ::MapWindowPoints(HWND_DESKTOP, lpnmh->hwndFrom, &ht.pt, 1);

        TreeView_HitTest(lpnmh->hwndFrom, &ht);

        hItemChild = m_ctrlTree.GetChildItem(ht.hItem);
        hItem1 = m_ctrlTree.GetParentItem(ht.hItem);
        hItem2 = m_ctrlTree.GetParentItem(hItem1);
        hItem3 = m_ctrlTree.GetParentItem(hItem2);

        if (ht.hItem) {
            CDocument* pDocument = (CDocument*)m_ctrlTree.GetItemData(ht.hItem);
            m_hItem = ht.hItem;
            if (hItem2 == m_nachal) {  // Выбрана распредсеть
                m_id = m_ctrlTree.GetItemData(ht.hItem);
                setU(m_id);
            }
        }
    }

    if ((lpnmh->code == NM_CLICK))
    {
        if (2 & ht.flags || 4 & ht.flags)
        {
            Click(ht.hItem);
        }
    }


    else if ((lpnmh->code == NM_RCLICK))
    {
        m_ctrlTree.Select(ht.hItem, TVGN_CARET);
        m_ctrlTree.Select(ht.hItem, TVGN_DROPHILITE);

        lpnmh->code = NM_CLICK;

        if (hItem2 && (hItem2 == m_nachal || hItem3 == m_raion || hItem2 == m_magistr)) {
            m_ctrlTree.SelectItem(ht.hItem);
            POINT point = ht.pt;
            CMenu menu;
            VERIFY(menu.CreatePopupMenu());

            ClientToScreen(&point);

            int id = m_ctrlTree.GetItemData(ht.hItem);
            UMSRS* msrs = ps_alma.getMsRs(id);

            if (msrs) {

                m_id = msrs->id;
                m_mag = msrs->mag;


                setU(m_id);

                if (hItem2 == m_nachal || hItem3 == m_raion || hItem2 == m_magistr) {
                    m_ctrlTree.SelectItem(ht.hItem);
                    m_id2 = msrs->id;

                    m_hi = ht.hItem;
                    menu.AppendMenu(MF_STRING, ID_EDIT, _TR("Характеристика"));
                    menu.AppendMenu(MF_STRING, ID_MOVE2, _TR("Перейти к участку"));
                    menu.AppendMenu(MF_STRING, ID_EDIT_GRAPH, _TR("Редактировать участок"));

                    //      if (bline(line)->line.ms > 0) menu.AppendMenu(MF_STRING, ID_VYD_MS2, _TR("Обработать Участок МС"));
                    //      if (bline(line)->line.rs > 0) menu.AppendMenu(MF_STRING, ID_VYD_RS2, _TR("Обработать Участок РС"));

//                    menu.AppendMenu(MF_STRING, ID_NODE_PTS_UT, _TR("Создать участки ПТС заново"));
                    menu.AppendMenu(MF_STRING, ID_NODE_PTS, _TR("Сделать выделенные узлы узлами ПТС"));

                    //        menu.AppendMenu(MF_STRING, ID_PASSPORT, _TR("Паспорт"));
                    menu.AppendMenu(MF_STRING, ID_PASSPORT2, _TR("Паспорт"));

//                    menu.AppendMenu(MF_SEPARATOR);
//                    menu.AppendMenu(MF_STRING, ID_OSMOTR_NEW, _TR("Создать осмотр"));
//                    menu.AppendMenu(MF_STRING, ID_OSMOTR_LAST, _TR("Последний осмотр"));
//                    menu.AppendMenu(MF_STRING, ID_OSMOTR_HIST, _TR("История осмотров"));
                }
                menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);

                //      *pResult = FALSE;
                //      m_ctrlTree.Select(ht.hItem, TVGN_CARET);
                //      m_ctrlTree.SelectItem(ht.hItem);
            }
            return FALSE;
        }

        if (hItem1 == m_nachal) {
            //      AfxMessageBox("!", MB_OK|MB_ICONINFORMATION);

            m_ctrlTree.SelectItem(ht.hItem);
            POINT point = ht.pt;
            CMenu menu;
            VERIFY(menu.CreatePopupMenu());

            ClientToScreen(&point);

            int id = m_ctrlTree.GetItemData(ht.hItem);
            m_id = id;
            setU(m_id);

            m_id2 = id;

            m_hi = ht.hItem;
            menu.AppendMenu(MF_STRING, ID_MOVE_NACH, _TR("Перейти к участкам"));
            menu.AppendMenu(MF_STRING, ID_OSMOTR_HIST_NACH, _TR("История осмотров"));

            menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);

            return FALSE;
        }


        if (hItem1 == m_magistr) {
            //      AfxMessageBox("!", MB_OK|MB_ICONINFORMATION);

            m_ctrlTree.SelectItem(ht.hItem);
            POINT point = ht.pt;
            CMenu menu;
            VERIFY(menu.CreatePopupMenu());

            ClientToScreen(&point);

            int id = m_ctrlTree.GetItemData(ht.hItem);
            m_id = id;
            setU(m_id);

            m_id2 = id;

            m_hi = ht.hItem;
            menu.AppendMenu(MF_STRING, ID_EDIT_MAG, _TR("Характеристика магистрали"));
            menu.AppendMenu(MF_STRING, ID_MOVE_MAG, _TR("Перейти к магистрали"));
            menu.AppendMenu(MF_STRING, ID_EDIT_GRAPH_MAG, _TR("Редактировать магистраль"));
            //      menu.AppendMenu(MF_STRING, ID_EDIT_GRAPH_MAG, _TR("Редактировать магистраль"));

            menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);


        }
    }

    return CPaneDialog::OnNotify(wParam, lParam, pResult);
}

//const CNode *getNode2All(CCxema *m_cxema, const char *m_uzel);

void CPtsAlmaM2::OnSelchangeListUzel1()
{
    UpdateData(TRUE);
    CGidrView* pView = getView();
    if (!pView) return;

    CCxema* cxema = pView->m_cxema;

    CString kod = m_uzel1;

    map<CString, const CNode2*>::const_iterator it = map_node.find(kod);

    if (it != map_node.end()) {
        const CNode2* node = it->second;
        //    pView->moveXYmas(node->coord);
        pView->moveXY(node->node.coord);
        pView->BeginDrawNode(node);
    }
}

void CPtsAlmaM2::OnSelchangeListUzel2()
{
    UpdateData(TRUE);
    CGidrView* pView = getView();
    if (!pView) return;

//    pView->m_cxema->m_graph->init_find_line_nom_hps();

    CCxema* cxema = pView->m_cxema;

    map<int, int>::const_iterator it = map_line.find(m_Index2);

    if (it != map_line.end()) {
        CLINE2* l = pView->m_cxema->m_graph->find_line_big(it->second);
        if (l) {
            int idbig = bline(l)->line.pipeSectionID;

            //           CGidrView::moveLine(l)
            pView->moveXY(centroid(l));
            //           pView->BeginDrawLine(l);
            if (idbig > 0) {
                //                pView->m_cxema->m_graph->set_pipeSectionID(idbig);
                //                pView->MoveVyd();
                pView->m_linePTS_ID = idbig;
                pView->MoveLinePTS();
                pView->Invalidate();
            }
        }
    }


    /*

        CString kod = m_uzel2;

        map<CString, const CNode2*>::const_iterator it = map_node.find(kod);

        if (it != map_node.end()) {
            const CNode2* node = it->second;

            pView->moveXYmas(node->node.coord);
            pView->BeginDrawNode(node);
        }
    */
}

void CPtsAlmaM2::Init0()
{
    //  id_map = 0;

    m_ctrlTree.DeleteAllItems();

    m_nachal = m_ctrlTree.InsertItem(_TR("Начальники участков"), 0);

    initNach1(m_ctrlTree, m_nachal, m_mag);

    m_raion = m_ctrlTree.InsertItem(_TR("Районы эксплуатации"), 0);

    if (m_mag == RT_MS) {
        m_magistr = m_ctrlTree.InsertItem(_TR("Магистрали"), 0);
    }

    m_list_uzel1.ResetContent();
    m_list_uzel2.ResetContent();

    initMSRE(mapMSRS);
}

void CPtsAlmaM2::OnEditGraph()
{
    CGidrView* pView = getView();

    if (pView) {
        if (m_mag == 0) {
            pView->setMagNew(0, m_id2, 0, "", false);
        }
        else if (m_mag == 1) {
            pView->setMagNew(0, 0, m_id2, "", false);
        }
    }
}

void set_nodes_PTS(CAdoFile* ado, CGraph2* graph, bool ms, int ms_rs);
void reset_ms_rs(CAdoFile* ado, CGraph2* graph, bool ms, int ms_rs);

void CPtsAlmaM2::OnNodePTS()
{
    CGidrView* pView = getView();

    if (pView) {
        if (AfxMessageBox("Сделать выделенные узлы узлами ПТС?", MB_YESNO) == IDYES) {
            if (m_mag == 0) {
                set_nodes_PTS(pView->m_cxema->m_ado, pView->m_cxema->m_graph, true, m_id2);
            }
            else if (m_mag == 1) {
                set_nodes_PTS(pView->m_cxema->m_ado, pView->m_cxema->m_graph, false, m_id2);
            }
            Invalidate();
        }
    }
}


void CPtsAlmaM2::OnNodePTSUt()
{
    CGidrView* pView = getView();

    if (pView) {
        if (AfxMessageBox("Создать участки ПТС?", MB_YESNO) == IDYES) {
            if (m_mag == 0) {
                reset_ms_rs(pView->m_cxema->m_ado, pView->m_cxema->m_graph, true, m_id2);
            }
            else if (m_mag == 1) {
                reset_ms_rs(pView->m_cxema->m_ado, pView->m_cxema->m_graph, false, m_id2);
            }
            Invalidate();
        }
    }
}



void CPtsAlmaM2::OnEditGraphMag()
{
    CGidrView* pView = getView();

    if (pView) {
        pView->setMagNew(m_id2, 0, 0, "", false);
    }
}

void CPtsAlmaM2::OnMoveMag()
{
    CGidrView* pView = getView();

    if (pView) {
        CGraph2* graph = pView->m_cxema->m_graph;
        graph->reset();
        int n = vydMagN(graph, m_id2);

        if (graph->isMark()) {
            pView->MoveVyd();
        }
        else {
            AfxMessageBox(_TR("Магистраль не описана"), MB_OK | MB_ICONINFORMATION);
        }
    }
}

void CPtsAlmaM2::OnMoveNach()
{
    CGidrView* pView = getView();

    if (pView) {
        CGraph2* graph = pView->m_cxema->m_graph;
        graph->reset();
        int n = vydNach(graph, m_id2);

        if (graph->isMark()) {
            pView->MoveVyd();
        }
        else {
            AfxMessageBox(_TR("Участки не описаны"), MB_OK | MB_ICONINFORMATION);
        }
    }
}

void NeOpisanMessage(int id_ms, int id_rs)
{
    CString q;
/*
    if (m_mag == 0) {
        q.Format(
            "SELECT DISTINCT fr.name\n"
            "FROM heatPipeSections hps\n"
            "JOIN linesobj l ON l.id=hps.lineID\n"
            "JOIN nodes n1 ON n1.id=l.nodeID1\n"
            "JOIN fragments fr ON fr.id=n1.fileID\n"
            "WHERE hps.magistralSite=%d\n", m_id2);
    }
    else {
        q.Format(
            "SELECT DISTINCT fr.name\n"
            "FROM heatPipeSections hps\n"
            "JOIN linesobj l ON l.id=hps.lineID\n"
            "JOIN nodes n1 ON n1.id=l.nodeID1\n"
            "JOIN fragments fr ON fr.id=n1.fileID\n"
            "WHERE hps.distSite=%d\n", m_id2);
    }
*/

    if (id_ms == 0) id_ms = -1;
    if (id_rs == 0) id_rs = -1;

     q.Format(
         "SELECT DISTINCT fr.name\n"
         "FROM heatPipeSections hps\n"
         "JOIN linesobj l ON l.id=hps.lineID AND l.removed=0\n"
         "JOIN nodes n1 ON n1.id=l.nodeID1\n"
         "JOIN fragments fr ON fr.id=n1.fileID\n"
         "WHERE (hps.magistralSite=%d OR hps.distSite=%d)\n", id_ms, id_rs);


    CAdoFile* ado = getAdo(getGidAdoName());
    if (!ado) return;
    CString s, name = readTableValue(ado, q);
    if (name != "") {
//        s.Format("Не могу найти участок\nОткройте фрагмент %s", name);
        s.Format("Участок отобразить невозможно, так как фрагмент %s не подключен. Подключите фрагмент командой: \"Файл>Отобразить дополнительную схему\"", name);
    }
    else {
        s.Format("Участок не описан");
    }
    AfxMessageBox(s);
}


void CPtsAlmaM2::OnMove2()
{
    CGidrView* pView = getView();
    if (!pView) return;

    bool fnd = false;

    CGraph2* graph = pView->m_cxema->m_graph;
    vydMSRS(m_id2, false);
    if (graph->isMark()) {
        pView->MoveVyd();
    }
    else {
      NeOpisanMessage(m_mag == 0 ? m_id2 : 0, m_mag == 1 ? m_id2 : 0);
#if 0
        CString q;
        if (m_mag == 0) {
            q.Format(
                "SELECT DISTINCT fr.name\n"
                "FROM heatPipeSections hps\n"
                "JOIN linesobj l ON l.id=hps.lineID\n"
                "JOIN nodes n1 ON n1.id=l.nodeID1\n"
                "JOIN fragments fr ON fr.id=n1.fileID\n"
                "WHERE hps.magistralSite=%d\n", m_id2);
        }
        else {
            q.Format(
                "SELECT DISTINCT fr.name\n"
                "FROM heatPipeSections hps\n"
                "JOIN linesobj l ON l.id=hps.lineID\n"
                "JOIN nodes n1 ON n1.id=l.nodeID1\n"
                "JOIN fragments fr ON fr.id=n1.fileID\n"
                "WHERE hps.distSite=%d\n", m_id2);
        }

        CAdoFile* ado = getAdo(getGidAdoName());
        if (!ado) return;
        CString s, name = readTableValue(ado, q);
        if (name != "") {
//            s.Format("Не могу найти участок\nОткройте фрагмент %s", name);
            s.Format("Участок отобразить невозможно, так как фрагмент %s не подключен. Подключите фрагмент командой: \"Файл>Отобразить дополнительную схему\"", name);
        }
        else {
            s.Format("Участок не описан");
        }
        AfxMessageBox(s);
#endif
    }
}

void setMS(CGraph2* m_graph, int typ, const char* kod0, int val);
void setMagMsRs(CGraph2* m_graph, int typ, int mag, int ms, int rs) {};
//int editWithDoc2(CWnd *parent, C1DbFile *dbf, const char *tn, const char *elem, long kk, int ro) { return 0;};

#include "ado2.h"


void CPtsAlmaM2::OnPopupEdit()
{
    CGidrView* pView = getView();

    if (pView) {
        CString tab = "";
        CString tab2 = "";
        CString tab3 = "";
        CString tab4 = "";

        TYP_OF_P m127 = TP_MS;

        switch (m_mag) {
        case RT_MS:
            tab = _TR("Участок МС");
            tab2 = _TR("Паспорт участка МС");
            tab3 = "uchastok_ms";
            m127 = TP_MS;
            break;
        case RT_RS:
            tab = _TR("Участок РС");
            tab2 = _TR("Паспорт участка РС");
            tab3 = "uchastok_rs";
            m127 = TP_RS;
            break;
        }

        CAdoFile* ado = getAdo(getPsAdoName());

        if (!ado) return;

        int ret = (int)ado_info(pView, ado, tab3, m_id, tab, m127, NULL, false);
    }
}

void CPtsAlmaM2::OnPopupEditMag()
{
    CGidrView* pView = getView();

    if (pView) {
        CString q;
        CString tab = _TR("Магистрали");

        CString tab2 = _TR("Паспорт участка РС");
        CString tab3 = "magistrali";

        CAdoFile* ado = getAdo(getPsAdoName());

        if (!ado) return;

        int ret = (int)ado_info(this, ado, tab3, m_id, tab, TP_ZERO, NULL, false);
    }
}


void CPtsAlmaM2::setU(int id)
{
}

//#include "PtsSheetAlma.h"
//extern CPtsSheetAlma *pm_cSheetPtsAlma;


HTREEITEM findItemLevel(CTreeCtrl& m_ctrlTree, HTREEITEM i1, int nach, int level)
{
    CString str = m_ctrlTree.GetItemText(i1);
    int id = (int)m_ctrlTree.GetItemData(i1);
    if (id == nach && level == 0) {
        return i1;
    }

    if (level) {
        HTREEITEM hti = m_ctrlTree.GetChildItem(i1);

        while (hti) {

            HTREEITEM hti1 = findItemLevel(m_ctrlTree, hti, nach, level - 1);
            if (hti1) return hti1;

            hti = m_ctrlTree.GetNextItem(hti, TVGN_NEXT);
        }
    }
    return 0;
}

HTREEITEM findItem(CTreeCtrl& m_ctrlTree, HTREEITEM i1, int nach)
{
    CString str = m_ctrlTree.GetItemText(i1);
    int id = (int)m_ctrlTree.GetItemData(i1);
    if (id == nach) {
        return i1;
    }

    HTREEITEM hti = m_ctrlTree.GetChildItem(i1);

    while (hti) {

        HTREEITEM hti1 = findItem(m_ctrlTree, hti, nach);
        if (hti1) return hti1;

        hti = m_ctrlTree.GetNextItem(hti, TVGN_NEXT);
    }
    return 0;
}


bool CPtsAlmaM2::selectKod(CWnd* wnd, CTreeCtrl& m_ctrlTree, HTREEITEM m_nachal, int ue, int id, int m_mag)
{
    int nach = 0;

    map<int, UE>::const_iterator itUE = ps_alma.map_UE.find(ue);
    if (itUE != ps_alma.map_UE.end()) {
        UE ue = itUE->second;
        nach = ue.nach;
    }

    //  if ( pm_cSheetPtsAlma->showPage(wnd, 1)) {
//    initNach1(m_ctrlTree, m_nachal, m_mag);

    HTREEITEM ht = findItemLevel(m_ctrlTree, m_nachal, ue, 1);

    if (ht) {
        //        initMag2(m_ctrlTree, ht, ue, m_mag);

        UMSRS* msrs = ps_alma.getMsRs(id);
        int id2 = ps_alma.getMsRs2(id, m_mag);


        HTREEITEM ht1 = findItemLevel(m_ctrlTree, ht, id2, 1);

        if (ht1) {
            m_ctrlTree.SelectItem(ht1);
            Click(ht1);
            return true;
        }

    }
    return false;
}

bool CPtsAlmaM2::findMsRs(int id)
{
    CGidrView* pView = getView();
    if (!pView) return false;
    CCxema* cxema = pView->m_cxema;

    m_ctrlTree.DeleteAllItems();

    ps_alma.init();

    Init0();

    map<int, UMSRS>::const_iterator it = mapMSRS.begin();
    for (; it != mapMSRS.end(); it++) {
        UMSRS ums = it->second;
        if (ums.id == id && ums.mag == m_mag) {
        //if (ums.id == id) {
            BOOL delay = TRUE;
            ShowPane(TRUE, delay, TRUE);
            bool ret = selectKod(this, m_ctrlTree, m_nachal, ums.ue, id, m_mag);


            return ret;
        }
    }
    return false;
}



bool CPtsAlmaM2::findMag(int mag)
{
    CGidrView* pView = getView();
    if (!pView) return false;
    CCxema* cxema = pView->m_cxema;

    m_ctrlTree.DeleteAllItems();
    Init0();
    initMagistr(m_ctrlTree, m_magistr);


    HTREEITEM ht = findItemLevel(m_ctrlTree, m_magistr, mag, 1);
    if (ht) {
        BOOL delay = TRUE;
        ShowPane(TRUE, delay, TRUE);
        m_ctrlTree.SelectItem(ht);

        Click(ht);

        return true;
    }
    return false;
}



void CPtsAlmaM2::reset()
{
    m_ctrlTree.DeleteAllItems();
    Init0();
}

/*
void findMS2(const char *kod)
  {
  if (!pm_cSheetPtsAlma) return;

  Cgid6App* pApp = (Cgid6App*) AfxGetApp();
  CMDIFrameWnd* pMainFrame = (CMDIFrameWnd *)pApp->m_pMainWnd;

  bool fnd;

  fnd = pm_cSheetPtsAlma->m_almaM2->findMsRs(kod);
  if (!fnd) fnd = pm_cSheetPtsAlma->m_almaR2->findMSRS(kod);

  if (!fnd) {
    UMS *ms = ps_alma.getMS(kod);
  }
  if (!fnd) {
    URS *rs = ps_alma.getRS(kod);
    if (!rs) {
    }
  }
}
*/
void resetMS2()
{
    //  if (!pm_cSheetPtsAlma) return;

    ps_alma.init();
    //  pm_cSheetPtsAlma->m_almaM2->reset();
    //  pm_cSheetPtsAlma->m_almaR2->reset();
}



void findMag(int mag)
{
    CPtsAlmaM2* msR = (CPtsAlmaM2*)getDockablePane(IDD_PTS_RIGHT_MS);
    if (msR) msR->findMag(mag);
}

void findMs(int mag)
{
    CPtsAlmaM2* msR = (CPtsAlmaM2*)getDockablePane(IDD_PTS_RIGHT_MS);
    if (msR) msR->findMsRs(mag);
}

void findRs(int mag)
{
    CPtsAlmaM2* rsR = (CPtsAlmaM2*)getDockablePane(IDD_PTS_RIGHT_RS);
    if (rsR) rsR->findMsRs(mag);
}


void CPtsAlmaM2::OnPassport()
{
    CGidrView* pView = getView();
    if (!pView) return;

    pView->PaspRSMS(0, 0, m_mag, m_id2);
}


void CPtsAlmaM2::OnPassport2()
{
    CGidrView* pView = getView();
    if (!pView) return;

    /*
        if (m_mag == 0) {
            reset_ms_rs(pView->m_cxema->m_ado, pView->m_cxema->m_graph, true, m_id2);
        }
        else if (m_mag == 1) {
            reset_ms_rs(pView->m_cxema->m_ado, pView->m_cxema->m_graph, false, m_id2);
        }
    */

    pView->PaspRSMS_2(0, 0, m_mag, m_id2);
}



int vydMagN(CGraph2* m_graph, int id)
{
    CMapGraph::const_iterator p = m_graph->map_node.begin();

    int nn = 0;

    while (p != m_graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.coord.x != 0 || n->node.coord.y != 0) {

            map<string, string>::const_iterator it;

            CLINE2* l;
            for (l = n->lines; l; l = next(l)) {
                CLine2* ll = bline(l);

                if (ll->line.mag == id) {
                    ll->line.isPjezo = 1;
                    ll->line.isPjezo = 1;
                    where(l)->node.isPjezo = 1;
                    other(l)->node.isPjezo = 1;
                    nn++;
                }
            }
        }
        p++;
    }
    return nn;
}


int vydNach(CGraph2* m_graph, int id)
{
    CMapGraph::const_iterator p = m_graph->map_node.begin();

    int nn = 0;

    set<int> set_ms;
    set<int> set_rs;

    for (auto it : ps_alma.map_UMS) {
        auto it2 = ps_alma.map_UE.find(it.second.ue);
        if (it2 != ps_alma.map_UE.end()) {
            if (it2->second.id == id) {
                set_ms.insert(it.second.id);
            }
        }
    }
    for (auto it : ps_alma.map_URS) {
        auto it2 = ps_alma.map_UE.find(it.second.ue);
        if (it2 != ps_alma.map_UE.end()) {
            if (it2->second.id == id) {
                set_rs.insert(it.second.id);
            }
        }
    }


    while (p != m_graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.coord.x != 0 || n->node.coord.y != 0) {

            map<string, string>::const_iterator it;

            CLINE2* l;
            for (l = n->lines; l; l = next(l)) {
                CLine2* ll = bline(l);

                auto itm = set_ms.find(ll->line.ms);
                auto itr = set_rs.find(ll->line.rs);

                if (itm != set_ms.end() || itr != set_rs.end()) {
                    ll->line.isPjezo = 1;
                    ll->line.isPjezo = 1;
                    where(l)->node.isPjezo = 1;
                    other(l)->node.isPjezo = 1;
                    nn++;
                }
            }
        }
        p++;
    }
    return nn;
}



LRESULT CPtsAlmaM2::OnAdoUpdated(WPARAM wParam, LPARAM)
{
    CAdoFile* ado = getAdo(getGidAdoName());

    if (ado) {
    }
    return 0;
}

#include "mmenu.h"

int InsertPipeSections(CAdoFile* ado, int idNode1, int idNode2, int magistral, int rs, double len, int id1, int id2, CString qq)
{
    CString q;

    CString mag_rs = "distSite";

    if (magistral) {
        mag_rs = "magistral";
    }
    else {
        magistral = rs;
    }

    q.Format("INSERT INTO %s (nodeID1, nodeID2, %s, pipeLength) VALUES (%d, %d, %d, %g)", "pipeSections", mag_rs, idNode1, idNode2, magistral, len);

    long affected;
    long id = ado->ExecuteInsert(q, &affected);

    if (id) {
        q.Format("UPDATE heatPipeSections SET pipeSectionID=%d WHERE ID IN (%s)", id, qq);
        ado->Execute(q, &affected);

        if (ado->m_type_of_net == 0)
        {  // MySQL
            q.Format("UPDATE pipeSections hs JOIN linesobj l ON l.id=%d AND hs.id=%d SET hs.organizationID=l.organizationID", id1, id);
        }
        else {
            q.Format("UPDATE hs SET hs.organizationID=l.organizationID FROM pipeSections hs JOIN linesobj l ON l.id=%d AND hs.id=%d", id1, id);
        }
        ado->Execute(q, &affected);

        if (ado->m_type_of_net == 0)
        {  // MySQL
            q.Format("UPDATE pipeSections hs JOIN heatPipeSections hps ON hps.id=%d AND hs.id=%d "
                " SET "

                " hs.DiamUslov=hps.diameterCondit, "
                " hs.tubingTypeID=hps.tubingTypeID, "

                " hs.tubeTypeID=hps.tubeTypeID, "
                " hs.firstPICdateHP=hps.firstPICdateHP, "
                " hs.lastTransDate=hps.lastTransDate, "
                " hs.lastIsolDate=hps.lastTransDate "
                //        " hs.sectExploitPeriod=hps.sectExploitPeriod, "
                //        " hs.sectExploitPeriod=FLOOR(DATEDIFF(NOW(), lastTransDate)/365)"
                , id2, id);
        }
        else {
            q.Format("UPDATE hs "
                " SET "

                " hs.DiamUslov=hps.diameterCondit, "
                " hs.tubingTypeID=hps.tubingTypeID, "

                " hs.tubeTypeID=hps.tubeTypeID, "
                " hs.firstPICdateHP=hps.firstPICdateHP, "
                " hs.lastTransDate=hps.lastTransDate, "
                " hs.lastIsolDate=hps.lastTransDate "
                //        " hs.sectExploitPeriod=hps.sectExploitPeriod, "
                //        " hs.sectExploitPeriod=FLOOR(DATEDIFF(NOW(), lastTransDate)/365)"

                " FROM pipeSections hs JOIN heatPipeSections hps ON hps.id=%d AND hs.id=%d "

                , id2, id);
        }

        if (!ado->Execute(q, &affected)) {
            AfxMessageBox(ado->getError(), MB_OK | MB_ICONINFORMATION);
        }



    }
    else {
        AfxMessageBox(ado->getError(), MB_OK | MB_ICONINFORMATION);
    }


    return id;
}

#if 1

bool copyID(CWnd* wnd, CAdoFile* ado, CGraph2* graph, int idMAG, int idRS)
{
    //    CGraph2 *graph = m_cxema->m_graph;

    graph->reset();

    int n = 0;

    if (idMAG) {
        n = vydMagN(graph, idMAG);
    }
    else if (idRS) {
        n = viewUchastok(graph, make_tuple(0, idRS), 2, true);
    }

    if (n > 1000) {
        CString ss;
        ss.Format("%d", n);
        AfxMessageBox(ss, MB_OK | MB_ICONINFORMATION);
    }

    list<const CNode2*> list_all_node;
    list<CLINE2*> list_l;

    fillLineList(graph, list_l);

    //    CMMenu menu2(wnd);

    CNode2* nn1 = NULL;
    CNode2* nn2 = NULL;

    CNode2* n1_old = NULL;
    CNode2* n2_old = NULL;

    CString qq = "";

    int napr = 0;

    list<CLINE2*>::const_iterator it = list_l.begin();

    for (it = list_l.begin(); it != list_l.end(); it++) {
        CLINE2* l2 = *it;
        CString s;

        CNode2* n1 = where(l2);
        CNode2* n2 = other(l2);

        int nn1 = num_MSRS2(n1->node.name);
        int nn2 = num_MSRS2(n2->node.name);

        if (nn1 && nn2) {
            if (nn1 > nn2) napr++;
            if (nn1 < nn2) napr--;
        }
    }

    if (napr > 0) {
        list_l.reverse();
    }

    double len = 0;
    double len_max = -1;
    int id_max = -1;
    int id2_max = -1;

    for (it = list_l.begin(); it != list_l.end(); it++) {
        CLINE2* l2 = *it;
        CString s;

        CNode2* n1 = where(l2);
        CNode2* n2 = other(l2);

        if (napr > 0) {
            n2 = where(l2);
            n1 = other(l2);
        }

        bool bb = false;

        if (n1 != n2_old) bb = true;
        if (isPS(n1)) bb = true;

        if (bb) {
            nn2 = n2_old;
            if (nn1 && nn2) {
                InsertPipeSections(ado, nn1->id, nn2->id, idMAG, idRS, len, id_max, id2_max, qq);
                qq = "";

                len = 0;
                len_max = -1;
                id_max = -1;
                id2_max = -1;
            }
            nn1 = n1;
        }

        s.Format("%d,%d", bline(l2)->line.idP2, bline(l2)->line.idO2);
        if (qq != "") qq += ",";
        qq += s;

        double dl = bline(l2)->line.nomP > 0 ? bline(l2)->line.pod.dl : bline(l2)->line.obr.dl;

        if (dl > len_max) {
            len_max = dl;
            id_max = bline(l2)->line.nomP;
            id2_max = bline(l2)->line.idP2;
        }
        len += dl;

        //        if (bb && n2_old) menu2.Add("---------", (void*)0);

        s.Format("%s %s", n1->getName(), n2->getName());
        //        menu2.Add(s, (void*)0);

        n1_old = n1;
        n2_old = n2;
    }

    nn2 = n2_old;

    if (nn1 && nn2) {
        InsertPipeSections(ado, nn1->id, nn2->id, idMAG, idRS, len, id_max, id2_max, qq);
    }

    //    if (menu2.DoModal() == IDOK) {
    //    }

    return true;
}

void CGidrView::OnRemontUchList()
{
    if (AfxMessageBox(_TR("Сформировать новый список участков тепловой сети?"), MB_YESNO) != IDYES) return;

    CAdoFile* ado = getAdo(getGidAdoName());
    if (!ado) return;


    long affected;
    CString q;

    q = "UPDATE  hps"
        " SET hps.magistral=ms.magistral"
        " FROM heatPipeSections hps"
        " JOIN linesobj l ON l.id=hps.lineID"
        " JOIN uchastok_ms ms ON ms.id=hps.magistralSite";

    ado->Execute(q, &affected);

    q.Format("UPDATE heatPipeSections SET pipeSectionID=NULL");

    ado->Execute(q, &affected);

    q.Format("DELETE FROM pipeSections");

    ado->ExecuteInsert(q, &affected);

    if (1) {
        Percent2 pc;
        {
            int sz = ps_alma.mapMAG.size();

            map<int, UMAG>::const_iterator it1 = ps_alma.mapMAG.begin();

            for (int i = 0; it1 != ps_alma.mapMAG.end(); it1++, i++) {
                UMAG mag = it1->second;
                copyID(this, ado, m_cxema->m_graph, mag.id, 0);
                pc.SetValue(100. * i / sz / 2);
            }
        }

        {
            int sz = ps_alma.map_URS.size();

            map<int, URS>::const_iterator it1 = ps_alma.map_URS.begin();
            for (int i = 0; it1 != ps_alma.map_URS.end(); it1++, i++) {
                URS rs = it1->second;
                copyID(this, ado, m_cxema->m_graph, 0, rs.id);
                pc.SetValue(50 + 100. * i / sz / 2);
            }
        }
    }
    else {
        map<int, UMAG>::const_iterator it1 = ps_alma.mapMAG.find(23);

        if (it1 != ps_alma.mapMAG.end()) {
            UMAG mag = it1->second;
            copyID(this, ado, m_cxema->m_graph, mag.id, 0);
        }
    }

    q.Format("UPDATE pipeSections p JOIN nodes n1 ON n1.id=p.nodeID1 JOIN nodes n2 ON n2.id=p.nodeID2 SET p.name=CONCAT(n1.externalNodeName, \" - \", n2.externalNodeName)");

    ado->Execute(q, &affected);
}

#else

/*

void CGidrView::OnRemontUchList()
{
    CAdoFile *ado = getAdo(getGidAdoName());
    if (!ado) return;

    CMMenu menu(this);

    map<int, URS>::const_iterator it1 = ps_alma.map_URS.begin();

    Percent pc;

    int sz = ps_alma.map_URS.size()

    int i = 0;

    for (; it1 != ps_alma.map_URS.end(); it1++) {
        URS rs = it1->second;
        menu.Add(rs.opis, (void*)rs.id);
    }

    if (menu.DoModal() == IDOK) {
        int id = (int)menu.getV();

        CGraph2 *graph = m_cxema->m_graph;

        graph->reset();

        int n = vydRsN(graph, id);

        CString ss;

        if (n > 1000) {
            ss.Format("%d", n);
            AfxMessageBox(ss, MB_OK|MB_ICONINFORMATION);
        }

        list<const CNode2 *> list_all_node;
        list<CLINE2 *> list_l;

        fillLineList(graph, list_l);

        list<CLINE2 *>::const_iterator it = list_l.begin();

        CMMenu menu2(this);

        CNode2 *n1_old = NULL;
        CNode2 *n2_old = NULL;

        for (; it != list_l.end(); it++) {
            CLINE2 *l2 = *it;
            CString s;

            CNode2 *n1 = where(l2);
            CNode2 *n2 = other(l2);

            if (n1 != n2_old && n2_old) menu2.Add("---------", (void*)0);

            s.Format("%s %s", n1->getName(), n2->getName());
            menu2.Add(s, (void*)0);

            n1_old = n1;
            n2_old = n2;
        }

        if (menu2.DoModal() == IDOK) {
        }
    }
}
*/
#endif


void CGidrView::OnFontPanel()
{
    LOGFONT lf;
    COLORREF color;
    CFont font;

    font.CreatePointFont(120, _T("Tahoma"));
    font.GetLogFont(&lf);
    font.DeleteObject();

    LoadFont2("PanelFont", lf, color);

    CFontDialog fd(&lf);

    if (fd.DoModal() == IDOK) {
        fd.GetCurrentFont(&lf);

        SaveFont(_T("PanelFont"), lf, fd.GetColor());
        AfxMessageBox("Для изменения шрифта в проводниках выйдите из программы и зайдите опять", MB_OK | MB_ICONINFORMATION);
    }
}
