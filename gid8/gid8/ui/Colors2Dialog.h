#pragma once

#include <QtSql>

#include <QDialog>
#include "ui_Colors2Dialog.h"

class CColors2Dialog;
typedef bool (CColors2DialogCallback)(CColors2Dialog *prop);

#include <dialog/SaveGeometry.h>

class CColors2Dialog : public QDialog 
{
    Q_OBJECT

    public:
        CColors2Dialog(QWidget *parent = nullptr);
        QString m_labelColors_window;
        QString m_strLine_thickness;
        QString m_strDistance;
        QString m_strRing;
        bool m_bPunkt;
        int m_cmbOpen;
        int m_cmbMag_ras;
        int m_cmbTyp;
        int m_iLine_thickness;


    public slots:
        void okClicked();
        void textEdited(QString txt);
        void currentTextChanged(QString txt);
        void valueChanged(int i);
        void buttonClicked(QAbstractButton*);
        void stateChanged(int);
        void onPod();
        void onObr();
        void onDefault();
        void onPodf();
        void onObrf();
        void onColor_uzel();
        void onDefault2();


    private:
        void changed();
        void changed0();
        CColors2DialogCallback *prop_callback = nullptr;
        CColors2DialogCallback *prop_callback0 = nullptr;
//@callback@

        void setRadioId(QButtonGroup * bg);
    public:
        void read();
        void write();
        void setCallback(CColors2DialogCallback *c)
        {
            prop_callback = c;
        };

        void setCallback0(CColors2DialogCallback *c)
        {
            prop_callback0 = c;
        };

    private:
        SaveGeometry save;
    public:
        Ui::Colors2Dialog ui;
};

