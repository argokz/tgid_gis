#pragma once

#include <QtSql>
#include <QDialog>
#include <QtHttpServer>

#include "ui_Webserver_dialog2Dialog.h"

class GidWidget;

class CWebserver_dialog2Dialog : public QDialog
{
    Q_OBJECT

  public:
    CWebserver_dialog2Dialog(GidWidget *gid);

//    ConnectStr cs;
    QString m_strHomedir;
    QString m_strTiles_dir;
    QString m_strDefindexfile;
    QString m_strPort;
    QString m_strPto;
    QString m_strPrivate;
    QString m_strPublic;
    QString m_strPort_https;
    QString m_strWeb_dir;
    bool m_bHttps;
    int m_iPort;
    int m_iPort_https;



  public slots:
    void okClicked();
    void onStart();
    void onStop();
    void onReset();
    void onHomedirbrowse();
    void onPrivatebrowse();
    void onPublicbrowse();
    void onTile_dir_browse();
    void onWeb_dir_browse();

 private:
    void read();
    void write();
//    void detach();

  private:
  public:
    Ui::Webserver_dialog2Dialog ui;

    QHttpServer httpServer;
    GidWidget *gid = nullptr;

};



