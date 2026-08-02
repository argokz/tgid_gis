// InputDia.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "InputDialogDate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputDialogDate dialog


CInputDialogDate::CInputDialogDate(CWnd* pParent, CString capt, CString pr, UINT nIDTemplate)
  : CDialog(nIDTemplate, pParent)
{
  //{{AFX_DATA_INIT(CInputDialogDate)
  m_strPrompt = pr;
//  m_strInput = inp;
  m_strCaption = capt;
  //}}AFX_DATA_INIT
}


void CInputDialogDate::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CInputDialogDate)
  DDX_Text(pDX, IDC_PROMPT, m_strPrompt);
  DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_date1);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputDialogDate, CDialog)
  //{{AFX_MSG_MAP(CInputDialogDate)
  ON_WM_CLOSE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputDialogDate message handlers

BOOL CInputDialogDate::OnInitDialog()
{
  CDialog::OnInitDialog();

  CenterWindow();
  SetWindowText(m_strCaption);

  m_date1 = CTime::GetCurrentTime();

  UpdateData(FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}
