#include "stdafx.h"
#include "Scroll.h"
#include "graph.h"
#include "Graph2.h"

#include "win.h"
#include "klassif.h"

bool isLine(int loc);

extern Prim* rs;

static int colMark = YELLOW;

//  int colMark = 0x007F00;
//  colMark = YELLOW;
//  int colMark = 0x00FFFF;
//  int colMark = 0x00FF00;



void CScroll::put_ras(CDC* m_dc, int x, int y, int typ, int col)
{
    int i, xx, yy, ii, kk;
    unsigned short k;

    //  return;

    unsigned short* str = (unsigned short*)rs[typ - 1].s;

    //  CClientDC odc(scr);
    CClientDC odc(NULL);

    int qq = 1;

    int dpix = m_dc->GetDeviceCaps(LOGPIXELSX);
    int dpis = odc.GetDeviceCaps(LOGPIXELSX);

    qq = dpix / dpis;
    qq = max(1, qq);

    if (qq > 1) {
        x -= rs[typ - 1].x * qq; y -= rs[typ - 1].y * qq;
        for (i = 0, yy = y; i < 16; i++, yy += qq) {
            for (ii = 0; ii < qq; ii++) {
                for (k = 0x8000, xx = x; k; k >>= 1, xx += qq) {
                    for (kk = 0; kk < qq; kk++) {
                        if (str[i] & k) putpixel(m_dc, xx + ii, yy + kk, col);
                    }
                }
            }
        }
    }
    else {
        x -= rs[typ - 1].x; y -= rs[typ - 1].y;
        for (i = 0, yy = y; i < 16; i++, yy += qq) {
            for (k = 0x8000, xx = x; k; k >>= 1, xx++) {
                if (str[i] & k) putpixel(m_dc, xx, yy, col);
            }
        }
    }
}


bool LiangBar(int XS1, int YS1, int XS2, int YS2,
    double& x0, double& y0, double& x1, double& y1);



#include <vector>
#include <algorithm>


// Определяем стороны экрана
enum ScreenEdge { LEFT, RIGHT, BOTTOM, TOP };

// Проверка, находится ли точка внутри области экрана
bool inside(const CPoint& point, ScreenEdge edge, int screenWidth, int screenHeight) {
    switch (edge) {
        case LEFT:   return point.x >= 0;
        case RIGHT:  return point.x < screenWidth;
        case BOTTOM: return point.y >= 0;
        case TOP:    return point.y < screenHeight;
        default:     return false;
    }
}

// Пересечение линии с одной из сторон экрана
CPoint intersect(const CPoint& p1, const CPoint& p2, ScreenEdge edge, int screenWidth, int screenHeight) 
{
    CPoint intersection;
    double t;
    switch (edge) {
        case LEFT:
            t = (0 - p1.x) / double(p2.x - p1.x);
            intersection = {0, static_cast<int>(p1.y + t * (p2.y - p1.y))};
            break;
        case RIGHT:
            t = (screenWidth - 1 - p1.x) / double(p2.x - p1.x);
            intersection = {screenWidth - 1, static_cast<int>(p1.y + t * (p2.y - p1.y))};
            break;
        case BOTTOM:
            t = (0 - p1.y) / double(p2.y - p1.y);
            intersection = {static_cast<int>(p1.x + t * (p2.x - p1.x)), 0};
            break;
        case TOP:
            t = (screenHeight - 1 - p1.y) / double(p2.y - p1.y);
            intersection = {static_cast<int>(p1.x + t * (p2.x - p1.x)), screenHeight - 1};
            break;
    }
    return intersection;
}

// Алгоритм Sutherland-Hodgman для отсечения многоугольника по экрану
std::vector<CPoint> clipPolygon(const std::vector<CPoint>& polygon, int screenWidth, int screenHeight) 
{
    std::vector<CPoint> clippedPolygon = polygon;

    if (polygon.size() >= 2) {
        for (ScreenEdge edge : {LEFT, RIGHT, BOTTOM, TOP}) {
            std::vector<CPoint> newPolygon;

            if (clippedPolygon.size() > 0) {

                CPoint prevPoint = clippedPolygon.back();

                for (const auto& currPoint : clippedPolygon) {
                    if (inside(currPoint, edge, screenWidth, screenHeight)) {
                        if (!inside(prevPoint, edge, screenWidth, screenHeight)) {
                            newPolygon.push_back(intersect(prevPoint, currPoint, edge, screenWidth, screenHeight));
                        }
                        newPolygon.push_back(currPoint);
                    }
                    else if (inside(prevPoint, edge, screenWidth, screenHeight)) {
                        newPolygon.push_back(intersect(prevPoint, currPoint, edge, screenWidth, screenHeight));
                    }
                    prevPoint = currPoint;
                }

                clippedPolygon = newPolygon;
            }
        }
    }

    return clippedPolygon;
}



CPoint CoordToScreen(const CFPoint& point, const ScrollGeom &geom)
{
    return CPoint((int)(point.x / geom.masx - geom.m_bx), (int)(point.y / geom.masy - geom.m_by));
}


void CoordToScreenCl(const CCoordList& cl, std::list< std::vector<CPoint> > & list_cl2, const ScrollGeom& geom)
{
    CPoint pt, pt0(INT_MAX, INT_MAX);

    std::vector<CPoint> cl2;

//    CCoordList::const_iterator it = cl.begin();

    for ( auto & it : cl) {
        if (isBreakCoord(it.x)) {
            list_cl2.push_back(cl2);
            cl2.clear();
        }
        else {
            pt = CoordToScreen(it, geom);
            if (pt != pt0) {
                cl2.push_back(pt);
            }

            pt0 = pt;
        }
    }

    list_cl2.push_back(cl2);
}


void CScroll::DrawPolygon2(CDC* m_dc, const CCoordList& cl, int loc) 
{
    CRect r = m_rect;
//    r.InflateRect(10, 10);

    CFRect rectScr = ScreenToCoord(r);

    CCoordList::const_iterator it = cl.begin();

    std::list< std::vector<CPoint> > list_cl2;
    CoordToScreenCl(cl, list_cl2, geom);

    for (auto& cl2 : list_cl2) {
        std::vector<CPoint> cl3 = clipPolygon(cl2, rectScr.Width(), rectScr.Height());
        if (cl3.size() >= 2) {
            m_dc->Polyline(cl3.data(), cl3.size());
        }
    }


#if 0

    int n, i, k = 0;
    int x1, y1, x2, y2;

    if (it != cl.end()) {

        n = cl.size();

        CPoint* p = new CPoint[n + 1];

        p[0] = p0 = CoordToScreen(*it);
        it++;
        k++;

        x1 = INT_MAX; x2 = INT_MIN; y1 = INT_MAX; y2 = INT_MIN;

        for (; it != cl.end(); it++) {
            if (isBreakCoord(it->x)) {
                p[k].x = C_SPR;
            }
            else {
                p[k] = CoordToScreen(*it);

                x1 = min(x1, p[k].x);      x2 = max(x2, p[k].x);
                y1 = min(y1, p[k].y);      y2 = max(y2, p[k].y);
            }


            if (k == 0 || p[k] != p[k - 1]) k++;
        }

        double x00, y00, x0, y0, x1, y1;

        x00 = x0 = p[0].x;
        y00 = y0 = p[0].y;


        for (i = 1; i < k; i++) {

            if (isBreakCoord(p[i].x)) {
                i++;
                x00 = x0 = p[i].x;
                y00 = y0 = p[i].y;
                break;
            }
            else {
                x1 = p[i].x;
                y1 = p[i].y;

                line(m_dc, x0, y0, x1, y1, m_rect);


                x0 = x1;
                y0 = y1;
            }
        }

        delete[] p;
    }
#endif
}


void CScroll::DrawPolygon3(CDC* m_dc, const CCoordList& cl) 
{
    CPoint p0;
    CRect r = m_rect;
//    r.InflateRect(10, 10);

    //  m_dc->SetPolyFillMode(ALTERNATE );

    CFRect rectScr = ScreenToCoord(r);

    CCoordList::const_iterator it = cl.begin();

    std::list< std::vector<CPoint> > list_cl2;
    CoordToScreenCl(cl, list_cl2, geom);

    for (auto& cl2 : list_cl2) {
        std::vector<CPoint> cl3 = clipPolygon(cl2, rectScr.Width(), rectScr.Height());
        if (cl3.size() >= 3) {
            m_dc->Polygon(cl3.data(), cl3.size());
        }
    }

#if 0

    CCoordList::const_iterator it = cl.begin();
    int n, k = 0;

    int x1, y1, x2, y2;

    if (it != cl.end()) {

        n = cl.size();

        if (n == 2090) {
            n = n;
        }


        CPoint* p = new CPoint[n + 1];

        p[0] = p0 = CoordToScreen(*it);
        ++it;
        k++;

        x1 = INT_MAX; x2 = INT_MIN; y1 = INT_MAX; y2 = INT_MIN;

        for (; it != cl.end(); ++it) {
            if (isBreakCoord(it->x)) {
                p[k].x = C_SPR;
            }
            else {
                p[k] = CoordToScreen(*it);
                x1 = min(x1, p[k].x);      x2 = max(x2, p[k].x);
                y1 = min(y1, p[k].y);      y2 = max(y2, p[k].y);
            }

            if (k == 0 || p[k] != p[k - 1]) k++;
        }

        //    if (x1 > m_rect.left-100*masx && x2 < m_rect.right+100*masx && y1 > m_rect.top-100*masy && y2 < m_rect.bottom+100*masy) {
        if (true) {

            CCoordList cl2 = cl;
            CCoordList crect;
            CString str;

            delete[] p;

            n = cl2.size();
            p = new CPoint[n + 1];

            it = cl2.begin();
            p[0] = p0 = CoordToScreen(*it);
            ++it;

            for (k = 1; it != cl2.end(); ++it) {
                if (isBreakCoord(it->x)) {
                    if (k > 2) m_dc->Polygon(p, k);
                    k = 0;
                }
                else {
                    p[k] = CoordToScreen(*it);
                    if (k == 0 || p[k] != p[k - 1]) k++;
                }
            }
            if (k > 2) m_dc->Polygon(p, k);
        }
        else {
            CCoordList cl2;
            CCoordList crect;
            CString str;
            if (cl.size() >= 3) {
                crect.push_back(CFPoint(rectScr.left, rectScr.top));
                crect.push_back(CFPoint(rectScr.left, rectScr.bottom));
                crect.push_back(CFPoint(rectScr.right, rectScr.bottom));
                crect.push_back(CFPoint(rectScr.right, rectScr.top));
                if (!cl2.And(crect, cl, PBO_ISECT)) {
                    cl2 = cl;
                }
            }

            delete[] p;

            n = cl2.size();
            p = new CPoint[n + 1];

            it = cl2.begin();
            p[0] = p0 = CoordToScreen(*it);
            ++it;

            for (k = 1; it != cl2.end(); ++it) {
                if (isBreakCoord(it->x)) {
                    if (k > 2) m_dc->Polygon(p, k);

                    k = 0;
                }
                else {
                    p[k] = CoordToScreen(*it);
                    if (k == 0 || p[k] != p[k - 1]) k++;
                }
            }
            if (k > 2) m_dc->Polygon(p, k);
        }
        delete[] p;
    }
#endif
}



void CScroll::drawLine(CDC* m_dc, double x1, double y1, double x2, double y2, int punct)
{
    if (x1 == x2 && y1 == y2) return;

    double x01 = x1, y01 = y1, y02 = y2, x02 = x2;

    if (!LiangBar(m_rect.left - 20 / geom.masx, m_rect.top - 20 / geom.masy,
        m_rect.right + 20 / geom.masx, m_rect.bottom + 20 / geom.masy,
        x1, y1, x2, y2)) return;

    if (punct == 0) {
        line(m_dc, x1, y1, x2, y2);
    }
    else {
        int i, n, x, y, xx, yy;
        double len1 = hypot(x2 - x1, y2 - y1);
        double len = len1 * mas_otn, len_dot = 20 * m_h;

        double d07 = 0.7;
        //    double d07 = 0.5;


        if (len_dot <= 0. || len1 < len_dot) {
            n = 0;
        }
        else {
            n = len / len_dot;
        }

        if (n <= 1) {
            line(m_dc, x1, y1, x2, y2);
        }
        else {
            for (i = 0; i < n; i++) {
                x = x1 + (x2 - x1) * i / n;
                y = y1 + (y2 - y1) * i / n;

                if (i == n - 1) {
                    xx = x2;
                    yy = y2;
                }
                else {
                    xx = x1 + (x2 - x1) * (i + d07) / n;
                    yy = y1 + (y2 - y1) * (i + d07) / n;
                }
                line(m_dc, x, y, xx, yy);
            }
        }
    }
}


void CScroll::DrawPolygon(CDC* m_dc, const CCoordList& cl, int loc, int typ)
{
    CFRect rectScr = ScreenToCoord(m_rect);
    CFRect rr, rect = cl.GetFRect();

    if (!rr.IntersectRect(rect, rectScr)) return;

    if (loc == LOC3) {
        if (typ) {
            DrawPolygon2(m_dc, cl, 3);
        }
        else {
            DrawPolygon3(m_dc, cl);
        }
    }
    else {
        CCoordList::const_iterator it = cl.begin();
        int n, i, k = 0;

        int x1, y1, x2, y2;

        if (it != cl.end()) {

            n = cl.size();
            CPoint* p = new CPoint[n + 1];

            p[0] = CoordToScreen(*it);
            ++it;
            k++;

            x1 = INT_MAX; x2 = INT_MIN; y1 = INT_MAX; y2 = INT_MIN;

            for (; it != cl.end(); ++it) {
                if (!isBreakCoord((*it).x)) {
                    p[k] = CoordToScreen(*it);
                    x1 = min(x1, p[k].x);      x2 = max(x2, p[k].x);
                    y1 = min(y1, p[k].y);      y2 = max(y2, p[k].y);
                }
                else {
                    for (i = 0; i < k - 1; i++) {
                        drawLine(m_dc, p[i].x, p[i].y, p[i + 1].x, p[i + 1].y, typ);
                    }
                    k = 0;
                    continue;
                }

                if (k == 0 || p[k] != p[k - 1]) k++;
            }

            for (i = 0; i < k - 1; i++) {
                drawLine(m_dc, p[i].x, p[i].y, p[i + 1].x, p[i + 1].y, typ);
            }
            delete[] p;
        }

    }
}

void CScroll::DrawPolygonAdd(const CCoordList& cl, int loc)
{
    CFRect rectScr = ScreenToCoord(m_rect);
    CFRect rr, rect = cl.GetFRect();

    if (!rr.IntersectRect(rect, rectScr)) return;

    CRect r = m_rect;

    CCoordList::const_iterator it = cl.begin();

    std::list< std::vector<CPoint> > list_cl2;
    CoordToScreenCl(cl, list_cl2, geom);

    for (auto& cl2 : list_cl2) {
        std::vector<CPoint> cl3 = clipPolygon(cl2, rectScr.Width(), rectScr.Height());
        if (cl3.size() >= 3) {
            m_points.insert(m_points.end(), cl3.begin(), cl3.end());
            m_pCounts.push_back(cl3.size());
//            m_dc->Polygon(cl3.data(), cl3.size());
        }
    }
//    m_dc->PolyPolygon(points.data(), pCounts.data(), pCounts.size());


}


extern double m_MasPic;

void CScroll::Draw6(CDC* m_dc, CCoordList& cl, bool isMark, bool redr, int col, int kls_typ, double mas_otn1, bool no_mas)
{
    double mas_otn_old = mas_otn;

//    if (isMark) col = colMark;  // Убрал желтый цвет при выделении

    if (!no_mas && !isFull()) return;

    if (cl.size() == 1) {
        CFPoint p1 = cl.first();
        cl.push_back(CFPoint(p1.x + 100, p1.y));
    }

    if (cl.size() >= 2) {
        //    double dm = m_dc->GetDeviceCaps(LOGPIXELSX)/2.54;
        //    mas_otn = masx*dm/(500);
        mas_otn = mas_otn1;

        CCoordList::const_iterator it1, it2;
        it1 = it2 = cl.begin();
        it2++;

        CFPoint pt1 = CoordToScreenF1(*it1);
        CFPoint pt2 = CoordToScreenF1(*it2);

        col = redr ? 0x007F00 : col;

        if (kls_typ <= 0) kls_typ = 67;

        double m_thickness = 1.;

        double len = Length(pt1, pt2);

        pt2.x = pt1.x + (pt2.x - pt1.x) * 100. / len;
        pt2.y = pt1.y + (pt2.y - pt1.y) * 100. / len;

        drawLineTyp(m_dc, m_thickness, pt1.x, pt1.y, pt2.x, pt2.y, kls_typ, col, col, TRUE, TRUE, FALSE, TRUE, no_mas);
        mas_otn = mas_otn_old;
    }
}

void get_tri_p(int x, int y, double r, CPoint* p)
{
    p[0].x = x;
    p[0].y = y - r;

    p[1].x = x - r * 1.732 / 2;
    p[1].y = y + r / 2;

    p[2].x = x + r * 1.732 / 2.;
    p[2].y = y + r / 2;
}

void get_quad_p(int x, int y, double rx, double ry, CPoint* p)
{
    p[0].x = x - rx;
    p[0].y = y - ry;

    p[1].x = x - rx;
    p[1].y = y + ry;

    p[2].x = x + rx;
    p[2].y = y + ry;

    p[3].x = x + rx;
    p[3].y = y - ry;

}


void get_quad_p(int x, int y, double r, CPoint* p)
{
    p[0].x = x - r;
    p[0].y = y - r;

    p[1].x = x - r;
    p[1].y = y + r;

    p[2].x = x + r;
    p[2].y = y + r;

    p[3].x = x + r;
    p[3].y = y - r;

}




void CScroll::draw_tri(CDC* m_dc, CPoint pt, double r, double h, int col, double mas_otn)
{
    int x = pt.x;
    int y = pt.y;

    CPoint p[8];
    CPoint p2[8];

    get_tri_p(pt.x, pt.y, r/ mas_otn, p);
    get_tri_p(pt.x, pt.y, r/ mas_otn - h, p2);

    p[3] = p[0];
    p[4] = p2[0]; p[5] = p2[2]; p[6] = p2[1]; p[7] = p2[0];

    setcolor(m_dc, col);
    setfillstyle(m_dc, 0, col);

    fillpoly(m_dc, 8, p);

    setfillstyle(m_dc, 0, 0xFEFEFE);
    fillpoly(m_dc, 3, p2);
}

long getGeoColor(int color_id);


static map<int, int> map_color;

int getIDColor(int col)
{
    auto it = map_color.find(col);
    if (it != map_color.end()) {
        return it->second;
    }

    for (int id = 0; id < 16; id++) {
        int col0 = getGeoColor(id);
        if (col == col0) {
            map_color[col] = id;
            return id;
        }
    }
    map_color[col] = -1;
    return -1;
}

int getColorBorder(int coll)
{
    if (coll == YELLOW) return coll;

    int id1 = getIDColor(coll);
    int col = BLACK;

    switch (id1) {
    case 1: col = LIGHTRED; break;
    case 2: col = LIGHTBLUE; break;
    case 3: col = GREEN; break;
    }

    return col;
}

void CScroll::draw_defect(CDC* m_dc, CPoint pt, double r, double h, int coll, int coll2, double mas_otn)
{
//    int col2 = WHITE;
    int col2 = 0xFEFEFE;
    int id2 = getIDColor(coll2);

//    int с0 = LIGHTCYAN;

// 
//    int c1 = 256*256*51+256*51+204;   //  экспл

//    int c1 = 256*256*51+256*51+204;   //  экспл
//    int c1 = 0xC8D530;
    int c1 = 256*256*204+256*255+51;   //  экспл

// 48, 213, 200

    int c2 = 256*256*0+256*165+255;   //  шурф
    int c3 = 256*256*255+256*51+153;  //  осмотр
    int c4 = 256*256*141+256*141+143; //  опрессовка

//    int c5 = 256*256*0+256*165+255;

    switch (id2) {
///    case 0: col2 = c1; break;
    case 1: col2 = c1; break;   // Эксплуатация                       Эксплуатация', N'Э', 1)                   
    case 2: col2 = c4; break;   // Испытания                          Опрессовка', N'И', 2)                     
    case 3: col2 = c2; break;   // Плановый шурф                      шурф', N'Ш',3)                   
//    case 4: col2 = с0; break;   // Плановое освидетельствование      нет Плановое освидетельствование',N'О',4)     
//    case 5: col2 = с0; break;  // Плановая вырезка                   нет Плановая вырезка',N'В',5)                 
    case 6: col2 = c3; break;  // Плановый осмотр                     осмотр', N'С', 6)                
    }

    int col = getColorBorder(coll);

    int x = pt.x;
    int y = pt.y;

    CPoint p[8];
    CPoint p2[8];

    get_tri_p(pt.x, pt.y, r/mas_otn, p);
    get_tri_p(pt.x, pt.y, (r - h) / mas_otn, p2);

    p[3] = p[0];
    p[4] = p2[0]; p[5] = p2[2]; p[6] = p2[1]; p[7] = p2[0];

    setcolor(m_dc, col);
    setfillstyle(m_dc, 0, col);

    fillpoly(m_dc, 8, p);

    setfillstyle(m_dc, 0, col2);
    fillpoly(m_dc, 3, p2);

}

void CScroll::draw_shurf(CDC* m_dc, CPoint pt, int coll, int coll2, double mas_otn)
{
    int id2 = getIDColor(coll2);

//    int col2 = coll2;
//    int col2 = WHITE;
    int col2 = 0xFEFEFE;

    switch (id2) {
    case 1: col2 = LIGHTRED; break;
    case 2: col2 = 0x8D8D8F; break;
    case 3: col2 = 0x00A5FF; break;
    }

    int col = getColorBorder(coll);

    double rx = 10 * m_h, ry = 7 * m_h, h = 1 * m_h;
    int x = pt.x;
    int y = pt.y;

    CPoint p[18];
    CPoint p2[18];

    get_quad_p(pt.x, pt.y, rx/ mas_otn, ry/ mas_otn, p);
    get_quad_p(pt.x, pt.y, (rx - h)/mas_otn, (ry - h) / mas_otn, p2);

    p[4] = p[0];
    p[5] = p2[0]; p[6] = p2[3]; p[7] = p2[2]; p[8] = p2[1]; ; p[9] = p2[0];

    setcolor(m_dc, col);
    setfillstyle(m_dc, 0, col);

    fillpoly(m_dc, 10, p);
    setfillstyle(m_dc, 0, 0xFEFEFE);

    fillpoly(m_dc, 4, p2);

    p[0] = p2[0];
    p[1] = p2[1];
    p[2] = p2[3];

    setfillstyle(m_dc, 0, col2);
    fillpoly(m_dc, 3, p);

    p[0] = p2[1];
    p[1] = p2[2];
    p[2] = p2[3];

    setfillstyle(m_dc, 0, WHITE);
    fillpoly(m_dc, 3, p);
}

void CScroll::draw_quad(CDC* m_dc, CPoint pt, double r, double h, int col, double mas_otn)
{
    int x = pt.x;
    int y = pt.y;

    CPoint p[18];
    CPoint p2[18];

    get_quad_p(pt.x, pt.y, r, p);
    get_quad_p(pt.x, pt.y, r - h, p2);

    p[4] = p[0];
    p[5] = p2[0]; p[6] = p2[3]; p[7] = p2[2]; p[8] = p2[1]; ; p[9] = p2[0];

    setcolor(m_dc, col);
    setfillstyle(m_dc, 0, col);

    fillpoly(m_dc, 10, p);

    setfillstyle(m_dc, 0, 0xFEFEFE);
    fillpoly(m_dc, 4, p2);
}


void CScroll::Draw11(CDC* m_dc, CPoint pt, bool isMark, int col, int col2, int typ, int po)
{
    int tr = get_tr();
    setTransp(255, get_bk_color());

//    if (isMark) col = colMark;

    if (typ == 5) {
        int tr = get_tr();
        setTransp(255, get_bk_color());

        setline(m_dc, col, 2);
        setfillstyle(m_dc, 0, 0xFEFEFE);

        int r = 8 * m_h;
        fillellipse(m_dc, pt.x, pt.y, r, r);
        r = 1 * m_h;
        fillellipse(m_dc, pt.x, pt.y, r, r);

        setTransp(tr, get_bk_color());
    }
    else if (typ == 6) {  /// Индикатор коррозии

        int tr = get_tr();
        setTransp(255, get_bk_color());

        setline(m_dc, col, 2);
        setfillstyle(m_dc, 0, 0xFEFEFE);

        int r = 8 * m_h;

        fillellipse(m_dc, pt.x, pt.y, r, r);
        r = 1 * m_h;
        fillellipse(m_dc, pt.x, pt.y, r, r);

        r = 8 * m_h;
        int r2 = 4 * m_h;
        int r3 = 12 * m_h;

        if (po == 3) {  // Обратка
            r3 = -r3;
            r2 = -r2;
            r = -r;
        }


        m_dc->MoveTo(pt.x, pt.y - r2);
        m_dc->LineTo(pt.x, pt.y - r3);

        m_dc->MoveTo(pt.x - r, pt.y - r3);
        m_dc->LineTo(pt.x + r, pt.y - r3);

        setTransp(tr, get_bk_color());
    }
    else if (typ == 7) {  // Квадратик
        draw_quad(m_dc, pt, m_h * 7, m_h * 2, col, mas_otn);
    }
    else if (typ == 1024) {  // Квадратик
        draw_shurf(m_dc, pt, col, col2, mas_otn);
    }
    else if (typ == 1025) {
        draw_defect(m_dc, pt, m_h * 10, m_h * 1.5, col, col2, mas_otn);
    }
    else {
        draw_tri(m_dc, pt, m_h*10, m_h*2, col, mas_otn);
    }
    setTransp(tr, get_bk_color());
}

void CScroll::Draw1(CDC* m_dc, CCoordList& cl, bool isMark, bool redr, int col, int kls_typ, bool no_mas)
{
    if (!no_mas && !isFull()) return;
    int l = 3;

    CPoint pt = CoordToScreen(*cl.begin());

    if (redr) {
        col = 0x00FFFF;
    }

//    if (isMark) col = colMark;

    //  draw_tri(m_dc, pt, 30, 5);

    if (kls_typ == 210) {
        draw_tri(m_dc, pt, 10, 2, col, mas_otn);
        return;
    }

    setfillstyle(m_dc, 0, col);

    CLine* line = NULL;
    if (line && (line->pod.isOtkl || line->obr.isOtkl)) col = 0xFF0000;

    if (kls_typ == 0) kls_typ = 99;

    double thickness = 1.;

    drawLineTyp(m_dc, thickness, pt.x, pt.y, pt.x + 100, pt.y, kls_typ, col, col, TRUE, TRUE, FALSE, TRUE, false);
}

int getNpen();


void CScroll::Draw23(CDC* m_dc, CCoordList& cl, int loc, bool isMark, bool redr, int col, int colb, int kls_typ, int kls_punct)
{
    int n1 = getNpen();

    int markThickness = 0;

    if (isMark) {
//        col = colMark;
        markThickness = 4;
    }

#if 0
    CFRect rrr;
    rrr = cl.GetFRect();
    CRect cr = CoordToScreen(rrr);
    rectangle(m_dc, cr.left, cr.top, cr.right, cr.bottom);
    //    return;
#endif
    if (redr) {
        col = 0x00FFFF;
        if (loc == LOC3 && colb != 0xFFFFFF) {
            setline(m_dc, col, kls_typ / mas_otn + markThickness);
            setfillstyle(m_dc, 0, col);
        }
        else {
            //      setline(m_dc, col, (5+kls_typ)/mas_otn);
            setline(m_dc, col, 5 + kls_typ / mas_otn + markThickness);
        }
    }
    else {
        setline(m_dc, col, kls_typ / mas_otn + markThickness);
        setfillstyle(m_dc, 0, colb);

        if (loc == LOC12) {
            int tr = get_tr();
            setTransp(255, get_bk_color());
            setline(m_dc, col, kls_typ);
            setfillstyle(m_dc, 0, colb);
            setTransp(tr, get_bk_color());
        }
    }

    if (isLine(loc)) {
        DrawPolygon(m_dc, cl, loc, kls_punct);
    }
    else {
        DrawPolygon(m_dc, cl, loc, colb == 0xFFFFFF);
    }

    //  int n2 = getNpen();
    //  if (n1 != n2) {
    //    DrawPolygon(m_dc, cl, loc, colb == 0xFFFFFF);
    //  }
}
