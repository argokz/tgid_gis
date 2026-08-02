#pragma once

#include <QtSql>

#include <QDialog>
#include "ui_Param2Dialog.h"

#include <dialog/SaveGeometry.h>

class CParam2Dialog : public QDialog 
{
    Q_OBJECT

  public:
    CParam2Dialog(QWidget *parent = nullptr);
//    ConnectStr cs;
    QString m_strTn;
    QString m_str_kol_iter;

    QString m_strName;
    bool m_bAvtomat;
    bool m_bChar_sety;
    bool m_bVeter;
    bool m_bSave_po;
    bool m_bLeto;
    bool m_bSaveLeto;
    bool m_bMag_fragment;
    bool m_bKv;
    bool m_copy_calc;
    
    int m_sopr;
    double m_roP;
    double m_roO;
    bool m_ro_temp;

    int m_cmbGws;
    int m_cmbGws2;
    int m_radioDetaliz;

  public slots:
    void okClicked();
    void onT_ist();
    void setup();



 private:
    void setRadioId(QButtonGroup * bg);
 public:
    void read();
    void write();
//    void detach();

  private:
//    SaveGeometry save;
  public:
    Ui::Param2Dialog ui;
};



