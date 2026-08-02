#include <QtGui>
#include <QtSql>
#include <QVariant>

#include <mainwindow.h>
#include <gidview/GidWidget.h>
#include <dialog/MMenuDial.h>
#include <db/db.h>
#include "cxema/undo.h"


#define N_1_3 "1.3"

QString getNodeQ(int typ, const QString & tn, const QString & qq)
{
    QString q, par_pot;
    QString param0 = "!.externalSignID,!.fileID,!.externalCodeID,!.internalNodeID,!.externalNodeName,!.nodeName,!.x,!.y,!.geoMarkTopTube,!.geoMarkNodeArea,!.displaySign,!.nodeTypeID";
    QString param = param0;
    param.replace("!.", "nodes.");

    if (tn != "") {
        q = QString("SELECT nodes.id,n2.id AS id2, %1 FROM nodes,%2 n2 WHERE nodes.id=n2.nodeID AND %3 AND nodes.removed = 0").arg(param, tn, qq);

        if (typ == TIP_PR) {
            par_pot = ",calcHLdep,calcHLindep,relLoadFacade,calcInternHD,calcHLventil,expendHWpart,avgHLcond,avgHLcloseSys,avgHLopenSysFlow,avgHLopenSysRet,avgHLGVSopenFlow,avgHLGVSopenRet,avgHLGVScloseParall,avgHLGVScloseMix,avgHLGVScloseConseq,avgHLGVSclosePreON,WEmeteringDeviceID";
            q = QString("SELECT nodes.id,n2.id AS id2, n2.consumerStateID, n2.schemeNum, buildHeight, automDegID, %1 %2 FROM nodes,%3 n2 WHERE nodes.id=n2.nodeID AND %4 AND nodes.removed = 0")
                .arg(param, par_pot, tn, qq);
        }
        else if (typ == TIP_PO) {
            par_pot = ",calcHLdep,calcHLindep,calcHLventil,calcHLparall,calcHLGVSparall,avgHLcompParall,calcHLmix,calcHLGVSmix,avgHLcompMix,calcHLconseq,calcHLGVSconseq,avgHLcompconseq,calcHLpreON,calcHLGVSpreON,avgHLcompPreON,avgHLGVSopenSysFlow,avgHLGVSopenSysRet,avgHLcompOpen";
            q = QString("SELECT nodes.id,n2.id AS id2, n2.consumerStateID, maxBuildingHeight, %1 %2 FROM nodes,%3 n2 WHERE nodes.id=n2.nodeID AND %4 AND nodes.removed = 0").arg(param, par_pot, tn, qq);
        }
        else if (typ == TIP_US2) {
            q = QString("SELECT nodes.id,n2.id, connectID AS id2, %1 FROM nodes,%2 n2 WHERE nodes.id=n2.nodeID AND %3 AND nodes.removed = 0").arg(param, tn, qq);


            param = param0;
            param.replace("!.externalNodeName", "n3.externalNodeName");
            param.replace("!.nodeName", "n3.nodeName");
            param.replace("!.", "nodes.");

            //        q = QString("SELECT nodes.id,n2.id AS id2, n2.connectID,  %s FROM nodes,%s n2 WHERE nodes.id=n2.nodeID AND %s AND nodes.removed = 0", param, tn, qq);
            //        q = QString("SELECT nodes.id,n2.id AS id2, n2.connectID, %s FROM nodes,%s n2 WHERE nodes.id=n2.nodeID AND %s AND nodes.removed = 0", param, tn, qq);

            q = QString("SELECT nodes.id,n2.id AS id2, n2.connectID, %1 "
                "FROM %2 n2 "
                "LEFT JOIN nodes ON nodes.id=n2.nodeID "
                "LEFT JOIN nodes n3 ON n3.id=n2.connectID "

                "WHERE %3 AND nodes.removed = 0").arg(param, tn, qq);


        }
    }
    else {
        q = QString("SELECT nodes.id,%1 FROM nodes WHERE %2 AND nodes.removed = 0").arg(param, qq);
    }

    //  q += " AND (nodes.internalNodeID IS NULL OR nodes.internalNodeID = 0 OR )";

    return q;
}

QString getLineQ(int typ, const QString & tn, const QString & otkr, const QString & qq)
{
    QString q;

    q = QString("SELECT l1.ID, l2.ID AS id2, l1.nodeID1, l1.nodeID2, l1.externalSignLineID, l1.coords, l1.organizationID, l1.operatorID, l1.displaySign "
        " FROM linesobj l1, %1 l2, nodes "
        " WHERE l1.id=l2.lineID  AND l1.nodeID1=nodes.ID AND %2 AND l1.removed = 0")
        .arg(tn, qq);

    if (typ == TIP_UT) {
        q = QString("SELECT l1.ID, l2.ID AS id2, l1.nodeID1, l1.nodeID2, l1.externalSignLineID, l1.coords, l1.organizationID, l1.operatorID, l1.displaySign, "
            " l2.pipeSectStateIDflow, l2.pipeSectStateIDret, l2.magistral AS l2_magistral, l2.distSite AS l2_distSite, l2.magistralSite AS l2_magistralSite, l2.tubingTypeID, l2.diameterInternal, l2.diameterExternal, l2.diameterCondit, l2.pipeSectLength, l2.wallThickness, l2.crimpingQueSite, l2.pipeSectionID "
            " FROM linesobj l1, %1 l2, nodes "
            " WHERE l1.id=l2.lineID AND l1.nodeID1=nodes.ID AND %2 AND l1.removed = 0")
            .arg(tn, qq);
    }
    else if (typ == TIP_DR) {
        if (otkr != "") {
            q = QString("SELECT l1.ID, l2.ID AS id2, l1.nodeID1, l1.nodeID2, l1.externalSignLineID, l1.coords, l1.organizationID, l1.operatorID, l1.displaySign, l2.diameterInternal, l2.diameterInternal AS dru_home, "
                " l2.%1 "
                " FROM linesobj l1, %2 l2, nodes "
                " WHERE l1.id=l2.lineID AND l1.nodeID1=nodes.ID AND %3 AND l1.removed = 0").arg(otkr, tn, qq);
        }
    }
    else {
        if (otkr != "") {
            q = QString("SELECT l1.ID, l2.ID AS id2, l1.nodeID1, l1.nodeID2, l1.externalSignLineID, l1.coords, l1.organizationID, l1.operatorID, l1.displaySign, "
                " l2.%1 "
                " FROM linesobj l1, %2 l2, nodes "
                " WHERE l1.id=l2.lineID AND l1.nodeID1=nodes.ID AND %3 AND l1.removed = 0").arg(otkr, tn, qq);
        }
    }
    return q;
}

bool write_def(std::map<QString, QVariant> & map_v, int fileID, const char * fn, const map<int, map <QString, QString> > &map_v0)
{
    std::map<int, std::map <QString, QString> >::const_iterator it1 = map_v0.find(fileID);

    if (it1 != map_v0.end()) {
        std::map<QString, QString>::const_iterator it2 = it1->second.find(fn);
        if (it2 != it1->second.end()) {
            map_v[fn] = it2->second;
        }
    }
    return true;
}

QString shape_text_1(CFPoint &pt);


int addNodeUS(QSqlDatabase & db, const QString & tn, const QString & name, int fileID, int externalSignID, int internalNodeID, int externalCodeID, CFPoint pt, int m_user)
{
    std::map<QString, QVariant> map_v;

    map_v["x"] = pt.x;
    map_v["y"] = pt.y;

    if (internalNodeID == 0) {
        map_v["shape"] = shape_text_1(pt);
    }
    
    map_v["fileID"] = fileID;
    map_v["externalNodeName"] = name;

    if (!internalNodeID) {
        map_v["nodeTypeID"] = PS_NEUST;
    }

    if (internalNodeID) {
        map_v["internalNodeID"] = internalNodeID;
    }

    map_v["externalSignID"] = externalSignID;

//    saveOperator(ado);

    if (externalCodeID) {
        map_v["externalCodeID"] = externalCodeID;
    }

    map_v["sync_tgid"] = true;
    map_v["operatorID"] = m_user;
    map_v["archiveChangeDate"] = "$$$_NOW_$$$";

//    int id = execInsertQ(db, tn, map_v);
    int id = insertIntoDatabase(db, tn, map_v, "shape");


//    reset_shape_node(ado, id);

    return id;
}

int getCurrentExternalCode(int fileID);
QString findTableRusName(const QString & d, const QString & n);
//void create_b5(QSqlDatabase &db, CNode2 *nP, CCxema* cxema);


bool GidWidget::addNode(CFPoint point)
{
    if (!isEditGid()) return false;
    if ( isPot() ) return false;

    regim = R_INIT;
//    m_nRegim = gidrAction.aInfo;

    CNode2 *node = m_cxema.m_graph->find(m_parent_id, point, geom.masx*D5/mas_otn);
    CLINE2 *line = m_cxema.m_graph->findLine(m_parent_id, point, geom.masx*D5/mas_otn);
    QString str;


    if (m_fileID == -1 && m_parent_id == 0) {
        onMainCxema();

        if (m_fileID == -1 && m_parent_id == 0) {
//        QMessageBox::warning(this, tr(""), "Выберите активный фрагмент");
            return false;
        }
    }

    int fileID = 0;
    if (m_parent_id == 0) {
        fileID = m_fileID;
    }
    else {
        CNode2 *np = m_cxema.m_graph->find(m_parent_id);
        if (np) {
            fileID = np->node.fileID;
        }
    }


    if (node) {
        if (node->node.m_graph == m_cxema.m_graph) {
            str = QString(tr("Слишком близко к %1 %2")).arg(node->getTable(), node->getName());
            QMessageBox::warning(this, "", str);
            return false;
        }
    }

#if 1

    bool is_insert = false;

    if (line && where(line)->node.m_graph == m_cxema.m_graph) {
        fileID = where(line)->node.fileID;
        if (m_fileID != fileID) {
            Fragment *fr = m_cxema.getFragment(fileID);
            if (fr) {
                QString s, rus_name = findTableRusName("gid", fr->m_name);
                s = QString(tr("Для выполнения данной операции переключитесь на фрагмент %1")).arg(rus_name);
                QMessageBox::warning(this, "", s);;
            }
            return false;
        }

        str = QString(tr("Вставить Узел в %1 %2, После вставки узла, установите новые длины и местные сопротивления на созданных участках")).arg(/*getNodeTable(typ),*/  bline(line)->getTable(), bline(line)->getName());
        //    str.Format(_TR("Слишком близко к %s %s"), bline(line)->getTable(), bline(line)->getName());
        if (QMessageBox::question(this, "", str) == QMessageBox::Yes) {
            is_insert = true;
        }
        else {
            return false;
        }
    }
#endif

    m_CurNode = nullptr;
    m_CurLine = nullptr;


    int typ = TIP_US;

    int externalSignID = 0;

    if (m_parent_id == 0) {
        MMenuDial menu(this, tr("Установка типа узла"));

        for (int i = 0; i < nodefsize; i++) {

            if (nodef[i].typ != TIP_US2) {
                menu.Add(nodef[i].opis, nodef[i].typ);
            }
        }
        if (menu.exec() != QDialog::Accepted) return false;

        typ = menu.value().toInt();
        externalSignID = 1;
    }
    else {
        MMenuDial menu(this, QObject::tr("Установка типа узла"));
        menu.Add(tr("Подающий"), 2);
        menu.Add(tr("Обратный"), 3);

        if (menu.exec() != QDialog::Accepted) return false;

        externalSignID = menu.value().toInt();
    }

    {
//    int externalSignID = m_parent_id == 0 ? 1 : 2;
        int externalCodeID = getCurrentExternalCode(fileID);
//        int externalCodeID = 1;

        if (m_parent_id != 0) {
            CNode2 *np = m_cxema.m_graph->find(m_parent_id);
            if (np) {
                externalCodeID = np->node.id_kod;
            }
        }

        QSqlDatabase & db = m_cxema.m_db;

        double tn_1 = 8;
        double tn_5 = -32;

#if 0
        if (typ == TIP_IS) {  // Температуры
            if (ado->openTable0("SELECT * FROM heatSystem")) {
                if ( !ado->isEOF() ) {
                    tn_1 = ado->read_double("t_vnew");
                    tn_5 = ado->read_double("t_or");
                }
                ado->closeTable();
            }
        }
#endif

        int id_max = readTableVariant(m_cxema.m_db, "select max(id) from nodes").toInt();

        change_group_start(m_cxema.m_db, "Добавление узла");

        QString name = QString("Узел_%1").arg(id_max+1);

        int id = ::addNodeUS(m_cxema.m_db, "nodes", name, fileID, externalSignID, m_parent_id, externalCodeID, point, m_user);

        if (id > 0) {
            int idRem = addRemoved(m_cxema.m_db, m_user, CH_T_ADD_NODE, id, "");

            int id2 = 0;

            if (typ == TIP_PR || typ == TIP_PO || typ == TIP_IS || typ == TIP_V_PT || typ == TIP_V_IS || typ == TIP_HS) {

                std::map<QString, QVariant> map_v;
                map_v["nodeID"] = id;

                if (typ == TIP_PR || typ == TIP_PO) {   // Предыдущие значения
//                    write_def(map_v, fileID, "specExpendID", m_cxema.map_init_UR);
//                    write_def(map_v, fileID, "varCoeffID", m_cxema.map_init_KV);
//                    write_def(map_v, fileID, "calcTemperatureID", m_cxema.map_init_TR);
                };

                if (typ == TIP_PR) {
                    map_v["schemeNum"] = N_1_3;
                }

                if (typ == TIP_IS) {  // Температуры
//                    map_v["tn_1"] = tn_1; // 8,  'Температура наружного воздуха в 1-ой характерной точке графика (начало/конец отопительного сезона), Град.С',


                    map_v["tn_1"] = tn_1; // 8,  'Температура наружного воздуха в 1-ой характерной точке графика (начало/конец отопительного сезона), Град.С',
                    map_v["tn_5"] = tn_5;  // -32, 'Температура наружного воздуха в 5-ой характерной точке графика (расчетная для отопления), Град.С',
                }
//                id2 = execInsertQ(db, getNodeTableMySQL(typ), map_v);
                id2 = insertIntoDatabase(db, getNodeTableMySQL(typ), map_v);
            }
            CNode2 *n = m_cxema.m_graph->ins(id);

            if (n) {
                n->node._typPS = PS_NEUST;
                n->node.coord = point;
                n->node.typ = typ;
//        n->node.file = m_cxema.getGidFile(fileID);

                n->node.id2 = id2;
                n->node.fileID = fileID;
                n->node.name = name;
                n->node.externalSignID = externalSignID;
                n->node.schemeNum = N_1_3;
                n->node.id_kod = externalCodeID;

                n->node.internalNodeID = m_parent_id;


//                if (m_cxema.m_graph->map_node.size() == 1) m_rectBorder = CFRect(1000, 1000, -1000, -1000);

                CFRect rect = m_cxema.graph()->getBorderFile(fileID);

                rect.AddPoint(point);

                if (m_parent_id == 0) {
                    m_cxema.setRect(rect);
                }


#if 1
                if (is_insert) {
                    insertNode(n, line, geom.masx*D5 / mas_otn);
                }

#endif

                if (typ == TIP_PR) {
//                    create_b5(m_cxema.m_db, n, &m_cxema);
                }

                if (typ == TIP_PR || typ == TIP_PO) {
                    n->node.isValid = false;
                }


                repaint();

                viewNode(n, true);

//                GetDocument()->SetModifiedFlag(true);
//                info(m_cxema, n, NULL);
            }
        }
        change_group_end(m_cxema.m_db);
    }
    return true;
}

bool reset_shape_line(QSqlDatabase & db, int id);


int get_init_value(int fileID, const char * fn, map<int, int> &map_v)
{
    map<int, int>::const_iterator it = map_v.find(fileID);

    if (it != map_v.end()) {
        return it->second;
    }
    return -1;
}

double getLengthLine(CFPoint p1, CFPoint p2, CCoordList &cl, bool ismap)
{
  double l1, l2, len;

  if (cl.size() == 0) {
    len = LengthG(p1, p2, ismap);
  }
  else {
    CFPoint c1 = cl.first();
    CFPoint c2 = cl.last();

    len = cl.Length();
    l1 = LengthG(p1, c1, ismap);
    l2 = LengthG(p2, c2, ismap);
    len = len+l1+l2;
  }
  return len;
}

double okr_pr(double d, int n)
{
    double d10 = pow(10., n);

    d = d*d10+0.5;

    d = floor(d);

    d /= d10;

    return d;
}

QString shape_text(CNode2 *n1, CNode2 *n2, const CCoordList &cl1)
{
    CCoordList cl2 = cl1;

    cl2.push_front(n1->node.coord);
    cl2.push_back(n2->node.coord);


    QString s_shape = cl2.getWKT(LOC2, 0);
    return s_shape;
}

QString shape_text_1(CFPoint &pt)
{
    CCoordList cl;
    cl.push_back(pt);
    QString s_shape = cl.getWKT(LOC1, 0);
    return s_shape;
}


int addLine(QSqlDatabase & db, int typ, CNode2 *n1, CNode2 *n2, int externalSignLineID, CCoordList &cl, int &idv, int m_user)
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
    int id = 0;

    QString s_coords = cl.saveStr();

//    QString q;
//    q = QString("INSERT INTO %s (externalSignLineID, nodeID1, nodeID2, coords, operatorID) VALUES (%d,%d,%d,'%s',%d)", tn, externalSignLineID, id1, id2, s_coords, m_userRemoved);
//    id = ado->ExecuteInsert(q, &affected);

//    ado->AddNew(tn);

    std::map<QString, QVariant> map_v;

    map_v["externalSignLineID"] = externalSignLineID;
    map_v["nodeID1"] = id1;
    map_v["nodeID2"] = id2;
    map_v["coords"] = s_coords;

/*
    CCoordList cl2 = cl;

    cl2.push_front(n1->node.coord);
    cl2.push_back(n2->node.coord);


    QString s_shape = cl2.getWKT(LOC2, 0);
    map_v["shape"] = s_shape;
*/
    map_v["shape"] = shape_text(n1, n2, cl);

    map_v["sync_tgid"] = true;
    map_v["operatorID"] = m_user;
    map_v["archiveChangeDate"] = "$$$_NOW_$$$";

    /*
        map_v["operatorID"] = m_userRemoved;

        COleDateTime v = COleDateTime::GetCurrentTime();
        ado->write("archiveChangeDate", v);
    */
//    saveOperator(ado);

//    id = execInsertQ(db, tn, map_v);

    change_group_start(db, "Добавление участка");

    id = insertIntoDatabase(db, tn, map_v, "shape");

    if (id > 0 && tnV != "") {
        reset_shape_line(db, id);
        std::map<QString, QVariant> map_v;

        map_v["lineID"] = id;
        if (typ == TIP_UT) {
            bool ismap = false;

            double len = getLengthLine(n1->node.coord, n2->node.coord, cl, ismap);
//            double len = 10.;

            if (n1->node.internalNodeID != 0) len = 1;

            len = okr_pr(len, 1);

            /*


            CCxema *cxema = n1->node.m_graph->m_cxema;

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

            map_v["pipeSectLength"] = len;
        }
        else if (typ == TIP_RD) {
            map_v["pipelineSignID"] = 1;
            map_v["h"] = 0.;
        }
        else if (typ == TIP_BP) {
            map_v["pipelineSignID"] = 1;
            map_v["h"] = 0.;
        }

//        idv =  execInsertQ(db, tnV, map_v);
        idv = insertIntoDatabase(db, tnV, map_v);
    }

    change_group_end(db);

#if 0
    if (db_2) {

        std::map<QString, QVariant> map_v_2;

        map_v_2["linesobj_externalSignLineID"] = externalSignLineID;
        map_v_2["linesobj_nodeID1"] = id1;
        map_v_2["linesobj_nodeID2"] = id2;
        map_v_2["linesobj_shape"] = shape_text(n1, n2, cl);
//        map_v_2["coords"] = s_coords;
        
        
        id = execInsertQ(*db_2, "gid." + tnV, map_v_2);
    }
#endif

    return id;
}


int getLineTypMenu(QWidget *wnd)
{
    int i;
    MMenuDial menu(wnd, QObject::tr("Установка типа линии"));

    for (i = 0; i < linefsize; i++) {
        if (linef[i].typ != TIP_EL && linef[i].typ != TIP_RO && linef[i].typ != TIP_TO && linef[i].typ != TIP_KU) {
            menu.Add(linef[i].opis, linef[i].typ);
        }
    }
    if (menu.exec() == QDialog::Accepted) {
        return menu.value().toInt();
    }
    return -1;
}

bool GidWidget::addLine(CNode2 *n1, CNode2 *n2, CCoordList &cl)
{
    regim = R_INIT;
//    m_nRegim = gidrAction.aInfo;

    if (n1 == n2 || !n1 || !n2) return false;

    int fileID1 = n1->node.fileID;
    int fileID2 = n2->node.fileID;

    if (fileID1 != fileID2) {
        QMessageBox::warning(this, "", "Начальный и конечный узел должны относиться к одному фрагменту!");
        return false;
    }

    int externalSignLineID = SignNode_ob;

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

    int idv;
    int id = ::addLine(m_cxema.m_db, typ, n1, n2, externalSignLineID, cl, idv, m_user);

    if (id > 0) {
        int idRem = addRemoved(m_cxema.m_db, m_user, CH_T_ADD_LINE, id, "");

        CLINE2 *line = m_cxema.m_graph->insert_line(n1, n2, cl);
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
//          initUS1(n1, n2);
            }
            if (n2->node.isP && n2->node.typ != TIP_PR && n2->node.typ != TIP_PO) {
//          initUS1(n2, n1);
            }


            repaint();

            viewLine(l, true);

//      PtsAddLine(line);

//      info(m_cxema, NULL, l);
        }
    }

    return true;
}

bool delCxemaFull(CCxema *m_cxema, const CNode2 *node)
{
    if (node) {
        QString q = QString("DELETE FROM nodes  WHERE internalNodeID=%1").arg(node->id);
        QSqlQuery query(m_cxema->m_db);
        bool ret = query_exec(m_cxema->m_db, query, q);

        if (ret) {
            q = QString("DELETE FROM linesobj WHERE nodeID1 in (SELECT id FROM nodes WHERE internalNodeID=%1)").arg(node->id);
            bool ret = query_exec(m_cxema->m_db, query, q);
            if (ret) {
                m_cxema->m_graph->clearVnutr(node->id);
            }
        }
        if (node->node.typ != TIP_PR) {
//            addRemoved(m_cxema->m_db, CH_T_DELETE_CXEMA_END, node->id, "");
        }
    }
    return true;
}




bool GidWidget::ch_type_node2(CNode2 *node, int typ)
{
    if (!node) return false;

    int old_typ = node->node.typ;

    if (node->node.isP && node->node.typ != TIP_PR && node->node.typ != TIP_PO && (typ == TIP_PR || typ == TIP_PO)) {
        QMessageBox::warning(this, tr(""), "Нельзя изменить тип узла с внутренней схемой на потребитель");
        return false;
    }

        

    if (typ != node->node.typ && delObject(m_cxema.m_db, node->getTableMySQL(), node->node.id2)) {
        int id2 = 0;

        QString tn = getNodeTableMySQL(typ);
        std::map<QString, QVariant> data;

        if (tn != "") {
            QString q;
            QString pp = "", vv = "", s;

            
            if (typ == TIP_PR || typ == TIP_PO) {   // Предыдущие значения
                int idd1 = get_init_value(node->node.fileID, "calcTemperatureID", m_cxema.map_init_TR);
                if (idd1 > 0) {
                    pp += ",calcTemperatureID";
                    vv += QString(",%d").arg(idd1);

                    data["calcTemperatureID"] = idd1;
                }
                int idd2 = get_init_value(node->node.fileID, "specExpendID", m_cxema.map_init_UR);
                if (idd2 > 0) {
                    pp += ",specExpendID";
                    vv += QString(",%d").arg(idd2);

                    data["specExpendID"] = idd2;

                }
                int idd3 = get_init_value(node->node.fileID, "varCoeffID", m_cxema.map_init_KV);
                if (idd3 > 0) {
                    pp += ",varCoeffID";
                    vv += QString(",%d").arg(idd3);
                    data["varCoeffID"] = idd3;

                }
            }

/*
            if (typ == TIP_PR || typ == TIP_PO) {   // Предыдущие значения
                write_def(ado, "calcTemperatureID", pt_def.map_v);
                write_def(ado, "specExpendID", pt_def.map_v);
                write_def(ado, "varCoeffID", pt_def.map_v);
            };
*/

//            q = QString("INSERT INTO %1 (nodeID %2) VALUES (%3 %4)").arg(tn).arg(pp).arg(node->id).arg(vv);

//            id2 = execInsertQ(m_cxema.m_db, q);


            data["nodeID"] = (int)node->id;

            id2 = insertIntoDatabase(m_cxema.m_db, tn, data);


            if (id2 == 0) {
//                AfxMessageBox(m_cxema.m_ado->getError(), MB_OK|MB_ICONINFORMATION);
            }

//            if (id2 && typ != TIP_US && (typ == TIP_PR || typ == TIP_PO)) {
            if (id2 && typ != TIP_US) {
#if 0
                int fileID = node->node.fileID;

                CAdoFile* ado = m_cxema.m_ado;
                if (ado) {
                    ado->Edit(getNodeTableMySQL(typ), id2);
//                    ado->write("nodeID", id2);

                    if (typ == TIP_PR || typ == TIP_PO) {   // Предыдущие значения
                        write_def(ado, fileID, "specExpendID", m_cxema.map_init_UR);
                        write_def(ado, fileID, "varCoeffID", m_cxema.map_init_KV);
                        write_def(ado, fileID, "calcTemperatureID", m_cxema.map_init_TR);
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
#endif
            }
        }

        node->node.typ = typ;
        node->node.id2 = id2;
        node->node.nomgP = node->node.nomgO = -1;

        if (typ == TIP_PR) {
//            create_b5(m_cxema.m_db, node, m_cxema);
        }
        else if (old_typ == TIP_PR) {
            delCxemaFull(&m_cxema, node);
            node->node.isP = false;
        }
        if (typ == TIP_PR || typ == TIP_PO) {
            node->node.isValid = false;
        }

    }
    return true;
}



bool GidWidget::ch_type_node(CNode2 *node)
{
    MMenuDial menu(this, tr("Установка типа узла"));

    for (int i = 0; i < nodefsize - 1; i++) {
        menu.Add(nodef[i].opis, nodef[i].typ);
    }
    if (menu.exec()) {
        int typ = menu.value().toInt();
        change_group_start(m_cxema.m_db, "Изменить тип");
        ch_type_node2(node, typ);
        change_group_end(m_cxema.m_db);
        repaint();
    }
    return true;
}

QString getLineOtkr(int typ);

#include <QInputDialog>


bool rereadLine(QSqlDatabase &db, CGraph2 *graph, CLINE2 *line, const QString & qq)
{
    CLine2 *l = bline(line);

    QSqlQuery query(db);
    query.setForwardOnly(true);

    QString q = readQ("sql3/ut.sql");
    q.replace(QString("$user_gid$"), QString("Администратор"));

    q += " AND " +qq;

//    QString qqq = QInputDialog::getMultiLineText(nullptr, "title", "label", q);

    if (query_exec(db, query, q)) {
        if (query.next()) {
            l->line.nomP = -1;
            l->line.nomO = -1;
            readLineNew(query, l, l->line.typ, graph);
        }
    }
    return true;
}


bool GidWidget::ch_type_line(CLINE2 *l)
{
    int typ = getLineTypMenu(this);

    if (typ >= 0) {
        QString tnOld = bline(l)->getTableMySQL();

        if (typ != bline(l)->line.typ) {
//            if (typ == TIP_UT) {
//                PtsAddLine(l);
//            }
//            else {
//                PtsDelLine(l);
//            }

            QString tn = getLineTableMySQL(typ);

            QString q = QString("DELETE FROM %1 WHERE ID=%2 OR ID=%3").arg(tnOld).arg(bline(l)->line.idP2).arg(bline(l)->line.idO2);


            QSqlQuery query(m_cxema.m_db);
            bool ret = query_exec(m_cxema.m_db, query, q);

            if (ret) {

                change_group_start(m_cxema.m_db, "Изменить тип");

                int nomP = bline(l)->line.nomP;
                int nomO = bline(l)->line.nomO;

//                QString q;
                if (nomP > 0) {
                    std::map<QString, QVariant> data;

                    if (typ == TIP_RD) {
//                        q = QString("INSERT INTO %1 (lineID, pipelineSignID,h) VALUES (%2,1, 0)").arg(tn).arg(nomP);
                        data["lineID"] = nomP;
                        data["pipelineSignID"] = 1;
                        data["h"] = 0;
                    }
                    else if (typ == TIP_BP) {
//                        q = QString("INSERT INTO %1 (lineID, pipelineSignID,h) VALUES (%2,1, 0)").arg(tn).arg(nomP);
                        data["lineID"] = nomP;
                        data["pipelineSignID"] = 1;
                        data["h"] = 0;

                    }
                    else {
//                        q = QString("INSERT INTO %1 (lineID) VALUES (%2)").arg(tn).arg(nomP);
                        data["lineID"] = nomP;
                    }
//                    bline(l)->line.idP2 = execInsertQ(m_cxema.m_db, q);



                    bline(l)->line.idP2 = insertIntoDatabase(m_cxema.m_db, tn, data);


                    if (bline(l)->line.idP2 == 0) {
//                        AfxMessageBox(m_cxema.m_ado->getError(), MB_OK|MB_ICONINFORMATION);
                    }
                    else {
                        bline(l)->line.nomP = -1;
                        bline(l)->line.nomO = -1;
                    }
                }
                if (nomO > 0 && nomO != nomP) {
                    std::map<QString, QVariant> data;

                    if (typ == TIP_RD) {
//                        q = QString("INSERT INTO %1 (lineID, pipelineSignID,h) VALUES (%2,1, 0)").arg(tn).arg(nomO);
                        data["lineID"] = nomO;
                        data["pipelineSignID"] = 2;
                        data["h"] = 0;
                    }
                    else if (typ == TIP_BP) {
                        data["lineID"] = nomO;
                        data["pipelineSignID"] = 2;
                        data["h"] = 0;
//                        q = QString("INSERT INTO %1 (lineID, pipelineSignID,h) VALUES (%2,1, 0)").arg(tn).arg(nomP);
                    }
                    else {
                        data["lineID"] = nomO;
//                        data["h"] = 0;
//                        q = QString("INSERT INTO %1 (lineID) VALUES (%2)").arg(tn).arg(nomO);
                    }
//                    bline(l)->line.idO2 = execInsertQ(m_cxema.m_db, q);

                    bline(l)->line.idO2 = insertIntoDatabase(m_cxema.m_db, tn, data);



                    if (bline(l)->line.idO2 == 0) {
//                        ErrorMessage(query);
                    }
                    else {
                        bline(l)->line.nomP = -1;
                        bline(l)->line.nomO = -1;
                    }
                }
                bline(l)->line.typ = typ;
                bline(l)->line.nomgP = bline(l)->line.nomgO = -1;

                QString qq = QString("l.id IN (%1, %2)").arg(nomP).arg(nomO);
                rereadLine(m_cxema.m_db, m_cxema.m_graph, l, qq);

                change_group_end(m_cxema.m_db);

                repaint();
            }
        }
    }
    return true;
}
