// Распредсеть при создании
//

#include "stdafx.h"
#include "gid6.h"
#include "PtsOpressovka.h"

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


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CGeoFile *m_geofile;

CDockablePane * getDockablePane(int nID);


static HTREEITEM m_hItem = 0;
static HTREEITEM m_hItem0 = 0;

/////////////////////////////////////////////////////////////////////////////
// CPtsOpressovka1 property page

//IMPLEMENT_DYNCREATE(CPtsOpressovka, CPtsTab)

void findMs(int mag);
void findRs(int mag);



CPtsOpressovka::CPtsOpressovka(const char *cap, int is_mag) 
//CPtsOpressovka::CPtsOpressovka() 
{
  //{{AFX_DATA_INIT(CPtsOpressovka)
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
  m_mag  = is_mag;
}

CPtsOpressovka::~CPtsOpressovka()
{
}

void CPtsOpressovka::DoDataExchange(CDataExchange* pDX)
{
    CPaneDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CPtsOpressovka)
  DDX_Control(pDX, IDC_LIST1, m_list_uzel1);
  DDX_Control(pDX, IDC_LIST2, m_list_uzel2);
  DDX_Control(pDX, IDC_TREE, m_ctrlTree);
  DDX_LBString(pDX, IDC_LIST1, m_uzel1);
  DDX_LBString(pDX, IDC_LIST2, m_uzel2);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPtsOpressovka, CPaneDialog)
  //{{AFX_MSG_MAP(CPtsOpressovka)
//  ON_COMMAND(ID_VYD, OnVyd)
//  ON_WM_CREATE()

  ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
  ON_COMMAND(ID_PASSPORT, OnPassport)
  ON_COMMAND(ID_EDIT, OnPopupEdit)
  ON_COMMAND(ID_EDIT_MAG, OnPopupEditMag)
  ON_COMMAND(ID_EDIT_GRAPH, OnEditGraph)
  ON_COMMAND(ID_EDIT_GRAPH_MAG, OnEditGraphMag)
  ON_COMMAND(ID_MOVE2, OnMove2)
  ON_COMMAND(ID_MOVE_MAG, OnMoveMag)
  ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeListUzel1)
  ON_LBN_SELCHANGE(IDC_LIST2, OnSelchangeListUzel2)
  ON_WM_SIZE()
  ON_WM_CONTEXTMENU()
  ON_MESSAGE(WM_USER_ADO_UPDATED, OnAdoUpdated)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPtsOpressovka message handlers

void CPtsOpressovka::OnSize(UINT nType, int cx, int cy) 
{
  CPaneDialog::OnSize(nType, cx, cy);

  if (!m_list_uzel2.m_hWnd) return;

  CRect sz, sz1, sz2, sz3;

  GetClientRect(sz);
//  sz.DeflateRect(3,3);

  CRect rectClient;
  GetClientRect(rectClient);

//  int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;
  int cyTlb = 0;

//  m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
//  m_wndClassView.SetWindowPos(nullptr, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);

  int h = sz.bottom;
  int w = sz.right;

  sz1 = sz; sz1.bottom = h/2;
  sz2 = sz; sz2.top = sz1.bottom;
  sz2.bottom -= 70;

  sz3 = sz; sz3.top = sz2.bottom;

//  sz1.DeflateRect(1,1);
//  sz2.DeflateRect(1,1);
//  sz3.DeflateRect(1,1);

  m_ctrlTree.MoveWindow(sz1);
  m_list_uzel1.MoveWindow(sz2);
  m_list_uzel2.MoveWindow(sz3);

//  m_ctrlTree.SetWindowPos(nullptr, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}









void copyVydGraphAll(CGraph2 *graph1, CGraph2 *graph2, list<CNode*> &list_sort, list<CLINE2 *> &list_l, int mag_typ, int mag_id);
CString getPSTable(int n);


int vydMagN(CGraph2 *m_graph, int id);


void CPtsOpressovka::vydMSRS(int id)
{
}




static void initUE(CTreeCtrl &m_ctrlTree, HTREEITEM i1, int id, int is_mag)
{
}

//BOOL CPtsOpressovka::OnInitDialog() 
//{
//  CPaneDialog::OnInitDialog();






//int CPtsOpressovka::OnCreate(LPCREATESTRUCT lpCreateStruct)
LRESULT CPtsOpressovka::HandleInitDialog(WPARAM, LPARAM)
{
  UpdateData(FALSE);

  HTREEITEM i0 = 0;

  m_imgList.Create( IDB_TREE, 16, 1, RGB( 0, 128, 128 ) );
  m_ctrlTree.SetImageList( &m_imgList, TVSIL_NORMAL );

  void setTreeFont(CTreeCtrl * m_ctrlTree, CFont * m_Font);
  setTreeFont(&m_ctrlTree, &m_Font);

  m_ctrlTree.SetItemData(TVI_ROOT, (DWORD) 0);

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


BOOL CPtsOpressovka::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
  // TODO: Add your specialized code here and/or call the base class

  LPNMHDR lpnmh = (LPNMHDR) lParam;
  TVHITTESTINFO ht = {0};

  HTREEITEM hItemChild = 0;
  HTREEITEM hItem1 = 0;
  HTREEITEM hItem2 = 0;
  HTREEITEM hItem3 = 0;
  HTREEITEM hItem4 = 0;


  if ((lpnmh->code == NM_CLICK) || (lpnmh->code  == NM_RCLICK)) {

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
    hItem4 = m_ctrlTree.GetParentItem(hItem3);

    if (ht.hItem) {
      CDocument *pDocument = (CDocument *) m_ctrlTree.GetItemData(ht.hItem);
      m_hItem = ht.hItem;
      if (hItem2 == m_nachal) {  // Выбрана распредсеть
         m_id = m_ctrlTree.GetItemData(ht.hItem);
         setU(m_id);
      }
    }
  }

  if ((lpnmh->code == NM_CLICK))
  {
  }


  if ((lpnmh->code == NM_RCLICK))
  {
  }

  return CPaneDialog::OnNotify(wParam, lParam, pResult);
}

//const CNode *getNode2All(CCxema *m_cxema, const char *m_uzel);

void CPtsOpressovka::OnSelchangeListUzel1() 
{
  UpdateData(TRUE);
  CGidrView *pView = getView();
  if (!pView) return;

  CCxema *cxema = pView->m_cxema;

  CString kod = m_uzel1;

  map<CString, const CNode2 *>::const_iterator it = map_node.find(kod);

  if (it != map_node.end()) {
    const CNode2 *node = it->second;
//    pView->moveXYmas(node->coord);
    pView->moveXY(node->node.coord);
    pView->BeginDrawNode(node);
  }
}

void CPtsOpressovka::OnSelchangeListUzel2() 
{
  UpdateData(TRUE);
  CGidrView *pView = getView();
  if (!pView) return;

  CCxema *cxema = pView->m_cxema;

  CString kod = m_uzel2;

  map<CString, const CNode2 *>::const_iterator it = map_node.find(kod);

  if (it != map_node.end()) {
    const CNode2 *node = it->second;

    pView->moveXYmas(node->node.coord);
    pView->BeginDrawNode(node);
  }
}

void CPtsOpressovka::Init0() 
{
}

void CPtsOpressovka::OnEditGraph()
{
  CGidrView *pView = getView();

  if (pView) {
  }
}

void CPtsOpressovka::OnEditGraphMag()
{
  CGidrView *pView = getView();

  if (pView) {
    pView->setMagNew(m_id2, 0, 0, "", false);
  }
}

void CPtsOpressovka::OnMoveMag()
{
  CGidrView *pView = getView();

  if (pView) {
    CGraph2 *graph = pView->m_cxema->m_graph;
    graph->reset();
    int n = vydMagN(graph, m_id2);

    if (graph->isMark()) {
      pView->MoveVyd();
    }
    else {
      AfxMessageBox(_TR("Магистраль не описана"), MB_OK|MB_ICONINFORMATION);
    }
  }
}


void CPtsOpressovka::OnMove2()
{
  CGidrView *pView = getView();

  bool fnd = false;

  if (pView) {
    CGraph2 *graph = pView->m_cxema->m_graph;
    vydMSRS(m_id2);
    if (graph->isMark()) {
      pView->MoveVyd();
    }
    else {
      AfxMessageBox(_TR("Участок не описан"), MB_OK|MB_ICONINFORMATION);
    }
  }
}

void setMS(CGraph2 *m_graph, int typ, const char *kod0, int val);
void setMagMsRs(CGraph2 *m_graph, int typ, int mag, int ms, int rs);
//int editWithDoc2(CWnd *parent, C1DbFile *dbf, const char *tn, const char *elem, long kk, int ro) { return 0;};

#include "ado2.h"

void CPtsOpressovka::OnPopupEdit()
{
  CGidrView *pView = getView();

  if (pView) {
    CString tab = "";
    CString tab2 = "";
    CString tab3 = "";
    CString tab4 = "";

    switch(m_mag) {
    case RT_MS : 
      tab = _TR("Участок МС"); 
      tab2 = _TR("Паспорт участка МС"); 
      tab3 = "uchastok_ms"; 
      break;
    case RT_RS : 
      tab = _TR("Участок РС"); 
      tab2 = _TR("Паспорт участка РС"); 
      tab3 = "uchastok_rs"; 
      break;
    }

    CAdoFile *ado = getAdo(getPsAdoName());

    if (!ado) return;

    ado_info(this, ado, tab3, m_id, tab, TP_ZERO, NULL, false);


/*

    C1DbFile dbf;

    if (dbf.openMDB(getPSAlma())) {

      int ret = editWithDoc2(pView, &dbf, tab, tab2, m_id2, false);

      if (ret == IDOK) {
        if (m_mag  == RT_MS) {
          setMagMsRs(pView->m_cxema->m_graph, TIP_UT, 0, m_id2, 0);
          findMs(m_id2);
        }
        else {
          setMagMsRs(pView->m_cxema->m_graph, TIP_UT, 0, 0, m_id2);
          findRs(m_id2);
        }
      }
      dbf.closeMDB();
    }
*/

/*
    CString q;
    q.Format("SELECT * FROM [%s] WHERE ID=%d", tab, m_id2);

    int ret = editQ_1(this, getPSAlma(), tab, q, -1, FALSE);

    if (ret == IDOK) {
//      setMS(graph, TIP_UT, _TR("Участок очереди опрессовки"), m_id2);
      if (m_mag  == RT_MS) {
        setMagMsRs(pView->m_cxema->m_graph, TIP_UT, 0, m_id2, 0);
        findMs(m_id2);
      }
      else {
        setMagMsRs(pView->m_cxema->m_graph, TIP_UT, 0, 0, m_id2);
        findRs(m_id2);
      }
    }
*/

  }
}

void CPtsOpressovka::OnPopupEditMag()
{
  CGidrView *pView = getView();

  if (pView) {
    CString q;
    CString tab = _TR("Магистрали");

    CString tab2 = _TR("Паспорт участка РС"); 
    CString tab3 = "magistrali";

    CAdoFile *ado = getAdo(getPsAdoName());

    if (!ado) return;

    ado_info(this, ado, tab3, m_id, tab, TP_ZERO, NULL, false);
  }
}


void CPtsOpressovka::setU(int id) 
{
}

//#include "PtsSheetAlma.h"
//extern CPtsSheetAlma *pm_cSheetPtsAlma;


HTREEITEM findItemLevel(CTreeCtrl& m_ctrlTree, HTREEITEM i1, int nach, int level);
HTREEITEM findItem(CTreeCtrl& m_ctrlTree, HTREEITEM i1, int nach);



bool CPtsOpressovka::findMsRs(int id)
{
  return false;
}



bool CPtsOpressovka::findMag(int mag)
{
  return false;
}



void CPtsOpressovka::reset()
{
  m_ctrlTree.DeleteAllItems();
  Init0();
}


void CPtsOpressovka::OnPassport()
{
  CGidrView *pView = getView();
  if (!pView) return;

  pView->PaspRSMS(0, 0, m_mag, m_id2);
}




LRESULT CPtsOpressovka::OnAdoUpdated(WPARAM wParam, LPARAM)
{
    CAdoFile *ado = getAdo(getGidAdoName());

    if (ado) {
    }
    return 0;
}

