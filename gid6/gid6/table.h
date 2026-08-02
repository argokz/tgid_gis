#pragma once

#define TABLE_NODE  1
#define TABLE_LINE  2
#define TABLE_GID   3
#define TABLE_GEO   4

#include "Ultimate Grid/MyCug.h"

class MyCug;
class CVirtData;
class CVirtListData;

class CTable  : public CView
{
protected: // create from serialization only
  CTable();
  DECLARE_DYNCREATE(CTable)

// Attributes
  public:
  virtual void OnInitialUpdate();

  void init(CVirtListData *data);



  int m_printPage;
  BOOL m_isPrinting;
  CRect m_drawRect;

  MyCug  m_grid;
  CVirtListData *m_data;

protected:

  public:
  virtual void OnDraw(CDC* pDC);      // overridden to draw this view
  protected:

  // Standard printing commands
  virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
  virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
  virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
  virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
  virtual void OnMoveTo();

  virtual void OnColumnSort();
  virtual void OnEditFind();
  virtual void OnEditRepeat();
  virtual void OnShiftF1();

protected:
  // Generated message map functions
  //{{AFX_MSG(CTable)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  virtual void OnHideColumn();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};