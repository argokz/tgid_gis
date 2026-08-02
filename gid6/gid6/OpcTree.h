#pragma once

#include "opc.h"


/////////////////////////////////////////////////////////////////////////////
// COpcTree dialog

class COpcTree : public CDialog
{
// Construction
public:
  COpcTree(CWnd* pParent, OPCServer *server);   // standard constructor

// Dialog Data
  //{{AFX_DATA(COpcTree)
  enum { IDD = IDD_OPCTREE };
  CTreeCtrl m_tree;
  //}}AFX_DATA

  CString str;

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(COpcTree)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  OPCServer *m_server;
  // Generated message map functions
  //{{AFX_MSG(COpcTree)
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};
