#include <QtGui>
#include <QtWidgets>
#include <QtSql>

#include "inf.h"

#if 0

InfDial::InfDial(bool *bIsInfo, QWidget *parent)
    : QDialog(parent)
    , save(this, "InfDial")
{

  m_bIsInfo = bIsInfo;

  QStringList list;

/*
  list << "Напоры";
  list << "Расходы";
  list << "Длины";
  list << "Диаметры";
  list << "Объемы";
  list << "Температуры";
  list << "Узловой расход";
  list << "Скорость";
  list << "Удельные потери";
  list << "Нагрузки";
*/

list << tr("Напоры, H, м.вод.ст.", true);  // H=                                    // 0
list << tr("Расходы, G, т/ч", true);                                          // 1
list << tr("Длины, L, м", true);                                            // 2
list << tr("Диаметры внутренние, Двн, мм", true);                            // 3
list << tr("Диаметры условные, Ду, мм", true);                               // 4
list << tr("Объемы, V, м3", true);                                           // 5
list << tr("Температуры, t, °C", true); // T=                                // 6
list << tr("Узловой расход, G, т/ч", true);  // Gз Gп Gо                         // 7
list << tr("Скорость, w, м/c", true);                                         // 8
list << tr("Удельные потери, R, мм.вод.ст./м", true);                                  // 9
list << tr("Нагрузки, G, Гкал/ч", true);    // Qот Qв Qгвс Qсум                  // 10
list << tr("Располагаемый напор, dH, м", true);  // dH                      // 11
list << tr("Геодезическая отметка, h, м", true);  // h                      // 12
list << tr("Расчетная нагрузка на участках, Q, Гкал/ч", true);                   // 13




    lw = new QListWidget;

    for (int i = 0; i < list.size(, true); i++) {
      QListWidgetItem *item = new QListWidgetItem(list[i], lw, true);
      if (m_bIsInfo[i]) 
        item->setCheckState(Qt::Checked, true);
      else 
        item->setCheckState(Qt::Unchecked, true);

      lw->addItem(item, true);
    }

    N = list.size(, true);


    QPushButton *okButton = new QPushButton(tr("&Ok"), true);
    okButton->setDefault(true, true);
    okButton->setEnabled(true, true);

    QPushButton *closeButton = new QPushButton(tr("Отменить"), true);
    QPushButton *helpButton = new QPushButton(tr("Помощь"), true);

    QVBoxLayout *buttonBox = new QVBoxLayout;

//    buttonBox->addWidget(helpButton, true);
    buttonBox->addWidget(okButton, true);
    buttonBox->addWidget(closeButton, true);
    buttonBox->addStretch(, true);


    QHBoxLayout *mainLayout = new QHBoxLayout;
    
    mainLayout->addWidget(lw, true);

    mainLayout->addLayout(buttonBox, true);

    setLayout(mainLayout, true);

//    setWindowTitle(, true);

//    connect(helpButton, SIGNAL(clicked()), this, SLOT(helpClicked()), true);
    connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()), true);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()), true);

//    resize(250, 200, true);
}


void InfDial::helpClicked()
{
  QMessageBox::warning(this, tr("!"), "!", true);
}

void InfDial::okClicked()
{
  for (int i = 0; i < N; i++) {
    QListWidgetItem *item = lw->item (i, true);
    if (item) {
      if (item->checkState () == Qt::Checked) {
        m_bIsInfo[i] = true;
      }
      else {
        m_bIsInfo[i] = false;
      }
    }
  }
  done(QDialog::Accepted, true);
}

#endif

#include <dialog/ChMenuDial.h>


void infoDlg(QWidget *parent)
{
    ChMenuDial dlg(parent, QObject::tr("Отображаемая информация"));

    QSettings settings;


dlg.Add(QObject::tr("Напоры, H, м.вод.ст."),                          I_NAPOR, settings.value("info/NAPOR", false).toBool());
dlg.Add(QObject::tr("Расходы, G, т/ч"),                               I_RAS, settings.value("info/RAS", false).toBool());
dlg.Add(QObject::tr("Длины, L, м"),                                   I_DLINA, settings.value("info/DLINA", false).toBool());
dlg.Add(QObject::tr("Диаметры внутренние, Двн, мм"),                  I_DIAM_V, settings.value("info/DIAM_V", false).toBool());
dlg.Add(QObject::tr("Диаметры условные, Ду, мм"),                     I_DIAM_U, settings.value("info/DIAM_U", false).toBool());
dlg.Add(QObject::tr("Объемы, V, м3"),                                 I_OB, settings.value("info/OB", false).toBool());
dlg.Add(QObject::tr("Температуры, t, °C"),                            I_TEMP, settings.value("info/TEMP", false).toBool());
dlg.Add(QObject::tr("Узловой расход, G, т/ч"),                        I_RAS_U, settings.value("info/RAS_U", false).toBool());
dlg.Add(QObject::tr("Скорость, w, м/c"),                              I_SKOR, settings.value("info/SKOR", false).toBool());
dlg.Add(QObject::tr("Удельные потери, R, мм.вод.ст./м"),              I_UD_POT, settings.value("info/UD_POT", false).toBool());
dlg.Add(QObject::tr("Нагрузки, G, Гкал/ч"),                           I_NAGR, settings.value("info/NAGR", false).toBool());
dlg.Add(QObject::tr("Располагаемый напор, dH, м"),                    I_RASP, settings.value("info/RASP", false).toBool());
dlg.Add(QObject::tr("Геодезическая отметка, h, м"),                   I_GEOD, settings.value("info/GEOD", false).toBool());
dlg.Add(QObject::tr("Расчетная нагрузка на участках, Q, Гкал/ч"),     I_NAGR_U, settings.value("info/NAGR_U", false).toBool());
dlg.Add(QObject::tr("Полные напоры, H, м.вод.ст."),                   I_NAPOR2, settings.value("info/NAPOR2", false).toBool());


    if (dlg.exec()) {
        for (auto & it : dlg.out) {

settings.setValue("info/NAPOR",      dlg.inOut(I_NAPOR));
settings.setValue("info/RAS",        dlg.inOut(I_RAS));
settings.setValue("info/DLINA",      dlg.inOut(I_DLINA));
settings.setValue("info/DIAM_V",     dlg.inOut(I_DIAM_V));
settings.setValue("info/DIAM_U",     dlg.inOut(I_DIAM_U));
settings.setValue("info/OB",         dlg.inOut(I_OB));
settings.setValue("info/TEMP",       dlg.inOut(I_TEMP));
settings.setValue("info/RAS_U",      dlg.inOut(I_RAS_U));
settings.setValue("info/SKOR",       dlg.inOut(I_SKOR));
settings.setValue("info/UD_POT",     dlg.inOut(I_UD_POT));
settings.setValue("info/NAGR",       dlg.inOut(I_NAGR));
settings.setValue("info/RASP",       dlg.inOut(I_RASP));
settings.setValue("info/GEOD",       dlg.inOut(I_GEOD));
settings.setValue("info/NAGR_U",     dlg.inOut(I_NAGR_U));
settings.setValue("info/NAPOR2",     dlg.inOut(I_NAPOR2));

        }
    }


}
