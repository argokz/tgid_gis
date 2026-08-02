#ifndef PUMPCOMPUTEDIALOG_H
#define PUMPCOMPUTEDIALOG_H

#include <QDialog>
#include <QtGui>
#include <QtWidgets>
#include "qsqldatabase.h"
#include <QtSql>
#include "pumps/pumpgraphwidget.h"
#include <pumps/PumpReport.h>

#include <dialog/SaveGeometry.h>

namespace Ui {
class PumpComputeDialog;
}

class PumpComputeDialog : public QDialog 
{
    Q_OBJECT

public:
    explicit PumpComputeDialog(QWidget *parent, QSqlDatabase &db, int pumpId, bool approximateMode);
    ~PumpComputeDialog();

public slots:
    void stateCharN(int);
    void stateCharP(int);
    void clickReport();
    void okClicked();

signals:
    void dialogAccepted();

private:
    Ui::PumpComputeDialog *ui;
    PumpGraphWidget* paintWidget;

public:
    PUMP_REPORT pReport;

private:
    QSqlDatabase *m_db;
    bool mApproximateMode;
    SaveGeometry save;

public:
    void addGraph(int id, GRAPHR_PARAM grApr, int k_kol);
};

#endif // PUMPCOMPUTEDIALOG_H
