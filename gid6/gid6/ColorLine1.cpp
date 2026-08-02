// ColorLine1.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "ColorLine1.h"
#include "Bgi.h"
#include "ado.h"

#include "win.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorLine1 dialog

struct PT_FN {
  char *fn;
  double d1, d2;
};

static PT_FN pt_fn[] = {
{"Qotz", 0, 1,}, //  Тепл.нагрузка  на отопление, зав.сх.   
{"Qotn", 0, 1,}, //  Тепл.нагрузка  на отопление, незав.сх. 
{"dop12", 0, 1,}, //  Тепловая нагрузка потребителя на вентиляцию 
{"dop13", 0, 1,}, //  Тепл.нагрузка на кондиционирование  
{"dop17", 0, 0.5,}, //  Тепл.нагрузка на ГВ в закрытой системе  0, 
{"dop18", 0, 0.5,}, //  Тепл.нагрузка на ГВ из под.теплопр. в откр.сист.  0, 
{"dop19", 0, 0.5,}, //  Тепл.нагрузка на ГВ из обр.теплопр. в откр.сист.  0, 
{"dop20", 0, 0.2,}, //  Тепл.нагрузка в цирк.теплопроводе ГВС 0, 
{"Qsum_z", 0, 1.5,}, //  Суммарная тепл.нагрузка  в закрытой системе  0, 
{"Qfact", 0, 1.5,}, //  Суммарная тепловая нагрузка систем теплопотребления 0, 
{"Q_obesp_min", 0, 95},  //  Требуемый расход тепла  на горячее водоснабжение  в открытой системе из под.трубопр.
{"Q_obesp_otopl_min", 0, 95},  //  Требуемый расход тепла  на горячее водоснабжение  в открытой системе из обр.трубопр.
{"Q_obesp_gvs_min", 0, 95},  //  Требуемый расход тепла на рециркуляцию в  в открытой системе ГВ
};


void CColorLine1::init_colors() 
{
  int i;

  for (i = 0; i < 3; i++) {
    m_color[i] = 0;
    m_dd[i] = 0;
  }
  m_color[0] = 0x0000FF;
  m_color[1] = 0x00FF00;
  m_color[2] = 0xFF0000;

  m_d1 = pt_fn[0].d1;
  m_d2 = pt_fn[0].d2;
  m_d3 = 0.0;

  m_d1_d = m_d1;
  m_d2_d = m_d2;


  m_dd[0] = m_d1;
  m_dd[1] = m_d2;
  m_dd[2] = m_d3;

}

    
    
void CColorLine1::get_colors(const char *fn) 
{
  CString sc = AfxGetApp()->GetProfileString("ColorAn1", fn, "");
  if (!sc.IsEmpty()) {
    int n = sscanf(sc, "%lf %d %lf %d %lf %d", 
      &m_d1, &m_color[0],
      &m_d2, &m_color[1],
      &m_d3, &m_color[2]
      );
      if (n == 6) return;
  }

  init_colors();

  for (int i = 0; i < sizeof(pt_fn)/sizeof(pt_fn[0]); i++) {
    if (!strcmp(pt_fn[i].fn, fn)) {
      m_d1 = pt_fn[i].d1;
      m_d2 = pt_fn[i].d2;
      break;
    }
  }

  m_d1_d = m_d1;
  m_d2_d = m_d2;
}



CColorLine1::CColorLine1(CWnd* pParent, CAdoFile *ado, const TCHAR *name, const TCHAR *cod)
  : CDialog(CColorLine1::IDD, pParent)
{
  //{{AFX_DATA_INIT(CColorLine1)
  m_d1 = 0.0;
  m_d2 = 0.0;
  m_d3 = 0.0;
  m_typ = 0;
  //}}AFX_DATA_INIT

  m_ado = ado;
  m_strTitle = name;
  m_cod = cod;
  m_hAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_DBTYPE));

  init_colors();

  for (int i = 0; i < 3; i++) {
    m_brush[i] = 0;
  }
  m_fn = AfxGetApp()->GetProfileString("ColorAnFN1", "fn", "");
}


void CColorLine1::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CColorLine1)
  DDX_Control(pDX, IDC_COMBO1, m_combo);
  DDX_Control(pDX, IDC_COLOR1, m_color1);
  DDX_Control(pDX, IDC_COLOR2, m_color2);
  DDX_Control(pDX, IDC_COLOR3, m_color3);
  
  DDX_Text(pDX, IDC_EDIT1, m_d1);
  DDX_Text(pDX, IDC_EDIT2, m_d2);
  DDX_Text(pDX, IDC_EDIT3, m_d3);

  DDX_Text(pDX, IDC_EDIT1_D, m_d1_d);
  DDX_Text(pDX, IDC_EDIT2_D, m_d2_d);
  
  
  DDX_CBIndex(pDX, IDC_COMBO1, m_typ);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColorLine1, CDialog)
  //{{AFX_MSG_MAP(CColorLine1)
  ON_WM_CTLCOLOR()
  ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
  ON_EN_CHANGE(IDC_EDIT1, OnChange)
  ON_EN_CHANGE(IDC_EDIT2, OnChange)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorLine1 message handlers


BOOL CColorLine1::OnInitDialog() 
{
  CDialog::OnInitDialog();
  
  CString str, str1, str2, str3;
  CString s2;

  m_d1 = m_dd[0];
  m_d2 = m_dd[1];
  m_d3 = m_dd[2];

  m_d1_d = m_d1;
  m_d2_d = m_d2;


  for (int i = 0; i < sizeof(pt_fn) / sizeof(pt_fn[0]); i++) {
      s2 = pt_fn[i].fn;

      if (s2 == m_fn) {
          m_typ = i;
          get_colors(m_fn);
      }

      str1 = s2;
      str2 = "";

      MySQLColumns *col = m_ado->findMySQLColumns("PT_OUT", s2);
      MySQLColumns2 *col2 = m_ado->findMySQLColumns2("PT_OUT", s2);

      if (col) str2 = col->strCOLUMN_COMMENT;
      if (col2) str2 = col2->hlp;

      m_combo.AddString(str2);
//      m_combo.AddString(str1 + ", " + str2);
  }

  m_d1_d = m_d1;
  m_d2_d = m_d2;

  UpdateData(FALSE);
  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}


HBRUSH CColorLine1::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

  int i = -1;

  switch (pWnd->GetDlgCtrlID())
  {
  case IDC_COLOR_W1: i = 0; break;
  case IDC_COLOR_W2: i = 1; break;
  case IDC_COLOR_W3: i = 2; break;
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

BOOL CColorLine1::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  UINT nID = LOWORD(wParam);
  int i = -1;

  switch (nID)
  {
  case IDC_COLOR1: i = 0; break;
  case IDC_COLOR2: i = 1; break;
  case IDC_COLOR3: i = 2; break;
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

void CColorLine1::OnOK() 
{
  UpdateData(TRUE);

  m_fn = "";

  if (0 <= m_typ && m_typ < sizeof(pt_fn)/sizeof(pt_fn[0])) {
    m_fn = pt_fn[m_typ].fn;
  }

  m_dd[0] = m_d1;
  m_dd[1] = m_d2;
  m_dd[2] = m_d3;

  CString str, s;

//  str.Format("%s ", utd.m_fn);
  str = "";

  for (int i = 0; i < 3; i++) {
    s.Format("%f %d ", m_dd[i], m_color[i]);
    str += s;
  }

  AfxGetApp()->WriteProfileString("ColorAn1", m_fn, str);
  AfxGetApp()->WriteProfileString("ColorAnFN1", "fn", m_fn);

  CDialog::OnOK();
}

void CColorLine1::OnSelchangeCombo1() 
{
  UpdateData(TRUE);

  if (0 <= m_typ && m_typ < sizeof(pt_fn)/sizeof(pt_fn[0])) {
    get_colors(pt_fn[m_typ].fn);
    UpdateData(FALSE);
    Invalidate();
  }
}

void CColorLine1::OnChange()
{
  UpdateData(TRUE);

  m_d1_d = m_d1;
  m_d2_d = m_d2;

  CString str;

  str.Format("%g", m_d1);
  SetDlgItemText(IDC_EDIT1_D, str);

  str.Format("%g", m_d2);
  SetDlgItemText(IDC_EDIT2_D, str);


//  UpdateData(FALSE);
}
