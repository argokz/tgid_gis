#include <QtGui>
#include <QtWidgets>
#include <QtSql>

#include "DbWindow.h"

#include <gidview/GidWidget.h>

#include "QSortFilterSqlQueryModel.h"
#include <db/db.h>

#include <init/init_files.h>

#include <mainwindow.h>
#include <any/MyMain.h>

//#include "DbDelegate.h"

//#include "edit/help.h"

//#include "db.h"

//#include "mainwindow.h"
//#include "GidWidget.h"
//#include "coordlis.h"
//#include "Graph.h"

QString findColumnRusName(const QString & d, const QString & n1, const QString & n2);
QString findNewColumnName(const QString & tn, const QString & old_name);
const CColName *findColumnRusNameFull(const QString & d, const QString & n1, const QString & n2);

DbWindow *getTableView(QSqlDatabase &db, const QString & tn, const QString & q, const QString & title)
{
    if (!&db)  return nullptr;

    DbWindow *view = new DbWindow;

    if (!view) return nullptr;

    QString style = readQSS("qss2/table.qss");
    view->setStyleSheet(style);

    view->init(db, tn, q, title);

    return view;
}

DbWindow::DbWindow()
    : QTableView()
{

    // Это для выделения строки
   setMouseTracking(true);  // Включаем отслеживание движения мыши
   hoverDelegate = new HoverDelegate(this);
   setItemDelegate(hoverDelegate);
}

QAction *addToggleValue(QWidget *widget, const QString & text, const QString & text2, const QString & icon_name, bool * value);
void initActionValue();

void setRowHeightForTwoLines(QTableView *tableView, int n);

bool no_view(const QString &col, const QString &rus, const QString & table)
{
    QString coll = col.toLower();
    QString tablel = table.toLower();

    if (coll == "lineid" || coll == "calculationid" || coll == "n_pot" || coll == "id_old") return true;

    if (coll == "nodeid" && tablel != "pressregulators" && tablel != "bypass") return true;



    if (col == rus) {
        QRegularExpression regex("^b\\d+$");
        QRegularExpressionMatch match = regex.match(coll);

        if (match.hasMatch()) {
            return true;
        }

    }

    return false;
}


void DbWindow::init(QSqlDatabase &db, const QString & _tn, const QString & q0, const QString & _title)
{
    m_db = &db;
    m_title = _title;

    QString q = split_query(db, q0);

    QSortFilterSqlQueryModel *model = new QSortFilterSqlQueryModel(nullptr);

//    QString qq = QString("SELECT TOP 2147483647 * FROM ( %1 ) __").arg(q);
    QString qq = SelectTop(q);

    model->setQuery(qq, db);
    model->setQuery0(qq);

    if (this->m_delete) {
        model->setDelete();
    }


//    QInputDialog::getMultiLineText(nullptr, "title", "label", qq);
    qDebug() << qq;

    old_names.clear();
    int nc = model->columnCount();
    for (int i = 0; i < nc; i++) {
        QString old_name = model->headerData(i, Qt::Horizontal).toString();
        old_names.push_back(old_name);

        QString new_name = findNewColumnName(_tn, old_name);
        QString fn = findColumnRusName(getDatabaseName(db), _tn, new_name);
        v_col.push_back(old_name);
        model->setHeaderData(i, Qt::Horizontal, fn);
    }

    this->tn = _tn;
    this->setModel(model);
//    this->setItemDelegate(new ButtonDelegate());  // Устанавливаем делегат для рисования кнопок


#if 1
    CustomHeaderView *header = new CustomHeaderView(Qt::Horizontal, getDatabaseName(db), _tn, old_names);
    this->setSortingEnabled(true);
    header->setSectionsClickable(true);
    header->setHighlightSections(true);
#else

    QHeaderView *header = this->horizontalHeader();
#endif

    this->setHorizontalHeader(header);

//    header->setFixedHeight(100);
//    setRowHeightForTwoLines(this, 4);
//    header->setDefaultAlignment(Qt::AlignCenter | (Qt::Alignment)Qt::TextWordWrap);

    header->setDefaultAlignment(Qt::AlignCenter | (Qt::Alignment)Qt::TextWrapAnywhere);
    
    QTimer::singleShot(150, [=]() {
        this->resizeColumnsToContents();

        QSettings settings;

        for (int c = 0; c < nc; c++) {
            QString name = this->model()->headerData(c, Qt::Horizontal).toString();
            int y = settings.value(QString("DbWindow/%1/%2").arg(_tn, v_col[c]), true).toBool();

            if (no_view(v_col[c], name, this->tn)) y = false;

            int width = settings.value(QString("DbWindow/%1/width/%2").arg(_tn, v_col[c]), -1).toInt();
            this->setColumnHidden(c, !y);
            if (width > 0) {
                this->setColumnWidth(c, width);
//                qDebug() << "c=" << c << " w=" << width << " col=" << v_col[c];
            }
        }
        this->closed = false;
    });

    this->verticalHeader()->setVisible(false);
    this->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
    this->setSortingEnabled(true);

}

void DbWindow::set_hide(std::set<int> &h)
{
    for (auto &it : h) {
        this->setColumnHidden(it, true);
    }
}



//#include <OpenXLSX.hpp>
//using namespace OpenXLSX;

// [0] include QXlsx headers
#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"
using namespace QXlsx;

#include <property/variantdelegate.h>

bool save_excel(const QString & filename, QSqlDatabase &db, const QString & tn, const QString & q, int *hide);

bool DbWindow::save_excel(const QString & filename)
{
    QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)this->model();
//    QSqlQuery query = model->query();
//    const QSqlQuery *query = &model->query();
//    QString q = query->executedQuery();

    QString q = model->query().executedQuery();

    int h[256];

    QHeaderView *hv = this->horizontalHeader();

    for (int c = 0; c < hv->count(); c++) {
        h[c] = this->isColumnHidden(c);
    }

    return ::save_excel(filename, *m_db, this->tn, q, h);
}

void DbWindow::OnExcel() 
{
    QString fn = QString("%1/tgid.xlsx").arg(QDir::tempPath());

    if (save_excel(fn)) {
        QString path = QString("file:///%1").arg(fn);
        QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
    }
    else {
        QMessageBox::warning(this, "", QString(tr("Не могу открыть на запись файл %1. Закройте открытый файл excel")).arg(fn));
    }

}



void DbWindow::OnFindAll() 
{
    QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)this->model();

    model->setFilterColumn(v_col[0]);
    model->setFilterFlags(Qt::MatchContains);
    model->filter("");
}

void DbWindow::OnAll()
{
    QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)this->model();

    bool all = model->getAll();

    model->setAll(!all);
}

#include <dialog/ChMenuDial.h>

void DbWindow::OnHide()
{
    QHeaderView *hv = this->horizontalHeader();
    ChMenuDial dlg(this, QObject::tr(""));
    
    for (int c = 0; c < hv->count(); c++) {
        QString name = this->model()->headerData(c, Qt::Horizontal).toString();
        bool y = this->isColumnHidden(c);
        if (!no_view(v_col[c], name, this->tn)) {
            dlg.Add(name, c, !y);
        }
    }

    if (dlg.exec()) {
        QSettings settings;
        
        for (int c = 0; c < hv->count(); c++) {
            QString name = this->model()->headerData(c, Qt::Horizontal).toString();
//            bool y = dlg.out.find(c) != dlg.out.end();
            bool y = std::find(dlg.out.begin(),dlg.out.end(), c)  != dlg.out.end();

            this->setColumnHidden(c, !y);

            settings.setValue(QString("DbWindow/%1/%2").arg(this->tn, v_col[c]), y);

//            QString name = this->model()->headerData(c, Qt::Horizontal).toString();
//            dlg.Add(name, c, true);
        }
    }
}

#include <property/PropertyDial.h>

void DbWindow::onPropertyAccepted()
{
    auto *dlg = qobject_cast<PropertyDial *>(sender());
    if (dlg) {
        if (update_db(dlg->map_edited, dlg->getUser())) {
            QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)this->model();
            model->select();

            emit dataChanged();
        }
    }
}

void DbWindow::onPropertyRejected()
{
    auto *dlg = qobject_cast<PropertyDial *>(sender());
    if (dlg) {
        if (dlg->get_autodelete()) {
            QString q = QString("delete from %1 where id in (%2)").arg(br_text(dlg->m_table)).arg(dlg->m_id1);
            query_exec(*this->m_db, q);

            QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)this->model();
            model->select();
        }
    }
}


void DbWindow::reset_q()
{
    QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)this->model();
    model->select();
}


void DbWindow::reset_q(const QString & q)
{
    QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)this->model();
    model->setQuery0(q);
    model->select();
}


void DbWindow::OnEdit()
{
    if (this->m_id <= 0) return;

    QString file_table = this->tn;

    if (this->m_dop != "") {
        file_table = QString("%1_%2").arg(this->tn, this->m_dop);
    }


    auto *dlg = new PropertyDial(this, *this->m_db, this->m_title, this->tn, file_table, this->m_id, -1, pr_type_any, nullptr);

//    if (this->m_dop != "") {
//        dlg->set_table_file(QString("%1_%2").arg(this->tn, this->m_dop));
//    }

    dlg->show();

    connect(dlg, SIGNAL(accepted()), this, SLOT(onPropertyAccepted()));
}

void DbWindow::OnAdd()
{
    QString q = QString("INSERT INTO %1 DEFAULT VALUES").arg(br_text(this->tn));
    QSqlQuery query(*this->m_db);
    //const QSqlQuery *query = &model->query();


//    int last_id = execInsertQ(*this->m_db, q);

    std::map<QString, QVariant> data;

    int last_id = insertIntoDatabase(*this->m_db, this->tn, data);


//    if (query_exec(*this->m_db, query, q)) {
    if (last_id > 0) {
        QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)this->model();
        model->select();
        
        int lastRow = model->rowCount() - 1;
        this->selectRow(lastRow);  // Выделяет последнюю строку
        this->scrollToBottom();     // Прокручивает к последней строке

        QString file_table = this->tn;

        if (this->m_dop != "") {
            file_table = QString("%1_%2").arg(this->tn, this->m_dop);
        }


        auto *dlg = new PropertyDial(this, *this->m_db, this->m_title, this->tn, file_table, last_id, -1, pr_type_any, nullptr);

        connect(dlg, SIGNAL(accepted()), this, SLOT(onPropertyAccepted()));
        connect(dlg, SIGNAL(rejected()), this, SLOT(onPropertyRejected()));
    
        dlg->set_autodelete();
        dlg->show();
    }

//    auto *dlg = new PropertyDial(this, *this->m_db, this->m_title, this->tn, this->m_id, -1, pr_type_any, nullptr);
//    dlg->show();

//    connect(dlg, SIGNAL(accepted()), this, SLOT(onPropertyAccepted()));
}



void DbWindow::OnDelete()
{
    if (this->m_id <= 0) return;
      
    if (QMessageBox::question(0, "", QString("Удалить строку № %1 в таблице %2 ?").arg(this->m_id).arg(this->tn)) != QMessageBox::Yes) return;

    QString q = QString("DELETE FROM %1 where id=%2").arg(br_text(this->tn)).arg(this->m_id);
    QSqlQuery query(*this->m_db);
    //const QSqlQuery *query = &model->query();

    if (query_exec(*this->m_db, query, q)) {
        QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)this->model();
        model->select();
    }

}

void DbWindow::OnOpres()
{
    if (this->m_id <= 0) return;

    GidWidget *gid = this->gid;
    if (!gid) return;

    gid->setOpres(m_opres, this->m_id);

//    if (QMessageBox::question(0, "", QString("!! строку № %1 в таблице %2 ?").arg(this->m_id).arg(this->tn)) != QMessageBox::Yes) return;
}

void DbWindow::OnMoveTo()
{
//  QModelIndex index = currentIndex();
//  int row = index.row();
//  int n = index.sibling(row, 0).data().toInt();

    QItemSelectionModel *model = this->selectionModel();

    model->currentIndex();

    QModelIndex index = model->currentIndex();

    int id = this->model()->data(this->model()->index(index.row(),0)).toInt();
//    QMessageBox::information(this, "", QString("%1").arg(id));

    GidWidget *gid = this->gid;
    if (!gid) return;

    switch (this->move_to) {
        case move_to_node:
            gid->moveNode(id);
            break;
//        case move_to_node_out:
//            gid->moveNodeOut(id);
//            break;
        case move_to_line:
            gid->moveLine(id);
            break;
//        case move_to_line_out:
//            gid->moveLineOut(id);
//            break;
        case move_to_line_big:
            break;
        case move_to_geo:
            gid->moveGeo(tn, id);
            break;
        case move_to_unknown:
            break;
    }

/*
  QModelIndexList ilist = model->selectedRows();


  QModelIndex index = model->currentIndex();

  for (auto &it: ilist) {
    int id = this->model()->data(this->model()->index(it.row(),0)).toInt();
    QMessageBox::information(this, "", QString("%1").arg(id));
  }
*/

//  QString fieldName = record.fieldName(0);
//  init(record);
}

void DbWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (this->move_to != move_to_unknown && this->gid) {
            OnMoveTo();
        }
    }

    QTableView::mouseDoubleClickEvent(event);
}

void DbWindow::mousePressEvent(QMouseEvent *event)
{
    QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)this->model();

//    CFPoint pt = ScreenToCoord(CFPoint(event->position().x(), event->position().y()));
    if (event->button() == Qt::RightButton) {
//        RButMenu(QCursor::pos(), pt);
        QModelIndex index = this->currentIndex();

        int col = columnAt(event->position().x());
        int row = rowAt(event->position().y());

        this->m_id = -1;

        if (index.isValid()) {
            QModelIndex index2 = index.sibling(index.row(), 0);
            this->m_id = index2.data().toInt();
        }

/*
        if (this->m_id == -1 && row >= 0 && index.isValid()) {
            QModelIndex index2 = index.sibling(row, 0);
            this->m_id = index2.data().toInt();
        }
*/

        QMenu *menu = new QMenu( this);

        QAction *action = nullptr;
//        QMenu *subMenu = menu->addMenu("Перейти к объекту на схеме");

        action = new QAction(tr("Экспорт в MS Excel"),this);
        connect(action, SIGNAL(triggered()),this,SLOT(OnExcel()));
        menu->addAction(action);

        action = new QAction(tr("Скрыть/показать столбцы"),this);
        connect(action, SIGNAL(triggered()),this,SLOT(OnHide()));
        menu->addAction(action);

        action = new QAction(tr("Скрыть/показать пустые столбцы"),this);
        connect(action, SIGNAL(triggered()),this,SLOT(OnHide2()));
        menu->addAction(action);

        bool all = model->getAll();
        
        if (all) {
            action = new QAction(tr("Только по внешнему слою"),this);
        }
        else {
            action = new QAction(tr("Все объекты"),this);
        }
        
        connect(action, SIGNAL(triggered()),this,SLOT(OnAll()));
        menu->addAction(action);


        if (col >= 0 || model->filter() != "") {
            menu->addSeparator();
        }


        if (col >= 0) {
            QString name = model->headerData(col, Qt::Horizontal).toString();

            action = new QAction(QString(tr("Фильтр по полю %1")).arg(name),this);
            action->setData(col);
            connect(action, SIGNAL(triggered()),this,SLOT(OnFind()));
            menu->addAction(action);
        }

        if (model->filter() != "") {
            action = new QAction(tr("Сбросить фильтр"),this);
            connect(action, SIGNAL(triggered()),this,SLOT(OnFindAll()));
            menu->addAction(action);
        }


        if (this->m_id <= 0 && this->m_add) {
            menu->addSeparator();
            action = new QAction(tr("Добавить объект"),this);
            connect(action, SIGNAL(triggered()),this,SLOT(OnAdd()));
            menu->addAction(action);
        }


        if (this->m_id > 0) {
            if (this->m_edit || this->m_delete || this->move_to != move_to_unknown && this->gid) {
                menu->addSeparator();
            }
            if (this->m_edit) {
                action = new QAction(tr("Редактирование объекта"),this);
                connect(action, SIGNAL(triggered()),this,SLOT(OnEdit()));
                menu->addAction(action);
            }

            if (this->m_add) {
                action = new QAction(tr("Добавить объект"),this);
                connect(action, SIGNAL(triggered()),this,SLOT(OnAdd()));
                menu->addAction(action);
            }

            if (this->m_delete) {
                action = new QAction(tr("Удалить объект"),this);
                connect(action, SIGNAL(triggered()),this,SLOT(OnDelete()));
                menu->addAction(action);
            }

            if (this->move_to != move_to_unknown && this->gid) {
                action = new QAction(tr("Перейти к объекту на схеме"),this);
                connect(action, SIGNAL(triggered()),this,SLOT(OnMoveTo()));
                menu->addAction(action);
            }

            if (this->m_opres != "") {
                action = new QAction(tr("Перейти"),this);
                connect(action, SIGNAL(triggered()),this,SLOT(OnOpres()));
                menu->addAction(action);
            }
        }

        //        connect(action, SIGNAL(hovered()),this,SLOT(viewNode2()));
        menu->exec( QCursor::pos() );

        return;
    }

    QTableView::mousePressEvent(event);
}

void DbWindow::saveSettings()
{
    if (this->closed) return;

    QHeaderView *hv = this->horizontalHeader();

    QSettings settings;

    for (int c = 0; c < hv->count(); c++) {
        QString name = this->model()->headerData(c, Qt::Horizontal).toString();
        int width = this->columnWidth(c);
        if (width == 0) {
            settings.setValue(QString("DbWindow/%1/%2").arg(this->tn, v_col[c]), false);
        }
        qDebug() << v_col[c] << " " << width;
        settings.setValue(QString("DbWindow/%1/width/%2").arg(this->tn, v_col[c]), width);
    }
}


void DbWindow::hideEvent(QHideEvent *event)
{    
    saveSettings();
    QTableView::hideEvent(event);
}


void DbWindow::findText(const QString & text)
{
    QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)this->model();

    int c = m_find_col;

    if (c >= 0) {
        model->setFilterColumn(v_col[c]);
        model->setFilterFlags(Qt::MatchContains);
        model->filter(text);
    }
}


void DbWindow::OnFind() 
{
    QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)this->model();

    int c = -1;

    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        c = action->data().toInt();
    }

    if (c >= 0 && c < v_col.size()) {
        QString name = model->headerData(c, Qt::Horizontal).toString();

        bool ok;
        QString text = "";
        text = QInputDialog::getText(this,"", QString(tr("Введите строку для поиска в колонке %1")).arg(name), QLineEdit::Normal, text, &ok);

        if (ok) {
            model->setFilterColumn(v_col[c]);
            model->setFilterFlags(Qt::MatchContains);
            model->filter(text);
        }
    }
}



QString html_text(const QString & txt);

void DbWindow::createToolBar(MyMain *main_window)
{
// Колонки таблицы
// Формат таблицы
// Скрыть пустые столбцы
// Печать
// Excel
// Группировать
// Колонка для поиска
// Поиск



    QToolBar *bar = main_window->toolBar();

    bar->setFixedHeight(48);
    bar->setIconSize(QSize(30, 30)); 


    if (this->m_add) {
        QAction *aAdd = new QAction(tr("Добавить объект"));
        aAdd->setIcon(QIcon(":/images/db2/add_item_blue.svg"));
        aAdd->setToolTip("Добавить объект");
        connect(aAdd, SIGNAL(triggered()), this, SLOT(OnAdd()));
        bar->addAction(aAdd);
    }


    QAction *aHide = new QAction(tr("Скрыть/показать столбцы"));
    aHide->setIcon(QIcon(":/images/db2/table-col-list.svg"));
    aHide->setToolTip("Скрыть/показать столбцы");
    connect(aHide, SIGNAL(triggered()), this, SLOT(OnHide()));
    bar->addAction(aHide);

    QAction *aHide2 = new QAction(tr("Скрыть пустые столбцы"));
    aHide2->setIcon(QIcon(":/images/db2/col_remove.svg"));
    aHide2->setToolTip("Скрыть пустые столбцы");
    connect(aHide2, SIGNAL(triggered()), this, SLOT(OnHide2()));
    bar->addAction(aHide2);

    /*
    QAction *aFilePrint = new QAction(tr("Печать"));
    aFilePrint->setIcon(QIcon(":/images/db2/print.svg"));
    aFilePrint->setToolTip("Вывод на печать текущего документа\\nПечать");
    connect(aFilePrint, SIGNAL(triggered()), this, SLOT(onFilePrint()));
    */

    QAction *aExcel = new QAction(tr("Таблицы Excel.."));
    aExcel->setIcon(QIcon(":/images/db2/excel.svg"));
    aExcel->setToolTip(html_text("Экспорт в MS Excel\nЭкспорт в MS Excel"));
    connect(aExcel, SIGNAL(triggered()), this, SLOT(OnExcel()));
    bar->addAction(aExcel);

/*
    QAction *aFind = new QAction(tr("Поиск"));
    aFind->setIcon(QIcon(":/images/db/find.png"));
    aFind->setToolTip("Поиск");
    connect(aFind, SIGNAL(triggered()), this, SLOT(OnFind()));
    bar->addAction(aFind);
*/

    /*

    aFilePrintPreview = new QAction(tr("Предварительный просмотр"));
    aFilePrintPreview->setIcon(QIcon(":/images/pjezo/FilePrintPreview.png"));
    aFilePrintPreview->setToolTip("Предварительный просмотр документа перед выводом на печать\\nПредварительный просмотр");
    connect(aFilePrintPreview, SIGNAL(triggered()), this, SLOT(onFilePrintPreview()));
    */

//    bar->addAction(aFilePrint);

    QComboBox *findCB = new QComboBox;

    QHeaderView *hv = this->horizontalHeader();
    
    for (int c = 0; c < hv->count(); c++) {
        QString name = this->model()->headerData(c, Qt::Horizontal).toString();
        bool y = this->isColumnHidden(c);
        if (!y) {
            findCB->addItem(name, c);
        }
    }

    bar->addWidget(findCB);


    QAction *aFind = new QAction(tr("Таблицы Find.."));
    aFind->setIcon(QIcon(":/images/db2/find-svgrepo-com.svg"));
    bar->addAction(aFind);


    QLineEdit *findEdit = new QLineEdit();
    findEdit->setPlaceholderText(tr("Введите текст для поиска..."));



    QTimer *timer = new QTimer();
    timer->setInterval(500);
    timer->setSingleShot(true);

    QObject::connect(timer, &QTimer::timeout, [&]() {
        timer->start();
    });

    connect(findCB, &QComboBox::currentIndexChanged, [=](int id) {
        m_find_col = findCB->currentData().toInt();
        findEdit->setPlaceholderText(QString(tr("Введите текст для поиска в %1\t🔍")).arg(findCB->currentText()));
        findEdit->clear();
    });


    bar->addWidget(findEdit);
    connect(findEdit, &QLineEdit::textChanged, this, &DbWindow::findText);



    QAction *aExit = new QAction(tr("Закрыть вкладку"));
    aExit->setIcon(QIcon(":/images/db2/cross-svgrepo-com.svg"));
    aExit->setToolTip("Закрыть вкладку");
    bar->addAction(aExit);

    connect(aExit, SIGNAL(triggered()), this, SLOT(closeTable()));
}

void DbWindow::closeTable()
{
    saveSettings();
    this->closed = true;
    main_window->removeWidgetFromCenter(this->parentWidget());
    qDebug() << "Закрылся";
}

void DbWindow::OnHide2()
{
    QHeaderView *hv = this->horizontalHeader();
    ChMenuDial dlg(this, QObject::tr(""));
    QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)this->model();

    QString params = "";

    {
        QString q = model->getOriginalQuery();

        QSqlQuery query(*m_db);
        query.setForwardOnly(true);

        if (query.exec(q)) {
            QSqlRecord record = query.record();
            int columnCount = record.count();
            for (int i = 0; i < columnCount; ++i) {
                QString fieldName = record.fieldName(i);
                if (params != "") params += ",\n";
                params += QString("max(\"%1\") as \"%1\"").arg(fieldName);
            }
        } 
        else {
            qWarning() << "Query failed:" << query.lastError().text();
        }
    }

    QString q = QString("select\n%1\nfrom (%2) _\n LIMIT 1").arg(params, model->getOriginalQuery());
    qDebug() << q;

    std::vector<QVariant> vv;
    bool ret = readTableRowValues(*m_db, q, vv);

    if (!ret) return;

#if 0
    for (int c = 0; c < hv->count(); c++) {
        bool y = this->isColumnHidden(c);
        QString name = this->model()->headerData(c, Qt::Horizontal).toString();

        QVariant val = vv[c];

        this->setColumnHidden(c, y || val.isNull() || val.toString() == "0"  || val.toString() == "" || no_view(v_col[c], name, this->tn)) ;
    }
#endif

//    QHeaderView *hv = mainTable->horizontalHeader();
//    ChMenuDial dlg(this, QObject::tr(""));

    auto *mainTable = this;

    for (int c = 0; c < hv->count(); c++) {
        bool y = mainTable->isColumnHidden(c);
        QString name = mainTable->model()->headerData(c, Qt::Horizontal).toString();

        bool empty = vv[c].isNull() || vv[c].toString() == "" || vv[c].toString() == "0";

        if (empty && !no_view(v_col[c], name, this->tn)) {
            dlg.Add(name, c, !y);
        }
    }


    if (dlg.exec()) {
        QSettings settings;

        for (int c = 0; c < hv->count(); c++) {
            QString name = mainTable->model()->headerData(c, Qt::Horizontal).toString();
            bool y = std::find(dlg.out.begin(),dlg.out.end(), c)  != dlg.out.end();
            bool empty = vv[c].isNull() || vv[c].toString() == "" || vv[c].toString() == "0";

            if (empty) {
                mainTable->setColumnHidden(c, !y);

                settings.setValue(QString("DbWindow/%1/%2").arg(this->tn, v_col[c]), y);
            }

        }
    }




}


void ButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == 0) {  // Если это последняя колонка с кнопкой
        QPushButton button;
        button.setText("Удалить");
        button.setIcon(QIcon(":/images/db2/remove_item.svg"));
        button.setGeometry(option.rect);
        button.render(painter);
    }
    else {
        QItemDelegate::paint(painter, option, index);
    }
}

// Обработка клика по кнопке
bool ButtonDelegate::editorEvent(QEvent *event, QAbstractItemModel  *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
    if (event->type() == QEvent::MouseButtonRelease) {
        if (index.column() == 0) {  // Если клик по колонке с кнопкой
#if 0
            // Здесь добавьте логику для удаления строки
            qDebug() << "Удаление строки: " << index.row();
            // Например, удалить строку в модели
            QSqlQuery query;
            query.prepare("DELETE FROM table_name WHERE id = :id");
            query.bindValue(":id", index.data().toInt());  // Используйте уникальный идентификатор строки
            query.exec();
#endif
            return true;
        }
    }
    return QItemDelegate::editorEvent(event, model, option, index);
}


void CustomHeaderView::mouseMoveEvent(QMouseEvent *event)
{
    int section = logicalIndexAt(event->pos());
    if (section >= 0 && model()) {

        QString columnName = model()->headerData(section, Qt::Horizontal).toString();
//        QString tooltipText = columnName;  // Текст для ToolTip

        QString f1 = columnName;

        const CColName *c = findColumnRusNameFull(m_dbName, m_tableName, m_old_names[section]);

        if (c && c->name_full != "") {
            f1 = c->name_full;
        }
        
//        QString tooltipText = QString("Table: %1\nColumn: %2").arg(m_tableName, m_old_names[section]);  // Формируем ToolTip
        QString tooltipText = QString("<p>%1</p>").arg(f1);

        QToolTip::showText(event->globalPosition().toPoint(), tooltipText, this); // Показываем ToolTip
//        qDebug() << "Mouse is over header section:" << section;
    }
    QHeaderView::mouseMoveEvent(event);
}

QSize CustomHeaderView::sectionSizeFromContents(int logicalIndex) const 
{
/*
    if (orientation() == Qt::Horizontal && (logicalIndex == 0 || logicalIndex == 2)) {
        int width = sectionSize(logicalIndex) + sectionSize(logicalIndex + 1); // Объединяем ширину колонок
        return QSize(width, 60); // Высота больше для нескольких строк
    }
*/
    return QHeaderView::sectionSizeFromContents(logicalIndex);
}


void CustomHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    QHeaderView::paintSection(painter, rect, logicalIndex);
}


#if 0
void CustomHeaderView::drawMultiLineHeader(QPainter *painter, const QRect &rect, const QString &line1, const QString &line2, const QString &line3) const 
{
    painter->setBrush(QColor(220, 220, 220)); // Заливка
    painter->drawRect(rect);

    // Устанавливаем шрифт и высоту строк
    QFont font = painter->font();
    QFontMetrics metrics(font);
    int lineHeight = metrics.height();
    int margin = 5;

    // Рисуем текст по уровням
    int y = rect.top() + margin;
    if (!line1.isEmpty()) {
        painter->drawText(rect.adjusted(0, 0, 0, -2 * lineHeight), Qt::AlignHCenter | Qt::AlignTop, line1);
        y += lineHeight;
    }
    if (!line2.isEmpty()) {
        painter->drawText(rect.adjusted(0, lineHeight, 0, -lineHeight), Qt::AlignHCenter | Qt::AlignTop, line2);
        y += lineHeight;
    }
    if (!line3.isEmpty()) {
        painter->drawText(rect.adjusted(0, 2 * lineHeight, 0, 0), Qt::AlignHCenter | Qt::AlignTop, line3);
    }
}
#endif
#if 0

2. Применение кастомного заголовка
#include <QApplication>
#include <QTableView>
#include <QStandardItemModel>
#include "CustomHeaderView.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QTableView tableView;

    // Создаём модель
    QStandardItemModel model(5, 5); // 5 строк, 5 столбцов
    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 5; ++col) {
            model.setItem(row, col, new QStandardItem(QString("Cell %1,%2").arg(row).arg(col)));
        }
    }
    tableView.setModel(&model);

    // Устанавливаем кастомный заголовок
    CustomHeaderView *headerView = new CustomHeaderView(Qt::Horizontal);
    tableView.setHorizontalHeader(headerView);

    // Настраиваем заголовки
    tableView.horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView.verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    tableView.show();
    return app.exec();
}

Что делает этот код:
Многострочные заголовки:

Использует три уровня текста (line1, line2, line3) для заголовков.
Линии рисуются с интервалом на основе высоты шрифта.
Объединение заголовков:

Колонки 0 и 1 объединяются под "Group 1" с подзаголовками "Sub 1" и "Sub 2".
Колонки 2 и 3 объединяются под "Group 2" с подзаголовками "Sub 3" и "Sub 4".
Размеры секций:

Высота строки заголовка увеличивается для размещения нескольких строк текста.
Одиночные заголовки:

Колонки без объединения получают одноуровневую шапку, например, "Col 4".
Итог
При запуске таблицы вы получите заголовок с несколькими строками и объединением:

Group 1 -> объединяет "Sub 1" и "Sub 2".
Group 2 -> объединяет "Sub 3" и "Sub 4".
Оставшиеся колонки имеют одиночные заголовки.
Если нужно добавить больше уровней или улучшить отступы, скажите!


#endif





