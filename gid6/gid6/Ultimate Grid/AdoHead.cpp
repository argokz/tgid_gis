
#include "stdafx.h"

#include "AdoHead.h"
#include "Win.h"

int regex_match(const char *p, const char *buf, vector<CString> & match);
CString argpath();

#include "ado.h"
#include "join2.h"

#include "geofile.h"


CString getFullName(CString cx, CString tn, CString fn);

int field_name_to_id(const char *fn)
{
    return atoi(fn);
}


void setDefaultTable(CString db, CString tn, AdoHead &h, bool nolookup, int shift)
{
    h.rows = 1;

    db.Replace("$ps$", getPsAdoName());
    db.Replace("$main$", getGidAdoName());

    CString q;

    CAdoFile *ado = getAdo(db);

    if (ado) {
        Join2 j;
//        int n0 = j.initJoin(ado, tn);
//        j.allJoin(ado, tn, n0);
//        q = j.getQ();

        int n0 = j.initJoin(ado, tn, true, h.filtr);
        j.allJoin(ado, tn, n0);
        CString q;

        if (shift == 0) {
            q = j.getQ();
        }
        else if (shift == 999) {
            q = j.getQ_shape();
            shift = 0;
        }
        else {
            q = j.getQ_shape();
        }

        if (h.cols > 0) {
            vector<CString> match;

            int l = regex_match("^SELECT\\s+(.+?)\\s+FROM\\s+.+$", q, match);
            if (l >= 1) {
                CString par = match[1];

                CString fn0;

                map<CString, list<CString> > ff;

                int i = 0;


                for (CString s = par.Tokenize(",", i); i >= 0; s = par.Tokenize(",", i)) {
                    s += " ";
                    int l = regex_match("^\\s*_TAB([0-9]+)_.([^ ]+)\\s+.*$", s, match);
                    if (l >= 1) {
                        int n = atoi(match[1]);
                        CString fn = match[2];

                        if (n == 1) {
                            fn0 = fn;
                        }
                        ff[fn0].push_back(s);
                    }
                }

                CString ss;

                int c = 0;

                for (auto it : h.m_fields0) {
                    auto it2 = ff.find(it.second);
                    if (it2 != ff.end()) {
                        int cc = 0;

                        for (auto it3 : it2->second) {
                            if (ss != "") ss += ",";
                            ss += &*it3;

                            if (cc == 0 && it2->second.size() > 1) h.width[c] = 0;

                            c++;
                            cc++;
                        }
                    }
                }

                h.q1 = q;
                q.Replace(par, ss);
                h.q2 = q;
            }
        }



        if (ado->openTable0(q)) {
            int nf = ado->NFlds();

            vector<COleVariant> vv(nf);
            vector<CAdoField> vf(nf);

            for (long i = 0; i < nf; i++) {
                COleVariant v = ado->read(i);
                CAdoField f = ado->Field(i);

                vv[i] = v;
                vf[i] = f;
            }
            ado->closeTable();

            bool look_old = false;
            CString old_name = "";
            CString old_q = "";

            for (long i = 0; i < nf; i++) {
//                COleVariant v = vv[i];
//                CAdoField f = vf[i];
  
//                const CJField *jf = j.getField(i);

                COleVariant v = vv[i];
                CAdoField f = vf[i];

                CString fname = f.name;

/*
                vector<CString> match;
                int l = regex_match("^_TAB[0-9]+____(.+)$", f.name, match);

                if (l > 0) {
                    fname = match[1];
                }
*/

                const CJField *jf = j.getFieldName(fname); 

                bool look = false;

                if (jf) {
                    CString tname = j.getTable(jf->table_num);

                    look = jf->isList;

                    h.width[i+shift] =  look ? 0 : -1;
                    h.map_h[0][i+shift] = "$";

                    CString rus_name =  getFullName(db, tname, jf->name);

//                    if (i > 0 && !look_old && !look) {
                    if (i > 0 && !look_old) {
                        h.is_edit.insert(i+shift);
                    }

                    if (look_old) {
                        h.map_h[0][i+shift] = old_name;
                    }
                    else {
                        h.map_h[0][i+shift] = rus_name;
                    }

//                    h.map_rus[jf->name] = rus_name;
                    h.map_rus[f.name] = h.map_h[0][i+shift];

                    if (look_old && !nolookup) {
                       AdoLookup look;
                       look.db = db;
                       look.tn = tname;
                       look.f_name = jf->name;
                       look.rus_name = rus_name;
                       look.n_ret = 0;
                       look.q = old_q;
                       h.m_lookup[i+shift] = look;
                    }
                    old_name = rus_name;

                    if (look_old) {
                        look_old = false;
                    }
                    else {
                        look_old = look;
                    }
                    old_q = jf->q;
                }
            }
        }
    }
}



bool readTable(const char *db, const char *tn, AdoHead &h)
{
    bool nolookup = false;
    
    h.rows = 0;

    CString fn;
    fn.Format("%stable\\%s.txt", argpath(), tn);

    h.map_h.clear();

    FILE *f = fopen(fn, "r");


    int n1 = 0;
    int n2 = 0;

    h.save = true;
    h.filtr = "";

/*
    if (!f) {
        if (db[0]) {
            setDefaultTable(db, tn, h, nolookup);
            h.save = false;
            return true;
        }
    }
*/

    if (f) {
        char str[1024 + 1];

        while (!feof(f)) {
            if (fgets(str, 1024, f) == NULL) break;
            str[1024] = 0;
            trim(str);

            int l;

            vector<CString> match;

            l = regex_match("^defaut table\\s+(.+)\\s+(.+)\\s*$", str, match);
            if (l >= 2) {
                setDefaultTable(match[1], match[2], h, nolookup, 0);
                continue;
            }

            l = regex_match("^Filtr\\s+(.+)\\s*$", str, match);
            if (l >= 1) {
                h.filtr = match[1];
                continue;
            }


            l = regex_match("^geo_table\\s+([0-9]+)\\s*$", str, match);
            if (l >= 1) {
                int shift = atoi(match[1]);

                for (int j = 0; j < shift; j++) {
                    h.width[j] = -1;
                }

                Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
                if (kls) {
                    setDefaultTable(kls->m_ado->m_schema, tn, h, nolookup, shift);
                }
                continue;
            }

/*

            if (!strcmp(str, "default gid")) {
                Klassif *kls = m_geofile->m_kl_list->findKlN(tn);
                if (kls) {
                    setDefaultTable(kls->m_ado->m_schema, tn, h, nolookup);
                }
                continue;
            }
*/

            if (!strcmp(str, "NoSave")) {
                h.save = false;
                continue;
            }

            l = regex_match("^List\\s+(.+)\\s*$", str, match);

            if (l >= 1) {
                CString s = match[1];
                int i = 0;
                h.cols = 0;
                for(CString sItem = s.Tokenize(" ", i); i >= 0; sItem = s.Tokenize(" ",i)) {
                    h.cols++;
                    h.m_fields[sItem] = h.cols;
                    h.m_fields0[h.cols] = sItem;
                }
            }

            if (!strcmp(str, "List")) {
                h.cols = 0;

                while (!feof(f)) {
                    if (fgets(str, 1024, f) == NULL) break;

                    str[1024] = 0;
                    trim(str);
                    if (str[0] == '-') continue;
                    if (str[0] != ' ') break;
                    ltrim(str);

                    h.cols++;
                    h.m_fields[str] = h.cols;
                    h.m_fields0[h.cols] = str;
                }
            }



            l = regex_match("^t\\s+([-0-9]+)\\s+([-0-9A-Za-z_]+)\\s+(.+)\\s*$", str, match);
            if (l >= 3) {
                int r = atoi(match[1]);
                int c = field_name_to_id(match[2]);
                CString txt = match[3];
                h.map_h[r][c] = txt;

                continue;
            }

            l = regex_match("^view_documents\\s+([-0-9A-Za-z_]+)\\s+([^\\s]+)\\s+([^\\s]+)\\s+(.+)\\s*$", str, match);
            if (l >= 4) {
                int c = field_name_to_id(match[1]);

                h.m_docs[c].tab1 = match[2];
                h.m_docs[c].tab2 = match[3];
                h.m_docs[c].s3 = match[4];

                continue;
            }



            l = regex_match("^j\\s+([-0-9]+)\\s+([-0-9A-Za-z_]+)\\s+([-0-9]+)\\s+([-0-9A-Za-z_]+)", str, match);
            if (l >= 4) {
                P4 p4;
                p4.r1 = atoi(match[1]);
                p4.c1 = field_name_to_id(match[2]);
                p4.r2 = atoi(match[3]);
                p4.c2 = field_name_to_id(match[4]);
                h.list_p4.push_back(p4);
                continue;
            }

            l = regex_match("^jr\\s+([-0-9]+)\\s+([-0-9A-Za-z_]+)\\s+([-0-9]+)", str, match);
            if (l >= 3) {
                P4 p4;
                p4.r1 = atoi(match[1]);
                p4.c1 = field_name_to_id(match[2]);
                p4.r2 = p4.r1;
                p4.c2 = p4.c1+atoi(match[3])-1;
                h.list_p4.push_back(p4);
                continue;
            }

            l = regex_match("^jd\\s+([-0-9]+)\\s+([-0-9A-Za-z_]+)\\s+([-0-9]+)", str, match);
            if (l >= 3) {
                P4 p4;
                p4.r1 = atoi(match[1]);
                p4.c1 = field_name_to_id(match[2]);
                p4.r2 = p4.r1+atoi(match[3])-1;
                p4.c2 = p4.c1;
                h.list_p4.push_back(p4);
                continue;
            }

            l = regex_match("^w\\s+([0-9A-Za-z_]+)\\s+([-0-9]+)", str, match);
            if (l >= 2) {
                int c = field_name_to_id(match[1]);
                int w = atoi(match[2]);
                h.width[c] = w;
                continue;
            }

            l = regex_match("^h\\s+([0-9]+)\\s+([0-9]+)", str, match);
            if (l >= 2) {
                int r = atoi(match[1]);
                int hh = atoi(match[2]);
                h.height[r] = hh;
                continue;
            }


            l = regex_match("^Rows\\s+([-0-9]+)", str, match);
            if (l >= 1) {
                h.rows = atoi(match[1]);
                continue;
            }

            l = regex_match("^edit\\s+([-0-9A-Za-z_]+)", str, match);
            if (l >= 1) {
                int c = field_name_to_id(match[1]);
                h.is_edit.insert(c);
                continue;
            }

            l = regex_match("^date\\s+([-0-9A-Za-z_]+)", str, match);
            if (l >= 1) {
                int c = field_name_to_id(match[1]);
                h.is_date.insert(c);
                continue;
            }


            l = regex_match("^Lookup ([0-9A-Za-z_]+)\\s+\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*([0-9A-Za-z_]+)$", str, match);
            if (l >= 4) {
                int c = field_name_to_id(match[1]);
                AdoLookup look;
                look.db = match[2];
                look.tn = match[3];
                look.n_ret = field_name_to_id(match[4]);
                h.m_lookup[c] = look;
            }


            l = regex_match("^Lookup2 ([0-9A-Za-z_]+)\\s+\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*([0-9A-Za-z_]+)$", str, match);
            if (l >= 5) {
                int c = field_name_to_id(match[1]);
                AdoLookup look;
                look.db = match[2];
                look.tn = match[3];
                look.f_name = match[4];
                look.n_ret = field_name_to_id(match[5]);
                h.m_lookup[c] = look;
            }

            l = regex_match("^Lookup3 ([0-9A-Za-z_]+)\\s+\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*([0-9A-Za-z_]+)\\s*,\\s*\"(.+?)\"\\s*$", str, match);
            if (l >= 5) {
                int c = field_name_to_id(match[1]);
                AdoLookup look;
                look.db = match[2];
                look.tn = match[3];
                look.f_name = match[4];
                look.n_ret = field_name_to_id(match[5]);

                CString s = match[6];
                AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);
                
                h.m_lookup[c] = look;
            }


//            l = regex_match("^LookupQ ([0-9A-Za-z_]+)\\s+\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*([0-9A-Za-z_]+)\\s*,\\s*\"(.+?)\"\\s*$", str, match);
            l = regex_match("^LookupQ ([0-9A-Za-z_]+)\\s+\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*([0-9A-Za-z_]+)$", str, match);
            if (l >= 5) {
                int c = field_name_to_id(match[1]);
                AdoLookup look;
//                look.db = match[2];
//                look.tn = match[3];
//                look.f_name = match[4];
//                look.n_ret = field_name_to_id(match[5]);

                look.db = match[2];
                look.tn = match[3];
                look.q = match[4];
                look.n_ret = field_name_to_id(match[5]);
                h.m_table[c] = look;

//                CString s = match[6];
//                AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);
                
                h.m_lookup[c] = look;
            }




            l = regex_match("^File ([0-9A-Za-z_]+)\\s+\"([^\"]+)\",\\s*\"([^\"]+)\",\\s*\"([^\"]+)\"", str, match);
            if (l >= 1) {
                int c = field_name_to_id(match[1]);

                AdoFile file;

                file.s1 = match[2];
                file.s2 = match[3];
                file.s3 = match[4];

                h.m_file[c] = file;
            }


            l = regex_match("^FilePattern ([0-9A-Za-z_]+)\\s+\"([^\"]+)\",\\s*\"([^\"]+)\",\\s*\"([^\"]+)\",\\s*\"([^\"]+)\"", str, match);
            if (l >= 1) {
                int c = field_name_to_id(match[1]);

                AdoFile file;

                file.s1 = match[2];
                file.s2 = match[3];
                file.s3 = match[4];
                file.pattern = match[5];

                h.m_file[c] = file;
            }


            l = regex_match("^Table ([0-9A-Za-z_]+)\\s+\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*([0-9A-Za-z_]+)$", str, match);
            if (l >= 4) {
                int c = field_name_to_id(match[1]);
                AdoLookup look;
                look.db = match[2];
                look.tn = match[3];
                look.n_ret = field_name_to_id(match[4]);
                h.m_table[c] = look;
            }

            l = regex_match("^TableQ ([0-9A-Za-z_]+)\\s+\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*([0-9A-Za-z_]+)$", str, match);
            if (l >= 5) {
                int c = field_name_to_id(match[1]);
                AdoLookup look;
                look.db = match[2];
                look.tn = match[3];
                look.q = match[4];
                look.n_ret = field_name_to_id(match[5]);
                h.m_table[c] = look;
            }


            l = regex_match("^(nolookup).*", str, match);
            if (l >= 1) {
                nolookup = true;
            }
        }

        fclose(f);
        return h.rows > 0;
    }
    return false;
}

CString AdoHead::getRus(CString name)
{
    map<CString, CString>::const_iterator it = map_rus.find(name);
    if (it != map_rus.end()) {
        return it->second;        
    }

    return "";
}

CString AdoHead::getText(int r, int c)
{
  map<int, map<int, CString> >::const_iterator it = map_h.find(r);
  if (it == map_h.end()) return "";

  map<int, CString>::const_iterator it2 = it->second.find(c);
  if (it2 == it->second.end()) return "";

  return it2->second;
}

int AdoHead::getRealValue(int r, int c)
{
  map<int, map<int, int> >::const_iterator it = map_real_value.find(r);
  if (it == map_real_value.end()) return 0;

  map<int, int>::const_iterator it2 = it->second.find(c);
  if (it2 == it->second.end()) return 0;

  return it2->second;
}



bool AdoHead::isEdit(int c)
{
  set<int>::const_iterator it = is_edit.find(c);
  if (it != is_edit.end()) return true;

  return false;
}


bool AdoHead::isDate(int c)
{
  set<int>::const_iterator it = is_date.find(c);
  if (it != is_date.end()) return true;

  return false;
}



const AdoLookup *AdoHead::getLookup(int col)
{
    map<int, AdoLookup>::const_iterator it =  m_lookup.find(col);
    if (it == m_lookup.end()) return NULL;
    return &it->second;
}


AdoFile *AdoHead::getFile(int col)
{
    map<int, AdoFile>::iterator it =  m_file.find(col);
    if (it == m_file.end()) return NULL;
    return &it->second;
}


AdoDocs *AdoHead::getDocs(int col)
{
    map<int, AdoDocs>::iterator it =  m_docs.find(col);
    if (it == m_docs.end()) return NULL;
    return &it->second;
}



const AdoLookup *AdoHead::getTable(int col)
{
    map<int, AdoLookup>::const_iterator it =  m_table.find(col);
    if (it == m_table.end()) return NULL;
    return &it->second;
}


CString AdoLookup::getQ() const
{
  CString qq = q;

  if (qq == "") {
    qq.Format("SELECT id, %s FROM %s", f_name, tn);
  }
  return qq;
}


#include "TableDialog.h"


void view_documents(CWnd *wnd, CString tab1, CString tab2, CString tn1, int id)
{
    CAdoFile* ado = NULL;

//    Klassif* kls = m_geofile->m_kl_list->findKlN(tn1);
//    if (kls) {
//        ado = kls->m_ado;
//    }
//    else {
        ado = getAdo(getPsAdoName());
//    }

    CString q;
    int n_ret = 1;
    q.Format("SELECT d.id, d.objID, d.remontDocumentTypeID, p.name, date_doc, path FROM %s d"
        " LEFT JOIN %s p ON p.id=d.remontDocumentTypeID "
        " WHERE objID=%d"

        , tab1, tab2, id);

    CTableDialog dlg(wnd, ado->m_type_of_net, ado->m_schema, ado->getConnect(), _TR("Документы"), q, n_ret, virt_data_edit_add_del);

    dlg.SetKey("objID", id);
    dlg.setHead(ado->m_schema, tab1);

    dlg.DoModal();
}
