#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bbb.h"


#include "rdgif.h"

int const DD = 128;

void LzwInit();
void LzwExit();
int LzwCode(unsigned char *p, unsigned char *u, int np);
void decod(unsigned char *p, unsigned char *u, unsigned int n);
unsigned int cod(unsigned char *p, unsigned char *u, unsigned int n);

extern long *off, n_off, off_pr;
extern long xx, yy;

void QQ1(FILE * g, char *aaa, int bp, int dd, int BTS, int typ);

gif_source_struct * jinit_read_gif();
void start_input_gif (gif_source_struct * sinfo);
void end_input_gif (gif_source_struct * sinfo);
  
void gif2bbb(const char *gifN, const char *bbbN)
{
  FILE *input_file;
  FILE *output_file;
  
  long len_aaa, npcx = 0;
  int BTS;
  int i, j, l;
  char *aaa;
  long n_aaa = 0;
  PODL podl;

  /* Open the input file. */
  if ((input_file = fopen(gifN, "rb")) == NULL) return;
  if ((output_file = fopen(bbbN, "wb")) == NULL) return;

  /* Figure out the input file format, and set up to read it. */

  gif_source_struct *  src_mgr;

  src_mgr = (gif_source_struct *) malloc (sizeof(gif_source_struct));

  src_mgr->input_file = input_file;

  /* Read the input file header to obtain file size & colorspace. */
  start_input_gif(src_mgr);

  podl.nColors = src_mgr->colormaplen;

  podl.sign = 47316+1;
  podl.BitCount = src_mgr->colormaplen == 256 ? 8 : 1;

  BTS = DD*podl.BitCount/8;
  podl.typ = 2;
  podl.DD = DD;
  podl.BTS = BTS;
  podl.x = src_mgr->image_width;
  podl.y = src_mgr->image_height;
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

  int bp = (podl.x+8/podl.BitCount-1)/(8/podl.BitCount);

  if (podl.typ) LzwInit();

  for (l = 0; l < src_mgr->image_height;) {
    n_aaa = 0;
    memset(aaa, 0, len_aaa);

    for (i = 0; i < DD && l < src_mgr->image_height; i++, l++) {

      (*src_mgr->get_pixel_rows) (src_mgr);

      n_aaa = (DD-i-1)*len;

      for (j = 0; j < bp; j ++ ) {
        aaa[n_aaa++] = src_mgr->buffer[j];
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

  RGBQUAD rgb;
  for (i = 0; i < src_mgr->colormaplen; i++) {
    rgb.rgbBlue  = src_mgr->colormap[i].rgb[CM_BLUE];
    rgb.rgbGreen = src_mgr->colormap[i].rgb[CM_GREEN];
    rgb.rgbRed   = src_mgr->colormap[i].rgb[CM_RED];
    fwrite((void*) &rgb, sizeof(RGBQUAD), 1, output_file);
  }
  end_input_gif(src_mgr);

  fclose(input_file);
  fclose(output_file);
}
