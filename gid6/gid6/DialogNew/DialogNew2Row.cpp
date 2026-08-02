#include "stdafx.h"
#include "gid6.h"

#include "DialogNew2Row.h"

bool GotoKey(CAdoFile *dbf, int n1);


CString ROW2::getName(int i)
{
  map<int, CString>::const_iterator it = map_name.find(i);

  if (it != map_name.end()) {
    return it->second;
  }
  return "";
}

CString ROW2::getTable(int i)
{
  map<int, CString>::const_iterator it = map_table.find(i);

  if (it != map_table.end()) {
    return it->second;
  }
  return "";
}

BOOL ROW2::readRow(CAdoFile *dbf, const char *tn1, int nom, int is_new)
{
  FIELD fld;

  if (!is_new) m_geo_nom = atoi(dbf->read("nomer_geo"));

  CDaoFieldInfo fieldInfo;

  nFlds = dbf->rs->GetFieldCount();
  for ( int i = 0; i < nFlds; i++ ) {
    dbf->rs->GetFieldInfo(i, fieldInfo, AFX_DAO_SECONDARY_INFO);

    fld.typ = fieldInfo.m_nType;
    fld.size = fieldInfo.m_lSize;
    fld.val = "";
    if (!is_new) {
      fld.val = dbf->read(i+1);
      dbf->rs->GetFieldValue(i, fld.var);
    }
    fld.num = i+1;

    CString fn = fieldInfo.m_strName;  fn.MakeUpper();
    CString tn = tn1;  tn.MakeUpper();

    map_table[i] = tn;
    map_name[i] = fn;

    map_f[tn][nom][fn] = fld;
  }

  dbf->readCoord(m_NP);

  return TRUE;
}

void ROW2::delRowTable(const char *tn1)
{
  CString tn = tn1;  tn.MakeUpper();
  map<CString, map<int, map<CString , FIELD> > >::iterator it1 = map_f.find(tn);
  if (it1 != map_f.end()) {
    map_f.erase(it1);
  }
}


BOOL ROW2::addRow(const char *tn1)
{
  int nom = 0;

  CString tn = tn1;  tn.MakeUpper();
  map<CString, map<int, map<CString , FIELD> > >::iterator it1 = map_f.find(tn);
  if (it1 != map_f.end()) {
    nom = it1->second.size();
    nom ++;
  }

  FIELD fld;
  fld.typ = VT_I4;
  fld.size = 4;
  fld.val.Format("%d", nom);
  fld.var = COleVariant((long)nom, VT_I4);
 
  map_f[tn][nom]["ID_ПЕЛНМР"] = fld;

  fld.typ = VT_I4;
  fld.size = 4;
  fld.val.Format("%d", 0);
  fld.var = COleVariant(0L, VT_I4);
  
  map_f[tn][nom]["ID_йнкнд_нонпш_мюдг"] = fld;
  map_f[tn][nom]["ID_йнкндж_нонпш_лепно"] = fld;
  map_f[tn][nom]["ID_йнкндж_нонпш_ондг"] = fld;
  map_f[tn][nom]["ID_йюопелнмр_рпса"] = fld;
  map_f[tn][nom]["ID_йюопелнмр_рпса_лепноп"] = fld;

  return FALSE;
}

BOOL ROW2::delRow(const char *tn1, int nom)
{
  CString tn = tn1;  tn.MakeUpper();

  map<CString, map<int, map<CString , FIELD> > >::iterator it1 = map_f.find(tn);
  if (it1 != map_f.end()) {

    map<int, map<CString , FIELD> >::iterator it2 = it1->second.begin();
    map<int, map<CString , FIELD> >::iterator it3;

    for (; it2 != it1->second.end(); it2++) {
      int n = it2->first;
      if (n >= nom) {
        it3 = it2;
        it3 ++;
        if (it3 != it1->second.end()) {
          it1->second[n] = it3->second;
        }
        else {
          it1->second.erase(it2);
          break;
        }
      }
    }
  }
  return FALSE;
}

void ROW2::init()
{
  map_f.clear();
}

int ROW2::size(const char *tn1)
{
  CString tn = tn1;  tn.MakeUpper();

  map<CString, map<int, map<CString , FIELD> > >::iterator it1 = map_f.find(tn);
  if (it1 != map_f.end()) {
    return it1->second.size();
  }
  return 1;
  
//  return map_f.size();
}

int ROW2::readRows(CAdoFile *dbf, const char *tn, const char *q, int n1)
{
  CString que = tn;
  if (q[0]) que = q;

  FIELD fld;

  int n = 0;

  if (dbf->openTable(que)) {
    if (n1 == 0) {
      if (readRow(dbf, tn, n, 1)) {
        n++;
      }
    }

    else if (n1 == -1) {
        while ( !dbf->isEOF() ) {
          if (readRow(dbf, tn, n, 0)) {
            n++;
          }
          dbf->MoveNext();
      }
    }
    else {
      if (GotoKey(dbf, n1)) {
        if (readRow(dbf, tn, n, 0)) {
          n++;
        }
      }
    }
    dbf->closeTable();
  }
  return n;
}

int ROW2::getRecs(const char *tn1)
{
  CString tn = tn1;
  tn.MakeUpper();

  map<CString, map<int, map<CString , FIELD> > >::const_iterator it =  map_f.find(tn);

  if (it != map_f.end()) {
    return it->second.size();
  }
  return 0;
}

BOOL ROW2::getField(const char *tn1, const char *fn1, FIELD & field, int nom)
{
  CString tn = tn1;  tn.MakeUpper();
  CString fn = fn1;  fn.MakeUpper();

  map<CString, map<int, map<CString , FIELD> > >::const_iterator it1 = map_f.find(tn);
  if (it1 != map_f.end()) {
    map<int, map<CString , FIELD> >::const_iterator it2 = it1->second.find(nom);
    if (it2 != it1->second.end()) {
      map<CString, FIELD>::const_iterator it3 = it2->second.find(fn);
      if (it3 != it2->second.end()) {
        field = it3->second;
        return TRUE;
      }
      else {
      }
    }
  }

  return FALSE;
}
