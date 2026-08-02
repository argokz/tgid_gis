#include "stdafx.h"

#include <stdio.h>
#include <string.h>
#include "dxf.h"



void write_DXF_hex(FILE* dxf, int tag, int v);
void write_DXF_int(FILE* dxf, int tag, int v);
void write_DXF_double(FILE* dxf, int tag, double v);
//void write_DXF(FILE* dxf, int tag, const void* value);
void write_DXF_txt(FILE* dxf, int tag, const void* value);


int handle = 1;
static int h_MS = 0;
static int h_PS = 0;
static int h_BLOCK_RECORD = 0;

int get_h_MS() 
{
    return h_MS;
}

int get_h_BLOCK_RECORD()
{
    return h_BLOCK_RECORD;
}


static int handle_parent = 1;

void init_handle()
{
    handle = 1;
}

/*

int write_5_330(FILE *dxf, int handle_parent)
{
    int h = handle;
    write_DXF_hex(dxf, 5, handle++);
//    write_DXF_hex(dxf, 330, handle_parent);
    write_DXF_hex(dxf, 330, 0);
    return h;
}

int write_105_330(FILE *dxf, int handle_parent)
{
    int h = handle;
    write_DXF_hex(dxf, 105, handle++);
//    write_DXF_hex(dxf, 330, handle_parent);
    write_DXF_hex(dxf, 330, 0);
    return h;
}

*/

/*

int write_5_330(FILE *dxf, int handle_parent)
{
    int h = handle;
    write_DXF_hex(dxf, 5, handle++);
//    write_DXF_hex(dxf, 330, handle_parent);
    write_DXF_hex(dxf, 330, 0);
    return h;
}

int write_105_330(FILE *dxf, int handle_parent)
{
    int h = handle;
    write_DXF_hex(dxf, 105, handle++);
//    write_DXF_hex(dxf, 330, handle_parent);
    write_DXF_hex(dxf, 330, 0);
    return h;
}
*/


int write_5_330(FILE *dxf, int handle_parent)
{
//    if (handle_parent == 0) handle_parent = get_h_MS();

    int h = handle;
    write_DXF_hex(dxf, 5, handle++);
    write_DXF_hex(dxf, 330, handle_parent);
    return h;
}

int write_105_330(FILE *dxf, int handle_parent)
{
//    if (handle_parent == 0) handle_parent = get_h_MS();
    
    int h = handle;
    write_DXF_hex(dxf, 105, handle++);
    write_DXF_hex(dxf, 330, handle_parent);
    return h;
}



void generate_header2(FILE* dxf, double x1, double y1, double x2, double y2) {
    double limminx, limminy, limmaxx, limmaxy;

    limminx = x1 / 100.;  limmaxx = x2 / 100.;
    limminy = y1 / 100.;  limmaxy = y2 / 100.;

    limminx = x1;  limmaxx = x2;
    limminy = y1;  limmaxy = y2;


    write_DXF_txt(dxf, 0, "SECTION");
    write_DXF_txt(dxf, 2, "HEADER");

//    write_DXF(dxf, 9, "$ACADVER");  write_DXF(dxf, 1, "AC1021");
    write_DXF_txt(dxf, 9, "$ACADVER");  
    write_DXF_txt(dxf, 1, "AC1015");

    write_DXF_txt(dxf, 9, "$DWGCODEPAGE");  
    write_DXF_txt(dxf, 3, "ANSI_1251");

    write_DXF_txt(dxf, 9, "$HANDSEED");
    write_DXF_hex(dxf, 5, 200000000);


    write_DXF_txt(dxf, 9, "$EXTMIN");  write_DXF(dxf, 10, &limminx);  write_DXF(dxf, 20, &limminy);
    write_DXF_txt(dxf, 9, "$EXTMAX");  write_DXF(dxf, 10, &limmaxx);  write_DXF(dxf, 20, &limmaxy);


    write_DXF_txt(dxf, 9, "$LIMMIN");  write_DXF(dxf, 10, &limminx);  write_DXF(dxf, 20, &limminy);
    write_DXF_txt(dxf, 9, "$LIMMAX");  write_DXF(dxf, 10, &limmaxx);  write_DXF(dxf, 20, &limmaxy);


    int zero = 6;

    write_DXF_txt(dxf, 9, "$INSUNITS");  write_DXF(dxf, 70, &zero);


    write_DXF_txt(dxf, 0, "ENDSEC");

    write_DXF_txt(dxf, 0, "SECTION");
    write_DXF_txt(dxf, 2, "CLASSES");
    write_DXF_txt(dxf, 0, "ENDSEC");
}

void write_layer(FILE *dxf, const char *layer, int color, int handle_parent)
{
    write_DXF_txt(dxf, 0, "LAYER");
//    write_DXF_hex(dxf, 5, handle++);
//    write_DXF_hex(dxf, 330, handle_parent);
    write_5_330(dxf, handle_parent);

    write_DXF_txt(dxf, 100, "AcDbSymbolTableRecord");
    write_DXF_txt(dxf, 100, "AcDbLayerTableRecord");
    write_DXF(dxf, 2, layer);
    write_DXF_int(dxf, 70, 0);
    write_DXF_int(dxf, 62, color);
    write_DXF_txt(dxf, 6, "CONTINUOUS");
//290
    if (!strcmp(layer, "Defpoints")) {
        write_DXF_int(dxf, 290, 0);
    }

    write_DXF_txt(dxf, 370, "-3");
    write_DXF_txt(dxf, 390, "ByLayer");
}

int write_block_record(FILE *dxf, const char *name, int handle_parent)
{
    write_DXF_txt(dxf, 0, "BLOCK_RECORD");

    int h = write_5_330(dxf, handle_parent);

    write_DXF_txt(dxf, 100, "AcDbSymbolTableRecord");
    write_DXF_txt(dxf, 100, "AcDbBlockTableRecord");
    write_DXF(dxf, 2, name);
    write_DXF_int(dxf, 70, 0);
    write_DXF_int(dxf, 280, 1);
    write_DXF_int(dxf, 281, 0);

    return h;
}

int write_block_head(FILE *dxf, const char *name, int h)
{
    write_DXF_txt(dxf, 0, "BLOCK");
    int h2 = write_5_330(dxf, h);

    write_DXF_txt(dxf, 100, "AcDbEntity");
    write_DXF_int(dxf, 8, 0);
    write_DXF_int(dxf, 62, 1);
    write_DXF_txt(dxf, 100, "AcDbBlockBegin");
    write_DXF(dxf, 2, name);
    write_DXF_int(dxf, 70, 0);
    write_DXF_int(dxf, 10, 0);
    write_DXF_int(dxf, 20, 0);
    write_DXF_int(dxf, 30, 0);
    write_DXF_txt(dxf, 3, name);
    write_DXF_txt(dxf, 1, "");
    return h2;
}


void write_block_tail(FILE *dxf, int h)
{
    write_DXF_txt(dxf, 0, "ENDBLK");
    write_5_330(dxf, h);

    write_DXF_txt(dxf, 100, "AcDbEntity");
    write_DXF_txt(dxf, 100, "AcDbBlockEnd");
}

void write_vport(FILE *dxf)
{
    write_DXF_txt(dxf, 0, "TABLE");
    write_DXF_txt(dxf, 2, "VPORT");

    
//    write_5_330(dxf, 0);
    int hv1 = handle;

    write_DXF_hex(dxf, 5, handle++);
    write_DXF_hex(dxf, 330, 0);


    write_DXF_txt(dxf, 100, "AcDbSymbolTable");
    write_DXF_int(dxf, 70, 1);
    //------------
    write_DXF_txt(dxf, 0, "VPORT");

    
//    write_5_330(dxf, 2);

    write_DXF_hex(dxf, 5, handle++);
    write_DXF_hex(dxf, 330, hv1);


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

void write_dimstyle(FILE *dxf)
{
    write_DXF_txt(dxf, 0, "TABLE");
    write_DXF_txt(dxf, 2, "DIMSTYLE");

    handle_parent = write_5_330(dxf, 0);

    write_DXF_txt(dxf, 100, "AcDbSymbolTable");
    write_DXF_int(dxf, 70, 1);
    write_DXF_txt(dxf, 100, "AcDbDimStyleTable");
    write_DXF_int(dxf, 71, 1);
    //------------
    write_DXF_txt(dxf, 0, "DIMSTYLE");
//    write_DXF_int(dxf, 105, 46);
//    write_DXF_hex(dxf, 330, handle_parent);
    write_105_330(dxf, handle_parent);

    write_DXF_txt(dxf, 100, "AcDbSymbolTableRecord");
    write_DXF_txt(dxf, 100, "AcDbDimStyleTableRecord");
    write_DXF_txt(dxf, 2, "Standard");
    write_DXF_int(dxf, 70, 0);
    write_DXF_int(dxf, 40, 1);
    write_DXF_double(dxf, 41, 0.18);
    write_DXF_double(dxf, 42, 0.0625);
    write_DXF_double(dxf, 43, 0.38);
    write_DXF_double(dxf, 44, 0.18);
    write_DXF_int(dxf, 45, 0);
    write_DXF_int(dxf, 46, 0);
    write_DXF_int(dxf, 47, 0);
    write_DXF_int(dxf, 48, 0);
    write_DXF_double(dxf, 140, 0.18);
    write_DXF_double(dxf, 141, 0.09);
    write_DXF_int(dxf, 142, 0);
    write_DXF_double(dxf, 143, 25.4);
    write_DXF_int(dxf, 144, 1);
    write_DXF_int(dxf, 145, 0);
    write_DXF_int(dxf, 146, 1);
    write_DXF_double(dxf, 147, 0.09);
    write_DXF_int(dxf, 148, 0);
    write_DXF_int(dxf, 71, 0);
    write_DXF_int(dxf, 72, 0);
    write_DXF_int(dxf, 73, 0);
    write_DXF_int(dxf, 74, 1);
    write_DXF_int(dxf, 75, 0);
    write_DXF_int(dxf, 76, 0);
    write_DXF_int(dxf, 77, 0);
    write_DXF_int(dxf, 78, 0);
    write_DXF_int(dxf, 79, 0);
    write_DXF_int(dxf, 170, 0);
    write_DXF_int(dxf, 171, 2);
    write_DXF_int(dxf, 172, 0);
    write_DXF_int(dxf, 173, 0);
    write_DXF_int(dxf, 174, 0);
    write_DXF_int(dxf, 175, 0);
    write_DXF_int(dxf, 176, 0);
    write_DXF_int(dxf, 177, 0);
    write_DXF_int(dxf, 178, 0);
    write_DXF_int(dxf, 179, 0);
    write_DXF_int(dxf, 271, 4);
    write_DXF_int(dxf, 272, 4);
    write_DXF_int(dxf, 273, 2);
    write_DXF_int(dxf, 274, 2);
    write_DXF_int(dxf, 275, 0);
    write_DXF_int(dxf, 276, 0);
    write_DXF_int(dxf, 277, 2);
    write_DXF_int(dxf, 278, 46);
    write_DXF_int(dxf, 279, 0);
    write_DXF_int(dxf, 280, 0);
    write_DXF_int(dxf, 281, 0);
    write_DXF_int(dxf, 282, 0);
    write_DXF_int(dxf, 283, 1);
    write_DXF_int(dxf, 284, 0);
    write_DXF_int(dxf, 285, 0);
    write_DXF_int(dxf, 286, 0);
    write_DXF_int(dxf, 288, 0);
    write_DXF_int(dxf, 289, 3);
    write_DXF_txt(dxf, 340, "Standard");
    write_DXF_txt(dxf, 341, "");
    write_DXF_txt(dxf, 371, "-2");
    write_DXF_txt(dxf, 372, "-2");
    write_DXF_txt(dxf, 0, "ENDTAB");
}



void write_minimun(FILE* dxf, double x1, double y1, double x2, double y2, int n_records)
{
    init_handle();

    generate_header2(dxf, x1, y1, x2, y2);

    write_DXF_txt(dxf, 0, "SECTION");
    write_DXF_txt(dxf, 2, "TABLES");
    //------------

    write_vport(dxf);

    //------------
    write_DXF_txt(dxf, 0, "TABLE");
    write_DXF_txt(dxf, 2, "LTYPE");

    handle_parent = write_5_330(dxf, 0);


    write_DXF_txt(dxf, 100, "AcDbSymbolTable");
    write_DXF_int(dxf, 70, 4);
    //------------
    write_DXF_txt(dxf, 0, "LTYPE");
    write_5_330(dxf, handle_parent);
    write_DXF_txt(dxf, 100, "AcDbSymbolTableRecord");
    write_DXF_txt(dxf, 100, "AcDbLinetypeTableRecord");
    write_DXF_txt(dxf, 2, "ByBlock");
    write_DXF_int(dxf, 70, 0);
    write_DXF_txt(dxf, 3, "");
    write_DXF_int(dxf, 72, 65);
    write_DXF_int(dxf, 73, 0);
    write_DXF_int(dxf, 40, 0);
    //------------

    write_DXF_txt(dxf, 0, "LTYPE");
    write_5_330(dxf, handle_parent);
    write_DXF_txt(dxf, 100, "AcDbSymbolTableRecord");
    write_DXF_txt(dxf, 100, "AcDbLinetypeTableRecord");
    write_DXF_txt(dxf, 2, "ByLayer");
    write_DXF_int(dxf, 70, 0);
    write_DXF_txt(dxf, 3, "");
    write_DXF_int(dxf, 72, 65);
    write_DXF_int(dxf, 73, 0);
    write_DXF_int(dxf, 40, 0);
    //------------

    write_DXF_txt(dxf, 0, "LTYPE");
    write_5_330(dxf, handle_parent);
    write_DXF_txt(dxf, 100, "AcDbSymbolTableRecord");
    write_DXF_txt(dxf, 100, "AcDbLinetypeTableRecord");
    write_DXF_txt(dxf, 2, "Continuous");
    write_DXF_int(dxf, 70, 0);
    write_DXF_txt(dxf, 3, "Solid line");
    write_DXF_int(dxf, 72, 65);
    write_DXF_int(dxf, 73, 0);
    write_DXF_int(dxf, 40, 0);
    write_DXF_txt(dxf, 0, "ENDTAB");
    //------------

    write_DXF_txt(dxf, 0, "TABLE");
    write_DXF_txt(dxf, 2, "LAYER");
    handle_parent = write_5_330(dxf, 0);
    write_DXF_txt(dxf, 100, "AcDbSymbolTable");

    write_DXF_int(dxf, 70, 5);
    //------------

    write_layer(dxf, "0", 7, handle_parent);
    write_layer(dxf, "BLACK", 255, handle_parent);
    write_layer(dxf, "RED", 12, handle_parent);
    write_layer(dxf, "BLUE", 16, handle_parent);

    write_layer(dxf, "Defpoints", 7, handle_parent);

    write_DXF_txt(dxf, 0, "ENDTAB");
    //------------


/////////////////////////////////////////////////

    //------------
    write_DXF_txt(dxf, 0, "TABLE");
    write_DXF_txt(dxf, 2, "STYLE");
    int hst = write_5_330(dxf, 0);


    write_DXF_txt(dxf, 100, "AcDbSymbolTable");
    write_DXF_int(dxf, 70, 1);
    //------------
    write_DXF_txt(dxf, 0, "STYLE");
    write_5_330(dxf, hst);


    write_DXF_txt(dxf, 100, "AcDbSymbolTableRecord");
    write_DXF_txt(dxf, 100, "AcDbTextStyleTableRecord");
    write_DXF_txt(dxf, 2, "Standard");
    write_DXF_int(dxf, 70, 0);
    write_DXF_int(dxf, 40, 0);
    write_DXF_int(dxf, 41, 1);
    write_DXF_int(dxf, 50, 0);
    write_DXF_int(dxf, 71, 0);
    write_DXF_int(dxf, 42, 1);
    write_DXF_txt(dxf, 3, "txt");
    write_DXF_txt(dxf, 4, "");
    write_DXF_txt(dxf, 0, "ENDTAB");
    //------------
    write_DXF_txt(dxf, 0, "TABLE");
    write_DXF_txt(dxf, 2, "VIEW");
    write_5_330(dxf, 0);

    write_DXF_txt(dxf, 100, "AcDbSymbolTable");
    write_DXF_int(dxf, 70, 0);
    write_DXF_txt(dxf, 0, "ENDTAB");
    //------------
    write_DXF_txt(dxf, 0, "TABLE");
    write_DXF_txt(dxf, 2, "UCS");
    write_5_330(dxf, 0);


    write_DXF_txt(dxf, 100, "AcDbSymbolTable");
    write_DXF_int(dxf, 70, 0);
    write_DXF_txt(dxf, 0, "ENDTAB");
    //------------


    write_DXF_txt(dxf, 0, "TABLE");
    write_DXF_txt(dxf, 2, "APPID");

    handle_parent = write_5_330(dxf, 0);

    write_DXF_txt(dxf, 100, "AcDbSymbolTable");
    write_DXF_int(dxf, 70, 1);
    //------------
    write_DXF_txt(dxf, 0, "APPID");
    write_5_330(dxf, handle_parent);

    write_DXF_txt(dxf, 100, "AcDbSymbolTableRecord");
    write_DXF_txt(dxf, 100, "AcDbRegAppTableRecord");
    write_DXF_txt(dxf, 2, "ACAD");
    write_DXF_int(dxf, 70, 0);
    write_DXF_txt(dxf, 0, "ENDTAB");

    //------------

    write_dimstyle(dxf);

    //------------

#if 1
    write_DXF_txt(dxf, 0, "TABLE");
    write_DXF_txt(dxf, 2, "BLOCK_RECORD");


    h_BLOCK_RECORD = write_5_330(dxf, 0);

    write_DXF_txt(dxf, 100, "AcDbSymbolTable");
    write_DXF_int(dxf, 70, 2+n_records);
    //------------


    h_MS = write_block_record(dxf, "*Model_Space", h_BLOCK_RECORD);
    h_PS = write_block_record(dxf, "*Paper_Space", h_BLOCK_RECORD);
#endif

}


void write_minimun_2(FILE* dxf)
{
    //-------------------------------


    write_DXF_txt(dxf, 0, "ENDTAB");

    write_DXF_txt(dxf, 0, "ENDSEC");
    //----------------------------------------

#if 1
    
    
    write_DXF_txt(dxf, 0, "SECTION");
    write_DXF_txt(dxf, 2, "BLOCKS");
    //------------


    write_block_head(dxf, "*Model_Space", h_MS);
    write_DXF_txt(dxf, 1, "");
    write_block_tail(dxf, h_MS);

    write_block_head(dxf, "*Paper_Space", h_PS);
    write_DXF_txt(dxf, 1, "");
    write_block_tail(dxf, h_PS);
#endif

    /////////////
}

void write_minimun_3(FILE* dxf)
{
    write_DXF_txt(dxf, 0, "ENDSEC");
    //----------------------------------------
    write_DXF_txt(dxf, 0, "SECTION");
    write_DXF_txt(dxf, 2, "ENTITIES");
}



void write_minimun_tail(FILE *dxf)
{
/*
    write_DXF_txt(dxf, 0, "ENDSEC");
    //----------------------------------------
    write_DXF_txt(dxf, 0, "SECTION");
    write_DXF_txt(dxf, 2, "OBJECTS");
    write_DXF_txt(dxf, 0, "DICTIONARY");
    handle_parent = write_5_330(dxf, 0);

    write_DXF_txt(dxf, 100, "AcDbDictionary");
    write_DXF_int(dxf, 281, 1);
    write_DXF_txt(dxf, 3, "ACAD_GROUP");
    write_DXF_hex(dxf, 350, 0xD);
    write_DXF_txt(dxf, 0, "DICTIONARY");
    write_5_330(dxf, handle_parent);


    write_DXF_txt(dxf, 100, "AcDbDictionary");
    write_DXF_int(dxf, 281, 1);
    write_DXF_txt(dxf, 0, "ENDSEC");
    write_DXF_txt(dxf, 0, "EOF");
*/

    write_DXF_txt(dxf, 0, "ENDSEC");
    //----------------------------------------
    write_DXF_txt(dxf, 0, "SECTION");
    write_DXF_txt(dxf, 2, "OBJECTS");
    write_DXF_txt(dxf, 0, "DICTIONARY");
    handle_parent = write_5_330(dxf, 0);

    write_DXF_txt(dxf, 100, "AcDbDictionary");
    write_DXF_txt(dxf, 3, "ACAD_GROUP");
    write_DXF_hex(dxf, 350, handle);

    write_DXF_int(dxf, 281, 1);


    write_DXF_txt(dxf, 0, "DICTIONARY");

    handle_parent = write_5_330(dxf, handle_parent);

    write_DXF_txt(dxf, 100, "AcDbDictionary");
    write_DXF_int(dxf, 281, 1);
 
    write_DXF_txt(dxf, 0, "ENDSEC");
    write_DXF_txt(dxf, 0, "EOF");
}
