#pragma once

#include "gid6Doc.h"
#include "gidrView.h"


/////////////////////////////////////////////////////////////////////////////
// CMapNavigator window

class CMapNavigator : public CStatic
{
// Construction
public:
  CMapNavigator();

// Attributes
public:

  CPoint pt_0;
  CPoint pt2_0;
  int xx_0, yy_0;
  int xx2_0, yy2_0;

// Operations
public:
  void draw(CGidrView *view);
  void moveRect(CPoint pt, BOOL off = FALSE);
  void curRect();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMapNavigator)
  //}}AFX_VIRTUAL

// Implementation
public:
  virtual ~CMapNavigator();

  // Generated message map functions
protected:
  //{{AFX_MSG(CMapNavigator)
  afx_msg void OnPaint();
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg LRESULT OnMouseLeave(WPARAM WParam, LPARAM LParam);
  afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
  //}}AFX_MSG


  DECLARE_MESSAGE_MAP()
};

