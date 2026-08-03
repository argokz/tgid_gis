#include <QtGui>
#include <QtSql>
#include <QtWidgets>
//#include <QtPrinter>
#include <cmath>

#include "qobjectdefs.h"
#include "std.h"

#include <gidview/GidWidget.h>

#include "mainwindow.h"

QAction *addToolbarToggle(QWidget *widget, const QString & text, const QString & text2, const QString & icon_name, bool excl);


extern MainWindow *main_window;

QString html_text(const QString & txt)
{
    QString text2 = txt;

    QStringList lines = text2.split('\n');

    if (lines.size() <= 1) {
        text2 = QString("<p>%1</p>").arg(text2);
    }
    else {
        QString title = lines[lines.size() - 1];
        text2 = QString("<p><b>%1</b></p>\n").arg(title);

        for (int i = 0; i < lines.size()-1; i++ ) {
            QString t = QString("<p>%1</p>\n").arg(lines[i]);
            text2 += t;
        }

    }

    return text2;
}

#include <QSvgRenderer>

QIcon generateDisabledIcon(const QString &svgPath) {
    QSvgRenderer svgRenderer(svgPath);

    // Получаем размер из SVG файла
    QSize size = svgRenderer.defaultSize();
    if (!size.isValid()) {
        size = QSize(48, 48); // Размер по умолчанию, если в SVG не указан размер
    }

    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent); // Прозрачный фон

    QPainter painter(&pixmap);
    svgRenderer.render(&painter);

    // Преобразование в оттенки серого
    QImage grayImage = pixmap.toImage().convertToFormat(QImage::Format_Grayscale8);
    return QIcon(QPixmap::fromImage(grayImage));
}

void setMyIcon(QAction *a, const QString & icon_name)
{
    if (icon_name != "") {
        QIcon icon(icon_name);
//        icon.addFile(icon_name, QSize(), QIcon::Normal);

#if 0

        if (icon_name.indexOf(".svg") != -1) {
            icon.addFile(":/images/new/pjezo/pjezo_v.svg", QSize(), QIcon::Disabled, QIcon::On);
            icon.addFile(":/images/new/pjezo/pjezo_v.svg", QSize(), QIcon::Disabled, QIcon::Off);
//            icon.addFile(":/images/new/pjezo/pjezo_v.svg", QSize(), QIcon::Normal, QIcon::On);
//            icon.addPixmap(generateDisabledIcon(icon_name).pixmap(31, 31), QIcon::Disabled);
        }
#endif
        a->setIcon(icon);
    }
}

void setMyIcon(QToolButton *b, const QString & icon_name)
{
    if (icon_name != "") {
        QIcon icon(icon_name);
//        icon.addFile(icon_name, QSize(), QIcon::Normal);
//        QIcon icon(":/images/new/pjezo/pjezo_v.svg");

#if 0

        if (icon_name.indexOf(".svg") != -1 && false) {
            icon = generateDisabledIcon(icon_name);

            icon.addFile(":/images/new/pjezo/pjezo_v.svg"              , QSize(), QIcon::Normal, QIcon::On);
            icon.addFile(":/images/new/1/rectangle 1.svg"              , QSize(), QIcon::Normal, QIcon::Off);
            icon.addFile(":/images/new/1/turn-left-arrows 1.svg"       , QSize(), QIcon::Disabled, QIcon::On);
            icon.addFile(":/images/new/3/document-info 1.svg"          , QSize(), QIcon::Disabled, QIcon::Off);
            icon.addFile(":/images/new/4/sum 1.svg"                    , QSize(), QIcon::Active, QIcon::On);
            icon.addFile(":/images/new/4/up-down-arrow 1.svg"          , QSize(), QIcon::Active, QIcon::Off);
            icon.addFile(":/images/new/4/binoculars-find 1.svg"        , QSize(), QIcon::Selected, QIcon::On);
            icon.addFile(":/images/new/7/select-all-svgrepo-com 1.svg" , QSize(), QIcon::Selected, QIcon::Off);

/*
            ":/images/new/pjezo/pjezo_v.svg"
            ":/images/new/1/rectangle 1.svg"
            ":/images/new/1/turn-left-arrows 1.svg"
            ":/images/new/3/document-info 1.svg"
            ":/images/new/4/sum 1.svg"
            ":/images/new/4/up-down-arrow 1.svg"
            ":/images/new/4/binoculars-find 1.svg"
            ":/images/new/7/select-all-svgrepo-com 1.svg"
            ":/images/new/7/copy.svg"
*/

//            icon.addPixmap(generateDisabledIcon(icon_name).pixmap(31, 31), QIcon::Disabled);
        }
#endif
        b->setIcon(icon);
    }
}



QAction *addTrigger(QWidget *widget, const QString & text, const QString & text2_0, const QString & icon_name, const char *slot_name = nullptr)
{
    QString text2 = text2_0;

    QAction *a = new QAction(text);

    if (text2 == "") text2 = text;

    setMyIcon(a, icon_name);


//    text2.split('\n');

    text2 = html_text(text2);

    a->setToolTip(text2);
    if (slot_name) {
        widget->connect(a, SIGNAL(triggered()), widget, slot_name);
    }

    return a;
}


QString ispr_text(const QString &text);


QAction *addToggleGroup(QActionGroup *group, QWidget *widget, const QString & text, const QString & text2_0, const QString & icon_name)
{
    QString text2 = text2_0;

//    QString text_ispr = ispr_text(text);

    QAction *a = new QAction(text);
    if (icon_name != "") {
//        a->setIcon(QIcon(icon_name));
        setMyIcon(a, icon_name);
    }
    if (text2 == "") text2 = text;

    text2 = html_text(text2);

    a->setToolTip(text2);
    a->setCheckable(true);
    a->setChecked(false);

    if (group) {
        group->addAction(a);
    }

    return a;
}

QAction *addToggle(QWidget *widget, const QString & text, const QString & text2_0, const QString & icon_name, const char *slot_name, bool v)
{
    QString text2 = text2_0;

    QAction *a = new QAction(text);
    if (icon_name != "") {
//        a->setIcon(QIcon(icon_name));
        setMyIcon(a, icon_name);
    }
    if (text2 == "") text2 = text;
    text2 = html_text(text2);
    a->setToolTip(text2);
    if (slot_name) {
        widget->connect(a, SIGNAL(toggled(bool)), widget, slot_name);
    }
    a->setCheckable(true);
    a->setChecked(v);

    return a;
}



void GidWidget::onToggleValue(bool on)
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        bool *v = action->data().value<bool*>();
        if (v) {
            *v = on;
            repaint();
        }
    }
}

std::set<QAction*> set_action_value;

//void GidWidget::initActionValue()
void initActionValue()
{
    for (auto &a : set_action_value) {
        bool *v = a->data().value<bool*>();
        if (v) {
            a->setChecked(*v);
        }
    }
}

QAction *addToggleValue(QWidget *widget, const QString & text, const QString & text2_0, const QString & icon_name, bool * value)
{
    QAction *a = new QAction(text);
    if (icon_name != "") {
//        a->setIcon(QIcon(icon_name));
        setMyIcon(a, icon_name);
    }

    QString text2 = text2_0;

    if (text2 == "") text2 = text;

    text2 = html_text(text2);

    a->setToolTip(text2);

    QVariant v = QVariant::fromValue(value);
    a->setData(v);

    set_action_value.insert(a);

    widget->connect(a, SIGNAL(toggled(bool)), widget, SLOT(onToggleValue(bool)));
    a->setCheckable(true);
    a->setChecked(false);

    return a;
}

QToolButton *getButton(QAction *a);


static QMap<QAction *, QList<QAction *>> actionMenus;
static std::map<QAction *, int> conditions;
static std::map<int, std::function<bool()>> cond_functions;
static int cond_id = 0;



QList<QAction*> getMenuActions(QAction *mainAction)
{
    if (actionMenus.contains(mainAction))
        return actionMenus.value(mainAction);
    return {};
}


void GidWidget::updateUI()
{
//    qDebug() << "=======================";
    std::map<int, bool> conditionResults;

    for (auto & [action, condition_id] : conditions) {

        auto it = conditionResults.find(condition_id);
        if (it == conditionResults.end()) {
            auto itf = cond_functions.find(condition_id);
            if (itf != cond_functions.end()) {
                conditionResults[condition_id] = itf->second();
            }
        }

        bool newState = conditionResults[condition_id];

        if (action->isEnabled() != newState) {
            action->setEnabled(newState);

            QToolButton *button = getButton(action);
            if (button) {
                button->setEnabled(newState);
            }

            QIcon icon = action->icon();
            if (!icon.isNull()) {
                qDebug() << action->text() << " = " << newState;
            }
        }
    }
}

int addCondition(std::function<bool()> condition)
{
    cond_id += 1;

    cond_functions[cond_id] = condition;
    return cond_id;
}


//void GidWidget::addActionWithCondition(QAction *action, std::function<bool()> condition)
void addActionWithCondition(QAction *action, int id)
{
    conditions[action] = id;
}

GidWidget *getView();


bool isMarkGid()
{
//    qDebug() << "isMarkGid()";
    GidWidget *view = getView();

    if (view) {
        return view->isMark();
    }

    return false;
}

void setView(GidWidget *view);

void GidWidget::createActions()
{
    setView(this);


gidrAction.aFileClose = addTrigger(this, tr("Закрыть"), "Закрыть текущую базы данных SQL\nЗакрыть", ":/images2/FileClose.png", SLOT(onFileClose()));


gidrAction.aSqlSave = addTrigger(this, tr("Создание копии Базы данных"), "", ":/images2/SqlSave.png", SLOT(onSqlSave()));
gidrAction.aSqlSave->setToolTip(html_text("Создание резервной копии теущей Базы данных"));

gidrAction.aSqlCopyFragment = addTrigger(this, tr("Вставить Фрагмент схемы"), "", ":/images2/SqlCopyFragment.png", SLOT(onSqlCopyFragment()));
gidrAction.aSqlCopyFragment->setToolTip(html_text("Вставить выбранный Фрагмент расчетной схемы из внешней Базы данных в текущую Базу данных"));

gidrAction.aSqlDelFragment = addTrigger(this, tr("Удалить Фрагмент схемы"), "", ":/images2/SqlDelFragment.png", SLOT(onSqlDelFragment()));
gidrAction.aSqlDelFragment->setToolTip(html_text("Удалить Фрагмент расчетной схемы из текущей Базы данных"));

//gidrAction.aAddFragment = addTrigger(this, tr("Добавить Фрагмент схемы"), "", ":/images2/AddFragment.png", SLOT(onAddFragment()));
//gidrAction.aAddFragment->setToolTip(html_text("Добавить Фрагмент схемы"));

gidrAction.aAddFragment = addTrigger(this, tr("Объединить Фрагменты схемы"), "", ":/images2/AddFragment.png", SLOT(onAddFragment()));
gidrAction.aAddFragment->setToolTip(html_text("Объединить Фрагменты схемы"));


gidrAction.aExportFragment = addTrigger(this, tr("Экспорт фрагмента"), "", ":/images2/ExportFragment.png", SLOT(onExportFragment()));
gidrAction.aExportFragment->setToolTip(html_text("Экспорт фрагмента\nЭкспорт фрагмента"));

gidrAction.aExportFragmentVyd = addTrigger(this, tr("Экспорт выделенного фрагмента"), "", ":/images2/ExportFragmentVyd.png", SLOT(onExportFragmentVyd()));
gidrAction.aExportFragmentVyd->setToolTip(html_text("Экспорт выделенного фрагмента"));

gidrAction.aExportFragmentsMulty = addTrigger(this, tr("Экспорт всех фрагментов"), "", ":/images2/ExportFragmentsMulty.png", SLOT(onExportFragmentsMulty()));
gidrAction.aExportFragmentsMulty->setToolTip(html_text("Экспорт всех фрагментов"));

gidrAction.aImportFragment = addTrigger(this, tr("Импорт фрагмента"), "", ":/images2/ImportFragment.png", SLOT(onImportFragment()));
gidrAction.aImportFragment->setToolTip(html_text("Импорт фрагмента\nИмпорт фрагмента"));

gidrAction.aImportFragmentMdb = addTrigger(this, tr("Конвертор ТГИД-05/ ТГИД-07"), "", ":/images2/ImportFragmentMdb.png", SLOT(onImportFragmentMdb()));
gidrAction.aImportFragmentMdb->setToolTip(html_text("Конвертация данных из формата ТГИД-05/ТГИД-07 в формат ТГИД-07sql"));

gidrAction.aMainCxema = addTrigger(this, tr("Активная схема"), "", ":/images2/MainCxema.png", SLOT(onMainCxema()));
gidrAction.aMainCxema->setToolTip(html_text("Назначение фрагмента расчетной схемы в качестве активного слоя для расчета режимов"));

gidrAction.aAddDop = addTrigger(this, tr("Отобразить дополнительную схему"), "", ":/images2/AddDop.png", SLOT(onAddDop()));
gidrAction.aAddDop->setToolTip(html_text("Добавить фрагмент расчетной схемы на общем плане тепловой сети"));

gidrAction.aBmp = addTrigger(this, tr("Файл графической подложки..."), "", ":/images2/Bmp.png", SLOT(onBmp()));
gidrAction.aBmp->setToolTip(html_text("Выбрать файл графической подложки"));

gidrAction.aBmpFind = addTrigger(this, tr("Найти графическую подложку"), "", ":/images2/BmpFind.png", SLOT(onBmpFind()));
gidrAction.aBmpFind->setToolTip(html_text("Найти графическую подложку"));

//gidrAction.aCatalog = addTrigger(this, tr("Каталоги"), "", ":/images2/Catalog.png", SLOT(onCatalog()));
gidrAction.aCatalog = addTrigger(main_window, tr("Каталоги"), "", ":/images2/Catalog.png", SLOT(onCatalog()));
gidrAction.aCatalog->setToolTip(html_text("Настройка каталогов хранения документов\nКаталоги"));

gidrAction.aFilePrint = addTrigger(this, tr("Печать...\tCtrl+P"), "", ":/images/new/1/print-color 1.svg", SLOT(onFilePrint()));
gidrAction.aFilePrint->setToolTip(html_text("Вывод на печать текущего документа\nПечать (Ctrl+P)"));

gidrAction.aFilePrintPreview = addTrigger(this, tr("Предварительный просмотр"), "", ":/images/new/1/preview-color 1.svg", SLOT(onFilePrintPreview()));
gidrAction.aFilePrintPreview->setToolTip(html_text("Предварительный просмотр документа перед выводом на печать\nПредварительный просмотр"));

gidrAction.aPrintFr = addTrigger(this, tr("Фрагмент для печати"), "", ":/images/new/1/Vector.svg", SLOT(onPrintFr()));
gidrAction.aPrintFr->setToolTip(html_text("Выбрать фрагмент для печати\nФрагмент печати"));

//gidrAction.aFragment = addTrigger(this, tr("Выделить фрагмент"), "", ":/images2/Fragment.png", SLOT(onFragment()));
//gidrAction.aFragment->setToolTip(html_text("Выделить фрагмент расчетной схемы для копирования в буфер обмена или печати"));

gidrAction.aBdDel = addTrigger(this, tr("Удалить базу"), "", "", SLOT(onBdDel()));
gidrAction.aBdDel->setToolTip(html_text("Удалить текущую базу данных"));

gidrAction.aFilePrintSetup = addTrigger(this, tr("Параметры страницы..."), "", ":/images2/FilePrintSetup.png", SLOT(onFilePrintSetup()));
gidrAction.aFilePrintSetup->setToolTip(html_text("Выбор принтера и свойств печати\nПараметры страницы"));

gidrAction.aAppExit = addTrigger(this, tr("Выход"), "", ":/images2/AppExit.png", SLOT(onAppExit()));
gidrAction.aAppExit->setToolTip(html_text("Выход из программы\nЗавершить работу"));

gidrAction.aEditFindNode = addTrigger(this, tr("Найти узел...\tCtrl+F"), "Поиск узла расчетной схемы тепловой сети по его имени\nНайти узел", ":/images/gidr/EditFindNode.png", SLOT(onEditFindNode()));

gidrAction.aEditUndo = addTrigger(this, tr("Отменить\tCtrl+Z"), "", ":/images3/EditUndo.png", SLOT(onEditUndo()));
gidrAction.aEditUndo->setToolTip(html_text("Отменить выполненное действие\nОтменить (Ctrl+Z)"));

gidrAction.aEditUndoTU = addTrigger(this, tr("Отменить\tCtrl+Z"), "", ":/images3/EditUndo.png", SLOT(onEditUndoTU()));
gidrAction.aEditUndoTU->setToolTip(html_text("Отменить выполненное действие по работе с ТУ\nОтменить (Ctrl+Z)"));



gidrAction.aResetGid = addTrigger(this, tr("Перезагрузить схему"), "", "", SLOT(onResetGid()));
gidrAction.aResetGid->setToolTip(html_text("Перезагрузить обновления схемы"));

#if USE_GID9
gidrAction.aToGid9 = addTrigger(this, tr("Передать информацию в QGIS"), "", "", SLOT(onToGid9()));
gidrAction.aToGid9->setToolTip(html_text("Передать информацию в QGIS"));

//gidrAction.aSetupGid9 = addTrigger(this, tr("Настройка синхронизации..."), "", ":/images/gidr/SetupGid9.png", SLOT(onSetupGid9()));
//gidrAction.aSetupGid9->setToolTip(html_text("Настройка синхронизации\nНастройка синхронизации"));

#endif

gidrAction.aEditUndoDel = addTrigger(this, tr("Отменить удаления"), "", ":/images2/EditUndoDel.png", SLOT(onEditUndoDel()));
gidrAction.aEditUndoDel->setToolTip(html_text("Отменить удаления"));

gidrAction.aZapVnCx = addTrigger(this, tr("Узлы с внутренней схемой"), "", ":/images/zapros/ZapVnCx.png", SLOT(onZapVnCx()));
gidrAction.aZapVnCx->setToolTip(html_text("Поиск узла расчетной схемы тепловой сети с внутренней схемой\nУзлы с внутренней схемой"));

gidrAction.aZapNullGeo = addTrigger(this, tr("Узлы с нулевой геодезической отметкой"), "", ":/images/zapros/ZapNullGeo.png", SLOT(onZapNullGeo()));
gidrAction.aZapNullGeo->setToolTip(html_text("Поиск узла расчетной схемы тепловой сети с нулевой геодезической отметкой\nУзлы с нулевой геодезической отметкой"));

gidrAction.aOdnolin = addTrigger(this, tr("Однолинейные участки"), "", ":/images2/Odnolin.png", SLOT(onOdnolin()));
gidrAction.aOdnolin->setToolTip(html_text("Выделение цветом однолинейных участков во всех фрагментах тепловой сети отображенных в окне"));

gidrAction.aRaznolin = addTrigger(this, tr("Участки с разными подачей и обраткой"), "", ":/images2/Raznolin.png", SLOT(onRaznolin()));
gidrAction.aRaznolin->setToolTip(html_text("Выделение цветом участков тепловой сети с разными трубопроводами по подаче и обратке"));

gidrAction.aRasprMag = addTrigger(this, tr("Расчетные схемы"), "", ":/images/zapros/RasprMag.png", SLOT(onRasprMag()));
gidrAction.aRasprMag->setToolTip(html_text("Выделить цветом фрагмент Расчетной схемы тепловой сети\nРасчетная схема"));

gidrAction.aRasprMag2 = addTrigger(this, tr("Объекты системы теплоснабжения"), "", ":/images/zapros/RasprMag2.png", SLOT(onRasprMag2()));
gidrAction.aRasprMag2->setToolTip(html_text("Выделить цветом Объект системы теплоснабжения\nОбъект системы теплоснабжения"));

gidrAction.aIstSety = addTrigger(this, tr("Источник"), "", ":/images/zapros/IstSety.png", SLOT(onIstSety()));
gidrAction.aIstSety->setToolTip(html_text("Выделить цветом объекты Источника тепла\nИсточник тепла"));

gidrAction.aIstSetyNew = addTrigger(this, tr("Потоки по источникам"), "", ":/images/zapros/IstSety.png", SLOT(onIstSetyNew()));
gidrAction.aIstSetyNew->setToolTip(html_text("Выделить цветом участков разделения потоков по источникам\nПотоки по источникам"));


gidrAction.aVyd = addTrigger(this, tr("Фрагмент сети"), "", ":/images/zapros/Vyd.png", SLOT(onVyd()));
gidrAction.aVyd->setToolTip(html_text("Выделить изолированный фрагмент сети от заданного узла расчетной схемы тепловой сети\nОт узла"));

gidrAction.aBalans = addTrigger(this, tr("Балансовая принадлежность"), "", ":/images2/Balans.png", SLOT(onBalans()));
gidrAction.aBalans->setToolTip(html_text("Выделить цветом объекты расчетной схемы тепловой сети по балансовой принадлежности"));

gidrAction.aTubing1 = addTrigger(this, tr("Канальная"), "", ":/images2/Tubing1.png", SLOT(onTubing1()));
gidrAction.aTubing1->setToolTip(html_text("Канальная"));

gidrAction.aTubing2 = addTrigger(this, tr("Бесканальная"), "", ":/images2/Tubing2.png", SLOT(onTubing2()));
gidrAction.aTubing2->setToolTip(html_text("Бесканальная"));

gidrAction.aTubing4 = addTrigger(this, tr("Надземная"), "", ":/images2/Tubing4.png", SLOT(onTubing4()));
gidrAction.aTubing4->setToolTip(html_text("Надземная"));

gidrAction.aTubing3 = addTrigger(this, tr("Подвальная"), "", ":/images2/Tubing3.png", SLOT(onTubing3()));
gidrAction.aTubing3->setToolTip(html_text("Подвальная"));

gidrAction.aTubing5 = addTrigger(this, tr("Обвязка узлов и насосных станций"), "", ":/images2/Tubing5.png", SLOT(onTubing5()));
gidrAction.aTubing5->setToolTip(html_text("Обвязка узлов и насосных станций"));

gidrAction.aFindKti = addTrigger(this, tr("По коэффициенту тепловых испытаний"), "", ":/images2/FindKti.png", SLOT(onFindKti()));
gidrAction.aFindKti->setToolTip(html_text("По коэффициенту тепловых испытаний"));

gidrAction.aSetOtv = addTrigger(this, tr("Установить ФИО техников"), "", ":/images2/SetOtv.png", SLOT(onSetOtv()));
gidrAction.aSetOtv->setToolTip(html_text("Установить ФИО техников, для всех потребителей области, ответственных за их техническое сопровождение"));

gidrAction.aSetTr = addTrigger(this, tr("Установить Код расчетных температур"), "", ":/images2/SetTr.png", SLOT(onSetTr()));
gidrAction.aSetTr->setToolTip(html_text("Установить Код расчетных температур для всех потребителей области"));

gidrAction.aSetUr = addTrigger(this, tr("Установить Код удельных расходов"), "", ":/images2/SetUr.png", SLOT(onSetUr()));
gidrAction.aSetUr->setToolTip(html_text("Установить Код удельных расходов для всех потребителей области"));

gidrAction.aSetKvPt = addTrigger(this, tr("Установить Коэффициенты вариации по потребителям"), "", ":/images2/SetKvPt.png", SLOT(onSetKvPt()));
gidrAction.aSetKvPt->setToolTip(html_text("Установить Коэффициенты вариации по потребителям"));

gidrAction.aSetUf = addTrigger(this, tr("Установить Коэффициенты смешения элеватора"), "", ":/images2/SetUf.png", SLOT(onSetUf()));
gidrAction.aSetUf->setToolTip(html_text("Установить коэффициенты смешения элеваторов по потребителям"));

gidrAction.aSetTp = addTrigger(this, tr("Установить Тепловые пункты по потребителям"), "", ":/images2/SetTp.png", SLOT(onSetTp()));
gidrAction.aSetTp->setToolTip(html_text("Установить Тепловые пункты по потребителям"));

gidrAction.aSetUdobVent = addTrigger(this, tr("Установить Удельный объем системы вентиляции"), "", ":/images2/SetUdobVent.png", SLOT(onSetUdobVent()));
gidrAction.aSetUdobVent->setToolTip(html_text("Установить удельный объем системы вентиляции для всех потребителей области"));

gidrAction.aSetUdobOt = addTrigger(this, tr("Установить Удельный объем системы отопления"), "", ":/images2/SetUdobOt.png", SLOT(onSetUdobOt()));
gidrAction.aSetUdobOt->setToolTip(html_text("Установить удельный объем системы отопления для всех потребителей области"));

gidrAction.aSetOpenKoef = addTrigger(this, tr("Коэф. часовой неравномерности"), "", ":/images2/SetOpenKoef.png", SLOT(onSetOpenKoef()));
gidrAction.aSetOpenKoef->setToolTip(html_text("Коэф. часовой неравномерности"));

gidrAction.aSetOpenRez = addTrigger(this, tr("Расчетные тепловые потери в рециркуляц. контуре ГВС"), "", ":/images2/SetOpenRez.png", SLOT(onSetOpenRez()));
gidrAction.aSetOpenRez->setToolTip(html_text("Расчетные тепловые потери в рециркуляц. контуре ГВС"));

gidrAction.aSetOpenRezT = addTrigger(this, tr("Температура в рециркуляционном трубопроводе ГВС"), "", ":/images2/SetOpenRezT.png", SLOT(onSetOpenRezT()));
gidrAction.aSetOpenRezT->setToolTip(html_text("Температура в рециркуляционном трубопроводе ГВС"));

gidrAction.aSetOpenGvsT = addTrigger(this, tr("Расчетная температура горячей воды"), "", ":/images2/SetOpenGvsT.png", SLOT(onSetOpenGvsT()));
gidrAction.aSetOpenGvsT->setToolTip(html_text("Расчетная температура горячей воды"));

gidrAction.aSetDiams = addTrigger(this, tr("Установить Диаметр"), "", ":/images2/SetDiams.png", SLOT(onSetDiams()));
gidrAction.aSetDiams->setToolTip(html_text("Установить выбранное значение диаметра для всех выделенных участков теплопроводов"));

gidrAction.aSetLosesShare = addTrigger(this, tr("Установить Долю местных потерь"), "", ":/images2/SetLosesShare.png", SLOT(onSetLosesShare()));
gidrAction.aSetLosesShare->setToolTip(html_text("Установить долю местных потерь для всех выделенных участков теплопроводов"));

gidrAction.aSetKolChas = addTrigger(this, tr("Установить Количество часов работы"), "", ":/images2/SetKolChas.png", SLOT(onSetKolChas()));
gidrAction.aSetKolChas->setToolTip(html_text("Установить количество часов работы для всех участков области"));

gidrAction.aSetKvUt = addTrigger(this, tr("Установить Коэффициенты вариации по участкам"), "", ":/images2/SetKvUt.png", SLOT(onSetKvUt()));
gidrAction.aSetKvUt->setToolTip(html_text("Установить Коэффициенты вариации по участкам"));

gidrAction.aSetKti = addTrigger(this, tr("Установить Коэффициенты тепловых испытаний"), "", ":/images2/SetKti.png", SLOT(onSetKti()));
gidrAction.aSetKti->setToolTip(html_text("Установить коэффициенты тепловых испытаний для всех участков области"));

gidrAction.aSetOrg = addTrigger(this, tr("Установить Организации"), "", ":/images2/SetOrg.png", SLOT(onSetOrg()));
gidrAction.aSetOrg->setToolTip(html_text("Установить организации владельцев участков тепловой сети"));

gidrAction.aSetPipeRemontType = addTrigger(this, tr("Установить Признак ремонта"), "", ":/images2/SetPipeRemontType.png", SLOT(onSetPipeRemontType()));
gidrAction.aSetPipeRemontType->setToolTip(html_text("Установить Признак ремонта"));

gidrAction.aSetTubingType = addTrigger(this, tr("Установить Тип прокладки"), "", ":/images2/SetTubingType.png", SLOT(onSetTubingType()));
gidrAction.aSetTubingType->setToolTip(html_text("Установить тип прокладки для всех выделенных участков теплопроводов"));

gidrAction.aSetSher = addTrigger(this, tr("Установить Эквивалентную шероховатость"), "", ":/images2/SetSher.png", SLOT(onSetSher()));
gidrAction.aSetSher->setToolTip(html_text("Установить Эквивалентную шероховатость для всех участков области"));

gidrAction.aSetDate2 = addTrigger(this, tr("Установить Дату первичного ввода в эксплуатацию"), "", ":/images2/SetDate2.png", SLOT(onSetDate2()));
gidrAction.aSetDate2->setToolTip(html_text("Установить дату первичного ввода в эксплуатацию"));

gidrAction.aSetDate1 = addTrigger(this, tr("Установить Дату последней перекладки"), "", ":/images2/SetDate1.png", SLOT(onSetDate1()));
gidrAction.aSetDate1->setToolTip(html_text("Установить дату последней перекладки"));

gidrAction.aSetDate3 = addTrigger(this, tr("Установить Дату планируемого ремонта"), "", ":/images2/SetDate3.png", SLOT(onSetDate3()));
gidrAction.aSetDate3->setToolTip(html_text("Установить дату планируемого ремонта"));

gidrAction.aSetKodRs = addTrigger(this, tr("Установить Код расчетной схемы"), "", ":/images2/SetKodRs.png", SLOT(onSetKodRs()));
gidrAction.aSetKodRs->setToolTip(html_text("Установить Код расчетной схемы для всех узлов области"));

gidrAction.aGeodz = addTrigger(this, tr("Установить Отметки узлов горизонтали"), "", ":/images2/Geodz.png", SLOT(onGeodz()));
gidrAction.aGeodz->setToolTip(html_text("Установить геодезическую отметку для всех узлов области"));

gidrAction.aSetKorrozia = addTrigger(this, tr("Установить Индикаторы коррозии"), "", ":/images2/SetKorrozia.png", SLOT(onSetKorrozia()));
gidrAction.aSetKorrozia->setToolTip(html_text("Установить Индикаторы коррозии"));

gidrAction.aSetPodpOn = addTrigger(this, tr("Показать Надписи"), "", ":/images2/SetPodpOn.png", SLOT(onSetPodpOn()));
gidrAction.aSetPodpOn->setToolTip(html_text("Показать имена узлов и отображаемую информацию для всех объектов области"));

gidrAction.aSetPodpOff = addTrigger(this, tr("Не отображать Надписи"), "", ":/images2/SetPodpOff.png", SLOT(onSetPodpOff()));
gidrAction.aSetPodpOff->setToolTip(html_text("Не отображать имена узлов и отображаемую информаци. для всех объектов области"));

//gidrAction.aFragmentPoly = addTrigger(this, tr("Выделить многоугольный фрагмент"), "", ":/images2/FragmentPoly.png", SLOT(onFragmentPoly()));
//gidrAction.aFragmentPoly->setToolTip(html_text("Выделить многоугольный фрагмент"));

//gidrAction.aMoveVyd = addTrigger(this, tr("Передвинуть фрагмент"), "", ":/images2/MoveVyd.png", SLOT(onMoveVyd()));
//gidrAction.aMoveVyd->setToolTip(html_text("Передвинуть фрагмент"));

//gidrAction.aFragment = addTrigger(this, tr("Выделить фрагмент"), "", ":/images2/Fragment.png", SLOT(onFragment()));
//gidrAction.aFragment->setToolTip(html_text("Выделить фрагмент расчетной схемы для копирования в буфер обмена или печати"));

gidrAction.aEditCopy = addTrigger(this, tr("Копировать\tCtrl+C"), "", ":/images3/EditCopy.png", SLOT(onEditCopy()));
gidrAction.aEditCopy->setToolTip(html_text("Сохранить выделение в буфере обмена\nКопировать"));

gidrAction.aEditCopyPng = addTrigger(this, tr("Копировать в PNG"), "", ":/images2/EditCopyPng.png", SLOT(onEditCopyPng()));
gidrAction.aEditCopyPng->setToolTip(html_text("Копировать в PNG"));

gidrAction.aNavigator = addToggle(this, tr("Навигатор"), "", "", SLOT(onNavigator(bool)), false);
gidrAction.aNavigator->setToolTip(html_text("Показать/скрыть окно Навигатора"));



gidrAction.aLeftBar = addToggle(this, tr("Проводник карты"), "", "", SLOT(onLeftBar(bool)), main_window->isVisibleDocks(Qt::LeftDockWidgetArea));
gidrAction.aLeftBar->setToolTip(html_text("Показать/скрыть окно Проводника объектов карты местности и тепловой сети"));

gidrAction.aMagAlma = addToggle(this, tr("Проводник структуры сети"), "", "", SLOT(onMagAlma(bool)), main_window->isVisibleDocks(Qt::RightDockWidgetArea));
gidrAction.aMagAlma->setToolTip(html_text("Показать/скрыть окно Проводника объектов структуры тепловой сети"));

//gidrAction.aBottomRemont = addToggle(this, tr("Проводник мониторинга"), "", "", SLOT(onBottomRemont(bool)), main_window->isVisibleDocks(Qt::BottomDockWidgetArea));
//gidrAction.aBottomRemont->setToolTip(html_text("Показать/скрыть окно Проводника мониторинга ремонтов, шурфовок, объектов электрический сетей, обработка индикаторов коррозии"));

//gidrAction.aProtocol = addToggle(this, tr("Протокол"), "", "", SLOT(onProtocol(bool)), true);
//gidrAction.aProtocol->setToolTip(html_text("Показать/скрыть окно протокола теплогидравлического расчета режима тепловой сети"));

gidrAction.aBottomRemont = addToggle(this, tr("Протокол"), "", "", SLOT(onBottomRemont(bool)), main_window->isVisibleDocks(Qt::BottomDockWidgetArea));
gidrAction.aBottomRemont->setToolTip(html_text("Показать/скрыть окно протокола теплогидравлического расчета режима тепловой сети"));

gidrAction.aPovorot = addTrigger(this, tr("Повернуть"), "", ":/images2/Povorot.png", SLOT(onPovorot()));
gidrAction.aPovorot->setToolTip(html_text("Повернуть отображение объектов схемы и геобазы на заданный угол"));

gidrAction.aPovorot2 = addTrigger(this, tr("Возврат"), "", ":/images2/Povorot2.png", SLOT(onPovorot2()));
gidrAction.aPovorot2->setToolTip(html_text("Вернуть в исходное положение отображение объектов схемы и геобазы на заданный угол"));

gidrAction.aMasall = addTrigger(this, tr("Отобразить всю схему\tF3"), "", ":/images/new/2/pan 1.svg", SLOT(onMasall()));
gidrAction.aMasall->setToolTip(html_text("Отобразить всю расчетную схему тепловой сети\nОтобразить всю схему"));

gidrAction.aPlus = addTrigger(this, tr("Увеличить\t+"), "", ":/images/new/2/zoom-in-alt 1.svg", SLOT(onPlus()));
gidrAction.aPlus->setToolTip(html_text("Ступенчато увеличить изображение\nУвеличить"));

gidrAction.aMinus = addTrigger(this, tr("Уменьшить\t-"), "", ":/images/new/2/zoom-out-alt 1.svg", SLOT(onMinus()));
gidrAction.aMinus->setToolTip(html_text("Ступенчато уменьшить изображение\nУменьшить"));

gidrAction.aMasshtab = addTrigger(this, tr("Масштаб...\tCtrl+F3"), "", ":/images2/Masshtab.png", SLOT(onMasshtab()));
gidrAction.aMasshtab->setToolTip(html_text("Изменить масштаб отображения схемы"));

gidrAction.aPicture = addTrigger(this, tr("Графическая подложка"), "", ":/images2/Picture.png", SLOT(onPicture()));
gidrAction.aPicture->setCheckable(true);
gidrAction.aPicture->setChecked(m_bIsPicture);
gidrAction.aPicture->setToolTip(html_text("Показать/убрать графическую подложку\nПоказать/убрать графическую подложку"));

gidrAction.aF5 = addTrigger(this, tr("Перерисовать схему\tF5"), "", ":/images/new/2/update 1.svg", SLOT(onF5()));
gidrAction.aF5->setToolTip(html_text("Обновить отображение на экране"));

gidrAction.aCtrlF5 = addTrigger(this, tr("Перерисовать карту\tCtrl+F5"), "", ":/images2/F5.png", SLOT(onCtrlF5()));
gidrAction.aCtrlF5->setToolTip(html_text("Обновить отображение на экране"));

gidrAction.aFindGeo = addTrigger(this, tr("Поиск в геобазе..."), "", ":/images2/FindGeo.png", SLOT(onFindGeo()));
gidrAction.aFindGeo->setToolTip(html_text("Поиск объектов геобазы\nПоиск объектов геобазы"));

gidrAction.aGeoFindNext = addTrigger(this, tr("Продолжение поиска"), "", ":/images2/GeoFindNext.png", SLOT(onGeoFindNext()));
gidrAction.aGeoFindNext->setToolTip(html_text("Продолжить поиск объектов геобазы\nПродолжить поиск объектов геобазы"));

gidrAction.aFindGeoAddr = addTrigger(this, tr("Поиск по адресу"), "", ":/images2/FindGeoAddr.png", SLOT(onFindGeoAddr()));
gidrAction.aFindGeoAddr->setToolTip(html_text("Поиск здания по его адресу в Геобазе"));

gidrAction.aSetLength = addTrigger(this, tr("Установить длины"), "", ":/images2/SetLength.png", SLOT(onSetLength()));
gidrAction.aSetLength->setToolTip(html_text("Установить длины участков теплопроводов по их координатам"));

gidrAction.a1000 = addTrigger(this, tr("Создать файл полей"), "", ":/images2/1000.png", SLOT(on1000()));
gidrAction.a1000->setToolTip(html_text("Создать файл *.txt2 с именами полей объектов из подключенной геобазы"));

gidrAction.a1001 = addTrigger(this, tr("Переименовать поля"), "", ":/images2/1001.png", SLOT(on1001()));
gidrAction.a1001->setToolTip(html_text("Запустить программу переименования имен полей объектов геобазы"));

gidrAction.a1002 = addTrigger(this, tr("Сохранить переименованные поля"), "", ":/images2/1002.png", SLOT(on1002()));
gidrAction.a1002->setToolTip(html_text("Сохранить файл *.txt2 с переименованными полями из папки Загрузки ПК в подключенную геобазу"));

gidrAction.aZapNezak = addTrigger(this, tr("Незаконченные узлы"), "", ":/images/zapros/ZapNezak.png", SLOT(onZapNezak()));
gidrAction.aZapNezak->setToolTip(html_text("Найти незаконченный отконвертированный узел из файла формата Гид99w/Gid2005 \nНезаконченные"));

gidrAction.aSetCoordNull = addTrigger(this, tr("Удалить координаты выделенных узлов"), "", ":/images2/SetCoordNull.png", SLOT(onSetCoordNull()));
gidrAction.aSetCoordNull->setToolTip(html_text("Удалить коордимнаты узлов выделенной области"));

gidrAction.aDoItDr = addTrigger(this, tr("Плановый...\tCtrl+R"), "", ":/images/gidr/DoItDr.png", SLOT(onDoItDr()));
gidrAction.aDoItDr->setToolTip(html_text("Расчет теплогидравлического режима тепловой сети с заданными нагрузками потребителей и расчет дроссельных органов\nПлановый режим (Ctrl+R)"));

gidrAction.aDoIt = addTrigger(this, tr("Фактический...\tCtrl+T"), "", ":/images/gidr/DoIt.png", SLOT(onDoIt()));
gidrAction.aDoIt->setToolTip(html_text("Расчет теплогидравлического режима тепловой сети с установленными дроссельными органами\nФактический режим (Ctrl+T)"));

gidrAction.aDoItListDr = addTrigger(this, tr("Плановый по списку..."), "", ":/images2/DoItListDr.png", SLOT(onDoItListDr()));
gidrAction.aDoItListDr->setToolTip(html_text("Расчет всех выбранных фрагментов теплогидравлического режима с заданными нагрузками потребителей и расчет дроссельных органов\nПлановый режим"));

gidrAction.aDoItList = addTrigger(this, tr("Фактический по списку..."), "", ":/images2/DoItList.png", SLOT(onDoItList()));
gidrAction.aDoItList->setToolTip(html_text("Расчет всех выбранных фрагментов теплогидравлического режима с установленными дроссельными органами\nФактический режим"));

gidrAction.aMagFragment = addTrigger(this, tr("Магистральный фрагмент"), "", ":/images2/MagFragment.png", SLOT(onMagFragment()));
gidrAction.aMagFragment->setToolTip(html_text("Установить магистральный фрагмент в качестве основного фрагментам распределительных сетей"));

gidrAction.aSavePjezo = addTrigger(this, tr("Сохранить направление..."), "", ":/images2/SavePjezo.png", SLOT(onSavePjezo()));
gidrAction.aSavePjezo->setToolTip(html_text("Сохранить направление пьезометра"));

gidrAction.aListPjezo = addTrigger(this, tr("Список направлений..."), "", ":/images2/ListPjezo.png", SLOT(onListPjezo()));
gidrAction.aListPjezo->setToolTip(html_text("Выбрать направление для выделения/построения пьезометрического графика гидравлического режима тепловой сети"));

gidrAction.aClearOut = addTrigger(this, tr("Удалить старые расчеты"), "", ":/images2/ClearOut.png", SLOT(onClearOut()));
gidrAction.aClearOut->setToolTip(html_text("Удалить старые расчеты"));

//gidrAction.aProtocol = addTrigger(this, tr("Протокол"), "", ":/images2/Protocol.png", SLOT(onProtocol()));
//gidrAction.aProtocol->setToolTip(html_text("Показать/скрыть окно протокола теплогидравлического расчета режима тепловой сети"));

gidrAction.aViewDb = addTrigger(this, tr("Таблицы..."), "", ":/images/new/1/table.svg", SLOT(onViewDb()));
gidrAction.aViewDb->setToolTip(html_text("Открыть таблицу с исходной информацией объекта расчетной схемы тепловой сети\nТаблицы"));

gidrAction.aNovisual = addTrigger(this, tr("Системные параметры..."), "", ":/images/gidr/Novisual.png", SLOT(onNovisual()));
gidrAction.aNovisual->setToolTip(html_text("Входная информация об объектах, содержащих общие характеристики расчетной схемы тепловой сети\nСистемные параметры"));

//gidrAction.aMoveTo = addTrigger(this, tr("Таблица объекта"), "", ":/images3/MoveTo.png", SLOT(onMoveTo()));
//gidrAction.aMoveTo->setToolTip(html_text("Открыть таблицу объекта с исходной информацией расчетной схемы тепловой сети\nТаблица объекта"));

gidrAction.aOut2 = addTrigger(this, tr("Отчеты..."), "", ":/images/gidr/Out2.png", SLOT(onOut2()));
gidrAction.aOut2->setToolTip(html_text("Открыть таблицу с результата расчета теплогидравлического режима расчетной схемы тепловой сети\nОтчеты"));

gidrAction.aTableDan = addTrigger(this, tr("Редактор таблиц..."), "", ":/images/new/1/table.svg", SLOT(onTableDan()));
gidrAction.aTableDan->setToolTip(html_text("Открыть редактор входных, выходных документов"));

gidrAction.aExcel2 = addTrigger(this, tr("Таблицы Excel..."), "", ":/images/gidr/Excel2.png", SLOT(onExcel2()));
gidrAction.aExcel2->setToolTip(html_text("Экспорт данных и результатов расчета режима тепловой сети в таблицы MS Excel\nТаблицы Excel/Режимы."));


gidrAction.aSelect6 = addTrigger(this, tr("Выделить"),  "", ":/images/new/6/select-window 1.svg",              SLOT(onSelect6()));   // "Выделить",  
gidrAction.aExport6 = addTrigger(this, tr("Экспорт"),   "", ":/images/new/6/export.svg",                       SLOT(onExport6()));   // "Экспорт",   
gidrAction.aRezhim6 = addTrigger(this, tr("Режим"),     "", ":/images/new/6/switch-double-svgrepo-com 1.svg",  SLOT(onRezhim6()));   // "Режим",     );
gidrAction.aRezhim6->setToolTip(html_text("Выбор режима работы со схемой\nВыбор режима"));


gidrAction.aFile6  = addTrigger(this, tr("Файл"), "", ":/images/new/big/file-svgrepo-com.svg",                        SLOT(onFile6 ()));   // "Файл", 
gidrAction.aSetup6  = addTrigger(this, tr("Настройка"), "", ":/images/new/6/setup.svg",                        SLOT(onSetup6 ()));   // "Настройка", 
gidrAction.aGeobaza6  = addTrigger(this, tr("Геобаза"), "", ":/images/new/big/geobaza.svg",                  SLOT(onGeobaza6 ()));   // "Геобаза", 
gidrAction.aFind6  = addTrigger(this, tr("Поиск по параметрам"), "", ":/images/new/big/query-svgrepo-com.svg",                  SLOT(onFind6 ()));   // "", 

gidrAction.aAnalVyd  = addTrigger(this, tr("Анализ\nвыделение"), "", ":/images/new/big/a/AnalizVyd.svg",               SLOT(onAnalVyd ()));   // "Анализ-выделение", 
gidrAction.aZaprosy  = addTrigger(this, tr("Запросы"), "", ":/images/new/big/a/Queries.svg",               SLOT(onZaprosy ()));   // 
gidrAction.aZaprosy2  = addTrigger(this, tr("Настройка"), "", ":/images/new/big/a/Queries.svg",               SLOT(onZaprosy2 ()));   // 

gidrAction.aRunProgram  = addTrigger(this, tr(""), "", "",        SLOT(onRunProgram ()));   // 


gidrAction.aInf = addTrigger(this, tr("Отображаемая информация..."), "", ":/images/gidr/Inf.png", SLOT(onInf()));
gidrAction.aInf->setToolTip(html_text("Настройка селективного вывода информации на расчетную схему тепловой сети\nОтображаемая информация"));

gidrAction.aPodpPo = addTrigger(this, tr("Отображать схему ПТС"), "", ":/images2/PodpPo.png", SLOT(onPodpPo()));
gidrAction.aPodpPo->setToolTip(html_text("Отображение расчетной схемы тепловой сети в формате производственно-технической службы"));

gidrAction.aPribor = addTrigger(this, tr("Отображать Приборы учета"), "", ":/images2/Pribor.png", SLOT(onPribor()));
gidrAction.aPribor->setToolTip(html_text("Отображать Приборы учета тепловой энергии на расчетной схеме\nОтображать Приборы учета"));

gidrAction.aGeobaza = addToggleValue(this, tr("Показать геобазу"), "Показать/скрыть все объекты Геобазы", "", &m_bIsGeo);
gidrAction.aGidinf = addToggleValue(this, tr("Отображать информацию"), "Отображение информации на расчетной схеме тепловых сетей (отображать/не отображать)\nОтображаемая информация", ":/images/gidr/Gidinf.png", &m_bIsGidrInf);
gidrAction.aPodp = addToggleValue(this, tr("Отображать имена узлов"), "Отображение имен узлов расчетной схемы тепловой сети (отображать/не отображать)\nИмена узлов", ":/images/gidr/Podp.png", &m_bIsPodp);
gidrAction.aPodpRes = addToggleValue(this, tr("Отображать имена узлов с #"), "Отображение имен узлов расчетной схемы тепловой сети, начинающихся с символа # (отображать/не отображать)\nИмена узлов с #", "", &m_bIsPodpRes);
gidrAction.aRezhim = addToggleValue(this, tr("Отображать вспомогательные участки"), "Скрыть/показать вспомогательные участки расчетной схемы\nВспомогательные участки", "", &m_bIsRezhim);
gidrAction.aRamka = addToggleValue(this, tr("Рамка схемы"), "Показать рамку активной схемы", "", &m_bIsRamka);
gidrAction.aIsMas = addToggleValue(this, tr("Сохранять пропорции"), "Включение режима пропорционального изменения объектов расчетной схемы при изменении масштаба ее отображения\nСохранять пропорции", "", &m_bIsMas);
gidrAction.aMapYes = addToggleValue(this, tr("Восстанавливать подключение"), "Восстанавливать подключение к интернет-карте при следующем запуске программы", "", &m_bIsMapYes);
gidrAction.aAstr = addToggleValue(this, tr("Сетка"), "Вывод и скрытие сетки", "", &m_bIsAstr);



gidrAction.aNaprGid = addToggleValue(this, tr("Распределение потоков"), "", ":/images/gidr/NaprGid.png", &m_bIsNaprGid);
gidrAction.aNaprGid->setToolTip(html_text("Показать распределение потоков рассчитанного теплогидравлического режима расчетной схемы тепловой сети\nРаспределение потоков"));


gidrAction.aPodpAll = addToggleValue(this, tr("Отображать имена узлов"), "Отображение имен узлов расчетной схемы тепловой сети (отображать/не отображать)\nИмена узлов", ":/images/gidr/Podp.png", &m_bIsPodpAll);

/*

gidrAction. = addToggleValue(this, tr(""), "", "", &);

gidrAction. = addToggleValue(this, tr(""), "", "", &);
*/

gidrAction.aPopupPodpOn = addTrigger(this, tr("Показать скрытые имена"), "", ":/images2/PopupPodpOn.png", SLOT(onPopupPodpOn()));
gidrAction.aPopupPodpOn->setToolTip(html_text("Отображать надписи по всем объектам, у которых они были скрыты"));

gidrAction.aPopupPodpOff = addTrigger(this, tr("Не показывать скрытые имена"), "", ":/images2/PopupPodpOff.png", SLOT(onPopupPodpOff()));
gidrAction.aPopupPodpOff->setToolTip(html_text("Не отображать надписи по всем объектам, у которых они были скрыты"));

gidrAction.aPodpYesNo = addToggle(this, tr("Отображать/Не отображать надпись"), "", ":/images2/PodpYesNo.png", SLOT(onPodpYesNo(bool)), true);
gidrAction.aPodpYesNo->setToolTip(html_text("Отображать /Не отображать информацию по объекту на расчетной схеме"));

//gidrAction.aNaprGid = addTrigger(this, tr("Распределение потоков"), "", ":/images/gidr/NaprGid.png", SLOT(onNaprGid()));
//gidrAction.aNaprGid->setToolTip(html_text("Показать распределение потоков рассчитанного теплогидравлического режима расчетной схемы тепловой сети\nРаспределение потоков"));

gidrAction.aF4 = addTrigger(this, tr("Измерить расстояние\tF4"), "", ":/images/gidr/F4.png", SLOT(onF4()));
gidrAction.aF4->setToolTip(html_text("Измерить выделенное расстояние\nИзмерить расстояние"));

gidrAction.aAnal = addTrigger(this, tr("Анализ..."), "", ":/images3/Anal.png", SLOT(onAnal()));
gidrAction.aAnal->setToolTip(html_text("Анализ параметров рассчитанного режима расчетной схемы тепловой сети по установленным критериям\nАнализ"));

gidrAction.aAdmissibilityAnalysis = addTrigger(this, tr("Анализ..."), "", ":/images3/Anal.png", SLOT(onAdmissibilityAnalysis()));
gidrAction.aAdmissibilityAnalysis->setToolTip(html_text("Анализ параметров рассчитанного режима расчетной схемы тепловой сети по установленным критериям\nАнализ"));



gidrAction.aColorLine = addTrigger(this, tr("Участки"), "", ":/images2/ColorLine.png", SLOT(onColorLine()));
gidrAction.aColorLine->setToolTip(html_text("Выделение цветом участков расчетной схемы тепловой сети по заданным пределам параметров"));

gidrAction.aColorLine1 = addTrigger(this, tr("Потребители"), "", ":/images2/ColorLine1.png", SLOT(onColorLine1()));
gidrAction.aColorLine1->setToolTip(html_text("Выделение цветом потребителей расчетной схемы тепловой сети по заданным пределам параметров"));

gidrAction.aZap1 = addTrigger(this, tr("Объем сети"), "", ":/images/zapros/Zap1.png", SLOT(onZap1()));
gidrAction.aZap1->setToolTip(html_text("Объем сети\nОбъем сети"));

gidrAction.aZap2 = addTrigger(this, tr("Длина теплопроводов"), "", ":/images/zapros/Zap2.png", SLOT(onZap2()));
gidrAction.aZap2->setToolTip(html_text("Длина теплопроводов\nДлина теплопроводов"));

gidrAction.aZap7 = addTrigger(this, tr("Длина теплопроводов по диаметрам"), "", ":/images/zapros/Zap7.png", SLOT(onZap7()));
gidrAction.aZap7->setToolTip(html_text("Длина теплопроводов по диаметрам\nДлина теплопроводов по диаметрам"));

gidrAction.aZap71 = addTrigger(this, tr("Длина теплопроводов по диаметрам и способам прокладки"), "", ":/images/zapros/Zap71.png", SLOT(onZap71()));
gidrAction.aZap71->setToolTip(html_text("Длина теплопроводов по диаметрам и способам прокладки\nДлина теплопроводов по диаметрам и способам прокладки"));

gidrAction.aTimePr = addTrigger(this, tr("Время прохождения"), "", ":/images/zapros/TimePr.png", SLOT(onTimePr()));
gidrAction.aTimePr->setToolTip(html_text("Время прохождения потока теплоносителя по выделенному направлению\nВремя прохождения"));

gidrAction.aPoteri = addTrigger(this, tr("Тепловые потери"), "", ":/images/zapros/Poteri.png", SLOT(onPoteri()));
gidrAction.aPoteri->setToolTip(html_text("Тепловые потери через теплоизоляционные конструкции теплопроводов\nТепловые потери"));

gidrAction.aZap3 = addTrigger(this, tr("Теплопотребление полученное"), "", ":/images/zapros/Zap3.png", SLOT(onZap3()));
gidrAction.aZap3->setToolTip(html_text("Теплопотребление общее\nТеплопотребление общее"));

gidrAction.aZap4 = addTrigger(this, tr("Теплопотребление в закрытых системах полученное"), "", ":/images/zapros/Zap4.png", SLOT(onZap4()));
gidrAction.aZap4->setToolTip(html_text("Теплопотребление в закрытых системах\nТеплопотребление в закрытых системах"));

gidrAction.aZap5 = addTrigger(this, tr("Теплопотребление в открытых системах полученное"), "", ":/images/zapros/Zap5.png", SLOT(onZap5()));
gidrAction.aZap5->setToolTip(html_text("Теплопотребление в открытых системах\nТеплопотребление в открытых системах"));

gidrAction.aZap6 = addTrigger(this, tr("Закрытые потребители"), "", ":/images/zapros/Zap6.png", SLOT(onZap6()));
gidrAction.aZap6->setToolTip(html_text("Физически отключенные потребители от тепловой сети во всех отображаемых фрагментах расчетных схем\nФизически отключенные потребители от тепловой сети во всех отображаемых фрагментах расчетных схем"));

gidrAction.aPotNagr0 = addTrigger(this, tr("С нулевой нагрузкой"), "", ":/images2/PotNagr0.png", SLOT(onPotNagr0()));
gidrAction.aPotNagr0->setToolTip(html_text("Потребители у которых не заданы расчетные нагрузки на систему теплопотребителей во всех отображаемых фрагментах расчетных схем"));

gidrAction.aPotrOtkl = addTrigger(this, tr("Отключенные потребители"), "", ":/images2/PotrOtkl.png", SLOT(onPotrOtkl()));
gidrAction.aPotrOtkl->setToolTip(html_text("Потребители, в которые не поступает теплоноситель от тепловой сети во всех отображаемых фрагментах расчетных схем\nПотребители, в которые не поступает теплоноситель от тепловой сети во всех отображаемых фрагментах расчетных схем"));

gidrAction.aUtZakr = addTrigger(this, tr("Закрытые участки"), "", ":/images2/UtZakr.png", SLOT(onUtZakr()));
gidrAction.aUtZakr->setToolTip(html_text("Закрытые участки"));

gidrAction.aUtZakrAll = addTrigger(this, tr("Отключенные участки"), "", ":/images2/UtZakrAll.png", SLOT(onUtZakrAll()));
gidrAction.aUtZakrAll->setToolTip(html_text("Отключенные участки"));

gidrAction.aZapOtr = addTrigger(this, tr("Отрицательные перепады"), "", ":/images/zapros/ZapOtr.png", SLOT(onZapOtr()));
gidrAction.aZapOtr->setToolTip(html_text("Выделить цветом Потребителей тепла с отрицательными перепадами напора на входе теплового пункта\nОтрицательные перепады"));

gidrAction.aPtTempMin = addTrigger(this, tr("Низкие температуры"), "", ":/images2/PtTempMin.png", SLOT(onPtTempMin()));
gidrAction.aPtTempMin->setToolTip(html_text("Выделить цветом Потребителей тепла с низкой температурой теплоносителя на входе теплового пункта\nНизкие температуры"));

gidrAction.aZavozd = addTrigger(this, tr("Завоздушивание"), "", ":/images2/Zavozd.png", SLOT(onZavozd()));
gidrAction.aZavozd->setToolTip(html_text("Значение напора в подающем трубопроводе ниже высоты здания у реальных потребителей"));

gidrAction.aIsprLast = addTrigger(this, tr("Изменённые объекты"), "", ":/images2/IsprLast.png", SLOT(onIsprLast()));
gidrAction.aIsprLast->setToolTip(html_text("Выбрать объекты со свойствами изменёнными после указанной даты"));

gidrAction.aPjezo = addTrigger(this, tr("Пьезометр"), "", ":/images/gidr/Pjezo.png", SLOT(onPjezo()));
gidrAction.aPjezo->setToolTip(html_text("Выполнить визуализацию, анализ и печать пьезометрического графика по заданному направлению на расчетной схеме тепловой сети\nПьезометр"));

gidrAction.aZona = addTrigger(this, tr("Гидростатические зоны"), "", ":/images2/Zona.png", SLOT(onZona()));
gidrAction.aZona->setToolTip(html_text("Гидростатические зоны теплоснабжающей системы"));

gidrAction.aNagrZd = addTrigger(this, tr("Физические лица"), "", ":/images2/NagrZd.png", SLOT(onNagrZd()));
gidrAction.aNagrZd->setToolTip(html_text("Заключенные договора по тепловым нагрузкам (по видам потребления), физические лица"));

gidrAction.aNagrOrg = addTrigger(this, tr("Юридические лица"), "", ":/images2/NagrOrg.png", SLOT(onNagrOrg()));
gidrAction.aNagrOrg->setToolTip(html_text("Заключенные договора по тепловым нагрузкам (по видам потребления), юридические лица"));

gidrAction.aTuTable = addTrigger(this, tr("Технические условия"), "", ":/images/tu/TuTable.png", SLOT(onTuTable()));
gidrAction.aTuTable->setToolTip(html_text("Созданные договора Технических условий на присоединение"));

gidrAction.aTuYear = addTrigger(this, tr("ТУ по годам"), "", ":/images/tu/TuYear.png", SLOT(onTuYear()));
gidrAction.aTuYear->setToolTip(html_text("Отчеты по выданным договорам ТУ по годам"));

gidrAction.aTuSost = addTrigger(this, tr("ТУ состояние"), "", ":/images/tu/TuSost.png", SLOT(onTuSost()));
gidrAction.aTuSost->setToolTip(html_text("Отчеты по состоянию ТУ\nОтчеты по состоянию ТУ"));

gidrAction.aTuExcel = addTrigger(this, tr("Итоговый тепловой баланс"), "", ":/images/tu/TuExcel.png", SLOT(onTuExcel()));
gidrAction.aTuExcel->setToolTip(html_text("Формирование итогового отчета по тепловому балансу в Excel\nИтоговый баланс ТУ"));

gidrAction.aTuSvod = addTrigger(this, tr("Итоговые по годам"), "", ":/images/tu/TuSvod.png", SLOT(onTuSvod()));
gidrAction.aTuSvod->setToolTip(html_text("Формирование итогового отчета по годам в Excel\nОтчет годовой ТУ"));

gidrAction.aTuZhurnal = addTrigger(this, tr("Журнал регистрации ТУ"), "", ":/images/tu/TuZhurnal.png", SLOT(onTuZhurnal()));
gidrAction.aTuZhurnal->setToolTip(html_text("Журнал регистрации ТУ в Excel\nЖурнал регистрации ТУ"));

gidrAction.aFindTuIst = addTrigger(this, tr("ТУ по источнику"), "", ":/images2/FindTuIst.png", SLOT(onFindTuIst()));
gidrAction.aFindTuIst->setToolTip(html_text("Выделение на карте ТУ по выбранному источнику"));

gidrAction.aTuFind = addTrigger(this, tr("ТУ, Договора"), "", ":/images/tu/TuFind.png", SLOT(onTuFind()));
gidrAction.aTuFind->setToolTip(html_text("Поиск ТУ\nПоиск ТУ"));

gidrAction.aPrisNagrEdit = addTrigger(this, tr("Присоединенная нагрузка по источникам"), "", ":/images2/PrisNagrEdit.png", SLOT(onPrisNagrEdit()));
gidrAction.aPrisNagrEdit->setToolTip(html_text("Ввод значений присоединенной нагрузки по источникам по годам"));

gidrAction.aQGvs = addTrigger(this, tr("Коэффициент перевода k=Qгвmax/Qгвср"), "", ":/images2/QGvs.png", SLOT(onQGvs()));
gidrAction.aQGvs->setToolTip(html_text("Коэффициент перевода k=Qгвmax/Qгвср"));

gidrAction.aNagrZdNeiz = addTrigger(this, tr("Физические лица"), "", ":/images2/NagrZdNeiz.png", SLOT(onNagrZdNeiz()));
gidrAction.aNagrZdNeiz->setToolTip(html_text("Формирование отчета, ненайденных адресов по договорам с физическими лицами, на электронной карте"));

gidrAction.aNagrOrgNeiz = addTrigger(this, tr("Юридические лица"), "", ":/images2/NagrOrgNeiz.png", SLOT(onNagrOrgNeiz()));
gidrAction.aNagrOrgNeiz->setToolTip(html_text("Формирование отчета ненайденных адресов по договорам с юридическими лицами, на электронной карте"));

gidrAction.aTuTableNeiz = addTrigger(this, tr("Ненайденные ТУ на карте"), "", ":/images/tu/TuTableNeiz.png", SLOT(onTuTableNeiz()));
gidrAction.aTuTableNeiz->setToolTip(html_text("Формирование отчета по ненайденным Техническим условиям"));

gidrAction.aTuIst = addTrigger(this, tr("Отчет по источникам"), "", ":/images/tu/TuIst.png", SLOT(onTuIst()));
gidrAction.aTuIst->setToolTip(html_text("Отчет по источникам"));

gidrAction.aPtsAdd = addTrigger(this, tr("Установить объект"), "", ":/images/pts/PtsAdd.png", SLOT(onPtsAdd()));
gidrAction.aPtsAdd->setToolTip(html_text("Установить объект ПТС\nУстановить объект ПТС"));

gidrAction.aPtsTable = addTrigger(this, tr("Таблица"), "", ":/images/pts/PtsTable.png", SLOT(onPtsTable()));
gidrAction.aPtsTable->setToolTip(html_text("Открыть таблицу по объекту ПТС\nОткрыть таблицу по объекту ПТС"));

gidrAction.aProtPass = addTrigger(this, tr("Протяженность"), "", ":/images2/ProtPass.png", SLOT(onProtPass()));
gidrAction.aProtPass->setToolTip(html_text("Общий паспорт протяженности"));

gidrAction.aProtNew = addTrigger(this, tr("Протяженность выделенных трубопроводов"), "", ":/images2/ProtNew.png", SLOT(onProtNew()));
gidrAction.aProtNew->setToolTip(html_text("Протяженность трубопроводов"));

gidrAction.aProtMag = addTrigger(this, tr("Протяженность магистралей"), "", ":/images2/ProtMag.png", SLOT(onProtMag()));
gidrAction.aProtMag->setToolTip(html_text("Протяженность выделенной/всех учатков магистралей"));

gidrAction.aProtRs = addTrigger(this, tr("Протяженность распредсетей"), "", ":/images2/ProtRs.png", SLOT(onProtRs()));
gidrAction.aProtRs->setToolTip(html_text("Протяженность выделенной/всех распределительных сетей"));

gidrAction.aProtKot = addTrigger(this, tr("Протяженность котельных"), "", ":/images2/ProtKot.png", SLOT(onProtKot()));
gidrAction.aProtKot->setToolTip(html_text("Протяженность выделенной/всех котельных сетей"));

gidrAction.aPaspNew = addTrigger(this, tr("Паспорт сети"), "", ":/images2/PaspNew.png", SLOT(onPaspNew()));
gidrAction.aPaspNew->setToolTip(html_text("Паспорт сети"));

//---- Ремонты

gidrAction.aRemontPovrDefAdd = addTrigger(this, tr("Установить нарушение"), "", ":/images/defect/RemontPovrDefAdd.png", SLOT(onRemontPovrDefAdd()));
gidrAction.aRemontPovrDefAdd->setToolTip(html_text("Установить нарушение на тепловой сети\nУстановить нарушение"));

gidrAction.aDefectZhurnalExpluatacia = addTrigger(this, tr("Нарушения эксплуатации"), "", ":/images/defect/DefectZhurnalExpluatacia.png", SLOT(onDefectZhurnalExpluatacia()));
gidrAction.aDefectZhurnalExpluatacia->setToolTip(html_text("Нарушения эксплуатации на тепловой сети"));

gidrAction.aDefectZhurnalShurf = addTrigger(this, tr("Нарушения в шурфе"), "", ":/images/defect/DefectZhurnalShurf.png", SLOT(onDefectZhurnalShurf()));
gidrAction.aDefectZhurnalShurf->setToolTip(html_text("Нарушения в шурфе на тепловой сети\nНарушения в шурфе"));

gidrAction.aDefectZhurnalOsmotr = addTrigger(this, tr("Нарушения осмотра"), "", ":/images/defect/DefectZhurnalOsmotr.png", SLOT(onDefectZhurnalOsmotr()));
gidrAction.aDefectZhurnalOsmotr->setToolTip(html_text("Нарушения осмотра на тепловой сети"));

gidrAction.aDefectZhurnalOpress = addTrigger(this, tr("Нарушения опрессовки"), "", ":/images/defect/DefectZhurnalOpress.png", SLOT(onDefectZhurnalOpress()));
gidrAction.aDefectZhurnalOpress->setToolTip(html_text("Нарушения опрессовки на тепловой сети"));

gidrAction.aDefectZhurnalNarushenie = addTrigger(this, tr("Нарушения до ремонта"), "", ":/images/defect/DefectZhurnalNarushenie.png", SLOT(onDefectZhurnalNarushenie()));
gidrAction.aDefectZhurnalNarushenie->setToolTip(html_text("Нарушения до ремонта"));

gidrAction.aDefectZhurnalRemont = addTrigger(this, tr("Нарушения в процессе ремонта"), "", ":/images/defect/DefectZhurnalRemont.png", SLOT(onDefectZhurnalRemont()));
gidrAction.aDefectZhurnalRemont->setToolTip(html_text("Нарушения в процессе ремонта"));

gidrAction.aDefectZhurnalPosleRemont = addTrigger(this, tr("Нарушения после ремонта"), "", ":/images/defect/DefectZhurnalPosleRemont.png", SLOT(onDefectZhurnalPosleRemont()));
gidrAction.aDefectZhurnalPosleRemont->setToolTip(html_text("Нарушения после ремонта"));

gidrAction.aDefectZhurnalAll = addTrigger(this, tr("Отобразить все нарушения"), "", ":/images/defect/DefectZhurnalAll.png", SLOT(onDefectZhurnalAll()));
gidrAction.aDefectZhurnalAll->setToolTip(html_text("Отобразить все нарушения\nОтобразить все нарушения"));

gidrAction.aDefectWord1 = addTrigger(this, tr("Журнал нарушений"), "", ":/images/defect/DefectWord1.png", SLOT(onDefectWord1()));
gidrAction.aDefectWord1->setToolTip(html_text("Журнал нарушений\nЖурнал нарушений"));

gidrAction.aDefectWord2 = addTrigger(this, tr("Нарушения/ремонты трубопроводов тепловой сети(документ)"), "", ":/images/defect/DefectWord2.png", SLOT(onDefectWord2()));
gidrAction.aDefectWord2->setToolTip(html_text("Сведения по участку тепловой сети (документ)\nНарушения/ремонты трубопроводов тепловой сети(документ)"));

gidrAction.aDefectDocTable = addTrigger(this, tr("Нарушения/ремонты трубопроводов тепловой сети"), "", ":/images/defect/DefectDocTable.png", SLOT(onDefectDocTable()));
gidrAction.aDefectDocTable->setToolTip(html_text("Сведения по участку тепловой сети\nНарушения/ремонты трубопроводов тепловой сети"));

gidrAction.aDefectAnaliz = addTrigger(this, tr("Анализ нарушений"), "", ":/images/defect/DefectAnaliz.png", SLOT(onDefectAnaliz()));
gidrAction.aDefectAnaliz->setToolTip(html_text("Анализ нарушений\nАнализ нарушений"));

gidrAction.aRemontPovrDefOtobr = addTrigger(this, tr("Включить/отключить отображение объектов"), "", ":/images/defect/RemontPovrDefOtobr.png", SLOT(onRemontPovrDefOtobr()));
gidrAction.aRemontPovrDefOtobr->setToolTip(html_text("Включить/отключить отображение объектов"));

gidrAction.aRemontPovrOtop = addTrigger(this, tr("Выбор отопительного сезона"), "", ":/images/defect/RemontPovrOtop.png", SLOT(onRemontPovrOtop()));
gidrAction.aRemontPovrOtop->setToolTip(html_text("Выбор отопительного сезона\nВыбор отопительного сезона"));

gidrAction.aRemontOsmotrTeploSet = addTrigger(this, tr("Выбор фрагмента сети по Начальнику участка"), "", ":/images/defect/RemontOsmotrTeploSet.png", SLOT(onRemontOsmotrTeploSet()));
gidrAction.aRemontOsmotrTeploSet->setToolTip(html_text("Выбор фрагмента сети по Начальнику участка"));

gidrAction.aDefectBezUch = addTrigger(this, tr("Нарушения без участков"), "", ":/images/defect/DefectBezUch.png", SLOT(onDefectBezUch()));
gidrAction.aDefectBezUch->setToolTip(html_text("Нарушения без участков"));

gidrAction.aRemontPovrShurfAdd = addTrigger(this, tr("Установить плановый шурф"), "", ":/images/shurfy/RemontPovrShurfAdd.png", SLOT(onRemontPovrShurfAdd()));
gidrAction.aRemontPovrShurfAdd->setToolTip(html_text("Установить плановый шурф\nУстановить шурф"));

gidrAction.aRemontPovrShurfAddNeplan = addTrigger(this, tr("Установить НЕплановый шурф"), "", ":/images/shurfy/RemontPovrShurfAddNeplan.png", SLOT(onRemontPovrShurfAddNeplan()));
gidrAction.aRemontPovrShurfAddNeplan->setToolTip(html_text("Установить НЕплановый шурф\nУстановить шурф"));

gidrAction.aRemontShurfPlan = addTrigger(this, tr("Плановые шурфы"), "", ":/images/shurfy/RemontShurfPlan.png", SLOT(onRemontShurfPlan()));
gidrAction.aRemontShurfPlan->setToolTip(html_text("Плановые шурфы\nПлан шурфов"));

gidrAction.aRemontShurfPredpis = addTrigger(this, tr("Шурфы предписания"), "", ":/images/shurfy/RemontShurfPredpis.png", SLOT(onRemontShurfPredpis()));
gidrAction.aRemontShurfPredpis->setToolTip(html_text("Шурфы предписания\nШурфы предписания"));

gidrAction.aRemontShurfNarush = addTrigger(this, tr("Шурфы нарушения"), "", ":/images/shurfy/RemontShurfNarush.png", SLOT(onRemontShurfNarush()));
gidrAction.aRemontShurfNarush->setToolTip(html_text("Шурфы нарушения\nШурфы нарушения"));

gidrAction.aShurfProcess = addTrigger(this, tr("Шурфы в процессе выполнения"), "", ":/images/shurfy/ShurfProcess.png", SLOT(onShurfProcess()));
gidrAction.aShurfProcess->setToolTip(html_text("Шурфы в процессе выполнения\nШурфы в процессе выполнения"));

gidrAction.aShurfVypolneno = addTrigger(this, tr("Шурфы выполненные"), "", ":/images/shurfy/ShurfVypolneno.png", SLOT(onShurfVypolneno()));
gidrAction.aShurfVypolneno->setToolTip(html_text("Шурфы выполненные\nШурфы выполненные"));

gidrAction.aShurfAll = addTrigger(this, tr("Отобразить все Шурфы"), "", ":/images/shurfy/ShurfAll.png", SLOT(onShurfAll()));
gidrAction.aShurfAll->setToolTip(html_text("Отобразить все Шурфы\nОтобразить все Шурфы"));

gidrAction.aRemontShurfPlanUtverdit = addTrigger(this, tr("Утвердить План шурфов Отопительного сезона"), "", ":/images/shurfy/RemontShurfPlanUtverdit.png", SLOT(onRemontShurfPlanUtverdit()));
gidrAction.aRemontShurfPlanUtverdit->setToolTip(html_text("Утвердить План шурфов Отопительного сезона"));

gidrAction.aRemontShurfPlanExcel = addTrigger(this, tr("План шурфов (документ)"), "", ":/images/shurfy/RemontShurfPlanExcel.png", SLOT(onRemontShurfPlanExcel()));
gidrAction.aRemontShurfPlanExcel->setToolTip(html_text("План шурфов (документ)"));

gidrAction.aRemontShurfPlanExcelMonth = addTrigger(this, tr("План шурфов по месяцам (документ)"), "", ":/images/shurfy/RemontShurfPlanExcelMonth.png", SLOT(onRemontShurfPlanExcelMonth()));
gidrAction.aRemontShurfPlanExcelMonth->setToolTip(html_text("План шурфов по месяцам (документ)"));

gidrAction.aShurfTable = addTrigger(this, tr("Анализ Плана шурфов"), "", ":/images/shurfy/ShurfTable.png", SLOT(onShurfTable()));
gidrAction.aShurfTable->setToolTip(html_text("Анализ Плана шурфов\nАнализ Плана шурфов"));

gidrAction.aRemontShurfOtobr = addTrigger(this, tr("Включить/отключить отображение объектов"), "", ":/images/shurfy/RemontShurfOtobr.png", SLOT(onRemontShurfOtobr()));
gidrAction.aRemontShurfOtobr->setToolTip(html_text("Включить/отключить отображение Шурфов на карте местности\nВключить/отключить\n"));

gidrAction.aRemontPovrOtop = addTrigger(this, tr("Выбор отопительного сезона"), "", ":/images/shurfy/RemontPovrOtop.png", SLOT(onRemontPovrOtop()));
gidrAction.aRemontPovrOtop->setToolTip(html_text("Выбор отопительного сезона\nВыбор отопительного сезона"));

//gidrAction.aRemontOsmotrTeploSet = addTrigger(this, tr("Выбор фрагмента сети по Начальнику участка"), "", ":/images/shurfy/RemontOsmotrTeploSet.png", SLOT(onRemontOsmotrTeploSet()));
//gidrAction.aRemontOsmotrTeploSet->setToolTip(html_text("????????????"));

gidrAction.aShurfBezUch = addTrigger(this, tr("Шурфовки без участков"), "", ":/images/shurfy/ShurfBezUch.png", SLOT(onShurfBezUch()));
gidrAction.aShurfBezUch->setToolTip(html_text("Шурфовки без участков"));

gidrAction.aOsmotrAdd = addTrigger(this, tr("Создать контур осмотра"), "", ":/images/osmotr/OsmotrAdd.png", SLOT(onOsmotrAdd()));
gidrAction.aOsmotrAdd->setToolTip(html_text("Создать контур осмотра фрагмента тепловой сети\nСоздать контур осмотра"));

gidrAction.aOsmotrSave = addTrigger(this, tr("Обновить сохранение контура"), "", ":/images/osmotr/OsmotrSave.png", SLOT(onOsmotrSave()));
gidrAction.aOsmotrSave->setToolTip(html_text("Обновить сохранение контура осмотра\nОбновить сохранение контура"));

gidrAction.aListOsmotr2 = addTrigger(this, tr("Выполнить осмотр контура"), "", ":/images/osmotr/ListOsmotr2.png", SLOT(onListOsmotr2()));
gidrAction.aListOsmotr2->setToolTip(html_text("Выполнить осмотр выбранного контура фрагмента тепловой сети\nВыполнить осмотр контура"));

gidrAction.aLastOsmotr2 = addTrigger(this, tr("Последний контур осмотра"), "", ":/images/osmotr/LastOsmotr2.png", SLOT(onLastOsmotr2()));
gidrAction.aLastOsmotr2->setToolTip(html_text("Последний контур осмотра\nПоследний контур осмотра"));

gidrAction.aLastFaktory = addTrigger(this, tr("Выбор трубопроводов для осмотра"), "", ":/images/osmotr/LastFaktory.png", SLOT(onLastFaktory()));
gidrAction.aLastFaktory->setToolTip(html_text("Список трубопроводов последнего контура\nВыбор трубопроводов для осмотра"));

gidrAction.aOsmotrZapros1 = addTrigger(this, tr("Осмотренные фрагменты сети"), "", ":/images/osmotr/OsmotrZapros1.png", SLOT(onOsmotrZapros1()));
gidrAction.aOsmotrZapros1->setToolTip(html_text("Осмотренные фрагменты тепловой сети за период\nОсмотренные фрагменты сети"));

gidrAction.aOsmotrZapros2 = addTrigger(this, tr("Осмотренные трубопроводы за период"), "", ":/images/osmotr/OsmotrZapros2.png", SLOT(onOsmotrZapros2()));
gidrAction.aOsmotrZapros2->setToolTip(html_text("Осмотренные трубопроводы тепловой сети за период\nОсмотренные трубопроводы за период"));

gidrAction.aOsmotrZapros3 = addTrigger(this, tr("Неосмотренные трубопроводы за период"), "", ":/images/osmotr/OsmotrZapros3.png", SLOT(onOsmotrZapros3()));
gidrAction.aOsmotrZapros3->setToolTip(html_text("Неосмотренные трубопроводы тепловой сети за период\nНеосмотренные трубопроводы за период"));

gidrAction.aOsmotrAnaliz2 = addTrigger(this, tr("Анализ осмотра контуров"), "", ":/images/osmotr/OsmotrAnaliz2.png", SLOT(onOsmotrAnaliz2()));
gidrAction.aOsmotrAnaliz2->setToolTip(html_text("Анализ осмотра контуров фрагментов тепловой сети\nАнализ осмотра контуров"));

gidrAction.aOsmotrAnaliz = addTrigger(this, tr("Анализ осмотра трубопроводов контура"), "", ":/images/osmotr/OsmotrAnaliz.png", SLOT(onOsmotrAnaliz()));
gidrAction.aOsmotrAnaliz->setToolTip(html_text("Анализ осмотра трубопроводов выбранного контура\nАнализ осмотра трубопроводов контура"));

gidrAction.aRemontPovrDefOtobr = addTrigger(this, tr("Включить/отключить отображение объектов"), "", ":/images/osmotr/RemontPovrDefOtobr.png", SLOT(onRemontPovrDefOtobr()));
gidrAction.aRemontPovrDefOtobr->setToolTip(html_text("Включить/отключить отображение объектов"));

gidrAction.aRemontPovrOtop = addTrigger(this, tr("Выбор отопительного сезона"), "", ":/images/osmotr/RemontPovrOtop.png", SLOT(onRemontPovrOtop()));
gidrAction.aRemontPovrOtop->setToolTip(html_text("Выбор отопительного сезона\nВыбор отопительного сезона"));

//gidrAction.aRemontOsmotrTeploSet = addTrigger(this, tr("Выбор фрагмента сети по Начальнику участка"), "", ":/images/osmotr/RemontOsmotrTeploSet.png", SLOT(onRemontOsmotrTeploSet()));
//gidrAction.aRemontOsmotrTeploSet->setToolTip(html_text("????????????"));

gidrAction.aRemontAddPlan = addTrigger(this, tr("Сохранить контур плана ремонта"), "", ":/images/remont/RemontAddPlan.png", SLOT(onRemontAddPlan()));
gidrAction.aRemontAddPlan->setToolTip(html_text("Сохранить контур плана капитального/инвестиционного ремонта\nСохранить контур плана ремонта"));

gidrAction.aRemontAddCurrent = addTrigger(this, tr("Сохранить контур текущего ремонта"), "", ":/images/remont/RemontAddCurrent.png", SLOT(onRemontAddCurrent()));
gidrAction.aRemontAddCurrent->setToolTip(html_text("Сохранить контур текущего ремонта\nСохранить контур текущего ремонта"));

gidrAction.aRemontSave = addTrigger(this, tr("Обновить сохранение контура"), "", ":/images/remont/RemontSave.png", SLOT(onRemontSave()));
gidrAction.aRemontSave->setToolTip(html_text("Обновить сохранение контура ремонта\nОбновить сохранение контура"));

gidrAction.aListRemont2 = addTrigger(this, tr("Список контуров ремонтов"), "", ":/images/remont/ListRemont2.png", SLOT(onListRemont2()));
gidrAction.aListRemont2->setToolTip(html_text("Список контуров ремонтов\nСписок контуров ремонтов"));

gidrAction.aLastRemont2 = addTrigger(this, tr("Последний контур ремонта"), "", ":/images/remont/LastRemont2.png", SLOT(onLastRemont2()));
gidrAction.aLastRemont2->setToolTip(html_text("Последний контур ремонта\nПоследний контур ремонта"));

gidrAction.aRemontTrub = addTrigger(this, tr("Выбор трубопроводов контура ремонта"), "", ":/images/remont/RemontTrub.png", SLOT(onRemontTrub()));
gidrAction.aRemontTrub->setToolTip(html_text("Список трубопроводов последнего контура ремонта\nВыбор трубопроводов контура ремонта"));

gidrAction.aRemontPlan = addTrigger(this, tr("Контуры капитального/инвестиционного ремонтов"), "", ":/images/remont/RemontPlan.png", SLOT(onRemontPlan()));
gidrAction.aRemontPlan->setToolTip(html_text("Контуры капитального/инвестиционного ремонтов\nКонтуры капитального/инвестиционного ремонтов"));

gidrAction.aRemontCurrent = addTrigger(this, tr("Контуры текущих ремонтов"), "", ":/images/remont/RemontCurrent.png", SLOT(onRemontCurrent()));
gidrAction.aRemontCurrent->setToolTip(html_text("Контуры текущих ремонтов\nКонтуры текущих ремонтов"));

gidrAction.aRemontProcess = addTrigger(this, tr("Ремонт в процессе выполнения"), "", ":/images/remont/RemontProcess.png", SLOT(onRemontProcess()));
gidrAction.aRemontProcess->setToolTip(html_text("Ремонт в процессе выполнения\nРемонт в процессе выполнения"));

gidrAction.aRemontVypolneno = addTrigger(this, tr("Законченные ремонты"), "", ":/images/remont/RemontVypolneno.png", SLOT(onRemontVypolneno()));
gidrAction.aRemontVypolneno->setToolTip(html_text("Законченные ремонты\nЗаконченные ремонты"));

gidrAction.aRemontAll2 = addTrigger(this, tr("Отобразить все контура"), "", ":/images/remont/RemontAll2.png", SLOT(onRemontAll2()));
gidrAction.aRemontAll2->setToolTip(html_text("Отобразить все контура ремонтов\nОтобразить все контура"));

gidrAction.aRemontWord1 = addTrigger(this, tr("График выполнения ремонтов (документ)"), "", ":/images/remont/RemontWord1.png", SLOT(onRemontWord1()));
gidrAction.aRemontWord1->setToolTip(html_text("График выполнения ремонтов (документ)\nГрафик выполнения ремонтов"));

gidrAction.aRemontWord2 = addTrigger(this, tr("План капитальных/инвестиционных ремонтов (документ)"), "", ":/images/remont/RemontWord2.png", SLOT(onRemontWord2()));
gidrAction.aRemontWord2->setToolTip(html_text("План капитальных/инвестиционных ремонтов (документ)\nПлан ремонтов"));

gidrAction.aRemontWord3 = addTrigger(this, tr("План ремонтов по месяцам (документ)"), "", ":/images/remont/RemontWord3.png", SLOT(onRemontWord3()));
gidrAction.aRemontWord3->setToolTip(html_text("План ремонтов по месяцам (документ)\nПлан ремонтов"));

gidrAction.aRemontAnaliz2 = addTrigger(this, tr("Анализ контуров ремонта"), "", ":/images/remont/RemontAnaliz2.png", SLOT(onRemontAnaliz2()));
gidrAction.aRemontAnaliz2->setToolTip(html_text("Анализ контуров ремонта\nАнализ контуров ремонта"));

gidrAction.aRemontAnaliz = addTrigger(this, tr("График выполнения ремонтов"), "", ":/images/remont/RemontAnaliz.png", SLOT(onRemontAnaliz()));
gidrAction.aRemontAnaliz->setToolTip(html_text("График выполнения ремонтов\nГрафик выполнения ремонтов"));

gidrAction.aRemontPovrDefOtobr = addTrigger(this, tr("Включить/отключить отображение объектов"), "", ":/images/remont/RemontPovrDefOtobr.png", SLOT(onRemontPovrDefOtobr()));
gidrAction.aRemontPovrDefOtobr->setToolTip(html_text("Включить/отключить отображение объектов"));

gidrAction.aRemontPovrOtop = addTrigger(this, tr("Выбор отопительного сезона"), "", ":/images/remont/RemontPovrOtop.png", SLOT(onRemontPovrOtop()));
gidrAction.aRemontPovrOtop->setToolTip(html_text("Выбор отопительного сезона\nВыбор отопительного сезона"));

//gidrAction.aRemontOsmotrTeploSet = addTrigger(this, tr("Выбор фрагмента сети по Начальнику участка"), "", ":/images2/RemontOsmotrTeploSet.png", SLOT(onRemontOsmotrTeploSet()));
//gidrAction.aRemontOsmotrTeploSet->setToolTip(html_text("????????????"));

gidrAction.aRemontVyborNach = addTrigger(this, tr("Создать контур ремонта"), "", ":/images/remont/RemontVyborNach.png", SLOT(onRemontVyborNach()));
gidrAction.aRemontVyborNach->setToolTip(html_text("Создать контур ремонта фрагмента(ов) тепловой сети\nСоздать контур ремонта"));

//gidrAction.aMarkRev = addTrigger(this, tr("Добавить/удалить трубопровод к контуру"), "", ":/images/remont/MarkRev.png", SLOT(onMarkRev()));
//gidrAction.aMarkRev->setToolTip(html_text("Добавить/удалить участок трубопровода к контуру\nДобавить/удалить трубопровод к контуру"));

gidrAction.aOpresAddPlan = addTrigger(this, tr("Сохранить контур плана опрессовки"), "", ":/images/opres/OpresAddPlan.png", SLOT(onOpresAddPlan()));
gidrAction.aOpresAddPlan->setToolTip(html_text("Сохранить контур плана опрессовки\nСохранить контур плана опрессовки"));

gidrAction.aOpresSave = addTrigger(this, tr("Обновить сохранение контура"), "", ":/images/opres/OpresSave.png", SLOT(onOpresSave()));
gidrAction.aOpresSave->setToolTip(html_text("Обновить сохранение контура опрессовки\nОбновить сохранение контура"));

gidrAction.aOpresGranitsaRazdela = addTrigger(this, tr("Границы раздела"), "", ":/images/opres/OpresGranitsaRazdela.png", SLOT(onOpresGranitsaRazdela()));
gidrAction.aOpresGranitsaRazdela->setToolTip(html_text("Границы раздела\nГраницы раздела"));

gidrAction.aListOpres = addTrigger(this, tr("Список контуров опрессовок"), "", ":/images/opres/ListOpres.png", SLOT(onListOpres()));
gidrAction.aListOpres->setToolTip(html_text("Список контуров опрессовок сезона\nСписок контуров опрессовок"));

gidrAction.aLastOpres = addTrigger(this, tr("Последний контур опрессовки"), "", ":/images/opres/LastOpres.png", SLOT(onLastOpres()));
gidrAction.aLastOpres->setToolTip(html_text("Последний выбранный контур опрессовки\nПоследний выбранный контур опрессовки"));

gidrAction.aOpresPlan = addTrigger(this, tr("Контуры планов опрессовок сезона"), "", ":/images/opres/OpresPlan.png", SLOT(onOpresPlan()));
gidrAction.aOpresPlan->setToolTip(html_text("Контуры планов опрессовок сезона\nКонтуры планов опрессовок сезона"));

gidrAction.aOpresProcess = addTrigger(this, tr("Опрессовка в процессе выполнения"), "", ":/images/opres/OpresProcess.png", SLOT(onOpresProcess()));
gidrAction.aOpresProcess->setToolTip(html_text("Опрессовка в процессе выполнения сезона\nОпрессовка в процессе выполнения"));

gidrAction.aOpresVypolneno = addTrigger(this, tr("Завершенные опрессовки"), "", ":/images/opres/OpresVypolneno.png", SLOT(onOpresVypolneno()));
gidrAction.aOpresVypolneno->setToolTip(html_text("Завершенные опрессовки сезона\nЗавершенные опрессовки"));

gidrAction.aOpresAll2 = addTrigger(this, tr("Отобразить все контура"), "", ":/images/opres/OpresAll2.png", SLOT(onOpresAll2()));
gidrAction.aOpresAll2->setToolTip(html_text("Отобразить все контура опрессовок сезона\nОтобразить все контура"));

gidrAction.aOpresWord1 = addTrigger(this, tr("План опрессовок (документ)"), "", ":/images/opres/OpresWord1.png", SLOT(onOpresWord1()));
gidrAction.aOpresWord1->setToolTip(html_text("План опрессовок (документ)\nПлан опрессовок"));

gidrAction.aOpresWord2 = addTrigger(this, tr("График опрессовок по месяцам (документ)"), "", ":/images/opres/OpresWord2.png", SLOT(onOpresWord2()));
gidrAction.aOpresWord2->setToolTip(html_text("График опрессовок по месяцам (документ)\nГрафик опрессовок"));

gidrAction.aOpresAnaliz2 = addTrigger(this, tr("Анализ контуров опрессовок"), "", ":/images/opres/OpresAnaliz2.png", SLOT(onOpresAnaliz2()));
gidrAction.aOpresAnaliz2->setToolTip(html_text("Анализ контуров опрессовок сезона\nАнализ контуров опрессовок"));

gidrAction.aOpresAnaliz = addTrigger(this, tr("Анализ нарушений контура"), "", ":/images/opres/OpresAnaliz.png", SLOT(onOpresAnaliz()));
gidrAction.aOpresAnaliz->setToolTip(html_text("Анализ нарушений контура опрессовки\nАнализ нарушений контура"));

gidrAction.aRemontPovrDefOtobr = addTrigger(this, tr("Включить/отключить отображение объектов"), "", ":/images/opres/RemontPovrDefOtobr.png", SLOT(onRemontPovrDefOtobr()));
gidrAction.aRemontPovrDefOtobr->setToolTip(html_text("Включить/отключить отображение объектов"));

gidrAction.aRemontPovrOtop = addTrigger(this, tr("Выбор отопительного сезона"), "", ":/images/opres/RemontPovrOtop.png", SLOT(onRemontPovrOtop()));
gidrAction.aRemontPovrOtop->setToolTip(html_text("Выбор отопительного сезона\nВыбор отопительного сезона"));

//gidrAction.aRemontOsmotrTeploSet = addTrigger(this, tr("Выбор фрагмента сети по Начальнику участка"), "", ":/images/opres/RemontOsmotrTeploSet.png", SLOT(onRemontOsmotrTeploSet()));
//gidrAction.aRemontOsmotrTeploSet->setToolTip(html_text("????????????"));

gidrAction.aOpresVyborNach = addTrigger(this, tr("Создать контур опрессовки"), "", ":/images/opres/OpresVyborNach.png", SLOT(onOpresVyborNach()));
gidrAction.aOpresVyborNach->setToolTip(html_text("Создать контур опрессовки фрагмента(ов) тепловой сети\nСоздать контур опрессовки"));

gidrAction.aListRemont2 = addTrigger(this, tr("Список контуров ремонтов"), "", ":/images/opres/ListRemont2.png", SLOT(onListRemont2()));
gidrAction.aListRemont2->setToolTip(html_text("Список контуров ремонтов\nСписок контуров ремонтов"));

//gidrAction.aMarkRev = addTrigger(this, tr("Добавить/удалить трубопровод к контуру"), "", ":/images/opres/MarkRev.png", SLOT(onMarkRev()));
//gidrAction.aMarkRev->setToolTip(html_text("Добавить/удалить участок трубопровода к контуру\nДобавить/удалить трубопровод к контуру"));

gidrAction.aRemontPovrKarta = addTrigger(this, tr("Карта нарушений"), "", ":/images2/RemontPovrKarta.png", SLOT(onRemontPovrKarta()));
gidrAction.aRemontPovrKarta->setToolTip(html_text("Печать Шаблона Карты повреждаемости\nПечать Шаблона"));

gidrAction.aRemontPovrKarta2 = addTrigger(this, tr("Карта осмотра шурфа"), "", ":/images2/RemontPovrKarta2.png", SLOT(onRemontPovrKarta2()));
gidrAction.aRemontPovrKarta2->setToolTip(html_text("Карта повреждаемости\nКарта повреждаемости"));

gidrAction.aKorrozAdd = addTrigger(this, tr("Установить индикатор коррозии"), "", "://images/korrozia//KorrozAdd.png", SLOT(onKorrozAdd()));
gidrAction.aKorrozAdd->setToolTip(html_text("Установить индикатор коррозии для текущего отопительного сезона\nУстановить индикатор коррозии"));

gidrAction.aKorrozDel = addTrigger(this, tr("Удалить индикатор коррозии"), "", "://images/korrozia//KorrozDel.png", SLOT(onKorrozDel()));
gidrAction.aKorrozDel->setToolTip(html_text("Удалить индикатор коррозии для текущего отопительного сезона\nУдалить индикатор коррозии"));

gidrAction.aFragmentPolyKorroziaAr = addTrigger(this, tr("Удалить выбранные индикаторы"), "", "://images/korrozia//FragmentPolyKorroziaAr.png", SLOT(onFragmentPolyKorroziaAr()));
gidrAction.aFragmentPolyKorroziaAr->setToolTip(html_text("Удалить выбранные индикаторы"));

gidrAction.aKorrozInfo = addTrigger(this, tr("Информация по индикатору"), "", "://images/korrozia//KorrozInfo.png", SLOT(onKorrozInfo()));
gidrAction.aKorrozInfo->setToolTip(html_text("Информация по индикатору коррозии\nИнформация по индикатору коррозии"));

gidrAction.aKorroziaUpdate = addTrigger(this, tr("Выбор шаблона плана"), "", "://images/korrozia//KorroziaUpdate.png", SLOT(onKorroziaUpdate()));
gidrAction.aKorroziaUpdate->setToolTip(html_text("Выбор шаблона схемы индикаторов для формирования плана индикаторов коррозии для текущего отопительного сезона\nВыбор шаблона схемы для формирования плана индикаторов коррозии"));

gidrAction.aKorrozDoc3 = addTrigger(this, tr("План установки"), "", "://images/korrozia//KorrozDoc3.png", SLOT(onKorrozDoc3()));
gidrAction.aKorrozDoc3->setToolTip(html_text("План установки индикаторов коррозии для текущего отопительного сезона\nПлан установки"));

gidrAction.aKorrozTable = addTrigger(this, tr("Таблица индикаторов коррозии"), "", "://images/korrozia//KorrozTable.png", SLOT(onKorrozTable()));
gidrAction.aKorrozTable->setToolTip(html_text("Открыть таблицу по индикаторам коррозии\nТаблица индикаторов коррозии"));

gidrAction.aKorroziaZhurnal1 = addTrigger(this, tr("План индикаторов"), "", "://images/korrozia//KorroziaZhurnal1.png", SLOT(onKorroziaZhurnal1()));
gidrAction.aKorroziaZhurnal1->setToolTip(html_text("План установки индикаторов коррозии для текущего отопительного сезона\nПлан индикаторов текущего сезона"));

gidrAction.aKorroziaZhurnal2 = addTrigger(this, tr("Индикаторы установленные "), "", "://images/korrozia//KorroziaZhurnal2.png", SLOT(onKorroziaZhurnal2()));
gidrAction.aKorroziaZhurnal2->setToolTip(html_text("Установленные индикаторы коррозии в текущем отопительном сезоне\nУстановленные индикаторы текущего сезона"));

gidrAction.aKorroziaZhurnal3 = addTrigger(this, tr("Индикаторы извлеченные"), "", "://images/korrozia//KorroziaZhurnal3.png", SLOT(onKorroziaZhurnal3()));
gidrAction.aKorroziaZhurnal3->setToolTip(html_text("Извлеченные индикаторы коррозии в текущем отопительном сезоне\nИзвлеченные индикаторы текущего сезона"));

gidrAction.aKorroziaCurrent = addTrigger(this, tr("Все индикаторы сезона"), "", "://images/korrozia//KorroziaCurrent.png", SLOT(onKorroziaCurrent()));
gidrAction.aKorroziaCurrent->setToolTip(html_text("Отображение всех индикаторов коррозии на карте местности за текущий сезон\nОтображение всех индикаторов текущего сезона"));

gidrAction.aKorrozDoc2 = addTrigger(this, tr("Оценка за отопительный сезон"), "", "://images/korrozia//KorrozDoc2.png", SLOT(onKorrozDoc2()));
gidrAction.aKorrozDoc2->setToolTip(html_text("Оценка корозийного процесса за текущий отопительный сезон\nОценка коррозии за отопительный сезон"));

gidrAction.aKorrozDoc1 = addTrigger(this, tr("Оценка за выбранные сезоны"), "", "://images/korrozia//KorrozDoc1.png", SLOT(onKorrozDoc1()));
gidrAction.aKorrozDoc1->setToolTip(html_text("Оценка корозийного процесса за заданное количество сезонов\nОценка коррозии за заданные сезоны"));

gidrAction.aKorroziaOnoff = addTrigger(this, tr("Включить/отключить отображение"), "", "://images/korrozia//KorroziaOnoff.png", SLOT(onKorroziaOnoff()));
gidrAction.aKorroziaOnoff->setToolTip(html_text("Индикаторы коррозии отображать/не отображать на карте местности\nИндикаторы коррозии отображать/не отображать"));

gidrAction.aKorroziaSezon = addTrigger(this, tr("Выбор отопительного сезона"), "", "://images/korrozia//KorroziaSezon.png", SLOT(onKorroziaSezon()));
gidrAction.aKorroziaSezon->setToolTip(html_text("Выбор текущего отопительного сезона\nВыбор текущего отопительного сезона"));

gidrAction.aIznos = addTrigger(this, tr("Износ оборудования"), "", ":/images2/Iznos.png", SLOT(onIznos()));
gidrAction.aIznos->setToolTip(html_text("Износ оборудования"));

gidrAction.aRemontPlan = addTrigger(this, tr("Планирование ремонтов"), "", ":/images/remont/RemontPlan.png", SLOT(onRemontPlan()));
gidrAction.aRemontPlan->setToolTip(html_text("Планирование ремонтов"));

gidrAction.aElectroAdd = addTrigger(this, tr("Установить объект"), "", ":/images/electro/ElectroAdd.png", SLOT(onElectroAdd()));
gidrAction.aElectroAdd->setToolTip(html_text("Установить объект электросети\nУстановить объект электросети"));

gidrAction.aElectroInfo = addTrigger(this, tr("Информация по объекту"), "", ":/images/electro/ElectroInfo.png", SLOT(onElectroInfo()));
gidrAction.aElectroInfo->setToolTip(html_text("Информация по объекту электросети"));

gidrAction.aElectroTable = addTrigger(this, tr("Таблица"), "", ":/images/electro/ElectroTable.png", SLOT(onElectroTable()));
gidrAction.aElectroTable->setToolTip(html_text("Открыть таблицу с исходной информацией объекта Электросети\nТаблицы ЭС"));

gidrAction.aElectroDel = addTrigger(this, tr("Удалить объект"), "", ":/images/electro/ElectroDel.png", SLOT(onElectroDel()));
gidrAction.aElectroDel->setToolTip(html_text("Удалить объект электросети"));

gidrAction.aZhurnalElectro = addTrigger(this, tr("Журнал"), "", ":/images/electro/ZhurnalElectro.png", SLOT(onZhurnalElectro()));
gidrAction.aZhurnalElectro->setToolTip(html_text("Журнал объектов Электросети\nЖурнал объектов ЭC"));



gidrAction.aElectroIst = addTrigger(this,     tr("Создать источник ЭС"), "Создать источник электрической сети"                                , ":/images/electro/ElectroIst.png", SLOT(onElectroIst()));
gidrAction.aElectroPriem = addTrigger(this,   tr("Создать приемник ЭС"), "Создать приемник электрической сети"                                , ":/images/electro/ElectroPriem.png", SLOT(onElectroPriem()));
gidrAction.aElectroLine = addTrigger(this,    tr("Создать ЛЭП"), "Создать линию электропередач"                                               , ":/images/electro/ElectroLine.png", SLOT(onElectroLine()));
gidrAction.aElectroMufta = addTrigger(this,   tr("Создать муфту ЛЭП"), "Создать муфту линии электропередач"                                   , ":/images/electro/ElectroMufta.png", SLOT(onElectroMufta()));
gidrAction.aElectroOpora = addTrigger(this,   tr("Создать опору ЛЭП"), "Создать опору линии электропередач"                                   , ":/images/electro/ElectroOpora.png", SLOT(onElectroOpora()));
gidrAction.aElectroGil = addTrigger(this,     tr("Создать гильзу ЛЭП"), "Создать гильзу линии электропередач"                                 , ":/images/electro/ElectroGil.png", SLOT(onElectroGil()));
gidrAction.aElectroKanal = addTrigger(this,   tr("Создать канал ЛЭП"), "Создать канал линии электропередач"                                   , ":/images/electro/ElectroKanal.png", SLOT(onElectroKanal()));
gidrAction.aElectroZhurnal = addTrigger(this, tr("Журнал объектов ЭC"), "Журнал объектов Электросети"                                         , ":/images/electro/ElectroZhurnal.png", SLOT(onElectroZhurnal()));
gidrAction.aElectroOnoff = addTrigger(this,   tr("Объект Электросети отображать/не отображать"), "Объект Электросети отображать/не отображать", ":/images/electro/ElectroOnoff.png", SLOT(onElectroOnoff()));


gidrAction.aTeplopoteri = addTrigger(this, tr("Теплопотери"), "", ":/images/gidr/Teplopoteri.png", SLOT(onTeplopoteri()));
gidrAction.aTeplopoteri->setToolTip(html_text("Расчет тепловых потерь\nТепловые потери"));

gidrAction.aColors = addTrigger(this, tr("Стиль линии"), "", ":/images2/Colors.png", SLOT(onColors()));
gidrAction.aColors->setToolTip(html_text("Настройка стилей линии"));

gidrAction.aFon = addTrigger(this, tr("Цвет фона"), "", ":/images2/Fon.png", SLOT(onFon()));
gidrAction.aFon->setToolTip(html_text("Выбрать цвет фона"));

//gidrAction.aFont2 = addTrigger(this, tr("Шрифт текста"), "", ":/images2/Font2.png", SLOT(onFont2()));
//gidrAction.aFont2->setToolTip(html_text("Выбрать шрифт текста"));

gidrAction.aFont = addTrigger(this, tr("Шрифт надписей"), "", ":/images2/Font.png", SLOT(onFont()));
gidrAction.aFont->setToolTip(html_text("Выбрать шрифт надписей"));

gidrAction.aFontMag = addTrigger(this, tr("Шрифт названия магистрали"), "", ":/images2/FontMag.png", SLOT(onFontMag()));
gidrAction.aFontMag->setToolTip(html_text("Выбрать шрифт названия магистрали"));

gidrAction.aFontPanel = addTrigger(this, tr("Шрифт в проводнике"), "", ":/images2/FontPanel.png", SLOT(onFontPanel()));
gidrAction.aFontPanel->setToolTip(html_text("Выбрать шрифт для проводников"));

gidrAction.aTransp = addTrigger(this, tr("Прозрачность"), "", ":/images2/Transp.png", SLOT(onTransp()));
gidrAction.aTransp->setToolTip(html_text("Указать степень прозрачности отображения"));

gidrAction.aMasPic = addTrigger(this, tr("Номинальный масштаб"), "", ":/images2/MasPic.png", SLOT(onMasPic()));
gidrAction.aMasPic->setToolTip(html_text("Номинальный масштаб\nНоминальный масштаб"));

gidrAction.aMasPodpis = addTrigger(this, tr("Масштаб откл.надписей"), "", ":/images2/MasPodpis.png", SLOT(onMasPodpis()));
gidrAction.aMasPodpis->setToolTip(html_text("Масштаб отключения надписей объектов расчетной схемы"));

gidrAction.aTrio = addTrigger(this, tr("Передвинуть"), "", ":/images2/Trio.png", SLOT(onTrio()));
gidrAction.aTrio->setToolTip(html_text("Изменить координаты расчетной схемы при помощи ее сдвига"));

gidrAction.aIndexes = addTrigger(this, tr("Обновление индексов"), "", "", SLOT(onIndexes()));
gidrAction.aIndexes->setToolTip(html_text("Обновление индексов"));

gidrAction.aUpdateSetup = addTrigger(this, tr("Настройка обновлений"), "", ":/images2/UpdateSetup.png", SLOT(onUpdateSetup()));
gidrAction.aUpdateSetup->setToolTip(html_text("Настройка обновления программы по местам применения (на предприятиям)"));

gidrAction.aHttp2 = addTrigger(this, tr("Веб-сервер"), "", ":/images2/Http2.png", SLOT(onHttp2()));
gidrAction.aHttp2->setIcon(QIcon(":/images/gidr/Http2.png"));
gidrAction.aHttp2->setToolTip(html_text("Запустить Веб-сервер"));

gidrAction.aQuery = addTrigger(this, tr("Выполнить запрос"), "", "", SLOT(onQuery()));
// gidrAction.aExportPts->setToolTip(html_text("????????????"));



gidrAction.aExportPts = addTrigger(this, tr("Экспорт фрагмента ПТС"), "", ":/images2/ExportPts.png", SLOT(onExportPts()));
// gidrAction.aExportPts->setToolTip(html_text("????????????"));

gidrAction.aImportPts = addTrigger(this, tr("Импорт фрагмента ПТС"), "", ":/images2/ImportPts.png", SLOT(onImportPts()));
// gidrAction.aImportPts->setToolTip(html_text("????????????"));

gidrAction.aSetAvtoOn = addTrigger(this, tr("Сделать потребители автоматическими"), "", ":/images2/SetAvtoOn.png", SLOT(onSetAvtoOn()));
// gidrAction.aSetAvtoOn->setToolTip(html_text("????????????"));

gidrAction.aSetAvtoOff = addTrigger(this, tr("Сделать потребители неавтоматическими"), "", ":/images2/SetAvtoOff.png", SLOT(onSetAvtoOff()));
// gidrAction.aSetAvtoOff->setToolTip(html_text("????????????"));

gidrAction.aCreateSortNode = addTrigger(this, tr("Создание таблицы sortNodesForUchastok"), "", ":/images2/CreateSortNode.png", SLOT(onCreateSortNode()));
// gidrAction.aCreateSortNode->setToolTip(html_text("????????????"));

gidrAction.aRasList = addTrigger(this, tr("Список расчетов"), "", ":/images2/RasList.png", SLOT(onRasList()));
// gidrAction.aRasList->setToolTip(html_text("????????????"));

gidrAction.aSetPsMap = addTrigger(this, tr("Установить объекты ПС"), "", ":/images2/SetPsMap.png", SLOT(onSetPsMap()));
// gidrAction.aSetPsMap->setToolTip(html_text("????????????"));

gidrAction.aSavePng = addTrigger(this, tr("Сохранить как PNG"), "", ":/images2/SavePng.png", SLOT(onSavePng()));
// gidrAction.aSavePng->setToolTip(html_text("????????????"));

gidrAction.aUtNapr2 = addTrigger(this, tr("Сортировка участков"), "", ":/images2/UtNapr2.png", SLOT(onUtNapr2()));
// gidrAction.aUtNapr2->setToolTip(html_text("????????????"));

gidrAction.aInvert = addTrigger(this, tr("Инвертировать выделение"), "", ":/images2/Invert.png", SLOT(onInvert()));
// gidrAction.aInvert->setToolTip(html_text("????????????"));

gidrAction.aSetAddr = addTrigger(this, tr("Адреса потребителей"), "", ":/images2/SetAddr.png", SLOT(onSetAddr()));
// gidrAction.aSetAddr->setToolTip(html_text("????????????"));

gidrAction.aAlma = addTrigger(this, tr("Паспортизация"), "", ":/images2/Alma.png", SLOT(onAlma()));
gidrAction.aAlma->setCheckable(true);
gidrAction.aAlma->setChecked(m_bIsPts);
gidrAction.aAlma->setToolTip(html_text("Режим паспортизации (скрывать стрелки потоков)"));

gidrAction.aTuZav = addTrigger(this, tr("Установить статус Завершен для ТУ"), "", ":/images/tu/TuZav.png", SLOT(onTuZav()));
gidrAction.aTuZav->setToolTip(html_text("Установить статус Завершен для ТУ"));

gidrAction.aExport = addTrigger(this, tr("Экспорт гидравлики в TXT"), "", ":/images2/Export.png", SLOT(onExport()));
// gidrAction.aExport->setToolTip(html_text("????????????"));

gidrAction.aSetLineid = addTrigger(this, tr("!"), "", ":/images2/SetLineid.png", SLOT(onSetLineid()));
// gidrAction.aSetLineid->setToolTip(html_text("????????????"));

gidrAction.aSetIst = addTrigger(this, tr("Расстановка источников"), "", ":/images2/SetIst.png", SLOT(onSetIst()));
// gidrAction.aSetIst->setToolTip(html_text("????????????"));

gidrAction.aUchList = addTrigger(this, tr("Список участков ПТС"), "", ":/images2/UchList.png", SLOT(onUchList()));
gidrAction.aUchList->setToolTip(html_text("Обновить список участков ПТС"));

//
gidrAction.aFindcoordDeg = addTrigger(this, tr("Поиск по координатам"), "", ":/images2/FindcoordDeg.png", SLOT(onFindcoordDeg()));

// //gidrAction.aFindcoordDeg->setToolTip(html_text("????????????"));
//connect(gidrAction.aFindcoordDeg, SIGNAL(triggered()), this, SLOT(onFindcoordDeg()));

gidrAction.aAsyncCheck = addTrigger(this, tr("Проверка Async Task"), "", ":/images2/AsyncCheck.png", SLOT(onAsyncCheck()));
// gidrAction.aAsyncCheck->setToolTip(html_text("????????????"));

gidrAction.aCheckPo = addTrigger(this, tr("Проверка"), "", ":/images2/CheckPo.png", SLOT(onCheckPo()));
// gidrAction.aCheckPo->setToolTip(html_text("????????????"));

gidrAction.aResetGeo = addTrigger(this, tr("Обновить надписи"), "", ":/images2/ResetGeo.png", SLOT(onResetGeo()));
gidrAction.aResetGeo->setToolTip(html_text("Обновить значения надписей объектов геобазы и расчетной схемы"));

gidrAction.aPsAstanaName = addTrigger(this, tr("Отображение имен узлов"), "", ":/images2/PsAstanaName.png", SLOT(onPsAstanaName()));
gidrAction.aPsAstanaName->setToolTip(html_text("Режим отображения имен узлов на расчетной схеме в режиме паспортизации"));

gidrAction.aColorOnlyPts = addTrigger(this, tr("Цвета только для участков ПТС"), "", ":/images2/ColorOnlyPts.png", SLOT(onColorOnlyPts()));
gidrAction.aColorOnlyPts->setToolTip(html_text("Цвета\nЦвета"));

gidrAction.aExportDxf = addTrigger(this, tr("Автокад DXF"), "", ":/images2/ExportDxf.png", SLOT(onExportDxf()));
gidrAction.aExportDxf->setToolTip(html_text("Экспорт отображаемых фрагментов Расчетных схем в файл формата Автокад DXF"));

gidrAction.aExportShape = addTrigger(this, tr("ESRI shape-файлы"), "", ":/images2/ExportShape.png", SLOT(onExportShape()));
gidrAction.aExportShape->setToolTip(html_text("Экспорт отображаемых фрагментов Расчетных схем в файлы формата ESRI-shape"));

gidrAction.aPrPo = addTrigger(this, tr("Реальные -> обобщенные"), "", ":/images2/PrPo.png", SLOT(onPrPo()));
gidrAction.aPrPo->setToolTip(html_text("Преобразовать реальные потребители в обобщенные потребители"));

gidrAction.aUtNapr = addTrigger(this, tr("Сортировка участков"), "", ":/images2/UtNapr.png", SLOT(onUtNapr()));
gidrAction.aUtNapr->setToolTip(html_text("Сортировать участки в порядке их размещения на расчетной схеме, по направлению движения воды"));

gidrAction.aC3Del = addTrigger(this, tr("Удалить внутренние схемы"), "", ":/images2/C3Del.png", SLOT(onC3Del()));
gidrAction.aC3Del->setToolTip(html_text("Удалить внутренние схемы у всех узлов смешения с трехходовым регулирующим органом"));

gidrAction.aC3Add = addTrigger(this, tr("Создать внутренние схемы"), "", ":/images2/C3Add.png", SLOT(onC3Add()));
gidrAction.aC3Add->setToolTip(html_text("Создать внутренние схемы у всех узлов смешения с трехходовым регулирующим органом"));

gidrAction.aIspravl = addTrigger(this, tr("Восстановить файл"), "", ":/images2/Ispravl.png", SLOT(onIspravl()));
gidrAction.aIspravl->setToolTip(html_text("Восстановить файл"));

gidrAction.aOpc = addTrigger(this, tr("Настройка OPC сервера"), "", ":/images2/Opc.png", SLOT(onOpc()));
gidrAction.aOpc->setToolTip(html_text("Настройка OPC сервера SCADA системы"));

gidrAction.aGoogleElevationSet = addTrigger(this, tr("Геодезическая отметка настройка"), "", ":/images2/GoogleElevationSet.png", SLOT(onGoogleElevationSet()));
gidrAction.aGoogleElevationSet->setToolTip(html_text("Указать сайт для геодезических отметок"));

gidrAction.aMapSearch = addTrigger(this, tr("Поиск"), "", ":/images2/MapSearch.png", SLOT(onMapSearch()));
gidrAction.aMapSearch->setToolTip(html_text("Найти объект по адресу в OpenStreetMap"));

gidrAction.aMapPath = addTrigger(this, tr("Каталог"), "", ":/images2/MapPath.png", SLOT(onMapPath()));
gidrAction.aMapPath->setToolTip(html_text("Выбрать каталог для сохранения интернет карт"));

gidrAction.aFileOpenOld = addTrigger(this, tr("Импорт"), "", ":/images2/FileOpenOld.png", SLOT(onFileOpenOld()));
gidrAction.aFileOpenOld->setToolTip(html_text("Открыть и отконвертировать файл с расчетной схемой, созданной в предыдущей версии ТГИД-05,ТГИД-07"));

gidrAction.aAddUser = addTrigger(this, tr("Регистрация пользователей"), "", ":/images2/AddUser.png", SLOT(onAddUser()));
gidrAction.aAddUser->setToolTip(html_text("Регистрация пользователей администратором програмного комплекса"));

gidrAction.aPassword = addTrigger(this, tr("Изменить доступ пользователя"), "", ":/images2/Password.png", SLOT(onPassword()));
gidrAction.aPassword->setToolTip(html_text("Управление администратором права доступа пользователей к ресурсам програмного комплекса"));

gidrAction.aPassword2 = addTrigger(this, tr("Изменить пароль"), "", ":/images2/Password2.png", SLOT(onPassword2()));
gidrAction.aPassword2->setToolTip(html_text("Измение пользователем собственного пароля"));

gidrAction.aViewStatusBar = addTrigger(this, tr("Строка статуса"), "", ":/images2/ViewStatusBar.png", SLOT(onViewStatusBar()));
gidrAction.aViewStatusBar->setToolTip(html_text("Показать или спрятать строку состояния\nСтрока состояния"));

gidrAction.aHelpFinder = addTrigger(this, tr("Содержание\tF1"), "", ":/images2/HelpFinder.png", SLOT(onHelpFinder()));
gidrAction.aHelpFinder->setToolTip(html_text("Запустить систему поиска помощи\nПоиск информации"));

gidrAction.aUpdate = addTrigger(this, tr("Последнее обновление"), "", ":/images2/Update.png", SLOT(onUpdate()));
gidrAction.aUpdate->setToolTip(html_text("Повторить последнее обновление программы\nПоследнее обновление"));

gidrAction.aUpdateSetup = addTrigger(this, tr("Настройка обновлений"), "", ":/images2/UpdateSetup.png", SLOT(onUpdateSetup()));
gidrAction.aUpdateSetup->setToolTip(html_text("Настройка обновления программы по местам применения (на предприятиям)"));


gidrAction.aSetMsNew = addTrigger(this, tr(""), "", ":/images/pts/SetMsNew.png", SLOT(onSetMsNew()));
gidrAction.aSetRsNew = addTrigger(this, tr(""), "", ":/images/pts/SetRsNew.png", SLOT(onSetRsNew()));
gidrAction.aEditFindPs = addTrigger(this, tr(""), "", ":/images/pts/EditFindPs.png", SLOT(onEditFindPs()));
gidrAction.aRasprMagNew = addTrigger(this, tr(""), "", ":/images/pts/RasprMagNew.png", SLOT(onRasprMagNew()));
gidrAction.aMagNode = addTrigger(this, tr(""), "", ":/images/pts/MagNode.png", SLOT(onMagNode()));

//gidrAction.aNapr2 = addTrigger(this, tr(""), "", ":/images/pts/Napr2.png", SLOT(onNapr2()));
//gidrAction.aVyd4 = addTrigger(this, tr(""), "", ":/images/pts/Vyd4.png", SLOT(onVyd4()));
//gidrAction.aNapr2 = addTrigger(this, tr(""), "", ":/images/pts/Napr2.png", SLOT(onNapr2()));
//gidrAction.aMarkDel = addTrigger(this, tr(""), "", ":/images/pts/MarkDel.png", SLOT(onMarkDel()));

//gidrAction.aMsNode1 = addTrigger(this, tr(""), "", ":/images/pts/MsNode1.png", SLOT(onMsNode1()));
//gidrAction.aMsNode2 = addTrigger(this, tr(""), "", ":/images/pts/MsNode2.png", SLOT(onMsNode2()));
gidrAction.aSaveMAG = addTrigger(this, tr("Сохранить выделенную магистраль"), "", ":/images/new/10/Mag.svg", SLOT(onSaveMAG()));
gidrAction.aSaveMAG->setToolTip(html_text("Сохранить выделенную магистраль"));


gidrAction.aSaveMS = addTrigger(this, tr("Сохранить выделенный участок МС"), "", ":/images/new/10/MS.svg", SLOT(onSaveMS()));
gidrAction.aSaveMS->setToolTip(html_text("Сохранить выделенный участок МС"));

gidrAction.aSaveRS = addTrigger(this, tr("Сохранить выделенный участок РС"), "", ":/images/new/10/RS.svg", SLOT(onSaveRS()));
gidrAction.aSaveRS->setToolTip(html_text("Сохранить выделенный участок РС"));


gidrAction.aMsPassport = addTrigger(this, tr(""), "", ":/images/pts/MsPassport.png", SLOT(onMsPassport()));
gidrAction.aMsPassport->setToolTip(html_text("Сформировать Паспорт МС/РС\nСформировать Паспорт"));

gidrAction.aAktCreate = addTrigger(this, tr(""), "", ":/images/pts/AktCreate.png", SLOT(onAktCreate()));
gidrAction.aAktCreate->setToolTip(html_text("Создать Акт раздела границ"));



gidrAction.aPassportOnOff = addTrigger(this, tr(""), "", ":/images/pts/PassportOnOff.png", SLOT(onPassportOnOff()));
gidrAction.aPtsZhurnal = addTrigger(this, tr(""), "", ":/images/pts/PtsZhurnal.png", SLOT(onPtsZhurnal()));





//gidrAction.aAppAbout = addTrigger(this, tr("О программе"), "", ":/images2/AppAbout.png", SLOT(onAppAbout()));
//gidrAction.aAppAbout->setToolTip(html_text("Показать информацию о программе, ее версии и авторах\nО программе"));

//gidrAction.aQtAbout = addTrigger(this, tr("О Qt"), "", ":/images2/QtAbout.png", SLOT(onQtAbout()));
//gidrAction.aQtAbout->setToolTip(html_text("Показать информацию о программе, ее версии и авторах\nО Qt"));

//------------------------------------

//
//gidrAction.aAppAbout = addTrigger(this, tr("О программе"), "", ":/images2/AppAbout.png", SLOT(onAppAbout()));

//gidrAction.aAppAbout->setToolTip(html_text("Показать информацию о программе, ее версии и авторах\nО программе"));
//connect(gidrAction.aAppAbout, SIGNAL(triggered()), this, SLOT(onEscape()));

//---------------------------------------------

//gidrAction.escAction = new QAction(tr("Escape"), this);
//gidrAction.escAction->setShortcut(Qt::Key_Escape);
//connect(gidrAction.escAction, SIGNAL(triggered()), geo, SLOT(escape()));

gidrAction.aEsc = addTrigger(this, tr("Escape"), "", ":/images2/Esc.png", SLOT(onEscape()));
gidrAction.aEsc->setShortcut(Qt::Key_Escape);

//gidrAction.aMarkRev = addTrigger(this, tr("Изменить выделение"), "", ":/images/remont/MarkRev.png", SLOT(onMarkRev()));
//gidrAction.aMarkRev->setToolTip(html_text("Изменить выделение выбранного участка на обратное"));

//gidrAction.aMarkRev->setShortcut(Qt::ShiftModifier|Qt::Key_Insert);
//connect(gidrAction.aMarkRev, SIGNAL(triggered()), this, SLOT(onPassword2()));

/*

gidrAction.aMark = addTrigger(this, tr("Выделить область"), "", ":/images2/Mark.png", SLOT(onMark()));
gidrAction.aMark->setToolTip(html_text("Выделить область расчетной схемы тепловой сети для одновременного изменения параметров входящих в нее объектов\nВыделить область расчетной схемы"));
gidrAction.aMark->setCheckable(true);
*/

gidrAction.aViewToolbarMain = addToolbarToggle(this, tr("Главная"), "Главная", "", false);
gidrAction.aViewToolbarCxema = addToolbarToggle(this, tr("Схема"), "Показать/скрыть панель инструментов построения расчетной схемы тепловой сети\nПанель инструментов схемы", "", false);
gidrAction.aViewToolbarZ = addToolbarToggle(this, tr("Запросы"), "Показать/скрыть панель запросов\nПанель запросов", "", false);
gidrAction.aViewToolbar2PtsNew = addToolbarToggle(this, tr("Паспортизация"), "Панель Объекты ПТС", ":/images2/ViewToolbar2PtsNew.png", true);
gidrAction.aTuOnOff = addToolbarToggle(this, tr("Технические условия"), "Показать/скрыть панель инструментов выдачи Технических учловий на присоединение", ":/images/tu/TuOnOff.png", true);
gidrAction.aViewToolbarElectro = addToolbarToggle(this, tr("Электрические сети"), "Показать/скрыть панель инструментов создания объектов электрических сетей на карте местности", ":/images2/ViewToolbarElectro.png", true);
gidrAction.aViewToolbarControlTu = addToolbarToggle(this, tr("Контроль технического состояния"), "Контроль технического состояния", ":/images2/ViewToolbarControlTu.png", true);
gidrAction.aViewToolbarKorrozia = addToolbarToggle(this, tr("Индикаторы коррозии"), "Индикаторы коррозии", ":/images2/ViewToolbarKorrozia.png", true);
gidrAction.aToolbarRemont2Defect = addToolbarToggle(this, tr("Панель Нарушений"), "Отобразить панель Нарушений", ":/images/remont_any/ToolbarRemont2Defect.png", true);
gidrAction.aToolbarRemont2Shurf = addToolbarToggle(this, tr("Панель Шурфовки"), "Отобразить панель Шурфовки", ":/images/remont_any/ToolbarRemont2Shurf.png", true);
gidrAction.aToolbarRemont2Osmotr = addToolbarToggle(this, tr("Панель Осмотра"), "Отобразить панель Осмотра", ":/images/remont_any/ToolbarRemont2Osmotr.png", true);
gidrAction.aToolbarRemont2Remont = addToolbarToggle(this, tr("Панель Ремонтов"), "Отобразить панель Ремонтов", ":/images/remont_any/ToolbarRemont2Remont.png", true);
gidrAction.aToolbarRemont2Opres = addToolbarToggle(this, tr("Панель Опрессовки"), "Отобразить панель Опрессовки", ":/images/remont_any/ToolbarRemont2Opres.png", true);

QActionGroup *regimGroup = new QActionGroup(this);

gidrAction.aFragmentPoly = addToggleGroup(regimGroup, this, tr("Выделить многоугольный фрагмент"), "", ":/images2/FragmentPoly.png");
gidrAction.aFragmentPoly->setToolTip(html_text("Выделить многоугольный фрагмент"));

gidrAction.aMoveVyd = addToggleGroup(regimGroup, this, tr("Передвинуть фрагмент"), "", ":/images2/MoveVyd.png");
gidrAction.aMoveVyd->setToolTip(html_text("Передвинуть фрагмент"));


gidrAction.aMark = addToggleGroup(regimGroup, this, tr("Выделить область"), "Выделить область расчетной схемы тепловой сети для одновременного изменения параметров входящих в нее объектов\nВыделить область расчетной схемы", ":/images3/Mark.png");
gidrAction.aMarkDel = addToggleGroup(regimGroup, this, tr("Убрать выделение"), "Убрать выделение области расчетной схемы для одновременного изменения параметров входящих в нее объектов\nУбрать выделение", ":/images3/Mark.png");

gidrAction.aNode = addToggleGroup(regimGroup, this, tr("Установить узел\tN"), "Создание узла расчетной схемы тепловой сети\nУстановить узел", ":/images/cxema/Node.png");
gidrAction.aLine = addToggleGroup(regimGroup, this, tr("Линейный объект\tL"), "Создание линейного объекта расчетной схемы тепловой сети\nЛинейный объект", ":/images/cxema/Line.png");
gidrAction.aRename = addToggleGroup(regimGroup, this, tr("Изменить тип объекта"), "Выбор типа узла или линейного объекта\nТип", ":/images/cxema/Rename.png");
gidrAction.aZn = addToggleGroup(regimGroup, this, tr("Узел с заданным напором"), "Определение узлов с заданным напором\nУзел с заданным напором", ":/images/cxema/Zn.png");
gidrAction.aKam = addToggleGroup(regimGroup, this, tr("Внутренняя схема"), "Переход в слой составных объектов расчетной схемы тепловой сети\nВнутренняя схема", ":/images/cxema/Kam.png");
gidrAction.aKamVnesh = addToggleGroup(regimGroup, this, tr("Внешняя схема"), "Переход в слой составных объектов расчетной схемы тепловой сети\nВнешняяя схема", ":/images/cxema/Kam.png");


gidrAction.aNodeNeotr = addToggleGroup(regimGroup, this, tr("Установка конвертированных узлов\tC"), "Установить конвертированный узел расчетной схемы тепловой сети из файла формата Гид99w/Gid2005", ":/images2/NodeNeotr.png");
gidrAction.aNodeMove = addToggleGroup(regimGroup, this, tr("Перенести\tV"), "Перенести узел или изменить форму линейного объекта расчетной схемы тепловой сети\nПеренести", ":/images/cxema/NodeMove.png");
gidrAction.aDel = addToggleGroup(regimGroup, this, tr("Удалить объект\tD"), "Удалить узел или линейный объект расчетной схемы тепловой сети\nУдалить", ":/images/cxema/Del.png");
gidrAction.aOpenClose = addToggleGroup(regimGroup, this, tr("Закрыть/открыть объект"), "Закрыть/открыть объект расчетной схемы тепловой сети", ":/images2/OpenClose.png");
gidrAction.aNodeMove1 = addToggleGroup(regimGroup, this, tr("Разъединить линии"), "Разъединение двухтрубного линейного объекта в два однотрубных линейных объектов расчетной схемы тепловой сети \nРазъединить линии", ":/images/cxema/NodeMove1.png");
gidrAction.aSwap = addToggleGroup(regimGroup, this, tr("Изменить направление потока"), "Изменить направление потока энергоносителя на расчетной схеме тепловой сети\nИзменить направление потока", ":/images/cxema/Swap.png");

gidrAction.aMoveTo = addToggleGroup(regimGroup, this, tr("Таблица объекта"), "", ":/images3/MoveTo.png");
gidrAction.aMoveTo->setToolTip(html_text("Открыть таблицу объекта с исходной информацией расчетной схемы тепловой сети\nТаблица объекта"));




gidrAction.aMarkRev = addToggleGroup(regimGroup, this, tr("Добавить/удалить трубопровод к контуру"), "", ":/images/remont/MarkRev.png");
gidrAction.aMarkRev->setToolTip(html_text("Добавить/удалить участок трубопровода к контуру\nДобавить/удалить трубопровод к контуру"));





gidrAction.aMoveLineEndNext = addToggleGroup(regimGroup, this, tr(""), "", "");

gidrAction.aMoveLineEnd = addToggleGroup(regimGroup, this, tr("Перенести линию"), "Перенести один из концов линейного объекта расчетной схемы в другой узел расчетной схемы тепловой сети", ":/images2/MoveLineEnd.png");
gidrAction.aText = addToggleGroup(regimGroup, this, tr("Установить текст"), "Установить текст на расчетной схема тепловой сети", ":/images2/Text.png");
gidrAction.aInfo = addToggleGroup(regimGroup, this, tr("Свойства объекта\tI"), "Просмотр и редактирование свойств объекта расчетной схемы тепловой сети\nСвойства объекта", ":/images/new/1/cursor-click 1.svg");
gidrAction.aMovew = addToggleGroup(regimGroup, this, tr("Перемещение\tM"), "Включить режим перемещения схемы по экрану левой клавишей мыши\nПередвинуть", ":/images/new/2/hand.svg");
gidrAction.aNapr = addToggleGroup(regimGroup, this, tr("Выбpать напpавление"), "Отметить узлы для выбранного направления пьезометрического графика гидравлического режима тепловой сети\nВыбрать направление", ":/images/gidr/Napr.png");
gidrAction.aInfoGid = addToggleGroup(regimGroup, this, tr("Результат расчета"), "Просмотр результата расчета теплогидравлического режима объекта расчетной схемы тепловой сети\nРезультат расчета", ":/images3/InfoGid.png");
gidrAction.aGeoInfo = addToggleGroup(regimGroup, this, tr("Информация из геобазы"), "Показать информацию по объекту Геобазы\nИнформация из Геобазы", ":/images2/GeoInfo.png");
gidrAction.aZoom = addToggleGroup(regimGroup, this, tr("Масштабировать"), "Увеличить выделенное изображение \nУвеличить", ":/images/new/2/zoom-select 1.svg");
gidrAction.aGoogleElevation = addToggleGroup(regimGroup, this, tr("Геодезическая отметка"), "Показать геодезическую отметку", "");
gidrAction.aFragment = addToggleGroup(regimGroup, this, tr("Выделить фрагмент"), "Выделить фрагмент расчетной схемы для копирования в буфер обмена или печати", "");

gidrAction.aNapr2 = addToggleGroup(regimGroup, this, tr(""), "", ":/images/pts/Napr2.png");
gidrAction.aVyd4 = addToggleGroup(regimGroup, this, tr(""), "", ":/images/pts/Vyd4.png");
gidrAction.aMsNode1 = addToggleGroup(regimGroup, this, tr(""), "", ":/images/pts/MsNode1.png");
gidrAction.aMsNode2 = addToggleGroup(regimGroup, this, tr(""), "", ":/images/pts/MsNode2.png");

gidrAction.aNapr2->setToolTip(html_text("Выделить фрагмент сети между выбранными узлами для формирования участка РС\nВыделить фрагмент сети между выбранными узлами"));
gidrAction.aVyd4->setToolTip(html_text("Выделить фрагмент сети, подключённый к выбранному узлу для формирования МС/РС\nВыделить фрагмент сети"));
gidrAction.aMsNode1->setToolTip(html_text("Выбрать начальный узел участка МС/РС"));
gidrAction.aMsNode2->setToolTip(html_text("Выбрать конечный узел участка МС/РС"));


//gidrAction.aTuNew = addTrigger(this, tr("Создать ТУ"), "", ":/images/tu/TuNew.png", SLOT(onTuNew()));
//gidrAction.aTuInfo = addTrigger(this, tr("Свойства ТУ"), "", ":/images/tu/TuInfo.png", SLOT(onTuInfo()));
//gidrAction.aTuDel  = addTrigger(this, tr("Удалить ТУ"), "",  ":/images/tu/TuDel.png", SLOT(onTuDel())); 


//gidrAction.aPtsAdd->setToolTip(html_text("Установить объект ПТС\nУстановить объект ПТС"));

gidrAction.aTuNew = addTrigger(this, tr(""), "", ":/images/tu/TuNew.png", SLOT(onTuNew()));
gidrAction.aTuNew->setToolTip(html_text("Создать ТУ"));

gidrAction.aTuLine = addTrigger(this, tr(""), "", ":/images/tu/TuLine.png", SLOT(onTuLine()));
gidrAction.aTuLine->setToolTip(html_text("Создать Перспективный трубопровод"));



//gidrAction.aTuNew = addToggleGroup(regimGroup, this, tr("Создать ТУ"), "",  ":/images/tu/TuNew.png");
//gidrAction.aTuLine = addToggleGroup(regimGroup, this, tr("Создать Перспективный трубопровод"), "", ":/images/tu/TuLine.png");
gidrAction.aTuInfo = addToggleGroup(regimGroup, this, tr("Свойства ТУ"), "", ":/images/tu/TuInfo.png");
gidrAction.aTuDel = addToggleGroup(regimGroup, this, tr("Удалить ТУ"), "",  ":/images/tu/TuDel.png"); 

QActionGroup *editGroup = new QActionGroup(this);

gidrAction.aEditor1 = addToggleGroup(editGroup, this, tr("Режимы"), "Полный доступ к возможностям и функциям системы для создания/редактирования, расчета режимов расчетной схемы тепловой сети", ":/images/gidr/Editor1.png");
gidrAction.aEditor2 = addToggleGroup(editGroup, this, tr("Просмотр схемы"), "Минимальный доступ к возможностям и функциям системы", ":/images/gidr/Editor2.png");
gidrAction.aEditor4 = addToggleGroup(editGroup, this, tr("ПТС"), "Режим паспортизации, мониторинга ремонта, гидравлических испытаний", ":/images/gidr/Editor4.png");



// Ремонты

gidrAction.aPtsDel = addToggleGroup(regimGroup, this, tr("Удалить объект"), "", ":/images/pts/PtsDel.png");
gidrAction.aPtsDel->setToolTip(html_text("Удалить объект ПТС\nУдалить объект ПТС"));

gidrAction.aPtsInfo = addToggleGroup(regimGroup, this, tr("Информация по объекту"), "", ":/images/pts/PtsInfo.png");
gidrAction.aPtsInfo->setToolTip(html_text("Информация по объекту ПТС\nИнформация по объекту ПТС"));

gidrAction.aRemontPovrDefDel = addToggleGroup(regimGroup, this, tr("Удалить нарушение"), "", ":/images/defect/RemontPovrDefDel.png");
gidrAction.aRemontPovrDefDel->setToolTip(html_text("Удалить нарушение тепловой сети\nУдалить нарушение"));

gidrAction.aRemontPovrDefMove = addToggleGroup(regimGroup, this, tr("Перенести нарушение"), "", ":/images/defect/RemontPovrDefMove.png");
gidrAction.aRemontPovrDefMove->setToolTip(html_text("Перенести нарушение тепловой сети\nПеренести нарушение"));

gidrAction.aRemontPovrDefInfo = addToggleGroup(regimGroup, this, tr("Свойства объекта Нарушение"), "", ":/images/defect/RemontPovrDefInfo.png");
gidrAction.aRemontPovrDefInfo->setToolTip(html_text("Свойства объекта Нарушение\nСвойства объекта Нарушение"));

gidrAction.aRemontPovrShurfDel = addToggleGroup(regimGroup, this, tr("Удалить шурф"), "", ":/images/shurfy/RemontPovrShurfDel.png");
gidrAction.aRemontPovrShurfDel->setToolTip(html_text("Удалить шурф\nУдалить шурф"));

gidrAction.aRemontPovrShurfMove = addToggleGroup(regimGroup, this, tr("Перенести шурф"), "", ":/images/shurfy/RemontPovrShurfMove.png");
gidrAction.aRemontPovrShurfMove->setToolTip(html_text("Перенести шурф\nПеренести шурф"));

gidrAction.aRemontPovrShurfInfo = addToggleGroup(regimGroup, this, tr("Свойства объекта Шурф"), "", ":/images/shurfy/RemontPovrShurfInfo.png");
gidrAction.aRemontPovrShurfInfo->setToolTip(html_text("Свойства объекта Шурф\nСвойства объекта Шурф"));

// Конец ремонтов


gidrAction.aInfo->setChecked(true);

gidrAction.aWmsCust = addTrigger(this, tr("Настройка"), "", ":/images2/WmsCust.png", SLOT(onWmsCust()));
gidrAction.aWmsCust->setToolTip(html_text("Настройка WMS"));

QActionGroup *mapGroup = new QActionGroup(this);

gidrAction.aNomap = addToggleGroup(mapGroup, this, tr("Нет карты"), "Не отображать интернет карту", "");
gidrAction.aGoogleMap = addToggleGroup(mapGroup, this, tr("Карта"), "Показывать карту Google из интернета", "");
gidrAction.aGoogleSat = addToggleGroup(mapGroup, this, tr("Спутник"), "Показать спутниковую карту Google из интернета", "");
gidrAction.aGoogleHybrid = addToggleGroup(mapGroup, this, tr("Гибридная"), "Показать гибридную карту Google из интернета", "");
gidrAction.aVisicomMap = addToggleGroup(mapGroup, this, tr("Карта VISICOM"), "Показать карту VISICOM из интернета", "");
gidrAction.aWmsMap = addToggleGroup(mapGroup, this, tr("Карта WMS"), "Просмотр карты WMS", "");
gidrAction.aMapSemey = addToggleGroup(mapGroup, this, tr("Семей (город)"), "Семей (город)", "");
gidrAction.aMapSemeyTs = addToggleGroup(mapGroup, this, tr("Семей (тепловые сети)"), "Семей (тепловые сети)", "");
gidrAction.aOpenstreetmap = addToggleGroup(mapGroup, this, tr("OpenStreetMap"), "Показать карту OpenStreetMap из интернета", "");
gidrAction.a2gisMap = addToggleGroup(mapGroup, this, tr("2ГИС"), "Показать спутниковую карту 2ГИС из интернета", "");
gidrAction.aEsriSatMap = addToggleGroup(mapGroup, this, tr("ESRI Satellite"), "Показать спутниковую карту ESRI Satellite из интернета", "");




//gidrAction.aNomap->setChecked(true);
setMaps(m_internetMap);

gidrAction.aFindcoord = addTrigger(this, tr("Поиск по координатам"), "", ":/images2/Findcoord.png", SLOT(onFindcoord()));
// gidrAction.aFindcoord->setToolTip(html_text("????????????"));

gidrAction.aFindcoordDeg = addTrigger(this, tr("Поиск по географическим координатам"), "", ":/images2/FindcoordDeg.png", SLOT(onFindcoordDeg()));
// gidrAction.aFindcoordDeg->setToolTip(html_text("????????????"));



gidrAction.aNagrAlmatyTable = addTrigger(this, tr("Объекты АЛСЕКО"), "", "", SLOT(onNagrAlmatyTable()));
gidrAction.aNagrAlmatyAdd1 = addTrigger(this, tr("Добавить Объект (МЖД) АЛСЕКО"), "", "", SLOT(onNagrAlmatyAdd1()));
gidrAction.aNagrAlmatyAdd2 = addTrigger(this, tr("Добавить Объект (Прочие) АЛСЕКО"), "", "", SLOT(onNagrAlmatyAdd2()));
gidrAction.aNagrAlmaty = addTrigger(this, tr("Объекты АЛСЕКО по адресам"), "", "", SLOT(onNagrAlmaty()));
gidrAction.aNagrAlmatyOnOff = addTrigger(this, tr("Вывести нагрузки АЛСЕКО"), "", "", SLOT(onNagrAlmatyOnOff()));
gidrAction.aNagrAlmaXlsx = addTrigger(this, tr("Реестр объектов АЛСЕКО"), "", "", SLOT(onNagrAlmaXlsx()));
gidrAction.aNagrAlmaNenaid1 = addTrigger(this, tr("Реестр ненайденных объектов (МЖД) АЛСЕКО"), "", "", SLOT(onNagrAlmaNenaid1()));
gidrAction.aNagrAlmaNenaid2 = addTrigger(this, tr("Реестр ненайденных объектов (Прочие) АЛСЕКО"), "", "", SLOT(onNagrAlmaNenaid2()));
gidrAction.aNagrAlmaNenaid3 = addTrigger(this, tr("Здания АЛСЕКО без потребителя"), "", "", SLOT(onNagrAlmaNenaid3()));


   
//gidrAction.downAction->setShortcut(Qt::Key_Down);
//gidrAction.escAction->setShortcut(Qt::Key_Escape);
//gidrAction.file_newAction->setShortcut(QKeySequence::New);
//gidrAction.file_openAction->setShortcut(QKeySequence::Open);
//gidrAction.leftAction->setShortcut(Qt::Key_Left);
//gidrAction.rightAction->setShortcut(Qt::Key_Right);
//gidrAction.upAction->setShortcut(Qt::Key_Up);
//pjezoAction.leftAction->setShortcut(Qt::Key_Left);
//pjezoAction.rightAction->setShortcut(Qt::Key_Right);

int is_fragment = addCondition([this]() { return m_bIsFragment; });
int is_multyfragment = addCondition([this]() { return m_bIsPolyFragment; });
int is_mark = addCondition([this]() { return m_cxema.graph()->isMark(); });
int is_main = addCondition([this]() { return m_parent_id == 0; });
int is_no_pr = addCondition([this]() { 
    if (m_parent_id == 0) return true;

    CNode2 *nc = m_cxema.m_graph->find(m_parent_id);
    if (nc && nc->node.typ == TIP_PR) return false;
    return true; 
}
);

int is_edit = addCondition([this]() { 
    if (m_parent_id == 0) return true;

    CNode2 *nc = m_cxema.m_graph->find(m_parent_id);
    if (nc && nc->node.typ == TIP_PR) return false;
    return true; 
}
);



addActionWithCondition(gidrAction.aEditCopy,  is_fragment);
addActionWithCondition(gidrAction.aMoveVyd,  is_multyfragment);

addActionWithCondition(gidrAction.aSaveMS,  is_mark);
addActionWithCondition(gidrAction.aSaveRS,  is_mark);
addActionWithCondition(gidrAction.aSaveMAG,  is_mark);

addActionWithCondition(gidrAction.aSetOtv,  is_mark); // Установить ФИО техников
addActionWithCondition(gidrAction.aSetTr,  is_mark); // Установить Код расчетных температур
addActionWithCondition(gidrAction.aSetUr,  is_mark); // Установить Код удельных расходов
addActionWithCondition(gidrAction.aSetKvPt,  is_mark); // Установить Коэффициенты вариации по потребителям
addActionWithCondition(gidrAction.aSetUf,  is_mark); // Установить Коэффициенты смешения элеватора
addActionWithCondition(gidrAction.aSetTp,  is_mark); // Установить Тепловые пункты по потребителям
addActionWithCondition(gidrAction.aSetUdobVent,  is_mark); // Установить Удельный объем системы вентиляции
addActionWithCondition(gidrAction.aSetUdobOt,  is_mark); // Установить Удельный объем системы отопления
addActionWithCondition(gidrAction.aSetOpenKoef,  is_mark); // Коэф. часовой неравномерности
addActionWithCondition(gidrAction.aSetOpenRez,  is_mark); // Расчетные тепловые потери в рециркуляц. контуре ГВС
addActionWithCondition(gidrAction.aSetOpenRezT,  is_mark); // Температура в рециркуляционном трубопроводе ГВС
addActionWithCondition(gidrAction.aSetOpenGvsT,  is_mark); // Расчетная температура горячей воды
addActionWithCondition(gidrAction.aSetDiams,  is_mark); // Установить Диаметр
addActionWithCondition(gidrAction.aSetLosesShare,  is_mark); // Установить Долю местных потерь
addActionWithCondition(gidrAction.aSetKolChas,  is_mark); // Установить Количество часов работы
addActionWithCondition(gidrAction.aSetKvUt,  is_mark); // Установить Коэффициенты вариации по участкам
addActionWithCondition(gidrAction.aSetKti,  is_mark); // Установить Коэффициенты тепловых испытаний
addActionWithCondition(gidrAction.aSetOrg,  is_mark); // Установить Организации
addActionWithCondition(gidrAction.aSetPipeRemontType,  is_mark); // Установить Признак ремонта
addActionWithCondition(gidrAction.aSetTubingType,  is_mark); // Установить Тип прокладки
addActionWithCondition(gidrAction.aSetSher,  is_mark); // Установить Эквивалентную шероховатость
addActionWithCondition(gidrAction.aSetDate2,  is_mark); // Установить Дату первичного ввода в эксплуатацию
addActionWithCondition(gidrAction.aSetDate1,  is_mark); // Установить Дату последней перекладки
addActionWithCondition(gidrAction.aSetDate3,  is_mark); // Установить Дату планируемого ремонта
addActionWithCondition(gidrAction.aSetKodRs,  is_mark); // Установить Код расчетной схемы
addActionWithCondition(gidrAction.aGeodz,  is_mark); // Установить Отметки узлов горизонтали
addActionWithCondition(gidrAction.aSetKorrozia,  is_mark); // Установить Индикаторы коррозии
addActionWithCondition(gidrAction.aSetPodpOn,  is_mark); // Показать Надписи
addActionWithCondition(gidrAction.aSetPodpOff,  is_mark); // Не отображать Надписи


addActionWithCondition(gidrAction.aNode, is_edit);
addActionWithCondition(gidrAction.aLine, is_edit);
addActionWithCondition(gidrAction.aRename, is_edit);
addActionWithCondition(gidrAction.aZn, is_edit);
addActionWithCondition(gidrAction.aMoveLineEnd, is_edit);
//addActionWithCondition(gidrAction.aKam, is_edit);
addActionWithCondition(gidrAction.aNodeNeotr, is_edit);
addActionWithCondition(gidrAction.aNodeMove, is_edit);
addActionWithCondition(gidrAction.aDel, is_edit);
addActionWithCondition(gidrAction.aOpenClose, is_edit);
addActionWithCondition(gidrAction.aNodeMove1, is_edit);
addActionWithCondition(gidrAction.aSwap, is_edit);




//createShortcuts();

connect(editGroup, SIGNAL(triggered(QAction *)), this, SLOT(onEdit(QAction *)));

connect(regimGroup, SIGNAL(triggered(QAction *)), this, SLOT(onRegim(QAction *)));
connect(mapGroup, SIGNAL(triggered(QAction *)), this, SLOT(onMaps(QAction *)));





/*

gidrAction.aTuOnOff = addToolbarToggle(this, tr("Технические условия"), "Показать/скрыть панель инструментов выдачи Технических учловий на присоединение", ":/images/tu/TuOnOff.png", true);

gidrAction.aViewToolbarElectro = addToolbarToggle(this, tr("Электрические сети"), "Показать/скрыть панель инструментов создания объектов электрических сетей на карте местности", ":/images2/ViewToolbarElectro.png", true);

gidrAction.aViewToolbarControlTu = addToolbarToggle(this, tr("Контроль технического состояния"), "Контроль технического состояния", ":/images2/ViewToolbarControlTu.png", true);

gidrAction.aViewToolbarKorrozia = addToolbarToggle(this, tr("Индикаторы коррозии"), "Индикаторы коррозии", ":/images2/ViewToolbarKorrozia.png", true);
*/

        initActionValue();

    actionMenus[gidrAction.aDoItDr] = {gidrAction.aDoItDr, gidrAction.aDoItListDr, gidrAction.aMagFragment};
    actionMenus[gidrAction.aDoIt] = {gidrAction.aDoIt, gidrAction.aDoItList, gidrAction.aMagFragment};

    switch(m_idMenu) {
    case 1: m_nMenu = gidrAction.aEditor1; break;
    case 2: m_nMenu = gidrAction.aEditor2; break;
    case 4: m_nMenu = gidrAction.aEditor4; break;
    }

    if (m_nMenu) {
        m_nMenu->setChecked(true);
    }
}

QShortcut *makeShortcut(QWidget * widget, QAction *action, const QKeySequence &ks)
{
    QShortcut *shortcut = new QShortcut(ks, widget);
    QObject::connect(shortcut, &QShortcut::activated, action, &QAction::trigger);
    return shortcut;
}



void GidWidget::createShortcuts()
{

#if 0

    gidrAction.aAstr->setShortcut(Qt::Key_Asterisk);
    gidrAction.aDel->setShortcut(Qt::Key_D);
    gidrAction.aEditCopy->setShortcut(QKeySequence::Copy);
    gidrAction.aEditFindNode->setShortcut(Qt::ControlModifier|Qt::Key_F);
    gidrAction.aF4->setShortcut(Qt::Key_F4);
    gidrAction.aF5->setShortcut(Qt::Key_F5);
    gidrAction.aCtrlF5->setShortcut(Qt::ControlModifier|Qt::Key_F5);
    gidrAction.aFilePrint->setShortcut(QKeySequence::Print);
    gidrAction.aFindcoord->setShortcut(Qt::Key_F9);
    gidrAction.aFindcoordDeg->setShortcut(Qt::ControlModifier|Qt::Key_F9);
    gidrAction.aHelpFinder->setShortcut(Qt::Key_F1);
    gidrAction.aInfo->setShortcut(Qt::Key_I);
    gidrAction.aLine->setShortcut(Qt::Key_L);
    gidrAction.aMasall->setShortcut(Qt::Key_F3);
    gidrAction.aMasshtab->setShortcut(Qt::ControlModifier|Qt::Key_F3);
    gidrAction.aMinus->setShortcut(Qt::Key_Minus);
    gidrAction.aMovew->setShortcut(Qt::Key_M);
    gidrAction.aNode->setShortcut(Qt::Key_N);
    gidrAction.aNodeMove->setShortcut(Qt::Key_V);
    gidrAction.aPlus->setShortcut(Qt::Key_Plus);
    gidrAction.aZoom->setShortcut(Qt::Key_Z);

    gidrAction.aNodeNeotr->setShortcut(Qt::Key_C);
#else

/*
    QShortcut *shortcut = nullptr;

    shortcut = new QShortcut(action->shortcut(), this);
    QObject::connect(shortcut, &QShortcut::activated, action, &QAction::trigger);
*/

    makeShortcut(this, gidrAction.aAstr, Qt::Key_Asterisk);
    makeShortcut(this, gidrAction.aDel, Qt::Key_D);
    makeShortcut(this, gidrAction.aEditCopy, QKeySequence::Copy);

    makeShortcut(this, gidrAction.aEditFindNode, QKeySequence("Ctrl+F"));

/*
    gidrAction.aEditFindNode->setShortcut(QKeySequence("Ctrl+F"));
    QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
    QObject::connect(shortcut, &QShortcut::activated, gidrAction.aEditFindNode, &QAction::trigger);
*/

    makeShortcut(this, gidrAction.aF4, Qt::Key_F4);
    makeShortcut(this, gidrAction.aF5, Qt::Key_F5);
    makeShortcut(this, gidrAction.aCtrlF5, Qt::ControlModifier|Qt::Key_F5);
    makeShortcut(this, gidrAction.aFilePrint, QKeySequence::Print);

    makeShortcut(this, gidrAction.aFindcoord, Qt::Key_F9);

    makeShortcut(this, gidrAction.aFindcoordDeg, Qt::ControlModifier|Qt::Key_F9);
    makeShortcut(this, gidrAction.aHelpFinder, Qt::Key_F1);
    makeShortcut(this, gidrAction.aInfo, Qt::Key_I);
    makeShortcut(this, gidrAction.aLine, Qt::Key_L);

    makeShortcut(this, gidrAction.aMasall, Qt::Key_F3);
    makeShortcut(this, gidrAction.aMasshtab, Qt::ControlModifier|Qt::Key_F3);
    makeShortcut(this, gidrAction.aMinus, Qt::Key_Minus);
    makeShortcut(this, gidrAction.aMovew, Qt::Key_M);
    makeShortcut(this, gidrAction.aNode, Qt::Key_N);
    makeShortcut(this, gidrAction.aNodeMove, Qt::Key_V);
    makeShortcut(this, gidrAction.aPlus, Qt::Key_Plus);
    makeShortcut(this, gidrAction.aZoom, Qt::Key_Z);
    makeShortcut(this, gidrAction.aNodeNeotr, Qt::Key_C);
#endif
}


