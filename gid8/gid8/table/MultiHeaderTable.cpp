#include "MultiHeaderTable.h"
#include "table/DbWindow.h"
#include "any/MyMain.h"
#include "init/init_files.h"
#include "qtoolbar.h"
#include "table/QSortFilterSqlQueryModel.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QLineEdit>
#include <db/db.h>
#include <gidview/GidWidget.h>
#include <dialog/ChMenuDial.h>
#include <property/PropertyDial.h>
#include <mainwindow.h>
#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"
#include <property/variantdelegate.h>
using namespace QXlsx;

QString findColumnRusName(const QString & d, const QString & n1, const QString & n2);
QString findNewColumnName(const QString & tn, const QString & old_name);
const CColName *findColumnRusNameFull(const QString & d, const QString & n1, const QString & n2);
bool no_view(const QString &col, const QString &rus, const QString & table);
QString html_text(const QString & txt);
bool save_excel(const QString & filename, QSqlDatabase &db, const QString & tn, const QString & q, int *hide);

QList<int> teplopoteryExecuteQuery(const QString& sql, QSqlDatabase* db);
QString teplopoteryReplacePlaceholders(const QString& jsonString, const QList<int>& queryResults);

MultiHeaderTable::MultiHeaderTable(QWidget *parent) : QWidget(parent), mainTable(new ResizableTableView(this)), customHeader(nullptr) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(mainTable);
    setMouseTracking(true);
}


void MultiHeaderTable::init(QSqlDatabase &db, const QString & _tn, const QString & q0, const QString & _title)
{
    m_db = &db;
    m_title = _title;

    QString q = split_query(db, q0);

    QSortFilterSqlQueryModel *model = new QSortFilterSqlQueryModel(nullptr);

    QString qq = SelectTop(q);

    model->setQuery(qq, db);
    model->setQuery0(qq);

    if (this->m_delete) {
        model->setDelete();
    }

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

    tn = _tn;
    mainTable->setModel(model);

    CustomHeaderView *header = new CustomHeaderView(Qt::Horizontal, getDatabaseName(db), _tn, old_names);
    mainTable->setSortingEnabled(true);
    header->setSectionsClickable(true);
    header->setHighlightSections(true);


    mainTable->setHorizontalHeader(header);

    header->setDefaultAlignment(Qt::AlignCenter | (Qt::Alignment)Qt::TextWrapAnywhere);

    // То же, что в DbWindow: обмеряем только видимые столбцы без
    // сохранённой ширины и по выборке строк, а не по всей таблице.
    // Подробное объяснение — в table/DbWindow.cpp рядом с константами.
    mainTable->horizontalHeader()->setResizeContentsPrecision(kWidthSampleRows);

    QTimer::singleShot(150, [=]() {
        QSettings settings;

        for (int c = 0; c < nc; c++) {
            QString name = mainTable->model()->headerData(c, Qt::Horizontal).toString();
            int y = settings.value(QString("DbWindow/%1/%2").arg(_tn, v_col[c]), true).toBool();

            if (no_view(v_col[c], name, this->tn)) y = false;

            int width = settings.value(QString("DbWindow/%1/width/%2").arg(_tn, v_col[c]), -1).toInt();
            mainTable->setColumnHidden(c, !y);

            if (!y) continue;

            if (width > 0) {
                mainTable->setColumnWidth(c, width);
            }
            else {
                mainTable->resizeColumnToContents(c);
                const int w = mainTable->columnWidth(c);
                if (w < kMinColWidth) mainTable->setColumnWidth(c, kMinColWidth);
                else if (w > kMaxColWidth) mainTable->setColumnWidth(c, kMaxColWidth);
            }
        }
        this->closed = false;
    });

    mainTable->verticalHeader()->setVisible(false);
    mainTable->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
    mainTable->setSortingEnabled(true);

}

void MultiHeaderTable::checkCustomHeader(){

    QString jsonString = readQUTF8("custom_headers/table/" + tn + "/"+m_dop+".json");
    if (!jsonString.isEmpty()){
        // Выполнение SQL-запроса
        QString sql = readQUTF8("custom_headers/table/" + tn + "/"+m_dop+".sql");
        if (!sql.isEmpty()){
            QList<int> queryResults = teplopoteryExecuteQuery(sql, m_db);
            if (queryResults.isEmpty()) {
                QMessageBox::critical(nullptr, "Ошибка", "SQL-запрос не вернул данных.");
                return;
            }

            // Замена плейсхолдеров
            QString updatedJson = teplopoteryReplacePlaceholders(jsonString, queryResults);

            QJsonDocument jsonDoc = QJsonDocument::fromJson(updatedJson.toUtf8());
            QJsonObject jsonObject = jsonDoc.object();
            addCustomHeader(jsonObject);
        }
        else
        {
            QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
            QJsonObject jsonObject = jsonDoc.object();
            addCustomHeader(jsonObject);}
    }

}

void MultiHeaderTable::createToolBar(MyMain *main_window)
{
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

    QAction *aHide2 = new QAction(tr("Скрыть/показать пустые столбцы"));
    aHide2->setIcon(QIcon(":/images/db2/col_remove.svg"));
    aHide2->setToolTip("Скрыть пустые столбцы");
    connect(aHide2, SIGNAL(triggered()), this, SLOT(OnHide2()));
    bar->addAction(aHide2);


    QAction *aExcel = new QAction(tr("Таблицы Excel.."));
    aExcel->setIcon(QIcon(":/images/db2/excel.svg"));
    aExcel->setToolTip(html_text("Экспорт в MS Excel\nЭкспорт в MS Excel"));
    connect(aExcel, SIGNAL(triggered()), this, SLOT(OnExcel()));
    bar->addAction(aExcel);

    if (this->m_mark_accessibility) {
    QAction *aMarker = new QAction(tr("Отметить на карте"));
    aMarker->setIcon(QIcon(":/images/db2/marker.svg"));
    aMarker->setToolTip(html_text("Отметка на карте\nПозволяет увидеть объекты с отклонением от нормы"));
    connect(aMarker, SIGNAL(triggered()), this, SLOT(OnMarkAccessibility()));
    bar->addAction(aMarker);
    }


    QComboBox *findCB = new QComboBox;

    QHeaderView *hv = mainTable->horizontalHeader();

    for (int c = 0; c < hv->count(); c++) {
        QString name = mainTable->model()->headerData(c, Qt::Horizontal).toString();
        bool y = mainTable->isColumnHidden(c);
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
    connect(findEdit, &QLineEdit::textChanged, this, &MultiHeaderTable::findText);



    QAction *aExit = new QAction(tr("Закрыть вкладку"));
    aExit->setIcon(QIcon(":/images/db2/cross-svgrepo-com.svg"));
    aExit->setToolTip("Закрыть вкладку");
    bar->addAction(aExit);

    connect(aExit, SIGNAL(triggered()), this, SLOT(closeTable()));
}

void MultiHeaderTable::addCustomHeader(const QJsonObject &tableData) {
    if (customHeader) return;


    mainTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    mainTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    // Создаём QScrollArea для заголовков
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Отключаем собственный скролл
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // Отключаем вертикальный скролл

    // Контейнер для заголовков
    customHeader = new QWidget();
    QGridLayout *headerLayout = new QGridLayout(customHeader);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(0);

    QFont headerFont;
    headerFont.setPointSize(10);
    headerFont.setBold(true);

    QJsonArray rows = tableData["table"].toObject()["rows"].toArray();

    int rowIndex = 0;
    QMap<int, int> colPositions; // Отслеживаем позицию колонок в каждой строке

    for (const QJsonValue &rowValue : rows) {
        QJsonArray cells = rowValue.toObject()["cells"].toArray();
        if (!colPositions.contains(rowIndex)) {
            colPositions[rowIndex] = 0; // Начало строки
        }

        for (const QJsonValue &cellValue : cells) {
            QJsonObject cell = cellValue.toObject();
            QString text = cell["text"].toString();
            int colspan = cell["colspan"].toInt(1);
            int rowspan = cell["rowspan"].toInt(1);
            int index = cell["index"].toInt(1);

            int colIndex = colPositions[rowIndex]; // Получаем текущую позицию колонки

            QLabel *headerLabel = new QLabel(text, this);
            headerLabel->setFont(headerFont);
            headerLabel->setAlignment(Qt::AlignCenter);
            headerLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            headerLabel->setWordWrap(true);
            headerLabel->setStyleSheet("border: 1px solid lightgray; padding: 2px; background-color: white;");

            headerLabel->setMinimumHeight(40 * rowspan);
            headerLabel->setProperty("colspan", colspan);
            headerLabel->setProperty("index", index);

            // Добавляем заголовок в сетку
            headerLayout->addWidget(headerLabel, rowIndex, colIndex, rowspan, colspan);
            qDebug() << "Added header" << text << "at row" << rowIndex << "col" << colIndex << "colspan" << colspan << "rowspan" << rowspan;

            // Обновляем позицию колонки для следующего заголовка
            for (int r = 0; r < rowspan; ++r) {
                colPositions[rowIndex + r] = colIndex + colspan;
            }
        }

        rowIndex++;
    }

    int rowHeight = 40;
    customHeader->setLayout(headerLayout);
    customHeader->setFixedHeight(rows.size() * rowHeight);

    // Динамически устанавливаем ширину customHeader по ширине таблицы
    int tableWidth = mainTable->horizontalHeader()->length();
    customHeader->setMinimumWidth(tableWidth);
    customHeader->setMaximumWidth(tableWidth);
    // Устанавливаем customHeader внутрь QScrollArea
    scrollArea->setWidget(customHeader);
    scrollArea->setFixedHeight(rows.size() * rowHeight);

    // Прячем стандартные заголовки
    mainTable->horizontalHeader()->setVisible(false);

    // Вставляем scrollArea в основной лейаут
    QVBoxLayout *parentLayout = qobject_cast<QVBoxLayout *>(layout());
    parentLayout->insertWidget(0, scrollArea);

    // Синхронизация прокрутки
    syncHeaderWithTable(scrollArea);
}

void MultiHeaderTable::resizeColumnDeal(){
    int offset = mainTable->horizontalHeader()->offset();  // Текущее смещение заголовка таблицы
    QGridLayout *headerLayout = qobject_cast<QGridLayout *>(customHeader->layout());
    QHeaderView *tableHeader = mainTable->horizontalHeader();
    // Устанавливаем начальную ширину столбцов
    for (int col = 0; col < tableHeader->count(); ++col) {
        int width = tableHeader->sectionSize(col);
        for (int row = 0; row < headerLayout->rowCount(); ++row) {
            QLayoutItem *item = headerLayout->itemAtPosition(row, col);
            if (item) {
                QWidget *widget = item->widget();
                if (widget) {
                    QLabel *label = qobject_cast<QLabel *>(widget);
                    if (label) {
                        QVariant colspan = label->property("colspan");
                        QVariant index = label->property("index");
                        if (colspan.isValid() && index.isValid()) {
                            int myColspan = colspan.toInt();
                            int myIndex = index.toInt();
                            QString k = label->text();
                            if (myColspan>1){
                                int mySize = 0;
                                for (int j=0; j<myColspan; j++)
                                    mySize+= tableHeader->sectionSize(myIndex+j);
                                widget->setFixedWidth(mySize);

                            }
                            else
                            {
                                widget->setFixedWidth(tableHeader->sectionSize(myIndex));
                            }


                        }
                    }

                }
            }
        }
    }

    int tableWidth = mainTable->horizontalHeader()->length();
    customHeader->setFixedWidth(tableWidth);

    QScrollBar *headerScrollBar = scrollArea->horizontalScrollBar();
    headerScrollBar->setValue(0);
    headerScrollBar->setValue(offset);
}


void MultiHeaderTable::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);  // Стандартное поведение

    if (customHeader != nullptr){
        int tableWidth = mainTable->viewport()->width();
        int customHeaderWidth = customHeader->width();
        if (tableWidth != customHeaderWidth){
            customHeader->setFixedWidth(tableWidth);
            resizeColumnDeal();
        }
    }
}

void MultiHeaderTable::syncHeaderWithTable(QScrollArea *scrollArea) {

    QHeaderView *tableHeader = mainTable->horizontalHeader();
    QGridLayout *headerLayout = qobject_cast<QGridLayout *>(customHeader->layout());

    if (!tableHeader || !headerLayout) return;

    mainTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);



    connect(mainTable->horizontalHeader(), &QHeaderView::sectionMoved, this, [=]() {
        QScrollBar *headerScrollBar = scrollArea->horizontalScrollBar();
        int offset = mainTable->horizontalHeader()->offset();  // Текущее смещение заголовка таблицы

        headerScrollBar->setValue(offset);
    });

    connect(mainTable->horizontalScrollBar(), &QScrollBar::valueChanged, this, [=]() {
        QScrollBar *headerScrollBar = scrollArea->horizontalScrollBar();
        int offset = mainTable->horizontalHeader()->offset();

        headerScrollBar->setValue(offset);
    });

    // Сигнал для синхронизации ширины столбцов
    connect(tableHeader, &QHeaderView::sectionResized, this, [this](int logicalIndex, int oldSize, int newSize) {
        resizeColumnDeal();

    });


    // Устанавливаем начальную ширину столбцов
    resizeColumnDeal();
}


void MultiHeaderTable::processHeaders(const QJsonArray &headers, QGridLayout *layout, int row, int &col, const QFont &font, int maxDepth) {
    for (const QJsonValue &headerValue : headers) {
        QJsonObject headerObj = headerValue.toObject();
        QString text = headerObj["text"].toString();
        int colspan = headerObj["colspan"].toInt(1);
        int index = headerObj["index"].toInt(1);

        QLabel *headerLabel = new QLabel(text, this);

        // Применяем шрифт и задаём размер
        headerLabel->setFont(font);
        headerLabel->setWordWrap(true);
        headerLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        if (headerObj.contains("children")) {
            headerLabel->setStyleSheet("border-left: 1px solid lightgray; border-bottom: 1px solid lightgray; border-right: 1px solid lightgray; padding-left: 2px; padding-right: 2px; background-color: white;");
        } else {
            headerLabel->setStyleSheet("border-left: 1px solid lightgray; border-right: 1px solid lightgray; padding-left: 2px; padding-right: 2px; background-color: white;");
        }
        headerLabel->setAlignment(Qt::AlignCenter);
        headerLabel->setProperty("colspan", colspan);
        headerLabel->setProperty("index", index);

        // Рассчитываем rowSpan
        int rowSpan = 1;
        if (!headerObj.contains("children")) {
            rowSpan = maxDepth - row; // Занять всё оставшееся пространство
        }
        headerLabel->setMinimumHeight(40 * rowSpan);

        // Добавляем в layout
        layout->addWidget(headerLabel, row, col, rowSpan, colspan);

        // Сохраняем текущее значение col
        int currentCol = col;

        // Обрабатываем вложенные заголовки
        if (headerObj.contains("children")) {
            QJsonArray children = headerObj["children"].toArray();
            processHeaders(children, layout, row + 1, col, font, maxDepth);
        }

        // Восстанавливаем col после обработки детей
        col = currentCol + colspan;
    }
}

int MultiHeaderTable::calculateDepth(const QJsonArray &headers) {
    int maxDepth = 1;
    for (const QJsonValue &headerValue : headers) {
        QJsonObject headerObj = headerValue.toObject();
        if (headerObj.contains("children")) {
            int childDepth = calculateDepth(headerObj["children"].toArray());
            maxDepth = qMax(maxDepth, 1 + childDepth);
        }
    }
    return maxDepth;
}


void MultiHeaderTable::findText(const QString & text)
{
    QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)mainTable->model();

    int c = m_find_col;

    if (c >= 0) {
        model->setFilterColumn(v_col[c]);
        model->setFilterFlags(Qt::MatchContains);
        model->filter(text);
    }
}


void MultiHeaderTable::OnEdit()
{
    if (this->m_id <= 0) return;

    QString file_table = this->tn;

    if (this->m_dop != "") {
        file_table = QString("%1_%2").arg(this->tn, this->m_dop);
    }

    auto *dlg = new PropertyDial(this, *this->m_db, this->m_title, this->tn, file_table, this->m_id, -1, pr_type_any, nullptr);


    dlg->show();

    connect(dlg, SIGNAL(accepted()), this, SLOT(onPropertyAccepted()));
}

void MultiHeaderTable::OnAdd()
{
    QString q = QString("INSERT INTO %1 DEFAULT VALUES").arg(tbl_sql(this->tn));
    QSqlQuery query(*this->m_db);

//    int last_id = execInsertQ(*this->m_db, q);

    std::map<QString, QVariant> data;
    int last_id = insertIntoDatabase(*this->m_db, this->tn, data);


    if (last_id > 0) {
        QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)mainTable->model();
        model->select();

        int lastRow = model->rowCount() - 1;
        mainTable->selectRow(lastRow);  // Выделяет последнюю строку
        mainTable->scrollToBottom();     // Прокручивает к последней строке

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


}



void MultiHeaderTable::OnDelete()
{
    if (this->m_id <= 0) return;

    if (QMessageBox::question(0, "", QString("Удалить строку № %1 в таблице %2 ?").arg(this->m_id).arg(this->tn)) != QMessageBox::Yes) return;

    QString q = QString("DELETE FROM %1 where id=%2").arg(tbl_sql(this->tn)).arg(this->m_id);
    QSqlQuery query(*this->m_db);

    if (query_exec(*this->m_db, query, q)) {
        QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)mainTable->model();
        model->select();
    }

}

void MultiHeaderTable::OnOpres()
{
    if (this->m_id <= 0) return;

    GidWidget *gid = this->gid;
    if (!gid) return;

    gid->setOpres(m_opres, this->m_id);

}

void MultiHeaderTable::OnMoveTo()
{


    qDebug() << "move_to = " << this->move_to;
    QItemSelectionModel *model = mainTable->selectionModel();

    model->currentIndex();

    QModelIndex index = model->currentIndex();

    int cols = mainTable->model()->columnCount();

    int row = index.row();

    int id = mainTable->model()->data(mainTable->model()->index(row, 0)).toInt();


    if (this->tn == "") {
        // проверка остальных колонок
        for (int col = 0; col < cols; ++col) {
            QVariant val = mainTable->model()->data(mainTable->model()->index(row, col));

            if (val.typeId() == QMetaType::QString) {
                QString s = val.toString();
                if (s.startsWith("01") || s.startsWith("00")) {
                    QVariant parseWKB(const QString &hexWkb, int *sridOut = nullptr);
                    QPointF centroid(const QVariant &geom, bool *ok = nullptr);

                    bool ok;
                    QVariant geom = parseWKB(s);
                    QPointF pt = centroid(geom, &ok);

                    if (ok) {
                        //qDebug() << QString("pt %1 %2").arg(pt.x()).arg(pt.y());
                        gid->movePoint(CFPoint(pt.x()*100, -pt.y()*100));
                        return;
                    }
                }
            }
        }
    }

    GidWidget *gid = this->gid;
    if (!gid) return;



    switch (this->move_to) {
    case move_to_node:
        gid->moveNode(id);
        break;

    case move_to_line:
        gid->moveLine(id);
        break;

    case move_to_line_big:
        break;
    case move_to_geo:
        gid->moveGeo(this->tn, id);
        break;
    case move_to_unknown:
        break;
    }
}

bool MultiHeaderTable::save_excel(const QString & filename)
{
    QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)mainTable->model();

    QString q = model->query().executedQuery();

    int h[256];

    QHeaderView *hv = mainTable->horizontalHeader();

    for (int c = 0; c < hv->count(); c++) {
        h[c] = mainTable->isColumnHidden(c);
    }

    return ::save_excel(filename, *m_db, this->tn, q, h);
}

void MultiHeaderTable::OnExcel()
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

void MultiHeaderTable::OnFind()
{
    QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)mainTable->model();

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

void MultiHeaderTable::OnFindAll()
{
    QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)mainTable->model();

    model->setFilterColumn(v_col[0]);
    model->setFilterFlags(Qt::MatchContains);
    model->filter("");
}

void MultiHeaderTable::OnAll()
{
    QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)mainTable->model();

    bool all = model->getAll();

    model->setAll(!all);
}


void MultiHeaderTable::OnHide()
{
    QHeaderView *hv = mainTable->horizontalHeader();
    ChMenuDial dlg(this, QObject::tr(""));

    for (int c = 0; c < hv->count(); c++) {
        QString name = mainTable->model()->headerData(c, Qt::Horizontal).toString();
        bool y = mainTable->isColumnHidden(c);
        if (!no_view(v_col[c], name, this->tn)) {
            dlg.Add(name, c, !y);
        }
    }

    if (dlg.exec()) {
        QSettings settings;

        for (int c = 0; c < hv->count(); c++) {
            QString name = mainTable->model()->headerData(c, Qt::Horizontal).toString();
            bool y = std::find(dlg.out.begin(),dlg.out.end(), c)  != dlg.out.end();

            mainTable->setColumnHidden(c, !y);

            settings.setValue(QString("DbWindow/%1/%2").arg(this->tn, v_col[c]), y);

        }
    }
}


void MultiHeaderTable::onPropertyAccepted()
{
    auto *dlg = qobject_cast<PropertyDial *>(sender());
    if (dlg) {
        if (update_db(dlg->map_edited, dlg->getUser())) {
            QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)mainTable->model();
            model->select();

            emit dataChanged();
        }
    }
}

void MultiHeaderTable::onPropertyRejected()
{
    auto *dlg = qobject_cast<PropertyDial *>(sender());
    if (dlg) {
        if (dlg->get_autodelete()) {
            QString q = QString("delete from %1 where id in (%2)").arg(tbl_sql(dlg->m_table)).arg(dlg->m_id1);
            query_exec(*this->m_db, q);

            QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)mainTable->model();
            model->select();
        }
    }
}

void MultiHeaderTable::OnMarkAccessibility(){

    GidWidget *gid = this->gid;
    if (!gid) return;

    gid->moveCurrent();
    QVector<int> ids;

    QAbstractItemModel* model = mainTable->model();
    if (!model) return;

    // 1) Находим индекс столбца "Режим" по заголовку
    int modeCol = -1;
    for (int c = 0; c < model->columnCount(); ++c) {
        QString h = model->headerData(c, Qt::Horizontal).toString().trimmed();
        if (h.compare(m_mark_column, Qt::CaseInsensitive) == 0) { modeCol = c; break; }
    }
    if (modeCol == -1) {
        // не нашли столбец "Режим" — можно выйти или выбрать запасной индекс
        // qWarning() << "Колонка 'Режим' не найдена";
        return;
    }

    // 2) Цвета
    const int COLOR_ABOVE = 0xFFA500; // выше нормы
    const int COLOR_BELOW = 0x0066FF; // ниже нормы
    const int COLOR_OTHER = 0xFF0000; // прочее/нет данных/исключение

    // 3) Собираем id->color и красим
    QHash<int,int> idToColor;
    const int rows = model->rowCount();

    for (int r = 0; r < rows; ++r) {
        int id = model->data(model->index(r, 0)).toInt();

        QString mode = model->data(model->index(r, modeCol)).toString().toLower().trimmed();
        int color = COLOR_OTHER; // по умолчанию красный

        // достаточно робастные проверки текста
        if (mode.contains("выше") || mode.contains("превыш")) {
            color = COLOR_ABOVE;
        } else if (mode.contains("ниже")) {
            color = COLOR_BELOW;
        }

        idToColor.insert(id, color);
    }

    // Если у тебя есть метод из прошлых сообщений:
    gid->applyNodeColors(idToColor);
}

void MultiHeaderTable::OnHide2()
{
    QHeaderView *hv = mainTable->horizontalHeader();
    ChMenuDial dlg(this, QObject::tr(""));
    QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)mainTable->model();

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
                params += QString("max(\"%1\")").arg(fieldName);
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
        bool y = mainTable->isColumnHidden(c);
        QString name = mainTable->model()->headerData(c, Qt::Horizontal).toString();

        mainTable->setColumnHidden(c, y || vv[c].isNull() || vv[c].toString() == "" || vv[c].toString() == "0" || no_view(v_col[c], name, this->tn)) ;
    }
#endif

//    QHeaderView *hv = mainTable->horizontalHeader();
//    ChMenuDial dlg(this, QObject::tr(""));


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

void MultiHeaderTable::reset_q()
{
    QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)mainTable->model();
    model->select();
}


void MultiHeaderTable::reset_q(const QString & q)
{
    QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)mainTable->model();
    model->setQuery0(q);
    model->select();
}

void MultiHeaderTable::set_hide(std::set<int> &h)
{
    for (auto &it : h) {
        mainTable->setColumnHidden(it, true);
    }
}

void MultiHeaderTable::saveSettings()
{
    if (this->closed) return;

    QHeaderView *hv = mainTable->horizontalHeader();

    QSettings settings;

    for (int c = 0; c < hv->count(); c++) {
        QString name = mainTable->model()->headerData(c, Qt::Horizontal).toString();
        int width = mainTable->columnWidth(c);
        if (width == 0) {
            settings.setValue(QString("DbWindow/%1/%2").arg(this->tn, v_col[c]), false);
        }
        qDebug() << v_col[c] << " " << width;
        settings.setValue(QString("DbWindow/%1/width/%2").arg(this->tn, v_col[c]), width);
    }
}

void MultiHeaderTable::closeTable()
{
    saveSettings();
    this->closed = true;
    main_window->removeWidgetFromCenter(this->parentWidget());
    qDebug() << "Закрылся";
}

void MultiHeaderTable::hideEvent(QHideEvent *event)
{
    saveSettings(); // Сохранение настроек перед скрытием

    if (mainTable) {
        mainTable->hide(); // Скрываем таблицу (вызовет её hideEvent автоматически)
    }

    // Вызываем hideEvent у текущего базового класса
    QWidget::hideEvent(event);
}

void MultiHeaderTable::showEvent(QShowEvent *event){
    if (mainTable) {
        mainTable->show();
    }


    QWidget::showEvent(event);
}


void MultiHeaderTable::mouseDoubleClickEvent(QMouseEvent *event)
{
    qDebug() << "move_to = " << this->move_to;
    if (event->button() == Qt::LeftButton) {

        if (this->move_to != move_to_unknown && this->gid) {
            OnMoveTo();
        }
    }
    // Вызываем базовый метод для MultiHeaderTable
    QWidget::mouseDoubleClickEvent(event);
}

void MultiHeaderTable::mousePressEvent(QMouseEvent *event)
{
    QSortFilterSqlQueryModel *model = (QSortFilterSqlQueryModel *)mainTable->model();

    if (event->button() == Qt::RightButton) {
        QModelIndex index = mainTable->currentIndex();
        int col = mainTable->columnAt(event->position().x());
        int row = mainTable->rowAt(event->position().y());

        this->m_id = -1;

        if (index.isValid()) {
            QModelIndex index2 = index.sibling(index.row(), 0);
            this->m_id = index2.data().toInt();
        }

        QMenu *menu = new QMenu( this);

        QAction *action = nullptr;

        action = new QAction(tr("Экспорт в MS Excel"),this);
        connect(action, SIGNAL(triggered()),this,SLOT(OnExcel()));
        menu->addAction(action);

        action = new QAction(tr("Скрыть/показать столбцы"),this);
        connect(action, SIGNAL(triggered()),this,SLOT(OnHide()));
        menu->addAction(action);

        action = new QAction(tr("Скрыть пустые столбцы"),this);
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

        }

        if (col > 0) {
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

        if (row >= 0 && col >= 0) {
            menu->addSeparator();
            action = new QAction(tr("Показать полный текст…"), this);
            // Сохраним адрес ячейки в data действия
            action->setData(QPoint(row, col));
            connect(action, SIGNAL(triggered()), this, SLOT(OnShowCellText()));
            menu->addAction(action);
        }


        menu->exec( QCursor::pos() );

        return;
    }
    else
        if (event->button() == Qt::LeftButton && mainTable->resizeColumn >= 0) {
            mainTable->resizing = true;
            mainTable->resizeStartPos = event->pos().x();
            mainTable->initialColumnWidth = mainTable->horizontalHeader()->sectionSize(mainTable->resizeColumn);
        }

    QWidget::mousePressEvent(event);
}



void MultiHeaderTable::OnShowCellText()
{
    QAction *act = qobject_cast<QAction*>(sender());
    if (!act) return;

    const QPoint rc = act->data().toPoint();
    const int row = rc.x();
    const int col = rc.y();

    QAbstractItemModel *model = mainTable->model();
    if (!model || row < 0 || col < 0) return;

    const QModelIndex idx = model->index(row, col);
    if (!idx.isValid()) return;

    // Берём DisplayRole, при пустоте пробуем EditRole
    QString text = model->data(idx, Qt::DisplayRole).toString();
    if (text.isEmpty())
        text = model->data(idx, Qt::EditRole).toString();

    // Заголовок поля — чтобы было понятно, что именно показываем
    const QString fieldName = model->headerData(col, Qt::Horizontal).toString();

    // Небольшое модальное окно
    QDialog dlg(this);
    dlg.setWindowTitle(tr("Полный текст: %1 (строка %2)")
                           .arg(fieldName)
                           .arg(row + 1));
    dlg.resize(600, 400);

    QVBoxLayout *layout = new QVBoxLayout(&dlg);

    QPlainTextEdit *viewer = new QPlainTextEdit(&dlg);
    viewer->setReadOnly(true);
    viewer->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    viewer->setPlainText(text);
    layout->addWidget(viewer);

    QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Close, &dlg);
    QPushButton *copyBtn = new QPushButton(tr("Копировать"), &dlg);
    bb->addButton(copyBtn, QDialogButtonBox::ActionRole);
    layout->addWidget(bb);

    // Копирование в буфер обмена
    connect(copyBtn, &QPushButton::clicked, [&](){
        QGuiApplication::clipboard()->setText(text);
    });
    connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    dlg.exec();
}

void MultiHeaderTable::mouseReleaseEvent(QMouseEvent *event) {
    QWidget::mouseReleaseEvent(event);

    if (mainTable->resizing && event->button() == Qt::LeftButton) {
        mainTable->resizing = false;
        setCursor(Qt::ArrowCursor);
    }
}

