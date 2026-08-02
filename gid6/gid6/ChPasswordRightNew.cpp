// ChPasswordRightNew.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "gid6Doc.h"

#include "ChPasswordRightNew.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChPasswordRightNew dialog


CChPasswordRightNew::CChPasswordRightNew(CWnd* pParent, const char *user, const char *password, int right)
  : CDialog(CChPasswordRightNew::IDD, pParent)
{
  //{{AFX_DATA_INIT(CChPasswordRightNew)
  m_password = password;
  m_password2 = password;
  m_user = user;
  //}}AFX_DATA_INIT

  rightDecode(right, m_r);

}


void CChPasswordRightNew::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CChPasswordRightNew)
  DDX_Control(pDX, IDC_FILTR, m_list);
  DDX_Text(pDX, IDC_PASSWORD, m_password);
  DDX_Text(pDX, IDC_PASSWORD2, m_password2);
  DDX_Text(pDX, IDC_USER, m_user);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChPasswordRightNew, CDialog)
  //{{AFX_MSG_MAP(CChPasswordRightNew)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChPasswordRightNew message handlers

BOOL CChPasswordRightNew::OnInitDialog() 
{
  CDialog::OnInitDialog();
  
  m_list.SetCheckStyle(BS_AUTOCHECKBOX);

  int n = sizeof(lst_right)/sizeof(lst_right[0]);

  for ( int i = 0; i < n; i++) {
    m_list.AddString(lst_right[i].r);
    m_list.SetCheck(i, m_r[i]);
  }
  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CChPasswordRightNew::OnOK() 
{
  UpdateData(TRUE);
  
  int n = sizeof(lst_right)/sizeof(lst_right[0]);
  for ( int i = 0; i < n; i++) {
    m_r[lst_right[i].ID] = (unsigned char) m_list.GetCheck(i);
  }

  if (m_user == "") {
    AfxMessageBox(_TR("Имя пользователя не введено"), MB_OK|MB_ICONINFORMATION);
    return;
  }

  if (m_password == "") {
//    AfxMessageBox(_TR("Пароли не введен"), MB_OK|MB_ICONINFORMATION);
//    return;
  }

  if (m_password != m_password2) {
    AfxMessageBox(_TR("Пароли должны быть одинаковыми"), MB_OK|MB_ICONINFORMATION);
    return;
  }

  CDialog::OnOK();
}
