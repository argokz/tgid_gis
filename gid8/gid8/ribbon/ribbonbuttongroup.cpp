/*
 * Copyright (C) Martijn Koopman
 * All Rights Reserved
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 */

#include "ribbonbuttongroup.h"
//#include "ui_ribbonbuttongroup.h"

#include <QGridLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QToolButton>
#include "WToolButton.h"

#include "mystd.h"

void setColumnWidths(QGridLayout *layout) 
{
    int columnCount = layout->columnCount();

    // Массив для хранения максимальных ширин столбцов
    QList<int> maxWidths(columnCount, 0);

    // Проходим по всем элементам макета
    for (int row = 0; row < layout->rowCount(); ++row) {
        for (int col = 0; col < columnCount; ++col) {
            auto item = layout->itemAtPosition(row, col);
            if (item) {
                QWidget *widget = item->widget();
                if (widget) {
                    // Получаем ширину текущего элемента
                    int width = widget->sizeHint().width();
                    // Обновляем максимальную ширину для столбца
                    if (width > maxWidths[col]) {
                        maxWidths[col] = width;
                    }
                }
            }
        }
    }

    // Установка ширины столбцов
    for (int col = 0; col < columnCount; ++col) {
        for (int row = 0; row < layout->rowCount(); ++row) {
            auto item = layout->itemAtPosition(row, col);
            if (item) {
                QWidget *widget = item->widget();
                if (widget) {
                    widget->setFixedWidth(maxWidths[col]);
                }
            }
        }
//        layout->setColumnMinimumWidth(col, maxWidths[col]);
//        layout->setColumnStretch(col, 0); // Убираем растяжение, если нужно
    }
}

RibbonButtonGroup::RibbonButtonGroup(QWidget *parent)
  : QWidget(parent)
 // , ui(new Ui::RibbonButtonGroup)
  , m_title("")
{
        // Создаем основной layout
        horizontalLayout = new QHBoxLayout(this);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout->setSpacing(0);

        setObjectName("myRibbonButtonGroup");

        QString styleSheetText = readFileUTF8("qss2/RibbonButtonGroup.qss");
        setStyleSheet(styleSheetText);
}

RibbonButtonGroup::~RibbonButtonGroup()
{
}

void RibbonButtonGroup::setTitle(const QString &title)
{
  m_title = title;
//  label->setText(m_title);
}

QString RibbonButtonGroup::title() const
{
  return m_title;
}

int RibbonButtonGroup::buttonCount() const
{
  return horizontalLayout->count();
}

void RibbonButtonGroup::addButton(WToolButton *button, int typ)
{
    button->setIconSize(QSize(48,48));
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setFixedHeight(72);
    button->setFixedWidth(72);
    horizontalLayout->addWidget(button);


#if 0

//    if (typ == 3) {
        button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        button->setFixedHeight(24);
        horizontalLayout->addWidget(button);
//    }
//    else {
        button->setIconSize(QSize(48,48));
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setFixedHeight(72);
        button->setFixedWidth(72);
        horizontalLayout->addWidget_full(button);
    }
#endif

}


void RibbonButtonGroup::removeButton(WToolButton *button)
{
  /// \todo What happens if button is not part of the layout?
  horizontalLayout->removeWidget(button);
}


void RibbonButtonGroup::adjustColumn()
{
#if 0
    setColumnWidths(horizontalLayout);

    int rowCount = horizontalLayout->rowCount();
    for (int row = 0; row < rowCount; ++row) {
//        horizontalLayout->setRowStretch(row, 0);
        horizontalLayout->setRowMinimumHeight(0, 24);
        horizontalLayout->setRowStretch(row, 0);
    }

    if (rowCount < 3) {
        for (int r = rowCount; r < 3; r++) {
            QSpacerItem *item = new QSpacerItem(1, 24);
            horizontalLayout->addItem(item, 2, 0);
        }
    }


//    horizontalLayout->setRowStretch(rowCount, 0);

    // Устанавливаем коэффициенты растяжения для всех столбцов равными 0
    int columnCount = horizontalLayout->columnCount();
    for (int col = 0; col < columnCount; ++col) {
    }
#endif
}
