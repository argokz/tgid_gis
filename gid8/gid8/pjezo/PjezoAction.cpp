#include <QtGui>
#include <QtSql>
#include <QtWidgets>
#include <gidview/GidWidget.h>
#include <gidview/colorgtd.h>
#include <any/MyMain.h>


#include "Pjezo.h"

void initActionValue();
QAction *addToggleValue(QWidget *widget, const QString & text, const QString & text2, const QString & icon_name, bool * value);

void CPjezo::onToggleValue(bool on)
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

QAction *addAction(QWidget *widget, const QString & text, const QString & text2, QIcon &icon, const char *slot_name)
{
    QAction *a;

    a = new QAction(text);
    a->setIcon(icon);
    a->setToolTip(text2);
    widget->connect(a, SIGNAL(triggered()), widget, slot_name);

    return a;
}

void CPjezo::createActions()
{
pjezoAction.aListPjezo = new QAction(tr("Список направлений..."));
pjezoAction.aListPjezo->setIcon(QIcon(":/images/pjezo/ListPjezo.png"));
pjezoAction.aListPjezo->setToolTip("Выбрать направление для выделения/построения пьезометрического графика гидравлического режима тепловой сети");
connect(pjezoAction.aListPjezo, SIGNAL(triggered()), this, SLOT(onListPjezo()));

pjezoAction.aFilePrint = new QAction(tr("Печать...\tCtrl+P"));
pjezoAction.aFilePrint->setIcon(QIcon(":/images/pjezo/FilePrint.png"));
pjezoAction.aFilePrint->setToolTip("Вывод на печать текущего документа\\nПечать");
connect(pjezoAction.aFilePrint, SIGNAL(triggered()), this, SLOT(onFilePrint()));

pjezoAction.aFilePrintPreview = new QAction(tr("Предварительный просмотр"));
pjezoAction.aFilePrintPreview->setIcon(QIcon(":/images/pjezo/FilePrintPreview.png"));
pjezoAction.aFilePrintPreview->setToolTip("Предварительный просмотр документа перед выводом на печать\\nПредварительный просмотр");
connect(pjezoAction.aFilePrintPreview, SIGNAL(triggered()), this, SLOT(onFilePrintPreview()));

pjezoAction.aFilePrintSetup = new QAction(tr("Параметры страницы..."));
pjezoAction.aFilePrintSetup->setIcon(QIcon(":/images/pjezo/FilePrintSetup.png"));
pjezoAction.aFilePrintSetup->setToolTip("Выбор принтера и свойств печати\\nПараметры страницы");
connect(pjezoAction.aFilePrintSetup, SIGNAL(triggered()), this, SLOT(onFilePrintSetup()));

pjezoAction.aAppExit = new QAction(tr("Выход"));
pjezoAction.aAppExit->setIcon(QIcon(":/images/pjezo/AppExit.png"));
pjezoAction.aAppExit->setToolTip("Выход из программы\\nЗавершить работу");
connect(pjezoAction.aAppExit, SIGNAL(triggered()), this, SLOT(onAppExit()));

pjezoAction.aEditCopy2 = new QAction(tr("Копировать\tCtrl+C"));
pjezoAction.aEditCopy2->setIcon(QIcon(":/images/pjezo/EditCopy2.png"));
pjezoAction.aEditCopy2->setToolTip("Сохранить пьезометрический график в буфере обмена");
connect(pjezoAction.aEditCopy2, SIGNAL(triggered()), this, SLOT(onEditCopy()));

pjezoAction.aPlus = new QAction(tr("Увеличить"));
pjezoAction.aPlus->setIcon(QIcon(":/images/pjezo/Plus.png"));
pjezoAction.aPlus->setToolTip("Ступенчато увеличить изображение\\nУвеличить");
connect(pjezoAction.aPlus, SIGNAL(triggered()), this, SLOT(onPlus()));

pjezoAction.aMinus = new QAction(tr("Уменьшить"));
pjezoAction.aMinus->setIcon(QIcon(":/images/pjezo/Minus.png"));
pjezoAction.aMinus->setToolTip("Ступенчато уменьшить изображение\\nУменьшить");
connect(pjezoAction.aMinus, SIGNAL(triggered()), this, SLOT(onMinus()));

pjezoAction.aOkno = new QAction(tr("Информация"));
pjezoAction.aOkno->setIcon(QIcon(":/images/pjezo/Okno.png"));
pjezoAction.aOkno->setToolTip("Вывод технической информации");
connect(pjezoAction.aOkno, SIGNAL(triggered()), this, SLOT(onOkno()));

pjezoAction.aPodacha = addToggleValue(this, tr("Подающий"), "Включить/выключить график напоров на участках подающего трубопровода", ":/images/pjezo/Podacha.png", &m_podacha);
pjezoAction.aObratka = addToggleValue(this, tr("Обратный"), "Включить/выключить график напоров на участках обратного трубопровода", ":/images/pjezo/Obratka.png", &m_obratka);
pjezoAction.aRealP = addToggleValue(this, tr("Факт"), "Включить/выключить график измеренных напоров на участках подающего и обратного трубопровода", ":/images/pjezo/RealP.png", &m_real);
pjezoAction.aStat = addToggleValue(this, tr("Статика"), "Включить/выключить график статических напоров на выбранном направлении", ":/images/pjezo/Stat.png", &m_stat);
pjezoAction.aNapor = addToggleValue(this, tr("Напоры"), "Включить/выключить оцифровку напоров в узлах подающего и обратного трубопроводов", ":/images/pjezo/Napor.png", &m_napor);
pjezoAction.aOtmet = addToggleValue(this, tr("Отметки"), "Включить/выключить оцифровку геодезических отметов трубопроводов", ":/images/pjezo/Otmet.png", &m_otmet);
pjezoAction.aVysot = addToggleValue(this, tr("Здания"), "Включить/выключить изображения зданий и их высоты в узлах присоединения", ":/images/pjezo/Vysot.png", &m_vysot);

pjezoAction.aExcel = new QAction(tr("Таблицы Excel.."));
pjezoAction.aExcel->setIcon(QIcon(":/images/pjezo/Excel.png"));
pjezoAction.aExcel->setToolTip("Экспорт в MS Excel\\nЭкспорт в MS Excel");
connect(pjezoAction.aExcel, SIGNAL(triggered()), this, SLOT(onExcel()));

pjezoAction.aTechinfo = new QAction(tr("Тех.информация"));
pjezoAction.aTechinfo->setIcon(QIcon(":/images/pjezo/Techinfo.png"));
pjezoAction.aTechinfo->setToolTip("Обзор технологической информации\\nТехнологическая информация");
connect(pjezoAction.aTechinfo, SIGNAL(triggered()), this, SLOT(onTechinfo()));

pjezoAction.aSavepjezo = new QAction(tr("Сохранить направление"));
pjezoAction.aSavepjezo->setIcon(QIcon(":/images/pjezo/Savepjezo.png"));
pjezoAction.aSavepjezo->setToolTip("Сохранить выбранное направление (путь) пьезометрического графика\\nСохранить направление графика");
connect(pjezoAction.aSavepjezo, SIGNAL(triggered()), this, SLOT(onSavePjezo()));

pjezoAction.aDouble = new QAction(tr("Двойной пьезометр"));
pjezoAction.aDouble->setIcon(QIcon(":/images/pjezo/Double.png"));
pjezoAction.aDouble->setToolTip("Выбор сохраненного результата расчета режима для построения сдвоенного пьез.графика по выбранному направлению\\nПостроение сдвоенного графика");
connect(pjezoAction.aDouble, SIGNAL(triggered()), this, SLOT(onDouble()));


/*
pjezoAction.aViewStatusBar = new QAction(tr("Строка статуса"));
pjezoAction.aViewStatusBar->setIcon(QIcon(":/images/pjezo/ViewStatusBar.png"));
pjezoAction.aViewStatusBar->setToolTip("Показать или спрятать строку состояния\\nСтрока состояния");
connect(pjezoAction.aViewStatusBar, SIGNAL(triggered()), this, SLOT(onViewStatusBar()));
*/

pjezoAction.aInf = new QAction(tr("Отображаемая информация..."));
pjezoAction.aInf->setIcon(QIcon(":/images/pjezo/Inf.png"));
pjezoAction.aInf->setToolTip("Настройка селективного вывода информации на расчетную схему тепловой сети\\nОтображаемая информация");
connect(pjezoAction.aInf, SIGNAL(triggered()), this, SLOT(onInf()));

pjezoAction.aFont = new QAction(tr("Шрифт"));
pjezoAction.aFont->setIcon(QIcon(":/images/pjezo/Font.png"));
pjezoAction.aFont->setToolTip("Выбрать шрифт надписей");
connect(pjezoAction.aFont, SIGNAL(triggered()), this, SLOT(onFont()));

pjezoAction.aHelpFinder = new QAction(tr("Содержание"));
pjezoAction.aHelpFinder->setIcon(QIcon(":/images/pjezo/HelpFinder.png"));
pjezoAction.aHelpFinder->setToolTip("Запустить систему поиска помощи\\nПоиск информации");
connect(pjezoAction.aHelpFinder, SIGNAL(triggered()), this, SLOT(onHelpFinder()));

//------------------------

pjezoAction.aMasall = new QAction(tr("Отобразить всю схему\tF3"));
pjezoAction.aMasall->setIcon(QIcon(":/images/pjezo/Masall.png"));
pjezoAction.aMasall->setToolTip("");
connect(pjezoAction.aMasall, SIGNAL(triggered()), this, SLOT(onMasall()));

pjezoAction.aMoveTo = new QAction(tr(""));
pjezoAction.aMoveTo->setIcon(QIcon(":/images/pjezo/MoveTo.png"));
pjezoAction.aMoveTo->setToolTip("");
connect(pjezoAction.aMoveTo, SIGNAL(triggered()), this, SLOT(onMoveTo()));

pjezoAction.aPlus->setShortcut(Qt::Key_Plus);
pjezoAction.aMinus->setShortcut(Qt::Key_Minus);
pjezoAction.aMasall->setShortcut(Qt::Key_F3);
pjezoAction.aEditCopy2->setShortcut(QKeySequence::Copy);

        initActionValue();
}
