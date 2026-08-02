#include <QtGui>
#include <QtSql>
#include <QtWidgets>
#include "GidWidget.h"
#include "colorgtd.h"

QPolygonF get_tri_p(double x, double y, double r)
{
    QPolygonF p;
    p << QPointF(x, y-r);
    p << QPointF(x - r * 1.732 / 2, y + r / 2);
    p << QPointF(x + r * 1.732 / 2, y + r / 2);
    return p;
}

QPolygonF get_quad_p(double x, double y, double rx, double ry)
{
    QPolygonF p;
    p << QPointF(x - rx, y - ry);
    p << QPointF(x - rx, y + ry);
    p << QPointF(x + rx, y + ry);
    p << QPointF(x + rx, y - ry);
    return p;
}


QPolygonF get_quad_p(double x, double y, double r)
{
    QPolygonF p;
    p << QPointF(x - r, y - r);
    p << QPointF(x - r, y + r);
    p << QPointF(x + r, y + r);
    p << QPointF(x + r, y - r);
    return p;
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


void draw_tri(QPainter *painter, QPointF pt, double r, double h, int col)
{
    QPolygonF p = get_tri_p(pt.x(), pt.y(), r);
    QPolygonF p2 = get_tri_p(pt.x(), pt.y(), r - h);

    p[3] = p[0];
    p[4] = p2[0]; p[5] = p2[2]; p[6] = p2[1]; p[7] = p2[0];


    p << p[0];
    p << p2[0];
    p << p2[2];
    p << p2[1];
    p << p2[0];

    painter->setPen(QPen(QColor(col), 0));
    painter->setBrush(QBrush(QColor(col)));
    painter->drawPolygon(p);

    painter->setBrush(QBrush(QColor(0xFFFFFF)));
    painter->drawPolygon(p2);
}


void draw_quad(QPainter *painter, QPointF pt, double r, double h, int col)
{
    QPolygonF p = get_quad_p(pt.x(), pt.y(), r);
    QPolygonF p2 = get_quad_p(pt.x(), pt.y(), r - h);

    p << p[0];
    p << p2[0];
    p << p2[3];
    p << p2[2];
    p << p2[1];
    p << p2[0];

    painter->setPen(QPen(QColor(col), 0));
    painter->setBrush(QBrush(QColor(col)));
    painter->drawPolygon(p);

    painter->setBrush(QBrush(QColor(0xFFFFFF)));
    painter->drawPolygon(p2);
}




void draw_shurf(QPainter *painter, QPointF pt, int coll, int coll2, double m_h)
{
    int id2 = getIDColor(coll2);

//    int col2 = coll2;
    int col2 = WHITE;

    switch (id2) {
    case 1: col2 = LIGHTRED; break;
    case 2: col2 = 0x8D8D8F; break;
    case 3: col2 = 0x00A5FF; break;
    }

    int col = getColorBorder(coll);

    double rx = 10 * m_h, ry = 7 * m_h, h = 1 * m_h;

    QPolygonF p = get_quad_p(pt.x(), pt.y(), rx, ry);
    QPolygonF p2 = get_quad_p(pt.x(), pt.y(), rx - h, ry - h);

    p << p[0];
    p << p2[0];
    p << p2[3];
    p << p2[2];
    p << p2[1];
    p << p2[0];

//    setcolor(m_dc, col);
//    setfillstyle(m_dc, 0, col);
    painter->setPen(QPen(QColor(col), 0));
    painter->setBrush(QBrush(QColor(col)));

//    fillpoly(m_dc, 10, p);
    painter->drawPolygon(p);
//    setfillstyle(m_dc, 0, 0xFFFFFF);
    painter->setPen(QPen(QColor(0xFFFFFF), 0));
    painter->setBrush(QBrush(QColor(0xFFFFFF)));

//    fillpoly(m_dc, 4, p2);
    painter->drawPolygon(p2);

    QPolygonF p3;
    p3 << p2[0];
    p3 << p2[1];
    p3 << p2[2];

//    setfillstyle(m_dc, 0, col2);
//    fillpoly(m_dc, 3, p3);
    painter->setPen(QPen(QColor(col2), 0));
    painter->setBrush(QBrush(QColor(col2)));
    painter->drawPolygon(p3);

    QPolygonF p4;
    p4 << p2[1];
    p4 << p2[2];
    p4 << p2[3];

//    setfillstyle(m_dc, 0, WHITE);
//    fillpoly(m_dc, 3, p4);
    painter->setPen(QPen(QColor(WHITE), 0));
    painter->setBrush(QBrush(QColor(WHITE)));
    painter->drawPolygon(p4);
}

void draw_indicator(QPainter *painter, QPointF pt, double m_h, int coll, int coll2)
{
//    setline(m_dc, col, 2);
//    setfillstyle(m_dc, 0, 0xFFFFFF);

    painter->setPen(QPen(QColor(coll), 2));
    painter->setBrush(QBrush(QColor(WHITE)));

    int r = 8 * m_h;

//    fillellipse(m_dc, pt.x, pt.y, r, r);
    painter->drawEllipse(QPointF(pt.x(), pt.y()), r, r);
    r = 1 * m_h;
//    fillellipse(m_dc, pt.x, pt.y, r, r);
    painter->drawEllipse(QPointF(pt.x(), pt.y()), r, r);

    r = 8 * m_h;
    int r2 = 4 * m_h;
    int r3 = 12 * m_h;

    int po = 1;

    if (po == 3) {  // Обратка
        r3 = -r3;
        r2 = -r2;
        r = -r;
    }

    painter->drawLine(QPointF(pt.x(), pt.y() - r2), QPointF(pt.x(), pt.y() - r3));
    painter->drawLine(QPointF(pt.x() - r, pt.y() - r3), QPointF(pt.x() + r, pt.y() - r3));
}



void draw_defect(QPainter *painter, QPointF pt, double r, double h, int coll, int coll2)
{
    int col2 = WHITE;
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

    int x = pt.x();
    int y = pt.y();

    QPolygonF p = get_tri_p(pt.x(), pt.y(), r);
    QPolygonF p2 = get_tri_p(pt.x(), pt.y(), r - h);

    p << p[0];
    p << p2[0];
    p << p2[2];
    p << p2[1];
    p << p2[0];

/*
    p[3] = p[0];
    p[4] = p2[0];
    p[5] = p2[2];
    p[6] = p2[1];
    p[7] = p2[0];
*/

//    setcolor(m_dc, col);
//    setfillstyle(m_dc, 0, col);
//    fillpoly(m_dc, 8, p);

    painter->setPen(QPen(QColor(col), 0));
    painter->setBrush(QBrush(QColor(col)));
    painter->drawPolygon(p);
//    setfillstyle(m_dc, 0, col2);
//    fillpoly(m_dc, 3, p2);

    painter->setPen(QPen(QColor(col2), 0));
    painter->setBrush(QBrush(QColor(col2)));
    painter->drawPolygon(p2);
}
