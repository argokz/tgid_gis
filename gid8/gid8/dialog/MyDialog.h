#pragma once

#include <QDialog>
#include <QVariant>
#include <QtWidgets>

class QListWidget;
class QLineEdit;

struct EditValue
{
    QWidget *w;
    QMetaType type;
};

class MyDialog : public QDialog
{
    Q_OBJECT

public:
    MyDialog(QWidget *parent, const QString & title);

protected:
    QWidget *getWidget(int id);
    QString getValueStr(int id);
    int getValueInt(int id);
    int getCurentCombo(int id);

    QLineEdit *addLineEdit(int id, QVariant value, int width);
    QComboBox *addComboBox(int id, int width);
    
    QWidget *addRow(QBoxLayout *layout, int id, const QString & text, int width, QWidget *w);
    QLineEdit *addRowLineEdit(QBoxLayout *layout, int id, const QString & text, QVariant value, int width1, int width2, bool enable=true);

    QIcon *actionIcon = nullptr;

private slots:

private:
    std::map<int, EditValue> map_v;
};
