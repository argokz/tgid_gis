/*
 * DatePopup.cpp
 *
 *  Created on: Aug 29, 2009
 *      Author: jordenysp
 */

#include <QtGui>
#include <QVBoxLayout>
#include <QCalendarWidget>
#include <QDialogButtonBox>
#include "DatePopup.h"

DatePopup::DatePopup(QWidget *parent)
:QDialog(parent, Qt::Popup)
{
    setSizeGripEnabled(false);
//    resize(260, 230);
    resize(310, 220);
    widget = new QWidget(this);
    widget->setObjectName("widget");
//    widget->setGeometry(QRect(0, 10, 258, 215));

    verticalLayout = new QVBoxLayout(widget);
    verticalLayout->setObjectName("verticalLayout");
    verticalLayout->setContentsMargins(0, 0, 0, 0);

    calendarWidget = new QCalendarWidget(widget);
    calendarWidget->setObjectName("calendarWidget");

    verticalLayout->addWidget(calendarWidget);


    QObject::connect(calendarWidget, SIGNAL(activated(QDate)), this, SLOT(select_date(QDate)));

/*
    buttonBox = new QDialogButtonBox(widget);
    buttonBox->setObjectName("buttonBox");
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    verticalLayout->addWidget(buttonBox);

//    adjustSize();

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
*/
}

void DatePopup::select_date(QDate date)
{
    done(QDialog::Accepted);
}

void DatePopup::setSelectedDate(QDate date) {
      calendarWidget->setSelectedDate(date);
}



QDate DatePopup::selectedDate() const{
    return calendarWidget->selectedDate();
}
