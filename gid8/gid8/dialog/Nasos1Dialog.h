#pragma once

#include "qsqldatabase.h"
#include <QDialog>
#include <QtSql>

#include <dialog/SaveGeometry.h>

class DbWindow;

class Nasos1Dialog : public QDialog
{
    Q_OBJECT

public:
    Nasos1Dialog(QWidget *parent, QSqlDatabase &db, int id);

private slots:
    void okClicked();
    void b1Clicked();
    void b2Clicked();
    void b3Clicked();
    void b4Clicked();
    void b5Clicked();


//    void editClicked();
    void helpClicked();
//    void colorLClicked();
//    void colorBClicked();
//    void fontClicked();

    void currentIndexChanged(int index);


private:
//    CCxema *m_cxema;
    QSqlDatabase *m_db;

    QComboBox *combo;
    DbWindow *tb1;
    DbWindow *tb2;
    int m_id;

    SaveGeometry save;
};

