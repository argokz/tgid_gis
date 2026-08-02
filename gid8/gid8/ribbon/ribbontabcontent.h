/*
 * Copyright (C) Martijn Koopman
 * All Rights Reserved
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 */

#ifndef RIBBONTABCONTENT_H
#define RIBBONTABCONTENT_H

#include <QWidget>
#include <QHBoxLayout>
#include <QToolButton>

#include "WToolButton.h"


//namespace Ui {
//class RibbonTabContent;
//}



class AutoGridLayout : public QGridLayout {
  Q_OBJECT
public:
    AutoGridLayout(int rows, int columns, QWidget *parent = nullptr)
        : QGridLayout(parent), rows(rows), columns(columns), currentRow(0), currentColumn(0) {}

    // Метод для добавления виджета по порядку
    void addWidgetAuto(QWidget *widget) {
        addWidget(widget, currentRow, currentColumn);
        setRowStretch(currentRow, 1); 
        advancePosition();
    }

private:
    int rows;
    int columns;
    int currentRow;
    int currentColumn;

    // Метод для перехода к следующей позиции
    void advancePosition() {
        currentRow++;
        if (currentRow >= rows) {
            currentRow = 0;
            currentColumn++;
        }


/*
        currentColumn++;
        if (currentColumn >= columns) {
            currentColumn = 0;
            currentRow++;
        }
*/
    }
};
class RibbonTabContent : public QWidget
{
  Q_OBJECT

public:
     explicit RibbonTabContent(QWidget *parent = 0);
     virtual ~RibbonTabContent();

     void addGroup(const QString &groupName);
     void removeGroup(const QString &groupName);
     int groupCount() const;
     void addButton(const QString &groupName, WToolButton *button, int typ);
     void removeButton(const QString &groupName, WToolButton *button);
     AutoGridLayout *addNest(int w, int h);

     void adjustColumn();

    QHBoxLayout *ribbonHorizontalLayout;

private:
//  Ui::RibbonTabContent *ui;

//    QHBoxLayout *ribbonHorizontalLayout;
//    QGridLayout *ribbonHorizontalLayout;


};

#endif // RIBBONTABCONTENT_H
