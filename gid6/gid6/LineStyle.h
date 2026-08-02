#pragma once

#include "colorsv.h"

/////////////////////////////////////////////////////////////////////////////
// CLineStyle dialog

class CLineStyle : public CDialog
{
// Construction
public:
  CLineStyle(CWnd* pParent, int regim);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CLineStyle)
  enum { IDD = IDD_COLORS2 };
  CColorsWindow m_w;
  int   m_mag_ras;
  int   m_open_close;
  double  m_thickness;
  double  m_distance;
  unsigned long m_color_uzel;
  int   m_typ;
  BOOL  m_style;
  double  m_ring;
  //}}AFX_DATA

  int is_init;
  int m_regim;

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CLineStyle)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  //}}AFX_VIRTUAL

// Implementation
protected:

  void init_data();
  void set_data();

  // Generated message map functions
  //{{AFX_MSG(CLineStyle)
  virtual void OnOK();
  virtual void OnCancel();
  afx_msg void OnObr();
  afx_msg void OnPod();
  virtual BOOL OnInitDialog();
  afx_msg void OnDefault();
  afx_msg void OnDefaultPS();
  afx_msg void OnPodf();
  afx_msg void OnObrf();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};
