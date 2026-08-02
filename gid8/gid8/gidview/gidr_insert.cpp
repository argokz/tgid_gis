#include <QtGui>
#include <QtSql>
#include <QVariant>

#include <mainwindow.h>
#include <gidview/GidWidget.h>
#include <dialog/MMenuDial.h>
#include <db/db.h>

bool ispr_us2(QSqlDatabase &db, int id)
{
    QString q = QString(
      "UPDATE nc "
      "SET  "
      "    nc.externalNodeName=n.externalNodeName, "
      "    nc.externalCodeID=n.externalCodeID, "
      "    nc.nodeName=n.nodeName "
      "FROM nodes nc "
      "JOIN connectNodes cn ON cn.nodeID=nc.id "
      "JOIN nodes n ON n.id=cn.connectID "
      "WHERE n.id=%1").arg(id);

    return execQ(db, q);
}

// Копировать одну строку Участка

int copyRowLine2_(QSqlDatabase &db, const QString & tn, int id, int lineID)
{
    QString foo = "#foo";
    bool ret;

    QString param = get_table_columns_txt(db, tn, "id");

    ret = execQ(db, QString("DROP TABLE %1").arg(foo), false);

    ret = execQ(db, QString("SELECT %1 INTO %2 FROM %3 WHERE id=%4").arg(param, foo, tn).arg(id), true);

    if (!ret) return -1;

    ret = execQ(db, QString("UPDATE %1 SET lineID=%2").arg(foo).arg(lineID), true);
    if (!ret) return -1;

//    int id2 = execInsertQ(db, QString("INSERT INTO %1 SELECT %2 FROM %3").arg(tn, param, foo), true);

//    std::map<QString, QVariant> data;

//    int id2 = insertIntoDatabase(db, tn, );

    int id2 = 0;

//    ret = execQ(db, QString("DROP TABLE %1").arg(foo), true);
    return id2;
}

int copyRowLine1Dubl(QSqlDatabase &db, const QString & tn, int nodeID1, int id, const QString & coord)
{
#if 0
    QString foo = "#foo";
    bool ret;

    ret = execQ(db, QString("DROP TABLE %1").arg(foo), false);

    QString param = get_table_columns_txt(db, tn, "id");

    ret = execQ(db, QString("SELECT %1 INTO %2 FROM %3 WHERE id=%4").arg(param, foo, tn).arg(id), true);
    if (!ret) return -1;

    ret = execQ(db, QString("UPDATE %1 SET nodeID1=%2, coords='%3'").arg(foo).arg(nodeID1).arg(coord), true);
    if (!ret) return -1;

    int id2 = execInsertQ(db, QString("INSERT INTO %1 SELECT %2 FROM %3").arg(tn, param, foo), true);

    ret = execQ(db, QString("DROP TABLE %1").arg(foo), true);
    if (!ret) return -1;
#endif
    int id2 = 0;
    return id2;
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

bool GidWidget::insertNode(CNode2 *n, CLINE2 *line, double r)
{
    CCoordList cl1, cl2;


    CNode2 *n1 = where(line), *n2 = other(line);
    CLine2 *l = bline(line);

    isprLine(n->node.coord, n1->node.coord, n2->node.coord, l->line.m_NP, cl1, cl2, r);

    QString s1_coords = cl1.saveStr();
    QString s2_coords = cl2.saveStr();

//    l->line.m_NP = cl1;

    int nomP2 = -1, nomO2 = -1;
    int idP2 = -1, idO2 = -1;

    QString q;

    if (l->line.nomP > 0) {
        nomP2 = copyRowLine1Dubl(m_cxema.m_db, "linesobj", n->id, l->line.nomP, s2_coords);
        idP2 = copyRowLine2_(m_cxema.m_db, l->getTableMySQL(), l->line.idP2, nomP2);
    }


    if (l->line.nomO > 0 && l->line.nomO != l->line.nomP) {
        nomO2 = copyRowLine1Dubl(m_cxema.m_db, "linesobj", n->id, l->line.nomO, s2_coords);
        idO2 = copyRowLine2_(m_cxema.m_db, l->getTableMySQL(), l->line.idO2, nomO2);
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

    q = QString("UPDATE linesobj SET coords='%1', nodeID2=%2 WHERE id IN (%3, %4)").arg(s1_coords).arg(n->id).arg(l->line.nomP).arg(l->line.nomO);
    bool ret = execQ(m_cxema.m_db, q);

    q = QString("UPDATE cn SET connectID=%1 FROM connectNodes cn WHERE connectID IN (%2,%3)").arg(n->id).arg(n1->id).arg(n2->id);
    bool ret2 = execQ(m_cxema.m_db, q);
    ispr_us2(m_cxema.m_db, n->id);

    CLINE2 *line1 = m_cxema.m_graph->insert_line(n1, n, cl1);
    if (line1) {
        bline(line1)->line = l->line;
        bline(line1)->line.m_NP = cl1;
    }

    CLINE2 *line2 = m_cxema.m_graph->insert_line(n, n2, cl2);
    if (line1) {
        bline(line2)->line = l->line;

        bline(line2)->line.nomP = nomP2;
        bline(line2)->line.nomO = nomO2;

        bline(line2)->line.idP2 = idP2;
        bline(line2)->line.idO2 = idO2;
        bline(line2)->line.m_NP = cl2;
    }

    m_cxema.m_graph->remove_line(line);

    return true;
}
