#pragma once

#include <QtSql>

#include <QDialog>
#include "ui_Klassif_editDialog.h"

class CKlassif_editDialog;
typedef bool (CKlassif_editDialogCallback)(CKlassif_editDialog *prop);

#include <dialog/SaveGeometry.h>

class CKlassif_editDialog : public QDialog 
{
    Q_OBJECT

    public:
        CKlassif_editDialog(QWidget *parent = nullptr);
        QString m_labelColor_w2;
        QString m_labelColor_w;
        QString m_labelWidth_static;
        QString m_strName;
        QString m_strPrio;
        QString m_strWidth;
        QString m_strRus;
        bool m_bLabel;
        bool m_bRamka;
        bool m_bPunct;
        bool m_bLabel_color;
        bool m_bNomas;
        int m_cmbList_label;
        int m_cmbList_label_color;
        int m_cmbLoc;
        int m_iPrio;
        int m_iWidth;


    public slots:
        void okClicked();
        void textChanged(QString txt);
        void currentTextChanged(QString txt);
        void buttonClicked(QAbstractButton*);
        void stateChanged(int);
        void onRastr();
        void onColor();
        void onColor2();
        void onFont();


    private:
        void changed();
        CKlassif_editDialogCallback *prop_callback = nullptr;
//@callback@

        void setRadioId(QButtonGroup * bg);
    public:
        void read();
        void write();
        void setCallback(CKlassif_editDialogCallback *c)
        {
            prop_callback = c;
        };

    private:
        SaveGeometry save;
    public:
        Ui::Klassif_editDialog ui;
};

