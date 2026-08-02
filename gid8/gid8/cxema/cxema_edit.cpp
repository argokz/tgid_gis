#include <QCoreApplication>
#include <QtWidgets>
#include <QtSql>

#include "cxema/Graph.h"
#include "cxema/cxema_names.h"
#include "cxema1.h"
#include <db/db.h>


bool edit(QSqlDatabase &db, QWidget *window, const QString & tn, int n);

bool CCxema::info(CFPoint point, double delta, QWidget *window)
{
    CNode2 *node = m_graph->find(0, point, delta);
    CLINE2 *line = m_graph->findLine(0, point, delta);

    QSqlQuery query(m_db);
    query.setForwardOnly(true);

    if (node) {
        int n = node->node.nomP != -1 ? node->node.nomP : node->node.nomO;
        QString tn = nodef[node->node.typ].tableMySQL;

        tn = "nodes";

        if (edit(m_db, window, tn, n)) {
            if (query_exec(m_db, query, QString("SELECT * FROM %1 WHERE id=%2").arg(tbl_sql(tn)).arg(n))) {
                QSqlRecord record = query.record();
//        init_PT(record);
                if (query.next()) {
                    readNodeNew(query, node, node->node.typ, m_graph);
                    window->repaint();
                }
            }
        }
    }
    else if (line) {

        CLine2 *l = bline(line);
        QString tn = linef[l->line.typ].table;
        int n = l->line.nomP != -1 ? l->line.nomP : l->line.nomO;

        tn = "linesobj";

        if (edit(m_db, window, tn, n)) {
            if (query_exec(m_db, query, QString("SELECT * FROM %1 WHERE id=%2").arg(tbl_sql(tn)).arg(n))) {

                QSqlRecord record = query.record();
                if (query.next()) {
                    readLineNew(query, l, l->line.typ, m_graph);
                    window->repaint();
                }

            }
        }
    }
    return true;
}
