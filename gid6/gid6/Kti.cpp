// Kti.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "Kti.h"
#include "ado.h"

#include "gidrView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//namespace NEW {
//#include "../potr4/prog/1/read_xx.h"
//}

//using namespace NEW;

void get_param_kti(CString &kod, double &dlina, double &diametr, double &diametr_usl, double &tol, int &year, int &typ_pr, int &kolwork);


/////////////////////////////////////////////////////////////////////////////
// CKti dialog


double get_beta(char typ, double diam) { return 1.; };


void open_vsp2(const char *vsp);
void open_vsp3(const char *vsp);



//const CT *getCT();
//const PC *getPC(string kod_rs);
//int get_TG(string & name, double tn, double &t1, double &t2, double &t3, bool is_leto);

double get_q_new(int year, char typ_pr, double diametr_usl, double diam, char po, double t1, double t2, double tn, double tg, bool kolvork);

double get_qq(int kod, char po, char typ_pr, int dateend, 
  double diametr_usl, double diametr, double tol, int kolwork, double Tn, double T1, double T2, double Tg)
{
  double qq;

  int y = 0;
  if (dateend < 1990) y = 1;
  else if (dateend < 1998) y = 2;
  else if (dateend <= 2003) y = 3;
  else y = 4;

  qq = get_q_new(y, typ_pr, diametr_usl, diametr+2*tol, po, T1, T2, Tn, Tg, kolwork);

  return qq;
}


double getQ(char typ_pr, double dlina, double diametr, double tol, double qq, double kti)
{
  double beta, Q;

  beta = get_beta(typ_pr, diametr+2*tol);
  Q = qq*dlina*beta*kti/1.e6;  // Нормативная среднегодовая Гкал
  
  return Q;
}


double dtUT(int kod, char po, char typ_pr, double dlina, double diametr, double diametr_usl, double tol, int year, int kolwork, double Tn, double T1, double T2, double Tg, double ras, double kti)
{
  double qq = get_qq(kod, po, typ_pr, year, diametr_usl, diametr, tol, kolwork, Tn, T1, T2, Tg);
  double Q = getQ(typ_pr, dlina, diametr, tol, qq, kti);

  return Q*1000/ras;
}


double ktiUT(int kod, char po, char typ_pr, double dlina, double diametr, double diametr_usl, double tol, int year, int kolwork, double Tn, double T1, double T2, double Tg, double dt, double ras) 
{
  double dq;
  int truba = 1;

  double kti, kti1 = 0, kti2 = 10;

//  dt = 1.8;
//  ras = 56.2;

  if (dt == 0 || ras == 0) return 0;

  double dq1 = dtUT(kod, po, typ_pr, dlina, diametr, diametr_usl, tol, year, kolwork, Tn, T1, T2, Tg, ras, kti1);
  double dq2 = dtUT(kod, po, typ_pr, dlina, diametr, diametr_usl, tol, year, kolwork, Tn, T1, T2, Tg, ras, kti2);

  if (dt < dq1) return kti1;
  if (dt > dq2) return kti2;


  while (kti2-kti1 > 0.00001) {
    kti = (kti1+kti2)/2;

    dq = dtUT(kod, po, typ_pr, dlina, diametr, diametr_usl, tol, year, kolwork, Tn, T1, T2, Tg, ras, kti);

    if (dt < dq) {
      kti2 = kti;
    }
    else {
      kti1 = kti;
    }
  }


  return kti;
}




CKti::CKti(CWnd* pParent, int id2, int kod, double dlina, double diametr, double diametr_usl, double tol, int year, int tubingTypeID, int kolwork,
    CTime date_isp_tep, double tn_isp_tep, double tg_isp_tep, double G1_isp_tep, double G2_isp_tep, double dt1_isp_tep, double dt2_isp_tep, double t1_isp_tep, double t2_isp_tep 

)
  : CDialog(CKti::IDD, pParent)
{
  //{{AFX_DATA_INIT(CKti)
  m_dt = 0.0;
  m_typ_pr = -1;
  m_tol = tol;
  m_dlina = dlina;
  m_diametr = diametr;
  m_diametr_usl = diametr_usl;
  m_dateend = year;
  m_ras = 0.0;
  m_kti = "";
  m_kolwork = kolwork;
  m_dt2 = 0.0;
  m_rasO = 0.0;
  m_t1 = 0.0;
  m_t2 = 0.0;
  m_tg = 0.0;
  m_tn = 0.0;
  m_tpn = 0.0;
  m_tpf = 0.0;
  m_radio = 0;
  m_ktiP = "";
  m_ktiO = "";
  //}}AFX_DATA_INIT

//t char *typ_pr, int kolwork)

  m_kod = kod;

  switch(tubingTypeID) {
  case 2 /*'Б'*/ : m_typ_pr = 0; break;
  case 1 /*'К'*/ : m_typ_pr = 1; break;
  case 4 /*'Н'*/ : m_typ_pr = 2; break;
  case 3 /*'П'*/ : m_typ_pr = 3; break;
  }

  m_tn = tn_isp_tep;
  m_tg = tg_isp_tep;
  m_ras = G1_isp_tep;
  m_rasO = G2_isp_tep;
  m_dt = dt1_isp_tep;
  m_dt2 = dt2_isp_tep;
  m_t1 = t1_isp_tep;
  m_t2 = t2_isp_tep;


  //  get_param_kti(m_kod, m_dlina, m_diametr, m_diametr_usl, m_tol, m_dateend, m_typ_pr, m_kolwork);

  bool getTG(long hSourceID, double tn, double &t1, double &t2, double &t3, double &tv);
  bool readTG(CAdoFile *ado);

  open_vsp2("sprav");
  open_vsp3("sprav");

  CAdoFile *ado = getAdo(getGidAdoName());

  if (!ado) return;

  readTG(ado);

  double t3, tv;
  CGidrView *pView = getView();
  if (pView) {
      const RasCx *cx = pView->m_cxema->getPC(kod);
  
      CLINE2 *l2 = pView->m_cxema->m_graph->findLine2(TIP_UT, id2);

      if (l2) {
        m_ras = fabs(bline(l2)->line.pod.q);
        m_rasO = fabs(bline(l2)->line.obr.q);
      }

      if (cx) {
          const Fragments *fr = pView->m_cxema->getFragments(cx->fileID);

          if (fr) {
              m_tn = fr->m_tn_god;
              m_tg = fr->m_tg_god;
              if (!getTG(cx->heatSourceID, m_tn, m_t1, m_t2, t3, tv)) {
              }
          }
      }
  }
}


void CKti::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CKti)
  DDX_Control(pDX, IDC_DT, m_e1);
  DDX_Control(pDX, IDC_TOL, m_e2);
  DDX_Control(pDX, IDC_LEN, m_e3);
  DDX_Control(pDX, IDC_DIAMETR, m_e4);
  DDX_Control(pDX, IDC_DIAMETR_USL, m_e5);
  DDX_Control(pDX, IDC_DATEEND, m_e6);
  DDX_Control(pDX, IDC_RAS, m_e7);
  DDX_Control(pDX, IDC_KTI, m_e8);
  DDX_Control(pDX, IDC_DT2, m_e9);
  DDX_Control(pDX, IDC_RAS2, m_e10);
  DDX_Control(pDX, IDC_T1, m_e11);
  DDX_Control(pDX, IDC_T2, m_e12);
  DDX_Control(pDX, IDC_TG, m_e13);
  DDX_Control(pDX, IDC_TN, m_e14);
  DDX_Control(pDX, IDC_TPN, m_e15);
  DDX_Control(pDX, IDC_TPF, m_e16);
  DDX_Control(pDX, IDC_KTI1, m_e17);
  DDX_Control(pDX, IDC_KTI2, m_e18);
  DDX_Control(pDX, IDOK, m_b1);
  DDX_Control(pDX, IDCANCEL, m_b2);
  DDX_Control(pDX, IDC__HELP, m_b3);
  DDX_Control(pDX, IDC_SAVE, m_b4);

  DDX_Control(pDX, IDC_TYP_PR, m_combo);

  DDX_Text(pDX, IDC_DT, m_dt);
  DDX_CBIndex(pDX, IDC_TYP_PR, m_typ_pr);
  DDX_Text(pDX, IDC_TOL, m_tol);
  DDX_Text(pDX, IDC_LEN, m_dlina);
  DDX_Text(pDX, IDC_DIAMETR, m_diametr);
  DDX_Text(pDX, IDC_DIAMETR_USL, m_diametr_usl);
  DDX_Text(pDX, IDC_DATEEND, m_dateend);
  DDX_Text(pDX, IDC_RAS, m_ras);
  DDX_Text(pDX, IDC_KTI, m_kti);
  DDX_Check(pDX, IDC_KOLWORK, m_kolwork);
  DDX_Text(pDX, IDC_DT2, m_dt2);
  DDX_Text(pDX, IDC_RAS2, m_rasO);
  DDX_Text(pDX, IDC_T1, m_t1);
  DDX_Text(pDX, IDC_T2, m_t2);
  DDX_Text(pDX, IDC_TG, m_tg);
  DDX_Text(pDX, IDC_TN, m_tn);
  DDX_Text(pDX, IDC_TPN, m_tpn);
  DDX_Text(pDX, IDC_TPF, m_tpf);
  DDX_Radio(pDX, IDC_RADIO1, m_radio);
  DDX_Text(pDX, IDC_KTI1, m_ktiP);
  DDX_Text(pDX, IDC_KTI2, m_ktiO);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKti, CDialog)
  //{{AFX_MSG_MAP(CKti)
  ON_EN_CHANGE(IDC_DATEEND, Raschet)
  ON_EN_CHANGE(IDC_DIAMETR_USL, Raschet)
  ON_EN_CHANGE(IDC_DT, Raschet)
  ON_EN_CHANGE(IDC_DT2, Raschet)
  ON_EN_CHANGE(IDC_LEN, Raschet)
  ON_EN_CHANGE(IDC_RAS, Raschet)
  ON_EN_CHANGE(IDC_RAS2, Raschet)
  ON_EN_CHANGE(IDC_T1, Raschet)
  ON_EN_CHANGE(IDC_T2, Raschet)
  ON_EN_CHANGE(IDC_TG, Raschet)
  ON_EN_CHANGE(IDC_TN, Raschet)
  ON_EN_CHANGE(IDC_TOL, Raschet)
  ON_BN_CLICKED(IDC_RADIO1, Raschet)
  ON_BN_CLICKED(IDC_RADIO2,Raschet)
  ON_BN_CLICKED(IDC_KOLWORK, Raschet)
  ON_CBN_SELCHANGE(IDC_TYP_PR, Raschet)
  ON_BN_CLICKED(IDC_SAVE, OnSave)
  ON_WM_CTLCOLOR()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKti message handlers

//  DDX_CBIndex(pDX, IDC_TYP_PR, m_typ_pr);
//  DDX_Check(pDX, IDC_KOLWORK, m_kolwork);

const char *bknp = "БКНП";


static double ktiP1, ktiO1, kti1;


BOOL CKti::OnInitDialog() 
{
  CDialog::OnInitDialog();

  Raschet();
  
  // TODO: Add extra initialization here
  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CKti::Raschet()
{
  UpdateData(TRUE);

  CString str;

  GetDlgItem(IDC_TN)->GetWindowText(str);

  if (str == "-") return;
  if (m_typ_pr < 0 || m_typ_pr >= 4) return;

  double kti;
  double ktiP =  ktiUT(m_kod, 'П', bknp[m_typ_pr], m_dlina, m_diametr, m_diametr_usl, m_tol, m_dateend, m_kolwork, m_tn, m_t1, m_t2, m_tg, m_dt, m_ras);
  double ktiO =  ktiUT(m_kod, 'О', bknp[m_typ_pr], m_dlina, m_diametr, m_diametr_usl, m_tol, m_dateend, m_kolwork, m_tn, m_t1, m_t2, m_tg, m_dt2, m_rasO);

  double qq = 0, qqP = 0, qqO = 0;

  qqP = get_qq(m_kod, 'П', bknp[m_typ_pr], m_dateend, m_diametr_usl, m_diametr, m_tol, m_kolwork, m_tn, m_t1, m_t2, m_tg);
  qqO = get_qq(m_kod, 'О', bknp[m_typ_pr], m_dateend, m_diametr_usl, m_diametr, m_tol, m_kolwork, m_tn, m_t1, m_t2, m_tg);

//  double yP = Y(char typ_pr, double dlina, double diametr, double tol, double qq, double kti)


  double beta = get_beta(bknp[m_typ_pr], m_diametr+2*m_tol);

  if (bknp[m_typ_pr] == 'Н') {


    ktiP1 = ktiP;
    ktiO1 = ktiO;

    str.Format("%g", ktiP);
    GetDlgItem(IDC_KTI1)->SetWindowText(str);

//    m_ktiP = str;

    str.Format("%g", ktiO);
    GetDlgItem(IDC_KTI2)->SetWindowText(str);

//    m_ktiO = str;


    GetDlgItem(IDC_TN)->EnableWindow(TRUE);
    GetDlgItem(IDC_TG)->EnableWindow(FALSE);

    if (m_radio == 0) {
      str.Format("%g", qqP);
    }
    else {
      str.Format("%g", qqO);
    }

    GetDlgItem(IDC_TPN)->SetWindowText(str);


    if (m_radio == 0) {
      str.Format("%g", m_dt*m_ras*1000/m_dlina/beta);
    }
    else {
      str.Format("%g", m_dt2*m_rasO*1000/m_dlina/beta);
    }

    GetDlgItem(IDC_TPF)->SetWindowText(str);
  
  }
  else {
    if (ktiP > 0 && ktiO > 0) {
      kti = (ktiP+ktiO)/2;
    }
    else {
      kti = ktiP+ktiO;
    }

    kti1 = kti;

    str.Format("%g", kti);
    GetDlgItem(IDC_KTI)->SetWindowText(str);
    GetDlgItem(IDC_TN)->EnableWindow(FALSE);
    GetDlgItem(IDC_TG)->EnableWindow(TRUE);

    str.Format("%g", (qqP+qqO)/2);
    GetDlgItem(IDC_TPN)->SetWindowText(str);

    str.Format("%g", (m_dt*m_ras+m_dt2*m_rasO)/2*1000/m_dlina/beta);
    GetDlgItem(IDC_TPF)->SetWindowText(str);
  }


  

//  UpdateData(FALSE);

}

void CKti::OnOK() 
{
  UpdateData(TRUE);

  if (bknp[m_typ_pr] == 'Н') {
    if (m_radio == 0) {
      m_kti = m_ktiP;
    }
    else {
      m_kti = m_ktiO;
    }
  }
  else {
//    double ktiP = atof(m_ktiP);
//    double ktiO = atof(m_ktiO);
/*
    double ktiP = ktiP1;
    double ktiO = ktiO1;
    if (ktiP > 0 && ktiO > 0) {
      m_kti.Format("%g", (ktiP+ktiO)/2);
    }
    else {
      m_kti.Format("%g", ktiP+ktiO);
    }
*/

    m_kti.Format("%g", kti1);

//    CString ss;
//    ss.Format(m_kti);
//    AfxMessageBox(ss, MB_OK|MB_ICONINFORMATION);
  }

  UpdateData(FALSE);

  CDialog::OnOK();
}

void CKti::OnSave() 
{
  UpdateData(TRUE);

//  CDialog::OnOK();
  EndDialog(IDC_SAVE);
  
}


HBRUSH CKti::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
    return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
//  return theApp.OnCtlColorDark(pDC,pWnd,nCtlColor);
}
