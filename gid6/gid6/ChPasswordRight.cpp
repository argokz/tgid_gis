// ChPasswordRight.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "gid6doc.h"

#include "ChPasswordRight.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChPasswordRight dialog


CChPasswordRight::CChPasswordRight(CWnd* pParent, const char *user, const CUser *pwd)
  : CDialog(CChPasswordRight::IDD, pParent)
{
  //{{AFX_DATA_INIT(CChPasswordRight)
  m_user = user;
  //}}AFX_DATA_INIT

  rightDecode(pwd->user_right, m_r);
}


void CChPasswordRight::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CChPasswordRight)
  DDX_Control(pDX, IDC_FILTR, m_list);
  DDX_Text(pDX, IDC_USER, m_user);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChPasswordRight, CDialog)
  //{{AFX_MSG_MAP(CChPasswordRight)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChPasswordRight message handlers

BOOL CChPasswordRight::OnInitDialog() 
{
  CDialog::OnInitDialog();
  
  m_list.SetCheckStyle(BS_AUTOCHECKBOX);

  int n = sizeof(lst_right)/sizeof(lst_right[0]);

  for ( int i = 0; i < n; i++) {
    m_list.AddString(lst_right[i].r);
    m_list.SetCheck(i, m_r[lst_right[i].ID]);
  }
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CChPasswordRight::OnOK() 
{
  UpdateData(TRUE);

  int n = sizeof(lst_right)/sizeof(lst_right[0]);

  for ( int i = 0; i < n; i++) {
    m_r[lst_right[i].ID] = (unsigned char) m_list.GetCheck(i);
  }
  CDialog::OnOK();
}
