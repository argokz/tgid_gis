#pragma once

/////////////////////////////////////////////////////////////////////////////
// CRastrWnd2 window

class CRastrWnd2 : public CStatic
{
// Construction
public:
  CRastrWnd2();
  void SetTyp(int t) {typ = max(1, t); Invalidate(); };

// Attributes
public:
  int typ;

// Operations
public:
  void Rectan(int x, int y, int col);
  void Rectan(int x, int y);

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CRastrWnd2)
  //}}AFX_VIRTUAL

// Implementation
public:
  virtual ~CRastrWnd2();

  // Generated message map functions
protected:
  //{{AFX_MSG(CRastrWnd2)
  afx_msg void OnPaint();
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};

