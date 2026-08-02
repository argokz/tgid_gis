#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bbb.h"

#include "lzw.h"
#include "lzw.hh"


int const DD = 128;
//int const DD = 256;

void LzwInit();
void LzwExit();
int LzwCode(unsigned char *p, unsigned char *u, int np);
void decod(unsigned char *p, unsigned char *u, unsigned int n);
unsigned int cod(unsigned char *p, unsigned char *u, unsigned int n);

long *off, n_off, off_pr;
long xx, yy;

void QQ1(FILE* g, char* aaa, int bp, int dd, int BTS, int typ) {
    unsigned int i, l, j;
    unsigned int n, kk;
    long k;
    unsigned char* p, * u = nullptr;

    p = new unsigned char[dd * BTS];

    memset(p, 0, dd * BTS);


    if (typ != -2) {
        u = new unsigned char[dd * BTS * 2];
        memset(u, 0, dd * BTS * 2);
    }

//  bp = 0;

  LZWCodec codec;

  for ( i = 0; i < xx; i++) {
    memset(p, -1, dd*BTS);
    memset(p, 0, dd*BTS);
    for ( n = 0, l = 0; l < dd; l++) {
      k = (long)l*bp+i*BTS;
      for ( j = 0; j < BTS; j++ ) {
        p[n++] = aaa[k++];
      }
    }

    switch ( typ ) {
      case 0:
      case 2:
        kk = n;
        fwrite(p, kk, 1, g);
        break;
      case 1:
        kk = cod(p, u, n);
        fwrite(u, kk, 1, g);
        break;
      case 22:

/*
        {
            vector<unsigned char> vec(p, p+n);
            vector<unsigned char> vec2;
            codec.encode(vec, vec2);

            u = &vec2[0];
            kk = vec2.size();

        }

        fwrite(u, kk, 1, g);


        if (n_off != -27) {
            vector<unsigned char> vec(u, u + kk);
            vector<unsigned char> vec2;
            codec.decode(vec, vec2);
        }
*/



        kk = LzwCode(p, u, n/2);
        fwrite(u, kk, 1, g);

        if (n_off == -27) {
            unsigned char* image = new unsigned char[dd * BTS * 2];
            LZW(dd * BTS * 2).decod(u, image);

            for (int j = 0; j < n; j++) {
                if (p[j] != image[j]) {
                    int qqq;
                    qqq = 0;
                }
            }
        }

        break;
    }
    off[n_off++] = off_pr;
    off_pr += kk;
  }
  delete [] p;
  if (typ != -2) {
      delete[] u;
  }
}

void bmp2bbb1(FILE *f, FILE *g) {
  unsigned int i, l, bp;
  long len_aaa, npcx = 0;
  int BTS;
  char *aaa;
  long n_aaa = 0;
  PODL podl;
  BITMAPFILEHEADER bf;
  BITMAPINFOHEADER bi;
  RGBQUAD *rgb = NULL;
  int j, n;

  n_off = 0; off_pr = 0;

  fread((void  *) &bf, sizeof(BITMAPFILEHEADER), 1, f);
  fread((void  *) &bi, sizeof(BITMAPINFOHEADER), 1, f);

  switch ( bi.biBitCount ) {
    case 1 : n = 2;   BTS = DD/8; bp = (bi.biWidth+7)/8; break;
    case 4 : n = 16;  BTS = DD/2; bp = (bi.biWidth+1)/2;  break;
    case 8 : n = 256; BTS = DD;   bp = bi.biWidth;  break;
    case 24: n = 0;   BTS = DD*3; bp = bi.biWidth*3;  break;
  }

  bp = (bp+3)/4*4;

  if (n) {
    rgb = new RGBQUAD[n];
    fread((void  *) rgb, sizeof(RGBQUAD), n, f);
  }
  fseek(f, bf.bfOffBits, SEEK_SET);

  podl.nColors = n;

  if (bi.biSizeImage) bp = bi.biSizeImage/bi.biHeight;

  podl.sign = 47316;
  podl.typ = 2;
  podl.DD = DD;
  podl.BTS = BTS;
  podl.dx = 0;
  podl.dy = 0;
  podl.x = bi.biWidth;
  podl.y = bi.biHeight;
  podl.xx = (podl.x+podl.DD-1)/podl.DD;
  podl.yy = (podl.y+podl.DD-1)/podl.DD;
  podl.BitCount = bi.biBitCount;

  podl.x0 = 40000;
  podl.y0 = 40000-podl.y;
  podl.dx0 = podl.x;
  podl.dy0 = podl.y;

  fwrite((void*) &podl, sizeof(PODL), 1, g);

  xx = podl.xx;
  yy = podl.yy;

  off = new long[xx*yy+1];
  fwrite(off, 4, (xx*yy+1), g);
  off_pr = xx*yy*4+4;

  len_aaa = (long)xx*DD*BTS;
  aaa = new char [len_aaa];

  int len = xx*BTS;

  if (podl.typ) LzwInit();

  for (i = 0; i < bi.biHeight; ) {
    n_aaa = 0;
    for (j = 0; j < len_aaa; j++ ) aaa[j] = -1;

    for ( j = 0; j < DD && i < bi.biHeight; j++, i++) {
      for ( l = 0; l < bp; l++ ) aaa[n_aaa++] = getc(f);
      n_aaa += (len-bp);
    }
    QQ1(g, aaa, xx*BTS, DD, podl.BTS, podl.typ);
  }

  if (podl.typ) LzwExit();

  delete [] aaa;

  off[n_off] = off_pr;
  fseek(g, sizeof(PODL), SEEK_SET);
  fwrite(off, 4, (xx*yy+1), g);
  if (n) {
    fseek(g, sizeof(PODL)+off_pr, SEEK_SET);
    fwrite((void*) rgb, sizeof(RGBQUAD), n, g);
  }

  if (n) delete [] rgb;
  delete [] off;
}

void clip2bbb1(const char  *txt, FILE *g) {
  unsigned int i, l, bp, ll;
  long len_aaa, npcx = 0;
  int BTS;
  char  *aaa;
  long n_aaa = 0;
  PODL podl;
  BITMAPINFOHEADER *bi;
  RGBQUAD *rgb = NULL;
  int j, n;

  n_off = 0; off_pr = 0;

  bi = (BITMAPINFOHEADER *) txt;

  switch ( bi->biBitCount ) {
    case 1 : n = 2;   BTS = DD/8; bp = bi->biWidth/8;  break;
    case 4 : n = 16;  BTS = DD/2; bp = bi->biWidth/2;  break;
    case 8 : n = 256; BTS = DD;   bp = bi->biWidth;    break;
    case 24: n = 0;   BTS = DD*3; bp = bi->biWidth*3;  break;
  }

  if (n) {
    rgb = (RGBQUAD*) (txt+sizeof(BITMAPINFOHEADER));
  }

  podl.nColors = n;

  if (bi->biSizeImage) bp = bi->biSizeImage/bi->biHeight;

  podl.sign = 47316;
  podl.typ = 2;
  podl.DD = DD;
  podl.BTS = BTS;
  podl.dx = 0;
  podl.dy = 0;
  podl.x = bi->biWidth;
  podl.y = bi->biHeight;
  podl.xx = (podl.x+podl.DD-1)/podl.DD;
  podl.yy = (podl.y+podl.DD-1)/podl.DD;
  podl.BitCount = bi->biBitCount;

  podl.x0 = 40000;
  podl.y0 = 40000-podl.y;
  podl.dx0 = podl.x;
  podl.dy0 = podl.y;


  fwrite((void*) &podl, sizeof(PODL), 1, g);

  xx = podl.xx;
  yy = podl.yy;

  off = new long[xx*yy+1];
  fwrite(off, 4, xx*yy+1, g);
  off_pr = xx*yy*4+4;

  len_aaa = (long)xx*DD*BTS;
  aaa = new char [len_aaa];
  for (ll = 0; ll < len_aaa; ll++ ) aaa[ll] = -1;

  int len = xx*BTS;

  long n_txt = sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*n;

  LzwInit();

  for (i = 0; i < bi->biHeight; ) {
    n_aaa = 0;
    for (j = 0; j < len_aaa; j++ ) aaa[j] = -1;

    for ( j = 0; j < DD && i < bi->biHeight; j++, i++) {
      for ( l = 0; l < bp; l++ ) aaa[n_aaa++] = txt[n_txt++];;
      n_aaa += (len-bp);
    }
    QQ1(g, aaa, xx*BTS, DD, podl.BTS, podl.typ);
  }

  LzwExit();

  delete [] aaa;

  off[n_off] = off_pr;
  fseek(g, sizeof(PODL), SEEK_SET);
  fwrite(off, 4, xx*yy+1, g);
  fseek(g, sizeof(PODL)+off_pr, SEEK_SET);
  fwrite((void*) rgb, sizeof(RGBQUAD), n, g);

  delete [] off;
}

void clip2bbb(const char  *txt, const char *bbbN) {
  FILE *g;

  g = fopen(bbbN, "wb");
  clip2bbb1(txt, g);
  fclose(g);
}

void bmp2bbb(const char *pcxN, const char *bbbN) {
  FILE *f, *g;

  f = fopen(pcxN, "rb"); if (!f) return;
  g = fopen(bbbN, "wb");

  bmp2bbb1(f, g);

  fclose(g);
  fclose(f);
}
