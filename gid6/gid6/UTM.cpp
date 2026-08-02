#include "stdafx.h"

#include <math.h>
#include <stdio.h>

#define M_PI        3.14159265358979323846
#define Z35 25


static double a = 6378.137;
static double f = 1/298.257223563;
static double k0 = 0.9996;
static double N0 = 0; //  in the northern hemisphere N0 =0 km and in the southern hemisphere N0=10000 
static double Hemi = 1;  // Hemi=+1 for Northern, Hemi=-1 for Southern
static double E0 = 500;

static double n, A, a1, a2, a3, b1, b2, b3, g1, g2, g3;

static int is_init = 0;

void init_UTM()
{
  if (!is_init) {
    is_init = 1;

    n = f/(2.-f);

    A = a/(1.+n)*(1.+n*n/4.+n*n*n*n/64.);  // + ...

    a1 = 1./2.*n-2./3.*n*n+5./16.*n*n*n;
    a2 = 13./48.*n*n-3./5.*n*n*n;
    a3 = 61./240.*n*n*n;

    b1 = 1./2.*n-2./3.*n*n+37./96.*n*n*n;
    b2 = 1./48.*n*n+1./15.*n*n*n;
    b3 = 17./480.*n*n*n;

    g1 = 2.*n-2./3.*n*n-2.*n*n*n;
    g2 = 7./3.*n*n-8./5.*n*n*n;
    g3 = 56./15.*n*n*n;
  }
}


double arth(double x)
{
  return log((1.+x)/(1.-x))/2.;
}


void deg2UTM(double L, double B, double &E, double&N)
{
  double L0 = 3;

  L0 = int(L/6)*6+3;

  L0 = L0*M_PI/180;
  L = L*M_PI/180;
  B = B*M_PI/180;

  init_UTM();


  double t = sinh(arth(sin(B)) -2.*sqrt(n)/(1.+n)*arth(2.*sqrt(n)/(1.+n)*sin(B)));
  double x1 = atan(t/cos(L-L0));
  double h1 = arth(sin(L-L0)/sqrt(1.+t*t));

  double s = 1.+2.*1.*a1*cos(2.*1.*x1)*cosh(2.*1.*h1) 
               +2.*2.*a2*cos(2.*2.*x1)*cosh(2.*2.*h1)
               +2.*3.*a3*cos(2.*3.*x1)*cosh(2.*3.*h1);


  double tau = 1.+2.*1.*a1*sin(2.*1.*x1)*sinh(2.*1.*h1) 
                 +2.*2.*a2*sin(2.*2.*x1)*sinh(2.*2.*h1)
                 +2.*3.*a3*sin(2.*3.*x1)*sinh(2.*3.*h1);


  E = E0+k0*A*(h1+a1*cos(2.*1.*x1)*sinh(2.*1.*h1)+a2*cos(2.*2.*x1)*sinh(2.*2.*h1)+a3*cos(2.*3.*x1)*sinh(2.*3.*h1));
  N = N0+k0*A*(x1+a1*sin(2.*1.*x1)*cosh(2.*1.*h1)+a2*sin(2.*2.*x1)*cosh(2.*2.*h1)+a3*sin(2.*3.*x1)*cosh(2.*3.*h1));

  E *= 1000;
  N *= 1000;


  double k = k0*A/a*sqrt((1.+pow((1.-n)/(1.+n)*tan(B), 2))*(s*s+tau*tau)/(t*t+pow(cos(L-L0), 2)));
  double gamma = atan((tau*sqrt(1.+t*t)+s*t*tan(L-L0)) / s*sqrt(1.+t*t)-tau*t*tan(L-L0));
}


void UTM2deg(int Z, double E, double N, double &L, double &B)
{
  E /= 1000;
  N /= 1000;

  init_UTM();

  double x = (N-N0)/(k0*A);
  double i = (E-E0)/(k0*A);

  double x1 = x-b1*sin(2.*1*x)*cosh(2.*1*i)-b2*sin(2.*2*x)*cosh(2.*2*i)-b3*sin(2.*3*x)*cosh(2.*3*i);
  double i1 = i-b1*cos(2.*1*x)*sinh(2.*1*i)-b2*cos(2.*2*x)*sinh(2.*2*i)-b3*cos(2.*3*x)*sinh(2.*3*i);

  double s1 = 1.-b1*cos(2.*1*x)*cosh(2.*1*i)-b2*2*cos(2.*2*x)*cosh(2.*2*i)-b3*3*cos(2.*3*x)*cosh(2.*3*i);
  double t1 = b1*sin(2.*1*x)*sinh(2.*1*i)+b2*2*sin(2.*2*x)*sinh(2.*2*i)+b3*3*sin(2.*3*x)*sinh(2.*3*i);

  double X = asin(sin(x1)/cosh(i1));

  B = X+g1*sin(2.*1*X)+g2*sin(2.*2*X)+g3*sin(2.*3*X);
  L = atan(sinh(i1)/cos(x1));

  B = B*180./M_PI;
  L = L*180./M_PI+Z*6.-183.;

}

void setUTM()
{
  a = 6378.137;
  f = 1./298.257223563;
  k0 = 0.9996;
  N0 = 0; //  in the northern hemisphere N0 =0 km and in the southern hemisphere N0=10000 
  E0 = 500;

  is_init = 0;
  init_UTM();

}

void set42(int zone)
{
  a = 6378.245;
  f = 1./298.3;
  k0 = 1.;
  N0 = 0; //  in the northern hemisphere N0 =0 km and in the southern hemisphere N0=10000 
  E0 = 500+zone*1000;

//  is_init = 0;
  init_UTM();

}

void d2m(int zona, double B, double L, double &x, double &y);
void m2d(double x, double y, double &B, double &L);


void m2UTM(double x, double y, double &N, double &E, int &Z) 
{
  double B, L;

  m2d(x, y, B, L);
  Z = int((L+183.)/6.);

  deg2UTM(L, B, E, N);
}

void UTM2m(int Z, int zona, double N, double E, double &x, double &y) 
{
  double B, L;

  setUTM();

  UTM2deg(Z, E, N, L, B);

  d2m(zona, B, L, x, y);
  
  
  set42(zona);
  deg2UTM(L, B, y, x);

  setUTM();
}

void UTM2m(double N, double E, double &x, double &y) 
{
  double B, L;

  int zona = -1;
  int Z = int(E/1000000.);
  E = E-Z*1000000.;

  setUTM();

  UTM2deg(Z, E, N, L, B);

//  d2m(zona, B, L, x, y);
  
  zona = L/6+1+Z35;

  set42(zona);
  deg2UTM(L, B, y, x);

  setUTM();
}


