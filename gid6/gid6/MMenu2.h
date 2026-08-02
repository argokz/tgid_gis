#pragma once

/////////////////////////////////////////////////////////////////////////////
// CMMenu2 dialog


typedef bool (*fun_del)(int id, CString name);

class CMMenu2;

struct CMenuRow {
  CMenuRow() {};
  
  CMenuRow(CMenuRow &r)
  {
    v = r.v;
    cols = r.cols;
  }
  void *v;
  vector<CString> cols;
};

//typedef void (*fun)(CMMenu2 *wnd, CMenuRow &row, void *pp1 = NULL, void *pp2 = NULL, void *pp3 = NULL, void *pp4 = NULL);
typedef void(*fun)(CMMenu2 *wnd, CMenuRow &row, void *pp1, void *pp2, void *pp3, void *pp4);

struct FUN {
  fun f;
  void *p1;
  void *p2;
  void *p3;
  void *p4;
};


class CMMenu2 : public CDialog
{
// Construction
public:
  CMMenu2(CWnd* pParent, UINT nIDTemplate, const char *capt);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CMMenu2)
  enum { IDD = IDD_MENU10 };
  CListCtrl m_List;
  CString m_edit;
  //}}AFX_DATA


  CArray<int, int> m_width;
  CArray<CString,CString> m_cols;
  CMenuRow m_row;
  CArray<CMenuRow, CMenuRow> m_rows;
  CArray<CMenuRow, CMenuRow> m_rows2;
  map<UINT, FUN> m_map_proc;

  CString m_strCaption;

  CString m_profile;
  CString m_strText;
  int m_index;

  void AddColumn(const char *s);
  void Add(int col, const char *s);
  void Add(void *v);
  void *getV(const char *text);
  void *getV();
  void AddProc(UINT nID, fun proc, void *pp1 = NULL, void *pp2 = NULL, void *pp3 = NULL, void *pp4 = NULL);
  void init();
  void Reset();
  void setWidth(int n, int w);
  void setDelFun(fun_del fun);

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMMenu2)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

private:
  fun_del m_fun_del;


// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CMMenu2)
  virtual BOOL OnInitDialog();
  afx_msg void OnChangeEdit();
  virtual void OnOK();
  afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnDestroy();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};
