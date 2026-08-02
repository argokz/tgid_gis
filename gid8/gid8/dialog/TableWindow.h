#pragma once

#include <QSqlQuery>
#include <QSqlDatabase>
#include <QString>

#include <dialog/SaveGeometry.h>

class QSqlTableModel;
class QSqlQueryModel;
class QTableView;

class TableWindow : public QDialog
{
    Q_OBJECT

public:
    TableWindow(QSqlDatabase &db, const QString & tn, const QString & title, const QString & que, QWidget *parent, int targetId = -1, bool find_str = true, bool ok_str = true, bool header = true, bool autofit = false);

    int value() { return m_id;};
    QVariant value(int i) { return m_v[i];};
    QVariant value(const QString & fn);

    void hide_column(int id);
    

private slots:
    void okClicked();
    void helpClicked();
    void textChanged(const QString & name);
    void activated(const QModelIndex &index);

private:
    void hideEvent(QHideEvent *event);
    void resizeEvent(QResizeEvent *event) override;

private:
    void adjustColumnWidths();

private:
    QString m_q;
    QString m_label;

    int m_id;
    std::vector<QVariant> m_v;
    std::map<QString, QVariant> m_map_v;

    QSqlDatabase *m_db;

    QSqlQueryModel *model;
    QTableView *view;
    QLineEdit *lineEdit;
    SaveGeometry save;
};
