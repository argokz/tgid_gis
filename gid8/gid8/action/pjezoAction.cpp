#include <QtGui>
#include <QtWidgets>

#include "std.h"

#include "mainwindow.h"
#include <gidview/GidWidget.h>
#include "pjezo.h"

struct PjezoAction pjAction;
extern struct MainAction mainAction;


#if 0

void MainWindow::createActionsPjezo()
{
/*
pjAction.leftAction = new QAction(tr("Left"), this);
pjAction.leftAction->setShortcut(Qt::Key_Left);
connect(pjAction.leftAction, SIGNAL(triggered()), this, SLOT(moveLeft()));

pjAction.rightAction = new QAction(tr("Right"), this);
pjAction.rightAction->setShortcut(Qt::Key_Right);
connect(pjAction.rightAction, SIGNAL(triggered()), this, SLOT(moveRight()));
*/

pjAction.file_closeAction = new QAction(tr("&Закрыть"), this);
pjAction.file_closeAction->setIcon(QIcon(":/images/file_close.png"));
connect(pjAction.file_closeAction, SIGNAL(triggered()), this, SLOT(unknown()));

//pjAction.file_saveAction = new QAction(tr("&Сохранить"), this);
//pjAction.file_saveAction->setShortcut(Qt::Key_Right);
//connect(pjAction.file_saveAction, SIGNAL(triggered()), this, SLOT(save()));

pjAction.file_save_asAction = new QAction(tr("Сохранить &как..."), this);
pjAction.file_save_asAction->setIcon(QIcon(":/images/file_save_as.png"));
connect(pjAction.file_save_asAction, SIGNAL(triggered()), this, SLOT(moveRight()));

pjAction.file_printAction = new QAction(tr("&Печать...\tCtrl+P"), this);
pjAction.file_printAction->setIcon(QIcon(":/images/file_print.png"));
pjAction.file_printAction->setShortcut(QKeySequence::Print);
connect(pjAction.file_printAction, SIGNAL(triggered()), this, SLOT(print()));

pjAction.file_print_previewAction = new QAction(tr("Пред&варительный просмотр"), this);
pjAction.file_print_previewAction->setIcon(QIcon(":/images/file_print_preview.png"));
connect(pjAction.file_print_previewAction, SIGNAL(triggered()), this, SLOT(unknown()));

pjAction.file_print_setupAction = new QAction(tr("Пара&метры страницы..."), this);
pjAction.file_print_setupAction->setIcon(QIcon(":/images/file_print_setup.png"));
connect(pjAction.file_print_setupAction, SIGNAL(triggered()), this, SLOT(unknown()));

//pjAction.file_mru_file1Action = new QAction(tr("Повторно открыть файл"), this);
//pjAction.file_mru_file1Action->setShortcut(Qt::Key_Right);
//connect(pjAction.rightAction, SIGNAL(triggered()), this, SLOT(moveRight())); GRAYED


pjAction.edit_copyAction = new QAction(tr("&Копировать"), this);
pjAction.edit_copyAction->setIcon(QIcon(":/images/copy.png"));
pjAction.edit_copyAction->setShortcut(QKeySequence::Copy);
connect(pjAction.edit_copyAction, SIGNAL(triggered()), this, SLOT(moveRight()));

pjAction.masallAction = new QAction(tr("masall"), this);
pjAction.masallAction->setIcon(QIcon(":/images/masall.png"));
pjAction.masallAction->setShortcut(Qt::Key_F3);
connect(pjAction.masallAction, SIGNAL(triggered()), this, SLOT(OnMasall()));

pjAction.plusAction = new QAction(tr("Увеличить"), this);
pjAction.plusAction->setIcon(QIcon(":/images/plus.png"));
pjAction.plusAction->setShortcut(Qt::Key_Plus);
connect(pjAction.plusAction, SIGNAL(triggered()), this, SLOT(OnPlus()));

pjAction.minusAction = new QAction(tr("Уменьшить"), this);
pjAction.minusAction->setIcon(QIcon(":/images/minus.png"));
pjAction.minusAction->setShortcut(Qt::Key_Minus);
connect(pjAction.minusAction, SIGNAL(triggered()), this, SLOT(OnMinus()));

pjAction.oknoAction = new QAction(tr("Информация"), this);
pjAction.oknoAction->setIcon(QIcon(":/images/okno.png"));
connect(pjAction.oknoAction, SIGNAL(triggered()), this, SLOT(moveRight()));

pjAction.naporAction = new QAction(tr("Напоры"), this);
pjAction.naporAction->setIcon(QIcon(":/images/napor.png"));
connect(pjAction.naporAction, SIGNAL(triggered()), this, SLOT(OnNapor()));

pjAction.otmetAction = new QAction(tr("Отметки"), this);
pjAction.otmetAction->setIcon(QIcon(":/images/otmet.png"));
connect(pjAction.otmetAction, SIGNAL(triggered()), this, SLOT(OnOtmet()));

pjAction.statAction = new QAction(tr("Отметки"), this);
pjAction.statAction->setIcon(QIcon(":/images/stat.png"));
connect(pjAction.statAction, SIGNAL(triggered()), this, SLOT(OnStat()));

pjAction.vysotAction = new QAction(tr("Здания"), this);
pjAction.vysotAction->setIcon(QIcon(":/images/vysot.png"));
connect(pjAction.vysotAction, SIGNAL(triggered()), this, SLOT(OnVysot()));

pjAction.move_toAction = new QAction(tr("Здания"), this);
pjAction.move_toAction->setIcon(QIcon(":/images/move_to.png"));
connect(pjAction.move_toAction, SIGNAL(triggered()), this, SLOT(OnMoveTo()));


pjAction.vysotAction = new QAction(tr("Здания"), this);
pjAction.vysotAction->setIcon(QIcon(":/images/vysot.png"));
connect(pjAction.vysotAction, SIGNAL(triggered()), this, SLOT(OnVysot()));

pjAction.techinfoAction = new QAction(tr("Тех.информация"), this);
pjAction.techinfoAction->setIcon(QIcon(":/images/real_p.png"));
connect(pjAction.techinfoAction, SIGNAL(triggered()), this, SLOT(moveRight()));

pjAction.savepjezoAction = new QAction(tr("Сохранить направление"), this);
connect(pjAction.savepjezoAction, SIGNAL(triggered()), this, SLOT(moveRight()));

pjAction.view_toolbarAction = new QAction(tr("Панели &инструментов"), this);
connect(pjAction.view_toolbarAction, SIGNAL(triggered()), this, SLOT(moveRight()));

pjAction.view_status_barAction = new QAction(tr("Полоса &статуса"), this);
connect(pjAction.view_status_barAction, SIGNAL(triggered()), this, SLOT(moveRight()));

}


void MainWindow::createMenusPjezo()
{
/*  
    QMenu *menu;
    QMenu *subMenu;

    menu = menuBar()->addMenu(tr("Файл"));
    menu->addAction(pjAction.file_newAction);
    menu->addSeparator();
    
    menu->addAction(pjAction.file_openAction);
    menu->addAction(pjAction.file_closeAction);
//    menu->addAction(pjAction.file_saveAsAction);
    menu->addSeparator();
    menu->addAction(pjAction.importAction);
    menu->addAction(pjAction.add_fileAction);
    menu->addSeparator();
    menu->addAction(pjAction.bmpAction);
    menu->addSeparator();

    menu->addAction(pjAction.file_printAction);
    menu->addAction(pjAction.file_print_previewAction);
    menu->addAction(pjAction.file_print_setupAction);

    separatorAction = menu->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
        menu->addAction(recentFileActions[i]);
    menu->addSeparator();
    menu->addAction(pjAction.app_exitAction);

*/

    QMenu *menu;
    QMenu *subMenu;

        menu = menuBar()->addMenu(tr("&Файл"));
    //    list_menu_pjezo.append(menu);
    // BEGIN
//         MENUITEM "Созд&ать\tCtrl+N",            ID_FILE_NEW
menu->addAction(mainAction.file_newAction);
//         MENUITEM "&Открыть...\tCtrl+O",         ID_FILE_OPEN
menu->addAction(mainAction.file_openAction);
//         MENUITEM "&Закрыть",                    ID_FILE_CLOSE
menu->addAction(pjAction.file_closeAction);
//         MENUITEM "&Сохранить\tCtrl+S",          ID_FILE_SAVE
menu->addAction(pjAction.file_saveAction);
//         MENUITEM "Сохранить &как...",           ID_FILE_SAVE_AS
menu->addAction(pjAction.file_save_asAction);
    menu->addSeparator();
//         MENUITEM "Конвертор Gid99w",            ID_IMPORT
menu->addAction(mainAction.importAction);
    menu->addSeparator();
//         MENUITEM "&Печать...\tCtrl+P",          ID_FILE_PRINT
menu->addAction(pjAction.file_printAction);
//         MENUITEM "Пред&варительный просмотр",   ID_FILE_PRINT_PREVIEW
menu->addAction(pjAction.file_print_previewAction);
//         MENUITEM "Пара&метры страницы...",      ID_FILE_PRINT_SETUP
menu->addAction(pjAction.file_print_setupAction);
    menu->addSeparator();
//         MENUITEM "Повторно открыть файл",       ID_FILE_MRU_FILE1, GRAYED
//menu->addAction(pjAction.file_mru_file1, grayedAction);
//    menu->addSeparator();
//         MENUITEM "В&ыход",                      ID_APP_EXIT
menu->addAction(mainAction.app_exitAction);
    // END
        menu = menuBar()->addMenu(tr("&Правка"));
    //    list_menu_pjezo.append(menu);
    // BEGIN
//         MENUITEM "&Отменить\tCtrl+Z",           ID_EDIT_UNDO
menu->addAction(pjAction.edit_undoAction);
    menu->addSeparator();
//         MENUITEM "&Вырезать\tCtrl+X",           ID_EDIT_CUT
menu->addAction(pjAction.edit_cutAction);
//         MENUITEM "&Копировать\tCtrl+C",         ID_EDIT_COPY
menu->addAction(pjAction.edit_copyAction);
//         MENUITEM "Вст&авить\tCtrl+V",           ID_EDIT_PASTE
menu->addAction(pjAction.edit_pasteAction);
    // END
        menu = menuBar()->addMenu(tr("Изображение"));
    //    list_menu_pjezo.append(menu);
    // BEGIN
//         MENUITEM "Увеличить",                   ID_PLUS
menu->addAction(pjAction.plusAction);
//         MENUITEM "Уменьшить",                   ID_MINUS
menu->addAction(pjAction.minusAction);
//         MENUITEM "Информация",                  ID_OKNO
menu->addAction(pjAction.oknoAction);
    // END
        menu = menuBar()->addMenu(tr("Пьезометр"));
    //    list_menu_pjezo.append(menu);
    // BEGIN
//         MENUITEM "Напоры",                      ID_NAPOR
menu->addAction(pjAction.naporAction);
//         MENUITEM "Отметки",                     ID_OTMET
menu->addAction(pjAction.otmetAction);
//         MENUITEM "Здания",                      ID_VYSOT
menu->addAction(pjAction.vysotAction);
//         MENUITEM "Тех.информация",              ID_TECHINFO
menu->addAction(pjAction.techinfoAction);
//         MENUITEM "Сохранить направление",       ID_SAVEPJEZO
menu->addAction(pjAction.savepjezoAction);
    // END
        menu = menuBar()->addMenu(tr("&Вид"));
    //    list_menu_pjezo.append(menu);
    // BEGIN
//         MENUITEM "Панели &инструментов",        ID_VIEW_TOOLBAR
menu->addAction(pjAction.view_toolbarAction);
//         MENUITEM "Полоса &статуса",             ID_VIEW_STATUS_BAR
menu->addAction(pjAction.view_status_barAction);
    // END
        menu = menuBar()->addMenu(tr("&Окно"));
    //    list_menu_pjezo.append(menu);

menu->addAction(mainAction.tileAction);
menu->addAction(mainAction.cascadeAction);
menu->addAction(mainAction.nextAction);
menu->addAction(mainAction.previousAction);

        menu = menuBar()->addMenu(tr("&?"));
    //    list_menu_pjezo.append(menu);
    // BEGIN
//         MENUITEM "&Содержание",                 ID_HELP_FINDER
menu->addAction(mainAction.help_finderAction);
    menu->addSeparator();
//         MENUITEM "&О программе...",             ID_APP_ABOUT
menu->addAction(mainAction.app_aboutAction);
    // END
// END


}


void MainWindow::createToolBarsPjezo()
{

    pjezoToolBar = addToolBar(tr("Pjezo"));
//pjezoToolBar->addAction(pjAction.leftAction);
//pjezoToolBar->addAction(pjAction.rightAction);


pjezoToolBar->addAction(mainAction.file_newAction);
pjezoToolBar->addAction(mainAction.file_openAction);
//pjezoToolBar->addAction(pjAction.file_saveAction);
pjezoToolBar->addSeparator();
pjezoToolBar->addAction(pjAction.naporAction);
pjezoToolBar->addSeparator();
pjezoToolBar->addAction(pjAction.otmetAction);
pjezoToolBar->addAction(pjAction.statAction);
pjezoToolBar->addAction(pjAction.vysotAction);
pjezoToolBar->addAction(pjAction.oknoAction);
pjezoToolBar->addSeparator();
pjezoToolBar->addAction(pjAction.move_toAction);
pjezoToolBar->addSeparator();
pjezoToolBar->addAction(pjAction.techinfoAction);
pjezoToolBar->addAction(pjAction.plusAction);
pjezoToolBar->addAction(pjAction.minusAction);
pjezoToolBar->addAction(pjAction.masallAction);
pjezoToolBar->addSeparator();
pjezoToolBar->addAction(pjAction.file_printAction);
pjezoToolBar->addAction(pjAction.file_print_previewAction);

//pjezoToolBar->setIconSize ( QSize(16, 16) );


//    m_bars.append(pjezoToolBar);

//    pjezoToolBar = pjezoToolBar;

/*
tableToolBar = addToolBar(tr("Db"));


tableToolBar->addAction(pjAction.open_table_sourceAction);
tableToolBar->addAction(pjAction.open_table_resultAction);
tableToolBar->addAction(pjAction.view_spravinfoAction);
tableToolBar->addAction(pjAction.view_standinfoAction);
tableToolBar->addSeparator();
tableToolBar->addAction(pjAction.edit_cutAction);
tableToolBar->addAction(pjAction.edit_copyAction);
tableToolBar->addAction(pjAction.edit_pasteAction);
tableToolBar->addSeparator();
tableToolBar->addAction(pjAction.edit_findAction);
tableToolBar->addAction(pjAction.edit_repeatAction);
tableToolBar->addAction(pjAction.edit_replaceAction);
tableToolBar->addSeparator();
tableToolBar->addAction(pjAction.column_sortAction);
tableToolBar->addSeparator();
tableToolBar->addAction(pjAction.move_to2Action);
tableToolBar->addSeparator();
tableToolBar->addAction(pjAction.excelAction);
tableToolBar->addSeparator();
tableToolBar->addAction(pjAction.file_printAction);
tableToolBar->addAction(pjAction.file_print_previewAction);
tableToolBar->addAction(pjAction.switch_recordsAction);

*/

}

#endif

