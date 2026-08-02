#pragma once

/////////////////////////////////////////////////////////////////////////////
// CInf2 dialog

class CInf2 : public CDialog
{
// Construction
public:
  CInf2(CWnd* pParent, int typ, int nInf, BOOL *bIsInfo);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CInf2)
  enum { IDD = IDD_INF2 };
//  CCoolButton m_idok;
//  CCoolButton m_idcancel;
  CButton m_idok;
  CButton m_idcancel;
  CCheckListBox  m_list;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CInf2)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  BOOL *m_bIsInfo;
  int m_nInf;
  int m_typ;

  // Generated message map functions
  //{{AFX_MSG(CInf2)
  virtual BOOL OnInitDialog();
  virtual void OnOK();
//  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

