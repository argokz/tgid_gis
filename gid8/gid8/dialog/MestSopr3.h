#pragma once

#include <QDialog>
class QSqlDatabase;

#include <dialog/SaveGeometry.h>

class MestSopr3 : public QDialog
{
    Q_OBJECT

public:
    MestSopr3(QWidget *parent, QSqlDatabase &db, int id, const QString & mestn);
    double getSum()  {return m_sum;};
    QString getSumList()  {return m_sum_list;};
    void parse(const QString & txt);

private slots:
    void okClicked();
//    void editClicked();
//    void onClicked();
//    void offClicked();
    void helpClicked();

    void vChanged(const QString &);

private:
    double calcSum();
    void initColors();

    QLineEdit *sopr;
    std::map<int, QSpinBox*> map_k;
    std::map<int, QDoubleSpinBox*> map_v;
    std::map<int, QLabel*> map_l;
    double m_sum = 0;
    QString m_sum_list = "";

//    QListWidget *lw;
    SaveGeometry save;


};
