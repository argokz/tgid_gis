// Okno.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "Okno.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COkno

COkno::COkno()
{
}

COkno::~COkno()
{
}

BEGIN_MESSAGE_MAP(COkno, CWnd)
  //{{AFX_MSG_MAP(COkno)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_CLOSE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COkno message handlers

void COkno::SetText(const char *txt)
{

  m_edt.SetWindowText(txt);
}

int COkno::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CWnd::OnCreate(lpCreateStruct) == -1)
    return -1;

  m_font.CreatePointFont(90, _T("Arial"));
   
  m_edt.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP, CRect(), this, 1);
  m_edt.SetFont(&m_font);
    
  return 0;
}

void COkno::OnSize(UINT nType, int cx, int cy) 
{
  CWnd::OnSize(nType, cx, cy);
  
  if (::IsWindow(m_edt.m_hWnd) )
    m_edt.MoveWindow(0,0,cx,cy,FALSE); // do not repaint
}


BOOL COkno::Create(const RECT& rect, CWnd* pParentWnd, UINT nID)
{
  return CWnd::Create(NULL, "Okno", 
    WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU, 
    rect, pParentWnd, nID);
}

void COkno::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
  ShowWindow( SW_HIDE );

	
//	CWnd::OnClose();
}
