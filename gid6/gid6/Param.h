#if !defined(AFX_PARAM_H__F6622567_ABC1_46D9_A4C7_0E0E357B3C83__INCLUDED_)
#define AFX_PARAM_H__F6622567_ABC1_46D9_A4C7_0E0E357B3C83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Param.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CParam dialog

//#include "CoolControls/CoolButton.h"


class CParam : public CDialog
{
// Construction
public:
  CParam(CWnd* pParent);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CParam)
  enum { IDD = IDD_PARAM };
  CButton m_obzor;
  CButton m_idcancel;
  CButton m_idok;
  int   m_fiks;
  int   m_kol_iter;
  CString m_n_fil;
  int   m_ekspl;
  BOOL  m_char_sety;
  BOOL  m_sav_off_sys;
  BOOL  m_sav_on_sys;
  double  m_Tn;
  BOOL  m_dross;
  BOOL  m_avtomat;
  BOOL  m_teplopoter;
  BOOL  m_teplovyd;
  BOOL  m_ur;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CParam)
	protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

  void set_par();

  // Generated message map functions
  //{{AFX_MSG(CParam)
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnObzor();
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARAM_H__F6622567_ABC1_46D9_A4C7_0E0E357B3C83__INCLUDED_)
