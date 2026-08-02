// Param.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "win.h"
#include "Param.h"
#include "Param2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CParam2 dialog

CParam2::CParam2(CWnd* pParent)
  : CDialog(CParam2::IDD, pParent)
{
  //{{AFX_DATA_INIT(CParam2)
  m_char_sety = FALSE;
  m_Tn = 0.0;
  m_nGWS = -1;
  m_teplopoter = FALSE;
  m_teplovyd = FALSE;
  m_ur = FALSE;
  m_nGWS2 = -1;
  m_avtomat = FALSE;
  m_veter = FALSE;
  m_mag_fn = "";
  m_detaliz = 1;
  m_save_po = FALSE;
  m_leto = FALSE;
  //}}AFX_DATA_INIT

  CString strRet = COleDateTime::GetCurrentTime().Format(_T("%d.%m.%Y %H:%M"));

  m_name.Format("Расчет фактического режима %s", strRet);

}



void CParam2::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CParam2)
  DDX_Text(pDX, IDC_NAME, m_name);
  DDX_Control(pDX, IDCANCEL, m_idcancel);
  DDX_Control(pDX, IDOK, m_idok);
  DDX_Check(pDX, IDC_CHAR_SETY, m_char_sety);
  DDX_Text(pDX, IDC_TN, m_Tn);
  DDX_CBIndex(pDX, IDC_GWS, m_nGWS);
  DDX_CBIndex(pDX, IDC_GWS2, m_nGWS2);
  DDX_Check(pDX, IDC_VETER, m_veter);
  DDX_Text(pDX, IDC_MAG, m_mag_fn);
  DDX_Radio(pDX, IDC_DETALIZ, m_detaliz);
  DDX_Check(pDX, IDC_SAVE_PO, m_save_po);
  DDX_Check(pDX, IDC_LETO, m_leto);
  DDX_Check(pDX, IDC_AVTOMAT, m_avtomat);
  DDX_Check(pDX, IDC_KV, m_kv);

  DDX_Check(pDX, IDC_MAG_FRAGMENT, m_mag_fragment);
  
  //}}AFX_DATA_MAP
}

//  DDX_Check(pDX, IDC_TEPLOPOTER, m_teplopoter);
//  DDX_Check(pDX, IDC_TEPLOVYD, m_teplovyd);
//  DDX_Check(pDX, IDC_UR, m_ur);


BEGIN_MESSAGE_MAP(CParam2, CDialog)
  //{{AFX_MSG_MAP(CParam2)
  ON_BN_CLICKED(IDC_MAG_BUT, OnMagBut)
  ON_BN_CLICKED(IDC_LETO, OnLeto)
  ON_BN_CLICKED(IDC_T_IST, OnTIst)
  //}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_MAG_FRAGMENT, &CParam2::OnBnClickedMagFragment)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CParam2 message handlers


void CParam2::set_par()
{
//  UpdateData(TRUE);
//  GetDlgItem(IDC_TEPLOPOTER)->EnableWindow(!m_ur);
//  GetDlgItem(IDC_TEPLOVYD)->EnableWindow(!m_ur);
}


BOOL CParam2::OnInitDialog() 
{
  CDialog::OnInitDialog();
 
  m_nGWS = AfxGetApp()->GetProfileInt(szSection, "GWS", 0);
  m_nGWS2 = AfxGetApp()->GetProfileInt(szSection, "GWS2", 0);
  m_char_sety = AfxGetApp()->GetProfileInt(szSection, "char_sety", 0);
  m_Tn = atof(AfxGetApp()->GetProfileString(szSection, "Tn", "-32"));

  m_leto = AfxGetApp()->GetProfileInt(szSection, "leto2", 0);

  m_teplopoter = AfxGetApp()->GetProfileInt(szSection, "teplopoter", 1);
  m_teplovyd = AfxGetApp()->GetProfileInt(szSection, "teplovyd", 1);
  m_veter = AfxGetApp()->GetProfileInt(szSection, "veter", 0);

  m_detaliz = AfxGetApp()->GetProfileInt(szSection, "detaliz", 1);

  m_mag_fn = AfxGetApp()->GetProfileString(szSection, "mag_fn", "");
//  set_mag_fn(m_mag_fn);

  m_save_po = AfxGetApp()->GetProfileInt(szSection, "save_po", FALSE);

  m_mag_fragment = AfxGetApp()->GetProfileInt(szSection, "mag_fragment", 0);

  m_kv = AfxGetApp()->GetProfileInt(szSection, "kv_ras", TRUE);


//  m_avtomat = AfxGetApp()->GetProfileInt(szSection, "avtomat", 0);
  m_avtomat = 0;


//  GetDlgItem(IDC_SAVE_PO)->ShowWindow(m_mag_fn.IsEmpty() ? SW_HIDE : SW_SHOW);


  m_ur = TRUE;
//  m_kol_iter = 20;

  m_nGWS = 0;
  m_nGWS2 = 0;

  UpdateData(FALSE);
  set_par();

  OnLeto();


  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CParam2::OnOK() 
{
  UpdateData(TRUE);

  CString str;

  str.Format("%lg", m_Tn);
  AfxGetApp()->WriteProfileString(szSection, "Tn", str);


  AfxGetApp()->WriteProfileInt(szSection, "kv_ras", m_kv);

  AfxGetApp()->WriteProfileInt(szSection, "char_sety", m_char_sety);
  AfxGetApp()->WriteProfileInt(szSection, "GWS", m_nGWS);
  AfxGetApp()->WriteProfileInt(szSection, "GWS2", m_nGWS2);

  AfxGetApp()->WriteProfileInt(szSection, "teplopoter", m_teplopoter);
  AfxGetApp()->WriteProfileInt(szSection, "teplovyd", m_teplovyd);
  AfxGetApp()->WriteProfileInt(szSection, "veter", m_veter);

  AfxGetApp()->WriteProfileInt(szSection, "detaliz", m_detaliz);

  AfxGetApp()->WriteProfileString(szSection, "mag_fn", m_mag_fn);
//  set_mag_fn(m_mag_fn);

  AfxGetApp()->WriteProfileInt(szSection, "save_po", m_save_po);

  AfxGetApp()->WriteProfileInt(szSection, "leto2", m_leto);

  AfxGetApp()->WriteProfileInt(szSection, "mag_fragment", m_mag_fragment);

  AfxGetApp()->WriteProfileInt(szSection, "avtomat", m_avtomat);

  CDialog::OnOK();
}

void CParam2::OnObzor() 
{
  CFileDialog fd(TRUE, _T("MDB"), NULL, OFN_HIDEREADONLY,
    _TR("Схема (*.mdb)|*.mdb|"), this);

  if (fd.DoModal() == IDOK) {
//    GetDlgItem(IDC_SAVE_PO)->ShowWindow(m_mag_fn.IsEmpty() ? SW_HIDE : SW_SHOW);

    UpdateData(FALSE);
  }
}
/*
HBRUSH CParam2::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
extern CBrush m_Brush;
extern CBrush m_Brush1;
  pDC->SetBkMode(TRANSPARENT);
  return m_Brush1;
}
*/

BOOL CParam2::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  UINT nID = LOWORD(wParam);
  if ( nID == IDC_UR ) {
    set_par();
  }
  
  return CDialog::OnCommand(wParam, lParam);
}

void CParam2::OnMagBut() 
{
  CFileDialog fd(TRUE, _T("MDB"), NULL, OFN_HIDEREADONLY,
    _TR("Схема (*.mdb)|*.mdb|"), this);

  if (fd.DoModal() == IDOK) {
    m_mag_fn = fd.GetPathName();

    UpdateData(FALSE);
  }
}

void CParam2::OnLeto() 
{
  UpdateData(TRUE);
  if (m_leto) {

    m_detaliz = 1;
    UpdateData(FALSE);
  }

  GetDlgItem(IDC_VETER)->EnableWindow(!m_leto);
  GetDlgItem(IDC_DETALIZ)->EnableWindow(!m_leto);

}

//#include "TIst.h"

void CParam2::OnTIst() 
{
  AfxMessageBox("!!", MB_OK|MB_ICONINFORMATION);
  
//  CTIst tist(this, m_dbf);


//  if (tist.DoModal() == IDOK) {
//    map_t_ist = tist.map_t_ist;
//  }
  
}


void CParam2::OnBnClickedMagFragment()
{
    // TODO: Add your control notification handler code here
}
