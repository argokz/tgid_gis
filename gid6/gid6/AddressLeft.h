#if !defined(AFX_ADDRESS3_H__89CB52C2_761C_4023_8C82_7C6A8AE40E40__INCLUDED_)
#define AFX_ADDRESS3_H__89CB52C2_761C_4023_8C82_7C6A8AE40E40__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Address3.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddressLeft dialog

class CAdoFile;

class KlList;

class CAddressLeft : public CPaneDialog
{
//  DECLARE_DYNCREATE(CAddressLeft)

// Construction
public:
  CAddressLeft(int is_nagr);
  ~CAddressLeft();

  void SetKls(KlList *kl_list);
  void Init();
  bool Init(const char *cxema, const char *tn, const char *raj, const char *str, const char *dom);
  void setTyp(int nagr);

  bool Addr(CString &tn,  CString &str,  CString &raj,  CString &nomer);


// Dialog Data
  //{{AFX_DATA(CAddressLeft)
  enum { IDD = IDD_ADDRESS3 };
  CEdit m_ctrl_street_find;
  CListBox  m_ctrl_street;
  CListBox  m_ctrl_raj;
  CListBox  m_ctrl_dom;
  CString m_street_find;
  //}}AFX_DATA


// Overrides
  // ClassWizard generate virtual function overrides
  //{{AFX_VIRTUAL(CAddressLeft)
  public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  HACCEL m_hAccel;                     // dialog accelerators

  
  BOOL m_bInit;
  KlList *m_kl_list;
  int m_is_nagr;

  CString m_tn;
  CString m_str;
  CString m_raj;
  CString m_nomer;
  
  CAdoFile *m_ado;

  void writeStreet(const char *raj, const char *pic = "");

  list<CString> m_list_r;
  list<pair<CString, CString> > m_list_u;
  
  // Generated message map functions
  //{{AFX_MSG(CAddressLeft)
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg LRESULT HandleInitDialog(WPARAM, LPARAM);
  afx_msg void OnSelchangeRaj();
  afx_msg void OnSelchangeStreet();
  afx_msg void OnSelchangeDom();
  afx_msg void OnChangeStreetFind();
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point) {};
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDRESS3_H__89CB52C2_761C_4023_8C82_7C6A8AE40E40__INCLUDED_)
