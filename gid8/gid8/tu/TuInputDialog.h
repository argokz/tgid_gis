#ifndef TUINPUTDIALOG_H
#define TUINPUTDIALOG_H

#include <QtSql>

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QTableView>
#include <QStandardItemModel>
#include <QTreeWidget>

#include <dialog/SaveGeometry.h>

class CCxema;

struct Nagr3
{
    double Qot, Qgvs, Qvent, Qpar;
};


class TuInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TuInputDialog(QWidget *parent,
    QSqlDatabase &db, const QString & title, const QString & table, int id, CCxema *m_cxema
    );

    void reject() override;

protected:
    void init();
    void init2();
    void read_data();

    void editTU(int id);


protected slots:
    virtual void customContextMenuRequested(const QPoint &pos);


private slots:
    void okClicked();
//    void cancelClicked();

    void onEdit();
    void onDelete();
    void onNenaid();
    void onPropertyAccepted();

    void onAddTUNew();
    void onAddTUOld();

    void change_tu(int tu_typ);



private:
    QComboBox *field1;
    QComboBox *field2;
    
    QLineEdit *field3;

    QComboBox *field4;
    QComboBox *field5;
    QComboBox *field6;
    QComboBox *field_ist;

    QPushButton *okButton;
    QPushButton *cancelButton;

    QPushButton *b2Button;
    QPushButton *b4Button;
    QPushButton *b5Button;
    QPushButton *b6Button;


    QTableView *tableView;
    QStandardItemModel *tableModel;

    QTreeWidget *tree1;
    QTreeWidget *tree2;


    int m_n1 = -1;  // Номер Здания ТУ

    QString m_gorod = "";
    QString m_raj = "";
    QString m_street = "";
    QString m_dom = "";
    int m_ist0 = -1;
    int m_rs0 = -1;
    QString m_comment = "";
    QString m_kod = "";
    QString m_uzel = "";

    std::map<QString, QString> map_str1;

    SaveGeometry save;

    QTreeWidgetItem *iD = nullptr;
    QTreeWidgetItem *iA = nullptr;
    QTreeWidgetItem *iZ = nullptr;


    CCxema *m_cxema;
};

#endif // TUINPUTDIALOG_H
