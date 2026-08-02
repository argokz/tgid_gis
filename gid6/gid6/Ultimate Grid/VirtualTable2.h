#pragma once


#include "AdoCug.h"
// include ADO datasource
#include "ADOImpDatasource.h"

class CVirtualTable2 : public CPaneDialog
{
protected: // create from serialization only
//  CVirtualTable2();
  DECLARE_DYNCREATE(CVirtualTable2)

// Attributes
public:
//  CADOSampleDoc* GetDocument();

  CFont *GetFontNormal();

protected:
  CFont m_defFont;



// Operations
public:

  CVirtualTable2();

  bool init(int type_of_net, const char *db, const char *connect, const char *tn, const char *q, const char *title, bool is_head, VIRT_DATA _typ);
  void Excel();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CVirtualTable2)
  public:
  protected:
  //}}AFX_VIRTUAL

// Implementation
public:
  virtual ~CVirtualTable2();

protected:
  AdoCug m_grid;

  int m_printPage;
  BOOL m_isPrinting;
  CRect m_drawRect;

  BOOL m_bInit;

  CString m_title;
  CString m_tn;
  CString m_q;
  VIRT_DATA m_typ;
  
  AdoHead m_head;

  _tagDSList *m_pDSList;

// Generated message map functions
protected:

  void resize();
  
  //{{AFX_MSG(CVirtualTable2)
  virtual LRESULT HandleInitDialog(WPARAM, LPARAM);
  virtual LRESULT OnDblClick(WPARAM, LPARAM);
  virtual void OnEditFind();
  virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
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
  virtual CTabbedPane* CreateTabbedPane();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


