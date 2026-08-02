#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void main(int ac, char **av) {
  char s[80];

  if (ac > 1) {
    psw(atoi(av[1]), s);
    printf("Ваш пароль %s\n", s);
  }
  else {
    for ( int i = 0; i < 32767; i++) {
      psw(i, s);
      printf("%5d %s\n", i, s);
    }
  }

}
