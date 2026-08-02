#ifndef SCOOTERWINDOW_H
#define SCOOTERWINDOW_H

#include <QDialog>

class QSqlTableModel;
class QSqlQueryModel;
class QTableView;

class ScooterWindow : public QDialog
{
    Q_OBJECT

public:
    ScooterWindow(const QString & dbn, const QString & tn, const QString & que, QWidget *parent);

private slots:
    void okClicked();
    void helpClicked();

private:
//    QSqlTableModel *model;
    QSqlQueryModel *model;
    QTableView *view;
};

#endif
