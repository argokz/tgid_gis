// ColorLine.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"

#include "ado.h"


#include "ColorLine.h"


#include "Bgi.h"
#include "win.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorLine dialog

struct UT_FN {
  char *fn;
  double d1, d2;
  char *rus;
  bool out;
};

static UT_FN ut_fn[] = {
{"a18", 6,15, "Располагаемый напор в конечном узле", true},  //  Располагаемый напор в конечном узле участка
{"a17", 0.01,2, "Общие потери напора", true},  //  Общие потери напора на участке
{"a14", 0.1,1, "Удельные линейные потери напора", true},  //  Удельные линейные потери напора на участке
{"a7", 10,100, "Длина,м", true},  //  Длина участка трубопровода
{"a8", 200,400, "Внутренний диаметр,мм", true},  //  Внутренний диаметр трубы участка
{"a9", 0.1,10, "Объем воды,м3", true},  //  Объем воды на участке трубопровода
{"a10", 0.5,2, "Скорость потока", true},  //  Скорость потока сетевой воды
{"t1", 0,70, "Температура на входе", true},
{"a13", 0,10, "Расход сетевой воды", true},  //  Расход сетевой воды на участке
{"a20", 0,360, "Геодезич.отметка в конечном узле", true},  //  Геодезич.отметка местности в конечном узле участка
//{"tzam", 0, 24, ""},  //  
{"tubeRoughness", 0.5, 10, "Шероховатость", false},  //  

};


void CColorLine::init_colors() 
{
  int i;

  for (i = 0; i < 6; i++) {
    m_color[i] = 0;
    m_dd[i] = 0;
  }
  m_color[0] = 255;
  m_color[1] = 128;
  m_color[2] = 6205183;
  m_color[0+3] = 16711680;
  m_color[1+3] = 8388608;
  m_color[2+3] = 16746692;


  m_color[0+3] = m_color[0] = 0x0000FF;
  m_color[1+3] = m_color[1] = 0x00FF00;
  m_color[2+3] = m_color[2] = 0xFF0000;

  m_d1 = ut_fn[0].d1;
  m_d2 = ut_fn[0].d2;
  m_d3 = 0.0;
  m_do1 = ut_fn[0].d1;
  m_do2 = ut_fn[0].d2;
  m_do3 = 0.0;

  m_d1_d = m_d1;
  m_d2_d = m_d2;
  m_do1_d = m_do1;
  m_do2_d = m_do2;


  m_dd[0] = m_d1;
  m_dd[1] = m_d2;
  m_dd[2] = m_d3;

  m_dd[3] = m_do1;
  m_dd[4] = m_do2;
  m_dd[5] = m_do3;

}

    
    
void CColorLine::get_colors(const char *fn) 
{
  CString sc = AfxGetApp()->GetProfileString("ColorAn", fn, "");
  if (!sc.IsEmpty()) {
    int n = sscanf(sc, "%lf %d %lf %d %lf %d %lf %d %lf %d %lf %d", 
      &m_d1, &m_color[0],
      &m_d2, &m_color[1],
      &m_d3, &m_color[2],
      &m_do1, &m_color[3],
      &m_do2, &m_color[4],
      &m_do3, &m_color[5]
      );
      if (n == 12) return;
  }

  init_colors();

  for (int i = 0; i < sizeof(ut_fn)/sizeof(ut_fn[0]); i++) {
    if (!strcmp(ut_fn[i].fn, fn)) {
      m_d1 = ut_fn[i].d1;
      m_d2 = ut_fn[i].d2;
      m_do1 = ut_fn[i].d1;
      m_do2 = ut_fn[i].d2;
      break;
    }
  }

  m_d1_d = m_d1;
  m_d2_d = m_d2;
  m_do1_d = m_do1;
  m_do2_d = m_do2;
}



CColorLine::CColorLine(CWnd* pParent, CAdoFile *ado, const TCHAR *name, const TCHAR *cod)
  : CDialog(CColorLine::IDD, pParent)
{
  //{{AFX_DATA_INIT(CColorLine)
  m_d1 = 0.0;
  m_d2 = 0.0;
  m_d3 = 0.0;
  m_do1 = 0.0;
  m_do2 = 0.0;
  m_do3 = 0.0;
  m_typ = 0;
  //}}AFX_DATA_INIT

  m_ado = ado;
  m_strTitle = name;
  m_cod = cod;
  m_hAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_DBTYPE));

  init_colors();

  for (int i = 0; i < 6; i++) {
    m_brush[i] = 0;
  }
  m_fn = AfxGetApp()->GetProfileString("ColorAnFN", "fn", "");

}


void CColorLine::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CColorLine)
  DDX_Control(pDX, IDC_COMBO1, m_combo);
  DDX_Control(pDX, IDC_COLOR1, m_color1);
  DDX_Control(pDX, IDC_COLOR2, m_color2);
  DDX_Control(pDX, IDC_COLOR3, m_color3);
  DDX_Text(pDX, IDC_EDIT1, m_d1);
  DDX_Text(pDX, IDC_EDIT2, m_d2);
  DDX_Text(pDX, IDC_EDIT3, m_d3);

  DDX_Text(pDX, IDC_EDIT1_D, m_d1_d);
  DDX_Text(pDX, IDC_EDIT2_D, m_d2_d);
  
  DDX_Control(pDX, IDC_COLORO1, m_coloro1);
  DDX_Control(pDX, IDC_COLORO2, m_coloro2);
  DDX_Control(pDX, IDC_COLORO3, m_coloro3);
  DDX_Text(pDX, IDC_EDITO1, m_do1);
  DDX_Text(pDX, IDC_EDITO2, m_do2);

  DDX_Text(pDX, IDC_EDITO1_D, m_do1_d);
  DDX_Text(pDX, IDC_EDITO2_D, m_do2_d);
  
  DDX_Text(pDX, IDC_EDITO3, m_do3);
  DDX_CBIndex(pDX, IDC_COMBO1, m_typ);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColorLine, CDialog)
  //{{AFX_MSG_MAP(CColorLine)
  ON_WM_CTLCOLOR()
  ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
  ON_EN_CHANGE(IDC_EDIT1, OnChange)
  ON_EN_CHANGE(IDC_EDIT2, OnChange)
  ON_EN_CHANGE(IDC_EDITO1, OnChange)
  ON_EN_CHANGE(IDC_EDITO2, OnChange)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorLine message handlers


BOOL CColorLine::OnInitDialog() 
{
  CDialog::OnInitDialog();
  
  CString str, str1, str2, str3;
  CString s2;


  m_d1 = m_dd[0];
  m_d2 = m_dd[1];
  m_d3 = m_dd[2];

  m_do1 = m_dd[3];
  m_do2 = m_dd[4];
  m_do3 = m_dd[5];

  m_d1_d = m_d1;
  m_d2_d = m_d2;
  m_do1_d = m_do1;
  m_do2_d = m_do2;

  for (int i = 0; i < sizeof(ut_fn)/sizeof(ut_fn[0]); i++) {
    s2 = ut_fn[i].fn;

    if (s2 == m_fn) {
      m_typ = i;
      get_colors(m_fn);
    }

    m_d1_d = m_d1;
    m_d2_d = m_d2;
    m_do1_d = m_do1;
    m_do2_d = m_do2;

    str1 = s2;
    str2 = "";

    str2 = ut_fn[i].rus;

    m_combo.AddString(str2);
  }



  UpdateData(FALSE);
  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}


HBRUSH CColorLine::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

  int i = -1;

  switch (pWnd->GetDlgCtrlID())
  {
  case IDC_COLOR_W1: i = 0; break;
  case IDC_COLOR_W2: i = 1; break;
  case IDC_COLOR_W3: i = 2; break;
  case IDC_COLOR_WO1: i = 3; break;
  case IDC_COLOR_WO2: i = 4; break;
  case IDC_COLOR_WO3: i = 5; break;
  }
    
  if (i >= 0) {
    delete m_brush[i];
    m_brush[i] = new CBrush(m_color[i]);
    hbr = (HBRUSH)(m_brush[i]->GetSafeHandle());
  }

  return hbr;
}


static COLORREF rgb[] = {
  0x000000, 0x7F0000, 0x007F00, 0x7F7F00, 0x00007F, 0x7F007F, 0x007F7F,
  0xBFBFBF, 0x7F7F7F, 0xFF0000, 0x00FF00, 0xFFFF00, 0x0000FF, 0xFF00FF,
  0x00FFFF, 0xFFFFFF,
};

BOOL CColorLine::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  UINT nID = LOWORD(wParam);
  int i = -1;

  switch (nID)
  {
  case IDC_COLOR1: i = 0; break;
  case IDC_COLOR2: i = 1; break;
  case IDC_COLOR3: i = 2; break;
  case IDC_COLORO1: i = 3; break;
  case IDC_COLORO2: i = 4; break;
  case IDC_COLORO3: i = 5; break;
  }

  if (i >= 0) {
    CColorDialog dlgColor(m_color[i]);
    dlgColor.m_cc.lpCustColors = rgb;

    if (dlgColor.DoModal() == IDOK)
    {
      m_color[i] = dlgColor.GetColor();
      Invalidate();
    }
  }

  return CDialog::OnCommand(wParam, lParam);
}

void CColorLine::OnOK() 
{
  UpdateData(TRUE);

  m_fn = "";

  if (0 <= m_typ && m_typ < sizeof(ut_fn)/sizeof(ut_fn[0])) {
    m_fn = ut_fn[m_typ].fn;
    m_out = ut_fn[m_typ].out;
  }

  m_dd[0] = m_d1;
  m_dd[1] = m_d2;
  m_dd[2] = m_d3;

  m_dd[3] = m_do1;
  m_dd[4] = m_do2;
  m_dd[5] = m_do3;

  CString str, s;

//  str.Format("%s ", utd.m_fn);
  str = "";

  for (int i = 0; i < 6; i++) {
    s.Format("%f %d ", m_dd[i], m_color[i]);
    str += s;
  }

  AfxGetApp()->WriteProfileString("ColorAn", m_fn, str);
  AfxGetApp()->WriteProfileString("ColorAnFN", "fn", m_fn);

  CDialog::OnOK();
}

void CColorLine::OnSelchangeCombo1() 
{
  UpdateData(TRUE);

  if (0 <= m_typ && m_typ < sizeof(ut_fn)/sizeof(ut_fn[0])) {
    get_colors(ut_fn[m_typ].fn);
    UpdateData(FALSE);
    Invalidate();
  }
}

void CColorLine::OnChange()
{
  UpdateData(TRUE);

  m_d1_d = m_d1;
  m_d2_d = m_d2;

  m_do1_d = m_do1;
  m_do2_d = m_do2;

  
  CString str;

  str.Format("%g", m_d1);
  SetDlgItemText(IDC_EDIT1_D, str);

  str.Format("%g", m_d2);
  SetDlgItemText(IDC_EDIT2_D, str);

  str.Format("%g", m_do1);
  SetDlgItemText(IDC_EDITO1_D, str);

  str.Format("%g", m_do2);
  SetDlgItemText(IDC_EDITO2_D, str);



//  UpdateData(FALSE);
}
