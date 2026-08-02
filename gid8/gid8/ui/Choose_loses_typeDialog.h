#pragma once

#include <QtSql>

#include <QDialog>
#include "ui_Choose_loses_typeDialog.h"

class CChoose_loses_typeDialog;
typedef bool (CChoose_loses_typeDialogCallback)(CChoose_loses_typeDialog *prop);

#include <dialog/SaveGeometry.h>

class CChoose_loses_typeDialog : public QDialog 
{
    Q_OBJECT

    public:
        CChoose_loses_typeDialog(QWidget *parent = nullptr);
        bool m_bFragment;
        int m_cmbCombo1;
        int m_radioRadio1;
        QDate m_dtDatetimepicker1;
        QDate m_dtDatetimepicker2;


    public slots:
        void okClicked();
        void textChanged(QString txt);
        void currentTextChanged(QString txt);
        void buttonClicked(QAbstractButton*);
        void stateChanged(int);
        void onAdd_heating_date();
        void onDel();


    private:
        void changed();
        CChoose_loses_typeDialogCallback *prop_callback = nullptr;
//@callback@

        void setRadioId(QButtonGroup * bg);
    public:
        void read();
        void write();
        void setCallback(CChoose_loses_typeDialogCallback *c)
        {
            prop_callback = c;
        };

    private:
        SaveGeometry save;
    public:
        Ui::Choose_loses_typeDialog ui;
};

