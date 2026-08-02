#pragma once

/////////////////////////////////////////////////////////////////////////////
// CInputDialogDate dialog

class CInputDialogDate : public CDialog
{
// Construction
public:
  CInputDialogDate(CWnd* pParent, CString capt, CString pr, UINT nIDTemplate = IDD);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CInputDialogDate)
  enum { IDD = IDD_INPUTDIALOG_DATE };
  CString m_strPrompt;
  CTime	m_date1;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CInputDialogDate)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  CString m_strCaption;

  // Generated message map functions
  //{{AFX_MSG(CInputDialogDate)
  virtual BOOL OnInitDialog();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};
