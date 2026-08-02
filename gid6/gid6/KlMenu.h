#pragma once

#include "GeoFile.h"

//

/////////////////////////////////////////////////////////////////////////////
// CKlMenu dialog

class CKlMenu : public CDialog
{
// Construction
public:
  Klassif *m_kls;
  CKlMenu(CWnd* pParent, CGeoFile *gf, const char *title, int loc = -1);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CKlMenu)
  enum { IDD = IDD_KLMENU };
  CListBox  m_klList;
  int   m_klIndex;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CKlMenu)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  KlList *m_kl_list;
  int m_multy;
  CString m_strCaption;

  // Generated message map functions
  //{{AFX_MSG(CKlMenu)
  afx_msg void OnDblclk1000();
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};
