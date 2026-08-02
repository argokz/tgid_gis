#include "stdafx.h"

#include "join.h"
#include "ado.h"
#include "win.h"

void brackets(CString &param, const char *br1, const char *br2)
{
    CString sout = "";

    char * str = new char[param.GetLength() + 1];
    strcpy(str, param);

    char * pch = strtok(str, ", ");
    while (pch != NULL)
    {
        CString s;
        s.Format("%s%s%s", br1, pch, br2);

        if (sout != "") sout += ",";
        sout += s;

        pch = strtok(NULL, _T(", "));
    }
    delete[] str;

    param = sout;
}

CString getLookupName(CString name)
{
  if (name == "uchastok_ms") {
      return "opisanie_uchastka_ms";
  }
  else if (name == "passwords") {
      return "user_password";
  }
  else if (name == "uchastok_rs") {
      return "naimenovanie_uchastka_rs";
  }
  else if (name == "magistrali") {
      return "naimenovanie_magistrali";
  }
  else if (name == "istochniki_tepla") {
      return "naimenovanie";
  }
  else if (name == "standardPumps") {
      return "tip_nas";
  }
  else if (name == "nodes") {
      return "externalNodeName";
  }
  return name;
}

CString fullName(int type_of_net, CString schema, CString table)
{
  CString s = "";

  if (type_of_net == 0) {
    s.Format("[%s].[%s]", schema, table);
  }
  else if (type_of_net == 1) {
    s.Format("[%s].dbo.[%s]", schema, table);
  }
  return s;
}


void Join::init2(CAdoFile *ado, map<int, MySQLColumns> &map_col, int fileID, const char *cx, const char *tn)
{
    ///    ClockMessage m("init2");

    CString qq;

    const map<CString, ForeignTable> *map_lookup = findLookup(cx, tn);
    if (!map_lookup) return;

    map<CString, ForeignTable>::const_iterator it1 = map_lookup->begin();

    for (; it1 != map_lookup->end(); it1++) {

        ForeignTable ft = it1->second;


        //            ado->MySQLTableInfo(ft.cxema, c->strREFERENCED_TABLE_NAME);

        MySQLColumns *c2 = NULL; //  ado->findMySQLColumnsOrdS(ft.cxema, c->strREFERENCED_TABLE_NAME, 2);

        CString nn = getLookupName(ft.table);

        if (nn != "") {
            c2 = ado->findMySQLColumnsS(ft.cxema, ft.table, nn);
        }

        if (nn != "") {
            c2 = ado->findMySQLColumnsS(ft.cxema, ft.table, ft.field);
        }

        if (nn != "") {
            c2 = ado->findMySQLColumnsS(ft.cxema, ft.table, ft.q);
        }

        if (!c2) {
            c2 = ado->findMySQLColumnsS(ft.cxema, ft.table, ft.name);
        }

        if (!c2) {
            c2 = ado->findMySQLColumnsOrdS(ft.cxema, ft.table, 2);
        }

        if (c2) {
            CString col_name = c2->strCOLUMN_NAME;
            qq.Format("SELECT ID, %s FROM %s ORDER BY ID", col_name, fullName(ado->m_type_of_net, ft.cxema, ft.table));
            if (ft.isSort) {
              qq.Format("SELECT ID, %s FROM %s ORDER BY %s", col_name, fullName(ado->m_type_of_net, ft.cxema, ft.table), col_name);
            }


            if (ft.table == "linesobj" && ft.field == "lineid") {
                CString ts = ft.cxema;

                qq.Format("SELECT l.ID, CONCAT(t1.name, ' ', n1.externalNodeName, ' - ',  t2.name, ' ',  n2.externalNodeName) AS name\nFROM %s l "
                    "\nLEFT JOIN %s.nodes n1 ON n1.id=l.nodeID1\nLEFT JOIN %s.nodes n2 ON n2.id=l.nodeID2 "
                    "\nLEFT JOIN externalCodes t1 ON t1.ID=n1.externalCodeID "
                    "\nLEFT JOIN externalCodes t2 ON t2.ID=n2.externalCodeID"
                    ,
                    fullName(ado->m_type_of_net, ts, ft.table), ts, ts);

                col_name = "name";
            }

//            if (ft.table.CompareNoCase("nodes") == 0 && ft.field.CompareNoCase("nodeID") == 0) {
            if (ft.table.CompareNoCase("nodes") == 0) {
                CString ts = ft.cxema;
                qq.Format("SELECT t1.ID, CONCAT(t2.name, ' ', externalNodeName) AS externalNodeName\nFROM %s t1\nLEFT JOIN externalCodes t2 ON t2.ID=t1.externalCodeID", fullName(ado->m_type_of_net, ts, ft.table));
                col_name = "externalNodeName";
            }

            CString ftq = ft.q;

            if (m_tn2 == "") {
                ftq.Replace("$node_type$", "all");
            }
            else {
                ftq.Replace("$node_type$", m_tn2);
            }

            if (fileID) {
                MySQLColumns *cf = ado->findMySQLColumnsS(ft.cxema, ft.table, "fileID");

                if (cf) {
                    qq.Format("SELECT ID, %s FROM %s WHERE fileID=%d ORDER BY ID", c2->strCOLUMN_NAME, fullName(ado->m_type_of_net, ft.cxema, ft.table), fileID);
                    if (ft.isSort) {
                        qq.Format("SELECT ID, %s FROM %s WHERE fileID=%d ORDER BY %s", c2->strCOLUMN_NAME, fullName(ado->m_type_of_net, ft.cxema, ft.table), fileID, c2->strCOLUMN_NAME);
                    }
                    col_name = c2->strCOLUMN_NAME;
                }
                CString sf;
                sf.Format("%d", fileID);

                ftq.Replace("$fragments$", sf);


//                if (ft.table.CompareNoCase("nodes") == 0 && ft.field.CompareNoCase("nodeID") == 0) {
                if (ft.table.CompareNoCase("nodes") == 0) {
                    CString ts = ft.cxema;
//                    qq.Format("SELECT t1.ID, CONCAT(t2.name, ' ', externalNodeName) AS name\nFROM %s t1\nLEFT JOIN externalCodes t2 ON t2.ID=t1.externalCodeID WHERE fileID=%d", fullName(ado->m_type_of_net, ts, ft.table), fileID);

qq.Format(
"SELECT n.ID,\n"
//"CONCAT(IIF(np.id IS NULL, '', CONCAT('(', ecp.name, ' ', np.externalNodeName, ') ')), t2.name, ' ', n.externalNodeName) AS name\n"
"CONCAT(IIF(np.id IS NULL, '', CONCAT('(', ecp.name, ' ', np.externalNodeName, ') ')), t2.name, ' ', n.externalNodeName, ' ', IIF(n.externalSignID = 2, '(П)', IIF(n.externalSignID = 3, '(О)', ''))) AS externalNodeName\n"
"FROM nodes n\n"
"LEFT JOIN externalCodes t2 ON t2.ID = n.externalCodeID\n"
"LEFT JOIN nodes np ON np.id = n.internalNodeID\n"
"LEFT JOIN externalCodes ecp ON ecp.ID = np.externalCodeID\n"

"WHERE n.fileID = %d\n", fileID);

                    col_name = "externalNodeName";
                }
            }
            

            this->addJoin(qq, ft.field, col_name, ftq);
        }
    }
}


#if 0
void Join::init2_old(CAdoFile *ado, map<int, MySQLColumns> &map_col, int fileID, const char *cx, const char *tn)
{
///    ClockMessage m("init2");

    map<int, MySQLColumns>::const_iterator it;

    clock_t t1, t2, t01, t02;

    int dt1 = 0, dt2 = 0, dt3 = 0;

    t1 = clock();

    CString q;

    q.Format(
        "SELECT *\nFROM information_schema.KEY_COLUMN_USAGE k "
        "JOIN information_schema.TABLES t ON k.REFERENCED_TABLE_NAME = t.TABLE_NAME AND k.REFERENCED_TABLE_SCHEMA = t.TABLE_SCHEMA "
        "JOIN information_schema.COLUMNS c ON c.TABLE_NAME = t.TABLE_NAME AND c.TABLE_SCHEMA = t.TABLE_SCHEMA "
        "WHERE k.TABLE_NAME = '%s' AND k.TABLE_SCHEMA = '%s' AND c.COLUMN_NAME = 'name'", cx, tn);

    for (it = map_col.begin(); it != map_col.end(); it++) {
        const MySQLColumns *c = &it->second;

        if (c->strREFERENCED_TABLE_NAME != "") {
            CString qq;

//            ado->MySQLTableInfo(c->strREFERENCED_TABLE_SCHEMA, c->strREFERENCED_TABLE_NAME);

            MySQLColumns *c2 = NULL; //  ado->findMySQLColumnsOrdS(c->strREFERENCED_TABLE_SCHEMA, c->strREFERENCED_TABLE_NAME, 2);

            CString nn = getLookupName(c->strREFERENCED_TABLE_NAME);

            if (nn != "") {
                t01 = clock();
                c2 = ado->findMySQLColumnsS(c->strREFERENCED_TABLE_SCHEMA, c->strREFERENCED_TABLE_NAME, nn);
                t02 = clock();
                dt1 += t02 - t01;
            }

            if (!c2) {
                t01 = clock();
                c2 = ado->findMySQLColumnsOrdS(c->strREFERENCED_TABLE_SCHEMA, c->strREFERENCED_TABLE_NAME, 2);
                t02 = clock();
                dt3 += t02 - t01;
            }

            if (c2) {
                CString col_name = c2->strCOLUMN_NAME;
                qq.Format("SELECT ID, %s\nFROM %s ORDER BY ID", col_name, fullName(ado->m_type_of_net, c->strREFERENCED_TABLE_SCHEMA, c->strREFERENCED_TABLE_NAME));

                if (c->strREFERENCED_TABLE_NAME == "linesobj" && c->strCOLUMN_NAME == "lineID") {
                    CString ts = c->strREFERENCED_TABLE_SCHEMA;

                    qq.Format("SELECT l.ID, CONCAT(t1.name, ' ', n1.externalNodeName, ' - ',  t2.name, ' ',  n2.externalNodeName) AS name\nFROM %s l "
                        "\nLEFT JOIN %s.nodes n1 ON n1.id=l.nodeID1\nLEFT JOIN %s.nodes n2 ON n2.id=l.nodeID2 "
                        "\nLEFT JOIN externalCodes t1 ON t1.ID=n1.externalCodeID "
                        "\nLEFT JOIN externalCodes t2 ON t2.ID=n2.externalCodeID"
                        ,
                        fullName(ado->m_type_of_net, ts, c->strREFERENCED_TABLE_NAME), ts, ts);

                    col_name = "name";
                }

                if (c->strREFERENCED_TABLE_NAME == "nodes" && c->strCOLUMN_NAME == "nodeID") {
                    CString ts = c->strREFERENCED_TABLE_SCHEMA;
                    qq.Format("SELECT t1.ID, CONCAT(t2.name, ' ', externalNodeName) AS name\nFROM %s t1\nLEFT JOIN externalCodes t2 ON t2.ID=t1.externalCodeID", fullName(ado->m_type_of_net, ts, c->strREFERENCED_TABLE_NAME));
                    col_name = "name";
                }

                if (fileID) {
                    MySQLColumns *cf = ado->findMySQLColumnsS(c->strREFERENCED_TABLE_SCHEMA, c->strREFERENCED_TABLE_NAME, "fileID");

                    if (cf) {
                        qq.Format("SELECT ID, %s\nFROM %s WHERE fileID=%d ORDER BY ID", c2->strCOLUMN_NAME, fullName(ado->m_type_of_net, c->strREFERENCED_TABLE_SCHEMA, c->strREFERENCED_TABLE_NAME), fileID);
                        col_name = c2->strCOLUMN_NAME;
                    }
                }
                this->addJoin(qq, c->strCOLUMN_NAME, col_name);
            }
        }
    }

    t2 = clock();

    CString str;

    str.Format("%g секунд\n%g %g", (double)(t2-t1) / CLOCKS_PER_SEC
        , (double)dt1 / CLOCKS_PER_SEC
        , (double)dt3 / CLOCKS_PER_SEC
    );
//    AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
}
#endif

CString Join::make_param(map<int, MySQLColumns> &map_col)
{
    CString param = "";

    for (const auto it : map_col) {
        const MySQLColumns* c = &it.second;

        CString col_name = c->strCOLUMN_NAME;

        if (col_name.CompareNoCase("id") && col_name.CompareNoCase("lineid")) {
            //        if (col_name != "id") {
            if (param != "") param += ",";
            param += col_name;
        }
    }
    return param;
}


Join::Join(const char *tn, const char *tn2, int id, CAdoFile *ado, int fileID, const char *param1)
{
//  ado->MySQLTableInfo(tn);

  m_tn2 = tn2;

  map<int, MySQLColumns> map_col;

  if (ado->getMapColumn(ado->m_schema, tn, map_col)) {

    CString param = param1;

    if (param1 == "")  {
        param = make_param(map_col);
    }


    init(tn, id, param);
    init2(ado, map_col, fileID, ado->m_schema, tn);

    is_open = true;
  }
  else {
    is_open = false;
  }
}


void Join::init(const char *tn, int id, const char *param)
{
  CString q1;

  is_open = true;;

  m_tn = tn;
  m_id = id;
  
  if (id > 0) {
      q1.Format("SELECT id, %s\nFROM %s WHERE id=%d", param, tn, id);
  }
  else {
      q1.Format("SELECT id, %s\nFROM %s", param, tn);
  }
  map_foreign.clear();

  m_param = param;
//  m_param.MakeLower();

  m_from0.Format("FROM (%s\n) _t1_", q1);
  brackets(m_param, "[", "]");
  m_n = 2;
}


/*
void Join::initQ(const char *q, const char *param)
{
  CString q1;

  is_open = true;;

  m_tn = tn;
  m_id = id;
  
  q1.Format("SELECT id, %s\nFROM %s WHERE id=%d", param, tn, id);
  map_foreign.clear();

  m_param = param;

  m_from0.Format("FROM (%s\n) _t1_", q1);
  brackets(m_param, "[", "]");
  m_n = 2;
}
*/



Join::Join(const char *tn, const char *tn2, int id, const char *param)
{
    m_tn2 = tn2;
    init(tn, id, param);
}

int regex_match(const char *p, const char *buf, vector<CString> & match);


CString stripOrderBy(CString q)
{
    vector<CString> match;

    q.Replace("\n", " ");

    int l = regex_match("^(.+)\\s+ORDER\\s+BY\\s+[A-Za-z0-9_]+$", q, match);

    if (l > 1) {
        return match[1];
    }
    return q;
}

CString isprQ(CString q, int id);


void Join::addJoin(CString _q2, CString fn, CString name, CString _q2init)
{
    _q2init = isprQ(_q2init, m_id);

    CString q2 = stripOrderBy(_q2);

    int nn = m_n;

    CString p1, p2;
    p1.Format("[%s]", fn);
    //  p2.Format("[%s], [_t%d_.%s] AS [___%s]", fn, nn, name, fn);
    p2.Format("[%s], [_t%d_.%s] AS [[___%s]]", fn, nn, name, fn);

    if (m_param.Find(p1) != -1) {
        m_param.Replace(p1, p2);

        CString m_from2;

        m_from2.Format("\nLEFT JOIN (%s) _t%d_ ON _t1_.%s=_t%d_.id", q2, nn, fn, nn);

        if (_q2init == "") {
            map_foreign[fn] = _q2;
        }
        else {
            map_foreign[fn] = _q2init;
            m_from2.Format("\nLEFT JOIN (%s) _t%d_ ON _t1_.%s=_t%d_.id", _q2init, nn, fn, nn);
        }

        m_from0 += m_from2;
        m_n++;
    }
}

CString Join::getQ()
{

  CString q;
  bool br = false;

  CString s = 0;
  CString m_param2 = "";

  m_param.Replace("[[", "");
  m_param.Replace("]]", "");

  for (int i = 0; i < m_param.GetLength(); i++) {
      char c = m_param[i];
      if (c == '[') {
          br = true;
          s = "";
      }
      else if (c == ']') {
          br = false;
          if (s.Find(".") != -1) {
              m_param2 += s;
          }
          else {
              m_param2 += "_t1_.";
              m_param2 += s;
          }
      }
      else {
          if (br) {
              s += c;
          }
          else {
              m_param2 += c;
          }
      }

  }

  m_param.Replace("[", "");
  m_param.Replace("]", "");

/*

  m_param.Replace(",name,", ",_t1_.name,");
  m_param.Replace(",specExpendID,", ",_t1_.specExpendID,");
  m_param.Replace(",calcTemperatureID,", ",_t1_.calcTemperatureID,");
  m_param.Replace(",gvsLoadGraphID,", ",_t1_.gvsLoadGraphID,");
  m_param.Replace(",internalNodeID,", ",_t1_.internalNodeID,");
  m_param.Replace(",externalCodeID,", ",_t1_.externalCodeID,");

  if (m_param.Find("internalNodeID,") == 0)
        m_param.Replace("internalNodeID,", "_t1_.internalNodeID,");
*/


  q.Format("SELECT _t1_.id, %s %s", m_param2, m_from0);

  return q;
}
