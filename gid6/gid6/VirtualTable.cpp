// PtsRemonts.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "gid6doc.h"
#include "gidrview.h"

#include "MainFrm.h"

#include "gidrview.h"
#include "win.h"

#include "VirtualTable.h"

#include "InputDia.h"

#include "wm_user.h"


//#include "PtsSheetBottom.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVirtualTable dialog


CVirtualTable::CVirtualTable()
  : CPaneDialog()
{
  //{{AFX_DATA_INIT(CVirtualTable)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT

  m_data = NULL;
}

void CVirtualTable::DoDataExchange(CDataExchange* pDX)
{
  CPaneDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CVirtualTable)
  DDX_Control(pDX, IDC_LIST1, m_grid);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CVirtualTable, CPaneDialog)
  //{{AFX_MSG_MAP(CVirtualTable)
  ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
  ON_MESSAGE(WM_LDBL, OnDblClick)
  ON_WM_TIMER()
  ON_WM_SIZE()
  ON_WM_DESTROY()
  ON_WM_CONTEXTMENU()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CVirtualTable::Init0(CVirtListData *data) 
{
  if (m_data) delete m_data;

  m_data = data;

  m_grid.setVirtListData(data);
  m_grid.init(-1);

  Invalidate();
}

void CVirtualTable::Refresh()
{ 
}

/////////////////////////////////////////////////////////////////////////////
// CVirtualTable message handlers

LRESULT CVirtualTable::HandleInitDialog(WPARAM, LPARAM)
{
  UpdateData(FALSE);

  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CVirtualTable::StartRefresh()
{
  if (m_is_open) m_nTimer = SetTimer(1, 1000, 0);
}

void CVirtualTable::OnTimer(UINT nIDEvent) 
{
  // TODO: Add your message handler code here and/or call default
  
  if (nIDEvent == m_nTimer)
  {
    KillTimer(m_nTimer);
    m_nTimer = NULL;

    Refresh();
  }

  CPaneDialog::OnTimer(nIDEvent);
}



void CVirtualTable::OnSize(UINT nType, int cx, int cy) 
{
  CPaneDialog::OnSize(nType, cx, cy);
  
  if (!m_grid.m_hWnd) return;

  CRect sz;

  GetClientRect(sz);
  sz.DeflateRect(1,1);

  m_grid.MoveWindow(sz);
}

void CVirtualTable::OnDestroy() 
{
  CPaneDialog::OnDestroy();
}

LRESULT CVirtualTable::OnDblClick(WPARAM wParam, LPARAM lParam)
{
  long row = (long)wParam;

  if (m_data) {

    VIRT_DATA typ = m_data->getType();
    int id = (int) m_data->getData(row);
    CString tn = m_data->getTable(row);

    CGidrView *pView = getView();
    if (pView) {
      switch (typ) {
        case virt_data_node:
          pView->SendMessage(WM_USER_FIND_NODE, (WPARAM)id);
          break;
        case virt_data_line:
          pView->SendMessage(WM_USER_FIND_LINE, (WPARAM)id);
          break;
        case virt_data_geo:
          pView->SendMessage(WM_USER_FIND_GEO, (WPARAM)id, (LPARAM) (LPCSTR) tn);
          break;
      }
    }
  }
  return 0;
}
