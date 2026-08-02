// KlMenu.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "Klassif.h"
#include "KlMenu.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKlMenu dialog

//static CDaoDatabase *db_list[256];

CKlMenu::CKlMenu(CWnd* pParent, CGeoFile *gf, const char *title, int loc)
  : CDialog(CKlMenu::IDD, pParent)
{
  //{{AFX_DATA_INIT(CKlMenu)
  m_klIndex = -1;
  //}}AFX_DATA_INIT


//    db_list[i] = gf->m_db->db;

  m_kl_list = gf->m_kl_list;


  KlList::iterator it = m_kl_list->begin();

  m_multy = 0;

/*
  for (int sl = 0; it != m_kl_list->end(); ++it, sl++ ) {
    Klassif *kl = &it->second;
    if (kl->m_db && (loc == -1 || kl->loc == loc)) {
      db_list[sl] = kl->m_db->db;
      if (db_list[sl] != db_list[0]) m_multy = 1;
    }
    else db_list[sl] = NULL;
  }
*/

  m_strCaption = "";
  if ( title ) {
    m_strCaption = title;
  }

}


void CKlMenu::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CKlMenu)
  DDX_Control(pDX, 1000, m_klList);
  DDX_LBIndex(pDX, 1000, m_klIndex);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKlMenu, CDialog)
  //{{AFX_MSG_MAP(CKlMenu)
  ON_LBN_DBLCLK(1000, OnDblclk1000)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKlMenu message handlers

void CKlMenu::OnDblclk1000()
{
  // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  EndDialog(IDOK);
  OnOK();
}



BOOL CKlMenu::OnInitDialog()
{
  CDialog::OnInitDialog();

  // TODO: Add extra initialization here

  CenterWindow();

  KlList::const_iterator it = m_kl_list->begin();
//  CDaoTableDefInfo tabInfo;

 
  for (int sl = 0; it != m_kl_list->end(); ++it, sl++ ) {
    const Klassif *kl = &it->second;
/**
    try {
      if (db_list[sl]) db_list[sl]->GetTableDefInfo(kl->nazv,tabInfo);
    }
    catch (CDaoException *e) {
      e->Delete();
      continue;
    }
*/
/*
    if (kl->ins) {
      if (m_multy) {
        CString str;
        if (kl->m_db) {
          str.Format("%s %s", kl->m_db->m_file_name, kl->nazv_alias);
        }
        else {
          if (kl->nazv_alias != "") {
            str.Format("%s", kl->nazv_alias);
          }
          else {
            str.Format("%s", kl->nazv);
          }
        }
//        str.Format("%x %s", it->geo4.m_db, it->nazv_alias.c_str());
        m_klList.AddString(str);
      }
      else {
//        m_klList.AddString(it->nazv.c_str());
//        m_klList.AddString(it->nazv_alias != "" ? it->nazv_alias.c_str() : it->nazv.c_str());
        
        m_klList.AddString(kl->TableName());
      }
    }
*/
  }
  m_klList.SetCurSel(0);

  SetWindowText(m_strCaption);


  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CKlMenu::OnOK() 
{
  // TODO: Add extra validation here

  UpdateData(TRUE);

  KlList::iterator it = m_kl_list->begin();
//  CDaoTableDefInfo tabInfo;
  int i, sl;

  for ( i = 0, sl = 0; it != m_kl_list->end(); ++it, sl++ ) {
    Klassif *kl = &it->second;
/**
    try {
      if (db_list[sl]) db_list[sl]->GetTableDefInfo(kl->nazv,tabInfo);
    }
    catch (CDaoException *e) {
      e->Delete();
      continue;
    }
*/
    if (kl->ins) {
      if (i == m_klIndex) {
        m_kls = kl;
        break;
      }
      i++;
    }
  }

  
  CDialog::OnOK();
}
