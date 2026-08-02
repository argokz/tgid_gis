#pragma once

/////////////////////////////////////////////////////////////////////////////
// CInputDialog dialog

class CInputDialog : public CDialog
{
// Construction
public:
  CInputDialog(CWnd* pParent, CString capt, CString pr, CString& inp, UINT nIDTemplate = IDD);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CInputDialog)
  enum { IDD = IDD_INPUTDIALOG };
  CString m_strPrompt;
  CString m_strInput;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CInputDialog)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  CString m_strCaption;

  // Generated message map functions
  //{{AFX_MSG(CInputDialog)
  virtual BOOL OnInitDialog();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};
