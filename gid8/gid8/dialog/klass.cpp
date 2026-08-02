#include <QtGui>
#include <QtWidgets>
#include <QtSql>

#include "std.h"

#include <geo/Klassif.h>
#include <geo/geol.h>

#include "klass.h"

#include "wexample.h"


Klass::Klass(Klassif *kls, QWidget *parent)
    : QDialog(parent)
{
  m_kls = kls;

  QPushButton *okButton = new QPushButton(tr("&Ok"));
  okButton->setDefault(true);
  okButton->setEnabled(true);

  QPushButton *closeButton = new QPushButton(tr("Отменить"));


  QPushButton *onButton = new QPushButton(tr("Выделить"));
  QPushButton *offButton = new QPushButton(tr("Очистить"));

  QLabel *label;
//  QLineEdit *lineEdit;
  QHBoxLayout *rowLayout;

  w = new WExample(this);
  w->setW(kls->coli, kls->colb, 1.);
  w->resize(90, 90);

  QHBoxLayout *hBox = new QHBoxLayout;

  QVBoxLayout *sBox = new QVBoxLayout;

  QPushButton *colorLButton = new QPushButton(tr("Цвет линии"));
  QPushButton *colorBButton = new QPushButton(tr("Цвет фигуры"));
  QPushButton *fontButton = new QPushButton(tr("Шрифт"));
  QDoubleSpinBox *lineWidth;

  rowLayout = new QHBoxLayout;
  label = new QLabel("Толщина линии");
  lineWidth = new QDoubleSpinBox;
  label->setBuddy(lineWidth);
  rowLayout->addWidget(label);
  rowLayout->addWidget(lineWidth);


  sBox->addWidget(colorLButton);
  sBox->addWidget(colorBButton);
  sBox->addWidget(fontButton);
  sBox->addLayout(rowLayout);
  sBox->addStretch();

  
//  QPushButton *delButton = new QPushButton(tr("Delete"));

  QHBoxLayout *buttonBox = new QHBoxLayout;

  buttonBox->addStretch();
  buttonBox->addWidget(okButton);
  buttonBox->addWidget(closeButton);

  hBox->addLayout(sBox);
  hBox->addWidget(w);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  
//  mainLayout->addWidget(lw);

  mainLayout->addLayout(hBox);
  mainLayout->addLayout(buttonBox);

  setLayout(mainLayout);

//  setWindowTitle();

//  connect(helpButton, SIGNAL(clicked()), this, SLOT(helpClicked()));
//  connect(editButton, SIGNAL(clicked()), this, SLOT(editClicked()));
//  connect(delButton, SIGNAL(clicked()), this, SLOT(delClicked()));
  connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
  connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

  connect(colorLButton, SIGNAL(clicked()), this, SLOT(colorLClicked()));
  connect(colorBButton, SIGNAL(clicked()), this, SLOT(colorBClicked()));
  connect(fontButton, SIGNAL(clicked()), this, SLOT(fontClicked()));


  resize(350, 300);

  setWindowTitle("Классификатор");

  QPainter painter(w);
  painter.drawLine(0,0,100,100);
}


void Klass::helpClicked()
{
}

void Klass::editClicked()
{
}

void Klass::okClicked()
{
  done(QDialog::Accepted);
}

void Klass::colorLClicked()
{
  QColor color = QColorDialog::getColor(QColor(m_kls->coli), this);

  if (color.isValid()) {
    m_kls->coli = color.rgb();
    w->set_colorL(m_kls->coli);
  }
}

void Klass::colorBClicked()
{
  QColor color = QColorDialog::getColor(QColor(m_kls->colb), this);

  if (color.isValid()) {
    m_kls->colb = color.rgb();
    w->set_colorB(m_kls->colb);
  }
}

void Klass::fontClicked()
{
  bool ok;
  QFont color = QFontDialog::getFont(&ok, this);

  if (ok) {
  }
}
