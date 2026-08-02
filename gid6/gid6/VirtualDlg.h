#if !defined(AFX_VIRTUALDLG_H__FAB175DB_6972_4B4B_991D_13DAA331B220__INCLUDED_)
#define AFX_VIRTUALDLG_H__FAB175DB_6972_4B4B_991D_13DAA331B220__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VirtualDlg.h : header file
//

#include "virtlistView.h"
#include "VirtualListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CVirtualDlg dialog

class CVirtualDlg : public CDialog
{
// Construction
public:
  CVirtualDlg(CWnd* pParent, CVirtListData *m_data, int id, const char *cap, bool modal, UINT nIDTemplate = IDD_VIRTUAL, bool fnd = false, bool add = false, bool edit = false, bool del = false, bool chk = false);
  ~CVirtualDlg();

// Dialog Data
  //{{AFX_DATA(CVirtualDlg)
  enum { IDD = IDD_VIRTUAL };
  CEdit m_edit;
  CVirtualListCtrl  m_grid;
  CString m_find;
  //}}AFX_DATA


  CFont m_font;

  long m_id;
  CString m_text;

  bool m_fnd;
  bool m_add;
  bool m_edt;
  bool m_del;
  bool m_chk_all_res = false;


  CString m_text_list[256];



// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CVirtualDlg)
  public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  void moveID(int id);
  void Size();

  // Generated message map functions
  //{{AFX_MSG(CVirtualDlg)
  afx_msg LRESULT OnDblClick(WPARAM, LPARAM);
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnChangeEdit1();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
private:
  CVirtListData *m_data;
  CString m_cap;
  bool m_modal;
  virtual void OnOK();

  CString m_profile;


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIRTUALDLG_H__FAB175DB_6972_4B4B_991D_13DAA331B220__INCLUDED_)
