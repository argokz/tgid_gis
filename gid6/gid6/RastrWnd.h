#pragma once

/////////////////////////////////////////////////////////////////////////////
// CRastrWnd window

class CRastrWnd : public CStatic
{
// Construction
public:
  CRastrWnd();

// Attributes
public:
  int xx, yy, ddx, ddy, x_old, y_old, nx, ny;

// Operations
public:
  void Rectan(int x, int y, int col);
  void Rectan(int x, int y);

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CRastrWnd)
  //}}AFX_VIRTUAL

// Implementation
public:
  virtual ~CRastrWnd();

  // Generated message map functions
protected:
  //{{AFX_MSG(CRastrWnd)
  afx_msg void OnPaint();
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};

