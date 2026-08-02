#if !defined(AFX_CHPASSWORDRIGHT_H__98E6DE8E_259B_42A8_BAAC_41660A510E01__INCLUDED_)
#define AFX_CHPASSWORDRIGHT_H__98E6DE8E_259B_42A8_BAAC_41660A510E01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChPasswordRight.h : header file
//

#include "LoginDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CChPasswordRight dialog

class CChPasswordRight : public CDialog
{
// Construction
public:
  CChPasswordRight(CWnd* pParent, const char *user, const CUser *pwd);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CChPasswordRight)
  enum { IDD = IDD_CH_PASSWORD_RIGHT };
  CCheckListBox  m_list;
  CString m_user;
  //}}AFX_DATA

  int m_r[256];

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CChPasswordRight)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CChPasswordRight)
  virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHPASSWORDRIGHT_H__98E6DE8E_259B_42A8_BAAC_41660A510E01__INCLUDED_)
