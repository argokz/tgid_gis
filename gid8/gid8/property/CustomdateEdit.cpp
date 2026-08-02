#include <QDateEdit>

#include "CustomdateEdit.h"


CustomDateEdit::CustomDateEdit(QWidget *parent) : QDateEdit(parent) {
    setDisplayFormat("dd.MM.yyyy");
    setMinimumDate(QDate(1800, 1, 1)); // Минимальная дата как "пустое" значение
    setSpecialValueText(" "); // Пустой текст для минимальной даты
    setDate(minimumDate()); // Устанавливаем "пустую" дату по умолчанию

    // Подключаем обработчик завершения редактирования
    connect(this, &QDateEdit::editingFinished, this, &CustomDateEdit::onEditingFinished);
    connect(lineEdit(), &QLineEdit::textChanged, this, &CustomDateEdit::onTextChanged);
}

void CustomDateEdit::focusInEvent(QFocusEvent *event)
{
    if (date() == minimumDate()) {
//        setDate(QDate::currentDate()); // Устанавливаем текущую дату
    }
    QDateEdit::focusInEvent(event); // Вызываем базовую реализацию
}

void CustomDateEdit::onEditingFinished()
{
    // Проверяем текст в lineEdit
    if (lineEdit()->text().isEmpty() || lineEdit()->text() == specialValueText()) {
        setDate(minimumDate()); // Устанавливаем минимальную дату
    }
}


void CustomDateEdit::onTextChanged(const QString &text) {
    if (text.trimmed().isEmpty() || text == specialValueText()) {
        setDate(minimumDate()); // Устанавливаем минимальную дату
        // Здесь можно считать, что дата "обнулилась"
        qDebug() << "Дата обнулена";
    }
}
