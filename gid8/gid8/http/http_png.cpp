#include <QApplication>
#include <QtGui>

#include <mainwindow.h>
#include "geodez/geodez.h"


extern MainWindow *main_window;

const int DD = 256;

CFPoint rotatePoint(const CFPoint& p, double theta) {
    CFPoint rotated;
    rotated.x = p.x * std::cos(theta) - p.y * std::sin(theta);
    rotated.y = p.x * std::sin(theta) + p.y * std::cos(theta);
    return rotated;
}


SystemCoord *get_system_coord();

void m2m_42_WGS84(SystemCoord& system_coord, double X1, double Y1, double &xx, double &yy, bool shift);


// Для html

CFPoint get_m_to_coord2(double x, double y)
{
    CFPoint pt;
    double yy1, xx1, X1, Y1;

    yy1 = x * 100000;
    xx1 = y * 100000;
    SystemCoord *system_coord = get_system_coord();

    m2m_42_WGS84(*system_coord, xx1, yy1, X1, Y1, true);

    pt.x = Y1 * 100;
    pt.y = -X1 * 100;

    return pt;
}



CFPoint tile_coord1(int zz, int xx, int yy)
{
    double n2 = pow(2., zz);

    double x1 = (xx*360./n2-180.);
    double y1 = -(yy*360./n2-180.);

    CFPoint pt = get_m_to_coord2(x1, y1);

    /*
    double yy1 = x1 * 100000, X1;
    double xx1 = y1 * 100000, Y1;

    m2m2(m_system_coord, xx1, yy1, X1, Y1, true);

    pt.x = Y1 * 100;
    pt.y = -X1 * 100;
*/
    return pt;
}



void tile_coord(int dx, int dy, double dmas, int zz, int xx, int yy, double &masx, double &masy, double &theta, CFRect &r)
{
    CFPoint pt1 = tile_coord1(zz, xx, yy);
    CFPoint pt2 = tile_coord1(zz, xx + dx, yy);
    CFPoint pt3 = tile_coord1(zz, xx + dx, yy + dy);
    CFPoint pt4 = tile_coord1(zz, xx, yy + dy);

    theta = atan2(pt2.y - pt1.y, pt2.x - pt1.x);

    CFPoint pt0 = {(pt1.x+pt2.x+pt3.x+pt4.x)/4, (pt1.y+pt2.y+pt3.y+pt4.y)/4};

    CFPoint pts[4];

    pts[0] = pt1-pt0;
    pts[1] = pt2-pt0;
    pts[2] = pt3-pt0;
    pts[3] = pt4-pt0;

    for (int i = 0; i < 4; i++) {
        pts[i] = rotatePoint(pts[i], theta);
    }

    double x1 = (pt1.x + pt4.x) / 2;
    double x2 = (pt2.x + pt3.x) / 2;

    double y1 = (pt1.y + pt2.y) / 2;
    double y2 = (pt3.y + pt4.y) / 2;


    masx = (x2 - x1) / DD / dx;
    masy = (y2 - y1) / DD / dy;

    r = { x1, y1, x2, y2 };
}


// Получить номер по координатам

//void coord_tile1(int dx, int dy, double dmas, int zz, int xx, int yy, double& masx, double& masy, double& theta, CFRect& r)

void coord_tile1(int dx, int dy, double dmas, int zz, double x, double y, int &xx, int &yy)
{

    double n2 = pow(2., zz);

    double x1 = (xx * 360. / n2 - 180.);
    double y1 = -(yy * 360. / n2 - 180.);

    CFPoint pt = get_m_to_coord2(x1, y1);

/*

    CFPoint pt1 = tile_coord1(zz, xx, yy);
    CFPoint pt2 = tile_coord1(zz, xx + dx, yy);
    CFPoint pt3 = tile_coord1(zz, xx + dx, yy + dy);
    CFPoint pt4 = tile_coord1(zz, xx, yy + dy);

    theta = atan2(pt2.y - pt1.y, pt2.x - pt1.x);

    CFPoint pt0 = { (pt1.x + pt2.x + pt3.x + pt4.x) / 4, (pt1.y + pt2.y + pt3.y + pt4.y) / 4 };

    CFPoint pts[4];

    pts[0] = pt1 - pt0;
    pts[1] = pt2 - pt0;
    pts[2] = pt3 - pt0;
    pts[3] = pt4 - pt0;

    for (int i = 0; i < 4; i++) {
        pts[i] = rotatePoint(pts[i], theta);
    }

    double x1 = (pt1.x + pt4.x) / 2;
    double x2 = (pt2.x + pt3.x) / 2;

    double y1 = (pt1.y + pt2.y) / 2;
    double y2 = (pt3.y + pt4.y) / 2;


    masx = (x2 - x1) / DD / dx;
    masy = (y2 - y1) / DD / dy;

    r = { x1, y1, x2, y2 };
*/
}



bool http_make_png(const QString &fn1, int zz, int yy, int xx, int internalNodeID, int layer, int n_layers, int *layers, int gid, int geo, int ms, int rs)
{

/*

    QImage image(800, 600, QImage::Format_RGB32);

    // Устанавливаем цвет фона (например, белый)
    image.fill(Qt::white);

    // Создаем QPainter для рисования на изображении
    QPainter painter(&image);

    // Настраиваем кисть и перо
    QPen pen(Qt::black);
    pen.setWidth(3);
    painter.setPen(pen);

    // Пример рисования прямоугольника
    painter.drawRect(100, 100, 600, 400);

    // Прекращаем работу с QPainter (обязательно вызывать после завершения рисования)
    painter.end();

    // Сохраняем изображение в PNG файл
    image.save(fn1);
 */

    QImage image(256, 256, QImage::Format_RGB32);
    QPainter painter(&image);


    int dx = 1, dy = 1;

//    main_window->view()->draw_painter(painter);

    double bx, by, masx, masy, dmas = 96;

    double alpha;

    CFRect r;
    tile_coord(dx, dy, dmas, zz, xx, yy, masx, masy, alpha, r);

    bx = r.left / masx;
    by = r.top / masy;

    ScrollGeom geom = {bx, by, masx, masy, dmas};

    main_window->view()->draw(&painter, 2560, 2560, geom);

    painter.end();

    mkdir_for_file(fn1);
    image.save(fn1);
    
    return true;
}
