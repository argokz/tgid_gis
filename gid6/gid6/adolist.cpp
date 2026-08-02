#include "stdafx.h"
#include "gid6.h"

#include "graph2.h"

//#include "virtlistDoc.h"
#include "virtlistView.h"

#include "adolist.h"
#include "ado.h"
#include "InputDia.h"
#include "win.h"

#include "join2.h"


CAdoListData::CAdoListData()
{
    m_q0 = "";
    m_typ = virt_data_unknown;
}

CString getTableNameFromSQL(CString q);


bool CAdoListData::initFlds(const char *q)
{
  MySQLColumns *col = m_ado->findMySQLColumns(m_tn, "id");
  
  if (m_ado->openTable0(q)) {
    CString qc;
    m_nFlds = m_ado->NFlds();
    m_adoField = new CAdoField[m_nFlds+1];

    for (int i = 0; i < m_nFlds; i++) {
        CAdoField f = m_ado->Field(i);
        m_adoField[i] = f;
    }

    CString tn = m_tn;


    if (tn == "") {
//        tn = getTableNameFromSQL(q);
    }


    for (int i = 0; i < m_nFlds; i++) {
        CAdoField f = m_adoField[i];

        CString name = f.name;
        CString title = name;
        CString f1 = name;

        MySQLColumns *col = m_ado->findMySQLColumns(tn, name);
        MySQLColumns2 *col2 = m_ado->findMySQLColumns2(tn, name);
        MySQLColumns2 *colOld = m_ado->findMySQLOldColumns(tn, name);

        if (col) {
            if (col->strCOLUMN_COMMENT != "") {
                title = col->strCOLUMN_COMMENT;
            }
        }

        if (colOld) {
            if (colOld->hlp != "")  title = colOld->hlp;
            if (colOld->helpshiftf1 != "" && f1 == name )  f1 = colOld->helpshiftf1;
        }
        else if (col2) {
            if (col2->hlp != "")  title = col2->hlp;
            if (col2->helpshiftf1 != "") f1 = col2->helpshiftf1;
        }

        if (col) {
            const CColName* col_name = findColumnRusName(col->strTABLE_SCHEMA, col->strTABLE_NAME, col->strCOLUMN_NAME);

            if (col_name) {
                title = col_name->name;
                f1 = col_name->name_full;
            }
        }

        f.name = name;
        f.title = title;
        f.f1 = f1;
        m_adoField[i] = f;
    }
    m_ado->closeTable();
  }
  else {
    AfxMessageBox(m_ado->getError(), MB_OK|MB_ICONINFORMATION);
  return false;
  }
  return true;
}

void CAdoListData::setGeo(const char *tn)
{
  m_tn = tn;
  m_typ = virt_data_geo;
}

CString stripOrderBy(CString q);


int CAdoListData::initQpat(CAdoFile *ado, const char *q)
{
  m_ado = ado;
  m_nFlds = 1;
  m_id_visible = true;
  m_after_id_visible = true;
  m_q = q;

  if (m_q0 == "") m_q0 = m_q;

  map_id.clear();

  m_q_pat.Format("SELECT * FROM (%s\n) __T__ WHERE id IN ($P$)", stripOrderBy(m_q));
  m_adoField = NULL;

  if (!initFlds(q)) return -1;

  int n = -1;

  if (ado->openTable0(q)) {
    n = 0;
    while (!ado->isEOF()) {
      long id = ado->read_long(0);
      map_id[n] = id;
      map_id_obr[id] = n;
      n++;
      ado->MoveNext();
    }

    ado->closeTable();
  }

  m_typ = virt_data_unknown;

  return n;
}

bool CAdoListData::initQsort(CAdoFile *ado, const char *q, int *cols,int num,int flags)
{
  m_ado = ado;
  m_nFlds = 1;
  m_id_visible = true;
  m_after_id_visible = true;
  m_q = q;

  m_q_pat.Format("SELECT * FROM (%s\n) __T__ WHERE id IN ($P$)", stripOrderBy(m_q));
  m_adoField = NULL;

  
  map_id.clear();
  
  CString qs;


  if (!initFlds(q)) return false;


  CString fns = GetFieldName(*cols);
  qs.Format("%s ORDER BY %s", q, fns);

  if (flags == 2) qs += " DESC";

  int n = 0;

  if (ado->openTable0(qs)) {
    while (!ado->isEOF()) {
      long id = ado->read_long(0);
      map_id[n] = id;
      n++;
      ado->MoveNext();
    }

    ado->closeTable();
  }
  else {
    initQpat(ado, q);
  }

  m_typ = virt_data_unknown;

  return true;
}



bool CAdoListData::initQ(CAdoFile *ado, const char *q)
{
  bool ret = initQpat(ado, q);
  
//  m_q_pat = q;

  return true;
}



void sort_graph(CGraph2 *graph);


bool CAdoListData::initNode(CGraph2 *graph, CAdoFile *ado, const char *tn, int typ)
{
  m_ado = ado;
  m_nFlds = 1;
  m_id_visible = true;
  m_after_id_visible = true;
  m_adoField = NULL;

  CString q;
  Join2 j;

  int n1 = j.initJoin(ado, "nodes");
  j.addJoin("", ado, n1, "externalCodeID", "externalCodes", "id", NULL, false);

  j.addJoin("", ado, n1, "organizationID" , "organizations","id", NULL, false);
  j.addJoin("", ado, n1, "fileID"        ,  "fragments"    ,"id", NULL, false);
  j.addJoin("", ado, n1, "operatorID"     , "operators"    ,"id", NULL, false);

  if (tn && tn[0]) {
    j.addJoin2(ado, n1, "id", tn , "nodeID", NULL);
  }

  q = j.getQ();
  
  m_q = q;
  m_q_pat = j.getQ2();

  CInputDialog inD(AfxGetMainWnd( ), "", "?", m_q);
  inD.DoModal();
 

  initFlds(q);
  
  sort_graph(graph);

  map<long, map<long, map<long, CNode2*> > > map_n;

  int cnt = 0;
  CMapGraph::const_iterator p;
  p = graph->map_node.begin();
  while (p != graph->map_node.end()) {
    CNode2 *n = p->second;
//    if (n->node.internalNodeID == 0 && n->node.isPjezo) {
    if (n->node.internalNodeID == 0 && n->node.typ == typ ) {
      if (n->node.coord.x != 0 || n->node.coord.y != 0) {
        map_n[n->node.fileID][n->node.n_sort][cnt++] = n;
      }
    }
    p++;
  }

  map_id.clear();

  cnt = 0;

  map<long, map<long, map<long, CNode2*> > >::iterator it1 = map_n.begin();

  for (; it1 != map_n.end(); it1++) {
    map<long, map<long, CNode2*> >::iterator it2 = it1->second.begin();
    for (; it2 != it1->second.end(); it2++) {
      map<long, CNode2*>::iterator it3 = it2->second.begin();
      for (; it3 != it2->second.end(); it3++) {
        map_id[cnt++] = it3->second->id;
      }
    }
  }

  m_typ = virt_data_node;

  return true;
}


bool CAdoListData::initLine(CGraph2 *graph, CAdoFile *ado, const char *tn, int typ)
{
  m_ado = ado;
  m_nFlds = 1;
  m_id_visible = true;
  m_after_id_visible = true;
  m_adoField = NULL;

  CString q;
  Join2 j;

  int n1 = j.initJoin(ado, "linesobj");
  q = j.getQ();
  int n2 = j.addJoin("", ado, n1, "nodeID1", "nodes", "id", "[nodeName][externalCodeID]", false);
  int n3 = j.addJoin("", ado, n1, "nodeID2", "nodes", "id", "[nodeName][externalCodeID]", false);
  j.addJoin("", ado, n2, "externalCodeID", "externalCodes", "id", NULL, false);
  j.addJoin("", ado, n3, "externalCodeID", "externalCodes", "id", NULL, false);
  j.addJoin("", ado, n1, "externalSignLineID", "externalSignLine", "id", NULL, false);

  j.addJoin2(ado, n1, "id", tn , "lineID", NULL);

  q = j.getQ();
  
  m_q = q;
  m_q_pat = j.getQ2();

  initFlds(q);
  
  sort_graph(graph);

  map<long, map<long, map<long, CLINE2*> > > map_n;

  int cnt = 0;
  CMapGraph::const_iterator p;
  p = graph->map_node.begin();
  while (p != graph->map_node.end()) {
    CNode2 *n = p->second;
//    if (n->node.internalNodeID == 0 && n->node.isPjezo) {
    if (n->node.internalNodeID == 0) {
      if (n->node.coord.x != 0 || n->node.coord.y != 0) {

        for ( CLINE2 *l = n->lines; l; l = next(l)) {
          if (IsBegin(l) && typ == bline(l)->line.typ) {
            map_n[n->node.fileID][bline(l)->line.n_sort][cnt++] = l;
          }
        }
      }
    }
    p++;
  }

  map_id.clear();

  cnt = 0;

  map<long, map<long, map<long, CLINE2*> > >::iterator it1 = map_n.begin();

  for (; it1 != map_n.end(); it1++) {
    map<long, map<long, CLINE2*> >::iterator it2 = it1->second.begin();
    for (; it2 != it1->second.end(); it2++) {
      map<long, CLINE2*>::iterator it3 = it2->second.begin();
      for (; it3 != it2->second.end(); it3++) {
        CLine2 *l = bline(it3->second);
        if (l->line.nomP >= 0) map_id[cnt++] = l->line.nomP;
        if (l->line.nomO >= 0 && l->line.nomP != l->line.nomO) map_id[cnt++] = l->line.nomO;
      }
    }
  }

  m_typ = virt_data_line;

  return true;
}

bool CAdoListData::initNodeOut(CGraph2 *graph, CAdoFile *ado, const char *tn, int typ)
{
  return false;
}


bool CAdoListData::initLineOut(CGraph2 *graph, CAdoFile *ado, const char *tn, int typ)
{
  m_ado = ado;
  m_nFlds = 1;
  m_id_visible = true;
  m_after_id_visible = true;
  m_adoField = NULL;

  CString q;
  Join2 j;

  int n1 = j.initJoin(ado, "linesobj");
  q = j.getQ();
  int n2 = j.addJoin("", ado, n1, "nodeID1", "nodes", "id", "[nodeName][externalCodeID]", false);
  int n3 = j.addJoin("", ado, n1, "nodeID2", "nodes", "id", "[nodeName][externalCodeID]", false);
  j.addJoin("", ado, n2, "externalCodeID", "externalCodes", "id", NULL, false);
  j.addJoin("", ado, n3, "externalCodeID", "externalCodes", "id", NULL, false);
  j.addJoin("", ado, n1, "externalSignLineID", "externalSignLine", "id", NULL, false);

  j.addJoin2(ado, n1, "id", tn , "lineID", NULL);

  q = j.getQ();
  
  m_q = q;
  m_q_pat = j.getQ2();

  initFlds(q);
  
  sort_graph(graph);

  map<long, map<long, map<long, CLINE2*> > > map_n;

  int cnt = 0;
  CMapGraph::const_iterator p;
  p = graph->map_node.begin();
  while (p != graph->map_node.end()) {
    CNode2 *n = p->second;
//    if (n->node.internalNodeID == 0 && n->node.isPjezo) {
    if (n->node.internalNodeID == 0) {
      if (n->node.coord.x != 0 || n->node.coord.y != 0) {

        for ( CLINE2 *l = n->lines; l; l = next(l)) {
          if (IsBegin(l)/* && typ == bline(l)->line.typ*/) {
            map_n[n->node.fileID][bline(l)->line.n_sort][cnt++] = l;
          }
        }
      }
    }
    p++;
  }

  map_id.clear();

  cnt = 0;

  map<long, map<long, map<long, CLINE2*> > >::iterator it1 = map_n.begin();

  for (; it1 != map_n.end(); it1++) {
    map<long, map<long, CLINE2*> >::iterator it2 = it1->second.begin();
    for (; it2 != it1->second.end(); it2++) {
      map<long, CLINE2*>::iterator it3 = it2->second.begin();
      for (; it3 != it2->second.end(); it3++) {
        CLine2 *l = bline(it3->second);
        if (l->line.nomP >= 0) map_id[cnt++] = l->line.nomP;
        if (l->line.nomO >= 0 && l->line.nomP != l->line.nomO) map_id[cnt++] = l->line.nomO;
      }
    }
  }

  m_typ = virt_data_line;

  return true;
}




CAdoListData::~CAdoListData()
{
  delete [] m_adoField;
}


int CAdoListData::get_id(int i)
{
  int id = 0;
  map<long, long>::const_iterator itm = map_id.find(i);
  if (itm != map_id.end()) {
    id = itm->second;
  }
  return id;
}

int CAdoListData::get_id_obr(int i)
{
  int id = 0;
  map<long, long>::const_iterator itm = map_id_obr.find(i);
  if (itm != map_id_obr.end()) {
    id = itm->second;
  }
  return id;
}



CString CAdoListData::GetItemText(int nSubItem, int nIndex, int first, int last)
{
  if (!m_id_visible) nSubItem += 1;
  if (!m_after_id_visible) nSubItem += 1;
  CString str = "";

  if (nSubItem < 0 || nSubItem >= m_nFlds) return "";


  if (nIndex >= GetCount()) {
    return "";
  }

/*
  if (nSubItem == 0) {
    str.Format("%d", nIndex+1);
    return str;
  }
*/

  int id = get_id(nIndex);

  map<long, vector<CString> >::const_iterator it = map_val.find(id);

  if (it != map_val.end()) {
    CString s = it->second[nSubItem];
    return s;
  }

  CString s, ss = "";

  for (int i = first; i <= last; i++) {
    int id1 = get_id(i);

    map<long, vector<CString> >::const_iterator it = map_val.find(id1);
    if (it == map_val.end()) {
      s.Format("%d", id1);
      if (ss != "") ss += ",";
      ss += s;
    }
  }

  if (ss == "") return "";

  
  CString q = m_q_pat;

  q.Replace("$P$", ss);


//  static int qqq = 1;

//  if (qqq) {
//    CInputDialog inD(AfxGetMainWnd( ), "", "?", q);
//    inD.DoModal();
//    qqq = 0;
//  }

  void log4(const char *podp, const char *s);
//  log4("", q);


  if (m_ado->openTable0(q)) {
    m_nFlds = m_ado->NFlds();

    while (!m_ado->isEOF()) {
      long id1 = m_ado->read_long("id");

      vector<CString> v(m_nFlds);
      for (int i = 0; i < m_nFlds; i++) {
        v[i] = m_ado->readStr(i);
      }
      map_val[id1] = v;
      m_ado->MoveNext();
    }

    m_ado->closeTable();
  }
  else {
    static int qqq = 1;

    if (qqq) {
      CInputDialog inD(AfxGetMainWnd( ), "", "?", q+"\n"+m_ado->getError());
      inD.DoModal();
      qqq = 0;
    }
  }


  it = map_val.find(id);

  if (it != map_val.end()) {
    str = it->second[nSubItem];
    return str;
  }


  return "";
}


bool CAdoListData::SetItem(int id)
{
  int id2 = get_id_obr(id);

  return false;
}




int CAdoListData::GetImage(int nIndex)
{
  return nIndex%3 - 1;
}

int CAdoListData::GetStateImage(int nIndex)
{
  // Ќумераци€ иконок состо€ни€ начинаетс€ с единицы
  return INDEXTOSTATEIMAGEMASK(nIndex%2 + 1); 
}

int CAdoListData::GetCount()
{ 
  return map_id.size(); 
}


int CAdoListData::GetNFlds()
{
  int dt = 0;
  if (!m_id_visible) dt += 1;
  if (!m_after_id_visible) dt +=1;
  

  return m_nFlds - dt;
}

CString CAdoListData::GetFieldF1(int i)
{
    if (!m_id_visible) i += 1;
    if (!m_after_id_visible) i += 1;
    if (i < 0 || i >= m_nFlds) return "";
    return m_adoField[i].f1;
}

CString CAdoListData::GetFieldName(int i)
{
  if (!m_id_visible) i += 1;
  if (!m_after_id_visible) i += 1;
  if (i < 0 || i >= m_nFlds) return "";

//  CString s = m_adoField[i].name;
  CString s = m_adoField[i].title;
  
  if (s == "name") s = "";

  if (m_nFlds <= 2) s = "";
  
  return s;
}

CString CAdoListData::GetFieldTitle(int i)
{
  if (!m_id_visible) i += 1;
  if (!m_after_id_visible) i += 1;
  if (i < 0 || i >= m_nFlds) return "";

  return m_adoField[i].title;
}



bool isAdoDigit(int typ)
{
  switch(typ) {
    case adEmpty :
      break;
    case adTinyInt :
    case adSmallInt :
    case adInteger :
    case adBigInt :
    case adUnsignedTinyInt :
    case adUnsignedSmallInt :
    case adUnsignedInt :
    case adUnsignedBigInt :
    case adSingle :
    case adDouble :
    case adCurrency :
    case adDecimal :
    case adNumeric :
      return true;
      break;
  
    case adBoolean :
    case adError :
    case adUserDefined :
    case adVariant :
    case adIDispatch :
    case adIUnknown :
    case adGUID :
    case adDate :
    case adDBDate :
    case adDBTime :
    case adDBTimeStamp :
    case adBSTR :
    case adChar :
    case adVarChar :
    case adLongVarChar :
    case adWChar :
    case adVarWChar :
      break;

    case adLongVarWChar :
    case adBinary :
    case adVarBinary :
    case adLongVarBinary :
    case adChapter :
    case adFileTime :
    case adPropVariant :
    case adVarNumeric :
      break;
  }
  return false;
}

int getAdoWidth(CAdoField &af)
{
  int ll = 0;

  switch(af.typ) {
    case adEmpty :
      break;
    case adTinyInt :
    case adSmallInt :
    case adInteger :
    case adBigInt :
    case adUnsignedTinyInt :
    case adUnsignedSmallInt :
    case adUnsignedInt :
    case adUnsignedBigInt :
    case adSingle :
    case adDouble :
    case adCurrency :
    case adDecimal :
    case adNumeric :
      ll = af.Precision;
      break;
  
    case adBoolean :
    case adError :
    case adUserDefined :
    case adVariant :
    case adIDispatch :
    case adIUnknown :
    case adGUID :
    case adDate :
    case adDBDate :
    case adDBTime :
    case adDBTimeStamp :
    case adBSTR :
    case adChar :
    case adVarChar :
    case adLongVarChar :
      break;
    case adWChar :
    case adVarWChar :
      ll = af.DefinedSize;
      break;

    case adLongVarWChar :
        ll = 50;
        break;
    case adBinary :
    case adVarBinary :
    case adLongVarBinary :
    case adChapter :
    case adFileTime :
    case adPropVariant :
    case adVarNumeric :
      break;
  }
  return ll;
}


int CAdoListData::GetFieldWidth(int i)
{
  if (!m_id_visible) i += 1;
  if (!m_after_id_visible) i += 1;
  if (i < 0 || i >= m_nFlds) return 100;
  short defSize = m_adoField[i].DefinedSize;
  int l = m_adoField[i].name.GetLength();
  
  
  int ll = getAdoWidth(m_adoField[i]);

  if (ll > l) l = ll;
  if (defSize > l) l = defSize;
  if (l > 50) l = 50;

  return l;
//  return m_adoField[i].Precision;
//  return 100;
}


int CAdoListData::GetFormat(int i) 
{ 
  if (!m_id_visible) i += 1;
  if (!m_after_id_visible) i += 1;
  if (i < 0 || i >= m_nFlds) return LVCFMT_LEFT;

  if (isAdoDigit(m_adoField[i].typ)) return LVCFMT_RIGHT; 

  return LVCFMT_LEFT; 
};

void * CAdoListData::getData(int nIndex)
{
  if (nIndex >= GetCount()) {
    return NULL;
  }

  int id = get_id(nIndex);

  return (void*) id;
}


int CAdoListData::SortBy(int *cols,int num,int flags)
{
    initQsort(m_ado, m_q, cols, num, flags);
    return 0;
}

int regex_match(const char *p, const char *buf, vector<CString> & match);



void CAdoListData::setFindText(const char *text)
{
    vector<CString> match;
    int l = regex_match(".+,\\s*([^ ]+)(\\s+FROM\\s+[^ ]+)\\s*", m_q, match);

    if (l == 0) {
        l = regex_match(".+\\s+AS\\s+([^ ]+)(\\s+FROM\\s+[^ ]+)\\s*", m_q, match);
    }

    CString q = m_q0;
    CString ss;

    bool no_id_vis = true;

    if (l > 1) {
        CString s1 = match[1];
        CString s2 = match[2];

        if (q.Find(" WHERE ") != -1) {
            ss.Format(" WHERE %s LIKE '%%%s%%' AND ", s1, text);
            q.Replace(" WHERE ", ss);
            no_id_vis = false;
        }
        else if (q.Find("ORDER BY ") != -1) {
            ss.Format(" WHERE %s LIKE '%%%s%%' ORDER BY ", s1, text);
            q.Replace("ORDER BY ", ss);
            no_id_vis = false;
        }
        else {
            ss.Format(" WHERE %s LIKE '%%%s%%' ", s1, text);
            q += ss;
            no_id_vis = false;
        }
    }
    else {
        q.Format("SELECT * FROM ( %s ) __QQQ WHERE name LIKE '%%%s%%'", m_q, text);
        no_id_vis = false;
    }


    initQpat(m_ado, q);

    if (!no_id_vis) {
        setIdVisible(false);
    }
}
