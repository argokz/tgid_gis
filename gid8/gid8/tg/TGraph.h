#pragma once

#include "std.h"
#include <QWidget>
#include <QSqlQuery>

struct GR_PARAM2
{
  double * x,*y;
//  CPen * pen; 
  int pen;
  int kol;
  bool show;
};


class TGraph : public QWidget
{
    Q_OBJECT

public:
    TGraph(QWidget *parent = 0);
//    ~TGraph();

    void init0(QSqlDatabase &db, int id);

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

    void copyToClipboard();


signals:

public slots:

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

    void DrawLineDotted(QPainter &painter, GR_PARAM2& gr);
    void DrawLine(QPainter &painter, GR_PARAM2 &gr);
    void DrawEdge(QPainter &painter);

    void InitArray();
    void InitArrayOT(QSqlDatabase & db, QSqlQuery &query);

    void MasShX(double min, double max, short kol, double *mas);
    void MasShY(double min, double max, short kol, double *mas);

public:
  bool  m_ch1 = true;
  bool  m_ch2 = true;
  bool  m_ch3 = true;
  bool  m_ch4 = true;
  bool  m_ch5 = true;

private:
  QColor colorL, colorB;
  double lwidth;

  int llx,lly;
  double stepX, stepY;
  short nGridX, nGridY;

  short lX, lY, rX, rY;
  double minX, maxX, minY, maxY;
  double masX, masY;


  bool showGraphOT;

  int m_id;

  GR_PARAM2 array1[4];
  GR_PARAM2 arrayOT[4];

};
