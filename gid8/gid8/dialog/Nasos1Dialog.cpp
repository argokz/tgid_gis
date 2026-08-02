#include <QtGui>
#include <QtWidgets>
#include <QtSql>

#include <table/DbWindow.h>
#include <dialog/SaveGeometry.h>

#include "std.h"

#include <db/db.h>

#include "Nasos1Dialog.h"

#include <mystd.h>


Nasos1Dialog::Nasos1Dialog(QWidget *parent, QSqlDatabase &db, int id)
    : QDialog(parent)
    , save(this, "Nasos1Dialog")
{
    m_db = &db;
    m_id = id;

//  QPushButton *closeButton = new QPushButton(tr("Отменить"));

//  QLineEdit *lineEdit;
//    QHBoxLayout *rowLayout;

    QVBoxLayout *mainLayout = new QVBoxLayout;

    QHBoxLayout *topBox = new QHBoxLayout;

    topBox->addWidget(new QLabel(tr("Внимание ! Все изменения будут внесены в\nбазу данных только после нажатия кнопки \"Добавить\"!")));
    topBox->addWidget(new QLabel(tr("Укажите тип насоса:")));

    combo = new QComboBox();

    QSqlQuery query(*m_db);
    query.setForwardOnly(true);

    QString q = "SELECT id, tip_nas FROM standardPumps ORDER BY tip_nas";

    if (query_exec(*m_db, query, q)) {
        int i = 0;
        while (query.next()) {
            int idd = query.value("id").toInt();
            QString s = query.value("tip_nas").toString();
            combo->addItem(s, idd);

            if (id == idd) {
                combo->setCurrentIndex(i);
            }
            i ++;

        }
    }

    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));

    topBox->addWidget(combo);
    mainLayout->addLayout(topBox);

    {
        QVBoxLayout *tbBox = new QVBoxLayout;
        QString q = readQ("sql/nasos_list1.sql");
        q.replace("$id$", QString("%1").arg(id));
        tb1 = getTableView(*m_db, "nasos_list1", q, "nasos_list1");
        tbBox->addWidget(tb1);
        mainLayout->addLayout(tbBox);
    }

    {
        QVBoxLayout *tbBox = new QVBoxLayout;
        QString q = readQ("sql/nasos_list.sql");
        q.replace("$id$", QString("%1").arg(id));
      //  QInputDialog::getMultiLineText(nullptr, "title", "label", q);
        tb2 = getTableView(*m_db, "nasos_list", q, "nasos_list");
        tbBox->addWidget(tb2);
        mainLayout->addLayout(tbBox);
    }

    QHBoxLayout *buttonBox = new QHBoxLayout;
    QPushButton *b1Button = new QPushButton(tr("Помощь"));
    QPushButton *b2Button = new QPushButton(tr("График"));

    QPushButton *okButton = new QPushButton(tr("Далее"));
    QPushButton *closeButton = new QPushButton(tr("Отменить"));
    okButton->setDefault(true);
    okButton->setEnabled(true);

    buttonBox->addStretch();
    buttonBox->addWidget(b1Button);
    buttonBox->addWidget(b2Button);

    buttonBox->addWidget(okButton);
    buttonBox->addWidget(closeButton);


    mainLayout->addLayout(buttonBox);

    setLayout(mainLayout);

    connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    connect(b1Button, SIGNAL(clicked()), this, SLOT(b1Clicked()));
    connect(b2Button, SIGNAL(clicked()), this, SLOT(b2Clicked()));
  //  connect(b3Button, SIGNAL(clicked()), this, SLOT(b3Clicked()));
  //  connect(b4Button, SIGNAL(clicked()), this, SLOT(b4Clicked()));
  //  connect(b5Button, SIGNAL(clicked()), this, SLOT(b5Clicked()));



    QSettings setting;

  //  resize(350, 300);

    setWindowTitle(tr("Насосы"));

    //QPainter painter(w);
    //painter.drawLine(0,0,100,100);
}

void Nasos1Dialog::currentIndexChanged(int index)
{
    int id = combo->currentData().toInt();

    QString q = readQ("sql/nasos_list1.sql");
    q.replace("$id$", QString("%1").arg(id));
    tb1->reset_q(q);

    q = readQ("sql/nasos_list.sql");
    q.replace("$id$", QString("%1").arg(id));
    tb2->reset_q(q);
}


void Nasos1Dialog::helpClicked()
{
}

#include <ui/Nas_charDialog.h>

void Nasos1Dialog::okClicked()
{
//  done(QDialog::Accepted);

    // CNas_charDialog dlg(this);

    // dlg.m_strEdit_p = "0";
    // dlg.m_strEdit_g = "1";
    // dlg.m_strEdit_s = "0";

    // dlg.m_strEdit_kol = "1";
    // dlg.m_strEdit_dnom;  // Номинальный, мм
    // dlg.m_strEdit_dzad;  // Новое значение диаметра, мм

    // dlg.m_strEdit_fnom;  // Номинальная, об/мин
    // dlg.m_strEdit_fzad;  // Новое значение частоты, об/мин

    // dlg.m_radioRadio1 = 0; // Функция (Аппроксимация/Расчет)
    // dlg.m_radioRadio7 = 0; // Привод (Нерегулируемый/Частотно регулируемый)
    // dlg.m_radioRadio9 = 0; // Колесо (Номинальное/Срезанное)
    // dlg.m_radioRadio5 = 1; // Гидравлическое сопротивление обвязки (Учитывать при расчете/Не учитывать)

    // dlg.write();

    // dlg.exec();
}

void Nasos1Dialog::b1Clicked()  // Данные
{
}

void Nasos1Dialog::b2Clicked()  // Стационарный
{
}

void Nasos1Dialog::b3Clicked()  // Расчет
{
}

void Nasos1Dialog::b4Clicked()  // В буфер ГРФ
{
}

void Nasos1Dialog::b5Clicked()  // В буфер ТБЛ
{
}

