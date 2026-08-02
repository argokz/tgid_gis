#include "TuQDialog.h"



void TuQDialog::addAllCheckBox(QVBoxLayout *column1Layout, QListWidget *listWidget1)
{
    QCheckBox *selectAllCheckBox1 = new QCheckBox("Все", this);
    column1Layout->addWidget(selectAllCheckBox1);
    selectAllCheckBox1->setCheckState(Qt::Checked); 

    connect(selectAllCheckBox1, &QCheckBox::stateChanged, this,
            [listWidget1](int state) {
                for (int i = 0; i < listWidget1->count(); ++i) {
                    QListWidgetItem *item = listWidget1->item(i);
                    item->setCheckState(state == Qt::Checked ? Qt::Checked : Qt::Unchecked);
                }
            });
}


TuQDialog::TuQDialog(QWidget *parent)
    : QDialog(parent)
    , save(this, "TuQDialog")
{
    setWindowTitle("Выберите критерии формирования отчета");

    // Основной layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5); // Уменьшенные отступы
    mainLayout->setSpacing(5); // Уменьшенное расстояние

    // Создаем QSplitter для изменения ширины столбцов
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setHandleWidth(2); // Уменьшенная ширина разделителя

    // Создаем виджеты для каждого столбца
    QWidget *column1Widget = new QWidget(this);
    QVBoxLayout *column1Layout = new QVBoxLayout(column1Widget);
    column1Layout->setContentsMargins(2, 2, 2, 2); // Уменьшенные отступы
    column1Layout->setSpacing(2); // Уменьшенное расстояние
    QLabel *header1 = new QLabel("Источники тепла", this);
    listWidget1 = new QListWidget(this);
    column1Layout->addWidget(header1);

    addAllCheckBox(column1Layout, listWidget1);

    column1Layout->addWidget(listWidget1);

    QWidget *column2Widget = new QWidget(this);
    QVBoxLayout *column2Layout = new QVBoxLayout(column2Widget);
    column2Layout->setContentsMargins(2, 2, 2, 2);
    column2Layout->setSpacing(2);
    QLabel *header2 = new QLabel("Районы эксплаутации", this);
    listWidget2 = new QListWidget(this);
    column2Layout->addWidget(header2);

    addAllCheckBox(column2Layout, listWidget2);
    
    column2Layout->addWidget(listWidget2);

    QWidget *column3Widget = new QWidget(this);
    QVBoxLayout *column3Layout = new QVBoxLayout(column3Widget);
    column3Layout->setContentsMargins(2, 2, 2, 2);
    column3Layout->setSpacing(2);
    QLabel *header3 = new QLabel("Годы", this);
    listWidget3 = new QListWidget(this);
    column3Layout->addWidget(header3);

    addAllCheckBox(column3Layout, listWidget3);
    
    column3Layout->addWidget(listWidget3);

    QWidget *column4Widget = new QWidget(this);
    QVBoxLayout *column4Layout = new QVBoxLayout(column4Widget);
    column4Layout->setContentsMargins(2, 2, 2, 2);
    column4Layout->setSpacing(2);
    QLabel *header4 = new QLabel("Состояние", this);
    listWidget4 = new QListWidget(this);
    column4Layout->addWidget(header4);
    column4Layout->addWidget(listWidget4);

    // Добавляем столбцы в splitter
    splitter->addWidget(column1Widget);
    splitter->addWidget(column2Widget);
    splitter->addWidget(column3Widget);
    splitter->addWidget(column4Widget);

    // Устанавливаем начальные размеры столбцов
    splitter->setSizes({150, 150, 150, 150});

    // Заполняем списки начальными данными (код, текст)
//    setupList(listWidget1, {{1001, "Задача 1"}, {1002, "Задача 2"}, {1003, "Задача 3"}});
//    setupList(listWidget2, {{2001, "Проект A"}, {2002, "Проект B"}, {2003, "Проект C"}, {2004, "Проект D"}, {2005, "Проект E"}});
//    setupList(listWidget3, {{3001, "Элемент X"}, {3002, "Элемент Y"}});
//    setupList(listWidget4, {{4001, "Пункт 1"}, {4002, "Пункт 2"}, {4003, "Пункт 3"}, {4004, "Пункт 4"}});

    // Создаем кнопки
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // Добавляем все в главный layout
    mainLayout->addWidget(splitter);
    mainLayout->addWidget(buttonBox);

    // Устанавливаем минимальный размер окна
//    resize(600, 400);
}

void TuQDialog::setupList(QListWidget *listWidget, const QList<QPair<int, QString>> &items)
{
    // Заполняем список элементами с чекбоксами
    for (const QPair<int, QString> &item : items) {
        addItemToList(listWidget == listWidget1 ? 0 : listWidget == listWidget2 ? 1 : listWidget == listWidget3 ? 2 : 3, item.first, item.second, false);
    }
}

void TuQDialog::addItemToList(int listIndex, int id, const QString &text, bool checked)
{
    QListWidget *targetList;
    switch (listIndex) {
    case 0: targetList = listWidget1; break;
    case 1: targetList = listWidget2; break;
    case 2: targetList = listWidget3; break;
    case 3: targetList = listWidget4; break;
    default: return; // Некорректный индекс
    }

    QListWidgetItem *item = new QListWidgetItem(text);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // Включаем чекбоксы
    item->setCheckState(checked ? Qt::Checked : Qt::Unchecked); // Устанавливаем состояние чекбокса
    item->setData(Qt::UserRole, id); // Сохраняем код элемента
    targetList->addItem(item);
}

QMap<int, QMap<int, bool>> TuQDialog::getCheckedStates() const
{
    QMap<int, QMap<int, bool>> states;
    QList<QListWidget*> lists = {listWidget1, listWidget2, listWidget3, listWidget4};

    for (int col = 0; col < lists.size(); ++col) {
        QMap<int, bool> columnStates;
        for (int row = 0; row < lists[col]->count(); ++row) {
            QListWidgetItem *item = lists[col]->item(row);
            columnStates[item->data(Qt::UserRole).toInt()] = item->checkState() == Qt::Checked;
        }
        states[col] = columnStates;
    }
    return states;
}


QMap<int, QMap<QString, bool>> TuQDialog::getCheckedStatesS() const
{
    QMap<int, QMap<QString, bool>> states;
    QList<QListWidget*> lists = {listWidget1, listWidget2, listWidget3, listWidget4};

    for (int col = 0; col < lists.size(); ++col) {
        QMap<QString, bool> columnStates;
        for (int row = 0; row < lists[col]->count(); ++row) {
            QListWidgetItem *item = lists[col]->item(row);
            columnStates[item->text()] = item->checkState() == Qt::Checked;
        }
        states[col] = columnStates;
    }
    return states;
}


QMap<int, bool> TuQDialog::getCheckedStates(int col) const
{
    QList<QListWidget*> lists = {listWidget1, listWidget2, listWidget3, listWidget4};

    QMap<int, bool> columnStates;
    for (int row = 0; row < lists[col]->count(); ++row) {
        QListWidgetItem *item = lists[col]->item(row);
        columnStates[item->data(Qt::UserRole).toInt()] = item->checkState() == Qt::Checked;
    }

    return columnStates;
}



QMap<QString, bool> TuQDialog::getCheckedStatesS(int col) const
{
    QList<QListWidget*> lists = {listWidget1, listWidget2, listWidget3, listWidget4};

    QMap<QString, bool> columnStates;
    for (int row = 0; row < lists[col]->count(); ++row) {
        QListWidgetItem *item = lists[col]->item(row);
        columnStates[item->text()] = item->checkState() == Qt::Checked;
    }

    return columnStates;
}

QSet<int> TuQDialog::getCheckedStates2(int col) const
{
    QList<QListWidget*> lists = {listWidget1, listWidget2, listWidget3, listWidget4};

    QSet<int> columnStates;
    for (int row = 0; row < lists[col]->count(); ++row) {
        QListWidgetItem *item = lists[col]->item(row);
        if (item->checkState() == Qt::Checked) {
            columnStates.insert(item->data(Qt::UserRole).toInt());
        }
    }

    return columnStates;
}

QSet<QString> TuQDialog::getCheckedStatesS2(int col) const
{
    QList<QListWidget*> lists = {listWidget1, listWidget2, listWidget3, listWidget4};

    QSet<QString> columnStates;
    for (int row = 0; row < lists[col]->count(); ++row) {
        QListWidgetItem *item = lists[col]->item(row);
        if (item->checkState() == Qt::Checked) {
            columnStates.insert(item->text());
        }
    }

    return columnStates;
}



QString transl(const QString & s);


QString TuQDialog::getQ() const
{
    QSet<QString> l_ist = getCheckedStatesS2(0);
    QSet<QString> l_rai = getCheckedStatesS2(1);
    QSet<int> l_y = getCheckedStates2(2);
    QSet<int> l_st = getCheckedStates2(3);
    QMap<int, bool> l_s = getCheckedStates(3);

    QString q1 = "";
    QString q2 = "";
    QString q3 = "";

    for (const QString &s : l_ist) {
        QString q = QString("%1 = '%2'").arg(transl("Источник"), s);
        if (s == "Не заданный") {
            q = QString("%1 IS NULL OR %1=''").arg(transl("Источник"));
        }

        if (q1 != "") q1 += " OR ";
        q1 += q;
    }

    for (const QString &s : l_rai) {
        QString q = QString("%1 = '%2'").arg(transl("Район эксплуатации"), s);
        if (s == "Не заданный") {
            q = QString("%1 IS NULL OR %1=''").arg(transl("Район эксплуатации"));
        }
        if (q2 != "") q2 += " OR ";
        q2 += q;
    }

    for (const int &y : l_y) {
        QString q = QString("DATE_PART('year', %1)::int = %2").arg(transl("Дата выдачи ТУ")).arg(y);
        if (y == 0) {
//            q = QString("%1 IS NULL OR %1=''").arg(transl("Дата выдачи ТУ"));
            q = QString("%1 IS NULL").arg(transl("Дата выдачи ТУ"));
        }
        if (q3 != "") q3 += " OR ";
        q3 += q;
    }

    bool check_d = l_s.value(0);
    bool check_z = l_s.value(1);
    bool check_a = l_s.value(2);

    QString qq = "";

    if (check_a) {
        if (qq != "") qq += " OR ";
        QString s = QString("%1 IN (2,3)").arg(transl("Состояние договора"));
        qq += s;
    }
    if (check_d) {
        if (qq != "") qq += " OR ";
        QString s = QString("%1 IN (1)").arg(transl("Состояние договора"));
        qq += s;
    }
    if (check_z) {
        if (qq != "") qq += " OR ";
        QString s = QString("%1 IN (4)").arg(transl("Состояние договора"));
        qq += s;
    }

    QString q = "FALSE";

    if (q1 == "" || q2 == "" || q3 == "" || qq == "") {
        q = "FALSE";
    }
    else {
        q = QString("(%1)\nAND (%2)\nAND (%3)\nAND (%4)").arg(q1, q2, q3, qq);
    }

    return q;
}
