// MMenu2.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "MMenu2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMMenu2 dialog

double getScreenMas();


CMMenu2::CMMenu2(CWnd* pParent, UINT nIDTemplate, const char* capt)
    : CDialog(nIDTemplate, pParent)
{
    //{{AFX_DATA_INIT(CMMenu2)
    m_edit = "";
    //}}AFX_DATA_INIT

    m_strCaption = "";
    if (capt) {
        m_strCaption = capt;
    }
    m_fun_del = nullptr;
}


void CMMenu2::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CMMenu2)
    DDX_Control(pDX, IDC_LIST1, m_List);
    DDX_Text(pDX, IDC_EDIT, m_edit);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMMenu2, CDialog)
    //{{AFX_MSG_MAP(CMMenu2)
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_EN_CHANGE(IDC_EDIT, OnChangeEdit)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMMenu2 message handlers

BOOL CMMenu2::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_strText = "";

    int w[1024];

    int i, j;

    for (j = 0; j < m_cols.GetSize(); j++) {
        w[j] = m_List.GetStringWidth(m_cols[j]);
    }

    for (i = 0; i < m_rows.GetSize(); i++) {
        CMenuRow r = m_rows[i];
        for (j = 0; j < m_cols.GetSize(); j++) {

            CString s = r.cols[j];
            s = s.Left(30);

            int ww = m_List.GetStringWidth(s);
            w[j] = max(ww, w[j]);
        }
    }


    for (i = 0; i < m_cols.GetSize(); i++) {
        m_List.InsertColumn(i, m_cols[i], LVCFMT_LEFT, w[i]);
    }

    init();

    for (i = 0; i < m_width.GetSize(); i++) {
        if (m_width[i] > 0) {
            m_List.SetColumnWidth(i, m_width[i]);
        }
    }





    /*

      for (i = 0; i < m_rows.GetSize() ; i++) {
        CMenuRow r = m_rows[i];
        m_List.InsertItem(i, r.cols[0]);

        for ( j = 1; j < m_cols.GetSize(); j++ ) {
          m_List.SetItem(i, j, LVIF_TEXT, r.cols[j], 0,0,0,0);
        }
      }
    */

    SetWindowText(m_strCaption);

    m_profile.Format("mmenu\\%s", m_strCaption);


    int bx = AfxGetApp()->GetProfileInt(m_profile, "bx", 0);
    int by = AfxGetApp()->GetProfileInt(m_profile, "by", 0);
    int cx = AfxGetApp()->GetProfileInt(m_profile, "cx", 0);
    int cy = AfxGetApp()->GetProfileInt(m_profile, "cy", 0);

    if (cx > 0 && cy > 0) {
        SetWindowPos(&wndTopMost, bx, by, cx, cy, SWP_NOZORDER);
    }

    for (int j = 0; j < m_cols.GetSize(); j++) {
        CString s;
        s.Format("col%d", j);
        int w = AfxGetApp()->GetProfileInt(m_profile, s, m_List.GetColumnWidth(j));
        m_List.SetColumnWidth(j, w);
    }

    return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CMMenu2::AddColumn(const char* s)
{
    m_width.Add(-1);
    m_cols.Add(s);
    m_row.cols.resize(m_cols.GetSize());
}

void CMMenu2::setWidth(int n, int w)
{
    n = n - 1;

    if (n < 0) return;
    if (n >= m_cols.GetSize()) return;
    m_width[n] = w;
}


void CMMenu2::Reset()
{
    m_rows.RemoveAll();
    m_List.DeleteAllItems();
}

void CMMenu2::Add(int col, const char* s)
{
    //  m_cols.Add(s);
    m_row.cols[col - 1] = s;
}

void CMMenu2::Add(void* v)
{
    m_row.v = v;
    m_rows.Add(m_row);
}

void CMMenu2::init()
{
    int i, j;
    CString str, pic;

    m_rows2.RemoveAll();

    UpdateData(TRUE);

    pic = m_edit;;
    pic.MakeUpper();

    m_List.DeleteAllItems();

    int ii = 0;

    for (i = 0; i < m_rows.GetSize(); i++) {
        CMenuRow r = m_rows[i];

        CString str = r.cols[0];
        str.MakeUpper();

        if (strstr(str, pic)) {
            m_rows2.Add(r);

            m_List.InsertItem(ii, r.cols[0]);
            for (j = 1; j < m_cols.GetSize(); j++) {
                m_List.SetItem(ii, j, LVIF_TEXT, r.cols[j], 0, 0, 0, 0);
            }
            ii++;
        }
    }
}

void CMMenu2::OnChangeEdit()
{
    init();
}


int getCurItem(CListCtrl& lst)
{
    POSITION p = lst.GetFirstSelectedItemPosition();
    if (p) {
        return lst.GetNextSelectedItem(p);
    }

    return -1;
}

void CMMenu2::OnOK()
{
    int nItem = getCurItem(m_List);
    if (nItem >= 0) {
        m_row = m_rows2[nItem];
        m_strText = m_List.GetItemText(nItem, 0);
        CDialog::OnOK();
    }
}

void CMMenu2::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pia = (LPNMITEMACTIVATE)pNMHDR;
    CPoint point(pia->ptAction);

    //    UINT uFlags;
        //  int nItem = m_List.HitTest(point, &uFlags);
    int nItem = pia->iItem;

    *pResult = 0;

    if (nItem >= 0) {
        m_row = m_rows2[nItem];
        m_strText = m_List.GetItemText(nItem, 0);
        EndDialog(IDOK);
    }
}

BOOL CMMenu2::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UINT nID = LOWORD(wParam);
    UINT msg = HIWORD(wParam); // == EN_CHANGE

    switch (msg) {
    case BN_CLICKED:
        //      OnBnClicked(control);  
        if (nID != IDOK && nID != IDCANCEL) {
            int nItem = getCurItem(m_List);
            if (nItem >= 0) {
                map<UINT, FUN>::iterator it = m_map_proc.find(nID);
                if (it != m_map_proc.end()) {
                    FUN f = it->second;
                    f.f(this, m_rows2[nItem], f.p1, f.p2, f.p3, f.p4);
                }
            }
        }
        break;
    }


    if (nID == IDC_DEL && m_fun_del) {

        int nItem = getCurItem(m_List);
        if (nItem >= 0) {
            m_row = m_rows2[nItem];

            CString str = m_row.cols[0];

            if (m_fun_del((int)m_row.v, str)) {
                for (int i = 0; i < m_rows.GetSize(); i++) {
                    CMenuRow r = m_rows[i];
                    if (m_row.v == r.v) {
                        m_rows.RemoveAt(i);
                        break;
                    }
                }

                init();
            }
        }
    }
    else if (nID == IDC_DEL) {
        int nItem = getCurItem(m_List);
        if (nItem >= 0) {
            m_row = m_rows2[nItem];
            m_strText = m_List.GetItemText(nItem, 0);
        }
        EndDialog(nID);
    }

    return CDialog::OnCommand(wParam, lParam);
}

void CMMenu2::AddProc(UINT nID, fun proc, void* pp1, void* pp2, void* pp3, void* pp4)
{
    m_map_proc[nID].f = proc;
    m_map_proc[nID].p1 = pp1;
    m_map_proc[nID].p2 = pp2;
    m_map_proc[nID].p3 = pp3;
    m_map_proc[nID].p4 = pp4;
}

void ButtonsMove(CWnd* wnd, int cx, int cy, int width, int off)
{
    CWnd* c = wnd->GetWindow(GW_CHILD);

    while (c) {
        // Get the class name for the window
        TCHAR szClassName[100];
        GetClassName(c->GetSafeHwnd(), szClassName, 100);

        if (_tcscmp(_T("Button"), szClassName) == 0)
        {
            //      CButton* pButton = (CButton*)c;
            WINDOWPLACEMENT wpi;
            c->GetWindowPlacement(&wpi);

            wpi.rcNormalPosition.right = cx - off;
            wpi.rcNormalPosition.left = wpi.rcNormalPosition.right - width;

            c->SetWindowPlacement(&wpi);
        }

        c = c->GetWindow(GW_HWNDNEXT);
    }
}

void CMMenu2::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    CRect sz;
    GetClientRect(sz);

    sz.DeflateRect(5, 5);

    ButtonsMove(this, cx, cy, 90 * getScreenMas(), 5 * getScreenMas());

    sz.top = 40 * getScreenMas();
    sz.right -= 100 * getScreenMas();

    if (::IsWindow(m_List.m_hWnd)) m_List.MoveWindow(sz);
}




void* CMMenu2::getV()
{
    return m_row.v;
}

void CMMenu2::OnDestroy()
{
    WINDOWPLACEMENT wpi;
    GetWindowPlacement(&wpi);

    AfxGetApp()->WriteProfileInt(m_profile, "bx", wpi.rcNormalPosition.left);
    AfxGetApp()->WriteProfileInt(m_profile, "by", wpi.rcNormalPosition.top);
    AfxGetApp()->WriteProfileInt(m_profile, "cx", wpi.rcNormalPosition.right - wpi.rcNormalPosition.left);
    AfxGetApp()->WriteProfileInt(m_profile, "cy", wpi.rcNormalPosition.bottom - wpi.rcNormalPosition.top);


    for (int j = 0; j < m_cols.GetSize(); j++) {
        int w = m_List.GetColumnWidth(j);
        CString s;
        s.Format("col%d", j);
        AfxGetApp()->WriteProfileInt(m_profile, s, w);
    }

    CDialog::OnDestroy();
}

void CMMenu2::setDelFun(fun_del fun_proc)
{
    m_fun_del = fun_proc;
}

