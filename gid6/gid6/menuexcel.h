#pragma once

#include "MMenu.h"

/////////////////////////////////////////////////////////////////////////////
// CMenyExcel dialog

class CMenuExcel : public CMMenu
{
// Construction
public:                                                 
  CMenuExcel(CWnd* pParent, const char *capt, CCxema *cxema);

// Dialog Data
  //{{AFX_DATA(CMenuExcel)
  enum { IDD = IDD_MENU_EXCEL };
  int   m_out;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMenuExcel)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  CCxema *m_cxema;

  // Generated message map functions
  //{{AFX_MSG(CMenuExcel)
  afx_msg void OnDblclkList();
  afx_msg void OnOut();
  afx_msg void OnOK();
  virtual BOOL OnInitDialog();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};
