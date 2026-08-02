#pragma once

#include "virtlistView.h"
#include "VirtualListCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CVirtualTable dialog

class CVirtualTable : public CPaneDialog
{
// Construction
public:
  CVirtualTable();   // standard constructor

  void Init0(CVirtListData *data);

// Dialog Data
  //{{AFX_DATA(CVirtualTable)
  enum { IDD = IDD_PTS_VIRTUAL_TABLE };
  CVirtualListCtrl  m_grid;
  //}}AFX_DATA

  void Refresh();
  void StartRefresh();
  void zapros(const char *table, const char *avaria, bool is_time, bool is_check, CTime date1, CTime date2);
  void all();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CVirtualTable)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  UINT m_nTimer;

  long m_row;
  long m_col;

  long m_rowL;
  long m_colL;
  long m_rowE;
  long m_colE;
  
  long m_id;
  CString m_pri;

  bool m_all_vis;
  CString m_strProfile;

  bool m_is_open;

  CVirtListData *m_data;

  // Generated message map functions
  //{{AFX_MSG(CVirtualTable)
  afx_msg LRESULT OnDblClick(WPARAM, LPARAM);
  afx_msg LRESULT HandleInitDialog(WPARAM, LPARAM);
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnClickMshflexgrid1();
  afx_msg void OnMoveTo();
  afx_msg void OnSort();
  afx_msg void OnEdit();
  afx_msg void OnMeropr();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnDestroy();
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point) {};
//  DECLARE_EVENTSINK_MAP()
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};
