#include <stdio.h>
#include <string.h>
#include "dxf.h"

int rgb_l(int color);

extern int handle;

void write_DXF_hex(FILE* dxf, int tag, int v);
void write_DXF_int(FILE* dxf, int tag, int v);
void write_DXF_double(FILE* dxf, int tag, double v);
//void write_DXF(FILE* dxf, int tag, const void* value);
int write_5_330(FILE *dxf, int handle_parent);


void write_DXF_hex(FILE* dxf, int tag, int v)
{
    fprintf(dxf, "%3d\n", tag);
    fprintf(dxf, "%X\n", v);
}

void write_DXF_int(FILE* dxf, int tag, int v)
{
    fprintf(dxf, "%3d\n", tag);
    fprintf(dxf, "%d\n", v);
}

void write_DXF_double(FILE* dxf, int tag, double v)
{
    fprintf(dxf, "%3d\n", tag);
    fprintf(dxf, "%f\n", v);
}


void write_DXF_txt(FILE* dxf, int tag, const void* value) {
    int i;

    fprintf(dxf, "%3d\n", tag);
    char* s = (char*)value;
    for (i = 0; s[i]; i++) if (s[i] == '\n' || s[i] == '\r') s[i] = ' ';
    fprintf(dxf, "%s\n", (char*)value);
}



void write_DXF(FILE* dxf, int tag, const void* value) {
    int i;

    fprintf(dxf, "%3d\n", tag);
    if (tag%100 <= 9) {
        char* s = (char*)value;
        for (i = 0; s[i]; i++) if (s[i] == '\n' || s[i] == '\r') s[i] = ' ';
        fprintf(dxf, "%s\n", (char*)value);
    }
    else {
        if (tag%100 <= 59)
            fprintf(dxf, "%f\n", *(double*)value);
        else
            fprintf(dxf, "%d\n", *(int*)value);
    }
}


void write_VPORT(FILE* dxf)
{
  write_DXF_txt(dxf, 0, "TABLE");
  write_DXF_txt(dxf, 2, "VPORT");
//  write_DXF_int(dxf, 5, 8);
  write_DXF_hex(dxf, 5, handle++);
  write_DXF_int(dxf, 330, 0);
  write_DXF_txt(dxf, 100, "AcDbSymbolTable");
  write_DXF_int(dxf, 70, 1);
  write_DXF_txt(dxf, 0, "VPORT");
//  write_DXF_int(dxf, 5, 31);
  write_DXF_hex(dxf, 5, handle++);
  write_DXF_int(dxf, 330, 2);
  write_DXF_txt(dxf, 100, "AcDbSymbolTableRecord");
  write_DXF_txt(dxf, 100, "AcDbViewportTableRecord");
  write_DXF_txt(dxf, 2, "*ACTIVE");
  write_DXF_int(dxf, 70, 0);
  write_DXF_int(dxf, 10, 0);
  write_DXF_int(dxf, 20, 0);
  write_DXF_int(dxf, 11, 1);
  write_DXF_int(dxf, 21, 1);
  write_DXF_double(dxf, 12, 209.475294253);
  write_DXF_double(dxf, 22, 86.0026335861);
  write_DXF_int(dxf, 13, 0);
  write_DXF_int(dxf, 23, 0);
  write_DXF_int(dxf, 14, 10);
  write_DXF_int(dxf, 24, 10);
  write_DXF_int(dxf, 15, 10);
  write_DXF_int(dxf, 25, 10);
  write_DXF_int(dxf, 16, 0);
  write_DXF_int(dxf, 26, 0);
  write_DXF_int(dxf, 36, 1);
  write_DXF_int(dxf, 17, 0);
  write_DXF_int(dxf, 27, 0);
  write_DXF_int(dxf, 37, 0);
  write_DXF_double(dxf, 40, 319.744231092);
  write_DXF_double(dxf, 41, 2.12946428571);
  write_DXF_int(dxf, 42, 50);
  write_DXF_int(dxf, 43, 0);
  write_DXF_int(dxf, 44, 0);
  write_DXF_int(dxf, 50, 0);
  write_DXF_int(dxf, 51, 0);
  write_DXF_int(dxf, 71, 0);
  write_DXF_int(dxf, 72, 100);
  write_DXF_int(dxf, 73, 1);
  write_DXF_int(dxf, 74, 3);
  write_DXF_int(dxf, 75, 0);
  write_DXF_int(dxf, 76, 0);
  write_DXF_int(dxf, 77, 0);
  write_DXF_int(dxf, 78, 0);
  write_DXF_int(dxf, 281, 0);
  write_DXF_int(dxf, 65, 1);
  write_DXF_int(dxf, 110, 0);
  write_DXF_int(dxf, 120, 0);
  write_DXF_int(dxf, 130, 0);
  write_DXF_int(dxf, 111, 1);
  write_DXF_int(dxf, 121, 0);
  write_DXF_int(dxf, 131, 0);
  write_DXF_int(dxf, 112, 0);
  write_DXF_int(dxf, 122, 1);
  write_DXF_int(dxf, 132, 0);
  write_DXF_int(dxf, 79, 0);
  write_DXF_int(dxf, 146, 0);
  write_DXF_int(dxf, 348, 10020);
  write_DXF_int(dxf, 60, 7);
  write_DXF_int(dxf, 61, 5);
  write_DXF_int(dxf, 292, 1);
  write_DXF_int(dxf, 282, 1);
  write_DXF_int(dxf, 141, 0);
  write_DXF_int(dxf, 142, 0);
  write_DXF_int(dxf, 63, 250);
  write_DXF_int(dxf, 421, 3358443);
  write_DXF_txt(dxf, 0, "ENDTAB");
}






void write_line(FILE* dxf, double x1, double y1, double x2, double y2, const char* layer) {
    write_DXF_txt(dxf, 0, "LINE");
    write_5_330(dxf, 0);
    write_DXF_txt(dxf, 100, "AcDbEntity");
    write_DXF(dxf, 8, layer);
    write_DXF_txt(dxf, 100, "AcDbLine");
    write_DXF(dxf, 10, &x1);
    write_DXF(dxf, 20, &y1);
    write_DXF(dxf, 11, &x2);
    write_DXF(dxf, 21, &y2);
}
void line(FILE* dxf, double x1, double y1, double x2, double y2, const char* layer) {
    write_line(dxf, x1, y1, x2, y2, layer);
}

void rectangle(FILE* dxf, double x1, double y1, double x2, double y2, const char* layer) {
    write_line(dxf, x1, y1, x1, y2, layer);
    write_line(dxf, x1, y1, x2, y1, layer);
    write_line(dxf, x2, y2, x1, y2, layer);
    write_line(dxf, x2, y2, x2, y1, layer);
}

void write_point(FILE* dxf, double x, double y, const char* layer) {
    write_DXF_txt(dxf, 0, "POINT");
    write_5_330(dxf, 0);
    write_DXF_txt(dxf, 100, "AcDbEntity");
    write_DXF(dxf, 8, layer);
    write_DXF(dxf, 10, &x);
    write_DXF(dxf, 20, &y);
}

void write_circle(FILE* dxf, double x, double y, double r, const char* layer) {
    write_DXF_txt(dxf, 0, "CIRCLE");
    write_5_330(dxf, 0);
    write_DXF_txt(dxf, 100, "AcDbEntity");
    write_DXF(dxf, 8, layer);
    write_DXF_txt(dxf, 100, "AcDbCircle");
    write_DXF(dxf, 10, &x);
    write_DXF(dxf, 20, &y);
    write_DXF(dxf, 40, &r);
}


void write_pline(FILE* dxf, int flags, double width1, double width2, const char* layer) {
    int i;

    write_DXF_txt(dxf, 0, "POLYLINE");
    write_5_330(dxf, 0);
    write_DXF_txt(dxf, 100, "AcDbEntity");
    write_DXF(dxf, 8, layer);
    write_DXF_txt(dxf, 100, "AcDb2dPolyline");
    write_DXF(dxf, 70, &flags);

    i = 1;
    write_DXF(dxf, 66, &i);
    write_DXF(dxf, 40, &width1);
    write_DXF(dxf, 41, &width2);
}

void write_vertex(FILE* dxf, double x, double y, double width1, double width2, double bulge, const char* layer) {

    write_DXF_txt(dxf, 0, "VERTEX");
    write_5_330(dxf, 0);
    write_DXF_txt(dxf, 100, "AcDbEntity");
    write_DXF(dxf, 8, layer);
    write_DXF_txt(dxf, 100, "AcDbVertex");
    write_DXF_txt(dxf, 100, "AcDb2dVertex");

    write_DXF(dxf, 10, &x);
    write_DXF(dxf, 20, &y);
    if (width1) write_DXF(dxf, 40, &width1);
    if (width2) write_DXF(dxf, 41, &width2);
    if (bulge)  write_DXF(dxf, 42, &bulge);
}

void write_seqend(FILE* dxf, const char* layer) {
    write_DXF_txt(dxf, 0, "SEQEND");
    write_5_330(dxf, 0);
    write_DXF_txt(dxf, 100, "AcDbEntity");
    write_DXF(dxf, 8, layer);
}

void fillpoly(FILE* dxf, int n, double* x, double* y, const char* layer) {
    int i;

    write_pline(dxf, 1, 0., 0., layer);

    for (i = 0; i < n; i++) {
        write_vertex(dxf, x[i], y[i], 0., 0., 0., layer);
    }

    write_seqend(dxf, layer);
}


void drawpoly(FILE* dxf, int n, double* x, double* y, const char* layer) {
    int i;

    write_pline(dxf, 0, 0., 0., layer);

    for (i = 0; i < n; i++) {
        write_vertex(dxf, x[i], y[i], 0., 0., 0., layer);
    }

    write_seqend(dxf, layer);
}


void write_mtext(FILE* dxf, double x, double y, const char* txt, double angle, double ht, int just) {
    double xx;

    write_DXF_txt(dxf, 0, "MTEXT");
    write_5_330(dxf, 0);
    write_DXF_txt(dxf, 100, "AcDbEntity");
    write_DXF_txt(dxf, 8, "BLACK");
    //  write_DXF_txt(dxf, 7, "SYMRYB");

    xx = x;
    //  if (just) xx = x - ht*strlen(txt)*3;
    write_DXF_txt(dxf, 100, "AcDbMText");

    write_DXF(dxf, 10, &xx);
    write_DXF(dxf, 20, &y);
    write_DXF(dxf, 40, &ht);
    write_DXF(dxf, 1, txt);
    if (angle) write_DXF(dxf, 50, &angle);
    //  if (xscale != 1.0) write_DXF(dxf, 41, &xscale);
    if (just) {
        write_DXF_int(dxf, 71, 2);
//        write_DXF(dxf, 72, &just);
//        write_DXF(dxf, 11, &x);
//        write_DXF(dxf, 21, &y);
    }
}


void write_text(FILE* dxf, double x, double y, const char* txt, double angle, double ht, int just, const char* layer) 
{
    write_DXF_txt(dxf, 0, "TEXT");
    write_5_330(dxf, 0);
    write_DXF_txt(dxf, 100, "AcDbEntity");
    write_DXF(dxf, 8, layer);

    write_DXF_int(dxf, 62, 111);

    write_DXF_txt(dxf, 100, "AcDbText");
    write_DXF(dxf, 10, &x);
    write_DXF(dxf, 20, &y);
    write_DXF(dxf, 40, &ht);
    write_DXF(dxf, 1, txt);
    write_DXF(dxf, 11, &x);
    write_DXF(dxf, 21, &y);

//    write_DXF_txt(dxf, 7, "SYMRYB");
    if (angle) write_DXF(dxf, 50, &angle);
    //  if (xscale != 1.0) write_DXF(dxf, 41, &xscale);
    if (just) {
        write_DXF(dxf, 72, &just);
        write_DXF(dxf, 11, &x);
        write_DXF(dxf, 21, &y);
    }
    write_DXF_txt(dxf, 100, "AcDbText");
    write_DXF_int(dxf, 73, 2);
}

void write_text(FILE* dxf, double x, double y, const char* txt, double angle, double ht, int just) 
{
    write_text(dxf, x, y, txt, angle, ht, just, "RED");
}



#if 0

void write_text(FILE* dxf, double x, double y, const char* txt, double angle, double ht, int just) 
{
    write_DXF_txt(dxf, 0, "TEXT");
    write_5_330(dxf, 0);
    write_DXF_txt(dxf, 100, "AcDbEntity");
    write_DXF_txt(dxf, 8, "RED");

    write_DXF_int(dxf, 62, 111);

    write_DXF_txt(dxf, 100, "AcDbText");
    write_DXF(dxf, 10, &x);
    write_DXF(dxf, 20, &y);
    write_DXF(dxf, 40, &ht);
    write_DXF(dxf, 1, txt);
    write_DXF(dxf, 11, &x);
    write_DXF(dxf, 21, &y);

//    write_DXF_txt(dxf, 7, "SYMRYB");
    if (angle) write_DXF(dxf, 50, &angle);
    //  if (xscale != 1.0) write_DXF(dxf, 41, &xscale);
    if (just) {
        write_DXF(dxf, 72, &just);
        write_DXF(dxf, 11, &x);
        write_DXF(dxf, 21, &y);
    }
    write_DXF_txt(dxf, 100, "AcDbText");
    write_DXF_int(dxf, 73, 2);
}
    
    
void write_text(FILE* dxf, double x, double y, const char* txt, double angle, double ht, int just, const char* layer) {
    double xx;

    write_DXF_txt(dxf, 0, "TEXT");
    write_5_330(dxf, 0);
    write_DXF_txt(dxf, 100, "AcDbEntity");
    write_DXF(dxf, 8, layer);
    write_DXF_txt(dxf, 7, "SYMRYB");

    xx = x;
    //  if (just) xx = x - ht*strlen(txt)*3;

    write_DXF(dxf, 10, &xx);
    write_DXF(dxf, 20, &y);
    write_DXF(dxf, 40, &ht);
    write_DXF(dxf, 1, txt);
    if (angle) write_DXF(dxf, 50, &angle);
    //  if (xscale != 1.0) write_DXF(dxf, 41, &xscale);
    if (just) {
        write_DXF(dxf, 72, &just);
        write_DXF(dxf, 11, &x);
        write_DXF(dxf, 21, &y);
    }
}
#endif


void write_insert(FILE* dxf, const char* name, double x, double y, int at, const char* layer) 
{
    write_DXF_txt(dxf, 0, "INSERT");
    write_5_330(dxf, 0);
    write_DXF_txt(dxf, 100, "AcDbEntity");
    write_DXF(dxf, 8, layer);
    write_DXF_txt(dxf, 100, "AcDbBlockReference");
    write_DXF(dxf, 2, name);
    if (at) write_DXF_int(dxf, 66, 1);
    write_DXF(dxf, 10, &x);
    write_DXF(dxf, 20, &y);

    //  write_DXF(dxf, 42, &x_scale);
    //  write_DXF(dxf, 43, &y_scale);
    //  write_DXF(dxf, 50, &angle);
}


void write_block(FILE* dxf, const char* name, double x, double y, const char* layer) {
    write_DXF_txt(dxf, 0, "BLOCK");
    write_DXF(dxf, 2, name);
    write_DXF(dxf, 8, layer);
    write_DXF(dxf, 10, &x);
    write_DXF(dxf, 20, &y);
}

void write_block(FILE* dxf, const char* name, const char* layer) {
    write_DXF_txt(dxf, 0, "BLOCK");
    write_DXF(dxf, 2, name);
    write_DXF(dxf, 8, layer);
}

void write_attrib(FILE* dxf, const char* atr, const char* name, const char* layer) {
    double x = 0;
    int n = 1;

    write_DXF_txt(dxf, 0, "ATTRIB");
    write_DXF(dxf, 8, layer);
    write_DXF(dxf, 1, name);
    write_DXF(dxf, 2, atr);
    write_DXF(dxf, 70, &n);
}

void write_attdef(FILE* dxf, const char* atr, const char* layer) {
    int n = 1;

    write_DXF_txt(dxf, 0, "ATTDEF");
    write_DXF(dxf, 8, layer);
    write_DXF_txt(dxf, 1, "");
    write_DXF_txt(dxf, 3, "");
    write_DXF(dxf, 2, atr);
    write_DXF(dxf, 70, &n);
}


void generate_tables(FILE* dxf, const char* layer, int color) {
    int n;
    int col_cod[] = { 7, 5, 3, 4, 1, 6, 2, 9, 8, 5, 3, 4, 1, 6, 2, 7 };

    write_DXF_txt(dxf, 0, "SECTION");
    write_DXF_txt(dxf, 2, "TABLES");

    write_DXF_txt(dxf, 0, "TABLE");
    write_DXF_txt(dxf, 2, "LAYER");
    write_DXF_hex(dxf, 5, handle++);
    write_DXF_int(dxf, 330, 0);
    write_DXF_txt(dxf, 100, "AcDbSymbolTable");

    n = 1;
    write_DXF(dxf, 70, &n);

    write_DXF_txt(dxf, 0, "LAYER");
    write_DXF(dxf, 2, (void*)layer);
    write_DXF_int(dxf, 330, 0);
    write_DXF_txt(dxf, 100, "AcDbSymbolTableRecord");
    write_DXF_txt(dxf, 100, "AcDbLayerTableRecord");


    write_DXF_int(dxf, 70, 64);
    write_DXF_int(dxf, 62, color);
    write_DXF_txt(dxf, 6, "CONTINUOUS");

    write_DXF_txt(dxf, 0, "ENDTAB");
    write_DXF_txt(dxf, 0, "ENDSEC");
}


void generate_tables(FILE* dxf) {
    int i, n;
    int col_cod[] = { 7, 5, 3, 4, 1, 6, 2, 9, 8, 5, 3, 4, 1, 6, 2, 7 };

    write_DXF_txt(dxf, 0, "SECTION");
    write_DXF_txt(dxf, 2, "TABLES");

    write_VPORT(dxf);

    write_DXF_txt(dxf, 0, "TABLE");
    write_DXF_txt(dxf, 2, "LAYER");
    write_DXF_hex(dxf, 5, handle++);
    write_DXF_int(dxf, 330, 0);
    write_DXF_txt(dxf, 100, "AcDbSymbolTable");

    n = 1;

    write_DXF(dxf, 70, &n);

    for (i = 0; i < n; i++) {
        write_DXF_txt(dxf, 0, "LAYER");
        write_DXF_txt(dxf, 2, "0");
        write_DXF_int(dxf, 330, 0);
        write_DXF_txt(dxf, 100, "AcDbSymbolTableRecord");
        write_DXF_txt(dxf, 100, "AcDbLayerTableRecord");

        write_DXF_int(dxf, 70, 64);
        //    k = col_cod[kl[i].coli];
        write_DXF_int(dxf, 62, 5);
        write_DXF_txt(dxf, 6, "CONTINUOUS");
    }

    write_DXF_txt(dxf, 0, "LAYER");
    write_DXF_txt(dxf, 2, "BLACK");

    write_DXF_hex(dxf, 5, handle++);
    write_DXF_int(dxf, 330, 0);
    write_DXF_txt(dxf, 100, "AcDbSymbolTable");


    write_DXF_int(dxf, 70, 64);
    write_DXF_int(dxf, 62, 7);
    write_DXF_txt(dxf, 6, "CONTINUOUS");

    write_DXF_txt(dxf, 0, "ENDTAB");

    write_DXF_txt(dxf, 0, "TABLE");
    write_DXF_txt(dxf, 2, "STYLE");
    write_DXF_hex(dxf, 5, handle++);
    write_DXF_int(dxf, 330, 0);
    write_DXF_txt(dxf, 100, "AcDbSymbolTable");

    write_DXF_int(dxf, 70, 1);
    write_DXF_txt(dxf, 0, "STYLE");
    write_DXF_hex(dxf, 5, handle++);
    write_DXF_int(dxf, 330, 0);
    write_DXF_txt(dxf, 100, "AcDbSymbolTableRecord");
    write_DXF_txt(dxf, 100, "AcDbTextStyleTableRecord");

    write_DXF_txt(dxf, 2, "SYMRYB");

    write_DXF_int(dxf, 70, 2);
    write_DXF_int(dxf, 40, 0);
    write_DXF_int(dxf, 41, 1);
    write_DXF_int(dxf, 50, 0);
    write_DXF_int(dxf, 71, 0);
    write_DXF_int(dxf, 42, 0);
    write_DXF_txt(dxf, 3, "SYMRYB");
    write_DXF_txt(dxf, 4, "");
    write_DXF_txt(dxf, 0, "ENDTAB");

    write_DXF_txt(dxf, 0, "ENDSEC");
}

void generate_header(FILE* dxf, long x1, long y1, long x2, long y2) {
    double limminx, limminy, limmaxx, limmaxy;

    limminx = x1 / 100.;  limmaxx = x2 / 100.;
    limminy = y1 / 100.;  limmaxy = y2 / 100.;

    write_DXF_txt(dxf, 0, "SECTION");
    write_DXF_txt(dxf, 2, "HEADER");

    write_DXF_txt(dxf, 9, "$ACADVER");  write_DXF(dxf, 1, "AC1021");

    write_DXF_txt(dxf, 9, "$HANDSEED");
    write_DXF_int(dxf, 5, 2000000);



    write_DXF_txt(dxf, 9, "$EXTMIN");  write_DXF(dxf, 10, &limminx);  write_DXF(dxf, 20, &limminy);
    write_DXF_txt(dxf, 9, "$EXTMAX");  write_DXF(dxf, 10, &limmaxx);  write_DXF(dxf, 20, &limmaxy);


    write_DXF_txt(dxf, 9, "$LIMMIN");  write_DXF(dxf, 10, &limminx);  write_DXF(dxf, 20, &limminy);
    write_DXF_txt(dxf, 9, "$LIMMAX");  write_DXF(dxf, 10, &limmaxx);  write_DXF(dxf, 20, &limmaxy);


    int zero = 0;

    write_DXF_txt(dxf, 9, "$INSUNITS");  write_DXF(dxf, 70, &zero);


    write_DXF_txt(dxf, 0, "ENDSEC");

    write_DXF_txt(dxf, 0, "SECTION");
    write_DXF_txt(dxf, 2, "CLASSES");
    write_DXF_txt(dxf, 0, "ENDSEC");
}



unsigned long dxf_color(int c) {
    unsigned long rgb[] = {
    0x000000, 0x0000FF, 0x00FFFF, 0x00FF00, 0xFFFF00, 0xFF0000, 0xFF00FF, 0x000000,

//    0x000000, 0x0000FF, 0x00FFFF, 0x00FF00, 0xFFFF00, 0xFF0000, 0xFF00FF, 0xFFFFFF,

    0x414141, 0x808080, 0x0000FF, 0xAAAAFF, 0x0000BD, 0x7E7EBD, 0x000081, 0x565681,
    0x000068, 0x454568, 0x00004F, 0x35354F, 0x003FFF, 0xAABFFF, 0x002EBD, 0x7E8DBD,
    0x001F81, 0x566081, 0x001968, 0x454E68, 0x00134F, 0x353B4F, 0x007FFF, 0xAAD4FF,
    0x005EBD, 0x7E9DBD, 0x004081, 0x566B81, 0x003468, 0x455668, 0x00274F, 0x35424F,
    0x00BFFF, 0xAAEAFF, 0x008DBD, 0x7EADBD, 0x006081, 0x567681, 0x004E68, 0x455F68,
    0x003B4F, 0x35494F, 0x00FFFF, 0xAAFFFF, 0x00BDBD, 0x7EBDBD, 0x008181, 0x568181,
    0x006868, 0x456868, 0x004F4F, 0x354F4F, 0x00FFBF, 0xAAFFEA, 0x00BD8D, 0x7EBDAD,
    0x008160, 0x568176, 0x00684E, 0x45685F, 0x004F3B, 0x354F49, 0x00FF7F, 0xAAFFD4,
    0x00BD5E, 0x7EBD9D, 0x008140, 0x56816B, 0x006834, 0x456856, 0x004F27, 0x354F42,
    0x00FF3F, 0xAAFFBF, 0x00BD2E, 0x7EBD8D, 0x00811F, 0x568160, 0x006819, 0x45684E,
    0x004F13, 0x354F3B, 0x00FF00, 0xAAFFAA, 0x00BD00, 0x7EBD7E, 0x008100, 0x568156,
    0x006800, 0x456845, 0x004F00, 0x354F35, 0x3FFF00, 0xBFFFAA, 0x2EBD00, 0x8DBD7E,
    0x1F8100, 0x608156, 0x196800, 0x4E6845, 0x134F00, 0x3B4F35, 0x7FFF00, 0xD4FFAA,
    0x5EBD00, 0x9DBD7E, 0x408100, 0x6B8156, 0x346800, 0x566845, 0x274F00, 0x424F35,
    0xBFFF00, 0xEAFFAA, 0x8DBD00, 0xADBD7E, 0x608100, 0x768156, 0x4E6800, 0x5F6845,
    0x3B4F00, 0x494F35, 0xFFFF00, 0xFFFFAA, 0xBDBD00, 0xBDBD7E, 0x818100, 0x818156,
    0x686800, 0x686845, 0x4F4F00, 0x4F4F35, 0xFFBF00, 0xFFEAAA, 0xBD8D00, 0xBDAD7E,
    0x816000, 0x817656, 0x684E00, 0x685F45, 0x4F3B00, 0x4F4935, 0xFF7F00, 0xFFD4AA,
    0xBD5E00, 0xBD9D7E, 0x814000, 0x816B56, 0x683400, 0x685645, 0x4F2700, 0x4F4235,
    0xFF3F00, 0xFFBFAA, 0xBD2E00, 0xBD8D7E, 0x811F00, 0x816056, 0x681900, 0x684E45,
    0x4F1300, 0x4F3B35, 0xFF0000, 0xFFAAAA, 0xBD0000, 0xBD7E7E, 0x810000, 0x815656,
    0x680000, 0x684545, 0x4F0000, 0x4F3535, 0xFF003F, 0xFFAABF, 0xBD002E, 0xBD7E8D,
    0x81001F, 0x815660, 0x680019, 0x68454E, 0x4F0013, 0x4F353B, 0xFF007F, 0xFFAAD4,
    0xBD005E, 0xBD7E9D, 0x810040, 0x81566B, 0x680034, 0x684556, 0x4F0027, 0x4F3542,
    0xFF00BF, 0xFFAAEA, 0xBD008D, 0xBD7EAD, 0x810060, 0x815676, 0x68004E, 0x68455F,
    0x4F003B, 0x4F3549, 0xFF00FF, 0xFFAAFF, 0xBD00BD, 0xBD7EBD, 0x810081, 0x815681,
    0x680068, 0x684568, 0x4F004F, 0x4F354F, 0xBF00FF, 0xEAAAFF, 0x8D00BD, 0xAD7EBD,
    0x600081, 0x765681, 0x4E0068, 0x5F4568, 0x3B004F, 0x49354F, 0x7F00FF, 0xD4AAFF,
    0x5E00BD, 0x9D7EBD, 0x400081, 0x6B5681, 0x340068, 0x564568, 0x27004F, 0x42354F,
    0x3F00FF, 0xBFAAFF, 0x2E00BD, 0x8D7EBD, 0x1F0081, 0x605681, 0x190068, 0x4E4568,
    0x13004F, 0x3B354F, 0x333333, 0x505050, 0x696969, 0x828282, 0xBEBEBE, 0xFFFFFF
    };

    return rgb[c];

/*
    if (0 <= c && c < sizeof(rgb) / sizeof(rgb[0])) {
        if (rgb[c] == 0xFFFFFF) return rgb_l(0);
        return rgb_l(rgb[c]);
    }
    return rgb_l(0);
*/
}

void generate_block1(FILE* dxf, const char* name)
{
    double z = 0;
    write_DXF_txt(dxf, 0, "BLOCK");
    write_DXF_txt(dxf, 8, "0");
    write_DXF(dxf, 70, &z);
    write_DXF(dxf, 10, &z);
    write_DXF(dxf, 20, &z);
    write_DXF(dxf, 2, name);
}


void generate_blocks(FILE* dxf) {

    write_DXF_txt(dxf, 0, "SECTION");
    write_DXF_txt(dxf, 2, "BLOCKS");

    generate_block1(dxf, "US");
    write_circle(dxf, 0, 0, 0.6, "RED");
    write_DXF_txt(dxf, 0, "ENDBLK");


    generate_block1(dxf, "PR");
    write_circle(dxf, 0, 0, 0.3, "RED");
    write_circle(dxf, 0, 0, 0.6, "RED");
    write_DXF_txt(dxf, 0, "ENDBLK");

    write_DXF_txt(dxf, 0, "ENDSEC");
}
