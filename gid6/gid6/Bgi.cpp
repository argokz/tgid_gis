s// Bgi.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "Bgi.h"
#include <stdlib.h>

int rgb_l(int color);

/////////////////////////////////////////////////////////////////////////////
// CBgi

IMPLEMENT_DYNCREATE(CBgi, CView)

COLORREF rgb_color(int c) {
  COLORREF rgb[] = {
    0x000000, 0x7F0000, 0x007F00, 0x7F7F00, 0x00007F, 0x7F007F, 0x007F7F,
    0xBFBFBF, 0x7F7F7F, 0xFF0000, 0x00FF00, 0xFFFF00, 0x0000FF, 0xFF00FF,
    0x00FFFF, 0xFFFFFF,
  };

  if (0 <= c && c < 16) return rgb[c];
  return 0;
}

CBgi::CBgi()
{
  m_wait = 0;
  CursorModule = 0;
  CursorResId = 0;
  HCursor = 0;

  oldpen = pen = 0;
  oldbrush = brush = 0;
  oldfont = font = 0;
  is_paint = 0;
  m_h = 1;
//  m_pDC = NULL;
}

CBgi::~CBgi()
{
}


BEGIN_MESSAGE_MAP(CBgi, CView)
  //{{AFX_MSG_MAP(CBgi)
  ON_WM_CREATE()
  ON_WM_DESTROY()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBgi drawing

void CBgi::OnDraw(CDC* /*pDC*/)
{
  CDocument* pDoc = GetDocument();
  // TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CBgi diagnostics

#ifdef _DEBUG
void CBgi::AssertValid() const
{
  CView::AssertValid();
}

void CBgi::Dump(CDumpContext& dc) const
{
  CView::Dump(dc);
}
#endif //_DEBUG

void CBgi::SetDC(CDC *m_dc)
{
//    m_dc = DC;
    CClientDC screenDC(NULL);
    m_h = (double) m_dc->GetDeviceCaps(LOGPIXELSX)/screenDC.GetDeviceCaps(LOGPIXELSX);

    oldpen = pen = 0;
    oldbrush = brush = 0;
    oldfont = font = 0;

    m_dc->RealizePalette();
//    return m_dc;
}


CDC * CBgi::BeginPaint()
{
    if (is_paint) return NULL;
    is_paint = 1;

//    log1("CDC * CBgi::BeginPaint()");

    CDC *m_dc = (CDC*) new CClientDC(this);
//    CDC *m_dc = (CDC*) new CPaintDC(this);

    GetClientRect(&m_rect);

    CClientDC screenDC(NULL);
    m_h = (double) m_dc->GetDeviceCaps(LOGPIXELSX)/screenDC.GetDeviceCaps(LOGPIXELSX);

    oldpen = pen = 0;
    oldbrush = brush = 0;
    oldfont = font = 0;

    m_dc->RealizePalette();

    return m_dc;
}

/*
CDC * CBgi::BeginMemoPaint(CDC *dc, int dx, int dy)
{
    if (is_paint) return NULL;
    is_paint = 1;
    CDC *m_dc = (CDC*) new CDC();

//    CDC *m_dc = new CPaintDC(this);

    m_dc->CreateCompatibleDC(0);

//    CDC *m_dc = (CDC*) new CPaintDC(this);

    m_rect =  CRect(0, 0, dx, dy);

    CClientDC screenDC(NULL);
    m_h = (double) m_dc->GetDeviceCaps(LOGPIXELSX)/screenDC.GetDeviceCaps(LOGPIXELSX);

    oldpen = pen = 0;
    oldbrush = brush = 0;
    oldfont = font = 0;

    m_dc->RealizePalette();

    return m_dc;
}
*/


void CBgi::EndPaint(CDC *m_dc)
{
    if (pen) {
      m_dc->SelectObject(oldpen);
      delete pen;
    }
    if (brush) {
      m_dc->SelectObject(oldbrush);
      delete brush;
    }
    if (font) {
      m_dc->SelectObject(oldfont);
      delete font;
    }
    pen = oldpen = 0;
    brush = oldbrush = 0;
    font = oldfont = 0;
    delete m_dc;
    is_paint = 0;
//    log1("void CBgi::EndPaint(CDC *m_dc)");


}


void CBgi::SetRect(CRect r)
{
    m_rect = r;
}

void CBgi::rectangle(CDC *m_dc, int x1, int y1, int x2, int y2) {
  CPoint fil[5];

  fil[0] = CPoint(x1,y1);
  fil[1] = CPoint(x2,y1);
  fil[2] = CPoint(x2,y2);
  fil[3] = CPoint(x1,y2);
  fil[4] = CPoint(x1,y1);

  m_dc->Polyline(fil, 5);
}

void CBgi::fillrectangle(CDC *m_dc, int x1, int y1, int x2, int y2) {
  CPoint fil[5];

  fil[0] = CPoint(x1,y1);
  fil[1] = CPoint(x2,y1);
  fil[2] = CPoint(x2,y2);
  fil[3] = CPoint(x1,y2);
  fil[4] = CPoint(x1,y1);

  m_dc->Polygon(fil, 5);
}


void CBgi::xrectangle(CDC *m_dc, int x1, int y1, int x2, int y2) {
  int saveROP;

  saveROP = m_dc->SetROP2(R2_NOTXORPEN);
  rectangle(m_dc, x1, y1, x2, y2);
  m_dc->SetROP2(saveROP);
}

void CBgi::xline(CDC *m_dc, int x1, int y1, int x2, int y2) {
  int saveROP;

  saveROP = m_dc->SetROP2(R2_NOTXORPEN);
  line(m_dc, x1, y1, x2, y2);
  m_dc->SetROP2(saveROP);
}

void CBgi::xlineto(CDC *m_dc, int x, int y) {
  int saveROP;

  saveROP = m_dc->SetROP2(R2_NOTXORPEN);
  lineto(m_dc, x, y);
  m_dc->SetROP2(saveROP);
}


void CBgi::setcolor(CDC *m_dc, int c) {
  color = c;
  setline(m_dc, c, 1);
}


void CBgi::setcolor(CDC *m_dc, int c, int l) {
  setline(m_dc, c, l);
}


void CBgi::setline(CDC *m_dc, int c, int l)
{
  setline(m_dc, c, (double)l);
}

void CBgi::setline(CDC *m_dc, int c, double l)
{
  setline(m_dc, c, l, PS_SOLID);
}


void CBgi::fillellipse(CDC *m_dc, int x, int y, int rx, int ry) {
  m_dc->Ellipse(x-rx, y-ry, x+rx, y+ry);
}

void CBgi::ellipse(CDC *m_dc, int x, int y, int rx, int ry) 
{
  m_dc->Arc(x-rx, y-ry, x+rx, y+ry, x-rx, 0, x-rx, 0);
}

static int npen = 0;

int getNpen()
{
  return npen;
}



void CBgi::setfillstyle(CDC *m_dc, int /*typ*/, int c) 
{
  if (brush) {
    if (oldbrush) m_dc->SelectObject(oldbrush);
    brush->DeleteObject();
    delete brush;
  }
  brush = new CBrush(rgb_l(c));
  oldbrush = m_dc->SelectObject(brush);
}

void CBgi::setline(CDC *m_dc, int c, double l, int style)
{
  if (pen) {
    if (oldpen) m_dc->SelectObject(oldpen);
    pen->DeleteObject();
    delete pen;
    npen--;
  }

  int h = m_h*l;

  if (m_h > 1 && h < m_h) {
    h = m_h;
  }
  pen = new CPen(style, h, rgb_l(c));
  oldpen = m_dc->SelectObject(pen);
  npen++;

  color = c;
}

void CBgi::EndPaint2(CDC *m_dc)
{
  if (pen) {
    m_dc->SelectObject(oldpen);
    pen->DeleteObject();
    delete pen;
    npen--;
  }
  if (brush) {
    m_dc->SelectObject(oldbrush);
    brush->DeleteObject();
    delete brush;
  }
  if (font) {
    m_dc->SelectObject(oldfont);
    delete font;
  }
  pen = oldpen = 0;
  brush = oldbrush = 0;
  font = oldfont = 0;
}


void CBgi::putpixel(CDC *m_dc, int x, int y, int col) {
  m_dc->SetPixel(x, y, col);
}

void CBgi::line(CDC *m_dc, int x1, int y1, int x2, int y2) 
{
  m_dc->MoveTo(x1, y1);
  m_dc->LineTo(x2, y2);



//  Line(m_dc, x1, y1, x2, y2);
}

bool LiangBar(int XS1, int YS1, int XS2, int YS2,
  double& x0, double& y0, double& x1, double& y1);


void CBgi::line(CDC *m_dc, int _x1, int _y1, int _x2, int _y2, CRect rect) 
{
  double x1 = _x1, y1 = _y1, x2 = _x2, y2 = _y2;

  if (LiangBar(rect.left, rect.top, rect.right, rect.bottom, x1, y1, x2, y2)) {
    m_dc->MoveTo(x1, y1);
    m_dc->LineTo(x2, y2);
  }
}


void CBgi::hline(CDC *m_dc, double x1, double y1, double x2, double y2) {
  double hx1, hy1, hx2, hy2;

  hx1 = max((double)m_rect.left-1.,x1);
  hx1 = min((double)m_rect.right+1, hx1);

  hx2 = min((double)m_rect.right+1, x2);
  hx2 = max((double)m_rect.left-1.,hx2);

  hy1 = max((double)m_rect.top-1,y1);
  hy1 = min((double)m_rect.bottom+1, hy1);

  hy2 = min((double)m_rect.bottom+1, y2);
  hy2 = max((double)m_rect.top-1,hy2);

  m_dc->MoveTo((int)hx1, (int)hy1);
  m_dc->LineTo((int)hx2, (int)hy2);
}


void CBgi::lineto(CDC *m_dc, int x, int y) {
  m_dc->LineTo(x, y);
}

void CBgi::moveto(CDC *m_dc, int x, int y) {
  m_dc->MoveTo(x, y);
}

void CBgi::moveto(CDC *m_dc, POINT p) {
  m_dc->MoveTo(p);
}

void CBgi::lineto(CDC *m_dc, POINT p) {
  m_dc->LineTo(p);
}


void CBgi::fillpoly(CDC *m_dc, int n, CPoint *p) 
{
  m_dc->Polygon(p,n);
}


void CBgi::fillpoly(CDC *m_dc, int n, int *fil)
{
  int i, k;
  CPoint *p = new CPoint[n];

  for ( i = k = 0; i < n; i++, k+= 2 ) p[i] = CPoint(fil[k],fil[k+1]);

  m_dc->Polygon(p,n);
  delete [] p;
}


void CBgi::drawpoly(CDC *m_dc, int n, int *fil) {
  int i, k;
  CPoint *p = new CPoint[n];

  for ( i = k = 0; i < n; i++, k+= 2 ) p[i] = CPoint(fil[k],fil[k+1]);

  m_dc->Polyline(p,n);
  delete [] p;
}

void CBgi::Mxy(int x, int y) {
  CPoint pt(x, y);

  ClientToScreen(&pt);

  SetCursorPos(pt.x, pt.y);
}

BOOL CBgi::MySetCursor(CWinApp *module, LPCTSTR resId)
{
  if (m_wait) return FALSE;

  if (module == CursorModule && resId == CursorResId)
    return false;


  HCURSOR hOldCursor = (HCursor && CursorModule) ? HCursor : 0;

  CursorModule = module;
  CursorResId = resId;
  if (CursorResId)
    if (CursorModule)
      HCursor = CursorModule->LoadCursor(CursorResId);
    else
      HCursor = ::LoadCursor(0, CursorResId);
  else
    HCursor = 0;

  // If the cursor is in our client window then set it now
  //
//  if (GetHandle()) {
  if (GetSafeHwnd()) {
    CPoint p;
    CRect r;
    GetCursorPos(&p);
    ScreenToClient(&p);
    GetClientRect(&r);
    if (r.PtInRect(p))
      ::SetCursor(HCursor);
  }

    ::SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (long)HCursor);

  // Destroy old cursor if there was one & it was not loaded from USER
  //
  if (hOldCursor)
    ::DestroyCursor(hOldCursor);
  return true;
}

Percent::Percent() {
  indic = new CProgressCtrl();
//  indic->Create(WS_VISIBLE, CRect(0,0,300,18), pm_ctlStatus, 100);

  pp = -1;
//  indic->SetRange(0, 100);
//  SetValue(0);
}

Percent::~Percent() {
  delete indic;
}

void Percent::SetValue(int p) {
  if (p != pp) {
    pp = p;
//    indic->SetPos(p);
  }
}

int segmint(
    double x11, double y11, double x12, double y12,
    double x21, double y21, double x22, double y22,
    double& x, double& y,
    double& x31, double& y31, double& x32, double& y32
    );

void CBgi::lineR(CDC *m_dc, CPoint p1, CPoint p2) {

  if (m_rect.PtInRect(p1) && m_rect.PtInRect(p2)) {
    m_dc->MoveTo(p1);
    m_dc->LineTo(p2);
  }
  else {
    double x, y, x31, y31, x32, y32;
    int i, s, np = 0;
    CPoint p[4];

    for ( i = 0; i < 4 && np < 2; i++ ) {
      switch ( i ) {
        case 0 :
          s = segmint(p1.x, p1.y, p2.x, p2.y,
              m_rect.left, m_rect.top, m_rect.right, m_rect.top,
              x, y, x31, y31, x32, y32);
          break;
        case 1 :
          s = segmint(p1.x, p1.y, p2.x, p2.y,
              m_rect.left, m_rect.bottom, m_rect.right, m_rect.bottom,
              x, y, x31, y31, x32, y32);
          break;
        case 2 :
          s = segmint(p1.x, p1.y, p2.x, p2.y,
              m_rect.left, m_rect.bottom, m_rect.left, m_rect.top,
              x, y, x31, y31, x32, y32);
          break;
        case 3 :
          s = segmint(p1.x, p1.y, p2.x, p2.y,
              m_rect.right, m_rect.bottom, m_rect.right, m_rect.top,
              x, y, x31, y31, x32, y32);
          break;
      }

      if (s == 1) {
        p[np++] = CPoint(x, y);
      }
      else if (s == 2) {
        p[np++] = CPoint(x31, y31);
        p[np++] = CPoint(x32, y32);
      }
    }
    if (np >= 2) {
      m_dc->MoveTo(p[0]);
      m_dc->LineTo(p[1]);
    }
    else if (np == 1) {
      m_dc->MoveTo(p[0]);
      if (m_rect.PtInRect(p1))
          m_dc->LineTo(p1);
      else
          m_dc->LineTo(p2);
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
// CBgi message handlers

void CBgi::OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView)
{
  // TODO: Add your specialized code here and/or call the base class

  CView::OnEndPrintPreview(pDC, pInfo, point, pView);

  void SetBar(CView *pView);

  SetBar(NULL);
  SetBar(this);

}

void saveBitmap(const CBitmap &bitmap, const char *fn)
{
  BITMAP bm;
  HGLOBAL             hMem;
  DWORD cb;
  LPBYTE              lpbDst;
  BITMAPFILEHEADER  bfh;


  bitmap.GetObject( sizeof(BITMAP), &bm );

  cb = bm.bmHeight*bm.bmWidthBytes * bm.bmPlanes;

  hMem=GlobalAlloc(GHND, cb);
  lpbDst=(LPBYTE)GlobalLock(hMem);

  bitmap.GetBitmapBits( cb, lpbDst );


  BITMAPINFOHEADER BIH;
  int iSize = sizeof(BITMAPINFOHEADER) ;
  memset(&BIH, 0, iSize);

  // Fill in the header info.
  BIH.biSize = iSize;
  BIH.biWidth = bm.bmWidth;
  BIH.biHeight = bm.bmHeight;
  BIH.biPlanes = bm.bmPlanes;
  BIH.biBitCount = bm.bmWidthBytes/bm.bmWidth;
  BIH.biBitCount = 32;
  BIH.biCompression = BI_RGB;

  FILE *f;

  f = fopen(fn, "wb");

  bfh.bfType = 0x4D42;
  bfh.bfSize = cb+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
  bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

  fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, f);
  fwrite(&BIH, sizeof(BITMAPINFOHEADER), 1, f);

  for (int i = 0; i < bm.bmHeight; i++) {
    fwrite((void*)&lpbDst[(bm.bmHeight-i-1)*bm.bmWidthBytes], bm.bmWidthBytes, 1, f);
  }
//  fwrite(lpbDst, cb, 1, f);

  fclose(f);

  GlobalUnlock(hMem);
}

void CBgi::MyTextOut(CDC *m_dc, int x, int y, const char *s)
{
  int lp, ls;

  CClientDC sDC(NULL);

  lp = min(DPIPR, m_dc->GetDeviceCaps(LOGPIXELSX));
  ls = sDC.GetDeviceCaps(LOGPIXELSX);

  if (1) {
//  if (lp == ls) {
//  if (0) {
    m_dc->TextOut(x, y, s);
    return;
  }

  UINT align = m_dc->GetTextAlign();

  int i, len;
  char ss[2];
  CSize size;
  CSize size1;

  ss[1] = 0;

  for (i = len = 0; s[i]; i++) {
    ss[0] = s[i];
    size = sDC.GetTextExtent(ss, 1);
    len += size.cx;
  }

  if ((align & 0xF) == TA_RIGHT) x -= len;
  if ((align & 0xF) == TA_CENTER) x -= len/2;

  for (i = 0; s[i]; i++) {
    ss[0] = s[i];
    m_dc->TextOut(x, y, ss);
    size = sDC.GetTextExtent(ss, 1);
    x += size.cx;
  }
}


int CBgi::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CView::OnCreate(lpCreateStruct) == -1)
    return -1;

  // m_pDC must be initialized here instead of the constructor
  // because the HWND isn't created until Create is called.
//  m_pDC = new CClientDC(this);
//  UpdateTitle();

  return 0;
}


void CBgi::OnDestroy()
{
//  OnKillThreads();

//  delete m_pDC;
//  m_pDC = NULL;

  CView::OnDestroy();
}


#if 0


void PaintTranslucentPolygon(HDC hDC, const POINT pVerts[], int NumVerts, COLORREF FillColor, int Opacity)
{
  const unsigned int TransparentColor = 0xFF00FF; // magenta (ядовито-розовый цвет)

  assert(NumVerts >= 3);
  assert(FillColor != RGB(255, 0, 255)); // не magenta
  assert((0 <= Opacity) && (Opacity <= 255));

  // считаем границы многоугольника
  int MinX = INT_MAX;
  int MaxX = INT_MIN;
  int MinY = INT_MAX;
  int MaxY = INT_MIN;
  for (int i = 0; i < NumVerts; i++)
  {
    int x = pVerts[i].x;
    if (x < MinX) MinX = x;
    if (x > MaxX) MaxX = x;
    int y = pVerts[i].y;
    if (y < MinY) MinY = y;
    if (y > MaxY) MaxY = y;
  }
  // слегка раздвигаем границы (чтобы избежать нежелательного отсечения)
  MinX--;
  MaxX++;
  MinY--;
  MaxY++;

  // создаём временный буфер для вне-экранного рисования
  int BufWidth = MaxX - MinX + 1;
  int BufHeight = MaxY - MinY + 1;
  BITMAPINFO bi;
  bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bi.bmiHeader.biWidth = BufWidth;
  bi.bmiHeader.biHeight = BufHeight;
  bi.bmiHeader.biPlanes = 1;
  bi.bmiHeader.biBitCount = 32;
  bi.bmiHeader.biCompression = BI_RGB;
  bi.bmiHeader.biSizeImage = 0;
  bi.bmiHeader.biXPelsPerMeter = 0;
  bi.bmiHeader.biYPelsPerMeter = 0;
  bi.bmiHeader.biClrUsed = 0;
  bi.bmiHeader.biClrImportant = 0;
  void* pBufPixels;
  HBITMAP hBuf = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, &pBufPixels, NULL, 0);

  // заполняем буфер прозрачным цветом
  int NumPixels = BufHeight * BufWidth;
  unsigned int* pPix = static_cast<unsigned int*>(pBufPixels);
  for ( i = 0; i < NumPixels; i++)
  {
    *pPix++ = TransparentColor;
  }

  // рисуем многоугольник в буфер
  HDC hBufDC = CreateCompatibleDC(NULL);
  SelectObject(hBufDC, hBuf);
  SelectObject(hBufDC, GetStockObject(DC_BRUSH));
  SetDCBrushColor(hBufDC, FillColor);
  SelectObject(hBufDC, GetStockObject(NULL_PEN)); // контур не рисовать
  SetWindowOrgEx(hBufDC, MinX, MinY, NULL);
  Polygon(hBufDC, pVerts, NumVerts);
  SetWindowOrgEx(hBufDC, 0, 0, NULL);

  // приводим содержимое буфера к формату с premultiplied alpha
  pPix = static_cast<unsigned int*>(pBufPixels);
  for ( i = 0; i < NumPixels; i++)
  {
    unsigned int c = *pPix;
    if (c == TransparentColor)
    {
      *pPix = 0;
    }
    else
    {
      c |= 0xFF000000; // сделать alpha = 255
      *pPix = c;
    }
    pPix++;
  }

  // рисуем содержимое буфера в указанный DC, со смешиванием (blending) цветов
  BLENDFUNCTION bf;
  bf.BlendOp = AC_SRC_OVER;
  bf.BlendFlags = 0;
  bf.SourceConstantAlpha = Opacity;
  bf.AlphaFormat = AC_SRC_ALPHA;
  AlphaBlend(hDC, MinX, MinY, BufWidth, BufHeight, hBufDC, 0, 0, BufWidth, BufHeight, bf);

  // освобождаем временные GDI-объекты
  DeleteDC(hBufDC);
  DeleteObject(hBuf);
}

#endif


