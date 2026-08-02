#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include "coordlis.h"

double Det(int n, const double *A) {
  double Result = 1;
  int k = 0;
  int i, j, l, f, z;
  double m1, t;

  double *a = new double[n*n];

  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      a[i*n+j] = A[i*n+j];
    }
  }

  do
  {
    m1 = 0;
    i = k;
    while (i < n) {
      t = a[i*n+k];
      if (fabs(t) > fabs(m1)) {
        m1 = t;
        j = i;
      }
      i++;
    }
    if (m1 == 0)
    {
      Result = 0;
      k = n+1;
    }
    else
    {
      if (j != k)
      {
        Result = -Result;
        l = k;
        while (l < n) 
        {
          t = a[j*n+l];
          a[j*n+l] = a[k*n+l];
          a[k*n+l] = t;
          l++;
        }
      }
      f = k+1;
      while (f < n)
      {
        t = a[f*n+k]/m1;
        z = k+1;
        while (z < n)
        {
          a[f*n+z] = a[f*n+z]-t*a[k*n+z];
          z++;
        }
        f++;
      }
      Result = Result*a[k*n+k];
    }
    k++;
  }
  while (k < n);

  delete [] a;

  return Result;
}


void lur(int n, const double *a, const double *b, double *x)
{
  int s = 0;
  int i, j;
  double *t,D;

  t = new double[n*n];

  s = 1;
  D = Det(n, a);

  if (D != 0)
  {
    for (i = 0; i < n; i++) {
      for (j = 0; j < n; j++) {
        t[i*n+j] = a[i*n+j];
      }
    }

    for (i = 0; i < n; i++) {
      t[i*n] = b[i];
    }

    for (i = 1; i < n; i++) {
      x[i-1] = Det(n, t)/D;

      for (j = 0; j < n; j++) {
        t[j*n+i-1] = a[j*n+i-1];
        t[j*n+i] = b[j];
      }
    }

    x[n-1] = Det(n, t)/D;
  }
  delete [] t;

//--  for (i = 0; i < n; i++) {
//--    printf("x[%d] = %g\n", i, x[i]);
//--  }
}

void preobr(const CFPoint *p1, const CFPoint *p2, double *x)
{
  double a[36];
  double b[6];

  for (int i = 0; i < 36; i++) a[i] = 0;

  a[0]  = p1[0].x; a[1]  = p1[0].y; a[2]  = 1;
  a[9]  = p1[0].x; a[10] = p1[0].y; a[11] = 1;
  a[12] = p1[1].x; a[13] = p1[1].y; a[14] = 1;
  a[21] = p1[1].x; a[22] = p1[1].y; a[23] = 1;
  a[24] = p1[2].x; a[25] = p1[2].y; a[26] = 1;
  a[33] = p1[2].x; a[34] = p1[2].y; a[35] = 1;

  b[0] = p2[0].x;  b[1] = p2[0].y;
  b[2] = p2[1].x;  b[3] = p2[1].y;
  b[4] = p2[2].x;  b[5] = p2[2].y;

  lur(6, a, b, x);
}
/*
void main()
{
  Point p1[3] = {1,1,1,2,5,1}, p2[3] = {7,8,9,10,11,12};
  double x[6];
 

  preobr(p1, p2, x);
}
*/