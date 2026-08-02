#pragma once

#include <QApplication>
#include <QWidget>
#include <QTreeWidget>
#include "QVBoxLayout"
#include "std.h"

#include <QDialog>
#include <dialog/SaveGeometry.h>

class QMenuBar;
class QToolBar;
class Ribbon;


class MyMain : public QWidget
{
    Q_OBJECT

public:
    MyMain(const QString & title, QWidget *parent = 0);
    void setWidget(QWidget *widget);

    QMenuBar *menuBar() { return _menuBar;};
    QToolBar *toolBar() { return _toolBar;};
//    Ribbon *ribbon() { return _ribbon;};

signals:

public slots:
    
protected:

private:
    void hideEvent(QHideEvent *event);

private:
    QVBoxLayout *mainLayout;
    QMenuBar* _menuBar;
    QToolBar* _toolBar;

//      Ribbon *_ribbon;
  
    QWidget *_widget;
    SaveGeometry save;
};
