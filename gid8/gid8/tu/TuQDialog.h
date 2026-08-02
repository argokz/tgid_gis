#ifndef TUQDIALOG_H
#define TUQDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QMap>

#include <dialog/SaveGeometry.h>

class TuQDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TuQDialog(QWidget *parent = nullptr);

    // Добавление элемента в указанный список с кодом
    void addItemToList(int listIndex, int id, const QString &text, bool checked = false);

    // Получение состояния чекбоксов для всех списков по кодам
    QMap<int, QMap<int, bool>> getCheckedStates() const;
    QMap<int, QMap<QString, bool>> getCheckedStatesS() const;

    QMap<int, bool> getCheckedStates(int col) const;
    QMap<QString, bool> getCheckedStatesS(int col) const;

    QSet<int> getCheckedStates2(int col) const;
    QSet<QString> getCheckedStatesS2(int col) const;
    

    QString getQ() const;


protected:
    void addAllCheckBox(QVBoxLayout *column1Layout, QListWidget *listWidget1);

private:
    QListWidget *listWidget1;
    QListWidget *listWidget2;
    QListWidget *listWidget3;
    QListWidget *listWidget4;
    QDialogButtonBox *buttonBox;

    void setupList(QListWidget *listWidget, const QList<QPair<int, QString>> &items);

    SaveGeometry save;

};

#endif // TUQDIALOG_H