#include <QApplication>
#include <QtGui>
#include <QtWidgets>
#include <QInputDialog>

#include <QtSql>
#include <any/MyMain.h>
#include <table/MultiHeaderTable.h>

#include "db/db.h"
#include "geo/Klassif.h"
#include "mystd.h"
#include "DockSprav.h"
#include <cxema/cxema1.h>
#include <table/DbWindow.h>
#include <db/db.h>
#include "mainwindow.h"


QString readCP1251Line(QFile &file);

void save_tree(QTreeWidget *tree, const QString & name);
void restore_tree(QTreeWidget *tree, const QString & name);
void view_db(DbWindow *view, const QString & title, QWidget *parent);


void view_db2(DbWindow *view, const QString & title, QWidget *parent)
{
    if (!view) return;

    MyMain *pm = new MyMain(title, main_window);
    view->createToolBar(pm);
    pm->setWidget(view);

    main_window->addWidgetToTabs(pm, title);
}

void view_my_table(MultiHeaderTable *view, const QString & title, QWidget *parent)
{
    if (!view) return;

    MyMain *pm = new MyMain(title, main_window);
    view->createToolBar(pm);
    pm->setWidget(view);

    main_window->addWidgetToTabs(pm, title);
}

DockSprav::DockSprav(QWidget *parent)
    : QWidget(parent)
{

    QTreeWidgetItem *i0 = nullptr;
    tree = new QTreeWidget;
    tree->setHeaderHidden(true);

    QString style1 = "QTreeWidget::item:!selected "
      "{ "
        "border: 1px solid gainsboro; "
        "border-left: none; "
        "border-top: none; "
      "}"
      "QTreeWidget::item:selected {}";

//    tree->setStyleSheet(style1);


    groupIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),
                        QIcon::Normal, QIcon::Off);
    groupIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon),
                        QIcon::Normal, QIcon::On);
    keyIcon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));



    tree->setHeaderLabels({ tr("") });
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addWidget(tree);
    setLayout(mainLayout);

#if 0

    QString fn = QString("%1kls/sprav.txt").arg(argpath());
    QFile file(fn);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        QTextStream in(&file);
//        in.setEncoding(QStringConverter::System);
        QString par;

//        while (!in.atEnd()) {
         while (!file.atEnd()) {
//            QString s = in.readLine();
            QString s = readCP1251Line(file);

            if (s.left(1) == "-") continue;

            if (s == "STRING") {
                //                par = in.readLine();
                par = readCP1251Line(file);

                i0 = new QTreeWidgetItem(tree);
                i0->setText(0, par);
                i0->setIcon(0, groupIcon);
                continue;
            }

            QUE3 que;
            QString tn;

            tn = que.tab = s;

//            que.mdb = in.readLine();
            que.mdb = readCP1251Line(file);

            if (que.mdb == "MAP") {
//                que.tab = in.readLine();
                que.tab = readCP1251Line(file);
            }

//            que.q = in.readLine();
            que.q = readCP1251Line(file);

            map_q[par + "#" + tn] = que;

            QTreeWidgetItem *i1 = new QTreeWidgetItem(i0);
            i1->setText(0, tn);
            i1->setIcon(0, keyIcon);
        }
    }
#endif


    QString fn = QString("%1kls/sprav2_ps.txt").arg(argpath());
    QFile file(fn);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        QTextStream in(&file);
//        in.setEncoding(QStringConverter::System);
        QUE3 que;
        QString par = "";
        QString tn = "";
        int nn = 0;
        QSqlDatabase *db = nullptr;

//        while (!in.atEnd()) {
         while (!file.atEnd()) {
//            QString s = in.readLine();
            QString s = readCP1251Line(file);

            if (s == "") continue;
            if (s.left(1) == "-") continue;
            if (s.left(1) == "=") {
                par = s.mid(1);

                i0 = new QTreeWidgetItem(tree);
                i0->setText(0, par);
                i0->setIcon(0, groupIcon);
                continue;
            }

            if (s.left(1) != " ") {
                tn = que.tab = s;
                que.q = "";
                que.mdb = "";
                nn = 0;

                QTreeWidgetItem *i1 = new QTreeWidgetItem(i0);
                i1->setText(0, tn);
                i1->setIcon(0, keyIcon);
            }
            else {
                QString ss = s.trimmed();
                if (ss == "") continue;

                if (nn == 0) {
                    que.mdb = ss;
                    if (ss.indexOf("MAP") == 0) {
                        static QRegularExpression re("MAP\\s+(.+)$");
                        QRegularExpressionMatch match = re.match(s);
                        if (match.hasMatch()) {
                            que.mdb = "MAP";
                            que.tab = match.captured(1);
                        }
                    }
                }
                else if (nn == 1) {
                    que.q = ss;
                }
                else if (nn == 2) {  // Дополнительные параметры
                    que.add = ss;
                }
                else if (nn == 3) {
                    que.tab = ss;
                }
                nn++;
            }

            map_q[par + "#" + tn] = que;
        }
    }


    restore_tree(tree, "Tree/DockSprav");

    tree->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(tree, &QTreeWidget::itemClicked, this, &DockSprav::itemClicked);
    connect(tree, &QTreeWidget::customContextMenuRequested, this, &DockSprav::customContextMenuRequested);
}

QString transl(const QString & s);
QString create_select_rus(QSqlDatabase & db, const QString & tn, const QString & s_shape, const QString & s_id);

#include <ui2/OpenServer.h>

void init_cs(ConnectStr &cs);
MultiHeaderTable* getMyTableView(QSqlDatabase &db, const QString & tn, const QString & q, const QString & title);
void DockSprav::edit_sprav(const QString & par, const QString & tn)
{
    auto it = this->map_q.find(par + "#" + tn);

    QString tn1 = transl(tn);

    if (it != this->map_q.end()) {
        QString q = it->second.q;
        QString mdb = it->second.mdb;
        QString tab = it->second.tab;
        QString add = it->second.add;
        QSqlDatabase *db0 = &m_cxema->m_db;

        if (mdb == "MAP") {
            Klassif *kls = m_kl_list.findKlN(tab);

            if (kls) {
                db0 = kls->m_db;
            }
        }
        else if (mdb.left(1) != "$") {
            ConnectStr cs;
            init_cs(cs);

            db0 = new QSqlDatabase();

            bool ok = connectSQL0(cs.rdbms, cs.host, cs.port, mdb, cs.user, cs.password, *db0);

            if (!ok) {
                QMessageBox::warning(this, "", "Error");
            }
        }

        if (q.indexOf("SELECT",0,Qt::CaseInsensitive) != 0) {
            if (q != "") {
                tn1 = transl(q);
                q = create_select_rus(*db0, tn1, "shape", "id");
            }

            q = create_select_rus(*db0, tn1, "shape", "id");
//            QInputDialog::getMultiLineText(nullptr, "title", "label", q);
        }
        else {
            if (tab != "") {
                tn1 = tab;
            }
            else {
                tn1 = getTableNameFromSql(q);
            }
        }

        MultiHeaderTable* table = getMyTableView(*db0, tn1, q, tn);

        if (!table) {
            QMessageBox::warning(nullptr, "", QString("Ошибка"));
            return;
        }

        table->setAdd(true);
        table->setEdit(true);
        table->setDelete(true);
        table->setDop(add);
        table->checkCustomHeader();

        view_my_table(table, tn, this);
    }
}

struct ST3
{
    //    ST1(int _typ, int _id) {id = _id; typ = _typ;};
    QString par, tn;
};


void DockSprav::itemClicked(QTreeWidgetItem *item, int column)
{
    QTreeWidgetItem *parent = item->parent();
    if (!parent) return;

    QString par = parent->text(0);
    QString tn = item->text(0);

    edit_sprav(par, tn);
}

void DockSprav::onEdit()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        QVariant var = action->data();
        ST3 st = var.value<ST3>();

        edit_sprav(st.par, st.tn);
    }
}



void DockSprav::hideEvent(QHideEvent *event)
{    
    save_tree(tree, "Tree/DockSprav");
    QWidget::hideEvent(event);
}



void DockSprav::customContextMenuRequested(const QPoint &pos)
{

    QTreeWidgetItem *nd = tree->itemAt( pos );
    if (!nd) return;

    QTreeWidgetItem *parent = nd->parent();
    if (!parent) return;

    QString par = parent->text(0);
    QString tn = nd->text(0);

    ST3 st;
    st.par = par;
    st.tn = tn;

    QMenu *menu=new QMenu(this);

    QAction *aEdit = new QAction("Редактировать", this);
    aEdit->setData(QVariant::fromValue(st));
    connect(aEdit, SIGNAL(triggered()), this, SLOT(onEdit()));
    menu->addAction(aEdit);

    menu->popup(tree->viewport()->mapToGlobal(pos));

}
