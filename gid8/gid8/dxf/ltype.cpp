#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Token.h"
#include "dxfi.h"

ltype_type *find_ltype(char *s, cdxf *dxf)
{
  ltype_type *l;

  for (l = dxf->lt_anchor; l; l = l->next) {
    if (!qstricmp(s, l->name)) return l;
  }
//  printf("Тип линии не найден\n");
  return NULL;
}

// Pg. 124
void ltype(FILE *f, cdxf *dxf)
{
  int state;
  int quit;
  char *s;
  ltype_type *l = NULL;
  dash_type *dl;
  int t;
  int used_flag;

  quit = FALSE;
  state = 1;

  while (!quit)
  {
    t = get_tag_token(s = get_tag(f));

    switch (state)
    {
    case 1:
      state = 0;
      switch (t)
      {
      case N_0  : state = 2; break;
      case N_2  : state = 3; break;
      case N_3  : state = 4; break;
      case N_72 : state = 5; break;
      case N_73 : state = 6; break;
      case N_40 : state = 7; break;
      case N_49 : state = 8; break;
      case N_70 : state = 9; break;
      }
      break;
    case 2:
      state = 1;
      l = NULL;
      if (t == ENDTAB) quit = TRUE;
      break;
    case 3:
      state = 1;
      l = (ltype_type*) calloc(1, sizeof(ltype_type));
      if (l)
      {
        l->next = dxf->lt_anchor;
        dxf->lt_anchor = l;
        l->name = locate(s);
      }
//--      else 
//--        printf("Не хватает памяти --LINETYPE\n");
      break;
    case 4:
      state = 1;
      if (l) l->descript = locate(s);
      break;
    case 5:
      state = 1;
      if (l) l->alignment = atoi(s);
      break;
    case 6:
      state = 1;
      if (l) l->dashes = atoi(s);
      break;
    case 7:
      state = 1;
      if (l) l->length = atof(s);
      break;
    case 8:
      state = 1;
      if (!l) break;
      dl = l->dash_length;
      if (!dl) {
        l->dash_length = (dash_type *) calloc(1, sizeof(dash_type));
        l->next->length = atof(s);
      }
      else {
        while (dl->next) dl = dl->next;
        dl->next = (dash_type *) calloc(1, sizeof(dash_type));
        dl->next->length = atof(s);
      }
      break;
    case 9:
      state = 1;
      used_flag = atoi(s);
      if (l) l->flags = used_flag;
      break;
    default:
      state = 1;
      break;
    }
  }
}
