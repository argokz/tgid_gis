// Dialog3.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "Dialog3.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialog3 dialog


CDialog3::CDialog3(CWnd* pParent, int idd, double e1, double e2, double e3)
  : CDialog(idd, pParent)
{
  //{{AFX_DATA_INIT(CDialog3)
  m_e1 = e1;
  m_e2 = e2;
  m_e3 = e3;
  m_sum = (e1+e2+e3)/3;
  //}}AFX_DATA_INIT
}


void CDialog3::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CDialog3)
  DDX_Control(pDX, IDC_EDIT_SUM, m_ctrl_sum);
  DDX_Text(pDX, IDC_EDIT1, m_e1);
  DDX_Text(pDX, IDC_EDIT2, m_e2);
  DDX_Text(pDX, IDC_EDIT3, m_e3);
  DDX_Text(pDX, IDC_EDIT_SUM, m_sum);
  DDX_Control(pDX, IDC_EDIT1, m_ctrl_e1);
  DDX_Control(pDX, IDC_EDIT2, m_ctrl_e2);
  DDX_Control(pDX, IDC_EDIT3, m_ctrl_e3);
  
  
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialog3, CDialog)
  //{{AFX_MSG_MAP(CDialog3)
  ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit)
  ON_EN_CHANGE(IDC_EDIT2, OnChangeEdit)
  ON_EN_CHANGE(IDC_EDIT3, OnChangeEdit)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialog3 message handlers

BOOL CDialog3::OnInitDialog() 
{
  CDialog::OnInitDialog();
  
  // TODO: Add extra initialization here
  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialog3::OnChangeEdit() 
{
  
  UpdateData(TRUE);


  CString s1, s2, s3;

  m_ctrl_e1.GetWindowText(s1);
  m_ctrl_e2.GetWindowText(s2);
  m_ctrl_e3.GetWindowText(s3);

  s1.Replace(",", ".");
  s2.Replace(",", ".");
  s3.Replace(",", ".");

  double e1 = atof(s1);
  double e2 = atof(s2);
  double e3 = atof(s3);

//  m_sum = (m_e1+m_e2+m_e3)/3;

  CString s;

  s.Format("%g", (e1+e2+e3)/3);

  m_ctrl_sum.SetWindowText(s);

//  UpdateData(FALSE);


}

void CDialog3::setMax(double e1, double e2, double e3)
{
    m_max_e1 = e1;
	m_max_e2 = e2;
	m_max_e3 = e3;
}


void CDialog3::OnOK() 
{
  // TODO: Add extra validation here

    UpdateData(TRUE);

    CString s;

    int n = 1;

    if (m_e1 > m_max_e1) {
        s.Format("Вес %d-й пластины не может быть больше %g г", n++, m_max_e1);
        AfxMessageBox(s);
        return;
    }
    if (m_e2 > m_max_e2) {
        s.Format("Вес %d-й пластины не может быть больше %g г", n++, m_max_e2);
        AfxMessageBox(s);
        return;
    }
    if (m_e3 > m_max_e3) {
        s.Format("Вес %d-й пластины не может быть больше %g г", n++, m_max_e3);
        AfxMessageBox(s);
        return;
    }
  
    CDialog::OnOK();
}


