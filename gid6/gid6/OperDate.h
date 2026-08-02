#pragma once

/////////////////////////////////////////////////////////////////////////////
// COperDate dialog

class COperDate : public CDialog
{
// Construction
public:
  COperDate(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(COperDate)
  enum { IDD = IDD_OPER_DATA };
  CComboBox m_table;
  CListBox  m_List;
  CTime m_date;
  int   m_Index;
  CString m_strText;
  CString m_strTextTable;
  //}}AFX_DATA

  CArray<CString,CString> lst;
  CArray<CString,CString> lst_t;
  map<CString,void *> map_menu;

  void Add(const char *text, void *v = NULL);
  void *getV(const char *text);
  void *getV();


  void Add2(const char *text, void *v = NULL);
  void *getV2(const char *text);
  void *getV2();



// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(COperDate)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(COperDate)
  afx_msg void OnDblclkList();
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

