#include <QtGui>
#include <QtWidgets>

#include "std.h"

#include "wexample.h"


WExample::WExample(QWidget *parent)
    : QWidget(parent)
{
}

void WExample::paintEvent(QPaintEvent * /* event */)
{
  QPainter painter(this);

  static const QPointF points[4] = {
     QPointF(10.0, 80.0),
     QPointF(20.0, 10.0),
     QPointF(80.0, 30.0),
     QPointF(90.0, 70.0)
  };

  painter.setPen(QPen(colorL, lwidth));
  painter.setBrush(QBrush(QColor(colorB)));  

  painter.drawPolygon(points, 4);
}
