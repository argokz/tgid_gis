#if !defined(AFX_LOGINDIALOG_H__7A6B429F_AC54_4485_A4DD_8B1022C20FB4__INCLUDED_)
#define AFX_LOGINDIALOG_H__7A6B429F_AC54_4485_A4DD_8B1022C20FB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoginDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLoginDialog dialog

struct CUser
{
  int id;
  CString password;
  int user_right;
};

extern map <CString, CUser> list_user;


class CLoginDialog : public CDialog
{
// Construction
public:
  CLoginDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CLoginDialog)
  enum { IDD = IDD_LOGIN2 };
  CStatic m_ctrl_nazv;
  CComboBox m_ctl_user;
  CString m_password;
  CString m_user;
  CString m_nazv;
  //}}AFX_DATA

  CFont m_font;
  CBrush *m_brush;


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CLoginDialog)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CLoginDialog)
  virtual BOOL OnInitDialog();
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGINDIALOG_H__7A6B429F_AC54_4485_A4DD_8B1022C20FB4__INCLUDED_)
