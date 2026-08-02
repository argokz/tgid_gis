// LineStyle.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "LineStyle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern GID_COLORS m_colors;

/////////////////////////////////////////////////////////////////////////////
// CLineStyle dialog


CLineStyle::CLineStyle(CWnd* pParent, int regim)
  : CDialog(CLineStyle::IDD, pParent)
{
  //{{AFX_DATA_INIT(CLineStyle)
  m_mag_ras = 1;
  m_open_close = 0;
  m_thickness = 0.0;
  m_distance = 0.0;
  m_typ = 0;
  m_style = FALSE;
  m_ring = 1.0;
  //}}AFX_DATA_INIT

  m_color_uzel = WHITE;
  m_regim = regim;

  is_init = 0;
}


void CLineStyle::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CLineStyle)
  DDX_Control(pDX, IDC_COLORS_WINDOW, m_w);
  DDX_CBIndex(pDX, IDC_MAG_RAS, m_mag_ras);
  DDX_CBIndex(pDX, IDC_OPEN, m_open_close);
  DDX_Text(pDX, IDC_LINE_THICKNESS, m_thickness);
  DDV_MinMaxDouble(pDX, m_thickness, 0., 100.);
  DDX_Text(pDX, IDC_DISTANCE, m_distance);
  DDV_MinMaxDouble(pDX, m_distance, 0., 100.);
  DDX_CBIndex(pDX, IDC_TYP, m_typ);
  DDX_Check(pDX, IDC_PUNKT, m_style);
  DDX_Text(pDX, IDC_RING, m_ring);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLineStyle, CDialog)
  //{{AFX_MSG_MAP(CLineStyle)
  ON_BN_CLICKED(IDC_OBR, OnObr)
  ON_BN_CLICKED(IDC_POD, OnPod)
  ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
  ON_BN_CLICKED(IDC_DEFAULT2, OnDefaultPS)
  ON_BN_CLICKED(IDC_PODF, OnPodf)
  ON_BN_CLICKED(IDC_OBRF, OnObrf)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLineStyle message handlers

COLORREF rgb[] = {
  0x000000, 0x7F0000, 0x007F00, 0x7F7F00, 0x00007F, 0x7F007F, 0x007F7F,
  0xBFBFBF, 0x7F7F7F, 0xFF0000, 0x00FF00, 0xFFFF00, 0x0000FF, 0xFF00FF,
  0x00FFFF, 0xFFFFFF,
};

void SetColor(unsigned long & color)
{
  CColorDialog dlgColor(color);
  dlgColor.m_cc.lpCustColors = rgb;

  if (dlgColor.DoModal() == IDOK)
  {
    color = dlgColor.GetColor();
  }
}

void CLineStyle::OnOK() 
{
  CString str;

  UpdateData(TRUE);
  
  str.Format("%f", m_ring);

  AfxGetApp()->WriteProfileString(szSection, "ring", str);
  AfxGetApp()->WriteProfileInt(szSection, "color_uzel", m_color_uzel);
  
  m_colors.save(m_regim);

  CDialog::OnOK();
}

void CLineStyle::OnCancel() 
{
  // TODO: Add extra cleanup here
  
  CDialog::OnCancel();
}

void CLineStyle::OnObr() 
{
  // TODO: Add your control notification handler code here
}

void CLineStyle::OnPod() 
{
}

void CLineStyle::set_data()
{
//  m_colors.style(m_mag_ras, m_typ).m_distance = m_distance;
//  m_colors.style(m_mag_ras, m_typ).m_thickness = m_thickness;
//  m_colors.style(m_mag_ras, m_typ).m_style = !m_style;

  int i;

//  for (i = 0; i < 4; i++) {
//    m_colors.style(m_mag_ras, i).m_distance = m_distance;
//    m_colors.style(m_mag_ras, i).m_thickness = m_thickness;
//  }

    m_colors.style(m_mag_ras, m_typ).m_distance = m_distance;
    m_colors.style(m_mag_ras, m_typ).m_thickness = m_thickness;
  
  
  for (i = 0; i < 2; i++) {
    m_colors.style(i, m_typ).m_style = !m_style;
  }

  if (m_w) {
    m_w.set_colors(m_mag_ras, m_open_close, m_typ);
  }
}

void CLineStyle::init_data()
{
//  m_thickness = m_colors.style(m_mag_ras, m_typ).m_thickness;
//  m_distance = m_colors.style(m_mag_ras, m_typ).m_distance;
//  m_style = !m_colors.style(m_mag_ras, m_typ).m_style;

//  m_thickness = m_colors.style(m_mag_ras, 0).m_thickness;
//  m_distance = m_colors.style(m_mag_ras, 0).m_distance;

  m_thickness = m_colors.style(m_mag_ras, m_typ).m_thickness;
  m_distance = m_colors.style(m_mag_ras, m_typ).m_distance;
  
  
  m_style = !m_colors.style(0, m_typ).m_style;
  
  CString str = AfxGetApp()->GetProfileString(szSection, "ring", "1");
  m_ring = atof(str);
  if (m_w) m_w.set_ring(m_ring);

}

BOOL CLineStyle::OnInitDialog() 
{
  CDialog::OnInitDialog();

  m_w.set_colors(m_mag_ras, m_open_close, m_typ);
  m_w.set_color_uzel(m_color_uzel);

  init_data();

  m_colors.load(m_regim);

  UpdateData(FALSE);

  is_init = 1;
  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CLineStyle::OnCommand(WPARAM wParam, LPARAM lParam) 
{
//  UpdateData(TRUE);

  UINT nID = LOWORD(wParam);
  if (is_init) {
    is_init  = 0;

    switch (nID) {
    case IDC_POD     : 
      UpdateData(TRUE);
      SetColor(m_colors.pod(m_open_close, m_mag_ras, m_typ)); 
      if (m_w) m_w.set_colors(m_mag_ras, m_open_close, m_typ);
      break;
    case IDC_OBR     : 
      UpdateData(TRUE);
      SetColor(m_colors.obr(m_open_close, m_mag_ras, m_typ)); 
      if (m_w) m_w.set_colors(m_mag_ras, m_open_close, m_typ);
      break;
    case IDC_PODF    : 
      UpdateData(TRUE);
      SetColor(m_colors.pod(3, m_mag_ras, m_typ)); 
      if (m_w) m_w.set_colors(m_mag_ras, m_open_close, m_typ);
      break;
    case IDC_OBRF    : 
      UpdateData(TRUE);
      SetColor(m_colors.obr(3, m_mag_ras, m_typ)); 
      if (m_w) m_w.set_colors(m_mag_ras, m_open_close, m_typ);
      break;
    case IDC_COLOR_UZEL :
      UpdateData(TRUE);
      SetColor(m_color_uzel); 
      if (m_w) m_w.set_color_uzel(m_color_uzel);
      break;
    case IDC_RING    : 
      UpdateData(TRUE);
//      SetColor(m_colors.obr(3, m_typ)); 
      if (m_w) m_w.set_ring(m_ring);
      break;
    }

   switch (nID) {
      case IDC_DISTANCE:
      case IDC_LINE_THICKNESS :
      case IDC_PUNKT :
        UpdateData(TRUE);
        set_data();
        if (m_w) m_w.set_colors(m_mag_ras, m_open_close, m_typ);
        break;
    }

    switch (nID) {
      case IDC_MAG_RAS:
      case IDC_TYP:
      case IDC_OPEN:        
        UpdateData(TRUE);
        if (m_w) m_w.set_colors(m_mag_ras, m_open_close, m_typ);
        init_data();
        UpdateData(FALSE);
        break;
      }
    is_init  = 1;
  }

  return CDialog::OnCommand(wParam, lParam);
}

void CLineStyle::OnDefault() 
{
  m_colors.set_def();
  init_data();

  is_init = 0;
  UpdateData(FALSE);
  is_init = 1;

  if (m_w) {
    m_w.set_colors(m_mag_ras, m_open_close, m_typ);
  }
}

void CLineStyle::OnDefaultPS() 
{
  m_colors.set_defPS();
  init_data();

  is_init = 0;
  UpdateData(FALSE);
  is_init = 1;

  if (m_w) {
    m_w.set_colors(m_mag_ras, m_open_close, m_typ);
  }
}


void CLineStyle::OnPodf() 
{
  // TODO: Add your control notification handler code here
  
}

void CLineStyle::OnObrf() 
{
  // TODO: Add your control notification handler code here
  
}
