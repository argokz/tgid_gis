#include <QtGui>

#include <mainwindow.h>
#include <gidview/GidWidget.h>
#include <dialog/MMenuDial.h>

#include <QInputDialog>

#include <db/db.h>

#include <cxema/undo.h>

#include "std.h"

#include <dialog/TableWindow.h>

#include <init/init_files.h>

//bool create_vyd_line_table(QSqlDatabase &db, CGraph2* graph, QString& tn);
//bool create_vyd_node_table(QSqlDatabase &db, CGraph2* graph, QString& tn);

QString create_vyd_node_table1(CGraph2 * graph)
{
    QString vals = "";

    for(auto p : graph->map_node) {
        CNode2* n = p.second;
        if (n->node.isPjezo) {
            if (vals != "") vals += ",";
            vals += QString("%1").arg(n->id);
        }
    }

    //QString q = "select id from (values %) tmpn(id)".arg(vals);

    return vals;
}


QString create_vyd_line_table1(CGraph2 * graph)
{
    QString vals = "";

    for(auto p : graph->map_node) {
        CNode2* n = p.second;
        for (CLINE2* l = n->lines; l; l = nextl(l)) {
            if (IsBegin(l) && bline(l)->line.isPjezo) {
                if (bline(l)->line.nomP > 0) {
                    if (vals != "") vals += ",";
                    vals += QString("%1").arg(bline(l)->line.nomP);
                }

                if (bline(l)->line.nomO > 0 && bline(l)->line.nomP != bline(l)->line.nomO) {
                    if (vals != "") vals += ",";
                    vals += QString("%1").arg(bline(l)->line.nomO);
                }
            }
        }
    }

//    QString q = "select id from (values %) tmpn(id)".arg(vals);

    return vals;
}


bool setMarkNodeLine2Value(QSqlDatabase &db, CGraph2* graph, const QString & tn, const QString & fn, const QVariant & vval, bool all, bool isNode, const QString & node_line_id)
{
    bool ret = false;
    QString tmpn = "temp_table1";
    QString q;


    qDebug() << "vval " << vval.isNull(); // true

    QString val = vval.toString();

    QSqlQuery query(db);

    if (all) {
        q = QString("UPDATE %1 SET %2=%3").arg(tn).arg(fn).arg(val);
        if (vval.isNull()) {
            q = QString("UPDATE %1 SET %2=null").arg(tn).arg(fn);
        }
        ret = query_exec(db, query, q);
    }
    else {
        QString vals;
        if (isNode) {
            vals = create_vyd_node_table1(graph);
        }
        else {
            vals = create_vyd_line_table1(graph);
        }

        if (vals != "") {
            q = QString("UPDATE %1 SET %2=%3 WHERE %4 IN (%5)").arg(tn).arg(fn).arg(val).arg(node_line_id).arg(vals);

            if (vval.isNull()) {
                q = QString("UPDATE %1 SET %2=null WHERE %4 IN (%5)").arg(tn).arg(fn).arg(node_line_id).arg(vals);;
            }

            ret = query_exec(db, query, q);
        }
    }
    return ret;
}


bool setMarkNode2Value(QSqlDatabase &db, CGraph2* graph, const QString & tn, const QString & fn, const QVariant & val, bool all)
{
    return setMarkNodeLine2Value(db, graph, tn, fn, val, all, true, "nodeID");
}

bool setMarkNode1Value(QSqlDatabase &db, CGraph2* graph, const QString & tn, const QString & fn, const QVariant & val, bool all)
{
    return setMarkNodeLine2Value(db, graph, tn, fn, val, all, true, "id");
}



bool setMarkLine1Value(QSqlDatabase &db, CGraph2* graph, const QString & fn, const QVariant & val, bool all)
{
    return setMarkNodeLine2Value(db, graph, "linesobj", fn, val, all, false, "ID");
}

bool setMarkLine2Value(QSqlDatabase &db, CGraph2* graph, const QString & tn, const QString & fn, const QVariant & val, bool all)
{
    return setMarkNodeLine2Value(db, graph, tn, fn, val, all, false, "lineID");
}


void setT(QSqlDatabase &db, const QString & label, const QString & tn, const QString & fn)
{
}

#include <property/variantdelegate.h>
QVariant prop_fun1(QWidget *widget, const QString & fun, QVariant value, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);

QVariant getValueForTable(QWidget *widget, QSqlDatabase &db, int pr_fileID, const QString & label, const QString & table, const QString & col, bool *ok, QString &rus, const QVariant &def_value, QString &txt)
{
//    QString rus = col;
    QVariant v;

    rus = col;
    const CColName *c = findColumnRusNameFull("gid", table, col);

    *ok = true;

    if (c) {
        rus = c->name;
    }

    const ForeignTable *ft = findLookup("gid", table, col);
    if (ft) {
        std::map<int, QString> *l2 = findLookup2("gid", ft->table);
        if (l2) {
            MMenuDial menu(widget, rus);
            for (auto &it : *l2) {
                menu.Add(it.second, it.first);
            }
            if (menu.exec() == QDialog::Accepted) {
                txt = menu.curText;
                return menu.value().toInt();
            }
        }
        else {
            QString q = QString("SELECT %1,%2 AS %3 FROM %4").arg(ft->id, ft->name, quot_text("Наименование"), ft->table);

            if (column_in_table(db, ft->table, "fileID")) {
                q = QString("SELECT %1,%2 AS %3 FROM %4 WHERE fileID=%5").arg(ft->id, ft->name, quot_text("Наименование"), ft->table).arg(pr_fileID);
                if (pr_fileID <= 0) {
                    QMessageBox::warning(widget, "", "Для выполнения данной операции необходимо выбрать фрагмент");
                    *ok = false;
                    return v;
                }
            }

            TableWindow tab(db, "", rus, q, widget, -1, false, true);
            if (tab.exec()) {
                txt = tab.value("Наименование").toString();
                return tab.value(ft->id);
            }
        }
    }
    else {
        QInputDialog input(widget);
        input.setInputMode(QInputDialog::TextInput);
        input.setTextValue("");
        input.setLabelText(rus);

        QList<QLineEdit *> ll = input.findChildren<QLineEdit *>();
        for (QLineEdit *l :  ll) {
//            QIcon actionIcon;
//            QSize sz(16, 16);
//            actionIcon.addPixmap(widget->style()->standardIcon(QStyle::SP_TitleBarUnshadeButton).pixmap(sz));

QIcon *icon_from_text(const QString & str, int cx, int cy);

            QIcon *actionIcon = icon_from_text("...", 16, 16);

            QAction *myAction = l->addAction(*actionIcon, QLineEdit::TrailingPosition);
            l->connect(myAction, &QAction::triggered, [&]( bool on ) {
                QVariant value;
                bool ok, yes;
                QVariant var = prop_fun1(widget, "Date", value, rus, db, &ok, &yes);
                if (ok) {
                    l->setText(VariantDelegate::displayText(var));
                }
            }
            );
        }




/*
        QIcon actionIcon;
        QSize sz(16, 16);
        actionIcon.addPixmap(dialog->style()->standardIcon(QStyle::SP_TitleBarUnshadeButton).pixmap(sz));

        QAction *myAction = lineEdit->addAction(actionIcon, QLineEdit::TrailingPosition);
        myAction->setProperty("col", QVariant::fromValue(pr));
        tree->connect(myAction, SIGNAL(triggered()), dialog, SLOT(clicked()));
*/

//        if (input.exec()) {
//        }


        double d0 = def_value.toDouble();
        double d = QInputDialog::getDouble(widget, rus,  rus, d0, -2147483647, 2147483647, 1, ok,  Qt::WindowFlags(), 1);
        return d;

    }

    *ok = false;
    return v;
}

bool GidWidget::setSomething(bool is_node, int typ, const QString &col, const QVariant &def_value, QVariant &val)
{
//    int typ = TIP_PR;
//    QString col = "responsibleID";

//    bool is_node = true;

    QString rus, txt, table, label;

    if (is_node)  {
        label = getNodeTable(typ);
        table = getNodeTableMySQL(typ);

        if (typ == TIP_US) {
            table = "nodes";
        }
    }
    else {
        label = getLineTable(typ);
        table = getLineTableMySQL(typ);
        if (col == "organizationID") {
            table = "linesobj";
        }
    }


    CGraph2 *graph = m_cxema.graph();

    bool ok;
    bool ret = false;


    val = getValueForTable(this, m_cxema.m_db, m_fileID, label, table, col, &ok, rus, def_value, txt);

    if (ok) {
        QString str = QString(tr(
        "Внимание!\n"
        "При выполнении этой операции все поля '%1' объекта '%2' будут установлены в '%3'\n"
        "Выполнить операцию?"))
            .arg(rus, label, txt);

        if (QMessageBox::question(0, "", str) == QMessageBox::Yes) {

            change_group_start(m_cxema.m_db, QString("Операция по установлению значений поля '%1'").arg(label));
            
            if (is_node) {
                if (table == "nodes") {
                    setMarkNode1Value(m_cxema.m_db, m_cxema.graph(), table, col, val, false);
                }
                else {
                    setMarkNode2Value(m_cxema.m_db, m_cxema.graph(), table, col, val, false);
                }
            }
            else {
                if (table == "linesobj") {
                    setMarkLine1Value(m_cxema.m_db, m_cxema.graph(), col, val, false);
                }
                else {
                    setMarkLine2Value(m_cxema.m_db, m_cxema.graph(), table, col, val, false);
                }
            }
            change_group_end(m_cxema.m_db);
            return true;
        }
    }


    return false;
}

bool GidWidget::setValue(int typ, const QString & col, const QVariant &val)
{
    bool is_node = true;

    QString rus, txt, table, label;


    table = getNodeTableMySQL(typ);
    label = getNodeTable(typ);

    change_group_start(m_cxema.m_db, QString("Операция по установлению значений поля '%1'").arg(col));

    if (table == "") {
        table = getLineTableMySQL(typ);
        label = getLineTable(typ);
        is_node = false;
    }

    if (is_node) {
        setMarkNode2Value(m_cxema.m_db, m_cxema.graph(), table, col, val, false);
    }
    else {
        setMarkLine2Value(m_cxema.m_db, m_cxema.graph(), table, col, val, false);
    }

    change_group_end(m_cxema.m_db);

    return true;
}



