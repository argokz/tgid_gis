#include "CustomDateInput.h"
#include <QDate>
#include <QApplication>

CustomDateInput::CustomDateInput(QWidget *parent)
    : QLineEdit(parent),
      calendarButton(new QToolButton(this)),
      calendarPopup(new QCalendarWidget(nullptr))
{
    setPlaceholderText("дд.мм.гггг");
    setInputMask("00.00.0000;_"); // точка разрешена

    // Кнопка календаря
    calendarButton->setCursor(Qt::ArrowCursor);
    calendarButton->setFocusPolicy(Qt::NoFocus);
    calendarButton->setStyleSheet("QToolButton { border: none; icon-size: 16px; }");
//    calendarButton->setIcon(QIcon(":/icons/calendar.png")); // укажите свою иконку
//    calendarButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarUnshadeButton));

    calendarButton->setText("📅");

    connect(calendarButton, &QToolButton::clicked, this, &CustomDateInput::showCalendar);
    connect(calendarPopup, &QCalendarWidget::clicked, this, &CustomDateInput::calendarDateSelected);
    connect(this, &QLineEdit::editingFinished, this, &CustomDateInput::validateAndEmitDate);
    connect(this, &QLineEdit::textEdited, this, &CustomDateInput::onTextEdited);

    calendarPopup->setWindowFlags(Qt::Popup);
    calendarPopup->hide();
}

void CustomDateInput::resizeEvent(QResizeEvent *event)
{
    QLineEdit::resizeEvent(event);
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    QSize sz = calendarButton->sizeHint();
    calendarButton->move(rect().right() - sz.width() - frameWidth, (rect().height() - sz.height()) / 2);
}

void CustomDateInput::showCalendar()
{
    updateCalendarPosition();
    calendarPopup->setSelectedDate(date().isValid() ? date() : QDate::currentDate());
    calendarPopup->show();
    calendarPopup->setFocus();
}

void CustomDateInput::updateCalendarPosition()
{
    QPoint pos = mapToGlobal(QPoint(0, height()));
    calendarPopup->move(pos);
}

void CustomDateInput::calendarDateSelected(const QDate &selectedDate)
{
    setText(selectedDate.toString("dd.MM.yyyy"));
    calendarPopup->hide();
    emit dateChanged(selectedDate);
}

void CustomDateInput::onTextEdited(const QString &text)
{
//    Q_UNUSED(text);
    // можно добавить валидацию по мере ввода

   if (text.isEmpty() || text == "..") {
        setStyleSheet("");  // без ошибок
        emit dateChanged(QDate());
        return;
    }

    QDate date = QDate::fromString(text, "dd.MM.yyyy");
    if (date.isValid()) {
        setStyleSheet("");  // Ок
        emit dateChanged(date);
    } else {
        setStyleSheet("color: red;");
    }
}

void CustomDateInput::validateAndEmitDate()
{
    QDate d = QDate::fromString(text(), "dd.MM.yyyy");
    if (d.isValid()) {
        emit dateChanged(d);
    } else if (text().trimmed().isEmpty()) {
        emit dateChanged(QDate());  // пустая дата
    }
}

QDate CustomDateInput::date() const
{
    QDate d = QDate::fromString(text(), "dd.MM.yyyy");
    return d.isValid() ? d : QDate();
}

void CustomDateInput::setDate(const QDate &date)
{
    if (date.isValid()) {
        setText(date.toString("dd.MM.yyyy"));
    } else {
        clear();
    }
}
