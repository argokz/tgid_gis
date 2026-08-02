// KlTyp2.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "KlTyp2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKlTyp2 dialog


CKlTyp2::CKlTyp2(CWnd* pParent)
  : CDialog(CKlTyp2::IDD, pParent)
{
  //{{AFX_DATA_INIT(CKlTyp2)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void CKlTyp2::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CKlTyp2)
  DDX_Control(pDX, 101, m_rastr);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKlTyp2, CDialog)
  //{{AFX_MSG_MAP(CKlTyp2)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKlTyp2 message handlers

BOOL CKlTyp2::PreTranslateMessage(MSG* pMsg) 
{
  return CDialog::PreTranslateMessage(pMsg);
}

