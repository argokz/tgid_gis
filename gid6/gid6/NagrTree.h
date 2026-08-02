#pragma once

#include "graph.h"
#include "Graph2.h"

/////////////////////////////////////////////////////////////////////////////
// CNagrTree dialog

class CNagrTree : public CDialog
{
// Construction
public:
  CNagrTree(CWnd* pParent, CNode2 *node);   // standard constructor
                                
// Dialog Data
  //{{AFX_DATA(CNagrTree)
  enum { IDD = IDD_NAGR_TREE };
  CTreeCtrl m_tree;
  double  m_gvs;
  double  m_otopl;
  double  m_vent;
  double  m_all;
  CString m_name;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CNagrTree)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  CNode2 *m_node;
  CString m_profile;
  CFont m_Font;

  // Generated message map functions
  //{{AFX_MSG(CNagrTree)
  virtual BOOL OnInitDialog();
  virtual void OnTab();
  virtual void OnSize(UINT nType, int cx, int cy);
  virtual void OnDestroy();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

