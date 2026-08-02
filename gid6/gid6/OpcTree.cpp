// OpcTree.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "OpcTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COpcTree dialog


COpcTree::COpcTree(CWnd* pParent, OPCServer *server)
  : CDialog(COpcTree::IDD, pParent)
{
  //{{AFX_DATA_INIT(COpcTree)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT

  m_server = server;
}


void COpcTree::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(COpcTree)
  DDX_Control(pDX, IDC_TREE, m_tree);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpcTree, CDialog)
  //{{AFX_MSG_MAP(COpcTree)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpcTree message handlers

BOOL COpcTree::OnInitDialog() 
{
  CDialog::OnInitDialog();

  m_server->Menu2(&m_tree);
  
  // TODO: Add extra initialization here
  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void COpcTree::OnOK() 
{
  // TODO: Add extra validation here

  CString s;

  HTREEITEM h = m_tree.GetSelectedItem();

  str = "";

  while (h != NULL) {
    s = m_tree.GetItemText(h);
    if (str == "") str = s;
    else str = s + "." + str;
    h = m_tree.GetParentItem(h);
  }
 
  CDialog::OnOK();
}
