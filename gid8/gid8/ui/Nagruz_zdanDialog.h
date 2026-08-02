#pragma once

#include <QtSql>

#include <QDialog>
#include "ui_Nagruz_zdanDialog.h"

class CNagruz_zdanDialog;
typedef bool (CNagruz_zdanDialogCallback)(CNagruz_zdanDialog *prop);

#include <dialog/SaveGeometry.h>

class CNagruz_zdanDialog : public QDialog 
{
    Q_OBJECT

    public:
        CNagruz_zdanDialog(QWidget *parent = nullptr);
        QString m_strEdit2;
        QString m_strEdit3;
        QString m_strEdit5;
        QString m_strEdit6;
        QString m_strEdit7;
        QString m_strEdit4;
        QString m_strEdit8;
        int m_cmbCombo1;


    public slots:
        void okClicked();
        void textChanged(QString txt);
        void currentTextChanged(QString txt);
        void buttonClicked(QAbstractButton*);
        void stateChanged(int);
        void on_help();


    private:
        void changed();
        CNagruz_zdanDialogCallback *prop_callback = nullptr;
//@callback@

        void setRadioId(QButtonGroup * bg);
    public:
        void read();
        void write();
        void setCallback(CNagruz_zdanDialogCallback *c)
        {
            prop_callback = c;
        };

    private:
        SaveGeometry save;
    public:
        Ui::Nagruz_zdanDialog ui;
};

