#include <QtGui>
#include <QtWidgets>

#include "DockPTS.h"
#include <cxema/cxema1.h>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include <gidview/GidWidget.h>
#include <db/db.h>

struct ST1
{
//    ST1(int _typ, int _id) {id = _id; typ = _typ;};
    int typ;
    int id;
};

void save_tree(QTreeWidget *tree, const QString & name);
void restore_tree(QTreeWidget *tree, const QString & name);

DockPTS::DockPTS(QWidget *parent)
    : QWidget(parent)
{

    tree = new QTreeWidget;
    list1 = new QListWidget;
    list2 = new QListWidget;

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
    tree->setHeaderHidden(true);

//    tree->header()-> setHheight()

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,0,0,0);

    QSizePolicy sp1(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp1.setVerticalStretch(2);
    tree->setSizePolicy(sp1);

    mainLayout->addWidget(tree);

    QSizePolicy sp2(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp2.setVerticalStretch(1);
    list1->setSizePolicy(sp2);


    QSizePolicy sp3(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp3.setVerticalStretch(1);
    list2->setSizePolicy(sp3);


    mainLayout->addWidget(list1);
    mainLayout->addWidget(list2);
    setLayout(mainLayout);

    tree->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(tree, &QTreeWidget::itemClicked, this, &DockPTS::itemClicked);
    connect(tree, &QTreeWidget::customContextMenuRequested, this, &DockPTS::customContextMenuRequested);

    connect(list1, &QListWidget::itemSelectionChanged, this, &DockPTS::onSelectionChanged1);
    connect(list2, &QListWidget::itemSelectionChanged, this, &DockPTS::onSelectionChanged2);

//    customContextMenuRequested(const QPoint &pos)
}

#include <property/PropertyDial.h>


void DockPTS::onPropertyAccepted()
{
    auto *dlg = qobject_cast<PropertyDial *>(sender());
    if (dlg) {
        if (update_db(dlg->map_edited, dlg->getUser())) {
            repaint();
        }
    }
}


void DockPTS::onEdit()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        ST1 st = action->data().value<ST1>();
        if (st.typ == 1) { // MS
            auto *dlg = new PropertyDial(this, m_cxema->m_db, "Участок МС", "uchastok_ms", "uchastok_ms", st.id, -1, pr_type_any, m_cxema);
            dlg->show();
            connect(dlg, SIGNAL(accepted()), this, SLOT(onPropertyAccepted()));
        }
        if (st.typ == 2) { // RS
            auto *dlg = new PropertyDial(this, m_cxema->m_db, "Участок РС", "uchastok_rs", "uchastok_rs", st.id, -1, pr_type_any, m_cxema);
            dlg->show();
            connect(dlg, SIGNAL(accepted()), this, SLOT(onPropertyAccepted()));
        }
        if (st.typ == 3) { // Магистраль
            auto *dlg = new PropertyDial(this, m_cxema->m_db, "Магистраль", "magistrali", "magistrali", st.id, -1, pr_type_any, m_cxema);
            dlg->show();
            connect(dlg, SIGNAL(accepted()), this, SLOT(onPropertyAccepted()));
        }
    }
//    connect(dlg, SIGNAL(rejected()), this, SLOT(onPropertyRejected()));
}

void NeOpisanMessage(QSqlDatabase &db, int id_ms, int id_rs)
{
    QString q;
    if (id_ms == 0) id_ms = -1;
    if (id_rs == 0) id_rs = -1;
    q = QString(R"SQL(
        SELECT DISTINCT fr.name
        FROM heatPipeSections hps
        JOIN net.v_linesobj l ON l.id=hps.lineID AND l.removed=0
        JOIN net.v_nodes n1 ON n1.id=l.nodeID1
        JOIN fragments fr ON fr.id=n1.fileID
        WHERE (hps.magistralSite=%1 OR hps.distSite=%2)
        )SQL"
        ).arg(id_ms).arg(id_rs);



    QString s, name = readTableValue(db, q);
    if (name != "") {
        s = QString(R"TXT(Участок отобразить невозможно, так как фрагмент %1 не подключен. 
                Подключите фрагмент командой: "Файл>Отобразить дополнительную схему")TXT").arg(name);
    }
    else {
        s = QString("Участок не описан");
    }
    QMessageBox::about(0, "", s);
}


void DockPTS::onMoveTo()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        m_cxema->m_graph->reset();
        ST1 st = action->data().value<ST1>();
        if (st.typ == 1) { // MS
            m_cxema->m_graph->vydMS(st.id);
        }
        else if (st.typ == 2) { // RS
            m_cxema->m_graph->vydRS(st.id);
        }
        else if (st.typ == 3) { // Магистраль
            m_cxema->m_graph->vydMag(st.id);
        }
        if (!m_view->moveVyd()) {
            NeOpisanMessage(m_cxema->m_db, st.typ == 1 ? st.id : 0, st.typ == 2 ? st.id : 0);
        }
    }
}

void DockPTS::onEditGraph()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        ST1 st = action->data().value<ST1>();
        if (st.typ == 1) { // MS
            auto *dlg = new PropertyDial(this, m_cxema->m_db, "", "uchastok_ms", "uchastok_ms", st.id, -1, pr_type_any, m_cxema);
            dlg->show();
        }
        if (st.typ == 2) { // RS
            auto *dlg = new PropertyDial(this, m_cxema->m_db, "", "uchastok_rs", "uchastok_rs", st.id, -1, pr_type_any, m_cxema);
            dlg->show();
        }
    }
}

void DockPTS::onPassport()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        ST1 st = action->data().value<ST1>();
//        if (st.typ == 1) { // MS
            m_view->Passport(0, 0, st.typ, st.id);
//            auto *dlg = new PropertyDial(this, m_cxema->m_db, "", "uchastok_ms", st.id, -1, pr_type_any);
//            dlg->show();
//        }
//        if (st.typ == 2) { // RS
//            auto *dlg = new PropertyDial(this, m_cxema->m_db, "", "uchastok_rs", st.id, -1, pr_type_any);
//            dlg->show();
//        }
    }
}

void DockPTS::customContextMenuRequested(const QPoint &pos)
{

    QTreeWidgetItem *nd = tree->itemAt( pos );
    if (!nd) return;

    ST1 st = nd->data(0, Qt::UserRole).value<ST1>();

    QMenu *menu=new QMenu(this);

    QAction *aEdit = new QAction("Характеристика", this);
    aEdit->setData(QVariant::fromValue(st));
    connect(aEdit, SIGNAL(triggered()), this, SLOT(onEdit()));
    menu->addAction(aEdit);

    QAction *aMoveTo = new QAction("Перейти к участку", this);
    aMoveTo->setData(QVariant::fromValue(st));
    connect(aMoveTo, SIGNAL(triggered()), this, SLOT(onMoveTo()));
    menu->addAction(aMoveTo);

    QAction *aEditGraph = new QAction("Редактировать участок", this);
    aEditGraph->setData(QVariant::fromValue(st));
    connect(aEditGraph, SIGNAL(triggered()), this, SLOT(onEditGraph()));
    menu->addAction(aEditGraph);

    QAction *aPassport = new QAction("Паспорт", this);
    aPassport->setData(QVariant::fromValue(st));
    connect(aPassport, SIGNAL(triggered()), this, SLOT(onPassport()));
    menu->addAction(aPassport);

//        menu->addAction(new QAction("Использовать в адресном поиске", this));
    menu->popup(tree->viewport()->mapToGlobal(pos));

//    ST1 st = item->data(0, Qt::UserRole).value<ST1>();
//    QMessageBox::warning(0, "", nd->text(0));
}


QString transl(const QString & s);
#include <QtSql>
#include <any/MyMain.h>


bool DockPTS::init(GidWidget *view, QSqlDatabase &db, bool is_ms)
{
    if (view) {
        m_view = view;
    }
    m_cxema = m_view->getCxema();

    tree->clear();

    QString q;

    if (is_ms) {
        q =

"SELECT \n"
"nach.id AS nach_id,\n"
"nach.fio AS nach_name,\n"
"re.id AS re_id,\n"
"re.naimenovanie_rayona_ekspluatatsii_istochnika_tepla AS re_name,\n"
"ue.id AS ue_id,\n"
"ue.nomer_uchastka AS ue_name,\n"
"ms.id AS ms_id,\n"
"ms.opisanie_uchastka_ms AS ms_name\n"
//",*\n"
"FROM uchastok_ms ms\n"
"LEFT JOIN uchastki_ekspluatatsii ue ON ue.id=ms.nomer_uchastka\n"
"LEFT JOIN nachalniki_uchastkov nach ON nach.id=ue.nachalnik_uchastka\n"
"LEFT JOIN rayon_ekspluatatsii re ON re.id=ue.rayon_ekspluatatsii\n"
"ORDER BY nach.fio,nach.id,re.naimenovanie_rayona_ekspluatatsii_istochnika_tepla, re.id,ms.opisanie_uchastka_ms,ms.id    \n";
    }
    else {
        q =

"SELECT \n"
"nach.id AS nach_id,\n"
"nach.fio AS nach_name,\n"
"re.id AS re_id,\n"
"re.naimenovanie_rayona_ekspluatatsii_istochnika_tepla AS re_name,\n"
"ue.id AS ue_id,\n"
"ue.nomer_uchastka AS ue_name,\n"
"ms.id AS ms_id,\n"
"ms.naimenovanie_uchastka_rs AS ms_name\n"
//",*\n"
"FROM uchastok_rs ms\n"
"LEFT JOIN uchastki_ekspluatatsii ue ON ue.id=ms.nomer_uchastka\n"
"LEFT JOIN nachalniki_uchastkov nach ON nach.id=ue.nachalnik_uchastka\n"
"LEFT JOIN rayon_ekspluatatsii re ON re.id=ue.rayon_ekspluatatsii\n"
"ORDER BY nach.fio,nach.id,re.naimenovanie_rayona_ekspluatatsii_istochnika_tepla, re.id,ms.naimenovanie_uchastka_rs,ms.id    \n";

    }

    QSqlQuery query(db);
    query.setForwardOnly(true);

    if (query_exec(db, query, q)) {
        QTreeWidgetItem *i1  = nullptr;
        QTreeWidgetItem *i0 = new QTreeWidgetItem(tree);
        i0->setText(0, tr("Начальники участков"));

        int nach_id_old = -1;
        int ms_id_old = -1;

        while (query.next()) {
            int nach_id = query.value("nach_id").toInt();
            QString nach_name = query.value("nach_name").toString();
            int ms_id = query.value("ms_id").toInt();
            QString ms_name = query.value("ms_name").toString();

            if (nach_id != nach_id_old) {
                i1 = new QTreeWidgetItem(i0);
                i1->setText(0, nach_name);
                i1->setIcon(0, groupIcon);
                nach_id_old = nach_id;
                ms_id_old = -1;
            }
            if (ms_id != ms_id_old) {
                QTreeWidgetItem *i2 = new QTreeWidgetItem(i1);
                i2->setText(0, ms_name);
                i2->setIcon(0, keyIcon);
                ST1 st1;
                st1.typ  = is_ms ? 1 : 2;
                st1.id  = ms_id;
                i2->setData(0,Qt::UserRole,QVariant::fromValue(st1));

                ms_id_old = ms_id;
            }
        }
    }

    if (is_ms) {
        q =

"SELECT \n"

"re.id AS re_id,\n"
"re.naimenovanie_rayona_ekspluatatsii_istochnika_tepla AS re_name,\n"
"ue.id AS ue_id,\n"
"ue.nomer_uchastka AS ue_name,\n"
"ms.id AS ms_id,\n"
"ms.opisanie_uchastka_ms AS ms_name\n"
//",*\n"
"FROM uchastok_ms ms\n"
"LEFT JOIN uchastki_ekspluatatsii ue ON ue.id=ms.nomer_uchastka\n"
"LEFT JOIN rayon_ekspluatatsii re ON re.id=ue.rayon_ekspluatatsii\n"
"ORDER BY re.naimenovanie_rayona_ekspluatatsii_istochnika_tepla, re.id, ue.nomer_uchastka, ue.id, ms.opisanie_uchastka_ms,ms.id    \n";
//    QInputDialog::getMultiLineText(nullptr, "title", "label", q);
    }
    else {
        q =

"SELECT \n"

"re.id AS re_id,\n"
"re.naimenovanie_rayona_ekspluatatsii_istochnika_tepla AS re_name,\n"
"ue.id AS ue_id,\n"
"ue.nomer_uchastka AS ue_name,\n"
"ms.id AS ms_id,\n"
"ms.naimenovanie_uchastka_rs AS ms_name\n"
//",*\n"
"FROM uchastok_rs ms\n"
"LEFT JOIN uchastki_ekspluatatsii ue ON ue.id=ms.nomer_uchastka\n"
"LEFT JOIN rayon_ekspluatatsii re ON re.id=ue.rayon_ekspluatatsii\n"
        "ORDER BY re.naimenovanie_rayona_ekspluatatsii_istochnika_tepla, re.id, ue.nomer_uchastka, ue.id, ms.naimenovanie_uchastka_rs,ms.id    \n";
    }

    if (query_exec(db, query, q)) {
        QTreeWidgetItem *i1  = nullptr;
        QTreeWidgetItem *i2  = nullptr;
        QTreeWidgetItem *i0 = new QTreeWidgetItem(tree);
        i0->setText(0, tr("Районы эксплуатации"));

        int re_id_old = -1;
        int ue_id_old = -1;
        int ms_id_old = -1;

        while (query.next()) {
            int ms_id = query.value("ms_id").toInt();
            QString ms_name = query.value("ms_name").toString();
            int ue_id = query.value("ue_id").toInt();
            QString ue_name = query.value("ue_name").toString();
            int re_id = query.value("re_id").toInt();
            QString re_name = query.value("re_name").toString();

            if (re_id != re_id_old) {
                i1 = new QTreeWidgetItem(i0);
                i1->setText(0, re_name);
                i1->setIcon(0, groupIcon);
                re_id_old = re_id;
                ms_id_old = -1;
                ue_id_old = -1;
            }
            if (ue_id != ue_id_old) {
                i2 = new QTreeWidgetItem(i1);
                i2->setText(0, ue_name);
                i2->setIcon(0, groupIcon);
                ue_id_old = ue_id;
                ms_id_old = -1;
            }
            if (ms_id != ms_id_old) {
                QTreeWidgetItem *i3 = new QTreeWidgetItem(i2);
                i3->setText(0, ms_name);
                i3->setIcon(0, keyIcon);
                ST1 st1;
                st1.typ  = is_ms ? 1 : 2;
                st1.id  = ms_id;
                i3->setData(0,Qt::UserRole, QVariant::fromValue(st1));

                ms_id_old = ms_id;
            }
        }
    }

    if (is_ms) {
        q = 
"SELECT\n"
"mag.id AS mag_id,\n"
"mag.naimenovanie_magistrali AS mag_name,\n"
"ms.id AS ms_id,\n"
"ms.opisanie_uchastka_ms AS ms_name\n"
"FROM uchastok_ms ms\n"
"LEFT JOIN magistrali mag ON mag.id=ms.magistral\n"
"ORDER BY\n"
"mag.naimenovanie_magistrali, mag.id, ms.opisanie_uchastka_ms, ms.id\n";

        if (query_exec(db, query, q)) {
            QTreeWidgetItem *i1  = nullptr;
            QTreeWidgetItem *i0 = new QTreeWidgetItem(tree);
            i0->setText(0, tr("Магистрали"));

            int mag_id_old = -1;
            int ms_id_old = -1;

            while (query.next()) {
                int mag_id = query.value("mag_id").toInt();
                QString mag_name = query.value("mag_name").toString();
                int ms_id = query.value("ms_id").toInt();
                QString ms_name = query.value("ms_name").toString();

                if (mag_id != mag_id_old) {
                    i1 = new QTreeWidgetItem(i0);
                    i1->setText(0, mag_name);
                    i1->setIcon(0, groupIcon);
                    mag_id_old = mag_id;
                    ms_id_old = -1;

                    ST1 st1;
                    st1.typ = 3;
                    st1.id  = mag_id;
                    i1->setData(0,Qt::UserRole, QVariant::fromValue(st1));

                }
                if (ms_id != ms_id_old) {
                    QTreeWidgetItem *i2 = new QTreeWidgetItem(i1);
                    i2->setText(0, ms_name);
                    i2->setIcon(0, keyIcon);
                    ST1 st1;
                    st1.typ  = 1;
                    st1.id  = ms_id;
                    i2->setData(0,Qt::UserRole, QVariant::fromValue(st1));

                    ms_id_old = ms_id;
                }
            }
        }
    }

    m_ms_rs = is_ms ? 1 : 2;

    restore_tree(tree, QString("Tree/DockPTS/%1").arg(m_ms_rs));

    return true;
}

void copyVydGraph_new(CGraph2* graph1, CGraph2* graph2, bool is_po = false);
void sort_line_rs_new(CGraph2* graph, list<CNode2*>& list_sort, list<CLINE2*>& st_l, int fileID = -1);
bool isPSstrict_(CNode2* node);


void DockPTS::itemClicked(QTreeWidgetItem *item, int column)
{
    QTreeWidgetItem *parent = item->parent();
    if (!parent) return;

    ST1 st = item->data(0, Qt::UserRole).value<ST1>();

//    QMessageBox::warning(0, "", QString("%1").arg(st.id));

    CGraph2 *graph = m_cxema->graph();
    CGraph2 graph2(NULL);

    list<CNode2*> list_sort;
    list<CLINE2*> st_l;
    list <list<int> > list_ut;
    list<int> list_ut_min;


    graph->save_pjezo();

    m_cxema->m_graph->reset();

    if (st.typ == 1) { // MS
        m_cxema->m_graph->vydMS(st.id);
    }
    if (st.typ == 2) { // RS
        m_cxema->m_graph->vydRS(st.id);
    }

    copyVydGraph_new(graph, &graph2, true);
    sort_line_rs_new(&graph2, list_sort, st_l);

    graph->restore_pjezo();


    CNode2* n1_old = nullptr, * n2_old = nullptr;
    CNode2* n_old = nullptr;

    graph->init_find_line_nom();

    list1->clear();
    list2->clear();
    bool first = true;

    for (auto p : st_l) {
        CLINE2* l = p;

        CLINE2* lP = graph->find_line_nom(bline(l)->line.nomP);
        CLINE2* lO = graph->find_line_nom(bline(l)->line.nomO);

        if (lP || lO) {
            CNode2* n1 = where(l);
            CNode2* n2 = other(l);

            CLINE2* l = lP ? lP : lO;

            if (first) {
                if (n1) {
                    QString name1 = n1->getName();
                    QListWidgetItem *item = new QListWidgetItem(name1);
                    item->setData(Qt::UserRole, (int)n1->id);
                    list1->addItem(item);
                }
                first = false;
            }


            if (n1_old && (isPSstrict_(n1) || n1 != n2_old)) {
                for (int & it : list_ut_min) {
//                    s = QString("(%1, %2)").arg(nn++).arg(it);
//                    if (ss != "") ss += ",";
//                    ss += s;

//                    if (ins.ado_insert_int2(nn++, it)) ret = true;

                }

                list_ut_min.clear();
            }

            list_ut_min.push_back(lP ? bline(lP)->line.nomP : bline(lO)->line.nomO);

            if (n1 && n2) {
                QString name1 = n1->getName();
                QString name2 = n2->getName();

                QListWidgetItem *item = new QListWidgetItem(name2);
                item->setData(Qt::UserRole, (int)n2->id);

                
                list1->addItem(item);

                QListWidgetItem *item2 = new QListWidgetItem(QString("%1 - %2").arg(name1, name2));

                item2->setData(Qt::UserRole, bline(l)->line.nomP);

                list2->addItem(item2);
            }

            n1_old = n1;
            n2_old = n2;
        }
    }
}

void DockPTS::onSelectionChanged1()
{
    QList<QListWidgetItem *> selectedItems = list1->selectedItems();
    if (selectedItems.isEmpty()) return;

    int id = selectedItems.first()->data(Qt::UserRole).toInt();

    m_view->moveNode(id);
}

void DockPTS::onSelectionChanged2()
{
    QList<QListWidgetItem *> selectedItems = list2->selectedItems();
    if (selectedItems.isEmpty()) return;

    int id = selectedItems.first()->data(Qt::UserRole).toInt();
    m_view->moveLine(id);

}



void DockPTS::hideEvent(QHideEvent *event)
{    
    save_tree(tree, QString("Tree/DockPTS/%1").arg(m_ms_rs));
    QWidget::hideEvent(event);
}

#if 0

// Функция для поиска элемента в QTreeWidget по данным
QTreeWidgetItem* findItemByData(QTreeWidget* treeWidget, int column, const QString& value) 
{
    // Рекурсивная функция для поиска в элементе и его потомках
    std::function<QTreeWidgetItem*(QTreeWidgetItem*)> searchItem = [&](QTreeWidgetItem* item) -> QTreeWidgetItem* {
        if (item->text(column) == value) { // Проверяем данные
            return item;
        }
        for (int i = 0; i < item->childCount(); ++i) {
            QTreeWidgetItem* result = searchItem(item->child(i)); // Рекурсивный вызов
            if (result) {
                return result;
            }
        }
        return nullptr;
    };

    // Перебираем все верхние элементы
    for (int i = 0; i < treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* result = searchItem(treeWidget->topLevelItem(i));
        if (result) {
            return result;
        }
    }
    return nullptr; // Элемент не найден
}

#endif


// Функция для поиска элемента по данным
//QTreeWidgetItem* findItemByData(QTreeWidget* treeWidget, int column, const QVariant& value) 
QTreeWidgetItem* findItemByData(QTreeWidget* treeWidget, int column, int typ, int id) 
{
    // Рекурсивная функция для поиска данных
    std::function<QTreeWidgetItem*(QTreeWidgetItem*)> searchItem = [&](QTreeWidgetItem* item) -> QTreeWidgetItem* {

        ST1 st = item->data(column, Qt::UserRole).value<ST1>();

        if (st.typ == typ && st.id == id) { // MS
            return item;
        }

//        if (item->data(column, Qt::UserRole) == value) { // Проверяем данные
//            return item;
//        }
        for (int i = 0; i < item->childCount(); ++i) {
            QTreeWidgetItem* result = searchItem(item->child(i)); // Рекурсивный вызов
            if (result) {
                return result;
            }
        }
        return nullptr;
    };

    // Перебираем все верхние элементы
    for (int i = 0; i < treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* result = searchItem(treeWidget->topLevelItem(i));
        if (result) {
            return result;
        }
    }
    return nullptr; // Элемент не найден
}

void DockPTS::vydMag(int id)
{
    QTreeWidgetItem* it = findItemByData(tree, 0, 3, id);
    if (it) {
        tree->setCurrentItem(it);
        it->setSelected(true);
        QDockWidget *p1 = qobject_cast<QDockWidget *>(this->parentWidget());
        if (p1)  {
            p1->raise();
        }
    }
}

void DockPTS::vydMS(int id)
{
    QTreeWidgetItem* it = findItemByData(tree, 0, 1, id);
    if (it) {
        tree->setCurrentItem(it);
        it->setSelected(true);
        QDockWidget *p1 = qobject_cast<QDockWidget *>(this->parentWidget());
        if (p1)  {
            p1->raise();
        }
    }
}

void DockPTS::vydRS(int id)
{
    QTreeWidgetItem* it = findItemByData(tree, 0, 2, id);
    if (it) {
        tree->setCurrentItem(it);
        it->setSelected(true);

        QDockWidget *p1 = qobject_cast<QDockWidget *>(this->parentWidget());
        if (p1)  {
            p1->raise();
        }
    }
}
