#if !defined(AFX_GEOMENU_H__AD299B03_2723_11D3_B153_885725EB4A0E__INCLUDED_)
#define AFX_GEOMENU_H__AD299B03_2723_11D3_B153_885725EB4A0E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MMenu.h : header file
//

#include "klassif.h"


/////////////////////////////////////////////////////////////////////////////
// CGeoMenu dialog

class CGeoMenu : public CDialog
{
// Construction
public:
//  CGeoMenu(CWnd* pParent, int isEdit = 1);   // standard constructor
  CGeoMenu(CView* pParent, UINT nIDTemplate = IDD_MENU2_1, const char *capt = NULL);

// Dialog Data
  //{{AFX_DATA(CGeoMenu)
	enum { IDD = IDD_GEO_MENU };
	CButton	m_ctrlTu;
	CButton	m_ctrlNadpis;
	CButton	m_ctrlIsprPodp;
	CButton	m_ctrlIspr;
	CButton	m_ctrlDel;
  BOOL  m_nadpis;
  CListBox  m_List;
  int   m_Index;
  CString m_strCaption;
	//}}AFX_DATA

  CString m_strText;
  int   m_num;


  CArray<CString,CString> list_txt;
  CArray<const Klassif*,const Klassif*> list;
  CArray<int,int> list_num;

  void Add(const Klassif *kls, int num);
  void Add(const char *text, int num);
  void close();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CGeoMenu)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  CView *m_view;
  CDC *m_old_dc;

  // Generated message map functions
  //{{AFX_MSG(CGeoMenu)
  afx_msg void OnDblclkList();
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnSelChange();
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg void OnClose();
  afx_msg void OnNadpis();
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GEOMENU_H__AD299B03_2723_11D3_B153_885725EB4A0E__INCLUDED_)
