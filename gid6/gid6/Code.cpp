#include "stdafx.h"

#define N0 8
#define BITS 12                   /* setting the number of bits to 12,13 */
#define HASHING_SHIFT BITS-8      /* or 14 affects several constants.    */
short *code_value;                  /* This is code value array            */
unsigned short *prefix_code;        /* This array holds the prefix codes   */
unsigned char *append_character;  /* This array holds the appended chars */

#define TABLE_SIZE 5021

void LzwInit() {
  code_value = new short[TABLE_SIZE];
  prefix_code = new unsigned short[TABLE_SIZE];
  append_character = new unsigned char[TABLE_SIZE];
//  if (code_value==NULL || prefix_code==NULL || append_character==NULL) {
//    printf("Fatal error allocating table space!\n");
//    exit(1);
//  }
}

void LzwExit() {
  delete [] code_value;
  delete [] prefix_code;
  delete [] append_character;
}

unsigned short tec_cep;
unsigned short cc, eoi, n_cod, N1;
unsigned char *uu1;
unsigned short nu;

static unsigned long buf;
static unsigned short beg;

void put_code(unsigned short c) {
  buf |= ((unsigned long)c << beg);
  beg += N1;

  while (beg >= 8) {
    uu1[nu++] = buf;
    buf >>= 8;
    beg -= 8;
  }
}


/*
** This is the hashing routine. It tries to find a match for the prefix+char
** string in the string table. If it finds it, the index is returned. If
** the string is not found, the first available index in the string table is
** returned instead.
*/
int find_match(int hash_prefix,unsigned int hash_character)
{
  int index,offset;

  index = ((hash_character<<HASHING_SHIFT) ^ hash_prefix);

  if (index==0)
    offset=1;
  else
    offset=TABLE_SIZE-index;

  while (1) {
    if (code_value[index] == -1) return index;

    if (prefix_code[index]==hash_prefix
          && append_character[index]==hash_character) return index;

    index-=offset;
    if (index < 0) index+=TABLE_SIZE;
  }
}


int LzwCode(unsigned char *p, unsigned char *u, int np) {
  int c, i, j, n, old_n, index;

  buf = 0; beg = 0;

  uu1 = u;
  nu = 0;

  old_n = -1;

  N1 = N0+1;

  cc = 1 << N0;
  n_cod = 1 << N1;
  tec_cep = cc+2;

  for (i=0; i < TABLE_SIZE; i++) code_value[i] = -1;

  for (i = 0; i < np; i++) {
    c = p[i];

    if (old_n == -1) {
      n = c;
    }
    else {
      index = find_match(old_n, c);
      n = code_value[index];
    }

    if (n == -1) {
      put_code(old_n);

      code_value[index] = tec_cep;
      prefix_code[index] = old_n;
      append_character[index] = c;

      tec_cep ++;
      n = tec_cep;

      if (n == n_cod) {
        N1++;
        n_cod = 1 << N1;
      }
      if (n == 4096) {
        put_code(cc);
        N1 = N0+1;
        n_cod = 1 << N1;
        tec_cep = cc+2;
        for (j=0; j < TABLE_SIZE; j++) code_value[j] = -1;
      }
      n = c;
    }
    old_n = n;
  }
  put_code(old_n);

  put_code(cc+1);
  put_code(0);

  return nu;
}
