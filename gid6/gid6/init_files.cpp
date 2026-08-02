#include "stdafx.h"
#include "gid6.h"
#include "ado.h"
#include "geofile.h"

#include "klassif.h"
#include "win.h"


static set<CString> set_special;

char *readstr(char *s, char *ss);
char *readint(char *s, int &d);
char *readint(char *s, unsigned char &d);
char *readint(char *s, unsigned long &d);
char *readint(char *s, long &d);

bool find_special(CString ss)
{
    set<CString>::const_iterator it = set_special.find(ss);
    if (it == set_special.end()) {
        return true;
    }
    return false;
}


bool open_special(const char *kln) {
  FILE *f;
  char *s, ss[256], str[1024];

  set_special.clear();

  if (!kln[0]) return false;

  f = fopen(kln,"r");

  if (!f) return false;

  while (!feof(f)) {
    s = str;
    if (fgets(s,1023,f) == NULL) break;

    int iv;

    s = readstr(s, ss); if (ss[0] == 0) break;
    s = readint(s, iv);
    s = readint(s, iv);
    s = readint(s, iv);
    s = readint(s, iv);
    s = readint(s, iv);
    s = readint(s, iv);
    s = readint(s, iv);
    s = readstr(s, ss);
    CString str = ss;
    str.MakeLower();
    set_special.insert(str);
  }
  fclose(f);

  return true;
}


static map<CString, map<CString, CString> > map_map_txt;
static map<CString, map<CString, map<CString, CColName> > > map_map_col_txt;

CString findTableRusName(CString d, CString n)
{
    CString nn = n;

    d.MakeLower();
    n.MakeLower();

    map<CString, map<CString, CString> >::const_iterator it1 = map_map_txt.find(d);
    if (it1 != map_map_txt.end()) {
        map<CString, CString>::const_iterator it2 = it1->second.find(n);
        if (it2 != it1->second.end()) {
            return it2->second;
        }
    }

    return nn;
}


CString findTableEngName(CString d, CString n)
{
    CString nn = n;

    d.MakeLower();
    n.MakeLower();

    map<CString, map<CString, CString> >::const_iterator it1 = map_map_txt.find(d);
    if (it1 != map_map_txt.end()) {

        for (map<CString, CString>::const_iterator it2 = it1->second.begin(); it2 != it1->second.end(); it2++) {
            CString n2 = it2->second;
            n2.MakeLower();
            if (n2 == n) {
                return it2->first;
            }
        }
    }

    return nn;
}



const CColName *findColumnRusName(CString d, CString n1, CString n2)
{
    d.MakeLower();
    n1.MakeLower();
    n2.MakeLower();

    map<CString, map<CString, map<CString, CColName> > >::const_iterator it1 = map_map_col_txt.find(d);
    
    if (it1 != map_map_col_txt.end()) {
        map<CString, map<CString, CColName> >::const_iterator  it2 = it1->second.find(n1);
        if (it2 != it1->second.end()) {

          map<CString, CColName>::const_iterator  it3 = it2->second.find(n2);
          if (it3 != it2->second.end()) {
              return &it3->second;
          }
        }
    }

    if (n1 != "?") {
        return findColumnRusName(d, "?", n2);
    }

    return nullptr;

//    CColName col_name;
//    col_name.name = n2;
//    col_name.name_full = "";

//    return col_name;
}



CString findTableRusNameGid(CString n)
{
    return findTableRusName(getGidAdoName(), n);
}

/**
CString findColumnRusNameGid(CString n1, CString n2)
{
    return findColumnRusName(getGidAdoName(), n1,n2);
}
*/

CString findTableRusNameGeo(CString n0)
{
    CString n = n0;
    Klassif *kls = m_geofile->m_kl_list->findKlN(n);
    if (kls) {
        n = findTableRusName(kls->m_ado->m_schema, n);
        if (n == n0) {
            n = findTableRusName("OpenStreetMap", n);
        }
    }
    return n;
}

/**
CString findColumnRusNameGeo(CString n1, CString n2)
{
    Klassif *kls = m_geofile->m_kl_list->findKlN(n1);
    if (kls) {
        return findColumnRusName(kls->m_ado->m_schema, n1,n2);
    }
    return n2;
}
*/


void initColumnRusNameFile(CString database, CString database2, CString klfn)
{
    FILE *f = fopen(klfn, "r");
    if (f) {
      char *s, str[10240], name_e[1024], name_col_e[1024], name_col_r[1024], name_full[10240];

      while (!feof(f)) {
        s = str;
        if (fgets(s,10230,f) == NULL) break;

        s = readstr(s, name_e);
        s = readstr(s, name_col_e);
        s = readstr(s, name_col_r);
        s = readstr(s, name_full);

        CString d = database; d.MakeLower();
        CString n1 = name_e;   n1.MakeLower();
        CString n2 = name_col_e;   n2.MakeLower();

        CColName col_name;
        col_name.name = name_col_r;
        col_name.name_full = name_full;
        col_name.name_full.Replace("\\n", "\n");

        map_map_col_txt[d][n1][n2] = col_name;
      }
      fclose (f);
    }
}

void initColumnRusName(CString database, CString database2)
{
    CString klfn;

    klfn.Format("%skls\\%s.txt1", argpath(), database2);
    initColumnRusNameFile(database, database2, klfn);

    klfn.Format("%skls\\%s.txt2", argpath(), database2);
    initColumnRusNameFile(database, database2, klfn);

    klfn.Format("%s%s.txt2", argpath_2(), database2);
    initColumnRusNameFile(database, database2, klfn);


    CString sprav = "sprav";
    klfn.Format("%skls\\%s.txt2", argpath(), sprav);
    initColumnRusNameFile(sprav, sprav, klfn);
}

//static map<CString

static map<CString, map<CString, map<CString, ForeignTable> > > map_lookup;


void initLookup(CString database, CString database2)
{
    CString klfn;

    klfn.Format("%skls\\%s.lookup", argpath(), database2);
    FILE *f = fopen(klfn, "r");
    if (f) {
      char *s, str[1024], s1[1024], s2[1024], s3[1024], s4[1024], s5[1024];
      CString tn, fn, tn_l, s_id = "id", s_name = "name", q = "";
      int isSort = true;

      while (!feof(f)) {
        s = str;
        if (fgets(s,1023,f) == NULL) break;

        s = readstr(s, s1); tn = s1;
        s = readstr(s, s2); fn = s2;
        s = readstr(s, s3); tn_l = s3;
        s = readstr(s, s4); s_id = s4;

        s = readstr(s, s5); s_name = s5;

        s = readint(s, isSort);

        s = readstr(s, s5); q = s5;

        CString d = database; d.MakeLower();
        tn.MakeLower();
        fn.MakeLower();
        tn_l.MakeLower();
        s_id.MakeLower();

//        tn = transl(tn);
//        fn = transl(fn);
//        tn_l = transl(tn_l);
//        s_id = transl(s_id);

        ForeignTable ft;

        ft.field = s2;
        ft.id = s_id;
        ft.name = s_name;
        ft.table = tn_l;
        ft.cxema = d;
        ft.isSort = (isSort != 0);

        if (q != "") {
          ft.q = q;
        }
        else {
//          ft.q.Format("SELECT %s, %s FROM %s.dbo.%s", ft.id, s_name, ft.cxema, ft.table);
           ft.q = "";
        }

        map_lookup[d][tn][fn] = ft;
      }
      fclose (f);
    }
}


// Старые и новые названия полей

static map<CString, map<CString, CString> > map_col_old_new;
static map<CString, map<CString, CString> > map_col_new_old;

void initStruct1()
{
    CString klfn;

    klfn.Format("%skls\\struct1.txt", argpath());
    FILE *f = fopen(klfn, "r");
    if (f) {
      char *s, str[1024], s1[1024], s2[1024], s3[1024];
      CString tn, fn_old, fn_new;
      int isSort = true;

      while (!feof(f)) {
        s = str;
        if (fgets(s,1023,f) == NULL) break;

        s = readstr(s, s1); tn = s1;
        s = readstr(s, s2); fn_old = s2;
        s = readstr(s, s3); fn_new = s3;

        tn.MakeLower();
        fn_old.MakeLower();
        fn_new.MakeLower();

        map_col_old_new[tn][fn_old] = fn_new;
        map_col_new_old[tn][fn_new] = fn_old;
      }
      fclose (f);
    }
}

CString find_old_name_col(const char* tn1, const char* fn_new1)
{
    CString tn = tn1;
    CString fn_new = fn_new1;
    tn.MakeLower();
    fn_new.MakeLower();

    auto it1 = map_col_new_old.find(tn);
    if (it1 != map_col_new_old.end()) {
        auto it2 = it1->second.find(fn_new);
        if (it2 != it1->second.end()) {
            return it2->second;
        }
    }
    return fn_new1;
}


CString find_new_name_col(const char* tn1, const char* fn_old1)
{
    CString tn = tn1;
    CString fn_old = fn_old1;
    tn.MakeLower();
    fn_old.MakeLower();

    auto it1 = map_col_old_new.find(tn);
    if (it1 != map_col_old_new.end()) {
        auto it2 = it1->second.find(fn_old);
        if (it2 != it1->second.end()) {
            return it2->second;
        }
    }
    return fn_old1;

}





ForeignTable findLookup(CString d, CString n1, CString n2)
{
    ForeignTable ft;
    ft.id = "";
    ft.table = "";

    d.MakeLower();
    n1.MakeLower();
    n2.MakeLower();

    map<CString, map<CString, map<CString, ForeignTable> > >::const_iterator it1 = map_lookup.find(d);
    
    if (it1 != map_lookup.end()) {
        map<CString, map<CString, ForeignTable> >::const_iterator  it2 = it1->second.find(n1);
        if (it2 != it1->second.end()) {

          map<CString, ForeignTable>::const_iterator  it3 = it2->second.find(n2);
          if (it3 != it2->second.end()) {
              return it3->second;
          }
        }
    }
    return ft;
}

const map<CString, ForeignTable> *findLookup(CString d, CString n1)
{
    d.MakeLower();
    n1.MakeLower();

    map<CString, map<CString, map<CString, ForeignTable> > >::const_iterator it1 = map_lookup.find(d);

    if (it1 == map_lookup.end()) {
        CString dd = getGidAdoName();
        dd.MakeLower();
      
        if (d == dd) {
            it1 = map_lookup.find("gid");
        }

    }
    
    if (it1 != map_lookup.end()) {
        map<CString, map<CString, ForeignTable> >::const_iterator  it2 = it1->second.find(n1);
        if (it2 != it1->second.end()) {
            return &it2->second;
        }
    }
    return 0;
}



void initTableRusNameFile(CString database, CString database2, CString klfn)
{
    FILE *f = fopen(klfn, "r");
    if (f) {
        char *s, str[1024], name_e[1024], name_r[1024];
        int loc;

        while (!feof(f)) {
            s = str;
            if (fgets(s, 1023, f) == NULL) break;

            s = readint(s, loc);

            if (s) {
                s = readstr(s, name_e);
                if (s) {
                    s = readstr(s, name_r);
                    if (s) {

                        CString d = database; d.MakeLower();
                        CString n = name_e;   n.MakeLower();

                        map_map_txt[d][n] = name_r;
                    }
                }
            }
        }
        fclose(f);
    }
}

void initTableRusName(CString database, CString database2)
{
    CString klfn;

    klfn.Format("%skls\\%s.txt", argpath(), database2);

    initTableRusNameFile(database, database2, klfn);

    klfn.Format("%s%s.txt", argpath_2(), database2);

    initTableRusNameFile(database, database2, klfn);
}


void addTableRusName(CString database, CString name_e, CString name_r)
{
    CString d = database; d.MakeLower();
    CString n = name_e;   n.MakeLower();

    map_map_txt[d][n] = name_r;
}


/*

void saveTableRusName(CString database)
{
    CString klfn;

    CString dgid = getGidAdoName(); dgid.MakeLower();
    CString d0 = database; d0.MakeLower();
    CString d = d0;

    if (d == dgid) {
        d = "gid";
    }

    klfn.Format("%s%s.txt", argpath_2(), d);
    FILE* f1 = fopen(klfn, "w");

    klfn.Format("%s%s.txt2", argpath_2(), d);
    FILE *f2 = fopen(klfn, "w");
    if (f1 && f2) {
        KlList::iterator it = m_geofile->m_kl_list->begin();

        for (; it != m_geofile->m_kl_list->end(); it++) {
            Klassif* kl = &it->second;

            CString q;
            CAdoFile* ado = kl->m_ado;

            if (ado->m_schema.CompareNoCase(d0) == 0) {
                CString rus = findTableRusName(database, kl->nazv);

//                fprintf(f1, "0,\"%s\",\"%s\"\n", kl->nazv, rus);
                
                q.Format("SELECT * FROM %s", transl(kl->nazv));
                if (ado && ado->openTable0(q)) {
                    for (int i = 0; i < ado->NFlds(); i++) {
                        CString name = ado->FieldName(i);
                        CString rus = findColumnRusName(database, kl->nazv, name);
                        fprintf(f2, "\"%s\",\"%s\",\"%s\"\n", kl->nazv, name, rus);
                    }
                }
            }
        }
        fclose(f1);
        fclose(f2);
    }
}
*/


void saveTableRusName(CString database)
{
    CString klfn;

    CString dgid = getGidAdoName(); dgid.MakeLower();
    CString d0 = database; d0.MakeLower();
    CString d = d0;

    if (d == dgid) {
        d = "gid";
    }

    klfn.Format("%s%s.txt", argpath_2(), d);
    mkdir_for_file(klfn);
    FILE* f1 = fopen(klfn, "w");

    if (f1) {
        KlList::iterator it = m_geofile->m_kl_list->begin();

        for (; it != m_geofile->m_kl_list->end(); it++) {
            Klassif* kl = &it->second;

            CString q;
            CAdoFile* ado = kl->m_ado;

            if (ado->m_schema.CompareNoCase(d0) == 0) {
                CString rus = findTableRusName(database, kl->nazv);

                fprintf(f1, "0,\"%s\",\"%s\"\n", kl->nazv, rus);
            }
        }
        fclose(f1);
    }
}



void saveTableRusName2(CString database, CString klfn)
{
//    CString klfn;

    CString dgid = getGidAdoName(); dgid.MakeLower();
    CString d0 = database; d0.MakeLower();
    CString d = d0;

    if (d == dgid) {
        d = "gid";
    }



//    klfn.Format("%s%s.txt2", argpath_2(), d);

    
    mkdir_for_file(klfn);
    FILE *f2 = fopen(klfn, "w");
    if (f2) {
        KlList::iterator it = m_geofile->m_kl_list->begin();

        for (; it != m_geofile->m_kl_list->end(); it++) {
            Klassif* kl = &it->second;

            CString q;
            CAdoFile* ado = kl->m_ado;

            if (ado->m_schema.CompareNoCase(d0) == 0) {
                CString rus = findTableRusName(database, kl->nazv);
                
                q.Format("SELECT * FROM %s", transl(kl->nazv));
                if (ado && ado->openTable0(q)) {
                    for (int i = 0; i < ado->NFlds(); i++) {
                        CString name = ado->FieldName(i);
                        CString rus = name;
                        const CColName *col_name = findColumnRusName(database, kl->nazv, name);
                        if (col_name) {
                            rus = col_name->name;
                        }
                        fprintf(f2, "\"%s\",\"%s\",\"%s\"\n", kl->nazv, name, rus);
                    }
                    ado->closeTable();
                }
            }
        }
        fclose(f2);
    }
}

