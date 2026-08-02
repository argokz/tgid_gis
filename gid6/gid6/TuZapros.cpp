// TuZapros.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "TuZapros.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTuZapros dialog


CTuZapros::CTuZapros(CWnd* pParent /*=NULL*/)
  : CDialog(CTuZapros::IDD, pParent)
{
  //{{AFX_DATA_INIT(CTuZapros)
  m_check_a = TRUE;
  m_check_d = TRUE;
  m_check_z = TRUE;
  //}}AFX_DATA_INIT

  c_year = 1;
  c_raj = 1;
  c_ist = 1;


}


void CTuZapros::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CTuZapros)
  DDX_Control(pDX, IDC_IST, m_ctrl_ist);
  DDX_Control(pDX, IDC_RAJ, m_ctrl_raj);
  DDX_Control(pDX, IDC_YEAR, m_ctrl_year);
  DDX_Check(pDX, IDC_CHECK_A, m_check_a);
  DDX_Check(pDX, IDC_CHECK_D, m_check_d);
  DDX_Check(pDX, IDC_CHECK_Z, m_check_z);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTuZapros, CDialog)
  //{{AFX_MSG_MAP(CTuZapros)
  ON_CLBN_CHKCHANGE(IDC_IST, OnChkChangeIst)
  ON_CLBN_CHKCHANGE(IDC_RAJ, OnChkChangeRaj)
  ON_CLBN_CHKCHANGE(IDC_YEAR,OnChkChangeYear)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTuZapros message handlers

BOOL CTuZapros::OnInitDialog() 
{
  CDialog::OnInitDialog();
  
//  m_ctrl_year.SetCheckStyle(BS_AUTOCHECKBOX);

m_ctrl_ist .SetFont(GetFont());
m_ctrl_raj .SetFont(GetFont());
m_ctrl_year.SetFont(GetFont());








  int i;

  m_ctrl_year.AddString(_TR("Все"));
  m_ctrl_year.SetCheck(0, 1);

  for ( i = 0; i < list_year.GetSize(); i++ ) {
    m_ctrl_year.AddString(list_year[i]);
    m_ctrl_year.SetCheck(i+1, 1);
  }

  m_ctrl_raj.AddString(_TR("Все"));
  m_ctrl_raj.SetCheck(0, 1);

  for ( i = 0; i < list_raj.GetSize(); i++ ) {
    m_ctrl_raj.AddString(list_raj[i]);
    m_ctrl_raj.SetCheck(i+1, 1);
  }

  m_ctrl_ist.AddString(_TR("Все"));
  m_ctrl_ist.SetCheck(0, 1);


  for ( i = 0; i < list_ist.GetSize(); i++ ) {
    m_ctrl_ist.AddString(list_ist[i]);
    m_ctrl_ist.SetCheck(i+1, 1);
  }



//  if (m_Index >= 0) {
//    UpdateData(FALSE);
//  }


  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}


void CTuZapros::OnCancel() 
{
  // TODO: Add extra cleanup here
  
  CDialog::OnCancel();
}


void CTuZapros::AddYear(const char *text, void *v)
{
  list_year.Add(text);
//  if (v) {
//    map_menu[CString(text)] = v;
//  }
}

void CTuZapros::AddRaj(const char *text, void *v)
{
  list_raj.Add(text);
//  if (v) {
//    map_menu[CString(text)] = v;
//  }
}

void CTuZapros::AddIst(const char *text, void *v)
{
  list_ist.Add(text);
//  if (v) {
//    map_menu[CString(text)] = v;
//  }
}



void * CTuZapros::getV(const char *text)
{
  map<CString, void*>::const_iterator it;

  it = map_menu.find(CString(text));

  if (it != map_menu.end()) {
    return it->second;
  }


  return NULL;
}


void * CTuZapros::getV()
{
//  return getV(m_strText);

  return 0;
}



void CTuZapros::OnChkChangeIst()
{
  int c = m_ctrl_ist.GetCheck(0);

  if (c != c_ist) {
    c_ist = c;
    for ( int i = 0; i < list_ist.GetSize(); i++ ) {
      m_ctrl_ist.SetCheck(i+1, c);
    }
  }
}

void CTuZapros::OnChkChangeRaj()
{
  int c = m_ctrl_raj.GetCheck(0);
  if (c != c_raj) {
    c_raj = c;
    for ( int i = 0; i < list_raj.GetSize(); i++ ) {
      m_ctrl_raj.SetCheck(i+1, c);
    }
  }
}

void CTuZapros::OnChkChangeYear()
{
  int c = m_ctrl_year.GetCheck(0);
  if (c != c_year) {
    c_year = c;
    for ( int i = 0; i < list_year.GetSize(); i++ ) {
      m_ctrl_year.SetCheck(i+1, c);
    }
  }
}

void CTuZapros::OnOK() 
{
  int i, f;
  UpdateData(TRUE);

  m_q = "";

  CString q, q1 = "", q2 = "", q3 = "", q4 = "", q5 = "", q6 = "";

  for ( i = 0, f = 1; i < list_ist.GetSize(); i++ ) {
    if (m_ctrl_ist.GetCheck(i+1)) {

      if (!f) {
        q1 += " OR ";
      }

      if (list_ist[i] == _TR("Не заданный")) {
        q.Format("%s IS NULL OR %s=''", transl("Источник"), transl("Источник"));
      }
      else {
        q.Format("%s = '%s'", transl("Источник"), list_ist[i]);
      }
      q1 += q;
      f = 0;
    }
  }

  for ( i = 0, f = 1; i < list_raj.GetSize(); i++ ) {
    if (m_ctrl_raj.GetCheck(i+1)) {
      if (!f) {
        q2 += " OR ";
      }

      if (list_raj[i] == _TR("Не заданный")) {
        q.Format("%s IS NULL OR %s=''", transl("Район эксплуатации"), transl("Район эксплуатации"));
      }
      else {
        q.Format("%s = '%s'", transl("Район эксплуатации"), list_raj[i]);
      }
      q2 += q;

      f = 0;
    }
  }

  for ( i = 0, f = 1; i < list_year.GetSize(); i++ ) {
    if (m_ctrl_year.GetCheck(i+1)) {

      if (!f) {
        q3 += " OR ";
      }

      if (list_year[i] == _TR("Не заданный")) {
        q.Format("%s IS NULL", transl("Дата выдачи ТУ"));
      }
      else {
        q.Format("YEAR(%s) = %s", transl("Дата выдачи ТУ"), list_year[i]);
      }
      q3 += q;

      f = 0;
    }
  }

  CString s;
  CString qq = "";

  if (m_check_a) {
    if (qq != "") qq += " OR ";
//    qq += "[Состояние договора]='А'";
    s.Format("%s IN (2,3)", transl("Состояние договора"));
    qq += s;
  }
  if (m_check_d) {
    if (qq != "") qq += " OR ";
//    qq += "[Состояние договора]='Д'";
    s.Format("%s=1", transl("Состояние договора"));
    qq += s;
  }
  if (m_check_z) {
    if (qq != "") qq += " OR ";
//    qq += "[Состояние договора]='З'";
    s.Format("%s=4", transl("Состояние договора"));
    qq += s;
  }

  if (q1 == "" || q2 == "" || q3 == "" || qq == "") {
    m_q = "FALSE";
  }
  else {
    m_q.Format("(%s) AND (%s) AND (%s) AND (%s)",  q1, q2, q3, qq);
  }
  
  CDialog::OnOK();
}
