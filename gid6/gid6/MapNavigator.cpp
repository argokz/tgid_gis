// MapNavigator.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "MapNavigator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapNavigator

CMapNavigator::CMapNavigator()
{
}

CMapNavigator::~CMapNavigator()
{
}


BEGIN_MESSAGE_MAP(CMapNavigator, CStatic)
  //{{AFX_MSG_MAP(CMapNavigator)
  ON_WM_PAINT()
  ON_WM_MOUSEMOVE()
  ON_WM_MOUSEWHEEL()
  ON_WM_LBUTTONDOWN()
  ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave) 
  ON_WM_SETCURSOR()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapNavigator message handlers

void xrectangle(CDC &dc, int x1, int y1, int x2, int y2) {
  CPoint fil[5];

  fil[0] = CPoint(x1,y1);
  fil[1] = CPoint(x2,y1);
  fil[2] = CPoint(x2,y2);
  fil[3] = CPoint(x1,y2);
  fil[4] = CPoint(x1,y1);

  int saveROP = dc.SetROP2(R2_NOTXORPEN);
  dc.Polyline(fil, 5);
}



void CMapNavigator::OnPaint() 
{
  CGidrView *view = getView();
  if (view) draw(view);
}

void CMapNavigator::draw(CGidrView *view) 
{
  CPaintDC dc(this); // device context for painting
  CRect rect, r;
  GetClientRect(rect);

  pt_0.x = -100000;
  pt2_0.x = -100000;

  if (view && ::IsWindow(view->m_hWnd)) {
    view->drawAll(&dc, rect);
    curRect();
  }
}

void CMapNavigator::curRect() 
{
  CClientDC dc(this); // device context for painting
  CRect rect, r;
  CPoint pt2;
  GetClientRect(rect);

  CGidrView *view = getView();

  if (view) {
    view->GetClientRect(r);
    CFRect m_rectBorder = view->m_rectBorder;

    double m = max((double)m_rectBorder.Width()/rect.Width(), (double)m_rectBorder.Height()/rect.Height());

    pt2.x = (view->geom.m_bx*view->geom.masx-m_rectBorder.left)/m;
    pt2.y = (view->geom.m_by*view->geom.masy-m_rectBorder.top)/m;
    int xx = r.Width()*view->geom.masx/m;
    int yy = r.Height()*view->geom.masy/m;

//    CPen *oldpen, pen(PS_SOLID, 3, 0x00007F);
    CPen *oldpen, pen(PS_SOLID, 3, 0x0000FF);
    oldpen = dc.SelectObject(&pen);

    if (pt2_0.x != -100000) {
      xrectangle(dc, pt2_0.x, pt2_0.y, pt2_0.x+xx2_0, pt2_0.y+yy2_0);
    }
    xrectangle(dc, pt2.x, pt2.y, pt2.x+xx, pt2.y+yy);

    dc.SelectObject(oldpen);


    pt2_0 = pt2;
    xx2_0 = xx;
    yy2_0 = yy;
  }
}

void CMapNavigator::moveRect(CPoint pt, BOOL off) 
{
  // TODO: Add your message handler code here and/or call default

  CClientDC dc(this); // device context for painting
  CRect rect, r;
  GetClientRect(rect);

  CGidrView *view = getView();

  if (view && ::IsWindow(view->m_hWnd)) {
    view->GetClientRect(r);

    CFRect m_rectBorder = view->m_rectBorder;

    double m = max((double)m_rectBorder.Width()/rect.Width(), (double)m_rectBorder.Height()/rect.Height());

    int xx = r.Width()*view->geom.masx/m;
    int yy = r.Height()*view->geom.masy/m;

    if (pt_0.x != -100000) {
      xrectangle(dc, pt_0.x, pt_0.y, pt_0.x+xx_0, pt_0.y+yy_0);
    }

//    if (!off) {
      xrectangle(dc, pt.x, pt.y, pt.x+xx, pt.y+yy);
      pt_0 = pt;
      xx_0 = xx;
      yy_0 = yy;
//    }
//    else {
//      xx_0 = -100000;
//    }
  }
}

void CMapNavigator::OnMouseMove(UINT nFlags, CPoint pt) 
{
  // TODO: Add your message handler code here and/or call default

  moveRect(pt);
  CStatic::OnMouseMove(nFlags, pt);
}

BOOL CMapNavigator::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
  // TODO: Add your message handler code here and/or call default
  
  return CStatic::OnMouseWheel(nFlags, zDelta, pt);
}

void CMapNavigator::OnLButtonDown(UINT nFlags, CPoint pt) 
{
  // TODO: Add your message handler code here and/or call default

  CGidrView *view = getView();
  CRect rect, r;
  GetClientRect(rect);

  if (view && ::IsWindow(view->m_hWnd)) {
    CFRect m_rectBorder = view->m_rectBorder;
    view->GetClientRect(r);

    double m = max((double)m_rectBorder.Width()/rect.Width(), (double)m_rectBorder.Height()/rect.Height());
  
    view->moveXY2(m_rectBorder.left + pt.x*m, m_rectBorder.top + pt.y*m);
    view->RedrawWindow();
  }

  CStatic::OnLButtonDown(nFlags, pt);
}


LRESULT CMapNavigator::OnMouseLeave(WPARAM WParam, LPARAM LParam) 
{ 
  moveRect(CPoint(-100000, 0), TRUE);
  return CStatic::Default(); 
} 

BOOL CMapNavigator::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
  if(nHitTest == HTCLIENT) 
  { 
    TRACKMOUSEEVENT tme; 
    tme.cbSize = sizeof(TRACKMOUSEEVENT); 
    tme.dwFlags = TME_LEAVE; 
    tme.hwndTrack = m_hWnd; 
    ::_TrackMouseEvent(&tme); 
  } 
  
  return CStatic::OnSetCursor(pWnd, nHitTest, message);
}
