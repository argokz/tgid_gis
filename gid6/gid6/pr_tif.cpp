#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bbb.h"
#include "lzw.h"

#include "geodez.h"


#if 1

#include "tiff/tiffio.h"

int const DD0 = 128;

void LzwInit();
void LzwExit();
int LzwCode(unsigned char *p, unsigned char *u, int np);
void decod(unsigned char *p, unsigned char *u, unsigned int n);
unsigned int cod(unsigned char *p, unsigned char *u, unsigned int n);

extern long *off, n_off, off_pr;
extern long xx, yy;

void QQ1(FILE * g, char *aaa, int bp, int dd, int BTS, int typ);

void QQ11(FILE* g, char* p, int n, int dd, int BTS, int typ) {
    unsigned kk;
    unsigned char* u = nullptr;

    u = new unsigned char[dd * BTS * 2];
    memset(u, 0, dd * BTS * 2);


    switch ( typ ) {
      case 0:
      case 2:
        kk = n;
        fwrite(p, kk, 1, g);
        break;
      case 1:
        kk = cod((unsigned char*)p, u, n);
        fwrite(u, kk, 1, g);
        break;
      case 22:

        kk = LzwCode((unsigned char*)p, u, n/2);
        fwrite(u, kk, 1, g);

        if (1) {
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
    delete[] u;
}
  
void tif2bbb(const char *tifN, const char *bbbN)
{
  FILE *output_file;
  long len_aaa, npcx = 0;
  int BTS;
  int i, j, l;
  char *aaa;
  long n_aaa = 0;
  PODL podl;

  TIFF *tif;
  uint32 imagelength, imagewidth;
  float resx, resy;
  uint16 resunit;
  uint16 config, bps, spp;
  tdata_t buf;


  int DD = DD0;

  /* Open the input file. */

  TIFFSetErrorHandler(0);
  TIFFSetWarningHandler(0);

  tif = TIFFOpen(tifN, "r"); if (!tif) return;

  if ((output_file = fopen(bbbN, "wb")) == NULL) return;

  double A = 0., B = 0., C = 0., D = -1., E = 0., F = 0;

  CString tifN2 = tifN;

  FILE *f2 = fopen(tifN2+"w", "r");
  if (f2) {
/*
      fscanf(f2, "%lg", &A);
      fscanf(f2, "%lg", &C);
      fscanf(f2, "%lg", &B);
      fscanf(f2, "%lg", &D);
      fscanf(f2, "%lg", &E);
      fscanf(f2, "%lg", &F);
*/

      fscanf(f2, "%lg", &A);
      fscanf(f2, "%lg", &D);
      fscanf(f2, "%lg", &B);
      fscanf(f2, "%lg", &E);
      fscanf(f2, "%lg", &C);
      fscanf(f2, "%lg", &F);
      
      
      fclose(f2);
  }
 



  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imagelength);
  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imagewidth);
  TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &config);
  TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bps);
  TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);

  TIFFGetField(tif, TIFFTAG_XRESOLUTION, &resx);
  TIFFGetField(tif, TIFFTAG_YRESOLUTION, &resy);
  TIFFGetField(tif, TIFFTAG_RESOLUTIONUNIT, &resunit); //  1 - ? 2 - inch 3 - cm
  



  if (TIFFIsTiled(tif)) {
      uint32 twidth;
      uint32 tlenght;
      uint32 tfileoffset;
      uint32 tbytecount;


      TIFFGetField(tif, TIFFTAG_TILEWIDTH, &twidth);
      TIFFGetField(tif, TIFFTAG_TILELENGTH, &tlenght);
      TIFFGetField(tif, TIFFTAG_TILEOFFSETS, &tfileoffset);
      TIFFGetField(tif, TIFFTAG_TILEBYTECOUNTS, &tbytecount);

      DD = twidth/2;
      DD = twidth;

      int spp1 = spp;
      if (spp == 3) spp1 = 4;


      if (spp1 == 4 && spp == 3) {
          DD = twidth/2;
      }

      double w_cm = imagewidth / resx;
      double h_cm = imagelength / resy;

      if (resunit == 2) {
          w_cm *= 2.54;
          h_cm *= 2.54;
      }


      podl.nColors = spp == 3 || spp == 4 ? 0 : bps == 8 ? 256 : bps == 1 ? 2 : 1;

      podl.sign = 47316 + 1;


      podl.BitCount = bps * spp1;

      BTS = DD * spp1;
      if (bps == 1) BTS = BTS / 8;
      podl.typ = 2;
      podl.DD = DD;
      podl.BTS = BTS;

      podl.x = imagewidth;
      podl.y = imagelength;
      podl.xx = (podl.x + podl.DD - 1) / podl.DD;
      podl.yy = (podl.y + podl.DD - 1) / podl.DD;
      podl.dx = 0;
      podl.dy = podl.yy * podl.DD - podl.y;

      podl.x0 = 40000;
      podl.y0 = 40000 - podl.y;

//      podl.x0 = E*100;
//      podl.y0 = F*100 - podl.y;

      double D500 = A * 3000000 / 254;

      if (spp1 == 4 && spp == 3) {
          D500 *= 2;
      }

      podl.dx0 = w_cm * D500;
      podl.dy0 = h_cm * D500;

      podl.x0 = C*100;
      podl.y0 = F*100 - podl.dy0;


      fwrite((void*) &podl, sizeof(PODL), 1, output_file);

      if (podl.typ) LzwInit();


      int buf_size = TIFFTileSize(tif);

      buf = _TIFFmalloc(buf_size);


      len_aaa = twidth * tlenght * 4;

      aaa = new char[len_aaa];

      xx = podl.xx;
      yy = podl.yy;

      n_off = 0; off_pr = 0;
      off = new long[xx*yy+1];
      fwrite(off, 4, (xx*yy+1), output_file);
      off_pr = xx*yy*4+4;

      xx = 1;

      for (int y = 0; y < podl.yy ; y++) {
          for (int x = 0; x < podl.xx; x++) {
              tmsize_t size = TIFFReadTile(tif, buf, x*twidth, y* tlenght, 0, 0);

              int n_aaa = 0;

              memset(aaa, 255, len_aaa);

              if (spp1 == 4 && spp == 3) {
                  //                for (j = 0; j < size; j += 3) {
                  for (int y1 = tlenght; y1 >= 0; y1--) {
                      for (int x1 = 1; x1 < twidth; x1 +=2) {
                          j = (y1 * twidth + x1) * 3;

                          if (j < size) {
                              ///  bgr
                              /// 
                              aaa[n_aaa++] = ((char*)buf)[j + 1];
                              aaa[n_aaa++] = ((char*)buf)[j];
                              aaa[n_aaa++] = ((char*)buf)[j + 2];
                              aaa[n_aaa++] = 255;
                          }
                      }
                  }
              }
              else {
//                for (j = 0; j < size; j++) {
//                    aaa[n_aaa++] = ((char*)buf)[j];
//                }

                  for (int y1 = tlenght; y1 >= 0; y1--) {
                      for (int x1 = 0; x1 < twidth; x1 ++) {
                          j = (y1 * twidth + x1);


                          if (j < size) {
//                              if (x1 == 0 || y1 == 0) {
//                                  aaa[n_aaa++] = 0;
//                              }
//                              else {
                                  aaa[n_aaa++] = ((char*)buf)[j];
//                              }
                          }
                      }
                  }
              }


              QQ11(output_file, aaa, n_aaa, DD, BTS, podl.typ);

          }
      }

      _TIFFfree(buf);
      TIFFClose(tif);


      if (podl.typ) LzwExit();

      delete[] aaa;

      off[n_off] = off_pr;
      fseek(output_file, sizeof(PODL), SEEK_SET);
      fwrite(off, 4, (podl.xx * podl.yy + 1), output_file);

      fseek(output_file, sizeof(PODL) + off_pr, SEEK_SET);

      if (podl.nColors) {
          RGBQUAD rgb;
          if (podl.nColors == 2 || podl.nColors == 1) {
              rgb.rgbBlue = 255;
              rgb.rgbGreen = 255;
              rgb.rgbRed = 255;
              fwrite((void*)&rgb, sizeof(RGBQUAD), 1, output_file);
              rgb.rgbBlue = 0;
              rgb.rgbGreen = 0;
              rgb.rgbRed = 0;
              fwrite((void*)&rgb, sizeof(RGBQUAD), 1, output_file);
          }
          else {
              for (i = 0; i < podl.nColors; i++) {
                  rgb.rgbBlue = i;
                  rgb.rgbGreen = i;
                  rgb.rgbRed = i;
                  fwrite((void*)&rgb, sizeof(RGBQUAD), 1, output_file);
              }
          }
      }



      fclose(output_file);


      return;
  }





  podl.nColors = spp == 3 || spp == 4 ? 0 : bps == 8 ? 256 : bps == 1 ? 2 : 1;

  podl.sign = 47316+1;

  int spp1 = spp;

  if (spp == 3) spp1 = 4;

  podl.BitCount = bps*spp1;

  BTS = DD*spp1;
  if (bps == 1) BTS = BTS/8;
  podl.typ = 2;
  podl.DD = DD;
  podl.BTS = BTS;

  podl.x = imagewidth;
  podl.y = imagelength;
  podl.xx = (podl.x+podl.DD-1)/podl.DD;
  podl.yy = (podl.y+podl.DD-1)/podl.DD;
  podl.dx = 0;
  podl.dy = podl.yy*podl.DD-podl.y;

  podl.x0 = 40000;
  podl.y0 = 40000-podl.y;
  podl.dx0 = podl.x;
  podl.dy0 = podl.y;







  fwrite((void*) &podl, sizeof(PODL), 1, output_file);

  xx = podl.xx;
  yy = podl.yy;

  n_off = 0; off_pr = 0;
  off = new long[xx*yy+1];
  fwrite(off, 4, (xx*yy+1), output_file);
  off_pr = xx*yy*4+4;

  len_aaa = xx*DD*BTS;

  aaa = new char [len_aaa];

  int len = xx*BTS;

  int bp = TIFFScanlineSize(tif);

  if (podl.typ) LzwInit();

  buf = _TIFFmalloc(bp);

  for (l = 0; l < imagelength;) {
    n_aaa = 0;
    memset(aaa, 0, len_aaa);

    for (i = 0; i < DD && l < imagelength; i++, l++) {

        if (TIFFReadScanline(tif, buf, l, 0)) {

            n_aaa = (DD - i - 1) * len;

            if (spp1 == 3) {
                for (j = 0; j < bp; j += 3) {
                    aaa[n_aaa++] = ((char*)buf)[j + 2];
                    aaa[n_aaa++] = ((char*)buf)[j + 1];
                    aaa[n_aaa++] = ((char*)buf)[j];
                }
            }

            else if (spp1 == 4 && spp == 3) {
                for (j = 0; j < bp; j += 3) {
                    aaa[n_aaa++] = ((char*)buf)[j + 2];
                    aaa[n_aaa++] = ((char*)buf)[j + 1];
                    aaa[n_aaa++] = ((char*)buf)[j];
                    aaa[n_aaa++] = 255;
                }
            }

            else {
                for (j = 0; j < bp; j++) {
                    aaa[n_aaa++] = ((char*)buf)[j];
                }

                int qq;
                qq = 1;
            }
        }
    }

    QQ1(output_file, aaa, xx*BTS, DD, BTS, podl.typ);
  }

  if (podl.typ) LzwExit();

  delete [] aaa;
  
  off[n_off] = off_pr;
  fseek(output_file, sizeof(PODL), SEEK_SET);
  fwrite(off, 4, (xx*yy+1), output_file);

  fseek(output_file, sizeof(PODL)+off_pr, SEEK_SET);

  if (podl.nColors) {
    RGBQUAD rgb;
    if (podl.nColors == 2 || podl.nColors == 1) {
      rgb.rgbBlue  = 255;
      rgb.rgbGreen = 255;
      rgb.rgbRed   = 255;
      fwrite((void*) &rgb, sizeof(RGBQUAD), 1, output_file);
      rgb.rgbBlue  = 0;
      rgb.rgbGreen = 0;
      rgb.rgbRed   = 0;
      fwrite((void*) &rgb, sizeof(RGBQUAD), 1, output_file);
    }
    else {
      for (i = 0; i < podl.nColors; i++) {
        rgb.rgbBlue  = i;
        rgb.rgbGreen = i;
        rgb.rgbRed   = i;
        fwrite((void*) &rgb, sizeof(RGBQUAD), 1, output_file);
      }
    }
  }

  _TIFFfree(buf);
  TIFFClose(tif);

  fclose(output_file);
}

#endif
