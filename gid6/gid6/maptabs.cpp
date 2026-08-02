#include "stdafx.h"

#include "gid6.h"

#include "gid6Doc.h"
#include "gidrView.h"
#include "ado.h"

extern CGeoFile *m_geofile;


void OpenList2(CAdoFile *ado, const char *q, list<COleVariant> & lst);

#include "win.h"

#include "maptabs.h"

static map<CString, map<CString, MFIELDS> > map_tab_all;

void clear_map_tab_all()
{
  map_tab_all.clear();
}



BOOL getTab1(const char *mdb, const char *m_tn, map<CString,  map <CString, MFIELDS > > &map_tabs, bool isADO)
{
  CString tn;
  map <CString, MFIELDS> map_tab;

//  tn.Format("tabs/%s.tab", m_tn);

  char s[1024];

//  FILE *f = fopenexe(tn, "r");

//  if (!f) {
//    tn.Format("tabs4/%s.tab", m_tn);
//    f = fopenexe(tn, "r");
//  }

  FILE *f = fopen_tabs(mdb, m_tn, "tab", "r");

  if (f) {
    while (!feof(f)) {
      CString tabn = "", fn, q, cod, tn;
      CString s1, s2, s3;
      
      if (fgetstr(s, 1023, f) == NULL) break;

      cod = s;

      MFIELDS map_tab_1;

      if (cod == "Z3") {
        int qq;
        qq = 1;
      }

      if (cod == "Z1" || cod == "Z3" || cod == "Q1" || cod == "Q2" || cod == "Q3" || cod == "T1" || cod == "T2" || cod == "T3") {
        if (fgetstr(s, 1023, f) == NULL) break;
        tabn = s;

        if (fgetstr(s, 1023, f) == NULL) break;
        fn = s;

        bool isSprav = false;

        if (fn == "MAIN") {
//          fn = m_dbf->m_strFileName;
          fn = mdb;
        }
#if 0
        else if (fn == "PS") {
          fn = getPS();
        }
        else if (fn == _TR("Договорные нагрузки")) {
//          fn = getPSAlma();
          KlList *m_kl_list = m_geofile->m_kl_list;
          Klassif *kls = m_kl_list->findKlN(transl("Здания потребителей"));
          if (kls) fn = kls->FileName();
        }
        else if (fn == "PS_STRUCT") {
          fn = getPSAlma();
          isSprav = true;
        }
        else if (fn == _TR("СтандОбор.mdb")) {
          fn = getStandObor();
          isSprav = true;
        }
        else if (fn == _TR("Справочная.mdb")) {
          fn = getSpravochnaya();
          isSprav = true;
        }
        else if (fn == "PSSprav") {
          fn = getPsSprav();
          isSprav = true;
        }
#endif
        else if (fn == "PS2") {
          fn = argpath()+"PS2.mdb";
        }
        else if (fn == "MAP") {
          if (fgetstr(s, 1023, f) == NULL) break;
          tn = s;

          KlList *m_kl_list = m_geofile->m_kl_list;
          Klassif *kls = m_kl_list->findKlN(tn);
          if (kls) fn = kls->FileName();
        }
        else if (fn.Find(":") == -1) {
          fn = argpath()+fn;
          isSprav = true;
        }


        if (fgetstr(s, 1023, f) == NULL) break;
        q = s;

//        q.Replace("$PS$", getPS());

        bool isFind = false;


        if (isSprav) {
          map<CString, map<CString, MFIELDS> >::const_iterator it1 = map_tab_all.find(fn);

          if (it1 != map_tab_all.end()) {
            map<CString, MFIELDS>::const_iterator it2 = it1->second.find(q);
            if (it2 != it1->second.end()) {
              map_tab_1 = it2->second;
              isFind = true;
            }
          }
        }

        if (!isFind) {
          if (!isADO) {
/*
            CDbFile dbf;

            if (dbf.openMDB(fn)) {
              if (dbf.openTable(q)) {
                while ( !dbf.rs->IsEOF() ) {
                  CString s1 = "", s2 = "", s3 = "";
                  s1 = dbf.read(1);
                  s2 = s1;
                  if (cod == "Q2" || cod == "Q3" || cod == "T2" || cod == "T3" || cod == "Z3") {
                    s2 = dbf.read(2);
                  }

                  if (cod == "Z3") {
                    s3 = dbf.read(3);
                  }


                  map_tab_1.map1[s1] = s2;

                  map_tab_1.map_q[s1] = s3;
                  
                  map_tab_1.list2.push_back(pair<CString, CString> (s1, s2));

                  dbf.rs->MoveNext();
                }
                dbf.closeTable();

                if (isSprav) {
                  map_tab_all[fn][q] = map_tab_1;
                }
              }
              dbf.closeMDB();
            }
*/
          }
          else {
            list< vector<COleVariant> > lst;
            OpenList2(getAdo(fn), q, lst);

            list< vector<COleVariant> >::const_iterator it = lst.begin();

            for (; it != lst.end(); it++) {
              CString s1 = "", s2 = "";
              s1 = CCrack::strVARIANT2((*it)[0]);

              s2 = s1;
              if (cod == "Q2" || cod == "Q3" || cod == "T2" || cod == "T3") {
                s2 = CCrack::strVARIANT2((*it)[1]);
              }

              map_tab_1.map1[s1] = s2;
              map_tab_1.list2.push_back(pair<CString, CString> (s1, s2));
            }

            if (isSprav) {
              map_tab_all[fn][q] = map_tab_1;
            }
          }
        }
      }
      else if (cod == "L2") {
        if (fgetstr(s, 1023, f) == NULL) break;
        tabn = s;
        if (fgetstr(s, 1023, f) == NULL) break;
        int n = atoi(s);
        for (int i = 0; i < n && i < 256; i++) {
          if (fgetstr(s, 1023, f) == NULL) break;
          trim(s);
          s1 = s;
          if (s1 == "@") s1 = "";


          if (fgetstr(s, 1023, f) == NULL) break;
          trim(s);
          s2 = s;
//          map_tab_1[s1] = s2;
          map_tab_1.map1[s1] = s2;
          map_tab_1.list2.push_back(pair<CString, CString> (s1, s2));
        }
      }
      else if (cod == "L1") {
        if (fgetstr(s, 1023, f) == NULL) break;
        tabn = s;
        if (fgetstr(s, 1023, f) == NULL) break;
        int n = atoi(s);
        for (int i = 0; i < n && i < 256; i++) {
          if (fgetstr(s, 1023, f) == NULL) break;
          trim(s);
          s1 = s;
          if (s1 == "@") s1 = "";
          map_tab_1.map1[s1] = s1;
          map_tab_1.list2.push_back(pair<CString, CString> (s1, s1));
        }
      }

      if (tabn != "") {
        tabn.MakeUpper();
        map_tab[tabn] = map_tab_1;
      }
    }
    fclose(f);

    map_tabs[m_tn] = map_tab;
  }


  return TRUE;
}

bool MapTabs::getMap(const char *tn, const char *fn, MFIELDS &map1) 
{
  map < CString, map <CString, MFIELDS > >::const_iterator it1;

  it1 = map_tabs.find(tn);
  if (it1 == map_tabs.end()) return false;

  map <CString, MFIELDS> ::const_iterator it2;

  it2 = it1->second.find(fn);
  if (it2 == it1->second.end()) return false;

  map1 = it2->second;

  return true;
}



MapTabs::MapTabs()
{
}

void MapTabs::Init(const char *mdb, const char *m_tn, bool isADO)
{
  CString tnU = m_tn;  tnU.MakeUpper();
  
  getTab1(mdb, tnU, map_tabs, isADO);
}

static MapTabs maptabs;

void InitTab(const char *mdb, const char *m_tn, bool isADO)
{
  CString tnU = m_tn;  tnU.MakeUpper();
  maptabs.Init(mdb, tnU, isADO);
}

bool getMap(const char *tn, const char *fn, MFIELDS &map1) 
{
  CString tnU = tn;  tnU.MakeUpper();
  CString fnU = fn;  fnU.MakeUpper();

  return maptabs.getMap(tnU, fnU, map1) ;
}

CString getReal(const char *tn, const char *fn, const char *val) 
{
  MFIELDS map1;

  CString v = val;

  CString TN = tn; TN.MakeUpper();
  CString FN = fn; FN.MakeUpper();

  if (maptabs.getMap(TN, FN, map1)) {

    map<CString, CString>::iterator it = map1.map1.find(val);

    if (it != map1.map1.end()) {
      v = it->second;
    }
  }

  return v;
}


CString getReal(const char *tn, const char *fn, int val) 
{
  CString s;
  s.Format("%d", val);

  return getReal(tn, fn, s) ;
}

void InitTab0(const char *mdb, const char *m_tn)
{
  InitTab(mdb, m_tn, false);
}

int getMap0(const char *tn, const char *fn, char **buf) 
{
  MFIELDS map0;
  int n = 0;
  
  bool ret = getMap(tn, fn, map0);

  if (ret) {
    map<CString, CString>::const_iterator it;

    int sz = 0;

    for (it = map0.map1.begin(); it != map0.map1.end(); it++) {
      CString s1 = it->first;
      CString s2 = it->second;

      sz += s1.GetLength()+20;
      sz += s2.GetLength()+20;
      n ++;
    }

    *buf = new char[sz];

    char *b = *buf;

    for (it = map0.map1.begin(); it != map0.map1.end(); it++) {
      CString s1 = it->first;
      CString s2 = it->second;

      strcpy(b, s1); b += s1.GetLength(); *b = 0; b++;
      strcpy(b, s2); b += s2.GetLength(); *b = 0; b++;
    }
  }
  return n;
}
