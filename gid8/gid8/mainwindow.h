#pragma once

#define _RIBBON 1

#include <QMainWindow>

#include "Shell/shell.h"

#include "gidview/GidWidget.h"

#include "docks/DockAddr.h"
#include "docks/DockSprav.h"
#include "docks/DockPTS.h"
#include "docks/DockGeo.h"
#include "docks/DockGid.h"

#include <any/download2.h>


//QT_BEGIN_NAMESPACE
//namespace Ui { class MainWindow; }
//QT_END_NAMESPACE


class QSettings;
class QMdiArea;
class QLabel;
class Shell;

class QTextEdit;
class QListWidget;
class QTreeWidget;
class Ribbon;


#include "action/mainAction1.h"

#if 0
// Пьезометр
struct PjezoAction
{
QAction *rightAction;
QAction *leftAction;


QAction *savepjezoAction;
//
QAction *statAction;
QAction *move_toAction;
QAction *real_pAction;

QAction *edit_copyAction;
QAction *edit_cutAction;
QAction *edit_pasteAction;
QAction *edit_undoAction;
QAction *file_closeAction;

QAction *file_printAction;
QAction *file_print_previewAction;
QAction *file_print_setupAction;
QAction *file_saveAction;
QAction *file_save_asAction;
QAction *importAction;
QAction *minusAction;
QAction *naporAction;
QAction *oknoAction;
QAction *otmetAction;
QAction *plusAction;
QAction *saveQAction;
QAction *techinfoAction;
QAction *view_status_barAction;
QAction *view_toolbarAction;
QAction *vysotAction;
QAction *masallAction;
};

#endif


// Таблицы
struct TableAction
{
QAction *move_to2Action;
QAction *excelAction;
QAction *helpAction;
QAction *switch_recordsAction;

QAction *open_table_sourceAction;
QAction *open_table_resultAction;
QAction *analiz2Action;
QAction *view_spravinfoAction;
QAction *view_standinfoAction;
QAction *repairspavAction;
QAction *file_closeAction;
QAction *file_printAction;
QAction *file_print_previewAction;
QAction *file_print_setupAction;
QAction *file_page_setupAction;
QAction *file_headerfooterAction;
QAction *edit_undoAction;
QAction *edit_redoAction;
QAction *edit_undorecordAction;
QAction *edit_cutAction;
QAction *edit_copyAction;
QAction *edit_pasteAction;
QAction *select_allAction;
QAction *edit_findAction;
QAction *edit_repeatAction;
QAction *edit_replaceAction;
QAction *record_firstAction;
QAction *record_prevAction;
QAction *record_nextAction;
QAction *record_lastAction;
QAction *record_deleteAction;
QAction *record_newqueryAction;
QAction *record_allAction;
QAction *column_sortAction;
QAction *hide_columnAction;
QAction *view_useadvanceddatecontrolAction;
QAction *format_cellsAction;
QAction *format_freezecolsAction;
QAction *format_unfreezecolsAction;
QAction *format_freezerowsAction;
QAction *format_unfreezerowsAction;
QAction *view_zoominAction;
QAction *view_zoomoutAction;
QAction *view_100Action;
};


extern struct TableAction tableAction;
extern struct PjezoAction pjAction;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void start(const QString & cmd, QWidget *widget);
    void start(const QString & cmd, QWidget *widget, const char *slot_name);

    QStringList geoFiles;
    QSettings *settings;

    QDockWidget *addWidgetTab(QWidget *widget, const QString & title, Qt::DockWidgetArea area, bool can_close = true);
    QDockWidget *findWidgetTab(const QString & title, Qt::DockWidgetArea area);

    void updateStatusBar();

    MainAction mainAction;

    void removeLeftDocks();
    void removeRightDocks();

    void addLeftDocks();
    void addRightDocks();

    void showDocks(Qt::DockWidgetArea area, bool on);
    bool isVisibleDocks(Qt::DockWidgetArea area);

    GidWidget *view() {return m_geo;};

    void showTabsSideBySide(int firstTabIndex, int secondTabIndex);
    void splitCombinedTab(int currentIndex);
    void addWidgetToTabs(QWidget *widget, const QString &title);
    void removeWidgetFromCenter(QWidget *widget);

    void setCurrent(QWidget *widget);

protected:
    void closeEvent(QCloseEvent *event);

private:
    GidWidget *activeGeo();

    void createActions();
    
    void createMenus();
    void createContextMenu();
    void createStatusBar();
    void createToolBars();

    void createActionsPjezo();
    void createMenusPjezo();
    void createToolBarsPjezo();

    void createActionsDb();
    void createMenusDb();
    void createToolBarsDb();

    void createDockWindows();

    void onTabContextMenu(const QPoint &pos);

    void init(GidWidget *m_geo);
    void init_geo();

    Ribbon *createRibbon();


    QAction *separatorAction;

private:
    QSettings *m_settings;


    QLabel *txtLabel;
    QLabel *locationLabel;
    QLabel *masLabel;

public:
    void setCxema(CCxema *c) {m_sprav->setCxema(c);};

private :
    QToolBar *pjezoToolBar;
    QToolBar *mainToolBar;
    QToolBar *tableToolBar;


public:

    Shell *shellDoc;

    DockGeo *m_left_geo;
    DockGid *m_left_gid;
    QListWidget *m_left3;
    QListWidget *m_left4;
    DockAddr *m_addr;

//    QListWidget *m_right1;
//    QListWidget *m_right2;

    DockSprav *m_sprav;
    DockPTS *m_ms;
    DockPTS *m_rs;

    QDockWidget *ribbon_dock = nullptr;

private:

    QMenu *mainMenu;
//    Ribbon *mainRibbon;

    QTabWidget *m_tabWidget;

    enum TabType {
        Regular,
        Combined
    };

    QMap<QWidget *, TabType> tabStates;

    bool check_protection();

//    DownloadManager2 download_manager;

private:
    GidWidget *m_geo = nullptr;

//    Ribbon *mainRibbon = nullptr;

//private slots:

//#include "action/mainSlot.h"

private slots:
//------------------------------------------------------    
//  Из MFC
//------------------------------------------------------    
    void onFileNew(); // Подключить
    void onFileSqlite(); // Подключить
    void onBdNew(); // Создать БД
//    void onBdDel(); // Создать БД
    void onImportShape(); // Импортировать Shape-файлы
    void onFilePrintSetup(); // Параметры страницы...
    void onConvert(); // Конвертировать
    void onCatalog(); // Каталоги
    void onAppExit(); // Выход
    void onProtocol(); // Протокол
    void onViewStatusBar(); // Строка статуса
    void onUpdate(); // Последнее обновление
    void onUpdateSetup(); // Настройка обновлений
    void onAppAbout(); // О программе
//------------------------------------------------------    

    void onCatalogAccepted();


    void unknown(); 
    void showEvent(QShowEvent *e);
    
//    Ui::MainWindow *ui;
};

extern MainWindow *main_window;

QSettings *get_Settings();
