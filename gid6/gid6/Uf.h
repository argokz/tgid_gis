#pragma once

/////////////////////////////////////////////////////////////////////////////
// CUf dialog

class CUf : public CDialog
{
// Construction
public:
  CUf(CWnd* pParent, double t1, double t2, double t3);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CUf)
  enum { IDD = IDD_UTF };
  double  m_g;
  double  m_g0;
  int   m_m1;
  double  m_t1;
  double  m_t2;
  double  m_t3;
  CString m_uf;
  //}}AFX_DATA

  double m_u;


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CUf)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  void HideShow();

  // Generated message map functions
  //{{AFX_MSG(CUf)
  afx_msg void OnChange();
  afx_msg void OnM1();
  virtual void OnOK();
  virtual BOOL OnInitDialog();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

