#ifndef CUSTOMDATEINPUT_H
#define CUSTOMDATEINPUT_H

#include <QLineEdit>
#include <QToolButton>
#include <QCalendarWidget>
#include <QDate>
#include <QHBoxLayout>
#include <QFocusEvent>

class CustomDateInput : public QLineEdit
{
    Q_OBJECT

public:
    explicit CustomDateInput(QWidget *parent = nullptr);

    QDate date() const;
    void setDate(const QDate &date);

signals:
    void dateChanged(const QDate &date);

private slots:
    void showCalendar();
    void calendarDateSelected(const QDate &date);
    void onTextEdited(const QString &text);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QToolButton *calendarButton;
    QCalendarWidget *calendarPopup;

    void updateCalendarPosition();
    void validateAndEmitDate();
};

#endif // CUSTOMDATEINPUT_H
