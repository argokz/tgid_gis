// ADO SampleView.cpp : implementation of the CTPView class
//

#include "stdafx.h"

#include "TPView.h"
//#include "SqlEditDlg.h"

#include "resource.h"
#include "gid6Doc.h"


#include "gidrview.h"
#include "virtualdata.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTPView

IMPLEMENT_DYNCREATE(CTPView, CView)

BEGIN_MESSAGE_MAP(CTPView, CView)
  //{{AFX_MSG_MAP(CTPView)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_SETFOCUS()
  ON_WM_ERASEBKGND()
  ON_COMMAND(ID_REFRESH_DATA, OnRefreshData)
/**
  ON_COMMAND(ID_SHOW_SQL, OnShowSql)
  ON_COMMAND(ID_APPEND_ROW, OnAppendRow)
  ON_COMMAND(ID_DELETE_RECORD, OnDeleteRecord)
  ON_COMMAND(ID_ADD_TABLE, OnAddTable)
*/
  ON_COMMAND(ID_SHIFT_F1, OnShiftF1)
  //}}AFX_MSG_MAP
  // Standard printing commands
  ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
  ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
  ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTPView construction/destruction

CTPView::CTPView()
{
  // TODO: add construction code here
  m_pDSList = NULL;
}

CTPView::~CTPView()
{
  // clear the DS link list
  _tagDSList *pTempItem = m_pDSList,
        *pDeleteItem = NULL;

  while ( pTempItem != NULL )
  {
    pDeleteItem = pTempItem;
    pTempItem = pTempItem->next;
    delete pDeleteItem->m_dsADO;
    delete pDeleteItem;
  }
}

BOOL CTPView::PreCreateWindow(CREATESTRUCT& cs)
{
  // TODO: Modify the Window class or styles here by modifying
  //  the CREATESTRUCT cs

  return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CTPView drawing

void CTPView::OnDraw(CDC* pDC)
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
// CTPView printing

BOOL CTPView::OnPreparePrinting(CPrintInfo* pInfo)
{
  // default preparation
  return DoPreparePrinting(pInfo);
}

void CTPView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
  pInfo->SetMaxPage(m_grid.PrintInit(pDC,pInfo->m_pPD,0,0,m_grid.GetNumberCols()-1,
    m_grid.GetNumberRows()-1));
  m_isPrinting = TRUE;
}

void CTPView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
  m_isPrinting = FALSE;
}


void CTPView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
    // TODO: Add your specialized code here and/or call the base class

    m_drawRect = pInfo->m_rectDraw;
    m_printPage = pInfo->m_nCurPage;

    CView::OnPrint(pDC, pInfo);
}


/////////////////////////////////////////////////////////////////////////////
// CTPView diagnostics

#ifdef _DEBUG
void CTPView::AssertValid() const
{
  CView::AssertValid();
}

void CTPView::Dump(CDumpContext& dc) const
{
  CView::Dump(dc);
}

/**
CADOSampleDoc* CTPView::GetDocument() // non-debug version is inline
{
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CADOSampleDoc)));
  return (CADOSampleDoc*)m_pDocument;
}
*/
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTPView message handlers

int CTPView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CView::OnCreate(lpCreateStruct) == -1)
    return -1;
  
  // TODO: Add your specialized creation code here
  m_grid.CreateGrid( WS_CHILD|WS_VISIBLE, CRect(0,0,2000,1000), this, 23423 );
  
  return 0;
}

void CTPView::OnSize(UINT nType, int cx, int cy) 
{
  CView::OnSize(nType, cx, cy);
  
  // TODO: Add your message handler code here
  m_grid.MoveWindow( 0, 0, cx, cy );
}

void CTPView::OnSetFocus(CWnd* pOldWnd) 
{
  CView::OnSetFocus(pOldWnd);
  
  // TODO: Add your message handler code here
  m_grid.SetFocus();
}

BOOL CTPView::OnEraseBkgnd(CDC* pDC) 
{
  // TODO: Add your message handler code here and/or call default
  return TRUE;
  // return CView::OnEraseBkgnd(pDC);
}

/////////////////////////////////////////////////////////////////////////////
//  OnInitialUpdate
//    The OnInitialUpdate function is used to bind the grid to ADO datasource
//    using the SQL and connect information specified by the user.
void CTPView::OnInitialUpdate()
{
  CView::OnInitialUpdate();

  Cgid6Doc* pDoc = (Cgid6Doc*) GetDocument();

//  m_grid.init(pDoc->m_view_db);
}

void CTPView::init(const char *db)
{
//  m_grid.init(db);
}



/////////////////////////////////////////////////////////////////////////////
//  OnRefreshData
//    The OnRefreshData function will force the data in current sheet to
//    be reloaded from the database.
void CTPView::OnRefreshData() 
{
  int nSheetNumber = m_grid.GetSheetNumber();
  _tagDSList *pTempItem = m_pDSList;
  
  for( int nIndex = 0; nIndex < nSheetNumber && pTempItem != NULL; nIndex ++ )
    pTempItem = pTempItem->next;

  if ( pTempItem == NULL )
    return;

  pTempItem->m_dsADO->RefreshData();
  m_grid.RedrawAll();
}

/////////////////////////////////////////////////////////////////////////////
//  OnShowSql
//    The OnShowSql function provides user with ability to view and modify
//    current SQL query.
void CTPView::OnShowSql() 
{
}

/////////////////////////////////////////////////////////////////////////////
//  OnAppendRow
void CTPView::OnAppendRow() 
{
  m_grid.AppendRow();
}

/////////////////////////////////////////////////////////////////////////////
//  OnDeleteRecord
void CTPView::OnDeleteRecord() 
{
  m_grid.DeleteRow( m_grid.GetCurrentRow());
}

/////////////////////////////////////////////////////////////////////////////
//  OnAddTable
//    The OnAddTable function demonstrates that the Ultimate Grid can display
//    number of sheets, where each sheet is bound to a different datasource.
//    In this case we provide the user to bind additional sheets to datasources
//    that use same database.  Each sheet is completely independent of each other.
void CTPView::OnAddTable() 
{
}



void CTPView::OnShiftF1()
{
//    if (!m_data) return;

    int col = m_grid.GetCurrentCol();
    long row = m_grid.GetCurrentRow();

    CString s;
    
    s.Format("%d %d", row, col);

//    CString s = m_data->GetFieldF1(col);

//    s.Format("Help %d %d", row, col);
    AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);
}


