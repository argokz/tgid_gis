#include "stdafx.h"
#include "gid6.h"
#include "Cxema.h"
#include "gidrview.h"


#include "dxf/DXF.H"

int get_h_MS();
int get_h_BLOCK_RECORD();

void write_minimun(FILE* dxf, double x1, double y1, double x2, double y2, int n_records);
void write_minimun_2(FILE* dxf);
void write_minimun_3(FILE* dxf);
void write_minimun_tail(FILE* dxf);
int write_block_record(FILE* dxf, const char* name, int handle_parent);
int write_block_head(FILE* dxf, const char* name, int h);
void write_block_tail(FILE* dxf, int h);


//void generate_tables(FILE* dxf);
//void generate_tables(FILE* dxf, int i);
void generate_header(FILE* dxf, long x1, long y1, long x2, long y2);

void generate_blocks(FILE* dxf);

CString formatOutLine(const char* c, int np, int no, double p, double o);
//void formatOutNode(CString& str, int np, int no, double p, double o);
CString formatOutNode(const char* c, int np, int no, double p, double o);



const int* get_pics(int typ, double coef);
void chXY(double& x, double& y, double x1, double y1, double x2, double y2, double mas, double dx, double dy);


#define HTXT 1.2

bool CCxema::uch_pod_dxf(FILE* dxf, double dx1, double dy1, double dx2, double dy2, CLine* l)
{
    int nn = 0, i;
    CString str[20];

    if (!l || !l->isPodp) return false;

    if (m_bIsInfo[1]) {
        if (l->pod.q != 0. || l->obr.q != 0.) {
            str[nn++] = formatOutLine("G", l->nomP, l->nomO, fabs(l->pod.q), fabs(l->obr.q));
        }
    }
    if (m_bIsInfo[2] && l->typ == TIP_UT) {
        str[nn++] = formatOutLine("L", l->nomP, l->nomO, l->pod.dl, l->obr.dl);
    }
    if (m_bIsInfo[3] && l->typ == TIP_UT) {
        str[nn++] = formatOutLine("Двн", l->nomP, l->nomO, l->pod.diam, l->obr.diam);
    }
    if (m_bIsInfo[4] && l->typ == TIP_UT) {
        str[nn++] = formatOutLine("Ду", l->nomP, l->nomO, l->pod.diam_usl, l->obr.diam_usl);
    }
    if (m_bIsInfo[5] && l->typ == TIP_UT) {
        str[nn++] = formatOutLine("V", l->nomP, l->nomO, l->pod.v, l->obr.v);
    }
    if (m_bIsInfo[8] && l->typ == TIP_UT) {
        if (l->pod.q != 0. || l->obr.q != 0.) {
            str[nn++] = formatOutLine("w", l->nomP, l->nomO, l->pod.w, l->obr.w);
        }
    }
    if (m_bIsInfo[9] && l->typ == TIP_UT) {
        str[nn++] = formatOutLine("R", l->nomP, l->nomO, l->pod.poter, l->obr.poter);
    }
    if (m_bIsInfo[13] && l->typ == TIP_UT) {
        if (l->pod.q != 0. || l->obr.q != 0.) {
            //        str[nn++] = formatOutLine("Qр", l->nomP, l->nomO, l->pod.tzam, l->obr.tzam);

            str[nn++] = formatOutLine("Q", l->nomP, l->nomO, l->pod.ql, l->obr.ql);

            if (l->pod.ql_ot != 0 || l->obr.ql_ot != 0) {
                str[nn++] = formatOutLine("Qот", l->nomP, l->nomO, l->pod.ql_ot, l->obr.ql_ot);
            }
            if (l->pod.ql_v != 0 || l->obr.ql_v != 0) {
                str[nn++] = formatOutLine("Qвент", l->nomP, l->nomO, l->pod.ql_v, l->obr.ql_v);
            }
            if (l->pod.ql_gv != 0 || l->obr.ql_gv != 0) {
                str[nn++] = formatOutLine("Qгвс", l->nomP, l->nomO, l->pod.ql_gv, l->obr.ql_gv);
            }
        }
    }


    int sz = 0;

    for (i = 0; i < nn; i++) {
        sz = max(sz, str[i].GetLength() * 0.4);
    }


    double dd, dx, dy;
    double x, y, x1, y1, x2, y2, xx, yy, dc;


    double ht = 0.01 * m_coef0 / m_coef * HTXT;


    dc = ht / 5;
    xx = ht * sz + dc;
    yy = ht * nn + dc;

    double x0 = (dx1 + dx2) / 2;
    double y0 = (dy1 + dy2) / 2;


    if (dx2 < dx1) {
        x = dx1; dx1 = dx2; dx2 = x;
        y = dy1; dy1 = dy2; dy2 = y;
    }

    dd = hypot(dx2 - dx1, dy2 - dy1);    if (dd < 0.001) return false;
    dx = (dx2 - dx1) / dd * dc * 4;
    dy = (dy2 - dy1) / dd * dc * 4;



    if (dy2 > dy1) {
        x1 = x0 + dy;
        y1 = y0 - dx;
    }
    else {
        x1 = x0 - dy;
        y1 = y0 + dx;
    }

    double x10 = x1;
    double y10 = y1;


    if (dy2 > dy1) {
        y1 -= ht * nn * 2 * 0.85;
    }

    x2 = x1 + xx + dc;
    y2 = y1 + yy + dc;


    CString ss = "";

    if (l->isPodp) {
        for (i = 0; i < nn; i++) {
            //    drawPodp2(m_dc, x1+dc/2, y1+sz.cy*i+dc/2, str[i]);
            if (str[i][0]) {
                //                write_text(dxf, x1 + dc / 2, y1 + ht * i + dc / 2, str[i], 0, ht, 0, "RED");
                if (ss != "") ss += "\\P";
                ss += str[i];
            }
        }

        ss = "{\\fArial|b0|i0|c204|p34;\\C7;" + ss + "}";
        write_mtext(dxf, x1 + dc / 2, y1 + ht * nn * 2 * 0.85 - dc / 2 - dc, ss, 0, ht, 0, get_h_MS());
    }

    /*
        write_line(dxf, x0, y0, x10, y10, "BLACK");

        double xx2 = (x2-x1)*2 + x1;

        for (i = 0; i < nn+1 ; i++) {
            write_line(dxf, x1, y1+ht*i*2, xx2, y1+ht*i*2, "BLACK");
        }

        write_line(dxf, x1, y1, x1, y1+ht*nn*2, "BLACK");
    */

    write_line(dxf, x0, y0, x10, y10, "BLACK", get_h_MS());


    int hpline = write_pline(dxf, 0, 0., 0., "BLACK", get_h_MS());
    //    write_vertex(dxf, x0, y0, 0., 0., 0., "BLACK");
    //    write_vertex(dxf, x10, y10, 0., 0., 0., "BLACK");


    ht = ht * 0.85;

    double xx2 = (x2 - x1) * 2 + x1;

    for (i = 0; i < nn + 1; i++) {
        write_vertex(dxf, xx2, y1 + ht * i * 2, 0., 0., 0., "BLACK", hpline);
        write_vertex(dxf, x1, y1 + ht * i * 2, 0., 0., 0., "BLACK", hpline);

        if (i < nn) {
            write_vertex(dxf, x1, y1 + ht * (i + 1) * 2, 0., 0., 0., "BLACK", hpline);
            write_vertex(dxf, xx2, y1 + ht * (i + 1) * 2, 0., 0., 0., "BLACK", hpline);
        }
    }

    //    write_vertex(dxf, x1, y1, 0., 0., 0., "BLACK");
    //    write_vertex(dxf, x1, y1+ht*nn*2, 0., 0., 0., "BLACK");
    write_seqend(dxf, "BLACK", get_h_MS());

    dx = (dx1 - dx2) / dd;
    dy = (dy1 - dy2) / dd;

    if (l->typ != TIP_UT) {
        const int* pics = get_pics(l->typ, 1);

        if (!pics) pics = get_pics(TIP_UT, 1);

        int i, t, ip, c1, c2;
        double mas = 1, r;
        double dx, dy, x01, y01, y02, x02;
        x01 = dx1, y01 = dy1, y02 = dy2, x02 = dx2;
        dx = (x02 - x01) / dd;
        dy = (y02 - y01) / dd;

        ip = 0;

        while (1) {
            t = pics[ip++];

            if (t == 'q') break;

            double xx1, yy1;

            switch (t)
            {
            case 'f':
            case 'l':

                i = 0;
                while (true) {
                    xx = pics[ip++]; if (xx == 9999) break;
                    yy = pics[ip++];
                    chXY(xx, yy, x01, y01, x02, y02, 1000. * m_coef / m_coef0, dx, dy);
                    //        chXY(xx, yy, x01, y01, x02, y02, 10, dx, dy);

                    if (i >= 1) {
                        write_line(dxf, xx1, yy1, xx, yy, "RED", get_h_MS());
                    }
                    i++;
                    xx1 = xx;
                    yy1 = yy;
                }

                break;

            case 'k':  // Круг
            case 'r': //  Окружность
                xx = pics[ip++];
                yy = pics[ip++];

                chXY(xx, yy, x01, y01, x02, y02, 1000. * m_coef / m_coef0, dx, dy);
                //      chXY(xx, yy, x01, y01, x02, y02, 10, dx, dy);

                r = 0.001 * pics[ip++] * m_coef0 / m_coef;


                //      r = 0.003*m_coef0/m_coef;
                //      r /= mas_otn2;
                //      setline(c_color, 1./mas_otn+0);
                //      ellipse(xx, yy, r/mas, r/mas);
                write_circle(dxf, xx, yy, r, "RED", get_h_MS());

                break;

            case 'c':   // Цвет
                c1 = pics[ip++];
                c2 = pics[ip++];
                /*
                      switch (c1) {
                      case 'c' :  setcolor(color, 1./mas_otn);  break;
                      case 'b' :  setcolor(BLACK, 1./mas_otn);  break;
                      case 'w' :  setcolor(m_bk_color, 1./mas_otn);  break;
                      case 'x' :  setfillstyle(1, LIGHTCYAN);  break;
                //      case 'x' :  setfillstyle(1, color*2);  break;
                      }
                      switch (c2) {
                      case 'c' :  setfillstyle(1, color);  break;
                      case 'b' :  setfillstyle(1, BLACK);  break;
                      case 'w' :  setfillstyle(1, m_bk_color);  break;
                      case 'x' :  setfillstyle(1, LIGHTCYAN);  break;
                //      case 'x' :  setfillstyle(1, color*2);  break;
                      }
                */
                break;
            case 'd':   // Дуга
        /*
              xx = pic[ip++];
              yy = pic[ip++];

              xx0 = x + xx/mas;
              yy0 = y + yy/mas;

              xx1 = xx0-r;
              yy1 = yy0-r;
              xx2 = xx0+r;
              yy2 = yy0+r;

              xx3 = pic[ip++]/mas;
              yy3 = pic[ip++]/mas;
              xx4 = pic[ip++]/mas;
              yy4 = pic[ip++]/mas;

        //      m_dc->Arc(xx1, yy1, xx2, yy2,
        //        x + xx3, y + yy3, x + xx4, y + yy4);
        */
                break;
            }
        }

    }


    //  if (l->typ == TIP_ZD) {
    //    write_line(dxf, x0, y0, x0+dy/m_coef, y0+dx/m_coef, "BLACK");
    ///    write_line(dxf, x0, y0, x0+dy, y0-dx, "BLACK");
    //    write_line(dxf, x0, y0, x0+dx+dy, y0+dy-dx, "BLACK");
    //    write_line(dxf, x0+dx+dy, y0+dy-dx, x0+dx-dy, y0+dy+dx, "BLACK");
    //    write_line(dxf, x0+dx-dy, y0+dy+dx, x0, y0, "BLACK");
    //  }


    return true;
}

CString drawPodp(FILE* dxf, const char* c, int np, int no, double p, double o)
{
    CString str, s;

    str = formatOutNode(c, np, no, p, o);

    if (np != -1 && no != -1) {
        str = formatOutNode(c, np, -1, p, o);
        str += ",";
        str += formatOutNode("", -1, no, p, o);
    }

    //    s.Format("%s=%s", c, str);

    return str;
}

bool LoadFont(const TCHAR* szSection, LOGFONT& lf, COLORREF& color);

// Узловой расход текст

CString getUzRas(const CNode2 *node) 
{
    CString str = "", s;

    if (node->node.qz) {
        s.Format("Gз=%.1f", node->node.qz);
        if (str != "") str += "; ";
        str += s;
    }
    if (node->node.qP) {
        s.Format("Gп=%.1f", node->node.qP);
        if (str != "") str += "; ";
        str += s;
    }
    if (node->node.qO) {
        s.Format("Gо=%.1f", node->node.qO);
        if (str != "") str += "; ";
        str += s;
    }

    return str;
}

#define DXF_BLOCK 1


bool CCxema::exportDxf(const char* fn, int m_isGeobaza)
{

    COLORREF color;
    LOGFONT lf;
    LoadFont(_T("Font Gidr"), lf, color);

    double coef_font = fabs((double)lf.lfHeight/23.);


    int m_parent_id = 1;
    CGidrView* pView = getView();

    if (pView) {
        m_parent_id = pView->m_parent_id;
    }

    CString str;

    FILE* dxf;

    dxf = fopen(fn, "w");

    if (!dxf) {
        AfxMessageBox("Не могу открыть файл", MB_OK | MB_ICONINFORMATION);
        return false;
    }


    CFRect rectBorder = m_graph->getBorder(0);

    m_coef0 = 10000 * coef_font;
    m_coef = 100;
    //  m_coef = 1;

    CClientDC screenDC(NULL);
    double dmas = screenDC.GetDeviceCaps(LOGPIXELSX) / 2.54;

    //  if (m_kam_parent || !isGeobaza()) m_coef *= 10000;

    char ss[256];
    CLINE2* l;
    CLine2* ll;
    CNode2* n1, * n2;
    int jj;

    long i = 1;

    write_minimun(dxf,
        rectBorder.left / m_coef, -rectBorder.bottom / m_coef,
        rectBorder.right / m_coef, -rectBorder.top / m_coef, 3);

    /*


        generate_header(dxf,
            rectBorder.left / m_coef, -rectBorder.top / m_coef,
            rectBorder.right / m_coef, -rectBorder.bottom / m_coef);
        generate_tables(dxf);
        //  generate_tables(dxf, "RED", 12);  // Правильно
        generate_tables(dxf, "RED", 12);  // Правильно
        generate_tables(dxf, "BLUE", 16);  // Правильно

        generate_blocks(dxf);
    */

    /*
     1 - красный
     2 - желтый
     3 - зеленый
     4 - голубой
     5 - синий
     6 - пурпурный
     7 - черный
     8 - серый
     9 -
     10 - красный
    */
#if 0
    extern CGeoFile* m_geofile;

    if (m_geofile && m_isGeobaza) {
        m_geofile->DxfKlass(dxf);
    }


    write_DXF(dxf, 0, "SECTION");
    write_DXF(dxf, 2, "ENTITIES");


    if (m_geofile && m_isGeobaza) {
        m_geofile->Dxf(dxf, rectBorder);
    }

#endif

    int handle_parent = 0;

#ifdef DXF_BLOCK
    int h_US = write_block_record(dxf, "US", get_h_BLOCK_RECORD());
    int h_PR = write_block_record(dxf, "PR", get_h_BLOCK_RECORD());
    int h_PO = write_block_record(dxf, "PO", get_h_BLOCK_RECORD());
#endif

    write_minimun_2(dxf);

#ifdef DXF_BLOCK
    int h2;

    h2 = write_block_head(dxf, "US", h_US);
    write_circle(dxf, 0, 0, 0.6, "RED", h_US);
    write_block_tail(dxf, h_US);

    h2 = write_block_head(dxf, "PR", h_PR);
    write_circle(dxf, 0, 0, 0.3, "RED", h_PR);
    write_circle(dxf, 0, 0, 0.6, "RED", h_PR);
    write_block_tail(dxf, h_PR);

    h2 = write_block_head(dxf, "PO", h_PO);
    write_circle(dxf, 0, 0, 0.3, "RED", h_PO);
    write_circle(dxf, 0, 0, 0.6, "RED", h_PO);
    write_block_tail(dxf, h_PO);
#endif



    write_minimun_3(dxf);


    double ht = 0.01 * m_coef0 / m_coef * HTXT;
    double dy = 0.015 * m_coef0 / m_coef * HTXT;
    double dy1 = 0.02 * m_coef0 / m_coef * HTXT;
    double r1 = 0.003 * m_coef0 / m_coef * HTXT;

    if (m_geofile && m_isGeobaza) {
        m_geofile->Dxf(dxf, rectBorder);
    }

    //    write_insert(dxf, "US", 1, 1, 0, "RED");

    CMapGraph::const_iterator pp = m_graph->map_node.begin();

#if 1

    while (pp != m_graph->map_node.end()) {
        CNode2* p = pp->second;
        if ((p->node.coord.x != 0 || p->node.coord.y != 0) && p->node.internalNodeID == m_parent_id) {
            double y = -p->node.coord.y / m_coef - dy1;

            switch (p->node.typ) {
            case TIP_PR:
#ifdef DXF_BLOCK
                write_insert(dxf, "PR", p->node.coord.x / m_coef, -p->node.coord.y / m_coef, 0, "RED", get_h_MS() );

#else
                write_circle(dxf, p->node.coord.x / m_coef, -p->node.coord.y / m_coef, 0.3, "RED");
                write_circle(dxf, p->node.coord.x / m_coef, -p->node.coord.y / m_coef, 0.6, "RED");
#endif

                break;
            case TIP_PO:
#ifdef DXF_BLOCK
                write_insert(dxf, "PO", p->node.coord.x / m_coef, -p->node.coord.y / m_coef, 0, "RED", get_h_MS() );
#else
                write_circle(dxf, p->node.coord.x / m_coef, -p->node.coord.y / m_coef, 0.3, "RED");
                write_circle(dxf, p->node.coord.x / m_coef, -p->node.coord.y / m_coef, 0.6, "RED");
#endif

                break;
            case TIP_US:
#ifdef DXF_BLOCK
                write_insert(dxf, "US", p->node.coord.x / m_coef, -p->node.coord.y / m_coef, 0, "RED", get_h_MS() );
#else
                write_circle(dxf, p->node.coord.x / m_coef, -p->node.coord.y / m_coef, 0.6, "RED");
#endif

                break;
            }

            if (p->node.isPodp) {
                CString ss = "";

                if (p->node.name[0] != '#') {
                    str = p->node.name;
                    if (ss != "") ss += "\\P";  ss += str;
                }
                if (m_bIsInfo[0] || m_bIsInfo[6] || m_bIsInfo[10]) {
                    if (p->node.nomgP > 0 || p->node.nomgO > 0) {
                        if (m_bIsInfo[0]) {
                            str = drawPodp(dxf, "P", p->node.nomP, p->node.nomO, p->node.data.pihP, p->node.data.pihO);
                            if (ss != "") ss += "\\P";  ss += str;
                        }
                        if (m_bIsInfo[6]) {
                            str = drawPodp(dxf, "t", p->node.nomP, p->node.nomO, p->node.data.tempP, p->node.data.tempO);
                            if (ss != "") ss += "\\P";  ss += str;
                        }
                    }
                }

                if (m_bIsInfo[11]) {
                    if (p->node.nomgP > 0 && p->node.nomgO > 0) {
                        CString str;
                        str.Format("dH=%.1f", p->node.data.pihP - p->node.data.pihO);
                        if (ss != "") ss += "\\P"; ss += str;
                    }
                }
                if (m_bIsInfo[12]) {
                    CString str;
                    str.Format("h=%.1f", p->node.geoMarkTopTube);
                    if (ss != "") ss += "\\P"; ss += str;
                }


                if (m_bIsInfo[7] && (p->node.typ == TIP_PR || p->node.typ == TIP_PO)) {
//                    CString str;
//                    str.Format("Gз=%.1f; Gп=%.1f; Gо=%.1f", p->node.qz, p->node.qP, p->node.qO);
                    CString str = getUzRas(p);

                    if (ss != "") ss += "\\P"; ss += str;
                }

                if (m_bIsInfo[10]) { // Нагрузки
                    if (p->node.Qot != 0 || p->node.Qgvs != 0 || p->node.Qvent != 0) {

                        str.Format("Qот=%.3f", p->node.Qot);
                        if (ss != "") ss += "\\P"; ss += str;

                        str.Format("Qв=%.3f", p->node.Qvent);
                        if (ss != "") ss += "\\P"; ss += str;

                        str.Format("Qгвс=%.3f", p->node.Qgvs);
                        if (ss != "") ss += "\\P"; ss += str;

                        str.Format("Qсум=%.3f", p->node.Qot + p->node.Qgvs + p->node.Qvent);
                        if (ss != "") ss += "\\P"; ss += str;
                    }
                }
                ss = "{\\fArial|b0|i0|c204|p34;\\C7;" + ss + "}";
                write_mtext(dxf, p->node.coord.x / m_coef, y, ss, 0, ht, 1, get_h_MS());
            }
        }
        pp++;
    }
#endif

#if 1
    pp = m_graph->map_node.begin();
    while (pp != m_graph->map_node.end()) {
        CNode2* p = pp->second;
        if ((p->node.coord.x != 0 || p->node.coord.y != 0) && p->node.internalNodeID == m_parent_id) {

            for (l = p->lines; l; l = next(l)) {
                if (!IsBegin(l)) continue;

                n1 = where(l);
                n2 = other(l);

                if (n1->node.coord.x != 0 && n2->node.coord.y != 0) {
                    ll = bline(l);

                    int nsz = ll->line.m_NP.size();

                    CCoordList::const_iterator it;

                    it = ll->line.m_NP.begin();

                    sprintf(ss, "N%ld", i++);

                    if (it == ll->line.m_NP.end()) {

                        if (ll->line.isPodp)
                            write_line(dxf, n1->node.coord.x / m_coef, -n1->node.coord.y / m_coef, n2->node.coord.x / m_coef, -n2->node.coord.y / m_coef, "RED", get_h_MS());
                        uch_pod_dxf(dxf, n1->node.coord.x / m_coef, -n1->node.coord.y / m_coef, n2->node.coord.x / m_coef, -n2->node.coord.y / m_coef, &ll->line);
                    }
                    else {
                        int i_pic = ll->line.m_NP.size() / 2;
                        double x, y;
                        x = n1->node.coord.x;
                        y = n1->node.coord.y;

                        int hpline = write_pline(dxf, 0, 0., 0., "RED", get_h_MS());
                        write_vertex(dxf, n1->node.coord.x / m_coef, -n1->node.coord.y / m_coef, 0., 0., 0., "RED", hpline);
                        for (jj = 0; it != ll->line.m_NP.end(); ++it, jj++) {
                            write_vertex(dxf, it->x / m_coef, -it->y / m_coef, 0., 0., 0., "RED", hpline);
                            if (jj == i_pic) {
                                //            uch_pod_dxf(dxf, x/m_coef, -y/m_coef, it->x/m_coef, -it->y/m_coef, ll);
                            }
                            x = it->x;
                            y = it->y;
                        }
                        write_vertex(dxf, n2->node.coord.x / m_coef, -n2->node.coord.y / m_coef, 0., 0., 0., "RED", hpline);
                        write_seqend(dxf, "RED", get_h_MS());


                        x = n1->node.coord.x;
                        y = n1->node.coord.y;

                        it = ll->line.m_NP.begin();
                        for (jj = 0; it != ll->line.m_NP.end(); ++it, jj++) {
                            if (jj == i_pic) {
                                uch_pod_dxf(dxf, x / m_coef, -y / m_coef, it->x / m_coef, -it->y / m_coef, &ll->line);
                            }
                            x = it->x;
                            y = it->y;
                        }
                    }
                }
            }
        }
        pp++;
    }
#endif

    write_minimun_tail(dxf);

    fclose(dxf);

    return true;
}
