#pragma once

#include "MapNavigator.h"

/////////////////////////////////////////////////////////////////////////////
// CMapDlg dialog

class CMapDlg : public CPaneDialog
{
// Construction
public:
  CMapDlg();   // standard constructor

// Dialog Data
  //{{AFX_DATA(CMapDlg)
  enum { IDD = IDD_MAP_DLG };
  CMapNavigator m_map;
  //}}AFX_DATA

  void moveRect();
  void curRect();


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMapDlg)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CMapDlg)
  afx_msg LRESULT HandleInitDialog(WPARAM, LPARAM);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point) {};
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};
