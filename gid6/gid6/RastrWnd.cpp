// RastrWnd.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "bgi.h"


#include "RastrWnd.h"
#include "Klassif.h"
#include "kltypVect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRastrWnd

const int DDX = 60;

CRastrWnd::CRastrWnd()
{
  x_old = -1;
}

CRastrWnd::~CRastrWnd()
{
}

BEGIN_MESSAGE_MAP(CRastrWnd, CStatic)
  //{{AFX_MSG_MAP(CRastrWnd)
  ON_WM_PAINT()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_RBUTTONDOWN()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRastrWnd message handlers

const int *get_pics(int typ, double coef);
const int *get_pics_map(int typ);

/*
int typ_typ(int n)
{
  const int *pic;
  const int *picRO = get_pics(TIP_RO, 1);

  int i = 58, j = 0, k;

  for (k = 0; k <= n; k++) {
    do
    {
      pic = get_pics(i, 1);
      for (j = 0; j < 20; j++) {
        if (pic[j] != picRO[j]) break;
      }
      i++;
    } while (i < 99 && j == 20);
  }
  if (j != 20 && i != 100) return i-1;

  return -1;
}
*/

void CRastrWnd::OnPaint() 
{
  CPaintDC dc(this); // device context for painting
  
  CRect rect;
  
  GetClientRect(rect);
  int x, y, i;

  xx = rect.right-rect.left;  yy = rect.bottom-rect.top;

  nx = xx/DDX;  ny = yy/DDX;

  ddx = (xx-xx/DDX*DDX)/2;
  ddy = (yy-yy/DDX*DDX)/2;

  for ( i = 0, y = ddy-1; y < yy; y += DDX, i++ ) {
    dc.MoveTo(ddx-1, y);
    dc.LineTo(xx-ddx-1, y);
  }

  for ( i = 0, x = ddx-1; x < xx; x += DDX, i++ ) {
    dc.MoveTo(x, ddy-1);
    dc.LineTo(x, yy-ddy-1);
  }

  CBgi scr;

  scr.m_rect = rect;
  scr.SetDC(&dc);

  for ( i = 0, y = ddy; y+DDX < yy; y += DDX  ) {
    for ( x = ddx; x+DDX < xx; x += DDX ) {
//      int k = typ_typ(i++);
      int k = i+58;
      i++;

      if (k >= 0) {

        const int *pics;
        
        pics = get_pics_map(k);

        if (!pics) {
          pics = get_pics(k, 1);
        }

        scr.drawLinePic(&dc, 1, x+DDX/2, y+DDX/2, x+DDX/2+100, y+DDX/2, BLACK, 0, WHITE, pics, 1, false);
      }
    }
  }
  x_old = -1;
}

COLORREF rgb_color(int c);

void CRastrWnd::Rectan(int x, int y) 
{
  int x1, y1, x2, y2, saveROP;

  if (x < 0) return;

  x1 = ddx+x*DDX+1;  x2 = ddx+x*DDX+DDX-3;
  y1 = ddy+y*DDX+1;  y2 = ddy+y*DDX+DDX-3;

  CClientDC dc(this);

  saveROP = dc.SetROP2(R2_NOTXORPEN);
  dc.MoveTo(x1, y1);
  dc.LineTo(x1, y2);  dc.LineTo(x2, y2);
  dc.LineTo(x2, y1);  dc.LineTo(x1, y1);
  dc.SetROP2(saveROP);
}

void CRastrWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
  int x = (point.x-ddx)/DDX, y = (point.y-ddy)/DDX;
  if (x != x_old || y != y_old) {
    Rectan(x_old, y_old);
    Rectan(x, y);
    x_old = x; y_old = y;
  }
  
  CStatic::OnMouseMove(nFlags, point);
}

void CRastrWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
//  int k = typ_typ(nx*y_old+x_old);

  int k = nx*y_old+x_old+58;

  if (k >= 0) {
    ((CDialog*)GetParent())->EndDialog(100+k);
  }
  
  CStatic::OnLButtonDown(nFlags, point);
}

void CRastrWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
  
  CKlTypVect kltyp(this);
  int ret;

  kltyp.m_rastr.init(nx*y_old+x_old+58);

  ret = kltyp.DoModal();

  Invalidate();
  
  CStatic::OnRButtonDown(nFlags, point);
}
