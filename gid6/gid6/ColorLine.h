#pragma once

class CAdoFile;

// ColorLine.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorLine dialog

class CColorLine : public CDialog
{
// Construction
public:
  CColorLine(CWnd* pParent, CAdoFile *db, const TCHAR *name, const TCHAR *cod);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CColorLine)
  enum { IDD = IDD_COLOR_LINE };
  CComboBox m_combo;
  CButton   m_color1;
  CButton   m_color2;
  CButton   m_color3;
  double  m_d1;
  double  m_d2;
  double  m_d3;

  double  m_d1_d;
  double  m_d2_d;
 
  double  m_do1_d;
  double  m_do2_d;

  CButton   m_coloro1;
  CButton   m_coloro2;
  CButton   m_coloro3;
  double  m_do1;
  double  m_do2;
  double  m_do3;
  int   m_typ;
  //}}AFX_DATA

  CString m_fn;
  bool m_out;
  double m_dd[6];
  unsigned long m_color[6];

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CColorLine)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  CAdoFile *m_ado;
  CString m_strTitle;
  CString m_cod;
  HACCEL m_hAccel;                     // dialog accelerators

  CBrush *m_brush[6];


  void get_colors(const char *fn);
  void init_colors();

  // Generated message map functions
  //{{AFX_MSG(CColorLine)
  virtual BOOL OnInitDialog();
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  virtual void OnOK();
  virtual void OnSelchangeCombo1();
  virtual void OnChange();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

