// ADO SampleView.cpp : implementation of the CVirtualTable2 class
//

#include "stdafx.h"

#include "VirtualTable2.h"
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
// CVirtualTable2

IMPLEMENT_DYNCREATE(CVirtualTable2, CPaneDialog)

BEGIN_MESSAGE_MAP(CVirtualTable2, CPaneDialog)
    //{{AFX_MSG_MAP(CVirtualTable2)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_SETFOCUS()
    ON_WM_ERASEBKGND()
    ON_WM_DESTROY()
    ON_COMMAND(ID_REFRESH_DATA, OnRefreshData)
    ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
    ON_MESSAGE(WM_LDBL, OnDblClick)

    ON_COMMAND(ID_HIDE_COLUMN, OnHideColumn)
    ON_COMMAND(ID_MOVE_TO2, OnMoveTo)
    ON_COMMAND(ID_SHIFT_F1, OnShiftF1)
    ON_COMMAND(ID_EDIT_FIND, OnEditFind)
    //}}AFX_MSG_MAP
    // Standard printing commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVirtualTable2 construction/destruction

CVirtualTable2::CVirtualTable2()
{
    // TODO: add construction code here
    m_pDSList = NULL;
    m_bInit = FALSE;

    m_defFont.CreatePointFont(90, _T("Tahoma"));

}

CVirtualTable2::~CVirtualTable2()
{
    // clear the DS link list
    _tagDSList* pTempItem = m_pDSList,
        * pDeleteItem = NULL;

    while (pTempItem != NULL)
    {
        pDeleteItem = pTempItem;
        pTempItem = pTempItem->next;
        delete pDeleteItem->m_dsADO;
        delete pDeleteItem;
    }
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualTable2 message handlers

int CVirtualTable2::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CPaneDialog::OnCreate(lpCreateStruct) == -1)
        return -1;

    // TODO: Add your specialized creation code here
    m_grid.CreateGrid(WS_CHILD | WS_VISIBLE, CRect(0, 0, 4000, 2000), this, 23422);

    return 0;
}

void CVirtualTable2::resize()
{
    CRect sz;
    GetClientRect(sz);
    if (!::IsWindow(m_grid.m_hWnd)) return;

    if (!m_grid.m_hWnd) return;

    if (!m_bInit) return;

    SetWindowText(m_title);

    m_grid.MoveWindow(0, 0, sz.Width(), sz.Height());
}



void CVirtualTable2::OnSize(UINT nType, int cx, int cy)
{
    CPaneDialog::OnSize(nType, cx, cy);
    resize();
}

void CVirtualTable2::OnSetFocus(CWnd* pOldWnd)
{
    CPaneDialog::OnSetFocus(pOldWnd);

    if (!m_bInit) return;

    SetWindowText(m_title);

    m_grid.SetFocus();
}

BOOL CVirtualTable2::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default
    return TRUE;
    // return CPaneDialog::OnEraseBkgnd(pDC);
}


/////////////////////////////////////////////////////////////////////////////
//  OnRefreshData
//    The OnRefreshData function will force the data in current sheet to
//    be reloaded from the database.
void CVirtualTable2::OnRefreshData()
{
    int nSheetNumber = m_grid.GetSheetNumber();
    _tagDSList* pTempItem = m_pDSList;

    for (int nIndex = 0; nIndex < nSheetNumber && pTempItem != NULL; nIndex++)
        pTempItem = pTempItem->next;

    if (pTempItem == NULL)
        return;

    pTempItem->m_dsADO->RefreshData();
    m_grid.RedrawAll();
}

/////////////////////////////////////////////////////////////////////////////
//  OnShowSql
//    The OnShowSql function provides user with ability to view and modify
//    current SQL query.
void CVirtualTable2::OnShowSql()
{
#if 0
    CSQLEditDlg sqlDlg;
    int nSheetNumber = m_grid.GetSheetNumber();

    // get a pointer to the current datasource
    _tagDSList* pTempItem = m_pDSList;

    for (int nIndex = 0; nIndex < nSheetNumber && pTempItem != NULL; nIndex++)
        pTempItem = pTempItem->next;

    if (pTempItem == NULL)
        return;

    sqlDlg.SetSQLStr(pTempItem->m_dsADO->GetSQL());

    if (sqlDlg.DoModal() == IDOK)
    {
        GetDocument()->SetSQLString(sqlDlg.GetSQLStr(), nSheetNumber);
        // Reset the grid
        m_grid.SetNumberRows(0);
        m_grid.SetNumberCols(0);
        // reload the recordset
        pTempItem->m_dsADO->Close();
        pTempItem->m_dsADO->OpenSQL(GetDocument()->GetSQLString(nSheetNumber), GetDocument()->GetConnectionString());
        m_grid.SetGridUsingDataSource(pTempItem->m_dsADO->GetID());
        m_grid.AdjustComponentSizes();
    }
#endif
}


/////////////////////////////////////////////////////////////////////////////
//  OnDeleteRecord
void CVirtualTable2::OnDeleteRecord()
{
    m_grid.DeleteRow(m_grid.GetCurrentRow());
}

void setView(CView* pView0);

void CVirtualTable2::OnMoveTo() // перейти в окно схемы
{
    long row = m_grid.GetCurrentRow();

    long id;
    CUGCell cell;
    m_pDSList->m_dsADO->GetCell(0, row, &cell);
    cell.GetNumber(&id);

    //    VIRT_DATA typ = m_data->getType();
    VIRT_DATA typ = m_grid.m_typ;

    CGidrView* pView = getView();
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
            pView->SendMessage(WM_USER_FIND_GEO, (WPARAM)id, (LPARAM)(LPCSTR)m_tn);
            break;
        case virt_data_geo_big_line:
            pView->SendMessage(WM_USER_FIND_GEO_BIG_LINE, (WPARAM)id, (LPARAM)(LPCSTR)m_tn);
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

    void setView(CView * pView0);

    CGidrView* pView = getView();
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
            pView->SendMessage(WM_USER_FIND_GEO, (WPARAM)id, (LPARAM)(LPCSTR)tn);
        }
        break;
        }
    }
#endif
}


void CVirtualTable2::OnHideColumn()
{
    m_grid.HideColumn();
}

void CVirtualTable2::OnShiftF1()
{
    int col = m_grid.GetCurrentCol();
    long row = m_grid.GetCurrentRow();

    CString s;
    s.Format("%d %d", row, col);
    AfxMessageBox(s, MB_OK | MB_ICONINFORMATION);
}

LRESULT CVirtualTable2::HandleInitDialog(WPARAM, LPARAM)
{
    UpdateData(FALSE);

    //    m_grid.AttachGrid(this, IDC_LIST1);
    return TRUE;
}

bool CVirtualTable2::init(int type_of_net, const char* db, const char* connect, const char* tn, const char* q, const char* title, bool is_head, VIRT_DATA typ)
{
    if (is_head && readTable(db, tn, m_head)) {
        m_grid.setHead(&m_head);
    }
    m_grid.m_typ = typ;


    // bind the datasource to data, provide it with query and connection information
    m_pDSList = new _tagDSList;
    m_pDSList->m_dsADO = new CADOImpDatasource;


    if (m_pDSList->m_dsADO->OpenSQL(q, connect, type_of_net) == UG_ERROR) {
        return false;
    }

    m_pDSList->m_dsADO->m_head = m_head.rows > 0 ? &m_head : NULL;
    m_pDSList->m_dsADO->SetTableName(db, tn);

    m_grid.SetUniformRowHeight(TRUE);
    m_grid.SetDefRowHeight(20 * getScreenMas());

    m_grid.AddDataSource(m_pDSList->m_dsADO);
    // set added datasource to be default
    m_grid.SetDefDataSource(m_pDSList->m_dsADO->GetID());
    // complete the datasource binding operation, let grid adjust
    // itself to reflect information found in the datasource
    m_grid.SetGridUsingDataSource(m_pDSList->m_dsADO->GetID());

    m_title = title;
    m_typ = virt_data_geo;
    m_typ = typ;
    m_tn = tn;
    m_q = q;


    m_grid.setName(tn, m_typ);

    m_bInit = TRUE;

    resize();

    //  m_grid.SetFocus();
    //  SetDefFont( &m_defFont );

    SetWindowText(m_title);

    return true;
}



LRESULT CVirtualTable2::OnDblClick(WPARAM wParam, LPARAM lParam)
{
    long row = m_grid.GetCurrentRow();

    long id;
    CUGCell cell;


    m_pDSList->m_dsADO->GetCell(0, row, &cell);
    cell.GetNumber(&id);

    //    long row = (long)wParam;

    VIRT_DATA typ = m_typ;
    //    int id = (int)m_grid.m_data.m_data->getData(row);
    //    CString tn = m_grid.m_data.m_data->getTable(row);

    CGidrView* pView = getView();
    if (pView) {
        switch (typ) {
        case virt_data_node:
            pView->SendMessage(WM_USER_FIND_NODE, (WPARAM)id);
            break;
        case virt_data_line:
            pView->SendMessage(WM_USER_FIND_LINE, (WPARAM)id);
            break;
        case virt_data_line_big:
            pView->SendMessage(WM_USER_FIND_LINE_BIG, (WPARAM)id);
            break;
        case virt_data_geo:
            pView->SendMessage(WM_USER_FIND_GEO, (WPARAM)id, (LPARAM)(LPCSTR)m_tn);
            break;
        case virt_data_geo_big_line:
            pView->SendMessage(WM_USER_FIND_GEO_BIG_LINE, (WPARAM)id, (LPARAM)(LPCSTR)m_tn);
            break;
        }
    }
    return 0;
}

void CVirtualTable2::Excel()
{
    int cols = m_grid.GetNumberCols();

    if (cols <= 0) return;

    list<int> list_col;

    for (int col = 0; col < cols; col++) {
        int w = m_grid.GetColWidth(col);
        if (w != 0) {
            list_col.push_back(col);
        }
    }

    m_pDSList->m_dsADO->excel(list_col);
}



CFont* CVirtualTable2::GetFontNormal()
{
    return &m_defFont;
}


void CVirtualTable2::OnEditFind()
{
    //    AfxMessageBox("!", MB_OK|MB_ICONINFORMATION);
}


void CVirtualTable2::OnDestroy()
{
    // 

    CPaneDialog::OnDestroy();
}


CTabbedPane* CVirtualTable2::CreateTabbedPane()
{
    CTabbedPane* pTabbedBar = CDockablePane::CreateTabbedPane();

    if (pTabbedBar) {
      pTabbedBar->SetMinSize(CSize(200, 200)); //Here is that
    }

    return pTabbedBar;
}