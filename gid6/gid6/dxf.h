#pragma once


void write_DXF(FILE *dxf, int tag, const void *value);
void generate_header(FILE *dxf);
void generate_tables(FILE *dxf);
void generate_styles(FILE *dxf);
void generate_linetypes(FILE *dxf);
void generate_layers(FILE *dxf);
void generate_views(FILE *dxf);

void generate_tables(FILE *dxf, const char *layer, int color);
void generate_tables2(FILE *dxf, const char *layer, int color);


void write_circle(FILE *dxf, double x, double y, double r, const char *layer, int h = 0);
void write_line(FILE *dxf, double x1, double y1, double x2, double y2, const char *layer, int h = 0);
void write_point(FILE *dxf, double x, double y, const char *layer, int h = 0);
int write_pline(FILE *dxf, int flags, double width1, double width2, const char *layer, int h = 0);
void write_vertex(FILE *dxf, double x, double y, double width1, double width2, double bulge, const char *layer, int h = 0);
void write_seqend(FILE *dxf, const char *layer, int h = 0);
int write_pline(FILE *dxf, int n, double *x, double *y, const char *layer, int h = 0);
void write_text(FILE *dxf, double x, double y, const char *txt, double ang = 0., double ht = 1., int just = 0, int h = 0);
void write_text(FILE *dxf, double x, double y, const char *txt, double angle, double ht, int just, const char *layer, int h = 0);

void fillpoly(FILE *dxf, int n, double *x, double *y, const char *layer);
void drawpoly(FILE *dxf, int n, double *x, double *y, const char *layer);
void line(FILE *dxf, double x1, double y1, double x2, double y2, const char *layer);
void rectangle(FILE *dxf, double x1, double y1, double x2, double y2, const char *layer);
void write_block(FILE *dxf, const char *name, const char *layer);
void write_block(FILE *dxf, const char *name, double x, double y, const char *layer);
void write_attrib(FILE *dxf, const char *atr, const char *name, const char *layer);
void write_attrib(FILE *dxf, const char *atr, const char *prompt, const char *name, const char *layer);
void write_attdef(FILE *dxf, const char *atr, const char *layer);
void write_insert(FILE *dxf, const char *name, double x, double y, const char *layer, int h = 0);
void write_insert(FILE *dxf, const char *name, double x, double y, int at, const char *layer, int h = 0);

unsigned long dxf_color(int c);
