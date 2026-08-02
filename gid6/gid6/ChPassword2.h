#if !defined(AFX_CHPASSWORD2_H__3C598948_D742_4023_BCF7_26DC9567B5CB__INCLUDED_)
#define AFX_CHPASSWORD2_H__3C598948_D742_4023_BCF7_26DC9567B5CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChPassword2.h : header file
//

#include "LoginDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CChPassword2 dialog

class CChPassword2 : public CDialog
{
// Construction
public:
  CChPassword2(CWnd* pParent, const char *user, const CUser *pwd);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CChPassword2)
  enum { IDD = IDD_CH_PASSWORD2 };
  CString m_oldpassword;
  CString m_password;
  CString m_password2;
  //}}AFX_DATA

  CString m_oldpassword_pr;

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CChPassword2)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CChPassword2)
  virtual void OnOK();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHPASSWORD2_H__3C598948_D742_4023_BCF7_26DC9567B5CB__INCLUDED_)
