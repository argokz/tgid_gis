#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QToolButton>
#include <QCalendarWidget>
#include <QHBoxLayout>
#include <QDate>
#include <QDateTimeEdit>
#include <QStyle>

class DateLineEditWithButton : public QWidget {
    Q_OBJECT

public:
    DateLineEditWithButton(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        lineEdit = new QLineEdit(this);
        lineEdit->setPlaceholderText("дд.мм.гггг");

        button = new QToolButton(this);
        button->setIcon(style()->standardIcon(QStyle::SP_ArrowDown));
        button->setCursor(Qt::ArrowCursor);
        button->setFocusPolicy(Qt::NoFocus);
        button->setFixedSize(20, 20);
        button->setStyleSheet("QToolButton { border: none; padding: 0px; }");

        // Установим layout с полем ввода и кнопкой
        auto layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        layout->addWidget(lineEdit);
        layout->addWidget(button);

        setFocusProxy(lineEdit);

        // Календарь
        calendar = new QCalendarWidget(this);
        calendar->setWindowFlags(Qt::Popup);
        calendar->setVisible(false);

        connect(button, &QToolButton::clicked, this, &DateLineEditWithButton::showCalendar);
        connect(calendar, &QCalendarWidget::clicked, this, &DateLineEditWithButton::dateSelected);
    }

    QDate date() const {
        return QDate::fromString(lineEdit->text(), "dd.MM.yyyy");
    }

    void setDate(const QDate &date) {
        lineEdit->setText(date.toString("dd.MM.yyyy"));
    }

    QString text() const {
        return lineEdit->text();
    }

    void clear() {
        lineEdit->clear();
    }

private slots:
    void showCalendar() {
        if (calendar->isVisible()) {
            calendar->hide();
        } else {
            QPoint pos = mapToGlobal(button->geometry().bottomRight());
            calendar->move(pos.x() - calendar->width(), pos.y());
            calendar->show();
            calendar->raise();
            calendar->setFocus();
        }
    }

    void dateSelected(const QDate &date) {
        lineEdit->setText(date.toString("dd.MM.yyyy"));
        calendar->hide();
    }

private:
    QLineEdit *lineEdit;
    QToolButton *button;
    QCalendarWidget *calendar;
};
