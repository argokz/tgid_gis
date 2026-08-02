// ADO SampleView.cpp : implementation of the CFormDialog class
//

#include "stdafx.h"

#include "FormDialog.h"
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

static int IDLIST = 23422;

/////////////////////////////////////////////////////////////////////////////
// CFormDialog

//IMPLEMENT_DYNCREATE(CFormDialog, CDialog)

BEGIN_MESSAGE_MAP(CFormDialog, CDialog)
    //{{AFX_MSG_MAP(CFormDialog)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_WM_SETFOCUS()
    ON_WM_ERASEBKGND()

    ON_COMMAND(ID_SHIFT_F1, OnShiftF1)
    //}}AFX_MSG_MAP
    // Standard printing commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormDialog construction/destruction

CFormDialog::CFormDialog(CWnd *pParent, const char *cap)
    : CDialog(CFormDialog::IDD, pParent)
{
    m_cap = cap;

    m_profile.Format("FormDialog\\%s", m_cap);

}

CFormDialog::~CFormDialog()
{
}


/////////////////////////////////////////////////////////////////////////////
// CFormDialog message handlers

int CFormDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDialog::OnCreate(lpCreateStruct) == -1)
        return -1;

    // TODO: Add your specialized creation code here
    m_grid.CreateGrid(WS_CHILD | WS_VISIBLE, CRect(0, 0, 2000, 1000), this, IDLIST);

    return 0;
}

void CFormDialog::OnSize(UINT nType, int cx, int cy)
{
  CDialog::OnSize(nType, cx, cy);

  if (!m_grid.m_hWnd) return;

  if (cx > 0 && cy > 0)  {
    CRect rect;
    int id_grid = m_grid.GetDlgCtrlID();

    HDWP hDwp = BeginDeferWindowPos(1);

    hDwp = DeferWindowPos(hDwp, m_grid.m_hWnd, NULL, 0, 0, cx, cy-40, SWP_NOZORDER | SWP_NOZORDER);

    for (CWnd *wnd = GetTopWindow(); wnd; wnd = wnd->GetNextWindow()) {
        CString ss;
        wnd->GetWindowText(ss);

        int id = wnd->GetDlgCtrlID();
        if (id != id_grid) {
            wnd->GetWindowRect(rect);
            ScreenToClient(&rect);
            hDwp = DeferWindowPos(hDwp, wnd->m_hWnd, NULL, rect.left, cy - rect.Height() - 5, -1, -1, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
        }
    }
    EndDeferWindowPos(hDwp);
  }
}

void CFormDialog::OnSetFocus(CWnd* pOldWnd)
{
    CDialog::OnSetFocus(pOldWnd);

    // TODO: Add your message handler code here
    m_grid.SetFocus();
}

BOOL CFormDialog::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default
//    return TRUE;
    return CDialog::OnEraseBkgnd(pDC);
}



void setView(CView *pView0);

void CFormDialog::OnShiftF1()
{
    int col = m_grid.GetCurrentCol();
    long row = m_grid.GetCurrentRow();

    CString s;
    s.Format("%d %d", row, col);
    AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);
}

BOOL CFormDialog::OnInitDialog()
{
    UpdateData(FALSE);

    m_grid.init();


    int bx = AfxGetApp()->GetProfileInt(m_profile, "bx", 0);
    int by = AfxGetApp()->GetProfileInt(m_profile, "by", 0);
    int cx = AfxGetApp()->GetProfileInt(m_profile, "cx", 0);
    int cy = AfxGetApp()->GetProfileInt(m_profile, "cy", 0);

    if (cx > 0 && cy > 0) {
        SetWindowPos(&wndTopMost, bx, by, cx, cy, SWP_NOZORDER);
    }
    SetWindowText(m_cap);
    return TRUE;
}

void CFormDialog::OnDestroy()
{
    WINDOWPLACEMENT wpi;
    GetWindowPlacement(&wpi);

    AfxGetApp()->WriteProfileInt(m_profile, "bx", wpi.rcNormalPosition.left);
    AfxGetApp()->WriteProfileInt(m_profile, "by", wpi.rcNormalPosition.top);
    AfxGetApp()->WriteProfileInt(m_profile, "cx", wpi.rcNormalPosition.right - wpi.rcNormalPosition.left);
    AfxGetApp()->WriteProfileInt(m_profile, "cy", wpi.rcNormalPosition.bottom - wpi.rcNormalPosition.top);


    CDialog::OnDestroy();
    //    if (!m_modal)
    //      delete this; // Shown as non-modal, we'll clean up ourselves
}

void CFormDialog::addRow(const char *name, const char *label, const char *razmer)
{
  FormRow row;
  row.name = name;
  row.label = label;
  row.razmer = razmer;

  m_grid.m_row.push_back(row);
}


void CFormDialog::OnOK() 
{
  vector<FormRow>::const_iterator it = m_grid.m_row.begin();

  CString s = "";

  for (; it != m_grid.m_row.end(); it++) {
    FormRow r = *it;
    if (!r.m_val[0].IsEmpty()) {
      CString ss;
      ss.Format("(%s >= %s)", r.name, r.m_val[0]);
      if (s != "") s += " AND ";
      s += ss;
    }
    if (!r.m_val[1].IsEmpty()) {
      CString ss;
      ss.Format("(%s <= %s)", r.name, r.m_val[1]);
      if (s != "") s += " AND ";
      s += ss;
    }
  }
//  AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);
  m_q = s;

  CDialog::OnOK();
}
