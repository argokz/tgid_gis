// Podl.cpp : implementation file
//

#include "stdafx.h"
#include "Podl.h"
#include "lzw.h"
#include "lzw.hh"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int rgb_l(int color);


/////////////////////////////////////////////////////////////////////////////
// CPodl

CPodl::CPodl(const char *fN)  : m_fN(fN)
{
  f = fopen(fN, "rb"); if (!f) throw 0;

  fread((unsigned char*)&p, sizeof(PODL), 1, f);
  fclose(f);


  m_x0 = p.x0;
//  m_y0 = -(p.y0+p.dy);
  m_y0 = p.y0;
  m_dx0 = p.dx0;
  m_dy0 = p.dy0;
  m_typ = p.typ;
  m_x = p.x;
  m_y = p.y;

//  m_x0 = m_y0 = 300000;
//  m_dx0 = m_dy0 = 1;
}

CPodl::~CPodl()
{
}


void decod(unsigned char *p, unsigned char *u, int n);

void SetHeader(LPBITMAPINFOHEADER p_bminfo, int w, int h, int nplanes);

void CPodl::SetPalette(RGBQUAD *palette) {
  if (p.nColors) memmove(palette, Palette, p.nColors*sizeof(RGBQUAD));
}

int CPodl::readQQQ(int x, int y) {
  int n;
  long off1, off2;

//  x = 0;
//  y = 0;


  memset(image, -1, DD*BTS);

  if (x < 0 || y < 0) return 0;
  if (x >= p.xx || y >= p.yy) return 0;

  if (p.sign == 47316) 
      n = p.xx*y+x;
  else
  if (p.sign == 47316+1)
      n = p.xx*(p.yy-y-1)+x;

//  n = 27;

  off1 = off[n];
  off2 = off[n+1];

  if (off2-off1 > 1000000 || off1 >= off2 ) return 0;

  unsigned char *pp = new unsigned char[off2-off1+1];

  fseek(f, off1+sizeof(PODL), SEEK_SET);
  fread(pp, 1, off2-off1, f);

  switch ( typ ) {
    case 2: 
    case 0: memmove(image, pp, off2-off1);  break;
    case 1: decod(pp, image, off2-off1);    break;
    case 22: 
    if (0) {
        LZWCodec codec;
        vector<unsigned char> vec(pp, pp+off2-off1);
        vector<unsigned char> vec2;
        codec.decode(vec, vec2);

        memmove(image, &vec2[0], vec2.size());
//        kk = vec2.size();
    }
    
        LZW(DD * BTS * 2).decod(pp, image);         
        break;
  }

  delete [] pp;

  return 1;
}

LPVOID p_dib;

HANDLE CPodl::readQQQ1(int x1, int y1, int x2, int y2) {

  HANDLE hndl;
  unsigned char *data;
  unsigned char *im;
  int i, j, k, x, y, l, n, c, dd = DD*mas, m = 1/mas, obm = 8/m;
  unsigned short image_height = DD*mas*(y2-y1+1);
  unsigned short bytes_per_line = BTS*mas*(x2-x1+1);
  unsigned char c1, c2;
  union {
    unsigned int ui;
    unsigned char uc[2];
  } two;
  int m8 = m/8, m2 = m/2, BTSm = BTS*m, bit;

  bytes_per_line = (bytes_per_line+3)/4*4;

  DWORD size = sizeof(BITMAPINFOHEADER)+p.nColors*sizeof(RGBQUAD)+
        (long)image_height*bytes_per_line;

  hndl = ::GlobalAlloc(GMEM_MOVEABLE,size);
//  if (!hndl) THROW( TXOutOfMemory() );

//  LPVOID p_dib = (LPBITMAPINFO)::GlobalLock(hndl);
  p_dib = (LPBITMAPINFO)::GlobalLock(hndl);

  SetHeader((LPBITMAPINFOHEADER) p_dib, dd*(x2-x1+1), dd*(y2-y1+1), p.BitCount);

  SetPalette((RGBQUAD*) ((LPSTR)p_dib+sizeof(BITMAPINFOHEADER)));

  for (y = y1; y <= y2; y++) {
    for (x = x1; x <= x2; x++) {
      readQQQ(x, y);      im = image;
      data = (unsigned char *) p_dib + sizeof(BITMAPINFOHEADER)+p.nColors*sizeof(RGBQUAD)+
          ((long)(y-y1)*dd)*bytes_per_line+BTS*(x-x1)/m;

      if (m == 1) {
        if (p.nColors == 0) {
          for ( k = 0; k < DD; k++, im += BTS, data += bytes_per_line) {
            for ( i = 0; i < BTS; i += 3) {
//              data[i] = im[i];
              unsigned char im1[4];
            
              im1[0] = im[i];
              im1[1] = im[i+1];
              im1[2] = im[i+2];
            
              long *ii = (long*) &im1;
              ii[0] = rgb_l(ii[0]);

              data[i] = im1[0];
              data[i+1] = im1[1];
              data[i+2] = im1[2];
            }
          }
        }
        else {
          for ( k = 0; k < DD; k++, im += BTS, data += bytes_per_line) {
            for ( i = 0; i < BTS; i++) data[i] = im[i];
          }
        }
      }
      else if (p.nColors == 256) {
        for ( k = 0; k < dd; k++, im += BTSm, data += bytes_per_line) {
          for ( i = j = 0; i < BTS; i += m) data[j++] = im[i];
        }
      }
      else if (p.nColors == 16) {
        for ( k = 0; k < dd; k++, im += BTSm, data += bytes_per_line) {
          for ( i = j = 0; i < BTS; j++) {
            c1 = im[i]; i += m2;
            c2 = im[i] & 0xF; i += m2;
            data[j] = c2 | c1 << 4;
          }
        }
      }
      else if (p.nColors == 0) {
        for ( k = 0; k < dd; k++, im += BTSm, data += bytes_per_line) {
          for ( i = j = 0; i < BTS; i += m*3) {

            unsigned char im1[4];
            
            im1[0] = im[i];
            im1[1] = im[i+1];
            im1[2] = im[i+2];
            
            long *ii = (long*) &im1;
            ii[0] = rgb_l(ii[0]);

//            data[j++] = im[i];
//            data[j++] = im[i+1];
//            data[j++] = im[i+2];

            data[j++] = im1[0];
            data[j++] = im1[1];
            data[j++] = im1[2];
          
          
          }
        }
      }
      else {
        switch ( m ) {
          case 2:
          case 4:
            for ( k = 0; k < dd; k++, im += BTSm, data += bytes_per_line) {
              for ( i = j = 0; i < BTS; ) {

                for ( n = 0; n < m; n++ ) {
                  two.uc[0] = im[i++];
                  for ( l = 0; l < obm; l++) {
                    two.ui <<= 1; two.uc[0] <<= (m-1);
                  }
                }
                data[j++] = two.uc[1];
              }
            }
            break;
          case 8 :
          case 16:
            for ( k = 0; k < dd; k++, im += BTSm, data += bytes_per_line) {
              for ( i = j = 0; i < BTS; j++ ) {

                c = 0;
                for ( bit = 0x80, c = 0; bit; bit >>= 1 ) {
                  if (im[i]) c |= bit; i += m8;
                }
                data[j] = c;
              }
            }
            break;
        }
      }
    }
  }

  return hndl;
}


void CPodl::PaintQQQ(CDC *m_pDC, int tsx, int tsy)
{
  int i1, k1, i2, k2, i;
  double m, dd;

  if (!OpenPodl(m_fN)) return;

  dd = DD*mas;

  i1 = (m_rect.left-tsx)/dd;
  i2 = (m_rect.right-tsx)/dd;
  k1 = (m_rect.top-tsy)/dd;
  k2 = (m_rect.bottom-tsy)/dd;

  i1 = max(0, i1);  i2 = max(i1, min((int)p.xx-1, i2));
  k1 = max(0, k1);  k2 = max(k1, min((int)p.yy-1, k2));


  CRect cR =
      CRect(dd*i1 + tsx, dd*k1 + tsy, dd*(i2+1) + tsx, dd*(k2+1) + tsy);

  if (cR.left < 0) cR.left --;
  if (cR.top < 0) cR.top --;

  if (mas >= 1) {
    i = 1;
    mas = 1;
  }
  else {
    m = 1./mas;
    for ( i = 1; i < 16; i *= 2 ) if (i*2 > m || i == 8) break;
    mas = 1./i;
  }

  HANDLE hndl = readQQQ1(i1, p.yy-k2-1, i2, p.yy-k1-1);
  CRect iR = CRect(0, 0, (i2-i1+1)*DD/i, (k2-k1+1)*DD/i);

  if (hndl) {
    m_pDC->SetStretchBltMode(STRETCH_DELETESCANS);

//    m_pDC->StretchDIBits(cR, iR, TDib(hndl), SRCAND);

    StretchDIBits( m_pDC->m_hDC,
    cR.left, cR.top, cR.Width(), cR.Height(),
    iR.left, iR.top, iR.Width(), iR.Height(),
    ((LPSTR)p_dib+sizeof(BITMAPINFOHEADER)+p.nColors*sizeof(RGBQUAD)),
    (BITMAPINFO*)p_dib,
    DIB_RGB_COLORS, SRCAND);



    GlobalUnlock(hndl);
    GlobalFree(hndl);
  }
  ClosePodl();
}


void CPodl::PaintQQQ_P(CDC *m_pDC, int tsx, int tsy)
{
  int i1, k1, i2, k2, i, ii, kk;
  double m, dd;

  if (!OpenPodl(m_fN)) return;

  dd = DD*mas;

  i1 = (m_rect.left-tsx)/dd;
  i2 = (m_rect.right-tsx)/dd;
  k1 = (m_rect.top-tsy)/dd;
  k2 = (m_rect.bottom-tsy)/dd;

  i1 = max(0, i1);  i2 = max(i1, min((int)p.xx-1, i2));
  k1 = max(0, k1);  k2 = max(k1, min((int)p.yy-1, k2));


  if (mas >= 1) {
    i = 1;
    mas = 1;
  }
  else {
    m = 1./mas;
    for ( i = 1; i < 16; i *= 2 ) if (i*2 > m || i == 8) break;
    mas = 1./i;
  }

  CRect cR;

  for ( ii = i1; ii <= i2; ii++ ) {
    for ( kk = k1; kk <= k2; kk++ ) {
      cR = CRect(dd*ii + tsx, dd*kk + tsy, dd*(ii+1) + tsx, dd*(kk+1) + tsy);
      if (cR.left < 0) cR.left --;
      if (cR.top < 0) cR.top --;

      HANDLE hndl = readQQQ1(ii, p.yy-kk-1, ii, p.yy-kk-1);
      CRect iR = CRect(0, 0, DD/i, DD/i);

      if (hndl) {
        m_pDC->SetStretchBltMode(STRETCH_DELETESCANS);

        StretchDIBits( m_pDC->m_hDC,
        cR.left, cR.top, cR.Width(), cR.Height(),
        iR.left, iR.top, iR.Width(), iR.Height(),
        ((LPSTR)p_dib+sizeof(BITMAPINFOHEADER)+p.nColors*sizeof(RGBQUAD)),
        (BITMAPINFO*)p_dib,
        DIB_RGB_COLORS, SRCAND);

        GlobalUnlock(hndl);
        GlobalFree(hndl);
      }
    }
  }

  ClosePodl();
}


void CPodl::ClosePodl() {
  delete [] off;
  delete [] image;
  if (p.nColors) delete [] Palette;
  fclose(f);
}

int CPodl::OpenPodl(const char *filename) {
  int n;

  f = fopen(filename, "rb"); if (!f) return 0;

  fread((unsigned char*)&p, sizeof(PODL), 1, f);

/*
  mx1 = 0;
  my1 = 0;
  mx2 = p.x;
  my2 = p.y;
*/

////////////////////
//  mas = 1/masx;
///////////////////////

  BTS = p.BTS;
  DD = p.DD;
  typ = p.typ;

  n = p.xx*p.yy+1;
  off = new long[n];
  fread((unsigned char*)off, sizeof(long), n, f);
  image = new unsigned char[DD*BTS*2];

  if (p.nColors != 0) {
    fseek(f, sizeof(PODL)+off[n-1], SEEK_SET);
    Palette = new RGBQUAD[p.nColors];
    fread((unsigned char*) Palette, sizeof(RGBQUAD), p.nColors, f);

    long *lp = (long *) Palette;

    for (int i = 0; i < p.nColors; i++) {
      lp[i] = rgb_l(lp[i]);
    }

  }

  return 1;
}


void SetHeader(LPBITMAPINFOHEADER p_bminfo, int w, int h, int nplanes) {

  p_bminfo->biSize = sizeof(BITMAPINFOHEADER);
  p_bminfo->biWidth = w;
  p_bminfo->biHeight = h;
  p_bminfo->biPlanes = 1;
  p_bminfo->biBitCount = nplanes;
  p_bminfo->biCompression = BI_RGB;
  p_bminfo->biSizeImage = 0;
  p_bminfo->biXPelsPerMeter = 0;
  p_bminfo->biYPelsPerMeter = 0;
  p_bminfo->biClrUsed = 0;
  p_bminfo->biClrImportant = 0;
}

void SetPalette(RGBQUAD *palette, int nplanes) {
  int j;

  if (nplanes == 1) {
    palette[0].rgbRed = palette[0].rgbGreen = palette[0].rgbBlue = 0xFF;
    palette[0].rgbReserved = 0;
    palette[1].rgbRed = palette[1].rgbGreen = palette[1].rgbBlue = 0;
    palette[1].rgbReserved = 0;

//    long *l = (long*) &palette[0].rgbRed;
//    *l = rgb_l(*l);

//    l = (long*) &palette[1].rgbRed;
//    *l = rgb_l(*l);

  }
  else {
    for ( j = 0; j < 16; j++ ) {
      palette[j].rgbRed = palette[j].rgbGreen = palette[j].rgbBlue = 128;
      palette[j].rgbReserved = 0;
    }
//    palette[0].rgbRed = palette[0].rgbGreen = palette[0].rgbBlue = 0xC0;
    palette[0].rgbRed = palette[0].rgbGreen = palette[0].rgbBlue = 0xFF;
    palette[8].rgbRed = palette[8].rgbGreen = palette[8].rgbBlue = 0;
  }
}

int CPodl::readBBBhead(const char *filename, int &x, int &y) {
  int n;

  f = fopen(filename, "rb"); if (!f) return 0;

  fread((unsigned char*)&hdr, 1, sizeof(PCXHEADER), f);

  x = hdr.x2-hdr.x1+1;
  y = hdr.y2-hdr.y1+1;
  BTS = hdr.manuf;
  DD = BTS*8;

  n = (hdr.bplin+BTS-1)/BTS*((hdr.y2-hdr.y1)/DD+1)+1;
  off = new long[n];
  fread((unsigned char*)off, sizeof(long), n, f);

  image = new unsigned char[DD*BTS*hdr.nplanes];

  return 1;
}

int CPodl::readBBB(int x, int y) {
  int n;
  long off1, off2;

  if (x*DD > hdr.x2-hdr.x1 || y*DD > hdr.y2-hdr.y1) return 0;
  n = (hdr.bplin+BTS-1)/BTS*y+x;

  off1 = off[n];
  off2 = min(off[n+1], off1+30000);

  unsigned char *pp = new unsigned char[off2-off1+1];

  fseek(f, off1+128, SEEK_SET);
  fread(pp, 1, off2-off1, f);

  if (typ == 0)
      decod(pp, image, off2-off1);
  else
      LZW().decod(pp, image);

  delete [] pp;

  return 1;
}

HANDLE CPodl::readBBB1(int x1, int y1, int x2, int y2) {

  HANDLE hndl;
  unsigned char *data;
  unsigned char *im;
  int i, j, k, x, y, l, n, c, dd = DD*mas, m = 1/mas, obm = 8/m;
  unsigned short image_height = DD*mas*(y2-y1+1);
  unsigned short bytes_per_line = BTS*mas*(x2-x1+1);
  int palettesize = 2;
  union {
    unsigned int ui;
    unsigned char uc[2];
  } two;

  bytes_per_line = (bytes_per_line+3)/4*4;

  DWORD size = sizeof(BITMAPINFOHEADER)+palettesize*sizeof(RGBQUAD)+
        (long)image_height*bytes_per_line;

  hndl = ::GlobalAlloc(GMEM_MOVEABLE,size);
//  if (!hndl) THROW( TXOutOfMemory() );
//  LPVOID p_dib = (LPBITMAPINFO)::GlobalLock(hndl);
  p_dib = (LPBITMAPINFO)::GlobalLock(hndl);

  SetHeader((LPBITMAPINFOHEADER) p_dib, dd*(x2-x1+1), dd*(y2-y1+1), 1);
  ::SetPalette((RGBQUAD*) ((LPSTR)p_dib+sizeof(BITMAPINFOHEADER)), 1);

  if (m <= 1) {
    for (y = y2; y >= y1; y--) {
      for (x = x1; x <= x2; x++) {

        readBBB(x, y);      im = image;

        data = (unsigned char *) p_dib + sizeof(BITMAPINFOHEADER)+palettesize*sizeof(RGBQUAD)+
            ((long)(y2-y+1)*dd-1)*bytes_per_line+BTS*(x-x1);

        for ( k = 0; k < dd; k++, im += BTS, data -= bytes_per_line) {
          for ( i = 0; i < BTS; i++) data[i] = im[i];
        }
      }
    }
  }
  else if (m < 8) {
    for (y = y2; y >= y1; y--) {
      for (x = x1; x <= x2; x++) {

        readBBB(x, y);      im = image;

        data = (unsigned char *) p_dib + sizeof(BITMAPINFOHEADER)+palettesize*sizeof(RGBQUAD)+
            ((long)(y2-y+1)*dd-1)*bytes_per_line+BTS*(x-x1)/m;

        for ( k = 0; k < dd; k++, im += BTS*m, data -= bytes_per_line) {
          for ( i = j = 0; i < BTS; ) {

            for ( n = 0; n < m; n++ ) {
              two.uc[0] = im[i++];
              for ( l = 0; l < obm; l++) {
                two.ui <<= 1; two.uc[0] <<= (m-1);
              }
            }
            data[j++] = two.uc[1];
          }
        }
      }
    }
  }
  else {
    int m8 = m/8, BTSm = BTS*m, bit;

    for (y = y2; y >= y1; y--) {
      for (x = x1; x <= x2; x++) {

        readBBB(x, y);      im = image;

        data = (unsigned char *)p_dib+sizeof(BITMAPINFOHEADER)+palettesize*sizeof(RGBQUAD)
             + ((long)(y2-y+1)*dd-1)*bytes_per_line+BTS*(x-x1)/m;

         for ( k = 0; k < dd; k++, im += BTSm, data -= bytes_per_line) {
          for ( i = j = 0; i < BTS; j++ ) {

            c = 0;
            for ( bit = 0x80, c = 0; bit; bit >>= 1 ) {
              if (im[i]) c |= bit; i += m8;
            }

            data[j] = c;
          }
        }
      }
    }
  }
  return hndl;
}

HANDLE CPodl::readBBB4(int x1, int y1, int x2, int y2) {

  HANDLE hndl;
  unsigned char *data;
  unsigned char *im;
  int lineindex, byteindex, k, ll, j, x, y, dd = DD*mas, m = 1/mas, obm = 4*mas;
  unsigned short image_height = DD*mas*(y2-y1+1);
  unsigned short bytes_per_line = BTS*mas*(x2-x1+1)*4;
  int palettesize = 16;
  unsigned short loc;
  unsigned char c1, c2, b[4], bn;

  bytes_per_line = (bytes_per_line+3)/4*4;

  DWORD size = sizeof(BITMAPINFOHEADER)+palettesize*sizeof(RGBQUAD)+
        (long)image_height*bytes_per_line;

  hndl = ::GlobalAlloc(GMEM_MOVEABLE,size);
//  if (!hndl) THROW( TXOutOfMemory() );
//  LPVOID p_dib = (LPBITMAPINFO)::GlobalLock(hndl);
  p_dib = (LPBITMAPINFO)::GlobalLock(hndl);

  union {
    unsigned short ui;
    unsigned char uc[2];
  } two;

  SetHeader((LPBITMAPINFOHEADER) p_dib, dd*(x2-x1+1), dd*(y2-y1+1), 4);
  ::SetPalette((RGBQUAD*) ((LPSTR)p_dib+sizeof(BITMAPINFOHEADER)), 4);

  if (m < 8) {

    for (y = y2; y >= y1; y--) {
      for (x = x1; x <= x2; x++) {

        readBBB(x, y);      im = image;

        data = (unsigned char *)p_dib+sizeof(BITMAPINFOHEADER)+palettesize*sizeof(RGBQUAD)+
            ((long)(y2-y+1)*dd-1)*bytes_per_line+BTS*4*(x-x1)/m;

        for ( lineindex = 0; lineindex < dd; lineindex++, im += BTS*2*m, data -= bytes_per_line) {
          loc = 0;
          for ( byteindex = 0; byteindex < BTS; ) {
            for (k = bn = 0; k < m; k++, byteindex++) {
              c1 = im[byteindex];
              c2 = im[byteindex+BTS];

              for (j = ll = 0; j < obm; j++ ) {
                two.uc[0] = c1 << ll; two.ui <<= 1;
                two.uc[0] = c2 << ll; two.ui <<= 1;
                two.uc[1] <<= 2;
                ll += m;

                two.uc[0] = c1 << ll; two.ui <<= 1;
                two.uc[0] = c2 << ll; two.ui <<= 1;
                two.uc[1] <<= 2;
                ll += m;

                b[bn++] = two.uc[1];
              }
            }
            data[loc++] = b[0];
            data[loc++] = b[1];
            data[loc++] = b[2];
            data[loc++] = b[3];
          }
        }
      }
    }
  }
  else {
    int m8 = m/8, BTS2m = BTS*2*m;

    for (y = y2; y >= y1; y--) {
      for (x = x1; x <= x2; x++) {

        readBBB(x, y);      im = image;

        data = (unsigned char *)p_dib+sizeof(BITMAPINFOHEADER)+palettesize*sizeof(RGBQUAD)+
            ((long)(y2-y+1)*dd-1)*bytes_per_line+BTS*4*(x-x1)/m;

        for ( lineindex = 0; lineindex < dd; lineindex++, im += BTS2m, data -= bytes_per_line) {
          loc = 0;
          for ( byteindex = 0; byteindex < BTS;) {
            for (k = 0; k < m && byteindex < BTS; k++) {
              two.uc[0] = im[byteindex];     two.ui <<= 1;
              two.uc[0] = im[byteindex+BTS]; two.ui <<= 1;
              two.uc[1] <<= 2;
              byteindex += m8;

              two.uc[0] = im[byteindex];     two.ui <<= 1;
              two.uc[0] = im[byteindex+BTS]; two.ui <<= 1;
              two.uc[1] <<= 2;
              byteindex += m8;

              data[loc++] = two.uc[1];
            }
          }
        }
      }
    }
  }
  return hndl;
}

void CPodl::PaintBBB(CDC *m_pDC, int tsx, int tsy, int t)
{
  int i1, k1, i2, k2, i, ph, pw;
  double dd;
  HANDLE hndl;

  typ = t;

  if (!readBBBhead(m_fN, pw, ph)) return;

  dd = DD*mas;

  i1 = (m_rect.left-tsx)/dd;
  i2 = (m_rect.right-tsx)/dd;
  k1 = (m_rect.top-tsy)/dd;
  k2 = (m_rect.bottom-tsy)/dd;

  i1 = max(0, i1);  i2 = max(i1, min(pw/DD-1, i2));
  k1 = max(0, k1);  k2 = max(k1, min(ph/DD-1, k2));

  CRect iR,cR =
      CRect(0, 0, (i2-i1+1)*dd, (k2-k1+1)*dd) +
      CSize(dd*i1 + tsx, dd*k1 + tsy);

  if (cR.left < 0) cR.left --;
  if (cR.top < 0) cR.top --;

  double masold = mas;
  int m;

  if (mas >= 1) {
    i = 1;
    mas = 1;
  }
  else {
    m = 1./mas;
    for ( i = 1; i < 16; i *= 2 ) if (i*2 > m || i == 8) break;
    mas = 1./i;
  }

  iR = CRect(0, 0, (i2-i1+1)*DD/i, (k2-k1+1)*DD/i);
  int palettesize;

  if (hdr.nplanes == 1) {
      hndl = readBBB1(i1,k1, i2, k2);
      palettesize = 2;
  }
  else {
      hndl = readBBB4(i1, k1, i2, k2);
      palettesize = 16;
  }

  if (hndl) {
    m_pDC->SetStretchBltMode(STRETCH_DELETESCANS);

//    pDC->StretchDIBits(cR, iR, TDib(hndl), SRCAND);

    StretchDIBits( m_pDC->m_hDC,
    cR.left, cR.top, cR.Width(), cR.Height(),
    iR.left, iR.top, iR.Width(), iR.Height(),
//    ((LPSTR)p_dib+sizeof(BITMAPINFOHEADER)+p.nColors*sizeof(RGBQUAD)),
    ((LPSTR)p_dib+sizeof(BITMAPINFOHEADER)+palettesize*sizeof(RGBQUAD)),

    (BITMAPINFO*)p_dib,
    DIB_RGB_COLORS, SRCAND);

    GlobalUnlock(hndl);
    GlobalFree(hndl);
  }
  mas = masold;

  delete [] off;
  delete [] image;
  fclose(f);
}

void rectangle(CDC *pDC, int x1, int y1, int x2, int y2) {
  CPoint fil[5];

  fil[0] = CPoint(x1,y1);
  fil[1] = CPoint(x2,y1);
  fil[2] = CPoint(x2,y2);
  fil[3] = CPoint(x1,y2);
  fil[4] = CPoint(x1,y1);

  pDC->Polyline(fil, 5);
}


void CPodl::Draw(CDC *pDC, CRect rect, double m_bx, double m_by, double masx, double masy)
{
  double bbx, bby, mx, my;

  m_rect = rect;
//  m_pDC = pDC;

//  m_dy0 = m_dx0*m_y/m_x;

  mas = m_dx0/m_x/masx;

//  bbx = m_bx-m_x0/masx+m_rect.left;
//  bby = m_by+(m_y0+m_dy0)/masy+m_rect.top;

  bbx = m_bx-m_x0/masx;
  bby = m_by+(m_y0+m_dy0)/masy;

  mx = m_x*mas;  my = m_y*mas;


//  if (bbx+mx > m_rect.left && bby+my > m_rect.top 
//    && bbx < m_rect.right && bby < m_rect.bottom) {
//  if (mx > bbx && my > bby && bbx > m_rect.left-m_rect.right && bby > m_rect.top-m_rect.bottom) {
  if (mx > bbx && my > bby && bbx > -m_rect.right && bby > -m_rect.bottom) {

    if (mas >= 1./16.) {
      if (m_typ == 2) {
        if (pDC->IsPrinting()) {
          PaintQQQ_P(pDC, -bbx, -bby);
        }
        else {
          PaintQQQ(pDC, -bbx, -bby);
        }
      }
      else {
        PaintBBB(pDC, -bbx, -bby, m_typ);
      }
    }
    else {
    }
  }


/*
  CPen penBlack;
  penBlack.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
  CPen* pOldPen = pDC->SelectObject(&penBlack);

//  rectangle(pDC, bbx, bby, bbx+mx, bby+my);

  rectangle(pDC, m_x0/masx-m_bx, -(m_y0+m_dy0)/masy-m_by,
            (m_x0+m_dx0)/masx-m_bx, -m_y0/masy-m_by);


  pDC->SelectObject(pOldPen);
*/
}
