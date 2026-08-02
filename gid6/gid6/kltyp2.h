#pragma once

#include "RastrWnd2.h"


/////////////////////////////////////////////////////////////////////////////
// CKlTyp2 dialog

class CKlTyp2 : public CDialog
{
// Construction
public:
  CKlTyp2(CWnd* pParent);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CKlTyp2)
  enum { IDD = IDD_KLTYP };
  CRastrWnd2 m_rastr;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CKlTyp2)
  public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CKlTyp2)
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};
