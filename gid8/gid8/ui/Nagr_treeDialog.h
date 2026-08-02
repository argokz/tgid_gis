#pragma once

#include <QtSql>

#include <QDialog>
#include "ui_Nagr_treeDialog.h"

class CNagr_treeDialog;
typedef bool (CNagr_treeDialogCallback)(CNagr_treeDialog *prop);

#include <dialog/SaveGeometry.h>

class CNagr_treeDialog : public QDialog 
{
    Q_OBJECT

    public:
        CNagr_treeDialog(QWidget *parent = nullptr);
        QString m_strName;
        QString m_strOtopl;
        QString m_strGvs;
        QString m_strVent;
        QString m_strAll;


    public slots:
        void okClicked();
        void textChanged(QString txt);
        void currentTextChanged(QString txt);
        void buttonClicked(QAbstractButton*);
        void stateChanged(int);


    private:
        void changed();
        CNagr_treeDialogCallback *prop_callback = nullptr;
//@callback@

        void setRadioId(QButtonGroup * bg);
    public:
        void read();
        void write();
        void setCallback(CNagr_treeDialogCallback *c)
        {
            prop_callback = c;
        };

    private:
        SaveGeometry save;
    public:
        Ui::Nagr_treeDialog ui;
};

