#pragma once

#include <QDateEdit>

class CustomDateEdit : public QDateEdit {
    Q_OBJECT
public:
    CustomDateEdit(QWidget *parent = nullptr);

protected:
    // Переопределяем метод обработки события получения фокуса
    void focusInEvent(QFocusEvent *event) override;

protected slots:
    void onEditingFinished();
    void onTextChanged(const QString &text);
};
