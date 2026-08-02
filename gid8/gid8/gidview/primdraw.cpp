#pragma warning(disable:4838)

#include "QtGui"
//#include "colorgtd.h"
//#include "typs.h"

#include "GidWidget.h"
#include "colorgtd.h"

#define max(x,y) (x > y) ? x : y;
#define min(x,y) (x < y) ? x : y;

//int m_ring = 1;

//#define BLACK 0
//#define LIGHTGRAY 0

const int *get_pics_map(int typ);




QColor rgb_l(QColor color, int tr, QColor bk_color);
bool LiangBar(int XS1, int YS1, int XS2, int YS2, double& x0, double& y0, double& x1, double& y1);


#define X1 9998000
#define X2 9997000
#define Y1 9996000
#define Y2 9995000


void drawArc(QPainter *painter, double xx1, double yy1, double xx2, double yy2, double xx3, double yy3, double xx4, double yy4) 
{
    // Прямоугольник, задающий эллипс
    QRectF rect(xx1, yy1, xx2 - xx1, yy2 - yy1);

    // Центр эллипса
    QPointF center = rect.center();

    // Вычисление начального и конечного углов
    double startAngle = std::atan2(yy3 - center.y(), xx3 - center.x()) * 180 / M_PI;
    double endAngle = std::atan2(yy4 - center.y(), xx4 - center.x()) * 180 / M_PI;

    // Угол размаха
    double spanAngle = endAngle - startAngle;
    if (spanAngle < 0) {
        spanAngle += 360;
//        startAngle = endAngle;
    }

    // Преобразование углов в шестнадцатые доли градуса для Qt
    int startAngle16 = static_cast<int>(startAngle * 16);
    int spanAngle16 = static_cast<int>(spanAngle * 16);

    // Рисование арки
    painter->drawArc(rect, startAngle16, spanAngle16);
}

void chXY(double &x, double &y, double x1, double y1, double x2, double y2, double mas, double dx, double dy)
{
  double xx, yy, x0, y0;

  if (X1-1000 < x && x < X1+1000) {
    x0 = x1;
    xx = x-X1;
  }
  else if (X2-1000 < x && x < X2+1000) {
    x0 = x2;
    xx = x-X2;
  }
  else {
    x0 = (x1+x2)/2;
    xx = x;
  }

  if (Y1-1000 < y && y < Y1+1000) {
    y0 = y1;
    yy = y-Y1;
  }
  else if (Y2-1000 < y && y < Y2+1000) {
    y0 = y2;
    yy = y-Y2;
  }
  else {
    y0 = (y1+y2)/2;
    yy = y;
  }

  xx /= mas;
  yy /= mas;

  x = x0 + dy*yy + dx*xx;
  y = y0 - dx*yy + dy*xx;
}

//void GidWidget::drawLinePic(QPainter *painter, int thickness, double x1, double y1, double x2, double y2, QColor color, QColor colorF, QColor bk_color,  const int *pics, double mas_otn, bool no_mas)


// colorT - яркий свет

void drawLinePic1(QPainter *painter, int thickness, double x1, double y1, double x2, double y2, QColor color, QColor colorT, QColor colorF, QColor m_bk_color, const int *pics, double mas_otn, bool no_mas, double m_h, double m_ring)
{
  double dd, dx, dy, x, y, x01, y01, y02, x02;
  double xx1, yy1, xx2, yy2, xx3, yy3, xx4, yy4;
  double xx, yy, r;
  int i, t, ip, c1, c2;
  QPointF fill[256];
//  double thickness3 = 0.5*m_h;
  double thickness3 = 0.5;

  if (!pics) return;

  
  QColor c_color = color;
  double mas_otn2 = mas_otn/m_h;
  double mas = 1.;

  if (m_ring != 0) mas = 1. / m_ring;

  unsigned int m_color_uzel = WHITE;

//  double mas = 1./m_h/m_ring;
//  double mas = 1./m_h/m_ring;

  x01 = x1, y01 = y1, y02 = y2, x02 = x2;

  x = (x01+x02)/2;
  y = (y01+y02)/2;
  dd = hypot(x02-x01, y02-y01);  
  
  if ((no_mas && dd <= 0.) || (!no_mas && dd < 1.)) return;

  dx = (x02-x01)/dd/mas;
  dy = (y02-y01)/dd/mas;

  dx = (x02-x01)/dd;
  dy = (y02-y01)/dd;

  ip = 0;

  QPen pen(color, thickness3/mas_otn);
  QBrush brush = QBrush(color);

  painter->setBrush(brush);
  painter->setPen(pen);

  while (1) {
    t = pics[ip++];

    if (t == 'q') break;

    switch (t)
    {
    case 'f' :
    case 'l' :
      i = 0;
      while (true) {
        xx = pics[ip++]; if (xx == 9999) break;
        yy = pics[ip++];
        chXY(xx, yy, x01, y01, x02, y02, mas_otn2*mas, dx, dy);
        fill[i++] = QPointF(xx, yy);
      }
      fill[i++] = fill[0];

      painter->setPen(QPen(c_color, thickness3/mas_otn+0));

      if (t == 'f') {
          painter->setPen(QPen(c_color, 0));
          painter->setBrush(brush);
          painter->drawPolygon(fill, i);
      }
      else {
          painter->setPen(pen);
          painter->drawPolyline(fill, i);
      }
      break;

    case 'a' :
        xx = pics[ip++]/mas;
        yy = pics[ip++]/mas;
          xx /= mas_otn2;
          yy /= mas_otn2;
        painter->setPen(pen);
        painter->drawLine(x1, y1, x + dy*yy + dx*xx, y - dx*yy + dy*xx);
        break;

    case 'b' :
        xx = pics[ip++]/mas;
        yy = pics[ip++]/mas;

          xx /= mas_otn2;
          yy /= mas_otn2;

        painter->setPen(pen);
        painter->drawLine(x2, y2, x + dy*yy + dx*xx, y - dx*yy + dy*xx);
      break;

    case 'k' :  // Круг
        xx = pics[ip++];
        yy = pics[ip++];

        chXY(xx, yy, x01, y01, x02, y02, mas_otn2*mas, dx, dy);

        r = pics[ip++];
        r /= mas_otn2;

        painter->setBrush(brush);
        painter->drawEllipse(QPointF(xx, yy), r/mas, r/mas);
        break;

    case 'r' : //  Окружность
        xx = pics[ip++];
        yy = pics[ip++];

        chXY(xx, yy, x01, y01, x02, y02, mas_otn2*mas, dx, dy);

        r = pics[ip++];
          r /= mas_otn2;
        painter->setPen(pen);
        painter->drawEllipse(QPointF(xx, yy), r/mas, r/mas);
        break;
#if 0
    case 'c' :   // Цвет
      c1 = pics[ip++];
      c2 = pics[ip++];
      switch (c1) {
      case 'C' :  {
        c_color = rgb_l(color, 30, 0xFFFFFF);  break;
      }

      case 'c' :  c_color = color;  break;
      case 'b' :  c_color = Qt::black;  break;
      case 'w' :  c_color = bk_color;  break;
//      case 'x' :  setfillstyle(m_dc, 1, colorF);  break;
      case 'x' :  
        painter->setBrush(QBrush(QColor(colorF)));
       break;
      }
      switch (c2) {
      case 'C' :  
        {
//          setfillstyle(m_dc, 1, rgb_l(color, 30, 0xFFFFFF));
        }
        break;
      case 'c' : painter->setBrush(QBrush(QColor(color)));    break;
      case 'b' : painter->setBrush(QBrush(Qt::black));        break;
      case 'w' : painter->setBrush(QBrush(QColor(bk_color))); break;
      case 'x' : painter->setBrush(QBrush(QColor(colorF)));   break;
      }
      break;
#else
    case 'c' :   // Цвет
      c1 = pics[ip++];
      c2 = pics[ip++];
      switch (c1) {
      case 'C' :
          pen = QPen(color, thickness3+0);
          break;
      case 'c' :
          brush = QBrush(QColor(color));
          break;
      case 'b' :
          pen = QPen(Qt::black, thickness3+0);
          break;
      case 'w' :
          pen = QPen(m_bk_color, thickness3+0);
          break;
      case 'x' :
          pen = QPen(QColor(LIGHTCYAN), thickness3+0);
          break;
      case 'y' :
          pen = QPen(QColor(YELLOW), thickness3+0);
          break;
      }
      switch (c2) {
      case 'C' :
          brush = QBrush(QColor(0xFFFFFF));
          break;

      case 'T' :
          brush = QBrush(QColor(colorT));
          break;
      
      case 'c' :
          brush = QBrush(QColor(color));
          break;
      case 'b' :
          brush = QBrush(QColor(BLACK));
          break;
      case 'w' :
          brush = QBrush(QColor(m_bk_color));
          break;
      case 'x' :
          brush = QBrush(QColor(m_color_uzel));
          break;
      case 'y' :
          brush = QBrush(QColor(YELLOW));
          break;
      }
      break;


#endif



    case 'd' :   // Дуга
      xx = pics[ip++];
      yy = pics[ip++];

      chXY(xx, yy, x01, y01, x02, y02, mas_otn2*mas, dx, dy);

      r = pics[ip++];
        r /= mas_otn2;

      xx1 = xx-r;
      yy1 = yy-r;
      xx2 = xx+r;
      yy2 = yy+r;

      xx3 = pics[ip++];
      yy3 = pics[ip++];

      chXY(xx3, yy3, x01, y01, x02, y02, mas_otn2*mas, dx, dy);

      xx4 = pics[ip++];
      yy4 = pics[ip++];

      chXY(xx4, yy4, x01, y01, x02, y02, mas_otn2*mas, dx, dy);

      painter->setPen(pen);

      drawArc(painter, xx1, yy1, xx2, yy2, xx3, yy3, xx4, yy4);
//      painter->drawArc(xx1, yy1, xx2, yy2, xx3, yy3, xx4, yy4);

      break;
    }
  }
}

/*
void GidWidget::drawLinePic(QPainter *painter, int thickness, double x1, double y1, double x2, double y2, QColor color, QColor colorF, QColor bk_color,  const int *pics, double mas_otn, bool no_mas)
{
    drawLinePic1(painter, thickness, x1, y1, x2, y2, color, colorF, bk_color, pics, mas_otn, no_mas, m_h);
}
*/


int sizeLineTyp(const int *pics)
{
  double xx, yy, r;
  double x1 = 100000, x2 = -100000;
  int i, t, ip;
//  double len = 0;

  ip = 0;

  while (1) {
    t = pics[ip++];

    if (t == 'q') break;

    switch (t)
    {
    case 'f' :
    case 'l' :
      i = 0;
      while (true) {
        xx = pics[ip++]; if (xx == 9999) break;
        yy = pics[ip++];
        chXY(xx, yy, 0, 0, 0, 0, 1, 1, 0);
        x1 = min(x1, xx);
        x2 = max(x2, xx);
      }
      break;

    case 'a' :
    case 'b' :
    case 'c' :   // Цвет
      ip +=2;
      break;

    case 'k' :  // Круг
    case 'r' : //  Окружность
      xx = pics[ip++];
      yy = pics[ip++];

      chXY(xx, yy, 0, 0, 0, 0, 1, 1, 0);
      r = pics[ip++];
      x1 = min(x1, xx-r);
      x2 = max(x2, xx+r);
      break;
    }
  }

  return x2-0;
//  return x2-x1;
}

#include "primdrawline.h"

//const int *get_pics(int typ, GID_STYLE style, double coef)
const int *get_pics(int typ, double coef)
{
  int *pics;

  pics = picUT;

  switch (typ) {

  case TIP_V_UT:   pics = picUT;    break;
  
  case TIP_UT:   pics = picUT;    break;
  case TIP_HC:   pics = picNS;    break;
  case TIP_RD:   pics = picRD;    break;
  case TIP_RR:   pics = picRR;    break;
  case TIP_RP:   pics = picRD;    break;
  case TIP_OK:   pics = picOK;    break;
  case TIP_ZD:   pics = picZD;    break;
  case TIP_ZD2:  pics = picZD2;    break;
  case TIP_BP:   pics = picVENT;  break;
  case TIP_DR:   pics = picDR;    break;
  case TIP_EL:   pics = picEL;    break;
  case TIP_TO:   pics = picTO;    break;
  case TIP_KU:   pics = picKU;    break;
  case TIP_RO:   pics = picRO;    break;

  case 55:   pics = picDR_zakr;   break;
  case 56:   pics = picZDZAKR;    break;
  case 156:  pics = picZD2;    break;
  case 57:   pics = picZD57;      break;

  case 58:   pics = picDREN;      break;
  case 59:   pics = picZD1;       break;

  case 60:   pics = picVOZD;      break;

  case 61:   pics = picOPORA;     break;
  case 62:   pics = picKompVert;  break;

//  case 63:   pics = picRING1;      break; ////

  case 64:   pics = picKompSaln;  break;
  case 65:   pics = picKompSilf;  break;
  case 66:   pics = picPER;       break;
  case 67:   pics = picCIRCLE;    break;
  case 68:   pics = picZD0;       break;
  case 69:   pics = picSPUSK;      break;


  case 70:   pics = picRING1;      break;
  case 71:   pics = picRING2;      break;
  case 72:   pics = picROMB;      break;

  case 73:   pics = picFLAG;      break;


//  case 74:   pics = picFLAG;      break;
//  case 75:   pics = picFLAG;      break;



  case 99:   pics = picCIRCLE0;   break;

  default :  pics = nullptr;      break;
  }
  return pics;
}

/*
static double mas_otn = 1.;
static double mas_1 = 1.;
static double m_h = 1.;
*/
static double len_dot = 0;

#include <QLineF>
#include <QRectF>

#include <optional>
#include <QLineF>
#include <QRectF>
#include <QVector>

inline std::optional<QLineF> clipLineToRect(const QLineF& line, const QRectF& rect)
{
    QVector<QPointF> pts;

    auto check = [&](QPointF a, QPointF b) {
        QLineF border(a, b);
        QPointF ip;
//#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
//        if (line.intersect(border, &ip) == QLineF::BoundedIntersection)
//            pts << ip;
//#else
        if (line.intersects(border, &ip) == QLineF::BoundedIntersection)
            pts << ip;
//#endif
    };

    // проверяем все стороны прямоугольника
    check(rect.topLeft(), rect.topRight());
    check(rect.topRight(), rect.bottomRight());
    check(rect.bottomRight(), rect.bottomLeft());
    check(rect.bottomLeft(), rect.topLeft());

    // добавляем концы линии, если они внутри прямоугольника
    if (rect.contains(line.p1())) pts << line.p1();
    if (rect.contains(line.p2())) pts << line.p2();

    if (pts.size() >= 2)
        return QLineF(pts[0], pts[1]);

    return std::nullopt; // линия не попадает в окно
}




void GidWidget::drawLineTyp(QPainter *painter, double thickness, double x1, double y1, double x2, double y2, int typ, QColor color, QColor colorT, QColor colorF, int ris, bool punct, bool napr, bool strelka, bool isQ, bool no_map) 
{
    if (!napr && (typ == TIP_UT || typ == TIP_V_UT)) {
        double x, y;
        x = x1; x1 = x2; x2 = x;
        y = y1; y1 = y2; y2 = y;
    }

    if (ris == 1 && typ == TIP_UT && !isQ) {
        return;
    }

    double x01 = x1, y01 = y1, y02 = y2, x02 = x2;

//  if (!LiangBar(m_rect.left-20/masx, m_rect.top-20/masy,
//                m_rect.right+20/masx, m_rect.bottom+20/masy,
//          x1, y1, x2, y2)) return;

    if (x1 == x2 && y1 == y2) return;

    QLineF line(QPointF(x1, y1), QPointF(x2, y2));
    QRectF rect(0, 0, width(), height());

    auto clipped = clipLineToRect(line, rect);

    if (!clipped) return;


    double main_thickness = thickness/mas_otn;

 //  if (main_thickness < thickness && thickness < 4) main_thickness = thickness;
    if (main_thickness < 1) main_thickness = 1;

    if (typ < 0) {
//    setline(m_dc, LIGHTGRAY, main_thickness);
//    setline(m_dc, LIGHTGRAY, main_thickness);

        painter->setPen(Qt::lightGray);
        painter->drawLine(x1, y1, x2, y2);

        return;
    }

      painter->setPen(QPen(color, main_thickness));



//  setline(m_dc, color, main_thickness);
//  setfillstyle(m_dc, 1, color);

    if (ris == -1 && typ != TIP_UT && typ != TIP_V_UT) {
///        painter->drawLine(x1, y1, x2, y2);
        painter->drawLine(clipped.value());
        return;
    }
    if (ris == 0 && typ != TIP_UT) {
//    line(x1, y1, x2, y2);
    }

    if (ris != 1 && (typ == TIP_UT || typ == TIP_V_UT)) {



    if (punct) {
        QPen pen = painter->pen();
//        pen.setStyle(Qt::DashLine);  // Пунктирная линия
        pen.setDashPattern({20, 5});

        painter->setPen(pen);

        painter->drawLine(x1, y1, x2, y2);
        pen.setStyle(Qt::SolidLine);
        painter->setPen(pen);

    }
    else {
//        painter->drawLine(x1, y1, x2, y2);
        painter->drawLine(QPointF(x1, y1), QPointF(x2, y2));
    }
    return;
  }

  int d = 4;
  double coef = 1;

  const int *pics = get_pics_map(typ);
  
  if (pics == NULL) {
    pics = get_pics(typ, coef);
  }

  if (pics == NULL) {
    pics = get_pics(0, coef);
  }

  if (pics == NULL) {
    return;
  }


  int dd = max(d*4, d*thickness);

  int picUT_NAPR[] = 
  {
    'f', (int)(-d+dd), (int)(d/2+thickness/2), (int)(0+dd), 0, (int)(-d+dd), (int)(-d/2-thickness/2),  9999,
    'q'
  };

  double len = hypot(x1-x2, y1-y2), minlen = 30*m_h;
  double mas_otn_old = mas_otn;

  minlen = sizeLineTyp(pics);

  if (typ == TIP_UT) minlen = sizeLineTyp(picUT_NAPR);

  if (!no_map && len*mas_otn < minlen) {
    mas_otn = minlen/len;
  }

//  QColor m_bk_color = Qt::white;

  if (typ != TIP_UT || strelka) {
    drawLinePic1(painter, thickness, x01, y01, x02, y02, color, colorT, colorF, m_bk_color, pics, mas_otn, no_map, m_h, m_ring);
  }
  if (typ != TIP_UT && strelka && isQ) {
    if (!napr) {
      double x, y;
      x = x01; x01 = x02; x02 = x;
      y = y01; y01 = y02; y02 = y;
    }
    drawLinePic1(painter, thickness, x01, y01, x02, y02, color, colorT, colorF, m_bk_color, picUT_NAPR, mas_otn, no_map, m_h, m_ring);
  }
  mas_otn = mas_otn_old;
}


#if 0



void GidWidget::drawNodeTyp(QPainter *painter, double x, double y, int typ, QColor color) 
{
  if (typ < 0) {
    painter->setPen(QPen(Qt::black, 1./mas_otn));
    if (color == Qt::yellow) 
      painter->setBrush(QBrush(color));
    else
      painter->setBrush(QBrush(Qt::lightGray));

//    fillellipse(x, y, 4, 4);
    return;
  }

  QColor bk_color = painter->background().color();

  painter->setPen(QPen(color, 1./mas_otn));
  painter->setBrush(QBrush(color));

  QPointF fill[256];

//  int d = 4;

  int *pics[34];
  double mas = mas_1/m_h;
//  double mas = mas_1;

#include "primdrawnode.h"

  pics[TIP_US] = picUS;
  pics[TIP_PR] = picPR;
  pics[TIP_PO] = picPT;
  pics[TIP_UP] = picUP;
  pics[TIP_IS] = picIS;
  pics[TIP_HS] = picNS;
//  pics[TIP_ZP] = picZP;
  pics[TIP_US2] = picUS2;
  pics[TIP_PODP] = picPODP;
//  pics[TIP_PODP] = picKRAN;
//  pics[TIP_VODOR] = picVODOR;
  pics[TIP_VODOR] = picKRAN;
  pics[22] = picMN; // С внутренностями
  pics[23] = picZN; // Узел с заданным напором
  pics[24] = picEL; // С элеватором
  pics[25] = picKAM; // Камера
  pics[26] = picINVALID; // 
  pics[27] = picPR_El; // 
  pics[28] = picPR_Nez; // 

  pics[29] = picORP; // Объекты с измеряемыми параметрами

  double mas_otn2 = mas_otn;

  double xx, yy, r, xx1, yy1, xx2, yy2, xx3, yy3, xx4, yy4, xx0, yy0;

  int i, t, ip, c1, c2;

  ip = 0;

  while (1) {
    t = pics[typ][ip++];

    if (t == 'q') break;

    switch (t)
    {
    case 'f' :
    case 'l' :
      i = 0;
      while (true) {
        xx = pics[typ][ip++]; if (xx == 9999) break;
        yy = pics[typ][ip++];
        xx /= mas_otn2;
        yy /= mas_otn2;

        fill[i++] = QPointF(x + xx/mas, y + yy/mas);
      }
      fill[i++] = fill[0];

      if (t == 'f') {
//        painter->setBrush(Qt::SolidPattern);
        painter->drawPolygon(fill, i);
      }
      else {
//        painter->setBrush(Qt::NoBrush);
        painter->drawPolyline(fill, i);
      }
      break;

    case 'k' :  // Круг
      xx = pics[typ][ip++];
      yy = pics[typ][ip++];
      r = pics[typ][ip++];
      r /= mas_otn2;
      painter->drawEllipse(x + (xx-r)/mas, y + (yy-r)/mas, r*2/mas, r*2/mas);
//      fillellipse(x + xx/mas, y + yy/mas, r/mas, r/mas);
      break;
    case 'r' : //  Окружность
      xx = pics[typ][ip++];
      yy = pics[typ][ip++];
      r = pics[typ][ip++];
      r /= mas_otn2;
//      ellipse(x + xx/mas, y + yy/mas, r/mas, r/mas);
      painter->setBrush(Qt::NoBrush);
      painter->drawEllipse(x + (xx-r)/mas, y + (yy-r)/mas, r*2/mas, r*2/mas);
      break;
    case 'c' :   // Цвет
      c1 = pics[typ][ip++];
      c2 = pics[typ][ip++];
      switch (c1) {
      case 'c' :  painter->setPen(QPen(color, 1./mas_otn));  break;
      case 'b' :  painter->setPen(QPen(Qt::black, 1./mas_otn));  break;
      case 'w' :  painter->setPen(QPen(bk_color, 1./mas_otn));  break;
      case 'x' :  painter->setPen(QPen(Qt::cyan, 1./mas_otn));  break;
//      case 'x' :  painter->setBrush(QBrush(color*2));  break;
      }
      switch (c2) {
      case 'c' :  painter->setBrush(QBrush(color));  break;
      case 'b' :  painter->setBrush(QBrush(Qt::black));  break;
      case 'w' :  painter->setBrush(QBrush(bk_color));  break;
      case 'x' :  painter->setBrush(QBrush(Qt::lightGray));  break;
//      case 'x' :  painter->setBrush(QBrush(color*2));  break;
      }
      break;
    case 'd' :   // Дуга
      xx = pics[typ][ip++];
      yy = pics[typ][ip++];
      r = pics[typ][ip++]/mas;
      r /= mas_otn2/mas;

      xx0 = x + xx/mas;
      yy0 = y + yy/mas;

      xx1 = xx0-r;
      yy1 = yy0-r;
      xx2 = xx0+r;
      yy2 = yy0+r;

      xx3 = pics[typ][ip++]/mas;
      yy3 = pics[typ][ip++]/mas;
      xx4 = pics[typ][ip++]/mas;
      yy4 = pics[typ][ip++]/mas;

//      m_dc->Arc(xx1, yy1, xx2, yy2,
//        x + xx3, y + yy3, x + xx4, y + yy4);
      break;
    }
  }
}

#endif


void GidWidget::drawNodeNull(QPainter *painter, double x, double y, QColor color)
{
  double thickness3 = 1;

  painter->setPen(QPen(Qt::black, thickness3/mas_otn));
  painter->setBrush(QBrush(color));

  if (color.rgb() == YELLOW)
      painter->setBrush(QBrush(color));
  else
      painter->setBrush(QBrush(Qt::lightGray));

  painter->drawEllipse(QPointF(x, y), 4., 4.);
}


void GidWidget::drawNodeTyp(QPainter *painter, double x, double y, int typ, QColor color, double markerPx)
{
//  double thickness3 = 0.5*m_h;
  double thickness3 = 0.5;

  if (typ < 0 || typ >= 1024) {
    drawNodeNull(painter, x, y, color);
    return;
  }

  QColor c_color = color;

  QPen pen(color, thickness3/mas_otn);
  QBrush brush = QBrush(color);

  painter->setPen(pen);
  painter->setBrush(brush);

  QPointF fill[256];

  int d = 4;
  int i;

  int *pics[1024];

  for (i = 0; i < 1024; i++) pics[i] = NULL;

  if (m_ring == 0) m_ring = 1;

  double mas;
  if (markerPx > 0.0) {
      // Текущий масштаб мира по X (обычно достаточно; поворотов, как правило, нет)
      QTransform T = painter->worldTransform();
      double worldScale = T.m11();           // берём масштаб по X
      if (worldScale < 0) worldScale = -worldScale;
      if (worldScale < 1e-6) worldScale = 1e-6;

      // Хотим: экранное смещение = worldScale * (xx / mas) = xx * markerPx  →  mas = worldScale / markerPx
      // Так размер символа на экране ~ пропорционален markerPx (в пикселях).
      mas = worldScale / markerPx;

      // Если хочешь учитывать HiDPI, раскомментируй:
      const double dpr = painter->device() ? painter->device()->devicePixelRatioF() : 1.0;
      mas = (worldScale) / (markerPx * dpr);
  } else {
      // Старое поведение:
      mas = 1./m_h/m_ring;
  }

#include "primdrawnode.h"

  pics[TIP_US] = picUS;
  pics[TIP_PR] = picPR;
  pics[TIP_PO] = picPT;
  pics[TIP_UP] = picUP;
  pics[TIP_IS] = picIS;
  pics[TIP_HS] = picNS;
  pics[TIP_C3] = picC3;
//  pics[TIP_ZP] = picZP;
  pics[TIP_US2] = picUS2;
  pics[TIP_PODP] = picPODP;
//  pics[TIP_PODP] = picKRAN;
//  pics[TIP_VODOR] = picVODOR;
  pics[TIP_VODOR] = picKRAN;
  
  pics[TIP_V_PT] = picPR;
  pics[TIP_V_IS] = picIS;
  
  pics[22] = picMN; // С внутренностями
  pics[TIP3_ZN] = picZN; // Узел с заданным напором
  pics[24] = picEL; // С элеватором
  pics[25] = picKAM; // Камера
  pics[26] = picINVALID; // 
  pics[27] = picPR_El; // 
  pics[28] = picPR_Nez; // 

  pics[TIP3_ORP] = picORP; // Объекты с измеряемыми параметрами

  pics[30] = picPT_C;
  pics[31] = picPRIBOR; // 
  pics[32] = pic3; // Трехходовой

  pics[35] = picKvadrat;
  pics[36] = picKrug;
  pics[37] = picTri;
  pics[38] = picPT_C2;

  if (!pics[typ]) {
//    drawNodeNull(m_dc, x, y, color);
//    return;
    typ = TIP_US;
  }

  double mas_otn2 = mas_otn;

  double xx, yy, r, xx1, yy1, xx2, yy2, xx3, yy3, xx4, yy4, xx0, yy0;

  int t, ip, c1, c2;

  ip = 0;

  unsigned int m_color_uzel = WHITE;


  while (1) {
    t = pics[typ][ip++];

    if (t == 'q') break;

    switch (t)
    {
    case 'f' :
    case 'l' :
      i = 0;
      while (true) {
        xx = pics[typ][ip++]; if (xx == 9999) break;
        yy = pics[typ][ip++];
        xx /= mas_otn2;
        yy /= mas_otn2;

        fill[i++] = QPointF(x + xx/mas, y + yy/mas);
      }

      if (t == 'f') {
        painter->setPen(QPen(c_color, 0));
        painter->setBrush(brush);
        painter->drawPolygon(fill, i);
      }
      else {
          painter->setPen(pen);
          painter->drawPolyline(fill, i);
      }
      break;

    case 'k' :  // Круг
      xx = pics[typ][ip++];
      yy = pics[typ][ip++];

      xx /= mas_otn2;
      yy /= mas_otn2;
      
      r = pics[typ][ip++];
      r /= mas_otn2;
//      fillellipse(m_dc, x + xx/mas, y + yy/mas, r/mas, r/mas);
      painter->setBrush(brush);
      painter->drawEllipse(QPointF(x + xx/mas, y + yy/mas), r/mas, r/mas);
      break;
    case 'r' : //  Окружность
      xx = pics[typ][ip++];
      yy = pics[typ][ip++];
      r = pics[typ][ip++];
      r /= mas_otn2;
//      ellipse(m_dc, x + xx/mas, y + yy/mas, r/mas, r/mas);
      painter->setPen(pen);
      painter->drawEllipse(QPointF(x + xx/mas, y + yy/mas), r/mas, r/mas);
      break;
    case 'c' :   // Цвет
      c1 = pics[typ][ip++];
      c2 = pics[typ][ip++];
      switch (c1) {
      case 'C' :
          pen = QPen(color, thickness3/mas_otn);
          break;
      case 'c' :
          brush = QBrush(QColor(color));
          break;
      case 'b' :
          pen = QPen(Qt::black, thickness3+0);
          break;
      case 'w' :
          pen = QPen(m_bk_color, thickness3+0);
          break;
      case 'x' :
          pen = QPen(QColor(LIGHTCYAN), thickness3+0);
          break;
      case 'y' :
          pen = QPen(QColor(YELLOW), thickness3+0);
          break;
      }
      switch (c2) {
      case 'C' :
          brush = QBrush(QColor(rgb_l(color, 60, 0xFFFFFF)));
          break;
      case 'c' :
          brush = QBrush(QColor(color));
          break;
      case 'b' :
          brush = QBrush(QColor(BLACK));
          break;
      case 'w' :
          brush = QBrush(QColor(m_bk_color));
          break;
      case 'x' :
          brush = QBrush(QColor(m_color_uzel));
          break;
      case 'y' :
          brush = QBrush(QColor(YELLOW));
          break;
      }
      break;
    case 'd' :   // Дуга
      xx = pics[typ][ip++];
      yy = pics[typ][ip++];
      r = pics[typ][ip++]/mas;
      r /= mas_otn2/mas;

      xx0 = x + xx/mas;
      yy0 = y + yy/mas;

      xx1 = xx0-r;
      yy1 = yy0-r;
      xx2 = xx0+r;
      yy2 = yy0+r;

      xx3 = pics[typ][ip++]/mas;
      yy3 = pics[typ][ip++]/mas;
      xx4 = pics[typ][ip++]/mas;
      yy4 = pics[typ][ip++]/mas;

//      m_dc->Arc(xx1, yy1, xx2, yy2,
//        x + xx3, y + yy3, x + xx4, y + yy4);
      break;
    }
  }
}


void GidWidget::drawLineTypL(QPainter *painter, CLine2* l, int po, int redr, double thickness, double x1, double y1, double x2, double y2, int typ, QColor color, QColor colorT, QColor colorF, int ris, bool punct, bool napr, bool strelka, bool isQ)
{
    drawLineTyp(painter, thickness, x1, y1, x2, y2, typ, color, colorT, colorF, ris, punct, napr, strelka, isQ, false);

/*
    if (l) {
        if (m_szY > 4 && m_cxema->m_bIsGidrInf && !redr && ris == 1) {
            bool isOpcOnOff = !l->line.strOpc1.IsEmpty() && (l->line.typ == TIP_HC || l->line.typ == TIP_ZD || l->line.typ == TIP_ZD2);

            if (isOpcOnOff && !redr && ris == 1 && pOPCThread) {
                opc_var.push_back(OPCvar(l->line.strOpc1, l, po, x1, y1, x2, y2));
            }
        }
    }
*/
}
