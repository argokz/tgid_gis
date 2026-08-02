#include "stdafx.h"

//-----------------------------------------------
// Проверяет пересечение отрезков
//-----------------------------------------------

int segmint(
    double x11, double y11,
    double x12, double y12,
    double x21, double y21,
    double x22, double y22,
    double& x, double& y,
    double& x31, double& y31, double& x32, double& y32
    )


/*
int segmint(
    double x11, double y11, double x12, double y12,
    double x21, double y21, double x22, double y22,
    double& x, double& y,
    double& x31, double& y31, double& x32, double& y32
    )
*/
{
  double D, D1, D2, t, t1, t2;
  int s;

  s = 0;
  D = (y12-y11)*(x21-x22)-(y21-y22)*(x12-x11);
  D1 = (y12-y11)*(x21-x11)-(y21-y11)*(x12-x11);
  D2 = (y21-y11)*(x21-x22)-(y21-y22)*(x21-x11);

  if (D != 0)
  {
    t1 = D1/D;
    t2 = D2/D;
    if (t1 <= 1 && t1 >= 0 && t2 >= 0 && t2 <= 1)
    {
      s = 1;
      x = x11+(x12-x11)*t2;
      y = y11+(y12-y11)*t2;
    }
  }
  else
  {
    if (D1 == 0 && D2 == 0)
    {
      if (x11 != x12)
      {
        if (y11 != y12)
        {
          t1 = (y21-y11)/(y12-y11);
          t2 = (y22-y11)/(y12-y11);
        }
      }
      else
      {
        t1 = (x21-x11)/(x12-x11);
        t2 = (x22-x11)/(x12-x11);
      }
      if (t1 > t2)
      {
        t = t2;
        x = x21;
        y = y21;
        t2 = t1;
        x21 = x22;
        y21 = y22;
        t1 = t;
        x22 = x;
        y22 = y;
      }
      if (t1 < 0)
      {
        if (t2 >= 1)
        {
          s = 2;
        }
        else
        {
          if (t2 > 0)
          {
            s = 2;
            x32 = x22;
            y32 = y22;
          }
          else
          {
            if (t2 == 0)
            {
              s = 1;
              x = x11;
              y = y11;
            }
          }
        }
      }
      else
      {
        if (t1 <= 1)
        {
          if (t1 == 1)
          {
            s = 1;
            x = x12;
            y = y12;
          }
          else
          {
            if (t2 >= 1)
            {
              x32 = x22;
              y32 = y22;
            }
            s = 2;
            x31 = x21;
            y31 = y21;
          }
        }
      }
    }
  }
  return s;
}


bool chline(double x1, double y1, double x2, double y2, double xx, double yy, double dx) {
  double xxx, yyy;
  double xmin, ymin, xmax, ymax;

  if (x1 == x2 && y1 == y2) return false;

  if ( x2 > x1) {xmin = x1; xmax = x2;} else {xmin = x2; xmax = x1;}
  if ( y2 > y1) {ymin = y1; ymax = y2;} else {ymin = y2; ymax = y1;}

  if (xmax-xmin >= ymax-ymin) {
    if ( xmin <= xx && xx <= xmax) {
      yyy = y1 + (xx-x1)*(y2-y1)/(x2-x1);
      if (yyy-dx < yy && yy < yyy+dx) return true;
    }
  }
  else {
    if ( ymin <= yy && yy <= ymax) {
      xxx = x1 + (yy-y1)*(x2-x1)/(y2-y1);
      if (xxx-dx < xx && xx < xxx+dx) return true;
    }
  }
  return false;
}

int peres(double &x1, double &y1, double &x2, double &y2, double rx1, double ry1, double rx2, double ry2) {
  double xmin, ymin, xmax, ymax, x, y;

  xmin = min(x1, x2); ymin = min(y1, y2); xmax = max(x1, x2); ymax = max(y1, y2);

  if (xmin > rx2 || xmax < rx1 || ymin > ry2 || ymax < ry1) return 0;

  return 1;

  if (xmin >= rx1 && xmax <= rx2 && ymin >= ry1 && ymax <= ry2) return 1;

  if (x1 > x2) {
    x = x2; x2 = x1; x1 = x;
    y = y2; y2 = y1; y1 = y;
  }
  if (x2 - x1 < 0.5) return 0;
  if (x1 < rx1) {
    y1 = y1+(rx1-x1)*(y2-y1)/(x2-x1);
    x1 = rx1;
  }
  if (x2 > rx2) {
    y2 = y1+(rx2-x1)*(y2-y1)/(x2-x1);
    x2 = rx2;
  }

  if (y1 > y2) {
    y = y2; y2 = y1; y1 = y;
    x = x2; x2 = x1; x1 = x;
  }
  if (y2 - y1 < 0.5) return 0;
  if (y1 < ry1) {
    x1 = x1+(ry1-y1)*(x2-x1)/(y2-y1);
    y1 = ry1;
  }
  if (y2 > ry2) {
    x2 = x1+(ry2-y1)*(x2-x1)/(y2-y1);
    y2 = ry2;
  }

  xmin = min(x1, x2); ymin = min(y1, y2); xmax = max(x1, x2); ymax = max(y1, y2);

  if (xmin > rx2 || xmax < rx1 || ymin > ry2 || ymax < ry1) return 0;
  if (xmin >= rx1 && xmax <= rx2 && ymin >= ry1 && ymax <= ry2) return 1;

  return 0;
}

//-----------------------------------------------
// Алгоритм Лианга-Барски, отсечение отрезка прямоугольным окном
//-----------------------------------------------

bool LiangBar(int XS1, int YS1, int XS2, int YS2,
  double& x0, double& y0, double& x1, double& y1)
{
  bool InS;
  double t0, t1;
  double dx, dy;

  t0 = 0;
  t1 = 1;
  InS = true;
  dx = x1-x0;

  if (dx != 0)
  {
    if (dx > 0)
    {
      if (x0-XS1 >= -t1*dx)
      {
        if (x0-XS1 < -t0*dx) t0 = (XS1-x0)/dx;

        if (XS2-x0 >= t0*dx)
        {
          if (XS2-x0 < t1*dx) t1 = (XS2-x0)/dx;
        }
        else
        {
          InS = false;
        }
      }
      else
      {
        InS = false;
      }
    }
    else
    {
      if (x0-XS1 >= -t0*dx)
      {
        if (x0-XS1<-t1*dx) t1 = (XS1-x0)/dx;

        if (XS2-x0 >= t1*dx)
        {
          if (XS2-x0 < t0*dx) t0 = (XS2-x0)/dx;
        }
        else
        {
          InS = false;
        }
      }
      else
      {
        InS = false;
      }
    }
  }
  else
  {
    if (x0 < XS1 || x0 > XS2)
    {
      InS = false;
    }
  }
  if (InS)
  {
    dy = y1-y0;
    if (dy != 0)
    {
      if (dy>0)
      {
        if (y0-YS1 >= -t1*dy)
        {
          if (y0-YS1 < -t0*dy) t0 = (YS1-y0)/dy;
          if (YS2-y0 >= t0*dy)
          {
            if (YS2-y0 < t1*dy) t1 = (YS2-y0)/dy;
          }
          else
          {
            InS = false;
          }
        }
        else
        {
          InS = false;
        }
      }
      else
      {
        if (y0-YS1 >= -t0*dy)
        {
          if (y0-YS1<-t1*dy) t1 = (YS1-y0)/dy;

          if (YS2-y0 >= t1*dy)
          {
            if (YS2-y0 < t0*dy) t0 = (YS2-y0)/dy;
          }
          else
          {
            InS = false;
          }
        }
        else
        {
          InS = false;
        }
      }
    }
    else
    {
      if (y0 < YS1 || y0 > YS2)
      {
        InS = false;
      }
    }
    if (InS)
    {
      if (t1 < 1)
      {
        x1 = x0+t1*dx;
        y1 = y0+t1*dy;
      }
      if (t0 > 0)
      {
        x0 = x0+t0*dx;
        y0 = y0+t0*dy;
      }
    }
  }
  return InS;
}

//-----------------------------------------------
// Алгоритм Лианга-Барски, отсечение отрезка прямоугольным окном 2
//-----------------------------------------------

bool LiangBar2(int XS1, int YS1, int XS2, int YS2,
  double x0, double y0, double x1, double y1)
{
  bool InS;
  double t0, t1;
  double dx, dy;

  t0 = 0;
  t1 = 1;
  InS = true;
  dx = x1-x0;

  if (dx != 0)
  {
    if (dx > 0)
    {
      if (x0-XS1 >= -t1*dx)
      {
        if (x0-XS1 < -t0*dx) t0 = (XS1-x0)/dx;

        if (XS2-x0 >= t0*dx)
        {
          if (XS2-x0 < t1*dx) t1 = (XS2-x0)/dx;
        }
        else
        {
          InS = false;
        }
      }
      else
      {
        InS = false;
      }
    }
    else
    {
      if (x0-XS1 >= -t0*dx)
      {
        if (x0-XS1<-t1*dx) t1 = (XS1-x0)/dx;

        if (XS2-x0 >= t1*dx)
        {
          if (XS2-x0 < t0*dx) t0 = (XS2-x0)/dx;
        }
        else
        {
          InS = false;
        }
      }
      else
      {
        InS = false;
      }
    }
  }
  else
  {
    if (x0 < XS1 || x0 > XS2)
    {
      InS = false;
    }
  }
  if (InS)
  {
    dy = y1-y0;
    if (dy != 0)
    {
      if (dy>0)
      {
        if (y0-YS1 >= -t1*dy)
        {
          if (y0-YS1 < -t0*dy) t0 = (YS1-y0)/dy;
          if (YS2-y0 >= t0*dy)
          {
            if (YS2-y0 < t1*dy) t1 = (YS2-y0)/dy;
          }
          else
          {
            InS = false;
          }
        }
        else
        {
          InS = false;
        }
      }
      else
      {
        if (y0-YS1 >= -t0*dy)
        {
          if (y0-YS1<-t1*dy) t1 = (YS1-y0)/dy;

          if (YS2-y0 >= t1*dy)
          {
            if (YS2-y0 < t0*dy) t0 = (YS2-y0)/dy;
          }
          else
          {
            InS = false;
          }
        }
        else
        {
          InS = false;
        }
      }
    }
    else
    {
      if (y0 < YS1 || y0 > YS2)
      {
        InS = false;
      }
    }
  }
  return InS;
}
