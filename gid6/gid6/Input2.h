#pragma once

/////////////////////////////////////////////////////////////////////////////
// CInput2 dialog

class CInput2 : public CDialog
{
// Construction
public:
  CInput2(map <CString, CString> &map_ist, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CInput2)
  enum { IDD = IDD_CH_KODE };
  CComboBox m_combo;
  CString m_edit1;
  CString m_edit2;
  int   m_n;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CInput2)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  map <CString, CString> m_map_ist;

  // Generated message map functions
  //{{AFX_MSG(CInput2)
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

