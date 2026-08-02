#pragma once

#include "OPCThread2.h"

class COPCThread2;


/////////////////////////////////////////////////////////////////////////////
// COpcParam dialog

class COpcParam : public CDialog
{
// Construction
public:
  COpcParam(const CString &opc1, const CString &opc2, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(COpcParam)
  enum { IDD = IDD_OPC_PARAM };
  CString m_opc1;
  CString m_opc2;
  CString m_out1;
  CString m_out2;
  double  m_a1;
  double  m_a2;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(COpcParam)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  UINT m_nTimer;
  COPCThread2 *m_pOPCThread;

  // Generated message map functions
  //{{AFX_MSG(COpcParam)
  afx_msg void OnButton1();
  afx_msg void OnButton2();
  virtual void OnOK();
  virtual BOOL OnInitDialog();
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnClose();
  afx_msg void OnChangeEdit();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};
