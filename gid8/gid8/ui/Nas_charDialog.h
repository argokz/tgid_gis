#pragma once

#include <QtSql>

#include <QDialog>
#include "ui_Nas_charDialog.h"
#include "ui/pumpcomputedialog.h"

class CNas_charDialog;
typedef bool (CNas_charDialogCallback)(CNas_charDialog *prop);

#include <dialog/SaveGeometry.h>

class CNas_charDialog : public QDialog 
{
    Q_OBJECT

public:
    CNas_charDialog(QWidget *parent, QSqlDatabase &db, int pumpId);
    QString m_strEdit_fzad;
    QString m_strEdit_p;
    QString m_strEdit_g;
    QString m_strEdit_s;
    QString m_strEdit_kol;
    QString m_strEdit_dnom;
    QString m_strEdit_dzad;
    QString m_strEdit_fnom;
    int m_radioFunction;
    int m_radioWheel;
    int m_radioResistance;
    int m_radioDrive;
    int pumpId;
    GRAPHR_PARAM grp;


public slots:
    void okClicked();
    void textChanged(QString txt);
    void currentTextChanged(QString txt);
    void buttonClicked(QAbstractButton*);
    void calcDiam();
    void stateChanged(int);
    void onButtonCalcResistance();
    void onButton6();
    void onCalc();

signals:
    void dialogAccepted();
private:
    void changed();
    CNas_charDialogCallback *prop_callback = nullptr;

    void setRadioId(QButtonGroup * bg);
public:
    void read();
    void write();
    void setCallback(CNas_charDialogCallback *c)
    {
        prop_callback = c;
    };

private:
    QSqlDatabase *m_db;
    SaveGeometry save;
public:
    Ui::CNas_charDialog ui;
    PUMP_REPORT pReport;
};

