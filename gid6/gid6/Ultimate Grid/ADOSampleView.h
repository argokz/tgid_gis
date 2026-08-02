#pragma once


#include "AdoCug.h"
// include ADO datasource
#include "ADOImpDatasource.h"

class CADOSampleView : public CView
{
protected: // create from serialization only
  CADOSampleView();
  DECLARE_DYNCREATE(CADOSampleView)

// Attributes
public:
//  CADOSampleDoc* GetDocument();

// Operations
public:

  void setHead(const char *db, const char *name);
  void SetColorCol(int col);


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CADOSampleView)
  public:
  virtual void OnDraw(CDC* pDC);  // overridden to draw this view
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
  virtual void OnInitialUpdate();
  protected:
  virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
  virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
  virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
  //}}AFX_VIRTUAL

// Implementation
public:
  virtual ~CADOSampleView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

public:
  CString m_view_db;
  CString m_view_tn;
  CString m_view_sql;


protected:
  AdoCug m_grid;
  AdoHead m_head;

  int m_colorCol;
  bool m_show_all;

  bool m_is_head;

  int m_printPage;
  BOOL m_isPrinting;
  CRect m_drawRect;

  _tagDSList *m_pDSList;

// Generated message map functions
protected:
  //{{AFX_MSG(CADOSampleView)
  virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
  virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
  virtual void OnEditFind();
  virtual void OnSize(UINT nType, int cx, int cy);
  virtual void OnSetFocus(CWnd* pOldWnd);
  virtual BOOL OnEraseBkgnd(CDC* pDC);
  virtual void OnRefreshData();
  virtual void OnShowSql();
  virtual void OnAppendRow();
  virtual void OnDeleteRecord();
  virtual void OnAddTable();
  virtual void OnMoveTo();
  virtual void OnHideColumn();
  virtual void OnShiftF1();
  virtual void OnExcel();
  virtual void OnSwitchRecord();
  virtual LRESULT OnTableChange(WPARAM wParam, LPARAM);

  virtual void OnZoomIn();
  virtual void OnZoomOut();
  virtual void OnZoom100();


  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


