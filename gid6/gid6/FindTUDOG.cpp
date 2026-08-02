// FindTUDOG.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "FindTUDOG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindTUDOG dialog


CFindTUDOG::CFindTUDOG(CWnd* pParent /*=NULL*/)
  : CDialog(CFindTUDOG::IDD, pParent)
{
  //{{AFX_DATA_INIT(CFindTUDOG)
  m_edit_find_name = "";
  m_edit_find_name_tu = "";
  m_edit_find_nd = "";
  //}}AFX_DATA_INIT
}


void CFindTUDOG::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CFindTUDOG)
  DDX_Text(pDX, IDC_EDIT_FIND_NAME, m_edit_find_name);
  DDX_Text(pDX, IDC_EDIT_FIND_NAME_TU, m_edit_find_name_tu);
  DDX_Text(pDX, IDC_EDIT_FIND_ND, m_edit_find_nd);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindTUDOG, CDialog)
  //{{AFX_MSG_MAP(CFindTUDOG)
  ON_BN_CLICKED(IDC_FIND_ND, OnFindNd)
  ON_BN_CLICKED(IDC_TU, OnTu)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindTUDOG message handlers

BOOL CFindTUDOG::OnInitDialog() 
{
  CDialog::OnInitDialog();
  
  // TODO: Add extra initialization here
  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CFindTUDOG::OnFindNd() 
{
  UpdateData(TRUE);
  EndDialog(1);
}

void CFindTUDOG::OnTu() 
{
  UpdateData(TRUE);
  EndDialog(2);
}
