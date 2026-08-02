#pragma once

typedef unsigned short UINT16;
typedef unsigned char UINT8;

#define CM_RED    0 /* color component numbers */
#define CM_GREEN  1
#define CM_BLUE   2

typedef struct {
  unsigned char rgb[3];
} RGBG;


/* Private version of data source object */


struct gif_source_struct {
//  struct cjpeg_source_struct pub; /* public fields */

//  void (*start_input)(cjpeg_source_struct * sinfo);
  unsigned int (*get_pixel_rows)(gif_source_struct * sinfo);

  FILE *input_file;

  unsigned char * buffer;

  unsigned int image_width;
  unsigned int image_height;
  int colormaplen;


//  j_compress_ptr cinfo;   /* back link saves passing separate parm */

  RGBG *colormap;    /* GIF colormap (converted to my format) */

  /* State for GetCode and LZWReadByte */
  char code_buf[256+4];   /* current input data block */
  int last_byte;    /* # of bytes in code_buf */
  int last_bit;     /* # of bits in code_buf */
  int cur_bit;      /* next bit index to read */
  bool out_of_blocks;  /* TRUE if hit terminator data block */

  int input_code_size;    /* codesize given in GIF file */
  int clear_code,end_code;  /* values for Clear and End codes */

  int code_size;    /* current actual code size */
  int limit_code;   /* 2^code_size */
  int max_code;     /* first unused code value */
  bool first_time;   /* flags first call to LZWReadByte */

  /* Private state for LZWReadByte */
  int oldcode;      /* previous LZW symbol */
  int firstcode;    /* first byte of oldcode's expansion */

  /* LZW symbol table and expansion stack */
  UINT16 FAR *symbol_head;  /* => table of prefix symbols */
  UINT8  FAR *symbol_tail;  /* => table of suffix bytes */
  UINT8  FAR *symbol_stack; /* => stack for symbol expansions */
  UINT8  FAR *sp;   /* stack pointer */

  /* State for interlaced image processing */
  bool is_interlaced;  /* TRUE if have interlaced image */
  unsigned char * interlaced_image; /* full image in interlaced order */
  unsigned int cur_row_number;  /* need to know actual row number */
  unsigned int pass2_offset;  /* # of pixel rows in pass 1 */
  unsigned int pass3_offset;  /* # of pixel rows in passes 1&2 */
  unsigned int pass4_offset;  /* # of pixel rows in passes 1,2,3 */
};


/* Forward declarations */

unsigned int get_pixel_rows(gif_source_struct * sinfo);
unsigned int load_interlaced_image(gif_source_struct * sinfo);
unsigned int get_interlaced_row(gif_source_struct * sinfo);
unsigned int get_pixel_rows8 (gif_source_struct * sinfo);
unsigned int get_pixel_rows1 (gif_source_struct * sinfo);


#define JERR_INPUT_EOF 1
#define JWRN_GIF_BADDATA 2
#define JWRN_GIF_NOMOREDATA 3
#define JERR_GIF_NOT 4
#define JERR_GIF_IMAGENOTFOUND 5
#define JERR_GIF_CODESIZE 6
