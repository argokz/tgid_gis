#pragma once

#include <QDialog>
#include <QVariant>
#include <QtWidgets>

class CCxema;
class QListWidget;
class QLineEdit;
class QSqlDatabase;
class CGraph2;
class GidWidget;

#include <dialog/SaveGeometry.h>


class NoVisual : public QDialog
{
    Q_OBJECT

public:
    NoVisual(QWidget *parent, const QString & title, int fileID, CCxema * cxema, QSqlDatabase &db, CGraph2 *graph, GidWidget *gid);
    void Add(const QString & txt, QVariant id);

    void AddButton(const QString & txt, int id);


protected slots:
    virtual void customContextMenuRequested(const QPoint &pos);

private slots:
    void okClicked();
    void helpClicked();

    void onAdd();
    void onEdit();
    void onTg();
    void onDelete();
    void onMoveTo();

    void onPropertyAccepted();

private:
    void hideEvent(QHideEvent *event);

    QSqlDatabase *m_db;

    CCxema *m_cxema;

    QVBoxLayout *buttonBox;

    QIcon groupIcon;
    QIcon keyIcon;

    QTreeWidget tree;
    SaveGeometry save;

    int m_fileID = -1;
    GidWidget *gid = nullptr;


    QTreeWidgetItem *i_CT;
    QTreeWidgetItem *i_FR;
    QTreeWidgetItem *i_RS;
    QTreeWidgetItem *i_UR;
    QTreeWidgetItem *i_TR;
    QTreeWidgetItem *i_KV;

    QTreeWidgetItem *i_IST1;
    QTreeWidgetItem *i_IST2;
    QTreeWidgetItem *i_IST3;

};
