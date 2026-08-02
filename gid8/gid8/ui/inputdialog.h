#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QDialog>

class QLineEdit;
class QLabel;

#include <dialog/SaveGeometry.h>

class InputDialog : public QDialog 
{
    Q_OBJECT
public:
    InputDialog(QStringList label, QStringList value, QWidget *parent = nullptr);

    QStringList getStrings(bool *ok = nullptr);

private:
    SaveGeometry save;
    QList<QLineEdit*> fields;
};

#endif // INPUTDIALOG_H
