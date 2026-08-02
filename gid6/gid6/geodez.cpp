#include "stdafx.h"

#include "geodez.h"

#include "win.h"
#include "math.h"

const double p = 180./M_PI;

const double a = 6378245.;
const double b = a*(1.-1./298.3);

//const double a = 6378137.;
//const double b = a*(1.-1./298.257223563);


//const double b = 6356863.019;

double e2, e12;

double rad(double x) { return x/180*M_PI;}
double obr(double x) { return 1./x;}
double sq(double x) { return x*x;}
double lon2x(double x);
double x2lon(double x);

#include "coordlis.h"

static map<CFPoint, SystemCoord> map_system_coord;

void setSystemCoord(CFPoint pt)
{
    if (map_system_coord.size() == 0) return;
    
    double min_l = -1;

    for (auto it : map_system_coord) {
        double l = Length(pt, it.first);
        if (l < min_l || min_l < 0) {
            min_l = l;
            set_system_coord(it.second);
        }
    }    
}

void open_special_sk(const char* db_name)
{
    map_system_coord.clear();
    CString fn;
    fn.Format("%skls\\%s.sk", argpath(), db_name);
    FILE* f = fopen(fn, "r");
    if (f) {
        while (!feof(f)) {
            CFPoint pt;
            SystemCoord sc;

            if (fscanf(f, "%lg %lg %lg %lg %lg %lg %lg %lg", &pt.x, &pt.y, 
                &sc.Central_Meridian, 
                &sc.Latitude_Of_Origin, 
                &sc.False_Easting, 
                &sc.False_Northing, 
                &sc.Scale_Factor, 
                &sc.Angle) == 8) {
                map_system_coord[pt] = sc;
            }

        }
        fclose(f);
    }
}

double P(double x) {
  double A1, B1, C1;

  double l = 1./12.;

  x = rad(x);
  A1 = 1.00336361;
  B1 = 0.00112403;
  C1 = 0.00000170;

  return M_PI*b*b/360*(A1*sin(l)*cos(x)-B1*sin(l*3)*cos(x*3)+C1*sin(l*5)*cos(x*5))/1000000;
}

double PP() {
  return b*b*M_PI*4*(1.+2./3.*e2+3./5.*e2*e2+4./7.*e2*e2*e2)/1000000;
}

double W(double x) { return sqrt(1-e2*sq(sin(x)));}
double N(double x) { return a/W(x);}
double M(double x) {
  double w = W(x);
  return a*(1-e2)/(w*w*w);
}

double R(double x) {
  double w = W(x);
  return a*(1-e2)/(w*w);
}

double aa(double x) {
  const double r = 0.25;
  return N(x)*cos(x)*r;
}

//  Расстояние до экватора

double duga1(double x) {
  double A, B, C, D;

  A = 6367558.49587;
  B = 16036.48027;
  C = 16.828067;
  D = 0.021975;

  return A*x-B*sinl(x*2.)+C*sinl(x*4.)-D*sinl(x*6.);
}

double duga1_new(double x) 
{
    double n = (a-b)/(a+b);

    double B0 = b * (1 + n + 5. / 4. * n * n + 5. / 4. * n * n * n);
    double B2 = -b * (3. / 2. * n + 3. / 2. * n * n + 21. / 16. * n * n * n);
    double B4 = n * (15. / 16. * n * n + 15. / 16. * n * n * n);
    double B6 = -b * (35. / 48. * n * n * n);

    double y = B0*x + B2*sinl(2*x) + B4*sinl(4*x) + B6*sinl(6*x);

    return y;
}

double duga2(double x1, double x2) {
  return duga1(x2)-duga1(x1);
}

double BB(double x) {
  double x1, x2, xx, delta, d;

  if (x < 0  || x > 9000000) return 0.;

  x1 = 0, x2 = M_PI/2;

  delta = 0.000001;
  delta = 0.000000001;

  do {
    xx = (x1+x2)/2;

    d = duga1(xx)-x;

    if (d < 0) {
      d = -d;
      x1 = xx;
    }
    else {
      x2 = xx;
    }
  } while (d > delta);

  return xx;
}

//-----------------------------------------------
// Перевод градусов в метры
//-----------------------------------------------

void d2m(int zona, double B, double L, double &x, double &y) {
  double X, l, Br, a2, a4, a6, N0, t2, n2, cosB, sinB;
  double b1, b3, b5, l2, l3, l4, l5, l6;
  int nz;

  e2 =(a*a-b*b)/(a*a);
  e12=(a*a-b*b)/(b*b);

  if (zona < 0)
      nz = L/6+1;
  else
      nz = zona;

  Br = rad(B);
  l = rad(L-(6*nz-3));
  X = duga1(Br);
//  X = duga1(B);
  cosB = cos(Br);
  sinB = sin(Br);
  t2 = tanl(Br)*tanl(Br);
  n2 = e12*cosB*cosB;

  l2 = l*l;  l3 = l2*l;  l4 = l3*l;  l5 = l4*l;  l6 = l5*l;

  N0 = N(Br);

  a2 = N0/2*cosB*sinB;
  a4 = N0*pow(cosB, 3)*sinB/24*(5. - t2 + 9.*n2 + 4.*n2*n2);
  a6 = N0*pow(cosB, 5)*sinB/720*(61. - 58.*t2 + t2*t2);

  x = X+a2*l2+a4*l4+a6*l6;

  b1 = N0*cosB;
  b3 = N0*pow(cosB, 3)/6*(1-t2+n2);
  b5 = N0*pow(cosB, 5)/120*(5. - 18.*t2 + t2*t2 + 14.*n2 - 58.*n2*t2);

  y = b1*l+b3*l3+b5*l5+500000 + nz*1000000;
}



void d2m3(SystemCoord& system_coord, double B, double L, double &x, double &y) 
{
  double X, l, Br, a2, a4, a6, N0, t2, n2, cosB, sinB;
  double b1, b3, b5, l2, l3, l4, l5, l6;
//  int nz;

  e2 =(a*a-b*b)/(a*a);
  e12=(a*a-b*b)/(b*b);

//  if (zona < 0)
//      nz = L/6+1;
//  else
//      nz = zona;

  Br = rad(B - system_coord.Latitude_Of_Origin);
  l = rad(L- system_coord.Central_Meridian);
  X = duga1(Br);
//  X = duga1(B);
  cosB = cos(Br);
  sinB = sin(Br);
  t2 = tanl(Br)*tanl(Br);
  n2 = e12*cosB*cosB;

  l2 = l*l;  l3 = l2*l;  l4 = l3*l;  l5 = l4*l;  l6 = l5*l;

  N0 = N(Br);

  a2 = N0/2*cosB*sinB;
  a4 = N0*pow(cosB, 3)*sinB/24*(5. - t2 + 9.*n2 + 4.*n2*n2);
  a6 = N0*pow(cosB, 5)*sinB/720*(61. - 58.*t2 + t2*t2);

  x = X+a2*l2+a4*l4+a6*l6;

  b1 = N0*cosB;
  b3 = N0*pow(cosB, 3)/6*(1-t2+n2);
  b5 = N0*pow(cosB, 5)/120*(5. - 18.*t2 + t2*t2 + 14.*n2 - 58.*n2*t2);

  y = b1*l+b3*l3+b5*l5;

///
//  x *= system_coord.Scale_Factor;
  y *= system_coord.Scale_Factor;


  x += system_coord.False_Northing;
  y += system_coord.False_Easting;




  double alpha = system_coord.Angle * M_PI / 180;

  double xx = x, yy = y;

  x = xx * cos(alpha) - yy * sin(alpha);
  y = xx * sin(alpha) + yy * cos(alpha);

///
//  x *= system_coord.Scale_Factor;
//  y *= system_coord.Scale_Factor;

}


//-----------------------------------------------
// Перевод метров в градусы 6 градусная
//-----------------------------------------------


void m2d(double x, double y, double &B, double &L) {
  double a2, a4, a6, sec, t0, t02, n02;
  double Br, V02, N0;
  double b1, b3, b5;
  double yN1, yN2, yN3, yN4, yN5, yN6;
  int nz;

  nz = y/1000000;
  y -= (nz*1000000+500000);

  e2 =(a*a-b*b)/(a*a);
  e12=(a*a-b*b)/(b*b);

  Br = BB(x);

  sec = 1./cos(Br);
  t0 = tanl(Br);
  t02 = t0*t0;
  n02 = e12*sq(cos(Br));
  V02 = 1+n02;
  N0 = N(Br);

  yN1 = y/N0;
  yN2 = yN1*yN1;  yN3 = yN2*yN1;
  yN4 = yN3*yN1;  yN5 = yN4*yN1;  yN6 = yN5*yN1;

  a2 = -t0/2 * V02;
  a4 = t0/24. * (5.+3.*t02+6*n02-6*n02*t02);
  a6 = -t0/720. * (61.+90*t02+45*t02*t02);
  B = (Br+a2*yN2+a4*yN4+a6*yN6)*p;

  b1 = sec;
  b3 = -sec/6.*(1.+2.*t02+n02);
  b5 = sec/120*(5.+28.*t02+24.*t02*t02+6.*n02+8.*n02*t02);
  L = (b1*yN1+b3*yN3+b5*yN5)*p+(6*nz-3);
}

// Метры в градусы 3 градусная система


void m2d3(SystemCoord& system_coord, double x, double y, double &B, double &L) {

//  double a = 6378245.;
//  double b = a*(1.-1./298.3);

  double a2, a4, a6, sec, t0, t02, n02;
  double Br, V02, N0;
  double b1, b3, b5;
  double yN1, yN2, yN3, yN4, yN5, yN6;

///
//  x /= system_coord.Scale_Factor;
//  y /= system_coord.Scale_Factor;

  double alpha = system_coord.Angle * M_PI / 180;

  double xx = x, yy = y;
 

//  x = xx * cos(alpha) + yy * sin(alpha);
//  y = -xx * sin(alpha) + yy * cos(alpha);

  x -= system_coord.False_Northing;
  y -= system_coord.False_Easting;


///
//  x /= system_coord.Scale_Factor;
  y /= system_coord.Scale_Factor;

  e2 =(a*a-b*b)/(a*a);
  e12=(a*a-b*b)/(b*b);

  Br = BB(x);

  sec = 1./cos(Br);
  t0 = tanl(Br);
  t02 = t0*t0;
  n02 = e12*sq(cos(Br));
  V02 = 1+n02;
  N0 = N(Br);

  yN1 = y/N0;
  yN2 = yN1*yN1;  yN3 = yN2*yN1;
  yN4 = yN3*yN1;  yN5 = yN4*yN1;  yN6 = yN5*yN1;

  a2 = -t0/2 * V02;
  a4 = t0/24. * (5.+3.*t02+6*n02-6*n02*t02);
  a6 = -t0/720. * (61.+90*t02+45*t02*t02);
  B = (Br+a2*yN2+a4*yN4+a6*yN6)*p + system_coord.Latitude_Of_Origin;

  b1 = sec;
  b3 = -sec/6.*(1.+2.*t02+n02);
  b5 = sec/120*(5.+28.*t02+24.*t02*t02+6.*n02+8.*n02*t02);
  L = (b1*yN1+b3*yN3+b5*yN5)*p + system_coord.Central_Meridian; // +(6*nz-3);
}

double m2mas(double x, double y) {
  int nz;
  double R0;

  nz = y/1000000;
  y -= (nz*1000000+500000);

  R0 = R(BB(x));

  double yR1, yR2, yR4;
  yR1 = y/R0; yR2 = yR1*yR1; yR4 = yR2*yR2;

  return 1. + yR2/2. + yR4/24.;
}

double geo_len(double x1, double y1, double x2, double y2) {
  int nz;
  double R0, x, y, s, dy, ds;

  nz = y1/1000000;
  y1 -= (nz*1000000+500000);
  y2 -= (nz*1000000+500000);

  s = hypot(x1-x2, y1-y2);
  x = (x1+x2)/2;
  y = (y1+y2)/2;
  dy = fabs(y2-y1);

  R0 = R(BB(x));

  ds = 1. + pow(y/R0, 2)/2. + pow(dy/R0, 4)/24.;

  return s*ds;
}

void grad(double ug, int &gr, int &mn, double &sc) {
  if (ug >= 0) {
    gr = ug;
    mn = (ug-gr)*60;
    sc = ((ug-gr)*60-mn)*60;
  }
  else {
    ug = -ug;
    gr = ug;
    mn = (ug-gr)*60;
    sc = ((ug-gr)*60-mn)*60;
    gr = -gr;
  }
}

//-----------------------------------------------
// Вычисление координат углов по названию планшета
//-----------------------------------------------

int plan_coor(const char *pn, double &x1, double &x2, double &y1, double &y2) {
  int i, k, n0 = 0, n1 = 0, n2 = 0, n3 = 0, n4 = 0, n5 = 0;
  double dx = 1./3., dy = 0.5;
  char s[80], ss[80];
  const char r[] = "АВСDЕFGНIJКLМNОРQRSТUVWХYZ";
  const char e[] = "A-B--E----K-MHO-PCTY-X----------";

  strcpy(s, pn);
  AnsiUpper(s);

  if (s[1] != '-') return 0;

// Первая буква

//  if ('Я'<= s[0] && s[0] <= 'А') {
  if ('А' <= s[0] && s[0] <= 'Я') {
    s[0] = e[s[0]-'А'];
  }
  if ('A' > s[0] || s[0] > 'Z') return 0;
  n0 = s[0]-'A';

// Две цифры

  for ( k = 0, i = 2; isdigit(s[i]); i++ ) ss[k++] = s[i]; ss[k] = 0;
  if (ss[0] == 0 || s[i] != '-') return 0;
  n1 = atoi(ss);

// Буква для 1 : 500 000

  if (strchr(_TR("АаБбВвГг"), s[i+1])) {
    n2 = n3 = 0;
    if (strchr(_TR("БбГг"), s[i+1])) {
      n3 = 1;
    }
    if (strchr(_TR("ВвГг"), s[i+1])) {
      n2 = 1;
    }

    x2 = (n0+1)*4.-n2*2;
    x1 = x2-2;

    y1 = -186.+n1*6+n3*3;
    y2 = y1+3;

    return 1;
  }



// Еще две цифры

  for ( k = 0, i++; isdigit(s[i]); i++ ) ss[k++] = s[i]; ss[k] = 0;
//  if (ss[0] == 0 || s[i] != '-') return 0;
  if (ss[0] == 0) return 0;
  n2 = atoi(ss);

// Буква

  if (strlen(s) > i+1) {
    if ('A'<= s[i+1] && s[i+1] <= 'Z') s[i+1] = r[s[i+1]-'A'];
    if (s[i+1] > 0) return 0;
    n3 = s[i+1]-'А';
    dx/=2.;  dy/=2.;
  }

// Буква

  if (strlen(s) > i+3) {
    if ('A'<= s[i+3] && s[i+3] <= 'Z') s[i+3] = r[s[i+3]-'A'];
    if (s[i+3] > 0) return 0;
    n4 = s[i+3]-'А';
    dx/=2.;  dy/=2.;
  }

// Цифра

  if (strlen(s) > i+5) {
    if ('1' > s[i+5]  && s[i+5] > '4') return 0;
    n5 = s[i+5]-'1';
    dx/=2.;  dy/=2.;
  }

  x2 = (n0+1)*4. - 1./3.*((n2-1)/12) - 1./6.*(n3/2) - 1./12*(n4/2) - 1./24*(n5/2);
  x1 = x2-dx;

  y1 = -186.+n1*6+((n2-1)%12)*0.5 + 0.25*(n3%2) + 0.125*(n4%2) + 0.0625*(n5%2);
  y2 = y1+dy;

  return 1;
}

const bool ispravleno = false;
//bool ispravleno = true;

// Метры из 42 года 3 градусной в метры WGS84


//double _dx = -13;
//double _dy = +15;

//double _dx = -12;
//double _dy = +16;


//double _dx = 0;
//double _dy = 0;

double lon2x_(double x)
{
    return x;
}

double lat2y_(double y)
{
    double lat = y / 180. * M_PI;

    return log(tan(M_PI / 4 + lat / 2)) * 180 / M_PI;
}



void m2m(SystemCoord& system_coord, double X1, double Y1, double &xx, double &yy, bool shift)
{
    double xxx, yyy;

    if (shift) {
        X1 += system_coord.dy;
        Y1 += system_coord.dx;
    }

    if (system_coord.Scale_Factor < 1) {
        X1 += 2000;

        m2d3(system_coord, X1, Y1, xxx, yyy);

        xx = xxx;
        yy = yyy;

        yy = lon2x(yy) * 100000;
        xx = lat2y(xx) * 100000;

        return;
    }


    m2d3(system_coord, X1, Y1, xxx, yyy);

    if (ispravleno) {
        xx = xxx;
        yy = yyy;
    }
    else {
        xx = SK42_WGS84_Lat(xxx, yyy, 0);
        yy = SK42_WGS84_Long(xxx, yyy, 0);
    }

    yy = lon2x_(yy) * 100000;
    xx = lat2y_(xx) * 100000;
}

// Метры из WGS84 в метры 42 года 3 градусной 

static double geocoef = 1.;

double x2lon_(double x)
{
    return x/geocoef;
}


double y2lat_(double y)
{
    double lat = y / 180. * M_PI / geocoef;
    return (atan(exp(lat)) * 360 / M_PI - 90);
}


void m2m2(SystemCoord& system_coord, double X1, double Y1, double &xx, double &yy, bool shift)
{

//656273.21935
//4789142.7807

    double xxx, yyy;

/*
    if (system_coord.Scale_Factor < 1) {
        yyy = x2lon(Y1 / 100000);
        xxx = y2lat(X1 / 100000);

        d2m3(system_coord, xxx, yyy, xx, yy);

        xx -= 2000;
        return;
    }
*/


/*
    
    double xxxx1 = 43.238895758;
    double yyyy1 = 76.924717976;
//    double x01 = WGS84_SK42_Lat(xxxx1, yyyy1, 0);
//    double y01 = WGS84_SK42_Long(xxxx1, yyyy1, 0);

    d2m3(system_coord, xxxx1, yyyy1, xx, yy);
//    d2m3(system_coord, x01, y01, xx, yy);
  
*/

    yy = x2lon_(Y1/100000);
    xx = y2lat_(X1/100000) ;

    if (ispravleno) {
        xxx = xx;
        yyy = yy;
    }
    else {
        xxx = WGS84_SK42_Lat(xx, yy, 0);
        yyy = WGS84_SK42_Long(xx, yy, 0);
    }


    d2m3(system_coord, xxx, yyy, xx, yy);

    if (shift) {
        xx -= system_coord.dy;
        yy -= system_coord.dx;
    }

}

