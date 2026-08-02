#include <QtGui>
#include <QtSql>
#include <QtWidgets>
#include <gidview/GidWidget.h>
#include <gidview/colorgtd.h>
#include <any/MyMain.h>

#include "mainwindow.h"
#include "Pjezo.h"


void CPjezo::createMenu(MyMain *my_main_window)
{
    QMenu *menu1;

    QMenuBar *menu = my_main_window->menuBar();
    menu->clear();

    menu1 = menu->addMenu(tr("Файл"));
//    map_menu[pjezoAction].push_back(menu1);
        menu1->addAction(pjezoAction.aListPjezo); // Список направлений...
        menu1->addSeparator();
        menu1->addAction(pjezoAction.aFilePrint); // Печать...\tCtrl+P
        menu1->addAction(pjezoAction.aFilePrintPreview); // Предварительный просмотр
//        menu1->addAction(pjezoAction.aFilePrintSetup); // Параметры страницы...
        menu1->addSeparator();
        menu1->addAction(pjezoAction.aAppExit); // Выход
    menu1 = menu->addMenu(tr("Правка"));
//    map_menu[pjezoAction].push_back(menu1);
        menu1->addAction(pjezoAction.aEditCopy2); // Копировать\tCtrl+C
    menu1 = menu->addMenu(tr("Изображение"));
//    map_menu[pjezoAction].push_back(menu1);
        menu1->addAction(pjezoAction.aPlus); // Увеличить
        menu1->addAction(pjezoAction.aMinus); // Уменьшить
        menu1->addAction(pjezoAction.aOkno); // Информация
        menu1->addSeparator();
        menu1->addAction(pjezoAction.aPodacha); // Подающий
        menu1->addAction(pjezoAction.aObratka); // Обратный
        menu1->addAction(pjezoAction.aRealP); // Факт
        menu1->addAction(pjezoAction.aStat); // Статика
    menu1 = menu->addMenu(tr("Пьезометр"));
//    map_menu[pjezoAction].push_back(menu1);
        menu1->addAction(pjezoAction.aNapor); // Напоры
        menu1->addAction(pjezoAction.aOtmet); // Отметки
        menu1->addAction(pjezoAction.aVysot); // Здания
        menu1->addSeparator();
        menu1->addAction(pjezoAction.aExcel); // Таблицы Excel..
        menu1->addAction(pjezoAction.aTechinfo); // Тех.информация
        menu1->addAction(pjezoAction.aSavepjezo); // Сохранить направление
        menu1->addSeparator();
        menu1->addAction(pjezoAction.aDouble); // Двойной пьезометр
    menu1 = menu->addMenu(tr("Вид"));
//    map_menu[pjezoAction].push_back(menu1);
//        menu1->addAction(pjezoAction.aViewStatusBar); // Строка статуса
        menu1->addAction(pjezoAction.aInf); // Отображаемая информация...
        menu1->addAction(pjezoAction.aFont); // Шрифт
    menu1 = menu->addMenu(tr("&?"));
//    map_menu[pjezoAction].push_back(menu1);
        menu1->addAction(pjezoAction.aHelpFinder); // Содержание
        menu1->addSeparator();
        menu1->addAction(main_window->mainAction.aUpdate); // Последнее обновление
        menu1->addAction(main_window->mainAction.aUpdateSetup); // Настройка обновлений
        menu1->addAction(main_window->mainAction.aAppAbout); // О программе...
}


void CPjezo::createToolBar(MyMain *main_window)
{
    QToolBar *bar = main_window->toolBar();

    bar->addAction(pjezoAction.aFilePrintPreview);
    bar->addAction(pjezoAction.aFilePrint);
    bar->addAction(pjezoAction.aExcel);
    bar->addSeparator();
    bar->addAction(pjezoAction.aNapor);
    bar->addSeparator();
    bar->addAction(pjezoAction.aOtmet);
    bar->addAction(pjezoAction.aStat);
    bar->addAction(pjezoAction.aVysot);
    bar->addAction(pjezoAction.aOkno);
    bar->addSeparator();
    bar->addAction(pjezoAction.aMoveTo);
    bar->addSeparator();
    bar->addAction(pjezoAction.aRealP);
    bar->addAction(pjezoAction.aPlus);
    bar->addAction(pjezoAction.aMinus);
    bar->addAction(pjezoAction.aMasall);

    QWidget* empty = new QWidget();
    empty->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    bar->addWidget(empty);

    QAction *aExit = new QAction(tr("Закрыть вкладку"));
    aExit->setIcon(QIcon(":/images/db2/cross-svgrepo-com.svg"));
    aExit->setToolTip("Закрыть вкладку");
    bar->addAction(aExit);

    connect(aExit, SIGNAL(triggered()), this, SLOT(closeTable()));
}

void CPjezo::closeTable()
{
//    saveSettings();
//    this->closed = true;
    main_window->removeWidgetFromCenter(this->parentWidget());
    qDebug() << "Закрылся";
}



