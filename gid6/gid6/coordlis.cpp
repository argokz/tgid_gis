// GeoFile.cpp: implementation of the CGeoFile class.
//
//////////////////////////////////////////////////////////////////////

//#include <boost/geometry.hpp>
//#include <iostream>



#include "stdafx.h"



#include "coordlis.h"
#include "klassif.h"

#include <sys/stat.h>
#include <share.h>
#include <math.h>
#include <float.h>
#include "geodez.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


static double cos_a = 1., sin_a = 0., m_alpha = 0;
static double xx0 = 0, yy0 = 0;

double get_alpha()
{
    return m_alpha;
}


void set_coord(double x0, double y0, double alpha)
{
  m_alpha = alpha;
  xx0 = x0;
  yy0 = y0;
  cos_a = cos(alpha);
  sin_a = sin(alpha);
}

void set42(int zone);


CFPoint geoToMercator(CFPoint g);



CFPoint conv(CFPoint pt)
{
/*
  if (m_alpha == 0) return pt;

  double B, L;  

  int nz = pt.x/100000000;
  set42(nz);
  UTM2deg(nz, pt.x/100., -pt.y/100., B, L);

  if (B < 0) B += 180;
  L  = lat2y(L);
  return CFPoint(B*10000000., -L*10000000.);
*/

  if (m_alpha == 0) return pt;
  double x = pt.x-xx0;
  double y = pt.y-yy0;
  
  return CFPoint(x*cos_a-y*sin_a+xx0, x*sin_a+y*cos_a+yy0);
}

CFPoint conv_obr(CFPoint pt)
{
  if (m_alpha == 0) return pt;
  double x = pt.x-xx0;
  double y = pt.y-yy0;
  
  return CFPoint(x*cos_a+y*sin_a+xx0, -x*sin_a+y*cos_a+yy0);
}


CFRect conv(CFRect r)
{
  if (m_alpha == 0) return r;
  CRect rect;
  CFPoint p1, p2, p3, p4;
  p1 = conv(CFPoint(r.left, r.top));
  p2 = conv(CFPoint(r.left, r.bottom));
  p3 = conv(CFPoint(r.right, r.top));
  p4 = conv(CFPoint(r.right, r.bottom));

  rect.left = min(min(p1.x, p2.x), min(p3.x, p4.x));
  rect.right = max(max(p1.x, p2.x), max(p3.x, p4.x));
  rect.top = min(min(p1.y, p2.y), min(p3.y, p4.y));
  rect.bottom = max(max(p1.y, p2.y), max( p3.y, p4.y));

  return rect;
}

CFRect conv_obr(CFRect r)
{
  if (m_alpha == 0) return r;
  CRect rect;
  CFPoint p1, p2, p3, p4;
  p1 = conv_obr(CFPoint(r.left, r.top));
  p2 = conv_obr(CFPoint(r.left, r.bottom));
  p3 = conv_obr(CFPoint(r.right, r.top));
  p4 = conv_obr(CFPoint(r.right, r.bottom));

  rect.left = min(min(p1.x, p2.x), min(p3.x, p4.x));
  rect.right = max(max(p1.x, p2.x), max(p3.x, p4.x));
  rect.top = min(min(p1.y, p2.y), min(p3.y, p4.y));
  rect.bottom = max(max(p1.y, p2.y), max( p3.y, p4.y));

  return rect;
}


bool isBreakCoord(int x)
{
    double xx = x;

    return (double)C_SPR-1000. < x && x < (double)C_SPR+1000.;
//    return it->x == C_SPR;
}


void CCoordList::conv()
{
  if (m_alpha == 0) return;
//  CCoordList::iterator it = begin();

  for (auto it = begin() ; it != end(); ++it) {
    if (!isBreakCoord(it->x)) {
      CFPoint pt = ::conv(*it);
      it->x = pt.x;
      it->y = pt.y;
    }
  }
}

void CCoordList::conv_obr()
{
  if (m_alpha == 0) return;
//  CCoordList::iterator it = begin();

  for (auto it = begin() ; it != end(); ++it) {
    if (!isBreakCoord(it->x)) {
      CFPoint pt = ::conv_obr(*it);
      it->x = pt.x;
      it->y = pt.y;
    }
  }
}



CFRect CCoordList::GetFRect() const
{
  CFRect r(DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX);
  double x, y;

//  CCoordList::const_iterator it = begin();

  for ( auto it = begin(); it != end(); ++it) {
    x = (*it).x;
    if (isBreakCoord(x)) continue;
    y = (*it).y;
    r.left = min(r.left, x); r.right = max(r.right, x);
    r.top = min(r.top, y); r.bottom = max(r.bottom, y);
  }

  return r;
}


bool CCoordList::Obrez(CCoordList& cl, const CRect& rect)
{
  return true;
}

int pline(double x1, double y1, double x2, double y2, double y, double &x) {
  if (y == y1 || y == y2) y--;

  if (y < y1 && y < y2) return 0;
  if (y > y1 && y > y2) return 0;

  x = x1+(x2-x1)*(y-y1)/(y2-y1);

  if (x < x1 && x < x2) return 0;
  if (x > x1 && x > x2) return 0;

  return 1;
}


#include <limits.h>


PLINE *CCoordList::LoadPLINE() const
{
  PLINE *res = NULL;
  Vector v;
//  const_iterator it;

  for (auto it = begin(); it != end(); ++it) {
    v[0] = it->x;
    if (isBreakCoord(it->x)) break;
    v[1] = it->y;
    v[2] = 0.0;
    if ( res == NULL )
    {
        if ((res = poly_NewContour(v)) == NULL)
            return NULL;
    }
    else
        poly_InclVertex(res->head.prev, poly_CreateNode(v));
  }

  poly_PreContour(res, TRUE);
  return res;
}


int CCoordList::LoadPOLYAREA( POLYAREA **PA) const
{
    PLINE *cntr;
    POLYAREA *p;

    *PA = NULL;
    if ( (cntr = LoadPLINE()) == NULL)
        return err_bad_parm;
    if ( (cntr->Flags & PLF_ORIENT) != (unsigned int)(0 ? PLF_INV : PLF_DIR) )
        poly_InvContour(cntr);


    p = poly_Create();
    if (p == NULL)
        return err_no_memory;
    poly_InclContour(p, cntr);
    cntr = NULL;
    if (!poly_Valid(p))
        return err_bad_parm;

    poly_M_Incl(PA, p);
    return err_ok;
}

void CCoordList::SavePLINE(PLINE * res)
{
    VNODE *cur;
    CFPoint p;

    cur = &res->head;

    do
    {
      p.x = cur->point[0];
      p.y = cur->point[1];
      push_back(p);
    } while ( ( cur = cur->next ) != &res->head );
}


int CCoordList::SavePOLYAREA( POLYAREA *PA)
{
    int cnt;
    PLINE *cntr;
    POLYAREA * curpa;

    cnt = 0, curpa = PA;
    do
    {
        cnt++;
    } while ( (curpa = curpa->f) != PA);

//    fprintf(f, "%d\n", cnt);
    curpa = PA;

    clear();

    do
    {
        for ( cntr = curpa->contours, cnt = 0; cntr != NULL; cntr = cntr->next, cnt++ ) {}
//        fprintf(f, "%d\n", cnt);
        for ( cntr = curpa->contours; cntr != NULL; cntr = cntr->next )
            SavePLINE(cntr);

        push_back(CFPoint(C_SPR, C_SPR));
    }  while ( (curpa = curpa->f) != PA);

    return err_ok;
}


bool CCoordList::And(const CCoordList &cl1, const CCoordList &cl2, int op)
{
  POLYAREA *a = NULL, *b = NULL, *res = NULL;
  bool ret;
  int error;

  cl1.LoadPOLYAREA(&a);
  cl2.LoadPOLYAREA(&b);

  if ( !(poly_Valid(a) && poly_Valid(b)) ) {
//    AfxMessageBox(_TR("Один из полигонов неправильный"), MB_OK|MB_ICONINFORMATION);
    poly_Free(&a);
    poly_Free(&b);
    return false;
  }

//  int op = PBO_ISECT;
//  int op = PBO_UNITE;
//  int op = PBO_XOR;
//  int op = PBO_SUB;

  if ((error = poly_Boolean(a, b, &res, op)) == err_ok) {
    if (res) {
      SavePOLYAREA(res);
      if ( !poly_Valid(res)) {
//        AfxMessageBox(_TR("Получившийся полигон неправильный"), MB_OK|MB_ICONINFORMATION);
      }
    }
  }

  ret = res != NULL;

  poly_Free(&a);
  poly_Free(&b);
  if (res) poly_Free(&res);
  return ret;
}

int sfd( const void *a, const void *b) {
  double *a1, *b1, l;

  a1 = (double*) a;
  b1 = (double*) b;
  l = *a1 - *b1;

  if (l > 0) return 1;
  if (l < 0) return -1;
  return 0;
}

CFPoint CCoordList::centroid2() const
{
  double x1, y1, x2, y2;
  napr(x1, y1, x2, y2);

  return CFPoint((x1+x2)/2, (y1+y2)/2);
}



CFPoint CCoordList::centroid() const
{
  int x;
  int i, nx;
  double x1, y1, x2, y2, x0, y0, xx, yy;
  double lx[2560];
  const_iterator it;

  if (size() == 1) {
    it = begin();
    return CFPoint(it->x, it->y);
  }

  if (size() == 2) {
    it = begin();
    xx = it->x; yy = it->y;
    ++it;
    return CFPoint(xx, yy);
//    return CFPoint((xx+it->x)/2, (yy+it->y)/2);
  }


  y1 =  DBL_MAX;
  y2 = -DBL_MAX;

  for (it = begin(); it != end(); ++it) {
    double y = it->y;

    if (isBreakCoord(y)) break;


    if (!isBreakCoord(y)) {
      if (it->y < y1) y1 = y;
      if (it->y > y2) y2 = y;
    }
  }
  yy = (y1+y2)/2;

  nx = 0;

  it = begin();
  x0 = x1 = it->x;
  y0 = y1 = it->y;

  for (;it != end(); ++it) {
    x2 = it->x;
    y2 = it->y;

    if (isBreakCoord(y2)) break;

    if (pline(x1, y1, x2, y2, yy, xx)) lx[nx++] = xx;

    x1 = x2;
    y1 = y2;
  }

  if (pline(x1, y1, x0, y0, yy, xx)) lx[nx++] = xx;

  if (nx < 2) return CFPoint(xx, yy);

  qsort((void *) lx, (size_t) nx, sizeof(double), sfd);

  long maxl = 0;

  x = xx;

  for ( i = 0; i < nx; i += 2 ) {
    if (lx[i+1]-lx[i] > maxl) {
      maxl = lx[i+1]-lx[i];
      x = (lx[i+1]+lx[i])/2;
    }
  }
  return CFPoint(x, yy);
}


void CCoordList::napr(double &x1, double &y1, double &x2, double &y2) const
{
  double len, len2, xx, yy;

  len = 0;

  const_iterator it;

  it = begin();

  x1 = x2 = it->x;
  y1 = y2 = it->y;

  
  ++it;

  for (;it != end(); ++it) {
    x2 = it->x;
    y2 = it->y;

    len += hypot(x2-x1, y2-y1);

    x1 = x2;
    y1 = y2;
  }

  len2 = len/2;
  len = 0;

  it = begin();

  x1 = it->x;
  y1 = it->y;
  ++it;

  for (;it != end(); ++it) {
    x2 = it->x;
    y2 = it->y;

    len += hypot(x2-x1, y2-y1);

    if (len >= len2) break;

    x1 = x2;
    y1 = y2;
  }

  if (x2 < x1) {xx = x1; x1 = x2; x2 = xx; yy = y1; y1 = y2; y2 = yy;}
}


bool CCoordList::find2(CFPoint p, double d, int loc) const
{
  return Length2(p, loc) < d;

/*

  double xmin, ymin, xmax, ymax, x0, y0, x1, y1, x2, y2, xx, yy;

  const_iterator it = begin();

  x0 = x1 = it->x;
  y0 = y1 = it->y;
  it ++;

  for (;; ++it) {
    if ( it == end()) {
      if (loc == LOC3) {
        x2 = x0;
        y2 = y0;
      }
      else
          break;
    }
    else {
      x2 = it->x;
      y2 = it->y;
    }
    if (x1 == x2 && y1 == y2) {
      if ( it == end()) break;
      continue;
    }

    if ( x2 > x1) {xmin = x1; xmax = x2;} else {xmin = x2; xmax = x1;}
    if ( y2 > y1) {ymin = y1; ymax = y2;} else {ymin = y2; ymax = y1;}

//    if ( x1 == x2 && x1-d < p.x && p.x < x1+d && ymin < p.y && p.y < ymax) return true;
//    if ( y1 == y2 && y1-d < p.y && p.y < y1+d && xmin < p.x && p.x < xmax) return true;

    if ( fabs(x1-x2) < d && x1-d < p.x && p.x < x1+d && ymin < p.y && p.y < ymax) return true;
    if ( fabs(y1-y2) < d && y1-d < p.y && p.y < y1+d && xmin < p.x && p.x < xmax) return true;


    if ( xmin <= p.x && p.x <= xmax && ymin <= p.y && p.y <= ymax) {
      if (xmax-xmin >= ymax-ymin) {
        yy = y1 + (p.x-x1)*(y2-y1)/(x2-x1);
        if (yy-d < p.y && p.y < yy+d) return true;
      }
      else {
        xx = x1 + (p.y-y1)*(x2-x1)/(y2-y1);
        if (xx-d < p.x && p.x < xx+d) return true;
      }
    }
    x1 = x2;
    y1 = y2;
    if (it == end()) break;
  }
  return false;
*/

}

// Расстояние до отрезка

double LengthPL(double x0, double y0, double x1, double y1, double x2, double y2) 
{
  double r0 = 1e30;
  double r1 = hypot(x0-x1,y0-y1);
  double r2 = hypot(x0-x2,y0-y2);

  double r12 = hypot(x1-x2,y1-y2);

  if (r1 >= hypot(r2,r12)) {
    r0 = min(r1, r2);
  }
  else if (r2 >= hypot(r1,r12)) {
    r0 = min(r1, r2);
  }
  else {
    double a = y2-y1; 
    double b = x1-x2; 
    double c = -x1*(y2-y1)+y1*(x2-x1);
    double t = hypot (a,b);
    if (c > 0)  { a = -a; b = -b; c = -c; };
    r0 = (a*x0+b*y0+c)/t;
  }
  return fabs(r0);
}



double CCoordList::Length2(CFPoint p, int loc) const
{
  double min_len = 1e30;
  double x0, y0, x1, y1, x2, y2;

  const_iterator it = begin();

  if (it == end()) return 0;

  x0 = x1 = it->x;
  y0 = y1 = it->y;
  it ++;

  for (;; ++it) {
    if ( it == end()) {
      if (loc == LOC3) {
        x2 = x0;
        y2 = y0;
      }
      else
          break;
    }
    else {
      x2 = it->x;
      y2 = it->y;
    }

    if (!isBreakCoord(x2)) {
      double len = LengthPL(p.x, p.y, x1, y1, x2, y2);

      if (len < min_len) {
        min_len = len;
      }
      x1 = x2;
      y1 = y2;
    }
    else {
      ++it;
      if (it == end()) break;

      x0 = x1 = it->x;
      y0 = y1 = it->y;
    }
 
    if (it == end()) break;
  }
  return min_len;
}



bool CCoordList::find2(CFPoint p, double d, int loc, CFPoint &p1, CFPoint &p2) const
{
  double xmin, ymin, xmax, ymax, x0, y0, x1, y1, x2, y2, xx, yy;

  const_iterator it = begin();

  x0 = x1 = it->x;
  y0 = y1 = it->y;
  p1 = *it;
  it ++;

  for (;; ++it) {
    if ( it == end()) {
      if (loc == LOC3) {
        x2 = x0;
        y2 = y0;
        p2 = CFPoint(x0, y0);
      }
      else
          break;
    }
    else {
      x2 = it->x;
      y2 = it->y;
      p2 = *it;
    }
    if (x1 == x2 && y1 == y2) {
      if ( it == end()) break;
      continue;
    }

    if ( x2 > x1) {xmin = x1; xmax = x2;} else {xmin = x2; xmax = x1;}
    if ( y2 > y1) {ymin = y1; ymax = y2;} else {ymin = y2; ymax = y1;}

    if ( x1 == x2 && x1-d < p.x && p.x < x1+d && ymin < p.y && p.y < ymax) return true;
    if ( y1 == y2 && y1-d < p.y && p.y < y1+d && xmin < p.x && p.x < xmax) return true;
    if ( xmin <= p.x && p.x <= xmax && ymin <= p.y && p.y <= ymax) {
      if (xmax-xmin >= ymax-ymin) {
        yy = y1 + (p.x-x1)*(y2-y1)/(x2-x1);
        if (yy-d < p.y && p.y < yy+d) return true;
      }
      else {
        xx = x1 + (p.y-y1)*(x2-x1)/(y2-y1);
        if (xx-d < p.x && p.x < xx+d) return true;
      }
    }
    x1 = x2;
    y1 = y2;
    p1 = p2;
    if (it == end()) break;
  }
  return false;
}


#if 0
bool CCoordList::find3(CFPoint p) const
{
  bool Result = false;
  double x0, y0, x1, y1, x2, y2;

  const_iterator it = begin();

  x0 = x1 = it->x;
  y0 = y1 = it->y;
  ++it;

  for (; it != end(); ++it) {
    x2 = it->x;
    y2 = it->y;
    if ((p.y > y1 || p.y > y2) && (p.y <= y1 || p.y <= y2))
    {
      if (p.x-x1 < (p.y-y1)*(x2-x1)/(y2-y1))
      {
        Result = ! Result;
      }
    }
    x1 = x2;
    y1 = y2;
  }

  if ((p.y > y1 || p.y > y0) && (p.y <= y1 || p.y <= y0))
  {
    if (p.x-x1 < (p.y-y1)*(x0-x1)/(y0-y1))
    {
      Result = ! Result;
    }
  }

  return Result;
}

#endif


bool CCoordList::find3(CFPoint p) const
{
  bool Result = false;
  double x0, y0, x1, y1, x2, y2;

  const_iterator it = begin();

  bool first = true;

//  x0 = x1 = it->x;
//  y0 = y1 = it->y;
//  ++it;

  for (; it != end(); ++it) {
    if (isBreakCoord(it->x)) {
        first = true;
        continue;
    }
    
    if (first) {
        x0 = x1 = it->x;
        y0 = y1 = it->y;
        first = false;
        continue;
    }


    x2 = it->x;
    y2 = it->y;

    if ((p.y > y1 || p.y > y2) && (p.y <= y1 || p.y <= y2))
    {
      if (p.x-x1 < (p.y-y1)*(x2-x1)/(y2-y1))
      {
        Result = ! Result;
      }
    }
    x1 = x2;
    y1 = y2;
  }

  if ((p.y > y1 || p.y > y0) && (p.y <= y1 || p.y <= y0))
  {
    if (p.x-x1 < (p.y-y1)*(x0-x1)/(y0-y1))
    {
      Result = ! Result;
    }
  }

  return Result;
}



double CCoordList::Square() const
{
  if (size() < 3) return 0.;

  double S = 0;
  int i;
  CFPoint p0, p1, p2;

  const_iterator it = begin();
  p0 = *it;  ++it;
  p1 = *it;  ++it;

  for ( i = 0; i < size(); i++, ++it ) {
    if (it == end()) it = begin();
    p2 = *it;
    if (!isBreakCoord(p2.x)) {
      S += (double)p1.x*((double)p2.y-(double)p0.y);
      p0 = p1;
      p1 = p2;
    }
  }
  if (S < 0.) S = -S;

  return S*0.0001/2;

};

void CCoordList::SetV(int n, CFPoint pt)
{
  iterator it = begin();

  for (int i = 0; it != end(); ++it, ++i) {
    if (i == n) {
      *it = pt;
      break;
    }
  }
}

CFPoint CCoordList::GetV(int n) const
{
  const_iterator it = begin();

  for (int i = 0; it != end(); ++it, ++i) if (i == n) return *it;;
  return CFPoint();
}

CFPoint CCoordList::first() const
{
  const_iterator it = begin();

  if (it != end()) return *it;
  return CFPoint();
}

CFPoint CCoordList::second() const
{
  const_iterator it = begin();

  if (it != end()) {
    it++;
    if (it != end()) {
      return *it;
    }
  }
  return CFPoint();
}


CFPoint CCoordList::last() const
{
  const_reverse_iterator it = rbegin();

  if (it != rend()) return *it;
  return CFPoint();
}



void CCoordList::Del(int n)
{
  iterator it = begin();

  for (int i = 0; it != end(); ++it, ++i) {
    if (i == n) {
      erase(it);
      break;
    }
  }
}


double CCoordList::Length() const
{
  if (size() < 2) return 0.;

  double L = 0;
  CFPoint p1, p2;

  const_iterator it = begin();
  p1 = *it;  ++it;

  for ( ; it != end(); ++it ) {
    p2 = *it;
    if (isBreakCoord(p2.x)) {
      while (it != end() && isBreakCoord(it->x)) {
        it ++;
        p2 = *it;
      }
      p1 = p2;
    }
    else {
      L += hypot(p2.x-p1.x, p2.y-p1.y);
      p1 = p2;
    }
  }
//  return L*0.01;
  return L*0.01;
};


double dot(const CFPoint& p1, const CFPoint& p2) {
    return p1.x * p2.x + p1.y * p2.y;
}


double distance_to_segment(const CFPoint& p, const CFPoint& v, const CFPoint& w) {
    // Вычисление длины отрезка
    double l2 = Length(v, w) * Length(v, w);
    if (l2 == 0.0) return Length(p, v);  // v == w, отрезок вырождается в точку

    // Вычисление проекции точки p на линию, проходящую через v и w
    CFPoint projection;
    double t = dot(CFPoint{p.x - v.x, p.y - v.y}, CFPoint{w.x - v.x, w.y - v.y}) / l2;
    if (t < 0.0) return Length(p, v);       // Проекция находится за точкой v
    if (t > 0.0) return Length(p, w);       // Проекция находится за точкой w

    // Вычисление проекции точки p на отрезок
    projection = CFPoint{v.x + t * (w.x - v.x), v.y + t * (w.y - v.y)};
    return Length(p, projection);
}




bool in_line(const CFPoint &p1, const CFPoint &p2, const CFPoint &point)
{
  return false;
}


double CCoordList::Length(const CFPoint &point) const
{
  if (size() < 2) return 0.;

  double L = 0;
  CFPoint p1, p2;

  const_iterator it = begin();
  p1 = *it;  ++it;

  for ( ; it != end(); ++it ) {
    p2 = *it;
    if (!isBreakCoord(p2.x)) {
      if (in_line(p1, p2, point))  {
        L += hypot(point.x-p1.x, point.y-p1.y);
        break;
      }
      L += hypot(p2.x-p1.x, p2.y-p1.y);
      p1 = p2;
    }
  }
//  return L*0.01;
  return L*0.01;
}


double CCoordList::LengthG(BOOL ismap) const
{
  if (size() < 2) return 0.;

  double L = 0;
  CFPoint p1, p2;

  const_iterator it = begin();
  p1 = *it;  ++it;

  for ( ; it != end(); ++it ) {
    p2 = *it;
    if (!isBreakCoord(p2.x)) {
      L += ::LengthG(p1, p2, ismap);
      p1 = p2;
    }
  }
  return L;
};

CFPoint toGeo(CFPoint pt)
{
  double y = y2lat(-0.0000001*pt.y);
  double x = x2lon(0.0000001*pt.x);

  void d2m(int zona, double B, double L, double &x, double &y);

  double xx, yy;
  d2m(-1, y, x, xx, yy);


  return CFPoint(xx*100, yy*100);
}

double Length(CFPoint p1, CFPoint p2)
{
  return hypot(p1.x-p2.x, p1.y-p2.y)*0.01;
}

double LengthG(CFPoint p1, CFPoint p2, BOOL ismap)
{
  if (ismap) {
    p1 = toGeo(p1);
    p2 = toGeo(p2);
  }
  return hypot(p1.x-p2.x, p1.y-p2.y)*0.01;
}


CString CCoordList::saveStr() const
{
  CString s;
  CString str = "";

  for (const_iterator it = begin(); it != end(); ++it) {
    CFPoint pt = ::conv_obr(*it);
    s.Format("%.0lf %.0lf,", pt.x, pt.y);
    str += s;
  }
  return str;
}

CString CCoordList::getWKT(int loc, int type_of_net) const
{
  CString s;
  CString str = "";
  CString s1 = "";

  loc = loc % 10;

  int nn = 0;

  for (const_iterator it = begin(); it != end(); ++it) {
    CFPoint pt = ::conv_obr(*it);
    if ( str != "") str += ",";
//    s.Format("%lf %lf", pt.x/100, -pt.y/100);
    s.Format("%.2lf %.2lf", pt.x/100, -pt.y/100);
//    s.Format("%.12lf %.12lf", pt.x/100, -pt.y/100);
    str += s;

    if (loc == 1) break;
    if (loc == LOC6 && nn == 1) break;
    if (nn == 0) {
        s1 = s;
    }

    nn++;
  }
  if (loc == LOC3) {
    if ( str != "") str += ",";
    str += s1;
  }


  if (type_of_net == 1) {  // MS SQL
      if (loc == 1) {
          s.Format("geometry::STGeomFromText('POINT(%s)', 0)", str);
      }
      else if (loc == LOC2 || loc == LOC6) {
          s.Format("geometry::STGeomFromText('LINESTRING(%s)', 0).MakeValid()", str);
      }
      else if (loc == LOC3) {
//          s.Format("geometry::STGeomFromText('LINESTRING(%s)', 0)", str);
          s.Format("geometry::STGeomFromText('POLYGON((%s))', 0).MakeValid()", str);
      }
  }
  else {
      if (loc == 1) {
          s.Format("ST_GeomFromText('POINT(%s)', 0)", str);
      }
      else if (loc == LOC2 || loc == LOC6) {
          s.Format("ST_GeomFromText('LINESTRING(%s)', 0)", str);
      }
      else if (loc == LOC3) {
          s.Format("ST_GeomFromText('POLYGON((%s))', 0)", str);
      }
  }
  return s;
}



void CCoordList::saveStrMinus(CString &str) const
{
  CString s;
  str = "";

  for (const_iterator it = begin(); it != end(); ++it) {
    if (isBreakCoord(it->x)) break;
//    CFPoint pt = ::conv_obr(*it);
    s.Format("%.0lf %.0lf,", it->x, -it->y);
    str += s;
  }
}


/*
void CCoordList::saveStr(CString &str, int& xmin, int& ymin, int& xmax, int& ymax) const
{
  double x, y;
  char s[256];

  xmin = ymin = LONG_MAX;  xmax = ymax = LONG_MIN;
  str = "";

  for ( const_iterator it = begin(); it != end(); ++it) {
    if (isBreakCoord(it->x)) break;

    CFPoint pt = ::conv_obr(*it);

    x = (*it).x;
    y = -(*it).y;

    xmin = min(xmin, pt.x); xmax = max(xmax, pt.x);
    ymin = min(ymin, -pt.y); ymax = max(ymax, -pt.y);
    sprintf(s, "%.0f %.0f ", pt.x, -pt.y);
    str += s;
  }
}
*/

void CCoordList::Swap()
{
  CCoordList cl = *this;
  clear();
  for (reverse_iterator it = cl.rbegin(); it != cl.rend(); ++it) push_back(*it);
}

//-----------------------------------------------
// Загрузить из строки
//-----------------------------------------------

void CCoordList::loadStr(const char *str)
{
  int i, j;
  bool first;
  double x, y;
  double x0 = 1e20, y0 = 1e20;
  char s[256];

  clear();

  for (i = j = 0, first = true; str[i] && j < 256; i++) {
    s[j++] = str[i];
    if (s[j-1] == ' ' || s[j-1] == ',') {
      if (!first) {
        s[j-1] = 0;
        j = 0;
        sscanf(s, "%lg %lg", &x, &y);
        if (x != x0 || y != y0) {
          CFPoint pt = ::conv(CFPoint(x, y));
          push_back(pt);
        }
        x0 = x;
        y0 = y;
      }
      first = !first;
    }
  }
}

int CCoordList::NHole() const
{
  int n = 0;
  CCoordList::const_iterator it = begin();

  for ( ; it != end(); ++it) {
    if (isBreakCoord((*it).x)) n++;
  }

  return n;
}

void CCoordList::y_minus()
{
  CCoordList::iterator it = begin();

  for ( ; it != end(); ++it) {
    if (!isBreakCoord((*it).x)) {
        (*it).y = -(*it).y;
    }
  }
}

CFPoint GetProjection(CFPoint p, CFPoint p1, CFPoint p2)
{
  double fDenominator = (p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y)*(p2.y - p1.y);
  if (fDenominator == 0) // p1 and p2 are the same
      return p1;

  double t = (p.x*(p2.x - p1.x) - (p2.x - p1.x)*p1.x + p.y*(p2.y - p1.y) - (p2.y - p1.y)*p1.y) / fDenominator;

  return CFPoint(p1.x + (p2.x - p1.x)*t, p1.y + (p2.y - p1.y)*t);
}

double dist(CFPoint p, CFPoint p1, CFPoint p2)
{

  double c1 = ((p2.x-p1.x)*(p.x-p1.x)+(p2.y-p1.y)*(p.y-p1.y));

  if (c1 <= 0) {
    return hypot(p1.x-p.x, p1.y-p.y);
  }


  double c2 = ((p2.x-p1.x)*(p2.x-p1.x)+(p2.y-p1.y)*(p2.y-p1.y));

  if (c2 <= c1) {
    return hypot(p2.x-p.x, p2.y-p.y);
  }
  return ((p1.y-p2.y)*p.x+(p2.x-p1.x)*p.y+(p1.x*p2.y-p2.x*p1.y))/hypot(p1.x-p2.x, p1.y-p2.y);
}

CFPoint CCoordList::GetProjection(CFPoint p, CFPoint &p1, CFPoint& p2)
{
  double x0, y0, x1, y1, x2, y2;
  double d, min_d = 1e20;
  CFPoint p0 = p, p01, p02;
 

  const_iterator it = begin();

  x0 = x1 = it->x;
  y0 = y1 = it->y;
  p01 = CFPoint(x1, y1);
  it ++;

  for (;it != end(); ++it) {
    x2 = it->x;
    y2 = it->y;
    p02 = CFPoint(x2, y2);


    if (x1 == x2 && y1 == y2) {
      if ( it == end()) break;
      continue;
    }

    d = fabs(dist(p, p01, p02));
    if (d < min_d) {
      min_d = d;
      p1 = p01;
      p2 = p02;
      p0 = ::GetProjection(p, p1, p2);
    }

    x1 = x2;
    y1 = y2;
    p01 = p02;
    if (it == end()) break;
  }
  return p0;
}


double CCoordList::GetProjection(CFPoint p)
{
  double x0, y0, x1, y1, x2, y2;
  double d, min_d = 1e20, min_l;
  CFPoint p0 = p, p01, p02;

  double length = Length(), l1 = 0;

  const_iterator it = begin();

  x0 = x1 = it->x;
  y0 = y1 = it->y;
  p01 = CFPoint(x1, y1);
  it ++;

  for (;it != end(); ++it) {
    x2 = it->x;
    y2 = it->y;
    p02 = CFPoint(x2, y2);


    if (x1 == x2 && y1 == y2) {
      if ( it == end()) break;
      continue;
    }

    d = fabs(dist(p, p01, p02));
    if (d < min_d) {
      min_d = d;
      p0 = ::GetProjection(p, p01, p02);
      min_l = l1 +:: Length(p01, p0);
    }

    l1 += ::Length(p01, p02);

    x1 = x2;
    y1 = y2;
    p01 = p02;
    if (it == end()) break;
  }
  return min_l/length;
}


CFPoint CCoordList::SetProjection(double d)
{
  double len = Length()*d;

  double x0, y0, x1, y1, x2, y2;
  CFPoint p0, p01, p02;

  double l = 0;

  const_iterator it = begin();

  x0 = x1 = it->x;
  y0 = y1 = it->y;
  p01 = CFPoint(x1, y1);
  it ++;

  for (;it != end(); ++it) {
    x2 = it->x;
    y2 = it->y;
    p02 = CFPoint(x2, y2);


    if (x1 == x2 && y1 == y2) {
      if ( it == end()) break;
      continue;
    }

    double l1 = ::Length(p01, p02);

    if (l+l1 > len) {
      return CFPoint(p01.x+(p02.x-p01.x)*(len-l)/l1, p01.y+(p02.y-p01.y)*(len-l)/l1);
    }
    l += l1;


    x1 = x2;
    y1 = y2;
    p01 = p02;
    if (it == end()) break;
  }

  return CFPoint(0, 0);
}

bool CCoordList::move(double dx, double dy)
{
  CCoordList::iterator it = begin();

  for ( ; it != end(); ++it) {
    if (!isBreakCoord((*it).x)) {
      (*it).x += dx;
      (*it).y += dy;
    }
  }
  return true;
}

bool CCoordList::peres(CCoordList &cl)
{
  for (auto it = begin(); it != end(); ++it) {
    if (!isBreakCoord((*it).x)) {
      if (cl.find3(*it)) {
        return true;
      }
    }
  }
  return false;
}


CFRect GetLineRect(CFPoint pt1, CFPoint pt2, const CCoordList &cl)
{

  CFRect rect = cl.GetFRect();

  rect.AddPoint(pt1);
  rect.AddPoint(pt2);

  return rect;
}


// Функция для нормализации вектора
void normalize(double& x, double& y) {
    double length = std::sqrt(x * x + y * y);
    x /= length;
    y /= length;
}


#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/geometries.hpp>


// Функция для создания буфера вокруг полигона
CCoordList CCoordList::createBuffer(double width)
{
    CCoordList cl;

    namespace bg = boost::geometry;
    typedef bg::model::d2::point_xy<double> point;
    typedef bg::model::polygon<point> polygon;

    // Declare strategies
    const double buffer_distance = width;
    const int points_per_circle = 36;

//    bg::strategy::buffer::distance_asymmetric<double> distance_strategy(width, 0);
    bg::strategy::buffer::distance_symmetric<double> distance_strategy(buffer_distance);
//    bg::strategy::buffer::join_round join_strategy(points_per_circle);
    bg::strategy::buffer::join_miter join_strategy;

//    bg::strategy::buffer::end_round end_strategy(points_per_circle);
    bg::strategy::buffer::end_flat end_strategy;

    bg::strategy::buffer::point_circle circle_strategy(points_per_circle);
    bg::strategy::buffer::side_straight side_strategy;

    bg::model::multi_polygon<polygon> result;

    std::vector<polygon> buffered_polygons;

    bg::model::linestring<point> ls;

    for (auto& p : *this) {
        bg::append(ls, point(p.x, p.y));
    }


    bg::buffer(ls, result,
        distance_strategy, side_strategy,
        join_strategy, end_strategy, circle_strategy);


    for (auto& buffered_poly : result) {
        for (const auto& p : bg::exterior_ring(buffered_poly)) {
            double x = bg::get<0>(p);
            double y = bg::get<1>(p);
            cl.push_back(CFPoint(x, y));
        }
    }



/**
    if (!result.empty()) {
        // Извлекаем первую внешнюю границу первого полигона в мультиполигоне
        const polygon& poly = result.front();
        for (const auto& pt : poly.outer()) {
//            offset_line.push_back(pt);
            double x = pt.x();
            double y = pt.y();
            cl.push_back(CFPoint(x, y));
        }
    }
*/

    return cl;
}



CCoordList CCoordList::subline(CFPoint pt1, CFPoint pt2, double dx)
{
    const_iterator it = this->begin();
    CFPoint pt_1 = *it;
    ++it;

    int find_point = 0;

    CCoordList cl;

    for (; it != this->end(); ++it) {
        CFPoint pt_2 = *it;

//        double d1 = distance_to_segment(pt1, pt_1, pt_2);
//        double d2 = distance_to_segment(pt2, pt_1, pt_2);

        double d1 = fabs(dist(pt1, pt_1, pt_2));
        double d2 = fabs(dist(pt2, pt_1, pt_2));

        bool f1 = d1 < dx;
        bool f2 = d2 < dx;

        if (f1) {
            CFPoint pt = GetProjection(pt1, pt_1, pt_2);
            cl.push_back(pt);
            find_point += 1;
        }
        if (f2) {
            CFPoint pt = GetProjection(pt2, pt_1, pt_2);
            cl.push_back(pt);
            find_point += 1;
        }
        if (find_point == 2) break;

        if (find_point == 1) {
            cl.push_back(pt_2);
        }

        pt_1 = pt_2;
    }
    
    return cl;
}