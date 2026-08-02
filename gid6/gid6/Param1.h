#if !defined(AFX_PARAM1_H__DAC2BBF2_FB6F_4865_BE93_2318BEA42C5C__INCLUDED_)
#define AFX_PARAM1_H__DAC2BBF2_FB6F_4865_BE93_2318BEA42C5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Param1.h : header file
//

#include "global.h"


/////////////////////////////////////////////////////////////////////////////
// CParam1 dialog

class CParam1 : public CDialog
{
// Construction
public:
  CParam1(CWnd* pParent);   // standard constructor
  CParam1(CWnd* pParent, const GLOBAL *golbal, double Tn);   // standard constructor


  bool is_read_registr;
  bool is_save_registr;
  

// Dialog Data
  //{{AFX_DATA(CParam1)
  enum { IDD = IDD_PARAM1 };
  CString m_name;
  int   m_kol_iter;
  BOOL  m_char_sety;
  BOOL  m_dross;
  BOOL  m_avtomat;
  BOOL  m_teplopoter;
  BOOL  m_teplovyd;
  BOOL m_uf_calc;

  BOOL  m_zn0;
  BOOL  m_utechki;


  double  m_Tn;
  int   m_ur;
  int   m_fiks;
  int   m_ekspl;
  CString m_n_fil;
  BOOL  m_sav_off_sys;
  BOOL  m_sav_on_sys;
  BOOL  m_veter;
  BOOL  m_sav_off;
  BOOL  m_sav_on;
  CString m_mag_fn;
  BOOL  m_save_po;
  BOOL  m_uf_new;
  BOOL  m_leto;
  BOOL  m_kv;
  BOOL  m_plan;
  BOOL  m_mag_fragment;
  int   m_metod;
  int   m_trtp;
  int   m_nagr;
  //}}AFX_DATA

  map <CString, double> map_t_ist;

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CParam1)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  //}}AFX_VIRTUAL

// Implementation
protected:

  void set_par();

  // Generated message map functions
  //{{AFX_MSG(CParam1)
  virtual void OnOK();
  virtual BOOL OnInitDialog();
  afx_msg void OnMagBut();
  afx_msg void OnTIst();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedUr0();
    afx_msg void OnBnClickedTeplopoter();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARAM1_H__DAC2BBF2_FB6F_4865_BE93_2318BEA42C5C__INCLUDED_)
