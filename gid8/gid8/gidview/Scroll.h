#pragma once

#include "std.h"
#include <QDateTime>
#include <QWidget>
//#include <QOpenGLWidget>
#include <QResizeEvent>

#include <cxema/coordlis.h>

class QToolBar;
class QMenu;
class CFPoint;
class CFRect;
class QTimer;
struct GID_STYLE;

double getStdMas();
void setStdMas(double MasPic);
double getMasPodp();
void setMasPodp(double MasPodp);


struct ScrollGeom
{
    double bx, by, masx, masy, dmas;
};


class Scroll : 
#ifdef OPENGL
public QOpenGLWidget
#else
public QWidget
#endif
{
    Q_OBJECT

public:
    Scroll(QWidget *parent = 0);
//    ~Scroll();
    void draw_painter(QPainter &painter);


signals:

public slots:
    void onF5();

protected:
    void paintEvent(QPaintEvent *event);

    void resizeEvent( QResizeEvent * event ) {
//        resizeEvent(event);
       if (!event->isAccepted()) return;
       backBuffer = QPixmap( event->size() );
     }
    
    virtual void draw(QPainter *painter) = 0;
    virtual void draw(QPainter *painter, double w, double h, const ScrollGeom &geom1) {};


    virtual void XDraw(QPainter *painter, double dx, double dy) {};
    
    void drawToPixmap(QPixmap &backBuffer, double MasPic);

    void copyBuffer(CFRect rect);
    bool setRect(CFRect rect);

    void ScrollWindow(int dx, int dy);
    bool moveXY(const CFPoint& pt);
//    bool moveRect(const CFRect& rect);

//    QPainter *beginPaintX();
//    QPainter *beginPaint();
//    void endPaint(QPainter *painter);
    void repaintBackBuffer(int dx = 0, int dy = 0);

    double lfHeight() { return  m_lfHeight;};;
    void init_draw(QPainter *painter, double stdmas);

    void drawToBuffer();


    CFPoint ScreenToCoord(const CFPoint& point) {
      return CFPoint(geom.masx*(geom.bx+point.x), geom.masy*(geom.by+point.y));
    };
    CFPoint CoordToScreen(const CFPoint& point) {
      return CFPoint(point.x/geom.masx-geom.bx, point.y/geom.masy-geom.by);
    };

    QPoint CoordToQPoint(const CFPoint& point) {
      return QPoint(point.x/geom.masx-geom.bx, point.y/geom.masy-geom.by);
    };

    QPointF CoordToQPointF(const CFPoint& point) {
      return QPointF(point.x/geom.masx-geom.bx, point.y/geom.masy-geom.by);
    };

    CFPoint m_pt;
    CFPoint m_pt1;
    CFPoint m_pt2;
    CCoordList m_NP;
    int m_NP_loc;

    bool m_mouse_press;
    
    int m_dX;

    int m_dx_pix, m_dy_pix;

    double mas_otn;
    double mas_1;
    double m_h;   // Толщина линии

    QColor m_bk_color;

    bool m_isMouseMoved;

protected:
    // Флаги
    bool m_bIsMas = false;

    QFont m_font1;
    QFont m_font2;

    double m_scale = 1.0;
    bool m_reread = false;

private:
    QPixmap backBuffer;
    bool bOnlyCopyBackBuffer;
    double m_lfHeight;

public:
//    bool moveXY(const CFPoint& pt);

    CFRect ScreenToCoord(const CFRect& rect);
    CFRect CoordToScreen(const CFRect& rect);

    QTimer* scaleTimer;
    bool scaling = false;
    int scalingTime = 300;
    
    ScrollGeom geom;
    ScrollGeom geom_old;
  
    QVector<QMenu*> m_menus;
    QVector<QToolBar*> m_bars;
};

