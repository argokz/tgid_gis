#ifndef TYPEPUMPDIALOG_H
#define TYPEPUMPDIALOG_H

#include <QDialog>
#include <QtGui>
#include <QtWidgets>
#include "qsqldatabase.h"
#include <QtSql>
#include <pumps/PumpReport.h>

#include <dialog/SaveGeometry.h>


namespace Ui {
class TypePumpDialog;
}

class TypePumpDialog : public QDialog 
{
    Q_OBJECT

public:
    TypePumpDialog(QWidget *parent, QSqlDatabase &db, int initPumpId, int initCountPumps, int rotorDiameterTypeID, double diam_zad, int driveTypeID, double rate_zad);
    ~TypePumpDialog();

public slots:
    void currentIndexChanged(int index);
    void nextStepClicked();
    void onInnerDialogAccepted();

private:
    Ui::TypePumpDialog *ui;
    void updateComboBox(QComboBox* combo, int selectedId);
    void addTableTab(const QString& query, QString tabName);
    void addGraphTab(const QString& title, int id);

private:
    QSqlDatabase *m_db;
    int m_id;
    int count_pumps = 1;
    double d_nomin = 0.0;
    double rate_nomin = 0.0;

    double d_zad = 0.0;
    double rate_zad = 0.0;


    bool diam_b = false;  
    bool rate_b = false;  
    

    SaveGeometry save;

public:
    PUMP_REPORT pReport;
};

#endif // TYPEPUMPDIALOG_H
