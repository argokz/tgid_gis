// LoginDialog.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "LoginDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoginDialog dialog

map <CString, CUser> list_user;

CLoginDialog::CLoginDialog(CWnd* pParent /*=NULL*/)
  : CDialog(CLoginDialog::IDD, pParent)
{
  //{{AFX_DATA_INIT(CLoginDialog)
  m_password = "";
  m_user = "";
//  m_nazv = "";
  //}}AFX_DATA_INIT

  m_brush = 0;
  m_font.CreatePointFont(120, _T("Arial"));
}


void CLoginDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CLoginDialog)
  DDX_Control(pDX, IDC_NAZV, m_ctrl_nazv);
  DDX_Control(pDX, IDC_USER, m_ctl_user);
  DDX_Text(pDX, IDC_PASSWORD, m_password);
  DDX_Text(pDX, IDC_USER, m_user);
//  DDX_Text(pDX, IDC_NAZV, m_nazv);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoginDialog, CDialog)
  //{{AFX_MSG_MAP(CLoginDialog)
  ON_WM_CTLCOLOR()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*

int CLoginDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CDialog::OnCreate(lpCreateStruct) == -1)
    return -1;

  m_font.CreatePointFont(90, _T("Arial"));
   
//  m_edt.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP, CRect(), this, 1);
  m_ctrl_nazv.SetFont(&m_font);
    
  return 0;
}
*/

/////////////////////////////////////////////////////////////////////////////
// CLoginDialog message handlers

BOOL CLoginDialog::OnInitDialog() 
{
  CDialog::OnInitDialog();

  map <CString, CUser>::const_iterator it;
  
  for (it = list_user.begin(); it != list_user.end(); it++) {
    m_ctl_user.AddString(it->first);
  }


  m_ctrl_nazv.SetFont(&m_font);
  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}


HBRUSH CLoginDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
 

  if (pWnd->GetDlgCtrlID() == IDC_NAZV)
    {

     pDC->SetTextColor(0x00007F);   
//      if (m_brush) delete m_brush;
//      m_brush = new CBrush(0xFF0000);
//      hbr = (HBRUSH)(m_brush->GetSafeHandle());
    }
 
  return hbr;
}