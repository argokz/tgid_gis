#pragma once

#include <QDialog>
#include <QVariant>
#include <QtWidgets>

class QListWidget;
class QLineEdit;

#include <dialog/SaveGeometry.h>

struct MMenuItem
{
    QVariant id;
    QString txt;
};

class MMenuDial : public QDialog
{
    Q_OBJECT

public:
    MMenuDial(QWidget *parent, const QString & title);
    void Add(const QString & txt, QVariant id);

    void AddButton(const QString & txt, int id, bool choice = true);

    QList<MMenuItem> out;

    int cnt() { return lst.size(); };
    int index;
    QString curText;
    QVariant value() { return _value; };
    int button_clicked() { return _button_clicked; }

private slots:
    void okClicked();
    void buttonClicked(const QVariant &value, bool choice);
    void helpClicked();
    void textChanged(const QString &name);


private:
    QVBoxLayout *buttonBox;

    QListWidget lw;
    QLineEdit *lineEdit;
    QList<MMenuItem> lst;
    SaveGeometry save;
    int N;
    int _button_clicked = -1;
    QVariant _value;
};
