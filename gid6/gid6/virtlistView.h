// virtlistView.h : interface of the CVirtListView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIRTLISTVIEW_H__7978698C_A449_11D5_8B75_9BEA409C597E__INCLUDED_)
#define AFX_VIRTLISTVIEW_H__7978698C_A449_11D5_8B75_9BEA409C597E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <AFXCVIEW.H>
#include "virtualdata.h"

class CVirtListView : public CListView
{
protected: // create from serialization only
  CVirtListView();
  DECLARE_DYNCREATE(CVirtListView)

// Attributes
public:
//  CVirtListDoc* GetDocument();
  CVirtListData *getVirtListData();
  static void setVirtListData(CVirtListData *data);

  void UpdateCount();



//  int GetCount();
//  CString GetItemText(int nSubItem, int nIndex);
//  int GetImage(int nIndex);
//  int GetStateImage(int nIndex);

// Operations
public:

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CVirtListView)
  public:
  virtual void OnDraw(CDC* pDC);  // overridden to draw this view
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
  protected:
  virtual void OnInitialUpdate(); // called first time after construct
  virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
  virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
  virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
  virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
  //}}AFX_VIRTUAL

// Implementation
public:
  virtual ~CVirtListView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:
  CImageList m_ImageSmall;
  CImageList m_ImageState;

  CVirtListData *m_virtListData;
//  static CVirtListData *m_virtListDataInit;
  
  void InitListCtrl(int id);
// Generated message map functions
protected:
  //{{AFX_MSG(CVirtListView)
  afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnOdcachehint(NMHDR* pNMHDR, LRESULT* pResult);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in virtlistView.cpp
//inline CVirtListDoc* CVirtListView::GetDocument()
//   { return (CVirtListDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIRTLISTVIEW_H__7978698C_A449_11D5_8B75_9BEA409C597E__INCLUDED_)
