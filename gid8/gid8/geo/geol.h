#ifndef GEOL_H
#define GEOL_H

//#include "shp_dbf.h"
#include <cxema/coordlis.h>

class QString;
class KlList;

class CGeoObject1
{
public:
  CGeoObject1()
  {
  }

  CFRect rect;
  int nom;
  bool mark;
  CCoordList cl;
};


class GeoL
{
public:
  GeoL() { 
    m_num = -1;
    m_rect = CFRect(LONG_MAX, LONG_MAX, LONG_MIN, LONG_MIN);
  };

  virtual bool begin()
  {
    m_num = 0;
    return true;
  }

  virtual void end()
  {
    m_num = -1;
  }

  virtual bool next()
  {
    if (m_num < 0 || m_num >= size()) return false;
    m_num ++;
    bool ret = go_to(m_num);
    return ret;
  }

  virtual bool go_to(unsigned long n) = 0;
  virtual bool read_coor(CCoordList &cl) = 0;

  int size()
  {
    return m_geo4.size();
  } 

  CFRect rect() {
    return m_rect;
  }

  QVector<CGeoObject1> m_geo4;
  bool is_shape;
  CFRect m_rect;
  int m_num;
};

#if 0
class GeoShape : public GeoL
{
public:
  GeoShape() : GeoL() {
    m_shp = NULL;
  };
  ~GeoShape() { 
    if (m_shp) delete m_shp;
  }

  bool go_to(unsigned long n);
  bool read_coor(CCoordList &cl);
  
  bool open(const char *fn);
  Cshp_dbf *m_shp;
};


class GeoArc : public GeoL
{
public:
  GeoArc() : GeoL() {
    m_query = NULL;
    m_cur = -1;
  };
  ~GeoArc() { 
    delete m_query;
  }

  bool go_to(unsigned long n);
  bool read_coor(CCoordList &cl);

  bool open(const QString & fn, const QString & tn);

  QString m_tn;
  QString m_fn;
  int m_n_shape; 
  CCoordList m_coord;
  QSqlQuery *m_query;
  int m_cur;
};


class Geomedia : public GeoL
{
public:
  Geomedia() : GeoL() {
    m_cur = -1;
    m_query = NULL;
  };
  ~Geomedia() { 
    delete m_query;
  }

  bool go_to(unsigned long n);
  bool read_coor(CCoordList &cl);

  bool open(const QString & fn, const QString & tn);

  QString m_tn;
  QString m_fn;
  int m_n_shape; 
  CCoordList m_coord;
  QSqlQuery *m_query;
  int m_cur;
};

#endif

class GeoMap : public QMap<QString, GeoL *>
{
public :
  bool open(const QString & fn);
  CFRect rect();
};

#endif
