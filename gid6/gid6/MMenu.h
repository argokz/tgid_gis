#pragma once

/////////////////////////////////////////////////////////////////////////////
// CMMenu dialog


class CMMenu : public CDialog
{
// Construction
public:
//  CMMenu(CWnd* pParent, int isEdit = 1);   // standard constructor
  CMMenu(CWnd* pParent, UINT nIDTemplate = IDD_MENU2, const char *capt = NULL);

// Dialog Data
  //{{AFX_DATA(CMMenu)
  enum { IDD = IDD_MENU };
  enum { IDD2 = IDD_MENU2 };
  enum { IDD3 = IDD_MENU3 };
  CEdit m_Edit;
  CListBox  m_List;
  int   m_Index;
  CString m_strText;
  CString m_text;
  CString m_strCaption;
  //}}AFX_DATA

  void Add(const char *text, void *v = NULL);
  void *getV(const char *text);
  void *getV();
  void *getV2();
  void ReadData();


  void CleanList();

  CArray<CString,CString> lst;
  map<CString,void *> map_menu;
  map<int,void *> map_menu2;

  int n_multy;

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMMenu)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CMMenu)
  afx_msg void OnDblclkList();
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnChangeEdit();
  afx_msg void OnCopy();
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

