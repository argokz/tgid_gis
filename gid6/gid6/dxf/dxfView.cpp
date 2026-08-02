// dxfView.cpp : implementation of the CDxfView class
//

#include "stdafx.h"
#include "dxfi.h"

#include <io.h>
#include <math.h>
#include <string.h>

int rgb_l(int color);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//static double p25400 = 1;
//static double p25400 = 10000;
//static double p25400 = 25400;
//static double p25400 = 5000;
//static double p25400 = -100000;
//static double p25400 = -25400;
//static double p25400 = -127000;
//static double p25400 = -125000;
//static double p25400 = -25400;


//static double p25400 = 100;


static double s_dx = 0;
static double s_dy = 0;

void set_dxf_dx_dy(double dx, double dy)
{
    s_dx = dx;
    s_dy = dy;
}


struct Mtext
{
    CString s;
    CString font;
    CString U;
    int i, b;
    int c, p;
};


char lower(char a)
{
    if ('A' <= a && a <= 'Z') {
        a = a - ('A' - 'a');
    }
    return a;
}


#include "token.h"
#include "dxfi.h"

unsigned long dxf_color(int c);

/*
void cross(CDC *pDC, int x, int y) {
//  pDC->SelectObject(TPen(TColor::LtRed, 1));

  CPen pen(PS_SOLID, 1, RGB(255, 0, 0)),
       *pOldPen = pDC->GetCurrentPen();

  pDC->SelectObject(&pen);
  pDC->MoveTo(x-10, y);
  pDC->LineTo(x+10, y);
  pDC->MoveTo(x, y-10);
  pDC->LineTo(x, y+10);
  pDC->SelectObject(pOldPen);
}
*/

struct Transf
{
    double dx, dy, x_scale, y_scale, angle;

    Transf() : dx(0), dy(0), x_scale(1), y_scale(1), angle(0)
    {};
    Transf(double _dx, double _dy, double _x_scale, double _y_scale, double _angle) :
        dx(_dx), dy(_dy), x_scale(_x_scale), y_scale(_y_scale), angle(_angle)
    {}
};

#define M_PI        3.14159265358979323846


void transf_coord(double x, double y, Transf transf, double& x2, double& y2)
{
    if (transf.angle != 0) {
        if (transf.x_scale < 0)  transf.angle = -transf.angle;


        double cosa = cos(transf.angle * M_PI / 180);
        double sina = sin(transf.angle * M_PI / 180);
        double x1 = cosa * x - sina * y;
        double y1 = sina * x + cosa * y;

        x = x1;
        y = y1;
    }

    x2 = x * transf.x_scale + transf.dx;
    y2 = y * transf.y_scale + transf.dy;
}


CPoint screen_coord(cdxf* dxf, double x, double y, Transf transf)
{
    //  return CPoint(x*dxf->p25400/masx-m_bx, -y*p25400/masy-m_by);

    double x2, y2;

    if (transf.dx == 0 && transf.dy == 0) {
        x -= s_dx;
        y -= s_dy;
    }

    transf_coord(x, y, transf, x2, y2);

    return CPoint(x2 * dxf->p25400 / dxf->masx - dxf->m_bx, -y2 * dxf->p25400 / dxf->masy - dxf->m_by);
}

int screen_width(cdxf* dxf, double w) {

    if (w <= 0) {
        w = dxf->tracewid;
        if (w <= 0) {
            w = dxf->plinewid;
        }
    }

    int r = fabs(w * dxf->p25400 / dxf->masx) + 0.5;

    //  int r = fabs(w*dxf->p25400/dxf->masx)+0.5;

    //  if (r > 10) {
    //    r = 10;
    //  }
    //  return r;

    return fabs(w * dxf->p25400 / dxf->masx) + 0.5;
}


bool utf8_check_is_valid(const char* str, int len)
{
    int n;
    for (int i = 0; i < len; ++i) {
        unsigned char c = (unsigned char)str[i];
        //if (c==0x09 || c==0x0a || c==0x0d || (0x20 <= c && c <= 0x7e) ) n = 0; // is_printable_ascii
        if (0x00 <= c && c <= 0x7f) {
            n = 0; // 0bbbbbbb
        }
        else if ((c & 0xE0) == 0xC0) {
            n = 1; // 110bbbbb
        }
        else if (c == 0xed && i < (len - 1) && ((unsigned char)str[i + 1] & 0xa0) == 0xa0) {
            return false; //U+d800 to U+dfff
        }
        else if ((c & 0xF0) == 0xE0) {
            n = 2; // 1110bbbb
        }
        else if ((c & 0xF8) == 0xF0) {
            n = 3; // 11110bbb
        //} else if (($c & 0xFC) == 0xF8) { n=4; // 111110bb //byte 5, unnecessary in 4 byte UTF-8
        //} else if (($c & 0xFE) == 0xFC) { n=5; // 1111110b //byte 6, unnecessary in 4 byte UTF-8
        }
        else {
            return false;
        }

        for (int j = 0; j < n && i < len; ++j) { // n bytes matching 10bbbbbb follow ?
            if ((++i == len) || (((unsigned char)str[i] & 0xC0) != 0x80)) {
                return false;
            }
        }
    }
    return true;
}


int to_ascii(cdxf* dxf, const char* s1, unsigned short* s)
{
    int i, j = 0, x;

    if (dxf->dwgcodepage == 3/* || dxf->dwgcodepage == 1*/ || utf8_check_is_valid(s1, strlen(s1)))
    {
        for (i = 0, j = 0; s1[i]; i++) {
            unsigned short a1, a2, a3;
            a1 = s1[i];
            a2 = s1[i + 1];
            a3 = s1[i + 2];

            if ((s1[i] & 0x80) == 0) {
                s[j++] = s1[i];
            }
            else if ((a1 & 0xE0) == 0xC0 && (a2 & 0xC0) == 0x80) {
                s[j++] = (a2 & 0x3F) + ((a1 & 0x1F) << 6);
            }
            else if ((a1 & 0xF) == 0xE && (a2 & 0xC0) == 0x80 && (a3 & 0xC0) == 0x80) {
                s[j++] = (a3 & 0x3F) + ((a2 & 0x3F) << 6) + ((a1 & 0xF) << 12);
            }
        }
        s[j] = 0;
    }
    else {
        MultiByteToWideChar(CP_ACP, 0, s1, strlen(s1), (LPWSTR)s, 1000);
        j = strlen(s1);
        s[j] = 0;
    }

    for (i = 0, j = 0; s[i] && j < 999; i++, j++) {
        s[j] = s[i];
        if (s[i] == L'\\' && (s[i + 1] == L'u' || s[i + 1] == L'U') && s[i + 2] == L'+') {
            unsigned short ss[5];
            wcsncpy((wchar_t*)ss, (wchar_t*)&s[i + 3], 4);
            ss[4] = 0;
            swscanf((wchar_t*)ss, L"%x", &x);
            s[j] = x;
            i += 6;
        }
    }
    s[j] = 0;

    return j;
}

void mtext_parser(char* s, Mtext& mtext);

void drawDxfanchor(generic* anchor, cdxf* dxf, CDC* pDC, CRect m_rect, double m_bx, double m_by, double masx, double masy, Transf transf);
block_type* find_block(char* s, cdxf* dxf);


void drawDxfEnt(generic* en, cdxf* dxf, CDC* pDC, CRect m_rect, double m_bx, double m_by, double masx, double masy, Transf transf)
{
    CPoint* p;
    int color;
    int n, j;
    CPen* oldpen = NULL;
    CBrush* oldbrush = NULL;
    LOGFONT lf;
    unsigned short us[1024];

    switch (en->ident) {
    case BLOCK:
    {
    }
    break;
    case INSERT:
    {
        insert_type* in = (insert_type*)en;

        block_type* bl = find_block(in->b, dxf);
        if (bl) {
            Transf tr2(in->ix + transf.dx, in->iy + transf.dy, in->x_scale * transf.x_scale, in->y_scale * transf.y_scale, in->angle + transf.angle);
            //            Transf tr2(in->ix + transf.dx, in->iy + transf.dy, in->x_scale * transf.x_scale, in->y_scale * transf.y_scale, 0 + transf.angle);

            //            if (!strcmp(bl->name, "êðàí")) {
            //                int qq;
            //                qq = 1;
            //            }
            //            if (strcmp(bl->name, "388(1)")) {
            //                break;
            //            }

            //            if (in->angle == 0) break;

            CPen pen(PS_SOLID, 0 * screen_width(dxf, 1), dxf_color(0));
            oldpen = pDC->SelectObject(&pen);

            CPoint p = screen_coord(dxf, in->ix, in->iy, transf);
            pDC->Ellipse(p.x - 1, p.y - 1, p.x + 1, p.y + 1);

            if (0) {
                CPoint p1 = screen_coord(dxf, bl->xmin, bl->ymin, tr2);
                CPoint p2 = screen_coord(dxf, bl->xmin, bl->ymax, tr2);
                CPoint p3 = screen_coord(dxf, bl->xmax, bl->ymax, tr2);
                CPoint p4 = screen_coord(dxf, bl->xmax, bl->ymin, tr2);

                pDC->MoveTo(p1.x, p1.y);
                pDC->LineTo(p2.x, p2.y);
                pDC->LineTo(p3.x, p3.y);
                pDC->LineTo(p4.x, p4.y);
                pDC->LineTo(p1.x, p1.y);
            }


            pDC->SelectObject(oldpen);


            drawDxfanchor(bl->en_anchor, dxf, pDC, m_rect, m_bx, m_by, masx, masy, tr2);
        }
    }
    break;



    case LWPOLYLINE:
    {
        if (transf.dx != 0) {
            int qq;
            qq = 1;
        }

        lwpline_type* pw = (lwpline_type*)en;

        color = pw->color > 0 ? pw->color : pw->lyr->color;

        n = pw->n;

        p = new CPoint[n + 1];

        for (j = 0; j < n; j++) p[j] = screen_coord(dxf, pw->x[j], pw->y[j], transf);
        if (pw->last_one) p[j++] = p[0];

        //      setline(dxf_color(color), screen_width(dxf, pw->w));
        //      if (j) pDC->Polyline(p, j);

        if (j) {
            CPen pen(PS_SOLID, 0 * screen_width(dxf, pw->w), dxf_color(color));
            oldpen = pDC->SelectObject(&pen);
            pDC->Polyline(p, j);
            pDC->SelectObject(oldpen);
        }

        delete[] p;
    }

    break;
    /*
        case SPLINE :
          {
            BYTE *types;

            spline_type *sp = (spline_type *) en;

            color = sp->color >= 0 ? sp->color : sp->lyr->color;

            n = sp->n;

            p = new CPoint[n+1];
            types = new BYTE[n+1];

            for (j = 0; j < n; j++) {
              p[j] = screen_coord(dxf,sp->x[j], sp->y[j]);
              types[j] = PT_BEZIERTO;
            }
            if (sp->last_one == 1) {
              types[j] = PT_BEZIERTO;
              p[j++] = p[0];
            }

            if (j) {
              CPen pen(PS_SOLID, 0*screen_width(dxf, sp->w), dxf_color(color));
              oldpen = pDC->SelectObject(&pen);
              pDC->Polyline(p, j);
              pDC->SelectObject(oldpen);
            }

            delete [] p;
            delete [] types;
          }
          break;
    */

    case SPLINE:
    {
        BYTE* types;

        spline_type* sp = (spline_type*)en;

        color = sp->color > 0 ? sp->color : sp->lyr->color;

        n = sp->n;

        p = new CPoint[n + 1];
        types = new BYTE[n + 1];

        for (j = 0; j < n; j++) {
            p[j] = screen_coord(dxf, sp->x[j], sp->y[j], transf);
            types[j] = PT_BEZIERTO;
        }
        if (sp->last_one == 1) {
            //          types[j] = PT_BEZIERTO;
            //          p[j++] = p[0];
        }

        if (j) {
            CPen pen(PS_SOLID, 0 * screen_width(dxf, 0), dxf_color(color));
            oldpen = pDC->SelectObject(&pen);
            //            pDC->Polyline(p, j);

            pDC->PolyBezier(p, j);

            pDC->SelectObject(oldpen);
        }

        delete[] p;
        delete[] types;
    }
    break;


    case HATCH:
    {
        hatch_type* ht = (hatch_type*)en;

        //        color = ht->lyr->color;
        color = ht->color > 0 ? ht->color : ht->lyr->color;

        n = ht->n;

        p = new CPoint[n + 1];

        for (j = 0; j < n; j++) {
            p[j] = screen_coord(dxf, ht->x[j], ht->y[j], transf);
        }
        if (ht->last_one) {
            p[j++] = p[0];
        }
        //      setline(dxf_color(color), 0);
        //      if (j) pDC->Polyline(p, j);

       /////////////!!!!!!!!!

       /*
              if (j) {
                CPen pen(PS_SOLID, 0*screen_width(dxf, 0), dxf_color(color));
                oldpen = pDC->SelectObject(&pen);
                pDC->Polyline(p, j);
                pDC->SelectObject(oldpen);
              }
      */

        if (j) {
            CPen pen(PS_SOLID, 0 * screen_width(dxf, 0), dxf_color(color));
            oldpen = pDC->SelectObject(&pen);
            pDC->Polyline(p, j);
            pDC->SelectObject(oldpen);


            /*
                      CBrush brush(dxf_color(color));
                      oldbrush = pDC->SelectObject(&brush);

                      pDC->Polygon(p, j);
                      pDC->SelectObject(oldbrush);
            */

        }





        delete[] p;
    }

    break;

    case POLYLINE:
    {
        line_type* pl = (line_type*)en;

        double w1 = pl->w1;
        color = pl->color;
        color = pl->color > 0 ? pl->color : pl->lyr->color;

        if (pl->vertex) {
            if (color < 0) {
                color = pl->vertex->color;
            }
            if (w1 < 0) {
                w1 = pl->vertex->w1;
            }
        }

        if (w1 <= 0) w1 = 0.5;

        vertex_type* v;

        for (n = 0, v = pl->vertex; v; v = v->next) n++;

        p = new CPoint[n + 1];

        //      if (pl->last_one == 128) {
        //        break;
        //      }
        if (pl->last_one == 64 || pl->last_one == 128) {

            int qq = 1;

            j = 0;
            for (v = pl->vertex; v; v = v->next) {
                if (v->x != 0 || v->y != 0) {
                    p[j++] = screen_coord(dxf, v->x, v->y, transf);
                }
            }
            for (v = pl->vertex; v; v = v->next) {
                CPoint pp[4];

                if (v->x == 0 && v->y == 0) {
                    pp[0] = p[j - abs(v->n74)];
                    pp[1] = p[j - abs(v->n71)];
                    pp[2] = p[j - abs(v->n72)];
                    pp[3] = p[j - abs(v->n73)];

                    //            setline(dxf_color(color), screen_width(dxf, pl->w1));
                    CPen pen(PS_SOLID, 0 * screen_width(dxf, w1), dxf_color(color));
                    oldpen = pDC->SelectObject(&pen);

                    if (v->n73 > 0) pDC->LineTo(pp[0].x, pp[0].y);
                    else pDC->MoveTo(pp[0].x, pp[0].y);

                    if (v->n74 > 0) pDC->LineTo(pp[1].x, pp[1].y);
                    else pDC->MoveTo(pp[1].x, pp[1].y);

                    if (v->n71 > 0) pDC->LineTo(pp[2].x, pp[2].y);
                    else pDC->MoveTo(pp[2].x, pp[2].y);

                    if (v->n72 > 0) pDC->LineTo(pp[3].x, pp[3].y);
                    else pDC->MoveTo(pp[3].x, pp[3].y);
                    pDC->SelectObject(oldpen);
                }
            }
        }
        else {
            j = 0;
            for (v = pl->vertex; v; v = v->next) {
                if (v->x != 0 || v->y != 0) {
                    p[j++] = screen_coord(dxf, v->x, v->y, transf);
                }
            }
            if (pl->last_one == 1) {
                p[j++] = p[0];
            }
            //        if (j) pDC->Polyline(p, j);
            if (j) {
                CPen pen(PS_SOLID, 0 * screen_width(dxf, w1), dxf_color(color));
                oldpen = pDC->SelectObject(&pen);
                pDC->Polyline(p, j);
                pDC->SelectObject(oldpen);
            }

        }

        delete[] p;
    }

    break;
    case SOLID:
    {
        CPoint p[5];
        solid_type* sl = (solid_type*)en;

        //        color = sl->color;
        color = sl->color > 0 ? sl->color : sl->lyr->color;
        p[0] = screen_coord(dxf, sl->x1, sl->y1, transf);
        p[1] = screen_coord(dxf, sl->x2, sl->y2, transf);
        p[2] = screen_coord(dxf, sl->x4, sl->y4, transf);
        p[3] = screen_coord(dxf, sl->x3, sl->y3, transf);
        p[4] = screen_coord(dxf, sl->x1, sl->y1, transf);
        layer_type* lyr = sl->lyr;
        //        setcolor(dxf_color(color));

        CPen pen(PS_SOLID, 0 * screen_width(dxf, 0), dxf_color(color));
        oldpen = pDC->SelectObject(&pen);
        pDC->Polyline(p, 5);
        pDC->SelectObject(oldpen);
    }
    break;

    case LINE:
    {
        line_type* lt = (line_type*)en;

        CPoint p[2];
        p[0] = screen_coord(dxf, lt->sx, lt->sy, transf);
        p[1] = screen_coord(dxf, lt->ex, lt->ey, transf);

        layer_type* lyr = lt->lyr;

        //        if (lyr) setcolor(dxf_color(lyr->color));
        //        pDC->Polyline(p, 2);

        if (lyr) {
            CPen pen(PS_SOLID, 0 * screen_width(dxf, 0), dxf_color(lyr->color));
            oldpen = pDC->SelectObject(&pen);
            pDC->Polyline(p, 2);
            pDC->SelectObject(oldpen);
        }

    }

    break;

    case ELLIPSE:
    {
        ellipse_type* el = (ellipse_type*)en;
        layer_type* lyr = el->lyr;
        //        color = el->color;
        color = el->color > 0 ? el->color : el->lyr->color;

        //        setline(dxf_color(color), screen_width(dxf, el->w));
        CPen pen(PS_SOLID, 0 * screen_width(dxf, el->w), dxf_color(color));
        oldpen = pDC->SelectObject(&pen);

        CPoint p1 = screen_coord(dxf, el->xmin, el->ymin, transf);
        CPoint p2 = screen_coord(dxf, el->xmax, el->ymax, transf);
        //        pDC->Ellipse(p1.x, p1.y, p2.x, p2.y);
        pDC->Arc(p1.x, p1.y, p2.x, p2.y, p1.x, p1.y, p1.x, p1.y);


        pDC->SelectObject(oldpen);
    }
    break;

    case CIRCLE:
    {

        circle_type* cl = (circle_type*)en;
        layer_type* lyr = cl->lyr;
        //        color = cl->color;
        color = cl->color > 0 ? cl->color : cl->lyr->color;

        CPen pen(PS_SOLID, 0 * screen_width(dxf, cl->w), dxf_color(color));
        oldpen = pDC->SelectObject(&pen);

        CPoint p1 = screen_coord(dxf, cl->xmin, cl->ymin, transf);
        CPoint p2 = screen_coord(dxf, cl->xmax, cl->ymax, transf);
        //        pDC->Ellipse(p1.x, p1.y, p2.x, p2.y);
        //        pDC->SelectObject(oldpen);

        pDC->Arc(p1.x, p1.y, p2.x, p2.y, p1.x, p1.y, p1.x, p1.y);
        pDC->SelectObject(oldpen);
    }
    break;


    case ARC:
    {
        arc_type* cl = (arc_type*)en;
        layer_type* lyr = cl->lyr;
        //        color = cl->color;
        color = cl->color > 0 ? cl->color : cl->lyr->color;

        CPen pen(PS_SOLID, 0 * screen_width(dxf, cl->w), dxf_color(color));
        oldpen = pDC->SelectObject(&pen);

        CPoint p = screen_coord(dxf, cl->x, cl->y, transf);


        CPoint p1 = screen_coord(dxf, cl->xmin, cl->ymin, transf);
        CPoint p2 = screen_coord(dxf, cl->xmax, cl->ymax, transf);
        //        pDC->Ellipse(p1.x, p1.y, p2.x, p2.y);
        //        pDC->SelectObject(oldpen);


        double a1 = cl->a1;
        double a2 = cl->a2;

        a1 += transf.angle;
        a2 += transf.angle;

        if (a2 < a1) a2 += 360;

        a2 -= a1;

        pDC->SetArcDirection(AD_CLOCKWISE);

        double r = cl->radius * dxf->p25400 / masx * transf.x_scale;

        double x1 = p.x + r * cos(a1 * M_PI / 180);
        double y1 = p.y - r * sin(a1 * M_PI / 180);

        pDC->MoveTo(x1, y1);
        pDC->AngleArc(p.x, p.y, r, a1, a2);

        //        pDC->Ellipse(x1 - 5, y1 - 5, x1 + 5, y1 + 5);
        //        pDC->Ellipse(p.x - 3, p.y - 3, p.x + 3, p.y + 3);

        pDC->SelectObject(oldpen);
    }
    break;



    case TEXT:
    {
        text_type* te = (text_type*)en;

        if (te->text) {
            if (te->lyr && te->lyr->color < 0) break;

            int h = screen_width(dxf, te->height);



            if (h < 2) break;

            CFont font;
            font.CreatePointFont(100, "Times New Roman");

            font.GetLogFont(&lf);

            if (te->font && !_stricmp(te->font, "Arial")) {
                strcpy(lf.lfFaceName, te->font);
            }

            if (te->font && !_stricmp(te->font, "Arial")) {
                strcpy(lf.lfFaceName, te->font);
            }
            else {
                strcpy(lf.lfFaceName, "Arial");
            }

            lf.lfHeight = h;

            lf.lfEscapement = te->angle * 10;

            if (dxf->p25400 < 0) lf.lfEscapement += 1800;

            lf.lfOrientation = lf.lfEscapement;


            switch (te->justify) {
            case 1:  pDC->SetTextAlign(TA_LEFT | TA_TOP); break;
            case 2:  pDC->SetTextAlign(TA_CENTER | TA_TOP); break;
            case 3:  pDC->SetTextAlign(TA_RIGHT | TA_TOP); break;
            case 4:  pDC->SetTextAlign(TA_LEFT | TA_BOTTOM); break;
            case 5:  pDC->SetTextAlign(TA_CENTER | TA_BOTTOM); break;
            case 6:  pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM); break;
            case 7:  pDC->SetTextAlign(TA_LEFT | TA_BOTTOM); break;
            case 8:  pDC->SetTextAlign(TA_CENTER | TA_BOTTOM); break;
            case 9:  pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM); break;
            }

            int nc = to_ascii(dxf, te->text, us);

            font.DeleteObject();
            font.CreateFontIndirect(&lf);
            CFont* oldfont = pDC->SelectObject(&font);

            pDC->SetBkMode(TRANSPARENT);
            pDC->SetTextAlign(TA_LEFT | TA_BASELINE);


            CPoint p1 = screen_coord(dxf, te->ix, te->iy, transf);
            ::TextOutW(pDC->m_hDC, p1.x, p1.y, (LPCWSTR)us, nc);
            //          pDC->TextOut(p1.x, p1.y, us);
            pDC->SelectObject(oldfont);
            //          pDC->Ellipse(p1.x-1, p1.y-1, p1.x+1, p1.y+1);
        }
    }
    break;

    case MTEXT:
    {
        mtext_type* mte = (mtext_type*)en;

        if (mte->text) {

            if (!strcmp(mte->text, "14")) {
                int qq = 1;
                qq = 1;
            }

            int h = screen_width(dxf, mte->height);

            if (h < 2) break;

            CFont font;
            font.CreatePointFont(100, "Times New Roman");

            font.GetLogFont(&lf);

            Mtext mtext;

            if (mte->font && !_stricmp(mte->font, "Arial")) {
                strcpy(lf.lfFaceName, mte->font);
                mtext.font = mte->font;
            }

            if (mte->font && !_stricmp(mte->font, "Arial")) {
                strcpy(lf.lfFaceName, mte->font);
                mtext.font = mte->font;
            }
            else {
                strcpy(lf.lfFaceName, "Arial");
                mtext.font = "Arial";
            }

            lf.lfHeight = h;

            //            double M_PI = 3.1415926;
            if (mte->angle == 0) {
                mte->angle = atan2(mte->ay, mte->ax) * 180 / M_PI;
            }

            lf.lfEscapement = mte->angle * 10;
            if (dxf->p25400 < 0) lf.lfEscapement += 1800;
            lf.lfOrientation = lf.lfEscapement;

            mtext_parser(mte->text, mtext);


            strcpy(lf.lfFaceName, mtext.font);
            lf.lfItalic = mtext.i;
            //          lf.lfWeight = 1;

            font.DeleteObject();
            font.CreateFontIndirect(&lf);
            CFont* oldfont = pDC->SelectObject(&font);

            pDC->SetBkMode(TRANSPARENT);
            pDC->SetTextAlign(TA_LEFT | TA_TOP);

            switch (mte->gen_flags) {
            case 1:  pDC->SetTextAlign(TA_LEFT | TA_TOP); break;
            case 2:  pDC->SetTextAlign(TA_CENTER | TA_TOP); break;
            case 3:  pDC->SetTextAlign(TA_RIGHT | TA_TOP); break;
            case 4:  pDC->SetTextAlign(TA_LEFT | TA_BOTTOM); break;
            case 5:  pDC->SetTextAlign(TA_CENTER | TA_BOTTOM); break;
            case 6:  pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM); break;
            case 7:  pDC->SetTextAlign(TA_LEFT | TA_BOTTOM); break;
            case 8:  pDC->SetTextAlign(TA_CENTER | TA_BOTTOM); break;
            case 9:  pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM); break;
            }

            CPoint p1 = screen_coord(dxf, mte->ix, mte->iy, transf);

            CString s = mtext.s;

            int ns = -1;

            CString s1, s2 = s;

            double dx = sin(mte->angle * M_PI / 180);
            double dy = cos(mte->angle * M_PI / 180);


            do {
                ns = s2.Find("\n");

                if (ns != -1) {
                    s1 = s2.Left(ns);
                    s2 = s2.Mid(ns + 1);
                }
                else {
                    s1 = s2;
                }

                int nc = to_ascii(dxf, s1, us);

                ::TextOutW(pDC->m_hDC, p1.x, p1.y, (LPCWSTR)us, nc);

                p1.x = p1.x + 1.73 * dx * h;
                p1.y = p1.y + 1.73 * dy * h;



            } while (ns > 1);

            pDC->SelectObject(oldfont);
        }
    }
    break;
    }
}



void drawDxfanchor(generic* anchor, cdxf* dxf, CDC* pDC, CRect m_rect, double m_bx, double m_by, double masx, double masy, Transf transf)
{
    generic* en;

    if (!dxf) return;

    pDC->SetTextColor(rgb_l(0));

    dxf->pDC = pDC, dxf->m_rect = m_rect, dxf->m_bx = m_bx, dxf->m_by = m_by, dxf->masx = masx, dxf->masy = masy;

    double sxmin1, sxmax1, symin1, symax1;
    double sxmin, sxmax, symin, symax;
    double xmin, xmax, ymin, ymax;

    sxmin1 = masx * (m_bx + m_rect.left) / dxf->p25400 + s_dx;
    sxmax1 = masx * (m_bx + m_rect.right) / dxf->p25400 + s_dx;
    symin1 = -masy * (m_by + m_rect.bottom) / dxf->p25400 + s_dy;
    symax1 = -masy * (m_by + m_rect.top) / dxf->p25400 + s_dy;

    sxmin = min(sxmin1, sxmax1);
    sxmax = max(sxmin1, sxmax1);
    symin = min(symin1, symax1);
    symax = max(symin1, symax1);

    int num = 0;

    //  for (en = dxf->en_anchor; en; en = en->next) {
    for (en = anchor; en; en = en->next) {
        if (en->ident == MTEXT) {
            int qq;
            qq = 1;
        }


        if (en->ident == INSERT) {
            insert_type* in = (insert_type*)en;
            block_type* bl = find_block(in->b, dxf);
            if (bl) {
                Transf tr2(in->ix + transf.dx, in->iy + transf.dy, in->x_scale * transf.x_scale, in->y_scale * transf.y_scale, in->angle + transf.angle);

                transf_coord(bl->xmin, bl->ymin, tr2, xmin, ymin);
                transf_coord(bl->xmax, bl->ymax, tr2, xmax, ymax);

                in->xmin = xmin;
                in->ymin = ymin;
                in->xmax = xmax;
                in->ymax = ymax;
            }
        }

        double xmin1 = min(dxf->extent_x, dxf->extent_mx);
        double ymin1 = min(dxf->extent_y, dxf->extent_my);
        double xmax1 = max(dxf->extent_x, dxf->extent_mx);
        double ymax1 = max(dxf->extent_y, dxf->extent_my);


        if (dxf->extent_x || dxf->extent_y || dxf->extent_x || dxf->extent_y) {
            if (en->xmin > xmax1) continue;
            if (en->ymin > ymax1) continue;
            if (en->xmax < xmin1) continue;
            if (en->ymax < ymin1) continue;
        }

        if (!en->lyr || !en->lyr->uze) {
            continue;
        }

        transf_coord(en->xmin, en->ymin, transf, xmin, ymin);
        transf_coord(en->xmax, en->ymax, transf, xmax, ymax);

        if (xmax < xmin) {
            double xx = xmax; xmax = xmin; xmin = xx;
        }
        if (ymax < ymin) {
            double yy = ymax; ymax = ymin; ymin = yy;
        }



        if (transf.dx == 0) {
            if (xmin <= xmax && ymin <= ymax) {
                if (xmin > sxmax || xmax < sxmin) {
                    continue;
                }
                if (ymin > symax || ymax < symin) {
                    continue;
                }
            }
            else {
            }
        }

        drawDxfEnt(en, dxf, pDC, m_rect, m_bx, m_by, masx, masy, transf);
    }
}



void drawDxf(cdxf* dxf, CDC* pDC, CRect m_rect, double m_bx, double m_by, double masx, double masy)
{
    if (!dxf) return;

    Transf tr1;

    drawDxfanchor(dxf->en_anchor, dxf, pDC, m_rect, m_bx, m_by, masx, masy, tr1);


    CPoint pt1 = screen_coord(dxf, dxf->extent_x, dxf->extent_y, tr1);
    CPoint pt2 = screen_coord(dxf, dxf->extent_mx, dxf->extent_my, tr1);

    CPen pen(PS_SOLID, 5, dxf_color(1));
    CPen* oldpen = pDC->SelectObject(&pen);

    pDC->MoveTo(pt1.x, pt1.y);
    pDC->LineTo(pt1.x, pt2.y);
    pDC->LineTo(pt2.x, pt2.y);
    pDC->LineTo(pt2.x, pt1.y);
    pDC->LineTo(pt1.x, pt1.y);
    pDC->SelectObject(oldpen);



    return;

    for (layout_type* lt = dxf->layout_anchor; lt; lt = lt->next) {

        dxf->pDC = pDC, dxf->m_rect = m_rect, dxf->m_bx = m_bx, dxf->m_by = m_by, dxf->masx = masx, dxf->masy = masy;

        CPoint pt1 = screen_coord(dxf, lt->xmin, lt->ymin, tr1);
        CPoint pt2 = screen_coord(dxf, lt->xmax, lt->ymax, tr1);

        CPen pen(PS_SOLID, 5, dxf_color(1));
        CPen* oldpen = pDC->SelectObject(&pen);

        pDC->MoveTo(pt1.x, pt1.y);
        pDC->LineTo(pt1.x, pt2.y);
        pDC->LineTo(pt2.x, pt2.y);
        pDC->LineTo(pt2.x, pt1.y);
        pDC->LineTo(pt1.x, pt1.y);

        pt1 = screen_coord(dxf, lt->x10, lt->y20, tr1);
        pt2 = screen_coord(dxf, lt->x11, lt->y21, tr1);

        CPen pen2(PS_SOLID, 5, dxf_color(2));
        pDC->SelectObject(&pen2);


        pDC->MoveTo(pt1.x, pt1.y);
        pDC->LineTo(pt1.x, pt2.y);
        pDC->LineTo(pt2.x, pt2.y);
        pDC->LineTo(pt2.x, pt1.y);
        pDC->LineTo(pt1.x, pt1.y);


        pDC->SelectObject(oldpen);


    }

}

/**
void mtext_parser(char *s, Mtext &mtext)
{
  char sss[1024], ff[256];
  int status = 0;
  int f0 = 0;
  char chf = ' ';
  int si = 0;

  sss[0] = 0;

  if (s[0] != '{') {
    status = 1;
  }

  for (int i = 0, k = 0; s[k]; k++) {
    if (status == 0) {
      if (s[k] == '{') {
        status = 1;
      }
    }
    else if (status == 1) {
      if (s[k] == '\\' ) {
        status = 2;
      }
      else if (s[k] == '}' ) {
        status = 0;
      }
      else if (s[k] == '%' && s[k+1] == '%') {

        char c = lower(s[k+2]);

        switch(c) {
          case 'c' : c = 'O'; break;
          case 'd' : c = '°'; break;
//          case 'd' :
//          case 'u' :
//          case 'o' :
          case 'p' : c = '±'; break;

// %%c   diameter character (O)
// %%d   degree character (°)
// %%u   toggles underline mode
// %%o   toggles overline mode
// %%k   toggles strikethrough mode (since 2013)
// %%p   tolerance character, plusminus (±, +-)
// %%%   percent character (escape
        }
        k++;
        k++;

        sss[si++] = c;
        sss[si] = 0;
      }
      else {
        sss[si++] = s[k];
        sss[si] = 0;
      }
    }
    else if (status == 2) {
//      if (s[k] == 'f' ) {
        status = 3;
        f0 = k+1;
        chf = lower(s[k]);
        chf = s[k];
        //      }
//      if (s[k] == 'p') {
//          sss[si++] = '\n';
//          sss[si] = 0;
//      }
    }
    else if (status == 3) {
      if (s[k] == ';' || s[k] == '|') {
        status = 2;

        strncpy(ff, &s[f0], k-f0); ff[k-f0] = 0;

        switch(chf) {
        case 'f' :
          mtext.font = ff;
          break;
        case 'b' :
          mtext.b = ff[0] == '1';
          break;
        case 'i' :
          mtext.i = ff[0] == '1';
          break;
        case 'c' :
          mtext.i = atoi(ff);
          break;
        case 'p' :
          mtext.p = atoi(ff);
          break;
        case 'h' :
//          mtext.p = atoi(ff);

          break;
        case 'u' :
          mtext.U = ff;
          break;

        }
      }
      if (s[k] == ';') {
        status = 1;
      }
      if (s[k] == '|') {
        f0 = k+1;
      }
    }
  }
  mtext.s = sss;
}

*/



void mtext_parser(char* s, Mtext& mtext)
{
    char sss[1024], ff[256];
    int status = 0;
    int f0 = 0;
    char chf = ' ';
    int si = 0;

    sss[0] = 0;

    if (s[0] != '{') {
        status = 1;
    }

    for (int i = 0, k = 0; s[k]; k++) {
        if (status == 0) {
            if (s[k] == '{') {
                status = 1;
            }
        }
        else if (status == 1) {
            if (s[k] == '\\') {
                status = 2;
            }
            else if (s[k] == '}') {
                status = 0;
            }
            else if (s[k] == '%' && s[k + 1] == '%') {

                char c = lower(s[k + 2]);

                switch (c) {
                case 'c': c = 'O'; break;
                case 'd': c = '°'; break;
                    //          case 'd' :
                    //          case 'u' :
                    //          case 'o' :
                case 'p': c = '±'; break;

                    // %%c   diameter character (O)
                    // %%d   degree character (°)
                    // %%u   toggles underline mode
                    // %%o   toggles overline mode
                    // %%k   toggles strikethrough mode (since 2013)
                    // %%p   tolerance character, plusminus (±, +-)
                    // %%%   percent character (escape
                }
                k++;
                k++;

                sss[si++] = c;
                sss[si] = 0;
            }
            else {
                sss[si++] = s[k];
                sss[si] = 0;
            }
        }
        else if (status == 2) {
            if (s[k] == 'f') {
                status = 3;
            }
            else if (s[k] == 'P') {
                status = 1;
                sss[si++] = '\n';
                sss[si] = 0;
            }
            else {
                status = 4;
            }
            f0 = k + 1;
            chf = s[k];
        }
        else if (status == 5) {
            status = 3;
            f0 = k + 1;
            chf = s[k];
        }
        else if (status == 4) { // 
            if (s[k] == ';' || s[k] == '}') {
                status = 2;
                strncpy(ff, &s[f0], k - f0); ff[k - f0] = 0;

                switch (chf) {
                case 'C':
                    break;
                }
                status = 1;
            }
        }
        else if (status == 3) { // Font
            if (s[k] == ';' || s[k] == '|') {
                status = 5;

                strncpy(ff, &s[f0], k - f0); ff[k - f0] = 0;

                switch (chf) {
                case 'f':
                    mtext.font = ff;
                    break;
                case 'b':
                    mtext.b = ff[0] == '1';
                    break;
                case 'i':
                    mtext.i = ff[0] == '1';
                    break;
                case 'c':
                    mtext.i = atoi(ff);
                    break;
                case 'p':
                    mtext.p = atoi(ff);
                    break;
                }
            }
            if (s[k] == ';') {
                status = 1;
            }
            f0 = k + 1;
        }
    }
    mtext.s = sss;
}
