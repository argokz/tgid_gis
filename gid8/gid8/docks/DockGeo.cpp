#include <QtGui>
#include <QtWidgets>
#include <QPoint>

#include <any/MyMain.h>
#include <table/MultiHeaderTable.h>

#include <geo/geofile.h>
#include <geo/Klassif.h>
#include "mainwindow.h"

#include "mystd.h"
#include "property/PropertyDial.h"
#include "DockGeo.h"
#include <cxema/cxema1.h>
#include <table/DbWindow.h>

void save_tree(QTreeWidget *tree, const QString & name)
{
    QSettings settings;
    QTreeWidgetItemIterator it(tree);
    int id = 0;

    while (*it) {
        if ((*it)->childCount() > 0) {
            settings.setValue(QString("%1/%2").arg(name).arg(id), (*it)->isExpanded());
            id++;
        }
        it++;
    }
}

void restore_tree(QTreeWidget *tree, const QString & name)
{
    QSettings settings;
    QTreeWidgetItemIterator it(tree);
    int id = 0;
    
    while (*it) {
        if ((*it)->childCount() > 0) {
            (*it)->setExpanded(settings.value(QString("%1/%2").arg(name).arg(id), true).toBool());
            id++;
        }
        it++;
    }
}

DockGeo::DockGeo(QWidget *parent)
    : QWidget(parent)
{

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

    tree = new QTreeWidget;
    tree->setHeaderHidden(true);

    tree->setHeaderLabels({ tr("") });
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addWidget(tree);
    setLayout(mainLayout);

    tree->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(tree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
    connect(tree, &QTreeWidget::itemClicked, this, &DockGeo::itemClicked);
}

QString transl(const QString & s);
#include <QtSql>
#include <any/MyMain.h>

MultiHeaderTable* getMyTableView(QSqlDatabase &db, const QString & tn, const QString & q, const QString & title);

QString findTableRusName(const QString & d, const QString & n);

void DockGeo::reset()
{
    QSettings settings;
    QTreeWidgetItemIterator it(tree);
    
    while (*it) {
        if ((*it)->childCount() == 0) {
            QString name = (*it)->data(0, Qt::UserRole).toString();
            Klassif *kls = m_kl_list.findKlN(name);
            if (kls) {
                (*it)->setCheckState(0, kls->ins ? Qt::Checked : Qt::Unchecked);
            }
        }
        it++;
    }
}

// Какие слои показывать в проводнике геобазы.
//
// В дерево досыпались ВСЕ таблицы классификатора, поэтому там оказались
// строки, которые на карте включить нельзя: у них попросту нет
// геометрии, и галочка ничего не меняла. Плюс представления *_wgs84 —
// это те же объекты в другой системе координат, они существуют для
// внешних ГИС и в программе не нужны.
//
// Признак «есть что рисовать» берётся из самой БД: наличие колонки типа
// geometry. Список запрашивается один раз за сеанс.
static const QSet<QString> &geo_tables_with_geometry(QSqlDatabase &db)
{
    static QSet<QString> tables;
    static bool loaded = false;

    if (!loaded) {
        loaded = true;
        QSqlQuery query(db);
        query.setForwardOnly(true);
        if (query.exec("SELECT lower(table_schema) || '.' || lower(table_name) "
                       "FROM information_schema.columns "
                       "WHERE udt_name = 'geometry'")) {
            while (query.next()) {
                const QString full = query.value(0).toString();
                tables.insert(full);
                // Имя без схемы: в классификаторе слой может быть записан
                // и так, и так.
                tables.insert(full.section('.', 1));
            }
        }
        qInfo() << "геобаза: таблиц с геометрией" << tables.size();
    }
    return tables;
}

static bool geo_layer_visible(QSqlDatabase &db, const QString &name)
{
    const QString n = name.toLower();

    if (n.endsWith("_wgs84")) return false;

    const QSet<QString> &geo = geo_tables_with_geometry(db);
    if (geo.isEmpty()) return true;      // не смогли спросить — не прячем

    return geo.contains(n) || geo.contains(n.section('.', 1));
}

// Русские названия групп. Группа в дереве — это имя схемы БД, и до сих
// пор она показывалась как есть: net, ref, addr. Понять по такой
// подписи, что внутри, невозможно.
static QString geo_group_rus(const QString &schema)
{
    static const QMap<QString, QString> names = {
        {"net",   QStringLiteral("Объекты тепловой сети")},
        {"ref",   QStringLiteral("Справочники")},
        {"addr",  QStringLiteral("Адресный план")},
        {"ops",   QStringLiteral("Эксплуатация: ремонты, дефекты, шурфы")},
        {"org",   QStringLiteral("Организации и ответственные")},
        {"doc",   QStringLiteral("Документы и паспорта")},
        {"el",    QStringLiteral("Электроснабжение")},
        {"calc",  QStringLiteral("Результаты расчётов")},
        {"meta",  QStringLiteral("Служебное: каталог слоёв и журналы")},
        {"attic", QStringLiteral("Архив (изъятое при переносе)")},
    };
    return names.value(schema.toLower(), schema);
}

bool DockGeo::init(GidWidget *view, const QString & baza)
{
    m_view = view;

    QString baza_label = baza;

    if (baza == "gid") {
        baza_label = "Тепловая сеть";
    }
    else {
//    if (baza == "step") {
        baza_label = "Карта местности";
    }

    QTreeWidgetItem *i00 = new QTreeWidgetItem(tree);
    i00->setText(0, baza_label);
    i00->setCheckState(0, Qt::Checked);
    

    QString fn = QString("%1kls/%2.lst").arg(argpath(), baza);

    if (baza.toLower() == "gid") {
        fn = QString("%1kls/gid2.lst").arg(argpath());
    }

    QFile file(fn);

    QSet<QString> set_s;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        QTextStream in(&file);
//        in.setEncoding(QStringConverter::System);
        QString par;
        QTreeWidgetItem *i0 = nullptr;

        bool end_of_list = false;

        while (!file.atEnd()) {

//            QString s = in.readLine();
            QString s = readCP1251Line(file);

            if (s.left(1) == "-") continue;
            if (s.left(1) == "=") {
                end_of_list = true;
                continue; 
            }
            if (s.length() == 0) continue;

            if (s.left(1) != " ") {
//                par = in.readLine();



                i0 = new QTreeWidgetItem(i00);
                i0->setText(0, s);
//                i0->setIcon(0, groupIcon);
                i0->setCheckState(0, Qt::Checked);
                continue;
            }
            else {
                s = s.trimmed();
  
                Klassif *kls = m_kl_list.findKlN(s);

                if (kls) {
                    set_s.insert(s.toLower());
                    QString rus_name = findTableRusName(baza, s);

                    if (!end_of_list) {
                        QTreeWidgetItem *i1 = new QTreeWidgetItem(i0);
                        i1->setText(0, rus_name);
                        i1->setCheckState(0, kls->ins? Qt::Checked : Qt::Unchecked);
                        if (!kls->ins) {
                            i0->setCheckState(0, Qt::Unchecked);
                        }

                        i1->setData(0,Qt::UserRole,s);
//                        i1->setData(0, Qt::UserRole, QVariant::fromValue(*kls));
                    }
                    else {
                        kls->ins = false;
                    }
                }
            }
        }
    }

    QString schema_old = "";

    QTreeWidgetItem *i0 = nullptr;

    for (auto & kl : m_kl_list) {
        if (kl.baza == baza && !set_s.contains(kl.nazv.toLower())) {
            // Слои без геометрии и представления *_wgs84 в дерево
            // не попадают: включить их на карте всё равно нельзя.
            if (!geo_layer_visible(m_view->getCxema()->m_db, kl.nazv)) continue;

            QString schema = "";
            QString table = kl.nazv;
            
            static QRegularExpression re("^(.+)\\.(.+)$");
            QRegularExpressionMatch match = re.match(kl.nazv);
            if (match.hasMatch()) {
                schema = match.captured(1);
                table = match.captured(2);
            }

            if (schema != "" && schema != schema_old) {
                i0 = new QTreeWidgetItem(i00);
                QString rus_name = geo_group_rus(schema);
                i0->setText(0, rus_name);
                i0->setCheckState(0, Qt::Checked);
            }
            if (schema == "") {
                i0 = i00;
            }
            schema_old = schema;

//            QString rus_name = findTableRusName(baza, kl.nazv);
            QString rus_name = findTableRusName(baza, kl.nazv);
            
            QTreeWidgetItem *i1 = new QTreeWidgetItem(i0);
            i1->setText(0, rus_name);
            i1->setCheckState(0, kl.ins? Qt::Checked : Qt::Unchecked);

            if (!kl.ins) {
                i0->setCheckState(0, Qt::Unchecked);
            }


            i1->setData(0,Qt::UserRole,kl.nazv);

//                i1->setData(0, Qt::UserRole, QVariant::fromValue(kl));
         }
    }
   
    restore_tree(tree, "Tree/DockGeo");
    return true;
}

void reset_geo(QSqlDatabase &db, QWidget *w)
{
    bool readMySql_Shape(QSqlDatabase &db, QProgressDialog *percent);

    QProgressDialog progress("Операция выполняется, подождите пожалуйста...", "Прервать", 0, 100, w);
    progress.setWindowModality(Qt::WindowModal);
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    progress.move(screenGeometry.center() - progress.rect().center());

    progress.show();
    progress.setValue(0);
    QApplication::processEvents();

    readMySql_Shape(db, &progress);
}



bool DockGeo::setCheck(QTreeWidgetItem *item, bool yes)
{
    bool open_new = false;

    int cnt = item->childCount();

    for (int i = 0; i < cnt; i++) {
        QTreeWidgetItem *it = item->child(i);

        bool op_new = setCheck(it, yes);

        if (op_new) open_new = true;

        if (yes) {
            if (it->checkState(0) != Qt::Checked) {
                it->setCheckState(0, Qt::Checked);
            }
        }
        else {
            if (it->checkState(0) != Qt::Unchecked) {
                it->setCheckState(0, Qt::Unchecked);
            }
        }
    }

    if (!cnt) {
        QString name = item->data(0, Qt::UserRole).toString();
        Klassif *kls = m_kl_list.findKlN(name);

        if (kls) {
            m_kls = kls;
            kls->ins = yes;

            if (yes && !kls->open) {
                open_new = true;
//                reset_geo(*kls->m_db, this);
            }

//            QString klfn = QString("%1%2.kls2").arg(argpath_2(), kls->baza);
//            m_kl_list.save2(klfn, kls->baza);
//            m_view->repaint();
        }
    }
    return open_new;
}

void DockGeo::itemClicked(QTreeWidgetItem *item, int column)
{
    bool yes = item->checkState(0) == Qt::Checked;
    if (setCheck(item, yes)) {
        reset_geo(*m_kls->m_db, this);
    }

    QString klfn = QString("%1/%2.kls2").arg(argpath_2(), m_kls->baza);
    m_kl_list.save2(klfn, m_kls->baza);

    m_view->repaint();
}

Klassif *DockGeo::getKls()
{
    QTreeWidgetItem *item = tree->currentItem();
    if (!item) return nullptr;

    QString name = item->data(0, Qt::UserRole).toString();

    Klassif *kls = m_kl_list.findKlN(name);
    return kls;
}

void DockGeo::contextMenu(QPoint point)
{
    Klassif *kls = getKls();
    if (kls) {
        QMenu *menu=new QMenu(this);

        if (kls->ins) {
            QAction *aTable = new QAction("Таблица", this);
            connect(aTable, SIGNAL(triggered()), this, SLOT(onTable()));
            menu->addAction(aTable);

            QAction *aKls = new QAction("Классификатор", this);
            connect(aKls, SIGNAL(triggered()), this, SLOT(onKalssif()));
            menu->addAction(aKls);

            QAction *aAddr = new QAction("Использовать в адресном поиске", this);
            connect(aAddr, SIGNAL(triggered()), this, SLOT(onAddr()));
            menu->addAction(aAddr);

            menu->popup(tree->viewport()->mapToGlobal(point));

            QAction *aAdd = new QAction("Добавить объект", this);
            connect(aAdd, SIGNAL(triggered()), this, SLOT(onAdd()));
            menu->addAction(aAdd);
        }
    }
}

#include <db/db.h>




unsigned long rgb_bgr(unsigned long col);
DbWindow *getTableView(QSqlDatabase &db, const QString & tn, const QString & q);

QString findTableRusName(const QString & d, const QString & n);
QString findColumnRusName(const QString & d, const QString & table, const QString & column);

void view_my_table(MultiHeaderTable *view, const QString & title, QWidget *parent);

void DockGeo::onTable()
{
    Klassif *kls = getKls();
    if (kls) {
        auto w = main_window->findWidgetTab(kls->RusName(), Qt::BottomDockWidgetArea);
        if (w) {
            w->raise();
            return;
        }

//        std::map<QString, AdoField> map_fields;
        std::list<AdoField> fields;

//        get_table_columns(*kls->m_db, kls->nazv, map_fields);
        get_table_columns(*kls->m_db, kls->nazv, fields);

        QString s = "";

        for (const auto& field : fields) {
            if (field.COLUMN_NAME == kls->shape) continue;
            if (field.DATA_TYPE == "geometry") continue;
            if (s != "") s += ",";
            s += field.COLUMN_NAME;
            QString rus = findColumnRusName(kls->baza, kls->nazv, field.COLUMN_NAME);
            s += QString(" AS \"%1\"").arg(rus);
        }

        QString q = QString("SELECT %1 FROM %2").arg(s, kls->nazv);

        QString getGeoSelect(Klassif *kls, CGraph2 *graph);
        q = getGeoSelect(kls, m_view->getCxema()->m_graph);

//        DbWindow *table = getTableView(*kls->m_db, kls->nazv, q, kls->RusName());
        MultiHeaderTable* table = getMyTableView(*kls->m_db, kls->nazv, q, kls->RusName());


        if (table) {
            table->setGidWidget(this->m_view);
            table->setGeo(kls->nazv);
            table->setEdit(true);
//            main_window->addWidgetTab(table, kls->RusName(), Qt::BottomDockWidgetArea);

            view_my_table(table, kls->RusName(), this);
//view_my_table
        }
        else {
            QMessageBox::warning(nullptr, "", QString("Нет данных в таблице %1").arg(kls->RusName()));
        }
    }
}


//QString sql_for_list_of_columns(QSqlDatabase &db, const QString &tn)
QString sql_for_list_of_columns(Klassif * kls)
{
    std::list<AdoField> fields;

    get_table_columns(*kls->m_db, kls->nazv, fields);

    QString p = "";

    int id = 1;

    for (auto & field : fields) {
        if (p != "") p += ",";

        QString fn = findColumnRusName(getDatabaseName(*kls->m_db), kls->nazv, field.COLUMN_NAME);

//        p += QString("(%1, '%2', '%3')").arg(id).arg(fn, field.COLUMN_NAME);
        p += QString("('%1')").arg(fn);
        id += 1;
    }

//    QString q = QString("SELECT * FROM (VALUES %1) t(id, %2, %3)").arg(p, quot_text(QObject::tr("Поле 1")), quot_text(QObject::tr("Поле 2")));
    QString q = QString("SELECT * FROM (VALUES %1) t(%2)").arg(p, quot_text(QObject::tr("Поле")));
    return q;

}

QString findColumnName(const QString & d, const QString & table, const QString & rus_name);

void DockGeo::onAddrAccepted()
{
    Klassif *kls = getKls();
    if (!kls) return;
    
    auto *dlg = qobject_cast<PropertyDial *>(sender());
    if (dlg) {
        m_raj = findColumnName(getDatabaseName(*kls->m_db), kls->nazv, dlg->getValue("N_raj").toString());
        m_str = findColumnName(getDatabaseName(*kls->m_db), kls->nazv, dlg->getValue("N_str").toString());
        m_dom = findColumnName(getDatabaseName(*kls->m_db), kls->nazv, dlg->getValue("N_dom").toString());

        if (!column_in_table(*kls->m_db, kls->nazv, m_str)) m_str = "";
        if (!column_in_table(*kls->m_db, kls->nazv, m_dom)) m_dom = "";
        if (!column_in_table(*kls->m_db, kls->nazv, m_raj)) m_raj = "";

        main_window->m_addr->init2(kls->nazv, m_raj, m_str, m_dom);
    }
}

void DockGeo::onAddr()
{
    Klassif *kls = getKls();
    if (!kls) return;

    auto *dlg = new PropertyDial(this, "address", "", "shape", "id", -1, -1);

    std::map<QString, QVariant> map_val;

    QString m_nazv;

    main_window->m_addr->init2_out(m_nazv, m_raj, m_str, m_dom);

    map_val["N_raj"] = findColumnRusName(getDatabaseName(*kls->m_db), kls->nazv, m_raj);
    map_val["N_str"] = findColumnRusName(getDatabaseName(*kls->m_db), kls->nazv, m_str);
    map_val["N_dom"] = findColumnRusName(getDatabaseName(*kls->m_db), kls->nazv, m_dom);

    dlg->init_dialog(kls->m_db, "Выберите поля для адресного поиска", "address", map_val);

    QString q = sql_for_list_of_columns(kls);
//    QString q = sql_for_list_of_columns(*kls->m_db, kls->nazv);

    dlg->addParam("q", q);
    dlg->addParam("db", kls->m_db->connectionName());
    dlg->show();

    connect(dlg, SIGNAL(accepted()), this, SLOT(onAddrAccepted()));
}


void DockGeo::onAdd()
{
    Klassif *kls = getKls();
    if (kls) {
        std::map<QString, QVariant> map_value;
        m_view->addGeo(kls, &map_value);
    }
}


bool KlEdit(QWidget *widget, Klassif *kls);


void DockGeo::onKalssif()
{

    Klassif *kls = getKls();
    if (kls) {
        Klassif kls_old = *kls;
        
        if (KlEdit(this, kls)) {
            if (
                (kls->label && (kls->codlabel != kls_old.codlabel || !kls_old.label))
                || 
                (kls->label_color && (kls->codlabel_color != kls_old.codlabel_color || !kls_old.label_color))
                || 
                (kls->label_color2 && (kls->codlabel_color2 != kls_old.codlabel_color2 || !kls_old.label_color2))
                || 
                (kls->label_color3 && (kls->codlabel_color3 != kls_old.codlabel_color3 || !kls_old.label_color3))
                ) 
            {
                kls->open = false;
                reset_geo(*kls->m_db, this);
            }

            QString klfn = QString("%1/%2.kls2").arg(argpath_2(), kls->baza);
            m_kl_list.save2(klfn, kls->baza);
            m_view->repaint();
        }
    }
}

void DockGeo::hideEvent(QHideEvent *event)
{    
    save_tree(tree, "Tree/DockGeo");
    QWidget::hideEvent(event);
}
