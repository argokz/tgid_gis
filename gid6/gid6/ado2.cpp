#include "stdafx.h"
#include "stdafx.h"
#include "crack.h"

#include <oledb.h>
#include <stdio.h>
#include <conio.h>
#include "icrsint.h"

#include "gid6.h"
#include "graph2.h"

#include "resource.h"
#include "InputDia.h"

#include "ado.h"
#include "json.h"

#include "GeoFile.h"
#include "Klassif.h"

#include "gidrView.h"

#include "electro.h"

int regex_match(const char* p, const char* buf, vector<CString>& match);


#define NEW_DIALOG -1
#define FIND_DIALOG -2

int m_userRemoved = 1;

static CString adoError = "";

void readCxema(const char *schemeNum, CString &otop, CString &gv, CString &vent, CString &teh, CString &cond, CString &rez, CString &gvs_all, bool &isEl)
{
    if (!schemeNum) return;

    int n1, n2;

//    bool 
    isEl = false;
    otop = ""; // зависимая

    if (sscanf(schemeNum, "%d.%d", &n1, &n2) == 2) {
        isEl = false;
        if (n2 == 1 || n2 == 5 || n2 == 6 || n2 == 7 || n2 == 8 || !strcmp(schemeNum, "1.3") || !strcmp(schemeNum, "1.4") || !strcmp(schemeNum, "1.1")) {
            otop = "z"; // Зависимая
            isEl = true; // Элеваторная
        }

        if (n2 == 3 || n2 == 4 || n2 == 2) {
            otop = "z"; // Зависимая
            isEl = false; // Безэлеваторная
        }


        if (n2 == 9 || n2 == 10 || n2 == 11 || n2 == 12 || !strcmp(schemeNum, "1.5") || !strcmp(schemeNum, "1.6")) {
            otop = "n"; // Независимая
            isEl = false;
        }

        gv = "";

        switch(n1) {
        case 5: gv = "ps"; break;
        case 4: gv = "sm"; break;
        case 3: gv = "pr"; break;
        case 15: gv = "pw"; break;
        case 6: gv = "op"; break;
        case 7: gv = "oo"; break;
        }

        if (gv != "") gvs_all = "1";

        vent = "0";

        if (n1 > 2 && (n2 == 3 || n2 == 4 || n2 == 7 || n2 == 8 || n2 == 11 || n2 == 12 ) || n1 == 1 && (n2 == 2 || n2 == 4 || n2 == 6) || n1 == 2 && n2 == 1) {
            vent = "1";
        }

        rez = "0";
        if (n1 > 2 && n2%2 == 0) {
            rez= "1";
        }


//        teh = "1";
//        cond = "1";
    }
}




// возвращает дополнительный capt для внутренних узлов
CString get_capt2(CNode2 *node)
{
    CString capt = "";

    if (!node) return "";

    if (node->node.internalNodeID != 0) {
        CNode2 *nn = node->node.m_graph->find(node->node.internalNodeID);
        if (nn) {
            capt += " [Узел присоединения ";
            capt += " ";
            capt += nn->getTable();
            capt += " ";
            capt += nn->getName();
            capt += "]";
        }
    }
    return capt;
}


CString getAdoError()
{
    return adoError;
}

bool ado_execute(CAdoFile *ado, const char *q, long * affected)
{
    if (!ado || !ado->isOpen()) return false;

    bool ret = false;

//    CString s = q;
//    CInputDialog inD(AfxGetMainWnd(), "", "?", s);
//    inD.DoModal();

    ret = ado->Execute(q, affected);
    if (!ret) {
        adoError = ado->getError();
    }

    return ret;
}

long ado_executeInsert(CAdoFile *ado, const char *q, long * affected)
{
    if (!ado || !ado->isOpen()) return false;

    long id = ado->ExecuteInsert(q, affected);
    return id;
}

void saveOperator(CAdoFile *ado)
{
    ado->write("operatorID", (long)m_userRemoved);
    COleDateTime v = COleDateTime::GetCurrentTime();
    ado->write("archiveChangeDate", v);
}


long addNodeUS(CAdoFile *ado, const char *tn, const char *name, int fileID, int externalSignID, int internalNodeID, int externalCodeID, CFPoint pt)
{
    CString q;

    ado->AddNew(tn);
    ado->write("x", pt.x);
    ado->write("y", pt.y);
    ado->write("fileID", (long)fileID);

    ado->write("externalNodeName", name);

    if (!internalNodeID) {
        ado->write("nodeTypeID", PS_NEUST);
    }

    if (internalNodeID) ado->write("internalNodeID", (long)internalNodeID);

    ado->write("externalSignID", (long)externalSignID);

    saveOperator(ado);

    if (externalCodeID) ado->write("externalCodeID", (long)externalCodeID);

    int id = ado->Update();

    reset_shape_node(ado, id);

    return id;

/*
    if (internalNodeID) {
        q.Format("INSERT INTO %s (x,y,fileID,internalNodeID,externalSignID, externalCodeID) VALUES (%lf,%lf,%d,%d,%d)", tn, pt.x, pt.y, fileID, internalNodeID,externalSignID);
    }
    else {
        q.Format("INSERT INTO %s (x,y,fileID,externalSignID) VALUES (%lf,%lf,%d,%d)", tn, pt.x, pt.y, fileID,externalSignID);
    }
*/

//    return ado_executeInsert(ado, q, &affected);
}

long addNode2(CAdoFile *ado, const char *tn, int id)
{
    long affected;
    CString q;
    q.Format("INSERT INTO %s (nodeID) VALUES (%d)", tn, id);

    return ado_executeInsert(ado, q, &affected);
}


double getLengthLine(CFPoint p1, CFPoint p2, CCoordList &cl, BOOL ismap);


UT_DEF ut_def;
PT_DEF pt_def;

bool write_def(CAdoFile *ado, const char * fn, map <CString, CString> &map_v);



long addLineZD(CAdoFile *ado, int typ, CNode2 *n1, CNode2 *n2, int externalSignLineID, CCoordList &cl, long &idv, double diam)
{
    if (!ado || !ado->isOpen()) return false;
    if (n1->node.fileID != n2->node.fileID) {
        return 0;
    }


    int id1 = n1->id;
    int id2 = n2->id;

    CString tn = "linesobj";
    CString tnV = getLineTableMySQL(typ);

    idv = 0;
    long id = 0;

    CString s_coords = cl.saveStr();

//    CString q;
//    q.Format("INSERT INTO %s (externalSignLineID, nodeID1, nodeID2, coords, operatorID) VALUES (%d,%d,%d,'%s',%d)", tn, externalSignLineID, id1, id2, s_coords, m_userRemoved);
//    id = ado->ExecuteInsert(q, &affected);

    ado->AddNew(tn);

    ado->write("externalSignLineID", (long)externalSignLineID);
    ado->write("nodeID1", (long)id1);
    ado->write("nodeID2", (long)id2);
    ado->write("coords", s_coords);

/*
    ado->write("operatorID", (long)m_userRemoved);
//    
    COleDateTime v = COleDateTime::GetCurrentTime();
    ado->write("archiveChangeDate", v);
*/
    saveOperator(ado);

    id = ado->Update();

    if (id > 0 && tnV && tnV[0]) {
        reset_shape_line(ado, id);
        
        ado->AddNew(tnV);
        ado->write("lineID", id);
//        ado->write("pipeSectLength", len);
        ado->write("diameterCondit", diam);
        idv = ado->Update();
    }

    return id;
}



long addLine(CAdoFile *ado, int typ, CNode2 *n1, CNode2 *n2, int externalSignLineID, CCoordList &cl, long &idv)
{
    if (!ado || !ado->isOpen()) return false;

    if (n1->node.fileID != n2->node.fileID) {
        return 0;
    }

    int id1 = n1->id;
    int id2 = n2->id;

    CString tn = "linesobj";
    CString tnV = getLineTableMySQL(typ);


    idv = 0;
    long id = 0;

    CString s_coords = cl.saveStr();

//    CString q;
//    q.Format("INSERT INTO %s (externalSignLineID, nodeID1, nodeID2, coords, operatorID) VALUES (%d,%d,%d,'%s',%d)", tn, externalSignLineID, id1, id2, s_coords, m_userRemoved);
//    id = ado->ExecuteInsert(q, &affected);

    ado->AddNew(tn);

    ado->write("externalSignLineID", (long)externalSignLineID);
    ado->write("nodeID1", (long)id1);
    ado->write("nodeID2", (long)id2);
    ado->write("coords", s_coords);

/*
    ado->write("operatorID", (long)m_userRemoved);
    
    COleDateTime v = COleDateTime::GetCurrentTime();
    ado->write("archiveChangeDate", v);
*/
    saveOperator(ado);
    
    id = ado->Update();

    reset_shape_line(ado, id);

    if (id > 0 && tnV && tnV[0]) {
//        q.Format("INSERT INTO %s (lineID) VALUES (%d)", tnV, id);
//        idv = ado->ExecuteInsert(q, &affected);

        ado->AddNew(tnV);
        ado->write("lineID", id);
        if (typ == TIP_UT) {
            BOOL ismap = FALSE;
            
            double len = getLengthLine(n1->node.coord, n2->node.coord, cl, ismap);

            if (n1->node.internalNodeID != 0) len = 1;

            char buf[256];
            sprintf(buf, "%.1f", len);
            len = atof(buf);

            CCxema *cxema = n1->node.m_graph->m_cxema;

/*
            if (cxema) {
                ado->write("diameterInternal", cxema->init_diameterInternal);
                ado->write("diameterCondit", cxema->init_diameterCondit);
                ado->write("diameterExternal", cxema->init_diameterExternal);
                ado->write("wallThickness", cxema->init_wallThickness);
            }
*/

//            ado->write("varCoeffIDflow", ut_def.map_v);
//            ado->write("varCoeffIDret", ut_def.map_v);
//            ado->write("tubingTypeID", ut_def.map_v);

            ado->write("pipeSectLength", len);
        }
        else if (typ == TIP_RD) {
            ado->write("pipelineSignID", 1);
            ado->write("h", 0);
        }
        else if (typ == TIP_BP) {
            ado->write("pipelineSignID", 1);
            ado->write("h", 0);
        }

        idv = ado->Update();
    }

    return id;
}

bool delObject(CAdoFile *ado, const char *tn, int id)
{
    if (!tn || !tn[0]) return true;
    long affected;
    CString q;
    q.Format("DELETE FROM %s WHERE ID=%d", tn, id);
    return ado_execute(ado, q, &affected);
}

int addGeoAdd(CAdoFile *ado, const CString &tn, int id)
{
    COleDateTime v = COleDateTime::GetCurrentTime();
    CString s = dateTimeToString(v);

    long affected;
    CString q;
    q.Format("INSERT INTO changed_object (dateRemoved, userRemoved, changeTypeID, changedID, changedText) VALUES ('%s', %d, %d, %d, '%s')", s, m_userRemoved, CH_T_ADD_GEO, id, tn);
    long id1 = ado->ExecuteInsert(q, &affected);

    return id1;
}

int addGeoDelete(CAdoFile *ado, const CString &tn, int id)
{
    COleDateTime v = COleDateTime::GetCurrentTime();
    CString s = dateTimeToString(v);

    long affected;
    CString q;
    q.Format("INSERT INTO changed_object (dateRemoved, userRemoved, changeTypeID, changedID, changedText) VALUES ('%s', %d, %d, %d, '%s')", s, m_userRemoved, CH_T_DELETE_GEO, id, tn);
    long id1 = ado->ExecuteInsert(q, &affected);

    return id1;
}



//void addEdited(CAdoFile *ado, const char *tn1, int id1, const char *text1, const char *tn2, int id2, const char *text2)
int addEdited(CAdoFile *ado, map<CString, map< int, map<CString, CString> > >  &map_old_val)
{
    long affected;
    COleDateTime v = COleDateTime::GetCurrentTime();
    CString s = dateTimeToString(v);

    CString q = "";

    q += "";

    for (auto& it : map_old_val) {
        if (q != "") q += ",";

        CString tn = it.first;
        CString s1 = "";
        CString s;

        s1.Format("{\"%s\":{", tn);

        for (auto& it2 : it.second) {
            int id = it2.first;

            CString qq = "";

            s.Format("\"id\":%d", id);
            qq += s;

            for (auto& it3 : it2.second) {
                if (qq != "") qq += ",";

                CString col = it3.first;
                CString val = it3.second;
                val.Replace("'", "''");
                val.Replace("\"", "\\\"");

//                s.Format("\"%s\":\"%s\"", col, val);
                s.Format("\"%s\":\"%s\"", col, val);
                qq += s;
            }
            s1 += qq;
        }

        q += s1;
        q += "}}";
    }

    q += "";

//    AfxMessageBox(q);
    q.Format("INSERT INTO changed_object (dateRemoved, userRemoved, changeTypeID, changedID, changedText) VALUES ('%s', %d, %d, %d, '[%s]')", s, m_userRemoved, CH_T_EDIT, -1, q);
    long id1 = ado->ExecuteInsert(q, &affected);

    return id1;

/*


  if (tn1 && tn1[0] && text1 && text1[0]) {
      q.Format("INSERT INTO changed_object (dateRemoved, userRemoved, changeTypeID, changedID, changedText) VALUES ('%s', %d, %d, %d, '%s')", s, m_userRemoved, CH_T_EDIT, id1, text1);
      long id1 = ado->ExecuteInsert(q, &affected);
  }
  if (tn2 && tn2[0] && text2 && text2[0]) {
      q.Format("INSERT INTO changed_object (dateRemoved, userRemoved, changeTypeID, changedID, changedText) VALUES ('%s', %d, %d, %d, '%s')", s, m_userRemoved, CH_T_EDIT, id2, text2);
      long id2 = ado->ExecuteInsert(q, &affected);
  }
*/
}


int addRemoved(CAdoFile *ado, int typeID, int id, const char *text)
{
  long affected;
  COleDateTime v = COleDateTime::GetCurrentTime();
  CString s = dateTimeToString(v);

  CString q;

  if (text && text[0] && id > 0) {
    q.Format("INSERT INTO changed_object (dateRemoved, userRemoved, changeTypeID, changedID, changedText) VALUES ('%s', %d, %d, %d, '%s')", s, m_userRemoved, typeID, id, text);
  }
  else {
    q.Format("INSERT INTO changed_object (dateRemoved, userRemoved, changeTypeID, changedID) VALUES ('%s', %d, %d, %d)", s, m_userRemoved, typeID, id);
  }

  long id1 = ado->ExecuteInsert(q, &affected);

  return id1;
}


bool delLinesobj(CAdoFile *ado, const char *tn, int idP, int idO)
{
    long affected;
    CString q;
//    q.Format("D-ELETE FROM %s WHERE ID=%d OR ID=%d", tn, idP, idO);

    int idRem = addRemoved(ado, CH_T_DELETE_LINE, idP, "");

    q.Format("UPDATE %s SET removed=1, idRemoved=%d WHERE ID=%d OR ID=%d", tn, idRem, idP, idO);

    return ado_execute(ado, q, &affected);
}

bool delNodes(CAdoFile *ado, const char *tn, int id)
{
    if (!tn || !tn[0]) return true;
    long affected;
    CString q;

    //    q.Format("D-ELETE FROM %s WHERE ID=%d", tn, id);

    int idRem = addRemoved(ado, CH_T_DELETE_NODE, id, "");

    q.Format("UPDATE %s SET removed=1, idRemoved=%d WHERE ID=%d", tn, idRem, id);

    return ado_execute(ado, q, &affected);
}




#include "PropertyGrid\test\PropGridDlg.h"

CGridData getDataGrid(CAdoField &f, COleVariant &v, const char *table, long id)
{
    CGridData dg;

    dg.table = table;
    dg.id = id;

    dg.name = f.name;
    dg.label = f.name;
    dg.typ = f.typ;
    dg.DefinedSize = f.DefinedSize;
    dg.NumericScale = f.NumericScale;
    dg.Precision = f.Precision;
    dg.table_num = 0;

    dg.value = v;

    return dg;
}

static map<CString, map<int, map<CString, COleVariant> > > map_table_value;

bool readUndo(CString table, int id)
{
    table.MakeLower();

    map<CString, map<int, map<CString, COleVariant> > >::const_iterator it1 = map_table_value.find(table);
    if (it1 == map_table_value.end()) return false;

    map<int, map<CString, COleVariant> >::const_iterator it2 = it1->second.find(id);
    if (it2 == it1->second.end()) return false;

    map<CString, COleVariant>::const_iterator it = it2->second.begin();
    for (; it != it2->second.end(); it++) {
        CString name = it->first;
        COleVariant val = it->second;

        int qq;
        qq = 1;
    }

    return true;
}



int make_list_table(CAdoFile *ado, list<CGridData> &lst, const char *table, long id, const char *q0 = NULL)
{
    CString str;

    if (!ado || !ado->isOpen()) return 0;

//    ado->MySQLTableInfo(table);

    CString q;
    if (q0 && q0[0]) {
        q = q0;
    }
    else {
        q.Format("SELECT * FROM %s WHERE id=%d", table, id);
    }

    map<CString, COleVariant> mapP;
    list<CAdoField> lstFld;

    OpenX(ado, q, mapP, lstFld);

    list<CAdoField>::const_iterator it = lstFld.begin();

    for (; it != lstFld.end(); it++) {
        CAdoField f = *it;
        COleVariant v;
        
        if (id > 0) {
            v = getV(mapP, f.name);
            if (f.name.Find("__") != 0) {
                CString tn = table;
                CString fn = f.name;
                tn.MakeLower();
                fn.MakeLower();

                map_table_value[tn][id][fn] = v;
            }
        }
        else {
            v.ChangeType(VT_NULL);
        }

        CGridData dg = getDataGrid(f, v, table, id);
        MySQLColumns *col = ado->findMySQLColumns(table, f.name);
        MySQLColumns2 *col2 = ado->findMySQLColumns2(table, f.name);

        if (col) {
            if (col->strCOLUMN_COMMENT != "") {
                dg.label = col->strCOLUMN_COMMENT;
            }
            dg.NumericScale = (short)col->lNUMERIC_SCALE;
        }

        if (col2) {
            if (col2->hlp != "") {
                dg.label = col2->hlp;

                if (col2->stand_razmer != "") {
                    dg.label += ", ";
                    dg.label += col2->stand_razmer;
                }
                dg.help = col2->helpshiftf1;
            }
        }

        if (col) {
            const CColName *col_name = findColumnRusName(col->strTABLE_SCHEMA, table, col->strCOLUMN_NAME);

            if (col->strCOLUMN_COMMENT != "") {
                dg.label = col->strCOLUMN_COMMENT;
                dg.help = col->name_full;
            }
            dg.NumericScale = (short)col->lNUMERIC_SCALE;
        }

        if (f.name.CompareNoCase("id")) {
            lst.push_back(dg);
        }
    }

    return lst.size();
}

class Update
{
public:
    Update(CAdoFile *ado);
    void addValue(const char *_schema, const char *_table, int id, const char *_field, const char *s, const char* s_old);
    bool getValue(const char *_schema, const char *_table, int id, const char *_field, CString &val);
    bool getValue_old(const char *_schema, const char *_table, int id, const char *_field, CString &val);
    void set_list_id(list<int>& list_id);

    void getQ();

private:
    map<CString, map<CString, map<int, map <CString, CString> > > > map_val;
    map<CString, map<CString, map<int, map <CString, CString> > > > map_val_old;
    list<int> m_list_id;
    CAdoFile *m_ado;
};

Update::Update(CAdoFile *ado)
{
    map_val.clear();
    map_val_old.clear();
    m_ado = ado;
}

void Update::addValue(const char *_schema, const char *_table, int id, const char *_field, const char *s, const char* s_old)
{
    CString schema = _schema;
    CString table = _table;
    CString field = _field;

    map_val[schema][table][id][field] = s;
    map_val_old[schema][table][id][field] = s_old;
}

void Update::set_list_id(list<int> & list_id)
{
    m_list_id = list_id;
}



bool Update::getValue(const char *_schema, const char *_table, int id, const char *_field, CString &val)
{
    CString schema = _schema;
    CString table = _table;
    CString field = _field;

    map<CString, map<CString, map<int, map <CString, CString> > > >::const_iterator it1 = map_val.find(schema);
    if (it1 != map_val.end()) {
        map<CString, map<int, map <CString, CString> > >::const_iterator it2 = it1->second.find(table);
        if (it2 != it1->second.end()) {
            map<int, map <CString, CString> >::const_iterator it3 = it2->second.find(id);
            if (it3 != it2->second.end()) {
                map <CString, CString>::const_iterator it4 = it3->second.find(field);
                if ( it4 != it3->second.end()) {
                    val =  it4->second;
                    return true;
                }
            }
        }
    }
    return false;
}

bool Update::getValue_old(const char *_schema, const char *_table, int id, const char *_field, CString &val)
{
    CString schema = _schema;
    CString table = _table;
    CString field = _field;

    map<CString, map<CString, map<int, map <CString, CString> > > >::const_iterator it1 = map_val_old.find(schema);
    if (it1 != map_val_old.end()) {
        map<CString, map<int, map <CString, CString> > >::const_iterator it2 = it1->second.find(table);
        if (it2 != it1->second.end()) {
            map<int, map <CString, CString> >::const_iterator it3 = it2->second.find(id);
            if (it3 != it2->second.end()) {
                map <CString, CString>::const_iterator it4 = it3->second.find(field);
                if ( it4 != it3->second.end()) {
                    val =  it4->second;
                    return true;
                }
            }
        }
    }
    return false;
}




CString fullName(int type_of_net, CString schema, CString table);

CString trim_br(CString s)
{
    if (s != "") {
        if (s[0] == '\'') {
            s = s.Mid(1, s.GetLength() - 2);  // Обработать 'Текст'
        }
        else if (strlen(s) > 2 && s.Left(2) == "N'") {  // Обработать N'Текст'
            s = s.Mid(2, s.GetLength() - 3);
        }
    }

    s.Replace("\\r", "\r");
    s.Replace("\\n", "\n");

    s.Replace("\\\\", "\\");

    return s;
}

CString getString(list<int>& v)
{
    CString par = "";

    for (auto &it : v) {
        CString s;
        s.Format("%d", it);
        if (par != "") par += ",";
        par += s;
    }
    return par;
}


void set_colorID1(int sostoyanie_shurfaID);
void set_colorID2(int naznachenie_vskrID);
int color_open_close(int stateID);



void Update::getQ()
{
//    map<CString, map< int, list<CString> > > list_col;
    map<CString, map< int, map<CString, CString> > >  map_old_val;


    map<CString, map<CString, map<int, map <CString, CString> > > >::const_iterator it1 = map_val.begin();
    for (; it1 != map_val.end(); it1++) {
        CString schema = it1->first;
        map<CString, map<int, map <CString, CString> > >::const_iterator it2 = it1->second.begin();
        for (; it2 != it1->second.end(); it2++) {
            CString table = it2->first;

            map<int, map <CString, CString> >::const_iterator it3 = it2->second.begin();
            for (; it3 != it2->second.end(); it3++) {
                int id = it3->first;


                CString par = "";
                CString p1 = "";
                CString p2 = "";


                map <CString, CString>::const_iterator it4 = it3->second.begin();
                for (; it4 != it3->second.end(); it4++) {
                    CString field = it4->first;
                    MySQLColumns *col = m_ado->findMySQLColumnsS(schema, table, field);
                    if (col) {
                        CString val = it4->second;

                        if (val == "0" && field.CompareNoCase("nodeTypeID") == 0 && table.CompareNoCase("nodes") == 0) val = "NULL";

                        if (par != "") par += ",";
                        CString s;
                        s.Format("%s=%s", field, val);
                        par += s;

                        CString val_old;

                        if (getValue_old(schema, table, id, field, val_old)) {
                            map_old_val[table][id][field] =  val_old;
                        }

                        if (p1 != "") p1 += ",";
                        if (p2 != "") p2 += ",";

                        p1 += field;
                        p2 += val;
                    }
                }

                if (table == "nodes" || table == "linesobj") {
                    CString ss;

                    if (p1 != "") p1 += ",";
                    if (p2 != "") p2 += ",";

                    p1 += "operatorID";

                    ss.Format("%d", (long)m_userRemoved);
                    p2 += ss;

                    p1 += ",";
                    p2 += ",";
                
                    p1 += "archiveChangeDate";

                    COleDateTime v = COleDateTime::GetCurrentTime();
                    ss = v.Format("%Y%m%d");

                    p2 += ss;
                }


//                    if (table == "nodes" || table == "linesobj") {
//                    par, getString(m_list_id)
//                    }





                CString q;

                if (id == -1) {
                    q.Format("INSERT INTO %s (%s) VALUES(%s)", fullName(m_ado->m_type_of_net, schema, table), p1, p2);
                }
                else {
                    if (m_list_id.size() > 0) {
                        q.Format("UPDATE %s SET %s WHERE id IN (%s)", fullName(m_ado->m_type_of_net, schema, table), par, getString(m_list_id));
                    }
                    else {
                        q.Format("UPDATE %s SET %s WHERE id=%d", fullName(m_ado->m_type_of_net, schema, table), par, id);
                    }
                }
                //                AfxMessageBox(q, MB_OK|MB_ICONINFORMATION);
                long affected;
                bool ret = m_ado->Execute(q, &affected);

                if (ret && affected) {
                    Klassif *kls = m_geofile->m_kl_list->findKlN(table);
                    if (kls && (kls->label || kls->label_color || kls->label_color2 || kls->label_color3 || kls->label_stateID)) {
                        CGeoObject *geo = kls->getGeoObjectById(id);
                        if (geo) {
                            if (kls->label) {
                                CString val;
                                if (getValue(schema, table, id, kls->codlabel, val)) {
                                    val = trim_br(val);
                                    geo->text00000 = val;
                                }
                            }
                            if (kls->label_date) {
                                CString val;
                                geo->cl.is_date = false;
                                if (getValue(schema, table, id, kls->codlabel_date, val)) {
                                    vector<CString> match;
                                    int l = regex_match("([0-9]{4})([0-9]{2})([0-9]{2})", val, match);

                                    if (l > 0) {
                                        int y = atoi(match[1]);
                                        int m = atoi(match[2]);
                                        int d = atoi(match[3]);

                                        COleDateTime dt(y, m, d, 0, 0, 0);
                                        geo->cl.is_date = true;
                                        geo->cl.date = dt;
                                    }
                                }
                            }
                            if (kls->label_color) {
                                CString val;
                                if (getValue(schema, table, id, kls->codlabel_color, val)) {
                                    long color = atoi(val);
                                    if (color != 0) {
                                        geo->cl.color = getGeoColor(color);
                                        if (kls->label_color2) {
//                                          set_colorID2(color);
                                          set_colorID1(color);
                                        }
                                    }
                                }
                            }
                            if (kls->label_color2) {
                                CString val;
                                if (getValue(schema, table, id, kls->codlabel_color2, val)) {
                                    long color = atoi(val);
//                                    set_colorID1(color);
                                    set_colorID2(color);
                                    
                                    if (color != 0) {
                                        geo->cl.color2 = getGeoColor(color);
                                    }
                                }
                            }


                            if (kls->label_color3) {
                                CString val;
                                if (getValue(schema, table, id, kls->codlabel_color3, val)) {
                                    int color = atof(val) == 0 ? 56 : 57;
                                    set_colorID2(color);
                                    
                                    if (color != 0) {
                                        geo->cl.color = getGeoColor(color);
                                    }
                                }
                            }



                            if (kls->label_stateID) {
                                CString val;
                                if (getValue(schema, table, id, kls->codlabel_stateID, val)) {
                                    geo->cl.stateID = atoi(val);
                                    geo->cl.color = color_open_close(geo->cl.stateID);
                                }
                            }


                            CGidrView *pView = getView();
                            if (pView) pView->Invalidate();
                        }
                    }
                }

                if (!ret) {
                    AfxMessageBox(CString(_TR("Ошибка 1\n"))+m_ado->getError(), MB_OK|MB_ICONINFORMATION);
                }
                else if (affected == 0) {
//                    CInputDialog inD(AfxGetMainWnd(), _TR("Ошибка обновления данных"), "?", q);
//                    inD.DoModal();
                }
            }
        }
    }

    addEdited(m_ado, map_old_val);
}

bool ado_table_updated(list<CItemChanged> & lst, void *param1, long id)
{
    CPropertyParam *param = (CPropertyParam *)param1;

    if (!param) {
        AfxMessageBox("Ошибка!");
        return true;
    }

    CAdoFile *ado = param->ado;
    CString table = param->table;

    if (!isEditGeo(table, false)) return true;

    bool isNode = false;

    if (table.CompareNoCase("nodes") == 0) {
        isNode = true;
    }

    Update u(ado);

    u.set_list_id(param->m_list_id);


    CString nodePTS1 = "";
    CString nodePTS2 = "";


    for (list<CItemChanged>::iterator it = lst.begin(); it != lst.end(); ++it) {
        CItemChanged ic = *it;

        CString s1 = ic.sql_value;

        if (ic.m_is_lookup) s1.Format("%d", ic.index);
        if (ic.m_undefined) s1 = "NULL";

        CString s2 = ic.sql_value_old;

        if (ic.m_is_lookup) s2.Format("%d", ic.index_old);
        if (ic.m_undefined_old) s2 = "NULL";



        u.addValue(ic.m_schema, ic.m_table, ic.id, ic.m_name, s1, s2);

        CString name = ic.m_name;

        if (isNode) {
            if (name.CompareNoCase("nodeName") == 0) {
                nodePTS1 = s1;
            }
            if (name.CompareNoCase("nodeTypeID") == 0) {
                if (s1 == "0") {
                    s1 = "NULL";
                    it->sql_value = "NULL";
                }
                nodePTS2 = s1;
            }
        }
    }


    if (isNode) {
/*
        if (nodePTS1 == "''" && nodePTS2 != "NULL") {
            AfxMessageBox("Введите наименование узла ПТС", MB_OK|MB_ICONINFORMATION);
            return false;
        }
        if (nodePTS1 != "''" && nodePTS2 == "NULL") {
            AfxMessageBox("Введите конструкцию узла", MB_OK|MB_ICONINFORMATION);
            return false;
        }
*/
    }

    u.getQ();
//    delete param;

    return true;
}


bool ado_table_find(list<CItemChanged> & lst, void *param)
{
    CAdoFile *ado = (CAdoFile *)param;

    Update u(ado);

    for (list<CItemChanged>::iterator it = lst.begin(); it != lst.end(); ++it) {
        CItemChanged ic = *it;

        CString s = ic.sql_value;

        if (ic.m_is_lookup) {
            s.Format("%d", ic.index);
        }
        u.addValue(ic.m_schema, ic.m_table, ic.id, ic.m_name, s, "");
    }

//    u.getQ();

    return true;
}




#include "join.h"
#include "join2.h"


#include "win.h"

CString getRusName(const char *tn);
CString argpath();

//static map <int, CString> map_1;
//static map <int, map <int, CString> > map_2;
//static map <CString, CString> map_3;
static map <CString, CString> map_fun;
static map <CString, CString> map_after;
static map <CString, CString> map_before;
static map <CString, CString> map_validate;

static map <CString, map<int, list<CString> > > map_disable;



CString getFun(CString name)
{
   map<CString, CString>::const_iterator it2 = map_fun.find(name);
   if (it2 != map_fun.end()) {
       return it2->second;
   }
   return "";
}

CString getAfter(CString name)
{
   map<CString, CString>::const_iterator it2 = map_after.find(name);
   if (it2 != map_after.end()) {
       return it2->second;
   }
   return "";
}

CString getValidate(CString name)
{
   map<CString, CString>::const_iterator it2 = map_validate.find(name);
   if (it2 != map_validate.end()) {
       return it2->second;
   }
   return "";
}




FILE *fopen_tab(const char *tab, const char *tn, const char *ext)
{
    FILE *f = NULL;
    CString fn;

    if (m_idMenu == TYP_MENU_PTS) {
        fn.Format("%s%s\\PTS\\%s.%s", argpath(), tab, tn, ext);
        if (!f) {
            f = fopen(fn, "r");
        }
    }

    if (!f) {
      fn.Format("%s%s\\%s.%s", argpath(), tab, tn, ext);
      f = fopen(fn, "r");
    }

    if (!f) {
      fn.Format("%stab\\ps\\%s.%s", argpath(), tn, ext);
      f = fopen(fn, "r");
    }
    if (!f) {
      fn.Format("%stab\\remont\\%s.%s", argpath(), tn, ext);
      f = fopen(fn, "r");
    }

    if (!f) {
      fn.Format("%stab\\Water\\%s.%s", argpath(), tn, ext);
      f = fopen(fn, "r");
    }

    return f;
}



bool readTab(const char *tn, const char *real_tn, int table_num,
  map <int, CString> &map_1, map <int, map <int, CString> > &map_2,  map <CString, CString> &map_3
//  ,list<CGridData> &lst
)
{
    FILE *f = fopen_tab("tab", tn, "txt");

    int n1 = 0;
    int n2 = 0;

    n1 = map_1.size();


    if (f) {
        char str[1024 + 1];

        while (!feof(f)) {
            if (fgets(str, 1024, f) == NULL) break;
            str[1024] = 0;
            trim(str);

            if (str[0] == '-' || str[0] == 0) {
            }
            else if (str[0] != ' ') {
                map_1[n1] = str;
                n1++;
                n2 = 0;
            }
            else {
                ltrim(str);
                CString s2 = str;

                if (str[0] == '$') {
                    CString st = str;

                    for (int i = 0; str[i]; i++) {
                        if (str[i] == ' ') {
                            str[i] = 0;
                            st = str;
                            map_3[st] = &str[i + 1];
                            break;
                        }
                    }
                    map_2[n1-1][n2] = st;
                    n2++;
                }
                else {
                    CString st = CString(real_tn) + "##" + str;
                    st.Format("%d##%s", table_num, str);

                    for (int i = 0; str[i]; i++) {
                        if (str[i] == ' ') {
                            str[i] = 0;
                            st = CString(real_tn) + "##" + str;
                            st.Format("%d##%s", table_num, str);
                            map_3[st] = &str[i + 1];
                            break;
                        }
                    }
                    map_2[n1-1][n2] = st;
                    n2++;
                }
            }
        }
        fclose(f);
        return true;
    }
    return false;
}


bool readFun(const char *tab, const char *tn)
{
    map_fun.clear();

    FILE *f = fopen_tab(tab, tn, "fun");

    if (f) {
        char str[1024 + 1];

        while (!feof(f)) {
            if (fgets(str, 1024, f) == NULL) break;
            str[1024] = 0;
            trim(str);
            ltrim(str);

            CString s2 = str;

            for (int i = 0; str[i]; i++) {
              if (str[i] == ' ') {
                str[i] = 0;
                s2 = &str[i+1];
                map_fun[str] = s2;
                break;
              }
            }
        }
        fclose(f);
        return true;
    }
    return false;
}


bool readAfter(const char *tab, const char *tn)
{
    map_after.clear();

    FILE *f = fopen_tab(tab, tn, "after");

    if (f) {
        char str[1024 + 1];

        while (!feof(f)) {
            if (fgets(str, 1024, f) == NULL) break;
            str[1024] = 0;
            trim(str);
            ltrim(str);

            CString s2 = str;

            for (int i = 0; str[i]; i++) {
              if (str[i] == ' ') {
                str[i] = 0;
                s2 = &str[i+1];
                map_after[str] = s2;
                break;
              }
            }
        }
        fclose(f);
        return true;
    }
    return false;
}

bool readBefore(const char *tab, const char *tn)
{
    map_before.clear();

    FILE *f = fopen_tab(tab, tn, "before");

    if (f) {
        char str[1024 + 1];

        while (!feof(f)) {
            if (fgets(str, 1024, f) == NULL) break;
            str[1024] = 0;
            trim(str);
            ltrim(str);

            CString s2 = str;

            for (int i = 0; str[i]; i++) {
              if (str[i] == ' ') {
                str[i] = 0;
                s2 = &str[i+1];
                map_before[str] = s2;
                break;
              }
            }
        }
        fclose(f);
        return true;
    }
    return false;
}



bool readValidate(const char *tab, const char *tn)
{
    map_validate.clear();

    FILE *f = fopen_tab(tab, tn, "validate");

    if (f) {
        char str[1024 + 1];

        while (!feof(f)) {
            if (fgets(str, 1024, f) == NULL) break;
            str[1024] = 0;
            trim(str);
            ltrim(str);

            CString s2 = str;

            for (int i = 0; str[i]; i++) {
              if (str[i] == ' ') {
                str[i] = 0;
                s2 = &str[i+1];
                map_validate[str] = s2;
                break;
              }
            }
        }
        fclose(f);
        return true;
    }
    return false;
}






int firstSpace(const char *s)
{
    int i = 0;
    for (; s[i] == ' '; i++) {
    }
    return i;
}


bool readDisable(const char *tab, const char *tn)
{
    map_disable.clear();

    FILE *f = fopen_tab(tab, tn, "disable");

    int n_old = -1;
    CString fn = "";
    int id = -1;
    int level = 0;
    int n1 = 0;
    int n2 = -1;
    int n3 = -1;

    if (f) {
        char str[1024 + 1];

        while (!feof(f)) {
            if (fgets(str, 1024, f) == NULL) break;
            str[1024] = 0;

            if (str[0] == '-') continue;

            int n = firstSpace(str);

            trim(str);
            ltrim(str);

            if (str[0]) {
                if (n == 0) {
                    fn = str;
                    level = 0;
                }
                else if (n > 0 && (level == 0 || (level == 1 && n <= n1) || (level == 2 && n < n2))) {
                    id = atoi(str);
                    level = 1;
                    n1 = n;
                }
                else if (n > 0 && ((level == 1 && n > n1) || (level == 2  && n == n2))) {
                    map_disable[fn][id].push_back(str);
                    level = 2;
                    n2 = n;
                }
            }
        }
        fclose(f);
        return true;
    }
    return false;
}

static map <CString, map<CString, list<CString> > > map_vis;


void get_map_vis(map <CString, map<CString, list<CString> > > &_map_vis)
{
    _map_vis = map_vis;
}



bool readVis(const char *tab, const char *tn)
{
//    map_vis.clear();

    FILE *f = fopen_tab(tab, tn, "vis");

    int n_old = -1;
    CString fn = "";
    CString id = "";
    int level = 0;
    int n1 = 0;
    int n2 = -1;
    int n3 = -1;

    if (f) {
        char str[1024 + 1];

        while (!feof(f)) {
            if (fgets(str, 1024, f) == NULL) break;
            str[1024] = 0;

            if (str[0] == '-') continue;

            int n = firstSpace(str);

            trim(str);
            ltrim(str);

            if (str[0]) {
                if (n == 0) {
                    fn = str;
                    level = 0;
                }
                else if (n > 0 && (level == 0 || (level == 1 && n <= n1) || (level == 2 && n < n2))) {
                    id = str;
                    level = 1;
                    n1 = n;
                }
                else if (n > 0 && ((level == 1 && n > n1) || (level == 2  && n == n2))) {
                    map_vis[fn][id].push_back(str);
                    level = 2;
                    n2 = n;
                }
            }
        }
        fclose(f);
        return true;
    }
    return false;
}





bool isNull(COleVariant var) 
{
    CString s = CCrack::strVARIANT2(var);
    if (s == "") return true;

    switch (var.vt) {
    case VT_I2:        return (V_I2(&var) == 0);
    case VT_I4:        return (V_I4(&var) == 0);
    case VT_R4:        return (V_R4(&var) == 0);
    case VT_R8:        return (V_R8(&var) == 0);
    }
    return false;
}

int ado_add_table(CAdoFile *ado, CPropGridDlg *cEditDlg, Join &join, const char *cap, const char *tn_txt, int number, bool readonly)
{

    if (!join.is_open) return 0;

    CString tn = getRusName(join.m_tn);

    int id = join.m_id;
    CString q = join.getQ();

//    q.Replace("LEFT JOIN linesobj _TAB2_ ON _TAB1_.lineID=_TAB2_.id", "LEFT JOIN linesobj _TAB2_ ON _TAB2_.shape.STDistance(_TAB1_.shape) < 1");

//    CInputDialog inD(cEditDlg, _TR(""), _TR(""), q);
//    if (inD.DoModal() == IDOK) {
//    }

    list<CGridData> lst;
    int n = make_list_table(ado, lst, tn, id, q);

    if (!n) return 0;

    CString tnn = tn;
    if (tn_txt && tn_txt[0]) tnn = tn_txt;

    map <int, CString> map_1;
    map <int, map <int, CString> > map_2;
    map <CString, CString> map_3;

    CString schemeNum = "";

    if (!readTab(tnn, tn, 0, map_1, map_2, map_3)) {
        if (cap) 
            map_1[0] = cap;
        else 
            map_1[0] = tn;

        list<CGridData>::const_iterator it = lst.begin();

        int n = 0;
        for (; it != lst.end(); it++) {
            CGridData dg = *it;
//            if (dg.name == "nodeID") continue;

            map<CString, CString>::const_iterator itf = join.map_foreign.find(dg.name);
            map_2[0][n++] = dg.getIdent();
            if (itf != join.map_foreign.end()) {
                it++;
                if (it == lst.end()) break;
            }
        }
    }

    CString otop = "", gv = "";

    for (list<CGridData>::const_iterator it = lst.begin(); it != lst.end(); it++) {
        CGridData dg = *it;

        if (dg.name == "schemeNum") {
            schemeNum = CCrack::strVARIANT2(dg.value);
//            readVis("tab", tn);
        }
    }

    readVis("tab", tn);

    readFun("tab", tn);
    readAfter("tab", tn);
    readBefore("tab", tn);
    readValidate("tab", tn);
    readDisable("tab", tn);


    HITEM hi;
    HSECTION hs;

    map <int, CString>::const_iterator it1 =  map_1.begin();

    for (; it1 != map_1.end(); it1++) {
        CString s1 = it1->second;
        hs = cEditDlg->addSection(s1, number > 0, number++);

        map <int, map <int, CString> >::const_iterator it2 = map_2.find(it1->first);

        if (it2 != map_2.end()) {

            map <int, CString>::const_iterator it3 = it2->second.begin();

            for (; it3 != it2->second.end(); it3++) {
                CString s2 = it3->second;

                if (s2[0] == '$') {
                    map <CString, CString>::const_iterator it3 = map_3.find(s2);
                    if (it3 != map_3.end()) {
//                        s2 = s2.Mid(1);
                        hi = cEditDlg->addButton(hs, it3->second, s2, id);
                        cEditDlg->m_ctrlGrid.SetItemTable(hi, ado->m_schema, tn, id);
                    }
                }
                else {
                    list<CGridData>::const_iterator it = lst.begin();

                    for (; it != lst.end(); it++) {
                        CGridData dg = *it;

                        if (dg.name == "calculationID") continue;

                        if (dg.name == "nodeID" && !_stricmp(tn, "DR_OUT")) continue;

//                        if (dg.name == "nodeID" && _stricmp(tn, "pressRegulators") && _stricmp(tn, "pressDropRegulators") && _stricmp(tn, "bypass")) {
//                            continue;
//                        }
//                        if ((dg.name == "nodeID1" || dg.name == "nodeID2") && _stricmp(tn, "defect")) continue;



                        if (dg.name == "externalSign") continue;

                        map <CString, CString>::const_iterator it3 = map_3.find(dg.getIdent());
                        if (it3 != map_3.end()) {
                            dg.label = it3->second;
                        }

                        if (dg.getIdent() == s2) {
                            map<CString, CString>::const_iterator itf = join.map_foreign.find(dg.name);


                            if (!readonly || !isNull(dg.value)) {
                                if (itf != join.map_foreign.end()) {
                                    COleVariant value = dg.value;
                                    CString name = itf->second;

                                    if (dg.name == "connectID" && dg.table == "connectNodes") {
                                        name.Format("SELECT DISTINCT n2.id, CONCAT(ec.name, ' ', n2.externalNodeName) AS externalNodeName FROM linesobj l JOIN nodes n2 ON(n2.id = l.nodeID1 OR n2.id = l.nodeID2) LEFT JOIN externalCodes ec ON ec.id = n2.externalCodeID JOIN nodes n1 ON(n1.id = l.nodeID1 OR n1.id = l.nodeID2) AND n1.id <> n2.id JOIN nodes nc ON nc.internalNodeID = n1.id JOIN connectNodes cn ON cn.nodeID = nc.id WHERE cn.id = %d", dg.id);
                                    }

                                    it++;
                                    if (it == lst.end()) {
                                        break;
                                    }
                                    CGridData dg1 = *it;
                                    dg1.label = dg.label;
                                    dg1.name = dg.name;
                                    dg1.help = dg.help;

                                    hi = cEditDlg->addDataGrid(hs, dg1);

                                    long id = to_long(dg.value);

                                    cEditDlg->m_ctrlGrid.SetItemLookup(hi, (const char*)name, id);
                                }
                                else {
                                    hi = cEditDlg->addDataGrid(hs, dg);
                                }

                                bool ro = false;
                                bool noedit = false;

                                map<CString, CString>::const_iterator it2 = map_fun.find(dg.name);
                                if (it2 != map_fun.end()) {
                                    if (it2->second == "ReadOnly") {
                                        ro = true;
                                    }
                                    if (it2->second.Find("NoEdit_") != -1) {
                                        noedit = true;
                                    }
                                }

                                if (ro) {
                                    cEditDlg->m_ctrlGrid.SetItemReadOnly(hi);
                                }

                                if (noedit) {
                                    cEditDlg->m_ctrlGrid.SetItemNoEdit(hi);
                                }


                                cEditDlg->m_ctrlGrid.SetItemHelp(hi, dg.help);
                                cEditDlg->m_ctrlGrid.SetItemTable(hi, ado->m_schema, tn, id);

                                if (!ro) {
                                    map<CString, CString>::const_iterator it2 = map_fun.find(dg.name);
                                    if (it2 != map_fun.end()) {
                                        cEditDlg->m_ctrlGrid.SetItemFun(hi, it2->second);
                                    }


                                    auto it4 = map_after.find(dg.name);
                                    if (it4 != map_after.end()) {
                                        cEditDlg->m_ctrlGrid.SetItemAfter(hi, it4->second);
                                    }

                                    {
                                        auto it = map_before.find(dg.name);
                                        if (it != map_before.end()) {
                                            cEditDlg->m_ctrlGrid.SetItemBefore(hi, it->second);
                                        }
                                    }



                                    map<CString, CString>::const_iterator it6 = map_validate.find(dg.name);
                                    if (it6 != map_validate.end()) {
                                        cEditDlg->m_ctrlGrid.SetItemValidate(hi, it6->second);
                                    }

                                }
                                map <CString, map<int, list<CString> > >::const_iterator it3 = map_disable.find(dg.name);
                                if (it3 != map_disable.end()) {
                                    cEditDlg->m_ctrlGrid.SetItemFunDisable(hi, "fun_disable", it3->second, map_disable);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (schemeNum != "") {
        cEditDlg->m_ctrlGrid.initCxema(schemeNum);
    }

    return n;
}

int get_ID2(const char *name, const char *name2);

void sort_list(list<CGridData> &lst, const char *tn)
{
    FILE *f = fopen_tab("tab", tn, "txt");

    list<CGridData> lst2;


    if (f) {

        char str[1024 + 1];

        while (!feof(f)) {
            if (fgetstr(str, 1024, f) == NULL) break;
            str[1024] = 0;

            CString s = str;

            list<CGridData>::const_iterator it = lst.begin();

            for (; it != lst.end(); it++) {
                if (s == it->name || "___" + s == it->name) {
                    lst2.push_back(*it);
                }
            }
        }

        lst = lst2;

        fclose(f);
    }
}


struct PR_list
{
  char *name;
  int id;
  int typ_nagr;
};

int ado_add_table_PR(CAdoFile *ado, CPropGridDlg *cEditDlg, Join &join, const char *schemeNum1, const char *cap)
{

PR_list ss[] = {
{"Основные", 1, 0},
{"Нагрузки", 2, 1},
{"Дроссели", 3, 0},
{"Нагрузки договорные", 2, 2},
{"Нагрузки перспективные", 2, 3},
{"Оборудование", 4, 0},
{"Характерные точки", 5, 0},
{"Здание", 6, 0},
{"Договор", 7, 0},
{"Вывод на схему", 8, 0},
{"Строка заголовка окна", 9, 0},
//{"Остальные 2", 11, 0}
{"Остальные", 10, 0}
};

    int n10 = sizeof(ss)/sizeof(ss[0]);

    
    if (!join.is_open) return 0;

    CString tn = getRusName(join.m_tn);
    
    readVis("tab", tn);

    readFun("tab", tn);
    readAfter("tab", tn);
    readBefore("tab", tn);
    readValidate("tab", tn);
    readDisable("tab", tn);

    int id = join.m_id;
    CString q = join.getQ();

    list<CGridData> lst;
    int n = make_list_table(ado, lst, tn, id, q);

    sort_list(lst, "realConsumers");
    

    //  pm_cEditDlg->Populate(tn, lst);

    if (!n) return 0;

    CString schemeNum = "";
    
    for (list<CGridData>::const_iterator it = lst.begin(); it != lst.end(); it++) {
        CGridData dg = *it;

        if (dg.name == "schemeNum") {
            schemeNum = CCrack::strVARIANT2(dg.value);
            break;
        }
    }

    int number = 0;

    HSECTION hs;

    for (int i = 0; i < n10; i++) {
        hs = NULL;

        list<CGridData>::const_iterator it = lst.begin();

        HITEM hi;

        for (; it != lst.end(); it++) {
            CGridData dg = *it;

//            if (dg.name == "nodeID" && _stricmp(tn, "pressRegulators") && _stricmp(tn, "pressDropRegulators") && _stricmp(tn, "bypass")) {
//                continue;
//            }
//            if ((dg.name == "nodeID1" || dg.name == "nodeID2") && _stricmp(tn, "defect")) continue;

            int ids = get_ID2(schemeNum, dg.name);
            if (dg.name == "schemeNum") ids = 1;
            if (ids == 8) ids = 1;
//            if (ids <= 0) ids = 10;
            if (dg.name == "buildingTypeID") ids = 1;

            bool is_persp = false;
            bool is_cont = false;
            bool is___ = false;

            if (dg.name.Find("__") == 0) is___ = true;
            if (dg.name.Find("persp") == 0) is_persp = true;
            if (dg.name.Find("cont") == 0) is_cont = true;
            if (dg.name == "contractNumber") is_cont = false;

            bool is_nagr = !is_cont && !is_persp;

//            dg.label = dg.name+" "+dg.label;
//            if (ids == ss[i].id && !is_cont && !is_persp && !is___) {

            if (ids == ss[i].id && !is___) {

                if (ss[i].typ_nagr == 1 && !is_nagr) continue;
                if (ss[i].typ_nagr == 2 && !is_cont) continue;
                if (ss[i].typ_nagr == 3 && !is_persp) continue;

                if (hs == NULL) hs = cEditDlg->addSection(_TR(ss[i].name), number > 0, number++);
                
                map<CString, CString>::const_iterator itf = join.map_foreign.find(dg.name);

                if (itf != join.map_foreign.end()) {
                    COleVariant value = dg.value;
                    CString name = itf->second;

                    it++;
                    if (it == lst.end()) {
                        break;
                    }
                    CGridData dg1 = *it;
                    dg1.label = dg.label;
                    dg1.name = dg.name;
                    hi = cEditDlg->addDataGrid(hs, dg1);

                    long id = to_long(dg.value);

                    cEditDlg->m_ctrlGrid.SetItemLookup(hi, (const char*)name, id);
                }
                else {
                    hi = cEditDlg->addDataGrid(hs, dg);
                }

                cEditDlg->m_ctrlGrid.SetItemHelp(hi, dg.help);
                cEditDlg->m_ctrlGrid.SetItemTable(hi, ado->m_schema, tn, id);

                 map<CString, CString>::const_iterator it2 = map_fun.find(dg.name);
                 if (it2 != map_fun.end()) {
                     cEditDlg->m_ctrlGrid.SetItemFun(hi, it2->second);
                 }
                 map <CString, map<int, list<CString> > >::const_iterator it3 = map_disable.find(dg.name);
                 if (it3 != map_disable.end()) {
                     cEditDlg->m_ctrlGrid.SetItemFunDisable(hi, "fun_disable", it3->second, map_disable);
                 }
            }
        }
    }

    return n;
}

CPropGridDlg *getPropGridDlg(CWnd *wnd, CAdoFile *ado, const char *table, const char *capt, bool modal, TYP_OF_P typ_of_p, void *param2, long ID /* = IDD_PROPGRID_DIALOG */, bool autodelete)
{
    CPropertyParam *param = new CPropertyParam;
    param->ado = ado;
    param->table = table;
    param->id = (int) param2;
    param->typ = typ_of_p;
//    param->m_list_id = 


    if (typ_of_p == TP_GEO && autodelete) typ_of_p = TP_AUTODELETE; // автоудаление
    if (typ_of_p == TP_OSMOTR && autodelete) typ_of_p = TP_OSMOTR_AUTODELETE; // автоудаление
    if (typ_of_p == TP_REMONT && autodelete) typ_of_p = TP_REMONT_AUTODELETE; // автоудаление
    if (typ_of_p == TP_OPRES && autodelete) typ_of_p = TP_OPRES_AUTODELETE; // автоудаление
    if (typ_of_p == TP_FAKTORY_OSMOTR && autodelete) typ_of_p = TP_FAKTORY_AUTODELETE; // автоудаление

    CGidrView *pView = getView();
    bool isEd = pView && pView->isEditGid();    
    
    Klassif *kls = m_geofile->m_kl_list->findKlN(table);
    if (kls && isEditGeo(kls->nazv, false)) {
        isEd = true;
    }


    CPropGridDlg *cEditDlg = new CPropGridDlg(wnd, isEd ? ado_table_updated : nullptr, (void*)param, table, capt, typ_of_p, param2);

    if (cEditDlg != NULL) {
        BOOL ret = cEditDlg->Create(ID, wnd);

        if (!ret) {
            delete cEditDlg;
            return NULL;
        }
        cEditDlg->m_property_param = param;
    }
    
    
    return cEditDlg;
}

#include "gidrView.h"

void splitTableName(const CString schema0, const CString tn0, CString &schema, CString &tn);


static map<CString, COleVariant> map_ado_info_value;


void ado_infoAddViewValue(const char *name, const COleVariant &v)
{
    map_ado_info_value[name] = v;
}

bool is_line_field(CString tname2)
{
    if (
        tname2 == "kod1" ||
        tname2 == "name1" ||
        tname2 == "kod2" ||
        tname2 == "name2" ||
        tname2 == "externalSignLineID" ||

        tname2 == "diameterCondit" ||

        tname2 == "diameterExternal" ||
        tname2 == "wallThickness" ||

        tname2 == "tubingTypeID" ||
        tname2 == "firstPICdateHP" ||
        tname2 == "uchastok_eks" ||
        tname2 == "uchastok_ms_rs" ||
        tname2 == "nach_fio" ||
        tname2 == "naimenovanie_magistrali" ||
        tname2 == "naimenovanie_isochnika" ||
        tname2 == "org_name" ||
        tname2 == "im_name"



        ) return true;

    return false;
}

void ispr_q_line(CString &q, CString defect, bool is_point, int id)
{
    CGidrView* pView = getView();
    CString qqq, fragments_nums = pView->m_cxema->getParAll();

    if (!pView) return;

    CString s_n1 = "_TAB5_";
    CString s_n2 = "_TAB6_";

    bool node_in_q = false;

    if (q.Find("nodes _TAB5_") != -1) {
        node_in_q = true;
    }

    if (!node_in_q) {
        s_n1 = "n1";
        s_n2 = "n2";
    }


    CString p1;

    p1.Format(
    "ec1.name AS kod1,\n"
    "%s.externalNodeName AS name1,\n"
    "ec2.name AS kod2,\n"
    "%s.externalNodeName AS name2,\n"
    "esl.name AS externalSignLineID,\n"
    "hps.diameterCondit,\n"

    "hps.diameterExternal,\n"
    "hps.wallThickness,\n"

    "tt.name AS tubingTypeID,\n"
    "hps.firstPICdateHP,\n"

    "ue.nomer_uchastka AS uchastok_eks,\n"
    "ms_rs.name AS uchastok_ms_rs,\n"
    "nach.fio AS nach_fio,\n"
    "mag.naimenovanie_magistrali,\n"
    "ist.naimenovanie AS naimenovanie_isochnika,\n"
    
    "org.name AS org_name,\n"
    "im.name AS im_name,\n", s_n1, s_n2);



    CString p2 = "LEFT JOIN linesobj l ON  l.shape.STDistance(_TAB1_.shape.STPointN(1)) < 0.1\n";

    p2.Format(

"LEFT JOIN linesobj l ON  l.id = \n"
//"--l.shape.STDistance(_TAB1_.shape.STPointN(1)) < 0.1\n"
"(select top 1\n"
"t.lineID as lineID\n"
"from (\n"

"select\n"
"   distinct\n"
"       l.id as lineID,\n"
"       d.id as obj_id,\n"
"       l.shape.STDistance(d.shape) as length,\n"
"       MIN(l.shape.STDistance(d.shape)) OVER(PARTITION BY d.id ) AS \"min_len\"\n"
"from %s d\n"
"JOIN linesobj l ON ( l.removed = 0 and l.shape.STDistance(d.shape) < 0.1 )\n"
"JOIN nodes n ON n.id=l.nodeID1\n"
"WHERE  \n"
"n.fileID IN (%s) AND\n"
"d.id = %d\n"
//"--and n1.fileID in (%s)\n"
")\n"

"t where t.length = t.min_len) and l.shape.STDistance(_TAB1_.shape) < 0.1 \n", defect, fragments_nums, id);


    if (!is_point) {
        p2 = "LEFT JOIN linesobj l ON  l.shape.STDistance(_TAB1_.shape) = 0\n";
    }


    CString p3;


    CString p3_1 = 
        "LEFT JOIN nodes n1 ON n1.id=l.nodeID1\n"
        "LEFT JOIN nodes n2 ON n2.id=l.nodeID2\n";


    p3.Format(

//    "LEFT JOIN nodes n1 ON n1.id=l.nodeID1\n"
//    "LEFT JOIN nodes n2 ON n2.id=l.nodeID2\n"
    "LEFT JOIN externalCodes ec1 ON ec1.id=%s.externalCodeID\n"
    "LEFT JOIN externalCodes ec2 ON ec2.id=%s.externalCodeID\n"
    "LEFT JOIN heatPipeSections hps ON hps.lineID=l.id\n"
    "LEFT JOIN externalSignLine esl ON esl.id=l.externalSignLineID\n"
    "LEFT JOIN tubingTypes tt ON tt.id=hps.tubingTypeID\n"
    "LEFT JOIN \n"
    "(\n"
    "SELECT id, 1 AS typ, nomer_uchastka,opisanie_uchastka_ms AS name FROM uchastok_ms\n"
    "UNION\n"
    "SELECT id, 2 AS typ, nomer_uchastka, naimenovanie_uchastka_rs AS name FROM uchastok_rs\n"
    ") ms_rs ON (ms_rs.typ=1 AND ms_rs.id=hps.magistralSite OR ms_rs.typ=2 AND ms_rs.id=hps.distSite)\n"

    "LEFT JOIN magistrali mag ON mag.id=hps.magistral\n"
    "LEFT JOIN uchastki_ekspluatatsii ue ON ue.id=ms_rs.nomer_uchastka\n"
    "LEFT JOIN nachalniki_uchastkov nach ON nach.id=ue.nachalnik_uchastka\n"
    "LEFT JOIN istochniki_tepla ist ON ist.id=ue.istochnik_tepla\n"


    "LEFT JOIN organizations org ON org.id=l.organizationID\n"
    "left join isolMaterials im on im.id = hps.isolMaterialID\n", s_n1, s_n2);

    if (!node_in_q) {
        p3 = p3_1+p3;
    }



    q.Replace("_TAB1_.id AS id,", "_TAB1_.id AS id," + p1);

    qqq.Format("WHERE %s.fileID IN (%s) AND _TAB1_.id", s_n1, fragments_nums);

    q.Replace("WHERE _TAB1_.id", p2 + p3 + qqq);
}


CPropertyGrid *view_join(CWnd *wnd, CAdoFile *ado, Join2 &j, const char *tn, int id, const char *capt, bool modal, const char *tn_txt, long ID, TYP_OF_P typ_of_p, bool autodelete)
{
    CString str;
    list<CGridData> lst;
    CString q = j.getQid(id);

//    bool is_line = (q.Find("_TAB1_.lineID,") != -1);
    bool is_line = (q.Find("_.lineID,") != -1);

    Klassif *kls = m_geofile->m_kl_list->findKlN(tn);

    CString q_old = q;

    if (kls && is_line) {
        ispr_q_line(q, kls->nazv, isPoint(kls->loc), id);
    }

//    CInputDialog inD(AfxGetMainWnd(), "", "?", q);
//    inD.DoModal();


    CPropGridDlg* pm_cEditDlg = getPropGridDlg(wnd, ado, tn, capt, modal, typ_of_p, (void*)id, ID /*IDD_PROPGRID_DIALOG*/, autodelete);
    if (!pm_cEditDlg) return nullptr;

    pm_cEditDlg->m_ctrlGrid.ResetContents();
    pm_cEditDlg->m_ctrlGrid.RecalcOff();

    map<CString, COleVariant> mapP;
    list<CAdoField> lstFld;

    lstFld.clear();
    lst.clear();

    if (!ado->openTable0(q)) {
        return nullptr;
    }
    if (id != -1 && ado->isEOF()) {
        if (!ado->openTable0(q_old)) {
            return nullptr;
        }
        if (id != -1 && ado->isEOF()) {
            return nullptr;
        }
    }

    int nf = ado->NFlds();

    vector<COleVariant> vv(nf);
    vector<CAdoField> vf(nf);

    HITEM hi;
    HSECTION hs;

    int number = 0;
    int n1 = 0;


    if (kls && is_line) {
        hs = pm_cEditDlg->addSection("Участок трубопровода", false, number++);
        n1++;
    }


    for (long i = 0; i < nf; i++) {
        CAdoField f = ado->Field(i);
        COleVariant v;
        
        if (id != -1) {
            v = ado->read(i);
        }

        map<CString, COleVariant>::const_iterator it = map_ado_info_value.find(f.name);
        if (it != map_ado_info_value.end()) {
            v = it->second;
        }

        vv[i] = v;
        vf[i] = f;

        if (kls && is_line) {
            if (is_line_field(f.name)) {

                const CColName *col_name = findColumnRusName(ado->m_schema, "line2", f.name);
                CGridData dg = getDataGrid(f, v, f.name, id);

                if (col_name) {
                    dg.label = col_name->name;
                    dg.help = col_name->name_full;
                }

                hi = pm_cEditDlg->addDataGrid(hs, dg);
                pm_cEditDlg->m_ctrlGrid.SetItemReadOnly(hi);
            }
        }
    }

    ado->closeTable();

    for (long i = 0; i < nf; i++) {
        COleVariant v = vv[i];
        CAdoField f = vf[i];

        const CJField *jf = j.getFieldName(f.name); 

        if (jf) {
            CString tname = j.getTable(jf->table_num);
            CString fieldQ = j.getFieldQ(jf->table_num);

            CGridData dg = getDataGrid(f, v, tname, id);
            dg.q = jf->q;


            CString schema2, tname2;

            splitTableName(ado->m_schema, tname, schema2, tname2);

            MySQLColumns *col = ado->findMySQLColumnsS(schema2, tname2, f.name);

            if (tname2 == "externalCodes") tname2 = "calcSchemes";

            MySQLColumns2 *col2 = ado->findMySQLColumns2(tname2, f.name);

            if (!col2 && tname2 == "heatSources") {
                col2 = ado->findMySQLColumns2("normTemperaturesGraphs", f.name);
            }

            if (col) {
                if (col->strCOLUMN_COMMENT != "") {
                    dg.label = col->strCOLUMN_COMMENT;
                    dg.help = col->name_full;
                }
                dg.NumericScale = (short)col->lNUMERIC_SCALE;
            }

            if (col2) {
                if (col2->hlp != "") {
                    dg.label = col2->hlp;

                    if (col2->stand_razmer != "") {
                        dg.label += ", ";
                        dg.label += col2->stand_razmer;
                    }

                    dg.help = col2->hlp;
                }

                if (col2->helpshiftf1 != "") {
                    dg.help = col2->helpshiftf1;
                }
            }

            dg.table_num = jf->table_num;
            dg.isList = jf->isList;

            if (f.name.CompareNoCase("id") || dg.isList) {
                lst.push_back(dg);
            }
        }
    }

    CString tnn = tn;
    if (tn_txt && tn_txt[0]) tnn = tn_txt;

    map <int, CString> map_1;
    map <int, map <int, CString> > map_2;
    map <CString, CString> map_3;



    if (!readTab(tnn, tn, 1, map_1, map_2, map_3)) {
        if (capt)
            map_1[n1] = capt;
        else
            map_1[n1] = tn;

        list<CGridData>::const_iterator it = lst.begin();

        int n = 0;
        for (; it != lst.end(); it++) {
            CGridData dg = *it;
            map_2[n1][n++] = dg.getIdent();
            if (dg.isList) {
                it++;
                if (it == lst.end()) break;
            }
        }
    }

    readVis("tab", tnn);

    readFun("tab", tnn);
    readAfter("tab", tnn);
    readBefore("tab", tnn);
    readValidate("tab", tnn);
    readDisable("tab", tnn);

    list<CGridData>::const_iterator it = lst.begin();

    int n = 0;


    map <int, CString>::const_iterator it1 =  map_1.begin();

    CString tn0 = tn;

    for (; it1 != map_1.end(); it1++) {
        CString s1 = it1->second;

        int number0 = number;

        hs = pm_cEditDlg->addSection(s1, false,  number++);

        map <int, map <int, CString> >::const_iterator it2 = map_2.find(it1->first);

        if (it2 != map_2.end()) {

            map <int, CString>::const_iterator it3 = it2->second.begin();

            for (; it3 != it2->second.end(); it3++) {
                CString s2 = it3->second;

                if (s2[0] == '$') {
                    map <CString, CString>::const_iterator it3 = map_3.find(s2);
                    if (it3 != map_3.end()) {
//                        s2 = s2.Mid(1);
                        hi = pm_cEditDlg->addButton(hs, it3->second, s2, id);
                        pm_cEditDlg->m_ctrlGrid.SetItemTable(hi, ado->m_schema, tn, id);
                        number = number0;
                    }
//                    pm_cEditDlg->m_ctrlGrid.SetItemTable(hi, ado->m_schema, tn, id);
                }
                else {
                    list<CGridData>::iterator it = lst.begin();

                    for (; it != lst.end(); it++) {
                        CGridData dg = *it;

//                        if (dg.name == "nodeID" && _stricmp(tn, "pressRegulators") && _stricmp(tn, "pressDropRegulators") && _stricmp(tn, "bypass")) {
//                            continue;
//                        }
//                        if ((dg.name == "nodeID1" || dg.name == "nodeID2") && _stricmp(tn, "defect")) continue;


                        if (dg.getIdent() != s2) continue;
                        if (!dg.isFirst) continue;
                        if (dg.name == "shape") continue;

                        it->isFirst = false;

                        map <CString, CString>::const_iterator it33 = map_3.find(dg.getIdent());
                        if (it33 != map_3.end()) {
                            if (dg.help == "") {
                                dg.help = it33->second;
                            }
                            dg.label = it33->second;
                        }

                        //            hi = pm_cEditDlg->addDataGrid(hs, dg);

                        if (dg.isList) {
                            COleVariant value = dg.value;
                            //                        CString name = dg.name; // !!!!!!!!! Переделать
                            CString name = dg.q;

                            it++;
                            if (it == lst.end()) {
                                break;
                            }
                            CGridData dg1 = *it;

                            MySQLColumns *c2 = ado->findMySQLColumns(dg1.table, "ord");

                            if (name == "") {
                                if (c2) {
                                    name.Format("SELECT ID, %s FROM %s AS name ORDER BY ORD", dg1.name, dg1.table);
                                }
                                else {
                                    name.Format("SELECT ID, %s FROM %s AS name ORDER BY ID", dg1.name, dg1.table);
                                }
                            }

                            if (dg.name == "belongMagistral" && dg.table == "externalCodes") {
//                                name.Format("SELECT ID, %s AS name FROM %s WHERE objectID=1 ORDER BY ID", dg1.name, dg1.table);
                            }


                            dg1.label = dg.label;
                            dg1.name = dg.name;
                            hi = pm_cEditDlg->addDataGrid(hs, dg1);

                            long id = to_long(dg.value);

                            pm_cEditDlg->m_ctrlGrid.SetItemLookup(hi, (const char*)name, id);
                        }
                        else {
                            hi = pm_cEditDlg->addDataGrid(hs, dg);
                        }

                        CString schema2, tname2;

                        splitTableName(ado->m_schema, dg.table, schema2, tname2);

                        bool ro = false;
                        bool noedit = false;

                        map<CString, CString>::const_iterator it2 = map_fun.find(dg.name);
                        if (it2 != map_fun.end()) {
                            if (it2->second == "ReadOnly") {
                                ro = true;
                            }
                            if (it2->second.Find("NoEdit_") != -1) {
                                noedit = true;
                            }
                        }

                        if (ro || tname2 == "nodes" || tname2 == "linesobj" || tname2 == "heatPipeSections"
//                             || tname2 == "uchastok_ms" || tname2 == "uchastok_rs" || tname2 == "magistrali" 
                             || tname2 == "nachalniki_uchastkov"
    
                        ) {
                            pm_cEditDlg->m_ctrlGrid.SetItemReadOnly(hi);
                        }

                        if (noedit) {
                            pm_cEditDlg->m_ctrlGrid.SetItemNoEdit(hi);
                        }



                        pm_cEditDlg->m_ctrlGrid.SetItemHelp(hi, dg.help);
                        pm_cEditDlg->m_ctrlGrid.SetItemTable(hi, ado->m_schema, tn, id);

                        if (!ro) {
                            map<CString, CString>::const_iterator it2 = map_fun.find(dg.name);
                            if (it2 != map_fun.end()) {
                                pm_cEditDlg->m_ctrlGrid.SetItemFun(hi, it2->second);
                            }
                            map<CString, CString>::const_iterator it4 = map_after.find(dg.name);
                            if (it4 != map_after.end()) {
                                pm_cEditDlg->m_ctrlGrid.SetItemAfter(hi, it4->second);
                            }

                            {
                                auto it = map_before.find(dg.name);
                                if (it != map_before.end()) {
                                    pm_cEditDlg->m_ctrlGrid.SetItemBefore(hi, it->second);
                                }
                            }

                            map<CString, CString>::const_iterator it6 = map_validate.find(dg.name);
                            if (it6 != map_validate.end()) {
                                pm_cEditDlg->m_ctrlGrid.SetItemValidate(hi, it6->second);
                            }
                        }

                        map <CString, map<int, list<CString> > >::const_iterator it3 = map_disable.find(dg.name);
                        if (it3 != map_disable.end()) {
                            pm_cEditDlg->m_ctrlGrid.SetItemFunDisable(hi, "fun_disable", it3->second, map_disable);
                        }

                        break;
                    }
                }
            }
        }
    }


    pm_cEditDlg->m_ctrlGrid.RecalcOn();

    pm_cEditDlg->ShowWindow(SW_SHOW);
    
    return &pm_cEditDlg->m_ctrlGrid;
}


#if 0

bool ado_init(CAdoFile *ado, const char *tn, Join2 &j)
{
    map_ado_info_value.clear();
    
    if (!ado || !ado->isOpen()) return false;

    int n0 = j.initJoin(ado, tn);

    CString gg = getGidAdoName() + ".";
    CString ps = getPsAdoName() + ".";
    if (ps == gg) {
      gg = ps = "";
    }

    int n1 = j.addJoin("", ado, n0, "lineID", gg + "linesobj", "id", "[nodeID1][nodeID2][externalSignLineID]", false);

    if (n1) {
        j.addJoin("", ado, n1, "externalSignLineID", gg + "externalSignLine", "id", "[name]", true);

        int n2 = j.addJoin("", ado, n1, "nodeID1", gg + "nodes", "id", "[id][externalCodeID][externalNodeName]", false);
        j.addJoin("", ado, n2, "externalCodeID", gg + "externalCodes", "id", "[name]", true);

        int n3 = j.addJoin("", ado, n1, "nodeID2", gg + "nodes", "id", "[id][externalCodeID][externalNodeName]", false);
        j.addJoin("", ado, n3, "externalCodeID", gg + "externalCodes", "id", "[name]", true);
    }

  if (n1 > 0) {
      bool dop = false;

      CString qq = "[diameterCondit][tubingTypeID][magistral][magistralSite][distSite]";
      if (!_stricmp(tn, REM_INDICATOR)) {
//          qq = "[diameterCondit][tubingTypeID][magistral][magistralSite][distSite][firstPICdateHP][exploitSite]";
           qq = "[diameterCondit][tubingTypeID][magistral][magistralSite][distSite][firstPICdateHP]";
          dop = true;
      }


      int nn1 = j.addJoin2(ado, n1, "id", gg+"heatPipeSections", "lineID", qq);
      if (nn1) {
          j.addJoin("", ado, nn1, "magistral", ps + "magistrali", "id", "[naimenovanie_magistrali]", true);
          int nn_ms = j.addJoin("", ado, nn1, "magistralSite", ps + "uchastok_ms", "id", "[opisanie_uchastka_ms][nomer_uchastka]", true);
          int nn_rs = j.addJoin("", ado, nn1, "distSite", ps + "uchastok_rs", "id", "[naimenovanie_uchastka_rs][nomer_uchastka]", true);
          j.addJoin("", ado, nn1, "tubingTypeID", gg + "tubingTypes", "id", "[name]", true);

          if (dop) {
              int nn_ek_ms = j.addJoin("", ado, nn_ms, "nomer_uchastka", ps + "uchastki_ekspluatatsii", "id", "[nomer_uchastka][nachalnik_uchastka]", true);
              j.addJoin("", ado, nn_ek_ms, "nachalnik_uchastka", ps + "nachalniki_uchastkov", "id", "[fio]", true);

              int nn_ek_rs = j.addJoin("", ado, nn_rs, "nomer_uchastka", ps + "uchastki_ekspluatatsii", "id", "[nomer_uchastka][nachalnik_uchastka]", true);
              j.addJoin("", ado, nn_ek_rs, "nachalnik_uchastka", ps + "nachalniki_uchastkov", "id", "[fio]", true);
          }
      }
  }

  if (!n1) {
      int n12 = j.addJoin("", ado, n0, "nodeID", gg + "nodes", "id", "[externalCodeID][externalNodeName]", false);
      if (n12) {
          j.addJoin("", ado, n12, "externalCodeID", gg + "externalCodes", "id", "[name]", true);
      }
  }

  j.allJoin(ado, tn, n0);

  return true;
}

#endif


bool ado_init(CAdoFile *ado, const char *tn, Join2 &j)
{
    map_ado_info_value.clear();
    
    if (!ado || !ado->isOpen()) return false;

    int n0 = j.initJoin(ado, tn);

    j.allJoin(ado, tn, n0);

    return true;
}

CPropertyGrid *ado_info(CWnd *wnd, CAdoFile *ado, const char *tn, int id, const char *capt, TYP_OF_P typ_of_p, const char *tn_txt, bool autodelete)
{
  if (!ado || !ado->isOpen()) return 0;

  map_vis.clear();

  Join2 j;
  ado_init(ado, tn, j);
  CPropertyGrid *prop = view_join(wnd, ado, j, tn, id, capt, true, tn_txt, IDD_PROPGRID_DIALOG, typ_of_p, autodelete);

  if (ado->getError() != "") {
      AfxMessageBox(CString(_TR("Ошибка 2\n"))+ado->getError(), MB_OK|MB_ICONINFORMATION);
  }

  return prop;
}

static Join2 _j;

int ado_infoInit(CAdoFile *ado, const char *tn)
{
  if (!ado || !ado->isOpen()) return 0;
  ado_init(ado, tn, _j);
  return 0;
}

CPropertyGrid* ado_infoView(CWnd *wnd, CAdoFile *ado, const char *tn, int id, const char *capt, TYP_OF_P typ_of_p, const char *tn_txt, bool autodelete)
{
  map_vis.clear();

  if (!ado || !ado->isOpen()) return nullptr;
  
  CPropertyGrid* prop = view_join(wnd, ado, _j, tn, id, capt, true, tn_txt, IDD_PROPGRID_DIALOG, typ_of_p, autodelete);

  if (ado->getError() != "") {
      AfxMessageBox(CString(_TR("Ошибка 2\n"))+ado->getError(), MB_OK|MB_ICONINFORMATION);
  }

  return prop;
}



CPropertyGrid* ado_info_id(CWnd *wnd, CAdoFile *ado, const char *tn, int id, const char *capt, TYP_OF_P typ_of_p, const char *tn_txt, long ID)
{
  map_vis.clear();
  if (!ado || !ado->isOpen()) return 0;
  
  Join2 j;
  ado_init(ado, tn, j);

  CPropertyGrid* prop = view_join(wnd, ado, j, tn, id, capt, true, tn_txt, ID, typ_of_p, false);

  if (ado->getError() != "") {
      AfxMessageBox(CString(_TR("Ошибка 3\n"))+ado->getError(), MB_OK|MB_ICONINFORMATION);
  }

  return prop;
}

CString getExternalCode(int fileID)
{
    CString s;
    s.Format("SELECT ID, name FROM externalCodes WHERE fileID=%d ORDER BY ID", fileID);

    return s;
}

int ado_info_node2(CWnd* wnd, CAdoFile* ado, CNode2* node, const char* tn1, int id1, const char* tn2, int id2, int fileID, const char* tn_out1, int idgP, int idgO, int idPT, const char* capt0)
{
    map_vis.clear();
    ado->clearError();

    if (!ado || !ado->isOpen()) return 0;

    CString capt = capt0;

    bool is_zn = false;
    bool is_us2 = false;

    if (!strcmp(tn2, "setPressNodes")) is_zn = true;
    if (!strcmp(tn2, "connectNodes")) is_us2 = true;

    //    CString param1 = "externalCodeID, externalNodeName, externalSignID, geoMarkTopTube, geoMarkNodeArea, nodeTypeID";
    CString param1 = "externalCodeID, externalNodeName, externalSignID, geoMarkTopTube, geoMarkNodeArea";
    if (is_zn) {
        param1 = "externalCodeID, externalNodeName";
    }
    if (node->node.internalNodeID != 0) {
        param1 = "externalCodeID, externalNodeName, externalSignID";
    }

    if (is_us2) {
        param1 = "externalCodeID, externalNodeName, externalSignID";
    }

    Join join(tn1, tn2, id1, param1);

    join.addJoin(getExternalCode(fileID), "externalCodeID");

    join.addJoin("SELECT ID, name FROM organizations ORDER BY ID", "organizationID");
    join.addJoin("SELECT ID, name FROM externalSigns ORDER BY ID", "externalSignID");

    join.addJoin("SELECT ID, name FROM nodeTypes ORDER BY ID", "nodeTypeID");

    Join join2(tn2, tn2, id2, ado, fileID);

    capt += get_capt2(node);

    CString tn = tn2 != 0 && tn2[0] ? tn2 : tn1;

    CPropGridDlg *pm_cEditDlg = getPropGridDlg(wnd, ado, tn, capt, false, TP_NODE, (void*)id1, IDD_PROPGRID_DIALOG, false);
    if (!pm_cEditDlg) return 0;

    pm_cEditDlg->m_ctrlGrid.ResetContents();

    ado_add_table(ado, pm_cEditDlg, join, _TR("Узел"), NULL, 0, false);

    if (1) {
      ado_add_table(ado, pm_cEditDlg, join2, capt, NULL, 0, false);

      if (!is_zn) {
          Join join3(tn1, tn2, id1, ado, fileID);

          ado_add_table(ado, pm_cEditDlg, join3, _TR("Узел2"), "nodes2", 2000, false);
      }
    }

    if (ado->getError() != "") AfxMessageBox(CString(_TR("Ошибка 4\n"))+ado->getError(), MB_OK|MB_ICONINFORMATION);

    pm_cEditDlg->ShowWindow(SW_SHOW);

    return 0;
}


int ado_info_node2_vnutr(CWnd *wnd, CAdoFile *ado, CNode2 *node, const char *tn1, int id1, const char *tn2, int id2, int fileID, const char *tn_out1, int idgP, int idgO, int idPT, const char *capt)
//int ado_info_node2(CWnd *wnd, CAdoFile *ado, CNode2 *node)
{

    map_vis.clear();
    if (!ado || !ado->isOpen()) return 0;

//    CString param1 = "externalCodeID, externalNodeName, externalSignID, geoMarkTopTube, geoMarkNodeArea, operatorID";
    CString param1 = "externalCodeID, externalNodeName, externalSignID, geoMarkTopTube, geoMarkNodeArea";

    Join join(tn1, tn2, id1, param1);

    join.addJoin(getExternalCode(fileID), "externalCodeID");

    join.addJoin("SELECT ID, name FROM organizations ORDER BY ID", "organizationID");
    join.addJoin("SELECT ID, name FROM externalSigns ORDER BY ID", "externalSignID");

    CPropGridDlg *pm_cEditDlg = getPropGridDlg(wnd, ado, tn1, capt, false, TP_NODE, (void*)id1, IDD_PROPGRID_DIALOG, false);
    if (!pm_cEditDlg) return 0;

    pm_cEditDlg->m_ctrlGrid.ResetContents();
    ado_add_table(ado, pm_cEditDlg, join, _TR("Узел"), NULL, 0, false);

    pm_cEditDlg->ShowWindow(SW_SHOW);
    

    return 0;
}


bool openAdo(int type_of_net, const char *server, int port, const char *database, const char *user, const char *password)
{
    CAdoFile *ado = new CAdoFile;

    if (ado->openMySQL(type_of_net, server, port, database, user, password)) {
        return true;
    }
    return false;
}

int ado_info_line2(CWnd *wnd, CAdoFile *ado, CLINE2 *line, const char *tn1, int id1, const char *tn2, int id2, int idN1, int idN2, int fileID, const char *tn_out, int idgP, int idgO, const char *capt0)
{
    map_vis.clear();
    if (!ado || !ado->isOpen()) return 0;

    Join join1("nodes", "", idN1, "externalCodeID, externalNodeName");
    join1.addJoin("SELECT ID, name FROM externalCodes ORDER BY ID", "externalCodeID");

    Join join2("nodes", "", idN2, "externalCodeID, externalNodeName");
    join2.addJoin("SELECT ID, name FROM externalCodes ORDER BY ID", "externalCodeID");

//    CString p = "externalSignLineID, hydroRes, organizationID, registNum, archiveChangeDate, operatorID";
    CString p = "externalSignLineID, organizationID, hydroRes";

    if (m_idMenu == TYP_MENU_PTS) {
        p = "externalSignLineID, organizationID";
    }


    Join join3("linesobj", "", id1, p);
    join3.addJoin("SELECT ID, name FROM externalSignLine ORDER BY ID", "externalSignLineID");
    join3.addJoin("SELECT ID, name FROM organizations ORDER BY ID", "organizationID");

    Join join4(tn2, "", id2, ado, fileID);

    CString capt = capt0;
    capt += get_capt2(where(line));

    CPropGridDlg *pm_cEditDlg = getPropGridDlg(wnd, ado, tn1, capt, false, TP_LINE, (void*) id1, IDD_PROPGRID_DIALOG, false);
    if (!pm_cEditDlg) return 0;

    pm_cEditDlg->m_ctrlGrid.ResetContents();

    ado_add_table(ado, pm_cEditDlg, join1, _TR("Начальный узел"), NULL, 0, true);
    ado_add_table(ado, pm_cEditDlg, join2, _TR("Конечный узел"), NULL, 0, true);
    ado_add_table(ado, pm_cEditDlg, join3, _TR("Общая информация"), NULL, 0, false);
//    ado_add_table(ado, pm_cEditDlg, join4, capt, NULL, 1000, false);
    ado_add_table(ado, pm_cEditDlg, join4, capt, NULL, 0, false);

    if (1) {
      Join join5(tn1, "", id1, ado, fileID);
      ado_add_table(ado, pm_cEditDlg, join5, _TR("Архив"), "linesobj2", 2000, false);
    }



    pm_cEditDlg->ShowWindow(SW_SHOW);
    

    if (ado->getError() != "") AfxMessageBox(CString(_TR("Ошибка 5\n"))+ado->getError(), MB_OK|MB_ICONINFORMATION);

    return 0;
}

#if 0

int ado_info_line_big2(CWnd *wnd, CAdoFile *ado, CLINE2 *line, const char *tn, int id, int idN1, int idN2, const char *capt)
{
    if (!ado || !ado->isOpen()) return 0;

    Join join1("nodes", idN1, "externalCodeID, externalNodeName");
    join1.addJoin("SELECT ID, name FROM externalCodes ORDER BY ID", "externalCodeID");

    Join join2("nodes", idN2, "externalCodeID, externalNodeName");
    join2.addJoin("SELECT ID, name FROM externalCodes ORDER BY ID", "externalCodeID");

    CString p = "externalSignLineID, hydroRes, organizationID, registNum, archiveChangeDate, operatorID";

    Join join3("linesobj", id1, p);
    join3.addJoin("SELECT ID, name FROM externalSignLine ORDER BY ID", "externalSignLineID");
    join3.addJoin("SELECT ID, name FROM organizations ORDER BY ID", "organizationID");
    join3.addJoin("SELECT ID, name FROM operators ORDER BY ID", "operatorID");

    Join join4(tn2, id2, ado, fileID);

    CPropGridDlg *pm_cEditDlg = getPropGridDlg(wnd, ado, tn, capt, false, 2, (void*) id1, IDD_PROPGRID_DIALOG, false);
    if (!pm_cEditDlg) return 0;

    pm_cEditDlg->m_ctrlGrid.ResetContents();

    
    ado_add_table(ado, pm_cEditDlg, join1, _TR("Начальный узел"), NULL, 0, true);
    ado_add_table(ado, pm_cEditDlg, join2, _TR("Конечный узел"), NULL, 0, true);
    ado_add_table(ado, pm_cEditDlg, join3, _TR("Общая информация"), NULL, 0, false);
    ado_add_table(ado, pm_cEditDlg, join4, capt, NULL, 1000, false);

    pm_cEditDlg->ShowWindow(SW_SHOW);
    

    return 0;
}

#endif

int ado_info_PR(CWnd *wnd, CAdoFile *ado, CNode2 *node, const char *tn1, int id1, const char *tn2, int id2, int fileID, const char *tn_out1, int idgP, int idgO, int idPT, const char *capt, CString &schemeNum)
{
    map_vis.clear();
    if (!ado || !ado->isOpen()) return 0;

    if (m_idMenu == TYP_MENU_PTS) {
      return ado_info_node2(wnd, ado, node, tn1, id1, tn2, id2, fileID, tn_out1, idgP, idgO, idPT, capt);
//      int ado_info_PR(CWnd *wnd, CAdoFile *ado, CNode2 *node, const char *tn1, int id1, const char *tn2, int id2, int fileID, const char *tn_out1, int idgP, int idgO, int idPT, const char *capt, CString &schemeNum)
    }



    CString tn_out = "US_OUT";

    Join joinPT("PT_OUT", "", idPT, ado);

    Join joinOP(tn_out, "", idgP, ado);
    Join joinOO(tn_out, "", idgO, ado);

    CString param1 = "externalCodeID, externalNodeName, externalSignID, geoMarkTopTube, geoMarkNodeArea";

    Join join(tn1, tn2, id1, param1);

    join.addJoin(getExternalCode(fileID), "externalCodeID");
    join.addJoin("SELECT ID, name FROM organizations ORDER BY ID", "organizationID");

    
    join.addJoin("SELECT ID, name FROM externalSigns ORDER BY ID", "externalSignID");
    join.addJoin("SELECT ID, name FROM nodeTypes ORDER BY ID", "nodeTypeID");


    Join join2(tn2, tn2, id2, ado, fileID);

    CPropGridDlg *pm_cEditDlg = getPropGridDlg(wnd, ado, tn1, capt, false, TP_NODE, (void*) id1, IDD_PROPGRID_DIALOG, false);
    if (!pm_cEditDlg) return 0;

    pm_cEditDlg->m_ctrlGrid.ResetContents();

    ado_add_table(ado, pm_cEditDlg, join, _TR("Узел"), NULL, 0, false);
    ado_add_table_PR(ado, pm_cEditDlg, join2, schemeNum, _TR("Потребитель"));

    Join join3(tn1, tn2, id1, ado, fileID);

    ado_add_table(ado, pm_cEditDlg, join3, _TR("Узел2"), "nodesPR2", 1000, false);


    pm_cEditDlg->ShowWindow(SW_SHOW);
    

    return 0;
}


int ado_info_IS(CWnd *wnd, CAdoFile *ado, CNode2 *node, const char *tn1, int id1, const char *tn2, int id2, int fileID, const char *tn_out1, int idgP, int idgO, int idPT, const char *capt, CString &schemeNum)
{
    map_vis.clear();
    if (!ado || !ado->isOpen()) return 0;

//    CString param1 = "externalCodeID, externalNodeName, externalSignID, geoMarkTopTube, geoMarkNodeArea, nodeTypeID, operatorID";
//    CString param1 = "externalCodeID, externalNodeName, externalSignID, geoMarkTopTube, geoMarkNodeArea, nodeTypeID";
    CString param1 = "externalCodeID, externalNodeName, externalSignID, geoMarkTopTube, geoMarkNodeArea";

    Join join(tn1, tn2, id1, param1);

    join.addJoin(getExternalCode(fileID), "externalCodeID");

    join.addJoin("SELECT ID, name FROM organizations ORDER BY ID", "organizationID");
    join.addJoin("SELECT ID, name FROM externalSigns ORDER BY ID", "externalSignID");

//    CString qq;
//    qq.Format("SELECT TOP 2000000 id, name FROM nodeTypes WHERE PATINDEX('%%%s%%',visibles) > 0 ORDER BY ID", tn2);
    join.addJoin("SELECT ID, name FROM nodeTypes ORDER BY ID", "nodeTypeID");
//    join.addJoin(qq, "nodeTypeID");

    Join join2(tn2, tn2, id2, ado, fileID);

    CPropGridDlg *pm_cEditDlg = getPropGridDlg(wnd, ado, tn1, capt, false, TP_NODE, (void*)id1, IDD_PROPGRID_DIALOG, false);
    if (!pm_cEditDlg) return 0;

    pm_cEditDlg->m_ctrlGrid.ResetContents();

    ado_add_table(ado, pm_cEditDlg, join, _TR("Узел"), NULL, 0, false);

//    Join join_ist("istochniki_tepla", 86, ado, -1);

    if (1) {
      ado_add_table(ado, pm_cEditDlg, join2, capt, NULL, 0, false);

//      ado_add_table(ado, pm_cEditDlg, join_ist, _TR("Источник ПС"), NULL, 0, false);
      
      Join join3(tn1, tn2, id1, ado, fileID);

      ado_add_table(ado, pm_cEditDlg, join3, _TR("Узел2"), "nodes2", 2000, false);
    }

    pm_cEditDlg->ShowWindow(SW_SHOW);
    

    return 0;
}



int ado_info_line_out(CWnd *wnd, CAdoFile *ado, const char *tn1, int id1, const char *tn2, int id2, int idN1, int idN2, int fileID, const char *tn_out, int idgP, int idgO, const char *capt)
{
    map_vis.clear();
    if (!ado || !ado->isOpen()) return 0;

    Join joinOP(tn_out, "", idgP, ado);
    Join joinOO(tn_out, "", idgO, ado);

    Join join1("nodes", "", idN1, "externalCodeID, externalNodeName");
    join1.addJoin(getExternalCode(fileID), "externalCodeID");

    Join join2("nodes", "", idN2, "externalCodeID, externalNodeName");
    join2.addJoin(getExternalCode(fileID), "externalCodeID");
    CString p = "externalSignLineID, hydroRes, organizationID, registNum, archiveChangeDate";

    Join join3("linesobj", "", id1, p);
    join3.addJoin("SELECT ID, name FROM externalSignLine ORDER BY ID", "externalSignLineID");
    join3.addJoin("SELECT ID, name FROM organizations ORDER BY ID", "organizationID");

    Join join4(tn2, "", id2, ado);

    CString capt1 = CString(capt)+" : Результат расчета";

    CPropGridDlg *pm_cEditDlg = getPropGridDlg(wnd, ado, tn1, capt1, false, TP_ZERO, NULL, IDD_PROPGRID_DIALOG, false);
    if (!pm_cEditDlg) return 0;

    pm_cEditDlg->m_ctrlGrid.ResetContents();

    ado_add_table(ado, pm_cEditDlg, join1, _TR("Начальный узел"), NULL, 0, true);
    ado_add_table(ado, pm_cEditDlg, join2, _TR("Конечный узел"), NULL, 0, true);

    // Входные

    CString tn2s = tn2;

    if (tn2s == "elevators" || tn2s == "diaphragms" || tn2s == "heatExchangers" || tn2s == "airHeaters" || tn2s == "systemRadiators") {
        Join join_in(tn2, "", id2, ado, 0, "");
        ado_add_table(ado, pm_cEditDlg, join_in, _TR("Входные данные"), NULL, 0, true);
    }

    if (idgP > 0) ado_add_table(ado, pm_cEditDlg, joinOP, _TR("Подающий"), NULL, 0, true);
    if (idgO > 0) ado_add_table(ado, pm_cEditDlg, joinOO, _TR("Обратный"), NULL, 0, true);
  
    pm_cEditDlg->ShowWindow(SW_SHOW);

    return 0;
}


int ado_info_node_out(CWnd *wnd, CAdoFile *ado, const char *tn1, int id1, const char *tn2, int id2, int fileID, const char *tn_out1, int idgP, int idgO, int idPT, const char *capt)
{
    map_vis.clear();
    if (!ado || !ado->isOpen()) return 0;

    CString tn_out = "US_OUT";

    Join joinPT("PT_OUT", "", idPT, ado);

    Join joinOP(tn_out, "", idgP, ado, 0, "pih,t,t2");
    Join joinOO(tn_out, "", idgO, ado, 0, "pih,t,t2");

    CString param = "externalCodeID, externalNodeName, externalSignID";

    Join join(tn1, tn2, id1, param);
    join.addJoin(getExternalCode(fileID), "externalCodeID");
    join.addJoin("SELECT ID, name FROM organizations ORDER BY ID", "organizationID");
    join.addJoin("SELECT ID, name FROM fragments ORDER BY ID", "fileID");
    join.addJoin("SELECT ID, name FROM externalSigns ORDER BY ID", "externalSignID");

    join.addJoin("SELECT ID, name_building AS name FROM addresses ORDER BY ID", "addressID");


    joinPT.addJoin("SELECT ID, Tn AS name FROM CALCULATION", "calculationID");

    Join join2(tn2, tn2, id2, ado, fileID);

    CString capt1 = CString(capt)+" : Результат расчета";

    CPropGridDlg *pm_cEditDlg = getPropGridDlg(wnd, ado, tn1, capt1, false, TP_ZERO, NULL, IDD_PROPGRID_DIALOG, false);
    if (!pm_cEditDlg) return 0;

    pm_cEditDlg->m_ctrlGrid.ResetContents();

    ado_add_table(ado, pm_cEditDlg, join, _TR("Узел"), NULL, 0, true);

    if (idPT > 0) {
        ado_add_table(ado, pm_cEditDlg, joinPT, _TR("Потребитель"), NULL, 0, true);
    }
    else {
        if (idgP > 0) ado_add_table(ado, pm_cEditDlg, joinOP, _TR("Подающий"), "US_OUT_P", 0, true);
        if (idgO > 0) ado_add_table(ado, pm_cEditDlg, joinOO, _TR("Обратный"), "US_OUT_O", 0, true);
    }

    pm_cEditDlg->ShowWindow(SW_SHOW);

    return 0;
}


int ado_info_node_out_DR(CWnd *wnd, CAdoFile *ado, const char *tn1, int id1, const char *tn2, int id2, int fileID, const char *tn_out1, int idPT, const char *capt)
{
    map_vis.clear();
    if (!ado || !ado->isOpen()) return 0;

    Join joinPT("DR_OUT", "", idPT, ado);

    CString param = "externalCodeID, externalNodeName, externalSignID";

    Join join(tn1, "", id1, param);
    join.addJoin(getExternalCode(fileID), "externalCodeID");
    join.addJoin("SELECT ID, name FROM organizations ORDER BY ID", "organizationID");
    join.addJoin("SELECT ID, name FROM fragments ORDER BY ID", "fileID");
    join.addJoin("SELECT ID, name FROM externalSigns ORDER BY ID", "externalSignID");

    join.addJoin("SELECT ID, name_building AS name FROM addresses ORDER BY ID", "addressID");

    Join join2(tn2, tn2, id2, ado, fileID);

    CString capt1 = CString(capt) + " : Результат расчета";

    CPropGridDlg *pm_cEditDlg = getPropGridDlg(wnd, ado, tn1, capt1, false, TP_ZERO, NULL, IDD_PROPGRID_DIALOG, false);
    if (!pm_cEditDlg) return 0;

    pm_cEditDlg->m_ctrlGrid.ResetContents();

    ado_add_table(ado, pm_cEditDlg, join, _TR("Узел"), NULL, 0, true);

    if (idPT > 0) {
        ado_add_table(ado, pm_cEditDlg, joinPT, _TR("Дроссели"), NULL, 0, true);
    }

    pm_cEditDlg->ShowWindow(SW_SHOW);
    

    return 0;
}



int ado_info_find(CWnd *wnd, CAdoFile *ado, const char *tn, const char *capt)
{
    map_vis.clear();
    if (!ado || !ado->isOpen()) return 0;

    Join join(tn, "", FIND_DIALOG, ado);

    CPropGridDlg *pm_cEditDlg = getPropGridDlg(wnd, ado, tn, capt, false, TP_ZERO, NULL, IDD_PROPGRID_DIALOG, false);
    if (!pm_cEditDlg) return 0;

    pm_cEditDlg->m_ctrlGrid.ResetContents();

    pm_cEditDlg->m_ctrlGrid.SetAddCol(1);

    ado_add_table(ado, pm_cEditDlg, join, capt, CString(tn)+CString("An"), 0, false);

    pm_cEditDlg->ShowWindow(SW_SHOW);
    

    return 0;
}


int ado_info_line_big2(CWnd *wnd, CAdoFile *ado, const char *tn2, int id2, int idN1, int idN2, const char *capt)
{
    map_vis.clear();
    if (!ado || !ado->isOpen()) return 0;

    Join join1("nodes", "", idN1, "nodeName, externalCodeID, externalNodeName");
    join1.addJoin("SELECT ID, name FROM externalCodes ORDER BY ID", "externalCodeID");

    Join join2("nodes", "", idN2, "nodeName, externalCodeID, externalNodeName");
    join2.addJoin("SELECT ID, name FROM externalCodes ORDER BY ID", "externalCodeID");

    Join join4(tn2, "", id2, ado, 0);

    CPropGridDlg *pm_cEditDlg = getPropGridDlg(wnd, ado, tn2, capt, false, TP_ZERO, NULL, IDD_PROPGRID_DIALOG, false);

    if (!pm_cEditDlg) return 0;

    pm_cEditDlg->m_ctrlGrid.ResetContents();
    
    ado_add_table(ado, pm_cEditDlg, join1, _TR("Начальный узел"), NULL, 0, true);
    ado_add_table(ado, pm_cEditDlg, join2, _TR("Конечный узел"), NULL, 0, true);
    ado_add_table(ado, pm_cEditDlg, join4, capt, NULL, 0, false);

    pm_cEditDlg->ShowWindow(SW_SHOW);
    

    return 0;
}


int getLineTyp2(const char* tn);
int getNodeTyp2(const char* tn);


int ado_view(CWnd *wnd, CAdoFile *ado, CString table_name, int id, CString title, bool readonly)
{
    int ret = -1;
//    CAdoFile* ado = getAdo(getGidAdoName());
//    ado = getAdo("sprav");

    CGidrView *pView = getView();
    if (!pView) return -1;

    CCxema *cxema = pView->m_cxema;
    CGraph2 *graph = cxema->m_graph;

    CString s;
    s.Format("%s %d", table_name, id);

    int typ = getLineTyp2(table_name);
   
    if (typ >= 0) {
        graph->init_find_line_nom();
        CLINE2 *line = graph->find_line_nom(id);

        if (line) {
            return pView->info(cxema, nullptr, line, wnd);
        }
        AfxMessageBox(s);
        return -1;
    } 

    typ = getNodeTyp2(table_name);

    if (table_name == "nodes") typ = 0;
   
    if (typ >= 0) {
        CNode2 *node = graph->find(id);

        if (node) {
            return pView->info(cxema, node, nullptr, wnd);
        }
        AfxMessageBox(s);
        return -1;
    } 

    typ = getLineTypOut(table_name);
   
    if (typ >= 0) {
        graph->init_find_line_nom();
        CLINE2 *line = graph->find_line_nom(id);

        if (line) {
            return pView->info_gid(cxema, nullptr, line, wnd);
        }
        AfxMessageBox(s);
        return -1;
    } 

    typ = getNodeTypOut(table_name);
   
    if (typ >= 0) {
        CNode2 *node = graph->find(id);

        if (node) {
            return pView->info_gid(cxema, node, nullptr, wnd);
        }
        AfxMessageBox(s);
        return -1;
    } 

    Klassif *kls = m_geofile->m_kl_list->findKlN(table_name);

    if (kls) {
        ado = kls->m_ado;
    }

    if (!ado) {
        ado = getAdo(getGidAdoName());
    }

    ado_info(wnd, ado, table_name, id, title, TP_DMITRI, NULL, false);

    return ret;
}
