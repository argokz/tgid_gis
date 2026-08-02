// MMenu.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "GidrView.h"
#include "GeoMenu.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGeoMenu dialog


//UINT nIDTemplate
//CGeoMenu::CGeoMenu(CWnd* pParent, int isEdit)
//  : CDialog(isEdit ? CGeoMenu::IDD2 : CGeoMenu::IDD, pParent)

  
CGeoMenu::CGeoMenu(CView* pParent, UINT nIDTemplate, const char *capt)
  : CDialog(nIDTemplate, pParent)
//  : CDialog(isEdit ? CGeoMenu::IDD2 : CGeoMenu::IDD, pParent)
{
  //{{AFX_DATA_INIT(CGeoMenu)
  m_Index = -1;
  //}}AFX_DATA_INIT

  m_num = -1;

  m_view = pParent;
  m_old_dc = NULL;

  m_strCaption = "";
  if ( capt ) {
    m_strCaption = capt;
  }
}


void CGeoMenu::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CGeoMenu)
  DDX_Control(pDX, IDC_TU, m_ctrlTu);
  DDX_Control(pDX, IDC_NADPIS, m_ctrlNadpis);
  DDX_Control(pDX, IDC_ISPR_PODP, m_ctrlIsprPodp);
  DDX_Control(pDX, IDC_ISPR, m_ctrlIspr);
  DDX_Control(pDX, IDC_DEL, m_ctrlDel);
  DDX_Check(pDX, IDC_NADPIS, m_nadpis);
  DDX_Control(pDX, IDC_LIST, m_List);
  DDX_LBIndex(pDX, IDC_LIST, m_Index);
  //}}AFX_DATA_MAP
}

BOOL CGeoMenu::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  UINT nID = LOWORD(wParam);

  UpdateData(TRUE);

  switch ( nID ) {
  case IDC_DEL :
  case IDC_ISPR :
  case IDC_ISPR_PODP :
  case IDC_TU :
    close();
    EndDialog(nID);
    return 0;
  }
  return CDialog::OnCommand(wParam, lParam);
}

BEGIN_MESSAGE_MAP(CGeoMenu, CDialog)
  //{{AFX_MSG_MAP(CGeoMenu)
  ON_BN_CLICKED(IDC_NADPIS, OnNadpis)
  ON_LBN_SELCHANGE(IDC_LIST, OnSelChange)
  ON_WM_CLOSE()
//  ON_WM_CTLCOLOR()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeoMenu message handlers


BOOL CGeoMenu::OnInitDialog()
{
  CDialog::OnInitDialog();

  // TODO: Add extra initialization here

  int i;

  for ( i = 0; i < list_txt.GetSize(); i++ ) {
    m_List.AddString(list_txt[i]);
  }


  for ( i = 0; i < list.GetSize(); i++ ) {
    m_List.AddString(list[i]->RusName());
  }


//  SetWindowText(m_strCaption);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CGeoMenu::OnOK() 
{
  UpdateData(TRUE);
  close();
  CDialog::OnOK();
}


HBRUSH CGeoMenu::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
extern CBrush m_Brush;
extern CBrush m_Brush1;
  pDC->SetBkMode(TRANSPARENT);
  return m_Brush;
}

void CGeoMenu::Add(const Klassif *kl, int num)
{
  list.Add(kl);
  list_num.Add(num);
}

void CGeoMenu::Add(const char *txt, int num)
{
  list_txt.Add(txt);
//  list_num.Add(num);
}


extern CGeoFile *m_geofile;


void CGeoMenu::OnSelChange()
{
  UpdateData(TRUE);

  CGidrView *view = (CGidrView *) m_view;
  int n = m_List.GetCurSel();
  m_geofile->no_view_object(view, m_old_dc);
  m_old_dc = NULL;

  m_nadpis = FALSE;

  if (n >= 0) {
    m_old_dc = m_geofile->view_object(view, list[n]->nazv, list_num[n]);
    m_strText = list[n]->nazv;
    m_num = list_num[n];
    m_nadpis = list[n]->label;

    m_ctrlTu.EnableWindow(list[n]->loc == LOC3);
    m_ctrlNadpis.EnableWindow();
    m_ctrlIsprPodp.EnableWindow(list[n]->label);
    m_ctrlIspr.EnableWindow();
    m_ctrlDel.EnableWindow();
  }

  UpdateData(FALSE);
}

void CGeoMenu::close()
{
  CGidrView *view = (CGidrView *) m_view;
  m_geofile->no_view_object(view, m_old_dc);
  m_old_dc = NULL;
} 

void CGeoMenu::OnClose() 
{
  CGidrView *view = (CGidrView *) m_view;
  m_geofile->no_view_object(view, m_old_dc);
  m_old_dc = NULL;

  CDialog::OnClose();
}

void CGeoMenu::OnNadpis() 
{
  UpdateData(TRUE);
  if (m_num >= 0) {
    close();
    CGidrView *view = (CGidrView *) m_view;
    view->GeoNadpis(m_strText, m_num, m_nadpis);
  }
}
