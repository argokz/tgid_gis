#pragma once

/////////////////////////////////////////////////////////////////////////////
// CRastrBut window

class CRastrBut : public CButton
{
// Construction
public:
  CRastrBut();

// Attributes
public:
  int typ;

// Operations
public:
  void SetTyp(int t) {typ = max(0, t); Invalidate(); };

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CRastrBut)
  //}}AFX_VIRTUAL

// Implementation
public:
  virtual ~CRastrBut();

  // Generated message map functions
protected:
  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
  
  //{{AFX_MSG(CRastrBut)
//  afx_msg void OnPaint();
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};

