#pragma once

/////////////////////////////////////////////////////////////////////////////
// CPtsSprav2 dialog

class KlList;

struct QUE3 {
  CString q = "";
  CString tab = "";
  CString mdb = "";
  CString add = "";
};


class CPtsSprav2 : public CPaneDialog
{
//  DECLARE_DYNCREATE(CPtsSprav2)

// Construction
public:
  CPtsSprav2(const char *cap, const char *fn);
  ~CPtsSprav2();

  void SetKls(KlList *kl_list);

// Dialog Data
  //{{AFX_DATA(CPtsSprav2)
  enum { IDD = IDD_SPRAV };
  CTreeCtrl m_ctrlTree;
  //}}AFX_DATA


// Overrides
  // ClassWizard generate virtual function overrides
  //{{AFX_VIRTUAL(CPtsSprav2)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  CImageList m_imgList;
  CFont m_Font;

  BOOL m_bInit;
  KlList *m_kl_list;
  
//  CDbFile *m_dbf;
  CString m_cap;
  CString m_fn;

  HTREEITEM m_i1;
  HTREEITEM m_i2;
  HTREEITEM m_i3;
  HTREEITEM m_i4;
  HTREEITEM m_i5;


  map <CString, QUE3> map_q;
  
  // Generated message map functions
  //{{AFX_MSG(CPtsSprav2)
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg LRESULT HandleInitDialog(WPARAM, LPARAM);
  virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point) {};
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

};

