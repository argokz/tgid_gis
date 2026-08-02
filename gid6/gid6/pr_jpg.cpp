#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bbb.h"

#if 1

#ifdef  __cplusplus
extern "C" {
#endif

#include "jpeg-6a/jpeglib.h"

#ifdef  __cplusplus
}
#endif


int const DD = 128;

void LzwInit();
void LzwExit();
int LzwCode(unsigned char *p, unsigned char *u, int np);
void decod(unsigned char *p, unsigned char *u, unsigned int n);
unsigned int cod(unsigned char *p, unsigned char *u, unsigned int n);

extern long *off, n_off, off_pr;
extern long xx, yy;

void QQ1(FILE *g, char *aaa, int bp, int dd, int BTS, int typ);


#include <setjmp.h>

struct my_error_mgr {
  struct jpeg_error_mgr pub;  /* "public" fields */

  jmp_buf setjmp_buffer;  /* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

METHODDEF (void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}


void jpg2bbb1(FILE *f, FILE *g) {
  unsigned int l, bp;
  long len_aaa, npcx = 0;
  int BTS;
  char *aaa;
  long n_aaa = 0, ll;
  PODL podl;
  int i;

  struct jpeg_decompress_struct cinfo;


  struct my_error_mgr jerr;
  JSAMPARRAY buffer;    /* Output row buffer */
  FILE *infile;

  infile = f;

  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;

  if (setjmp(jerr.setjmp_buffer)) {
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return;
  }

  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, infile);
  (void) jpeg_read_header(&cinfo, TRUE);

  (void) jpeg_start_decompress(&cinfo);

  podl.nColors = cinfo.output_components == 3 ? 0 : 256;

  BTS = DD*cinfo.output_components;
  podl.sign = 47316+1;
  podl.typ = 2;
  podl.DD = DD;
  podl.BTS = BTS;
  podl.x = cinfo.output_width;
  podl.y = cinfo.output_height;
  podl.xx = (podl.x+podl.DD-1)/podl.DD;
  podl.yy = (podl.y+podl.DD-1)/podl.DD;
//  podl.dx = podl.xx*podl.DD-cinfo.output_width;
  podl.dx = 0;
  podl.dy = podl.yy*podl.DD-podl.y;

  podl.x0 = 40000;
  podl.y0 = 40000-podl.y;
  podl.dx0 = podl.x;
  podl.dy0 = podl.y;

//  1 12413.46 13096.71
//  2 11025.29 8606.67
//  Длина 4699.73  4698.22
//
 

// 4888 4674
//  double mm = 0.0846666666666667*96/128*1.05*4698.22/4699.73;//*4807.82/4699.37;
//  double mm = 0.0846666666666667*96/128*1.05;
//  double mm = 1./15;
  double mm = 1. /15;

//  mm = 1./11.2;

//  podl.x0 = 14174.0*100;
//  podl.y0 = 15119.0*100;
//  podl.dx0 = 40000/mm;
//  podl.dy0 = 40000/mm;

//  double xx0 = 14174.0;    //945
//  double yy0 = 15119.0;

//  podl.x0 = xx0*100-40000/mm;
//  podl.y0 = yy0*100-40000/mm;

//  podl.x0 = xx0*100-40000/mm-20000+2000-400-5000-500;
//  podl.y0 = yy0*100-40000/mm-150000-500;

  podl.x0 = 793800;     //318
  podl.y0 = 762000;


//  podl.x0 = 1417400-600000-20000+2000-400-5000-500+304;
//  podl.y0 = 1511900-600000-150000-500+603;
  
  podl.dx0 = 40000/mm;
  podl.dy0 = 40000/mm;


  podl.BitCount = cinfo.output_components*8;

  fwrite((void*) &podl, sizeof(PODL), 1, g);

  xx = podl.xx;
  yy = podl.yy;


  n_off = 0; off_pr = 0;
  off = new long[xx*yy+1];
  fwrite(off, 4, (xx*yy+1), g);
  off_pr = xx*yy*4+4;

  len_aaa = (long)xx*DD*BTS;
  aaa = new char [len_aaa];
  for (ll = 0; ll < len_aaa; ll++ ) {
    aaa[ll] = -1;
  }

  int len = xx*BTS;

  int row_stride = cinfo.output_width * cinfo.output_components;

  if (podl.typ) LzwInit();


  bp = (row_stride+3)/4*4;

  buffer = (*cinfo.mem->alloc_sarray)
    ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  while (cinfo.output_scanline < cinfo.output_height) {
    n_aaa = 0;
    for (l = 0; l < len_aaa; l++ ) {
      aaa[l] = -1;
    }

    for (i = 0; i < DD &&cinfo.output_scanline < cinfo.output_height; i++) {
      (void) jpeg_read_scanlines(&cinfo, buffer, 1);

      n_aaa = (DD-i-1)*len;

      if (cinfo.output_components == 3) {
        for (l = 0; l < row_stride; l += 3 ) {
          aaa[n_aaa++] = buffer[0][l+2];
          aaa[n_aaa++] = buffer[0][l+1];
          aaa[n_aaa++] = buffer[0][l];
        }
      }
      else {
        for (l = 0; l < row_stride; l ++ ) {
          aaa[n_aaa++] = buffer[0][l];
        }
      }
//      n_aaa += (len-row_stride);
    }

    QQ1(g, aaa, xx*BTS, DD, podl.BTS, podl.typ);
  }

  if (podl.typ) LzwExit();

  delete [] aaa;

  off[n_off] = off_pr;
  fseek(g, sizeof(PODL), SEEK_SET);
  fwrite(off, 4, (xx*yy+1), g);

  RGBQUAD rgb;

  if (cinfo.output_components == 1) {
    fseek(g, sizeof(PODL)+off_pr, SEEK_SET);
    for (i = 0; i < 256; i++) {
      rgb.rgbBlue  = i;
      rgb.rgbGreen = i;
      rgb.rgbRed   = i;
      fwrite((void*) &rgb, sizeof(RGBQUAD), 1, g);
    }
  }

  delete [] off;



  (void) jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
 
  fclose(infile);

}

#endif

void jpg2bbb(const char *pcxN, const char *bbbN) {
  FILE *f, *g;

  f = fopen(pcxN, "rb"); if (!f) return;
  g = fopen(bbbN, "wb");

  jpg2bbb1(f, g);

  fclose(g);
  fclose(f);
}

