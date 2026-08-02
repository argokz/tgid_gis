#include "stdafx.h"
#include "join2.h"
#include "ado.h"

bool isInFiltr(const char* p, const char* bfiltr)
{
    if (!bfiltr || !bfiltr[0]) return false;
    
    CString ff = bfiltr, fff;
    ff.MakeLower();
    fff.Format("[%s]", p);
    fff.MakeLower();

    if (ff.Find(fff) != -1) {
        return true;
    }
    return false;
}


CString get_param(list<CJField> &lst)
{
    CString s;

    for (const CJField &f : lst) {
        if (f.name != "id" && f.name != "shape") {
            CString ss;
            ss.Format("\n_TAB%d_.[%s]", f.table_num, f.name);

            if (s != "") s += ",";
            s += ss;
        }
    }

    return s;
}



CString get_param_as(list<CJField> &lst)
{
    CString s;

    for (const CJField &f : lst) {

        if (f.name != "id" && f.name != "shape") {
            CString ss;
            ss.Format("\n_TAB%d_.[%s] AS _TAB%d____%s", f.table_num, f.name, f.table_num, f.name);

            if (s != "") s += ",";
            s += ss;
        }
    }

    return s;
}


CString get_param_as2(list<CJField> &lst, const char *obj)
{
    CString s;

    for (const CJField &f : lst) {
        if (f.name != "id" && f.name != "shape") {
            CString ss;
            ss.Format("\n%s._TAB%d____%s", obj, f.table_num, f.name, f.table_num, f.name);

            if (s != "") s += ",";
            s += ss;
        }
    }

    return s;
}




CString get_param_sokr(list<CJField> &lst)
{
    CString s;
    CJField f1;

    list<CJField>::const_iterator it = lst.begin(), it2;

    for (; it != lst.end(); it++) {
        CJField f = *it, 
        f2 = f;
        it2 = it; it2 ++;
        
        if (it2 != lst.end()) {
            f2 = *it2;
        }

        if (f2.name != f.name && f2.table_num > f.table_num) {
        }
        else {
            if (f.name != "id" && f.name != "shape") {
                CString ss;
                ss.Format("\n_TAB%d_.[%s]", f.table_num, f.name);

                if (s != "") s += ",";
                s += ss;
            }
        }
        f1 = f;
    }

    return s;
}



CString get_param2(list<CJField> &lst)
{
    CString s;

    for (const CJField &f : lst) {

        if (f.name != "id") {
            CString ss;
            ss.Format("\n_TAB%d_.[%s]", f.table_num, f.name);

            if (s != "") s += ",";
            s += ss;
        }
    }

    return s;
}

const CJField *Join2::getField(int id)
{
    int i = 0;

    for (const CJField &it: lst1) {
        if (i == id) return &it;
        i++;
    }
    return NULL;
}

int regex_match(const char* p, const char* buf, vector<CString>& match);


const CJField* Join2::getFieldName(CString name)
{
    for (const CJField& it : lst1) {
        CString ss;
        ss.Format("\n_TAB%d____%s", it.table_num, it.name);

        if (ss.CollateNoCase(name) == 0) {
            return &it;
        }
    }

    CString name1 = name;

    vector<CString> match;
    int l = regex_match("_TAB[0-9]+____(.+)$", name, match);

    if (l > 0) {
        name1 = match[1];
    }

    for (const CJField& it : lst1) {
        if (it.name.CollateNoCase(name1) == 0) {
            return &it;
        }
        if (it.name.CollateNoCase(name) == 0) {
            return &it;
        }
    }
    return NULL;
}


void splitTableName(const CString schema0, const CString tn0, CString &schema, CString &tn);


bool getParamList(CAdoFile *ado, const char *tn, int num, list<CJField> &lst, const char *bfiltr)
{
    CString schema2, tn2;

    splitTableName(ado->m_schema, tn, schema2, tn2);

//    ado->MySQLTableInfo(tn);
    int i, n = ado->NColsMySQL_S(schema2, tn2);

    for (i = 0; i < n; i++) {
        MySQLColumns *col = ado->findMySQLColumnsOrdS(schema2, tn2, i + 1);
        if (col) {
            if (!isInFiltr(col->strCOLUMN_NAME, bfiltr)) {
                CJField f;
                f.name = col->strCOLUMN_NAME;
                f.table = tn;
                f.table_num = num;

                lst.push_back(f);
            }
        }
    }
    CString param = get_param(lst);

    return lst.size() > 0;
}

int Join2::initJoin(CAdoFile *ado, const char *t1, bool is_q, const char *bfiltr)
{
    m_is_q = is_q;
    m_num1 = 1;
    from2 = "";
    m_filtr = "";

    lst1.clear();

    map_table.clear();
    map_q.clear();

    
    if (bfiltr) m_filtr = bfiltr;

    getParamList(ado, t1, m_num1, lst1, bfiltr);

    from2.Format("\n[%s] _TAB%d_ ", t1, m_num1);

    map_table[m_num1] = t1;

    m_num2 = m_num1;

    return m_num1;
}

CString getParFragments();


CString isprQ(CString q, int id)
{
    q.Replace("$fragments$", getParFragments());

//    if (id > 0) {
        CString s;
        s.Format("%d", id);
        q.Replace("$id$", s);
//    }
    
    return q;
}


int Join2::addJoin(CString _q2, CAdoFile *ado, int num1, const char *fid1, const char *t2, const char *fid2, const char *filtr, bool isList, CString _q2init)
{
    if (num1 == 0) return 0;

    _q2init = isprQ(_q2init, -1);

    CString tn1 = getTable(num1);

    CString schema2, tn2;

    splitTableName(ado->m_schema, tn1, schema2, tn2);

    if (!ado->findMySQLColumnsS(schema2, tn2, fid1)) {
        return 0;
    }

    m_num2++;

    map_table[m_num2] = t2;
//    map_q[m_num2] = _q2;

    map_q[m_num2] = _q2init == "" ? _q2 : _q2init;

    list<CJField> lst2, lst3;
    if (!getParamList(ado, t2, m_num2, lst2, m_filtr)) return 0;

    list<CJField>::const_iterator it = lst1.begin();

    for (; it != lst1.end(); it++) {
        CJField f = *it;

        if (f.name == fid1 && f.table_num == num1 && isList) {
            f.q = _q2init == "" ? _q2 : _q2init;
            f.isList = true;
        }

        if (f.name != fid1 || f.table_num != num1 || isList) {
            lst3.push_back(f);
        }

        if (f.name == fid1 && f.table_num == num1) {

            list<CJField>::const_iterator it2 = lst2.begin();
            for (; it2 != lst2.end(); it2++) {
                CJField f2 = *it2;
                if (filtr) {
                    if (isInFiltr(f2.name, filtr)) {
                        lst3.push_back(f2);
                    }
                }
                else {
                    lst3.push_back(f2);
                }
            }
        }
    }

    CString fr;

    if (ado->m_type_of_net == TYPE_OF_NET_MSSQL) {
        fr.Format("\nLEFT JOIN %s _TAB%d_ ON _TAB%d_.[%s]=_TAB%d_.[%s]", t2, m_num2, num1, fid1, m_num2, fid2);
        if (m_is_q && _q2init != "") {
            fr.Format("\nLEFT JOIN (%s) _TAB%d_ ON _TAB%d_.[%s]=_TAB%d_.[%s]", _q2init, m_num2, num1, fid1, m_num2, fid2);
        }

    }
    else {
        fr.Format("\nLEFT JOIN %s _TAB%d_ ON _TAB%d_.[%s]=_TAB%d_.[%s]", t2, m_num2, num1, fid1, m_num2, fid2);
    }

    from2 += fr;

    lst1 = lst3;

    return m_num2;
}

int Join2::addJoin2(CAdoFile *ado, int num1, const char *fid1, const char *t2, const char *fid2, const char *filtr)
{
    m_num2++;

    map_table[m_num2] = t2;

    list<CJField> lst2;
    getParamList(ado, t2, m_num2, lst2, m_filtr);

    list<CJField>::const_iterator it2 = lst2.begin();
    for (; it2 != lst2.end(); it2++) {
        CJField f2 = *it2;
        if (filtr) {
            if (isInFiltr(f2.name, filtr)) {
                lst1.push_back(f2);
            }
        }
        else {
            lst1.push_back(f2);
        }
    }

    CString fr;
    fr.Format("\nLEFT JOIN %s _TAB%d_ ON _TAB%d_.[%s]=_TAB%d_.[%s]", t2, m_num2, num1, fid1, m_num2, fid2);

    from2 += fr;

    return m_num2;
}

CString Join2::getQ()
{
    CString q, param = get_param(lst1);
    q.Format("SELECT _TAB%d_.id AS id, %s\nFROM %s", m_num1, param, from2);

    return q;
}


CString Join2::getQ_as()
{
    CString q, param = get_param_as(lst1);
    q.Format("SELECT _TAB%d_.id AS id, %s\nFROM %s", m_num1, param, from2);

    return q;
}

CString Join2::getQ_shape()
{
    CString q, param = get_param_as(lst1);
    q.Format("SELECT _TAB%d_.id AS id, %s, _TAB1_.shape AS shape\nFROM %s", m_num1, param, from2);

    return q;
}

CString Join2::getParam_shape(const char *obj)
{
    return get_param_as2(lst1, obj);
}





CString Join2::getQ_sokr()
{
    CString q, param = get_param_sokr(lst1);
    q.Format("SELECT _TAB%d_.id AS id, %s\nFROM %s", m_num1, param, from2);

    return q;
}


CString Join2::getQid(int id)
{
    CString q, param = get_param2(lst1);
    if (id == -1) {
        q.Format("SELECT _TAB%d_.id AS id, %s\nFROM %s", m_num1, param, from2, m_num1);
    }
    else {
        q.Format("SELECT _TAB%d_.id AS id, %s\nFROM %s\nWHERE _TAB%d_.id=%d", m_num1, param, from2, m_num1, id);
    }



    return q;
}

CString Join2::getQ2()
{
    CString q, param = get_param(lst1);
    q.Format("SELECT _TAB%d_.id AS id, %s\nFROM %s\nWHERE _TAB%d_.id IN ($P$)", m_num1, param, from2, m_num1);

    return q;
}

CString Join2::getTable(int num)
{
    map<int, CString>::const_iterator it = map_table.find(num);

    if (it != map_table.end()) {
        return it->second;
    }
    return "";
}

CString Join2::getFieldQ(int num)
{
    map<int, CString>::const_iterator it = map_q.find(num);

    if (it != map_q.end()) {
        return it->second;
    }
    return "";
}


CString getLookupName(CString name);
CString fullName(int type_of_net, CString schema, CString table);




int Join2::allJoin(CAdoFile *ado, const char *t1, int n1)
{
    ///    ClockMessage m("init2");

    CString qq;
    int fileID = 0;

    const map<CString, ForeignTable> *map_lookup = findLookup(ado->m_schema, t1);
    if (!map_lookup) return 0;

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
            qq.Format("SELECT ID, %s AS name\nFROM %s ORDER BY ID", col_name, fullName(ado->m_type_of_net, ft.cxema, ft.table));
            if (ft.isSort) {
              qq.Format("SELECT ID, %s AS name\nFROM %s ORDER BY %s", col_name, fullName(ado->m_type_of_net, ft.cxema, ft.table), col_name);
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
                qq.Format("SELECT t1.ID, CONCAT(t2.name, ' ', externalNodeName) AS name\nFROM %s t1\nLEFT JOIN externalCodes t2 ON t2.ID=t1.externalCodeID", fullName(ado->m_type_of_net, ts, ft.table));
                col_name = "name";
            }

            if (fileID) {
                MySQLColumns *cf = ado->findMySQLColumnsS(ft.cxema, ft.table, "fileID");

                if (cf) {
                    qq.Format("SELECT ID, %s\nFROM %s AS name\nWHERE fileID=%d ORDER BY ID", c2->strCOLUMN_NAME, fullName(ado->m_type_of_net, ft.cxema, ft.table), fileID);
                    if (ft.isSort) {
                        qq.Format("SELECT ID, %s AS name\nFROM %s\nWHERE fileID=%d ORDER BY %s", c2->strCOLUMN_NAME, fullName(ado->m_type_of_net, ft.cxema, ft.table), fileID, c2->strCOLUMN_NAME);
                    }

                    col_name = c2->strCOLUMN_NAME;
                }

                if (ft.table.CompareNoCase("nodes") == 0) {
//              if (ft.table.CompareNoCase("nodes") == 0 && ft.field.CompareNoCase("nodeID") == 0) {
                    CString ts = ft.cxema;
//                    qq.Format("SELECT t1.ID, CONCAT(t2.name, ' ', externalNodeName) AS name\nFROM %s t1\nLEFT JOIN externalCodes t2 ON t2.ID=t1.externalCodeID\nWHERE fileID=%d", fullName(ado->m_type_of_net, ts, ft.table), fileID);

qq.Format(
"SELECT n.ID,\n"
"CONCAT(IIF(np.id IS NULL, '', CONCAT('(', ecp.name, ' ', np.externalNodeName, ') ')), t2.name, ' ', n.externalNodeName) AS name\n"
"FROM nodes n\n"
"LEFT JOIN externalCodes t2 ON t2.ID = n.externalCodeID\n"
"LEFT JOIN nodes np ON np.id = n.internalNodeID\n"
"LEFT JOIN externalCodes ecp ON ecp.ID = np.externalCodeID\n"

"WHERE n.fileID = %d\n", fileID);

                    col_name = "name";
                }
            }
            //            this->addJoin(qq, ft.field, col_name);
            CString filtr;
            filtr.Format("[%s]", c2->strCOLUMN_NAME);
//            filtr.Format("[%s]", col_name);


            if (ado->m_type_of_net == TYPE_OF_NET_MSSQL) {
                addJoin(qq, ado, n1, ft.field, ft.table, ft.id, filtr, true, ft.q);
            }
            else {
                addJoin(qq, ado, n1, ft.field, fullName(ado->m_type_of_net, ft.cxema, ft.table), ft.id, filtr, true, ft.q);
            }

        }
    }
    return 0;
}


