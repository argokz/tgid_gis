#if !defined(AFX_CHPASSWORDRIGHTNEW_H__ED1F1AAB_CACC_4F13_A213_B9494F2975E2__INCLUDED_)
#define AFX_CHPASSWORDRIGHTNEW_H__ED1F1AAB_CACC_4F13_A213_B9494F2975E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChPasswordRightNew.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChPasswordRightNew dialog

class CChPasswordRightNew : public CDialog
{
// Construction
public:
  CChPasswordRightNew(CWnd* pParent, const char *user = "", const char *password = "", int right = 0);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CChPasswordRightNew)
  enum { IDD = IDD_CH_PASSWORD_RIGHT_NEW };
  CCheckListBox  m_list;
  CString m_password;
  CString m_password2;
  CString m_user;
  //}}AFX_DATA

  int m_r[256];


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CChPasswordRightNew)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CChPasswordRightNew)
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHPASSWORDRIGHTNEW_H__ED1F1AAB_CACC_4F13_A213_B9494F2975E2__INCLUDED_)
