#if !defined(AFX_PARAM2_H__F6622567_ABC1_46D9_A4C7_0E0E357B3C83__INCLUDED_)
#define AFX_PARAM2_H__F6622567_ABC1_46D9_A4C7_0E0E357B3C83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Param.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CParam2 dialog

//#include "CoolControls/CoolButton.h"

class CParam2 : public CDialog
{
// Construction
public:
  CParam2(CWnd* pParent);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CParam2)
  enum { IDD = IDD_PARAM2 };
  CButton m_idcancel;
  CButton m_idok;
  CString m_name;
  BOOL  m_char_sety;
  double  m_Tn;
  int   m_nGWS;
  BOOL  m_teplopoter;
  BOOL  m_teplovyd;
  BOOL  m_ur;
  int   m_nGWS2;
  BOOL  m_veter;
  CString m_mag_fn;
  int   m_detaliz;
  BOOL  m_avtomat;
  BOOL  m_save_po;
  BOOL  m_leto;
  BOOL  m_mag_fragment;
  BOOL  m_kv;
  //}}AFX_DATA

  map <CString, double> map_t_ist;

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CParam2)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  //}}AFX_VIRTUAL

// Implementation
protected:

  void set_par();

  // Generated message map functions
  //{{AFX_MSG(CParam2)
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnObzor();
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg void OnMagBut();
  afx_msg void OnLeto();
  afx_msg void OnTIst();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedMagFragment();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARAM2_H__F6622567_ABC1_46D9_A4C7_0E0E357B3C83__INCLUDED_)
