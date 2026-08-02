#include <QApplication>
#include <QtGui>
#include <QtWidgets>
#include <QInputDialog>

#include <QtSql>
#include <any/MyMain.h>

#include "db/db.h"
#include "geo/Klassif.h"
#include "gidview/GidWidget.h"
#include "mystd.h"
#include "DockAddr.h"
#include <cxema/cxema1.h>
#include <table/DbWindow.h>
#include <db/db.h>

QString readCP1251Line(QFile &file);

DockAddr::DockAddr(QWidget *parent)
    : QWidget(parent)
{
    fnd = new QLineEdit;

    fnd->setPlaceholderText(tr("Введите название улицы..."));

    list1 = new QListWidget;
    list2 = new QListWidget;
    list3 = new QListWidget;

    list1->setSelectionMode(QAbstractItemView::SingleSelection);
    list2->setSelectionMode(QAbstractItemView::SingleSelection);
    list3->setSelectionMode(QAbstractItemView::SingleSelection);

    QVBoxLayout *topLayout = new QVBoxLayout();

    topLayout->setSpacing(0);
    topLayout->setContentsMargins(0,0,0,0);
    
//    topLayout->addWidget(list1);
    topLayout->addWidget(fnd);

 // Создаем контейнер для верхнего layout
    QWidget *topContainer = new QWidget();
    topContainer->setLayout(topLayout);


// Создаём нижний QSplitter для двух нижних QListWidget
    bottomSplitter = new QSplitter(Qt::Horizontal);
    bottomSplitter->addWidget(list2);
    bottomSplitter->addWidget(list3);

    bottomSplitter->setStretchFactor(0, 3); // Верхний виджет занимает больше места
    bottomSplitter->setStretchFactor(1, 1); // Нижний QSplitter растягивается
    bottomSplitter->setHandleWidth(2);  // Устанавливаем узкую ширину полоски

    // Создаём главный QSplitter для верхнего и нижнего виджетов
    mainSplitter = new QSplitter(Qt::Vertical);
//    mainSplitter->addWidget(list1);
    mainSplitter->addWidget(topContainer);
    mainSplitter->addWidget(bottomSplitter);

    // Задаём начальные размеры
    mainSplitter->setStretchFactor(0, 1); //
    mainSplitter->setStretchFactor(1, 5); //
    mainSplitter->setHandleWidth(2);  // Устанавливаем узкую ширину полоски

    // Добавляем главный QSplitter в компоновку
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(mainSplitter);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);

    restoreSettings();

    connect(list1, &QListWidget::itemSelectionChanged, this, &DockAddr::onSelectionChanged1);
    connect(list2, &QListWidget::itemSelectionChanged, this, &DockAddr::onSelectionChanged2);
  //  connect(list3, &QListWidget::itemSelectionChanged, this, &DockAddr::onSelectionChanged3);
    connect(list3, &QListWidget::itemClicked, this, &DockAddr::onItemClicked);

    connect(fnd, &QLineEdit::textChanged, this, &DockAddr::onTextChanged);

}



void DockAddr::hideEvent(QHideEvent *event)
{    
    saveSettings();
    QWidget::hideEvent(event);
}



void DockAddr::saveSettings() 
{
    QSettings settings;
    settings.beginGroup("DockAddr");

    // Сохраняем состояние сплиттеров
    settings.setValue("mainSplitter", mainSplitter->saveState());
    settings.setValue("bottomSplitter", bottomSplitter->saveState());

    // Сохраняем размер и положение окна
    settings.setValue("geometry", saveGeometry());


    settings.setValue("tab", m_nazv);
    settings.setValue("raj", m_raj);
    settings.setValue("str", m_str);
    settings.setValue("dom", m_dom);

    settings.endGroup();
}

void DockAddr::restoreSettings() 
{
    QSettings settings;
    settings.beginGroup("DockAddr");

    // Восстанавливаем состояние сплиттеров
    mainSplitter->restoreState(settings.value("mainSplitter").toByteArray());
    bottomSplitter->restoreState(settings.value("bottomSplitter").toByteArray());

    // Восстанавливаем размер и положение окна
    restoreGeometry(settings.value("geometry").toByteArray());

    m_nazv = settings.value("tab", m_nazv).toString();
    m_raj  = settings.value("raj", m_raj ).toString();
    m_str  = settings.value("str", m_str ).toString();
    m_dom  = settings.value("dom", m_dom ).toString();

    settings.endGroup();
}

void DockAddr::init(GidWidget *view, const QString &baza) 
{
    m_view = view;

/*
    m_nazv = "zdaniya";
    m_raj = "NAME_MASSI";
    m_str = "NAME_STREE";
    m_dom = "NUMBER_1";
*/
    init_find("");
}

void DockAddr::init_find(const QString &txt)
{

    Klassif *kls = m_kl_list.findKlN(m_nazv);
    if (!kls) return;

    if (!column_in_table(*kls->m_db, kls->nazv, m_str)) return;
    if (!column_in_table(*kls->m_db, kls->nazv, m_dom)) return;
    if (!column_in_table(*kls->m_db, kls->nazv, m_raj)) {
        m_raj = "";
    }


    QString q, where = "";

    if (txt != "") {
        if (is_POSTGRESQL()) {
            where = QString("WHERE %1 ILIKE '%%%2%%'").arg(m_str,  txt);
        }
        else {
            where = QString("WHERE %1 LIKE '%%%2%%'").arg(m_str,  txt);
        }
    }



    q = QString("select distinct %1, %2 from %3 %4 ORDER BY %1,%2").arg(m_raj, m_str, m_nazv, where);


    if (m_raj == "") {
        q = QString("select distinct '%1', %2 from %3 %4 ORDER BY %2").arg("город", m_str, m_nazv, where);
    }

    list1->clear();
    list2->clear();
    list3->clear();

    QSqlQuery query(*kls->m_db);
    query.setForwardOnly(true);

    if (query_exec(*kls->m_db, query, q)) {
        while (query.next()) {
            QString r = query.value(0).toString();
            QString s = query.value(1).toString();

            list2->addItem(s);
        }
    }
}



void DockAddr::onSelectionChanged1()
{
}

void DockAddr::onSelectionChanged2()
{
    Klassif *kls = m_kl_list.findKlN(m_nazv);
    if (!kls) return;

    QList<QListWidgetItem *> selectedItems = list2->selectedItems();
    if (selectedItems.isEmpty()) return;

    QString street = selectedItems.first()->text();

    QString q;

    QString ord =  QString(
        "NULLIF(regexp_replace(%1, '\\D.*$', ''), '')::int," //  -- Числовая часть
        "regexp_replace(%1, '^\\d+', '') ").arg(m_dom);      //  -- Буквенная часть

    if (!is_POSTGRESQL()) {
        ord = QString(
                "TRY_CAST(SUBSTRING(%1, 1, PATINDEX('%[^0-9]%', %1 + '0') - 1) AS INT)," // -- Числовая часть
                "SUBSTRING(%1, PATINDEX('%[^0-9]%', %1 + '0'), LEN(%1))").arg(m_dom);      //-- Буквенная часть
    }

    q = QString("select %1, %2 from %3 where %4 = '%5' order by %6").arg(kls->id, m_dom, m_nazv, m_str, street, ord);

    list3->clear();

    QSqlQuery query(*kls->m_db);
    query.setForwardOnly(true);

    if (query_exec(*kls->m_db, query, q)) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString d = query.value(1).toString();
//            list3->addItem(d);
            QListWidgetItem *item = new QListWidgetItem(d);
            item->setData(Qt::UserRole, id);
            list3->addItem(item);
        }
    }
}

/*
void DockAddr::onSelectionChanged3()
{
    Klassif *kls = m_kl_list.findKlN(m_nazv);
    if (!kls) return;

    QList<QListWidgetItem *> selectedItems = list3->selectedItems();
    if (selectedItems.isEmpty()) return;

    QString dom = selectedItems.first()->text();
}
*/

void DockAddr::onTextChanged(const QString &text)
{
    str_fnd = text;

    init_find(str_fnd);
}

void DockAddr::onItemClicked(QListWidgetItem *item)
{
    Klassif *kls = m_kl_list.findKlN(m_nazv);
    if (!kls) return;

    QVariant data = item->data(Qt::UserRole);

    int id = data.toInt();

    m_view->moveGeo(m_nazv, id);
}

