#include <QtGui>
#include <QtSql>

#include <mainwindow.h>
#include <gidview/GidWidget.h>
#include <QInputDialog>
#include <table/DbWindow.h>

#include <db/db.h>
#include "cxema/undo.h"

//long okr(double x, int dX);
QString transl(const QString & s);


const map<QString, Klassif*> *init_pts_list_file(const QString & fn1);
const map<QString, Klassif*> *init_pts_table(const QString & tab);

int insert_point_in_cl(CCoordList &cl, CFPoint pt, int md, int loc, bool ends) 
{
    int k = cl.find_point(pt, md, ends);
    if (k >= 0) return k;

    if (loc != LOC2 && loc != LOC3) return -1;

    k = cl.find_edge(pt, md, loc);

    if (k >= 0) {
        k += 1;
        int i = 0;
        for (auto it = cl.begin(); it != cl.end(); it++, i++) {
            if (i == k) {
                cl.insert(it, pt);
                return k;
            }
        }

        cl.push_back(pt);
        return k;
    }
    return -1;
}


int insert_point_in_line(CLINE2 *l, CFPoint pt, CCoordList &cl, int md) 
{
    cl = bline(l)->line.m_NP;

    if (IsBegin(l)) {
        cl.push_front(where(l)->node.coord);
        cl.push_back(other(l)->node.coord);
    }
    else {
        cl.push_front(other(l)->node.coord);
        cl.push_back(where(l)->node.coord);
    }

    return insert_point_in_cl(cl, pt, md, LOC2, false);
}


int m_userRemoved = 1;

CNode2 * GidWidget::addUS1(int fileID, int parent_id, int externalSignID, CFPoint point)
{
   regim = R_INIT;
   m_nRegim = gidrAction.aInfo;

//  QString str;

  CNode2 *n = NULL;

  if (!isEditGid()) return NULL;
  if ( isPot() ) return NULL;


  CNode2 *nc = m_cxema.m_graph->find(parent_id);

  if (!nc) return NULL;

  {
    std::map<QString, QVariant> map_v;
    map_v["externalNodeName"] = nc->node.name;
    map_v["externalCodeID"] = nc->node.id_kod;
    map_v["x"] = point.x;
    map_v["y"] = point.y;
    map_v["fileID"] = fileID;
    map_v["internalNodeID"] = parent_id;
    map_v["externalSignID"] = externalSignID;

    map_v["sync_tgid"] = true;
    map_v["operatorID"] = m_user;
    map_v["archiveChangeDate"] = "$$$_NOW_$$$";


//    int id = execInsertQ(m_cxema.m_db, "nodes", map_v);
    int id = insertIntoDatabase(m_cxema.m_db, "nodes", map_v);

    if (id > 0) {

//      AfxMessageBox("111 id > 0", MB_OK|MB_ICONINFORMATION);
      
      n = m_cxema.m_graph->ins(id);

      if (n) {
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

        repaint();
//        GetDocument()->SetModifiedFlag(true);
      }
    }
  }
  return n;
}


CNode2 * GidWidget::addUS2(int fileID, int parent_id, int connectID, int externalSignID, CFPoint point)
{
  CNode2 *n = NULL;

   regim = R_INIT;
   m_nRegim = gidrAction.aInfo;

  if (!isEditGid()) return NULL;
  if ( isPot() ) return NULL;


  CNode2 *nc = m_cxema.m_graph->find(connectID);

  if (!nc) return NULL;

  int typ = TIP_US2;

  {
    std::map<QString, QVariant> map_v;
    map_v["externalNodeName"] = nc->node.name;
    map_v["externalCodeID"] = nc->node.id_kod;
    map_v["x"] = point.x;
    map_v["y"] = point.y;
    map_v["fileID"] = fileID;
    map_v["internalNodeID"] = parent_id;
    map_v["externalSignID"] = externalSignID;

    map_v["sync_tgid"] = true;
    map_v["operatorID"] = m_user;
    map_v["archiveChangeDate"] = "$$$_NOW_$$$";

//    int id = execInsertQ(m_cxema.m_db, "nodes", map_v);
    int id = insertIntoDatabase(m_cxema.m_db, "nodes", map_v);

    if (id > 0) {
      std::map<QString, QVariant> map_v;
      map_v["nodeID"] = id;
      map_v["connectID"] = connectID;

//      int id2 = execInsertQ(m_cxema.m_db, "connectNodes", map_v);
      int id2 = insertIntoDatabase(m_cxema.m_db, "connectNodes", map_v);

      n = m_cxema.m_graph->ins(id);

      if (n) {
        n->node.coord = point;
        n->node.typ = typ;
//        n->node.file = m_cxema.getGidFile(fileID);

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

        repaint();
//        GetDocument()->SetModifiedFlag(true);
      }
    }
  }
  return n;
}



void GidWidget::find_point2(CFPoint pt)
{
    if (m_nRegim) QMessageBox::warning(this, tr("2"), m_nRegim->text());
}


bool reset_shape_line(QSqlDatabase & db, int id);

long addLineZD(QSqlDatabase &db, int typ, CNode2 *n1, CNode2 *n2, int externalSignLineID, CCoordList &cl, long &idv, double diam, int m_user)
{
//    if (!ado || !ado->isOpen()) return false;
    if (n1->node.fileID != n2->node.fileID) {
        return 0;
    }


    int id1 = n1->id;
    int id2 = n2->id;

    QString tn = "linesobj";
    QString tnV = getLineTableMySQL(typ);

    idv = 0;

      QString s_coords = cl.saveStr();

      std::map<QString, QVariant> map_v;

      map_v["externalSignLineID"] = externalSignLineID;
      map_v["nodeID1"] = id1;
      map_v["nodeID2"] = id2;
      map_v["coords"] = s_coords;

      map_v["sync_tgid"] = true;
      map_v["operatorID"] = m_user;
      map_v["archiveChangeDate"] = "$$$_NOW_$$$";



//    saveOperator(ado);
//    int id = execInsertQ(db, tn, map_v);
    int id = insertIntoDatabase(db, tn, map_v);

    if (id > 0 && tnV != "") {
        reset_shape_line(db, id);

        std::map<QString, QVariant> map_v;
//        map_v["pipeSectLength"] = len;
        map_v["lineID"] = id;
        map_v["diameterCondit"] = diam;
//        idv = execInsertQ(db, tnV, map_v);
        idv = insertIntoDatabase(db, tnV, map_v);
    }

    return id;
}



bool GidWidget::initUS(const CNode2* parent)
{
    change_group_start(m_cxema.m_db, "Создание внутренней схемы");

    double R = 2000, X0 = 40000, Y0 = -40000;

    CLINE2* l;
    CNode2* o;
    int n_n = 0;

    CGraph2* m_graph = m_cxema.m_graph;

    for (l = parent->lines; l; l = nextl(l)) {
        //        n_n += other(l)->node_name.p == ' ' ? 2 : 1;
        n_n += 1;
    }

    double fi = 0, dR = n_n <= 2 ? 0 : R / 8;

    CNode2* nP = NULL, * nO = NULL;

    //    CLINE2 *line;
    if (parent->node.externalSignID == 1 || parent->node.externalSignID == 2 || parent->node.externalSignID == 4) {
        //        AfxMessageBox("np!", MB_OK|MB_ICONINFORMATION);
        nP = addUS1(parent->node.fileID, parent->id, 2, CFPoint(X0 + dR, Y0 - R / 4));
    }
    if (parent->node.externalSignID == 1 || parent->node.externalSignID == 3 || parent->node.externalSignID == 5) {
        //        AfxMessageBox("no!", MB_OK|MB_ICONINFORMATION);
        nO = addUS1(parent->node.fileID, parent->id, 3, CFPoint(X0, Y0 + R / 4));
        //        nO = addNode(TIP_US, gid_file, CNodeName(parent->node_name.kod, parent->node_name.name, COBR), CFPoint(X0, Y0+R/4));
    }


    for (l = parent->lines; l; l = nextl(l)) {
        CNode2* n1P = NULL, * n1O = NULL;
        o = other(l);
        double cosfi = cos(fi);
        double sinfi = sin(fi);
        sinfi = (o->node.coord.x - parent->node.coord.x) / hypot(o->node.coord.x - parent->node.coord.x, o->node.coord.y - parent->node.coord.y);
        cosfi = (o->node.coord.y - parent->node.coord.y) / hypot(o->node.coord.x - parent->node.coord.x, o->node.coord.y - parent->node.coord.y);

        double diamP = bline(l)->line.pod.diam_usl;
        double diamO = bline(l)->line.obr.diam_usl;


        CFPoint ptP = CFPoint(X0 + R * sinfi + dR, Y0 + R * cosfi - R / 4);
        CFPoint ptO = CFPoint(X0 + R * sinfi, Y0 + R * cosfi + R / 4);



        //        if (bline(l)->line.nomP > 1 && bline(l)->line.nomO > 1) {
        if (bline(l)->line.nomP >= 1) {
            n1P = addUS2(o->node.fileID, parent->id, o->id, 2, ptP);
        }
        if (bline(l)->line.nomO >= 1) {
            n1O = addUS2(o->node.fileID, parent->id, o->id, 3, ptO);
        }
        /*
                }
                else {
                    CNode2 *nn =  addUS2(o->node.fileID, parent->id, o->id, bline(l)->line.nomP > 1 ? 2 : 3, CFPoint(X0 + R * sinfi, Y0 + R * cosfi));

                    if (bline(l)->line.nomP > 1) n1P = nn;
                    else n1O = nn;
                }
        */

        CCoordList cl;
        long idv;

        if (n1P && nP) {
            long id = addLineZD(m_cxema.m_db, TIP_ZD, n1P, nP, 2, cl, idv, diamP, m_user);

            CLINE2* line = m_graph->insert_line(n1P, nP);
            if (line) {
                bline(line)->line.nomP = id;
                bline(line)->line.idP2 = idv;
                bline(line)->line.typ = TIP_ZD;
            }
        }
        if (n1O && nO) {
            long id = addLineZD(m_cxema.m_db, TIP_ZD, n1O, nO, 3, cl, idv, diamO, m_user);

            CLINE2* line = m_graph->insert_line(n1O, nO);
            if (line) {
                bline(line)->line.nomO = id;
                bline(line)->line.idO2 = idv;
                bline(line)->line.typ = TIP_ZD;
            }
        }

        fi += M_PI * 2 / n_n;
    }

#if 0

    for (l = parent->lines; l; l = next(l)) {
        CLine2* ll = bline(l);

        double diamP = ll->line.pod.diam;
        double diamO = ll->line.obr.diam;

        o = other(l);
        if (ll->line.nomP != -1 && ll->line.nomO != -1) {
            CNode2* n1;
            n1 = m_graph->find(parent->file, CNodeName(o->node_name.kod, o->node_name.name, CPOD));

            if (n1 && (line = m_graph->insert_line(nP, n1)) != NULL) {
                addLine(line, TIP_ZD, diamP);
            }

            n1 = m_graph->find(parent->file, CNodeName(o->node_name.kod, o->node_name.name, COBR));

            if (n1 && (line = m_graph->insert_line(nO, n1)) != NULL) {
                addLine(line, TIP_ZD, diamO);
            }
        }
        else {
            n1 = m_graph->find(parent->file, CNodeName(o->node_name.kod, o->node_name.name, ll->line.nomP != -1 ? CPOD : COBR));
            if (n1
                && (ll->line.nomP != -1 && (line = m_graph->insert_line(nP, n1)) != NULL)
                || (ll->line.nomO != -1 && (line = m_graph->insert_line(nO, n1)) != NULL)
                ) {
                addLine(line, TIP_ZD, ll->line.nomP != -1 ? diamP : diamO);
            }
        }
    }
#endif

    change_group_end(m_cxema.m_db);

    return true;
}

//void create_b5(QSqlDatabase &db, CNode2 *nP, CCxema* cxema);
void create_b5_new(QSqlDatabase &db, CNode2 *nP, CCxema* cxema);


bool GidWidget::initCxema(CNode2* parent)
{
    // if (m_graph->set_node.size() > 0) return false;

    double R = 2000, X0 = 40000, Y0 = -40000;

    if (parent) {
        if (parent->node.typ == TIP_PR) {
//            create_b5(m_cxema.m_db, parent, &m_cxema);
        }
        else if (parent->node.typ == TIP_PO) {
            QMessageBox::warning(this, tr(""), tr("Нельзя создавать внутреннюю схему в обобщенном потребителе"));
            return false;
        }
        else if (parent->node.nZN != -1) {
            QMessageBox::warning(this, tr(""), tr("Нельзя создавать внутреннюю схему в узле с заданным напором"));
            return false;
        }
        else if (parent->node.typ == TIP_C3) {
            //      initC3(gid_file, parent);
        }
        else if (parent->node.typ == TIP_HS || parent->node.typ == TIP_US || parent->node.typ == TIP_C3 || parent->node.typ == TIP_IS) {
            initUS(parent);
        }
        else {
        }
    }
    return true;
}


bool GidWidget::moveLineEnd(CNode2 *n1, CNode2 *node, CLINE2 *line)
{
    QString str;
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

//    long affected;

    //  if (n1->node.externalSignID == SignNode_p && n2->node.externalSignID == SignNode_p) externalSignLineID = SignLine_pp;


    if (l->line.nomP > 0 && l->line.nomO > 0 && (node->node.nomP <= 0 || node->node.nomO <= 0)) {
        QMessageBox::warning(this, "", tr("Невозможно присоединить общий трубопровод к такому узлу"));
        return false;
    }



    CNode2 *nn1 = NULL, *nn2 = NULL;


    if ((l->line.nomP <= 0 || l->line.nomO <= 0) && (node->node.nomP <= 0 || node->node.nomO <= 0)) {
        if (node->node.nomP <= 0) {
        }
    }


    if (swap) {
        str = QString("UPDATE net.v_linesobj SET nodeID2=%1 WHERE (ID=%2 OR ID=%3)")
                  .arg(node->id).arg(l->line.nomP).arg(l->line.nomO);
    }
    else {
//        if ((l->line.nomP <= 0 || l->line.nomO <= 0) && ) {
//            if (where(line)->node.nomP);
//        }

        str = QString("UPDATE net.v_linesobj SET nodeID1=%1 WHERE (ID=%2 OR ID=%3)")
          .arg(node->id).arg(l->line.nomP).arg(l->line.nomO);
    }

//    bool ret = ado_execute(m_cxema->m_ado, str, &affected);
    bool ret = query_exec(m_cxema.m_db, str);

    //  if (affected <= 0) {
    //    str.Format("UPDATE linesobj SET nodeID2=%d WHERE nodeID1=%d AND nodeID2=%d AND (ID=%d OR ID=%d)", node->id, n2->id, n1->id, l->line.nomP, l->line.nomO);
    //    ret = ado_execute(m_cxema->m_ado, str, &affected);
    //  }

    if (!ret) {
        ErrorMessage(m_cxema.m_db);
//        AfxMessageBox(getAdoError(), MB_OK|MB_ICONINFORMATION);
        return false;
    }
//    if (affected <= 0) {
//        QMessageBox::warning(this, "", tr("Ошибка"));
//    }

    CLine line_old = l->line;

    m_cxema.m_graph->remove_line(line);

    CLINE2 *line2 = NULL; 
    
    if (swap) {
        line2 = m_cxema.m_graph->insert_line(n2, node, NP);
    }
    else {
        line2 = m_cxema.m_graph->insert_line(node, n2, NP);
    }

    if (line2) {
        bline(line2)->line = line_old;
    }

//    GetDocument()->SetModifiedFlag(true);
    repaint();
    return true;
}



bool GidWidget::find_point(CFPoint pt)
{
#if 1
//  if (m_nRegim) QMessageBox::warning(this, tr("1"), m_nRegim->text());

    CGraph2 *m_graph = m_cxema.graph();

    CFPoint point;
    CNode2 *node;
    CLINE2 *line;
//  Klassif *kls;

    point.x = okr(pt.x, m_dX);
    point.y = okr(pt.y, m_dX);

//    double delta = geom.mas*D5/mas_otn;
    double delta = geom.masx*D5;


    if (m_line_for_move) {
        if (m_cl_for_move.find2(pt, delta, 2)) {
            int n = insert_point_in_cl(m_cl_for_move, pt, delta, LOC2, false);
            if (n >= 0) {
                m_n_for_move = n;
                regim = R_MOVE_LINE;
            }
        }
        repaint();
        return false;
    }


    if (m_geo_for_move.kls) {
        if (m_cl_for_move.find2(pt, delta, m_geo_for_move.kls->loc)) {
            int n = insert_point_in_cl(m_cl_for_move, pt, delta, m_geo_for_move.kls->loc, true);
            if (n >= 0) {
                m_n_for_move = n;
                regim = R_MOVE_LINE;
            }
        }
        repaint();
        return false;
    }



    node = m_graph->find( m_parent_id, pt, delta);
//  if (m_nRegim == ID_NODE || m_nRegim == ID_NODE_MOVE_NEXT) {
//    node = m_graph->find(point, masx*5/mas_otn);
//  }

    line = m_graph->findLine( m_parent_id, pt, delta);

    /*
      if (m_nRegim)
        QMessageBox::warning(this, tr("1"), QString("nr=%1\nregim=%2\n%3\n%4 %5")
          .arg(m_nRegim->text())
          .arg(regim)
          .arg((int)line)
          .arg(pt.x).arg(pt.y));
      else
        QMessageBox::warning(this, tr("1"), "NULL");
    */


//  m_num = m_geofile0.findXY(pt, 5.*mas, 3, &kls);
//  if (m_num == 0) m_num = m_geofile0.findXY(pt, 5.*masx, 2, &kls);


    if (regim == R_INIT) {
        if (!m_nRegim) {
        }
        else if (m_nRegim == gidrAction.aMoveLineEnd) {
            if (!isEditGid()) return false;
            if ( line ) {
                CNode2 *n1 = where(line);
                CNode2 *n2 = other(line);

                double d1 = Length(pt, n1->node.coord);
                double d2 = Length(pt, n2->node.coord);

                m_nRegim = gidrAction.aMoveLineEndNext;
                m_line = line;
                m_n1 = (d1 < d2) ? n1 : n2;
                beginDraw(m_n1);
            }
        }
        else if (m_nRegim == gidrAction.aMoveLineEndNext) {
            endDraw();
            
            m_nRegim = gidrAction.aMoveLineEnd;
            if (!isEditGid()) return false;

            if ( node && node != where(m_line) && node != other(m_line)) {
                if (node->node.fileID != where(m_line)->node.fileID) {
                    QMessageBox::warning(this, "", "Участок и узел находятся в разных фрагментах!");
                    return false;
                }


                QString str = QString("Присоединить %1 к узлу?").arg(bline(m_line)->getTable());

                if (QMessageBox::question(this, "", str) == QMessageBox::Yes) {
                    if (moveLineEnd(m_n1, node, m_line)) {
                        SetModifiedFlag(true);
                        repaint();
                    }
                }
            }
        }
              /*
              else if (m_nRegim == gidrAction.aNew_house) {
                  if (m_kls) {
                    regim = R_BEGIN_DRAW_LINE;
                    if (m_kls->loc == 6) regim = R_BEGIN_DRAW_TYPE;
                    setTypeLine(m_kls->typ);
                    m_pt1 = m_pt2 = point;
                    m_NP.push_back(m_pt1);
                    BeginPaint();
                    DrawRect(point);
                  }
              }
              else
        */
        else if (m_nRegim == gidrAction.aNapr2) {
            if ( node ) {
                if ( m_napr1 != NULL ) {
                    m_graph->link2(m_napr1, node, false);
                    repaint();
                    m_nRegim = gidrAction.aInfo;
                }
                m_napr1 = node;
                node->node.isPjezo = true;
                m_graph->list_pjezo_min.push_back(node->id);
            }
        }
        else if (m_nRegim == gidrAction.aNapr) {
            if ( node ) {
                if ( m_napr1 != NULL ) {
                    m_graph->link(m_napr1, node);
//            repaintBackBuffer();
                    repaint();
                }
                m_napr1 = node;
                node->node.isPjezo = true;
                m_graph->list_pjezo_min.push_back(node->id);
//                QPainter *painter = beginPaint();
//                drawNode0(painter, node, true);
//                endPaint(painter);
            }
            else if ( line ) {
                if ( m_napr1 != NULL ) {
                    if (where(line) == m_napr1) {
                        node = other(line);
                    }
                    else if (other(line) == m_napr1) {
                        node = where(line);
                    }
                    else {
                        node = where(line);
                    }
                    if (node) {
                        m_graph->link(m_napr1, node);
//              repaintBackBuffer();
                        repaint();
                    }
                }
                if (node) {
                    m_napr1 = node;
                    node->node.isPjezo = true;
                    m_graph->list_pjezo_min.push_back(node->id);
//                    QPainter *painter = beginPaint();
//                    drawNode0(painter, node, true);
//                    endPaint(painter);
                }
            }
        }
        else if (m_nRegim == gidrAction.aNode) {
            addNode(point);
            /*
                    if (m_cxema.addNode(point, delta)) {
                      SetModifiedFlag(true);
                      repaint();
                    }
            */
        }
        else if (m_nRegim == gidrAction.aLine) {
            if (!isEditGid()) return false;
            if ( isPot() ) return false;
            m_n1 = node;
            m_n2 = NULL;
            if (m_n1) {
                regim = R_BEGIN_DRAW_LINE;
                m_pt1 = m_pt2 = CFPoint(m_n1->node.coord.x, m_n1->node.coord.y);
                m_NP_loc = LOC2;
                m_NP.push_back(m_pt1);
                repaint();
            }
        }
        else if (m_nRegim == gidrAction.aGoogleElevation) {
            googleElevation(point);
        }

        else if (m_nRegim == gidrAction.aPtsDel) pts_del(point, init_pts_list_file("pts"));
        else if (m_nRegim == gidrAction.aElectroDel) pts_del(point, init_pts_list_file("electro"));
        else if (m_nRegim == gidrAction.aRemontPovrDefDel) pts_del(point, init_pts_table("defect"));
        else if (m_nRegim == gidrAction.aRemontPovrShurfDel) pts_del(point, init_pts_table("shurfy"));
        else if (m_nRegim == gidrAction.aKorrozDel) pts_del(point, init_pts_table("indikator_korrozii"));

        else if (m_nRegim == gidrAction.aPtsInfo) pts_info(point, init_pts_list_file("pts"));
        else if (m_nRegim == gidrAction.aElectroInfo) pts_info(point, init_pts_list_file("electro"));
        else if (m_nRegim == gidrAction.aRemontPovrDefInfo) pts_info(point, init_pts_table("defect"));
        else if (m_nRegim == gidrAction.aRemontPovrShurfInfo) pts_info(point, init_pts_table("shurfy"));
        else if (m_nRegim == gidrAction.aKorrozInfo) pts_info(point, init_pts_table("indikator_korrozii"));

//        else if (m_nRegim == gidrAction.aPtsMove) pts_move(point, init_pts_list_file("pts"));
        else if (m_nRegim == gidrAction.aRemontPovrDefMove) pts_move(point, init_pts_table("defect"));
        else if (m_nRegim == gidrAction.aRemontPovrShurfMove) pts_move(point, init_pts_table("shurfy"));
        
        
        else if (m_nRegim == gidrAction.aTuDel) pts_del(point, init_pts_table(transl("Здания ТУ")));
        else if (m_nRegim == gidrAction.aTuInfo) pts_info(point, init_pts_table(transl("Здания ТУ")));


        /*
              else if (m_nRegim == gidrAction.aTrio) {
                if (!isEditGid()) return false;
                if ( isPot() ) return false;

                if (m_n_trio_point < 6) {
                  m_trio_point[m_n_trio_point++] = pt;
                }

                if (m_n_trio_point < 6) {
                  QString str;
                  char *cc[2] = {"откуда","куда"};
                  str.sprintf("Выберите %d точку %s нужно передвинуть", (m_n_trio_point+2)/2, cc[(m_n_trio_point)%2]);
                  QMessageBox::question(this, "", str);
                }
                if (m_n_trio_point == 6) {
                  QString str;
                  str = "Внимание!\nПри выполнении этой операции \nВыполнить операцию?";

                  if (QMessageBox::question(this, "", str) == QMessageBox::Yes) {

                    m_cxema.TrioMove(m_trio_point);

                    m_nRegim = gidrAction.aInfo;
                    SetModifiedFlag(true);
                    repaint();
                  }
                }
              }
        */
        else if (m_nRegim == gidrAction.aNodeNeotr) {
            neotr_node(node, point);
        }
        else if (m_nRegim == gidrAction.aZn) {
            if (!isEditGid()) return false;
            if ( isPot() ) return false;
            if (node) {
                viewZN(node);
            }
        }
        else if (m_nRegim == gidrAction.aDel) {
            if (node) {
                delNode(node);
            }
            else if (line) {
                delLine(line);
            }
        }
        else if (m_nRegim == gidrAction.aInfo) {
            if (node) {
                viewNode(node);
            }
            else if (line) {
                viewLine(line);
            }
        }
        /*
            else if (m_nRegim == gidrAction.aWincc_info) {
              wincc_info(pt);
            }
        */
//    else if (m_nRegim == gidrAction.aZd) {
//      zd(pt);
//    }

        else if (m_nRegim == gidrAction.aInfoGid) {
            info_gid(pt);
        }
//    else if (m_nRegim == gidrAction.aMove_to) {
//      move_to(pt);
//    }
        else if (m_nRegim == gidrAction.aGeoInfo) {
//            void findXY(CFPoint p, double md, list<KlGeo> & geo_list);
            findXY(pt, delta, geo_list);
            if (geo_list.size() > 0) {
                viewGeo(&geo_list.back());
            }
        }

        else if (m_nRegim == gidrAction.aOpenClose) {
            if (line) {
                closeLine(line);
            }
            else if (node) {
                closePotr(node);
            }
        }
        else if (m_nRegim == gidrAction.aNodeMove) {
            if (!isEditGid()) return false;
            if (node) {
                m_node_for_move = node;
                repaint();
            }
            else if (line) {
                CCoordList cl = bline(line)->line.m_NP;
                int n = insert_point_in_line(line, pt, m_cl_for_move, delta);
                if (n >= 0) {
                    m_line_for_move = line;
                    m_n_for_move = n;
                    regim = R_MOVE_LINE;
                    is_move1 = false;
                }
                repaint();
            }
/*
            else if ( m_num ) {
                m_nRegim = ism_houseAction;
                m_geofile0.GetCoord(m_NP);
                CCoordList::iterator it = m_NP.begin();

                BeginPaint();
                DrawLine();
                EndPaint();
                regim = R_BEGIN_MOVE_LINE;
            }
*/
        }


        else if (m_nRegim == gidrAction.aNodeMove1) {
            if (!isEditGid()) return false;
            if (line) {
                CCoordList cl = bline(line)->line.m_NP;
                int n = insert_point_in_line(line, pt, m_cl_for_move, delta);
                if (n >= 0) {
                    m_line_for_move = line;
                    m_n_for_move = n;
                    regim = R_MOVE_LINE;
                    is_move1 = true;
                }
                repaint();
            }
        }


        /*
            else if (m_nRegim == gidrAction.aNode_move1) {
              if (!isEditGid()) return false;
              if (node) {
                m_n1 = node;
                m_nRegim = node_move1_nextAction;

                BeginDrawNode(m_n1);
              }
              else if (line) {
                ism(line);
                m_nRegim = node_move1Action;
              }
            }
            else if (m_nRegim == gidrAction.aNode_move_next) {
              if (node && node != m_n1) {
                QMessageBox::question(this, "", "Слишком близко к узлу "+node->node_name.name);
                m_nRegim = node_moveAction;
              }
              else {
                EndDrawNode();
                if (!isEditGid()) return false;
                m_cxema.setXY(m_n1, point);
                m_nRegim = node_moveAction;
                SetModifiedFlag(true);
                repaint();
              }
            }
            else if (m_nRegim == gidrAction.aNode_move1_next) {
                EndDrawNode();
                if (!isEditGid()) return false;
                if (m_n1->node_name.p == S_OBA)
                  m_cxema.setXY1(m_n1, point);
                else
                  m_cxema.setXY(m_n1, point);

                m_graph = m_cxema.m_graph;

                m_nRegim = node_move1Action;
                SetModifiedFlag(true);
                repaint();
            }
        */
        else if (m_nRegim == gidrAction.aRename) {
            if (!isEditGid()) return false;
                 if (node) ch_type_node(node);
            else if (line) ch_type_line(line);
//            SetModifiedFlag(true);
            repaint();
        }
        else if (m_nRegim == gidrAction.aKam || m_nRegim == gidrAction.aKamVnesh) {
            if (node) {
#if 0
                if ( IsMain() && node->typ != TIP_PO && ( node->isP || (isEditGid() && QMessageBox::question(this, "", "Создать схему ?") == IDYES) )) {
                    CNode *n = new CNode(node->node_name);
                    if (n) {
                        n->typ = node->typ;
                        pDoc->Kam1(n);
                        node->isP = true;
                    }
                }
#else
                if ( IsMain() && node->node.typ != TIP_PO  && node->node.typ != TIP_PR) {
                    if ( !node->node.isP && (isEditGid() && QMessageBox::question(this, "", "Создать схему ?") == QMessageBox::Yes)) {
                        if (!initCxema(node)) {
                            return false;
                        }
                        node->node.isP = true;
                    }
                    if ( node->node.isP) {
                        node->node.isP = true;
                        m_parent_id = node->id;
                        m_vnutr_typ = 1;
                        geom_old = geom;
                        CFRect rect = m_cxema.graph()->getBorder(m_parent_id);
    //                    SetBorder(rect);
                        geom.dmas = 5;
                        ChMas(rect);
                        repaint();
                    }
                }
#endif

                if ( IsMain() && node->node.typ == TIP_PR) {
                    create_b5_new(m_cxema.m_db, node, &m_cxema);

                    node->node.isP = true;

                    m_parent_id = node->id;
                    m_vnutr_typ = 1;
                    geom_old = geom;
                    CFRect rect = m_cxema.graph()->getBorder(m_parent_id);
                    //                    SetBorder(rect);
                    geom.dmas = 5;
                    ChMas(rect);
                    repaint();


                }
            }
        }
        else if (m_nRegim == gidrAction.aSwap) {
            if (!isEditGid()) return false;
            if ( isPot() ) return false;
            if (line) {
                swap(line);
            }
        }
        else if (m_nRegim == gidrAction.aMoveTo) {
            QString q;
            QString tn;

            void view_db2(DbWindow *view, const QString & title, QWidget *parent);
            QString getLineCode(int typ);
            QString getNodeCode(int typ);

            if (node) {
                tn = node->getTableMySQL();
//                q = QString("SELECT * FROM %1").arg(tn);
                q = readQ(QString("sql/%1.sql").arg(getNodeCode(node->node.typ)));
                q.replace("$fileID$", QString("%1").arg(node->node.fileID));
                DbWindow *view = getTableView(m_cxema.m_db, tn, q, node->getTable());
                view->setMoveTo(move_to_node);
                view_db2(view, node->getTable(), this);
            }
            else if (line) {
                tn = bline(line)->getTableMySQL();
//                q = QString("SELECT * FROM %1").arg(tn);
                q = readQ(QString("sql/%1.sql").arg(getNodeCode(bline(line)->line.typ)));
                q.replace("$fileID$", QString("%1").arg(where(line)->node.fileID));
//                q = readQ(QString("sql/%1.sql").arg(st.code));
                DbWindow *view = getTableView(m_cxema.m_db, tn, q, bline(line)->getTable());
                view->setMoveTo(move_to_line);
                view_db2(view, bline(line)->getTable(), this);
            }
        }

        else if (m_nRegim == gidrAction.aMarkRev) {
            void sw_p(CLINE2* line);
            if (line) sw_p(line);
            repaint();
        }

        else if (m_nRegim == gidrAction.aText) {
            if (!isEditGid()) return false;

            if (m_fileID == -1 && m_parent_id == 0) {
                QMessageBox::warning(this, tr(""), "Выберите активный фрагмент");
            }
            else {
                bool ok;
                QString text = "";
                text = QInputDialog::getText(this,"", tr("Введите текст"), QLineEdit::Normal, text, &ok);

                if (ok) {
                   if (m_cxema.addText(point, text, m_fileID, m_parent_id)) {
                       repaint();
                   }
                }
            }
        }

        
        /*
            else if (m_nRegim == gidrAction.aText) {
              if (!isEditGid()) return false;

              QString text;
              CInputDialog inD(this, _T("Исходный текст"), _T("Введите текст"), text);

              if (inD.DoModal() == IDOK) {
                if (m_cxema.addText(point, inD.m_strInput)) {
                  SetModifiedFlag(true);
                  repaint();
                }
              }
            }
          }
          else if (regim == R_BEGIN_DRAW_TYPE) {
            m_NP.push_back(point);
            m_geofile0.createObj(m_kls->nazv.c_str(), m_NP);

            regim = R_INIT;
            m_NP.clear();
            SetModifiedFlag(true);
            repaint();
          }
          else if (regim == R_BEGIN_DRAW_LINE && m_nRegim == gidrAction.aNew_house) {
            if (m_kls->loc == 2 || m_kls->loc == 3) {
              if (m_NP.size() > 0) m_pt1 = m_NP.back();
              m_pt2 = point;
              m_NP.push_back(point);
              DrawRect(point);
              m_pt1 = point;
            }
          }
        */
    }
    else if (regim == R_BEGIN_DRAW_LINE && m_nRegim == gidrAction.aLine && m_n1) {
        
        if (node || m_n2) {
            m_NP.pop_front();

            regim = R_INIT;

            if (m_n2) {
                endDraw();
                m_nRegim = gidrAction.aNodeNeotr;
                setXY(m_n2, point);
                m_n2->node.coord = point;

//                QString par = QString("%1").arg(m_n2->node.fileID);
//                CWaitCursor wait;
//                m_cxema.m_db

/*
                set<int>  v;
                v.insert(m_n2->node.fileID);
                m_cxema->m_graph->clearFile(v);
                m_cxema->openGid(ado, par, pDoc->m_user_gid);
*/
                set<int> set_fileID = {m_n2->node.fileID};
                reopen( set_fileID);
                repaint();

            }
            else {
//        if (node != m_n1) m_cxema.addLine(*m_n1, *node, m_NP);
                addLine(m_n1, node, m_NP);
            }
            SetModifiedFlag(true);
            m_n1 = m_n2 = NULL;
            m_NP.clear();
            repaint();
        }
        else {

            if (m_NP.size() > 0) m_pt1 = m_NP.back();
            m_pt2 = point;
            m_NP.push_back(point);
//      DrawRect(point);

            m_pt1 = point;
        }
    }

    else if (regim == R_BEGIN_CANAL_1) {
        if (!line) {
            QMessageBox::warning(this, "", "Точка должна быть на участке");
        }
        else {
            m_pt1_canal = pt;
            m_l_canal = line;
            regim = R_BEGIN_CANAL_2;
            repaint();
            QMessageBox::warning(this, "", "Введите вторую точку на участке");
        }
    }


    else if (regim == R_BEGIN_CANAL_2) {
        if (line != m_l_canal) {
            QMessageBox::warning(this, "", "Точка должна быть на том же участке");
        }
        else {
            m_pt2_canal = pt;
            m_nRegim = gidrAction.aInfo;

            QSettings settings;

            double width = settings.value("flags/canal_width", 5.).toDouble();

            bool ok;

            width = QInputDialog::getDouble(this, tr("Введите ширину канала"),
                                                    tr("Введите ширину канала"), width, 0, 100000, width, &ok);

            if (ok) {
                if (width > 0) {

                    settings.setValue("flags/canal_width",  width);

                    CCoordList cl = bline(line)->line.m_NP;

                    cl.push_front(where(line)->node.coord);
                    cl.push_back(other(line)->node.coord);

                    cl = cl.subline(m_pt1_canal, m_pt2_canal, D5 * geom.masx);

                    cl = cl.createBuffer(width*100./2);

                    std::map<QString, QVariant> m_map_geo_value;

                    int n = m_geo.createObj2(this, "kanal", cl, m_map_geo_value);

/*
                    int n = m_geofile->createObj("kanal", cl, m_map_geo_value);
                    if (n) {
                        GeoInfo(m_kls->nazv, m_kls->geo4[n - 1]->nom, true);
                    }
*/
                    repaint();
                }
            }
            repaint();
        }
    }


#endif
    
    
    return true;
}
