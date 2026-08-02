#include <QtGui>
#include <QtWidgets>

#include "std.h"

#include "mainwindow.h"
#include <gidview/GidWidget.h>

struct TableAction dbAction;
extern struct MainAction mainAction;

/*
void MainWindow::OnPlus()       {if (activeDb()) activeDb()->OnPlus();}       
void MainWindow::OnMinus()      {if (activeDb()) activeDb()->OnMinus();}      
void MainWindow::OnMasall()     {if (activeDb()) activeDb()->OnMasall();}     
void MainWindow::OnNapor()      {if (activeDb()) activeDb()->OnNapor();}      
void MainWindow::OnOtmet()      {if (activeDb()) activeDb()->OnOtmet();}      
void MainWindow::OnOkno()       {if (activeDb()) activeDb()->OnOkno();}       
void MainWindow::OnVysot()      {if (activeDb()) activeDb()->OnVysot();}      
void MainWindow::OnStat()       {if (activeDb()) activeDb()->OnStat();}       
void MainWindow::OnRealP()      {if (activeDb()) activeDb()->OnRealP();}      
void MainWindow::OnSaveDb()     {if (activeDb()) activeDb()->OnSaveDb();}  
void MainWindow::OnMoveTo()     {if (activeDb()) activeDb()->OnMoveTo();}     
*/


void MainWindow::createActionsDb()
{

dbAction.helpAction = new QAction(tr("Подсказка"), this);
dbAction.helpAction->setShortcut(Qt::ShiftModifier|Qt::Key_F1);
connect(dbAction.helpAction, SIGNAL(triggered()), this, SLOT(db_help()));

        
dbAction.open_table_sourceAction = new QAction(tr("Выбрать таблицу "), this);
dbAction.open_table_sourceAction->setIcon(QIcon(":/images/db/view_db.png"));
connect(dbAction.open_table_sourceAction, SIGNAL(triggered()), this, SLOT(open_table_source()));
        
dbAction.open_table_resultAction = new QAction(tr("Выбрать отчет"), this);
dbAction.open_table_resultAction->setIcon(QIcon(":/images/db/out2.png"));
connect(dbAction.open_table_resultAction, SIGNAL(triggered()), this, SLOT(open_table_result()));
        
dbAction.analiz2Action = new QAction(tr("Выбрать отчет анализа"), this);
dbAction.analiz2Action->setIcon(QIcon(":/images/db/anal.png"));
connect(dbAction.analiz2Action, SIGNAL(triggered()), this, SLOT(analiz2()));
        
dbAction.view_spravinfoAction = new QAction(tr("Справочная информация"), this);
dbAction.view_spravinfoAction->setIcon(QIcon(":/images/db/view_spravinfo.png"));
connect(dbAction.view_spravinfoAction, SIGNAL(triggered()), this, SLOT(view_spravinfo()));
        
dbAction.view_standinfoAction = new QAction(tr("Стандартное оборудование"), this);
dbAction.view_standinfoAction->setIcon(QIcon(":/images/db/view_standinfo.png"));
connect(dbAction.view_standinfoAction, SIGNAL(triggered()), this, SLOT(view_standinfo()));
        
dbAction.repairspavAction = new QAction(tr("Восстановить справочную информацию"), this);
dbAction.repairspavAction->setIcon(QIcon(":/images/db/repairspav.png"));
connect(dbAction.repairspavAction, SIGNAL(triggered()), this, SLOT(repairspav()));
        
dbAction.file_closeAction = new QAction(tr("Выход"), this);
dbAction.file_closeAction->setIcon(QIcon(":/images/db/file_close.png"));
connect(dbAction.file_closeAction, SIGNAL(triggered()), this, SLOT(file_close()));
        
dbAction.file_printAction = new QAction(tr("&Печать...\tCtrl+P"), this);
dbAction.file_printAction->setIcon(QIcon(":/images/db/file_print.png"));
connect(dbAction.file_printAction, SIGNAL(triggered()), this, SLOT(file_print()));
        
dbAction.file_print_previewAction = new QAction(tr("Предварительный просмотр"), this);
dbAction.file_print_previewAction->setIcon(QIcon(":/images/db/file_print_preview.png"));
connect(dbAction.file_print_previewAction, SIGNAL(triggered()), this, SLOT(file_print_preview()));
        
dbAction.file_print_setupAction = new QAction(tr("Установки печати..."), this);
dbAction.file_print_setupAction->setIcon(QIcon(":/images/db/file_print_setup.png"));
connect(dbAction.file_print_setupAction, SIGNAL(triggered()), this, SLOT(file_print_setup()));
        
dbAction.file_page_setupAction = new QAction(tr("Параметры страницы"), this);
dbAction.file_page_setupAction->setIcon(QIcon(":/images/db/file_page_setup.png"));
connect(dbAction.file_page_setupAction, SIGNAL(triggered()), this, SLOT(file_page_setup()));
        
dbAction.file_headerfooterAction = new QAction(tr("Заголовок"), this);
dbAction.file_headerfooterAction->setIcon(QIcon(":/images/db/file_headerfooter.png"));
connect(dbAction.file_headerfooterAction, SIGNAL(triggered()), this, SLOT(file_headerfooter()));
        
dbAction.edit_undoAction = new QAction(tr("&Отменить\tCtrl+Z"), this);
dbAction.edit_undoAction->setIcon(QIcon(":/images/db/edit_undo.png"));
connect(dbAction.edit_undoAction, SIGNAL(triggered()), this, SLOT(edit_undo()));
        
dbAction.edit_redoAction = new QAction(tr("Вернуть\tCtrl+R"), this);
dbAction.edit_redoAction->setIcon(QIcon(":/images/db/edit_redo.png"));
connect(dbAction.edit_redoAction, SIGNAL(triggered()), this, SLOT(edit_redo()));
        
dbAction.edit_undorecordAction = new QAction(tr("Восстановить значение поля"), this);
dbAction.edit_undorecordAction->setIcon(QIcon(":/images/db/edit_undorecord.png"));
connect(dbAction.edit_undorecordAction, SIGNAL(triggered()), this, SLOT(edit_undorecord()));
        
dbAction.edit_cutAction = new QAction(tr("Вырезать\tCtrl+X"), this);
dbAction.edit_cutAction->setIcon(QIcon(":/images/db/edit_cut.png"));
connect(dbAction.edit_cutAction, SIGNAL(triggered()), this, SLOT(edit_cut()));
        
dbAction.edit_copyAction = new QAction(tr("&Копировать\tCtrl+C"), this);
dbAction.edit_copyAction->setIcon(QIcon(":/images/db/edit_copy.png"));
connect(dbAction.edit_copyAction, SIGNAL(triggered()), this, SLOT(edit_copy()));
        
dbAction.edit_pasteAction = new QAction(tr("&Вставить\tCtrl+V"), this);
dbAction.edit_pasteAction->setIcon(QIcon(":/images/db/edit_paste.png"));
connect(dbAction.edit_pasteAction, SIGNAL(triggered()), this, SLOT(edit_paste()));
        
dbAction.select_allAction = new QAction(tr("Выделить все\tCtrl+A"), this);
dbAction.select_allAction->setIcon(QIcon(":/images/db/select_all.png"));
connect(dbAction.select_allAction, SIGNAL(triggered()), this, SLOT(select_all()));
        
dbAction.edit_findAction = new QAction(tr("&Найти...\tCtrl+F"), this);
dbAction.edit_findAction->setIcon(QIcon(":/images/db/find.png"));
connect(dbAction.edit_findAction, SIGNAL(triggered()), this, SLOT(edit_find()));
        
dbAction.edit_repeatAction = new QAction(tr("Искать дальше\tF3"), this);
dbAction.edit_repeatAction->setIcon(QIcon(":/images/db/repeat.png"));
connect(dbAction.edit_repeatAction, SIGNAL(triggered()), this, SLOT(edit_repeat()));
        
dbAction.edit_replaceAction = new QAction(tr("Заменить\tCtrl+H"), this);
dbAction.edit_replaceAction->setIcon(QIcon(":/images/db/replace.png"));
connect(dbAction.edit_replaceAction, SIGNAL(triggered()), this, SLOT(edit_replace()));
        
dbAction.record_firstAction = new QAction(tr("Первая запись"), this);
dbAction.record_firstAction->setIcon(QIcon(":/images/db/record_first.png"));
connect(dbAction.record_firstAction, SIGNAL(triggered()), this, SLOT(record_first()));
        
dbAction.record_prevAction = new QAction(tr("Предыдущая запись"), this);
dbAction.record_prevAction->setIcon(QIcon(":/images/db/record_prev.png"));
connect(dbAction.record_prevAction, SIGNAL(triggered()), this, SLOT(record_prev()));
        
dbAction.record_nextAction = new QAction(tr("Следующая запись"), this);
dbAction.record_nextAction->setIcon(QIcon(":/images/db/record_next.png"));
connect(dbAction.record_nextAction, SIGNAL(triggered()), this, SLOT(record_next()));
        
dbAction.record_lastAction = new QAction(tr("Последняя запись"), this);
dbAction.record_lastAction->setIcon(QIcon(":/images/db/record_last.png"));
connect(dbAction.record_lastAction, SIGNAL(triggered()), this, SLOT(record_last()));
        
dbAction.record_deleteAction = new QAction(tr("Удалить запись"), this);
dbAction.record_deleteAction->setIcon(QIcon(":/images/db/record_delete.png"));
connect(dbAction.record_deleteAction, SIGNAL(triggered()), this, SLOT(record_delete()));
        
dbAction.record_newqueryAction = new QAction(tr("Выборка по критериям"), this);
dbAction.record_newqueryAction->setIcon(QIcon(":/images/db/record_newquery.png"));
connect(dbAction.record_newqueryAction, SIGNAL(triggered()), this, SLOT(record_newquery()));
        
dbAction.record_allAction = new QAction(tr("Все записи"), this);
dbAction.record_allAction->setIcon(QIcon(":/images/db/record_all.png"));
connect(dbAction.record_allAction, SIGNAL(triggered()), this, SLOT(record_all()));
        
dbAction.column_sortAction = new QAction(tr("Сортировать"), this);
dbAction.column_sortAction->setIcon(QIcon(":/images/db/sort.png"));
connect(dbAction.column_sortAction, SIGNAL(triggered()), this, SLOT(column_sort()));
        
dbAction.hide_columnAction = new QAction(tr("Скрыть/Показать столбцы"), this);
dbAction.hide_columnAction->setIcon(QIcon(":/images/db/hide_column.png"));
connect(dbAction.hide_columnAction, SIGNAL(triggered()), this, SLOT(hide_column()));
        
dbAction.view_useadvanceddatecontrolAction = new QAction(tr("Ввод даты/времени с помощью календаря"), this);
dbAction.view_useadvanceddatecontrolAction->setIcon(QIcon(":/images/db/view_useadvanceddatecontrol.png"));
connect(dbAction.view_useadvanceddatecontrolAction, SIGNAL(triggered()), this, SLOT(view_useadvanceddatecontrol()));
        
dbAction.format_cellsAction = new QAction(tr("Формат столбца"), this);
dbAction.format_cellsAction->setIcon(QIcon(":/images/db/format_cells.png"));
connect(dbAction.format_cellsAction, SIGNAL(triggered()), this, SLOT(format_cells()));
        
dbAction.format_freezecolsAction = new QAction(tr("Закрепить столбец"), this);
dbAction.format_freezecolsAction->setIcon(QIcon(":/images/db/format_freezecols.png"));
connect(dbAction.format_freezecolsAction, SIGNAL(triggered()), this, SLOT(format_freezecols()));
        
dbAction.format_unfreezecolsAction = new QAction(tr("Открепить столбец"), this);
dbAction.format_unfreezecolsAction->setIcon(QIcon(":/images/db/format_unfreezecols.png"));
connect(dbAction.format_unfreezecolsAction, SIGNAL(triggered()), this, SLOT(format_unfreezecols()));
        
dbAction.format_freezerowsAction = new QAction(tr("Закрепить строку"), this);
dbAction.format_freezerowsAction->setIcon(QIcon(":/images/db/format_freezerows.png"));
connect(dbAction.format_freezerowsAction, SIGNAL(triggered()), this, SLOT(format_freezerows()));
        
dbAction.format_unfreezerowsAction = new QAction(tr("Открепить строку"), this);
dbAction.format_unfreezerowsAction->setIcon(QIcon(":/images/db/format_unfreezerows.png"));
connect(dbAction.format_unfreezerowsAction, SIGNAL(triggered()), this, SLOT(format_unfreezerows()));
        
dbAction.view_zoominAction = new QAction(tr("Увеличить"), this);
dbAction.view_zoominAction->setIcon(QIcon(":/images/db/view_zoomin.png"));
connect(dbAction.view_zoominAction, SIGNAL(triggered()), this, SLOT(view_zoomin()));
        
dbAction.view_zoomoutAction = new QAction(tr("Уменьшить"), this);
dbAction.view_zoomoutAction->setIcon(QIcon(":/images/db/view_zoomout.png"));
connect(dbAction.view_zoomoutAction, SIGNAL(triggered()), this, SLOT(view_zoomout()));
        
dbAction.view_100Action = new QAction(tr("100%"), this);
dbAction.view_100Action->setIcon(QIcon(":/images/db/view_100.png"));
connect(dbAction.view_100Action, SIGNAL(triggered()), this, SLOT(view_100()));


dbAction.excelAction = new QAction(tr("Excel"), this);
dbAction.excelAction->setIcon(QIcon(":/images/db/excel.png"));
connect(dbAction.excelAction, SIGNAL(triggered()), this, SLOT(db_excel()));


dbAction.move_to2Action = new QAction(tr("Здания"), this);
dbAction.move_to2Action->setIcon(QIcon(":/images/db/move_to.png"));
connect(dbAction.move_to2Action, SIGNAL(triggered()), this, SLOT(db_moveto()));


}


void MainWindow::createMenusDb()
{
#if 0
    QMenu *menu;
    QMenu *subMenu;

//    POPUP "&Файл"
        menu = menuBar()->addMenu(tr("&Файл"));
    //BEGIN
//        MENUITEM "Выбрать таблицу ",            ID_OPEN_TABLE_SOURCE
menu->addAction(dbAction.open_table_sourceAction);
//        MENUITEM "Выбрать отчет",               ID_OPEN_TABLE_RESULT
menu->addAction(dbAction.open_table_resultAction);
//        MENUITEM "Выбрать отчет анализа",       ID_ANALIZ2
menu->addAction(dbAction.analiz2Action);
//        MENUITEM "Справочная информация",       ID_VIEW_SPRAVINFO
menu->addAction(dbAction.view_spravinfoAction);
//        MENUITEM "Стандартное оборудование",    ID_VIEW_STANDINFO
menu->addAction(dbAction.view_standinfoAction);
//        MENUITEM "Восстановить справочную информацию", ID_REPAIRSPAV
menu->addAction(dbAction.repairspavAction);
//        MENUITEM "Выход",                       ID_FILE_CLOSE
menu->addAction(dbAction.file_closeAction);
        //MENUITEM SEPARATOR
    menu->addSeparator();
//        MENUITEM "&Печать...\tCtrl+P",          ID_FILE_PRINT
menu->addAction(dbAction.file_printAction);
//        MENUITEM "Предварительный просмотр",    ID_FILE_PRINT_PREVIEW
menu->addAction(dbAction.file_print_previewAction);
//        MENUITEM "Установки печати...",         ID_FILE_PRINT_SETUP
menu->addAction(dbAction.file_print_setupAction);
        //MENUITEM SEPARATOR
    menu->addSeparator();
//        MENUITEM "Параметры страницы",          ID_FILE_PAGE_SETUP
menu->addAction(dbAction.file_page_setupAction);
        //MENUITEM SEPARATOR
    menu->addSeparator();
//        MENUITEM "Заголовок",                   ID_FILE_HEADERFOOTER
menu->addAction(dbAction.file_headerfooterAction);
    //END
//    POPUP "&Редактирование"
        menu = menuBar()->addMenu(tr("&Редактирование"));
    //BEGIN
//        MENUITEM "&Отменить\tCtrl+Z",           ID_EDIT_UNDO
menu->addAction(dbAction.edit_undoAction);
//        MENUITEM "Вернуть\tCtrl+R",             ID_EDIT_REDO
menu->addAction(dbAction.edit_redoAction);
//        MENUITEM "Восстановить значение поля",  ID_EDIT_UNDORECORD
menu->addAction(dbAction.edit_undorecordAction);
        //MENUITEM SEPARATOR
    menu->addSeparator();
//        MENUITEM "Вырезать\tCtrl+X",            ID_EDIT_CUT
menu->addAction(dbAction.edit_cutAction);
//        MENUITEM "&Копировать\tCtrl+C",         ID_EDIT_COPY
menu->addAction(dbAction.edit_copyAction);
//        MENUITEM "&Вставить\tCtrl+V",           ID_EDIT_PASTE
menu->addAction(dbAction.edit_pasteAction);
        //MENUITEM SEPARATOR
    menu->addSeparator();
//        MENUITEM "Выделить все\tCtrl+A",        ID_SELECT_ALL
menu->addAction(dbAction.select_allAction);
        //MENUITEM SEPARATOR
    menu->addSeparator();
//        MENUITEM "&Найти...\tCtrl+F",           ID_EDIT_FIND
menu->addAction(dbAction.edit_findAction);
//        MENUITEM "Искать дальше\tF3",           ID_EDIT_REPEAT
menu->addAction(dbAction.edit_repeatAction);
//        MENUITEM "Заменить\tCtrl+H",            ID_EDIT_REPLACE
menu->addAction(dbAction.edit_replaceAction);
    //END
//    POPUP "Запись"
        menu = menuBar()->addMenu(tr("Запись"));
    //BEGIN
//        MENUITEM "Первая запись",               ID_RECORD_FIRST
menu->addAction(dbAction.record_firstAction);
//        MENUITEM "Предыдущая запись",           ID_RECORD_PREV
menu->addAction(dbAction.record_prevAction);
//        MENUITEM "Следующая запись",            ID_RECORD_NEXT
menu->addAction(dbAction.record_nextAction);
//        MENUITEM "Последняя запись",            ID_RECORD_LAST
menu->addAction(dbAction.record_lastAction);
        //MENUITEM SEPARATOR
    menu->addSeparator();
//        MENUITEM "Удалить запись",              ID_RECORD_DELETE
menu->addAction(dbAction.record_deleteAction);
        //MENUITEM SEPARATOR
    menu->addSeparator();
//        MENUITEM "Выборка по критериям",        ID_RECORD_NEWQUERY
menu->addAction(dbAction.record_newqueryAction);
//        MENUITEM "Все записи",                  ID_RECORD_ALL
menu->addAction(dbAction.record_allAction);
    //END
//    POPUP "Столбец"
        menu = menuBar()->addMenu(tr("Столбец"));
    //BEGIN
//        MENUITEM "Сортировать",                 ID_COLUMN_SORT
menu->addAction(dbAction.column_sortAction);
//        MENUITEM "Скрыть/Показать столбцы",     ID_HIDE_COLUMN
menu->addAction(dbAction.hide_columnAction);
    //END
//    POPUP "Формат"
        menu = menuBar()->addMenu(tr("Формат"));
    //BEGIN
//        MENUITEM "Ввод даты/времени с помощью календаря", ID_VIEW_USEADVANCEDDATECONTROL
//menu->addAction(gidrAction.view_useadvanceddatecontrolAction);

//        MENUITEM "Формат столбца",              ID_FORMAT_CELLS
menu->addAction(dbAction.format_cellsAction);
//        MENUITEM "Закрепить столбец",           ID_FORMAT_FREEZECOLS
menu->addAction(dbAction.format_freezecolsAction);
//        MENUITEM "Открепить столбец",           ID_FORMAT_UNFREEZECOLS
menu->addAction(dbAction.format_unfreezecolsAction);
//        MENUITEM "Закрепить строку",            ID_FORMAT_FREEZEROWS
menu->addAction(dbAction.format_freezerowsAction);
//        MENUITEM "Открепить строку",            ID_FORMAT_UNFREEZEROWS
menu->addAction(dbAction.format_unfreezerowsAction);
    //END
//    POPUP "Вид"
        menu = menuBar()->addMenu(tr("Вид"));
    //BEGIN
//        MENUITEM "Увеличить",                   ID_VIEW_ZOOMIN
menu->addAction(dbAction.view_zoominAction);
//        MENUITEM "Уменьшить",                   ID_VIEW_ZOOMOUT
menu->addAction(dbAction.view_zoomoutAction);
//        MENUITEM "100%",                        ID_VIEW_100
menu->addAction(dbAction.view_100Action);
    //END
//    POPUP "Окно"
        menu = menuBar()->addMenu(tr("Окно"));

menu->addAction(mainAction.tileAction);
menu->addAction(mainAction.cascadeAction);
menu->addAction(mainAction.nextAction);
menu->addAction(mainAction.previousAction);
    //END
//    POPUP "Помощь"
    menu = menuBar()->addMenu(tr("&?"));
//    list_menu_gidr.append(menu);
//    BEGIN
//        MENUITEM "&Содержание\tF1",             ID_HELP_FINDER
menu->addAction(mainAction.help_finderAction);
    menu->addSeparator();
//        MENUITEM "&О программе",                ID_APP_ABOUT
menu->addAction(mainAction.app_aboutAction);
//    menu->addAction(mainAction.aboutQtAction);
//    END

#endif

}


void MainWindow::createToolBarsDb()
{

tableToolBar = addToolBar(tr("Db"));
tableToolBar->setObjectName(tr("Db"));

tableToolBar->addAction(dbAction.open_table_sourceAction);
tableToolBar->addAction(dbAction.open_table_resultAction);
tableToolBar->addAction(dbAction.view_spravinfoAction);
tableToolBar->addAction(dbAction.view_standinfoAction);
tableToolBar->addSeparator();
tableToolBar->addAction(dbAction.edit_cutAction);
tableToolBar->addAction(dbAction.edit_copyAction);
tableToolBar->addAction(dbAction.edit_pasteAction);
tableToolBar->addSeparator();
tableToolBar->addAction(dbAction.edit_findAction);
tableToolBar->addAction(dbAction.edit_repeatAction);
tableToolBar->addAction(dbAction.edit_replaceAction);
tableToolBar->addSeparator();
tableToolBar->addAction(dbAction.column_sortAction);
tableToolBar->addSeparator();
tableToolBar->addAction(dbAction.move_to2Action);
tableToolBar->addSeparator();
tableToolBar->addAction(dbAction.excelAction);
tableToolBar->addSeparator();
tableToolBar->addAction(dbAction.file_printAction);
tableToolBar->addAction(dbAction.file_print_previewAction);
tableToolBar->addAction(dbAction.switch_recordsAction);
tableToolBar->addAction(dbAction.helpAction);

//tableToolBar->setIconSize ( QSize(16, 16) );

}



