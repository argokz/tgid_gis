#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#include <math.h>

using namespace std;

#include "Token.h"
#include "dxfi.h"


void block_record(FILE* f, cdxf* dxf);
void vport(FILE* f, cdxf* dxf);
void style(FILE* f, cdxf* dxf);
void view(FILE* f, cdxf* dxf);
void objects(FILE* f, cdxf* dxf);

void ltype(FILE *f, cdxf *dxf);
void layer(FILE *f, cdxf *dxf);
void entities(FILE *f, block_type *blk, cdxf *dxf, int is_blk);
void print_generic();
void conv(const char *);

cdxf::~cdxf() 
{
  generic *enn, *en = en_anchor;
  while ( en ) {
    enn = en->next;      
    free(en);
    en = enn;
  }
}


// Pg. 116
void header(FILE *f, cdxf *dxf) 
{
  int state;
  int quit;
  int t;
  char *c;
  double *p1, *p2;
  int *p = NULL;

  p1 = p2 = NULL;

  quit = FALSE;
  state = 1;

  while (!quit) 
  {
    t = get_tag_token(c = get_tag(f));

    switch (state)
    {
    case 1:
      state = 6;
      if (t == N_3) state = 8;
      if (t == N_9) state = 3;
      if (t == N_0) state = 2;
      if (t == N_10) state = 4;
      if (t == N_20) state = 5;
      if (t == N_40) state = 4;
      if (t == N_70) state = 7;
      break;
    case 2:
      if (t == ENDSEC) quit = TRUE;
      state = 1;
      break;
    case 3:
      state = 1;
      switch(t)
      {
      case EXTMIN :
        p1 = &dxf->extent_x;
        p2 = &dxf->extent_y;
        break;
      case EXTMAX :
        p1 = &dxf->extent_mx;
        p2 = &dxf->extent_my;
        break;
      case PEXTMIN :
        p1 = &dxf->pextmin_x;
        p2 = &dxf->pextmin_y;
        break;
      case PEXTMAX :
        p1 = &dxf->pextmax_x;
        p2 = &dxf->pextmax_y;
        break;
      case LIMMIN :
        p1 = &dxf->limit_x;
        p2 = &dxf->limit_y;
        break;
      case LIMMAX :
        p1 = &dxf->limit_mx;
        p2 = &dxf->limit_my;
        break;
      case CMLSCALE :
        p1 = &dxf->cmlscale;
        break;
      case PLINEWID :
        p1 = &dxf->plinewid;
        break;
      case TRACEWID :
        p1 = &dxf->tracewid;
        break;
      case INSUNITS :
        p = &dxf->insunits;
        break;
      case DWGCODEPAGE :
        p = &dxf->dwgcodepage;
        break;
      case MEASUREMENT :
        p = &dxf->measurement;
        break;
      }
      break;
    case 4:
      if (p1) *p1 = atof(c);
      p1 = NULL;
      state = 1;
      break;
    case 7:
      if (p) *p = atoi(c);
      p = NULL;
      state = 1;
      break;
    case 5:
      if (p2) *p2 = atof(c);
      p2 = NULL;
      state = 1;
      break;
    case 6:
      state = 1;
      break;
    case 8:
      *p = 1;
      if (!qstricmp(c, "ANSI_1251")) {
//        *p = 3;
      }
      p = NULL;
      state = 1;
      break;
    }
  }
}


///void blocks(FILE *f, cdxf *dxf)
//{
//}



void blocks(FILE *f, cdxf *dxf) 
{
  int state;
  int quit;
  int t;
  char *c;

 
  quit = FALSE;
  state = 1;

  while (!quit) 
  {
    t = get_tag_token(c = get_tag(f));

    switch (state)
    {
    case 1:
      if (t == N_0) state = 2;
      break;
    case 2:
      state = 1;
      if (t == ENDSEC) quit = TRUE;
      if (t == ENDBLK) state = 1;
      if (t == BLOCK) {
        state = 1;
        entities(f, NULL, dxf, 1);  
      }
      break;
    }
  }
 }




// Pg. 119
void tables(FILE *f, cdxf *dxf) 
{
  int state;
  int quit;
  int t;
  char *c;

  quit = FALSE;
  state = 1;

  while (!quit) 
  {
    t = get_tag_token(c = get_tag(f));

    switch (state)
    {
    case 1:
      if (t == N_0) state = 2;
      break;
    case 2:
      state = 1;
      if (t == ENDSEC) quit = TRUE;
      if (t == ENDTAB) state = 1;
      if (t == TABLE) state = 3;
      break;
    case 3:
      state = 1;
      if (t == ENDSEC) quit = TRUE;
      if (t == N_2) state = 4;
      break;
    case 4:
      state = 1;
      switch(t)
      {
      case LTYPE : ltype(f, dxf); break;
      case LAYER : layer(f, dxf); break;
      case STYLE : style(f, dxf); break;
      case VIEW  : view(f, dxf);  break;
      case BLOCK_RECORD: block_record(f, dxf); break;
      case VPORT: vport(f, dxf); break;
      default:
          break;
      }
      break;
    }
  }
}


void dxf_in(FILE *f, cdxf *dxf)
{
  int state;
  int quit;
  int t;
  char *c;

  quit = FALSE;
  state = 1;

  while (!quit) 
  {
      c = get_tag(f);
    t = get_tag_token(c);



    switch (state)
    {
    case 1:
      state = 2;
      if (t == N_0) state = 3;
      break;
    case 2:
      state = 1;
      if (t == EOFILE) quit = TRUE;
      break;
    case 3:
      state = 1;
      if (t == SECTION) state = 4;
      break;
    case 4:
      state = 2;
      if (t == N_2) state = 5;
      break;
    case 5:
      state = 1;
      switch (t)
      {
      case HEADER   : header(f, dxf); state = 1; break;
      case TABLES   : tables(f, dxf); state = 1; break;
      case BLOCKS   : blocks(f, dxf); state = 1; break;
      case OBJECTS  : objects(f, dxf); state = 1; break;
      case ENTITIES : 
        entities(f, NULL, dxf, 0); 
        state = 1; break;
      }
      break;
    }
    if (feof(f)) break;
  }

  double d = 1;
  double in = 0.0254;

  if (dxf->insunits == 0) {
    dxf->insunits = 6;
  }
  
  switch (dxf->insunits)
  {
  case 0: d = 0.0001; break;         // 0 = Unitless;            
  case 1: d = in; break;         // 1 = Inches;              
  case 2: d = in*12; break;         // 2 = Feet;                
  case 3: d = 5280*12*in; break;   //    // 3 = Miles;               
  case 4: d = 0.001; break;         // 4 = Millimeters;         
  case 5: d = 0.01; break;          // 5 = Centimeters;         
//  case 6: d = 1; break;           // 6 = Meters;              
  case 6: d = 0.001; break;           // 6 = Meters;              
  case 7: d = 1000; break;         // 7 = Kilometers;          
  case 8: d = in/1000; break;      // 8 = Microinches;         
  case 9: d = 100; break;         // 9 = Mils;                
  case 10: d = in*36; break;      // 10 = Yards;              
  case 11: d = 100; break;        // 11 = Angstroms;          
  case 12: d = 100; break;        // 12 = Nanometers;         
  case 13: d = 100; break;        // 13 = Microns;            
  case 14: d = 0.1; break;        // 14 = Decimeters;         
  case 15: d = 10; break;         // 15 = Decameters;         
  case 16: d = 100; break;        // 16 = Hectometers;        
  case 17: d = 1e9; break;        // 17 = Gigameters;         
  case 18: d = 100; break;        // 18 = Astronomical units; 
  case 19: d = 100; break;        // 19 = Light years;        
  case 20: d = 100; break;        // 20 = Parsecs             
  }

  dxf->p25400 = d*100000;
//  dxf->p25400 = 0.1/d;
}

void dxf_min_max(cdxf *dxf, double &xmin, double &xmax, double &ymin, double &ymax)
{
  xmin = 1e100, xmax = -1e100, ymin = 1e100, ymax = -1e100;
  generic *en;

  for (en = dxf->en_anchor; en; en = en->next) {
    if ( !en->lyr || !en->lyr->uze ) {
      continue;
    }

    if (en->xmin == en->xmax && en->ymin == en->ymax && en->xmin == 0 && en->xmax == 0) {
      continue;
    }

    xmin = min(xmin, en->xmin);
    xmax = max(xmax, en->xmax);

    ymin = min(ymin, -en->ymax);
    ymax = max(ymax, -en->ymin);
  }
  xmin *= dxf->p25400;
  xmax *= dxf->p25400;
  ymin *= dxf->p25400;
  ymax *= dxf->p25400;
}


/*
void main(int ac, char **av) {
  init_tok_table();

  FILE *f;

  char s[256];

  if (ac <= 1) strcpy(s, "k10.dxf");
//  if (ac <= 1) strcpy(s, "1004.dxf");
  else 
    strcpy(s, av[1]);

  f = fopen(s, "r");
  if (f) {
    dxf_in(f);
    fclose(f);
    conv(s);
  }
}
*/







void block_record(FILE* f, cdxf* dxf)
{
    int state;
    int quit;
    char* s;
//    layer_type* l = NULL;
    int t;

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
            case N_0: state = 2; break;
            case N_2: state = 3; break;
            case N_62: state = 4; break;
            case N_6: state = 5; break;
            case N_70: state = 6; break;
            case N_290: state = 7; break;
            case N_100: state = 8; break;
            }
            break;
        case 2:
            state = 1;
//            l = NULL;
            if (t == ENDTAB) quit = TRUE;
            break;
        case 3:
            state = 1;
            break;

        default:
            state = 1;
            break;
        }
    }
}



void vport(FILE* f, cdxf* dxf)
{
    int state;
    int quit;
    int t;
    char* c;


    quit = FALSE;
    state = 1;

    double x1 = 0, x2 = 0, y1 = 0, y2 = 0, x0 = 0, y0 = 0;

    while (!quit)
    {
        t = get_tag_token(c = get_tag(f));

        switch (state)
        {
        case 1:
            state = 0;
            switch (t)
            {
            case N_0: state = 2; break;
            case N_2: state = 3; break;

            case N_10: state = 4; break;
            case N_20: state = 5; break;
            case N_11: state = 6; break;
            case N_21: state = 7; break;
            case N_12: state = 8; break;
            case N_22: state = 9; break;
            }

            break;
        case 2:
            state = 1;
            if (t == ENDTAB) quit = TRUE;
            break;
        case 3:
            state = 1;
            break;
        case 4:
            state = 1;  x1 = atof(c); break;
        case 5:
            state = 1;  y1 = atof(c); break;
        case 6:
            state = 1;  x2 = atof(c); break;
        case 7:
            state = 1;  y2 = atof(c); break;
        case 8:
            state = 1;  x0 = atof(c); break;
        case 9:
            state = 1;  y0 = atof(c); break;
        default:
            state = 1;
            break;
        }
    }
}


void style(FILE *f, cdxf *dxf)
{
  int state;
  int quit;
  int t;
  char *c;

 
  quit = FALSE;
  state = 1;

  while (!quit) 
  {
    t = get_tag_token(c = get_tag(f));

    switch (state)
    {
    case 1:
        state = 0;
        if (t == N_0) state = 2;
      break;
    case 2:
      state = 1;
      if (t == ENDTAB) quit = TRUE;
      break;
    default:
        state = 1;
        break;
    }
  }
}

void view(FILE* f, cdxf* dxf)
{
    int state;
    int quit;
    int t;
    char* c;


    quit = FALSE;
    state = 1;

    while (!quit)
    {
        t = get_tag_token(c = get_tag(f));

        switch (state)
        {
        case 1:
            state = 0;
            if (t == N_0) state = 2;
            break;
        case 2:
            state = 1;
            if (t == ENDTAB) quit = TRUE;
            break;
        default:
            state = 1;
            break;
        }
    }
}

void layout(FILE* f, cdxf* dxf)
{
    int t;
    char* c, c1[256];
//    double x3, y3;
    layout_type *lt = (layout_type*)calloc(1, sizeof(layout_type));

    while (true)
    {
        t = get_tag_token(c = get_tag(f));   if (t == N_0) break;
        strcpy(c1, c);
        c = get_tag(f);

        switch (t)
        {
        case N_1:  lt->name = locate(c); break;
        case N_10: lt->x10 = atof(c); break;
        case N_20: lt->y20 = atof(c); break;
        case N_11: lt->x11 = atof(c); break;
        case N_21: lt->y21 = atof(c); break;
        case N_14: lt->xmin = atof(c); break;
        case N_24: lt->ymin = atof(c); break;
        case N_15: lt->xmax = atof(c); break;
        case N_25: lt->ymax = atof(c); break;
//        case N_15: x3 = atof(c); break;
//        case N_25: y3 = atof(c); break;
        default:   break;
        }

    }
    lt->next = dxf->layout_anchor;
    dxf->layout_anchor = lt;


}



void objects(FILE* f, cdxf* dxf)
{
    int state;
    int quit;
    int t;
    char* c;


    quit = FALSE;
    state = 1;

    while (!quit)
    {
        t = get_tag_token(c = get_tag(f));

        switch (state)
        {
        case 1:
            state = 0;
            if (t == N_0) state = 2;
            break;
        case 2:
            state = 1;
            if (t == LAYOUT) {
                state = 2;
                layout(f, dxf);
            }
            else if (t == EOF) quit = TRUE;
            else if (t == ENDSEC) quit = TRUE;
            else if (t == DICTIONARY) {
                state = 1;
            }
            else {
                state = 1;
            }
            break;
        default:
            state = 1;
            break;
        }
    }
}
