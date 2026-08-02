#include "stdafx.h"
#include "gid6.h"

#include "colorsv.h"

#include "win.h"

extern GID_COLORS m_colors;

IMPLEMENT_DYNCREATE(CColorsWindow, CStatic)

BEGIN_MESSAGE_MAP(CColorsWindow, CStatic)
  //{{AFX_MSG_MAP(CColorsWindow)
  ON_WM_PAINT()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CColorsWindow::CColorsWindow()
{
  m_ring = 1.;
}

void line(CDC *dc, int x1, int y1, int x2, int y2, long c, long cZ, long th)
{
  CPen pen(PS_SOLID, th, c);
  dc->SelectObject(pen);

  CBrush brush(cZ);
  dc->SelectObject(brush);


  dc->MoveTo(x1, y1);
  dc->LineTo(x2, y2);

  int dx = ((x2 > x1) ? 8 : -8);

  CPoint fill[] = {
    CPoint((x1+x2)/2-dx, y1+3),
    CPoint((x1+x2)/2, y1),
    CPoint((x1+x2)/2-dx, y1-3)
  };

  dc->Polygon(fill,3);
}

void circle(CDC *dc, int x, int y, int r, long c1, long c2)
{
  CPen pen(PS_SOLID, 0, c1);
  CBrush *oldbrush;

  dc->SelectObject(pen);
  
  if ( c2 ) {
    CBrush brush(c2);
    oldbrush = (CBrush *) dc->SelectObject(brush);
    dc->Ellipse(x-r, y-r, x+r, y+r);
//    dc->SelectObject(oldbrush);
  }
  else {
    dc->Arc(x-r, y-r, x+r, y+r, x-r, 0, x-r, 0);
  }
}

void drawText(CDC *dc, int cx, int cy, const char *str);

void draw(CDC *dc, int x1, int y1, int x2, int y2, int _mag_ras, int _open_close, int _typ, int vid_nev, double m_ring, unsigned long color_uzel)
{
//  int st = m_colors.style(_mag_ras, _typ).m_style;
//  int d = m_colors.style(_mag_ras, _typ).m_distance;
//  double t = m_colors.style(_mag_ras, _typ).m_thickness;

  int st = m_colors.style(0, _typ).m_style;
  int d = m_colors.style(_mag_ras, _typ).m_distance;
  double t = m_colors.style(_mag_ras, _typ).m_thickness;


  long c1 = m_colors.pod(_open_close, _mag_ras, vid_nev);
  long c2 = m_colors.obr(_open_close, _mag_ras, vid_nev);

  long c1Z = m_colors.pod(3, _mag_ras, vid_nev);
  long c2Z = m_colors.obr(3, _mag_ras, vid_nev);

  if (st) {
    line(dc, x1, y1-d, x2, y2-d, c1, c1Z, t);
    line(dc, x2, y2+d, x1, y1+d, c2, c2Z, t);
  }
  else {
    line(dc, x1, y1-d, (x1+x2)/2-5, y2-d, c1, c1Z, t);
    line(dc, (x1+x2)/2+5, y1-d, x2, y2-d, c1, c1Z, t);

    line(dc, (x1+x2)/2-5, y2+d, x1, y1+d, c2, c2Z, t);
    line(dc, x2, y2+d, (x1+x2)/2+5, y1+d, c2, c2Z, t);
  }

//  circle(dc, x1, y1, 3*m_ring, RED, WHITE);
  circle(dc, x1, y1, 3*m_ring, RED, color_uzel);


  CFont font, *oldfont;
  LOGFONT lf;
  COLORREF color = 0;

  font.CreatePointFont(60, _T("Arial"));
  font.GetLogFont(&lf);
  font.DeleteObject();

  LoadFont(_T("Font Gidr"), lf, color);

//  double dm = dc->GetDeviceCaps(LOGPIXELSX)/2.54;

  if (lf.lfHeight == 0) lf.lfHeight = 1;
  font.CreateFontIndirect(&lf);

  oldfont = (CFont *) dc->SelectObject(&font);

  drawText(dc, x1, y1+5, _TR("Узел"));

  circle(dc, x2, y2, 5*m_ring, RED, WHITE);
  circle(dc, x2, y2, 3*m_ring, RED, RED);

  drawText(dc, x2, y2+7, _TR("Потребитель"));

  dc->SelectObject(oldfont);
}


void CColorsWindow::OnPaint() {

  CPaintDC dc(this);

  int x1, y1, x2, y2;

  CRect rect;
  GetClientRect(rect);

  x1 = y1 = 0;  x2 = rect.Width(), y2 = rect.Height();
  dc.Rectangle(x1, y1, x2-1, y2-1);

//  x1 = y1 = 40;  
//  x2 = 160, y2 = 40;

  draw(&dc, x1+40, y2/2, x2-50, y2/2, m_mag_ras, m_open_close, m_typ, 0, m_ring, m_color_uzel);
//  draw(&dc, 40, 80, 160, 80, m_mag_ras, m_open_close, m_typ, 1);
}
