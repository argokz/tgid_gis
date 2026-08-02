// TimeDial.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "TimeDial.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimeDial dialog


CTimeDial::CTimeDial(CWnd* pParent /*=NULL*/)
	: CDialog(CTimeDial::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTimeDial)
	m_date = 0;
	m_time = 0;
	//}}AFX_DATA_INIT

  m_date = CTime::GetCurrentTime();
  m_time = CTime::GetCurrentTime();
  
}


void CTimeDial::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimeDial)
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_date);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_time);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTimeDial, CDialog)
	//{{AFX_MSG_MAP(CTimeDial)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimeDial message handlers

void CTimeDial::OnOK() 
{
  UpdateData(TRUE);

  CTime time (m_date.GetYear(), m_date.GetMonth(), m_date.GetDay(), m_time.GetHour(), m_time.GetMinute(), m_time.GetSecond());
  
  m_time_data = m_time = m_date = time;
	
	CDialog::OnOK();
}
