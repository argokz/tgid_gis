#include "ReportInputDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

ReportInputDialog::ReportInputDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Введите данные для отчета");

    auto layout = new QVBoxLayout(this);

    // Дата отчета
    auto dateLayout = new QHBoxLayout();
    dateLayout->addWidget(new QLabel("Дата отчета:"));
    dateEdit = new QDateEdit(QDate::currentDate());
    dateEdit->setCalendarPopup(true);
    dateLayout->addWidget(dateEdit);
    layout->addLayout(dateLayout);

    // Начальный год
    auto startYearLayout = new QHBoxLayout();
    startYearLayout->addWidget(new QLabel("Начальный год отчета:"));
    startYearSpin = new QSpinBox();
    startYearSpin->setRange(1900, 2100);
    startYearSpin->setValue(QDate::currentDate().year() - 4);
    startYearLayout->addWidget(startYearSpin);
    layout->addLayout(startYearLayout);

    // Конечный год
    auto endYearLayout = new QHBoxLayout();
    endYearLayout->addWidget(new QLabel("Конечный год отчета:"));
    endYearSpin = new QSpinBox();
    endYearSpin->setRange(1900, 2100);
    endYearSpin->setValue(QDate::currentDate().year());
    endYearLayout->addWidget(endYearSpin);
    layout->addLayout(endYearLayout);

    // Год по источникам
    auto sourceYearLayout = new QHBoxLayout();
    sourceYearLayout->addWidget(new QLabel("Отчетный год по источникам:"));
    sourceYearSpin = new QSpinBox();
    sourceYearSpin->setRange(1900, 2100);
    sourceYearSpin->setValue(QDate::currentDate().year());
    sourceYearLayout->addWidget(sourceYearSpin);
    layout->addLayout(sourceYearLayout);

    // Коэффициент
    auto coefficientLayout = new QHBoxLayout();
    coefficientLayout->addWidget(new QLabel("Коэффициент:"));
    coefficientSpin = new QDoubleSpinBox();
    coefficientSpin->setRange(0.0, 100000.0);
    coefficientSpin->setDecimals(4);
    coefficientSpin->setValue(2.4);
    coefficientLayout->addWidget(coefficientSpin);
    layout->addLayout(coefficientLayout);

    // Кнопки OK / Cancel
    auto buttonLayout = new QHBoxLayout();
    auto okButton = new QPushButton("OK");
    auto cancelButton = new QPushButton("Отмена");
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

QDate ReportInputDialog::reportDate() const {
    return dateEdit->date();
}

int ReportInputDialog::startYear() const {
    return startYearSpin->value();
}

int ReportInputDialog::endYear() const {
    return endYearSpin->value();
}

int ReportInputDialog::sourceYear() const {
    return sourceYearSpin->value();
}

double ReportInputDialog::coefficient() const {
    return coefficientSpin->value();
}
