#pragma once

#include <QtSql>

#include <QDialog>
#include "ui_PotrcxemaDialog.h"
#include <gidview/rpdrawerwidget.h>
#include <dialog/SaveGeometry.h>

#include <dialog/SaveGeometry.h>

class CPotrcxemaDialog;
typedef bool (PropCallback)(CPotrcxemaDialog *prop);


class CPotrcxemaDialog : public QDialog 
{
    Q_OBJECT

    public:
        CPotrcxemaDialog(QWidget *parent = nullptr);
//    ConnectStr cs;
    QString m_labelNomer;
    bool m_bRez;
    bool m_bVentil;
    bool m_bOtopl;
    bool m_bGws;
    int m_radioZavis;
    int m_radioElevat;
    int m_radioPodacha;

protected:
    void paintEvent(QPaintEvent *event);

    public slots:
        void okClicked();
        void buttonClicked(QAbstractButton*);
        void stateChanged(int);


    private:
        void changed();
        PropCallback *prop_callback = nullptr;

        void setRadioId(QButtonGroup * bg);
    public:
        void read();
        void write();
//      void detach();
        void setCallback(PropCallback *c)
        {
            prop_callback = c;
        };
        void setSchemeNumber(QString number);


//    protected:
//        bool eventFilter ( QObject * o, QEvent * e );

    private:
        RPDrawerWidget *customWidget;
        SaveGeometry save;
    public:
        Ui::PotrcxemaDialog ui;
};

