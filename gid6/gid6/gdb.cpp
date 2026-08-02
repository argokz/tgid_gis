#include "stdafx.h"
#include "gid6.h"
#include "win.h"
#include "crack.h"
#include "GeoFile.h"

char *readstr(char *s, char *ss);
char *readint(char *s, int &d);
char *readint(char *s, unsigned char &d);
char *readint(char *s, unsigned long &d);
char *readint(char *s, long &d);

bool is_special_kls(CString nazv)
{
    if (nazv == "remont" || nazv == "defect") return true;
    if (nazv == "shurfy" || nazv == "indikator_korrozii") return true;
    return false;
}


bool KlList::open2(const char *kln) {
  FILE *f;
  char *s, ss[256], str[1024];

  if (!kln[0]) return false;

  m_name = kln;

  f = fopen(kln,"r");

  if (!f) return false;

  m_nMaxPrio = 0;

  while (!feof(f)) {
    s = str;
    if (fgets(s,1023,f) == NULL) break;

    Klassif kl;

    s = readstr(s, ss); if (ss[0] == 0) break;

    s = readint(s, kl.loc);
    s = readint(s, kl.prio);

    m_nMaxPrio = max(m_nMaxPrio, kl.prio);

    s = readint(s, kl.coli);
    s = readint(s, kl.colb);
    s = readint(s, kl.typ);
    s = readint(s, kl.col3);
    s = readint(s, kl.punct);
    s = readstr(s, ss);
    kl.nazv = ss;
    kl.nazv_alias = ss;
    s = readstr(s, ss);
    s = readstr(s, ss);
    kl.codlabel = ss;
    s = readint(s, kl.label);

    kl.lf.lfFaceName[0] = 0;

    if (s[0]) {
      s = readstr(s, kl.lf.lfFaceName);
      s = readint(s, kl.lf.lfHeight);
      s = readint(s, kl.lf.lfWidth);
      s = readint(s, kl.lf.lfEscapement);
      s = readint(s, kl.lf.lfOrientation);
      s = readint(s, kl.lf.lfWeight);
      s = readint(s, kl.lf.lfItalic);
      s = readint(s, kl.lf.lfUnderline);
      s = readint(s, kl.lf.lfStrikeOut);
      s = readint(s, kl.lf.lfCharSet);
      s = readint(s, kl.lf.lfOutPrecision);
      s = readint(s, kl.lf.lfClipPrecision);
      s = readint(s, kl.lf.lfQuality);
      s = readint(s, kl.lf.lfPitchAndFamily);
      s = readint(s, kl.color);
    }
    kl.is_edit = 1;
    if (s[0]) {
      s = readint(s, kl.is_edit);
    }

    s = readint(s, kl.ins);
    s = readint(s, kl.ins2);
    s = readint(s, kl.is_potr);
    s = readint(s, kl.ramka);

    kl.codlabel_color = "";
    if (s[0]) s = readstr(s, ss);
    kl.codlabel_color = ss;


    kl.label_color = 0;
    if (s[0]) s = readint(s, kl.label_color);

    kl.n_label_color = 0;
    if (s[0]) s = readint(s, kl.n_label_color);

    kl.n_label = 0;
    if (s[0]) s = readint(s, kl.n_label);

    kl.no_mas = 0;
    if (s[0]) s = readint(s, kl.no_mas);

    if (s[0]) s = readint(s, kl.mas1);
    if (s[0]) s = readint(s, kl.mas2);
    if (s[0]) s = readint(s, kl.mas_text1);
    if (s[0]) s = readint(s, kl.mas_text2);


    if (kl.typ == 12 && kl.loc == LOC3) kl.typ = 0;

    if (is_special_kls(kl.nazv)) {
        kl.no_mas = 1;
    }

    if (kl.nazv == "remont" || kl.nazv == "defect") {
        kl.label_date = 1;
        kl.codlabel_date = "data_osmotra";
    }

    kl.label_color2 = 0;
    
    if (kl.nazv == "shurfy") {
        kl.label_color = 1;
        kl.label_color2 = 1;
        kl.codlabel_color = "sostoyanie_shurfaID";
        kl.codlabel_color2 = "naznachenie_vskrID";
    }

    if (kl.nazv == "gps") {
        kl.codlabel = "h";
        kl.label = 1;
    }
    
    if (kl.nazv == "zdaniya_2") {
        kl.label_color3 = 1;
        kl.codlabel_color3 = "otop";
    }
    
    if (kl.nazv == "defect") {
        kl.label_color = 1;
        kl.label_color2 = 1;
        kl.codlabel_color = "stateID";
        kl.codlabel_color2 = "remontTypeID";
    }

    kl.label_stateID = 0;

    if (kl.nazv == "zapornaya_armatura") {
        kl.label_color = 1;
        kl.codlabel_color = "stateID";
        kl.label_stateID = 1;
        kl.codlabel_stateID = "stateID";
    }

    if (kl.nazv == "indikator_korrozii") {
        kl.label_color = 1;
        kl.codlabel_color = "sostoyanie";
    }

    CString str = kl.nazv;

    Klassif *kls = findKlN(str);

    if (kls) {
      kls->color = kl.color;
      kls->col3 = kl.col3;
      kls->colb = kl.colb;

      kls->coli = kl.coli;
      kls->prio = kl.prio;
      kls->typ = kl.typ;
      kls->punct = kl.punct;
      kls->codlabel = kl.codlabel;
      kls->label = kl.label;
      kls->lf = kl.lf;
      kls->ins = kl.ins;
      kls->ins2 = kl.ins2;
      kls->is_potr = kl.is_potr;
      kls->ramka = kl.ramka;
      kls->no_mas = kl.no_mas;
     

      kls->label_color =      kl.label_color;
      kls->label_color2 =      kl.label_color2;
      kls->label_color3 = kl.label_color3;
      kls->codlabel_color =   kl.codlabel_color;
      kls->codlabel_color2 =   kl.codlabel_color2;
      kls->codlabel_color3 = kl.codlabel_color3;

      kls->label_date =      kl.label_date;
      kls->codlabel_date =   kl.codlabel_date;
      kls->codlabel_stateID =   kl.codlabel_stateID;
      kls->label_stateID =   kl.label_stateID;
      kls->n_label_color =    kl.n_label_color;
     
      kls->n_label =    kl.n_label;

      kls->mas1 = kl.mas1;
      kls->mas2 = kl.mas2;
      kls->mas_text1 = kl.mas_text1;
      kls->mas_text2 = kl.mas_text2;


//      if (kl.loc == LOC1 && kls->loc == LOC6) 
        
        kls->loc = kl.loc;
    }
  }
  fclose(f);

  return true;
}
