#pragma once

class CGraph2;
class CAdoFile;

/////////////////////////////////////////////////////////////////////////////
// CCalcTree dialog

class CCalcTree : public CDialog
{
// Construction
public:
  CCalcTree(CWnd* pParent, CGraph2 *graph, CAdoFile *ado);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CCalcTree)
  enum { IDD = IDD_UNVISUAL };
  CTreeCtrl m_tree;
  //}}AFX_DATA

  CString str;

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CCalcTree)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

    CGraph2 *m_graph;
    HTREEITEM m_h1, m_hRS, m_hZN, m_h4, m_hUR, m_hTR, m_hKV, m_hGV;
    HTREEITEM m_h;
    CAdoFile *m_ado;
    int m_id;
    CString m_tn;
  CFont m_Font;


  void init();
  void add(const char *tn, const char *fn, HTREEITEM h);

  // Generated message map functions
  //{{AFX_MSG(CCalcTree)
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

//  virtual void OnPopupAdd();
  virtual void OnPopupDel();
//  virtual void OnPopupEdit();

  virtual LRESULT OnAdoUpdated(WPARAM wParam, LPARAM);

  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};
