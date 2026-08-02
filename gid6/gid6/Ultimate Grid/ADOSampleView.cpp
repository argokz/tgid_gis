// ADO SampleView.cpp : implementation of the CADOSampleView class
//

#include "stdafx.h"

#include "ADOSampleView.h"
//#include "SqlEditDlg.h"

#include "resource.h"
#include "gid6Doc.h"
#include "gidrview.h"
#include "virtualdata.h"

#include "wm_user.h"


double getScreenMas();


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CADOSampleView

IMPLEMENT_DYNCREATE(CADOSampleView, CView)

BEGIN_MESSAGE_MAP(CADOSampleView, CView)
  //{{AFX_MSG_MAP(CADOSampleView)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_SETFOCUS()
  ON_WM_ERASEBKGND()
  ON_COMMAND(ID_REFRESH_DATA, OnRefreshData)
  ON_COMMAND(ID_EDIT_FIND, OnEditFind)

//  ON_COMMAND(ID_SWITCH_RECORDS, OnHideZero)
  ON_COMMAND(ID_SWITCH_RECORDS, OnSwitchRecord)

/**
  ON_COMMAND(ID_SHOW_SQL, OnShowSql)
  ON_COMMAND(ID_APPEND_ROW, OnAppendRow)
  ON_COMMAND(ID_DELETE_RECORD, OnDeleteRecord)
  ON_COMMAND(ID_ADD_TABLE, OnAddTable)
*/

  ON_COMMAND(ID_EXCEL, OnExcel)
  ON_MESSAGE(WM_TABLE_CHANGE, OnTableChange)

  ON_COMMAND(ID_HIDE_COLUMN, OnHideColumn)
  ON_COMMAND(ID_MOVE_TO2, OnMoveTo)
  ON_COMMAND(ID_SHIFT_F1, OnShiftF1)

  ON_COMMAND(ID_VIEW_ZOOMIN , OnZoomIn)
  ON_COMMAND(ID_VIEW_ZOOMOUT, OnZoomOut)
  ON_COMMAND(ID_VIEW_100    , OnZoom100)

  ON_COMMAND(ID_EDIT_FIND, OnEditFind)


  //}}AFX_MSG_MAP
  // Standard printing commands
  ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
  ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
  ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CADOSampleView construction/destruction

CADOSampleView::CADOSampleView()
{
    // TODO: add construction code here
    m_pDSList = NULL;

    m_colorCol = -1;
    m_head.rows = 0;
    m_show_all = false;
    m_is_head = true;
}

CADOSampleView::~CADOSampleView()
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

BOOL CADOSampleView::PreCreateWindow(CREATESTRUCT& cs)
{
  // TODO: Modify the Window class or styles here by modifying
  //  the CREATESTRUCT cs

  return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CADOSampleView drawing

void CADOSampleView::OnDraw(CDC* pDC)
{
//  CDocument* pDoc = GetDocument();
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
// CADOSampleView printing

BOOL CADOSampleView::OnPreparePrinting(CPrintInfo* pInfo)
{
  // default preparation
  return DoPreparePrinting(pInfo);
}

void CADOSampleView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
  pInfo->SetMaxPage(m_grid.PrintInit(pDC,pInfo->m_pPD,0,0,m_grid.GetNumberCols()-1,
    m_grid.GetNumberRows()-1));
  m_isPrinting = TRUE;
}

void CADOSampleView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
  m_isPrinting = FALSE;
}


void CADOSampleView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
    // TODO: Add your specialized code here and/or call the base class

    m_drawRect = pInfo->m_rectDraw;
    m_printPage = pInfo->m_nCurPage;

    CView::OnPrint(pDC, pInfo);
}


/////////////////////////////////////////////////////////////////////////////
// CADOSampleView diagnostics

#ifdef _DEBUG
void CADOSampleView::AssertValid() const
{
  CView::AssertValid();
}

void CADOSampleView::Dump(CDumpContext& dc) const
{
  CView::Dump(dc);
}

/**
CADOSampleDoc* CADOSampleView::GetDocument() // non-debug version is inline
{
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CADOSampleDoc)));
  return (CADOSampleDoc*)m_pDocument;
}
*/
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CADOSampleView message handlers

int CADOSampleView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CView::OnCreate(lpCreateStruct) == -1)
    return -1;
  
  // TODO: Add your specialized creation code here
  m_grid.CreateGrid( WS_CHILD|WS_VISIBLE, CRect(0,0,2000,1000), this, 23424 );
  
  return 0;
}

void CADOSampleView::OnSize(UINT nType, int cx, int cy) 
{
  CView::OnSize(nType, cx, cy);
  
  // TODO: Add your message handler code here
  m_grid.MoveWindow( 0, 0, cx, cy );
}

void CADOSampleView::OnSetFocus(CWnd* pOldWnd) 
{
  CView::OnSetFocus(pOldWnd);
  
  // TODO: Add your message handler code here
  m_grid.SetFocus();
}

BOOL CADOSampleView::OnEraseBkgnd(CDC* pDC) 
{
  // TODO: Add your message handler code here and/or call default
  return TRUE;
  // return CView::OnEraseBkgnd(pDC);
}

CString findTableRusName(CString d, CString n);


/////////////////////////////////////////////////////////////////////////////
//  OnInitialUpdate
//    The OnInitialUpdate function is used to bind the grid to ADO datasource
//    using the SQL and connect information specified by the user.
void CADOSampleView::OnInitialUpdate()
{
  CView::OnInitialUpdate();
  
  // bind the datasource to data, provide it with query and connection information
  m_pDSList = new _tagDSList;
  m_pDSList->m_dsADO = new CADOImpDatasource;

//  m_pDSList->m_dsADO->SetOption(UG_ADO_DS_USE_GHOSTROW, FALSE, FALSE);
//  m_pDSList->m_dsADO->SetOption(UG_ADO_DS_USE_SIMPLENEW, FALSE, FALSE);


  Cgid6Doc* pDoc = (Cgid6Doc*) GetDocument();


  m_view_db = pDoc->m_view_db;
  m_view_tn = pDoc->m_view_tn;
  m_view_sql = pDoc->m_view_sql;

  m_pDSList->m_dsADO->showAllInit(pDoc->m_view_all);


  if (m_pDSList->m_dsADO->OpenSQL(pDoc->m_view_sql, pDoc->m_view_connect, pDoc->m_type_of_net) == UG_ERROR) {
      return;
  }

  setHead(pDoc->m_view_db, pDoc->m_view_head);

  m_pDSList->m_dsADO->SetColorCol(pDoc->m_view_color_col);


  m_pDSList->m_dsADO->m_head = m_head.rows > 0 ? &m_head : NULL;
  m_pDSList->m_dsADO->SetTableName(pDoc->m_view_db, pDoc->m_view_tn);

   m_grid.SetUniformRowHeight(TRUE);
   m_grid.SetDefRowHeight(20*getScreenMas());

//  m_grid.setName(pDoc->m_view_tn, pDoc->m_view_typ);

  // add the open datasource
  m_grid.AddDataSource( m_pDSList->m_dsADO );
  // set added datasource to be default
  m_grid.SetDefDataSource( m_pDSList->m_dsADO->GetID());
  // complete the datasource binding operation, let grid adjust
  // itself to reflect information found in the datasource
  m_grid.SetGridUsingDataSource( m_pDSList->m_dsADO->GetID());  

  m_grid.setName(pDoc->m_view_tn, pDoc->m_view_typ);

//  m_grid.setEdit(pDoc->m_view_is_edit);
//  m_grid.setAdd(pDoc->m_view_is_add);
//  m_grid.setDel(pDoc->m_view_is_del);

   CString rus_name = findTableRusName(pDoc->m_view_db, pDoc->m_view_tn);


   GetParentFrame()->SetWindowText(rus_name);
}

/////////////////////////////////////////////////////////////////////////////
//  OnRefreshData
//    The OnRefreshData function will force the data in current sheet to
//    be reloaded from the database.
void CADOSampleView::OnRefreshData() 
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
void CADOSampleView::OnShowSql() 
{
#if 0
  CSQLEditDlg sqlDlg;
  int nSheetNumber = m_grid.GetSheetNumber();

  // get a pointer to the current datasource
  _tagDSList *pTempItem = m_pDSList;
  
  for( int nIndex = 0; nIndex < nSheetNumber && pTempItem != NULL; nIndex ++ )
    pTempItem = pTempItem->next;

  if ( pTempItem == NULL )
    return;

  sqlDlg.SetSQLStr( pTempItem->m_dsADO->GetSQL());

  if ( sqlDlg.DoModal() == IDOK )
  {
    GetDocument()->SetSQLString( sqlDlg.GetSQLStr(), nSheetNumber );
    // Reset the grid
    m_grid.SetNumberRows( 0 );
    m_grid.SetNumberCols( 0 );
    // reload the recordset
    pTempItem->m_dsADO->Close();
    pTempItem->m_dsADO->OpenSQL( GetDocument()->GetSQLString( nSheetNumber ), GetDocument()->GetConnectionString());
    m_grid.SetGridUsingDataSource( pTempItem->m_dsADO->GetID());  
    m_grid.AdjustComponentSizes();
  }
#endif
}

/////////////////////////////////////////////////////////////////////////////
//  OnAppendRow
void CADOSampleView::OnAppendRow() 
{
  m_grid.AppendRow();
/*  
  int row = m_grid.GetNumberRows() - 1; 
  m_grid.QuickSetText(0, row, _T("LastName"));
  m_grid.QuickSetText(0, row, _T("LastName"));
  m_grid.QuickSetText(1, row, _T("FirstName"));
  m_grid.QuickSetText(2, row, _T("100100"));
  m_grid.RedrawRow(row);
*/
}

/////////////////////////////////////////////////////////////////////////////
//  OnDeleteRecord
void CADOSampleView::OnDeleteRecord() 
{
  m_grid.DeleteRow( m_grid.GetCurrentRow());
}

/////////////////////////////////////////////////////////////////////////////
//  OnAddTable
//    The OnAddTable function demonstrates that the Ultimate Grid can display
//    number of sheets, where each sheet is bound to a different datasource.
//    In this case we provide the user to bind additional sheets to datasources
//    that use same database.  Each sheet is completely independent of each other.
void CADOSampleView::OnAddTable() 
{
#if 0
  // Prompt the user to specify the SQL query to show in the new sheet
  CSQLEditDlg sqlDlg;
  sqlDlg.SetSQLStr( _T("SELECT * FROM Customers;"));

  if ( sqlDlg.DoModal() == IDOK )
  {
    if ( sqlDlg.GetSQLStr() == _T(""))
      return;

    int nNumberSheets = m_grid.GetNumberSheets();
    // Get the new SQL query and make sure that empty string is not used
    GetDocument()->SetSQLString( sqlDlg.GetSQLStr(), nNumberSheets );
    nNumberSheets ++;
    // Add and initialize new sheet
    m_grid.SetNumberSheets( nNumberSheets );
    // Add and initialize new tab
    CString tempStr;
    tempStr.Format( _T("Sheet %d"), nNumberSheets );
    m_grid.AddTab( tempStr, nNumberSheets );
    // Adjust the view to display the new sheet
    m_grid.SetCurrentTab( nNumberSheets );
    m_grid.SetSheetNumber( nNumberSheets - 1 );
    m_grid.SetPaintMode( TRUE );
    // Bind the new sheet to the datasource
    _tagDSList *pTempItem = m_pDSList;

    if ( pTempItem == NULL )
      return;

    while ( pTempItem->next != NULL )
      pTempItem = pTempItem->next;

    pTempItem->next = new _tagDSList;
    pTempItem = pTempItem->next;
    pTempItem->m_dsADO = new CADOImpDatasource;

    pTempItem->m_dsADO->OpenSQL( sqlDlg.GetSQLStr(), GetDocument()->GetConnectionString());
    // add the open datasource
    m_grid.AddDataSource( pTempItem->m_dsADO );
    // set added datasource to be default
    m_grid.SetDefDataSource( pTempItem->m_dsADO->GetID());
    // complete the datasource binding operation, let grid adjust
    // itself to reflect information found in the datasource
    m_grid.SetGridUsingDataSource( pTempItem->m_dsADO->GetID());
  }
#endif
}

void setView(CView *pView0);

CString findTableEngName(CString d, CString n);


void CADOSampleView::OnMoveTo() // перейти в окно схемы
{
    long row = m_grid.GetCurrentRow();

    long id;
    CUGCell cell;

//    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

    m_pDSList->m_dsADO->GetCell(0, row, &cell);
    cell.GetNumber(&id);

    int cc = m_grid.GetNumberCols();

    for (int c = 0; c < cc; c++) {
        m_pDSList->m_dsADO->GetCell(c, -1, &cell);
        CString s = cell.GetText();
        if (s == "Здание") {
            m_pDSList->m_dsADO->GetCell(c, row, &cell);
            cell.GetNumber(&id);
            break;
        }
    }



//    VIRT_DATA typ = m_data->getType();
    VIRT_DATA typ = m_grid.m_typ;

    CGidrView *pView = getView();
    if (pView) {
        setView(pView);
        
        switch (typ) {
            case virt_data_node:
            case virt_data_node_out:
                pView->SendMessage(WM_USER_FIND_NODE, (WPARAM)id);
                break;
            case virt_data_line:
            case virt_data_line_out:
                pView->SendMessage(WM_USER_FIND_LINE, (WPARAM)id);
                break;
            case virt_data_line_big:
                pView->SendMessage(WM_USER_FIND_LINE_BIG, (WPARAM)id);
                break;
            case virt_data_line_big2:
                pView->SendMessage(WM_USER_FIND_LINE_BIG2, (WPARAM)id);
                break;
            case virt_data_geo:
                {
                CString eng_name = findTableEngName(m_view_db, m_view_tn);
                pView->SendMessage(WM_USER_FIND_GEO, (WPARAM)id, (LPARAM) (LPCSTR)eng_name);
                }
                break;
            case virt_data_geo_big_line:
                {
                CString eng_name = findTableEngName(m_view_db, m_view_tn);
                pView->SendMessage(WM_USER_FIND_GEO_BIG_LINE, (WPARAM)id, (LPARAM) (LPCSTR)eng_name);
                }
                break;
            case virt_data_tu:
                {
                    pView->SendMessage(WM_USER_FIND_GEO, (WPARAM)id, (LPARAM)(LPCSTR)transl("Здания ТУ"));
                }
                break;
        }
    }



#if 0
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
        setView(pView);
        
        switch (typ) {
            case virt_data_node:
                pView->SendMessage(WM_USER_FIND_NODE, (WPARAM)id);
                break;
            case virt_data_line:
                pView->SendMessage(WM_USER_FIND_LINE, (WPARAM)id);
                break;
            case virt_data_out:
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
#endif
}


void CADOSampleView::OnHideColumn()
{
    m_grid.HideColumn();
}


void CADOSampleView::OnShiftF1()
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

void CADOSampleView::setHead(const char *db, const char *name)
{
    if (readTable(db, name, m_head)) {
        m_grid.setHead(&m_head);
    }
}

void CADOSampleView::SetColorCol(int col)
{
  m_colorCol = col;
}


void Status(int n, LPCTSTR txt);


LRESULT CADOSampleView::OnTableChange(WPARAM wParam, LPARAM)
{
    CString str;

    str = "43654326";

    int col = m_grid.GetCurrentCol()+1;
    long  row = m_grid.GetCurrentRow()+1;
    
    long rows = m_grid.GetNumRowsComplete();
    int cols = m_grid.GetNumberCols();

    str.Format("%d/%d %d/%d", row, rows, col, cols);
    

    Status(3, str);
    
    return 0;
}

// Показывать все или только часть объектов

void CADOSampleView::OnSwitchRecord()
{
//  AfxMessageBox("!", MB_OK|MB_ICONINFORMATION);
  m_show_all = !m_show_all;
  m_grid.showAll(m_show_all);
}


void CADOSampleView::OnExcel()
{
    int cols = m_grid.GetNumberCols();

    list<int> list_col;


    for (int col = 0; col < cols; col++) {
        int w = m_grid.GetColWidth(col);
        if (w != 0) {
            list_col.push_back(col);
        }
    }

    m_pDSList->m_dsADO->excel(list_col);
}


void CADOSampleView::OnZoomIn()
{
//    AfxMessageBox("+", MB_OK|MB_ICONINFORMATION);
}

void CADOSampleView::OnZoomOut()
{
//    AfxMessageBox("-", MB_OK|MB_ICONINFORMATION);
}

void SaveFont(const TCHAR *szSection, const LOGFONT &lf, COLORREF color);
bool LoadFont2(const TCHAR *szSection, LOGFONT &lf, COLORREF &color);

void CADOSampleView::OnZoom100()
{
    LOGFONT lf;
//    COLORREF color;

    CFont *font = m_grid.GetFontNormal();
    if (font) {
        font->GetLogFont(&lf);

        lf.lfCharSet = DEFAULT_CHARSET;

        CFontDialog fd(&lf, CF_EFFECTS | CF_SCREENFONTS);

        if (fd.DoModal() == IDOK) {
            fd.GetCurrentFont(&lf);

            font->CreateFontIndirect(&lf);
            m_grid.SetDefFont( font);
            m_grid.Refresh2();
            SaveFont(_T("AdoSampleFont"), lf, fd.GetColor());
        }
    }
}

#include "InputDia.h"


void CADOSampleView::OnEditFind()
{
    return;
    
    int col = m_grid.GetCurrentCol();
    long row = m_grid.GetCurrentRow();
    CString text = "";

    CInputDialog inD(this, _T("Поиск"), "Введите строку для поиска", text);
    if (inD.DoModal() == IDOK) {
        m_grid.FindFirst(&inD.m_strInput, &col, &row, UG_FIND_PARTIAL|UG_FIND_CASEINSENSITIVE);
    }
}
