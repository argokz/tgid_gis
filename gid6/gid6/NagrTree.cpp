// NagrTree.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "NagrTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

double getScreenMas();


/////////////////////////////////////////////////////////////////////////////
// CNagrTree dialog


struct Nagr
{
  Nagr() : o(0), g(0), v(0) {};
  double o, g, v;
};


CNode2 *other2(CLINE2 *l0) 
{
  CNode2 *o = other(l0);

  if (o->node.typ == TIP_PR || o->node.typ == TIP_PO/* || o->name_typ_alma*/) return o;

  CNode2 *n1 = o;
  int n_v = 0;

  CLINE2 *ln = NULL;

  for (CLINE2 *l = n1->lines; l; l = next(l)) {
    CLine2 *ll = bline(l);
    double naprP = ll->line.pod.q*napr(l);

    if (ll->line.nomP != -1 && naprP > 0) {
      ln = l;
      n_v ++;
    }
  }
  if (n_v != 1) return o;

  return other2(ln);
}


Nagr getNagr(CNode2 *n1, CTreeCtrl & m_tree, HTREEITEM ht, int level)
{
  Nagr nagr;

  if (level > 1000) return nagr;
  
  CString str = n1->getName();
//  str.Format("%s %s", n1->node_name.kod, n1->node_name.name);

  str = "";
  if (/*n1->name_typ_alma || */ n1->node.nZN != -1) 
    str.Format("%s", n1->node.name);

  int n_v = 0;

  for (CLINE2 *l = n1->lines; l; l = next(l)) {
    CLine2 *ll = bline(l);
    double naprP = ll->line.pod.q*napr(l);

    if (ll->line.nomP != -1 && naprP > 0) {
      n_v ++;
    }
  }


  if (n1->node.typ == TIP_PR || n1->node.typ == TIP_PO) {
    str.Format("%s (%g %g %g)", n1->getName(), n1->node.Qot, n1->node.Qgvs, n1->node.Qvent);
  }

  HTREEITEM h = m_tree.InsertItem(str, ht);

  if (n_v > 1) {
//    h = m_tree.InsertItem(str, ht);
  }
  else {
//    h = ht
  }

  if (n1->node.typ == TIP_PR || n1->node.typ == TIP_PO) {
    if (!n1->node.isPjezo) {
        nagr.o = n1->node.Qot;
        nagr.g = n1->node.Qgvs;
        nagr.v = n1->node.Qvent;
        n1->node.isPjezo = true;
    }
  }
  CLINE2 *l;

  for (l = n1->lines; l; l = next(l)) {
    CLine2 *ll = bline(l);
    double naprP = ll->line.pod.q*napr(l);

    if (ll->line.nomP != -1 && naprP > 0) {
      CNode2 *o = other2(l);

      Nagr nagr1 = getNagr(o, m_tree, h, level+1);

      nagr.o += nagr1.o;
      nagr.g += nagr1.g;
      nagr.v += nagr1.v;
    }
  }

  m_tree.Expand(ht, TVE_EXPAND);

  return nagr;
}


CNagrTree::CNagrTree(CWnd* pParent, CNode2 *node)
  : CDialog(CNagrTree::IDD, pParent)
{
  //{{AFX_DATA_INIT(CNagrTree)
  m_gvs = 0.0;
  m_otopl = 0.0;
  m_vent = 0.0;
  m_all = 0.0;
  m_name = "";
  //}}AFX_DATA_INIT
  m_node = node;

  m_profile = "CNagrTree";
}


void CNagrTree::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CNagrTree)
  DDX_Control(pDX, IDC_TREE1, m_tree);
  DDX_Text(pDX, IDC_GVS, m_gvs);
  DDX_Text(pDX, IDC_OTOPL, m_otopl);
  DDX_Text(pDX, IDC_VENT, m_vent);
  DDX_Text(pDX, IDC_NAME, m_name);
  DDX_Text(pDX, IDC_ALL, m_all);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNagrTree, CDialog)
  //{{AFX_MSG_MAP(CNagrTree)
  ON_BN_CLICKED(IDC_TAB, OnTab)
  ON_WM_SIZE()
  ON_WM_DESTROY()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CNagrTree::OnInitDialog() 
{
  CDialog::OnInitDialog();

  CGraph2 *graph = m_node->node.m_graph;
  if (!graph) return FALSE;
      
  graph->save_pjezo();
  graph->reset();

  Nagr nagr = getNagr(m_node, m_tree, 0, 0);

  graph->restore_pjezo();


  m_otopl = nagr.o;
  m_gvs = nagr.g;
  m_vent = nagr.v;

  m_all = nagr.o+nagr.g+nagr.v; 

  m_name = m_node->getName();

//  m_name.Format("%s %s", m_node->node_name.kod, m_node->node_name.name);

  
  UpdateData(FALSE);


  int bx = AfxGetApp()->GetProfileInt(m_profile, "bx", 0);
  int by = AfxGetApp()->GetProfileInt(m_profile, "by", 0);
  int cx = AfxGetApp()->GetProfileInt(m_profile, "cx", 0);
  int cy = AfxGetApp()->GetProfileInt(m_profile, "cy", 0);

  if (cx > 0 && cy > 0) {
    SetWindowPos(&wndTopMost,bx,by,cx,cy,SWP_NOZORDER);
  }


  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CNagrTree::OnTab() 
{
  EndDialog(2005);
}




void CNagrTree::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    CRect sz;
    GetClientRect(sz);

    sz.DeflateRect(10, 10);
    sz.top += 110*getScreenMas();;

    if (!m_tree.m_hWnd) return;

    m_tree.MoveWindow(sz);

//    m_grid.MoveWindow( 0, 0, cx, cy );
}



void CNagrTree::OnDestroy()
{
    WINDOWPLACEMENT wpi;
    GetWindowPlacement(&wpi); 

    AfxGetApp()->WriteProfileInt(m_profile, "bx", wpi.rcNormalPosition.left);
    AfxGetApp()->WriteProfileInt(m_profile, "by", wpi.rcNormalPosition.top);
    AfxGetApp()->WriteProfileInt(m_profile, "cx", wpi.rcNormalPosition.right-wpi.rcNormalPosition.left);
    AfxGetApp()->WriteProfileInt(m_profile, "cy", wpi.rcNormalPosition.bottom-wpi.rcNormalPosition.top);

    CDialog::OnDestroy();
}

