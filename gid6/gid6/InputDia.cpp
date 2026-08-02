// InputDia.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "InputDia.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputDialog dialog


CInputDialog::CInputDialog(CWnd* pParent, CString capt, CString pr, CString& inp, UINT nIDTemplate)
  : CDialog(nIDTemplate, pParent)
{
  //{{AFX_DATA_INIT(CInputDialog)
  m_strPrompt = pr;
  m_strInput = inp;
  m_strCaption = capt;
  //}}AFX_DATA_INIT
}


void CInputDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CInputDialog)
  DDX_Text(pDX, IDC_PROMPT, m_strPrompt);
  DDX_Text(pDX, IDC_INPUT, m_strInput);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputDialog, CDialog)
  //{{AFX_MSG_MAP(CInputDialog)
  ON_WM_CLOSE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputDialog message handlers

BOOL CInputDialog::OnInitDialog()
{
  CDialog::OnInitDialog();

  // TODO: Add extra initialization here
  CenterWindow();
  SetWindowText(m_strCaption);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}


CString GetPassword() 
{
  CString str = "";

  CInputDialog inD(AfxGetApp()->GetMainWnd(), "", _TR("¬ведите пароль"), str, IDD_PASSWORDDIALOG);
  if (inD.DoModal() == IDOK) {
    return inD.m_strInput;
  }
  return "";
}

double GetMas(double mas) 
{
  CString str;

  str.Format("%g", mas);

  CInputDialog inD(AfxGetApp()->GetMainWnd(), "", _TR("¬ведите масштаб"), str);

  if (inD.DoModal() == IDOK) {
    str = inD.m_strInput;
    if (str[0]) {
      double m;
      if (sscanf(str, "%lg", &m)) return m;
    }
  }
  return mas;
}
