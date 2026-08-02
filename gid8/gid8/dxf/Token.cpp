#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Token.h"

static tok_tab tok_table[] = {
  { NULL, "0", N_0 },
  { NULL, "1", N_1 },
  { NULL, "2", N_2 },
  { NULL, "3", N_3 },
  { NULL, "4", N_4 },
  { NULL, "5", N_5 },
  { NULL, "6", N_6 },
  { NULL, "7", N_7 },
  { NULL, "8", N_8 },
  { NULL, "9", N_9 },
  { NULL, "10", N_10 },
  { NULL, "11", N_11 },
  { NULL, "12", N_12 },
  { NULL, "13", N_13 },
  { NULL, "14", N_14 },
  { NULL, "15", N_15 },
  { NULL, "20", N_20 },
  { NULL, "21", N_21 },
  { NULL, "22", N_22 },
  { NULL, "23", N_23 },
  { NULL, "24", N_24 },
  { NULL, "25", N_25 },
  { NULL, "31", N_31 },
  { NULL, "39", N_39 },
  { NULL, "40", N_40 },
  { NULL, "41", N_41 },
  { NULL, "42", N_42 },
  { NULL, "43", N_43 },
  { NULL, "44", N_44 },
  { NULL, "45", N_45 },
  { NULL, "49", N_49 },
  { NULL, "50", N_50 },
  { NULL, "51", N_51 },
  { NULL, "62", N_62 },
  { NULL, "66", N_66 },
  { NULL, "70", N_70 },
  { NULL, "71", N_71 },
  { NULL, "72", N_72 },
  { NULL, "73", N_73 },
  { NULL, "74", N_74 },
  { NULL, "90", N_90 },
  { NULL, "93", N_93 },
  { NULL, "100", N_100 },
  { NULL, "290", N_290 },
  { NULL, "NONE", NONE },
  { NULL, "ARC", ARC },
  { NULL, "ATTDEF", ATTDEF },
  { NULL, "ATTRIB", ATTRIB },
  { NULL, "BLOCK", BLOCK },
  { NULL, "BLOCK_RECORD", BLOCK_RECORD },
  { NULL, "BLOCKS", BLOCKS },
  { NULL, "CIRCLE", CIRCLE },
  { NULL, "ELLIPSE", ELLIPSE },
  { NULL, "DIMENSION", DIMENSION },
  { NULL, "ENDBLK", ENDBLK },
  { NULL, "ENDSEC", ENDSEC },
  { NULL, "$EXTMIN", EXTMIN },
  { NULL, "$EXTMAX", EXTMAX },
  { NULL, "$PEXTMIN", PEXTMIN },
  { NULL, "$PEXTMAX", PEXTMAX },
  { NULL, "$INSUNITS", INSUNITS },
  { NULL, "SEQEND", SEQEND },
  { NULL, "ENDTAB", ENDTAB },
  { NULL, "ENTITIES", ENTITIES },
  { NULL, "EOF", EOFILE },
  { NULL, "HEADER", HEADER },
  { NULL, "INSERT", INSERT },
  { NULL, "LAYER", LAYER },
  { NULL, "LINE", LINE },
  { NULL, "$LIMMIN", LIMMIN },
  { NULL, "$LIMMAX", LIMMAX },
  { NULL, "$CMLSCALE", CMLSCALE },
  { NULL, "LTYPE", LTYPE },
  { NULL, "OBJECTS", OBJECTS },
  { NULL, "LAYOUT", LAYOUT },
  { NULL, "ONE", N_1 },
  { NULL, "$PLINEWID", PLINEWID },
  { NULL, "POINT", POINT1 },
  { NULL, "POLYLINE", POLYLINE },
  { NULL, "LWPOLYLINE", LWPOLYLINE },
  { NULL, "SECTION", SECTION },
  { NULL, "SHAPE", SHAPE },
  { NULL, "SOLID", SOLID },
  { NULL, "STYLE", STYLE },
  { NULL, "TABLE", TABLE },
  { NULL, "TABLES", TABLES },
  { NULL, "$TRACEWID", TRACEWID },
  { NULL, "$MEASUREMENT", MEASUREMENT },
  { NULL, "TEXT", TEXT },
  { NULL, "HATCH", HATCH },
  { NULL, "SPLINE", SPLINE },
  { NULL, "TRACE", TRACE1 },
  { NULL, "VERTEX", VERTEX },
  { NULL, "VIEW", VIEW },
  { NULL, "VPORT", VPORT },
  { NULL, "MTEXT", MTEXT },
  { NULL, "VIEWPORT", VIEWPORT },
  { NULL, "DICTIONARY", DICTIONARY },
  { NULL, "XRECORD", XRECORD },
  { NULL, "SCALE", SCALE },
  { NULL, "3DFACE", _3DFACE },
  { NULL, "REGION", REGION },
  { NULL, "RAY", RAY },
  { NULL, "WIPEOUT", WIPEOUT },
  { NULL, "IMAGE", IMAGE },
  { NULL, "ACAD_PROXY_OBJECT", ACAD_PROXY_OBJECT },
  { NULL, "$DWGCODEPAGE", DWGCODEPAGE },
  { NULL, NULL, 0 }
};

static tok_tab *tok_hash[64];

static char tag_str[2560+1];


int hash_dxf(const char *s)
{
  int j;
  j = 0;

  while (*s) j = (j+ *s++) & 63;
  return j;
}


// Pg. 104
char *get_tag(FILE *dxf)
{
  int i;

  fgets(tag_str, 2560, dxf);

  if (tag_str[strlen(tag_str)-1] == '\n')
      tag_str[strlen(tag_str)-1] = '\0';
  i = strspn(tag_str, " \t");
  return &tag_str[i];
}

// Pg. 105
int get_tag_token(const char *s)
{
  tok_tab *t;

  for (t = tok_hash[hash_dxf(s)]; t; t = t->next)
  {
    if (!strcmp(t->tok_name, s)) {
//        if (t->tok_value == BLOCK) {
//            int qq;
//            qq = 1;
//        }
        return t->tok_value;
    }
  }

  return 0;
}


struct token
{
  token *next;
  char *spell;
};


static token tok[80000];
static long next_token = 0;
static token *hash_table[64];
//static char name_table[320000];
//static char *next_word = name_table;
static char *name_table = NULL;
static char *next_word = NULL;

// Pg. 106
void init_tok_table()
{
  name_table = new char[10000000];
  next_word = name_table;
  
  tok_tab *t;
  int j;

  t = tok_table;

  while (t->tok_name)
  {
    j = hash_dxf(t->tok_name);
    t->next = tok_hash[j];
    tok_hash[j] = t;
    t++;
  }
}

// Pg. 58
char *locate(const char *s) 
{
  token *p;
  int h;

  if (s == NULL || *s == '\0') return NULL;

  if (p = hash_table[hash_dxf(s)]) {
    while (p)
    {
      if (p->spell == s) return p->spell;
      if (strcmp(s, p->spell) == 0) return p->spell;
      p = p->next;
    }
  }

  h = hash_dxf(s);
  p = &tok[next_token++];
  p->next = hash_table[h];
  hash_table[h] = p;
  p->spell = next_word;
  strcpy(p->spell, s);
  next_word += strlen(s)+1;
  return p->spell;
}

/*
double get_double(FILE *f)
{
  return atof(get_tag(f));
}

int get_int(FILE *f)
{
  return atoi(get_tag(f));
}

char *get_string(FILE *f)
{
  char s[256];
  fgets(s, 255, f);
  if (s[strlen(s)-1] == '\n') s[strlen(s)-1] = '\0';

  return locate(s);
}
*/
