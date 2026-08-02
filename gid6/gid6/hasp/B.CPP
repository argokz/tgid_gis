#include "stdafx.h"


#include <windows.h>
#include <stdio.h>
#include <string.h>
//#include <bcnthasp.h>
//#include "WIN.h"
#include "hasp.h"


/*
#define OK                        0
*/

extern "C" {
  void hasp(int Service, int SeedCode, int LptNum, int Pass1, int Pass2, int *p1, int *p2, int *p3, int *p4);
}
/*
extern "C" far hasp (int,int,int,int,int,int far *,int far *,
                int far *,int far *);
*/
int   LptNum, SeedCode;
int   Pass1;
int   Pass2;
int   p1, p2, p3, p4;
long  ID;

int ProgCode;   /* The program number for LOGIN */
int RetStatus;    /* The value returned from NET_LAST_STATUS */
int dummy;      /* A dummy parameter for NET_LAST_STATUS */

int login() {
   Pass1 = 1469;
   Pass2 = 13083;
   ProgCode = 0;
   SeedCode = 100;
   p1 = p2 = p3 = p4 = 0;

  return RetStatus;
}

unsigned long GetID() {

  hasp(GET_HASP_IDNUM, SeedCode, ProgCode, Pass1, Pass2,
      &p1, &p2, &p3, &p4);
  hasp(GET_HASP_STATUS, SeedCode, ProgCode, Pass1, Pass2,
      &RetStatus, &dummy, &dummy, &dummy);

  return 65536*(unsigned long)p2+p1;
}

int read_word(int n) {
  p1 = n;         /* The address from which the data word will be read */
  p2 = 0;

  hasp(READ_HASP_MEMO, SeedCode, ProgCode, Pass1, Pass2,
      &p1, &p2, &p3, &p4);

  hasp(GET_HASP_STATUS, SeedCode, ProgCode, Pass1, Pass2,
      &RetStatus, &dummy, &dummy, &dummy);

  if ( RetStatus == 0 ) return -1;

  return p2;
}

int write_word(int n, int x) {
  p1 = n;     /* The address to which the data word will be written */
  p2 = x;      /* The value to be written                  */

  hasp(WRITE_HASP_MEMO, SeedCode, ProgCode, Pass1, Pass2,
        &p1, &p2, &p3, &p4);

  hasp(GET_HASP_STATUS, SeedCode, ProgCode, Pass1, Pass2,
        &RetStatus, &dummy, &dummy, &dummy);

  if ( RetStatus == 0 ) return -1;

  return 0;
}

#if 0

void psw(int k, long l, char *s) {
  long n;
  int i;
  n = l*(k+1) & 0xFFFF;

  for (i = 0; n && i < 8; i++ ) {
    s[i] = 'A' + n%26;
    n = n/26;
  }
  s[i] = 0;
}

void psw(int k, char *s) {
  char ss[8];

  psw(k, 31991L, s);
  psw(k, 22259L, ss); strcat(s, ss);
  psw(k, 27011L, ss); strcat(s, ss);
  s[8] = 0;
}

#endif

void HaspError(int n, char *s) {
  switch (n ) {
    case 15 : strcpy(s, "Не найден активный NetHASP сервер"); break;
    default: sprintf(s, "Ошибка : %d", n);
  }
}
