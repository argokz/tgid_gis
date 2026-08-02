#include <QApplication>
#include <QtGui>
#include <QtWidgets>

#include "PercentDialog.h"


CPercentDialog::CPercentDialog(QWidget *parent)
  : QDialog(parent) , save(this, "CPercentDialog")
{
    ui.setupUi(this);

    ui.progressBar_CG_PROGDLG_PROGRESS->setRange(0, 100);

    show();

    pp = -1;

//    connect(ui.pushButton_IDOK, SIGNAL(clicked()), this, SLOT(okClicked()));
//    connect(ui.pushButton_IDCANCEL, SIGNAL(clicked()), this, SLOT(close()));
//    connect(ui.pushButton_BD_GID_BTN, SIGNAL(clicked()), this, SLOT(onBdGid()));
}

void CPercentDialog::okClicked()
{
  done(QDialog::Accepted);
}


void CPercentDialog::SetValue(int p)
{
      if (p != pp) {
        ui.progressBar_CG_PROGDLG_PROGRESS->setValue(p);
        pp = p;
//        show();
      }
}
