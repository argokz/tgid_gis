#include <QApplication>
#include <QtGui>
#include <QtWidgets>

#include <ribbon/ribbon.h>


#include "MyMain.h"

MyMain::MyMain(const QString & title, QWidget *parent)
    : QWidget(parent, Qt::Window)
    , save(this, "MyMain", title)
{
    _menuBar = new QMenuBar();
    _toolBar = new QToolBar();
//    _ribbon = new Ribbon();

    setWindowTitle(title);

    _widget = nullptr;

    mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,0,0,0);

//    mainLayout->addWidget(_ribbon);

    _menuBar->setStyleSheet("QMenuBar::icon { width: 0px; padding-left: 0px; }");

    mainLayout->setMenuBar(_menuBar);
    mainLayout->addWidget(_toolBar);

//    mainLayout->addWidget(m_widget);
    setLayout(mainLayout);
//    resize(800, 600);
}

void MyMain::setWidget(QWidget *widget)
{
    _widget = widget;
    mainLayout->addWidget(_widget);
}

void MyMain::hideEvent(QHideEvent *event)
{    
    save.save();

    QWidget::hideEvent(event);
}
