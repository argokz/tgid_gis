#pragma once

#include <QtSql>

#include <QDialog>
#include "ui_KtiDialog.h"

class CKtiDialog;
typedef bool (CKtiDialogCallback)(CKtiDialog *prop);

#include <dialog/SaveGeometry.h>

class CKtiDialog : public QDialog 
{
    Q_OBJECT

    public:
        CKtiDialog(QWidget *parent = nullptr);
        QString m_strTn;
        QString m_strTg;
        QString m_strT1;
        QString m_strT2;
        QString m_strDt;
        QString m_strDt2;
        QString m_strRas;
        QString m_strRas2;
        QString m_strLen;
        QString m_strDiametr;
        QString m_strDiametr_usl;
        QString m_strTol;
        QString m_strDateend;
        QString m_strKti;
        QString m_strKti1;
        QString m_strKti2;
        QString m_strTpn;
        QString m_strTpf;
        bool m_bKolwork;
        int m_cmbTyp_pr;
        int m_radioRadio1;


    public slots:
        void okClicked();
        void textChanged(QString txt);
        void currentTextChanged(QString txt);
        void buttonClicked(QAbstractButton*);
        void stateChanged(int);
        void onSave();
        void on_help();


    private:
        void changed();
        CKtiDialogCallback *prop_callback = nullptr;
//@callback@

        void setRadioId(QButtonGroup * bg);
    public:
        void read();
        void write();
        void setCallback(CKtiDialogCallback *c)
        {
            prop_callback = c;
        };

    private:
        SaveGeometry save;
    public:
        Ui::KtiDialog ui;
};

