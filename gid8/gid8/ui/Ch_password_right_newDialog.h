
#pragma once

#include <QtSql>

#include <QDialog>
#include "ui_Ch_password_right_newDialog.h"

#include <dialog/SaveGeometry.h>

class CCh_password_right_newDialog : public QDialog 
{
    Q_OBJECT

  public:
    CCh_password_right_newDialog(QWidget *parent = nullptr);
//    ConnectStr cs;
    QString m_strUser;
    QString m_strPassword;
    QString m_strPassword2;
    int m_lstbFiltr;


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
    Ui::Ch_password_right_newDialog ui;
};



