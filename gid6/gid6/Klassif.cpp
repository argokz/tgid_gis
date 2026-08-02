// Klassif.cpp: implementation of the Klassif class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "geo.h"
#include "Klassif.h"
#include "win.h"

#include "ado.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CString GetFName(const char *path);


Klassif::Klassif()
{
//  m_db = NULL;
  m_shp = NULL;
  nazv_alias = "";
  codlabel = "";
  codlabel_color = "";
  codlabel_date = "";
  codlabel_stateID = "";

  is_shpdbf = false;
  n_label = -1;
  n_label_color = -1;
  label_color = 0;
  label_date = 0;
  label_stateID = 0;
  typ = 72;
  n_ObjectText = -1;

  n_text_angle = n_text_size = 0;
  n_text_string = -1;
  no_mas = 0;

  m_ado = NULL;
  m_database = "";
  CFont font;

  font.CreatePointFont(100, "Arial");
  font.GetLogFont(&lf);
}

Klassif::~Klassif()
{
}

void Klassif::Copy(const Klassif& mc)
{
//  memcpy(this, &mc, sizeof(this));

  IdxOriginX = mc.IdxOriginX;
  IdxOriginY = mc.IdxOriginY;

  IdxGridSize = mc.IdxGridSize;
  x1 = mc.x1;
  x2 = mc.x2;
  y1 = mc.y1;
  y2 = mc.y2;
  coli = mc.coli;
  colb = mc.colb;
  loc = mc.loc;
  prio = mc.prio;
  typ = mc.typ;
  col3 = mc.col3;
  punct = mc.punct;
  ins = mc.ins;
  ins2 = mc.ins2;
  nazv = mc.nazv;
  nazv_alias = mc.nazv_alias;
  xy_table = mc.xy_table;
  codlabel = mc.codlabel;
  codlabel_color = mc.codlabel_color;
  codlabel_date = mc.codlabel_date;
  codlabel_stateID = mc.codlabel_stateID;
  coord_name = mc.coord_name;
  label = mc.label;
  lf = mc.lf;
  color = mc.color;
  is_edit = mc.is_edit;
  is_potr = mc.is_potr;
//  m_db = mc.m_db;
  m_shp = mc.m_shp;
  is_shpdbf = mc.is_shpdbf;
  n_label = mc.n_label;
  n_label_color = mc.n_label_color;
  label_color = mc.label_color;
  label_stateID = mc.label_stateID;
  label_date = mc.label_date;
  n_ObjectText = mc.n_ObjectText;
  no_mas = mc.no_mas;
  m_ado = mc.m_ado;
  m_database = mc.m_database;

  mas1 = mc.mas1;
  mas2 = mc.mas2;

  mas_text1 = mc.mas_text1;
  mas_text2 = mc.mas_text2;
}


Klassif::Klassif(const Klassif& mc)
{
  Copy(mc);
}


Klassif& Klassif::operator=(const Klassif& mc)
{
  Copy(mc);
  return *this;
}

bool Klassif::operator==(const Klassif& mc) const
{
  return loc == mc.loc && nazv == mc.nazv && m_ado == mc.m_ado;
}

bool Klassif::operator<(const Klassif& mc) const
{
  int ret;

  if (m_ado != mc.m_ado) return m_ado < mc.m_ado;

  ret = strcmp(nazv, mc.nazv);

  if (ret < 0) return true;

  if (ret == 0) return loc < mc.loc;
  return false;
}

CString GetTitle(CString path);

Klassif* KlList::findKlN(const char* name) 
{
    iterator p = begin();

    for (; p != end(); p++) {
        Klassif* kl = &p->second;

        CString s1, s2;
        s1 = name;
        s2 = kl->nazv;
        s1.MakeUpper();
        s2.MakeUpper();

        if (s1 == s2) {
            if (!strcmp(name, "indikator_korrozii")) {
                int qq;
                qq = 1;
            }

            if (!strcmp(name, "vozdushnik")) {
                int qq;
                qq = 1;
            }
            return kl;
        }
    }

    return NULL;
}

Klassif * KlList::getN(int n) {
  int i;
  iterator p = begin();

  for (i = 0; p != end(); p++, i++) {
    if (i == n) {
      return &p->second;
    }
  }

  return NULL;
}

void KlList::setN(int n, Klassif *kls) {
  int i;
  iterator p = begin();

  for (i = 0; p != end(); p++, i++) {
    Klassif *kl = &p->second;
    if (i == n) {
      *kl = *kls;
      return;
    }
  }
}


/*

bool KlList::save2(Klassif *kls)
{
  if (!kls) return false;
  
  if (!kls->m_db) {
    return save2(m_geofile->klass_i2);
  }
}
*/

#if 0

bool KlList::save2(const char *fn)
{
  AfxMessageBox(fn, MB_OK|MB_ICONINFORMATION);

    char s[512], ss[512]; //  , mdbn[256];
  FILE *f;

  f = fopen(fn, "w");
//  f = fopen(fn, a);
  
  if (!f) return false;


  LOGFONT lf, lf1;

  CFont font;

  font.CreatePointFont(100, "Arial");
  font.GetLogFont(&lf);

  KlList::iterator it = begin();

  for ( ; it != end(); ++it ) {
    Klassif *kl = &it->second;

    lf1 = lf;
    if (kl->lf.lfFaceName[0]) lf1 = kl->lf;

    strcpy(mdbn, "*");
//    if (kl->m_db) {
//      strcpy(mdbn, kl->m_db->db->GetName());
//    }


    strcpy(s, kl->nazv);
    strcpy(ss, kl->codlabel);

    fprintf(f,"*,%d,%d,%8ld,%8ld,%3d,%3d,%3d,\"%s\",\"\",\"%s\",%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\"%s\",%d,%d,%d\n",
      kl->loc, kl->prio, kl->coli, kl->colb, kl->typ, kl->col3, kl->punct,
//    fprintf(f,"\"%s\",%d,%d,%8ld,%8ld,%3d,%3d,%3d,\"%s\",\"\",\"%s\",%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
//      mdbn, kl->loc, kl->prio, kl->coli, kl->colb, kl->typ, kl->col3, kl->typ2,
      s, ss, 
      kl->label,

      lf1.lfFaceName,
      lf1.lfHeight,
      lf1.lfWidth,
      lf1.lfEscapement,
      lf1.lfOrientation,
      lf1.lfWeight,
      lf1.lfItalic,
      lf1.lfUnderline,
      lf1.lfStrikeOut,
      lf1.lfCharSet,
      lf1.lfOutPrecision,
      lf1.lfClipPrecision,
      lf1.lfQuality,
      lf1.lfPitchAndFamily, 
      kl->color, 
      kl->is_edit, 
      kl->ins, 
      kl->ins2, 
      kl->is_potr,
      kl->ramka,
      kl->codlabel_color,
      kl->label_color,
      kl->n_label_color,
      kl->no_mas
      );
  }

  fclose(f);

  return true;
}
#endif

bool KlList::save2()
{
  char s[512], ss[512];
  
  set<CString> m_set;

  LOGFONT lf, lf1;
  CFont font;

  font.CreatePointFont(100, "Arial");
  font.GetLogFont(&lf);

  KlList::iterator it = begin();

  for ( ; it != end(); ++it ) {
    Klassif *kl = &it->second;
    CString wa = "a";
    set<CString>::const_iterator it2 = m_set.find(kl->m_ado->m_schema);
    if (it2 == m_set.end()) {
        wa = "w";
        m_set.insert(kl->m_ado->m_schema);
    }

    CString fn = kl->getKls2();

    mkdir_for_file(fn);

    FILE *f = fopen(fn, wa);

    if (f) {
        lf1 = lf;
        if (kl->lf.lfFaceName[0]) lf1 = kl->lf;

        strcpy(s, kl->nazv);
        strcpy(ss, kl->codlabel);

//        fprintf(f, "*,%d,%d,%8ld,%8ld,%3d,%3d,%3d,\"%s\",\"\",\"%s\",%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\"%s\",%d,%d,%d,%d\n",
        fprintf(f, "*,%d,%d,%8ld,%8ld,%3d,%3d,%3d,\"%s\",\"\",\"%s\",%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\"%s\",%d,%d,%d,%d,%d,%d,%d,%d\n",
            kl->loc, kl->prio, kl->coli, kl->colb, kl->typ, kl->col3, kl->punct,

            s, ss,
            kl->label,

            lf1.lfFaceName,          //   11
            lf1.lfHeight,            //   12
            lf1.lfWidth,             //   13
            lf1.lfEscapement,        //   14
            lf1.lfOrientation,       //   15
            lf1.lfWeight,            //   16
            lf1.lfItalic,            //   17
            lf1.lfUnderline,         //   18
            lf1.lfStrikeOut,         //   19
            lf1.lfCharSet,           //   20
            lf1.lfOutPrecision,      //   21
            lf1.lfClipPrecision,     //   22
            lf1.lfQuality,           //   23
            lf1.lfPitchAndFamily,    //   24

            kl->color,               //   25
            kl->is_edit,             //   26
            kl->ins,                 //   27
            kl->ins2,                //   28
            kl->is_potr,             //   29

            kl->ramka,               //   30
            kl->codlabel_color,      //   31
            kl->label_color,         //   32
            kl->n_label_color,       //   33
            kl->n_label,             //   34
            kl->no_mas,              //   35

            kl->mas1,
            kl->mas2,
            kl->mas_text1,
            kl->mas_text2
        );
        fclose(f);
    }
  }
  return true;
}


/*
set <CString> KlList::getFileList() const
{
  set <CString> set_db;

  for (KlList::const_iterator it = begin(); it != end(); ++it ) {
    const Klassif *kl = &it->second;
    if (kl->m_db) set_db.insert(kl->m_db->m_file_name);
  }
  return set_db;
}

  */

/*
bool KlList::save()
{
//  set <CString> set_db = getFileList();

  set <CString> set_db;

  for (KlList::const_iterator it = begin(); it != end(); ++it ) {
    if (it->m_db) set_db.insert(it->m_db->db->GetName());
  }


  set <CString>::const_iterator it2 = set_db.begin();

  for (; it2 != set_db.end(); ++it2 ) {
//    save(*it2);
    save(GetFName(*it2)+".ki2");
  }
  return true;
}
*/

char *readstr(char *s, char *ss) {
  int i;

  if (!s) return s;

  ss[0] = 0;
  if (!s || !s[0]) return NULL;

  if (s[0] == '\"') {
    for (i = 1; s[i]; i++) {
      if (s[i] == '"' || s[i] == '\n') {
        strncpy(ss,  &s[1], i);
        ss[i-1] = 0;
        return &s[i+2];
      }
    }
  }
  else {
    for (i = 0; s[i]; i++) {
      if (s[i] == ',' || s[i] == '\n') {
        break;
      }
    }
  }
  strncpy(ss,  s, i);
  ss[i] = 0;
  return &s[i+1];
}

char *readint(char *s, int &d) {

  char ss[256];
  char *sss = readstr(s, ss);
  if (!sss) return NULL;


  d = atoi(ss);
  return sss;
}

char *readint(char *s, unsigned char &d) {

  char ss[256];
  char *sss = readstr(s, ss);
  if (!sss) return NULL;


  d = atoi(ss);
  return sss;
}

char *readint(char *s, unsigned long &d) {

  char ss[256];
  char *sss = readstr(s, ss);
  if (!sss) return NULL;

  d = atoi(ss);
  return sss;
}

char *readint(char *s, long &d) {

  char ss[256];
  char *sss = readstr(s, ss);
  if (!sss) return NULL;

  d = atoi(ss);
  return sss;
}


Prim *rs = NULL;

void CloseRas() 
{
  if (rs) delete [] rs;
}


void OpenRas() {
  FILE *f;
  int nrs;

  CloseRas();

  rs = new Prim[256];
  nrs = 0;

  f = fopenexe("kls/flibrt.ras", "rb");
  if (f) {
    while (!feof(f) && nrs < 256) {
      rs[nrs].x = getc(f);
      rs[nrs].y = getc(f);
      fread(rs[nrs].s,32,1,f);
      nrs++;
    }
    fclose(f);
  }
}

CString Klassif::RusName() const
{
  CString name = nazv;
  if (m_ado) {
    MySQLTables *tab = m_ado->findMySQLTable(nazv);
    if (tab && tab->strTABLE_COMMENT != "") {
      name = tab->strTABLE_COMMENT;
    }
//    name = m_ado->getTableName(nazv);
  }
  return name;
}



CString Klassif::TableName() const
{
//  if (m_db) return m_db->db->GetName();
//  return CString(nazv);

  return CString(nazv);

//  if (nazv == nazv_alias) {
//    return CString(nazv);
//  }
//  return CString(nazv)+" "+CString(nazv_alias);
}


CString Klassif::FileName() const
{
//  if (m_db) return m_db->db->GetName();
  return CString(nazv);
}


int Klassif::GetNom2(int n) 
{
  int nnn = geo4.size();
  for (int i = 0; i < nnn; i++) {
    int nn = geo4[i]->nom;

    if (nn == n) {
      return i+1;
    }
  }
  return 0;
}

CGeoObject *Klassif::getGeoObjectById(int id) 
{
  int nnn = geo4.size();
  for (int i = 0; i < nnn; i++) {
    int nn = geo4[i]->nom;

    if (nn == id) {
      return geo4[i];
    }
  }
  return NULL;
}



bool isPoint(int loc)
{
  return loc == LOC1 || loc == LOC6 || loc == LOC11;
//   || loc == LOC12 || loc == LOC13 || loc == LOC14;
}


bool isLine(int loc)
{
  return loc == LOC2 || loc == LOC12;
}



CString Klassif::getKls2() const
{
  CString fn;

//  fn.Format("%skls\\%s.kls2", argpath(), m_ado->m_schema);
  fn.Format("%s%s.kls2", argpath_2(), m_ado->m_schema);

  return fn;
}


void KlList::save_ins()
{
    for (auto& p : *this) {
        Klassif* kl = &p.second;
        kl->ins_save = kl->ins;
    }
}

void KlList::restore_ins()
{
    for (auto& p : *this) {
        Klassif* kl = &p.second;
        kl->ins = kl->ins_save;
    }
}

void KlList::zero_ins()
{
    for (auto& p : *this) {
        Klassif* kl = &p.second;
        kl->ins = 0;
    }
}


