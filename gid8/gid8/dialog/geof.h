#pragma once

#include <QDialog>

class QListWidget;
class QLineEdit;


class GeoFDial : public QDialog
{
    Q_OBJECT

public:
    GeoFDial(QStringList & _list, QWidget *parent);
    QString curText;
    int index;

private slots:
    void okClicked();
    void addClicked();
    void delClicked();
    void helpClicked();


private:
    QListWidget *lw;
    QStringList *list;
    int N;
};

