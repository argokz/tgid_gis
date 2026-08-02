#pragma once

#include <QtSql>

#include <QDialog>
#include "ui_UtfDialog.h"

#include <dialog/SaveGeometry.h>

class CUtfDialog;
typedef bool (CUtfDialogCallback)(CUtfDialog *prop);


class CUtfDialog : public QDialog 
{
    Q_OBJECT

    public:
        CUtfDialog(QWidget *parent = nullptr);
        QString m_strT1;
        QString m_strT2;
        QString m_strT3;
        QString m_strG;
        QString m_strG0;
        QString m_strUf;
        int m_radioM1;


    public slots:
        void okClicked();
        void textChanged(QString txt);
        void buttonClicked(QAbstractButton*);
        void stateChanged(int);


    private:
        void changed();
        CUtfDialogCallback *prop_callback = nullptr;

        void setRadioId(QButtonGroup * bg);
    public:
        void read();
        void write();
        void setCallback(CUtfDialogCallback *c)
        {
            prop_callback = c;
        };

    private:
        SaveGeometry save;
    public:
        Ui::UtfDialog ui;
};

