#include <QtGui>
#include <QtWidgets>
#include <QtSql>

#include "geof.h"

GeoFDial::GeoFDial(QStringList & _list, QWidget *parent)
    : QDialog(parent)
{
  lw = new QListWidget;

  list = &_list;

  for (int i = 0; i < _list.size(); i++) {
    QListWidgetItem *item = new QListWidgetItem(_list[i], lw);
    item->setCheckState(Qt::Checked);
    lw->addItem(item);
  }

  QPushButton *okButton = new QPushButton(tr("&Ok"));
  okButton->setDefault(true);
  okButton->setEnabled(true);

  QPushButton *closeButton = new QPushButton(tr("Отменить"));
  QPushButton *addButton = new QPushButton(tr("Добавить"));
  QPushButton *delButton = new QPushButton(tr("Удалить"));

  QVBoxLayout *buttonBox = new QVBoxLayout;

//  buttonBox->addWidget(helpButton);
  buttonBox->addWidget(okButton);
  buttonBox->addWidget(closeButton);
  buttonBox->addWidget(addButton);
  buttonBox->addWidget(delButton);
  buttonBox->addStretch();

  QHBoxLayout *mainLayout = new QHBoxLayout;
  
  mainLayout->addWidget(lw);

  mainLayout->addLayout(buttonBox);

  setLayout(mainLayout);

//  setWindowTitle();

//  connect(helpButton, SIGNAL(clicked()), this, SLOT(helpClicked()));
  connect(addButton, SIGNAL(clicked()), this, SLOT(addClicked()));
  connect(delButton, SIGNAL(clicked()), this, SLOT(delClicked()));
  connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
  connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
}


void GeoFDial::helpClicked()
{
  QMessageBox::warning(this, tr("!"), "!");
}

void GeoFDial::addClicked()
{
#if 0
  QStringList list = QFileDialog::getOpenFileNames(this,
                                   tr("Открыть геобазу"), QString(),
                                   tr("Файлы геобазы (*.shp *.mdb)"));

  for (int i = 0; i < list.size(); i++) {
    if (!list[i].isEmpty()) {
      QList<QListWidgetItem *> li = lw->findItems(list[i], 0);
      if (li.size() == 0) {
        QListWidgetItem *item = new QListWidgetItem(list[i], lw);
        item->setCheckState(Qt::Checked);
        lw->addItem(item);
      }
    }V
  }
#endif
}

void GeoFDial::delClicked()
{
  int curRow = lw->currentRow();
  QListWidgetItem *item = lw->takeItem(curRow);

  if (item) {
    lw->removeItemWidget(item);
  }
}


void GeoFDial::okClicked()
{
  list->clear();
  for (int i = 0; i < lw->count(); i++) {
    QListWidgetItem *item = lw->item (i);
    if (item) {
      *list  << item->text();
    }
  }
  done(QDialog::Accepted);
}
