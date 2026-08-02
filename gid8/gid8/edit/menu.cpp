#include <QtGui>
#include  <QtWidgets>

#include "menu.h"
#include "help.h"

MenuDial::MenuDial(const QStringList & list, const QString & title, QWidget *parent)
    : QDialog(parent)
    , save(this, "MenuDial", title)
{
    m_list = list;

    lw = new QListWidget;

    lw->addItems(list);

    QPushButton *okButton = new QPushButton(tr("&Ok"));
    okButton->setDefault(true);
    okButton->setEnabled(true);

    QPushButton *closeButton = new QPushButton(tr("Close"));
    QPushButton *helpButton = new QPushButton(tr("Help"));

    QHBoxLayout *buttonBox = new QHBoxLayout;

    buttonBox->addStretch();
//    buttonBox->addWidget(helpButton);
    buttonBox->addWidget(okButton);
    buttonBox->addWidget(closeButton);

    QHBoxLayout *rowLayout = new QHBoxLayout;
    QLabel *label = new QLabel(tr("Поиск"));
    lineEdit = new QLineEdit;
    label->setBuddy(lineEdit);
    rowLayout->addWidget(label);
    rowLayout->addWidget(lineEdit);

    connect(lineEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(textChanged(const QString &)));


    QVBoxLayout *mainLayout = new QVBoxLayout;

    mainLayout->addLayout(rowLayout);
    
    mainLayout->addWidget(lw);

    mainLayout->addLayout(buttonBox);

    setLayout(mainLayout);

    setWindowTitle(title);

//    connect(helpButton, SIGNAL(clicked()), this, SLOT(helpClicked()));
    connect(lw, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(okClicked()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
}


void MenuDial::helpClicked()
{
  QMessageBox::warning(this, tr("!"), "!");
}

void MenuDial::okClicked()
{
//  QMessageBox::warning(this, tr("!"), "!");
  QListWidgetItem *item = lw->currentItem ();
  if (item) {
    curText = item->text();
    index = lw->currentRow();
  }
  done(QDialog::Accepted);

//  close();
}


void MenuDial::textChanged(const QString &txt)
{
  lw->clear();
  for (int i = 0; i < m_list.size(); i++) {
    if (m_list[i].contains(txt, Qt::CaseInsensitive)) {
      lw->addItem(m_list[i]);
    }
  }
}
