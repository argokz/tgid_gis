#if !defined(AFX_TUFIND_H__16F7FA1F_0809_495A_93EC_BE7D07FA8C5A__INCLUDED_)
#define AFX_TUFIND_H__16F7FA1F_0809_495A_93EC_BE7D07FA8C5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TuFind.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTuFind dialog

class CTuFind : public CDialog
{
// Construction
public:
  CTuFind(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CTuFind)
  enum { IDD = IDD_TU_FIND };
  CComboBox m_ctrl_text;
  CString m_text;
  //}}AFX_DATA

  int m_id_nagr;
  int m_id_tu;


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CTuFind)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

  void initList(set<CString> &setp);


// Implementation
protected:

  set<CString> set_str;
  CString m_street;

  // Generated message map functions
  //{{AFX_MSG(CTuFind)
  virtual BOOL OnInitDialog();
  afx_msg void OnEditchangeText();
  afx_msg void OnSelchangeText();
  afx_msg void OnEditupdateText();
  afx_msg void OnSetfocusText();
  afx_msg void OnDropdownText();
  virtual void OnOK();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TUFIND_H__16F7FA1F_0809_495A_93EC_BE7D07FA8C5A__INCLUDED_)
