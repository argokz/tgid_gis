#include <QtGui>
#include <QtWidgets>
#include <QtSql>

#include "mainwindow.h"
#include "paramdr.h"

ParamDr::ParamDr(QWidget *parent)
    : QDialog(parent)
{
  QPushButton *okButton = new QPushButton(tr("Расчет"));
  okButton->setDefault(true);
  okButton->setEnabled(true);

  QPushButton *closeButton = new QPushButton(tr("Отмена"));
  QHBoxLayout *buttonBox = new QHBoxLayout;
  buttonBox->addStretch();
  buttonBox->addWidget(okButton);
  buttonBox->addWidget(closeButton);
  buttonBox->addStretch();

  QVBoxLayout *mainLayout = new QVBoxLayout;

  QGroupBox *urBox = new QGroupBox(tr(""));

  QRadioButton *rr1 = new QRadioButton(tr("По удельным расходам"));
  QRadioButton *rr2 = new QRadioButton(tr("По температурному графику"));

  rr1->setChecked(true);

  QHBoxLayout *url = new QHBoxLayout;

  url->addWidget(rr1);
  url->addWidget(rr2);

  urBox->setLayout(url);


 // ur = new QCheckBox(tr("По заданным удельным расходам (объект УР)"));
  
  teplopoter = new QCheckBox(tr("C учетом тепловых потерь в сети"));
  m_uf_calc = new QCheckBox(tr("С учетом рассчитанных коэффициентов смешения"));
  teplovyd = new QCheckBox(tr("С учетом внутренних тепловыделений"));

  QGroupBox *gr1 = new QGroupBox(tr("Расчетные расходы потребителей"));
  {
    QVBoxLayout *ly1 = new QVBoxLayout;
    ly1->addWidget(urBox);
    ly1->addWidget(teplopoter);
    ly1->addWidget(m_uf_calc);
    ly1->addWidget(teplovyd);
    gr1->setLayout(ly1);
  }

  QGroupBox *gr2 = new QGroupBox(tr("Температура наружного воздуха"));
  {
//      QHBoxLayout *r1 = new QHBoxLayout;
//    QLabel *l =  new QLabel("Температура наружного воздуха");
    e1 = new QDoubleSpinBox;
    e1->setDecimals(1);
    veter = new QCheckBox(tr("Учитывать ветер"));


//    e1->setValidator(new QDoubleValidator(e1));
//    r1->addWidget(l);
//    r1->addWidget(e1);
    QHBoxLayout *ly2 = new QHBoxLayout;
    ly2->addWidget(e1);
    ly2->addWidget(veter);

    gr2->setLayout(ly2);
  }

  QGroupBox *gr3 = new QGroupBox(tr(""));
  {
      QVBoxLayout *ly3 = new QVBoxLayout;
      dross = new QCheckBox(tr("Расчет дроссельных органов и запись сопротивлений"));
      avtomat = new QCheckBox(tr("Расчет автоматизированных потребителей"));
      uf_new = new QCheckBox(tr("Запись коэффициентов смешения"));
      char_sety = new QCheckBox(tr("Количественные характеристики сети"));
      ly3->addWidget(dross);
      ly3->addWidget(avtomat);
      ly3->addWidget(uf_new);
      ly3->addWidget(char_sety);
      gr3->setLayout(ly3);
  }

  
  QGroupBox *gr4 = new QGroupBox(tr(""));
  {
    QVBoxLayout *ly4 = new QVBoxLayout;

    zn0 = new QCheckBox(tr("Подбор заданных перепадов на источнике"));
    utechki = new QCheckBox(tr("Учет нормируемых утечек на трубопроводах"));
    ly4->addWidget(zn0);
    ly4->addWidget(utechki);
    gr4->setLayout(ly4);
  }


  QHBoxLayout *r2 = new QHBoxLayout;
  {
    QLabel *l =  new QLabel("Название магистрального фрагмента");
    e2 = new QComboBox;
    e2->setEditable (true);
    e2->installEventFilter(this); 
    r2->addWidget(l);
    r2->addWidget(e2);
  }
  
  save_po = new QCheckBox(tr("Записать тепловые нагрузки в обобщенный потребитель"));
  leto = new QCheckBox(tr("Летний режим"));

//  mainLayout->addWidget(ur);
  mainLayout->addWidget(gr1);
  mainLayout->addWidget(gr2);
  mainLayout->addWidget(gr3);
  mainLayout->addWidget(gr4);
  mainLayout->addLayout(r2);
  mainLayout->addWidget(save_po);
  mainLayout->addWidget(leto);
  
  mainLayout->addLayout(buttonBox);
  setLayout(mainLayout);

  QSettings *setting = get_Settings();
  if (0 && setting) {
 //   ur->setCheckState((Qt::CheckState)setting->value("ur", Qt::Checked).toInt());
    teplopoter->setCheckState((Qt::CheckState)setting->value("teplopoter", Qt::Checked).toInt());
    teplovyd->setCheckState((Qt::CheckState)setting->value("teplovyd", Qt::Checked).toInt());
    veter->setCheckState((Qt::CheckState)setting->value("veter", Qt::Checked).toInt());
    dross->setCheckState((Qt::CheckState)setting->value("dross", Qt::Checked).toInt());
    uf_new->setCheckState((Qt::CheckState)setting->value("uf_new", Qt::Checked).toInt());
    char_sety->setCheckState((Qt::CheckState)setting->value("char_sety", Qt::Checked).toInt());
    save_po->setCheckState((Qt::CheckState)setting->value("save_po", Qt::Checked).toInt());
    leto->setCheckState((Qt::CheckState)setting->value("leto", Qt::Checked).toInt());
    e1->setValue(setting->value("e1", -10.).toDouble());
    e2->setEditText(setting->value("e2", "").toString());
  }

//  if (ur->checkState() == Qt::Checked) {
//    teplopoter->setEnabled(false);
//    teplovyd->setEnabled(false);
 // }

  connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
  connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

//  connect(ur, SIGNAL(stateChanged(int)), this, SLOT(urClicked(int)));

  setWindowTitle(tr("Установки расчета Планового режима"));
}

void ParamDr::helpClicked()
{
  QMessageBox::warning(this, tr("!"), "!");
}

void ParamDr::okClicked()
{
  QSettings *setting = get_Settings();
  if (setting) {
//    setting->setValue("ur", ur->checkState());
    setting->setValue("teplopoter", teplopoter->checkState());
    setting->setValue("teplovyd", teplovyd->checkState());
    setting->setValue("veter", veter->checkState());
    setting->setValue("dross", dross->checkState());
    setting->setValue("uf_new", uf_new->checkState());
    setting->setValue("char_sety", char_sety->checkState());
    setting->setValue("save_po", save_po->checkState());
    setting->setValue("leto", leto->checkState());
    setting->setValue("e1", e1->value());
    setting->setValue("e2", e2->currentText());
  }


  QString str, par = "";

  if (dross->checkState() == Qt::Checked) {
    par = "-dross_yes ";
  }

  str = QString("%1 -iter %2 -dross -Tn %3 ")
    .arg(par)
    .arg(20)
    .arg(e1->value());

//    if ( dross->checkState() == Qt::Checked ) {
//      str += " -soff";
//      str += " -son";
//    }

  if ( !e2->currentText().isEmpty()) {
    str += QString(" -mn \"%1\"").arg(e2->currentText());
  }

  if ( char_sety->checkState() == Qt::Checked ) {
    str += " -char_sety";
  }
/*
  if ( ur->checkState() != Qt::Checked) {
    str += " -tg";

    if ( teplovyd->checkState() != Qt::Checked) {
      str += " -no_teplovyd";
    }
    if ( teplopoter->checkState() != Qt::Checked ) {
      str += " -no_teplopoter2";
    }
    if ( uf_new->checkState() != Qt::Checked ) {
//        if (QMessageBox::warning(this, "", tr("Сохранить коэффициенты смешения?") == IDYES)) {
//          str += " -save_uf_new";
//        }
    }
  }
*/

  if (leto->checkState() == Qt::Checked) {
    str += " -leto";
  }

  if (veter->checkState() == Qt::Checked) {
    str += " -veter";
  }

  if (save_po->checkState() == Qt::Checked) {
    str += " -save_po";
  }

  m_str = str;
    
  done(QDialog::Accepted);
}

void ParamDr::urClicked(int state)
{
  bool b = state == Qt::Unchecked;

  teplopoter->setEnabled(b);
  teplovyd->setEnabled(b);
}


bool ParamDr::eventFilter ( QObject * o, QEvent * e )
{
  if (e->type() == QEvent::MouseButtonPress/* && o == comboBox*/) {
    QString fileName = QFileDialog::getOpenFileName(this,
                               tr("Tgid"), ".",
                               tr("Tgid files (*.mdb)"));
    if (!fileName.isEmpty()) {
      e2->setEditText(fileName);
    }
  }
  return QDialog::eventFilter ( o, e );
}
