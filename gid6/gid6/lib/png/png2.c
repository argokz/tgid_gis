
/* png.c - location for general purpose libpng functions
 *
 * libpng 1.0.2 - June 14, 1998
 * For conditions of distribution and use, see copyright notice in png.h
 * Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.
 * Copyright (c) 1996, 1997 Andreas Dilger
 * Copyright (c) 1998, Glenn Randers-Pehrson
 */

#define PNG_INTERNAL
#define PNG_NO_EXTERN
#include "png.h"

int png_decode8(const char *inname, char *buf)
{
   static FILE *fpin;  /* "static" prevents setjmp corruption */
   png_structp read_ptr;
   png_infop read_info_ptr, end_info_ptr;
   png_uint_32 y;
   png_uint_32 width, height;
   int bit_depth, color_type;
   int num_pass;

   if ((fpin = fopen(inname, "rb")) == NULL)
   {
//      fprintf(STDERR, "Could not find input file %s\n", inname);
      return (1);
   }

   read_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL,
      (png_error_ptr)NULL, (png_error_ptr)NULL);

   read_info_ptr = png_create_info_struct(read_ptr);
   end_info_ptr = png_create_info_struct(read_ptr);

   png_debug(0, "Setting jmpbuf for read struct\n");
   if (setjmp(read_ptr->jmpbuf))
   {
      png_destroy_read_struct(&read_ptr, &read_info_ptr, &end_info_ptr);
      fclose(fpin);
      return (1);
   }

   png_init_io(read_ptr, fpin);

   png_set_read_status_fn(read_ptr, NULL);


   png_read_info(read_ptr, read_info_ptr);

   png_debug(0, "Transferring info struct\n");
   {
      int interlace_type, compression_type, filter_type;

      if (png_get_IHDR(read_ptr, read_info_ptr, &width, &height, &bit_depth,
          &color_type, &interlace_type, &compression_type, &filter_type))
      {
      }
   }
   {
      png_colorp palette;
      int num_palette;

      if (png_get_PLTE(read_ptr, read_info_ptr, &palette, &num_palette))
      {
      }
   }


   num_pass = png_set_interlace_handling(read_ptr);

   if (num_pass == 1) 
   {
      char *row_buf = buf;
      
      for (y = 0; y < height; y++)
      {
        unsigned int k;
        png_read_rows(read_ptr, (png_bytepp)&row_buf, (png_bytepp)NULL, 1);

        for (k = 0; k < read_info_ptr->width; k++) {
           unsigned char c = row_buf[k*3];
           row_buf[k*3] = row_buf[k*3+2];
           row_buf[k*3+2] = c;
        }

        row_buf += read_info_ptr->width*3;
      }
   }

   png_read_end(read_ptr, end_info_ptr);
   png_destroy_read_struct(&read_ptr, &read_info_ptr, &end_info_ptr);

   fclose(fpin);

   return 0;
}




int png_header(const char *inname, int *nWidth, int *nHeight)
{
   static FILE *fpin;  /* "static" prevents setjmp corruption */
   png_structp read_ptr;
   png_infop read_info_ptr, end_info_ptr;
   png_bytep row_buf;
   png_uint_32 width, height;
   int bit_depth, color_type;

   row_buf = (png_bytep)NULL;

   if ((fpin = fopen(inname, "rb")) == NULL)
   {
      return 1;
   }

   read_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL,
      (png_error_ptr)NULL, (png_error_ptr)NULL);

   read_info_ptr = png_create_info_struct(read_ptr);
   end_info_ptr = png_create_info_struct(read_ptr);

   png_debug(0, "Setting jmpbuf for read struct\n");
   if (setjmp(read_ptr->jmpbuf))
   {
      png_destroy_read_struct(&read_ptr, &read_info_ptr, &end_info_ptr);
      fclose(fpin);
      return 1;
   }

   png_init_io(read_ptr, fpin);

   png_set_read_status_fn(read_ptr, NULL);


   png_read_info(read_ptr, read_info_ptr);

   png_debug(0, "Transferring info struct\n");
   {
      int interlace_type, compression_type, filter_type;

      if (png_get_IHDR(read_ptr, read_info_ptr, &width, &height, &bit_depth,
          &color_type, &interlace_type, &compression_type, &filter_type))
      {
      }
   }
   {
      png_colorp palette;
      int num_palette;

      if (png_get_PLTE(read_ptr, read_info_ptr, &palette, &num_palette))
      {
      }
   }

/*
   png_debug(0, "\nAllocating row buffer \n");
   row_buf = (png_bytep)png_malloc(read_ptr, 
      png_get_rowbytes(read_ptr, read_info_ptr));
   if (row_buf == NULL)
   {
      png_destroy_read_struct(&read_ptr, &read_info_ptr, (png_infopp)NULL);
      fclose(fpin);
      return (1);
   }

   num_pass = png_set_interlace_handling(read_ptr);

   for (pass = 0; pass < num_pass; pass++)
   {
      png_debug1(0, "Writing row data for pass %d\n",pass);
      for (y = 0; y < height; y++)
      {
         png_read_rows(read_ptr, (png_bytepp)&row_buf, (png_bytepp)NULL, 1);
      }
   }

*/

   *nWidth = read_info_ptr->width;
   *nHeight = read_info_ptr->height;

   png_read_end(read_ptr, end_info_ptr);

 
   png_free(read_ptr, row_buf);
   png_destroy_read_struct(&read_ptr, &read_info_ptr, &end_info_ptr);

   fclose(fpin);

   return 0;
}
