#include "stdafx.h"
#include "lzw.h"

void LZW::decod(unsigned char *p, unsigned char *u) {
  unsigned short code, old;
  unsigned char k;
  unsigned short cc, eoi, n_cod, N00, tec_cep, mask;

  beg = 0;
  pp = (unsigned short*) p;

  uu = (char *) u;

  N00 = 8;
  cc = 1 << N00;
  cc2 = cc+2;
  eoi = cc+1;

  buf.s[0] = *pp++;
  buf.s[1] = *pp++;

LOOP0 :

  N = N00+1;
  n_cod = 1 << N;
  mask = n_cod-1;
  tec_cep = cc2;

  old = get_code() & mask;
  Read_Cep_Wr(old);

LOOP :
  code = get_code() & mask;

  if (code == cc) goto LOOP0;
  if (code == eoi) return;

  if (code < tec_cep) {
    k = Read_Cep_Wr(code);
  }
  else {
    k = Read_Cep_Wr(old);
    *uu++ = k;
  }

  cepk[tec_cep] = k;
  cepp[tec_cep] = old;
  tec_cep ++;

  if (tec_cep == mask) {
    N++;
    n_cod <<= 1;
    mask = n_cod-1;
  }

  old = code;

  goto LOOP;
}
