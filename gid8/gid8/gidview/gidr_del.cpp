#include <QtGui>
#include <QtSql>
#include <QVariant>

#include <mainwindow.h>
#include <gidview/GidWidget.h>
#include <dialog/MMenuDial.h>

#include <cxema/undo.h>
#include <db/db.h>


QString findTableRusNameGid(const QString & n);

bool delNodes(QSqlDatabase &db, int user, const QString & tn, int id, int m_user)
{
    if (tn == "") return true;

    int idRem = addRemoved(db, user, CH_T_DELETE_NODE, id, "");

    QString q = QString("UPDATE %1 SET removed=1, idRemoved=%2, operatorID=%4, archiveChangeDate=%5, sync_tgid=true WHERE ID=%3")
        .arg(tn)
        .arg(idRem)
        .arg(id)
        .arg(m_user)
        .arg(get_now());
        ;

    QSqlQuery query(db);
    bool ret = query_exec(db, query, q);

    return ret;
}


bool delLinesobj(QSqlDatabase &db, QSqlDatabase *db_2, int user, const QString & tn, const QString & tnV, int idP, int idO, int m_user)
{
//    q.Format("DELETE FROM %s WHERE ID=%d OR ID=%d", tn, idP, idO);

    int idRem = addRemoved(db, user, CH_T_DELETE_LINE, idP, "");

    QString q;

    q = QString("UPDATE %1 SET removed=1, idRemoved=%2, operatorID=%5, archiveChangeDate=%6, sync_tgid=true WHERE ID=%3 OR ID=%4")
        .arg(tn).arg(idRem).arg(idP).arg(idO)
        .arg(m_user)
        .arg(get_now());
        ;

    QSqlQuery query(db);
    bool ret = query_exec(db, query, q);


#if 0
    if (ret && db_2) {
        q = QString("DELETE FROM gid.%1 WHERE linesobj_ID=%3 OR linesobj_ID=%4").arg(tnV).arg(idRem).arg(idP).arg(idO);
        bool ret = query_exec(*db_2, query, q);
    }
#endif


    return ret;
}




bool GidWidget::delNode2(CNode2 *node)
{
    if (node->node.typ == TIP_US2) return false;
    if (inc(node) != 2) return false;

    CLINE2 *l1, *l2;

    l1 = node->lines;
    l2 = nextl(l1);

    if (bline(l1)->line.nomP != bline(l1)->line.nomO || bline(l2)->line.nomP != bline(l2)->line.nomO) {
        QMessageBox::warning(this, tr(""), tr("Удаление невозможно"));
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

    QString s1_coords = cl1.saveStr();

    double diametr1, diametr2, dlina;

    diametr1 = bline(l1)->line.pod.diam;
    diametr2 = bline(l2)->line.pod.diam;

    dlina = bline(l1)->line.pod.dl + bline(l2)->line.pod.dl;

    if (diametr1 != diametr2) {
        QMessageBox::warning(this, tr(""), tr("Диаметры разные"));
        return false;
    }

    if (QMessageBox::question(this,  tr(""), tr("Удалить узел и объединить участки? Длины участков будут просуммированы.")) != QMessageBox::Yes)
    {
        return false;
    }

    QString q;

    if (IsBegin(l1)) {
        q = QString("UPDATE net.v_linesobj SET coords='%1', nodeID1=%2 WHERE id IN (%3, %4)").arg(s1_coords).arg(n2->id).arg(bline(l1)->line.nomP).arg(bline(l1)->line.nomO);
    }
    else {
        q = QString("UPDATE net.v_linesobj SET coords='%1', nodeID2=%2 WHERE id IN (%3, %4)").arg(s1_coords).arg(n2->id).arg(bline(l1)->line.nomP).arg(bline(l1)->line.nomO);
    }

    QSqlQuery query(m_cxema.m_db);

    bool ret = query_exec(m_cxema.m_db, query, q);

    q = QString("UPDATE heatPipeSections SET pipeSectLength=%1 WHERE id IN (%2, %3)").arg(dlina).arg(bline(l1)->line.idP2).arg(bline(l1)->line.idP2);
    ret = query_exec(m_cxema.m_db, query, q);

    delLine(l2, false);

    if (delNodes(m_cxema.m_db, m_user, "nodes", node->id, m_user)) {
        m_cxema.m_graph->remove_node(*node);
    }

    CLine ll = bline(l1)->line;

    m_cxema.m_graph->remove_line(l1);



    CLINE2 *line = m_cxema.m_graph->insert_line(n1, n2, cl1);
    if (line) {
        bline(line)->line = ll;
        bline(line)->line.m_NP = m_NP;
    }

//    GetDocument()->SetModifiedFlag(true);
    repaint();
    return true;


#if 0
    m_dbf->write("dlina", dlina1 + dlina2);
    m_dbf->write("mestnoe", mestnoe1 + mestnoe2);
#endif
}



bool GidWidget::delNode(CNode2 *node, bool ask)
{
    if (!isEditGid()) return false;

    if (node->lines) {
        if (delNode2(node)) {
//            GetDocument()->SetModifiedFlag(true);
            repaint();
            return true;
        }
        if (ask) QMessageBox::warning(this, tr(""), tr("Нельзя удалить узел с привязанными участками"));
        return false;
    }

    if ( node->node.typ == TIP_US2 ) {
//        if (!ask) AfxMessageBox(_TR("Нельзя удалять узел присоединения"), MB_OK|MB_ICONINFORMATION);
        return false;
    }


    if ( node->node.isP && node->node.typ != TIP_PR ) {
        if (ask) QMessageBox::warning(this, tr(""), tr("Нельзя удалять узел с внутренней схемой"));
        return false;
    }

    int ret = QMessageBox::Yes;

    if (ask) {
        beginDraw(node);
        QString str = QString(tr("Удалить %1 %2 ?")).arg(findTableRusNameGid(node->getTable()), node->getName());
//        ret = MessageBox(str, _TR("Подтвердить удаление"),  MB_YESNO);
        ret = QMessageBox::question(this,  tr("Подтвердить удаление"), str);
        endDraw();
    }

    if (ret == QMessageBox::Yes) {
        if (node->node.isP) {
//            delCxemaFull(m_cxema, node);
        }

        if (delNodes(m_cxema.m_db, m_user, "nodes", node->id, m_user)) {
            m_cxema.m_graph->remove_node(*node);
//      GetDocument()->SetModifiedFlag(true);
            if (ask) repaint();
            return true;
        }
        else {
//            if (ask) AfxMessageBox(getAdoError(), MB_OK|MB_ICONINFORMATION);
        }
    }
    return false;
}




bool GidWidget::delLine(CLINE2 *line, bool ask)
{
    if (!isEditGid()) return false;

    int ret = QMessageBox::No;

    if (ask) {
        beginDraw(line);

        QString str = QString(tr("Удалить %1 %2 ?")).arg(findTableRusNameGid(bline(line)->getTable()), bline(line)->getName());
        ret = QMessageBox::question(this,  tr("Подтвердить удаление"), str);
        endDraw();
    }

    if (ret == QMessageBox::Yes) {
        if (delLinesobj(m_cxema.m_db, &m_cxema.m_db_2, m_user, "linesobj", bline(line)->getTableMySQL(), bline(line)->line.nomP, bline(line)->line.nomO, m_user)) {
            m_cxema.m_graph->remove_line(line);
//            GetDocument()->SetModifiedFlag(true);
            if (ask) repaint();
            return true;
        }
        else {
//            if (ask) AfxMessageBox(getAdoError(), MB_OK|MB_ICONINFORMATION);
        }
    }
    return false;
}

bool reset_shape_node(QSqlDatabase & db, int id);

QString shape_text_1(CFPoint &pt);


bool setNodeXY(QSqlDatabase &db, int id, double x, double y, int m_user) 
{
//    QString q0 = "SELECT f_table_schema, f_table_name, f_geometry_column, srid FROM geometry_columns WHERE f_table_schema = 'public' AND f_table_name = 'nodes'";
    QString q0 = "SELECT srid FROM geometry_columns WHERE f_table_schema = 'public' AND f_table_name = 'nodes'";
    QString ssrid = readTableValue(db, q0);
    
    int srid = ssrid.toInt();

    QString q = QString(

"WITH\n"
"-- Вход: id и новая точка\n"
"input(id, new_geom) AS (\n"
"  VALUES (%1, ST_SetSRID(ST_MakePoint((%2/100), -(%3/100)), %4))\n"
"),\n"

"-- Получаем старую геометрию узла\n"
"old_geom AS (\n"
"  SELECT id, shape AS old_point FROM net.v_nodes WHERE id = (SELECT id FROM input)\n"
"),\n"

"-- Обновляем точку\n"
"updated_node AS (\n"
"  UPDATE nodes\n"
"  SET x=%2, y=%3, shape = input.new_geom, operatorID=%5, archiveChangeDate=%6, sync_tgid=true\n"
"  FROM input\n"
"  WHERE nodes.id = input.id\n"
"  RETURNING nodes.id, input.new_geom, (SELECT old_point FROM old_geom) AS old_point\n"
"),\n"

"-- Выбираем и обновляем линии\n"
"to_update AS (\n"
"  SELECT\n"
"    l.id,\n"
"    CASE\n"
"      WHEN ST_DWithin(ST_StartPoint(l.shape), un.old_point, 0.001) THEN\n"
"        ST_MakeLine(\n"
"          ARRAY[\n"
"            un.new_geom\n"
"          ] || (\n"
"            SELECT array_agg((dp).geom ORDER BY (dp).path[1])\n"
"            FROM ST_DumpPoints(l.shape) AS dp\n"
"            WHERE (dp).path[1] > 1\n"
"          )\n"
"        )\n"

"      WHEN ST_DWithin(ST_EndPoint(l.shape), un.old_point, 0.001) THEN\n"
"        ST_MakeLine(\n"
"          (\n"
"            SELECT array_agg((dp).geom ORDER BY (dp).path[1])\n"
"            FROM ST_DumpPoints(l.shape) AS dp\n"
"            WHERE (dp).path[1] < ST_NPoints(l.shape)\n"
"          ) || ARRAY[\n"
"            un.new_geom\n"
"          ]\n"
"        )\n"

"      ELSE NULL\n"
"    END AS new_shape\n"
"  FROM net.v_linesobj l, updated_node un\n"
"  WHERE ST_DWithin(l.shape, un.old_point, 0.001)\n"
")\n"

"-- Применяем обновления\n"
"UPDATE linesobj\n"
"SET shape = to_update.new_shape, operatorID=%5, archiveChangeDate=%6, sync_tgid=true\n"
"FROM to_update\n"
"WHERE linesobj.id = to_update.id;\n"



).arg(id)
    .arg(x, 3, 'f')
    .arg(y, 3, 'f')
    .arg(srid)
    .arg(m_user)
    .arg(get_now());

    qDebug() << q;

    change_group_start(db, "Передвижение узла");
    bool ret =  query_exec(db, q);
    change_group_end(db);

    return ret;
}


bool setNodeXY_2(QSqlDatabase &db, const QString &tn_node0, int id, double x, double y) 
{
    int srid = 9998;

    QString tn_node = tn_node0;

    if (tn_node == "") {
        tn_node = "nodes";
    }


    tn_node = "gid." + tn_node;



    QString q = QString(

"WITH\n"
"-- Вход: id и новая точка\n"
"input(id, new_geom) AS (\n"
"  VALUES (%1, ST_SetSRID(ST_MakePoint((%2/100), -(%3/100)), %4))\n"
"),\n"

"-- Получаем старую геометрию узла\n"
"old_geom AS (\n"
"  SELECT id, nodes_shape AS old_point FROM %5 WHERE nodes_id = (SELECT id FROM input)\n"
"),\n"

"-- Обновляем точку\n"
"updated_node AS (\n"
"  UPDATE %5\n"
"  SET nodes_shape = input.new_geom\n"
"  FROM input\n"
"  WHERE %5.nodes_id = input.id\n"
"  RETURNING %5.id, input.new_geom, (SELECT old_point FROM old_geom) AS old_point\n"
"),\n"

"-- Выбираем и обновляем линии\n"
"to_update AS (\n"
"  SELECT\n"
"    l.id,\n"
"    CASE\n"
"      WHEN ST_DWithin(ST_StartPoint(l.linesobj_shape), un.old_point, 0.001) THEN\n"
"        ST_MakeLine(\n"
"          ARRAY[\n"
"            un.new_geom\n"
"          ] || (\n"
"            SELECT array_agg((dp).geom ORDER BY (dp).path[1])\n"
"            FROM ST_DumpPoints(l.linesobj_shape) AS dp\n"
"            WHERE (dp).path[1] > 1\n"
"          )\n"
"        )\n"

"      WHEN ST_DWithin(ST_EndPoint(l.linesobj_shape), un.old_point, 0.001) THEN\n"
"        ST_MakeLine(\n"
"          (\n"
"            SELECT array_agg((dp).geom ORDER BY (dp).path[1])\n"
"            FROM ST_DumpPoints(l.linesobj_shape) AS dp\n"
"            WHERE (dp).path[1] < ST_NPoints(l.linesobj_shape)\n"
"          ) || ARRAY[\n"
"            un.new_geom\n"
"          ]\n"
"        )\n"

"      ELSE NULL\n"
"    END AS new_shape\n"
"  FROM %6 l, updated_node un\n"
"  WHERE ST_DWithin(l.linesobj_shape, un.old_point, 0.001)\n"
")\n"

"-- Применяем обновления\n"
"UPDATE %6\n"
"SET linesobj_shape = to_update.new_shape\n"
"FROM to_update\n"
"WHERE %6.id = to_update.id;\n"



).arg(id)
    .arg(x, 3, 'f')
    .arg(y, 3, 'f')
    .arg(srid)
    .arg(tn_node)   // Узел
    .arg("gid.heatpipesections")   // Участок
    ;

    return query_exec(db, q);
}




bool GidWidget::setXY(CNode2 *n, CFPoint &pt)
{
  QSqlQuery query(m_cxema.m_db);
/*
  query.prepare("UPDATE nodes SET X=:x, Y=:y WHERE ID=:id");
  query.b1indValue(":x", pt.x);
  query.b1indValue(":y", pt.y);
  query.b1indValue(":id", (int)n->id);
*/

  QString shape = shape_text_1(pt);

//  query.prepare(QString("UPDATE nodes SET X=?, Y=?, shape=%1 WHERE ID=%2").arg(shape).arg((int)n->id));
//  query.addBindValue(pt.x);
//  query.addBindValue(pt.y);
//  query.addBindValue(shape);
//  query.addBindValue((int)n->id);

//  if (query_exec(m_cxema.m_db, query)) {
  if (setNodeXY(m_cxema.m_db, n->id, pt.x, pt.y, m_user)) {

#if 0
    setNodeXY_2(m_cxema.m_db_2, n->getTableMySQL(),  n->id, pt.x, pt.y);
#endif

    QString text = QString("%1 %2")
        .arg(n->node.coord.x, 0, 'f')
        .arg(n->node.coord.y, 0, 'f');
    addRemoved(m_cxema.m_db, m_user, CH_T_MOVE_NODE, n->id, text);

//    m_rectBorder.AddPoint(pt);

    n->node.coord = pt;
    m_cxema.m_graph->change();
//    GetDocument()->SetModifiedFlag(true);
    repaint();
    return true;
  }

  QMessageBox::warning(nullptr, "", QString("%1\n\n%2").arg(query.lastQuery(), query.lastError().text()));

//  AfxMessageBox(getAdoError(), MB_OK|MB_ICONINFORMATION);
  return false;
}

bool reset_shape_line(QSqlDatabase & db, int idP, int idO);


bool GidWidget::swap(CLINE2 *line)
{
  endDraw();

  CLine2 *l = bline(line);

  CCoordList cl = l->line.m_NP;
  cl.Swap();
  QString sc = cl.saveStr();
  
// externalSignID

  QString q = QString("UPDATE linesobj "
          " SET "
          "    externalSignLineID = case externalSignLineID when 4 then 5 when 5 then 4 else externalSignLineID end, "
          "    nodeID1 = nodeID2, "
          "    nodeID2 = nodeID1, "
          "    coords='%1' "
          " WHERE ID=%2 OR ID=%3").arg(sc).arg(l->line.nomP).arg(l->line.nomO);

//          " WHERE ID=%d OR ID=%d", other(l)->id, other(l)->id, where(l)->id, where(l)->id, sc, l->line.nomP, l->line.nomP);


//  bool ret = ado_execute(m_cxema->m_ado, q, &affected);
  QSqlQuery query(m_cxema.m_db);
  bool ret = query_exec(m_cxema.m_db, query, q);


  if (ret) {
//    QString qq = QString("l.id=%1 OR l.id=%2").arg(l->line.nomP).arg(l->line.nomO);
//    reset_shape_line(m_cxema.m_db, qq);
    reset_shape_line(m_cxema.m_db, l->line.nomP, l->line.nomO);
    
    m_cxema.m_graph->swap(line);

//    GetDocument()->SetModifiedFlag(true);
    repaint();

    return true;
  }
//  AfxMessageBox(getAdoError(), MB_OK|MB_ICONINFORMATION);
  return false;
}

