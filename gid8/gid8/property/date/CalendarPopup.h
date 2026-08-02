#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QToolButton>
#include <QCalendarWidget>
#include <QHBoxLayout>
#include <QDate>
#include <QStyle>
#include <QApplication>
#include <QFocusEvent>
#include <QDialog>
#include <QVBoxLayout>

class CalendarPopup : public QDialog {
    Q_OBJECT
public:
    CalendarPopup(QWidget *parent = nullptr) : QDialog(parent, Qt::Popup) {
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        calendar = new QCalendarWidget(this);
        layout->addWidget(calendar);
        connect(calendar, &QCalendarWidget::clicked, this, &CalendarPopup::onDateSelected);
    }

    void setSelectedDate(const QDate &date) {
        calendar->setSelectedDate(date);
    }

signals:
    void dateSelected(const QDate &date);

private slots:
    void onDateSelected(const QDate &date) {
        emit dateSelected(date);
        close();
    }

private:
    QCalendarWidget *calendar;
};
