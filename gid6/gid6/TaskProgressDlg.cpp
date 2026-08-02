// TaskProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "TaskProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTaskProgressDialog dialog

//CTaskProgressDialog::CTaskProgressDialog(UINT nCaptionID)
CTaskProgressDialog::CTaskProgressDialog(const char *capt)
{
//  m_nCaptionID = CG_IDS_PROGRESS_CAPTION;
//  if (nCaptionID != 0)
//    m_nCaptionID = nCaptionID;
    m_capt = "";
    if (capt) m_capt = capt;

    m_bCancel   =FALSE;
    m_nLower    =0;
    m_nUpper    =100;
    m_nStep     =10;
    //{{AFX_DATA_INIT(CTaskProgressDialog)
  //}}AFX_DATA_INIT
  m_bSelMode      = TRUE;
    m_bParentDisabled = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
CTaskProgressDialog::~CTaskProgressDialog()
{
    if(m_hWnd!=NULL)
      DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
//
BOOL CTaskProgressDialog::DestroyWindow()
{
    ReEnableParent();
    return CDialog::DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
//
void CTaskProgressDialog::ReEnableParent()
{
    if(m_bParentDisabled && (m_pParentWnd!=NULL))
      m_pParentWnd->EnableWindow(TRUE);
    m_bParentDisabled=FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
BOOL CTaskProgressDialog::Create(CWnd *pParent)
{
    // Get the true parent of the dialog
    m_pParentWnd = CWnd::GetSafeOwner(pParent);

    // m_bParentDisabled is used to re-enable the parent window
    // when the dialog is destroyed. So we don't want to set
    // it to TRUE unless the parent was already enabled.

    if((m_pParentWnd!=NULL) && m_pParentWnd->IsWindowEnabled())
    {
      m_pParentWnd->EnableWindow(FALSE);
      m_bParentDisabled = TRUE;
    }

    if(!CDialog::Create(CTaskProgressDialog::IDD,pParent))
    {
      ReEnableParent();
      return FALSE;
    }

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
void CTaskProgressDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CTaskProgressDialog)
    DDX_Text(pDX, IDC_TEXT, m_text);
    DDX_Control(pDX, CG_IDC_PROGDLG_PROGRESS, m_Progress);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTaskProgressDialog, CDialog)
    //{{AFX_MSG_MAP(CTaskProgressDialog)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CTaskProgressDialog::SetStatus(LPCTSTR lpszMessage)
{
    ASSERT(m_hWnd); // Don't call this _before_ the dialog has
                    // been created. Can be called from OnInitDialog
    CWnd *pWndStatus = GetDlgItem(CG_IDC_PROGDLG_STATUS);

    // Verify that the static text control exists
    ASSERT(pWndStatus!=NULL);
    pWndStatus->SetWindowText(lpszMessage);
}

/////////////////////////////////////////////////////////////////////////////
//
void CTaskProgressDialog::OnCancel()
{
    m_bCancel=TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
void CTaskProgressDialog::SetRange(int nLower,int nUpper)
{
    m_nLower = nLower;
    m_nUpper = nUpper;
    m_Progress.SetRange(nLower,nUpper);
}
  
/////////////////////////////////////////////////////////////////////////////
//
int CTaskProgressDialog::SetPos(int nPos)
{
    PumpMessages();
    int iResult = m_Progress.SetPos(nPos);
    UpdatePercent(nPos);
    return iResult;
}

/////////////////////////////////////////////////////////////////////////////
//
int CTaskProgressDialog::SetStep(int nStep)
{
    m_nStep = nStep; // Store for later use in calculating percentage
    return m_Progress.SetStep(nStep);
}

int CTaskProgressDialog::OffsetPos(int nPos)
{
    PumpMessages();
    int iResult = m_Progress.OffsetPos(nPos);
    UpdatePercent(iResult+nPos);
    return iResult;
}

/////////////////////////////////////////////////////////////////////////////
//
int CTaskProgressDialog::StepIt()
{
    PumpMessages();
    int iResult = m_Progress.StepIt();
    UpdatePercent(iResult+m_nStep);
    return iResult;
}

/////////////////////////////////////////////////////////////////////////////
//
void CTaskProgressDialog::PumpMessages()
{
    // Must call Create() before using the dialog
    ASSERT(m_hWnd!=NULL);

    MSG msg;
    // Handle dialog messages
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if(!IsDialogMessage(&msg))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);  
      }
    }
}

/////////////////////////////////////////////////////////////////////////////
//
BOOL CTaskProgressDialog::CheckCancelButton()
{
    // Process all pending messages
    PumpMessages();

    // Reset m_bCancel to FALSE so that
    // CheckCancelButton returns FALSE until the user
    // clicks Cancel again. This will allow you to call
    // CheckCancelButton and still continue the operation.
    // If m_bCancel stayed TRUE, then the next call to
    // CheckCancelButton would always return TRUE

    BOOL bResult = m_bCancel;
    m_bCancel = FALSE;

    return bResult;
}

/////////////////////////////////////////////////////////////////////////////
//
void CTaskProgressDialog::UpdatePercent(int nNewPos)
{
    CWnd *pWndPercent = GetDlgItem(CG_IDC_PROGDLG_PERCENT);
    int nPercent;
    
    int nDivisor = m_nUpper - m_nLower;
    ASSERT(nDivisor>0);  // m_nLower should be smaller than m_nUpper

    int nDividend = (nNewPos - m_nLower);
    ASSERT(nDividend>=0);   // Current position should be greater than m_nLower

    nPercent = nDividend * 100 / nDivisor;

    // Since the Progress Control wraps, we will wrap the percentage
    // along with it. However, don't reset 100% back to 0%
    if(nPercent!=100)
      nPercent %= 100;

    // Display the percentage
    CString strBuf;
    strBuf.Format(_T("%d%c"),nPercent,_T('%'));

  CString strCur; // get current percentage
    pWndPercent->GetWindowText(strCur);

  if (strCur != strBuf)
    pWndPercent->SetWindowText(strBuf);
}
    
/////////////////////////////////////////////////////////////////////////////
// CTaskProgressDialog message handlers

BOOL CTaskProgressDialog::OnInitDialog() 
{
    CDialog::OnInitDialog();

    m_Progress.SetRange(m_nLower,m_nUpper);
    m_Progress.SetStep(m_nStep);
    m_Progress.SetPos(m_nLower);

    if (m_capt != "") SetWindowText(m_capt);

    UpdateData(FALSE);
      return TRUE;  
}


/////////////////////////////////////////////////////////////////////////////
//
void CTaskProgressDialog::SetSelectMode(BOOL bSel /* = TRUE */)
{
  m_bSelMode = bSel;
}

/////////////////////////////////////////////////////////////////////////////
//
BOOL CTaskProgressDialog::IsSelectMode() const
{
  return (m_bSelMode);
}


Percent2::Percent2(const char *capt) 
{
  pp = -1;
  pm = new CTaskProgressDialog(capt);
  if (pm != NULL) {
    BOOL ret = pm->Create(AfxGetMainWnd());
    if (!ret) {
      delete pm;
      pm = 0;
    }
  }
}

Percent2::~Percent2() {
  if (pm) {
      delete pm;
  }
}


void Percent2::SetText(const CString &text) 
{
    pm->SetText(text);
}


bool Percent2::SetValue(int p) 
{
  if (pm) {
    if (p != pp) {
      pm->SetPos(p);
      pp = p;

      return pm->CheckCancelButton();
    }
  }
  return false;
}


void CTaskProgressDialog::SetText(const CString &text)
{
    m_text = text;

    UpdateData(FALSE);
}
