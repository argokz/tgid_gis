// Param.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "win.h"
#include "Param.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CParam dialog


CParam::CParam(CWnd* pParent)
  : CDialog(CParam::IDD, pParent)
{
  //{{AFX_DATA_INIT(CParam)
  m_char_sety = FALSE;
  m_dross = TRUE;
  m_teplopoter = FALSE;
  m_teplovyd = FALSE;
  m_ur = FALSE;
  //}}AFX_DATA_INIT

}


void CParam::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CParam)
  DDX_Control(pDX, IDCANCEL, m_idcancel);
  DDX_Control(pDX, IDOK, m_idok);
  DDX_Check(pDX, IDC_CHAR_SETY, m_char_sety);
  DDX_Check(pDX, IDC_DROSS, m_dross);
  DDX_Check(pDX, IDC_AVTOMAT, m_avtomat);
  DDX_Check(pDX, IDC_TEPLOPOTER, m_teplopoter);
  DDX_Check(pDX, IDC_TEPLOVYD, m_teplovyd);
  DDX_Check(pDX, IDC_UR, m_ur);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParam, CDialog)
  //{{AFX_MSG_MAP(CParam)
  ON_BN_CLICKED(IDC_OBZOR, OnObzor)
//  ON_WM_CTLCOLOR()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParam message handlers


void CParam::set_par()
{
  UpdateData(TRUE);
  GetDlgItem(IDC_TEPLOPOTER)->EnableWindow(!m_ur);
  GetDlgItem(IDC_TEPLOVYD)->EnableWindow(!m_ur);
}


BOOL CParam::OnInitDialog() 
{
  CDialog::OnInitDialog();
  
  m_Tn = -32;

  m_ur = TRUE;
  m_teplopoter = TRUE;
  m_teplovyd = TRUE;

  m_Tn = atof(AfxGetApp()->GetProfileString(szSection, "Tn", "-32"));
  m_char_sety = AfxGetApp()->GetProfileInt(szSection, "char_sety", m_char_sety);
  m_dross = AfxGetApp()->GetProfileInt(szSection, "dross", 1);
  m_avtomat = AfxGetApp()->GetProfileInt(szSection, "avtomat", 0);

//  m_fiks = gidr.reg_r ? 1 : 0;
//  m_ekspl = gidr.ident ? 1 : 0;
  m_kol_iter = 20;
//  m_n_fil = gidr.n_file;
//  m_char_sety = gidr.kparm;
//  m_sav_off_sys = gidr.sopr;
//  m_sav_on_sys = gidr.sopr1;

  UpdateData(FALSE);

  set_par();

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CParam::OnOK() 
{
  UpdateData(TRUE);

//  PARM_GRMAIN gidr;
 
//  gidr.reg_r = (m_fiks == 1);
//  gidr.ident = (m_ekspl == 1);
//  gidr.iter = m_kol_iter;
//  m_n_fil.AnsiToOem();
//  lstrcpy(gidr.n_file, m_n_fil);
//  gidr.kparm = m_char_sety;
//  gidr.sopr = m_sav_off_sys;
//  gidr.sopr1 = m_sav_on_sys;

  AfxGetApp()->WriteProfileInt(szSection, "char_sety", m_char_sety);
  AfxGetApp()->WriteProfileInt(szSection, "dross", m_dross);
  AfxGetApp()->WriteProfileInt(szSection, "avtomat", m_avtomat);

  CDialog::OnOK();
}

void CParam::OnObzor() 
{
  CFileDialog fd(TRUE, _T("MDB"), NULL, OFN_HIDEREADONLY,
    _TR("Ñץולא (*.mdb)|*.mdb|"), this);

  if (fd.DoModal() == IDOK) {
    m_n_fil = fd.GetPathName();

    UpdateData(FALSE);
  }
}
/*
HBRUSH CParam::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
extern CBrush m_Brush;
extern CBrush m_Brush1;
  pDC->SetBkMode(TRANSPARENT);
  return m_Brush1;
}
*/

BOOL CParam::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  set_par();
  
  return CDialog::OnCommand(wParam, lParam);
}
