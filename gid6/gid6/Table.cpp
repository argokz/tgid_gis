#include "stdafx.h"

#include "resource.h"
#include "gid6Doc.h"

#include "table.h"
#include "virtualdata.h"

#include "wm_user.h"


IMPLEMENT_DYNCREATE(CTable, CView)

BEGIN_MESSAGE_MAP(CTable, CView)
  ON_WM_CREATE()
  ON_WM_SIZE()

  ON_COMMAND(ID_HIDE_COLUMN, OnHideColumn)
  ON_COMMAND(ID_MOVE_TO2, OnMoveTo)

  ON_COMMAND(ID_COLUMN_SORT, OnColumnSort)
  ON_COMMAND(ID_EDIT_FIND, OnEditFind)
  ON_COMMAND(ID_EDIT_REPEAT, OnEditRepeat)
  ON_COMMAND(ID_SHIFT_F1, OnShiftF1)

  // Standard printing commands
  ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
  ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
  ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)

END_MESSAGE_MAP()

CTable::CTable()
{
  m_data = NULL;
}

void CTable::OnDraw(CDC* pDC)
{
  CDocument* pDoc = GetDocument();
  //If we are printing then "print" the appropriate page
  //otherwise fill the DC with white space
  
  CRect rect;
  if (m_isPrinting){
    rect = m_drawRect;
    m_grid.PrintPage(pDC, m_printPage);
  }
  else{
    GetClientRect(&rect);
    CBrush brush(RGB(255,255,255));
    pDC->FillRect(&rect,&brush);
  }
}

/////////////////////////////////////////////////////////////////////////////
// CEx21View printing

BOOL CTable::OnPreparePrinting(CPrintInfo* pInfo)
{
  // default preparation
  return DoPreparePrinting(pInfo);
}

void CTable::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
  // TODO: add extra initialization before printing

  pInfo->SetMaxPage(m_grid.PrintInit(pDC,pInfo->m_pPD,0,0,m_grid.GetNumberCols()-1,
    m_grid.GetNumberRows()-1));
  m_isPrinting = TRUE;
}

void CTable::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
  // TODO: add cleanup after printing
  m_isPrinting = FALSE;
}



void CTable::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);

    if (!m_grid.m_hWnd) return;

    CRect sz;

    GetClientRect(sz);
    sz.DeflateRect(1, 1);

    m_grid.MoveWindow(sz);
}


void CTable::OnInitialUpdate() 
{
  CView::OnInitialUpdate();

  Cgid6Doc* pDoc = (Cgid6Doc*) GetDocument();

//  m_grid.CreateGrid(WS_CHILD | WS_BORDER, CRect(0, 0, 0, 0), this, 3551515);

  init(pDoc->m_data);
}


void CTable::init(CVirtListData *data)
{
    m_data = data;

//  m_grid.AttachGrid(this, IDC_LIST1);

//  m_index = AddDataSource(&m_data);
    m_grid.setName(data->getTable(0));
    m_grid.init(data);
}


int CTable::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CView::OnCreate(lpCreateStruct) == -1)
    return -1;
  
  // Create the Grid control in the client rectangle
  RECT  rect;
  GetClientRect(&rect);
  m_grid.CreateGrid(WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
            rect,this,1234);
  
  return 0;
}


void CTable::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
    // TODO: Add your specialized code here and/or call the base class

    m_drawRect = pInfo->m_rectDraw;
    m_printPage = pInfo->m_nCurPage;

    CView::OnPrint(pDC, pInfo);
}


void CTable::OnHideColumn()
{
    m_grid.HideColumn();
}


#include "gidrView.h"

void CTable::OnMoveTo() // перейти в окно схемы
{
    long row = m_grid.GetCurrentRow();

    if (!m_data) return;

    long id;
    CUGCell cell;

    m_grid.m_data.GetCell(0, row, &cell);
    cell.GetNumber(&id);
    
    CString qq;
    qq.Format("%d", id);

//    AfxMessageBox(qq, MB_OK|MB_ICONINFORMATION);

    VIRT_DATA typ = m_data->getType();
//    int id = (int) m_->getData(row);

void setView(CView *pView0);

    CGidrView *pView = getView();
    if (pView) {
        setGidView(pView);
        
        switch (typ) {
            case virt_data_node:
            case virt_data_node_out:
                pView->SendMessage(WM_USER_FIND_NODE, (WPARAM)id);
                break;
            case virt_data_line:
            case virt_data_line_out:
                pView->SendMessage(WM_USER_FIND_LINE, (WPARAM)id);
                break;
            case virt_data_geo:
                {
                    CString tn = m_data->getTable(row);
                    pView->SendMessage(WM_USER_FIND_GEO, (WPARAM)id, (LPARAM) (LPCSTR) tn);
                }
                break;
        }
    }

}

void CTable::OnColumnSort()
{
    int col = m_grid.GetCurrentCol();
    m_grid.Sort(col);
}

void CTable::OnEditFind()
{
    m_grid.Find();
}

void CTable::OnEditRepeat()
{
}

void CTable::OnShiftF1()
{
    if (!m_data) return;

    int col = m_grid.GetCurrentCol();
    long row = m_grid.GetCurrentRow();

    CString s = m_data->GetFieldF1(col);

//    s.Format("Help %d %d", row, col);
    AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);
}