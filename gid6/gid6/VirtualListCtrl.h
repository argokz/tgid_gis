#pragma once

#include "virtualdata.h"

#include "SortHeaderCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CVirtualListCtrl window



class CVirtualListCtrl : public CListCtrl
{
// Construction
public:
  CVirtualListCtrl();

// Attributes
public:

  void init(int id);
  int getCur();
  int*  CVirtualListCtrl::getCurElements();

// Operations
public:
  void setVirtListData(CVirtListData *data);
  CVirtListData *getVirtListData() { return m_virtListData; };

  void UpdateCount();


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CVirtualListCtrl)
  public:
  virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
  //}}AFX_VIRTUAL

protected:
  CImageList m_ImageSmall;
  CImageList m_ImageState;

  CVirtListData *m_virtListData;

  CSortHeaderCtrl m_ctlHeader;

  void InitListCtrl(int id);

  virtual void PreSubclassWindow();


// Implementation
public:
  virtual ~CVirtualListCtrl();

  // Generated message map functions
protected:
  //{{AFX_MSG(CVirtualListCtrl)
  afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnOdcachehint(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  
  afx_msg void OnPopupEdit();
  afx_msg void OnPopupMoveTo();
  afx_msg void OnPopupDel();
  
  virtual void setFindText(const char *text);
  virtual void OnDestroy();
  //}}AFX_MSG

private:
  int m_row;
  int m_col;

  int m_iSortColumn;
  BOOL m_bSortAscending;

  CString m_section;

  DECLARE_MESSAGE_MAP()
};

