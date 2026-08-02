/*
 * Copyright (C) Martijn Koopman
 * All Rights Reserved
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 */

#include "ribbontabcontent.h"
//#include "ui_ribbontabcontent.h"
//#include "ribbonbuttongroup.h"


#include <QWidget>
#include <QGridLayout>
#include <QScrollArea>
#include <QHBoxLayout>

#include "mystd.h"


RibbonTabContent::RibbonTabContent(QWidget *parent)
  : QWidget(parent)
//  , ui(new Ui::RibbonTabContent)
{
//  ui->setupUi(this);

        // Установка геометрии
//    setGeometry(0, 0, 400, 90);
    setGeometry(0, 0, 10, 10);

    // Создание главного макета
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setSpacing(0);

    // Создание области прокрутки
    QScrollArea *ribbonTabScrollArea = new QScrollArea(this);
    ribbonTabScrollArea->setFrameShape(QFrame::NoFrame);
    ribbonTabScrollArea->setFrameShadow(QFrame::Plain);
    ribbonTabScrollArea->setLineWidth(0);
    ribbonTabScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ribbonTabScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ribbonTabScrollArea->setWidgetResizable(true);

    // Создание виджета для содержимого области прокрутки
    QWidget *ribbonTabScrollAreaContent = new QWidget();

    ribbonTabScrollAreaContent->setProperty("class", "RibbonTabContent");


//    ribbonTabScrollAreaContent->setGeometry(0, 0, 400, 90);
//    ribbonTabScrollAreaContent->setGeometry(0, 0, 10, 10);
    ribbonTabScrollAreaContent->setGeometry(0, 0, 0, 00);

    // Создание макета для содержимого
    QGridLayout *gridLayout2 = new QGridLayout(ribbonTabScrollAreaContent);
    gridLayout2->setContentsMargins(0, 2, 0, 2);
    gridLayout2->setSpacing(5);

    // Создание спейсера
    QWidget *spacer = new QWidget();
    
//    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    gridLayout2->addWidget(spacer, 0, 1);

    // Создание горизонтального макета
    ribbonHorizontalLayout = new QHBoxLayout();
//    ribbonHorizontalLayout = new QGridLayout();


    ribbonHorizontalLayout->setSpacing(5);
    ribbonHorizontalLayout->setContentsMargins(5, 2, 0, 2);
    gridLayout2->addLayout(ribbonHorizontalLayout, 0, 0);

    // Добавление области прокрутки в главный макет
    gridLayout->addWidget(ribbonTabScrollArea, 0, 0);
    ribbonTabScrollArea->setWidget(ribbonTabScrollAreaContent);


//    ribbonTabScrollArea->setFixedSize(sizeHint());

    QString styleSheetText = readFileUTF8("qss2/RibbonTabContent.qss");
    setStyleSheet(styleSheetText);
}


RibbonTabContent::~RibbonTabContent()
{
//  delete ui;
}

AutoGridLayout *RibbonTabContent::addNest(int rows, int columns)
{
    QWidget *nestedWidget = new QWidget;
    AutoGridLayout *nestedLayout = new AutoGridLayout(rows, columns, nestedWidget);

    return nestedLayout;
}


void RibbonTabContent::addButton(const QString &groupName, WToolButton *button, int typ)
{

//    button->setIconSize(QSize(48,48));
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setFixedHeight(60);
    button->setFixedWidth(60);

    ribbonHorizontalLayout->addWidget(button);

#if 0    
  
  // Find ribbon group
  RibbonButtonGroup *ribbonButtonGroup = nullptr;
  for (int i = 0; i < ribbonHorizontalLayout->count(); i++)
  {
    RibbonButtonGroup *group = static_cast<RibbonButtonGroup*>(ribbonHorizontalLayout->itemAt(i)->widget());
    if (group->title().toLower() == groupName.toLower())
    {
      ribbonButtonGroup = group;
      break;
    }
  }

  if (ribbonButtonGroup != nullptr)
  {
    // Group found
    // Add ribbon button
    ribbonButtonGroup->addButton(button, typ);
  }
  else
  {
    // Group not found
    // Add ribbon group
    addGroup(groupName);

    // Add ribbon button
    addButton(groupName, button, typ);
  }
#endif
}

#if 0
void RibbonTabContent::removeButton(const QString &groupName, WToolButton *button)
{

  // Find ribbon group
  RibbonButtonGroup *ribbonButtonGroup = nullptr;
  for (int i = 0; i < ribbonHorizontalLayout->count(); i++)
  {
    RibbonButtonGroup *group = static_cast<RibbonButtonGroup*>(ribbonHorizontalLayout->itemAt(i)->widget());
    if (group->title().toLower() == groupName.toLower())
    {
      ribbonButtonGroup = group;
      break;
    }
  }

  if (ribbonButtonGroup != nullptr)
  {
    // Group found
    // Remove ribbon button
    ribbonButtonGroup->removeButton(button);

    if (ribbonButtonGroup->buttonCount() == 0)
    {
      // Empty button group
      // Remove button group
      removeGroup(groupName);
    }
  }
}

#endif

void setColumnWidths(QGridLayout *layout);


void RibbonTabContent::adjustColumn()
{
/*
    for (int i = 0; i < ribbonHorizontalLayout->count(); i++)
    {
        RibbonButtonGroup *group = static_cast<RibbonButtonGroup*>(ribbonHorizontalLayout->itemAt(i)->widget());
        if (group) {
            group->adjustColumn();
        }
    }
*/

//    ribbonHorizontalLayout->addStretch();
}


