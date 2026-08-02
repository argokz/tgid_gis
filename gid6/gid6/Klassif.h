#pragma once

#pragma warning(disable:4786)
//#include <string>
//#include <set>
#include <vector>
#include <map>
using namespace std;

#include "shp_dbf.h"
#include "coordlis.h"

#define LOC1 1
#define LOC2 2
#define LOC3 3
#define LOC6 6
#define LOC11 11
#define LOC12 12
#define LOC13 13
#define LOC14 14

bool isPoint(int loc);
bool isLine(int loc);

bool is_special_kls(CString nazv);

struct Prim {
  char x,y;
  char s[32];
};

//extern Prim *rs;
class CNode;
class CLine;


class CAdoFile;

class CGeoObject
{
public:
  CGeoObject()
  {
//    is_node = false;
//    is_line = false;
//    m_node = NULL;
//    m_line = NULL;
    text00000 = "";
    text_color = "";
    p_text.x = 0;
    p_text.y = 0;
    alpha = 0;
    align = 0;
    coef = 0;
    is_read = false;
    id_remont = -1;
    n_label = -1;
    n_label_color = -1;
    n_label_color2 = -1;
    n_label_color3 = -1;
    n_ObjectText = -1;
    is_visible = true;
    mark = false;
  };

  CGeoObject(int id, const CCoordList &cl0)
  {
    text00000 = "";
    text_color = "";
    p_text.x = 0;
    p_text.y = 0;
    alpha = 0;
    align = 0;
    coef = 0;
    id_remont = -1;
    n_label = -1;
    n_label_color = -1;
    n_label_color2 = -1;
    n_label_color3 = -1;
    n_ObjectText = -1;
    mark = false;

    nom = id;
    nom2 = id;
    rect = cl0.GetFRect();
    cl = cl0;
    is_read = true;
    is_visible = true;
  };


  CFRect rect;
  double alpha;
  double coef;
  CFPoint p_text;
  CString text00000;
  CString text_color;
  CCoordList cl;
  int nom;
  int nom2;
  short typ;
  short align;
  bool mark;
  bool is_visible;
//  bool is_node;
//  bool is_line;
  bool is_read;
  int id_remont;
  int n_label;
  int n_label_color;
  int n_label_color2;
  int n_label_color3;
  int n_ObjectText;

  
//  CNode *m_node;
//  CLine *m_line;
};

class CGeoList : public vector<CGeoObject *>
{
};

class Klassif
{
public:
  Klassif();

  virtual ~Klassif();
  Klassif(const Klassif& mc);

  Klassif& operator=(const Klassif& mc);
  bool operator==(const Klassif& mc) const;
  bool operator<(const Klassif& mc) const;

  LOGFONT lf;
  unsigned long color;

  unsigned long coli, colb;
  unsigned char loc;
  unsigned char prio;
  unsigned char typ;
  unsigned long col3, punct;
  unsigned char ins;
  unsigned char ins2;
  unsigned char is_edit;
  unsigned char is_potr;
  unsigned char ramka;
  unsigned char no_mas;
  CString nazv;
  CString nazv_alias;
  CString xy_table;
  unsigned char label;
  unsigned char label_color;
  unsigned char label_color2;
  unsigned char label_color3;
  unsigned char label_date = 0;
  unsigned char label_stateID;
  CString codlabel;
  CString coord_name;
  CString codlabel_color;
  CString codlabel_color2;
  CString codlabel_color3;
  CString codlabel_date;
  CString codlabel_stateID;
  int n_label;
  int n_label_color;
  int n_ObjectText;
  int n_text_angle, n_text_size, n_text_string;

  int cnt;
  unsigned char ins_save;

  int mas1 = 0, mas2 = 100000000;
  int mas_text1 = 0, mas_text2 = 100000000;

  double x1, x2, y1, y2;
  double IdxGridSize, IdxOriginX, IdxOriginY;
  map <CString, CString> field_alias;

  int NObj() const 
    { return geo4.size();};


  CGeoList geo4;
  CAdoFile *m_ado;

  CString m_database;
  
  Cshp_dbf *m_shp;
  CView *view;
  bool is_shpdbf;

  CString TableName() const;
  CString RusName() const;
  CString FileName() const;
  int GetNom2(int n);
  CString getKls2() const;
  CGeoObject *getGeoObjectById(int id);


private:
  void Copy(const Klassif& mc);

};

class KlList : public map<CString, Klassif>
{
public:
  bool open0(const char *fn, int loc);
  bool open2(const char *kln);
  bool save2();
  Klassif * findKlN(const char * name);
  Klassif * getN(int n);
  void setN(int n, Klassif *kls);

  int m_nMaxPrio;
  set <CString> getFileList() const;

  void save_ins();
  void restore_ins();
  void zero_ins();


private:
  CString m_name;
  CString m_kls_fn;
};
