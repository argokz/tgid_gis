#include <QtGui>
#include <QtSql>
#include  <QtWidgets>
#include <QInputDialog>

#include "TableWindow.h"
//#include "help.h"

#include <db/db.h>
#include <mystd.h>

#include <QString>

void setRowHeightForTwoLines(QTableView *tableView, int n) 
{
    QFont headerFont = tableView->horizontalHeader()->font();
    QFontMetrics fontMetrics(headerFont);
    int lineHeight = fontMetrics.lineSpacing();  // Высота одной строки
    int twoLineHeight = lineHeight * n;          // Высота для двух строк текста
    tableView->horizontalHeader()->setFixedHeight(twoLineHeight);
}

QString insertLineBreaks(const QString& text, int maxLineLength = 50) 
{
    QString result = "";
    int currentLineLength = 0;

    // Разбиваем текст на слова
    QStringList words = text.split(" ");
    for (const QString& word : words) {
        // Проверяем, может ли слово уместиться в текущую строку
        if (currentLineLength + word.length() > maxLineLength && result != "") {
            // Если нет, добавляем перенос строки и сбрасываем счетчик
            result += '\n';
            currentLineLength = 0;
        } else if (!result.isEmpty()) {
            // Добавляем пробел перед словом, если это не начало строки
            result += ' ';
            currentLineLength += 1;
        }

        // Добавляем слово и обновляем длину текущей строки
        result += word;
        currentLineLength += word.length();
    }

    return result;
}



TableWindow::TableWindow(QSqlDatabase &db, const QString & tn, const QString & title0, const QString & que0, QWidget *parent, int targetId, bool find_str, bool ok_str, bool is_header, bool autofit)
    : QDialog(parent)
    , save(this, "TableWindow", title0)
{

    QString que = que0;

    QString title = title0;

    check_db(db);

    this->setUpdatesEnabled(false);


//    model = new QSqlTableModel(this);
    model = new QSqlQueryModel(this);
//    model->setTable("Help_BP_OUT");

    m_label = title;

    if (que != "") {
        que = QString("SELECT * FROM (\n%1\n) __").arg(que);
    }
    else {
        que = QString("SELECT * FROM %1").arg(tbl_sql(tn));
    }

    qDebug() << que;

    model->setQuery(que, db);

    if (model->lastError().isValid()) {
//        qDebug() << model.lastError();

        QSqlError e = model->lastError();
        QString msg = QString("%1 - %2").arg(e.nativeErrorCode(), e.text());
        QInputDialog::getMultiLineText(nullptr, "title", msg, que);
    }


    if (title == "") {
        title = tn;
    }

//    QInputDialog::getMultiLineText(nullptr, "title", "label", que);


    m_db = &db;
    m_q = que;

    m_id = -1;

    QSqlRecord record = model->query().record();

    QString findColumnRusName(const QString & d, const QString & n1, const QString & n2);
//    QString findNewColumnName(const QString & tn, const QString & old_name);

    for (int i = 0; i < record.count(); i++) {
//        QSqlField field = record.field(i);
//        QString old_name = model->headerData(i, Qt::Horizontal).toString();
        QString col = record.fieldName(i);
        if (tn != "") {
            QString fn = findColumnRusName("gid", tn, col);
            fn = insertLineBreaks(fn, 45);
            model->setHeaderData(i, Qt::Horizontal, fn);
        }
    }    

    QHBoxLayout *findLayout = new QHBoxLayout;

    view = new QTableView;
    view->setModel(model);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    if (record.count() > 1) {
        view->setColumnHidden(0, true);
    }

    QHeaderView *header = view->horizontalHeader();

//    header->setFixedHeight(100);
    header->setDefaultAlignment(Qt::AlignCenter | (Qt::Alignment)Qt::TextWordWrap);
    header->setVisible(is_header);

    view->verticalHeader()->setVisible(false);

    if (autofit && is_header) {
//        header->setSectionResizeMode(QHeaderView::ResizeToContents);
    }


    QHBoxLayout *buttonBox = new QHBoxLayout;


    if (ok_str) {
    }

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);

    if (find_str) {
        QLabel *label = new QLabel(tr("Поиск"));
        lineEdit = new QLineEdit;
        label->setBuddy(lineEdit);
        findLayout->addWidget(label);
        findLayout->addWidget(lineEdit);
        
        mainLayout->addLayout(findLayout);

        connect(lineEdit, SIGNAL(textChanged(const QString &)),
                this, SLOT(textChanged(const QString &)));
    }
    
    mainLayout->addWidget(view);

    if (ok_str) {
        QPushButton *okButton = new QPushButton(tr("Выбрать"));
        okButton->setDefault(true);
        okButton->setEnabled(true);
        okButton->setProperty("dialog", "property");
        okButton->setProperty("class", "ok");

        QPushButton *closeButton = new QPushButton(tr("Закрыть"));
    //    QPushButton *helpButton = new QPushButton(tr("Помощь"));
        closeButton->setProperty("dialog", "property");
        closeButton->setProperty("class", "cancel");

        buttonBox->setSpacing(10);
        buttonBox->setContentsMargins(10,10,10,10);
        buttonBox->addStretch();
    //    buttonBox->addWidget(helpButton);
        buttonBox->addWidget(okButton);
        buttonBox->addWidget(closeButton);

    //    connect(helpButton, SIGNAL(clicked()), this, SLOT(helpClicked()));
        connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
        connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
        
        mainLayout->addLayout(buttonBox);
    }

    setLayout(mainLayout);

    setWindowTitle(title);

    if (find_str) {
    }

//    connect(view, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClicked(QModelIndex)));
//    connect(view, SIGNAL(clicked(QModelIndex)), this, SLOT(clicked(QModelIndex)));

    connect(view, SIGNAL(activated(QModelIndex)), this, SLOT(activated(QModelIndex)));

//    connect(view, SIGNAL(selectRow(int)), this, SLOT(selectRow(int)));


#if 0
    int rowToSelect = -1;
    int left = 0;
    int right = model->rowCount() - 1;

    // Бинарный поиск по id
    while (left <= right) {
        int mid = left + (right - left) / 2;
        QModelIndex midIndex = model->index(mid, 0); // Предполагаем, что id находится в первом столбце
        int currentId = model->data(midIndex).toInt();

        if (currentId == targetId) {
            rowToSelect = mid;
            break;
        }
        else if (currentId < targetId) {
            left = mid + 1;
        }
        else {
            right = mid - 1;
        }
    }
#endif


    QSettings settings;

    QString fontFamily = settings.value("property/fontFamily", "Arial").toString();
    int fontSize = settings.value("property/fontSize", 12).toInt();

    QString style = readQSS("qss2/table_dial.qss");
    QString st = QString("\nQTableView {font-size: %1px; font-family: %2})").arg(fontSize).arg(fontFamily);
    style += st;
    view->setStyleSheet(style);

    QTimer::singleShot(150, [=]() {
        if (is_header && model->columnCount() > 2) {
            setRowHeightForTwoLines(view, 4);
        }
        view->resizeColumnsToContents();
//        view->resizeRowsToContents();



        int columnCount = model->columnCount();

        if (columnCount > 2) {
            QSettings settings;

            for (int i = 0; i < record.count(); i++) {
                int w = settings.value(QString("TableWindow/%1/width/%2").arg(this->m_label).arg(i), -1).toInt();
                if (w > 0) {
                    view->setColumnWidth(i, w);
                }
            }
        }
        adjustColumnWidths();

        int width = view->verticalHeader()->width() + view->horizontalHeader()->length() + 10;
        int height = view->horizontalHeader()->height() + view->verticalHeader()->length() + 5;

        QSize screenSize = QGuiApplication::primaryScreen()->availableGeometry().size();
        int screenWidth = screenSize.width();
        int screenHeight = screenSize.height();

        width = std::min(width, screenWidth*3/4);

        if (height > screenHeight/2) {
            height = screenHeight/2;
            width += view->verticalScrollBar()->width() + 4;
        }

        view->setMinimumSize(width, height);
        view->resize(width, height);

        adjustSize();

        view->setMinimumSize(0, 0);

        int rowToSelect = -1;

        // Проходим по всем строкам, чтобы найти строку с нужным id
        for (int row = 0; row < model->rowCount(); ++row) {
            QModelIndex index = model->index(row, 0); // Предполагаем, что id находится в первом столбце
            if (model->data(index).toInt() == targetId) {
                rowToSelect = row;
                break;
            }
        }


        // Если нужная строка найдена, выделяем её
        if (rowToSelect != -1) {
            QModelIndex targetIndex = model->index(rowToSelect, 1); // Можно выбрать любой столбец в этой строке
            view->setCurrentIndex(targetIndex);
            view->selectionModel()->select(targetIndex, QItemSelectionModel::Select | QItemSelectionModel::Rows | QItemSelectionModel::ClearAndSelect);
            view->setFocus();
        }


    });


    this->setUpdatesEnabled(true);
}


void TableWindow::helpClicked()
{
/*
  if (curent && curent->objectName () != "") {
    HelpStr *hs = getHelp(tn, curent->objectName ());
    if (hs) {
      QMessageBox::information(this, tr("Help"), hs->HelpShiftF1);
    }
    curent->setFocus(Qt::MouseFocusReason);
  }
*/
  QMessageBox::warning(this, tr("!"), "!");
}

void TableWindow::okClicked()
{
    QModelIndex index = view->currentIndex();

    if (!index.isValid()) {
        QMessageBox::warning(this, tr("!"),tr("Выберите строку"));
    }
    else {
        QModelIndex index2 = index.sibling(index.row(), 0);
        m_id =  index2.data().toInt();

        QSqlRecord record = model->query().record();

        for (int i = 0; i < index.model()->columnCount(); i++) {
            QModelIndex index2 = index.sibling(index.row(), i);
            m_v.push_back(index2.data());
            m_map_v[record.fieldName(i)] = index2.data();
        }

        done(QDialog::Accepted);
    }
}

bool is_POSTGRESQL();

void TableWindow::textChanged(const QString & txt)
{
//    QString que = m_q + QString(" AND (CONCAT(ec.name, ' ', n.externalNodeName) LIKE N'%%%1%%' OR n.nodeName LIKE N'%%%1%%')").arg(txt);
//    QString que = m_q + QString(" AND (CONCAT(ec.name, ' ', n.externalNodeName) LIKE N'%%%1%%' OR n.nodeName LIKE N'%%%1%%')").arg(txt);

    QString que = m_q;

    if (txt != "") {
        if (is_POSTGRESQL()) {
            que = que.replace("--AND_NODE",  QString(" AND (CONCAT(ec.name, ' ', n.externalNodeName) ILIKE '%%%1%%' OR n.nodeName ILIKE '%%%1%%')").arg(txt));
            que = que.replace("--AND_PS",  QString(" AND n.nodeName ILIKE '%%%1%%'").arg(txt));

            que = que.replace("--AND_LINE",  QString(" AND (CONCAT(ec1.name, ' ', n1.externalNodeName) ILIKE '%%%1%%' OR (CONCAT(ec2.name, ' ', n2.externalNodeName) ILIKE '%%%1%%')").arg(txt));
        }
        else {
            que = que.replace("--AND_NODE",  QString(" AND (CONCAT(ec.name, ' ', n.externalNodeName) LIKE N'%%%1%%' OR n.nodeName LIKE N'%%%1%%')").arg(txt));
            que = que.replace("--AND_PS",  QString(" AND n.nodeName LIKE N'%%%1%%'").arg(txt));
        }

        if (que.indexOf("--AND_TXT") != -1) {
            que = que.replace("--AND_TXT",  "");
            que = que.replace("$txt$",  txt);
        }
    }

//    QInputDialog::getMultiLineText(nullptr, "title", "label", que);

    qDebug() << que;

    model->setQuery(que, *m_db);
}


void TableWindow::activated(const QModelIndex &index)
{
    emit okClicked();
}

void TableWindow::hideEvent(QHideEvent *event)
{    
    save.save();

    QSettings settings;

    QSqlRecord record = model->query().record();

    for (int i = 0; i < record.count(); i++) {
//        QString name = view->model()->headerData(c, Qt::Horizontal).toString();
        int width = view->columnWidth(i);
//        if (width == 0) {
//            settings.setValue(QString("DbWindow/%1/%2").arg(this->tn, v_col[c]), false);
//        }
        settings.setValue(QString("TableWindow/%1/width/%2").arg(m_label).arg(i), width);
    }

    QDialog::hideEvent(event);
}


QVariant TableWindow::value(const QString & fn)
{
    QSqlRecord record = model->query().record();

    for (int i = 0; i < record.count(); i++) {
        if (record.fieldName(i).toLower() == fn.toLower()) {
            return m_v[i];
        }
    }
    return QVariant();
}


void TableWindow::hide_column(int id)
{
    view->setColumnHidden(id,true);
}

void TableWindow::adjustColumnWidths()
{
    int columnCount = model->columnCount();
    int availableWidth = view->viewport()->width();  // Доступная ширина внутри виджета

    if (columnCount == 2) {
        view->setColumnWidth(1, availableWidth);
    }
    if (columnCount == 1) {
        view->setColumnWidth(0, availableWidth);
    }
}

void TableWindow::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    adjustColumnWidths();
}
