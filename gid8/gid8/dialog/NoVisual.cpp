#include <QtGui>
#include <QtWidgets>
#include <QtSql>

#include <db/db.h>

#include <gidview/GidWidget.h>

#include "mainwindow.h"

#include "NoVisual.h"
#include "cxema/Graph.h"
#include "cxema/graph2.h"
#include "geodez/geodez.h"

#include <property/PropertyDial.h>


bool inTable(QSqlDatabase &db, const QString & tn, const QString & fn, int id)
{
    QString q = QString("SELECT id FROM %1 WHERE %2=%3").arg(tn).arg(fn).arg(id);

    if (sizeOfQ(db, q) > 0) {
        return true;
    }
    return false;
}


bool inConsumer(QSqlDatabase &db, const QString & fn, int id)
{
    if (inTable(db, "realConsumers", fn, id)) return true;
    if (inTable(db, "generalizedConsumers", fn, id)) return true;

    return false;
}

bool inTableNode(QSqlDatabase &db, const QString & tn, const QString & fn, int id)
{
    QString q = QString("SELECT id FROM net.v_nodes n WHERE %1=%2 and n.removed=0").arg(fn).arg(id);

    if (sizeOfQ(db, q) > 0) {
        return true;
    }
    return false;
}


bool inNode(QSqlDatabase &db, const QString & fn, int id)
{
    if (inTableNode(db, "nodes", fn, id)) return true;

    return false;
}



enum ST2Typ
{
    st2_typ_add,
    st2_typ_edit,
    st2_typ_delete,
    st2_typ_tg
};

struct ST2
{
    QSqlDatabase *db;
    QString title;
    QString tn;
    QString fn;
    int id;
    int idn;
    int typ;
    bool system = false;
    bool header = false;
    QTreeWidgetItem *i = nullptr;
};

QString findTableRusName(const QString & d, const QString & n);

void save_tree(QTreeWidget *tree, const QString & name);
void restore_tree(QTreeWidget *tree, const QString & name);

void initCT(QTreeWidgetItem *tree, QSqlDatabase &db, int fileID, QIcon &keyIcon)
{
    QString q = QString("SELECT * FROM heatSystem");

    QSqlQuery query(db);
    query.setForwardOnly(true);

    foreach(auto i, tree->takeChildren()) delete i;

    if (query_exec(db, query, q)) {
        while (query.next()) {
            int id = query.value("id").toInt();
            QString s = query.value("name").toString();
//            HTREEITEM h = tree->InsertItem(s, h0);
//            tree->SetItemData(h, id);

            QTreeWidgetItem *i = new QTreeWidgetItem(tree);
            i->setText(0, s);
            i->setIcon(0, keyIcon);

            ST2 st;
            st.db = &db;
            st.tn = "heatSystem";
            st.title = findTableRusName("gid", st.tn);
            st.id = id;
            st.system = true;

            i->setData(0, Qt::UserRole, QVariant::fromValue(st));

            double Central_Meridian    = query.value("Central_Meridian").toDouble();
            double Latitude_Of_Origin  = query.value("Latitude_Of_Origin").toDouble();
            double False_Easting       = query.value("False_Easting").toDouble();
            double False_Northing      = query.value("False_Northing").toDouble();
            double Scale_Factor        = query.value("Scale_Factor").toDouble();
            double Angle               = query.value("Angle").toDouble();
            if (False_Easting == 0) False_Easting = Central_Meridian * 10000;

            double dx               = query.value("dx").toDouble();
            double dy               = query.value("dy").toDouble();
            
            set_system_coord(False_Easting, False_Northing, Central_Meridian, Latitude_Of_Origin, Scale_Factor, Angle, dx, dy);


//            i0101->setIcon(0, keyIcon);

/*
            double Central_Meridian = ado->read_double("Central_Meridian");
            double Latitude_Of_Origin  = ado->read_double("Latitude_Of_Origin");
            double False_Easting       = ado->read_double("False_Easting");
            double False_Northing      = ado->read_double("False_Northing");
            double Scale_Factor        = ado->read_double("Scale_Factor");
            double Angle               = ado->read_double("Angle");
            if (False_Easting == 0) False_Easting = Central_Meridian * 10000;

            set_system_coord(False_Easting, False_Northing, Central_Meridian, Latitude_Of_Origin, Scale_Factor, Angle);
*/
        }
    }
}


//void initAdo(CTreeCtrl *tree, HTREEITEM h0, CAdoFile *ado, const char *tn, const char *fn, int fileID, const char *fn2 = "fileID")
//void initAdo(QTreeWidgetItem *tree, QSqlDatabase &db, const QString & tn, const QString & fn, int fileID, QIcon &keyIcon, const QString & fn2 = "fileID")
void initAdo(QTreeWidgetItem *tree, QSqlDatabase &db, const QString & tn, const QString & fn, int fileID, QIcon &keyIcon, const QString & fn2, bool srt = false)
{
  QString fn3;

//  q.Format("SELECT id, %s FROM %s WHERE %s=%d ORDER BY %s", fn, tn, fn2, fileID, fn3);

  QString q = QString(

"  SELECT\n"
"      id,\n"
"      name,\n"
"      al,\n"
"      CONVERT(INT, IIF(PATINDEX('%%[^0-9]%%', num1) > 1, LEFT(num1, PATINDEX('%%[^0-9]%%', num1) - 1), num1)) AS num\n"

"      FROM(\n"
"          SELECT\n"
"          id,\n"
"          %1 AS name,\n"
"          IIF(PATINDEX('%%[0-9]%%', %1) > 1, SUBSTRING(%1, PATINDEX('%%[0-9]%%', %1), LEN(%1)), '0') AS num1,\n"
//"          IIF(PATINDEX('%%[0-9]%%', %1) > 1, LEFT(%1, PATINDEX('%%[0-9]%%', %1) - 1), '') AS al\n"
"          IIF(PATINDEX('%%[0-9]%%', %1) > 1, LEFT(%1, PATINDEX('%%[0-9]%%', %1) - 1), %1) AS al\n"
"          FROM %2\n"
"          WHERE %3 = %4\n"
"      ) _AA\n"
"      ORDER BY\n"
"       UPPER(al),\n"
"       num,\n"
"       name").arg(fn, tn, fn2).arg(fileID);

    if (is_POSTGRESQL()) {
        q = QString("SELECT id, %1 AS name FROM %2 WHERE %3=%4")
            .arg(fn, tn, fn2)
            .arg(fileID);

        if (srt) {
            q = QString("SELECT id, %1 AS name FROM %2 WHERE %3=%4 ORDER BY %1")
                    .arg(fn, tn, fn2)
                    .arg(fileID)
                ;

        }
    }

    QSqlQuery query(db);
    query.setForwardOnly(true);

    foreach(auto i, tree->takeChildren()) delete i;

//    QInputDialog::getMultiLineText(nullptr, "q", "q", q);

    if (query_exec(db, query, q)) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString s = query.value(1).toString();
            if (s == "") s = "???";

            QTreeWidgetItem *i = new QTreeWidgetItem(tree);
            i->setText(0, s);
            i->setIcon(0, keyIcon);

            ST2 st;
            st.db = &db;
            st.tn = tn;
            st.fn = fn;
            st.title = findTableRusName("gid", st.tn);
            st.id = id;
            st.i = tree;

            i->setData(0, Qt::UserRole, QVariant::fromValue(st));
        }
    }
}


void initFR(QTreeWidgetItem *tree, QSqlDatabase &db, int fileID, QIcon &keyIcon)
{
    initAdo(tree, db, "fragments", "name", fileID, keyIcon, "id");
}

bool readTableMap(QSqlDatabase &db, const QString & q, const QString & name_id, const QString & name_name, std::map<int, QString>& map1);


void initRS(QTreeWidgetItem *tree, QSqlDatabase &db, int fileID, QIcon &keyIcon)
{
    initAdo(tree, db, "externalCodes", "name", fileID, keyIcon, "fileID", true);
}

void initUR(QTreeWidgetItem *tree, QSqlDatabase &db, int fileID, QIcon &keyIcon)
{
    initAdo(tree, db, "specExpends", "specExpendID", fileID, keyIcon, "fileID", true);
}

void initTR(QTreeWidgetItem *tree, QSqlDatabase &db, int fileID, QIcon &keyIcon)
{
    initAdo(tree, db, "calcTemperatures", "calcTemperatureID", fileID, keyIcon, "fileID", true);
}

void initKV(QTreeWidgetItem *tree, QSqlDatabase &db, int fileID, QIcon &keyIcon)
{
    initAdo(tree, db, "varCoefficients", "kodkv", fileID, keyIcon, "fileID", true);
}

void initGV(QTreeWidgetItem *tree, QSqlDatabase &db, int fileID, QIcon &keyIcon)
{
    initAdo(tree, db, "gvsLoadGraphs", "gvsLoadGraphID", fileID, keyIcon, "fileID", true);
}


void initIST(QTreeWidgetItem *tree, QSqlDatabase &db, int fileID, QIcon &keyIcon, int tec, CGraph2 *graph)
{
//    initAdo(tree, db, "varCoefficients", "kodkv", fileID, keyIcon);
    QString where;

//    q.Format("SELECT heatSources.id, heatSources.sourceName FROM net.v_heatsources JOIN net.v_nodes ON heatSources.nodeID=nodes.id WHERE fileID=%d", fileID);

    if (tec == 1 || tec == 2 || tec == 3) {
      where = QString("hSourceTypeID=%1").arg(tec);
    }
    else {
      where = "NOT hSourceTypeID IN (1,2,3)";
    }

    QString q = QString("SELECT heatSources.id, nodes.id AS idn, heatSources.sourceName, heatSources.name, nodes.externalNodeName FROM net.v_heatsources JOIN net.v_nodes ON heatSources.nodeID=nodes.id WHERE %1 ORDER BY hSourceTypeID, heatSources.name, heatSources.sourceName, nodes.externalNodeName").arg(where);


    QSqlQuery query(db);
    query.setForwardOnly(true);

    foreach(auto i, tree->takeChildren()) delete i;

    if (query_exec(db, query, q)) {
        while (query.next()) {
            int id = query.value(0).toInt();
            int idn = query.value("idn").toInt();
            if (graph && graph->findNode2(TIP_IS, id)) {
                QString s1 = query.value("sourceName"      ).toString();
                QString s2 = query.value("name"            ).toString();
                QString s3 = query.value("externalNodeName").toString();

                QString s = s2;
                if (s == "") s = s1;
                if (s == "") s = s3;

                QTreeWidgetItem *i = new QTreeWidgetItem(tree);
                i->setText(0, s);
                i->setIcon(0, keyIcon);

                ST2 st;
                st.db = &db;
                st.tn = "heatSources";
                st.fn = s;
                st.title = findTableRusName("gid", st.tn);
                st.id = id;
                st.idn = idn;
                st.i = tree;

                i->setData(0, Qt::UserRole, QVariant::fromValue(st));
            }
        }
    }
}


/*

void initNoVisual()
{
    QVariant val;
    if (dlg->getNewValue("heatSystem", "name", val)) {
        initCT(i_CT, *m_db, -1, keyIcon);
    }
    if (dlg->getNewValue("fragments", "name", val)) {
        initFR(i_FR, *m_db, m_fileID, keyIcon);
    }
    if (dlg->getNewValue("externalCodes", "name", val)) {
        initFR(i_RS, *m_db, m_fileID, keyIcon);
    }
    if (dlg->getNewValue("specExpends", "specExpendID", val)) {
        initUR(i_UR, *m_db, m_fileID, keyIcon);
    }
    if (dlg->getNewValue("calcTemperatures", "calcTemperatureID", val)) {
        initTR(i_TR, *m_db, m_fileID, keyIcon);
    }
    if (dlg->getNewValue("varCoefficients", "kodkv", val)) {
        initKV(i_KV, *m_db, m_fileID, keyIcon);
    }
}
*/


#include <any/rights.h>


void NoVisual::onPropertyAccepted()
{
    auto *dlg = qobject_cast<PropertyDial *>(sender());
    if (dlg) {
        if (!UserRight::isEditGid()) {
            QMessageBox::warning(this, "", QString("Редактирование запрещено!"));
            return;
        }

        if (update_db(dlg->map_edited, -1)) {
            QVariant val;
//            if (dlg->getNewValue("heatSystem", "name", val)) {
//                initCT(i_CT, *m_db, -1, keyIcon);
//            }

            if (QString::compare(dlg->m_table, "heatSystem", Qt::CaseInsensitive) == 0) {
                initCT(i_CT, *m_db, -1, keyIcon);
            }

            if (dlg->getNewValue("fragments", "name", val)) {
                initFR(i_FR, *m_db, m_fileID, keyIcon);

                m_cxema->openFragments(m_cxema->m_par);

                main_window->m_left_gid->init(gid, m_cxema);
            }
            if (dlg->getNewValue("externalCodes", "name", val)) {
                initRS(i_RS, *m_db, m_fileID, keyIcon);
                readTableMap(*m_db, "SELECT id,name FROM externalCodes WHERE removed = 0", "id", "name", m_cxema->graph()->map_kod);
            }
            if (dlg->getNewValue("specExpends", "specExpendID", val)) {
                initUR(i_UR, *m_db, m_fileID, keyIcon);
            }
            if (dlg->getNewValue("calcTemperatures", "calcTemperatureID", val)) {
                initTR(i_TR, *m_db, m_fileID, keyIcon);
            }
            if (dlg->getNewValue("varCoefficients", "kodkv", val)) {
                initKV(i_KV, *m_db, m_fileID, keyIcon);
            }
        }
    }
}

void NoVisual::onEdit()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        ST2 st = action->data().value<ST2>();

        auto *dlg = new PropertyDial(this, *st.db, st.title, st.tn, st.tn, st.id, -1, pr_type_any, m_cxema);
        dlg->show();

        connect(dlg, SIGNAL(accepted()), this, SLOT(onPropertyAccepted()));
//        connect(dlg, SIGNAL(rejected()), this, SLOT(onPropertyRejected()));
    }
}

#include <tg/TGDialog.h>

void NoVisual::onTg()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        ST2 st = action->data().value<ST2>();

        CNode2 *n = m_cxema->m_graph->find(st.idn);

        if (n) {
            TGDialog tg(gid, m_cxema, m_cxema->m_db, n->id, n->node.id2);
            tg.exec();
        }
    }
}



void NoVisual::onDelete()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action) return;

    ST2 st = action->data().value<ST2>();

    int m_id = st.id;

    if (st.tn == "heatSystem") {
        if (inConsumer(*m_db, "specExpendID", m_id)) {
            QMessageBox::warning(this, "", tr("Нельзя удалить"));
            return;
        }
    }
    else if (st.tn == "externalCodes") {
        if (inNode(*m_db, "externalCodeID", m_id)) {
            QMessageBox::warning(this, "", tr("Нельзя удалить"));
            return;
        }
    }
    else if (st.tn == "varCoefficients") {
        if (inConsumer(*m_db, "varCoeffID", m_id) || inTable(*m_db, "heatPipeSections", "varCoeffIDflow", m_id) || inTable(*m_db, "heatPipeSections", "varCoeffIDret", m_id)) {
            QMessageBox::warning(this, "", tr("Нельзя удалить"));
            return;
        }
    }
    else if (st.tn == "gvsLoadGraphs") {
        if (inConsumer(*m_db, "gvsLoadGraphID", m_id)) {
            QMessageBox::warning(this, "", tr("Нельзя удалить"));
            return;
        }
    }
    else if (st.tn == "calcTemperatures") {
        if (inConsumer(*m_db, "calcTemperatureID", m_id)) {
            QMessageBox::warning(this, "", tr("Нельзя удалить"));
            return;
        }
    }
    else {
        QMessageBox::warning(this, "", tr("Нельзя удалить"));
        return;
    }

    if (QMessageBox::question(0, "", QString(tr("Удалить строку № %1 в таблице %2 ?")).arg(m_id).arg(st.tn)) != QMessageBox::Yes) return;

    if (delObject(*m_db, st.tn, m_id)) {

//        void initAdo(QTreeWidgetItem *tree, QSqlDatabase &db, const QString & tn, const QString & fn, int fileID, QIcon &keyIcon, const QString & fn2 = "fileID");

        initAdo(st.i, *m_db, st.tn, st.fn, m_fileID, keyIcon, "fileID", true);
    }
    else {
        QMessageBox::warning(this, "", tr("Ошибка"));
    }
}

void NoVisual::onMoveTo()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action) return;

    ST2 st = action->data().value<ST2>();

    gid->moveNode(st.idn);

//    if (delObject(*m_db, st.tn, m_id)) {
//    }
}



int add_row(QSqlDatabase &db, const QString & tn, int fileID)
{
//    QString q = QString("INSERT INTO %1 (fileID) VALUES (%2)").arg(tn).arg(fileID);
//    int id = ExecuteInsert(db, q);

    std::map<QString, QVariant> data;
    data["fileID"] = fileID;
    int id = insertIntoDatabase(db, tn, data);

    return id;
}

void NoVisual::onAdd()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action) return;

    ST2 st = action->data().value<ST2>();

    int id = add_row(*m_db, st.tn, m_fileID);

    if (id <= 0) return;

    if (st.i == i_CT) {
//        initCT(i_CT, *m_db, -1, keyIcon);
    }
    else if (st.i == i_FR) {
        initFR(i_FR, *m_db, m_fileID, keyIcon);
    }
    else if (st.i == i_RS) {
        initRS(i_RS, *m_db, m_fileID, keyIcon);
    }
    else if (st.i == i_UR) {
        initUR(i_UR, *m_db, m_fileID, keyIcon);
    }
    else if (st.i == i_TR) {
        initTR(i_TR, *m_db, m_fileID, keyIcon);
    }
    else if (st.i == i_KV) {
        initKV(i_KV, *m_db, m_fileID, keyIcon);
    }
    else if (st.i == i_TR) {
        initTR(i_TR, *m_db, m_fileID, keyIcon);
    }
    else {
    }
}

void NoVisual::customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *nd = tree.itemAt( pos );
    if (!nd) return;

    QVariant v = nd->data(0, Qt::UserRole);

    if (v.isNull()) return;

    ST2 st = v.value<ST2>();

    QMenu *menu = new QMenu(this);

    QAction *a;

    if (UserRight::isEditGid()) {
        if (!st.system && st.tn != "heatSources" && st.tn != "fragments") {
            a = new QAction(tr("Добавить"), this);
            st.typ = st2_typ_add;
            a->setData(QVariant::fromValue(st));
            connect(a, SIGNAL(triggered()), this, SLOT(onAdd()));
            menu->addAction(a);
        }
    }

    if (!st.header) {
        a = new QAction(tr("Редактировать"), this);
        st.typ = st2_typ_edit;
        a->setData(QVariant::fromValue(st));
        connect(a, SIGNAL(triggered()), this, SLOT(onEdit()));
        menu->addAction(a);

        if (UserRight::isEditGid()) {
            if (!st.system && st.tn != "heatSources" && st.tn != "fragments") {
                a = new QAction(tr("Удалить"), this);
                st.typ = st2_typ_delete;
                a->setData(QVariant::fromValue(st));
                connect(a, SIGNAL(triggered()), this, SLOT(onDelete()));
                menu->addAction(a);
            }
        }
        if (st.tn == "heatSources") {

            if (UserRight::isEditGid()) {
                a = new QAction(tr("Температурный график"), this);
                st.typ = st2_typ_tg;
                a->setData(QVariant::fromValue(st));
                connect(a, SIGNAL(triggered()), this, SLOT(onTg()));
                menu->addAction(a);
            }

            a = new QAction(tr("Перейти"), this);
            st.typ = st2_typ_delete;
            a->setData(QVariant::fromValue(st));
            connect(a, SIGNAL(triggered()), this, SLOT(onMoveTo()));
            menu->addAction(a);
        }
    }

    menu->popup(tree.viewport()->mapToGlobal(pos));
}

QTreeWidgetItem *initItem(QWidget *widget, QTreeWidget &tree, const QString & title, const QString & tn, const QString & fn, QIcon &groupIcon, bool add = true)
{
    QTreeWidgetItem *i = new QTreeWidgetItem(&tree);
    i->setText(0, title);
    i->setIcon(0, groupIcon);

    if (add) {
        ST2 st;
        st.typ = st2_typ_add;
        st.header = true;
        st.title = title;
        st.tn = tn;
        st.fn = fn;
        st.i = i;

        i->setData(0, Qt::UserRole, QVariant::fromValue(st));
    }

    return i;
}


NoVisual::NoVisual(QWidget *parent, const QString  & title, int fileID, CCxema * cxema, QSqlDatabase &db, CGraph2 *graph, GidWidget *_gid)
    : QDialog(parent)
    , save(this, "NoVisual", title)
{

    gid = _gid;

    m_cxema = cxema;
    m_db = &db;
    m_fileID = fileID;

    QPushButton *okButton = new QPushButton(tr("&Ok"));
    okButton->setDefault(true);
    okButton->setEnabled(true);

    QPushButton *closeButton = new QPushButton(tr("Отменить"));
//    QPushButton *helpButton = new QPushButton(tr(tr("Помощь")));

    buttonBox = new QVBoxLayout;

    QHBoxLayout *rowLayout = new QHBoxLayout;
    QVBoxLayout *leftLayout = new QVBoxLayout;

    buttonBox->addWidget(okButton);
    buttonBox->addWidget(closeButton);
    buttonBox->addStretch();
    //    buttonBox->addWidget(helpButton);


    groupIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),
                        QIcon::Normal, QIcon::Off);
    groupIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon),
                        QIcon::Normal, QIcon::On);
    keyIcon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));

    tree.setHeaderHidden(true);
    tree.setHeaderLabels({ tr("") });

    tree.setContextMenuPolicy(Qt::CustomContextMenu);

    connect(&tree, &QTreeWidget::customContextMenuRequested, this, &NoVisual::customContextMenuRequested);


    i_CT = initItem(this, tree, tr("Система теплоснабжения"), "heatSystem", "name", groupIcon, false);
    initCT(i_CT, db, -1, keyIcon);

    i_FR = initItem(this, tree, tr("Фрагмент тепловой сети"), "fragments", "name", groupIcon, false);
    initFR(i_FR, db, fileID, keyIcon);

    i_RS = initItem(this, tree, tr("Расчетные схемы"), "externalCodes", "name", groupIcon);
    initRS(i_RS, db, fileID, keyIcon);

    i_UR = initItem(this, tree, tr("Удельные расходы"), "specExpends", "specExpendID", groupIcon);
    initUR(i_UR, db, fileID, keyIcon);

    i_TR = initItem(this, tree, tr("Расчетные температуры"), "calcTemperatures", "calcTemperatureID", groupIcon);
    initTR(i_TR, db, fileID, keyIcon);

    i_KV = initItem(this, tree, tr("Коэффициенты вариации"), "varCoefficients", "kodkv", groupIcon);
    initKV(i_KV, db, fileID, keyIcon);

    i_IST1 = initItem(this, tree, tr("ТЭЦ"), "1", "", groupIcon, false);
    initIST(i_IST1, db, fileID, keyIcon, 1, graph);
    
    i_IST2 = initItem(this, tree, tr("Районные котельные"), "1", "", groupIcon, false);
    initIST(i_IST2, db, fileID, keyIcon, 2, graph);

    i_IST3 = initItem(this, tree, tr("Местные котельные"), "1", "", groupIcon, false);
    initIST(i_IST3, db, fileID, keyIcon, 3, graph);

    restore_tree(&tree, "NoVisual");

    leftLayout->addLayout(rowLayout);
    leftLayout->addWidget(&tree);

    QHBoxLayout *mainLayout = new QHBoxLayout;

    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(buttonBox);

    setLayout(mainLayout);

    setWindowTitle(title);

//    connect(helpButton, SIGNAL(clicked()), this, SLOT(helpClicked()));

//    connect(&tree, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(okClicked()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
}


void NoVisual::AddButton(const QString & txt, int id)
{
    QPushButton *button = new QPushButton(txt);
    buttonBox->addWidget(button);
    connect(button, SIGNAL(clicked()), this, SLOT(okClicked()));
}


void NoVisual::helpClicked()
{
  QMessageBox::warning(this, tr("!"), "!");
}

void NoVisual::okClicked()
{
}

void NoVisual::hideEvent(QHideEvent *event)
{    
    save_tree(&tree, "NoVisual");
    QDialog::hideEvent(event);
}


