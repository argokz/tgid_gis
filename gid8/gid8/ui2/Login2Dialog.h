/*
    Õ≈ «¿Ã≈Õﬂ“‹, ”∆≈ √Œ“Œ¬Œ!!!
*/


#pragma once

#include <QtSql>

#include <QDialog>
#include "ui_Login2Dialog.h"

#include <dialog/SaveGeometry.h>

struct CUser
{
  int id;
  QString password;
  int user_right;
};


class CLogin2Dialog : public QDialog
{
    Q_OBJECT

  public:
    CLogin2Dialog(QSqlDatabase &db, QWidget *parent = nullptr);
//    ConnectStr cs;
    int user_right;
    int m_user;
    QString m_strUser;
    QString m_password;

  public slots:
    void okClicked();
 //   void onBdGid();
//    void onBdGeo();
//    void onCheck();


 private:
    void read();
    void write();
//    void detach();
    std::map <QString, CUser> list_user;
    SaveGeometry save;

  private:
    Ui::Login2Dialog ui;
};
