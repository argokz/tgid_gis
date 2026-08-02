ç// Bmp.cpp : implementation file
//

#include "stdafx.h"
#include "Bmp.h"
#include <io.h>
#include "dibapi.h"

/////////////////////////////////////////////////////////////////////////////
// CBmp

CBmp::CBmp(const char *fN)  : m_fN(fN)
{
  BITMAPFILEHEADER bf;
  BITMAPINFOHEADER bi;
  FILE *f;
  f = fopen(fN, "rb"); if (!f) throw 0;

  fread((void  *) &bf, sizeof(BITMAPFILEHEADER), 1, f);
  fread((unsigned char*)&bi, sizeof(BITMAPINFOHEADER), 1, f);
  fclose(f);

  m_x = bi.biWidth;
  m_y = bi.biHeight;

  m_x0 = m_y0 = 300000;
  m_dx0 = m_dy0 = 0;
}

CBmp::~CBmp()
{
}



void CBmp::PaintBMP(CDC *m_pDC, int bbx, int bby, int mx, int my)
{
  BITMAPINFO *bminfo;
  BITMAPFILEHEADER bf;
  BITMAPINFOHEADER bh;
  HANDLE hndl;
  FILE *f;
  int size;

  CRect iR, cR;

  cR = CRect(bbx, bby, bbx+mx, bby+my) ;
  iR = CRect(0, 0, m_x, m_y);

  if (cR.left < m_rect.left) {
    iR.left += (m_rect.left-cR.left)*m_x/mx;
    cR.left = m_rect.left;
  }

  if (cR.right > m_rect.right) {
    iR.right -= (cR.right-m_rect.right)*m_x/mx;
    cR.right = m_rect.right;
  }

  if (cR.top < m_rect.top) {
    iR.bottom -= (m_rect.top-cR.top)*m_y/my;
    cR.top = m_rect.top;
  }

  if (cR.bottom > m_rect.bottom) {
    iR.top += (cR.bottom-m_rect.bottom)*m_y/my;
    cR.bottom = m_rect.bottom;
  }


  f = fopen(m_fN, "rb"); if (!f) return;
  fread((void  *) &bf, sizeof(BITMAPFILEHEADER), 1, f);
  fread((unsigned char*)&bh, sizeof(BITMAPINFOHEADER), 1, f);
  fclose(f);

  f = fopen(m_fN, "rb"); if (!f) return;
  fread((void *) &bf, sizeof(BITMAPFILEHEADER), 1, f);

  size = bf.bfSize-sizeof(BITMAPFILEHEADER);

  hndl = ::GlobalAlloc(GMEM_MOVEABLE, size);

  if (hndl) {
    bminfo = (LPBITMAPINFO)::GlobalLock(hndl);
    unsigned char *info = ((unsigned char*)bminfo)+bh.biSize+ ::PaletteSize((LPSTR)&bh);

    fread(bminfo, size, 1, f);

    m_pDC->SetStretchBltMode(STRETCH_DELETESCANS);

    StretchDIBits( m_pDC->m_hDC,
//      bbx, bby, mx, my, 0, 0, m_x, m_y,
    cR.left, cR.top, cR.Width(), cR.Height(),
    iR.left, iR.top, iR.Width(), iR.Height(),
      info,
      bminfo,
      DIB_RGB_COLORS, SRCAND);

    GlobalUnlock(hndl);
    GlobalFree(hndl);
  }

  fclose(f);
}

void rectangle(CDC *pDC, int x1, int y1, int x2, int y2);


void CBmp::Draw(CDC *pDC, CRect rect, long m_bx, long m_by, double masx, double masy)
{
  int bbx, bby, mx, my;

  m_rect = rect;
//  m_pDC = pDC;

  if (m_dx0 == 0. && m_dy0 == 0) {
    m_dx0 = m_x/100.;
    m_dy0 = m_y/100.;
  }
  if (m_dx0 == 0.) m_dx0 = m_dy0*m_x/m_y;
  if (m_dy0 == 0.) m_dy0 = m_dx0*m_y/m_x;

  mx = m_dx0/masx;  my = m_dy0/masy;

  bbx = m_x0/masx-m_bx;
  bby = m_y0/masy-m_by;

  if (bbx+mx > m_rect.left && bby+my > m_rect.top 
    && bbx < m_rect.right && bby < m_rect.bottom) {

    PaintBMP(pDC, bbx, bby, mx, my);
  }
/*
  CPen penBlack;
  penBlack.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
  CPen* pOldPen = pDC->SelectObject(&penBlack);

  rectangle(pDC, bbx, bby, bbx+mx, bby+my);

  pDC->SelectObject(pOldPen);
*/
}

void CBmp::Draw(CScroll *scr, CDC *m_dc)
{
//  Draw(scr->m_dc, scr->m_rect, scr->m_bx, scr->m_by, scr->masx, scr->masy);
  Draw(m_dc, scr->m_rect, scr->geom.m_bx, scr->geom.m_by, scr->geom.masx, scr->geom.masy);
}
