#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
#include "MMenu.h"
#include "ado.h"
#include "ado2.h"

#include "gid6doc.h"
#include "gidrview.h"

#include "coordlis.h"
#include "math.h"

#define N_1_3 "1.3"

CString findTableRusNameGid(CString n);

int addRemoved(CAdoFile *ado, int typeID, int id, const char *text);

//void PtsAddLine(CLINE2 *l);
//void PtsDelLine(CLINE2 *l);


CString getAdoError();

bool ado_execute(CAdoFile *ado, const char *q, long * affected);
long ado_executeInsert(CAdoFile *ado, const char *q, long * affected);
long addNodeUS(CAdoFile *ado, const char *tn, const char *name, int fileID, int externalSignID, int internalNodeID, int externalCodeID, CFPoint pt);
long addNode2(CAdoFile *ado, const char *tn, int id);
void create_b5(CAdoFile * ado, CNode2 * nP, CCxema * cxema);
void setNodeCxema(CNode2* node2, const char* schemeNum);
bool delCxemaFull(CCxema *m_cxema, const CNode2 *node);
void reset_ms_rs_node(CAdoFile* ado, CGraph2* graph, CNode2* n);
void reset_ms_rs_line(CAdoFile* ado, CGraph2* graph, CLINE2* line);



long addLine(CAdoFile *ado, int typ, CNode2 *n1, CNode2 *n2, int externalSignLineID, CCoordList &cl, long &idv);
CString getLineQ(int typ, const char* tn, const char* otkr, const char* qq);

int ado_info_PR(CWnd *wnd, CAdoFile *ado, CNode2 *node, const char *tn1, int id1, const char *tn2, int id2, int fileID, const char *tn_out, int idgP, int idgO, int idPT, const char *capt, CString & cxema);
int ado_info_IS(CWnd *wnd, CAdoFile *ado, CNode2 *node, const char *tn1, int id1, const char *tn2, int id2, int fileID, const char *tn_out, int idgP, int idgO, int idPT, const char *capt, CString & cxema);

int ado_info_line_out(CWnd *wnd, CAdoFile *ado, const char *tn1, int id1, const char *tn2, int id2, int idN1, int idN2, int fileID, const char *tn_out, int idgP, int idgO, const char *capt);
int ado_info_node_out(CWnd *wnd, CAdoFile *ado, const char *tn1, int id1, const char *tn2, int id2, int fileID, const char *tn_out, int idgP, int idgO, int idPT, const char *capt);

int ado_info_node_out_DR(CWnd *wnd, CAdoFile *ado, const char *tn1, int id1, const char *tn2, int id2, int fileID, const char *tn_out1, int idPT, const char *capt);

int ado_info_node2_vnutr(CWnd *wnd, CAdoFile *ado, CNode2 *node, const char *tn1, int id1, const char *tn2, int id2, int fileID, const char *tn_out1, int idgP, int idgO, int idPT, const char *capt);

int ado_info_node2(CWnd *wnd, CAdoFile *ado, CNode2 *node, const char *tn1, int id1, const char *tn2, int id2, int fileID, const char *tn_out, int idgP, int idgO, int idPT, const char *capt);
int ado_info_line2(CWnd *wnd, CAdoFile *ado, CLINE2 *line, const char *tn1, int id1, const char *tn2, int id2, int idN1, int idN2, int fileID, const char *tn_out, int idgP, int idgO, const char *capt);

bool delObject(CAdoFile *ado, const char *tn, int id);
bool delNodes(CAdoFile *ado, const char *tn, int id);
bool delLinesobj(CAdoFile *ado, const char *tn, int idP, int idO);

const char *get_idrp(const char *name);

int ado_info_node2(CWnd *wnd, CAdoFile *ado, CNode2 *node);


// Исправить наименование в узле присоединения

bool ispr_us2(CAdoFile *ado, int id)
{
    long affected = 0;

    CString q;

    q.Format(
      "UPDATE nc "
      "SET  "
      "    nc.externalNodeName=n.externalNodeName, "
      "    nc.externalCodeID=n.externalCodeID, "
      "    nc.nodeName=n.nodeName "
      "FROM nodes nc "
      "JOIN connectNodes cn ON cn.nodeID=nc.id "
      "JOIN nodes n ON n.id=cn.connectID "
      "WHERE n.id=%d", id);

    bool ret = ado->Execute(q, &affected);

    return ret && affected > 0;
}

bool CGidrView::setXY1(CNode2 *node, CFPoint &pt)
{
#if 0
  if (!node) return true;

  CAdoFile* ado = node->node.m_graph->m_cxema->m_ado;

  if (node->node.nomP == 0 || node->node.nomO == 0 || node->node.typ != TIP_US) {
        return setXY(node, pt);
  }

  if ((node->node.nomP > 0 || node->node.nomO > 0) && node->node.typ >= 0) {

    if (node->node.nomO == node->node.nomP) {
      CLINE2 *l;
      for ( l = node->lines; l; l = next(l)) {
        if (!m_dbf->openTable(linef[bline(l)->typ].table)) return false;
        split(bline(l));
        m_dbf->closeTable();
      }
    }
    
    if (!m_dbf->openTable(nodef[node->typ].table)) return false;
    if (node->node.nomO == node->node.nomP) {
      split(node);
    }

    if (m_dbf->GotoKey(node->nomO)) {
      m_dbf->Edit();
      m_dbf->write_xy(pt);
      m_dbf->Update();
    }

    m_dbf->closeTable();

    reopenCxema(node->file, NULL);
  }
#endif
  return true;
}

bool CGidrView::info(CCxema *cxema, CNode2 *node, CLINE2 *line, CWnd *wnd)
{
    if (!wnd) wnd = this;

    if (node) {
        CString capt = node->getTable() + " (" + node->getFileName() + ")";

        BeginDrawNode(node);
        if (node->node.typ == TIP_PR && 0) {
            ado_info_PR(wnd, cxema->m_ado, node, "nodes", node->id, node->getTableMySQL(), node->node.id2, node->node.fileID, node->getTableOutMySQL(), node->node.nomgP, node->node.nomgO, node->node.nomPT, capt, node->node.schemeNum);
        }
        else if (node->node.typ == TIP_IS) {
            ado_info_IS(wnd, cxema->m_ado, node, "nodes", node->id, node->getTableMySQL(), node->node.id2, node->node.fileID, node->getTableOutMySQL(), node->node.nomgP, node->node.nomgO, node->node.nomPT, capt, node->node.schemeNum);
        }
        else {
            if (0 && node->node.internalNodeID) {
                ado_info_node2_vnutr(wnd, cxema->m_ado, node, "nodes", node->id, node->getTableMySQL(), node->node.id2, node->node.fileID, node->getTableOutMySQL(), node->node.nomgP, node->node.nomgO, node->node.nomPT, capt);
            }
            else {
                ado_info_node2(wnd, cxema->m_ado, node, "nodes", node->id, node->getTableMySQL(), node->node.id2, node->node.fileID, node->getTableOutMySQL(), node->node.nomgP, node->node.nomgO, node->node.nomPT, capt);
            }
            //      ado_info_node2(wnd, cxema->m_ado, node);
        }
    }
    else if (line) {
        BeginDrawLine(line);

        int nomP = bline(line)->line.nomP;
        int idP2 = bline(line)->line.idP2;
        int nomO = bline(line)->line.nomO;
        int idO2 = bline(line)->line.idO2;
        int nom = nomP, id2 = idP2;

        if (nomP != nomO && nomP != -1 && nomO != -1) {
            CMMenu menu(AfxGetMainWnd(), IDD_MENU);
            menu.lst.Add(_TR("Подающий"));
            menu.lst.Add(_TR("Обратный"));

            if (menu.DoModal() == IDOK) {
                if (menu.m_Index == 1) {
                    nom = nomO;
                    id2 = idO2;
                }
            }
            else {
                return false;
            }
        }
        if (nomP == -1) {
            nom = nomO;
            id2 = idO2;
        }
        CString capt = bline(line)->getTable() + " (" + where(line)->getFileName() + ")";

        ado_info_line2(wnd, cxema->m_ado, line, "linesobj", nom, bline(line)->getTableMySQL(), id2, where(line)->id, other(line)->id, where(line)->node.fileID, bline(line)->getTableOutMySQL(), bline(line)->line.nomgP, bline(line)->line.nomgO, capt);
    }
    return false;
}


bool CGidrView::info_gid(CCxema *cxema, const CNode2 *node, const CLINE2 *line, CWnd *wnd)
{
  if (!wnd) wnd = this;
  
  if (node) {
    BeginDrawNode(node);
    ado_info_node_out(wnd, cxema->m_ado, "nodes", node->id, node->getTableMySQL(), node->node.id2, node->node.fileID, node->getTableOutMySQL(), node->node.nomgP, node->node.nomgO,
        node->node.nomPT, node->getTable());
  }
  else if (line) {
    BeginDrawLine(line);

    int nomP = bline(line)->line.nomP;
    int idP2 = bline(line)->line.idP2;
    int nomO = bline(line)->line.nomO;
    int idO2 = bline(line)->line.idO2;
    int nom = nomP, id2 = idP2;

    if (nomP != nomO && nomP != -1 && nomO != -1) {
      CMMenu menu(AfxGetMainWnd(), IDD_MENU);
      menu.lst.Add(_TR("Подающий"));
      menu.lst.Add(_TR("Обратный"));

      if (menu.DoModal() == IDOK) {
        if ( menu.m_Index == 1) {
          nom = nomO;
          id2 = idO2;
        }
      }
      else {
        return false;
      }
    }
    if (nomP == -1) {
      nom = nomO;
      id2 = idO2;
    }
    ado_info_line_out(wnd, cxema->m_ado, "linesobj", nom, bline(line)->getTableMySQL(), id2, where(line)->id, other(line)->id, where(line)->node.fileID, bline(line)->getTableOutMySQL(), bline(line)->line.nomgP, bline(line)->line.nomgO, bline(line)->getTable());
  }

  return false;
}



bool CGidrView::info_gid_DR(CCxema *cxema, const CNode2 *node)
{
  if (node) {
    BeginDrawNode(node);
    ado_info_node_out_DR(this, cxema->m_ado, "nodes", node->id, node->getTableMySQL(), node->node.id2, node->node.fileID, "DR_OUT", node->node.nomDR, node->getTable());
  }

  return false;
}



bool CGidrView::infoZN(CCxema *cxema, CNode2 *node)
{
  if (node) {
    BeginDrawNode(node);
    ado_info_node2(this, cxema->m_ado, node, "nodes", node->id, "setPressNodes", node->node.nZN, node->node.fileID, "", -1, -1, -1, _TR("Узел с заданным напором"));
  }

  return false;
}

bool CGidrView::infoUP(CCxema *cxema, CNode2 *node)
{
  if (node) {
    BeginDrawNode(node);
    ado_info_node2(this, cxema->m_ado, node, "nodes", node->id, "refillNodes", node->node.nUP, node->node.fileID, "", -1, -1, -1, _TR("Узел подпитки"));
  }

  return false;
}

bool CGidrView::infoVP(CCxema *cxema, CNode2 *node)
{
  if (node) {
    BeginDrawNode(node);
    ado_info_node2(this, cxema->m_ado, node, "nodes", node->id, "WDOdevices", node->node.nVP, node->node.fileID, "", -1, -1, -1, _TR("Водоразборный прибор"));
  }

  return false;
}


/*
bool CGidrView::addLine(int typ, CNode2 *n1, CNode2 *n2, CCoordList &cl)
{
  if (n1 == n2 || !n1 || !n2) return false;

  int fileID1 = n1->node.file->id;
  int fileID2 = n1->node.file->id;

  if (fileID1 != fileID2) return false;

  CString q;
  CString tn = getLineTableMySQL(typ);
  CString sc = cl.saveStr();
  long affected;

  q.Format("INSERT INTO SELECT nodeID1, nodeID2, fileID, coords FROM linesobj, %s (nodeID1, nodeID2, fileID, coords) VALUES (%d,%d,%d,'%s')", tn, n1->id, n2->id, fileID1, sc);
  long id = ado_executeInsert(q, &affected);

  CLINE2 *line = m_cxema->m_graph->insert_line(n1, n2, cl);
  if (line) {
    CLine2 *l = bline(line);
    l->line.typ = typ;
    l->line.nomP = id;
    l->line.nomO = id;

    Invalidate();
    GetDocument()->SetModifiedFlag(true);
  }
  

  return true;
}
*/


// AfxGetMainWnd()

int getLineTypMenu(CWnd *wnd)
{
    int i;
    CMMenu menu(wnd, IDD_MENU, _TR("Установка типа линии"));

    for (i = 0; i < linefsize; i++) {
        if (linef[i].typ != TIP_EL && linef[i].typ != TIP_RO && linef[i].typ != TIP_TO && linef[i].typ != TIP_KU) {
            menu.Add(linef[i].opis, (void*)linef[i].typ);
        }
    }
    if (menu.DoModal() == IDOK) {
        return (int)menu.getV();
    }
    return -1;
}


// Добавить узел во внутреннюю схему

bool CGidrView::initUS1(const CNode2* parent, const CNode2* o)
{
    double R = 2000, X0 = 40000, Y0 = -40000;

    CAdoFile* ado = m_cxema->m_ado;
    CGraph2* m_graph = m_cxema->m_graph;

    double fi = 0, dR = R / 6;

    CNode2* nP = NULL, * nO = NULL;

    fi = atan2(o->node.coord.y - parent->node.coord.y, o->node.coord.x - parent->node.coord.x);

//    fi = atan2(o->node.x - parent->node.x, o->node.y - parent->node.y);


        CNode2* n1P = NULL, * n1O = NULL;
//        o = n2;
        double cosfi = cos(fi);
        double sinfi = sin(fi);
        sinfi = (o->node.coord.x - parent->node.coord.x) / hypot(o->node.coord.x - parent->node.coord.x, o->node.coord.y - parent->node.coord.y);
        cosfi = (o->node.coord.y - parent->node.coord.y) / hypot(o->node.coord.x - parent->node.coord.x, o->node.coord.y - parent->node.coord.y);

//        double diamP = bline(l)->line.pod.diam_usl;
//        double diamO = bline(l)->line.obr.diam_usl;

        CFPoint ptP = CFPoint(X0 + R * sinfi + dR, Y0 + R * cosfi - R / 4);
        CFPoint ptO = CFPoint(X0 + R * sinfi, Y0 + R * cosfi + R / 4);

        //        if (bline(l)->line.nomP > 1 && bline(l)->line.nomO > 1) {
        if (o->node.externalSignID != SignNode_o) {
            n1P = addUS2(o->node.fileID, parent->id, o->id, 2, ptP);
        }
        if (o->node.externalSignID != SignNode_p) {
            n1O = addUS2(o->node.fileID, parent->id, o->id, 3, ptO);
        }




    return true;
}




bool CGidrView::addLine(CNode2 *n1, CNode2 *n2, CCoordList &cl)
{
   regim = R_INIT;
   m_nRegim = ID_INFO;

  if (n1 == n2 || !n1 || !n2) return false;

  int fileID1 = n1->node.fileID;
  int fileID2 = n2->node.fileID;

  if (fileID1 != fileID2) {
      AfxMessageBox("Начальный и конечный узел должны относиться к одному фрагменту!");
      return false;
  }

  int externalSignLineID = SignNode_ob;

/*
  if (n1->node.externalSignID == SignNode_p && n2->node.externalSignID == SignNode_p) externalSignLineID = SignLine_pp;
  if (n1->node.externalSignID == SignNode_o && n2->node.externalSignID == SignNode_o) externalSignLineID = SignLine_oo;
  if (n1->node.externalSignID == SignNode_p && n2->node.externalSignID == SignNode_o) externalSignLineID = SignLine_po;
  if (n1->node.externalSignID == SignNode_o && n2->node.externalSignID == SignNode_p) externalSignLineID = SignLine_op;
*/

  if (n1->node.externalSignID == SignNode_p && n2->node.externalSignID == SignNode_p) externalSignLineID = SignLine_pp;
  if (n1->node.externalSignID == SignNode_o && n2->node.externalSignID == SignNode_o) externalSignLineID = SignLine_oo;
  if (n1->node.externalSignID == SignNode_p && n2->node.externalSignID == SignNode_o) externalSignLineID = SignLine_po;
  if (n1->node.externalSignID == SignNode_o && n2->node.externalSignID == SignNode_p) externalSignLineID = SignLine_op;


  if (n1->node.externalSignID == SignNode_ob && n2->node.externalSignID == SignNode_p) externalSignLineID = SignLine_pp;
  if (n1->node.externalSignID == SignNode_ob && n2->node.externalSignID == SignNode_o) externalSignLineID = SignLine_oo;

  if (n1->node.externalSignID == SignNode_p && n2->node.externalSignID == SignNode_ob) externalSignLineID = SignLine_pp;
  if (n1->node.externalSignID == SignNode_o && n2->node.externalSignID == SignNode_ob) externalSignLineID = SignLine_oo;


  int typ = TIP_UT;

  if (m_parent_id) {
      typ = getLineTypMenu(this);
      if (typ < 0) return false;
  }
  
  long idv;
  long id = ::addLine(m_cxema->m_ado, typ, n1, n2, externalSignLineID, cl, idv);

  if (id > 0) {

    int idRem = addRemoved(m_cxema->m_ado, CH_T_ADD_LINE, id, "");

    CLINE2 *line = m_cxema->m_graph->insert_line(n1, n2, cl);
    if (line) {
      CLine2 *l = bline(line);
      l->line.typ = typ;

      if (externalSignLineID == SignLine_ob || externalSignLineID == SignLine_pp || externalSignLineID == SignLine_po) {
        l->line.nomP = id;
        l->line.idP2 = idv;
      }
      if (externalSignLineID == SignLine_ob || externalSignLineID == SignLine_oo || externalSignLineID == SignLine_op) {
        l->line.nomO = id;
        l->line.idO2 = idv;
      }


      if (n1->node.isP && n1->node.typ != TIP_PR && n2->node.typ != TIP_PO) {
          initUS1(n1, n2);
      }
      if (n2->node.isP && n2->node.typ != TIP_PR && n2->node.typ != TIP_PO) {
          initUS1(n2, n1);
      }



      Invalidate();
      GetDocument()->SetModifiedFlag(true);

//      PtsAddLine(line);

      info(m_cxema, NULL, l);
    }
  }

  return true;
}

int getCurrentExternalCode(int fileID);

/*

CString get_init_value(int fileID, const char * fn, map<int, map <CString, CString> > &map_v)
{
    map<int, map <CString, CString> >::const_iterator it1 = map_v.find(fileID);

    if (it1 != map_v.end()) {

        map<CString, CString>::const_iterator it2 =  it1->second.find(fn);
        if (it2 != it1->second.end()) {
            return it2->second;
        }
    }
    return "";
}


bool write_def(CAdoFile *ado, int fileID, const char * fn, map<int, map <CString, CString> > &map_v)
{
    map<int, map <CString, CString> >::const_iterator it1 = map_v.find(fileID);

    if (it1 != map_v.end()) {

        map<CString, CString>::const_iterator it2 = it1->second.find(fn);
        if (it2 != it1->second.end()) {
            ado->write(fn, it2->second);
        }
    }
    return true;
}

bool write_def(CAdoFile *ado, const char * fn, map <CString, CString> &map_v)
{
    map <CString, CString>::const_iterator it = map_v.find(fn);

    if (it != map_v.end()) {
        ado->write(fn, it->second);
    }
    return true;
}
*/

bool write_def(CAdoFile *ado, int fileID, const char * fn, map <int, int> &map_v)
{
    map <int, int>::const_iterator it = map_v.find(fileID);

    if (it != map_v.end()) {
        ado->write(fn, it->second);
    }
    return true;
}

int get_init_value(int fileID, const char * fn, map<int, int> &map_v)
{
    map<int, int>::const_iterator it = map_v.find(fileID);

    if (it != map_v.end()) {
            return it->second;
    }
    return -1;
}



bool isprLine(CFPoint n, CFPoint n1, CFPoint n2, CCoordList &cl0, CCoordList &cl1, CCoordList &cl2, double r)
{
    cl1.clear();
    cl2.clear();

    bool first = true;

    CCoordList cl;
    
    cl = cl0;
    cl.push_front(n1);
    cl.push_back(n2);


    CCoordList::const_iterator it = cl.begin();

    CFPoint p1 = n1;
    it++;

    cl1.push_back(p1);

    for (; it != cl.end(); ++it) {
        CFPoint p2 = *it;

        double len = LengthPL(n.x, n.y, p1.x, p1.y, p2.x, p2.y);

        if (first && len < r) {
            cl1.push_back(n);
            first = false;
            cl2.push_back(n);
        }

        if (first) {
            cl1.push_back(p2);
        }
        else {
            cl2.push_back(p2);
        }
        p1 = p2;
    }

    cl1.pop_back();
    cl2.pop_back();
    cl1.pop_front();
    cl2.pop_front();

    return true;
}


// Копирование linesobj для разбивки

long getLastID(CAdoFile *ado, CString tn);

int copyRowLine2(CAdoFile *ado, CString tn, int id, int lineID);


int copyRowLine1Dubl(CAdoFile *ado, CString tn, int nodeID1, int id, CString coord)
{
    CString s;
    int i, n = ado->NColsMySQL_S(ado->m_schema, tn);

    CString params1 = "";
    CString params2 = "";

    for (i = 0; i < n; i++) {
        MySQLColumns *col = ado->findMySQLColumnsOrdS(ado->m_schema, tn, i + 1);
        if (col) {

            if (col->strCOLUMN_NAME != "id") {
                if (params1 != "") params1 += ",";
                if (params2 != "") params2 += ",";

                params1 += col->strCOLUMN_NAME;

                if (col->strCOLUMN_NAME == "coords") {
                    s.Format("'%s'", coord);
                    params2 += s;
                }
                else if (col->strCOLUMN_NAME == "nodeID1") {
                    s.Format("%d", nodeID1);
                    params2 += s;
                }
                else {
                    params2 += col->strCOLUMN_NAME;
                }
            }
        }
    }

    CString q;

    long affected;

    q.Format("insert into %s (%s) select %s from %s where id = %d", tn, params1, params2, tn, id);
    int id2 = ado->ExecuteInsert(q, &affected);
    id2 = getLastID(ado, tn);

    return id2;
}



bool CGidrView::insertNode(CNode2 *n, CLINE2 *line, double r)
{
    CCoordList cl1, cl2;

    CAdoFile *ado = m_cxema->m_ado;


    CNode2 *n1 = where(line), *n2 = other(line);
    CLine2 *l = bline(line);

    isprLine(n->node.coord, n1->node.coord, n2->node.coord, l->line.m_NP, cl1, cl2, r);

    CString s1_coords = cl1.saveStr();
    CString s2_coords = cl2.saveStr();

//    l->line.m_NP = cl1;

    int nomP2 = -1, nomO2 = -1;
    int idP2 = -1, idO2 = -1;

    CString q;

    long affected;

    if (l->line.nomP > 0) {
        nomP2 = copyRowLine1Dubl(ado, "linesobj", n->id, l->line.nomP, s2_coords);
        idP2 = copyRowLine2(ado, l->getTableMySQL(), l->line.idP2, nomP2);
/*

        int id = (int)lParam;
        CLine2 *l = bline(m_cxema->m_graph->findLine(id));
        if (!l) return 0;

        qq.Format("l1.id=%d", id);
        CString q = getLineQ(l->line.typ, l->getTableMySQL(), getLineOtkr(l->line.typ), qq);
        if (ado->openTable0(q)) {
            if (!ado->isEOF()) {
                l->line.nomP = -1;
                l->line.nomO = -1;

                readLine(ado, l, l->line.typ, m_cxema->m_graph, l->getTableMySQL(), getLineOtkr(l->line.typ));
            }
            ado->closeTable();
            Invalidate();
        }
*/
    }


    if (l->line.nomO > 0 && l->line.nomO != l->line.nomP) {
        nomO2 = copyRowLine1Dubl(ado, "linesobj", n->id, l->line.nomO, s2_coords);
        idO2 = copyRowLine2(ado, l->getTableMySQL(), l->line.idO2, nomO2);
    }

    if (l->line.nomO == l->line.nomP) {
        if (nomO2 == -1) {
            nomO2 = nomP2;
            idO2 = idP2;
        }
        if (nomP2 == -1) {
            nomP2 = nomO2;
            idP2 = idO2;
        }
    }



    q.Format("UPDATE linesobj SET coords='%s', nodeID2=%d WHERE id IN (%d, %d)", s1_coords, n->id,l->line.nomP, l->line.nomO);
    bool ret = ado->Execute(q, &affected);


    q.Format("UPDATE cn SET connectID=%d FROM connectNodes cn WHERE connectID IN (%d,%d)", n->id, n1->id, n2->id);
    bool ret2 = ado->Execute(q, &affected);
    ispr_us2(ado, n->id);



    CLINE2 *line1 = m_cxema->m_graph->insert_line(n1, n, cl1);
    if (line1) {
        bline(line1)->line = l->line;
        bline(line1)->line.m_NP = cl1;
    }

    CLINE2 *line2 = m_cxema->m_graph->insert_line(n, n2, cl2);
    if (line1) {
        bline(line2)->line = l->line;

        bline(line2)->line.nomP = nomP2;
        bline(line2)->line.nomO = nomO2;

        bline(line2)->line.idP2 = idP2;
        bline(line2)->line.idO2 = idO2;
        bline(line2)->line.m_NP = cl2;
    }

    m_cxema->m_graph->remove_line(line);





//    int id2 = copyRowLine2(ado, cl->getTableMySQL(), cl->line.idP2, nomO);


    /**

    int nomP = -1, nomO = -1, nomP1 = l->nomP, nomO1 = l->nomO, typ = l->typ;
    UTNapr pod = l->pod, obr = l->obr;
    char po;
    CNode *n1, *n2, *n = NULL;
    CString str;

    CGidFile *gid_file = where(l)->file;
    CDbFile *m_dbf = gid_file->m_dbf;

    po = (l->nomP != -1 && l->nomO != -1) ? ' ' : (l->nomP != -1) ? CPOD : COBR;

    n1 = where(l);
    n2 = other(l);

    //  CString new_name = get_new_name(m_dbf, m_kod);
    CString new_name = get_new_name(n1->file, m_kod);

    CCoordList cl1, cl2;



        if (!m_dbf->openTable(linef[l->typ].table)) return false;

        if (isprLine(m_dbf, l->nomP, new_name, po, cl1, cl2)) {
            m_dbf->rs->SetBookmark(m_dbf->rs->GetLastModifiedBookmark());
            m_dbf->read0(nomP);
            if (l->nomP == l->nomO) nomO = nomP;
        }

        if (l->nomP != l->nomO && l->nomO != -1) {
            if (isprLine(m_dbf, l->nomO, new_name, po, cl1, cl2)) {
                m_dbf->rs->SetBookmark(m_dbf->rs->GetLastModifiedBookmark());
                m_dbf->read0(nomO);
            }
        }

        m_dbf->closeTable();

        m_graph->remove(l);

        CLINE *line;

        line = m_graph->insert_line(n1, n, cl1);
        if (line) {
            bline(line)->typ = typ;
            bline(line)->nomP = nomP1;
            bline(line)->nomO = nomO1;
            bline(line)->pod = pod;
            bline(line)->obr = obr;
        }

        line = m_graph->insert_line(n, n2, cl2);
        if (line) {
            bline(line)->typ = typ;
            bline(line)->nomP = nomP;
            bline(line)->nomO = nomO;
            bline(line)->pod = pod;
            bline(line)->obr = obr;
        }

        if (n1->isP && n1->typ != TIP_PR && n1->typ != TIP_IS) ch_end_name(m_dbf->db, &n1->node_name, &n2->node_name, &n->node_name);
        if (n2->isP && n2->typ != TIP_PR && n2->typ != TIP_IS) ch_end_name(m_dbf->db, &n2->node_name, &n1->node_name, &n->node_name);

        copy_mestn(n1->file, n1->node_name, n2->node_name, n->node_name);
*/


    return true;
}

CString findTableRusName(CString d, CString n);


bool CGidrView::addNode(CFPoint &point)
{
   regim = R_INIT;
   m_nRegim = ID_INFO;

  CNode2 *node = m_cxema->m_graph->find(m_parent_id, point, geom.masx*D5/mas_otn);
  CLINE2 *line = m_cxema->m_graph->findLine(m_parent_id, point, geom.masx*D5/mas_otn);
  CString str;

  if (!isEditGid()) return false;
  if ( isPot() ) return false;

  if (!curFile() && m_parent_id == 0) {
    AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK|MB_ICONINFORMATION);
    return false;
  }

  int fileID = 0;
  if (m_parent_id == 0) {
      fileID = m_cxema->m_cur_file->id;
  }
  else {
      CNode2 *np = m_cxema->m_graph->find(m_parent_id);
      fileID = np->node.fileID;
  }


  if (node) {
      if (node->node.m_graph == m_cxema->m_graph) {
          str.Format(_TR("Слишком близко к %s %s"), node->getTable(), node->getName());
          AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
          return false;
      }
  }

  bool is_insert = false;

  if (line && where(line)->node.m_graph == m_cxema->m_graph) {
    fileID = where(line)->node.fileID;
    if (m_cxema->m_cur_file->id != fileID) {
      CGidFile *file = m_cxema->getGidFile(fileID);
      if (file) {
        CString s, rus_name = findTableRusName(file->m_schema, file->name);
        s.Format(_TR("Для выполнения данной операции переключитесь на фрагмент %s"), rus_name);
        AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);
      }
      return false;
    }

    str.Format(_TR("Вставить Узел в %s %s, После вставки узла, установите новые длины и местные сопротивления на созданных участках"), /*getNodeTable(typ),*/  bline(line)->getTable(), bline(line)->getName());
    //    str.Format(_TR("Слишком близко к %s %s"), bline(line)->getTable(), bline(line)->getName());
    if (AfxMessageBox(str, MB_YESNO) == IDYES) {
        is_insert = true;
    }
    else {
        return false;
    }
  }

  m_CurNode = nullptr;
  m_CurLine = nullptr;


  int typ = TIP_US;

  int externalSignID = 0;

  if (m_parent_id == 0) {
    CMMenu menu(AfxGetMainWnd(), IDD_MENU, _TR("Установка типа узла"));

    for (int i = 0; i < nodefsize; i++) {

        if (nodef[i].typ != TIP_US2) {
            menu.Add(nodef[i].opis, (void*)nodef[i].typ);
        }
    }
    if (menu.DoModal() != IDOK) return false;

    typ = (int)menu.getV();
    externalSignID = 1;
  }
  else {
    CMMenu menu(AfxGetMainWnd(), IDD_MENU);
    menu.lst.Add(_TR("Подающий"));
    menu.lst.Add(_TR("Обратный"));

    if (menu.DoModal() != IDOK) return false;

    externalSignID = menu.m_Index == 0 ? 2 : 3;
  }

  {
//    int externalSignID = m_parent_id == 0 ? 1 : 2;
    int externalCodeID = getCurrentExternalCode(fileID);

    CAdoFile *ado = m_cxema->m_ado;

    double tn_1 = 8;
    double tn_5 = -32;

    if (typ == TIP_IS) {  // Температуры 
        if (ado->openTable0("SELECT * FROM heatSystem")) {
          if ( !ado->isEOF() ) {
            tn_1 = ado->read_double("t_vnew");
            tn_5 = ado->read_double("t_or");
          }
          ado->closeTable();
        }
    }

    long id = ::addNodeUS(m_cxema->m_ado, "nodes", "", fileID, externalSignID, m_parent_id, externalCodeID, point);

    if (id > 0) {
      int idRem = addRemoved(m_cxema->m_ado, CH_T_ADD_NODE, id, "");
 
      long id2 = 0;
      

      if (typ == TIP_PR || typ == TIP_PO || typ == TIP_IS || typ == TIP_V_PT || typ == TIP_V_IS) {
          CAdoFile *ado = m_cxema->m_ado;
          if (ado) {
              ado->AddNew(getNodeTableMySQL(typ));
              ado->write("nodeID", id);

              if (typ == TIP_PR || typ == TIP_PO) {   // Предыдущие значения
                  write_def(ado, fileID, "specExpendID", m_cxema->map_init_UR);
                  write_def(ado, fileID, "varCoeffID", m_cxema->map_init_KV);
                  write_def(ado, fileID, "calcTemperatureID", m_cxema->map_init_TR);
              };

              if (typ == TIP_PR) {
                 ado->write("schemeNum", N_1_3);
              }

              if (typ == TIP_IS) {  // Температуры 
                  ado->write("tn_1", tn_1);  // 8,  'Температура наружного воздуха в 1-ой характерной точке графика (начало/конец отопительного сезона), Град.С',
                  ado->write("tn_5", tn_5);  // -32, 'Температура наружного воздуха в 5-ой характерной точке графика (расчетная для отопления), Град.С',
              }
              id2 = ado->Update();
          }
//          id2 = ::addNode2(m_cxema->m_ado, getNodeTableMySQL(typ), id);
      }
      
      CNode2 *n = m_cxema->m_graph->ins(id);

      if (n) {
        n->node._typPS = PS_NEUST;
        n->node.coord = point;
        n->node.typ = typ;
//        n->node.file = m_cxema->getGidFile(fileID);

        n->node.id2 = id2;
        n->node.fileID = fileID;
        n->node.name = "";
        n->node.externalSignID = externalSignID;
        n->node.schemeNum = N_1_3;
        n->node.id_kod = externalCodeID;

        n->node.internalNodeID = m_parent_id;

        if (m_cxema->m_graph->map_node.size() == 1) m_rectBorder = CFRect(1000, 1000, -1000, -1000);

        m_rectBorder.AddPoint(point);


        if (is_insert) {
            insertNode(n, line, geom.masx*D5 / mas_otn);
        }

        if (typ == TIP_PR) {
           create_b5(m_cxema->m_ado, n, m_cxema);
        }

        if (typ == TIP_PR || typ == TIP_PO) {
           n->node.isValid = false;
        }


        Invalidate();
        GetDocument()->SetModifiedFlag(true);
        info(m_cxema, n, NULL);
      }
    }
  }


  return true;
}


CNode2 * CGidrView::addUS2(int fileID, int parent_id, int connectID, int externalSignID, CFPoint &point)
{

   regim = R_INIT;
   m_nRegim = ID_INFO;

  CString str;
  CNode2 *n = NULL;

   regim = R_INIT;
   m_nRegim = ID_INFO;

  if (!isEditGid()) return NULL;
  if ( isPot() ) return NULL;


  CNode2 *nc = m_cxema->m_graph->find(connectID);

  if (!nc) return NULL;

  int typ = TIP_US2;

  {
    long affected;
    CString q;
//    q.Format("INSERT INTO nodes (x,y,fileID, internalNodeID, externalSignID, operatorID) VALUES (%lf, %lf,%d,%d, %d, %d)", point.x, point.y, fileID, parent_id, externalSignID, m_userRemoved);

    q.Format("INSERT INTO nodes (externalNodeName, externalCodeID, x,y,fileID, internalNodeID, externalSignID, operatorID) VALUES ('%s',%d,%lf, %lf,%d,%d, %d, %d)", 
       
        nc->node.name,
        nc->node.id_kod,
        point.x, point.y, fileID, parent_id, externalSignID, m_userRemoved
        
        );

//    AfxMessageBox(CString("1\n")+q, MB_OK|MB_ICONINFORMATION);

    int id = ado_executeInsert(m_cxema->m_ado, q, &affected);



    if (id > 0) {
      q.Format("INSERT INTO connectNodes (nodeID,connectID) VALUES (%d,%d)", id, connectID);

//      AfxMessageBox(CString("2\n") + q, MB_OK|MB_ICONINFORMATION);

      int id2 = ado_executeInsert(m_cxema->m_ado, q, &affected);

//      q.Format("id2=%d", id2);
//      AfxMessageBox(q, MB_OK|MB_ICONINFORMATION);

      n = m_cxema->m_graph->ins(id);

      if (n) {
//        AfxMessageBox("n", MB_OK|MB_ICONINFORMATION);
        n->node.coord = point;
        n->node.typ = typ;
//        n->node.file = m_cxema->getGidFile(fileID);

        if (externalSignID == 1 || externalSignID == 2) {
          n->node.nomP = id;        
        }
        if (externalSignID == 1 || externalSignID == 3) {
          n->node.nomO = id;        
        }
            
        n->node.id2 = id2;
        n->node.fileID = fileID;

        n->node.name = nc->node.name;
        
        n->node.externalSignID = externalSignID;
        
        n->node.internalNodeID = parent_id;

        Invalidate();
        GetDocument()->SetModifiedFlag(true);
      }
    }
  }
  return n;
}


// Добавить Узет TIP_US


CNode2 * CGidrView::addUS1(int fileID, int parent_id, int externalSignID, CFPoint &point)
{
   regim = R_INIT;
   m_nRegim = ID_INFO;

  CString str;

  CNode2 *n = NULL;

  if (!isEditGid()) return NULL;
  if ( isPot() ) return NULL;


  CNode2 *nc = m_cxema->m_graph->find(parent_id);

  if (!nc) return NULL;

  {
    long affected;
    CString q;
    q.Format("INSERT INTO nodes (externalNodeName, externalCodeID, x,y,fileID, internalNodeID, externalSignID, operatorID) VALUES ('%s',%d,%lf, %lf,%d,%d, %d, %d)", 
        nc->node.name,
        nc->node.id_kod,
        point.x, point.y, fileID, parent_id, externalSignID, m_userRemoved);

//    AfxMessageBox(q, MB_OK|MB_ICONINFORMATION);

    int id = ado_executeInsert(m_cxema->m_ado, q, &affected);

    if (id > 0) {

//      AfxMessageBox("111 id > 0", MB_OK|MB_ICONINFORMATION);
      
      n = m_cxema->m_graph->ins(id);

      if (n) {

//        AfxMessageBox("111 n > 0", MB_OK|MB_ICONINFORMATION);
        
        if (externalSignID == 1 || externalSignID == 2) {
          n->node.nomP = id;        
        }
        if (externalSignID == 1 || externalSignID == 3) {
          n->node.nomO = id;        
        }

        n->node.coord = point;
        n->node.typ = TIP_US;
            
        n->node.id2 = 0;
        n->node.fileID = fileID;

        n->node.id_kod = nc->node.id_kod;

        n->node.name = nc->node.name;
        n->node.externalSignID = externalSignID;
        
        n->node.internalNodeID = parent_id;

        Invalidate();
        GetDocument()->SetModifiedFlag(true);
      }
    }
  }
  return n;
}



bool CGidrView::delLine(CLINE2 *line, bool ask)
{
  CString str;

  int ret = IDYES;

  if (ask) {
    CDC *dc = SaveImage();

    BeginDrawLine(line);

    str.Format(_TR("Удалить %s %s ?"), findTableRusNameGid(bline(line)->getTable()), bline(line)->getName());
    ret = MessageBox(str, _TR("Подтвердить удаление"),  MB_YESNO);

    EndDrawLine();
    CloseImage(dc);
  }

  if (ret == IDYES) {
//     PtsDelLine(line);


    if (delLinesobj(m_cxema->m_ado, "linesobj", bline(line)->line.nomP, bline(line)->line.nomO)) {
      m_cxema->m_graph->remove_line(line);
      GetDocument()->SetModifiedFlag(true);
      if (ask) InvalidateG();
      return true;
    }
    else {
      if (ask) AfxMessageBox(getAdoError(), MB_OK|MB_ICONINFORMATION);
    }
  }
  return false;
}








CCoordList get_NP(const CLINE2 *l);


bool CGidrView::delNode2(CNode2 *node)
{
    if (node->node.typ == TIP_US2) return false;
    if (inc(node) != 2) return false;


    CLINE2 *l1, *l2;

    l1 = node->lines;
    l2 = next(l1);


    if (bline(l1)->line.typ != TIP_UT || bline(l2)->line.typ != TIP_UT) {
        AfxMessageBox(_TR("Обе линии должны быть Участками теплопровода"), MB_OK|MB_ICONINFORMATION);
        return false;
    }

    if (bline(l1)->line.nomP != bline(l1)->line.nomO || bline(l2)->line.nomP != bline(l2)->line.nomO) {
        AfxMessageBox(_TR("Удаление невозможно"), MB_OK|MB_ICONINFORMATION);
        return false;
    }

    UTNapr pod = bline(l1)->line.pod, obr = bline(l1)->line.obr;

    CNode2 *n1 = other(l1);
    CNode2 *n2 = other(l2);

    CCoordList cl1, cl2, m_NP;

    cl1 = get_NP(sym(l1));
    cl2 = get_NP(l2);

    //  CFPoint coord = node->coord;

    int x = node->node.coord.x;
    int y = node->node.coord.y;

    cl1.push_back(CFPoint(x, y));

    CCoordList::const_iterator it = cl2.begin();

    for (; it != cl2.end(); ++it) {
        cl1.push_back(*it);
    }

    m_NP = cl1;

    if (IsBegin(l1)) {
        cl1.Swap();
    }

    CString s1_coords = cl1.saveStr();

    double diametr1, diametr2, dlina;

    diametr1 = bline(l1)->line.pod.diam;
    diametr2 = bline(l2)->line.pod.diam;

    dlina = bline(l1)->line.pod.dl + bline(l2)->line.pod.dl;

    if (diametr1 != diametr2) {
        AfxMessageBox(_TR("Диаметры разные"), MB_OK|MB_ICONINFORMATION);
        return false;
    }

    if (AfxMessageBox(_TR("Удалить узел и объединить участки? Длины участков будут просуммированы."), MB_YESNO) != IDYES) {
        return false;
    }

    long affected;
    CString q;

    if (IsBegin(l1)) {
        q.Format("UPDATE linesobj SET coords='%s', nodeID1=%d WHERE id IN (%d, %d)", s1_coords, n2->id, bline(l1)->line.nomP, bline(l1)->line.nomO);
    }
    else {
        q.Format("UPDATE linesobj SET coords='%s', nodeID2=%d WHERE id IN (%d, %d)", s1_coords, n2->id, bline(l1)->line.nomP, bline(l1)->line.nomO);
    }

    bool ret = m_cxema->m_ado->Execute(q, &affected);

    q.Format("UPDATE heatPipeSections SET pipeSectLength=%g WHERE id IN (%d, %d)", dlina, bline(l1)->line.idP2, bline(l1)->line.idP2);
    ret = m_cxema->m_ado->Execute(q, &affected);


    delLine(l2, false);

    if (delNodes(m_cxema->m_ado, "nodes", node->id)) {
        m_cxema->m_graph->remove_node(*node);
    }

    CLine ll = bline(l1)->line;

    m_cxema->m_graph->remove_line(l1);



    CLINE2 *line = m_cxema->m_graph->insert_line(n1, n2, cl1);
    if (line) {
        bline(line)->line = ll;
        bline(line)->line.m_NP = m_NP;
    }

    GetDocument()->SetModifiedFlag(true);
    InvalidateG();
    return true;


#if 0
            m_dbf->write("dlina", dlina1 + dlina2);
            m_dbf->write("mestnoe", mestnoe1 + mestnoe2);
#endif
}


bool CGidrView::delNode(CNode2 *node, bool ask)
{
  CString str;

  if (node->lines) {
      if (delNode2(node)) {
        GetDocument()->SetModifiedFlag(true);
        InvalidateG();
        return true;
      }
      if (!ask) AfxMessageBox(_TR("Нельзя удалить узел с привязанными участками"), MB_OK|MB_ICONINFORMATION);
      return false;
  }

  if ( node->node.typ == TIP_US2 ) {
      if (!ask) AfxMessageBox(_TR("Нельзя удалять узел присоединения"), MB_OK|MB_ICONINFORMATION);
      return false;
  }


  if ( node->node.isP && node->node.typ != TIP_PR ) {
      if (!ask) AfxMessageBox(_TR("Нельзя удалять узел с внутренней схемой"), MB_OK|MB_ICONINFORMATION);
      return false;
  }

  int ret = IDYES;

  if (ask) {

    CDC *dc = SaveImage();

    BeginDrawNode(node);

    str.Format(_TR("Удалить %s %s ?"), findTableRusNameGid(node->getTable()), node->getName());
    ret = MessageBox(str, _TR("Подтвердить удаление"),  MB_YESNO);

    EndDrawNode();
    CloseImage(dc);
  }

  if (ret == IDYES) {
    if (node->node.isP) {
        delCxemaFull(m_cxema, node);
    }
    
    if (delNodes(m_cxema->m_ado, "nodes", node->id)) {
      m_cxema->m_graph->remove_node(*node);
      GetDocument()->SetModifiedFlag(true);
      if (ask) InvalidateG();
      return true;
    }
    else {
      if (ask) AfxMessageBox(getAdoError(), MB_OK|MB_ICONINFORMATION);
    }
  }
  return false;
}

bool CGidrView::setXY(CNode2 *n, CFPoint &pt)
{
  long affected;
  CString q;

  q.Format("UPDATE nodes SET X=%lf, Y=%lf WHERE ID=%d", pt.x, pt.y, n->id);
  if (ado_execute(m_cxema->m_ado, q, &affected)) {

    reset_shape_node(m_cxema->m_ado, n->id);

    CString qq;
    qq.Format("l.nodeID1=%d OR l.nodeID2=%d", n->id, n->id);
    reset_shape_line(m_cxema->m_ado, qq);

    CString text;
    text.Format("%g %g", n->node.coord.x, n->node.coord.y);
    addRemoved(m_cxema->m_ado, CH_T_MOVE_NODE, n->id, text);

    m_rectBorder.AddPoint(pt);

    n->node.coord = pt;
    m_cxema->m_graph->change();
    GetDocument()->SetModifiedFlag(true);
    InvalidateG();
    return true;
  }

  AfxMessageBox(getAdoError(), MB_OK|MB_ICONINFORMATION);
  return false;
}

bool setLineCoord(const char *tn, int idP, int idO, CCoordList &cl);

bool CGidrView::setCoord(CLINE2 *line, CCoordList &cl)
{
  CLine2 *l = bline(line);

  long affected;
  CString q;
  CString coord = cl.saveStr();

  q.Format("UPDATE linesobj SET coords='%s' WHERE ID=%d OR ID=%d", coord, l->line.nomP, l->line.nomO);

  if (ado_execute(m_cxema->m_ado, q, &affected)) {
    CString qq;
    qq.Format("l.id=%d OR l.id=%d", l->line.nomP, l->line.nomO);
    reset_shape_line(m_cxema->m_ado, qq);


    CString text = l->line.m_NP.saveStr();
    if (l->line.nomP > 0) {
      addRemoved(m_cxema->m_ado, CH_T_MOVE_LINE, l->line.nomP, text);
    }
    if (l->line.nomO > 0 && l->line.nomO != l->line.nomP) {
      addRemoved(m_cxema->m_ado, CH_T_MOVE_LINE, l->line.nomO, text);
    }

    l->line.m_NP = cl;
    m_cxema->m_graph->change();
    return true;
  }
  AfxMessageBox(getAdoError(), MB_OK|MB_ICONINFORMATION);
  return false;
}

CString getLineOtkr(int typ);


bool CGidrView::setState(CLINE2 *line, int state)
{
    CLine2 *l = bline(line);

    long affected;
    CString q;

    if (l->line.typ == TIP_UT) {
        q.Format("UPDATE %s SET pipeSectStateIDflow=%d, pipeSectStateIDret=%d WHERE ID=%d OR ID=%d", l->getTableMySQL(), state, state, l->line.idP2, l->line.idO2);
    }
    else {
        CString otkr = getLineOtkr(l->line.typ);
        if (otkr != "") {
            q.Format("UPDATE %s SET %s=%d WHERE ID=%d OR ID=%d", l->getTableMySQL(), otkr, state, l->line.idP2, l->line.idO2);
        }
    }

    //    q.Format("UPDATE %s SET state=%d WHERE ID=%d OR ID=%d",l->getTableMySQL(), state, l->line.idP2, l->line.idO2);

    if (ado_execute(m_cxema->m_ado, q, &affected)) {
        bool otkl = state == 2;
        l->line.pod.isOtkl = otkl;
        l->line.obr.isOtkl = otkl;

        Invalidate();
        return true;
    }
    return false;
}

bool CGidrView::moveLineEnd(CNode2 *n1, CNode2 *node, CLINE2 *line)
{
    CString str;
    CLine2 *l = bline(line);

    CNode2 *n2 = where(l);
    CCoordList NP = l->line.m_NP;

    bool swap = false;

    if (n2 == n1) {
        n2 = other(l);
    }
    else {
        NP.Swap();
        swap = true;
    }

    long affected;

    //  if (n1->node.externalSignID == SignNode_p && n2->node.externalSignID == SignNode_p) externalSignLineID = SignLine_pp;


    if (l->line.nomP > 0 && l->line.nomO > 0 && (node->node.nomP <= 0 || node->node.nomO <= 0)) {
        AfxMessageBox(_TR("Невозможно присоединить общий трубопровод к такому узлу"), MB_OK|MB_ICONINFORMATION);
        return false;
    }


    //  if (l->line.nomP <= 0 || l->line.nomO <= 0) {
    //  }

    CNode2 *nn1 = NULL, *nn2 = NULL;

//    long signLine = getExternalSignLineId(line);

    if ((l->line.nomP <= 0 || l->line.nomO <= 0) && (node->node.nomP <= 0 || node->node.nomO <= 0)) {
        if (node->node.nomP <= 0) {
        }
    }



    if (swap) {
        str.Format("UPDATE linesobj SET nodeID2=%d WHERE (ID=%d OR ID=%d)", 
            node->id, l->line.nomP, l->line.nomO);
    }
    else {
//        if ((l->line.nomP <= 0 || l->line.nomO <= 0) && ) {
//            if (where(line)->node.nomP);
//        }

        str.Format("UPDATE linesobj SET nodeID1=%d WHERE (ID=%d OR ID=%d)",
            node->id, l->line.nomP, l->line.nomO);
    }

    bool ret = ado_execute(m_cxema->m_ado, str, &affected);

    //  if (affected <= 0) {
    //    str.Format("UPDATE linesobj SET nodeID2=%d WHERE nodeID1=%d AND nodeID2=%d AND (ID=%d OR ID=%d)", node->id, n2->id, n1->id, l->line.nomP, l->line.nomO);
    //    ret = ado_execute(m_cxema->m_ado, str, &affected);
    //  }

    if (!ret) {
        AfxMessageBox(getAdoError(), MB_OK|MB_ICONINFORMATION);
        return false;
    }
    if (affected <= 0) {
        AfxMessageBox(_TR("Ошибка"), MB_OK|MB_ICONINFORMATION);
    }

    CLine line_old = l->line;

    m_cxema->m_graph->remove_line(line);

    CLINE2 *line2 = NULL; 
    
    if (swap) {
        line2 = m_cxema->m_graph->insert_line(n2, node, NP);
    }
    else {
        line2 = m_cxema->m_graph->insert_line(node, n2, NP);
    }

    if (line2) {
        bline(line2)->line = line_old;
    }

    GetDocument()->SetModifiedFlag(true);
    InvalidateG();
    return true;
}

bool CGidrView::swap(CLINE2 *line)
{
  CLine2 *l = bline(line);

//  ::setLineCoord("linesobj", l->line.nomP, l->line.nomO, cl)
  CString q;
  long affected;

  CCoordList cl = l->line.m_NP;
  cl.Swap();
  CString sc = cl.saveStr();
  
// externalSignID

  q.Format("UPDATE linesobj "
          " SET "
          "    externalSignLineID = case externalSignLineID when 4 then 5 when 5 then 4 else externalSignLineID end, "
          "    nodeID1 = nodeID2, "
          "    nodeID2 = nodeID1, "
          "    coords='%s' "
          " WHERE ID=%d OR ID=%d", sc, l->line.nomP, l->line.nomO);

//          " WHERE ID=%d OR ID=%d", other(l)->id, other(l)->id, where(l)->id, where(l)->id, sc, l->line.nomP, l->line.nomP);


  bool ret = ado_execute(m_cxema->m_ado, q, &affected);

  if (ret) {
    CString qq;
    qq.Format("l.id=%d OR l.id=%d", l->line.nomP, l->line.nomO);
    reset_shape_line(m_cxema->m_ado, qq);
    
    m_cxema->m_graph->swap(line);

    GetDocument()->SetModifiedFlag(true);
    InvalidateG();

    return true;
  }
  AfxMessageBox(getAdoError(), MB_OK|MB_ICONINFORMATION);
  return false;
}

bool CGidrView::renameNode(CNode2 *node, int typ)
{
    if (!node) return false;

    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) {
        return false;
    }


    int old_typ = node->node.typ;

    if (node->node.isP && node->node.typ != TIP_PR && node->node.typ != TIP_PO && (typ == TIP_PR || typ == TIP_PO)) {
        AfxMessageBox(_TR("Нельзя изменить тип узла с внутренней схемой на потребитель"), MB_OK|MB_ICONINFORMATION);
        return false;
    }
        

    if (typ != node->node.typ && delObject(m_cxema->m_ado, node->getTableMySQL(), node->node.id2)) {
        long id1 = 0, id2 = 0;
        CString tn = getNodeTableMySQL(typ);
        if (tn != "") {
            CString q;
            CString pp = "", vv = "", s;
            
            if (typ == TIP_PR || typ == TIP_PO) {   // Предыдущие значения
                int id1 = get_init_value(node->node.fileID, "calcTemperatureID", m_cxema->map_init_TR);
                if (id1 > 0) {
                    pp += ",calcTemperatureID";
                    s.Format(",%d", id1);
                    vv += s;
                }
                int id2 = get_init_value(node->node.fileID, "specExpendID", m_cxema->map_init_UR);
                if (id2 > 0) {
                    pp += ",specExpendID";
                    s.Format(",%d", id2);
                    vv += s;
                }
                int id3 = get_init_value(node->node.fileID, "varCoeffID", m_cxema->map_init_KV);
                if (id3 > 0) {
                    pp += ",varCoeffID";
                    s.Format(",%d", id3);
                    vv += s;
                }
            }

/*
            if (typ == TIP_PR || typ == TIP_PO) {   // Предыдущие значения
                write_def(ado, "calcTemperatureID", pt_def.map_v);
                write_def(ado, "specExpendID", pt_def.map_v);
                write_def(ado, "varCoeffID", pt_def.map_v);
            };
*/

            q.Format("INSERT INTO %s (nodeID %s) VALUES (%d %s)", tn, pp, node->id, vv);
            long affected;
            id2 = ado_executeInsert(m_cxema->m_ado, q, &affected);

            if (id2 == 0) {
                AfxMessageBox(m_cxema->m_ado->getError(), MB_OK|MB_ICONINFORMATION);
            }

//            if (id2 && typ != TIP_US && (typ == TIP_PR || typ == TIP_PO)) {
            if (id2 && typ != TIP_US) {

                int fileID = node->node.fileID;

                CAdoFile* ado = m_cxema->m_ado;
                if (ado) {
                    ado->Edit(getNodeTableMySQL(typ), id2);
//                    ado->write("nodeID", id2);

                    if (typ == TIP_PR || typ == TIP_PO) {   // Предыдущие значения
                        write_def(ado, fileID, "specExpendID", m_cxema->map_init_UR);
                        write_def(ado, fileID, "varCoeffID", m_cxema->map_init_KV);
                        write_def(ado, fileID, "calcTemperatureID", m_cxema->map_init_TR);
                    };

                    if (typ == TIP_PR) {
                        ado->write("schemeNum", N_1_3);
                        node->node.schemeNum = N_1_3;
                        setNodeCxema(node, node->node.schemeNum);
                    }

                    if (typ == TIP_IS) {  // Температуры 
//                        ado->write("tn_1", tn_1);  // 8,  'Температура наружного воздуха в 1-ой характерной точке графика (начало/конец отопительного сезона), Град.С',
//                        ado->write("tn_5", tn_5);  // -32, 'Температура наружного воздуха в 5-ой характерной точке графика (расчетная для отопления), Град.С',
                    }
                    ado->Update();
                }
            }
            addRemoved(m_cxema->m_ado, CH_T_RENAME_NODE, node->id, "");
        }

        node->node.typ = typ;
        node->node.id2 = id2;
        node->node.nomgP = node->node.nomgO = -1;

        if (typ == TIP_PR) {
            create_b5(m_cxema->m_ado, node, m_cxema);
        }
        else if (old_typ == TIP_PR) {
            delCxemaFull(m_cxema, node);
            node->node.isP = false;
        }
        if (typ == TIP_PR || typ == TIP_PO) {
            node->node.isValid = false;
        }



        if (typ == TIP_PR || typ == TIP_HS || typ == TIP_IS) {
            int typ2 = PS_NEUST;
            ado->Edit("nodes", node->id);
            switch (typ) {
                case TIP_PR: typ2 = PS_NEUST; break;
                case TIP_HS: typ2 = PS_NS; break;
                case TIP_IS: typ2 = PS_IS; break;
            }
            ado->write("nodeTypeID", typ2); 
            ado->Update();
        }

    }
    return true;
}



bool CGidrView::renameNode(CNode2 *node)
{
    int i, old_typ = node->node.typ;
    CMMenu menu(AfxGetMainWnd(), IDD_MENU, _TR("Установка типа узла"));

    for (i = 0; i < nodefsize - 1; i++) {
        menu.Add(nodef[i].opis, (void*)nodef[i].typ);
    }
    if (menu.DoModal() == IDOK) {
        int typ = (int)menu.getV();
        renameNode(node, typ);
        Invalidate();
    }
    return true;
}

bool readLine(CAdoFile* ado, CLine2* line, int typ, CGraph2* m_graph, const char* tn, const char* otkr);


bool rereadLine(CAdoFile *ado, CGraph2 *graph, CLINE2 *line, CString qq)
{
    CLine2 *l = bline(line);

    CString q = getLineQ(l->line.typ, l->getTableMySQL(), getLineOtkr(l->line.typ), qq);
    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            l->line.nomP = -1;
            l->line.nomO = -1;
            readLine(ado, l, l->line.typ, graph, l->getTableMySQL(), getLineOtkr(l->line.typ));
        }
        ado->closeTable();
    }
    return true;
}


bool CGidrView::renameLine(CLINE2 *l)
{
    int typ = getLineTypMenu(this);

    if (typ >= 0) {
        CString tnOld = bline(l)->getTableMySQL();

        if (typ != bline(l)->line.typ) {
//            if (typ == TIP_UT) {
//                PtsAddLine(l);
//            }
//            else {
//                PtsDelLine(l);
//            }

            CString tn = getLineTableMySQL(typ);

            long affected;
            CString q;
            q.Format("DELETE FROM %s WHERE ID=%d OR ID=%d", tnOld, bline(l)->line.idP2, bline(l)->line.idO2);
            bool ret = ado_execute(m_cxema->m_ado, q, &affected);

            if (ret) {
                int nomP = bline(l)->line.nomP;
                int nomO = bline(l)->line.nomO;
                long affected;

                CString q;
                if (nomP > 0) {

                    if (typ == TIP_RD) {
                        q.Format("INSERT INTO %s (lineID, pipelineSignID,h) VALUES (%d,1, 0)", tn, nomP);
                    }
                    else if (typ == TIP_BP) {
                        q.Format("INSERT INTO %s (lineID, pipelineSignID,h) VALUES (%d,1, 0)", tn, nomP);
                    }
                    else {
                        q.Format("INSERT INTO %s (lineID) VALUES (%d)", tn, nomP);
                    }
                    bline(l)->line.idP2 = ado_executeInsert(m_cxema->m_ado, q, &affected);

                    if (bline(l)->line.idP2 == 0) {
                        AfxMessageBox(m_cxema->m_ado->getError(), MB_OK|MB_ICONINFORMATION);
                    }
                    else {
                        bline(l)->line.nomP = -1;
                        bline(l)->line.nomO = -1;
                    }
                }
                if (nomO > 0 && nomO != nomP) {
                    if (typ == TIP_RD) {
                        q.Format("INSERT INTO %s (lineID, pipelineSignID,h) VALUES (%d,2, 0)", tn, nomO);
                    }
                    else if (typ == TIP_BP) {
                        q.Format("INSERT INTO %s (lineID, pipelineSignID,h) VALUES (%d,2, 0)", tn, nomO);
                    }
                    else {
                        q.Format("INSERT INTO %s (lineID) VALUES (%d)", tn, nomO);
                    }
                    bline(l)->line.idO2 = ado_executeInsert(m_cxema->m_ado, q, &affected);
                    if (bline(l)->line.idO2 == 0) {
                        AfxMessageBox(m_cxema->m_ado->getError(), MB_OK|MB_ICONINFORMATION);
                    }
                    else {
                        bline(l)->line.nomP = -1;
                        bline(l)->line.nomO = -1;
                    }
                }
                bline(l)->line.typ = typ;
                bline(l)->line.nomgP = bline(l)->line.nomgO = -1;

                CString qq;
                qq.Format("l1.id IN (%d, %d)", nomP, nomO);
                rereadLine(m_cxema->m_ado, m_cxema->m_graph, l, qq);

                if (bline(l)->line.nomP > 0) {
                    addRemoved(m_cxema->m_ado, CH_T_RENAME_LINE, bline(l)->line.nomP, "");
                }
                if (bline(l)->line.nomO > 0 && bline(l)->line.nomP != bline(l)->line.nomO) {
                    addRemoved(m_cxema->m_ado, CH_T_RENAME_LINE, bline(l)->line.nomO, "");
                }

                Invalidate();
            }
        }
    }
    return true;
}


bool delCxemaFull(CCxema *m_cxema, const CNode2 *node)
{
    if (node) {
        bool ret;
        CString q;
        long affected;

        q.Format("DELETE nodes  WHERE internalNodeID=%d", node->id);

        ret = ado_execute(m_cxema->m_ado, q, &affected);

        if (ret) {
            q.Format("DELETE linesobj WHERE nodeID1 in (SELECT id FROM nodes WHERE internalNodeID=%d)",  node->id);
            ret = ado_execute(m_cxema->m_ado, q, &affected);
            if (ret) {
                m_cxema->m_graph->clearVnutr(node->id);
            }
        }
        if (node->node.typ != TIP_PR) {
            addRemoved(m_cxema->m_ado, CH_T_DELETE_CXEMA_END, node->id, "");
        }
    }
    return true;
}


bool delCxema1(CCxema *m_cxema, const CNode2 *node)
{
    if (node) {
        bool ret;
        CString q;
        long affected;

//        q.Format("UPDATE %s SET removed=1, dateRemoved='%s', userRemoved=%d WHERE ID=%d", tn, s, m_userRemoved, id);
//        q.Format("D-ELETE FROM nodes WHERE internalNodeID=%d", node->id);

        int idRem = addRemoved(m_cxema->m_ado, CH_T_DELETE_CXEMA_BEGIN, node->id, "");
        idRem = addRemoved(m_cxema->m_ado, CH_T_DELETE_NODE, node->id, "");

        
        q.Format("UPDATE nodes SET removed=1, idRemoved=%d WHERE internalNodeID=%d", idRem, node->id);
        
        ret = ado_execute(m_cxema->m_ado, q, &affected);

        if (ret) {
//            q.Format("D-ELETE FROM linesobj WHERE nodeID1 in (SELECT id FROM nodes WHERE internalNodeID=%d)", node->id);

            idRem = addRemoved(m_cxema->m_ado, CH_T_DELETE_LINE, node->id, "");
            
            q.Format("UPDATE linesobj SET removed=1, idRemoved=%d WHERE nodeID1 in (SELECT id FROM nodes WHERE internalNodeID=%d)", idRem, node->id);
            ret = ado_execute(m_cxema->m_ado, q, &affected);
            if (ret) {
                m_cxema->m_graph->clearVnutr(node->id);
            }
        }
        addRemoved(m_cxema->m_ado, CH_T_DELETE_CXEMA_END, node->id, "");
    }
    return true;
}




bool CGidrView::delCxema(const CNode2 *node)
{
    return delCxema1(m_cxema, node);
}

CString getLineQ(int typ, const char *tn, const char *otkr, const char *qq);
CString getNodeQ(int typ, const char *tn, const char *qq);
bool readNode(CAdoFile *ado, CNode2 *node2, int typ, CGraph2 *m_graph, const char *tn);
bool readLine(CAdoFile *ado, CLine2 *line, int typ, CGraph2 *m_graph, const char *tn, const char *otkr);

CString getLineOtkr(int typ);
bool readUndo(CString table, int id);

int obj_type_faktory_riskaID();
int getOsmotrOrRemontId();


void delOsmotrOrRemont(CAdoFile *ado, CString opres, CString text, int directionID, bool ask);

LRESULT CGidrView::OnAdoNoUpdated(WPARAM wParam, LPARAM lParam)
{
    TYP_OF_P typ = (TYP_OF_P)wParam;
    
    CPropertyParam *param = (CPropertyParam *) lParam;

    if (!param) return 0;

    int id = param->id;
    
    if (param->table == "osmotr" && typ == TP_OSMOTR_AUTODELETE) {
        delOsmotrOrRemont(m_cxema->m_ado, param->table, "", param->id, false);
        Invalidate();
        return 0;
    }
    if (param->table == "remont2" && typ == TP_REMONT_AUTODELETE) {
        delOsmotrOrRemont(m_cxema->m_ado, param->table, "", param->id, false);
        Invalidate();
        return 0;
    }

    if (param->table == "opres" && typ == TP_OPRES_AUTODELETE) {
        delOsmotrOrRemont(m_cxema->m_ado, param->table, "", param->id, false);
        Invalidate();
        return 0;
    }



    if (typ == TP_GEO) {                        // Геобаза
//        m_geofile->refreshElectro();
//        resetZhurnal();
//        Invalidate();
    }
    else if (typ == TP_AUTODELETE) {                        // Геобаза автоудаление
///     delGeoObj(param->table, param->id);
        m_geofile->delGeoObj(param->table, param->id);
        Invalidate();
    }

    if (typ == TP_FAKTORY_AUTODELETE) {
        void faktory_riska_truboprovoda_osmotr_cancel(WPARAM wParam, LPARAM lParam);
        faktory_riska_truboprovoda_osmotr_cancel(wParam, lParam);
        Invalidate();
    }


    return 0;
}

bool isPSstrict(CNode2* node);
void PtsNodeOnOff(CNode2 *node);

bool isKonets(CNode2 *n)
{
    map<int, int> map_ms;
    map<int, int> map_rs;


    for (CLINE2 *l = n->lines; l; l = next(l)) {
        if (bline(l)->line.rs) {
            map_rs[bline(l)->line.rs] ++;
        }
        if (bline(l)->line.ms) {
            map_ms[bline(l)->line.ms] ++;
        }
    }
    
    for (auto it : map_ms) {
        if (it.second == 1) {
            return true;
        }
    }
    for (auto it : map_rs) {
        if (it.second == 1) {
            return true;
        }
    }
    return false;
}


void findMag(int mag);
void findMs(int mag);
void findRs(int mag);
void findOpr(int opr);

LRESULT CGidrView::OnAdoUpdated(WPARAM wParam, LPARAM lParam)
{
    if (!wParam) return 0;

    CString qq;
    TYP_OF_P typ = (TYP_OF_P)wParam;
    CAdoFile *ado = m_cxema->m_ado;

    CPropertyParam *param = (CPropertyParam *) lParam;

    if (!param) return 0L;

    if (!isEditGeo(param->table, true)) return 0L;

    if (!isEditGid() && (typ == TP_NODE || typ == TP_LINE)) {
        CString str;
        Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
        if (pDoc) {
            str.Format("%s не имеет прав на редактирование", pDoc->m_user_gid);
            AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
        }
        return 0;
    }

    bool dmitry = false;

    if (typ == TP_DMITRI) {
        dmitry = true;
    }
    
    if (typ == TP_NODE) {                       // Узел
//        int id = (int)lParam;
        int id = param->id;
        CNode2 *n = m_cxema->m_graph->find(id);
        if (!n) return 0;

        bool isps = isPSstrict(n);

        CString schemeNum = n->node.schemeNum;
        CString schemeNum2 = n->node.schemeNum;

        if (schemeNum == "") schemeNum = "1.1";

        qq.Format("nodes.id=%d", id);
        CString q = getNodeQ(n->node.typ, n->getTableMySQL(), qq);

        if (ado->openTable0(q)) {
            if (!ado->isEOF()) {
                n->node.nomP = -1;
                n->node.nomO = -1;
                readNode(ado, n, n->node.typ, m_cxema->m_graph, n->getTableMySQL());

                ispr_us2(ado, n->id);

                CString par;
                par.Format("connectID=%d", n->id);
                m_cxema->openNode(ado, getNodeTableMySQL(TIP_US2), TIP_US2, par);
            }
            ado->closeTable();
            Invalidate();
        }

        if (n->node.typ == TIP_PR && n->node.schemeNum == "") {
            n->node.schemeNum == N_1_3;
        }

/*
        if (n->node.typ == TIP_PR && schemeNum != n->node.schemeNum) {
            CString capt = n->getTable() + " (" + n->getFileName() + ")";
            ado_info_PR(this, ado, n, "nodes", n->id, n->getTableMySQL(), n->node.id2, n->node.fileID, n->getTableOutMySQL(), n->node.nomgP, n->node.nomgO, n->node.nomPT, capt, n->node.schemeNum);
        }
*/

        if (n->node.typ == TIP_PR && schemeNum2 != n->node.schemeNum) {
            create_b5(ado, n, m_cxema);
        }

        bool isps2 = isPSstrict(n);

        if (isps2) {
            for (CLINE2 *l = n->lines; l; l = next(l)) {
                if (!bline(l)->line.pipeSectionID) {
                    isps = !isps2;
                }
            }
        }

        if (isps != isps2) {
            reset_ms_rs_node(ado, m_cxema->m_graph, n);
        }

        if (!isps2 && isps) {
            if (isKonets(n)) {
                AfxMessageBox("Внимание! Этот конечный узел больше не являются узлом ПТС", MB_OK|MB_ICONINFORMATION);
            }
        }
        m_QG = -1;
    }
    if (typ == TP_LINE) {                        // Линейный объект
//        int id = (int)lParam;
        int id = param->id;
        CLine2 *l = bline(m_cxema->m_graph->findLine(id));
        if (!l) return 0;

        qq.Format("l1.id=%d", id);
        CString q = getLineQ(l->line.typ, l->getTableMySQL(), getLineOtkr(l->line.typ), qq);
        if (ado->openTable0(q)) {
            if (!ado->isEOF()) {
                if (l->line.nomP == l->line.nomO) {
                    l->line.nomP = -1;
                    l->line.nomO = -1;
                }
                else {

                }

                readLine(ado, l, l->line.typ, m_cxema->m_graph, l->getTableMySQL(), getLineOtkr(l->line.typ));
            }
            ado->closeTable();

            reset_ms_rs_line(ado, m_cxema->m_graph, l);

            Invalidate();
        }
    }
    if (typ == TP_GEO || typ == TP_AUTODELETE) {                        // Геобаза
        if (param->table == "shurfy") {
            CString q;
            q.Format("SELECT sostoyanie_shurfaID FROM shurfy WHERE id=%d", param->id);

            long sostoyanie_shurfaID = readTableValueInt(ado, q);

            if (sostoyanie_shurfaID != 1) {  // Не План
                bool update_pipeSections_from_faktor_shurf(CGidrView * view, CAdoFile * ado, CString tn, int id);
                update_pipeSections_from_faktor_shurf(this, ado, param->table, param->id);
            }
        }

//        m_geofile->refreshRemont();
        m_geofile->refreshElectro();
        resetZhurnal(false);

        Invalidate();
    }
    if (typ == TP_FAKTORY_OSMOTR) {                       
      bool setOsmotr(CAdoFile* ado, CString opres, CCxema* m_cxema, int directionID);
      int getOsmotrOrRemontId();
      setOsmotr(ado, "osmotr", m_cxema, getOsmotrOrRemontId());
      Invalidate();
    }
    if (typ == TP_OPRES || typ == TP_OPRES_AUTODELETE) {                        // Опрессовка
//         m_id_opr = (int)lParam;
         setOpresNull();
         m_id_opr = param->id;

         if (m_id_opr == -1) {
             m_id_opr = get_last_id(ado, "opres");
         }
         if (m_id_opr > 0) {
             SaveOpres("opres", "Опрессовка", m_id_opr, false, false);
         }
    }
    if (typ == TP_REMONT || typ == TP_REMONT_AUTODELETE) {                        // Ремонт 2
//         m_id_remont2 = (int)lParam;
         setOpresNull();
         m_id_remont2 = param->id;

         if (m_id_remont2 == -1) {
             m_id_remont2 = get_last_id(ado, "remont2");
         }
         if (m_id_remont2 > 0) {
             SaveOpres("remont2", "Ремонт", m_id_remont2, false, false);
         }
    }
    if (typ == TP_OSVIDET) {                        // Освидетельствование 2
         setOpresNull();
         m_id_osvidet2 = param->id;

         if (m_id_osvidet2 == -1) {
             m_id_osvidet2 = get_last_id(ado, "osvidet2");
         }
         if (m_id_osvidet2 > 0) {
             SaveOpres("osvidet2", "Освидетельствование", m_id_osvidet2, false, false);
         }
    }
    if (typ == TP_OSMOTR || typ == TP_OSMOTR_AUTODELETE) {                        // Осмотр
         setOpresNull();
         m_id_osmotr2 = param->id;

         if (m_id_osmotr2 == -1) {
             m_id_osmotr2 = get_last_id(ado, "osmotr");
         }
         if (m_id_osmotr2 > 0) {
             SaveOpres("osmotr", "Осмотр", m_id_osmotr2, false, false);
         }
    }

    if (typ == TP_FAKTORY_OSMOTR || typ == TP_FAKTORY_AUTODELETE) {
        void faktory_riska_truboprovoda_osmotr_ok2(int id);
        faktory_riska_truboprovoda_osmotr_ok2(-1);
    }

    if (typ == TP_MS) {                        // Характеристики МС
//       int id = (int)lParam;
       int id = param->id;
       findMs(id);
    }
    if (typ == TP_RS) {                        // Характеристики РС
//        int id = (int)lParam;
        int id = param->id;
        findRs(id);
    }

    return 0;
}