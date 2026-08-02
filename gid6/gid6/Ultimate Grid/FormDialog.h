#pragma once


#include "FormCug.h"

#include "resource.h"

class CFormDialog : public CDialog
{
public:

  CFormDialog(CWnd *wnd, const char *cap);

  void addRow(const char *name, const char *label, const char *razmer);

  enum { IDD = IDD_VIRTUAL_DLG };

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CFormDialog)
  public:
  protected:
  //}}AFX_VIRTUAL

// Implementation
public:
  virtual ~CFormDialog();
  CString  m_strRet;
  CString m_cap;
  CString m_q;

protected:
  FormCug m_grid;

  CRect m_drawRect;

  int m_n_ret;
  
  CString m_profile;


// Generated message map functions
protected:
  //{{AFX_MSG(CFormDialog)
  afx_msg BOOL OnInitDialog();
  virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
  virtual void OnSize(UINT nType, int cx, int cy);
  virtual void OnSetFocus(CWnd* pOldWnd);
  virtual BOOL OnEraseBkgnd(CDC* pDC);
  virtual void OnShiftF1();
  virtual void OnDestroy();
  virtual void OnOK();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


