#include <QtGui>
#include <QtWidgets>
#include <QtSql>

#include "MMenuDial.h"

MMenuDial::MMenuDial(QWidget *parent, const QString & title)
    : QDialog(parent)
    , save(this, "MMenuDial", title)
{

    QPushButton *okButton = new QPushButton(tr("&Ok"));
    okButton->setDefault(true);
    okButton->setEnabled(true);

    QPushButton *closeButton = new QPushButton(tr("Отменить"));
//    QPushButton *helpButton = new QPushButton(tr("Помощь"));

    buttonBox = new QVBoxLayout;

    QHBoxLayout *rowLayout = new QHBoxLayout;
    QVBoxLayout *leftLayout = new QVBoxLayout;

    QLabel *label = new QLabel(tr("Поиск"));
    lineEdit = new QLineEdit;
    label->setBuddy(lineEdit);
    rowLayout->addWidget(label);
    rowLayout->addWidget(lineEdit);

    buttonBox->addWidget(okButton);
    buttonBox->addWidget(closeButton);
    buttonBox->addStretch();
    //    buttonBox->addWidget(helpButton);

    leftLayout->addLayout(rowLayout);
    leftLayout->addWidget(&lw);

    QHBoxLayout *mainLayout = new QHBoxLayout;

    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(buttonBox);

    setLayout(mainLayout);

    setWindowTitle(title);

//    connect(helpButton, SIGNAL(clicked()), this, SLOT(helpClicked()));

    connect(lineEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(textChanged(const QString &)));


    connect(&lw, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(okClicked()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    curText = "";
    index = -1;
    _value = 0;
}


void MMenuDial::AddButton(const QString & txt, int id, bool choice)
{
    QPushButton *button = new QPushButton(txt);
    buttonBox->addWidget(button);
//    connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));

    connect(button, &QPushButton::clicked, this, [this, value = id, ch = choice] {
                buttonClicked(value, ch);
    });


}


void MMenuDial::Add(const QString & txt, QVariant id)
{
    QListWidgetItem *item = new QListWidgetItem(txt, &lw);

    lw.addItem(item);

    MMenuItem chitem;
    chitem.id = id;
    chitem.txt = txt;

    lst.push_back(chitem);
}


void MMenuDial::textChanged(const QString &txt)
{
  lw.clear();
  for (int i = 0; i < lst.size(); i++) {
    if (lst[i].txt.contains(txt, Qt::CaseInsensitive)) {
      lw.addItem(lst[i].txt);
    }
  }
}


void MMenuDial::helpClicked()
{
  QMessageBox::warning(this, tr("!"), "!");
}

void MMenuDial::okClicked()
{
  QListWidgetItem *item = lw.currentItem ();
  if (item) {
    curText = item->text();
    index = lw.currentRow();

    int N = lst.size();

    int ii = 0;

    QString txt = lineEdit->text();

    for (int i = 0; i < lst.size(); i++) {
      if (lst[i].txt.contains(txt, Qt::CaseInsensitive)) {
          if (ii == index) {
              _value = lst[i].id;
              done(QDialog::Accepted);
              break;
          }
          ii++;
      }
    }
  }
//  done(QDialog::Accepted);
}


void MMenuDial::buttonClicked(const QVariant &value, bool choice)
{
    if (!choice) {
        _button_clicked = value.toInt();
        done(QDialog::Accepted);
        return;
    }

    QListWidgetItem *item = lw.currentItem ();
    if (item) {
        curText = item->text();
        index = lw.currentRow();

        int N = lst.size();
        int ii = 0;

        QString txt = lineEdit->text();

        for (int i = 0; i < lst.size(); i++) {
            if (lst[i].txt.contains(txt, Qt::CaseInsensitive)) {
                if (ii == index) {
                    _value = lst[i].id;
                    _button_clicked = value.toInt();
                    done(QDialog::Accepted);
                    break;
                }
                ii++;
            }
        }
    }
//  done(QDialog::Accepted);
}



