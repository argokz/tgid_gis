// RemontZapros.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "RemontZapros2.h"
#include "ado.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRemontZapros2 dialog


CRemontZapros2::CRemontZapros2(CWnd* pParent, const char *capt)
  : CDialog(CRemontZapros2::IDD, pParent)
{
  //{{AFX_DATA_INIT(CRemontZapros2)
  m_date1 = 0;
  m_date2 = 0;
  m_check = FALSE;
  m_type1 = 0;
  //}}AFX_DATA_INIT

  m_tab = "";

  m_capt = "";
  if ( capt ) {
      m_capt = capt;
  }
}

void CRemontZapros2::setTab(CAdoFile *ado, const char *tab)
{
    m_ado = ado;
    m_tab = tab;
}



void CRemontZapros2::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CRemontZapros2)
  DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_date1);
  DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_date2);
  DDX_Check(pDX, IDC_CHECK1, m_check);
  DDX_Control(pDX, IDC_COMBO1, m_combo1);
  DDX_CBIndex(pDX, IDC_COMBO1, m_type1);

  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRemontZapros2, CDialog)
  //{{AFX_MSG_MAP(CRemontZapros2)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRemontZapros2 message handlers

BOOL CRemontZapros2::OnInitDialog() 
{
  CDialog::OnInitDialog();
  

  if (m_capt != "") {
      SetWindowText(m_capt);
  }
  m_combo1.AddString("Все");


  if (m_tab != "") {
      CString q;
      q.Format("SELECT * FROM %s", m_tab);

      if (m_ado->openTable0(q)) {
          while (!m_ado->isEOF()) {
              long id = m_ado->read_long("id");
              CString s = m_ado->readStr("name");
              m_combo1.AddString( s);
              m_ado->MoveNext();
          }
          m_ado->closeTable();
      }
  }



  m_date1 = CTime::GetCurrentTime();

  //int y = m_date1.GetYear();
//  int m = m_date1.GetMonth();
//  int d = m_date1.GetDay();

//  m_date1.set  .SetDate(y-5, m, d);

  CTimeSpan ts(365*5, 0, 0, 0);

  m_date1 -= ts;


  m_date2 = CTime::GetCurrentTime();
  m_date1 = CTime(m_date2.GetYear(), 1, 1, 0, 0, 0);


  UpdateData(FALSE);

  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}
