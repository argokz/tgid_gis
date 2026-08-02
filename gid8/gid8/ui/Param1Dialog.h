#pragma once

#include <QtSql>

#include <QDialog>
#include "ui_Param1Dialog.h"

#include <dialog/SaveGeometry.h>


class CParam1Dialog : public QDialog 
{
    Q_OBJECT

  public:
    CParam1Dialog(QWidget *parent = nullptr);
//    ConnectStr cs;
    QString m_strTn;
    QString m_str_kol_iter;
    QString m_strName;
    bool m_bTeplopoter;
    bool m_bUf_calc;
    bool m_bTeplovyd;
    bool m_bVeter;
    bool m_bDross;
    bool m_bAvtomat;
    bool m_bUf_new;
    bool m_bChar_sety;
    bool m_bZn0;
    bool m_bUtechki;
    bool m_bMag_fragment;
    bool m_bSave_po;
    bool m_bKv;
    bool m_copy_calc;

    bool m_bPlan;
    
    int m_sopr;
    double m_roP;
    double m_roO;
    bool m_ro_temp;

    int m_cmbMetod;
    int m_cmbTrtp;
    int m_radioUr;
    int m_radioDop_nagr;


  public slots:
    void okClicked();
    void setup();



 private:
    void setRadioId(QButtonGroup * bg);
 public:
    void read();
    void write();
//    void detach();

  private:
        SaveGeometry save;
  public:
    Ui::Param1Dialog ui;
};



