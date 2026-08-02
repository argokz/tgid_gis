#pragma once

#include "Klassif.h"

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CLeft2Dialog dialog

class CLeft2Dialog : public CPaneDialog
{
// Construction
public:
  CLeft2Dialog();   // standard constructor

//  HTREEITEM InsertItem(LPCTSTR txt, int nImage, int nSelectedImage, HTREEITEM i = NULL);
  void Init();


  void InsertView(LPCTSTR txt, const CView *pView);
  void RemoveView(const CView *pView);
  void FindView(const CView *pView);

  void InsertDocument(LPCTSTR txt, const CDocument *pDocument);
  void RemoveDocument(const CDocument *pDocument);
  HTREEITEM FindDocument(const CDocument *pDocument);

  void UpdateDocument();

  HTREEITEM getI() { return i1;};


  CTreeCtrl m_ctlTree;

  CFont m_Font;


// Dialog Data
  //{{AFX_DATA(CLeft2Dialog)
  enum { IDD = IDD_LEFT2 };
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CLeft2Dialog)
  public:
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  BOOL m_bInit;
  CImageList m_imgList;
  KlList *m_kl_list;
  HTREEITEM i1;

//protected:
  void SetCheckTree(HTREEITEM hItem, int flag, int ch);

  // Generated message map functions
  //{{AFX_MSG(CLeft2Dialog)
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg LRESULT HandleInitDialog(WPARAM, LPARAM);
  virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  virtual void OnOK();
  virtual void OnAdd();
  virtual void OnView();
  virtual void OnDel();
  virtual void OnCancel();
  afx_msg void OnSelchangedEdit(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point) {};
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

