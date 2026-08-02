#ifndef TEXTFILTERDIALOG_H
#define TEXTFILTERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>

#include <dialog/SaveGeometry.h>


class TextFilterDialog : public QDialog {
    Q_OBJECT

public:
    explicit TextFilterDialog(QWidget *parent = nullptr);
    int getResult() const {
        return result;
    };

private slots:
    void onTextChanged(const QString &text);

private:
    int result = 0;
    
    QLineEdit *lineEdit;
//    QListWidget *listWidget;
    QListWidget *listWidget;

    QStringList getMatchingItems(const QString &text);

    SaveGeometry save;

};

#endif // TEXTFILTERDIALOG_H
