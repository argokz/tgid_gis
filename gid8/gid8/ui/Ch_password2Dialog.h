
#pragma once

#include <QtSql>

#include <QDialog>
#include "ui_Ch_password2Dialog.h"

#include <dialog/SaveGeometry.h>

class CCh_password2Dialog : public QDialog 
{
    Q_OBJECT

  public:
    CCh_password2Dialog(QWidget *parent = nullptr);
//    ConnectStr cs;
    QString m_strOldpassword;
    QString m_strPassword;
    QString m_strPassword2;


  public slots:
    void okClicked();



 private:
    void setRadioId(QButtonGroup * bg);
 public:
    void read();
    void write();
//    void detach();

  private:
    SaveGeometry save;
  public:
    Ui::Ch_password2Dialog ui;
};



