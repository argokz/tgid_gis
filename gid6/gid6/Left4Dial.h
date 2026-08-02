#pragma once

/////////////////////////////////////////////////////////////////////////////
// CLeft4Dialog dialog

class KlList;

class CAdoFile;

class CLeft4Dialog : public CPaneDialog
{
//  DECLARE_DYNCREATE(CLeft4Dialog)

// Construction
public:
  CLeft4Dialog();
  ~CLeft4Dialog();

  void SetKls(KlList *kl_list);
  void Init(const char *pic);

// Dialog Data
  //{{AFX_DATA(CLeft4Dialog)
  enum { IDD = IDD_LEFT4 };
  CListBox  m_ctrlList;
  CEdit m_Edit;
  //}}AFX_DATA


// Overrides
  // ClassWizard generate virtual function overrides
  //{{AFX_VIRTUAL(CLeft4Dialog)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  BOOL m_bInit;
  KlList *m_kl_list;

  CString m_tn;
  CString m_str;
  CString m_Pic;
  
  CAdoFile *m_ado;

  void writeStreet(const char *raj, const char *pic = "");

  // Generated message map functions
  //{{AFX_MSG(CLeft4Dialog)
  afx_msg LRESULT HandleInitDialog(WPARAM, LPARAM);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnSelchangeList();
  afx_msg void OnChangeEdit();
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point) {};
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

};
