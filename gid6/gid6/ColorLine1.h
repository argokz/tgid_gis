#pragma once

/////////////////////////////////////////////////////////////////////////////
// CColorLine1 dialog

class CAdoFile;

class CColorLine1 : public CDialog
{
// Construction
public:
  CColorLine1(CWnd* pParent, CAdoFile *ado, const TCHAR *name, const TCHAR *cod);

// Dialog Data
  //{{AFX_DATA(CColorLine1)
  enum { IDD = IDD_COLOR_LINE1 };
  CComboBox m_combo;
  CButton   m_color1;
  CButton   m_color2;
  CButton   m_color3;
  double  m_d1;
  double  m_d2;
  double  m_d3;

  double  m_d1_d;
  double  m_d2_d;
  
  int   m_typ;
  //}}AFX_DATA

  CString m_fn;
  double m_dd[3];
  unsigned long m_color[3];

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CColorLine1)
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
  //{{AFX_MSG(CColorLine1)
  virtual BOOL OnInitDialog();
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  virtual void OnOK();
  virtual void OnSelchangeCombo1();
  virtual void OnChange();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};
