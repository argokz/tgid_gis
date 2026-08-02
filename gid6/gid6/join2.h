#pragma once

class CAdoFile;

struct CJField
{
  CJField ()
  {
      isList = false;
      yes = false;
  };

  CString q;
  CString name;
  CString name_alias;
  CString table;
  int table_num;
  bool isList;
  bool yes;
};


class Join2
{
public:
  int initJoin(CAdoFile *ado, const char *t1, bool is_q = false, const char *filtr = nullptr);
  int addJoin(CString _q2, CAdoFile *ado, int num1, const char *fid1, const char *t2, const char *fid2, const char *filtr, bool isList, CString _q2init = "");
  int addJoin2(CAdoFile *ado, int num1, const char *fid1, const char *t2, const char *fid2, const char *filtr);

  int allJoin(CAdoFile *ado, const char *t1, int n1);
  void clear();


  CString getQ();
  CString getQ_as();
  CString getQ_shape();
  CString getParam_shape(const char *obj);
  CString getQ_sokr();
  CString getQid(int id);
  CString getQ2();

  CString getTable(int num);
  CString getFieldQ(int num);
  const CJField *getField(int id);
  const CJField *getFieldName(CString name);

private:
  CString from2;
  list<CJField> lst1;

  map<int, CString> map_table;
  map<int, CString> map_q;

  CString m_filtr;

  int m_num1;
  int m_num2;
  bool m_is_q;
};
