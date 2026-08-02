#pragma once

#include "RastrWndVect.h"


/////////////////////////////////////////////////////////////////////////////
// CKlTypVect dialog

class CKlTypVect : public CDialog
{
// Construction
public:
  CKlTypVect(CWnd* pParent);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CKlTypVect)
  enum { IDD = IDD_KLTYP_VECT };
  CRastrWndVect m_rastr;
  //}}AFX_DATA

  BOOL m_is_white;

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CKlTypVect)
  public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CKlTypVect)
  afx_msg void OnLine();
  afx_msg void OnRing();
  afx_msg void OnArc();
  afx_msg void OnCircle();
  afx_msg void OnClear();
  afx_msg void OnPolygon();
  afx_msg void OnColor();
  afx_msg void OnRestore();
  afx_msg void OnDel();
  virtual void OnOK();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

