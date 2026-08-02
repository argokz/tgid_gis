#pragma once

struct AdoFile
{
    AdoFile()
    {
        s1 = ""; s2 = "", s3 = "", pattern = "";
    }

    CString s1, s2, s3;
    CString pattern;
};


struct AdoLookup
{
  AdoLookup() {
    q = "";
    f_name = "name";
    rus_name = "";
  }

  CString db;
  CString tn;
  CString rus_name;
  CString f_name;
  CString q;
  int n_ret;
  CString getQ() const;
};

struct P4
{
    int c1, c2, r1, r2;
};


struct AdoDocs
{
    CString tab1, tab2, s3;
};


struct AdoHead
{
    AdoHead()
    {
        q1 = ""; q2 = "";
        cols = -1;
    }

    CString q1;
    CString q2;
    
    bool save;
    int rows, cols;
    CString filtr;


    map<CString, int> m_fields;
    map<int, CString> m_fields0;
    

    map<int, AdoDocs> m_docs;
    map<int, AdoFile> m_file;
    map<int, AdoLookup> m_lookup;
    map<int, AdoLookup> m_table;
//    map<int, int> m_file;
    map<int, map<int, CString> > map_h;
    map<int, map<int, int> > map_real_value;

    map<CString, CString> map_rus;

    map<int, int> width;
    map<int, int> height;
    set<int> is_edit;
    set<int> is_date;
    list<P4> list_p4;

    const AdoLookup *getTable(int c);
    const AdoLookup *getLookup(int c);
    AdoFile *getFile(int col);
    AdoDocs *getDocs(int col);
    CString getText(int r, int c);
    CString getRus(CString name);
    int getRealValue(int r, int c);
    bool isEdit(int c);
    bool isDate(int c);
};


bool readTable(const char *db, const char *tn, AdoHead &h);
