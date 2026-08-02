#if !defined(AFX_PROTOCOL_H__00C6E417_D5AC_4C7A_B420_65FAF8C5EDA9__INCLUDED_)
#define AFX_PROTOCOL_H__00C6E417_D5AC_4C7A_B420_65FAF8C5EDA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Protocol.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProtocol dialog

class CProtocol : public CPaneDialog
{
// Construction
public:
  CProtocol();   // standard constructor

// Dialog Data
  //{{AFX_DATA(CProtocol)
  enum { IDD = IDD_OUTPUT };
  CEdit m_ctrl;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CProtocol)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  CString file_name;
  CFont m_font;

  // Generated message map functions
  //{{AFX_MSG(CProtocol)
  afx_msg LRESULT HandleInitDialog(WPARAM, LPARAM);
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point) {};
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROTOCOL_H__00C6E417_D5AC_4C7A_B420_65FAF8C5EDA9__INCLUDED_)
