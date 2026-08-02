#include <QApplication>
#include <QtGui>
#include <QtWidgets>
#include <QtSql>

#include "std.h"

#include <geo/Klassif.h>
#include <geo/geol.h>

#include "kldial.h"

KlDial::KlDial(const GeoMap &gmap, QWidget *parent)
    : QDialog(parent)
{
  GeoMap::const_iterator it;

  lw = new QListWidget;
  lw->setSortingEnabled(false);

  for (it = gmap.begin(); it != gmap.end(); it++) {
    Klassif *kls = m_kl_list.findKlN(it.key());

    QString name = it.key();

    if (kls) {
      name += " "+kls->nazv_alias;
    }

    QListWidgetItem *item = new QListWidgetItem(name, lw);

    if (kls && !kls->ins) {
      item->setCheckState(Qt::Unchecked);
    }
    else {
      item->setCheckState(Qt::Checked);
    }
    lw->addItem(item);
  }

  QPushButton *okButton = new QPushButton(tr("&Ok"));
  okButton->setDefault(true);
  okButton->setEnabled(true);

  QPushButton *closeButton = new QPushButton(tr("Отменить"));
  QPushButton *editButton = new QPushButton(tr("Редактировать"));


  QPushButton *onButton = new QPushButton(tr("Выделить"));
  QPushButton *offButton = new QPushButton(tr("Очистить"));

  
//  QPushButton *delButton = new QPushButton(tr("Delete"));

  QVBoxLayout *buttonBox = new QVBoxLayout;

//  buttonBox->addWidget(helpButton);
  buttonBox->addWidget(okButton);
  buttonBox->addWidget(closeButton);
  buttonBox->addWidget(editButton);
//  buttonBox->addWidget(delButton);
  buttonBox->addStretch();
  buttonBox->addWidget(onButton);
  buttonBox->addWidget(offButton);
  buttonBox->addStretch();

  QHBoxLayout *mainLayout = new QHBoxLayout;
  
  mainLayout->addWidget(lw);

  mainLayout->addLayout(buttonBox);

  setLayout(mainLayout);

//  setWindowTitle();

//  connect(helpButton, SIGNAL(clicked()), this, SLOT(helpClicked()));
  connect(editButton, SIGNAL(clicked()), this, SLOT(editClicked()));
//  connect(delButton, SIGNAL(clicked()), this, SLOT(delClicked()));
  connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
  connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

  connect(onButton, SIGNAL(clicked()), this, SLOT(onClicked()));
  connect(offButton, SIGNAL(clicked()), this, SLOT(offClicked()));

  resize(600, 300);

  setWindowTitle("Классификатор");

}


void KlDial::helpClicked()
{
  QMessageBox::warning(this, tr("!"), "!");
}

#include "klass.h"


void KlDial::editClicked()
{
  int curRow = lw->currentRow();
  QListWidgetItem *item = lw->item(curRow);

  if (item) {
    Klassif *kls = m_kl_list.findKlN2(item->text());

    if (!kls) {
      QMessageBox::warning(this, tr("Ошибка!"), item->text());
    }
    else { 
      Klass dial(kls, this);
      dial.exec();
    }
  }
}

void KlDial::okClicked()
{
  for (int i = 0; i < lw->count(); i++) {
    QListWidgetItem *item = lw->item(i);
    if (item) {
      Klassif *kls = m_kl_list.findKlN2(item->text());
      if (kls) {
        if (item->checkState () == Qt::Checked) {
          kls->ins = true;
        }
        else {
          kls->ins = false;
        }
      }
    }
  }

  done(QDialog::Accepted);
}


void KlDial::onClicked()
{
  for (int i = 0; i < lw->count(); i++) {
    QListWidgetItem *item = lw->item(i);
    if (item) {
      item->setCheckState(Qt::Checked);
    }
  }
}

void KlDial::offClicked()
{
  for (int i = 0; i < lw->count(); i++) {
    QListWidgetItem *item = lw->item(i);
    if (item) {
      item->setCheckState(Qt::Unchecked);
    }
  }
}

