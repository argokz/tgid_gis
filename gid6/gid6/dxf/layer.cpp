#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "dxfi.h"

layer_type *find_layer(char *s, cdxf *dxf)
{
  layer_type *l;

  for (l = dxf->ly_anchor; l; l = l->next) {
    if (!_strcmpi(s, l->name)) return l;
  }
//  printf("Слой %s не найден\n", s);
  return NULL;
}

layer_type *find_layer(char *s, int color, cdxf *dxf)
{
  layer_type *l;

  for (l = dxf->ly_anchor; l; l = l->next) {
    if (!_strcmpi(s, l->name) && color == l->color) return l;
  }
//  printf("Слой %s не найден\n", s);
  return NULL;
}


void print_layer(cdxf *dxf)
{
  layer_type *l;
  l = dxf->ly_anchor;

  while (l) {
//--    printf("%s\n", l->name);
    l = l->next;
  }
}

static int layer_num = 0;

layer_type * add_layer(const char *s, cdxf *dxf) 
{
  layer_type *l = NULL;

  if (l = (layer_type*) calloc(1, sizeof(layer_type)))
  {
    l->next = dxf->ly_anchor;
    dxf->ly_anchor = l;
    l->name = locate(s);

//          Я добавил
    l->num = layer_num++;
    l->uze = 1;

  }
//--  else
//--    printf("Не хватает памяти --LAYER\n");

  return l;
}


// Pg. 130
void layer(FILE *f, cdxf *dxf)
{

  int state;
  int quit;
  char *s;
  layer_type *l = NULL;
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
      case N_62 : state = 4; break;
      case N_6  : state = 5; break;
      case N_70 : state = 6; break;
      case N_290 : state = 7; break;
      }
      break;
    case 2:
      state = 1;
      l = NULL;
      if (t == ENDTAB) quit = TRUE;
      break;
    case 3:
      state = 1;
      l = add_layer(s, dxf);
      break;
    case 4:
      state = 1;
      if (l) l->color = atoi(s);
      break;
    case 5:
      state = 1;
      if (l) l->lt = locate(s);
      break;
    case 6:
      state = 1;
      used_flag = atoi(s);
      if (l) l->flags = used_flag;
      break;
    case 7:
      state = 1;
      if (l) l->uze = atoi(s);
//      AfxMessageBox(s);
      break;
    default:
      state = 1;
      break;
    }
  }
}
