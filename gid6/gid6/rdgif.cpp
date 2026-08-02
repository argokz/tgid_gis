/*
 * rdgif.c
 *
 * Copyright (C) 1991-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 **************************************************************************
 * WARNING: You will need an LZW patent license from Unisys in order to   *
 * use this file legally in any commercial or shareware application.      *
 **************************************************************************
 *
 * This file contains routines to read input images in GIF format.
 *
 * These routines may need modification for non-Unix environments or
 * specialized applications.  As they stand, they assume input from
 * an ordinary stdio stream.  They further assume that reading begins
 * at the start of the file; input_init may need work if the
 * user interface has already read some data (e.g., to determine that
 * the file is indeed GIF format).
 */

/*
 * This code is loosely based on giftoppm from the PBMPLUS distribution
 * of Feb. 1991.  That file contains the following copyright notice:
 * +-------------------------------------------------------------------+
 * | Copyright 1990, David Koblas.                                     |
 * |   Permission to use, copy, modify, and distribute this software   |
 * |   and its documentation for any purpose and without fee is hereby |
 * |   granted, provided that the above copyright notice appear in all |
 * |   copies and that both that copyright notice and this permission  |
 * |   notice appear in supporting documentation.  This software is    |
 * |   provided "as is" without express or implied warranty.           |
 * +-------------------------------------------------------------------+
 *
 * We are also required to state that
 *    "The Graphics Interchange Format(c) is the Copyright property of
 *    CompuServe Incorporated. GIF(sm) is a Service Mark property of
 *    CompuServe Incorporated."
 */

#include "stdafx.h"

#include "rdgif.h"


#define MAXCOLORMAPSIZE 256 /* max # of colors in a GIF colormap */

#define MAX_LZW_BITS  12  /* maximum LZW code size */
#define LZW_TABLE_SIZE  (1<<MAX_LZW_BITS) /* # of possible LZW symbols */

/* Macros for extracting header data --- note we assume chars may be signed */

#define LM_to_uint(a,b)   ((((b)&0xFF) << 8) | ((a)&0xFF))

#define BitSet(byte, bit) ((byte) & (bit))
#define INTERLACE 0x40  /* mask for bit signifying interlaced image */
#define COLORMAPFLAG  0x80  /* mask for bit signifying colormap presence */

#define ReadOK(file,buffer,len) ((size_t) fread((void *) (buffer), (size_t) 1, (size_t) (len), (file)) == ((size_t) (len)))

//(JFREAD(file,buffer,len) == ((size_t) (len)))

/* LZW decompression tables look like this:
 *   symbol_head[K] = prefix symbol of any LZW symbol K (0..LZW_TABLE_SIZE-1)
 *   symbol_tail[K] = suffix byte   of any LZW symbol K (0..LZW_TABLE_SIZE-1)
 * Note that entries 0..end_code of the above tables are not used,
 * since those symbols represent raw bytes or special codes.
 *
 * The stack represents the not-yet-used expansion of the last LZW symbol.
 * In the worst case, a symbol could expand to as many bytes as there are
 * LZW symbols, so we allocate LZW_TABLE_SIZE bytes for the stack.
 * (This is conservative since that number includes the raw-byte symbols.)
 *
 * The tables are allocated from FAR heap space since they would use up
 * rather a lot of the near data space in a PC.
 */



int ReadByte (gif_source_struct * sinfo)
/* Read next byte from GIF file */
{
  register FILE * infile = sinfo->input_file;
  int c;

  if ((c = getc(infile)) == EOF)
    throw JERR_INPUT_EOF;
  return c;
}


int GetDataBlock (gif_source_struct * sinfo, char *buf)
/* Read a GIF data block, which has a leading count byte */
/* A zero-length block marks the end of a data block sequence */
{
  int count;

  count = ReadByte(sinfo);
  if (count > 0) {
    if (! ReadOK(sinfo->input_file, buf, count))
      throw JERR_INPUT_EOF;
  }
  return count;
}


void SkipDataBlocks (gif_source_struct * sinfo)
/* Skip a series of data blocks, until a block terminator is found */
{
  char buf[256];

  while (GetDataBlock(sinfo, buf) > 0)
    /* skip */;
}


void ReInitLZW (gif_source_struct * sinfo)
/* (Re)initialize LZW state; shared code for startup and Clear processing */
{
  sinfo->code_size = sinfo->input_code_size + 1;
  sinfo->limit_code = sinfo->clear_code << 1; /* 2^code_size */
  sinfo->max_code = sinfo->clear_code + 2;  /* first unused code value */
  sinfo->sp = sinfo->symbol_stack;    /* init stack to empty */
}


void InitLZWCode (gif_source_struct * sinfo)
/* Initialize for a series of LZWReadByte (and hence GetCode) calls */
{
  /* GetCode initialization */
  sinfo->last_byte = 2;   /* make safe to "recopy last two bytes" */
  sinfo->last_bit = 0;    /* nothing in the buffer */
  sinfo->cur_bit = 0;   /* force buffer load on first call */
  sinfo->out_of_blocks = FALSE;

  /* LZWReadByte initialization: */
  /* compute special code values (note that these do not change later) */
  sinfo->clear_code = 1 << sinfo->input_code_size;
  sinfo->end_code = sinfo->clear_code + 1;
  sinfo->first_time = TRUE;
  ReInitLZW(sinfo);
}


int GetCode (gif_source_struct * sinfo)
/* Fetch the next code_size bits from the GIF data */
/* We assume code_size is less than 16 */
{
  register INT32 accum;
  int offs, ret, count;

  while ( (sinfo->cur_bit + sinfo->code_size) > sinfo->last_bit) {
    /* Time to reload the buffer */
    if (sinfo->out_of_blocks) {
//      WARNMS(sinfo->cinfo, JWRN_GIF_NOMOREDATA);
      return sinfo->end_code; /* fake something useful */
    }
    /* preserve last two bytes of what we have -- assume code_size <= 16 */
    sinfo->code_buf[0] = sinfo->code_buf[sinfo->last_byte-2];
    sinfo->code_buf[1] = sinfo->code_buf[sinfo->last_byte-1];
    /* Load more bytes; set flag if we reach the terminator block */
    if ((count = GetDataBlock(sinfo, &sinfo->code_buf[2])) == 0) {
      sinfo->out_of_blocks = TRUE;
//      WARNMS(sinfo->cinfo, JWRN_GIF_NOMOREDATA);
      return sinfo->end_code; /* fake something useful */
    }
    /* Reset counters */
    sinfo->cur_bit = (sinfo->cur_bit - sinfo->last_bit) + 16;
    sinfo->last_byte = 2 + count;
    sinfo->last_bit = sinfo->last_byte * 8;
  }

  /* Form up next 24 bits in accum */
  offs = sinfo->cur_bit >> 3; /* byte containing cur_bit */
#ifdef CHAR_IS_UNSIGNED
  accum = sinfo->code_buf[offs+2];
  accum <<= 8;
  accum |= sinfo->code_buf[offs+1];
  accum <<= 8;
  accum |= sinfo->code_buf[offs];
#else
  accum = sinfo->code_buf[offs+2] & 0xFF;
  accum <<= 8;
  accum |= sinfo->code_buf[offs+1] & 0xFF;
  accum <<= 8;
  accum |= sinfo->code_buf[offs] & 0xFF;
#endif

  /* Right-align cur_bit in accum, then mask off desired number of bits */
  accum >>= (sinfo->cur_bit & 7);
  ret = ((int) accum) & ((1 << sinfo->code_size) - 1);

  sinfo->cur_bit += sinfo->code_size;
  return ret;
}


int LZWReadByte (gif_source_struct * sinfo)
/* Read an LZW-compressed byte */
{
  register int code;    /* current working code */
  int incode;     /* saves actual input code */

  /* First time, just eat the expected Clear code(s) and return next code, */
  /* which is expected to be a raw byte. */
  if (sinfo->first_time) {
    sinfo->first_time = FALSE;
    code = sinfo->clear_code; /* enables sharing code with Clear case */
  } else {

    /* If any codes are stacked from a previously read symbol, return them */
    if (sinfo->sp > sinfo->symbol_stack)
      return (int) *(-- sinfo->sp);

    /* Time to read a new symbol */
    code = GetCode(sinfo);

  }

  if (code == sinfo->clear_code) {
    /* Reinit state, swallow any extra Clear codes, and */
    /* return next code, which is expected to be a raw byte. */
    ReInitLZW(sinfo);
    do {
      code = GetCode(sinfo);
    } while (code == sinfo->clear_code);
    if (code > sinfo->clear_code) { /* make sure it is a raw byte */
//      WARNMS(sinfo->cinfo, JWRN_GIF_BADDATA);
      code = 0;     /* use something valid */
    }
    /* make firstcode, oldcode valid! */
    sinfo->firstcode = sinfo->oldcode = code;
    return code;
  }

  if (code == sinfo->end_code) {
    /* Skip the rest of the image, unless GetCode already read terminator */
    if (! sinfo->out_of_blocks) {
      SkipDataBlocks(sinfo);
      sinfo->out_of_blocks = TRUE;
    }
    /* Complain that there's not enough data */
//    WARNMS(sinfo->cinfo, JWRN_GIF_ENDCODE);
    /* Pad data with 0's */
    return 0;     /* fake something usable */
  }

  /* Got normal raw byte or LZW symbol */
  incode = code;    /* save for a moment */

  if (code >= sinfo->max_code) { /* special case for not-yet-defined symbol */
    /* code == max_code is OK; anything bigger is bad data */
    if (code > sinfo->max_code) {
//      WARNMS(sinfo->cinfo, JWRN_GIF_BADDATA);
      incode = 0;   /* prevent creation of loops in symbol table */
    }
    /* this symbol will be defined as oldcode/firstcode */
    *(sinfo->sp++) = (UINT8) sinfo->firstcode;
    code = sinfo->oldcode;
  }

  /* If it's a symbol, expand it into the stack */
  while (code >= sinfo->clear_code) {
    *(sinfo->sp++) = sinfo->symbol_tail[code]; /* tail is a byte value */
    code = sinfo->symbol_head[code]; /* head is another LZW symbol */
  }
  /* At this point code just represents a raw byte */
  sinfo->firstcode = code;  /* save for possible future use */

  /* If there's room in table, */
  if ((code = sinfo->max_code) < LZW_TABLE_SIZE) {
    /* Define a new symbol = prev sym + head of this sym's expansion */
    sinfo->symbol_head[code] = sinfo->oldcode;
    sinfo->symbol_tail[code] = (UINT8) sinfo->firstcode;
    sinfo->max_code++;
    /* Is it time to increase code_size? */
    if ((sinfo->max_code >= sinfo->limit_code) &&
  (sinfo->code_size < MAX_LZW_BITS)) {
      sinfo->code_size++;
      sinfo->limit_code <<= 1;  /* keep equal to 2^code_size */
    }
  }

  sinfo->oldcode = incode;  /* save last input symbol for future use */
  return sinfo->firstcode;  /* return first byte of symbol's expansion */
}


void ReadColorMap (gif_source_struct * sinfo, int cmaplen, RGBG * cmap)
/* Read a GIF colormap */
{
  int i;

  for (i = 0; i < cmaplen; i++) {
    cmap[i].rgb[CM_RED]   = (unsigned char) ReadByte(sinfo);
    cmap[i].rgb[CM_GREEN] = (unsigned char) ReadByte(sinfo);
    cmap[i].rgb[CM_BLUE]  = (unsigned char) ReadByte(sinfo);
  }
}


void DoExtension (gif_source_struct * sinfo)
/* Process an extension block */
/* Currently we ignore 'em all */
{
  int extlabel;

  /* Read extension label byte */
  extlabel = ReadByte(sinfo);
//  TRACEMS1(sinfo->cinfo, 1, JTRC_GIF_EXTENSION, extlabel);
  /* Skip the data block(s) associated with the extension */
  SkipDataBlocks(sinfo);
}


/*
 * Read the file header; return image size and component count.
 */

void start_input_gif (gif_source_struct * sinfo)
{
  gif_source_struct * source = (gif_source_struct *) sinfo;
  char hdrbuf[10];    /* workspace for reading control blocks */
  unsigned int width, height; /* image dimensions */
  int colormaplen, aspectRatio;
  int c;


  source->colormap = NULL;
  source->symbol_head = NULL;
  source->symbol_tail = NULL;
  source->symbol_stack = NULL;
  source->interlaced_image = NULL;
  source->buffer = NULL;

  /* Allocate space to store the colormap */
  source->colormap = (RGBG *) malloc(MAXCOLORMAPSIZE * sizeof(RGBG));

  /* Read and verify GIF Header */
  if (! ReadOK(source->input_file, hdrbuf, 6))
    throw JERR_GIF_NOT;
  if (hdrbuf[0] != 'G' || hdrbuf[1] != 'I' || hdrbuf[2] != 'F')
    throw JERR_GIF_NOT;
  /* Check for expected version numbers.
   * If unknown version, give warning and try to process anyway;
   * this is per recommendation in GIF89a standard.
   */
//  if ((hdrbuf[3] != '8' || hdrbuf[4] != '7' || hdrbuf[5] != 'a') &&
//      (hdrbuf[3] != '8' || hdrbuf[4] != '9' || hdrbuf[5] != 'a'))
//    TRACEMS3(cinfo, 1, JTRC_GIF_BADVERSION, hdrbuf[3], hdrbuf[4], hdrbuf[5]);

  /* Read and decipher Logical Screen Descriptor */
  if (! ReadOK(source->input_file, hdrbuf, 7))
    throw JERR_INPUT_EOF;
  width = LM_to_uint(hdrbuf[0],hdrbuf[1]);
  height = LM_to_uint(hdrbuf[2],hdrbuf[3]);
  colormaplen = 2 << (hdrbuf[4] & 0x07);
  /* we ignore the color resolution, sort flag, and background color index */
  aspectRatio = hdrbuf[6] & 0xFF;
//  if (aspectRatio != 0 && aspectRatio != 49)
//    TRACEMS(cinfo, 1, JTRC_GIF_NONSQUARE);

  /* Read global colormap if header indicates it is present */
  if (BitSet(hdrbuf[4], COLORMAPFLAG))
    ReadColorMap(source, colormaplen, source->colormap);

  /* Scan until we reach start of desired image.
   * We don't currently support skipping images, but could add it easily.
   */
  for (;;) {
    c = ReadByte(source);

    if (c == ';')   /* GIF terminator?? */
      throw JERR_GIF_IMAGENOTFOUND;

    if (c == '!') {   /* Extension */
      DoExtension(source);
      continue;
    }

    if (c != ',') {   /* Not an image separator? */
//      WARNMS1(cinfo, JWRN_GIF_CHAR, c);
      continue;
    }

    /* Read and decipher Local Image Descriptor */
    if (! ReadOK(source->input_file, hdrbuf, 9))
      throw JERR_INPUT_EOF;
    /* we ignore top/left position info, also sort flag */
    width = LM_to_uint(hdrbuf[4],hdrbuf[5]);
    height = LM_to_uint(hdrbuf[6],hdrbuf[7]);
    source->is_interlaced = BitSet(hdrbuf[8], INTERLACE);

    /* Read local colormap if header indicates it is present */
    /* Note: if we wanted to support skipping images, */
    /* we'd need to skip rather than read colormap for ignored images */
    if (BitSet(hdrbuf[8], COLORMAPFLAG)) {
      colormaplen = 2 << (hdrbuf[8] & 0x07);
      ReadColorMap(source, colormaplen, source->colormap);
    }

    source->input_code_size = ReadByte(source); /* get min-code-size byte */
    if (source->input_code_size < 2 || source->input_code_size >= MAX_LZW_BITS)
      throw JERR_GIF_CODESIZE;

    /* Reached desired image, so break out of loop */
    /* If we wanted to skip this image, */
    /* we'd call SkipDataBlocks and then continue the loop */
    break;
  }

  /* Prepare to read selected image: first initialize LZW decompressor */
  source->symbol_head = (UINT16 FAR *) malloc (LZW_TABLE_SIZE * sizeof(UINT16));
  source->symbol_tail = (UINT8 FAR *)  malloc (LZW_TABLE_SIZE * sizeof(UINT8));
  source->symbol_stack = (UINT8 FAR *) malloc (LZW_TABLE_SIZE * sizeof(UINT8));
  InitLZWCode(source);

  /*
   * If image is interlaced, we read it into a full-size sample array,
   * decompressing as we go; then get_interlaced_row selects rows from the
   * sample array in the proper order.
   */
  if (source->is_interlaced) {
    /* We request the virtual array now, but can't access it until virtual
     * arrays have been allocated.  Hence, the actual work of reading the
     * image is postponed until the first call to get_pixel_rows.
     */
    source->interlaced_image = (unsigned char *) malloc(width * height);
    source->get_pixel_rows = load_interlaced_image;
  } else {
    if (colormaplen == 2) 
        source->get_pixel_rows = get_pixel_rows1;
     else
        source->get_pixel_rows = get_pixel_rows8;
  }

  /* Create compressor input buffer. */
  source->buffer = (unsigned char *) malloc (width+1);

  /* Return info about the image. */
  sinfo->image_width = width;
  sinfo->image_height = height;
  sinfo->colormaplen = colormaplen;

//  TRACEMS3(cinfo, 1, JTRC_GIF, width, height, colormaplen);
}


/*
 * Read one row of pixels.
 * This version is used for noninterlaced GIF images:
 * we read directly from the GIF file.
 */

unsigned int get_pixel_rows8 (gif_source_struct * sinfo)
{
  register unsigned char * ptr;
  register unsigned int col;

  ptr = sinfo->buffer;
  for (col = sinfo->image_width; col > 0; col--) {
    *ptr++ = LZWReadByte(sinfo);
  }
  return 1;
}


unsigned int get_pixel_rows1 (gif_source_struct * sinfo)
{
  register unsigned char * ptr;
  register unsigned int col;
  register unsigned char c, cc, bit;

  ptr = sinfo->buffer;

  bit = 0x80; c = 0;

  for (col = sinfo->image_width; col > 0; col--) {
    cc = LZWReadByte(sinfo);
    if (cc) c |= bit;

    if (!bit) {
      *ptr++ = c;
      bit = 0x80; c = 0;
    }
    else 
      bit >>= 1;
  }
  *ptr++ = c;
  return 1;
}


/*
 * Read one row of pixels.
 * This version is used for the first call on get_pixel_rows when
 * reading an interlaced GIF file: we read the whole image into memory.
 */

unsigned int load_interlaced_image (gif_source_struct * sinfo)
{
  gif_source_struct * source = (gif_source_struct *) sinfo;
  register unsigned char * sptr;
  register unsigned int col;
  unsigned int row;

  /* Read the interlaced image into the virtual array we've created. */
  for (row = 0; row < sinfo->image_height; row++) {
    sptr = &source->interlaced_image[sinfo->image_width*row];
    for (col = sinfo->image_width; col > 0; col--) {
      *sptr++ = (unsigned char) LZWReadByte(source);
    }
  }

  /* Replace method pointer so subsequent calls don't come here. */
  source->get_pixel_rows = get_interlaced_row;
  /* Initialize for get_interlaced_row, and perform first call on it. */
  source->cur_row_number = 0;
  source->pass2_offset = (sinfo->image_height + 7) / 8;
  source->pass3_offset = source->pass2_offset + (sinfo->image_height + 3) / 8;
  source->pass4_offset = source->pass3_offset + (sinfo->image_height + 1) / 4;

  return get_interlaced_row(sinfo);
}


/*
 * Read one row of pixels.
 * This version is used for interlaced GIF images:
 * we read from the virtual array.
 */

unsigned int get_interlaced_row (gif_source_struct * sinfo)
{
  register unsigned char * sptr, * ptr;
  register unsigned int col;
  unsigned int irow;

  /* Figure out which row of interlaced image is needed, and access it. */
  switch ((int) (sinfo->cur_row_number & 7)) {
  case 0:     /* first-pass row */
    irow = sinfo->cur_row_number >> 3;
    break;
  case 4:     /* second-pass row */
    irow = (sinfo->cur_row_number >> 3) + sinfo->pass2_offset;
    break;
  case 2:     /* third-pass row */
  case 6:
    irow = (sinfo->cur_row_number >> 2) + sinfo->pass3_offset;
    break;
  default:      /* fourth-pass row */
    irow = (sinfo->cur_row_number >> 1) + sinfo->pass4_offset;
    break;
  }

  sptr = &sinfo->interlaced_image[sinfo->image_width*irow];

  /* Scan the row, expand colormap, and output */
  ptr = sinfo->buffer;
  for (col = sinfo->image_width; col > 0; col--) {
    *ptr++ = *sptr++;
  }
  sinfo->cur_row_number++; /* for next time */
  return 1;
}

void end_input_gif (gif_source_struct * sinfo)
{
  if (sinfo) {
    if (sinfo->colormap) free(sinfo->colormap);
    if (sinfo->symbol_head) free(sinfo->symbol_head);
    if (sinfo->symbol_tail) free(sinfo->symbol_tail);
    if (sinfo->symbol_stack) free(sinfo->symbol_stack);
    if (sinfo->interlaced_image) free(sinfo->interlaced_image);
    if (sinfo->buffer) free(sinfo->buffer);
    free(sinfo);
  }
}
