// CNagruzZdan.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "CNagruzZdan.h"
#include "afxdialogex.h"

#include "ado.h"


// CNagruzZdan dialog

IMPLEMENT_DYNAMIC(CNagruzZdan, CDialogEx)

CNagruzZdan::CNagruzZdan(CWnd* pParent, int _is_otopl)
	: CDialogEx(IDD_NAGRUZ_ZDAN, pParent)
{
  m_nazn = _T("");
  m_god = 0;
  m_v = 0.0;
  m_udel = 0.0;
  m_tn = 0.0;
  m_tvn = 0.0;
  m_nagr = 0.0;
  m_alpha = 0.0;

  m_is_otopl = _is_otopl;

}

CNagruzZdan::~CNagruzZdan()
{
}

void CNagruzZdan::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

  DDX_Control(pDX, IDC__HELP, m_Help);
  DDX_Control(pDX, IDC_EDIT8, m_e8);
  DDX_Control(pDX, IDC_EDIT7, m_e7);
  DDX_Control(pDX, IDC_EDIT6, m_e6);
  DDX_Control(pDX, IDC_EDIT5, m_e5);
  DDX_Control(pDX, IDC_EDIT4, m_e4);
  DDX_Control(pDX, IDC_EDIT3, m_e3);
  DDX_Control(pDX, IDC_EDIT2, m_e2);
  DDX_Control(pDX, IDC_COMBO1, m_ctl_41);
  DDX_Control(pDX, IDCANCEL, Button1);
  DDX_Control(pDX, IDOK, Button2);
  DDX_Text(pDX, IDC_EDIT2, m_god);
  DDX_Text(pDX, IDC_EDIT3, m_v);
  DDV_MinMaxDouble(pDX, m_v, 0., 1000000.);
  DDX_Text(pDX, IDC_EDIT4, m_udel);
  DDX_Text(pDX, IDC_EDIT5, m_tn);
  DDV_MinMaxDouble(pDX, m_tn, -100, 0.);
  DDX_Text(pDX, IDC_EDIT6, m_tvn);
  DDV_MinMaxDouble(pDX, m_tvn, 0, 30.);
  DDX_Text(pDX, IDC_EDIT8, m_nagr);
  DDX_Text(pDX, IDC_EDIT7, m_alpha);
  DDX_Text(pDX, IDC_COMBO1, m_nazn);

}


double get_alpha(CAdoFile* ado, double t)
{
    double t1 = 0, t2, a1 = 0, a2;

    if (ado->openTable0("SELECT * FROM 40_popravochnyy_koeffitsient_k_teplovym_harakteristikam_zdaniy")) {
        while (!ado->isEOF()) {
            t2 = ado->read_double("t_r");
            a2 = ado->read_double("alfa");
            if (t > t2) {
                break;
            }
            t1 = t2;
            a1 = a2;

            ado->MoveNext();
        }
        ado->closeTable();
        double propor = (t - t1) / (t2 - t1);
        if (t2 == t1) return 1;

        return a1 * (1. - propor) + a2 * propor;
    }
    return 1.;
}


void CNagruzZdan::Raschet()
{
  UpdateData(TRUE);

  CAdoFile* ado = getAdo("sprav");

  if (!ado) {
      AfxMessageBox("Ошибка", MB_OK|MB_ICONINFORMATION);
      return;
  }


  m_alpha = m_tn;

//  AfxMessageBox(m_nazn, MB_OK|MB_ICONINFORMATION);


  int num = -1;

  CString q, q2 = "";

  if (m_nazn == "Жилое") {
    if (m_god <= 1958) {
      q2.Format(" AND (pr_year='С')", m_nazn);
    }
    else {
      q2.Format(" AND (pr_year='Н')", m_nazn);
    }
  }

  q.Format("SELECT Vmin, Vmax, q_ot, q_vt FROM [10_teplovye_harakteristiki_zdaniy]" 
    " WHERE name_zd='%s' %s", m_nazn, q2);

  if (ado->openTable0(q)) {
      while (!ado->isEOF()) {
          double v1, v2;
          v1 = ado->read_double("Vmin");
          v2 = ado->read_double("Vmax");
          if (v1 <= m_v && m_v <= v2) {
              if (m_is_otopl)
                  m_udel = ado->read_double("q_ot");
              else
                  m_udel = ado->read_double("q_vt");
          }
          ado->MoveNext();
      }
      ado->closeTable();
    }

  m_alpha = get_alpha(ado, m_tn) ;
  
  m_nagr = m_udel*m_v*m_alpha*(m_tvn-m_tn)/1000000;


  UpdateData(FALSE);

}



BEGIN_MESSAGE_MAP(CNagruzZdan, CDialogEx)
  ON_CBN_SELCHANGE(IDC_COMBO1, Raschet)
  ON_EN_CHANGE(IDC_EDIT2, Raschet)
  ON_EN_CHANGE(IDC_EDIT3, Raschet)
  ON_EN_CHANGE(IDC_EDIT5, Raschet)
  ON_EN_CHANGE(IDC_EDIT6, Raschet)
  ON_BN_CLICKED(IDC__HELP, OnHelp)
END_MESSAGE_MAP()




// CNagruzZdan message handlers


BOOL CNagruzZdan::OnInitDialog()
{
  CDialogEx::OnInitDialog();


  CAdoFile* ado = getAdo("sprav");

  if (!ado) {
      AfxMessageBox("Ошибка", MB_OK|MB_ICONINFORMATION);
      return TRUE;
  }

  int num = -1;

  CString q;
  q.Format("SELECT DISTINCT name_zd FROM [10_teplovye_harakteristiki_zdaniy]");

  if (ado->openTable0(q)) {
      int i = 0;

      while (!ado->isEOF()) {
          CString s = ado->read(0);
          if (s != "") m_ctl_41.AddString(s);
          ado->MoveNext();
      }
      ado->closeTable();
  }

  UpdateData(TRUE);
  m_alpha = get_alpha(ado, m_tn);
  m_nagr = m_udel * m_v * m_alpha * (m_tvn - m_tn) / 1000000;
  UpdateData(FALSE);


  m_ctl_41.SetCurSel(m_ctl_41.SelectString(0, m_nazn));



  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}



void CNagruzZdan::OnHelp() 
{
  CString str = 

"Расчетные  тепловые нагрузки на отопление, приточную вентиляцию и кондиционирование и кондиционирование воздуха по укрупнённым измерителям производится для оценки часового теплопотребленийя зданий, а также в тех случаях, когда для наладки системы теплопотребления достаточно определения тепловой нагрузки здания в целом.\r\n"
"Отопительная Qот и вентиляционная Qв тепловая нагрузка здания, Гкал/ч. определяется соответственно по формулам [Е.Я. Соколов. Теплофикация и тепловые сети. М.: Энергия, 1982]:\r\n"
"Qот=a*qот*V*(tв.р.- tн.р.о)*10**-6;\r\n"
"Qв=a*qв*V*(tв.р.- tн.р.в)*10**-6,\r\n"
"где a - поправочный коэффициент (корректировка значений qот, qв при расчетных температурах наружного воздуха, отличных от -30 Град.С. Смотрите Справочную таблицу 40_Поправочный коэффициент к тепловым характеристикам зданий;\r\n"
"qот, qв - соответственно удельные отопительная и вентиляционная тепловые характеристики зданий, ккал/(м**3*ч*Град.С). Смотрите Справочную таблицу 10_Тепловые характеристики зданий;\r\n"
"tв.р. - расчетная температура воздуха в помещениях, Грда.С;\r\n"
"tн.р.о, tн.р.в - расчетные температуры наружного для проектирование соответственно отопления и вентиляции, Град.С.";

    AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);

//  CHelpWnd HelpWnd(str, str);
//  HelpWnd.DoModal();  
}
