// Uf.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "Uf.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUf dialog

double get_uf(double t1, double t2, double t3)
{
    if (t3 != t2) {
      return (t1-t3)/(t3-t2);
    }
    return -1;
}

CString get_uf_text(double t1, double t2, double t3)
{
    CString s;

    if (t3 != t2) {
      double uf = (t1-t3)/(t3-t2);
      s.Format("%.3f", uf);
    }
    else {
      s.Format(_TR("Îøèáêà"));
    }
    return s;
}


CUf::CUf(CWnd* pParent, double t1, double t2, double t3)
  : CDialog(CUf::IDD, pParent)
{
  //{{AFX_DATA_INIT(CUf)
  m_g = 0.0;
  m_g0 = 0.0;
  m_m1 = 0;
  m_t1 = t1; // 150.0;
  m_t2 = t2; // 70.0;
  m_t3 = t3; // 95.0;
  //}}AFX_DATA_INIT

  m_uf = get_uf_text(t1, t2, t3);

  m_u = get_uf(t1, t2, t3);
}


void CUf::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CUf)
  DDX_Text(pDX, IDC_G, m_g);
  DDX_Text(pDX, IDC_G0, m_g0);
  DDX_Radio(pDX, IDC_M1, m_m1);
  DDX_Text(pDX, IDC_T1, m_t1);
  DDX_Text(pDX, IDC_T2, m_t2);
  DDX_Text(pDX, IDC_T3, m_t3);
  DDX_Text(pDX, IDC_UF, m_uf);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUf, CDialog)
  //{{AFX_MSG_MAP(CUf)
  ON_EN_CHANGE(IDC_T1, OnChange)
  ON_BN_CLICKED(IDC_M1, OnM1)
  ON_EN_CHANGE(IDC_T2, OnChange)
  ON_EN_CHANGE(IDC_T3, OnChange)
  ON_EN_CHANGE(IDC_G, OnChange)
  ON_EN_CHANGE(IDC_G0, OnChange)
  ON_BN_CLICKED(IDC_M2, OnM1)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUf message handlers



void CUf::OnChange() 
{
  UpdateData(TRUE);

  double uf;

  if (m_m1 == 0) {
    if (m_t3 != m_t2) {
      m_u = uf = (m_t1-m_t3)/(m_t3-m_t2);
//      m_uf.Format("%.1f", uf);
      m_uf.Format("%.3f", uf);
    }
    else {
      m_uf.Format(_TR("Îøèáêà"));
      m_u = -1;
    }
  }
  else {
    if (m_g0 != 0) {
      m_u = uf = m_g/m_g0;
//      m_uf.Format("%.1f", uf);
      m_uf.Format("%.3f", uf);
    }
    else {
      m_uf.Format(_TR("Îøèáêà"));
      m_u = -1;
    }
  }
  UpdateData(FALSE);
}

/**
double get_uf()
{
  CUf uf;

  if (uf.DoModal() == IDOK) {
    return uf.m_u;
  }
  return -1.;
}
*/

void CUf::HideShow() 
{
  UpdateData(TRUE);
 
  GetDlgItem(IDC_T1)->EnableWindow(m_m1 == 0);
  GetDlgItem(IDC_T2)->EnableWindow(m_m1 == 0);
  GetDlgItem(IDC_T3)->EnableWindow(m_m1 == 0);

  GetDlgItem(IDC_G)->EnableWindow(m_m1 != 0);
  GetDlgItem(IDC_G0)->EnableWindow(m_m1 != 0);
}

void CUf::OnM1() 
{
  HideShow();
  OnChange();
}


BOOL CUf::OnInitDialog() 
{
  CDialog::OnInitDialog();
  
  // TODO: Add extra initialization here

  HideShow();
  
  return TRUE;  // return TRUE unless you set the focus to a control
              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUf::OnOK() 
{
    UpdateData(TRUE);
    m_u = atof(m_uf);
    CDialog::OnOK();
}
