/*
 * Copyright (C) Martijn Koopman
 * All Rights Reserved
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 */

#ifndef RIBBONBUTTONGROUP_H
#define RIBBONBUTTONGROUP_H

#include <QWidget>
#include <QToolButton>

#include "WToolButton.h"

//namespace Ui {
//class RibbonButtonGroup;
//}

#include <QLabel>
#include <QHBoxLayout>


class TwoRowGridLayout : public QGridLayout {
public:
    explicit TwoRowGridLayout(QWidget *parent = nullptr) : QGridLayout(parent), currentIndex(0) {
//        setSpacing(10);
//        setContentsMargins(10, 10, 10, 10);
    }

    void addWidget(QWidget *widget) {
        int column = currentIndex / N3; // Вычисляем строку
        int row = currentIndex % N3; // Вычисляем столбец

        QGridLayout::addWidget(widget, row, column); // Вызываем метод базового класса

//        if (row == N3-1) {
//            addStretch();
//        }
        
        currentIndex++; // Увеличиваем индекс
    }

    void addWidget_full(QWidget *widget) {
        int column = currentIndex / N3; // Вычисляем строку
        int row = currentIndex % N3; // Вычисляем столбец
        if (row != 0) {
            row = 0;
            column += 1;
        }

        QGridLayout::addWidget(widget, row, column, N3, 1); // Вызываем метод базового класса
//        rowStretch();

        currentIndex = N3*(column+1); // Увеличиваем индекс
    }


private:
    int N3 = 3;
    int currentIndex; // Индекс для добавленных виджетов
};


class RibbonButtonGroup : public QWidget
{
  Q_OBJECT

public:
  explicit RibbonButtonGroup(QWidget *parent = 0);
  virtual ~RibbonButtonGroup();

  void setTitle(const QString &title);
  QString title() const;
  int buttonCount() const;
  void addButton(WToolButton *button, int typ);
  void removeButton(WToolButton *button);
  void adjustColumn();

  QHBoxLayout *horizontalLayout;

private:
//  Ui::RibbonButtonGroup *ui;
  QString m_title; ///< Title of the button group

//  QHBoxLayout *horizontalLayout;
//  TwoRowGridLayout  *horizontalLayout;
  QLabel *label;

};

#endif // RIBBONBUTTONGROUP_H
