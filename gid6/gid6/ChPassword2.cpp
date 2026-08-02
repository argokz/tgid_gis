// ChPassword2.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "ChPassword2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int md5hash(const char *msg, size_t len, char *hash) ;


/////////////////////////////////////////////////////////////////////////////
// CChPassword2 dialog


CChPassword2::CChPassword2(CWnd* pParent, const char *user, const CUser *pwd)
  : CDialog(CChPassword2::IDD, pParent)
{
  //{{AFX_DATA_INIT(CChPassword2)
  m_password = "";
  m_password2 = "";
  m_oldpassword = "";
  //}}AFX_DATA_INIT

//  char hash[265];
//  md5hash(pwd->password, pwd->password.GetLength(), hash);

  m_oldpassword_pr = pwd->password;
}


void CChPassword2::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CChPassword2)
  DDX_Text(pDX, IDC_OLDPASSWORD, m_oldpassword);
  DDX_Text(pDX, IDC_PASSWORD, m_password);
  DDX_Text(pDX, IDC_PASSWORD2, m_password2);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChPassword2, CDialog)
  //{{AFX_MSG_MAP(CChPassword2)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChPassword2 message handlers

void CChPassword2::OnOK() 
{
  UpdateData(TRUE);

  char hash[265];
  md5hash(m_oldpassword, m_oldpassword.GetLength(), hash);

  if (CString(hash) != m_oldpassword_pr && m_oldpassword != "" && m_oldpassword_pr != "") {
    AfxMessageBox(_TR("Неправильный пароль"), MB_OK|MB_ICONINFORMATION);
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

  if (m_password != m_oldpassword_pr) {
    
    if (m_password != "") {
      AfxMessageBox(_TR("Пароль изменен"), MB_OK|MB_ICONINFORMATION);
    }
    else {
      AfxMessageBox(_TR("Пароль отменен"), MB_OK|MB_ICONINFORMATION);
    }
  }
    
  CDialog::OnOK();
}
