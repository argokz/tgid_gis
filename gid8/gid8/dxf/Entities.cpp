#include <QtGui>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Token.h"
#include "dxfi.h"

static int qqq = 0;

block_type* block_parser(FILE* f, char* lyr, cdxf* dxf);
generic* parser(FILE* f, char* lyr, cdxf* dxf);

// Pg. 156
void entities(FILE* f, block_type* blk1, cdxf* dxf, int is_blk)
{
    int state;
    int quit;
    char* c;
    int t;
    char* lyr;
    generic* en;
    line_type* pl = NULL;

    int flags = 0;

    pl = NULL;
    quit = FALSE;
    state = 5;
    lyr = NULL;
    QString str;


    double x = 0, y = 0;

    int qq = 0;
    int iii = 0;

    if (!is_blk) {
        int i;
        i = 0;
    }


    block_type* blk = nullptr;

    block_type* block(FILE * f, cdxf * dxf);

    if (is_blk) {
        //      blk =  block_parser(f, lyr, dxf);
        blk = block(f, dxf);
        state = 3;

        blk->xmin = 1e20;
        blk->ymin = 1e20;
        blk->xmax = -1e20;
        blk->ymax = -1e20;



    }


    while (!quit)
    {
        t = get_tag_token(c = get_tag(f));

        qq++;

        if (t == ENDSEC) {
            t = t;
        }
        if (t == EOFILE) {
            break;
        }

        switch (state)
        {
        case 1:

            str = QString("1 %1").arg(c);
            //if (qqq > 56) AfxMessageBox(str);
            state = 5;
            break;
        case 5:
            str = QString("5 %1").arg(c);
            //if (qqq > 56)       AfxMessageBox(str);
            if (t == N_0) state = 3;
            else  state = 1;
            if (t == N_10) state = 510;
            if (t == N_20) state = 520;
            if (t == N_8) state = 2;
            if (t == N_70 && is_blk) state = 570;
            if (t == N_5 && is_blk) state = 505;
            break;

        case 510:
            state = 5;
            x = atof(c);
            break;
        case 520:
            state = 5;
            y = atof(c);
            break;
        case 570:
            state = 5;
            flags = atoi(c);
            //      if (flags > 2) AfxMessageBox(c);
            break;

        case 505:
            state = 5;
            //      AfxMessageBox(c);
            break;

        case 2:
            state = 5;
            lyr = locate(c);


            break;
        case 3:
            iii++;
            en = NULL;
            switch (t)
            {
            case ENDSEC:
            case ENDBLK:
                quit = TRUE;
                break;

            case LINE:
                en = (generic*) line_parser(f, lyr, dxf);
                break;

            case POINT1:
                en = (generic*) point_parser(f, lyr, dxf);
                break;
            case CIRCLE:
                en = (generic*) circle_parser(f, lyr, dxf);
                break;
            case ELLIPSE:
                en = (generic*) ellipse_parser(f, lyr, dxf);
                break;
            case ARC:
                en = (generic*) arc_parser(f, lyr, dxf);
                break;
            case TRACE1:
                en = (generic*) trace_parser(f, lyr);
                break;
            case SOLID:
                en = (generic*) solid_parser(f, lyr, dxf);
                break;
            case TEXT:
                en = (generic*) text_parser(f, lyr, dxf);
                break;
            case SHAPE:
                en = (generic*) shape_parser(f, lyr);
                break;
            case INSERT:
                en = (generic*) insert_parser(f, lyr, dxf);
                break;
            case DIMENSION:
                en = (generic*) dimension_parser(f, lyr);
                break;
            case ATTDEF:
                en = (generic*) attdef_parser(f, lyr);
                break;
            case ATTRIB:
                en = (generic*) attrib_parser(f, lyr);
                break;
            case POLYLINE:
                pl = pline_parser(f, lyr, dxf);
                break;
            case LWPOLYLINE:
                en = lwpline_parser(f, lyr, dxf);
                break;
            case SPLINE:
                en = spline_parser(f, lyr, dxf);
                break;
            case HATCH:
                en = (generic*) hatch_parser(f, lyr, dxf);
                //	      en = NULL;
                break;
            case VERTEX:
                if (pl) {
                    vertex_parser(f, pl, lyr, dxf);
                    en = NULL;
                }
                else {
                    en = (generic*) parser(f, lyr, dxf);
                }
                break;
            case SEQEND:
                en = (generic*) parser(f, lyr, dxf);

                if (pl) {
                    en = (generic*) pl;
                    pl = NULL;
                }
                break;
            case MTEXT:
                en = (generic*) mtext_parser(f, lyr, dxf);
                break;
            case SCALE:
            case VIEWPORT:
            case DICTIONARY:
            case XRECORD:
            case _3DFACE:
            case REGION:
            case WIPEOUT:
            case RAY:
            case IMAGE:
            case ACAD_PROXY_OBJECT:
                en = (generic*) parser(f, lyr, dxf);
                break;

            default:
                en = (generic*) parser(f, lyr, dxf);
                break;
            }
            if (en) {
                if (en == dxf->en_anchor) {
//                    AfxMessageBox("Ужасная ошибка!");

                }

                if (en->lyr && en->lyr->color >= 0 && (1 
//                            strcmp(en->lyr->name, "Дороги Улицы")
//                         en->ident == INSERT || is_blk
                         )) {
                    if (!is_blk) {
                        en->next = dxf->en_anchor;
                        dxf->en_anchor = en;

                        if (en->ident != INSERT && 0) {

                            if (en->ymin < dxf->l_ymin) {
                                int qq;
                                qq = 1;
                            }
                            if (en->ymax > dxf->l_ymax) {
                                int qq;
                                qq = 1;
                            }
                            if (en->xmin < dxf->l_xmin) {
                                int qq;
                                qq = 1;
                            }
                            if (en->xmax > dxf->l_xmax) {
                                int qq;
                                qq = 1;
                            }

//                        if (en->ident != INSERT) {
                            dxf->l_xmin = fmin(dxf->l_xmin, en->xmin);
                            dxf->l_xmax = fmax(dxf->l_xmax, en->xmax);
                            dxf->l_ymin = fmin(dxf->l_ymin, en->ymin);
                            dxf->l_ymax = fmax(dxf->l_ymax, en->ymax);
//                        }
                        }
                    }
                    else {
                        en->next = blk->en_anchor;
                        blk->en_anchor = en;

                        blk->xmin = fmin(blk->xmin, en->xmin);
                        blk->xmax = fmax(blk->xmax, en->xmax);
                        blk->ymin = fmin(blk->ymin, en->ymin);
                        blk->ymax = fmax(blk->ymax, en->ymax);
                    }
                }
            }
            break;
        }
    }

}
