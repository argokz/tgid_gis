#pragma once

#include <QtSql>

#include <QDialog>
#include "ui_Print_frDialog.h"

#include <dialog/SaveGeometry.h>

class CPrint_frDialog;
typedef bool (CPrint_frDialogCallback)(CPrint_frDialog *prop);


class CPrint_frDialog : public QDialog 
{
    Q_OBJECT

    public:
        CPrint_frDialog(QWidget *parent = nullptr);
        int m_cmbPaper;
        int m_cmbMas;
        int m_radioPortret;


    public slots:
        void okClicked();
        void textChanged(QString txt);
        void currentTextChanged(QString txt);
        void buttonClicked(QAbstractButton*);
        void stateChanged(int);


    private:
        void changed();
        CPrint_frDialogCallback *prop_callback = nullptr;
//@callback@

        void setRadioId(QButtonGroup * bg);
    public:
        void read();
        void write();
        void setCallback(CPrint_frDialogCallback *c)
        {
            prop_callback = c;
        };

    private:
        SaveGeometry save;
    public:
        Ui::Print_frDialog ui;
};

