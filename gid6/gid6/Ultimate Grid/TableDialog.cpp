// ADO SampleView.cpp : implementation of the CTableDialog class
//

#include "stdafx.h"

#include "TableDialog.h"
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

static int IDLIST = 23422;

/////////////////////////////////////////////////////////////////////////////
// CTableDialog

//IMPLEMENT_DYNCREATE(CTableDialog, CDialog)

BEGIN_MESSAGE_MAP(CTableDialog, CDialog)
    //{{AFX_MSG_MAP(CTableDialog)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_WM_SETFOCUS()
    ON_WM_ERASEBKGND()
    ON_COMMAND(ID_REFRESH_DATA, OnRefreshData)
    ON_MESSAGE(WM_LDBL, OnDblClick)

    ON_COMMAND(ID_HIDE_COLUMN, OnHideColumn)
    ON_COMMAND(ID_MOVE_TO2, OnMoveTo)
    ON_COMMAND(ID_SHIFT_F1, OnShiftF1)

    ON_BN_CLICKED(IDC_ADD, OnAdd)
    ON_BN_CLICKED(IDC_DEL, OnDel)
   //}}AFX_MSG_MAP
    // Standard printing commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTableDialog construction/destruction

CTableDialog::CTableDialog(CWnd *pParent, int type_of_net, const char *db, const char *connect, const char *tn, const char *q, int n_ret, VIRT_DATA typ, const char *caption, int idd)
///    : CDialog(CTableDialog::IDD, pParent)
    : CDialog(idd, pParent)
{
    m_pDSList = NULL;

    m_head.rows = 0;

    if (typ == virt_data_edit_add) typ = virt_data_edit;
    if (typ == virt_data_edit_add_del) typ = virt_data_edit_del;


    m_type_of_net = type_of_net;
    m_db = db;
    m_connect = connect;
    m_typ = virt_data_unknown;
    m_typ = typ;
    m_tn = tn;
    m_title = tn;
    m_q = q;
    m_n_ret = n_ret;
    m_strRet = "";

    m_n_keys = 0;

    m_idCol = -1;

    m_find = nullptr;

    m_caption = tn;

    if (caption) {
        m_caption = caption;
    }

    m_modal = TRUE;

    m_profile.Format("TableDialog\\%s", m_tn);

}

CTableDialog::~CTableDialog()
{
    // clear the DS link list
    _tagDSList *pTempItem = m_pDSList,
        *pDeleteItem = NULL;

    while (pTempItem != NULL)
    {
        pDeleteItem = pTempItem;
        pTempItem = pTempItem->next;
        delete pDeleteItem->m_dsADO;
        delete pDeleteItem;
    }
}


/////////////////////////////////////////////////////////////////////////////
// CTableDialog message handlers

int CTableDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDialog::OnCreate(lpCreateStruct) == -1)
        return -1;

    int dy = m_find ? 20 * getScreenMas() : 0;

    // TODO: Add your specialized creation code here
    m_grid.CreateGrid(WS_CHILD | WS_VISIBLE, CRect(0, dy, 2000, 1000), this, IDLIST);

    return 0;
}

void CTableDialog::OnSize(UINT nType, int cx, int cy)
{
  CDialog::OnSize(nType, cx, cy);

  if (!m_grid.m_hWnd) return;

  if (cx > 0 && cy > 0)  {
    CRect rect;
    int id_grid = m_grid.GetDlgCtrlID();

    HDWP hDwp = BeginDeferWindowPos(1);

    int dy = m_find ? 20 * getScreenMas() : 0;

    hDwp = DeferWindowPos(hDwp, m_grid.m_hWnd, NULL, 0, dy, cx, cy-40*getScreenMas(), SWP_NOZORDER | SWP_NOZORDER);

    for (CWnd *wnd = GetTopWindow(); wnd; wnd = wnd->GetNextWindow()) {
        CString ss;
        wnd->GetWindowText(ss);

        int id = wnd->GetDlgCtrlID();
        if (id != id_grid && id != IDC_EDIT) {
            wnd->GetWindowRect(rect);
            ScreenToClient(&rect);
            hDwp = DeferWindowPos(hDwp, wnd->m_hWnd, NULL, rect.left, cy - rect.Height() - 5, -1, -1, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
        }
    }
    EndDeferWindowPos(hDwp);
  }
}

void CTableDialog::OnSetFocus(CWnd* pOldWnd)
{
    CDialog::OnSetFocus(pOldWnd);

    // TODO: Add your message handler code here
    m_grid.SetFocus();
}

BOOL CTableDialog::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default
//    return TRUE;
    return CDialog::OnEraseBkgnd(pDC);
}


/////////////////////////////////////////////////////////////////////////////
//  OnRefreshData
//    The OnRefreshData function will force the data in current sheet to
//    be reloaded from the database.
void CTableDialog::OnRefreshData()
{
    int nSheetNumber = m_grid.GetSheetNumber();
    _tagDSList *pTempItem = m_pDSList;

    for (int nIndex = 0; nIndex < nSheetNumber && pTempItem != NULL; nIndex++)
        pTempItem = pTempItem->next;

    if (pTempItem == NULL)
        return;

    pTempItem->m_dsADO->RefreshData();
    pTempItem->m_dsADO->Refresh2();
    m_grid.RefreshData();
    m_grid.Refresh2();
    m_grid.RedrawAll();
}

/////////////////////////////////////////////////////////////////////////////
//  OnShowSql
//    The OnShowSql function provides user with ability to view and modify
//    current SQL query.
void CTableDialog::OnShowSql()
{
}


/////////////////////////////////////////////////////////////////////////////
//  OnDeleteRecord
void CTableDialog::OnDeleteRecord()
{
    m_grid.DeleteRow(m_grid.GetCurrentRow());
}

void setView(CView *pView0);

void CTableDialog::OnMoveTo() // перейти в окно схемы
{
    long row = m_grid.GetCurrentRow();

    long id;
    CUGCell cell;

    m_pDSList->m_dsADO->GetCell(0, row, &cell);
    cell.GetNumber(&id);

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
            pView->SendMessage(WM_USER_FIND_GEO, (WPARAM)id, (LPARAM)(LPCSTR)m_tn);
            break;
        case virt_data_geo_big_line:
            pView->SendMessage(WM_USER_FIND_GEO_BIG_LINE, (WPARAM)id, (LPARAM)(LPCSTR)m_tn);
            break;
        }
    }

}


void CTableDialog::OnHideColumn()
{
    m_grid.HideColumn();
}

void CTableDialog::OnShiftF1()
{
    int col = m_grid.GetCurrentCol();
    long row = m_grid.GetCurrentRow();

    CString s;
    s.Format("%d %d", row, col);
    AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);
}




BOOL CTableDialog::OnInitDialog()
{
    UpdateData(FALSE);

////////////////////////////////////////////////////////////////////////////////////

    CWnd* wnd = GetDlgItem(IDC_EDIT);

//    m_find = dynamic_cast<CEdit*>(wnd);
    m_find = (CEdit*)wnd;

    // bind the datasource to data, provide it with query and connection information
    m_pDSList = new _tagDSList;
    m_pDSList->m_dsADO = new CADOImpDatasource;

    if (m_pDSList->m_dsADO->OpenSQL(m_q, m_connect, m_type_of_net) == UG_ERROR) {
        return TRUE;
    }

    m_pDSList->m_dsADO->m_head = m_head.rows > 0 ? &m_head : NULL;
    m_pDSList->m_dsADO->SetTableName(m_db, m_tn);

    if (m_n_keys == 1) {
      m_pDSList->m_dsADO->SetKey(m_keyName, m_keyValue);
    }
    if (m_n_keys == 2) {
      m_pDSList->m_dsADO->SetKey2(m_keyName, m_keyValue, m_keyName2, m_keyValue2);
    }

    m_pDSList->m_dsADO->SetIdCol(m_idCol);


    m_grid.SetUniformRowHeight(TRUE);
    m_grid.SetDefRowHeight(20*getScreenMas());

    m_grid.AddDataSource(m_pDSList->m_dsADO);
    // set added datasource to be default
    m_grid.SetDefDataSource(m_pDSList->m_dsADO->GetID());
    // complete the datasource binding operation, let grid adjust
    // itself to reflect information found in the datasource
    m_grid.SetGridUsingDataSource(m_pDSList->m_dsADO->GetID());

    m_grid.m_GI->m_colInfo[0].dataSource->SetOption(UG_ADO_DS_USE_SIMPLENEW, 1, 0);

    m_grid.setName(m_tn, m_typ);

//////////////////////////////////////////////////////////////////////////////////////////

    SetWindowText(m_caption);

    int bx = AfxGetApp()->GetProfileInt(m_profile, "bx", 0);
    int by = AfxGetApp()->GetProfileInt(m_profile, "by", 0);
    int cx = AfxGetApp()->GetProfileInt(m_profile, "cx", 0);
    int cy = AfxGetApp()->GetProfileInt(m_profile, "cy", 0);

    if (cx > 0 && cy > 0) {
        SetWindowPos(&wndTopMost, bx, by, cx, cy, SWP_NOZORDER);
    }
    return TRUE;
}

void CTableDialog::OnDestroy()
{
    WINDOWPLACEMENT wpi;
    GetWindowPlacement(&wpi);

    AfxGetApp()->WriteProfileInt(m_profile, "bx", wpi.rcNormalPosition.left);
    AfxGetApp()->WriteProfileInt(m_profile, "by", wpi.rcNormalPosition.top);
    AfxGetApp()->WriteProfileInt(m_profile, "cx", wpi.rcNormalPosition.right - wpi.rcNormalPosition.left);
    AfxGetApp()->WriteProfileInt(m_profile, "cy", wpi.rcNormalPosition.bottom - wpi.rcNormalPosition.top);


    CDialog::OnDestroy();
    if (!m_modal)
         delete this; // Shown as non-modal, we'll clean up ourselves
}

bool CTableDialog::setText(long row)
{
    bool r = false;
    CUGCell cell;

    m_pDSList->m_dsADO->GetCell(m_n_ret, row, &cell);

    m_strRet = cell.GetText();

    int ret = m_pDSList->m_dsADO->GetCell(0, row, &cell);

    if (!ret) {
        CString s = cell.GetText();

        s.Replace("\xA0", "");

        m_id = atoi(s);

        int cols = m_grid.GetNumberCols();

        for (int col = 0; col < cols; col++) {
            CString s;
//            memset(&cell, 0, sizeof(cell));
            cell.SetDefaultInfo();
            m_pDSList->m_dsADO->GetColName(col, &s);
            m_pDSList->m_dsADO->GetCell(col, row, &cell);
            CString v = cell.GetText();
            m_val_id[col] = v;
            m_val_name[s] = v;
        }

//    cell.GetNumber(&id);
//    AfxMessageBox(txt, MB_OK|MB_ICONINFORMATION);
        r = true;
    }
    return r;
}

LRESULT CTableDialog::OnDblClick(WPARAM wParam, LPARAM lParam)
{
    long row = m_grid.GetCurrentRow();
    if (row >= 0 && setText(row)) {
        EndDialog(IDOK);
    }

    return 0;
}

CString CTableDialog::getValue(const char *s)
{
    map<CString, CString>::const_iterator it = m_val_name.find(s);

    if (it != m_val_name.end()) {
        return it->second;
    }
    return "";
}

void CTableDialog::setHead(const char *db, const char *name)
{
    if (readTable(db, name, m_head)) {
        m_grid.setHead(&m_head);
        if (m_head.q2 != "") {
            m_q = m_head.q2;

            if (m_n_keys == 1) {
                CString qq;
                qq.Format(" WHERE %s=%d", m_keyName, m_keyValue);
                m_q += qq;
            }
            if (m_n_keys == 2) {
                CString qq;
                qq.Format(" WHERE %s=%d AND %s=%d", m_keyName, m_keyValue, m_keyName2, m_keyValue2);
                m_q += qq;
            }
        }
    }
}


void CTableDialog::SetKey(CString key_name, int key_value)
{
  m_n_keys = 1;

  m_keyName = key_name;
  m_keyValue = key_value;
}

void CTableDialog::SetKey2(CString key_name1, int key_value1, CString key_name2, int key_value2)
{
  m_n_keys = 2;

  m_keyName = key_name1;
  m_keyValue = key_value1;

  m_keyName2 = key_name2;
  m_keyValue2 = key_value2;
}

void CTableDialog::SetIdCol(int idCol)
{
    m_idCol = idCol;
}


void CTableDialog::OnOK() 
{
    long row = m_grid.GetCurrentRow();

    if (row >= 0) {
        setText(row);
        CDialog::OnOK();
    }
    else if (m_modal) {
        CDialog::OnOK();
    }
}

void CTableDialog::OnAdd() 
{
//    if (isVTAdd(m_typ)) {
        m_grid.AppendRow();
//    }
}

void CTableDialog::OnDel() 
{
//    if (isVTDel(m_typ)) {
        long row = m_grid.GetCurrentRow();

        if (row >= 0) {
            m_grid.DeleteRow(row);
        }
//    }
}

void CTableDialog::proc(UINT id, void* p1, void* p2, void* p3, void* p4 )
{
    map<UINT, FUN2>::iterator it = m_map_proc.find(id);
    if (it != m_map_proc.end()) {
        FUN2 f = it->second;
        f.f(this, p1, p2, p3, p4);
    }
}



BOOL CTableDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    UINT nID = LOWORD(wParam);
    UINT msg = HIWORD(wParam);

    switch (msg) {
    case BN_CLICKED:
        //      OnBnClicked(control);  
        if (nID != IDOK && nID != IDCANCEL) {
            map<UINT, FUN2>::iterator it = m_map_proc.find(nID);
            if (it != m_map_proc.end()) {
                FUN2 f = it->second;
                f.f(this, f.p1, f.p2, f.p3, f.p4);
            }
        }
        break;
    }



  if (nID == IDC_EDIT) {
  }

  return CDialog::OnCommand(wParam, lParam);
}


void CTableDialog::addProc(UINT nID, fun2 proc, void* pp1, void* pp2, void* pp3, void* pp4)
{
    m_map_proc[nID].f = proc;
    m_map_proc[nID].p1 = pp1;
    m_map_proc[nID].p2 = pp2;
    m_map_proc[nID].p3 = pp3;
    m_map_proc[nID].p4 = pp4;
}

int CTableDialog::Execute(const char * q)
{
    return m_grid.Execute(q);
}
