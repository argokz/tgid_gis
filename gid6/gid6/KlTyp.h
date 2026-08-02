#pragma once

#include "RastrWnd.h"


/////////////////////////////////////////////////////////////////////////////
// CKlTyp dialog

class CKlTyp : public CDialog
{
// Construction
public:
  CKlTyp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CKlTyp)
  enum { IDD = IDD_KLTYP };
  CRastrWnd m_rastr;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CKlTyp)
  public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CKlTyp)
  virtual void OnOK();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};
