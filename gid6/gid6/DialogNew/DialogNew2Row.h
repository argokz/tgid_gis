#if !defined(DIALOGNEW2ROW)
#define DIALOGNEW2ROW

#include "ado.h"

struct FIELD {
  CString val;
  COleVariant var;
  int typ;
  int num;
  int size;
};

struct ROW2 
{
//  map<CString, map<int, map<CString , FIELD> > > map_f;
  map<CString, map<int, map<CString , FIELD> > > map_f;
  
  map<int, CString> map_name;
  map<int, CString> map_table;
  int nFlds;
  int m_geo_nom;
  CCoordList m_NP;

  int size(const char *tn);
  void init();
  BOOL addRow(const char *tn1);
  BOOL readRow(CAdoFile *dbf, const char *tn, int nom, int is_new);
  int readRows(CAdoFile *dbf, const char *tn, const char *q, int n1);
  BOOL getField(const char *tn1, const char *fn1, FIELD & field, int nom);
  BOOL delRow(const char *tn1, int nom);
  int getRecs(const char *tn1);
  void delRowTable(const char *tn1);
  CString getName(int i);
  CString getTable(int i);
};

#endif