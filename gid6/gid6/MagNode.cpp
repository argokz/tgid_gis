// MagNode.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "gid6doc.h"
#include "gidrview.h"

#include "MagNode.h"

#include "ps_alma.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMagNode dialog

const CNode2 *m_n1_opr_1;
const CNode2 *m_n1_opr_2;


CMagNode::CMagNode(CGidrView* pParent, int mag)
  : CDialog(CMagNode::IDD, (CWnd*) pParent)
{
  //{{AFX_DATA_INIT(CMagNode)
  m_mag = -1;
  m_node = -1;
  m_node2 = -1;
  //}}AFX_DATA_INIT

  m_n1 = NULL;
  m_n2 = NULL;

  gidView = pParent;

  m_mag_init = mag;
}


void CMagNode::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CMagNode)
  DDX_Control(pDX, IDC_LIST_NODE_2, m_list_node2);
  DDX_Control(pDX, IDC_LIST_NODE, m_list_node);
  DDX_Control(pDX, IDC_LIST_MAG, m_list_mag);
  DDX_LBIndex(pDX, IDC_LIST_MAG, m_mag);
  DDX_LBIndex(pDX, IDC_LIST_NODE, m_node);
  DDX_LBIndex(pDX, IDC_LIST_NODE_2, m_node2);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMagNode, CDialog)
  //{{AFX_MSG_MAP(CMagNode)
  ON_LBN_SELCHANGE(IDC_LIST_MAG, OnSelchangeListMag)
  ON_LBN_SELCHANGE(IDC_LIST_NODE, OnSelchangeListNode)
  ON_LBN_SELCHANGE(IDC_LIST_NODE_2, OnSelchangeListNode2)
  ON_LBN_DBLCLK(IDC_LIST_NODE, OnDblclkListNode)
  ON_LBN_DBLCLK(IDC_LIST_NODE_2, OnDblclkListNode2)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMagNode message handlers


BOOL CMagNode::OnInitDialog() 
{
  CDialog::OnInitDialog();

  map_node.clear();

  
  map<int, UMAG>::const_iterator it1 = ps_alma.mapMAG.begin();

  for (; it1 !=  ps_alma.mapMAG.end(); it1++) {
    UMAG mag0 = it1->second;
    MAG_NODE mag;

    mag.id = mag0.id;
    mag.name = mag0.name;
    mag.opis = mag0.opis;

    mag_mag[mag.opis] = mag;
  }

  map<CString, MAG_NODE, lessMSRS1>::const_iterator it = mag_mag.begin();

  for (int m = 0; it != mag_mag.end(); it++, m++) {
    MAG_NODE mag = it->second;
    m_list_mag.AddString(mag.opis);
    if (mag.id == m_mag_init) {
      m_mag = m;
    }
  }

  UpdateData(FALSE);

  if (m_mag) {
    OnSelchangeListMag();
  }
  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void fillNodeList(CGraph2 *graph, CListBox *m_list_uzel1, map<CString, const CNode2 *> &map_node, list<const CNode2 *> *list_all_node);

int vydMagN(CGraph2 *m_graph, int id);

void CMagNode::OnSelchangeListMag() 
{
  UpdateData(TRUE);

  m_list_node.ResetContent();
  m_list_node2.ResetContent();


  list_all_node.clear();

  if (m_mag >= 0) {
    CString s;
    m_list_mag.GetText(m_mag, s );

    map<CString, MAG_NODE, lessMSRS1>::const_iterator it = mag_mag.find(s);

    if (it != mag_mag.end()) {
      MAG_NODE ms = it->second;

      CGraph2 *graph = gidView->m_cxema->m_graph;

      graph->save_pjezo();
      graph->reset();

      vydMagN(graph, ms.id);
      fillNodeList(graph, &m_list_node, map_node, &list_all_node);
      graph->restore_pjezo();
    }
  }
}

void CMagNode::OnSelchangeListNode() 
{
  // TODO: Add your control notification handler code here
  
}

void CMagNode::OnSelchangeListNode2() 
{
  // TODO: Add your control notification handler code here
  
}

void CMagNode::fillList(list<CNode2*> &list_node)
{
/*
  int n = m_list_node2.GetCount();

  for (int i = 0; i < n; i++) {
    CString s;
    m_list_node2.GetText(i, s );

    map<CString, CNode2 *>::iterator it = map_node2.find(s);
    if (it != map_node2.end()) {
      list_node.push_back(it->second);
    }
  }
*/

  CNode2 *n1 = NULL, *n2 = NULL;

  int n = m_list_node2.GetCount();

  if (n < 2) return;

  CString s;
  m_list_node2.GetText(0, s );
  map<CString, CNode2 *>::iterator it = map_node2.find(s);
  if (it != map_node2.end()) n1 = it->second;

  m_list_node2.GetText(1, s );
  it = map_node2.find(s);
  if (it != map_node2.end()) n2 = it->second;

  if (n1 == NULL || n2 == NULL) return;

  list <const CNode2 *>::const_iterator ita = list_all_node.begin();

  const CNode2 *bn = NULL;

  for (; ita != list_all_node.end(); ita++) {
    const CNode2 *n = *ita;

    if ((n == n1 || n == n2) && bn == NULL) bn = n;

    if (bn) {
      CNode2 *n0 = n->node.m_graph->find(n->id);

      if (n0) {
        list_node.push_back(n0);
      }
      if ((n == n1 || n == n2) && n != bn) break;
    }
  }

/*
  int n = m_list_node2.GetCount();

  for (int i = 0; i < n; i++) {
    CString s;
    m_list_node2.GetText(i, s );

    map<CString, CNode2 *>::iterator it = map_node2.find(s);
    if (it != map_node2.end()) {
      list_node.push_back(it->second);
    }
  }
*/
}



void CMagNode::OnDblclkListNode() 
{
  UpdateData(TRUE);

  if (m_node >= 0) {
    CString s;
    m_list_node.GetText(m_node, s );

    map<CString, const CNode2 *>::iterator it = map_node.find(s);

    if (it != map_node.end()) {
      m_list_node2.AddString(s);

      CNode2 *n = gidView->m_cxema->m_graph->find(it->second->id);

      if (n) {
        map_node2[s] = n;

        list<CNode2*> list_node;
        fillList(list_node);

        gidView->vydP(list_node);
      }
    }
  }
}

void CMagNode::OnOK() 
{
  UpdateData(TRUE);
  int n = m_list_node2.GetCount();

  CString s1, s2;


  if (n >= 1) {
    m_list_node2.GetText(0, s1);

    map<CString, const CNode2 *>::iterator it = map_node.find(s1);
    if (it != map_node.end()) {
      m_n1 = it->second;
    }
  }
  if (n >= 2) {
    m_list_node2.GetText(n-1, s2);

    map<CString, const CNode2 *>::iterator it = map_node.find(s2);
    if (it != map_node.end()) {
      m_n2 = it->second;
    }
  }

  m_n1_opr_1 = m_n1;
  m_n1_opr_2 = m_n2;

  CDialog::OnOK();
}

void CMagNode::OnDblclkListNode2() 
{
  UpdateData(TRUE);

  if (m_node2 >= 0) {
    CString s;
    m_list_node2.GetText(m_node2, s );

    map<CString, CNode2 *>::iterator it = map_node2.find(s);

    if (it != map_node2.end()) {
      map_node2.erase(it->first);
      m_list_node2.DeleteString(m_node2);

      list<CNode2*> list_node;
      fillList(list_node);

      gidView->vydP(list_node);
    }
  }
}
