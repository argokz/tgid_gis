#pragma once

#include <QDialog>
#include <QDateEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>

class ReportInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReportInputDialog(QWidget *parent = nullptr);

    QDate reportDate() const;
    int startYear() const;
    int endYear() const;
    int sourceYear() const;
    double coefficient() const;

private:
    QDateEdit *dateEdit;
    QSpinBox *startYearSpin;
    QSpinBox *endYearSpin;
    QSpinBox *sourceYearSpin;
    QDoubleSpinBox *coefficientSpin;
};
