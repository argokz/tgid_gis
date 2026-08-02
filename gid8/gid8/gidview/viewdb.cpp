#include <QtGui>
#include <QtSql>
#include <QtWidgets>
#include <QVariant>
//#include <QtPrinter>
#include <cmath>
#include <QRegularExpression>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>

#include <dialog/MMenuDial.h>

#include <any/MyMain.h>
#include "mainwindow.h"
#include <gidview/GidWidget.h>
#include <cxema/graph2.h>

#include <table/DbWindow.h>
#include <edit/menu.h>


// С сортировкой

void copyVydGraph_new(CGraph2 *graph1, CGraph2 *graph2, bool is_po);
void sort_line_rs_new(CGraph2 *graph, list<CNode2*> &list_sort, list<CLINE2 *> &st_l, int fileID);


QString getQForMarkLineSort(CGraph2 * graph, bool br)
{
    QString qq = "";

    CGraph2 graph2(NULL);
    list<CNode2*> list_sort;
    list<CLINE2*> st_l;

    copyVydGraph_new(graph, &graph2, true);
    sort_line_rs_new(&graph2, list_sort, st_l, -1);

    int ord = 1;

    for (CLINE2 *l : st_l) {
        CLine *ll = &bline(l)->line;
        if (ll->nomP > 0) {
            if (qq != "") qq += ",";
            if (br) qq += "(";
            qq += QString("%1,%2").arg(ll->nomP).arg(ord++);
            if (br) qq += ")";
        }
        if (ll->nomO > 0 && ll->nomO != ll->nomP) {
            if (qq != "") qq += ",";
            if (br) qq += "(";
            qq += QString("%1,%2").arg(ll->nomO).arg(ord++);
            if (br) qq += ")";
        }
    }
    
//    QInputDialog::getMultiLineText(nullptr, "", "", qq);

    return qq;
}



QString getQForMarkNodeSort(CGraph2 * graph, bool br)
{
    QString qq = "";

    CGraph2 graph2(NULL);
    list<CNode2*> list_sort;
    list<CLINE2*> st_l;

    copyVydGraph_new(graph, &graph2, true);
    sort_line_rs_new(&graph2, list_sort, st_l, -1);

    int ord = 1;

    for (auto& pp : list_sort) {
//        CNode2* pp = p.second;

        if (qq != "") qq += ",";
        if (br) qq += "(";
        qq += QString("%1,%2").arg(pp->id).arg(ord++);
        if (br) qq += ")";
    }
//    QInputDialog::getMultiLineText(nullptr, "", "", qq);

    return qq;
}



// Без сортировки


QString getQForMarkLine(CGraph2 * graph, bool br)
{
    QString qq = "";

    for (auto& p : graph->map_node) {
        CNode2* pp = p.second;

        for (CLINE2 *l = pp->lines; l; l = nextl(l)) {
            if (IsBegin(l) && (bline(l)->line.isPjezo || bline(l)->line.isPjezoP || bline(l)->line.isPjezoO)) {
                CLine *ll = &bline(l)->line;
                if (ll->nomP > 0 && (bline(l)->line.isPjezo || bline(l)->line.isPjezoP)) {
                    if (qq != "") qq += ",";
                    if (br) qq += "(";
                    qq += QString("%1").arg(ll->nomP);
                    if (br) qq += ")";
                }
                if (ll->nomO > 0 && ll->nomO != ll->nomP  && (bline(l)->line.isPjezo || bline(l)->line.isPjezoO)) {
                    if (qq != "") qq += ",";
                    if (br) qq += "(";
                    qq += QString("%1").arg(ll->nomO);
                    if (br) qq += ")";
                }
            }
        }
    }
//    QInputDialog::getMultiLineText(nullptr, "", "", qq);

    return qq;
}


QString getQForMarkNode(CGraph2 * graph, bool br)
{
    QString qq = "";

    for (auto& p : graph->map_node) {
        CNode2* pp = p.second;

        if (pp->node.isPjezo) {
            if (qq != "") qq += ",";
            if (br) qq += "(";
            qq += QString("%1").arg(pp->id);
            if (br) qq += ")";
        }
    }
//    QInputDialog::getMultiLineText(nullptr, "", "", qq);

    return qq;
}


void isprQForMark(CGraph2 * graph, bool is_line, int m_fileID, QString &q)
{
/*
    QString pp = "";
    QString where = "";
    if (is_line) {
        pp = getQForMarkLine(graph);
        if (pp != "") {
            q = q.replace("WHERE n1.fileID=$fileID$ AND", QString("WHERE l.id IN (%1) AND").arg(pp));
        }
    }
    else {
        pp = getQForMarkNode(graph);
        if (pp != "") {
            q = q.replace("WHERE n.fileID=$fileID$ AND", QString("WHERE n.id IN (%1) AND").arg(pp));
        }
    }

    if (pp == "") {
        q.replace("$fileID$", QString("%1").arg(m_fileID));
    }
*/

    QString pp = "";
    QString where = "";
    if (is_line) {
        pp = getQForMarkLineSort(graph, true);
        if (pp != "") {
            q = q.replace("WHERE n1.fileID=$fileID$ AND", QString("\njoin (values %1) t(id, id_ord) on t.id=l.id\nWHERE").arg(pp));

            q = q.replace("l.id,", " l.id,id_ord,");
        }
    }
    else {
        pp = getQForMarkNodeSort(graph, true);
        if (pp != "") {
            q = q.replace("WHERE n.fileID=$fileID$ AND", QString("\njoin (values %1) t(id, id_ord) on t.id=n.id\nWHERE").arg(pp));

            q = q.replace("n.id,", " n.id,id_ord,");
        }
    }

    if (pp == "") {
        q.replace("$fileID$", QString("%1").arg(m_fileID));
    }


}

DbWindow *getTableView(QSqlDatabase &db, const QString & tn, const QString & q);

void view_db2(DbWindow *view, const QString & title, QWidget *parent);

void view_db(DbWindow *view, const QString & title, QWidget *parent)
{
    if (!view) return;

    MyMain *pm = new MyMain(title, parent);
    view->createToolBar(pm);
    pm->setWidget(view);

    pm->show();
}

void GidWidget::onViewDb() // Таблицы...
{
    bool all = false;
    bool mark = m_cxema.graph()->isMark();


    if (m_fileID <= 0 && !mark) {
        onMainCxema();
        if (m_fileID <= 0) {
//            QMessageBox::information(this, "", tr("Выберите активный фрагмент"));
            return;
         }
    }
    
    struct ST {
        int node_or_line;
        int typ;
        QString name;
        QString code;
        QString tn; //tableMySQL
    } st;


    do {
        QString title = tr("Таблицы");
        if (mark) {
            title = QString(tr("Таблицы в выделенном фрагменте"));
        }
        else {
            title = QString(tr("Таблицы во фрагменте %1")).arg(m_cxema.getFileName(m_fileID));
        }


        MMenuDial menu(this, title);

        if (!mark) {
            menu.AddButton("Все объекты", 1, false);
        }


        for (int i = 0; i < menuitemsize; i++) {
            if (true) {
                st.node_or_line = menudocUnvis[i].node_line;
                st.name = menudocUnvis[i].name;
                st.code = menudocUnvis[i].kode;
                st.tn = menudocUnvis[i].nameMySQL;
        //        st.typ = menudocUnvis[i].typ;
                st.typ = 9999;
                menu.Add(menudocUnvis[i].name, QVariant( QVariant::fromValue(st)));
            }
        }

        for (int i = 0; i < nodefsize; i++) {
            bool yes = true;

            if (mark) {
                yes = m_cxema.graph()->inGraphNodeVyd(nodef[i].typ, m_parent_id);
            }
            else {
                yes = m_cxema.graph()->inGraphNode(nodef[i].typ, m_parent_id, m_fileID);
            }

            if (all || yes) {
               st.node_or_line = 1;
               st.name = nodef[i].table;
               st.code = nodef[i].code;
               st.tn = nodef[i].tableMySQL;
               st.typ = nodef[i].typ;
               menu.Add(nodef[i].table, QVariant( QVariant::fromValue(st)));
            }
        }

        for (int i = 0; i < linefsize; i++) {
            bool yes = true;

            if (mark) {
                yes = m_cxema.graph()->inGraphLineVyd(linef[i].typ, m_parent_id);
            }
            else {
                yes = m_cxema.m_graph->inGraphLine(linef[i].typ, m_parent_id, m_fileID);
            }

            if (all || yes) {
                st.node_or_line = 2;
                st.name = linef[i].table;
                st.code = linef[i].code;
                st.tn = linef[i].tableMySQL;
                st.typ = linef[i].typ;
                menu.Add(linef[i].table, QVariant( QVariant::fromValue(st)));
            }
        }


        if (menu.exec() == QDialog::Accepted) {
            if (menu.button_clicked() == 1) {
                all = true;
                continue;
            }

            const ST st = menu.value().value<ST>();

            QString q = readQ(QString("sql/%1.sql").arg(st.code));

//            if () {
//            }

            if (q == "") {
                q = QString("SELECT * FROM %1").arg(st.tn);
            }

            isprQForMark(m_cxema.m_graph, st.node_or_line == 2, m_fileID, q) ;

            q = q.replace("$fileID$", QString("%1").arg(m_fileID));


    //        QInputDialog::getMultiLineText(nullptr, "title", "label", q);

            qDebug() << q;

            DbWindow *view = getTableView(m_cxema.m_db, st.tn, q, st.name);

            if (view) {
                view->setGidWidget(this);
                if (st.node_or_line != 0) {
                    view->setMoveTo(st.node_or_line == 1 ? move_to_node : move_to_line);
                }

                if (st.node_or_line == 0) {
//                    view->setEdit(true);
                }

                view_db2(view, st.name, this);
            }
        }
        break;
    } while(true);

#if 0
        return;


        if (view) {
//            view->setWindowTitle(st.name);

            view->setMoveTo(st.node_or_line == 1 ? move_to_node : move_to_line);
            view->setGidWidget(this);
//            view->setEdit(true);

#if 0

    //        MyMain *pm = new MyMain(this);
    //        pm->setWidget(view);
    //        pm->show();
            main_window->addWidgetTab(view, st.name, Qt::BottomDockWidgetArea);
//            this->setTabsClosable(true);
#else 
        MyMain *pm = new MyMain(st.name, this);
        view->createToolBar(pm);
        pm->setWidget(view);

        pm->show();
#endif
        }
    }
#endif
}
