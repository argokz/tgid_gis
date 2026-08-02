#pragma once

#include <QDialog>
#include "ui_PercentDialog.h"

#include <dialog/SaveGeometry.h>

class CPercentDialog : public QDialog 
{
    Q_OBJECT

  public:
    CPercentDialog(QWidget *parent = nullptr);
    void SetValue(int f);

  public slots:
    void okClicked();

 private:
//    void detach();
    int pp;
    SaveGeometry save;

  private:
    Ui::PercentDialog ui;
};
