#pragma once

#include "std.h"
#include <QWidget>


class WExample : public QWidget
{
    Q_OBJECT

public:
    WExample(QWidget *parent = 0);
//    ~WExample();

  void set_colorL(unsigned long color)
  {
    colorL = QColor(color);
    repaint();
  }
  void set_colorB(unsigned long color)
  {
    colorB = QColor(color);
    repaint();
  }

  void setW(unsigned long _colorL, unsigned long _colorB, double _lwidth)
  {
    colorL = QColor(_colorL);
    colorB = QColor(_colorB);
    lwidth = _lwidth;
    repaint();
  }

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *event);

private:
  QColor colorL, colorB;
  double lwidth;

};
