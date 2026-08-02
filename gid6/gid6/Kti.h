#pragma once

/////////////////////////////////////////////////////////////////////////////
// CKti dialog

//#include "CoolControls\\CoolButton.h"
//#include "CoolControls\\CoolEdit.h"

//#include "dialogwitthhelp.h"


class CKti : public CDialog
{
// Construction
public:
  CKti(CWnd* pParent, int id2, int kod, double dlina, double diametr, double diametr_usl, double tol, int year, int tubingTypeID, int kolwork,
    CTime date_isp_tep, double tn_isp_tep, double tg_isp_tep, double G1_isp_tep, double G2_isp_tep, double dt1_isp_tep, double dt2_isp_tep, double t1_isp_tep, double t2_isp_tep 
  );  

// Dialog Data
  //{{AFX_DATA(CKti)
  enum { IDD = IDD_KTI };

  CEdit   m_e1;
  CEdit   m_e2;
  CEdit   m_e3;
  CEdit   m_e4;
  CEdit   m_e5;
  CEdit   m_e6;
  CEdit   m_e7;
  CEdit   m_e8;
  CEdit   m_e9;
  CEdit   m_e10;
  CEdit   m_e11;
  CEdit   m_e12;
  CEdit   m_e13;
  CEdit   m_e14;
  CEdit   m_e15;
  CEdit   m_e16;
  CEdit   m_e17;
  CEdit   m_e18;

  CButton m_b1;
  CButton m_b2;
  CButton m_b3;
  CButton m_b4;

  CComboBox m_combo;


  double  m_dt;
  int   m_typ_pr;
  double  m_tol;
  double  m_dlina;
  double  m_diametr;
  double  m_diametr_usl;
  int   m_dateend;
  double  m_ras;
  CString m_kti;
  int m_kod;
  BOOL  m_kolwork;
  double  m_dt2;
  double  m_rasO;
  double  m_t1;
  double  m_t2;
  double  m_tg;
  double  m_tn;
  double  m_tpn;
  double  m_tpf;
  int   m_radio;
  CString m_ktiP;
  CString m_ktiO;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CKti)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CKti)
  virtual BOOL OnInitDialog();
  virtual void Raschet();
  virtual void OnOK();
  afx_msg void OnSave();
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};
