#pragma once

#include "virtlistView.h"
#include "VirtualListCtrl.h"

#include "Ultimate Grid\AdoCUG.h" 
#include "Ultimate Grid\ADOImpDatasource.h"


/////////////////////////////////////////////////////////////////////////////
// CTableDlg dialog

class CTableDlg : public CDialog
{
// Construction
public:
  CTableDlg(CWnd* pParent, int type_of_net, CString conn, CString db, CString q, CString tn, CString cap);
  ~CTableDlg();

// Dialog Data
  //{{AFX_DATA(CTableDlg)
  enum { IDD = IDD_TABLE_DLG };
//  CEdit m_edit;
  AdoCug m_grid;
//  CString m_find;
  //}}AFX_DATA

  int m_type_of_net;

  long m_id;
  CString m_text;
  CString m_q;
  CString m_tn;
  CString m_db;
  CString m_connect;
  CString m_cap;
  CString m_profile;

  _tagDSList *m_pDSList;




// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CTableDlg)
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
  //{{AFX_MSG(CTableDlg)
  afx_msg LRESULT OnDblClick(WPARAM, LPARAM);
  virtual BOOL OnInitDialog();
//  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnChangeEdit1();
  afx_msg void OnDestroy();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
private:
  bool m_modal;
  virtual void OnOK();
};
