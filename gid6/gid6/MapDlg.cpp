// MapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "MapDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapDlg dialog


CMapDlg::CMapDlg()
  : CPaneDialog()
{
  //{{AFX_DATA_INIT(CMapDlg)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void CMapDlg::DoDataExchange(CDataExchange* pDX)
{
  CPaneDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CMapDlg)
  DDX_Control(pDX, IDC_MAP, m_map);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMapDlg, CPaneDialog)
  //{{AFX_MSG_MAP(CMapDlg)
  ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
  ON_WM_SIZE()
  ON_WM_CONTEXTMENU()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapDlg message handlers

double getScreenMas();


void CMapDlg::OnSize(UINT nType, int cx, int cy) 
{
  CPaneDialog::OnSize(nType, cx, cy);
  
  if (::IsWindow(m_map.m_hWnd) ) {
      double dm = getScreenMas();
      m_map.MoveWindow(0+10*dm, 0+10*dm, cx-20*dm, cy-20*dm, FALSE); // do not repaint
  }
}



void CMapDlg::moveRect()
{
  CPoint pt;
  GetCursorPos(&pt);

  m_map.ScreenToClient(&pt);
  m_map.moveRect(pt);
}

void CMapDlg::curRect()
{
  m_map.curRect();
}


LRESULT CMapDlg::HandleInitDialog(WPARAM, LPARAM)
{
  UpdateData(FALSE);

  ShowWindow(SW_SHOW);
  
  return TRUE;  // return TRUE  unless you set the focus to a control
}
