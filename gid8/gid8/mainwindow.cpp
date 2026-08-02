#include <QtGui>
#include <QtWidgets>
#include <Shell/shell.h>

#include "mainwindow.h"

#include "mystd.h"

#include <db/db.h>

#include "version0.h"


//#include "./ui_mainwindow.h"

MainWindow *main_window = NULL;

QSettings *get_Settings()
{
    return main_window->settings;
}


#include <ui2/OpenServer.h>
#include <dialog/OpenServerDialog.h>

#include <ui2/OpenServer.h>

#include <config.h>

void MainWindow::init(GidWidget *geo)
{

    //    delete geo;
    //    geo = new GidWidget;


    statusBar()->showMessage(tr("File loaded"), 2000);

    QScrollArea *scrollView = new QScrollArea;

    scrollView->setWidget(geo);
    scrollView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollView->setWidgetResizable(true);
    //      windowMenu->addAction(geo->windowMenuAction());
    //      windowActionGroup->addAction(geo->windowMenuAction());

    geo->setTitle();
    addWidgetToTabs(scrollView, geo->getTitle());
    m_tabWidget->setTabBarAutoHide(true);
    m_tabWidget->setMovable(true);
    scrollView->showMaximized();
    scrollView->show();

    //    mainMenu->menuAction()->setVisible(false);

    QSettings settings;
    restoreState(settings.value("mainWindowState").toByteArray());


//    bool vl = isVisibleDocks(Qt::LeftDockWidgetArea);
//    bool lr = isVisibleDocks(Qt::RightDockWidgetArea);
//    bool lb = isVisibleDocks(Qt::BottomDockWidgetArea);


    geo->setToolbars();

//    setWindowTitle(QString("%1 %2").arg(VER_FILEDESCRIPTION_STR, VER_PRODUCT_VERSION_STR));
}

void MainWindow::onFileNew()
{
//    init_config();

    if (!get_ini()->ok) {
        QMessageBox::warning(nullptr, "", "Программа не может работать. Обратитесь за разъяснениями к разработчику");
        return;
    }

    if (get_ini()->protection && !check_protection()) return;
    
    OpenServerDialog dlg(this, tr("Подключение к серверу"));
    if (dlg.exec() != QDialog::Accepted) return;

    qDebug() << "m_geo->connectSQL";
    qDebug() << m_geo;
    qDebug() << dlg.cs.rdbms << " " << dlg.cs.host << " " << dlg.cs.port << " " << dlg.cs.baza << " " << dlg.cs.geo << " " << dlg.cs.user; // << " " << dlg.cs.password;

    QSettings settings;
    restoreState(settings.value("mainWindowState").toByteArray());

    m_geo = new GidWidget;

    if (dlg.cs.gid9 != "") {
        QSqlDatabase db_gid9;
        bool ok = connectSQL0(dlg.cs.rdbms, dlg.cs.host, dlg.cs.port, dlg.cs.gid9, dlg.cs.user, dlg.cs.password, db_gid9);

        set_gid9(true);
        set_baza_gid9(dlg.cs.gid9);

        if (!ok) {
            QMessageBox::information(this, "", QString("Не могу подключиться к БД %1. Работа продолжается без синхронизации!").arg(dlg.cs.gid9));
            set_gid9(false);
        }
        else {
           if (!checkTableExists(db_gid9, "gid", "nodes")) {
               QMessageBox::information(this, "", QString("БД %1 не предназначена для синхронизации!").arg(dlg.cs.gid9));
               set_gid9(false);
           }
        }
    }


    if (!m_geo->connectSQL(dlg.cs.rdbms, dlg.cs.host, dlg.cs.port, dlg.cs.baza, dlg.cs.geo, dlg.cs.user, dlg.cs.password)) {
        statusBar()->showMessage(tr("Loading canceled"), 2000);
        ErrorMessage(m_geo->getCxema()->m_db);
        return;
    }

    init_geo();
    init(m_geo);
}

void MainWindow::onFileSqlite()
{
    if (!check_protection()) return;

    QSettings settings;

    QString dir = settings.value("sqlite_file", "").toString();

    QString fn = QFileDialog::getOpenFileName(this,
                                              tr("Открыть файл"), dir,
                                              tr("Файлы (*.sqlite)"));
    if (fn != "") {
        m_geo = new GidWidget;

        if (!m_geo->connectSqlite(fn)) {
            statusBar()->showMessage(tr("Loading canceled"), 2000);
            ErrorMessage(m_geo->getCxema()->m_db);
            return;
        }
        init_geo();
        init(m_geo);
        settings.setValue("sqlite_file", fn);
    }
}

#include <ribbon/ribbon.h>

void MainWindow::start(const QString & cmd, QWidget *widge, const char *slot_name) {
    shellDoc->show();
    shellDoc->start(cmd, widge, slot_name);
}


void MainWindow::start(const QString & cmd, QWidget *widget) {
    shellDoc->show();
    shellDoc->start(cmd, widget, nullptr);
    //    shellDoc->start(cmd, widget, SLOT(onFinished(int, QProcess::ExitStatus)));
}



///MainWindow::

void create_ribbon_main(Ribbon *ribbon, MainAction *mainAction);
void create_ribbon(Ribbon *ribbon, GidrAction *gidrAction);



Ribbon *MainWindow::createRibbon()
{
    delete ribbon_dock;

    ribbon_dock = new QDockWidget();
    ribbon_dock->setTitleBarWidget(new QWidget());
    ribbon_dock->setFloating(false);
    ribbon_dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    //    ribbon_dock->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    Ribbon *mainRibbon;
    mainRibbon = new Ribbon();
    ribbon_dock->setWidget(mainRibbon);
    this->addDockWidget(Qt::TopDockWidgetArea, ribbon_dock);


    //    mainRibbon->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    mainRibbon->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainRibbon->setContentsMargins(0, 0, 0, 0);


    return mainRibbon;
}

void MainWindow::init_geo()
{
//    m_geo = new GidWidget;

//    createMenus();

#ifdef _RIBBON
    //    createToolBars();
    //#else
    Ribbon *mainRibbon = createRibbon();
//    create_ribbon(mainRibbon, &m_geo->gidrAction);
    m_geo->create_ribbon(mainRibbon);
    m_geo->createShortcuts();

#endif
}

void MainWindow::onTabContextMenu(const QPoint &pos) {
    // Определяем индекс вкладки под курсором
    int tabIndex = m_tabWidget->tabBar()->tabAt(pos);
    if (tabIndex == -1) return; // Не попали в область вкладки

    QMenu contextMenu(this);
    QWidget *firstTab = m_tabWidget->widget(tabIndex);
    QWidget *secondTab = m_tabWidget->widget(m_tabWidget->currentIndex());

    //если выбрана одна и та же вкладка
    if (m_tabWidget->currentIndex() == tabIndex){
        //если она совместная
        if(tabStates[firstTab]==Combined)
        {
            //опция разделить
            QAction *arrangeSideBySide = contextMenu.addAction("Разделить вкладки");
            connect(arrangeSideBySide, &QAction::triggered, [this, tabIndex]() {
                int activeTabIndex = m_tabWidget->currentIndex();
                if (activeTabIndex != -1) {
                    splitCombinedTab(activeTabIndex);
                }
            });
        }

    }
    else
        if (tabStates[firstTab]==Regular && tabStates[secondTab]==Regular)

    {
        QAction *arrangeSideBySide = contextMenu.addAction("Расположить рядом");
        // Обработчик для пункта "Расположить рядом"
        connect(arrangeSideBySide, &QAction::triggered, [this, tabIndex]() {
            int activeTabIndex = m_tabWidget->currentIndex();
            if (activeTabIndex != -1 && activeTabIndex != tabIndex) {
                showTabsSideBySide(activeTabIndex, tabIndex);
            }
        });
    }
    else
        return;

    contextMenu.exec(m_tabWidget->mapToGlobal(pos));
}

void MainWindow::showTabsSideBySide(int firstTabIndex, int secondTabIndex) {
    // Получаем виджеты из вкладок
    QWidget *firstTab = m_tabWidget->widget(firstTabIndex);
    QWidget *secondTab = m_tabWidget->widget(secondTabIndex);

    QString titleFirstTab = m_tabWidget->tabText(firstTabIndex);
    QString titleSecondTab = m_tabWidget->tabText(secondTabIndex);
    QString titleSplitter = titleFirstTab + " | "+ titleSecondTab;
    if (!firstTab || !secondTab) {
        qWarning() << "Не удалось получить виджеты вкладок";
        return;
    }

    // Создаем QSplitter
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);


    int index = m_tabWidget->indexOf(firstTab);
    m_tabWidget->removeTab(index);
    index = m_tabWidget->indexOf(secondTab);
    m_tabWidget->removeTab(index);


    tabStates.remove(firstTab);
    tabStates.remove(secondTab);
    // Добавляем виджеты в splitter
    splitter->addWidget(firstTab);
    splitter->addWidget(secondTab);
    // Устанавливаем равное распределение пространства
    splitter->setStretchFactor(0, 1); // Первый виджет занимает 1 часть
    splitter->setStretchFactor(1, 1); // Второй виджет занимает 1 часть
    splitter->setSizes({splitter->width() / 2, splitter->width() / 2});

    int insertIndex = firstTabIndex<secondTabIndex ? firstTabIndex : secondTabIndex;
    int newTabIndex = m_tabWidget->insertTab(insertIndex,splitter, titleSplitter);
    tabStates[splitter] = Combined;
    m_tabWidget->setCurrentIndex(newTabIndex);
    firstTab->show();
    secondTab->show();
}

void MainWindow::splitCombinedTab(int currentIndex)
{
    // Получаем текущий виджет
    QWidget *currentTab = m_tabWidget->widget(currentIndex);

    // Проверяем, является ли текущий виджет QSplitter
    QSplitter *splitter = qobject_cast<QSplitter *>(currentTab);
    if (!splitter) {
        QMessageBox::warning(this, "Ошибка", "Эта вкладка не является совмещённой.");
        return;
    }

    QStringList titleTab = m_tabWidget->tabText(currentIndex).split(" | ");


    // Удаляем вкладку с QSplitter
    m_tabWidget->removeTab(currentIndex);

    // Извлекаем виджеты из QSplitter


    if (splitter->count() != 2) {
        QMessageBox::warning(this, "Ошибка", "В совмещённой вкладке должно быть два виджета.");
        return;
    }

    // Добавляем виджеты обратно как отдельные вкладки
    QWidget *firstTab = splitter->widget(0);
    QWidget *secondTab = splitter->widget(1);


    int secondTabIndex = m_tabWidget->insertTab(currentIndex,secondTab, titleTab[1]);
    int firstTabIndex = m_tabWidget->insertTab(currentIndex,firstTab, titleTab[0]);


    // Устанавливаем вкладку как активную
    m_tabWidget->setCurrentIndex(firstTabIndex);

    // Показываем виджеты
    firstTab->show();
    secondTab->show();

    // Удаляем QSplitter, так как он больше не нужен
    splitter->deleteLater();

    // Обновляем статусы вкладок
    tabStates[firstTab] = Regular;
    tabStates[secondTab] = Regular;
    tabStates.remove(splitter);
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
//    , ui(new Ui::MainWindow)
{
    //    ui->setupUi(this);
    //    GidWidget *geo = new GidWidget;

    main_window = this;

    m_tabWidget= new QTabWidget(this);
    QString tabStyle = R"(
    QTabWidget::pane {
        border: 1px solid #ccc;
        background: #f0f0f0;
        padding: 5px;
    }

    QTabBar::tab {
        background: #f5f5f5;
        border: 1px solid #ccc;
        border-radius: 5px;
        min-width: 100px;
        padding: 8px;
        font-size: 14px;
        color: #444;
        margin: 2px;
    }

    QTabBar::tab:selected, QTabBar::tab:hover {
        background: #fff;
        border-color: #aaa;
        color: #333;
    }

    QTabBar::tab:selected {
        border-bottom-color: #fff;
        background: #f9f9f9;
        font-weight: bold;
    }

    QTabBar::tab:!selected {
        background: #f0f0f0;
        border-color: #ddd;
    }

    QTabWidget::tab-bar {
        alignment: center;
    }
)";
    //m_tabWidget->setStyleSheet(tabStyle);
    m_tabWidget->setStyleSheet(R"(
    QTabBar::tab {
        background: #F4F6FF;
        color: black;
        border: 3px solid #e1e1e1;
        padding: 2px;
        border-radius: 1px;
    }
    QTabBar::tab:selected {
        background: #ffffff;
        color: black;
    }
    QTabBar::tab:hover {
        background: #C5EFFD;
    }
    QTabBar::tab:!selected {
        margin-top: 4px;
    }
    QTabWidget::pane {
        border: 0px solid #1e90ff;
        margin: 1px;
    }
)");

    // Подключаем обработчик для правого клика
    connect(m_tabWidget, &QTabWidget::customContextMenuRequested, this, &MainWindow::onTabContextMenu);
    m_tabWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    //    m_settings = new QSettings(QString("Sirius LTD"), QString("Gidr 8.0"), this);


    setCentralWidget(m_tabWidget);

    QString style = readFileUTF8("qss2/style.qss");
    style.replace("url(:/", QString("url(%1qss2/").arg(argpath()));
    style.replace("url(/", QString("url(%1qss2/").arg(argpath()));

    //    QInputDialog::getMultiLineText(nullptr, "title", "label", style);

    if (style != "") {
        qApp->setStyleSheet(style);
    }

    //    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*))
    //            , this, SLOT(focusChanged(QWidget*,QWidget*)));

    //    connect(this, SIGNAL(window_loaded())
    //            , this, SLOT(onFileNew()), Qt::ConnectionType(Qt::QueuedConnection | Qt::UniqueConnection));


    createActions();
    createStatusBar();
    createDockWindows();

#if 1

//    m_geo = new GidWidget;

//    createMenus();

#ifndef _RIBBON
    createToolBars();
#else

    Ribbon *mainRibbon = createRibbon();
    create_ribbon_main(mainRibbon, &mainAction);
#endif

#endif

    setWindowIcon(QIcon(QString(":/images/%1").arg(GIDR_ICO)));

    QSettings settings;
    restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
    //    restoreState(settings.value("mainWindowState").toByteArray());

//    setWindowTitle(QString("%1 %2").arg(VER_FILEDESCRIPTION_STR, VER_PRODUCT_VERSION_STR));
    setWindowTitle(QString("%1 %2").arg(ITWIN_TEPLO_RUS, VER_PRODUCT_VERSION_STR));
}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow::~MainWindow()";
    //    delete geo;
    //    delete ui;
}

GidWidget* MainWindow::activeGeo()
{
    // Получаем текущий индекс активной вкладки
    int currentTabIndex = m_tabWidget->currentIndex();

    if (currentTabIndex != -1) {
        // Получаем виджет в активной вкладке
        QWidget* currentTabWidget = m_tabWidget->widget(currentTabIndex);

        if (currentTabWidget) {
            // Если виджет является QScrollArea, ищем GidWidget внутри него
            QScrollArea* scrollArea = qobject_cast<QScrollArea*>(currentTabWidget);
            if (scrollArea) {
                return qobject_cast<GidWidget*>(scrollArea->widget());
            }

            // Если виджет сам является GidWidget, возвращаем его
            return qobject_cast<GidWidget*>(currentTabWidget);
        }
    }

    // Если активной вкладки нет или нужный виджет не найден
    return nullptr;
}

void MainWindow::updateStatusBar()
{
    if (activeGeo()) {
        locationLabel->setText(activeGeo()->status(1));
        masLabel->setText(activeGeo()->status(2));

        QString txt = activeGeo()->status(4);
        if (txt == "") {
            txt = activeGeo()->status(3);
        }

        txtLabel->setText(txt);
    }
}

void MainWindow::createStatusBar()
{
    txtLabel = new QLabel("Участок теплопровода WWW WWWWWWW - WWW WWWWWW");
    txtLabel->setMinimumSize(txtLabel->sizeHint());
    txtLabel->setIndent(10);

    masLabel = new QLabel("1:999999.9");
    masLabel->setAlignment(Qt::AlignHCenter);
    masLabel->setIndent(10);

    locationLabel = new QLabel("88888888.88 88888888.88 ");
    locationLabel->setAlignment(Qt::AlignHCenter);
    //    locationLabel->setMinimumSize(locationLabel->sizeHint());
    locationLabel->setIndent(10);

    statusBar()->addWidget(new QLabel(""), 1);
    statusBar()->addWidget(txtLabel);
    statusBar()->addWidget(masLabel);
    statusBar()->addWidget(locationLabel);

    //    connect(this, SIGNAL(updateStatusBar()), this, SLOT(updateStatusBar()));

    updateStatusBar();
}

#if USE_ITWIN
//#include "version.h"
#else
//#include "version_tgid.h"
#endif

#include "version0.h"


QString getHaspInfo();


QString getFileDate()
{
    return QString("%1.%2.%3").arg(BUILDTM_DAY).arg(BUILDTM_MONTH).arg(BUILDTM_YEAR);
}

void MainWindow::onAppAbout()
{

    QString w = tr("<tr><td><hr/></td></tr><tr><td><p>Внимание: Эта компьютерная программа защищена законами и международными договорами. Неразрешенное воспроизведение и распространение этой программы влечет гражданскую и уголовную ответственность.</p></td></tr>");

#if 1
#if USE_ITWIN
    //    QString w = tr("Death for reproduction of this program");
    QString ph1 = "";
    QString ph2 = "";
    QString ph3 = "";
    //    QString ph = QString("<p><a href='tel:%1'>%1</a></p><p><a href='tel:%2'>%2</a></p><p><a href='tel:%3'>%3</p>").arg(ph1, ph2, ph3);
    QString ph = "";
    w = "";
#else
//    QString ph1 = "+7 (7212) 56-52-15";
    QString ph2 = "+7 (705) 314-92-34";
    QString ph3 = "+7 (705) 199-70-51";
    //    QString ph = QString("<p><a href='tel:%1'>%1</a></p><p><a href='tel:%2'>%2</a></p><p><a href='tel:%3'>%3</p>").arg(ph1, ph2, ph3);
    QString ph = QString("<tr><td><p>Телефоны:&nbsp;<a href='tel:%2'>%2</a>;&nbsp;<a href='tel:%3'>%3</p></td></tr>").arg(ph2, ph3);
#endif
#endif

#if USE_HASP

    QString hi = getHaspInfo();

    if (hi != "") {
        hi = "<p>" + hi + "</p>";
    }


#endif

    //    QString date = QString("%1.%2.%3 %4").arg(BUILDTM_DAY).arg(BUILDTM_MONTH).arg(BUILDTM_YEAR).arg(__DATE__);
    QString date = getFileDate();

#if 0

    QMessageBox about(QMessageBox::NoIcon, tr("О программе"),
                      QString(tr("<table><tr>"
                                 "<td>"
                                 "<table>"
                                 "<tr><td><img src=:/images/idc_spla.png></td></tr>"
                                 "<!--<tr><td><img src=:/images/sirius.png></td></tr>-->"
                                 "</table>"
                                 "</td>"
                                 "<td><h2>%1 версия %2 %3</h2><p>%4</p>"
                                 "<p>100009 Республика Казахстан</p><p>г.Караганда ул.Алиханова 18, оф.47</p>"
                                 "<table><tr>"
                                 "<td>E-mail: <a href='mailto:tgid@mail.ru'>tgid@mail.ru</a></td>"
                                 "<td>Сайт: <a href='http://tgid.kz'>tgid.kz</a></td>"
                                 "</tr></table>"
                                 "<p>Телефоны:</p>"
                                 "%5"
                                 "%6"
                                 "<p>%7</p>"
                                 "</td>"))
                          .arg(VER_FILEDESCRIPTION_STR, VER_PRODUCT_VERSION_STR, date, VER_COPYRIGHT_STR, ph, hi, w));
#endif


    QString text = QString(tr("<table><tr>"
                                 "<td>"
                                 "<table>"
                                 "<tr><td><img src=:/images/idc_spla.png></td></tr>"
                                 "<!--<tr><td><img src=:/images/sirius.png></td></tr>-->"
                                 "</table>"
                                 "</td>"
                                 "<td><h2>%1 Версия %2</h2><p>%3</p>"
#if !USE_ITWIN
                                 "<p>100009 Республика Казахстан</p><p>г.Караганда ул.Алиханова 18, оф.47</p>"
                                 "<table><tr>"
                                 "<td><a href='%4'>%5</a>&nbsp;&nbsp;E-mail: <a href='mailto:tgid@mail.ru'>tgid@mail.ru</a></td></tr>"
#endif
                                 "%6"
                                 "%7"
                                 "</tr>"

                                 "</table>")
                          .arg(
//                            VER_FILEDESCRIPTION_STR, 
                            ITWIN_TEPLO_RUS,
                            VER_PRODUCT_VERSION_STR, 
                            VER_COPYRIGHT_STR_RUS,
                            TGID_KZ,
                            VER_COMPANY_NAME,
                            ph, 
                            w

                            ));
//                          .arg(VER_FILEDESCRIPTION_STR, VER_PRODUCT_VERSION_STR, date, VER_COPYRIGHT_STR, ph, hi, w));

    qDebug() << text;

    QMessageBox about(QMessageBox::NoIcon, tr("О программе"), text);


    about.exec();
}

QDockWidget *MainWindow::findWidgetTab(const QString & title, Qt::DockWidgetArea area)
{                          
    QList<QDockWidget *> dockWidgets = findChildren<QDockWidget *>();

    QDockWidget *last = nullptr;

    for (QDockWidget *w :  dockWidgets) {
        if (dockWidgetArea(w) == area && w->windowTitle() == title) {
            last = w;
            return w;
        }
    }

    return nullptr;
}


QDockWidget *MainWindow::addWidgetTab(QWidget *widget, const QString & title, Qt::DockWidgetArea area, bool can_close)
{                          
    QList<QDockWidget *> dockWidgets = findChildren<QDockWidget *>();

    QDockWidget *last = nullptr;

    for (QDockWidget *w :  dockWidgets) {
        if (dockWidgetArea(w) == area) {
            last = w;
        }
    }

    QDockWidget *dock1 = new QDockWidget(title, this);

    dock1->setObjectName(title);

//    dock1->setFeatures(QDockWidget::DockWidgetMovable); 
    dock1->setFeatures(QDockWidget::NoDockWidgetFeatures); 

    QList<QTabWidget *> tabs = dock1->findChildren<QTabWidget*>();

    for (auto &tab : tabs) {
        tab->setTabsClosable(true);
    }

    dock1->setAllowedAreas(area);

    dock1->setWidget(widget);
//    dock1->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    addDockWidget(area, dock1);
    
    if (last) {
        tabifyDockWidget(last, dock1);
        dock1->show();
        dock1->raise();
    }

    if (!can_close) {
    }

    return dock1;
}

void MainWindow::removeLeftDocks()
{
    QList<QDockWidget *> dockWidgets = findChildren<QDockWidget *>();

    for (QDockWidget *w :  dockWidgets) {
        if (dockWidgetArea(w) == Qt::LeftDockWidgetArea) {
            removeDockWidget(w);
        }
    }
}

void MainWindow::removeRightDocks()
{
    QList<QDockWidget *> dockWidgets = findChildren<QDockWidget *>();

    for (QDockWidget *w :  dockWidgets) {
        if (dockWidgetArea(w) == Qt::RightDockWidgetArea) {
            removeDockWidget(w);
        }
    }
}

void MainWindow::showDocks(Qt::DockWidgetArea area, bool on)
{
    QList<QDockWidget *> dockWidgets = findChildren<QDockWidget *>();

    for (QDockWidget *w :  dockWidgets) {
        if (dockWidgetArea(w) == area) {
            w->setVisible(on);
        }
    }
}

bool MainWindow::isVisibleDocks(Qt::DockWidgetArea area)
{
    QList<QDockWidget *> dockWidgets = findChildren<QDockWidget *>();

    for (QDockWidget *w :  dockWidgets) {
        if (dockWidgetArea(w) == area) {
            if (w->isVisible()) {
                return true;
            }
        }
    }
    return false;
}

void MainWindow::addLeftDocks()
{
    m_left_geo = new DockGeo;
    addWidgetTab(m_left_geo, tr("Геобаза"), Qt::LeftDockWidgetArea, false);

    m_left_gid = new DockGid;
    addWidgetTab(m_left_gid, tr("Схема"), Qt::LeftDockWidgetArea, false);

    m_addr = new DockAddr;
    addWidgetTab(m_addr, tr("Адрес"), Qt::LeftDockWidgetArea, false);

    //    m_left4 = new QListWidget;
    //    addWidgetTab(m_left4, tr("Жилые комплексы"), Qt::LeftDockWidgetArea, false);
}

void MainWindow::addRightDocks()
{
    m_ms = new DockPTS;
    addWidgetTab(m_ms, tr("Магистрали"), Qt::RightDockWidgetArea, false);

    m_rs = new DockPTS;
    addWidgetTab(m_rs, tr("Распредсети"), Qt::RightDockWidgetArea, false);

    m_sprav = new DockSprav;
    addWidgetTab(m_sprav, tr("Справка"), Qt::RightDockWidgetArea, false);
}

void MainWindow::createDockWindows()
{
    shellDoc = new Shell(this);
    shellDoc->setAllowedAreas(Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, shellDoc);
    shellDoc->setFeatures(QDockWidget::NoDockWidgetFeatures);
    
    //    mainMenu->addAction(shellDoc->toggleViewAction());
    //    shellDoc->hide();

    addLeftDocks();
    addRightDocks();

    setDockOptions(QMainWindow::AllowTabbedDocks|QMainWindow::ForceTabbedDocks|QMainWindow::AnimatedDocks);

    //    mainMenu->addAction(dock3->toggleViewAction());

    //    connect(leftDock, &QListWidget::currentTextChanged,
    //            this, &MainWindow::insertCustomer);
    //    connect(rightDock, &QListWidget::currentTextChanged,
    //            this, &MainWindow::addParagraph);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    auto childList = findChildren<GidWidget*>();

    if (childList.size() > 0) {
        QSettings settings;

        settings.setValue("mainWindowGeometry", saveGeometry());
        settings.setValue("mainWindowState", saveState());
    }

    for (auto child : childList)
    {
        if (!child->close()) {
            event->ignore();
            return;
        }
    }

    // Закрываем все вкладки в QTabWidget
    while (m_tabWidget->count() > 0) {
        // Получаем текущую вкладку
        QWidget *currentTab = m_tabWidget->widget(0);

        // Удаляем вкладку
        m_tabWidget->removeTab(0);

        // Удаляем виджет, если нужно
        currentTab->deleteLater();
    }

    // Проверяем, остались ли вкладки
    if (m_tabWidget->count() > 0) {
        // Если вкладки остались, игнорируем закрытие
        event->ignore();
    } else {
        // Если вкладок больше нет, принимаем событие и закрываем главное окно
        event->accept();
        QMainWindow::closeEvent(event);
    }
}

void MainWindow::unknown()
{
}

void MainWindow::showEvent(QShowEvent *e)
{
    QMainWindow::showEvent(e);

    static bool firstStart = true;
    if (firstStart)
    {
        QTimer::singleShot(50, this, SLOT(onFileNew()));
        //        emit onFileNew();
        firstStart = false;
    }
}

void MainWindow::addWidgetToTabs(QWidget *widget, const QString &title) {
    m_tabWidget->addTab(widget, title);
    tabStates[widget] = Regular; // По умолчанию вкладка обычная
    m_tabWidget->setCurrentWidget(widget); // Переход на новую вкладку
    // Создаем кнопку закрытия
    QPushButton *closeButton = new QPushButton("×");
    closeButton->setFixedSize(24, 24); // Устанавливаем размер кнопки
    closeButton->setStyleSheet("QPushButton { border: none; }"
                               "QPushButton:hover { color: red; }");

    // Связываем сигнал нажатия кнопки с удалением вкладки
    QObject::connect(closeButton, &QPushButton::clicked, [widget, this]() {
        removeWidgetFromCenter(widget);
    });

    // Добавляем кнопку на вкладку
    m_tabWidget->tabBar()->setTabButton(m_tabWidget->currentIndex(), QTabBar::RightSide, closeButton);
}


void MainWindow::setCurrent(QWidget *widget) 
{
    m_tabWidget->setCurrentWidget(widget->parentWidget()->parentWidget()); // Переход на новую вкладку
///    m_tabWidget->setCurrentIndex(0); // Переход на новую вкладку
}


void MainWindow::removeWidgetFromCenter(QWidget *widgetToRemove) {

    // Найти текущую вкладку
    int currentIndex = m_tabWidget->currentIndex();
    if (currentIndex == -1) {
        qWarning() << "Нет активных вкладок для удаления.";
        return;
    }

    QWidget *currentWidget = m_tabWidget->widget(currentIndex);

    // Проверяем, является ли текущая вкладка QSplitter
    QSplitter *splitter = qobject_cast<QSplitter *>(currentWidget);
    if (splitter) {
        // Проверяем, есть ли виджет в splitter
        splitCombinedTab(currentIndex);
    }
    // Обработка обычных вкладок
    int index = m_tabWidget->indexOf(widgetToRemove);
    if (index != -1) {
        // Удаляем вкладку
        m_tabWidget->removeTab(index);

        // Удаляем виджет
        widgetToRemove->deleteLater();
    }


    // Проверка: если вкладок больше нет, можно добавить сообщение или заглушку
    if (m_tabWidget->count() == 0) {
        // Добавляем заглушку, например, пустой виджет
        QLabel *placeholder = new QLabel("Нет открытых вкладок", m_tabWidget);
        placeholder->setAlignment(Qt::AlignCenter);
        m_tabWidget->addTab(placeholder, "Добро пожаловать");
    }
}

