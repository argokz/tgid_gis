#pragma once

#include "RastrBut.h"


/////////////////////////////////////////////////////////////////////////////
// CKlEdit dialog

class CKlEdit : public CDialog
{
// Construction
public:
  CKlEdit(CWnd* pParent, Klassif *kl);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CKlEdit)
  enum { IDD = IDD_KLASSIF_EDIT };
  CComboBox m_ctrlListLabelColor;

  CComboBox m_ctrlLoc;
  int m_nLoc;

  CButton m_ctrlPunct;
  CStatic m_width_static;
  CStatic m_color_w2;
  CButton m_color2;
  CEdit m_width_ctrl;
  CRastrBut m_rastr;
  CSpinButtonCtrl m_SpinPrio;
  CComboBox m_ctrlListLabel;
  CSpinButtonCtrl m_SpinWidth;
  CString m_name;
  CString m_rus;
  double  m_dWidth;
  int   m_loc;
  int   m_loc10;
  int   m_nListLabel;
  BOOL  m_bLabel;
  int   m_nPrio;
  int   m_typ;
  int   m_potr;
  BOOL  m_ramka;
  BOOL  m_punct;
  BOOL  m_check_color;
  BOOL  m_label_color;
  int   m_nListLabelColor;
  BOOL  m_nomas;

  int m_mas1;
  int m_mas2;
  int m_mas_text1;
  int m_mas_text2;

  //}}AFX_DATA

  CBrush *m_brush1;
  CBrush *m_brush2;
  CBrush *m_brush3;


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CKlEdit)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  Klassif *m_kl;

  void setLoc(int loc);

  map<CString, CString> map_fn;

  // Generated message map functions
  //{{AFX_MSG(CKlEdit)
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnColor();
  afx_msg void OnColor2();
  afx_msg void OnFont();
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg void OnSelchangeLoc();
  afx_msg void OnRastr();
  afx_msg void OnRusSave();
  afx_msg void OnRusLoad();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedPunct();
};

