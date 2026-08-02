#pragma once

//#include "db.h"
//#include "shp_dbf.h"
#include <vector>
#include <cxema/coordlis.h>



#define LOC1 1
#define LOC2 2
#define LOC3 3
#define LOC6 6
#define LOC7 7
#define LOC11 11
#define LOC12 12
#define LOC13 13
#define LOC14 14


struct MyLOGFONT
{
    int      lfHeight = 10;
    int      lfWidth;
    int      lfEscapement;
    int      lfOrientation;
    int      lfWeight;
    unsigned char      lfItalic = 0;
    unsigned char      lfUnderline = 0;
    unsigned char      lfStrikeOut = 0;
    unsigned char      lfCharSet;
    unsigned char      lfOutPrecision;
    unsigned char      lfClipPrecision;
    unsigned char      lfQuality;
    unsigned char      lfPitchAndFamily;
    QString   lfFaceName;
};


class CDbFile;

struct Prim {
  char x,y;
  char s[32];
};

extern Prim *rs;
class CNode;
class CLine;

class CGeoObject
{
public:
  CGeoObject()
  {
//    is_node = false;
//    is_line = false;
//    m_node = NULL;
//    m_line = NULL;
    p_text.x = 0;
    p_text.y = 0;
    is_read = false;
  };

  CGeoObject(int id, const CCoordList &cl0)
  {
    p_text.x = 0;
    p_text.y = 0;

    nom = id;
    nom2 = id;
    rect = cl0.GetFRect();
    cl = cl0;
    is_read = true;
  };


  CFRect rect;
  double alpha = 0;
  double coef = 0;
  CFPoint p_text;
  QString text00000 = "";
  QString text_color = "";
  CCoordList cl;
  int nom;
  int nom2;
  short typ;
  short align = 0;
  bool mark = false;
  bool is_visible = true;
//  bool is_node;
//  bool is_line;
  bool is_read;
  int id_remont = -1;
  int n_label = -1;
  int n_label_color = -1;
  int n_label_color2 = -1;
  int n_label_color3 = -1;
  int n_ObjectText = -1;

  
//  CNode *m_node;
//  CLine *m_line;
};


class CGeoList : public std::vector<CGeoObject *>
{
};



class Klassif
{
public:
  Klassif();

  virtual ~Klassif();
//  Klassif(const Klassif& mc);

//  Klassif& operator=(const Klassif& mc);
  bool operator==(const Klassif& mc) const;
  bool operator<(const Klassif& mc) const;

  MyLOGFONT lf;
  unsigned long color;
  unsigned long coli, colb;

  bool loc_from_baza = false;
  unsigned char loc;
  unsigned char prio;
  unsigned char typ;
  unsigned long col3, punct;
  unsigned char ins = 0;
  unsigned char ins2;
  unsigned char is_edit;
  unsigned char is_potr;
  unsigned char ramka = 0;
  unsigned char no_mas = 0;
  unsigned char open = 0;
  QString nazv = "???";
  QString nazv_alias;
  QString xy_table;
  unsigned char label = 0;
  unsigned char label_color = 0;
  unsigned char label_color2 = 0;
  unsigned char label_color3 = 0;
  unsigned char label_date = 0;
  unsigned char label_stateID = 0;
  unsigned char label_angle = 0;
  unsigned char label_fileID = 0;
  QString codlabel = "";
  QString coord_name = "";
  QString codlabel_color = "";
  QString codlabel_color2 = "";
  QString codlabel_color3 = "";
  QString codlabel_date = "";
  QString codlabel_stateID = "";
  QString codlabel_angle = "";
  QString codlabel_fileID = "";
  int n_label = 0;
  int n_label_color = 0;
  int n_ObjectText = 0;
  int n_text_angle = 0;
  int n_text_size = 0;
  int n_text_string = 0;

  QString id = "id";
  QString shape = "shape";
  QString srid = 0;

  int cnt;

  

  double x1, x2, y1, y2;
  double IdxGridSize, IdxOriginX, IdxOriginY;
  std::map <QString, QString> field_alias;

  QString TableName() const;
  QString FileName() const;

  QString baza = "";
  CGeoList geo4;

  QSqlDatabase *m_db;

  CGeoObject *getGeoObjectById(int id);
  int GetNom2(int n);
  QString RusName() const;

private:
//  void Copy(const Klassif& mc);

};

class KlList : public QMap<QString, Klassif>
{
public:
  bool open(const QString & kln);
  bool open0(const QString & tn);
  bool open0(const QString & tn, const QString & db);
  bool open0(const QString & fn, int loc);
  bool open2(const QString & kln);
  bool GDB(const QString & name);
  bool save(const QString & fn);
  bool save2(const QString & fn, const QString & baza);
  bool save();
  Klassif * findKlN(const QString & name);
  Klassif * findKlN2(const QString & name);
  Klassif * getN(int n);
  void setN(int n, Klassif *kls);

  int m_nMaxPrio;
};

extern KlList m_kl_list;
