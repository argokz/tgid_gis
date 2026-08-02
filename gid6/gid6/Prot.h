#if !defined(AFX_PROT_H__15132B2E_1CAA_43F0_9E60_5E99586CE601__INCLUDED_)
#define AFX_PROT_H__15132B2E_1CAA_43F0_9E60_5E99586CE601__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Prot.h : header file
//

#include "ProtEdit.h"


class COutThread;



/////////////////////////////////////////////////////////////////////////////
// CProt dialog

class CProt : public CPaneDialog
{
// Construction
public:
  CProt();   // standard constructor

// Dialog Data
  //{{AFX_DATA(CProt)
  enum { IDD = IDD_PROTOCOL };
  CProtEdit m_edit;
  CString m_strEdit;
  //}}AFX_DATA

  void setFile(const char *fn);
  HANDLE start_proc(CWnd *wnd, const TCHAR *cmd, const TCHAR *prtn, int id);
  void end_proc();
  void Add(const TCHAR *Str, int lim);
  void resize();


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CProt)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void OnCancel();
  //}}AFX_VIRTUAL

// Implementation
protected:
  COutThread *m_pThread;
  FILE *inf, *outf, *errf;
  HANDLE m_hProcess;
  CWnd *m_wnd;
  CString m_prtn;
  int m_id;

  // Generated message map functions
  //{{AFX_MSG(CProt)
  afx_msg LRESULT HandleInitDialog(WPARAM, LPARAM);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  virtual CTabbedPane* CreateTabbedPane();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROT_H__15132B2E_1CAA_43F0_9E60_5E99586CE601__INCLUDED_)
