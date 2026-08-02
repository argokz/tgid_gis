#include "stdafx.h"


#include "dfm.h"

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

HRESULT __fastcall UnicodeToAnsi(LPCOLESTR pszW, LPSTR* ppszA);


STR str1[] = {
  {"TDbTable", ID_TDbTable},
  
  {"TCheckBox", ID_TCheckBox},
  {"TEdit", ID_TEdit},
  {"TButton", ID_TButton},
  {"TPanel", ID_TPanel},

  {"TEditFile",     ID_TEditFile},    
  {"TEditFilePic",     ID_TEditFilePic},    
  {"TEditFileDoc",     ID_TEditFileDoc},    
  {"TEditDir",      ID_TEditDir},     
  {"TEditPicture",  ID_TEditPicture}, 

  {"TComboEdit", ID_TComboEdit},
  {"TStringGrid", ID_TStringGrid},
  {"TForm1", ID_TForm1},
  {"TGroupBox", ID_TGroupBox},
  {"TLabel", ID_TLabel}
};


STR str[] = {

{"Caption", ID_Caption},
{"Color", ID_Color},
{"Cursor", ID_Cursor},
{"Font.Charset", ID_Font_Charset},
{"Font.Color", ID_Font_Color},
{"Font.Height", ID_Font_Height},
{"Font.Name", ID_Font_Name},
{"Font.Style", ID_Font_Style},
{"Height", ID_Height},
{"Left", ID_Left},
{"OldCreateOrder", ID_OldCreateOrder},
{"ParentColor", ID_ParentColor},
{"ParentFont", ID_ParentFont},
{"PixelsPerInch", ID_PixelsPerInch},
{"TabOrder", ID_TabOrder},
{"Text", ID_Text},
{"TextHeight", ID_TextHeight},
{"Top", ID_Top},
{"Width", ID_Width}
};



STR str_html[] = {
{"align", ID_align},
{"body", ID_body},
{"border", ID_border},
{"charset", ID_charset},
{"col", ID_col},
{"colspan", ID_colspan},
{"content", ID_content},
{"head", ID_head},
{"height", ID_height},
{"html", ID_html},
{"http-equiv", ID_http_equiv},
{"meta", ID_meta},
{"rowspan", ID_rowspan},
{"span", ID_span},
{"table", ID_table},
{"td", ID_td},
{"tr", ID_tr},
{"width", ID_width},
};


STR str2[] = {
  {"DbQuery", ID2_DbQuery},
  {"DbTable", ID2_DbTable},
  {"EditCall", ID2_EditCall},
  {"Call", ID2_Call},
  {"Table", ID2_Table},
  {"If",    ID2_If},
  {"EndIf",   ID2_EndIf},
  {"Loop",    ID2_Loop},
  {"LoopTable",    ID2_LoopTable},
  {"EndLoop", ID2_EndLoop},
  {"NextRow", ID2_NextRow},
  {"FirstRow", ID2_FirstRow},
  {"File", ID2_File}, 
  {"FileM", ID2_FileM}, 
  {"SetFile", ID2_SetFile}, 
  {"Dir",  ID2_Dir},  
  {"DirM",  ID2_DirM},  
  {"Picture",  ID2_Picture},  
  {"Font", ID2_Font},
  {"Edit", ID2_Edit},
  {"ReadOnly", ID2_ReadOnly},
  {"ComboEdit", ID2_ComboEdit},
  {"ComboEdit2", ID2_ComboEdit2},
  {"Label", ID2_Label},
  {"Button", ID2_Button},
  {"Set", ID2_Set},
  {"Cols", ID2_Cols},
  {"Rows", ID2_Rows},
  {"Width", ID2_Width},
  {"Height", ID2_Height},
  {"BkColor", ID2_BkColor},
  {"Bold",  ID2_Bold},
  {"Italic",ID2_Italic},
  {"Normal",ID2_Normal},
  {"Enter", ID2_Enter},

   {"Call_Table", ID2_Call_Table},
   {"Call_Table_MDB", ID2_Call_Table_MDB},
   {"Call_Table_Table", ID2_Call_Table_Table},
   {"Call_Table_ID", ID2_Call_Table_ID},
   {"Call_Table_Name", ID2_Call_Table_Name},

  {"Buffer", ID2_Buffer},
  {"Pattern",  ID2_Pattern},
  {"CopyDoc",  ID2_CopyDoc},

  {"ItIsRow", ID2_ItIsRow},


  {"Title", ID2_Title},
  {"FileExt",    ID2_FileExt},            
  {"FileFilter", ID2_FileFilter},         
  {"FilePath",   ID2_FilePath},
  {"FilePath1",   ID2_FilePath1},
  {"FilePath2",   ID2_FilePath2},
  {"FilePath3",   ID2_FilePath3},
  {"FilePath4",   ID2_FilePath4},
  {"FilePath5",   ID2_FilePath5},
  {"FilePath6",   ID2_FilePath6}
};




int get_id(int size, STR str[], const char *fn)
{
//  for (int i = 0; i < sizeof(str)/sizeof(str[0]); i++) {
  for (int i = 0; i < size; i++) {
    if (!strcmp(str[i].str, fn)) {
      return str[i].cod;
    }
  }
  return -1;
}

int get_id_html( const char *fn)
{
  return get_id(sizeof(str_html)/sizeof(str_html[0]), str_html, fn);
}

int get_id(const char *fn)
{
  return get_id(sizeof(str)/sizeof(str[0]), str, fn);
}

int get_id1(const char *fn)
{
  return get_id(sizeof(str1)/sizeof(str1[0]), str1, fn);
}

int get_id_txt2(const char *fn)
{
  return get_id(sizeof(str2)/sizeof(str2[0]), str2, fn);
}



/*

int get_id(const char *fn)
{
  for (int i = 0; i < sizeof(str)/sizeof(str[0]); i++) {
    if (!strcmp(str[i].str, fn)) {
      return str[i].cod;
    }
  }
  return -1;
}

int get_id1(const char *fn)
{
  for (int i =0; i < sizeof(str1)/sizeof(str1[0]); i++) {
    if (!strcmp(str1[i].str, fn)) {
      return str1[i].cod;
    }
  }
  return -1;
}
*/

int fgetstr(char *s, int n, FILE *f);


static OBJ obj[256];
static int level;
static int qq = 13427;



void print_label(OBJ &obj)
{
  printf("    LTEXT ");
  printf("\"%s\", ", obj.Caption);

  printf("IDC_STATIC,");
  

  printf("%d,%d,%d,%d", obj.Left, obj.Top, obj.Width, obj.Height);

  printf("\n");
}



void print_edit(OBJ &obj)
{
  printf("    EDITTEXT ");
//  printf("IDC_STATIC,");
  printf("%d,", qq++);
  printf("%d,%d,%d,%d,ES_AUTOHSCROLL", obj.Left, obj.Top, obj.Width, obj.Height);
//  printf("\"%s\", ", obj.Caption);
//  printf("%s ", obj.Caption);

  printf("\n");
}


void print_check(OBJ &obj)
{
  printf("    CONTROL ");
  printf("\"%s\", ", obj.Caption);
//  printf("IDC_STATIC,");
  printf("%d,", qq++);
  printf("\"Button\",BS_AUTOCHECKBOX|WS_TABSTOP,");
  printf("%d,%d,%d,%d", obj.Left, obj.Top, obj.Width, obj.Height);
//  printf("\"%s\", ", obj.Caption);
//  printf("%s ", obj.Caption);

  printf("\n");
}

void print_groupbox(OBJ &obj)
{
  printf("    GROUPBOX ");
  printf("\"%s\", ", obj.Caption);

  printf("IDC_STATIC,");
  

  printf("%d,%d,%d,%d", obj.Left, obj.Top, obj.Width, obj.Height);

  printf(",WS_GROUP\n");
}

void print_obj(OBJ &obj)
{
  switch(obj.ID) {
  case ID_TLabel:
    print_label(obj);
    break;
  case ID_TEdit:
    print_edit(obj);
    break;
  case ID_TCheckBox:
    print_check(obj);
    break;
  case ID_TGroupBox:
    print_groupbox(obj);
    break;
  }
}

void decode(const char *s1, char *s2)
{
  int k = 0;
  char ss[1024];

  for (int i = 0; s1[i]; i++) {
    if (s1[i] == '#') {
      strncpy(ss, &s1[i+1], 4);
      ss[4] = 0;
      i+= 4;


      unsigned short c1[2];
      c1[0] = atoi(ss);
      c1[1] = 0;

      char *buf;

      UnicodeToAnsi(c1, &buf);
      s2[k++] = buf[0];
      s2[k] = 0;

      
      CoTaskMemFree(buf);
    }
    else {
      if (s1[i] != '\'') {
        s2[k++] = s1[i];
        s2[k] = 0;
      }
    }

  }
}

int ispr(int a)
{
  return a*1.2;
//  return a*0.7+0.5;
}


void read_str(char *s, list<OBJ> &l_ob)
{
  int j = 0;
  int r = 0;
  char ss[1024];

  if (s[0] != ' ') r = 1;

  int id = -1;

  for (int i = 0; s[i]; i++) {

    if (s[i] != ' '  || r == 5) {
      ss[j++] = s[i];
      ss[j] = 0;
      if (r == 0) r = 1;
    }
    else {
      j = 0;
      switch(r) {
        case 1 :
          if (!strcmp(ss, "object")) {
            r = 2;
            level++;
            obj[level-1].Caption = "";
          }
          else {
            r = 3;
            id = get_id(ss);
//            printf("%s %d\n", ss, id);
          }
          break;
        case 2 :  // object
//          printf("%s\n", ss);
          if (strlen(ss) > 1 && ss[strlen(ss)-1] == ':') ss[strlen(ss)-1]  = 0;

          obj[level-1].name = ss;
          r = 4;
          break;
        case 3 :  // =
          r = 5;
          break;
      }
    }
  }

  if (r == 1) { // end
//    printf("<< %s\n", ss);

    if (level > 2) {
//      obj[level-1].Left += obj[level-2].Left;

      int l;

      for (l = level-2; l > 0; l--) {
        obj[level-1].Left += obj[l].Left;
      }




//      obj[level-1].Top += obj[level-2].Top;

        for (l = level-2; l > 0; l--) {
          obj[level-1].Top += obj[l].Top;
        }


    }

    obj[level-1].Top = ispr(obj[level-1].Top);
    obj[level-1].Height = ispr(obj[level-1].Height);
    obj[level-1].Left = ispr(obj[level-1].Left);
    obj[level-1].Width = ispr(obj[level-1].Width);

    l_ob.push_back(obj[level-1]);

    print_obj(obj[level-1]);
    level --;

  }
  if (r == 4) {
    obj[level-1].ID = get_id1(ss);

//    printf("<< %s\n", ss);
  }

  char s2[1024];

  if (r == 5) {  //id

    switch (id) {
    case ID_Caption :

      decode(ss, s2);

      obj[level-1].Caption = s2;

//      strcpy(obj[level-1].Caption, ss);
      break;
    case ID_Color :
      obj[level-1].Color = atoi(ss);
      break;
    case ID_Cursor :
      obj[level-1].Cursor = atoi(ss);
      break;
    case ID_Font_Charset :
      obj[level-1].Font_Charset = atoi(ss);
      break;
    case ID_Font_Color :
      obj[level-1].Font_Color = atoi(ss);
      break;
    case ID_Font_Height :
      obj[level-1].Font_Height = atoi(ss);
      break;
    case ID_Font_Name :
      obj[level-1].Font_Name = atoi(ss);
      break;
    case ID_Font_Style :
      obj[level-1].Font_Style = atoi(ss);
      break;
    case ID_Height :
      obj[level-1].Height = atoi(ss);
      break;
    case ID_Left :
      obj[level-1].Left = atoi(ss);
      break;
    case ID_OldCreateOrder :
      obj[level-1].OldCreateOrder = atoi(ss);
      break;
    case ID_ParentColor :
      obj[level-1].ParentColor = atoi(ss);
      break;
    case ID_ParentFont :
      obj[level-1].ParentFont = atoi(ss);
      break;
    case ID_PixelsPerInch :
      obj[level-1].PixelsPerInch = atoi(ss);
      break;
    case ID_TabOrder :
      obj[level-1].TabOrder = atoi(ss);
      break;
    case ID_Text :
      obj[level-1].Text = atoi(ss);
      break;
    case ID_TextHeight :
      obj[level-1].TextHeight = atoi(ss);
      break;
    case ID_Top :
      obj[level-1].Top = atoi(ss);
      break;
    case ID_Width :
      obj[level-1].Width = atoi(ss);
      break;
    }


//    printf("%s\n", ss);
  }
}


void read_dfm(FILE *f, list<OBJ> &l_ob)
{
  level = 0;
  qq = 13427;

  char s[1024];

  while (!feof(f)) {
    if (fgetstr(s,1023,f) == NULL) break;

    read_str(s, l_ob); 
  }
}

void read_par(const char *s, char *p1, char *p2)
{
  p1[0] = 0;
  p2[0] = 0;

  strcpy(p1, s);

  for (int i =0; s[i]; i++) {
    if (s[i] == '=') {
      strncpy(p1, s, i); 
      p1[i] = 0;

      strcpy(p2, &s[i+1]); 
      break;
    }
  }
}


struct TAG
{
  int tag;
  map<int, CString> par;
  CString text;
};

CString read_string(TAG tag, int p1)
{
  map<int, CString>::const_iterator it = tag.par.find(p1);

  if (it != tag.par.end()) {
    return it->second;
  }
  return "";
}


int read_int(TAG tag, int p1)
{
  CString s =  read_string(tag, p1);

  if (s != "") {
    return atoi(s);
  }
  return 0;
}



static CString text;
static TAG tag;
static is_tag;
static list<TAG> lst_tag;
static int is_td = 0;

void save_tag()
{
  if (is_tag) {
    tag.text = text;
    text = "";
    lst_tag.push_back(tag);
  }
}


void new_tag(int tg)
{
  save_tag();

  text = "";
  is_tag = 1;
  tag.tag = tg;

  if (tg == ID_td) {
    is_td = 1;
  }
}

void close_tag(int tg)
{
  save_tag();

  if (tg == ID_td) {
    is_td = 0;
  }
  is_tag = 0;

}

void add_tag(int tg, int p1, const char *p2)
{
  tag.tag = tg;
  tag.par[p1] = p2;
}

void read_tag(const char *s)
{
  char p1[1024]; 
  char p2[1024];
  char tag[256];
  char par[1024];
  int i = 0, j, k;

  int z = 0;

  if (s[0] == '/') {
    z = 1;
    i++;
  }

  int tg;

  for (i, k = 0; s[i]; k++) {
    for (; s[i] == ' '; i++);
    int i0 = i;
    for (j = 0; s[i] && s[i] != ' '; i++, j++) {
    }

    if (k == 0) {
      strncpy(tag, &s[i0], j); 
      tag[j] = 0;

      tg = get_id_html(tag);

      if (!z) {
        new_tag(tg);
      }
      else {
        close_tag(tg);
      }

    }
    else {
      if (j > 1023) j = 1023;
      strncpy(par, &s[i0], j); 
      par[j] = 0;
      read_par(par, p1, p2);
      int tp = get_id_html(p1);

      add_tag(tg, tp, p2);

      //      int t = get_id_html(tag)
    }
  }
}

bool is_numb(const char *s) 
{

  for (int i = 0; s[i]; i++) {
    if ('0' > s[i] || s[i] > '9') return false;
  }

  return true;
}

void read_tag(FILE *f)
{
  char s[1024];

  int regim = 0;
  int zak = 0;
  int j = 0;

  for (int i = 0; i < 1024; i++) {
    int c = getc(f);

    if (c == '\n' || c == '\r' || c == '\t') c = ' ';

    if (c == '>') break;

    if (regim == 0 && c == '/') {
      zak = 1;
    }
    if (regim == 0) regim = 1;

    s[j++] = c;
  }
  s[j] = 0;

  read_tag(s);
}

void read_html2(list<OBJ> &l_ob)
{

  list<TAG>::const_iterator it = lst_tag.begin();

  int DD = 2;

  int x = 0;
  int y = 0;

  int max_h = 0;

  int height_tr = 0;

  for (; it != lst_tag.end(); it++) {
    TAG tag = *it;

    if (tag.tag == ID_tr) {
      x = 0;
      y += max_h;
      if (y > 0) y += DD;
      max_h = 0;

      height_tr = read_int(tag, ID_height);

      if (height_tr == 0) {
        height_tr = 10;
      }
    }

    if (tag.tag == ID_td) {

      int colspan = read_int(tag, ID_colspan);
      int rowspan = read_int(tag, ID_rowspan);
      int height = read_int(tag, ID_height);

      if (rowspan == 0) rowspan = 1;

      if (height == 0) {
        height = height_tr*rowspan;
      }

      int width = read_int(tag, ID_width);

      OBJ obj;

      obj.ID = ID_TLabel;
//      obj.ID = ID_TEdit;


      obj.name = tag.text;
      obj.Caption = tag.text;



      if (tag.text == "&nbsp;" || is_numb(tag.text)) {
        obj.ID = ID_TEdit;
        obj.Caption = tag.text;
      }

      obj.Left = x;
      obj.Width = width;
      obj.Height = height;
      obj.Top = y;

      x += width;
      x += DD;

      l_ob.push_back(obj);

      if (height > max_h) {
        max_h = height;
      }
    }
  }
}


void read_html(FILE *f, list<OBJ> &l_ob)
{
  level = 0;
  qq = 13427;

  lst_tag.clear();

  is_td = 0;
  is_tag = 0;
  int regim = 0;
  text = "";

  while (!feof(f)) {
//    if (fgetstr(s,1023,f) == NULL) break;

    int c = getc(f);

    if (c == '<') {
      read_tag(f);
    }
    else {
      text += c;
    }
  }
  save_tag();

  read_html2(l_ob);

}




/*

void main()
{
  
  FILE *f;
  f = fopen("Unit1222.txt", "r");

  if (f) {

    convert(f);
    fclose(f);
  }
}
*/