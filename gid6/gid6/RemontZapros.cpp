// RemontZapros.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "RemontZapros.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRemontZapros dialog


CRemontZapros::CRemontZapros(CWnd* pParent /*=NULL*/)
  : CDialog(CRemontZapros::IDD, pParent)
{
  //{{AFX_DATA_INIT(CRemontZapros)
  m_date1 = 0;
  m_date2 = 0;
  m_check = FALSE;
  //}}AFX_DATA_INIT
}


void CRemontZapros::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CRemontZapros)
  DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_date1);
  DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_date2);
  DDX_Check(pDX, IDC_CHECK1, m_check);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRemontZapros, CDialog)
  //{{AFX_MSG_MAP(CRemontZapros)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRemontZapros message handlers

BOOL CRemontZapros::OnInitDialog() 
{
  CDialog::OnInitDialog();
  
  // TODO: Add extra initialization here


  m_date1 = CTime::GetCurrentTime();

  //int y = m_date1.GetYear();
//  int m = m_date1.GetMonth();
//  int d = m_date1.GetDay();

//  m_date1.set  .SetDate(y-5, m, d);

  CTimeSpan ts(365*5, 0, 0, 0);

  m_date1 -= ts;

  m_date2 = CTime::GetCurrentTime();

  UpdateData(FALSE);

  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}
