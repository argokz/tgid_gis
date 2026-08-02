#pragma once

#include "Klassif.h"

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CLeft1Dialog dialog

class CLeft1Dialog : public CPaneDialog
{
// Construction
public:
  CLeft1Dialog();   // standard constructor

  void Init();
//  void Init(const char *fn);
  void SetKls(KlList *kl_list);
  void setKlsOnOff();


  void InsertView(LPCTSTR txt, const CView *pView);
  void RemoveView(const CView *pView);
  void FindView(const CView *pView);
  void Del(Klassif *m_kls, HTREEITEM &m_hItem);

  void InsertDocument(LPCTSTR txt, const CDocument *pDocument);
  void RemoveDocument(const CDocument *pDocument);
  HTREEITEM FindDocument(const CDocument *pDocument);
  CString getMdbName();

  void UpdateDocument();

  HTREEITEM getI() { return i1;};

  CTreeCtrl m_ctlTree;

  CFont m_Font;


// Dialog Data
  //{{AFX_DATA(CLeft1Dialog)
  enum { IDD = IDD_LEFT1 };
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CLeft1Dialog)
  public:
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  BOOL m_bInit;
  CImageList m_imgList;
  HTREEITEM i1;
  KlList *m_kl_list;

//protected:
  void SetCheckTree(HTREEITEM hItem, int flag, int ch);

  // Generated message map functions
  //{{AFX_MSG(CLeft1Dialog)
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg LRESULT HandleInitDialog(WPARAM, LPARAM);
  virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  virtual void OnOK();
  virtual void OnAdd();
  virtual void OnAddTab();
  virtual void OnDelTab();
  virtual void OnTab();
  virtual void OnKls();
  virtual void OnView();
  virtual void OnDel();
  virtual void OnCancel();
  virtual void OnAddress();
  virtual void OnName();

  afx_msg void OnSelchangedEdit(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

