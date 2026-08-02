#pragma once

#include "ado.h"

class CAdoFile;

class Join
{
public:

  Join();

  Join(const char *tn, const char *tn2, int id, CAdoFile *ado, int fileID = 0, const char *param = "");
  Join(const char *tn, const char *tn2, int id, const char *param);

//  void set_tn2(const char *tn) {m_tn2 = tn;};

  void addJoin(CString q2, CString fn, CString fn2 = "name", CString q2init = "");

  CString getQ();

  map<CString, CString> map_foreign;
  CString m_tn;
  int m_id;

  bool is_open;


private:
  CString m_from0;   
  CString m_param;
  CString m_tn2 = "";
  int m_n;

  CString make_param(map<int, MySQLColumns> &map_col);
  void init(const char *tn, int id, const char *param);
  void initQ(const char *q, const char *param);
  void init2(CAdoFile *ado, map<int, MySQLColumns> &map_col, int fileID, const char *cx, const char *tn);
  void init2_old(CAdoFile *ado, map<int, MySQLColumns> &map_col, int fileID, const char *cx, const char *tn);
};
