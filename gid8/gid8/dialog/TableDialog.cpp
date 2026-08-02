#include <QtGui>
#include <QtWidgets>
#include <QtSql>

#include "TableDialog.h"


TableDialog::TableDialog(const QString & title, QWidget *parent)
    :  QDialog(parent) 
    , save(this, "TableDialog", title)
    
    {

    resize(400, 300);

    // Создаем таблицу
//    tableWidget = new QTableWidget(5, 3, this); // 5 строк и 3 столбца
    tableWidget = new QTableWidget(this);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Создаем кнопку для подтверждения выбора
    QPushButton *okButton = new QPushButton("OK", this);
    okButton->setDefault(true);
    okButton->setEnabled(true);

    QPushButton *closeButton = new QPushButton(tr("Отменить"));


    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));


    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(tableWidget);

/*
    // Создаем компоновку для диалогового окна
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(tableWidget);
//    layout->addWidget(okButton);
*/

    QVBoxLayout *buttonBox = new QVBoxLayout;

    buttonBox->addWidget(okButton);
    buttonBox->addWidget(closeButton);
    buttonBox->addStretch();

/*
    layout->addLayout(buttonBox);
    setLayout(layout);
*/

    //==============================
    QHBoxLayout *mainLayout = new QHBoxLayout;

    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(buttonBox);

    setLayout(mainLayout);

    //==============================



    setWindowTitle(title);
    m_title = title;

    // Обрабатываем нажатие на кнопку
    connect(okButton, &QPushButton::clicked, this, &TableDialog::selectRow);

    connect(tableWidget, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(tableItemClicked(int,int)));

    adjustColumnWidths();

}



void TableDialog::addColumn(const QString & col)
{

    int c = tableWidget->columnCount();

    tableWidget->insertColumn(c);

    QTableWidgetItem *headerItem = new QTableWidgetItem(col);
    tableWidget->setHorizontalHeaderItem(c, headerItem);
}


int TableDialog::addRow()
{
    int r = tableWidget->rowCount();

    tableWidget->insertRow(r);
    return r;
}

void TableDialog::setId(int row, QVariant id)
{
    mapId[row] = id;
}


void TableDialog::setValue(int row, int col, const QString & val)
{
    QTableWidgetItem *item = new QTableWidgetItem(val);
    tableWidget->setItem(row, col, item);
}

void TableDialog::selectRow() 
{
    int selectedRow = tableWidget->currentRow();
    if (selectedRow >= 0) {
/*
        QString selectedText;
        for (int column = 0; column < tableWidget->columnCount(); ++column) {
            selectedText += tableWidget->item(selectedRow, column)->text() + " ";
        }
        qDebug() << "Selected row:" << selectedText;
*/
        auto it = mapId.find(selectedRow);
        if (it != mapId.end()) {
            m_id = it->second;
        }

    }
    accept(); // Закрываем диалоговое окно
}

/*
void TableDialog::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (this->move_to != move_to_unknown && this->gid) {
            OnMoveTo();
        }
    }

    QDialog::mouseDoubleClickEvent(event);
}

*/

void TableDialog::setCurrentRow(int row)
{
    tableWidget->setCurrentCell(row, 0);
}


void TableDialog::tableItemClicked(int row, int col)
{
    selectRow();
}


void TableDialog::hideEvent(QHideEvent *event)
{    

    QSettings settings;

    for (int c = 0; c < tableWidget->columnCount(); c++) {
        int width = tableWidget->columnWidth(c);
        if (width == 0) {
            settings.setValue(QString("TableDialog/%1/%2").arg(m_title).arg(c), false);
        }
        settings.setValue(QString("TableDialog/%1/width/%2").arg(m_title).arg(c), width);
    }

    QDialog::hideEvent(event);
}



void TableDialog::init()
{
    QSettings settings;

    for (int c = 0; c < tableWidget->columnCount(); c++) {
        int y = settings.value(QString("TableDialog/%1/%2").arg(m_title).arg(c), true).toBool();
        int width = settings.value(QString("TableDialog/%1/width/%2").arg(m_title).arg(c), -1).toInt();
        tableWidget->setColumnHidden(c, !y);
        if (width > 0) {
            tableWidget->setColumnWidth(c, width);
        }
    }
}

void TableDialog::adjustColumnWidths()
{
    int columnCount = tableWidget->columnCount();
    int availableWidth = tableWidget->viewport()->width();  // Доступная ширина внутри виджета

    if (columnCount == 1) {
        tableWidget->setColumnWidth(0, availableWidth);
    }
}


void TableDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    adjustColumnWidths();
}
