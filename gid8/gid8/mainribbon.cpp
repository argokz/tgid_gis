#include <QtGui>
#include <QtWidgets>

#include <ribbon/ribbon.h>

#include "gidview/GidWidget.h"

#include <config.h>

AutoGridLayout *addNest2(QLayout *l, int rows, int columns, int typ);
void closeNest2(AutoGridLayout *l);


#if 0
QToolButton *createButton(QWidget *widget, const QString & text, const QString & text2, const QString & icon_name, const char *slot_name = nullptr)
{
    // Add 'Open project' button
    QToolButton *button = new QToolButton;
//    button->setText(text);
    button->setToolTip(text2);
    button->setIcon(QIcon(icon_name));

    button->setToolButtonStyle(Qt::ToolButtonIconOnly);

    button->setEnabled(true);

    if (slot_name) {
//        widget->connect(button, SIGNAL(triggered()), widget, slot_name);
    }

    return button;
}

#endif

QString ispr_text(const QString &atext);


static std::map<QAction *, QToolButton*> m_action_button;

QToolButton *getButton(QAction *a)
{
    auto it = m_action_button.find(a);
    if (it != m_action_button.end()) {
        return it->second;
    }

    return nullptr;
}


QToolButton *createButton(Ribbon *ribbon, const QString &tabName, const QString &groupName, QAction *a, const QString & text, int typ = 3)
{
    // Add 'Open project' button
    WToolButton *button = new WToolButton(typ);

    button->setProperty("class", QString("b%1").arg(typ));

    if (a) {
        button->setDefaultAction(a);
    }

    QString atext = text;

    if (text == "") {
        atext = ispr_text(a->text());
    }
    button->setText(atext);

    m_action_button[a] = button;


    ribbon->addButton(tabName, groupName, button, typ);


    //    button->setToolButtonStyle(Qt::ToolButtonIconOnly);
    //    button->setEnabled(true);


    return button;
}

void setMyIcon(QToolButton *b, const QString & icon_name);

#include <QScreen>





QList<QAction*> getMenuActions(QAction *mainAction);


void set_menu(WToolButton *button, QAction *a, const QString & text = "")
{

    QList<QAction*> actions = getMenuActions(a);
    
    if (!actions.isEmpty()) {
        button->setContextMenuPolicy(Qt::CustomContextMenu);

        // Подписываемся на событие
        button->connect(button, &QToolButton::customContextMenuRequested,
                [button, text, a](const QPoint &pos) {
//            menu.addActions(actions);
            QList<QAction*> actions = getMenuActions(a);
            if (actions.isEmpty()) {
                return;
            }

            QMenu menu;
            menu.addActions(actions);
            menu.exec(button->mapToGlobal(pos));  // открываем в правильной позиции
        });
    }

}


QToolButton *createButton(QLayout *layout, QAction *a, const QString & text = "", const QString & icon_name = "")
{
    // Add 'Open project' button

    QScreen *screen = QGuiApplication::primaryScreen();

    auto const rec = screen->availableSize();

//    auto const rec = QApplication::desktop()->screenGeometry();
    auto const height = rec.height();
    auto const width = rec.width();

    bool small_screen =  width < 1400;

//      small_screen = true;

    QWidget *w = layout->parentWidget();
    int typ = w->property("typ").toInt();
    int rows = w->property("rows").toInt();

    AutoGridLayout *al = qobject_cast<AutoGridLayout*>(layout);

    if (!al) {
        typ = 1;
    }
    else {
        al->setHorizontalSpacing(12);
        al->setVerticalSpacing(0);
        if (typ == 3) {
            al->setVerticalSpacing(8);
        }
    }

    WToolButton *button = new WToolButton(typ);

    button->setProperty("class", QString("b%1").arg(typ));

    double coef = 1.;

    if (small_screen) {
        button->setProperty("class", QString("b%1_small").arg(typ));
        coef = 0.7;
    }

    //    button->setProperty("class", "font-size: 8px;");

    if (a) {
        button->setDefaultAction(a);
    }

    int h = w->height();


    QString atext = text;

    if (text == "") {
        atext = ispr_text(a->text());
    }

    button->setText(atext);

    if (icon_name != "") {
//        button->setIcon(QIcon(icon_name));
        setMyIcon(button, icon_name);
    }

    int numRows = 1;
    int totalHeight = 60;
    int buttonSize = 48;
    int dd = 3;
    int iconSize = buttonSize * 1;

    if (typ != 1) {
        layout->setContentsMargins(dd, dd, dd, dd);  // Отступы по краям
    }

    if (typ == 2) {
        button->setToolButtonStyle(Qt::ToolButtonIconOnly);
        buttonSize = 30;
        buttonSize = 32;
        if (rows == 1) {
            buttonSize = 24;
        }

        iconSize = buttonSize;
        iconSize = 31;
//        buttonSize = 31;
//        iconSize = 31;
        numRows = 2;
    }
    else if (typ == 3) {
        button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        buttonSize = 19;
        iconSize = 14;
        numRows = 3;
    }
    else if (typ == 1) {
        if (text == "") {
            button->setToolButtonStyle(Qt::ToolButtonIconOnly);
        }
        else {
            button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        }
        buttonSize = 72;
        iconSize = 48;
        numRows = 1;
        if (text.indexOf('\n') != -1) {
            iconSize = 36;
        }
//        layout->setContentsMargins(dd, 0, dd, 0);  // Отступы по краям
    }

    //    button->setFixedSize(iconSize, iconSize);

    if (typ != 1) {
        button->setFixedHeight(buttonSize);
        if ( typ != 3) {
            button->setFixedWidth(buttonSize);
        }
    }
    else {
        button->setFixedHeight(buttonSize);


        
        button->setFixedWidth(buttonSize*0.9*coef);
    }

    button->setIconSize(QSize(iconSize, iconSize));

    //    button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    if (al) {
        al->addWidgetAuto(button);
    }
    else {
        layout->addWidget(button);
    }

    m_action_button[a] = button;


    set_menu(button, a, text);



    return button;
}

QToolButton *createButton0(Ribbon *ribbon, const QString &tabName, const QString &groupName, const QString & text, const QString & icon_name, int typ = 3)
{
    // Add 'Open project' button
    WToolButton *button = new WToolButton(typ);

    button->setText(text);
    button->setIcon(QIcon(icon_name));

    ribbon->addButton(tabName, groupName, button, typ);

    return button;
}


void ribbon_move(Ribbon *ribbon, const QString & tabName, GidrAction *gidrAction)
{
    auto *l1 = ribbon->addNest(tabName, 2,3, 2);
    if (l1) {
        createButton(l1, gidrAction->aMovew);
        createButton(l1, gidrAction->aZoom);

        createButton(l1, gidrAction->aPlus);
        createButton(l1, gidrAction->aMasall);

        createButton(l1, gidrAction->aMinus);
        createButton(l1, gidrAction->aF5);

        createButton(l1, gidrAction->aEditFindNode, "", ":/images/new/4/binoculars-find 1.svg");
//        createButton(l1, gidrAction->aF4, "", ":/images/new/3/ruler-svgrepo-com.svg");
        
        closeNest2(l1);
    }
}


//void GidWidget::create_ribbon(Ribbon *ribbon, GidrAction *gidrAction)
void GidWidget::create_ribbon(Ribbon *ribbon)
{
    //    mainLayout->addWidget(ribbon);
    //    ribbon->setTitleBarWidget(new QWidget());

#if 0
    //    ribbon->addTab(QIcon(":/icons/briefcase_1.svg"), "Проект");
    AutoGridLayout *ll01 = ribbon->addTab("Главная");
    AutoGridLayout *ll02 = ribbon->addTab("Схема");
    AutoGridLayout *ll03 = ribbon->addTab("Процессор режимов");
    AutoGridLayout *ll04 = ribbon->addTab("Тепловые потери");
//    AutoGridLayout *ll04 = ribbon->addTab("Запросы");


    AutoGridLayout *ll11 = ribbon->addTab("Технические условия");
    AutoGridLayout *ll12 = ribbon->addTab("АЛСЕКО");
#endif


    ribbon->addTab("Главная");
    ribbon->addTab("Схема");
    ribbon->addTab("Процессор режимов");
    ribbon->addTab("Тепловые потери");
//  ribbon->addTab("Запросы");

#if !USE_ITWIN
    if (get_ini()->enable_tu) {
        ribbon->addTab("Технические условия");
    }
    if (get_ini()->enable_alseco) {
        ribbon->addTab("АЛСЕКО");
    }
#endif



#if 0
    AutoGridLayout *ll05 = ribbon->addTab("Нарушения");
    AutoGridLayout *ll06 = ribbon->addTab("Шурфовка");
    AutoGridLayout *ll07 = ribbon->addTab("Осмотр");
    AutoGridLayout *ll08 = ribbon->addTab("Ремонт");
    AutoGridLayout *ll09 = ribbon->addTab("Опрессовка");
    AutoGridLayout *ll10 = ribbon->addTab("Паспортизация");
    AutoGridLayout *ll11 = ribbon->addTab("Технические условия");
    AutoGridLayout *ll12 = ribbon->addTab("Индикаторы коррозии");
    AutoGridLayout *ll13 = ribbon->addTab("Электросеть");
#endif

//    ribbon->addTab(QIcon(":/icons/engineering_1.svg"), "Tools");
//    ribbon->addTab(QIcon(":/icons/information_1.svg"), "Help");


//    QToolButton *q = createButton(gidrAction.aFilePrint, "Печать");
//    ribbon->addButton("Проект", "Проект", q);


//    QToolButton *openProjectButton = createButton(geo, QObject::tr("Открыть"), QObject::tr("Open existing project"), ":/icons/live_folder_2.svg");
//    ribbon->addButton("Проект", "Проект", openProjectButton);

//    createButton(ribbon, "Проект", "Проект", geo, QObject::tr("Новый"), QObject::tr("Create new project"), ":/icons/create_new_2.svg");

//    QToolButton *saveProjectButton = createButton(geo, QObject::tr("Сохранить"), QObject::tr("Save project"), ":/icons/save_2.svg");
//    saveProjectButton->setEnabled(false);
//    ribbon->addButton("Проект", "Проект", saveProjectButton);
#if 0

    QToolButton *openFileButton = createButton(geo, QObject::tr("Файл"), QObject::tr("Open file or directory"), ":/icons/add_folder_2.svg");

    // Add dropdown menu to button
    openFileButton->setPopupMode(QToolButton::MenuButtonPopup);
    QMenu *menu = new QMenu("Title");
    //menu->addAction(QIcon(":/icons/folder_2.svg"),
    //                "Recent directory");
    menu->addAction(QIcon(":/icons/file_2.svg"), "Recent file 1");
    menu->addAction(QIcon(":/icons/file_2.svg"), "Recent file 2");
    menu->addAction(QIcon(":/icons/file_2.svg"), "Recent file 3");


    openFileButton->setMenu(menu);

    ribbon->addButton("Проект", "Импорт", openFileButton);

    QToolButton *openDatabaseButton = createButton(geo, QObject::tr("База данных"), QObject::tr("Connect to database"), ":/icons/add_database_2.svg");
    ribbon->addButton("Проект", "Импорт", openDatabaseButton);

    QToolButton *connectWebserviceButton = createButton(geo, QObject::tr("Веб-сервер"), QObject::tr("Connect to web service"), ":/icons/add_link_2.svg");
    ribbon->addButton("Проект", "Импорт", connectWebserviceButton);
#endif

    //----------------------------------------------------------------------------------------

    AutoGridLayout *ll = nullptr;

    {


    QLayout *l01 = ribbon->getTabLayout("Главная");

    createButton(l01, gidrAction.aFile6,  "Файл", ":/images/new/big/file-svgrepo-com.svg");

    createButton(l01, gidrAction.aMainCxema, "Активный\nфрагмент", ":/images/new/big/MainCxema.svg");
    createButton(l01, gidrAction.aAddDop, "Добавить\nфрагмент", ":/images/new/big/AddDop.svg");

    auto *l = ribbon->addNest("Главная", 2,1, 4);
    {
        //        auto *l2 = ribbon->addNest("Главная", 2,4, 2);
        ll = addNest2(l, 1, 4, 2);
        createButton(ll, gidrAction.aFilePrint, "", "");
        createButton(ll, gidrAction.aFilePrintPreview, "", "");
        createButton(ll, gidrAction.aPrintFr, "", "");
        createButton(ll, gidrAction.aFilePrintSetup, "", ":/images/new/1/rectangle 1.svg");
        closeNest2(ll);
    }

    {
        ll = addNest2(l, 1, 4, 2);
        createButton(ll, gidrAction.aInfo, "", "");
        createButton(ll, gidrAction.aViewDb, "", "");
        createButton(ll, gidrAction.aMoveTo, "", ":/images/new/1/turn-left-arrows 1.svg");
        createButton(ll, gidrAction.aNovisual, "", ":/images/new/1/settings 1.svg");
        closeNest2(ll);
    }
    //    ribbon->
    closeNest2(l);


    ribbon_move(ribbon, "Главная", &gidrAction);



    //    createButton(ribbon, "Главная", "Файл", gidrAction.aFilePrintPreview, "");

    if (1) {
        ll = ribbon->addNest("Главная", 3,1, 3);

//        createButton(ll, gidrAction.aInf, "", ":/images/new/3/document-info 1.svg");
//        createButton(ll, gidrAction.aPodp, "", ":/images/new/3/eye-view-svgrepo-com 1.svg");
//        createButton(ll, gidrAction.aPodpRes, "Отображать скрытые имена", ":/images/new/3/hash-svgrepo-com 1.svg");

        createButton(ll, gidrAction.aInf, "", ":/images/new/3/document-info 1.svg");
        createButton(ll, gidrAction.aGidinf, "", ":/images/new/3/hash-svgrepo-com 1.svg");
        createButton(ll, gidrAction.aPodp, "", ":/images/new/3/eye-view-svgrepo-com 1.svg");

        closeNest2(ll);

        createButton(l01, gidrAction.aFind6,  "Поиск", ":/images/new/big/query-svgrepo-com.svg");


        ll = ribbon->addNest("Главная", 3,1, 3);

        createButton(ll, gidrAction.aF4, "", ":/images/new/3/ruler-svgrepo-com.svg");
//        createButton(ll, gidrAction.aPodpRes, "Отображение скрытых узлов", "");
        createButton(ll, gidrAction.aPodpAll, "Отображение надписей", "");

        closeNest2(ll);
    
    
    }


    ll = ribbon->addNest("Главная", 2,1, 4);
    {
        //        auto *l2 = ribbon->addNest("Главная", 2,4, 2);
        auto *l2 = addNest2(ll, 1, 4, 2);

        createButton(l2, gidrAction.aOut2, "", ":/images/new/4/sum 1.svg");
        createButton(l2, gidrAction.aNaprGid, "", ":/images/new/4/up-down-arrow 1.svg");
//        createButton(l2, gidrAction.aEditFindNode, "", ":/images/new/4/binoculars-find 1.svg");
        closeNest2(l2);
    }

    {
        auto *l2 = addNest2(ll, 1, 4, 2);
        createButton(l2, gidrAction.aFragment, "", ":/images/new/7/select-all-svgrepo-com 1.svg");
        createButton(l2, gidrAction.aEditCopy, "", ":/images/new/7/copy.svg");
//        createButton(l2, gidrAction.aEditCopyPng, "", ":/images/new/7/png-file-extension-interface-symbol-svgrepo-com 1.svg");
        closeNest2(l2);
    }
    closeNest2(ll);

    ll = ribbon->addNest("Главная", 2,1, 4);
    {
        auto *l2 = addNest2(ll, 1, 4, 2);
        createButton(l2, gidrAction.aEditUndo, "", ":/images/new/4/curve-arrow-left 1.svg");
        createButton(l2, gidrAction.aEditUndo, "", ":/images/new/4/curve-arrow-left 2.svg");
        closeNest2(l2);
    }

    {
        auto *l2 = addNest2(ll, 1, 4, 2);
        createButton(l2, gidrAction.aFragmentPoly, "", ":/images/new/7/select-object.svg");
        createButton(l2, gidrAction.aMoveVyd, "", ":/images/new/7/select-svgrepo-com 1.svg");
        closeNest2(l2);
    }
    closeNest2(ll);


//    QLayout *l01 = ribbon->getTabLayout("Главная");
    createButton(l01, gidrAction.aSelect6, "Выделить",  ":/images/new/big/select-window 1.svg");
    createButton(l01, gidrAction.aExcel2,  "Excel",   ":/images/new/big/file-excel.svg");
    createButton(l01, gidrAction.aExport6, "Экспорт",   ":/images/new/big/export.svg");
    createButton(l01, gidrAction.aRezhim6, "Режим",   ":/images/new/big/switch-double-svgrepo-com 1.svg");
    createButton(l01, gidrAction.aSetup6,  "Настройка", ":/images/new/big/setup.svg");


    createButton(l01, gidrAction.aGeobaza6,  "Геобаза", ":/images/new/big/geobaza.svg");


#if USE_GID9

    if (is_gid9()) {
    ll = ribbon->addNest("Главная", 2,1, 4);
    {
        auto *l2 = addNest2(ll, 2, 1, 2);
        createButton(l2, gidrAction.aResetGid, "", ":/images/sync/sync1.svg");
        createButton(l2, gidrAction.aToGid9, "", ":/images/sync/sync2.svg");
        closeNest2(l2);
    }
    }

#endif
/*
    {
        auto *l2 = addNest2(ll, 1, 4, 2);
        createButton(l2, gidrAction.aFragmentPoly, "", ":/images/new/7/select-object.svg");
        createButton(l2, gidrAction.aMoveVyd, "", ":/images/new/7/select-svgrepo-com 1.svg");
        closeNest2(l2);
    }
*/
    closeNest2(ll);


#if 0
gidrAction.aResetGid = addTrigger(this, tr("Перезагрузить схему"), "", "", SLOT(onResetGid()));
gidrAction.aToGid9 = addTrigger(this, tr("Синхронизировать -> gid9"), "", "", SLOT(onToGid9()));
#endif




//    createButton(l01, gidrAction.aFind6,  "Поиск", ":/images/new/big/query-svgrepo-com.svg");

//    createButton(ribbon, "Главная", "", gidrAction.aRemontOsmotrTeploSet, ":/images/new/big/select-window 1.svg");

//    closeNest2(l);




#if 0
//    auto a = createButton(ribbon, "Главная", "Файл", nullptr, "Меню", 3);
    auto a = createButton0(ribbon, "Главная", "Файл", "Меню", ":/images4/documents.svg", 1);

    QMenu *menu = new QMenu(ribbon);
    menu->addAction(QObject::tr("&First Item"));
    menu->addAction(QObject::tr("&Second Item"));
    menu->addAction(QObject::tr("&Third Item"));
    menu->addAction(QObject::tr("F&ourth Item"));
    a->setMenu(menu);
    a->setPopupMode(QToolButton::InstantPopup);
#endif

#if 0
        auto *l21 = ribbon->addNest("Главная", 2,1, 2);
        {
//        createButton(l21, gidrAction.aFilePrint, "");
//        createButton(l21, gidrAction.aPrintFr, "");
            createButton(l21, gidrAction.aExcel2, "");
//        createButton(l21, gidrAction.aZam, "");
            createButton(l21, gidrAction.aEditCopy, "");
            closeNest2(l21);
        }

        auto *l22 = ribbon->addNest("Главная", 2,1, 2);
        {
            createButton(l22, gidrAction.aEditFindNode, "");
            createButton(l22, gidrAction.aNovisual, "");
            createButton(l22, gidrAction.aInfo, "");
            createButton(l22, gidrAction.aMoveTo, "");
            createButton(l22, gidrAction.aViewDb, "");
            closeNest2(l22);
        }

        auto *l23 = ribbon->addNest("Главная", 2,1, 2);
        {
            createButton(l23, gidrAction.aDoItDr, "");
            createButton(l23, gidrAction.aDoIt, "");
            createButton(l23, gidrAction.aTeplopoteri, "");
            createButton(l23, gidrAction.aOut2, "");
            createButton(l23, gidrAction.aAnal, "");
            createButton(l23, gidrAction.aNapr, "");
            createButton(l23, gidrAction.aPjezo, "");
            closeNest2(l23);
        }

        auto *l25 = ribbon->addNest("Главная", 2,1, 2);
        {
            createButton(l25, gidrAction.aNaprGid, "");
            createButton(l25, gidrAction.aInf, "");
            createButton(l25, gidrAction.aGidinf, "");
            createButton(l25, gidrAction.aPodp, "");
            createButton(l25, gidrAction.aEditor1, "");
            createButton(l25, gidrAction.aEditor4, "");
            createButton(l25, gidrAction.aF4, "");
            createButton(l25, gidrAction.aHttp2, "");
            closeNest2(l25);
        }
#endif
    }

//----------------------------------------------------------------------------------------


    QLayout *l02 = ribbon->getTabLayout("Схема");

    {                                             
        createButton(l02, gidrAction.aNovisual, "Системные\nпараметры", ":/images/new/1/settings 1.svg");
        
        ll = ribbon->addNest("Схема", 2, 5, 2);

#if 0
Узел
линия
инфо
зн
перенести
Удалить


тип
разъед
поворот

перенести линию
закрыть
внутр сх

#endif


        createButton(ll, gidrAction.aNode, "", ":/images/new/8/node.svg");
        createButton(ll, gidrAction.aRename, "", ":/images/new/8/type-svgrepo-com.svg");

        createButton(ll, gidrAction.aLine, "", ":/images/new/8/line-tool-svgrepo-com 1.svg");
        createButton(ll, gidrAction.aNodeMove1, "", ":/images/new/8/flow-parallel-svgrepo-com 1.svg");

        createButton(ll, gidrAction.aInfo, "", ":/images/new/8/info-svgrepo-com 1.svg");
        createButton(ll, gidrAction.aSwap, "", ":/images/new/8/swap.svg");

        createButton(ll, gidrAction.aZn, "", ":/images/new/8/flag-2-svgrepo-com 1.svg");
        createButton(ll, gidrAction.aMoveLineEnd, "", ":/images/new/8/moveline.svg");

        createButton(ll, gidrAction.aNodeMove, "", ":/images/new/8/move.svg");
        createButton(ll, gidrAction.aOpenClose, "", ":/images/new/8/open_close.svg");

        createButton(ll, gidrAction.aDel, "", ":/images/new/8/delete-svgrepo-com 1.svg");
        createButton(ll, gidrAction.aKam, "", ":/images/new/8/kam.svg");



#if 0
        createButton(ll, gidrAction.aNode, "", ":/images/new/8/node.svg");
        createButton(ll, gidrAction.aNodeMove, "", ":/images/new/8/move.svg");

        createButton(ll, gidrAction.aDel, "", ":/images/new/8/delete-svgrepo-com 1.svg");
        createButton(ll, gidrAction.aNodeMove1, "", ":/images/new/8/flow-parallel-svgrepo-com 1.svg");

        createButton(ll, gidrAction.aLine, "", ":/images/new/8/line-tool-svgrepo-com 1.svg");
        createButton(ll, gidrAction.aSwap, "", ":/images/new/8/swap.svg");

        createButton(ll, gidrAction.aInfo, "", ":/images/new/8/info-svgrepo-com 1.svg");
        createButton(ll, gidrAction.aRename, "", ":/images/new/8/select-product-svgrepo-com 1.svg");

        createButton(ll, gidrAction.aZn, "", ":/images/new/8/flag-2-svgrepo-com 1.svg");
        createButton(ll, gidrAction.aMoveLineEnd, "", ":/images/new/8/moveline.svg");

        createButton(ll, gidrAction.aOpenClose, "", ":/images/new/8/open_close.svg");
        createButton(ll, gidrAction.aKam, "", ":/images/new/8/kam.svg");
#endif
        

        closeNest2(ll);

        createButton(l02, gidrAction.aText,  "Установить\nтекст", ":/images/new/big/text-field-focus-svgrepo-com 1.svg");


        ribbon_move(ribbon, "Схема", &gidrAction);


        ll = ribbon->addNest("Схема", 2, 5, 2);

        createButton(ll, gidrAction.aPtsAdd, "", ":/images/new/9/add.svg");
        createButton(ll, gidrAction.aPtsTable, "", ":/images/new/9/table.svg");

        createButton(ll, gidrAction.aPtsDel, "", ":/images/new/9/del.svg");
//        createButton(ll, gidrAction.aSetMsNew, "", ":/images/new/9/MS.svg");

//        createButton(ll, gidrAction.aSetRsNew, "", ":/images/new/9/RS.svg");

//        createButton(ll, gidrAction.aEditFindPs, "", ":/images/new/9/find.svg");
        createButton(ll, gidrAction.aPtsInfo, "", ":/images/new/9/info.svg");

        createButton(ll, gidrAction.aPassportOnOff, "", ":/images/new/9/view.svg");

        closeNest2(ll);

//        ll = ribbon->addNest("Схема", 2, 5, 2);
//        createButton(ll, gidrAction.aNodeNeotr   , "", ":/images/zapros/ZapNezak.png"); // Установка конвертированных узлов\tC
//        createButton(ll, gidrAction.aZapNezak    , "", ":/images/new/11/ZapNezak.svg"); // Незаконченные узлы                 
//        createButton(ll, gidrAction.aSetCoordNull, "", ":/images/new/11/SetCoordNull.svg"); // Удалить координаты выделенных узлов
//        closeNest2(ll);


#if 1
        ll = ribbon->addNest("Схема", 2, 5, 2);

/*
        createButton(ll, gidrAction.aNode, "Выделить расчетную схему", ":/images/new/10/Vydelit raschetnuju shemu.svg");
        createButton(ll, gidrAction.aMark, "", ":/images/new/10/Vydelit oblast raschetnoj shemy.svg");

        createButton(ll, gidrAction.aNode, "", ":/images/new/10/Vybrat napravlenie po magistrale.svg");
        createButton(ll, gidrAction.aMarkDel, "", ":/images/new/10/Ubrat vydelenie.svg");

        createButton(ll, gidrAction.aNapr, "", ":/images/new/10/Vybrat napravlenie.svg");
        createButton(ll, gidrAction.aMsNode1, "", ":/images/new/10/Vybrat nachalnyj uzel uchastka MS.svg");

        createButton(ll, gidrAction.aVyd4, "", ":/images/new/10/Vydelit fragment seti.svg");
        createButton(ll, gidrAction.aMsNode2, "", ":/images/new/10/Vybrat konechnyj uzel uchastka MS.svg");

        createButton(ll, gidrAction.aNapr2, "", ":/images/new/10/Vydelit fragment seti muzhdu uzlami.svg");
        createButton(ll, gidrAction.aSaveMAG, "", ":/images/new/10/Mag.svg");

        createButton(ll, gidrAction.aMarkRev, "", ":/images/new/10/Dobavit udalit truboprovod v konture.svg");

        createButton(ll, gidrAction.aSaveMS, "", ":/images/new/10/MS.svg");

        createButton(ll, gidrAction.aMsPassport, "", ":/images/new/10/Pasport.svg");
        createButton(ll, gidrAction.aSaveRS, "", ":/images/new/10/RS.svg");
*/

//        createButton(ll, gidrAction.aNode, "Выделить расчетную схему", ":/images/new/10/Vydelit raschetnuju shemu.svg");
        createButton(ll, gidrAction.aMark, "", ":/images/new/10/Vydelit oblast raschetnoj shemy.svg");

//        createButton(ll, gidrAction.aNode, "", ":/images/new/10/Vybrat napravlenie po magistrale.svg");
        createButton(ll, gidrAction.aMarkDel, "", ":/images/new/10/Ubrat vydelenie.svg");

        createButton(ll, gidrAction.aNapr, "", ":/images/new/10/Vybrat napravlenie.svg");
//        createButton(ll, gidrAction.aMsNode1, "", ":/images/new/10/Vybrat nachalnyj uzel uchastka MS.svg");

        createButton(ll, gidrAction.aVyd4, "", ":/images/new/10/Vydelit fragment seti.svg");
//        createButton(ll, gidrAction.aMsNode2, "", ":/images/new/10/Vybrat konechnyj uzel uchastka MS.svg");

        createButton(ll, gidrAction.aNapr2, "", ":/images/new/10/Vydelit fragment seti muzhdu uzlami.svg");
        createButton(ll, gidrAction.aSaveMAG, "", ":/images/new/10/Mag.svg");

        createButton(ll, gidrAction.aMarkRev, "", ":/images/new/10/Dobavit udalit truboprovod v konture.svg");

        createButton(ll, gidrAction.aSaveMS, "", ":/images/new/10/MS.svg");

        createButton(ll, gidrAction.aMsPassport, "", ":/images/new/10/Pasport.svg");
        createButton(ll, gidrAction.aSaveRS, "", ":/images/new/10/RS.svg");

        closeNest2(ll);
        
        createButton(l02, gidrAction.aAktCreate,  "Акты\nраздела", ":/images/new/big/act.svg");

#endif

#if 0
        
        ll = ribbon->addNest("Схема", 2, 5, 2);

        createButton(ll, gidrAction.a, "", ":/images/new/");
        createButton(ll, gidrAction.a, "", ":/images/new/");
        createButton(ll, gidrAction.a, "", ":/images/new/");
        createButton(ll, gidrAction.a, "", ":/images/new/");
        createButton(ll, gidrAction.a, "", ":/images/new/");
        createButton(ll, gidrAction.a, "", ":/images/new/");
        createButton(ll, gidrAction.a, "", ":/images/new/");
        createButton(ll, gidrAction.a, "", ":/images/new/");
        createButton(ll, gidrAction.a, "", ":/images/new/");
        createButton(ll, gidrAction.a, "", ":/images/new/");
        createButton(ll, gidrAction.a, "", ":/images/new/");
        createButton(ll, gidrAction.a, "", ":/images/new/");
        createButton(ll, gidrAction.a, "", ":/images/new/");
        createButton(ll, gidrAction.a, "", ":/images/new/");

        closeNest2(ll);
#endif
//        ribbon_move(ribbon, "Схема", gidrAction);
    }


    {

        QLayout *l03 = ribbon->getTabLayout("Процессор режимов");

        createButton(l03, gidrAction.aMainCxema, "Активный\nфрагмент", ":/images/new/big/MainCxema.svg");
        createButton(l03, gidrAction.aAddDop, "Добавить\nфрагмент", ":/images/new/big/AddDop.svg");

        auto button = createButton(l03, gidrAction.aDoItDr, "Плановый\nрежим",  ":/images/new/big/doitdr.svg");
        createButton(l03, gidrAction.aDoIt, "Фактический\nрежим",  ":/images/new/big/doit.svg");


        ll = ribbon->addNest("Процессор режимов", 2, 5, 2);
//        createButton(ll, gidrAction.aDoItDr, "", "");
//        createButton(ll, gidrAction.aDoIt, "", "");

        createButton(ll, gidrAction.aNapr, "", ":/images/new/pjezo/pjezo_v.svg");
        createButton(ll, gidrAction.aPjezo, "", ":/images/new/pjezo/pjezo.svg");
        createButton(ll, gidrAction.aSavePjezo, "", ":/images/new/pjezo/pjezo_save.svg");
        createButton(ll, gidrAction.aListPjezo, "", ":/images/new/pjezo/pjezo_list.svg");

        closeNest2(ll);

        ribbon_move(ribbon, "Процессор режимов", &gidrAction);

        ll = ribbon->addNest("Процессор режимов", 3,1, 3);

        createButton(ll, gidrAction.aZapOtr, "Отрицательные перепады", "");
        createButton(ll, gidrAction.aPtTempMin, "Низкие температуры", "");
        createButton(ll, gidrAction.aZavozd, "Завоздушивание", "");

        closeNest2(ll);

        createButton(l03, gidrAction.aOut2, "Результат", ":/images/new/4/sum 1.svg");
        createButton(l03, gidrAction.aExcel2,  "Excel",   ":/images/new/big/file-excel.svg");

        createButton(l03, gidrAction.aAnal, "Анализ", ":/images/new/big/a/Analiz.svg");
#if !USE_ITWIN

        createButton(l03, gidrAction.aAdmissibilityAnalysis, "Анализ\nрежима", ":/images/new/big/a/AdmissibilityAnalysis.svg");
#endif
        createButton(l03, gidrAction.aAnalVyd, "Анализ\nвыделение", ":/images/new/big/a/AnalizVyd.svg");
        createButton(l03, gidrAction.aZaprosy, "Запросы", ":/images/new/big/a/Queries.svg");
        createButton(l03, gidrAction.aZaprosy2, "Анализ\nсети", ":/images/new/big/setup.svg");

#if USE_ITWIN
#if _WIN32
        createButton(l03, gidrAction.aRunProgram, "Оптимизация\nисточников", ":/images/new/big/setup_blue.svg");
#endif
#endif
        
//        ribbon_move(ribbon, "Процессор режимов", gidrAction);
    }    




#if !USE_ITWIN

    if (get_ini()->enable_tu)
    {

//        QLayout *l11 = ribbon->getTabLayout("Технические условия");

        ll = ribbon->addNest("Технические условия", 2,1, 2);
        {
            createButton(ll, gidrAction.aTuNew    , "");
            createButton(ll, gidrAction.aTuLine   , "");
            closeNest2(ll);
        }

        ll = ribbon->addNest("Технические условия", 2,1, 2);
        {
        //    bar->addSeparator();
            createButton(ll, gidrAction.aTuInfo   , "");
            createButton(ll, gidrAction.aTuDel    , "");
        //    bar->addSeparator();
            createButton(ll, gidrAction.aTuFind   , "");
            createButton(ll, gidrAction.aTuSost   , "");
        //    bar->addSeparator();
            createButton(ll, gidrAction.aTuExcel  , "");
            createButton(ll, gidrAction.aTuSvod   , "");
            createButton(ll, gidrAction.aTuZhurnal, "");

            createButton(ll, gidrAction.aEditUndoTU, "", ":/images/new/4/curve-arrow-left 1.svg");
            
            closeNest2(ll);
        }

        ribbon_move(ribbon, "Технические условия", &gidrAction);
    }


    if (get_ini()->enable_alseco) 
    {
        QLayout *l11 = ribbon->getTabLayout("АЛСЕКО");

        createButton(l11, gidrAction.aNagrAlmatyTable, "Объекты АЛСЕКО", ":/images/alseco/alseco_00.bmp");
//        createButton(l11, gidrAction.aNagrAlmatyAdd1,  "Добавить Объект (МЖД) АЛСЕКО", ":/images/alseco/alseco_01.bmp");
//        createButton(l11, gidrAction.aNagrAlmatyAdd2,  "Добавить Объект (Прочие) АЛСЕКО", ":/images/alseco/alseco_02.bmp");
        createButton(l11, gidrAction.aNagrAlmaty,      "Объекты АЛСЕКО по адресам", ":/images/alseco/alseco_03.bmp");
        createButton(l11, gidrAction.aNagrAlmatyOnOff, "Вывести нагрузки АЛСЕКО", ":/images/alseco/alseco_04.bmp");
        createButton(l11, gidrAction.aNagrAlmaXlsx,    "Реестр объектов АЛСЕКО", ":/images/alseco/alseco_05.bmp");
        createButton(l11, gidrAction.aNagrAlmaNenaid1, "Реестр ненайденных объектов (МЖД) АЛСЕКО", ":/images/alseco/alseco_06.bmp");
        createButton(l11, gidrAction.aNagrAlmaNenaid2, "Реестр ненайденных объектов (Прочие) АЛСЕКО", ":/images/alseco/alseco_07.bmp");
        createButton(l11, gidrAction.aNagrAlmaNenaid3, "Здания АЛСЕКО без потребителя", ":/images/alseco/alseco_08.bmp");

        ribbon_move(ribbon, "АЛСЕКО", &gidrAction);
    }



#endif


    {
        QLayout *l03 = ribbon->getTabLayout("Тепловые потери");
        
        createButton(l03, gidrAction.aTeplopoteri, "Тепловые\nпотери",  ":/images/new/big/doitdr.svg");
        ribbon_move(ribbon, "Тепловые потери", &gidrAction);
    }    




#if 0
    //    m_barZapr = bar;

    //  bar = main_window->addToolBar(tr("Нарушения"));
    //  bar->setObjectName(tr("Нарушения"));


    {
        ll = ribbon->addNest("Нарушения", 2,1, 2);
        {

            createButton(ll, gidrAction.aRemontPovrDefAdd, "", ""); // Установить нарушение
            createButton(ll, gidrAction.aRemontPovrDefDel, "", ""); // Удалить нарушение
            createButton(ll, gidrAction.aRemontPovrDefMove, "", ""); // Перенести нарушение
            createButton(ll, gidrAction.aRemontPovrDefInfo, "", ""); // Свойства объекта Нарушение

            closeNest2(ll);
        }

        ll = ribbon->addNest("Нарушения", 2,1, 2);
        {

            //    bar->addSeparator();
            createButton(ll, gidrAction.aDefectZhurnalExpluatacia, "", ""); // Нарушения эксплуатации
            createButton(ll, gidrAction.aDefectZhurnalShurf, "", ""); // Нарушения в шурфе
            createButton(ll, gidrAction.aDefectZhurnalOsmotr, "", ""); // Нарушения осмотра
            createButton(ll, gidrAction.aDefectZhurnalOpress, "", ""); // Нарушения опрессовки
            //    bar->addSeparator();
            createButton(ll, gidrAction.aDefectZhurnalNarushenie, "", ""); // Нарушения до ремонта
            createButton(ll, gidrAction.aDefectZhurnalRemont, "", ""); // Нарушения в процессе ремонта
            createButton(ll, gidrAction.aDefectZhurnalPosleRemont, "", ""); // Нарушения после ремонта
            createButton(ll, gidrAction.aDefectZhurnalAll, "", ""); // Отобразить все нарушения
            closeNest2(ll);
        }

        ll = ribbon->addNest("Нарушения", 2,1, 2);
        {

            createButton(ll, gidrAction.aDefectWord1, "", ""); // Журнал нарушений
            createButton(ll, gidrAction.aDefectWord2, "", ""); // Нарушения/ремонты трубопроводов тепловой сети(документ)
            //    bar->addSeparator();
            createButton(ll, gidrAction.aDefectDocTable, "", ""); // Нарушения/ремонты трубопроводов тепловой сети
            createButton(ll, gidrAction.aDefectAnaliz, "", ""); // Анализ нарушений
            //    bar->addSeparator();

            closeNest2(ll);
        }

        ll = ribbon->addNest("Нарушения", 2,1, 2);
        {

            createButton(ll, gidrAction.aRemontPovrDefOtobr, "", ""); // Включить/отключить отображение объектов
            //    bar->addSeparator();
            createButton(ll, gidrAction.aRemontPovrOtop, "", ""); // Выбор отопительного сезона
            createButton(ll, gidrAction.aRemontOsmotrTeploSet, "", ""); // Выбор фрагмента сети по Начальнику участка
            ////    bar->addSeparator();
            //      createButton(ll, "Нарушения", "Гидравлика", gidrAction.aDefectBezUch, "", ""); // Нарушения без участков
            //    m_barDefect = bar;
            closeNest2(ll);
        }

        ribbon_move(ribbon, "Нарушения", gidrAction);

    }

    {
        ll = ribbon->addNest("Шурфовка", 2,1, 2);
        {

            //  bar = main_window->addToolBar(tr("Шурфовка"));
            //  bar->setObjectName(tr("Шурфовка"));
            createButton(ll, gidrAction.aRemontPovrShurfAdd, ""); // Установить плановый шурф
            createButton(ll, gidrAction.aRemontPovrShurfAddNeplan, ""); // Установить НЕплановый шурф
            createButton(ll, gidrAction.aRemontPovrShurfDel, ""); // Удалить шурф
            createButton(ll, gidrAction.aRemontPovrShurfMove, ""); // Перенести шурф
            createButton(ll, gidrAction.aRemontPovrShurfInfo, ""); // Свойства объекта Шурф
            closeNest2(ll);
        }

        ll = ribbon->addNest("Шурфовка", 2,1, 2);
        {

            //    bar->addSeparator();
            createButton(ll, gidrAction.aRemontShurfPlan, ""); // Плановые шурфы
            createButton(ll, gidrAction.aRemontShurfPredpis, ""); // Шурфы предписания
            createButton(ll, gidrAction.aRemontShurfNarush, ""); // Шурфы нарушения
            //    bar->addSeparator();
            createButton(ll, gidrAction.aShurfProcess, ""); // Шурфы в процессе выполнения
            createButton(ll, gidrAction.aShurfVypolneno, ""); // Шурфы выполненные
            createButton(ll, gidrAction.aShurfAll, ""); // Отобразить все Шурфы
            closeNest2(ll);
        }

        ll = ribbon->addNest("Шурфовка", 2,1, 2);
        {

            //    bar->addSeparator();
            createButton(ll, gidrAction.aRemontShurfPlanUtverdit, ""); // Утвердить План шурфов Отопительного сезона
            //    bar->addSeparator();
            createButton(ll, gidrAction.aRemontShurfPlanExcel, ""); // План шурфов (документ)
            createButton(ll, gidrAction.aRemontShurfPlanExcelMonth, ""); // План шурфов по месяцам (документ)
            createButton(ll, gidrAction.aShurfTable, ""); // Анализ Плана шурфов
            closeNest2(ll);
        }

        ll = ribbon->addNest("Шурфовка", 2,1, 2);
        {

            //    bar->addSeparator();
            createButton(ll, gidrAction.aRemontShurfOtobr, ""); // Включить/отключить отображение объектов
            //    bar->addSeparator();
            createButton(ll, gidrAction.aRemontPovrOtop, ""); // Выбор отопительного сезона
            createButton(ll, gidrAction.aRemontOsmotrTeploSet, ""); // Выбор фрагмента сети по Начальнику участка
            //    bar->addSeparator();
            createButton(ll, gidrAction.aShurfBezUch, ""); // Шурфовки без участков
            closeNest2(ll);
        }

        ribbon_move(ribbon, "Шурфовка", gidrAction);
    }
    //    m_barShurf = bar;

    //  bar = main_window->addToolBar(tr("Осмотр"));
    //  bar->setObjectName(tr("Осмотр"));

    {
        ll = ribbon->addNest("Осмотр", 2,1, 2);
        {

            createButton(ll, gidrAction.aOsmotrAdd, ""); // Создать контур осмотра
            createButton(ll, gidrAction.aOsmotrSave, ""); // Обновить сохранение контура
            createButton(ll, gidrAction.aListOsmotr2, ""); // Выполнить осмотр контура
            closeNest2(ll);
        }

        ll = ribbon->addNest("Осмотр", 2,1, 2);
        {
            createButton(ll, gidrAction.aLastOsmotr2, ""); // Последний контур осмотра
            createButton(ll, gidrAction.aLastFaktory, ""); // Выбор трубопроводов для осмотра

            createButton(ll, gidrAction.aOsmotrZapros1, ""); // Осмотренные фрагменты сети
            createButton(ll, gidrAction.aOsmotrZapros2, ""); // Осмотренные трубопроводы за период
            createButton(ll, gidrAction.aOsmotrZapros3, ""); // Неосмотренные трубопроводы за период
            //    bar->addSeparator();
            createButton(ll, gidrAction.aOsmotrAnaliz2, ""); // Анализ осмотра контуров
            createButton(ll, gidrAction.aOsmotrAnaliz, ""); // Анализ осмотра трубопроводов контура
            //    bar->addSeparator();
            closeNest2(ll);
        }

        ll = ribbon->addNest("Осмотр", 2,1, 2);
        {
            createButton(ll, gidrAction.aRemontPovrDefOtobr, ""); // Включить/отключить отображение объектов
            //    bar->addSeparator();
            createButton(ll, gidrAction.aRemontPovrOtop, ""); // Выбор отопительного сезона
            createButton(ll, gidrAction.aRemontOsmotrTeploSet, ""); // Выбор фрагмента сети по Начальнику участка
            closeNest2(ll);
        }

        ribbon_move(ribbon, "Осмотр", gidrAction);
    }

    //      m_barOsmotr = bar;

    //  bar = main_window->addToolBar(tr("Ремонт"));
    //  bar->setObjectName(tr("Ремонт"));
    {

        ll = ribbon->addNest("Ремонт", 2,1, 2);
        {
            createButton(ll, gidrAction.aRemontAddPlan, ""); // Сохранить контур плана ремонта
            createButton(ll, gidrAction.aRemontAddCurrent, ""); // Сохранить контур текущего ремонта
            createButton(ll, gidrAction.aRemontSave, ""); // Обновить сохранение контура
            closeNest2(ll);
        //    bar->addSeparator();
        }

        ll = ribbon->addNest("Ремонт", 2,1, 2);
        {
            createButton(ll, gidrAction.aListRemont2, ""); // Список контуров ремонтов
            createButton(ll, gidrAction.aLastRemont2, ""); // Последний контур ремонта
        //    bar->addSeparator();
            createButton(ll, gidrAction.aRemontTrub, ""); // Выбор трубопроводов контура ремонта
            closeNest2(ll);

        }

        ll = ribbon->addNest("Ремонт", 2,1, 2);
        {
        //    bar->addSeparator();
            createButton(ll, gidrAction.aRemontPlan, ""); // Контуры капитального/инвестиционного ремонтов
            createButton(ll, gidrAction.aRemontCurrent, ""); // Контуры текущих ремонтов
        //    bar->addSeparator();
            createButton(ll, gidrAction.aRemontProcess, ""); // Ремонт в процессе выполнения
            createButton(ll, gidrAction.aRemontVypolneno, ""); // Законченные ремонты
            createButton(ll, gidrAction.aRemontAll2, ""); // Отобразить все контура

            closeNest2(ll);
        }

        ll = ribbon->addNest("Ремонт", 2,1, 2);
        {

        //    bar->addSeparator();
            createButton(ll, gidrAction.aRemontWord1, ""); // График выполнения ремонтов (документ)
            createButton(ll, gidrAction.aRemontWord2, ""); // План капитальных/инвестиционных ремонтов (документ)
            createButton(ll, gidrAction.aRemontWord3, ""); // План ремонтов по месяцам (документ)
        //    bar->addSeparator();
            createButton(ll, gidrAction.aRemontAnaliz2, ""); // Анализ контуров ремонта
            createButton(ll, gidrAction.aRemontAnaliz, ""); // График выполнения ремонтов
        //    bar->addSeparator();
            closeNest2(ll);
        }

        ll = ribbon->addNest("Ремонт", 2,1, 2);
        {
            createButton(ll, gidrAction.aRemontPovrDefOtobr, ""); // Включить/отключить отображение объектов
        //    bar->addSeparator();
            createButton(ll, gidrAction.aRemontPovrOtop, ""); // Выбор отопительного сезона
            createButton(ll, gidrAction.aRemontOsmotrTeploSet, ""); // Выбор фрагмента сети по Начальнику участка
        //    bar->addSeparator();
            createButton(ll, gidrAction.aRemontVyborNach, ""); // Создать контур ремонта
            createButton(ll, gidrAction.aMarkRev, ""); // Добавить/удалить трубопровод к контуру
            closeNest2(ll);
        }

        ribbon_move(ribbon, "Ремонт", gidrAction);
    }

    {
        ll = ribbon->addNest("Опрессовка", 2,1, 2);
        {
        //    m_barRemont = bar;

        //  bar = main_window->addToolBar(tr("Опрессовка"));
        //  bar->setObjectName(tr("Опрессовка"));
            createButton(ll, gidrAction.aOpresAddPlan, ""); // Сохранить контур плана опрессовки
            createButton(ll, gidrAction.aOpresSave, ""); // Обновить сохранение контура
            closeNest2(ll);
        }

        ll = ribbon->addNest("Опрессовка", 2,1, 2);
        {
            createButton(ll, gidrAction.aOpresGranitsaRazdela, ""); // Границы раздела
            createButton(ll, gidrAction.aListOpres, ""); // Список контуров опрессовок
            createButton(ll, gidrAction.aLastOpres, ""); // Последний контур опрессовки
        //    bar->addSeparator();
            closeNest2(ll);
        }

        ll = ribbon->addNest("Опрессовка", 2,1, 2);
        {
            createButton(ll, gidrAction.aOpresPlan, ""); // Контуры планов опрессовок сезона
            createButton(ll, gidrAction.aOpresProcess, ""); // Опрессовка в процессе выполнения
            createButton(ll, gidrAction.aOpresVypolneno, ""); // Завершенные опрессовки
            createButton(ll, gidrAction.aOpresAll2, ""); // Отобразить все контура
            closeNest2(ll);
        }

        ll = ribbon->addNest("Опрессовка", 2,1, 2);
        {
            createButton(ll, gidrAction.aOpresWord1, ""); // План опрессовок (документ)
            createButton(ll, gidrAction.aOpresWord2, ""); // График опрессовок по месяцам (документ)
        //    bar->addSeparator();
            createButton(ll, gidrAction.aOpresAnaliz2, ""); // Анализ контуров опрессовок
            createButton(ll, gidrAction.aOpresAnaliz, ""); // Анализ нарушений контура
            closeNest2(ll);
        }

        ll = ribbon->addNest("Опрессовка", 2,1, 2);
        {
            createButton(ll, gidrAction.aRemontPovrDefOtobr, ""); // Включить/отключить отображение объектов
        //    bar->addSeparator();
            createButton(ll, gidrAction.aRemontPovrOtop, ""); // Выбор отопительного сезона
            createButton(ll, gidrAction.aRemontOsmotrTeploSet, ""); // Выбор фрагмента сети по Начальнику участка
        //    bar->addSeparator();
            createButton(ll, gidrAction.aOpresVyborNach, ""); // Создать контур опрессовки
            createButton(ll, gidrAction.aListRemont2, ""); // Список контуров ремонтов
            createButton(ll, gidrAction.aMarkRev, ""); // Добавить/удалить трубопровод к контуру
            closeNest2(ll);
        }

        ribbon_move(ribbon, "Опрессовка", gidrAction);
    }
    //    m_barOpres = bar;


    //  bar = main_window->addToolBar(tr("Паспортизация"));
    //  bar->setObjectName(tr("Паспортизация"));
    //      bar->addAction(gidrAction.);
    {
        auto *ll = ribbon->addNest("Паспортизация", 2,1, 2);
        {

            createButton(ll, gidrAction.aPtsAdd, "");
            createButton(ll, gidrAction.aPtsDel, "");
            createButton(ll, gidrAction.aPtsInfo, "");
            closeNest2(ll);
        }

        ll = ribbon->addNest("Паспортизация", 2,1, 2);
        {
            //    bar->addSeparator();
            createButton(ll, gidrAction.aPtsTable, "");
            //    bar->addSeparator();
            createButton(ll, gidrAction.aSetMsNew, "");
            createButton(ll, gidrAction.aSetRsNew, "");
            //    bar->addSeparator();
            createButton(ll, gidrAction.aEditFindPs, "");
            //    bar->addSeparator();
            createButton(ll, gidrAction.aRasprMagNew, "");
            //    bar->addSeparator();
            createButton(ll, gidrAction.aMagNode, "");
            //    bar->addSeparator();
            createButton(ll, gidrAction.aNapr2, "");
            createButton(ll, gidrAction.aVyd4, "");
            createButton(ll, gidrAction.aNapr2, "");
            createButton(ll, gidrAction.aMark, "");
            createButton(ll, gidrAction.aMarkDel, "");
            createButton(ll, gidrAction.aMarkRev, "");
            //    bar->addSeparator();
            createButton(ll, gidrAction.aMsNode1, "");
            createButton(ll, gidrAction.aMsNode2, "");
            //    bar->addSeparator();
            closeNest2(ll);
        }

        ll = ribbon->addNest("Паспортизация", 2,1, 2);
        {

            createButton(ll, gidrAction.aSaveMAG, "");
            createButton(ll, gidrAction.aSaveMS, "");
            createButton(ll, gidrAction.aSaveRS, "");
            closeNest2(ll);
        }

        ll = ribbon->addNest("Паспортизация", 2,1, 2);
        {

            //    bar->addSeparator();
            createButton(ll, gidrAction.aMsPassport, "");
            //    bar->addSeparator();
            createButton(ll, gidrAction.aAktCreate, "");

            closeNest2(ll);
        }

        ll = ribbon->addNest("Паспортизация", 2,1, 2);
        {

            createButton(ll, gidrAction.aPassportOnOff, "");
            //    bar->addSeparator();
            createButton(ll, gidrAction.aPtsZhurnal, "");

            closeNest2(ll);
        }

        ribbon_move(ribbon, "Паспортизация", gidrAction);
    }
    //    m_barPts = bar;

    //  bar = main_window->addToolBar(tr("Технические условия"));
    //  bar->setObjectName(tr("Технические условия"));

//#endif
    {
        ll = ribbon->addNest("Технические условия", 2,1, 2);
        {
            createButton(ll, gidrAction.aTuNew    , "");
            createButton(ll, gidrAction.aTuLine   , "");
            closeNest2(ll);
        }

        ll = ribbon->addNest("Технические условия", 2,1, 2);
        {
        //    bar->addSeparator();
            createButton(ll, gidrAction.aTuInfo   , "");
            createButton(ll, gidrAction.aTuDel    , "");
        //    bar->addSeparator();
            createButton(ll, gidrAction.aTuFind   , "");
            createButton(ll, gidrAction.aTuSost   , "");
        //    bar->addSeparator();
            createButton(ll, gidrAction.aTuExcel  , "");
            createButton(ll, gidrAction.aTuSvod   , "");
            createButton(ll, gidrAction.aTuZhurnal, "");
            closeNest2(ll);
        }

        ribbon_move(ribbon, "Технические условия", gidrAction);
    }

//#if 0

    //    m_barTu = bar;

    //  bar = main_window->addToolBar(tr("Индикаторы коррозии"));
    //  bar->setObjectName(tr("Индикаторы коррозии"));

    {
        ll = ribbon->addNest("Индикаторы коррозии", 2,1, 2);
        {
            createButton(ll, gidrAction.aKorrozAdd, "");
            createButton(ll, gidrAction.aKorrozDel, "");
            createButton(ll, gidrAction.aFragmentPolyKorroziaAr, "");
            createButton(ll, gidrAction.aKorrozInfo, "");
            closeNest2(ll);
        }

        ll = ribbon->addNest("Индикаторы коррозии", 2,1, 2);
        {
            createButton(ll, gidrAction.aKorroziaUpdate, "");
            createButton(ll, gidrAction.aKorrozDoc3, "");
        //    bar->addSeparator();
            createButton(ll, gidrAction.aKorrozTable, "");
        //    bar->addSeparator();
            createButton(ll, gidrAction.aKorroziaZhurnal1, "");
            createButton(ll, gidrAction.aKorroziaZhurnal2, "");
            createButton(ll, gidrAction.aKorroziaZhurnal3, "");
            createButton(ll, gidrAction.aKorroziaCurrent, "");
        //    bar->addSeparator();
            createButton(ll, gidrAction.aKorrozDoc2, "");
            createButton(ll, gidrAction.aKorrozDoc1, "");
            createButton(ll, gidrAction.aKorroziaOnoff, "");
            createButton(ll, gidrAction.aKorroziaSezon, "");
            closeNest2(ll);
        }

        ribbon_move(ribbon, "Индикаторы коррозии", gidrAction);
    }
    //    m_barKorrozia = bar;


    //  bar = main_window->addToolBar(tr("Электросеть"));
    //  bar->setObjectName(tr("Электросеть"));

    {
        ll = ribbon->addNest("Электросеть", 2,1, 2);
        {
            createButton(ll, gidrAction.aElectroIst, "");
            createButton(ll, gidrAction.aElectroPriem, "");
            createButton(ll, gidrAction.aElectroLine, "");
            createButton(ll, gidrAction.aElectroMufta, "");
            createButton(ll, gidrAction.aElectroOpora, "");
            createButton(ll, gidrAction.aElectroGil, "");
            createButton(ll, gidrAction.aElectroKanal, "");
            createButton(ll, gidrAction.aElectroTable, "");
            createButton(ll, gidrAction.aZhurnalElectro, "");
            closeNest2(ll);
        }

        ll = ribbon->addNest("Электросеть", 2,1, 2);
        {
            createButton(ll, gidrAction.aElectroZhurnal, "");
            createButton(ll, gidrAction.aElectroOnoff, "");
            closeNest2(ll);
        }

        ribbon_move(ribbon, "Электросеть", gidrAction);
    }
    //#endif
#endif

    ribbon->init();

    //    QToolButton *connectWebserviceButton1 = createButton(geo, QObject::tr("Веб-сервер"), QObject::tr("Connect to web service"), ":/icons/add_link_2.svg");
    //    ribbon->addButton("View", "1", connectWebserviceButton1);

    //      ribbon->connect(ribbon, SIGNAL(customContextMenuRequested(QPoint)), ribbon, SLOT(contextMenu(QPoint)));

}


#include "action/mainAction1.h"

void create_ribbon_main(Ribbon *ribbon, MainAction *mainAction)
{
//    ribbon->addTab(QIcon(":/icons/monitor_1.svg"), "Главная");
    ribbon->addTab("Главная");




#if 1
            createButton(ribbon, "Главная", "Файл", mainAction->aFileNew, "", 3);
//            createButton(ribbon, "Гидравлика", "Файл", mainAction->aFileSqlite, "", 3);
            
            if (get_ini()->enable_newbaza) {
                createButton(ribbon, "Главная", "Файл", mainAction->aBdNew, "", 3);
            }
    
            createButton(ribbon, "Главная", "Файл", mainAction->aAppExit, "", 3);


//:images/new/big/connect.svg
//:images/new/big/add-db.svg
//:images/new/big/Exit.svg



#else

    auto *l = ribbon->addNest("Главная", 2,1, 4);

    if (l) {
        //        auto *l2 = ribbon->addNest("Главная", 2,4, 2);
//        auto *l2 = addNest2(l, 2, 2, 2);

        createButton(l2, mainAction->aFileNew, "", "");
        createButton(l2, mainAction->aFileSqlite, "", "");
        createButton(l2, mainAction->aBdNew, "", "");
//        createButton(l2, mainAction->aBdDel, "", "");
        createButton(l2, mainAction->aAppExit, "", "");

//        closeNest2(l2);
    }

    closeNest2(l);
#endif


    ribbon->init();

}
