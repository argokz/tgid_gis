#pragma once

#include <QCoreApplication>
#include <QDateTime>

#include <iostream>
#include <list>

using namespace std;

#include <float.h>
#include <math.h>

//#include "clip/polyio.h"

#define C_SPR LONG_MAX

bool isBreakCoord(int x);

struct CRect
{
  CRect() {
      left    = 0;
      top     = 0;
      right   = 0;
      bottom  = 0;
  }

  CRect(double _left, double _top, double _right, double _bottom)
  {
      left    = _left   ;
      top     = _top    ;
      right   = _right  ;
      bottom  = _bottom ;
  }

  double left, top, right, bottom;
};


struct POLYAREA;
struct PLINE;

class CFPoint
{
public:
  CFPoint(double xx = 0., double yy = 0., double hh = 0.) : x(xx), y(yy), h(hh) {};

//  bool operator==(CFPoint point) const;
  bool operator!=(CFPoint point) const;
//  void operator+=(SIZE size);
//  void operator-=(SIZE size);
  void operator+=(CFPoint point);
  void operator-=(CFPoint point);

// Operators returning CFPoint values
//  CFPoint operator+(SIZE size) const;
//  CFPoint operator-(SIZE size) const;
  CFPoint operator-() const;
  CFPoint operator+(CFPoint point) const
    {
      return CFPoint(x+point.x, y+point.y);
    };
  CFPoint operator-(CFPoint point) const
    {
      return CFPoint(x-point.x, y-point.y);
    };

// Operators returning CSize values
//  CSize operator-(CFPoint point) const;

// Operators returning CRect values
//  CRect operator+(const RECT* lpRect) const;
//  CRect operator-(const RECT* lpRect) const;

  bool operator==(const CFPoint& mc) const
    {
      return x == mc.x && y == mc.y;
    };
  bool operator!=(const CFPoint& mc) const
    {
      return x != mc.x || y != mc.y;
    };
  bool operator<(const CFPoint& mc) const
    {
      if (x < mc.x) return true;
      if (x > mc.x) return false;
      return y < mc.y;
    };

  bool IsNull() const
    {
      return x == 0. && y == 0.;
    };


  double x, y, h;
};



class CFRect
{
public :
  CFRect() : left(0.), top(0.), right(0.), bottom(0.) {};
//  CFRect(const CRect& r) : left(r.left), top(r.top), right(r.right), bottom(r.bottom) {};
  CFRect(double l, double t, double r, double b) : left(l), top(t), right(r), bottom(b) {};
  CFRect(CFPoint p1, CFPoint p2) : left(p1.x), top(p1.y), right(p2.x), bottom(p2.y) {};

  void SetRect(double l, double t, double r, double b) { left = l, top = t, right = r, bottom = b;};

  CFPoint TopLeft() const
    {return CFPoint(left, top);};
  CFPoint BottomRight() const
    {return CFPoint(right, bottom);};

  void AddPoint(CFPoint p) {
    if (left > right) {
      left = right = p.x;
      top = bottom = p.y;
      InflateRect(100, 100);
      return;
    }
    
    left = fmin(left, p.x);
    right = fmax(right, p.x);
    top = fmin(top, p.y);
    bottom = fmax(bottom, p.y);
  }

  bool IntersectRect(CFRect r1, CFRect r2)
  {
    left = fmax(r1.left, r2.left);
    right = fmin(r1.right, r2.right);
    top = fmax(r1.top, r2.top);
    bottom = fmin(r1.bottom, r2.bottom);
    return right >= left && bottom >= top;
  };

  bool UnionRect(CFRect *r1, CFRect *r2)
  {
    left = fmin(r1->left, r2->left);
    right = fmax(r1->right, r2->right);
    top = fmin(r1->top, r2->top);
    bottom = fmax(r1->bottom, r2->bottom);
    return right >= left && bottom >= top;
  };

  double Width() { return fabs(right-left);};
  double Height() { return fabs(bottom-top);};
  void InflateRect( double x, double y ) { left -= x, right += x, top -= y, bottom += y; };
  bool PtInRect( CFPoint p ) {
    return left <= p.x && p.x <= right && top <= p.y && p.y <= bottom;
  };

  void NormalizeRect()
  {
    double x;
    if (left > right) { x = left; left = right; right = x; }
    if (top > bottom) { x = top; top = bottom; bottom = x; }
  }

  double left, top, right, bottom;

};



class CCoordList : public list<CFPoint>
{
public:
  QString text = "";
  QDateTime date;
  double angle = 0.;
  int fileID = -1;
  int align;
  int color;
  int color2;
  bool is_color = false;
  bool is_color2 = false;
  bool is_date;
  int stateID = 1;
  int loc;
  int po;

  CCoordList() : is_color(false), is_date(false), text("")
  {
        loc = 0;
  };


public:
    double Square() const;
    double Length() const;
    double Length(const CFPoint &point) const;
    double LengthG(bool ismap) const;
    CFPoint centroid() const;
    CFPoint centroid2() const;
    void napr(double &x1, double &y1, double &x2, double &y2) const;
    bool Obrez(CCoordList& cl, const CRect& rect);
    bool find3(CFPoint p) const;
    bool find2(CFPoint p, double dx, int loc) const;
    bool find2(CFPoint p, double d, int loc, CFPoint &p1, CFPoint &p2) const;
    int find_edge(CFPoint p, double d, int loc) const;
    double Length2(CFPoint p, int loc) const;

    bool peres(CCoordList &cl);

    QString getWKT(int loc, int type_of_net) const;
    QString saveStr() const;
    void saveStrMinus(QString &str) const;
    void saveStr(QString &str, int& xmin, int& ymin, int& xmax, int& ymax) const;
    void loadStr(const char *s);
    CFPoint first() const;
    CFPoint second() const;
    CFPoint last() const;

    CFRect GetFRect() const;

    void removeV(int n);
    void SetV(int n, CFPoint p);
    CFPoint GetV(int n) const;
    CFPoint GetProjection(CFPoint p, CFPoint &p1, CFPoint& p2);
    CFPoint GetPr(CFPoint pt);
    double GetProjection(CFPoint p);
    CFPoint SetProjection(double dd);
    void Del(int n);

    bool ClockWise() const;
    int NHole() const;
    int find_point(CFPoint pt, int md, CFPoint &pt2, bool ends);
    int find_point(CFPoint pt, int md, bool ends);

    bool move(double dx, double dy);
    bool And(const CCoordList &cl1, const CCoordList &cl2, int op);
    PLINE *LoadPLINE() const;
    int LoadPOLYAREA( POLYAREA **PA) const;

    void SavePLINE(PLINE * res);
    int SavePOLYAREA( POLYAREA *PA);
    void Swap();
    void y_minus();
    void conv();
    void conv_obr();
//  double dlina(CLINE2 *l, CFPoint *p);
    CCoordList subline(CFPoint pt1, CFPoint pt, double dx);
    CCoordList createBuffer(double width);
};

double Length(CFPoint p1, CFPoint p2);
double LengthG(CFPoint p1, CFPoint p2, bool ismap);
CFPoint conv(CFPoint pt);
CFRect conv(CFRect r);
CFPoint conv_obr(CFPoint pt);
CFRect conv_obr(CFRect r);
void set_coord(double x0, double y0, double alpha);

double LengthPL(double x0, double y0, double x1, double y1, double x2, double y2);  // Расстояние до отрезка

