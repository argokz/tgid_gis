#pragma once

#include <QtSql>

#include <QDialog>
#include "ui_Ch_passwordDialog.h"

#include <dialog/SaveGeometry.h>

class CCh_passwordDialog : public QDialog 
{
    Q_OBJECT

  public:
    CCh_passwordDialog(QWidget *parent = nullptr);
//    ConnectStr cs;
    QString m_strUser;
    bool m_bRight1;
    bool m_bRight2;
    bool m_bRight3;
    bool m_bRight4;
    bool m_bRight5;
    bool m_bRight6;
    bool m_bRight7;
    bool m_bRight8;
    bool m_bRight9;
    bool m_bCheck2;


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
    Ui::Ch_passwordDialog ui;
};



