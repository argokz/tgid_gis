
#include <QApplication>
#include <QtGui>

#include <mainwindow.h>
#include <gidview/GidWidget.h>
#include <dialog/MMenuDial.h>

#include <QInputDialog>
#include <any/rights.h>

#include <db/db.h>

#include "std.h"

#include <property/PropertyDial.h>

#include <table/DbWindow.h>


void view_db2(DbWindow *view, const QString & title, QWidget *parent);

QString getLineMarkTable(CGraph2 *graph);


void GidWidget::outView(int node_or_line, const QString & table, const QString & title, const QString & qq, bool mark)
{
    QString ss = column_table_out(m_cxema.m_db, table, "o");
    QString q = "";

//            QString mark_line = "";


            if (node_or_line == 1) {

//            if (mark) {
//                mark_line = getQForMarkNode(graph, true);
//            }





            q = QString(
"SELECT \n"
"n.id,\n"
"ec.name AS kod,\n"
"n.externalNodeName AS uzel,\n"
"%1 FROM %2 o\n"
"JOIN net.v_nodes n ON n.id=o.nodeID\n"
"JOIN externalCodes ec ON ec.id=n.externalCodeID\n"
"-- ++ --\n"
"LEFT JOIN net.v_heatsources hs ON hs.id=o.ist\n"
"JOIN (\n"
"SELECT \n"
"c.fileID,\n"
"max(c.id) AS cid\n"
"FROM CALCULATION c\n"
"LEFT JOIN fragments fr ON fr.id=c.fileID\n"
"GROUP BY c.fileID\n"
") calc ON calc.fileID=n.fileID AND calc.cid=o.calculationID\n"
"WHERE n.removed=0 AND n.fileID IN (%3)\n"
"AND %4"
).arg(ss, table, m_cxema.m_par, qq == "" ? " 1=1" : qq);
            }
            else {

/*
            if (mark) {
                mark_line = getLineMarkTable(m_cxema.graph());
            }

            QString mm1;
            if (mark_line != "") {
                mm1 = QString("join (values %1) mark_lines(ord, id) on mark_lines.id=l.id\n").arg(mark_line);
            }
*/


            q = QString(
"SELECT\n"
"l.id,\n"
"ec1.name AS kod1,\n"
"n1.externalNodeName AS uzel1,\n"
"ec2.name AS kod2,\n"
"n2.externalNodeName AS uzel2,\n"
"%1 FROM %2 o\n"
"JOIN net.v_linesobj l ON l.id=o.lineID\n"
"JOIN net.v_nodes n1 ON n1.id=l.nodeID1\n"
"JOIN net.v_nodes n2 ON n2.id=l.nodeID2\n"
"JOIN externalCodes ec1 ON ec1.id=n1.externalCodeID\n"
"JOIN externalCodes ec2 ON ec2.id=n2.externalCodeID\n"
"-- ++ --\n"
"LEFT JOIN net.v_heatsources hs ON hs.id=o.ist\n"
"JOIN (\n"
"SELECT\n"
"c.fileID,\n"
"max(c.id) AS cid\n"
"FROM CALCULATION c\n"
"LEFT JOIN fragments fr ON fr.id=c.fileID\n"
"GROUP BY c.fileID\n"
") calc ON calc.fileID=n1.fileID AND calc.cid=o.calculationID\n"
"WHERE l.removed=0 AND n1.fileID IN (%3)\n"
"AND %4\n"
"ORDER BY l.id , o.externalSignLineID \n"
) .arg(ss, table, m_cxema.m_par, qq == "" ? " 1=1" : qq);

            }


    bool isprQForMark2Sort(CGraph2 * graph, bool is_line, int m_fileID, QString &q);

    if (mark) {
        bool srt = isprQForMark2Sort(m_cxema.graph(), node_or_line != 1, -1, q);
        if (srt) {
            q = q.replace("ORDER BY l.id, o.externalSignLineID", "");
            q = q.replace("l.id,", "l.id, id_ord*2 + o.externalSignLineID-3  as id_ord,");
        }
    }


    DbWindow *view = getTableView(m_cxema.m_db, table, q, title);

    if (view) {
        if (qq != "" && node_or_line == 2) {
            //      QString qid = getIdQ(q);
            QString qid = QString("SELECT id FROM (%1\n) __").arg(q);
            std::set<int> set_id;

            QSqlQuery query(m_cxema.m_db);
            query.setForwardOnly(true);
            if (query_exec(m_cxema.m_db, query, qid)) {
                while (query.next()) {
                    int id = query.value("id").toInt();
                    set_id.insert(id);
                }
            }

            m_cxema.m_graph->vyd_lines(set_id);
            repaint();
        }
        view->setGidWidget(this);
        view->setMoveTo(node_or_line == 1 ? move_to_node : move_to_line);

//        main_window->addWidgetTab(view, title, Qt::BottomDockWidgetArea);

        view_db2(view, title, this);
    }
}


void GidWidget::onAnalizAccepted()
{
    auto *dlg = qobject_cast<PropertyDial *>(sender());
    if (dlg) {
        QString qq = analiz_db(m_cxema.m_db, dlg->map_edited);

        QString tn = dlg->property("table").toString();
        QString st_name = dlg->property("title").toString();
        int node_or_line = dlg->property("node_line").toInt();

        outView(node_or_line, tn, st_name, qq, false);
    }
}

#include <property/PropertyDial.h>
#include <table/DbWindow.h>


bool listOut(QWidget *widget, const QString & title, QString &table_out, QString &title_out, int &node_or_line)
{
    MMenuDial menu(widget, title);

    struct MenuItemOut {
      QString name;
      QString out;
      int node_line;
    };

    MenuItemOut menudoc[] = {
      "Участки теплопроводов", "UT_OUT", 2,
      "Дроссельные органы потребителей", "DR_OUT", 1,
      "Потребители", "PT_OUT", 1,
      "Насосные агрегаты", "NS_OUT", 2,
      "Сетевые регуляторы", "RS_OUT", 2,
      "Байпасы наружных теплопроводов", "BP_OUT", 2,
      "Задвижки", "ZD_OUT", 2,
      "Регулирующая арматура", "ZD2_OUT", 2,
      "Узлы", "US_OUT", 1,
      "", "", 0
    };

    for (int i = 0; menudoc[i].name != ""; i++) menu.Add(menudoc[i].name, i);
    if (menu.exec() != QDialog::Accepted) return false;

    int ii = menu.value().toInt();

    if (ii < 0) return false;

    node_or_line = menudoc[ii].node_line;

    table_out = menudoc[ii].out;
    title_out = menudoc[ii].name;
    return true;
}


bool listAdmissibilityAnalysis(QWidget *widget, const QString & title, QString &table_out, QString &title_out, QString &mark_column, int &node_or_line, int &menuItem)
{
    MMenuDial menu(widget, title);

    struct MenuItemOut {
        QString name;
        QString out;
        int node_line;
    };

    MenuItemOut menudoc[] = {
        "Узлы, подающий трубопровод, Анализ напора", "UT_OUT", 2,
        "Узлы, обратный трубопровод, Анализ напора", "UT_OUT", 2,
        "Потребители, Анализ располагаемого напора", "UT_OUT", 2,
        "Потребители, Анализ располагаемого напора на выходе потребителя", "UT_OUT", 2,
        "Потребители, Анализ теплообеспеченности", "UT_OUT", 2,
        "Потребители, Анализ теплового режима", "UT_OUT", 2,
        "Обобщенные потребители, Анализ теплообеспеченности", "UT_OUT", 2,
        "Обобщенные потребители, Анализ располагаемого напора", "UT_OUT", 2,
        "Обобщенные потребители, Анализ теплового режима", "UT_OUT", 2,
        "Участки трубопроводов, Анализ потерь напора", "UT_OUT", 3,
        "", "", 0
    };

    QString columnMark[] = {
        "Режим","Режим","Режим","Режим напора потребителя", "Режим (отд. потреб.)", "", "", "", "", ""
    };

    for (int i = 0; menudoc[i].name != ""; i++) menu.Add(menudoc[i].name, i);
    if (menu.exec() != QDialog::Accepted) return false;

    int ii = menu.value().toInt();

    if (ii < 0) return false;

    node_or_line = menudoc[ii].node_line;

    table_out = menudoc[ii].out;
    title_out = menudoc[ii].name;
    menuItem = ii;
    mark_column = columnMark[ii];
    return true;
}

#include <table/MultiHeaderTable.h>
void view_my_table(MultiHeaderTable *view, const QString & title, QWidget *parent);
MultiHeaderTable* getMyTableView(QSqlDatabase &db, const QString & tn, const QString & q, const QString & title);
void GidWidget::onAdmissibilityAnalysis(){

    QString table, title, markColumn;
    int node_or_line, ii;

    if (!listAdmissibilityAnalysis(this, tr("Выберите объект для анализа допустимости режима"), table, title,markColumn, node_or_line, ii)) return;


    if (m_fileID==-1)
    {
        QMessageBox::warning(nullptr, "", QString("Выберите активный фрагмент!"));
        return;
    }
    QString q = readQUTF8(QString("admissibilityAnalysis/%1.sql").arg(ii+1));
    q.replace("$fileID$", QString::number(m_fileID));


    MultiHeaderTable* table1 = getMyTableView(m_cxema.m_db, "nodes", q, title);
    if (table1) {

        table1->setGidWidget(this);
        table1->setMarkColumn(markColumn);
        table1->setMarkAccessibility(true);
        table1->setEdit(false);
        table1->checkCustomHeader();
        if (node_or_line == 3)
            table1->setMoveTo(move_to_line);
        else
            table1->setMoveTo(move_to_node);
        view_my_table(table1, QString(title), this);
    }
    else {
        QMessageBox::warning(nullptr, "", QString("Нет данных в таблице %1").arg("nodes"));
    }

    // auto *dlg = new PropertyDial(this, table, "", "shape", "id");

    // dlg->initAdmissibilityAnalysis(m_cxema.m_db, title, table, node_or_line == 1 ? pr_type_node_out : pr_type_line_out);
    // dlg->show();

    // dlg->setProperty("table", table);
    // dlg->setProperty("title", title);
    // dlg->setProperty("node_line", node_or_line);

    // connect(dlg, SIGNAL(accepted()), this, SLOT(onAnalizAccepted()));
}

void GidWidget::onAnal() // Анализ...
{
    QString table, title;
    int node_or_line;

    if (!listOut(this, tr("Выберите объект для анализа режима"), table, title, node_or_line)) return;

    auto *dlg = new PropertyDial(this, table, "", "shape", "id", -1, -1);

    dlg->init_analiz(m_cxema.m_db, title, table, node_or_line == 1 ? pr_type_node_out : pr_type_line_out);
    dlg->show();

    dlg->setProperty("table", table);
    dlg->setProperty("title", title);
    dlg->setProperty("node_line", node_or_line);

    connect(dlg, SIGNAL(accepted()), this, SLOT(onAnalizAccepted()));
}
