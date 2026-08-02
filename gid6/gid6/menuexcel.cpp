// MenyExcel.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"

#include "cxema.h"

#include "MenuExcel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMenuExcel dialog


CMenuExcel::CMenuExcel(CWnd* pParent, const char *capt, CCxema *cxema)
  : CMMenu(pParent, IDD_MENU_EXCEL, capt)
{
  //{{AFX_DATA_INIT(CMenuExcel)
  m_out = -1;
  //}}AFX_DATA_INIT

  m_out = 0;

  m_cxema = cxema;

}


void CMenuExcel::DoDataExchange(CDataExchange* pDX)
{
  CMMenu::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CMenuExcel)
  DDX_Radio(pDX, IDC_OUT1, m_out);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMenuExcel, CMMenu)
  //{{AFX_MSG_MAP(CMenuExcel)
  ON_LBN_DBLCLK(IDC_LIST, OnDblclkList)
  ON_BN_CLICKED(IDC_OUT, OnOut)
  ON_BN_CLICKED(IDC_OUT1, OnOut)
  ON_BN_CLICKED(IDC_OUT2, OnOut)
  ON_BN_CLICKED(IDC_OUT3, OnOut)
  ON_BN_CLICKED(IDC_OUT4, OnOut)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMenuExcel message handlers


void AddString(CListBox  *m_List, const char *str, const char *str2, CCxema *cxema)
{
  CString s;
  s.Format("%s.lst", str2);

  if (cxema->isExcel2List("Excel2", s)) {
    m_List->AddString(str);
  }
}

void CMenuExcel::OnOut() 
{

  m_List.ResetContent();

  UpdateData(TRUE);

  for ( int i = 0; i < lst.GetSize(); i++ ) {
    CString str = lst[i];
    if (str.Find("OUT_") == 0) {
      if (m_out == 0) ::AddString(&m_List, str.Mid(4), str, m_cxema);
    }
    else if (str.Find("CMP_") == 0) {
      if (m_out == 1) ::AddString(&m_List, str.Mid(4), str, m_cxema);
    }
    else if (str.Find("DBL_") == 0) {
      if (m_out == 2) ::AddString(&m_List, str.Mid(4), str, m_cxema);
    }
    else if (str.Find("HS_") == 0) {
      if (m_out == 4) ::AddString(&m_List, str.Mid(3), str, m_cxema);
    }
    else {
      if (m_out == 3) ::AddString(&m_List, str, str, m_cxema);
    }
  }
}


BOOL CMenuExcel::OnInitDialog() 
{
  CMMenu::OnInitDialog();
  
  OnOut();
  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}


void CMenuExcel::OnDblclkList()
{
  // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  int n = m_List.GetCurSel();
  m_strText = "";
  if (n >= 0) m_List.GetText(n, m_strText);

  m_List.ResetContent();

  for ( int i = 0; i < lst.GetSize(); i++ ) {
    m_List.AddString(lst[i]);
  }
  m_List.SelectString (-1, m_strText);

  if (m_out == 0) {
    m_strText = "OUT_" + m_strText;
  }
  else if (m_out == 1) {
    m_strText = "CMP_" + m_strText;
  }
  else if (m_out == 2) {
    m_strText = "DBL_" + m_strText;
  }
  else if (m_out == 4) {
    m_strText = "HS_" + m_strText;
  }
  else {
  }

  EndDialog(IDOK);
}

void CMenuExcel::OnOK() 
{
  OnDblclkList();

  CDialog::OnOK();
}
