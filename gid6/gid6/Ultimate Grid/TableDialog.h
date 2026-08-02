#pragma once


#include "AdoCug.h"
#include "ADOImpDatasource.h"

#include "resource.h"

class CTableDialog;

typedef void(*fun2)(CTableDialog *wnd, void *pp1, void *pp2, void *pp3, void *pp4);

struct FUN2 {
  fun2 f;
  void *p1;
  void *p2;
  void *p3;
  void *p4;
};

class CTableDialog : public CDialog
{
public:

  CTableDialog(CWnd *wnd, int type_of_net, const char *db, const char *connect, const char *tn, const char *q, int n_ret, VIRT_DATA m_typ, const char *caption = nullptr, int idd = IDD_TABLE_DEFECT);

  void setHead(const char *db, const char *name);
  void SetKey(CString key_name, int key_value);
  void SetKey2(CString key_name1, int key_value1, CString key_name2, int key_value2);

  void addProc(UINT nID, fun2 proc, void* pp1, void* pp2, void* pp3, void* pp4);
  void proc(UINT id, void* p1, void* p2, void* p3, void* p4 );
  
  int Execute(const char * q);

  void SetIdCol(int idCol);


  enum { IDD = IDD_TABLE_DEFECT };



// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CTableDialog)
  public:
  protected:
  //}}AFX_VIRTUAL

// Implementation
public:
  virtual ~CTableDialog();
  CString  m_strRet;
  int m_id;

  CString getValue(const char *s);
  bool setText(long row);

  void noModal() {m_modal = FALSE;};

  void proc(UINT id);
  
protected:
  AdoCug m_grid;

  int m_printPage;
  BOOL m_isPrinting;
  CRect m_drawRect;

  CString m_connect;
  CString m_db;
  CString m_tn;
  CString m_title;
  CString m_q;
  CString m_caption;
  int m_idCol;

  int m_type_of_net;
  VIRT_DATA m_typ;

  map<int, CString> m_val_id;
  map<CString, CString> m_val_name;

  int m_n_ret;
  AdoHead m_head;

  int m_n_keys;
  CString m_keyName;
  int m_keyValue;

  CString m_keyName2;
  int m_keyValue2;

  CEdit *m_find;
  
  CString m_profile;

  _tagDSList *m_pDSList;
  map<UINT, FUN2> m_map_proc;

private:
  bool m_modal;


// Generated message map functions
protected:
  //{{AFX_MSG(CTableDialog)
  afx_msg BOOL OnInitDialog();
  virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  virtual void OnSize(UINT nType, int cx, int cy);
  virtual void OnSetFocus(CWnd* pOldWnd);
  virtual BOOL OnEraseBkgnd(CDC* pDC);
  virtual void OnRefreshData();
  virtual void OnShowSql();
  virtual void OnDeleteRecord();
  virtual void OnMoveTo();
  virtual void OnHideColumn();
  virtual void OnShiftF1();
  virtual void OnDestroy();
  virtual LRESULT OnDblClick(WPARAM, LPARAM);
  virtual void OnOK();
  virtual void OnAdd();
  virtual void OnDel();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


