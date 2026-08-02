#include <stdio.h>
#include <string.h>
#include <setjmp.h>


extern "C" {

#include "jpeglib.h"

}

struct my_error_mgr {
  struct jpeg_error_mgr pub;  /* "public" fields */

  jmp_buf setjmp_buffer;  /* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;


/*
 * Here's the routine that will replace the standard error_exit method:
 */

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

int jpeg_header(const char *filename, int *width, int *height)
{
  struct jpeg_decompress_struct cinfo;
  struct my_error_mgr jerr;


  /* More stuff */
  FILE * infile;    /* source file */
  JSAMPARRAY buffer;    /* Output row buffer */
  int row_stride;   /* physical row width in output buffer */

  if ((infile = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return 0;
  }
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  /* Establish the setjmp return context for my_error_exit to use. */
  if (setjmp(jerr.setjmp_buffer)) {
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return 0;
  }

  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, infile);

   (void) jpeg_read_header(&cinfo, TRUE);

  *width = cinfo.image_width;
  *height = cinfo.image_height;

  
   jpeg_destroy_decompress(&cinfo);

  fclose(infile);
  return 1;
}


int jpeg_decode(const char *filename, char *buf)
{
  /* This struct contains the JPEG decompression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   */
  struct jpeg_decompress_struct cinfo;
  /* We use our private extension JPEG error handler.
   * Note that this struct must live as long as the main JPEG parameter
   * struct, to avoid dangling-pointer problems.
   */
  struct my_error_mgr jerr;


  /* More stuff */
  FILE * infile;    /* source file */
  JSAMPARRAY buffer;    /* Output row buffer */
  int row_stride;   /* physical row width in output buffer */

  /* In this example we want to open the input file before doing anything else,
   * so that the setjmp() error recovery below can assume the file is open.
   * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
   * requires it in order to read binary files.
   */

  if ((infile = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return 0;
  }

  /* Step 1: allocate and initialize JPEG decompression object */

  /* We set up the normal JPEG error routines, then override error_exit. */

  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  /* Establish the setjmp return context for my_error_exit to use. */
  if (setjmp(jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return 0;
  }

  /* Now we can initialize the JPEG decompression object. */
  jpeg_create_decompress(&cinfo);

  /* Step 2: specify data source (eg, a file) */

  jpeg_stdio_src(&cinfo, infile);

  /* Step 3: read file parameters with jpeg_read_header() */

   (void) jpeg_read_header(&cinfo, TRUE);
  /* We can ignore the return value from jpeg_read_header since
   *   (a) suspension is not possible with the stdio data source, and
   *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
   * See libjpeg.doc for more info.
   */

  /* Step 4: set parameters for decompression */

  /* In this example, we don't need to change any of the defaults set by
   * jpeg_read_header(), so we do nothing here.
   */

  /* Step 5: Start decompressor */

   (void) jpeg_start_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* We may need to do some setup of our own at this point before reading
   * the data.  After jpeg_start_decompress() we have the correct scaled
   * output image dimensions available, as well as the output colormap
   * if we asked for color quantization.
   * In this example, we need to make an output work buffer of the right size.
   */ 
  /* JSAMPLEs per row in output buffer */
  row_stride = cinfo.output_width * cinfo.output_components;
  /* Make a one-row-high sample array that will go away when done with image */
  buffer = (*cinfo.mem->alloc_sarray)
    ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  /* Step 6: while (scan lines remain to be read) */
  /*           jpeg_read_scanlines(...); */

  /* Here we use the library's state variable cinfo.output_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   */
  while (cinfo.output_scanline < cinfo.output_height) {
    /* jpeg_read_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could ask for
     * more than one scanline at a time if that's more convenient.
     */
     (void) jpeg_read_scanlines(&cinfo, buffer, 1);
    /* Assume put_scanline_someplace wants a pointer and sample count. */
//    put_scanline_someplace(buffer[0], row_stride);
    for (int i = 0; i < row_stride; i += 3) {
      buf[i] = buffer[0][i+2];
      buf[i+1] = buffer[0][i+1];
      buf[i+2] = buffer[0][i];
    }
//    memmove(buf, buffer[0], row_stride);
    buf += row_stride;
  }

  /* Step 7: Finish decompression */

   (void) jpeg_finish_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* Step 8: Release JPEG decompression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_decompress(&cinfo);

  /* After finish_decompress, we can close the input file.
   * Here we postpone it until after no more JPEG errors are possible,
   * so as to simplify the setjmp error logic above.  (Actually, I don't
   * think that jpeg_destroy can do an error exit, but why assume anything...)
   */
  fclose(infile);

  /* At this point you may want to check to see whether any corrupt-data
   * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
   */

  /* And we're done! */

  return 1;
}


int rgb_l_c(int color, int tr, int bk_color)
{
  int r, g, b;
  int rb, gb, bb;

//  int tr = m_nTransp;

  if (0 < tr && tr < 255) 
  {
    r = (color&0xFF0000) >> 16;
    g = (color&0x00FF00) >> 8;
    b = (color&0x0000FF);

    rb = (bk_color&0xFF0000) >> 16;
    gb = (bk_color&0x00FF00) >> 8;
    bb = (bk_color&0x0000FF);

    r = (rb*(255-tr)+r*tr)/255;
    g = (gb*(255-tr)+g*tr)/255;
    b = (bb*(255-tr)+b*tr)/255;

    color = r<<16|g<<8|b;
  }

  return color;
}


int jpeg_decode_tr(const char *filename, char *buf, int tr, int m_bk_color)
{
  struct jpeg_decompress_struct cinfo;
  struct my_error_mgr jerr;


  /* More stuff */
  FILE * infile;    /* source file */
  JSAMPARRAY buffer;    /* Output row buffer */
  int row_stride;   /* physical row width in output buffer */

  if ((infile = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return 0;
  }


  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  /* Establish the setjmp return context for my_error_exit to use. */
  if (setjmp(jerr.setjmp_buffer)) {
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return 0;
  }

  /* Now we can initialize the JPEG decompression object. */
  jpeg_create_decompress(&cinfo);

  /* Step 2: specify data source (eg, a file) */

  jpeg_stdio_src(&cinfo, infile);

  /* Step 3: read file parameters with jpeg_read_header() */

   (void) jpeg_read_header(&cinfo, TRUE);
  /* Step 5: Start decompressor */

   (void) jpeg_start_decompress(&cinfo);
  row_stride = cinfo.output_width * cinfo.output_components;
  /* Make a one-row-high sample array that will go away when done with image */
  buffer = (*cinfo.mem->alloc_sarray)
    ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  while (cinfo.output_scanline < cinfo.output_height) {
     (void) jpeg_read_scanlines(&cinfo, buffer, 1);
    for (int i = 0; i < row_stride; i += 3) {

      unsigned char im1[4];
    
      im1[0] = buffer[0][i+2];
      im1[1] = buffer[0][i+1];
      im1[2] = buffer[0][i];
      
      long *ii = (long*) &im1;
      ii[0] = rgb_l_c(ii[0], tr, m_bk_color);

      buf[i]   = im1[0];
      buf[i+1] = im1[1];
      buf[i+2] = im1[2];


//      buf[i] = buffer[0][i+2];
//      buf[i+1] = buffer[0][i+1];
//      buf[i+2] = buffer[0][i];
    }
//    memmove(buf, buffer[0], row_stride);
    buf += row_stride;
  }

  /* Step 7: Finish decompression */

   (void) jpeg_finish_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* Step 8: Release JPEG decompression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_decompress(&cinfo);

  /* After finish_decompress, we can close the input file.
   * Here we postpone it until after no more JPEG errors are possible,
   * so as to simplify the setjmp error logic above.  (Actually, I don't
   * think that jpeg_destroy can do an error exit, but why assume anything...)
   */
  fclose(infile);

  /* At this point you may want to check to see whether any corrupt-data
   * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
   */

  /* And we're done! */

  return 1;
}



int jpeg_decode_tr_obr(const char *filename, char *buf, int tr, int m_bk_color)
{
  struct jpeg_decompress_struct cinfo;
  struct my_error_mgr jerr;


  /* More stuff */
  FILE * infile;    /* source file */
  JSAMPARRAY buffer;    /* Output row buffer */
  int row_stride;   /* physical row width in output buffer */

  if ((infile = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return 0;
  }


  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  /* Establish the setjmp return context for my_error_exit to use. */
  if (setjmp(jerr.setjmp_buffer)) {
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return 0;
  }

  /* Now we can initialize the JPEG decompression object. */
  jpeg_create_decompress(&cinfo);

  /* Step 2: specify data source (eg, a file) */

  jpeg_stdio_src(&cinfo, infile);

  /* Step 3: read file parameters with jpeg_read_header() */

   (void) jpeg_read_header(&cinfo, TRUE);
  /* Step 5: Start decompressor */

   (void) jpeg_start_decompress(&cinfo);
  row_stride = cinfo.output_width * cinfo.output_components;
  /* Make a one-row-high sample array that will go away when done with image */
  buffer = (*cinfo.mem->alloc_sarray)
    ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  while (cinfo.output_scanline < cinfo.output_height) {
    (void) jpeg_read_scanlines(&cinfo, buffer, 1);

    for (int i = 0; i < row_stride; i += 3) {

      unsigned char im1[4];
    
      im1[0] = buffer[0][i+2];
      im1[1] = buffer[0][i+1];
      im1[2] = buffer[0][i];
      
      long *ii = (long*) &im1;
      ii[0] = rgb_l_c(ii[0], tr, m_bk_color);

      buf[i]   = im1[0];
      buf[i+1] = im1[1];
      buf[i+2] = im1[2];
    }
    buf += row_stride;
  }

  /* Step 7: Finish decompression */

   (void) jpeg_finish_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* Step 8: Release JPEG decompression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_decompress(&cinfo);

  /* After finish_decompress, we can close the input file.
   * Here we postpone it until after no more JPEG errors are possible,
   * so as to simplify the setjmp error logic above.  (Actually, I don't
   * think that jpeg_destroy can do an error exit, but why assume anything...)
   */
  fclose(infile);

  /* At this point you may want to check to see whether any corrupt-data
   * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
   */

  /* And we're done! */

  return 1;
}




int jpeg_decode8(const char *filename, char *buf)
{
  struct jpeg_decompress_struct cinfo;
  struct my_error_mgr jerr;


  /* More stuff */
  FILE * infile;    /* source file */
  JSAMPARRAY buffer;    /* Output row buffer */
  int row_stride;   /* physical row width in output buffer */

  if ((infile = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return 0;
  }

  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  /* Establish the setjmp return context for my_error_exit to use. */
  if (setjmp(jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return 0;
  }

  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, infile);
   (void) jpeg_read_header(&cinfo, TRUE);
   (void) jpeg_start_decompress(&cinfo);
  row_stride = cinfo.output_width * cinfo.output_components;
  buffer = (*cinfo.mem->alloc_sarray)
    ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);


  int row_stride8 = (row_stride+23)/24*24;


  for (int i = 0; i < row_stride8*cinfo.output_height; i++) {
    buf[i] = 0xFF;
  }


  while (cinfo.output_scanline < cinfo.output_height) {
     (void) jpeg_read_scanlines(&cinfo, buffer, 1);
    for (int i = 0; i < row_stride; i += 3) {
      buf[i] = buffer[0][i+2];
      buf[i+1] = buffer[0][i+1];
      buf[i+2] = buffer[0][i];
    }
    buf += row_stride8;
  }

   (void) jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  fclose(infile);
  return 1;
}
