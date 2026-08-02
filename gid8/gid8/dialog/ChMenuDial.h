#pragma once

#include <QDialog>
#include <QtWidgets>

class QListWidget;
class QLineEdit;

#include <dialog/SaveGeometry.h>

struct ChMenuItem
{
    QVariant id;
    QString txt;
    bool on;
};


class ChMenuDial : public QDialog
{
    Q_OBJECT

public:
    ChMenuDial(QWidget *parent, const QString & title, bool _up_down = false);
    void Add(const QString & txt, QVariant id, bool on);
    void addButton(const QString & txt, QWidget *widget, const char *slot_name);

//    QList<ChMenuItem> out;
    QList<QVariant> out;

    int cnt() { return lw.count(); };

    bool inOut(QVariant id);

private slots:
    void okClicked();
    void helpClicked();
    void upClicked();
    void downClicked();
    
    void all();
    void ubr();


private:
    QListWidget lw;
//    QList<ChMenuItem> lst;
    SaveGeometry save;
    int N;
    bool up_down = false;

    QVBoxLayout *buttonBox;
};
