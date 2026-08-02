#pragma once

#include <QDialog>
class QSqlDatabase;

#include <dialog/SaveGeometry.h>

class MestSopr2 : public QDialog
{
    Q_OBJECT

public:
    MestSopr2(QWidget *parent, QSqlDatabase &db, int id);

private slots:
//    void okClicked();
//    void editClicked();
//    void onClicked();
//    void offClicked();
//    void helpClicked();

    void onAdd();
    void onDel();

private:
//    QListWidget *lw;
    SaveGeometry save;
};
