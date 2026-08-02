// KlEdit.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "Klassif.h"
#include "KlEdit.h"
#include "KlTyp.h"
#include "ado.h"
#include "win.h"

#include "GeoFile.h"
#include "TaskProgressDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CKlEdit dialog


CString findTableRusName(CString d, CString n);


CKlEdit::CKlEdit(CWnd* pParent, Klassif *kl)
  : CDialog(CKlEdit::IDD, pParent)
{
  m_kl = kl;

  //{{AFX_DATA_INIT(CKlEdit)
  m_name = kl->nazv;
  m_rus = findTableRusName(kl->m_ado->m_schema, kl->nazv);
  m_dWidth = kl->typ;
  m_loc = kl->loc%10;
  m_loc10 = kl->loc >= 10;
  m_nListLabel = -1;
  m_bLabel = kl->label;
  m_nPrio = kl->prio;
  m_typ = kl->typ;
  m_potr = m_kl->is_potr;
  m_ramka = m_kl->ramka;
  m_punct = FALSE;
  m_check_color = FALSE;
  m_label_color = m_kl->label_color;
  m_nListLabelColor = -1;
  m_nomas = m_kl->no_mas;

  m_mas1 = m_kl->mas1;
  m_mas2 = m_kl->mas2;
  m_mas_text1 = m_kl->mas_text1;
  m_mas_text2 = m_kl->mas_text2;

  //}}AFX_DATA_INIT

  if (m_loc == 2) m_punct = m_kl->punct;


  m_brush1 = 0;
  m_brush2 = 0;
  m_brush3 = 0;

}


void CKlEdit::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CKlEdit)
  DDX_Control(pDX, IDC_LIST_LABEL_COLOR, m_ctrlListLabelColor);
  DDX_Control(pDX, IDC_PUNCT, m_ctrlPunct);
  DDX_Control(pDX, IDC_WIDTH_STATIC, m_width_static);
  DDX_Control(pDX, IDC_COLOR_W2, m_color_w2);
  DDX_Control(pDX, IDC_COLOR2, m_color2);
  DDX_Control(pDX, IDC_WIDTH, m_width_ctrl);
  DDX_Control(pDX, IDC_RASTR, m_rastr);
  DDX_Control(pDX, IDC_SPIN_PRIO, m_SpinPrio);
  DDX_Control(pDX, IDC_LIST_LABEL, m_ctrlListLabel);
  DDX_Control(pDX, IDC_SPIN_WIDTH, m_SpinWidth);

  DDX_Control(pDX, IDC_LOC, m_ctrlLoc);
  DDX_CBIndex(pDX, IDC_LOC, m_nLoc);
  

  DDX_Text(pDX, IDC_WIDTH, m_dWidth);

  DDX_Text(pDX, IDC_MAS1, m_mas1);
  DDX_Text(pDX, IDC_MAS2, m_mas2);
  DDX_Text(pDX, IDC_MAS_TEXT1, m_mas_text1);
  DDX_Text(pDX, IDC_MAS_TEXT2, m_mas_text2);
  

  DDX_Text(pDX, IDC_NAME, m_name);
  DDX_Text(pDX, IDC_RUS, m_rus);
  DDX_Text(pDX, IDC_WIDTH, m_dWidth);
  DDV_MinMaxDouble(pDX, m_dWidth, 0., 250.);
  DDX_CBIndex(pDX, IDC_LIST_LABEL, m_nListLabel);
  DDX_Check(pDX, IDC_LABEL, m_bLabel);
  DDX_Text(pDX, IDC_PRIO, m_nPrio);
  DDV_MinMaxInt(pDX, m_nPrio, 0, 10);
  DDX_CBIndex(pDX, IDC_POTR, m_potr);
  DDX_Check(pDX, IDC_RAMKA, m_ramka);
  DDX_Check(pDX, IDC_PUNCT, m_punct);
  DDX_Check(pDX, IDC_LABEL_COLOR, m_label_color);
  DDX_CBIndex(pDX, IDC_LIST_LABEL_COLOR, m_nListLabelColor);
  DDX_Check(pDX, IDC_NOMAS, m_nomas);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKlEdit, CDialog)
  //{{AFX_MSG_MAP(CKlEdit)

  ON_BN_CLICKED(IDC_RUS_SAVE, OnRusSave)
  ON_BN_CLICKED(IDC_RUS_LOAD, OnRusLoad)
  
  
  ON_BN_CLICKED(IDC_COLOR, OnColor)
  ON_BN_CLICKED(IDC_COLOR2, OnColor2)
  ON_BN_CLICKED(IDC_FONT, OnFont)
  ON_BN_CLICKED(IDC_RASTR, OnRastr)
  ON_CBN_SELCHANGE(IDC_LOC, OnSelchangeLoc)
  ON_WM_CTLCOLOR()
  //}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_PUNCT, &CKlEdit::OnBnClickedPunct)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKlEdit message handlers

BOOL CKlEdit::OnInitDialog()
{
  CDialog::OnInitDialog();

  m_SpinWidth.SetRange(0, 10);
  m_SpinPrio.SetRange(0, 10);

  int i = 0, j = 0, lab = -1, lab_color = -1;


  CAdoFile *ado = m_kl->m_ado;

  CString q;
  q.Format("SELECT * FROM %s", m_kl->nazv);

  if (ado && ado->openTable0(q)) {

    for ( i = 0; i < ado->NFlds(); i++ ) {
      CAdoField af = ado->Field(i);
      const CColName *col_name = findColumnRusName(ado->m_schema, m_kl->nazv, af.name);

      CString rus = af.name;
      
      if (col_name) {
            rus = col_name->name;
      }

      if (af.typ == dbLongBinary) continue; 

      if (af.name == m_kl->codlabel) lab = j;
      if (af.name == m_kl->codlabel_color) lab_color = j;

      m_ctrlListLabel.AddString(rus);
      m_ctrlListLabelColor.AddString(rus);

      map_fn[rus] = af.name;

      j++;
    }
  
    ado->closeTable();

    m_ctrlListLabel.SetCurSel(lab);
    m_ctrlListLabelColor.SetCurSel(lab_color);

    m_nListLabel = lab;
    m_nListLabelColor = lab_color;

    if (m_name == "TEH_USL Техническое условие" || m_name == "TEH_USL_Техническое_условие" || m_name == _TR("Техническое_условие")) {
      const char *fn = _TR("Нагрузки");
      m_ctrlListLabel.AddString(fn);
      if (!strcmp(fn, m_kl->codlabel)) {
        m_ctrlListLabel.SetCurSel(j);
      }
      if (!strcmp(fn, m_kl->codlabel_color)) {
        m_ctrlListLabelColor.SetCurSel(j);
      }
    }
  }

  m_ctrlLoc.AddString("Точечный");
  m_ctrlLoc.AddString("Линейный");
  m_ctrlLoc.AddString("Площадной");
  m_ctrlLoc.AddString("Точечный с поворотом");

  m_nLoc = m_loc <= 3 ? m_loc -1 : m_loc == 6 ? 3 : -1;

  setLoc(m_loc);


  UpdateData(FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}


void CKlEdit::OnOK()
{
  UpdateData(TRUE);

  m_kl->loc = m_loc + m_loc10*10;   

  m_kl->nazv = m_name;

  m_kl->mas1 = m_mas1;
  m_kl->mas2 = m_mas2;
  m_kl->mas_text1 = m_mas_text1;
  m_kl->mas_text2 = m_mas_text2;

  m_kl->prio = m_nPrio;
  if (m_loc == 2) m_kl->punct = m_punct;

  if (isPoint(m_loc)) {
    m_kl->typ = m_typ;
  }
  else {
    m_kl->typ = m_dWidth;
  }

  m_kl->ramka = m_ramka;
  m_kl->is_potr = m_potr;

  m_kl->no_mas = m_nomas;

    if (is_special_kls(m_name)) {
      m_kl->no_mas = 1;
    }


  char s[256];

  m_kl->codlabel = "";
  m_kl->codlabel_color = "";

  if (m_ctrlListLabel.GetLBText(m_nListLabel, s) != CB_ERR) {
    map<CString, CString>::const_iterator it = map_fn.find(s);
    if (it != map_fn.end()) {
      m_kl->codlabel = it->second;
    }
  }
  if (m_ctrlListLabel.GetLBText(m_nListLabelColor, s) != CB_ERR) {
    map<CString, CString>::const_iterator it = map_fn.find(s);
    if (it != map_fn.end()) {
      m_kl->codlabel_color = it->second;
    }
  }

  m_kl->n_label = m_nListLabel;
  if (!strcmp(m_name, "TEH_USL Техническое условие") || !strcmp(m_name, "TEH_USL_Техническое_условие") || !strcmp(m_name, _TR("Техническое_условие"))) {
    if (!lstrcmpi(_TR("Нагрузки"), m_kl->codlabel)) m_kl->n_label = 9999;
  }

  m_kl->n_label_color = m_nListLabelColor;

  m_kl->label = m_bLabel;
  m_kl->label_color = m_label_color;

//  m_kl->typ = typl.GetCurSel();
//  if (m_geofile) m_geofile->reset_geo(m_kl);

  if (m_geofile) {
      Percent2 pc;
      m_geofile->readMySql_Shape_table(m_kl->m_ado, m_kl, &pc);
  }

//  m_kl->loc = m_loc + m_loc10 * 10;

  CDialog::OnOK();
}

static COLORREF rgb[] = {
  0x000000, 0x7F0000, 0x007F00, 0x7F7F00, 0x00007F, 0x7F007F, 0x007F7F,
  0xBFBFBF, 0x7F7F7F, 0xFF0000, 0x00FF00, 0xFFFF00, 0x0000FF, 0xFF00FF,
  0x00FFFF, 0xFFFFFF,
};


void CKlEdit::OnColor()
{

  CColorDialog dlgColor(m_kl->coli);
  dlgColor.m_cc.lpCustColors = rgb;

  if (dlgColor.DoModal() == IDOK)
  {
    m_kl->coli = dlgColor.GetColor();
    Invalidate();
  }
}

void CKlEdit::OnColor2()
{
  CColorDialog dlgColor(m_kl->colb);
  dlgColor.m_cc.lpCustColors = rgb;

  if (dlgColor.DoModal() == IDOK)
  {
    m_kl->colb = dlgColor.GetColor();
    Invalidate();
  }
}

void CKlEdit::OnFont()
{
  CFontDialog fd(&m_kl->lf,  CF_EFFECTS | CF_SCREENFONTS);
  fd.m_cf.rgbColors = m_kl->color;

  if (fd.DoModal() == IDOK) {
    m_kl->color = fd.GetColor();
  }
}

HBRUSH CKlEdit::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

  switch (pWnd->GetDlgCtrlID())
  {
  case IDC_COLOR_W:
    {
      if (m_brush1) delete m_brush1;
      m_brush1 = new CBrush(m_kl->coli);
      hbr = (HBRUSH)(m_brush1->GetSafeHandle());
    }
    break;
  case IDC_COLOR_W2:
      if (m_brush2) delete m_brush2;
      m_brush2 = new CBrush(m_kl->colb);
      hbr = (HBRUSH)(m_brush2->GetSafeHandle());
    break;
  case IDC_COLOR_W3:
      if (m_brush3) delete m_brush3;
      m_brush3 = new CBrush(m_kl->col3);
      hbr = (HBRUSH)(m_brush3->GetSafeHandle());
    break;
  }

  // TODO: Return a different brush if the default is not desired
  return hbr;
}

//#include "KlTyp.h"

void CKlEdit::OnRastr()
{
  int ret;

  if (m_loc == 6)  {
    CKlTyp kltyp(this);
    ret = kltyp.DoModal();

    if (ret > 100) {
      m_typ = ret-100;
      m_rastr.SetTyp(m_typ);
    }
  }
//  if (m_loc == 99990)  {
  else if (m_loc == 1)  {
    CKlTyp kltyp(this);
    ret = kltyp.DoModal();

    if (ret > 100) {
      m_typ = ret-100;
      m_rastr.SetTyp(m_typ);
    }
  }
}


void CKlEdit::OnBnClickedPunct()
{
    // TODO: Add your control notification handler code here
}

void CKlEdit::setLoc(int loc) 
{
  if (m_loc == 3 || m_ramka) {
    m_color2.ShowWindow(SW_SHOW);
    m_color_w2.ShowWindow(SW_SHOW);
  }
  else {
    m_color2.ShowWindow(SW_HIDE);
    m_color_w2.ShowWindow(SW_HIDE);
  }


  if (m_loc == 2) {
    m_ctrlPunct.ShowWindow(SW_SHOW);
  }
  else {
    m_ctrlPunct.ShowWindow(SW_HIDE);
  }


  if (isPoint(m_loc)) {
    m_rastr.ShowWindow(SW_SHOW);
    m_width_ctrl.ShowWindow(SW_HIDE);
    m_SpinWidth.ShowWindow(SW_HIDE);
    m_width_static.ShowWindow(SW_HIDE);
  }
  else {
    m_rastr.ShowWindow(SW_HIDE);
    m_width_ctrl.ShowWindow(SW_SHOW);
    m_SpinWidth.ShowWindow(SW_SHOW);
    m_width_static.ShowWindow(SW_SHOW);
  }

  if (isPoint(m_loc)) {
    m_rastr.SetTyp(m_kl->typ);
  }
}


void CKlEdit::OnSelchangeLoc() 
{
  UpdateData(TRUE);

  m_loc = m_nLoc < 3 ? m_nLoc+1 : m_nLoc == 3 ? 6 : -1;
  setLoc(m_loc);
}

void saveTableRusName2(CString database, CString klfn);
void initColumnRusName(CString database, CString database2);

void CKlEdit::OnRusSave()
{
    CFileDialog fd(FALSE, _T("txt2"), NULL,
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
        _TR("*.txt2|*.txt2|"), this);

    if (fd.DoModal() == IDOK) {
        saveTableRusName2(m_kl->m_ado->m_schema, fd.GetPathName());
    }
}

void CKlEdit::OnRusLoad()
{
    CFileDialog fd(TRUE, _T("txt2"), NULL,
        OFN_HIDEREADONLY,
        _TR("*.txt2|*.txt2|"), this);

    if (fd.DoModal() == IDOK) {
        CString fn = fd.GetPathName();
        CString database = m_kl->m_ado->m_schema;

        CString fn2;

        fn2.Format("%s%s.txt2", argpath_2(), database);

        if (CopyFile(fn, fn2, 0)) {

            initColumnRusName(database, database);

            if (database == getGidAdoName()) {
                initColumnRusName(database, "GID");
            }
            AfxMessageBox(_TR("Выполнено! Перезагрузите программу"), MB_OK|MB_ICONINFORMATION);
        }
    }
}
