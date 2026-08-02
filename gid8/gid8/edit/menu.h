#pragma once

#include <QDialog>
#include <dialog/SaveGeometry.h>

class QListWidget;
class QLineEdit;


class MenuDial : public QDialog
{
    Q_OBJECT

public:
    MenuDial(const QStringList & list, const QString & title, QWidget *parent);
    QString curText;
    int index;

private slots:
    void okClicked();
    void helpClicked();
    void textChanged(const QString &name);


private:
    QListWidget *lw;
    QLineEdit *lineEdit;
    QStringList m_list;

    SaveGeometry save;
};
