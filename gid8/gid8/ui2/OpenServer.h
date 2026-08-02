
#pragma once

#include <QDialog>
#include "ui_OpenServer.h"


#include <config.h>

#if 0

struct ConnectStr
{
    int rdbms;
    QString host;
    unsigned short port;
    QString user;
    QString password;
    QString baza;
    QString geo;
};
#endif


class COpenServer : public QDialog
{
    Q_OBJECT

  public:
    COpenServer(QWidget *parent = nullptr);
    ConnectStr cs;

  public slots:
    void okClicked();
    void onBdGid();
    void onBdGeo();
    void onCheck();
    void onNewBaza();


 private:
    void read();
    void write();
//    void detach();

  private:
    Ui::OpenServer ui;
};
