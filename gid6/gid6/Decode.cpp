#include "stdafx.h"
#include <memory.h>

void decod(unsigned char *p, unsigned char *u, int n) {
  unsigned char c;
  unsigned i;

  for ( i = 0; i < n; i++ ) {
    c = *p++;

    if (c < 0xC0) {
      *u++ = c;
    }
    else {
      c -= 0xC0;

      memset(u, c, *p++);

      u += c;
      i++;
    }
  }
}


unsigned int cod(unsigned char *p, unsigned char *u, unsigned int n) {
  unsigned char c, cold, nn;
  unsigned int beg, kk, i;

  nn = 0; kk = 0; beg = 0;  cold = p[0];

  for ( i = 0; i < n; i++ ) {
    c = p[i];

    if (beg) {
      cold = c;
      beg = 0;
    }
    else {
      if (c == cold) {
        if (nn < 62 ) nn++;
        else { u[kk++] = 0xFF; u[kk++] = c; nn = 1; beg = 1; }
      }
      else {
        if (nn > 1 || cold >= 0xC0) u[kk++] = nn+0xC0;
        u[kk++] = cold; nn = 1; cold = c;
      }
    }
  }

  if (nn > 1 || c >= 0xC0) u[kk++] = nn+0xC0;
  u[kk++] = c;

  return kk;
}
