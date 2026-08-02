#pragma once

/////////////////////////////////////////////////////////////////////////////
// CRastrWndVect window


#include "Bgi.h"
#include "coordlis.h"

enum FIGURA_V {
  TIP_LINE,
  TIP_RING,
  TIP_ARC,
  TIP_CIRCLE,
  TIP_POLYGON,
  TIP_DEL,
  TIP_BLACK,
  TIP_WHITE
};

enum REGIM_V {
  RV_INIT,
  RV_BEGIN1,
  RV_BEGIN2,
  RV_BEGIN3
/*
  RV_LINE,
  RV_RING,
  RV_ARC,
  RV_CIRCLE,
  RV_POLYGON,
  RV_BEGIN_CIRCLE,
  RV_BEGIN_POLYGON,
  RV_BEGIN_LINE,
  RV_BEGIN_RING,
  RV_BEGIN_ARC
  */
};

struct FRAG
{
  int typ;
  int color;
  int color_bk;
  CCoordList cl;
};


class CRastrWndVect : public CBgi
{
// Construction
public:
  CRastrWndVect();
  void SetTyp(int t) {m_typ = max(1, t); Invalidate(); };
  void init(int typ);
  void restore();
  void clear();

// Attributes
public:
  int m_typ;
  BOOL m_is_white;

protected:
  int m_regim;
  int m_figura;
  CPoint m_pt1, m_pt2;
  CCoordList m_NP;
  list <FRAG> m_frag;


// Operations
public:
  void Rectan(int x, int y, int col);
  void Rectan(int x, int y);
  void SetRegim(int regim);
  void save();
  void setColor(BOOL is_white);

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CRastrWndVect)
  virtual void OnInitialUpdate();
  //}}AFX_VIRTUAL



// Implementation
public:
  virtual ~CRastrWndVect();

  // Generated message map functions
protected:
  virtual void OnDraw(CDC* pDC);      // overridden to draw this view
  void XDraw(CDC* m_dc, CPoint p1, CPoint p2); 
  void createFragm();
  void delObject(CPoint point);


  //{{AFX_MSG(CRastrWndVect)
//  afx_msg void OnPaint();
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
  virtual void PostNcDestroy();
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};

