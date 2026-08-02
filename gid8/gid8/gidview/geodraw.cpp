#include <QtGui>
#include <QtSql>
#include <QtWidgets>
#include "GidWidget.h"
#include "colorgtd.h"

double _xx1 = 0;
double _yy1 = 0;
double _xx2 = 0;
double _yy2 = 0;

bool isBreakCoord(int x);
bool isPoint(int loc);

void draw_indicator(QPainter *painter, QPointF pt, double m_h, int coll, int coll2);
void draw_shurf(QPainter *painter, QPointF pt, int coll, int coll2, double m_h);
void draw_defect(QPainter *painter, QPointF pt, double r, double h, int coll, int coll2);
void draw_quad(QPainter *painter, QPointF pt, double r, double h, int col);
void draw_tri(QPainter *painter, QPointF pt, double r, double h, int col);

QColor rgb_l(QColor color, int tr, QColor bk_color);
QColor rgb_l(QColor color);

bool GidWidget::isRezhim(const CNode2* n)
{
    if (n && !m_bIsRezhim) {
        CCxema* cxema = n->node.m_graph->m_cxema;

        if (cxema) {
//            auto it = cxema->m_AstanaMagSet.find(n->node.fileID);

//            if (n->node.fileID == cxema->m_AstanaMag || it != cxema->m_AstanaMagSet.end()) {
                QString kod = n->getKod();
                return kod == "П1" || kod == "П2";
//            }
        }
    }

    return false;
}

bool GidWidget::isRezhim(const CLINE2* l)
{
    if (!l) return false;
    return isRezhim(where(l)) || isRezhim(other(l));
}

void drawPolyline2(QPainter *painter, QPolygonF &p)
{
    QPointF pt0;
    bool first = true;
    for (auto &pt : p) {
        if (!first) {
            painter->drawLine(pt0, pt);
        }
        first = false;
        pt0 = pt;
    }
}

/*

void GidWidget::DrawPolygon2(QPainter *painter, const CCoordList& cl, int loc)
{
    QPointF pt0;
    QPolygonF p;

    int qq = 0;
    bool first = true;

    for (auto it : cl) {
        if (isBreakCoord(it.x)) {
            if (loc == 3) {
                p.push_back(pt0);
            }
//            painter->drawPolyline(p);
            drawPolyline2(painter, p);
            p.clear();
            first = true;
        }
        else {
            QPoint pt = CoordToQPoint(it);

            if (first) {
                pt0 = pt;
                first = false;
            }
            p.push_back(pt);
        }
        qq ++;
//        if (qq > 20) break;
    }

    if (p.size() >= 2) {
        if (loc == 3) {
            p.push_back(pt0);
        }
//        painter->drawPolyline(p);
        drawPolyline2(painter, p);
    }
}

*/

void GidWidget::DrawPolygon2(QPainter *painter, const CCoordList& cl, int loc)
{
    QPointF pt0, pt1;
    QList<QPointF> p;

    bool first = true;

    for (auto it : cl) {
        if (isBreakCoord(it.x)) {
            if (loc == 3) {
                p.push_back(pt1);
                p.push_back(pt0);
            }
            first = true;
        }
        else {
            QPointF pt = CoordToQPointF(it);
            if (first) {
                pt0 = pt;
                first = false;
            }
            else {
                p.push_back(pt1);
                p.push_back(pt);
            }
            pt1 = pt;
        }
    }

    painter->drawLines(p);
}

void GidWidget::DrawPolygon3(QPainter *painter, const CCoordList& cl)
{
    QPointF pt0;
    QPolygonF p;

    int qq = 0;
    bool first = true;

    for (auto it : cl) {
        if (isBreakCoord(it.x)) {
            painter->drawPolygon(p);
            p.clear();
            first = true;
        }
        else {
            QPoint pt = CoordToQPoint(it);

            if (first) {
                pt0 = pt;
                first = false;
            }

            p.push_back(pt);
        }
        qq ++;
//        if (qq > 100) break;
    }

    if (p.size() >= 3) {
        painter->drawPolygon(p);
    }
}


void GidWidget::drawObject(QPainter *painter, const CGeoObject *gl, Klassif *kls, bool redraw)
{
//    mas_otn = geom.mas*geom.dmas/(1500);
    if (gl->cl.size() < 1) return;

    if (kls->loc == 1 || kls->loc == 6) {
    }

    QColor col;

    CCoordList::const_iterator it = gl->cl.begin();
    if (it == gl->cl.end()) {
        return;
    }

    CFPoint pt = *it;

    double width = 1;
    if (kls->loc == LOC2 || kls->loc == LOC3) {
        width = kls->typ;

        if (gl->mark) {
            width += 5;
        }
    }

    int colb = kls->colb;

    if (gl->cl.is_color && colb != WHITE) {
        colb = gl->cl.color;
    }

    if (redraw) {
        painter->setPen(QPen(Qt::yellow, width/mas_otn));
        painter->setBrush(QBrush(Qt::yellow));
        col = Qt::yellow;
    }
    else {
        painter->setPen(QPen(QColor(kls->coli), width/mas_otn));
        if (colb != WHITE) {
            painter->setBrush(QBrush(QColor(colb)));
        }
        col = QColor(kls->color);
    }






    if (!scaling){

    if (kls->nazv == "shurfy") {
        draw_shurf(painter, CoordToQPointF(pt), gl->cl.color, gl->cl.color2, m_h);
        return;
    }
    else if (kls->nazv == "defect") {
        draw_defect(painter, CoordToQPointF(pt), m_h * 10, m_h * 3, gl->cl.color, gl->cl.color2);
        return;
    }
    else if (kls->nazv == "indikator_korrozii") {
        draw_indicator(painter, CoordToQPointF(pt), m_h, gl->cl.color, gl->cl.color2);
        return;
    }
    else if (kls->nazv == "remont") {  // Квадратик
        draw_quad(painter, CoordToQPointF(pt), m_h * 7, m_h * 2, gl->cl.color);
        return;
    }
    }
//    else if (kls->nazv == "defect") {  // Треугольник
//        draw_tri(painter, CoordToQPoint(pt), m_h * 7, m_h * 2, gl->cl.color);
//        return;
//    }

    if (kls->loc == LOC1) {
        col = QColor(kls->coli);

        GID_STYLE style;
        style.m_thickness = 1;

        double m_thickness = 1.;

        CFPoint pt1 = CoordToScreen(pt);

        double dx = 100.;
        double dy = 0;

        if (gl->cl.angle != 0) {
            dx = -100.*cos(gl->cl.angle*M_PI/180.);
            dy = -100.*sin(gl->cl.angle*M_PI/180.);
        }

        if (!scaling)

        drawLineTyp(painter, m_thickness, pt1.x, pt1.y, pt1.x+dx, pt1.y+dy, kls->typ, col, col, col, 1, false, false, false, true, true);
    }
    else if (kls->loc == LOC6) {
        col = QColor(kls->coli);

        
        double angle = gl->cl.angle;

        CFPoint pt1;
        CFPoint pt2;

        if (gl->cl.size() >= 2) {
            CCoordList::const_iterator it = gl->cl.begin();
            if (it != gl->cl.end()) {
                pt1 = CoordToScreen(*it);
                it++;
                if (it != gl->cl.end()) {
                    pt2 = CoordToScreen(*it);

                    GID_STYLE style;
                    style.m_thickness = 1;

                    //                    drawLineTyp(painter, style, pt1.x, pt1.y, pt2.x, pt2.y, kls->typ, col, col, 1, true, true, true);
                    double m_thickness = 1.;
                    drawLineTyp(painter, m_thickness, pt1.x, pt1.y, pt2.x, pt2.y, kls->typ, col, col,  col, 1, false, false, false, true, true);
                }
            }
        }
    }
    else if (kls->loc == LOC2) {
        DrawPolygon2(painter, gl->cl, 2);
    }
    else if (kls->loc == LOC3 && kls->colb == WHITE) {
        DrawPolygon2(painter, gl->cl, 3);
    }
    else if (kls->loc == LOC3) {
        if (!scaling)
        DrawPolygon3(painter, gl->cl);
    }
    double x = 0, y = 0, alpha = 0, coef = 1;
    if (kls->label) {
        if (!scaling)
            DrawLabel3(painter,gl->cl, gl->text00000, x, y, alpha, coef, kls->loc, kls);
    }
}


void GidWidget::drawGeoKl(QPainter *painter, Klassif &kl)
{
    double w = painter->device()->width();
    double h = painter->device()->height();

    CFRect rr, rectScr = ScreenToCoord(CFRect(0, 0, w, h));

    for (auto gl: kl.geo4) {
        if (gl) {
            CFRect rect = gl->rect;
            if (rr.IntersectRect(rect, rectScr)) {
                drawObject(painter, gl, &kl, false);
            }
        }
    }
}


void GidWidget::drawGeo(QPainter *painter, bool is_loc1)
{
    double mas = geom.masx*geom.dmas;

    int aloc[] = {LOC3, LOC2, LOC6, LOC1, LOC11, 0};

    for (int loc = 0; aloc[loc]; loc++) {
        if (aloc[loc] == LOC6 && mas > 5000) continue;
        if (aloc[loc] == LOC1 && mas > 5000) continue;

        if (!is_loc1 && isPoint(aloc[loc])) continue;
        if (is_loc1 && !isPoint(aloc[loc])) continue;

        for (int prio = 0; prio < 11; prio++) {
            for (auto & kl : m_kl_list) {
                if (kl.nazv == "zdaniya_tu") {
                    int qqq;
                    qqq = 0;
                }

                if (kl.prio == prio && kl.loc == aloc[loc] && kl.ins) {
                    drawGeoKl(painter, kl);
                }
            }
        }
    }
}
