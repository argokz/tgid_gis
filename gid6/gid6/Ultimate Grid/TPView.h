#pragma once


#include "TnCug.h"
// include ADO datasource
#include "ADOImpDatasource.h"

class CTPView : public CView
{
protected: // create from serialization only
//  CTPView();
  DECLARE_DYNCREATE(CTPView)

// Attributes
public:
  CTPView();

  void init(const char *db);

//  CADOSampleDoc* GetDocument();

// Operations
public:

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CTPView)
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
  virtual ~CTPView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:
  TpCug m_grid;

  int m_printPage;
  BOOL m_isPrinting;
  CRect m_drawRect;

  _tagDSList *m_pDSList;

// Generated message map functions
protected:
  //{{AFX_MSG(CTPView)
  virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
  virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
  virtual void OnSize(UINT nType, int cx, int cy);
  virtual void OnSetFocus(CWnd* pOldWnd);
  virtual BOOL OnEraseBkgnd(CDC* pDC);
  virtual void OnRefreshData();
  virtual void OnShowSql();
  virtual void OnAppendRow();
  virtual void OnDeleteRecord();
  virtual void OnAddTable();
  virtual void OnShiftF1();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


