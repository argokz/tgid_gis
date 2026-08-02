#pragma once

#define FALSE 0
#define TRUE 1

enum lexem_value {
 NONE, N_0, N_1, N_2, N_3, N_4, N_5, N_6, N_7, N_8, N_9, N_10, N_11, N_12, N_13,
 N_14, N_15, N_24, N_25,
 N_20, N_21, N_22, N_23, N_31, N_39, N_40, N_41, N_42, N_43, N_44, N_45, N_49,
 N_50, N_51, N_62, N_66, N_70, N_71, N_72, N_73, N_74, N_90, N_93, N_100, N_290, N_330,
 ARC, ATTDEF, ATTRIB, BLOCK, BLOCK_RECORD, BLOCKS, CIRCLE, DIMENSION, ENDBLK, ENDSEC,
 ENDTAB, ENTITIES, EOFILE, EXTMAX, EXTMIN, HEADER, INSERT, LAYER, LIMMAX,
 LIMMIN, LINE, LTYPE, POINT1, POLYLINE, LWPOLYLINE, HATCH, 
 PEXTMAX, PEXTMIN, CMLSCALE, INSUNITS, PLINEWID,
 ELLIPSE, 
 SECTION, 
 SEQEND, 
 SHAPE, 
 SOLID,
 STYLE, 
 TABLE, 
 TABLES, 
 SPLINE,
 TEXT, 
 TRACE1,
 TRACEWID, 
 MEASUREMENT,
 VERTEX, 
 VIEW,
 VIEWPORT,
 VPORT,
 OBJECTS,
 LAYOUT,
 SCALE,
 DICTIONARY,
 XRECORD,
 DWGCODEPAGE,
 ACAD_PROXY_OBJECT,
 MTEXT,
 _3DFACE,
 REGION,
 RAY,
 IMAGE,
 WIPEOUT
};

struct tok_tab {
  tok_tab *next;
  char *tok_name;
  int tok_value;
};

extern tok_tab tok_table[];

char *get_tag(FILE *dxf);
int get_tag_token(const char *s);
void init_tok_table();
int hash(const char *s);
char *locate(const char *s);
