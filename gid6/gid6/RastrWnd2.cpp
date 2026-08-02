// RastrWnd2.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "RastrWnd2.h"
#include "Klassif.h"
#include "kltyp2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRastrWnd2


CRastrWnd2::CRastrWnd2()
{
  typ = 1;
}

CRastrWnd2::~CRastrWnd2()
{
}


BEGIN_MESSAGE_MAP(CRastrWnd2, CStatic)
  //{{AFX_MSG_MAP(CRastrWnd2)
  ON_WM_PAINT()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_RBUTTONDOWN()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRastrWnd2 message handlers

void Rectan(CDC *dc, int x, int y, int col);

extern Prim *rs;


void CRastrWnd2::OnPaint() 
{
  CPaintDC dc(this); // device context for painting
  
  CRect rect;
  GetClientRect(rect);

  int x, y, i;

  for ( i = 0, y = 1; i <= 16; y += 18, i++ ) {
    dc.MoveTo(1, y);
    dc.LineTo(16*18+1, y);
  }

  for ( i = 0, x = 1; i <= 16; x += 18, i++ ) {
    dc.MoveTo(x, 1);
    dc.LineTo(x, 16*18+1);
  }

  unsigned short k;
  unsigned short *str = (unsigned short *) rs[typ-1].s;

  for (i = 0, y = 0; i < 16; i++, y++) {
    for (k = 0x8000, x = 0; k; k >>= 1, x++) {
      if (str[i] & k) {
        ::Rectan(&dc, x, y, 1);
      }
    }
  }
  ::Rectan(&dc, rs[typ-1].x, rs[typ-1].y, 2);
}

COLORREF rgb_color(int c);


//void CRastrWnd2::Rectan(CDC *dc, int x, int y, int col) 
void Rectan(CDC *dc, int x, int y, int col) 
{
   int x1, y1, x2, y2;

  if (x < 0) return;

  x1 = x*18+1;  x2 = x*18+20;
  y1 = y*18+1;  y2 = y*18+20;


  switch ( col ) {
    case 0 :
      {
        CBrush brush(::GetSysColor(COLOR_3DFACE));
        dc->SelectObject(&brush);
        dc->Rectangle(x1, y1, x2, y2);
      }
      break;
    case 1 :
      {
        CBrush brush(RGB(0x0, 0x0, 0x0));
        dc->SelectObject(&brush);
        dc->Rectangle(x1+2, y1+2, x2-2, y2-2);
      }
      break;

    case 2 :
      {
        CPen pen(PS_SOLID, 1, RGB(255, 0, 0));
        dc->SelectObject(&pen);
        dc->MoveTo(x1+1, y1+1);   dc->LineTo(x2-1, y2-1);
        dc->MoveTo(x1+1, y2-1);   dc->LineTo(x2-1, y1+1);
      }
      break;
  }
}


void CRastrWnd2::OnMouseMove(UINT nFlags, CPoint point) 
{
  
  CStatic::OnMouseMove(nFlags, point);
}

void CRastrWnd2::OnLButtonDown(UINT nFlags, CPoint point) 
{
//  ((CDialog*)GetParent())->EndDialog(100+nx*y_old+x_old+1);

  int x = point.x/18, y = point.y/18;

  unsigned short *str = (unsigned short *) rs[typ-1].s;

  CClientDC dc(this); // device context for painting

  if ( 0 <= x && x < 16 && 0 <= x && x < 16 ) {
    ::Rectan(&dc, x, y, (str[y] & (0x8000 >> x)) ? 0 : 1);
    str[y] ^= (0x8000 >> x);
    ::Rectan(&dc, rs[typ-1].x, rs[typ-1].y, 2);
  }
  

  CStatic::OnLButtonDown(nFlags, point);
}

void CRastrWnd2::OnRButtonDown(UINT nFlags, CPoint point) 
{

  int x = point.x/18, y = point.y/18, xx, yy;

  unsigned int *str = (unsigned int *) rs[typ-1].s;

  CClientDC dc(this); // device context for painting

  if ( 0 <= x && x < 16 && 0 <= x && x < 16 ) {
    xx = rs[typ-1].x; yy = rs[typ-1].y;
    ::Rectan(&dc, xx, yy, (str[yy] & (0x8000 >> xx)) ? 1 : 0);
    rs[typ-1].x = x; rs[typ-1].y = y;
    ::Rectan(&dc, x, y, (str[y] & (0x8000 >> x)) ? 1 : 0);
    ::Rectan(&dc, x, y, 2);
  }

 
  CStatic::OnRButtonDown(nFlags, point);
}
