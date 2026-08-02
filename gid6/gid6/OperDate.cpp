// OperDate.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "OperDate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COperDate dialog


COperDate::COperDate(CWnd* pParent /*=NULL*/)
  : CDialog(COperDate::IDD, pParent)
{
  //{{AFX_DATA_INIT(COperDate)
  m_date = 0;
  //}}AFX_DATA_INIT

  m_Index = 0;
}


void COperDate::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(COperDate)
  DDX_Control(pDX, IDC_TABLE, m_table);
  DDX_Control(pDX, IDC_LIST1, m_List);
  DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_date);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COperDate, CDialog)
  //{{AFX_MSG_MAP(COperDate)
  ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COperDate message handlers

BOOL COperDate::OnInitDialog() 
{
  CDialog::OnInitDialog();

  int i;

  for ( i = 0; i < lst.GetSize(); i++ ) {
    m_List.AddString(lst[i]);
  }

  for ( i = 0; i < lst_t.GetSize(); i++ ) {
    m_table.AddString(lst_t[i]);
  }

  m_List.SetCurSel(0);
  m_table.SetCurSel(m_Index);

  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void COperDate::OnOK() 
{
  OnDblclkList();
  
  CDialog::OnOK();
}


void COperDate::OnDblclkList()
{
  // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  m_strText = "";
  int n = m_List.GetCurSel();
  if (n >= 0) m_List.GetText(n, m_strText);

  m_strTextTable = "";
  n = m_table.GetCurSel();
  if (n >= 0) m_table.GetLBText(n, m_strTextTable);

  EndDialog(IDOK);
}



void COperDate::Add(const char *text, void *v)
{
  lst.Add(text);
  if (v) {
      map_menu[CString(text)] = v;
  }
}


void * COperDate::getV(const char *text)
{
  map<CString, void*>::const_iterator it;

  it = map_menu.find(CString(text));

  if (it != map_menu.end()) {
    return it->second;
  }
  return NULL;
}


void * COperDate::getV()
{
  return getV(m_strText);
}
