#include "typepumpdialog.h"
#include "ui_typepumpdialog.h"
#include "db/DataRepository.h"
#include "pumps/PumpsHelper.h"
#include "pumps/pumpgraphwidget.h"
#include "ui/Nas_charDialog.h"
#include <mystd.h>

TypePumpDialog::TypePumpDialog(QWidget *parent, QSqlDatabase &db, 
    int initPumpId, 
    int initCountPumps
    
    , int rotorDiameterTypeID, double _diam_zad, int driveTypeID, double  _rate_zad
    
    
    )
    : QDialog(parent)
    , ui(new Ui::TypePumpDialog)
    , save(this, "TypePumpDialog")
{
    m_db = &db;
    m_id = initPumpId;
    if (initCountPumps>1)
        count_pumps = initCountPumps;

//    , int rotorDiameterTypeID, 
    d_zad = _diam_zad;
    rate_zad = _rate_zad;


    diam_b = rotorDiameterTypeID == 2;  
    rate_b = driveTypeID == 2;  

    ui->setupUi(this);

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
    connect(ui->button_next, SIGNAL(clicked()), this, SLOT(nextStepClicked()));
    connect(ui->button_cancel, SIGNAL(clicked()), this, SLOT(close()));
    updateComboBox(ui->comboBox, m_id);
}

void TypePumpDialog::currentIndexChanged(int index){
    int id = PumpsHelper::getIdFromIndex(ui->comboBox, index);
    PumpsHelper::clearAllTabs(ui->tabWidget);
    QString q = readQUTF8("sql/nasos_list1.sql");
    q.replace("$id$", QString::number(id));
    addTableTab(q, "Параметры");


    q = readQUTF8("sql/nasos_list.sql");
    q.replace("$id$", QString::number(id));
    addTableTab(q, "Характеристики");
    addGraphTab("График", id);
}

void TypePumpDialog::onInnerDialogAccepted() {
    // Закрываем TypePumpDialog с результатом Accepted
    this->accept();
}

void TypePumpDialog::nextStepClicked(){
    int id = PumpsHelper::getIdFromIndex(ui->comboBox, ui->comboBox->currentIndex());
    CNas_charDialog dlg(this, *m_db, id);

    dlg.m_strEdit_p = "0";
    dlg.m_strEdit_g = "1";
    dlg.m_strEdit_s = "0";

    dlg.m_strEdit_kol = QString::number(count_pumps);
    dlg.m_strEdit_dnom = QString::number(d_nomin);  // Номинальный, мм
    dlg.m_strEdit_dzad = QString::number(d_zad);  // Новое значение диаметра, мм

    dlg.m_strEdit_fnom = QString::number(rate_nomin);  // Номинальная, об/мин
    dlg.m_strEdit_fzad = QString::number(rate_zad);  // Новое значение частоты, об/мин

    dlg.m_radioFunction = 0; // Функция (Аппроксимация/Расчет)
    dlg.m_radioDrive = rate_b ? 1 : 0; // Привод (Нерегулируемый/Частотно регулируемый)
    dlg.m_radioWheel = diam_b ? 1 : 0; // Колесо (Номинальное/Срезанное)
    dlg.m_radioResistance = 1; // Гидравлическое сопротивление обвязки (Учитывать при расчете/Не учитывать)

    dlg.write();

    connect(&dlg, &CNas_charDialog::dialogAccepted, this, &TypePumpDialog::onInnerDialogAccepted);


    if (dlg.exec()){
        pReport = dlg.pReport;
    }
}

void TypePumpDialog::updateComboBox(QComboBox* combo, int selectedId) {
    try {
        DataRepository repository(m_db);  // m_db — указатель на вашу базу данных
        auto items = repository.getNasTypes();

        PumpsHelper::populateComboBox(combo, items, selectedId);
    } catch (const std::exception& ex) {
        qDebug() << "TypePumpDialog: Failed to update combo box:" << ex.what();

    }
}

void TypePumpDialog::addTableTab(const QString& queryStr, QString tabName) {
    // Создаем таблицу
    QTableWidget* table = new QTableWidget(ui->tabWidget);

    try {
        qDebug() << "mySql " << queryStr;
        QSqlQuery query(*m_db);
        if (!query.exec(queryStr)) {
            throw std::runtime_error("TypePumpDialog: Query failed: " + query.lastError().text().toStdString());
        }

        // Получаем количество колонок
        int columnCount = query.record().count();
        table->setColumnCount(columnCount);

        // Устанавливаем заголовки колонок
        QStringList headers;
        for (int i = 0; i < columnCount; ++i) {
            headers << query.record().fieldName(i);
        }
        table->setHorizontalHeaderLabels(headers);

        // Настраиваем заголовок таблицы
        QHeaderView* header = table->horizontalHeader();
        header->setStretchLastSection(true);                // Растягиваем последнюю колонку
        header->setStretchLastSection(false);                // Растягиваем последнюю колонку
//        header->setSectionResizeMode(QHeaderView::Stretch); // Растягиваем колонки по ширине

        header->setSectionResizeMode(QHeaderView::ResizeToContents);

        table->verticalHeader()->setVisible(false);         // Скрываем вертикальный заголовок
        table->setAlternatingRowColors(true);               // Чередование цветов строк

        // Устанавливаем стиль
        table->setStyleSheet(
            "QTableWidget {"
            "    border: 1px solid #ccc;"
            "    gridline-color: #ddd;"
            "    font-size: 14px;"
            "}"
            "QHeaderView::section {"
            "    background-color: #f2f2f2;"
            "    color: #333;"
            "    font-weight: bold;"
            "    border: 1px solid #ddd;"
            "}"
            );

        // Заполняем данные
        int row = 0;
        while (query.next()) {
            table->insertRow(row);
            for (int col = 0; col < columnCount; ++col) {
                QTableWidgetItem* item = new QTableWidgetItem(query.value(col).toString());
                table->setItem(row, col, item);
            }

            if (query.value(0).toInt() == 8) {
                d_nomin = query.value(2).toDouble();
            }
            else if (query.value(0).toInt() == 9) {
                rate_nomin = query.value(2).toDouble();
            }

            ++row;
        }
    } catch (const std::exception& ex) {
        qDebug() << "Error adding table tab:" << ex.what();
        return;
    }



    // Убираем возможность редактирования данных
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Добавляем таблицу во вкладку
    ui->tabWidget->addTab(table, tabName);
}

void TypePumpDialog::addGraphTab(const QString& title, int id) {
    QSqlQuery query(*m_db);
    // Создаем виджет для рисования
    PumpGraphWidget* paintWidget = new PumpGraphWidget(ui->tabWidget,query,id);

    // Добавляем его в tabWidget
    ui->tabWidget->addTab(paintWidget, title);
}

TypePumpDialog::~TypePumpDialog()
{
    delete ui;
}
