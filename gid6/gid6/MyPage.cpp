// Address.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
//#include "TestSheet.h"
#include "mypage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMyPage dialog


CMyPage::CMyPage()
  : CPropertyPage()
{
  //{{AFX_DATA_INIT(CMyPage)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


CMyPage::CMyPage(UINT nIDTemplate, UINT nIDCaption)
  : CPropertyPage(nIDTemplate, nIDCaption)
{
  //{{AFX_DATA_INIT(CMyPage)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void CMyPage::DoDataExchange(CDataExchange* pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CMyPage)
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMyPage, CPropertyPage)
  //{{AFX_MSG_MAP(CMyPage)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyPage message handlers


BOOL CMyPage::OnSetActive()
{
#ifdef COOLDIAL
  ((CLeftSheet *) GetParent())->setActive(this);
#endif
  return CPropertyPage::OnSetActive();
}

