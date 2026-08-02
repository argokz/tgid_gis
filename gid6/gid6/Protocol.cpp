// Protocol.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "Protocol.h"
#include "win.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProtocol dialog


CProtocol::CProtocol()
  : CPaneDialog()
{
  //{{AFX_DATA_INIT(CProtocol)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT

  file_name = "";
}


void CProtocol::DoDataExchange(CDataExchange* pDX)
{
  CPaneDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CProtocol)
  DDX_Control(pDX, IDC_LIST, m_ctrl);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProtocol, CPaneDialog)
  //{{AFX_MSG_MAP(CProtocol)
  ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
//  ON_WM_CTLCOLOR()
  ON_WM_CONTEXTMENU()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProtocol message handlers

LRESULT CProtocol::HandleInitDialog(WPARAM, LPARAM)
{
  UpdateData(FALSE);
  
  // TODO: Add extra initialization here

//  m_font.CreatePointFont(80, _T("Courier"));
  m_font.CreatePointFont(80, _T("Fixedsys"));
  m_ctrl.SetFont(&m_font);

  char s[256];

  FILE *f;

  f = fopen(file_name, "r");

  if (f) {
    while (!feof(f)) {
      fgetstr(s, 255, f);
//      OemToAnsi(s, s);
      trim(s);
//      m_ctrl.AddString(s);
      int len = m_ctrl.GetWindowTextLength();
      m_ctrl.SetSel(len,len);
      m_ctrl.ReplaceSel(CString(s)+"\r\n");
    }
        
    fclose(f);
  }

  UpdateData(FALSE);
  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CProtocol::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
extern CBrush m_Brush;
extern CBrush m_Brush1;
//  pDC->SetBkMode(TRANSPARENT);
  return m_Brush;
}
