/*
 * Copyright (C) Martijn Koopman
 * All Rights Reserved
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 */

#include "ribbon.h"
#include "mainwindow.h"
#include "mystd.h"
#include "qmenubar.h"
#include "ribbontabcontent.h"

#include <QDockWidget>
#include <QApplication>
#include <QStyleOption>
#include <QPainter>
#include <QMessageBox>

Ribbon::Ribbon(QWidget *parent)
  : QTabWidget(parent)
{
  // Determine default colors
  QColor bg = qApp->palette().color(QPalette::Window);
  QColor mid = qApp->palette().color(QPalette::Mid);

  // Note: the order in which the background/palette/stylesheet functions are
  // called does matter. Should be same as in Qt designer.
//  setAutoFillBackground(true);

  // Set stylesheet
    QString styleSheetText = readQSS("qss2/ribbon.qss");

    this->setStyleSheet(styleSheetText);

//    this->setStyleSheet("background-color:black;");

  // Set background color
//  QPalette pal = palette();
//  pal.setColor(QPalette::Window, Qt::white);
//  setPalette(pal);


    this->setContextMenuPolicy(Qt::CustomContextMenu);

//    this->setFixedHeight(110);
    this->setFixedHeight(105);

    connect(this->tabBar(), &QTabBar::tabBarClicked, this, &Ribbon::onTabBarClicked);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
}


/*
void Ribbon::processSingleClick() {
    qDebug() << "Закончил одиночный клик";
    QMessageBox::information(this, "Clicked", QString("Tab %1 clicked").arg(lastClickedIndex));
}
*/

void Ribbon::onTabBarClicked(int index)
{
    if (hidden || index == currentIndex()) {
        hide_show();

        if (!hidden) {
            qDebug() << "index " << index << " currentIndex() " << currentIndex();
//            setCurrentIndex(index);
             QTimer::singleShot(100, [=](){ setCurrentIndex(index); });
        }
    }

}

AutoGridLayout *Ribbon::addTab(const QString &tabName)
{
    AutoGridLayout *tab = qobject_cast<AutoGridLayout *>(getTab(tabName));
    if (tab) return tab;
  
  // Note: superclass QTabWidget also has a function addTab()
    RibbonTabContent *ribbonTabContent = new RibbonTabContent;
    QTabWidget::addTab(ribbonTabContent, tabName);

    tab = qobject_cast<AutoGridLayout *>(getTab(tabName));
    return tab;
}

void Ribbon::addTab(const QIcon &tabIcon, const QString &tabName)
{
  // Note: superclass QTabWidget also has a function addTab()
  RibbonTabContent *ribbonTabContent = new RibbonTabContent;
  QTabWidget::addTab(ribbonTabContent, tabIcon, tabName);
}

void Ribbon::removeTab(const QString &tabName)
{
  // Find ribbon tab
  for (int i = 0; i < count(); i++)
  {
    if (tabText(i).toLower() == tabName.toLower())
    {
      // Remove tab
      QWidget *tab = QTabWidget::widget(i);
      QTabWidget::removeTab(i);
      delete tab;
      break;
    }
  }
}
#if 0
void Ribbon::addGroup(const QString &tabName, const QString &groupName)
{
  // Find ribbon tab
  QWidget *tab = nullptr;
  for (int i = 0; i < count(); i++)
  {
    if (tabText(i).toLower() == tabName.toLower())
    {
      tab = QTabWidget::widget(i);
      break;
    }
  }

  if (tab != nullptr)
  {
    // Tab found
    // Add ribbon group
    RibbonTabContent *ribbonTabContent = static_cast<RibbonTabContent*>(tab);
    ribbonTabContent->addGroup(groupName);
  }
  else
  {
    // Tab not found
    // Create tab
    addTab(tabName);

    // Add ribbon group
    addGroup(tabName, groupName);
  }
}
#endif


QWidget * Ribbon::getTab(const QString &tabName)
{
    // Find ribbon tab
    QWidget *tab = nullptr;
    for (int i = 0; i < count(); i++) {
        if (tabText(i).toLower() == tabName.toLower()) {
            tab = QTabWidget::widget(i);
            break;
        }
    }
    return tab;
}


#if 0

AutoGridLayout *addNest1(QWidget *tab, int rows, int columns)
{
    QWidget *nestedWidget = new QWidget(tab);
    AutoGridLayout *nestedLayout = new AutoGridLayout(rows, columns, nestedWidget);
    return nestedLayout;
}

void closeNest1(AutoGridLayout *layer)
{
    QWidget *w = layer->parentWidget();

    if (w) {
        RibbonTabContent *tab = static_cast<RibbonTabContent*>(w->parentWidget());
        if (tab) {
            tab->ribbonHorizontalLayout->addWidget(w);
        }
        else {
            w->parentWidget()->addWidget(w);
        }
    }
}
#endif

#include <QVariantPointer>




AutoGridLayout *addNest2(QLayout *l, int rows, int columns, int typ)
{
    QWidget *tab = l->parentWidget();

    if (tab) {
        QFrame *nestedWidget = new QFrame(tab);
        nestedWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        nestedWidget->setProperty("class", QString("nest%1").arg(typ));
        nestedWidget->setProperty("typ", typ);
        nestedWidget->setProperty("rows", rows);

        nestedWidget->setProperty("layer", QVariant::fromValue(l));

        AutoGridLayout *nestedLayout = new AutoGridLayout(rows, columns, nestedWidget);

//        nestedLayout->setSpacing(5);

        nestedLayout->setHorizontalSpacing(45);
        nestedLayout->setVerticalSpacing(5);


        nestedLayout->setContentsMargins(0, 0, 0, 0);

        if (rows != 1) {
            nestedWidget->setFixedHeight(72);
        }
        if (rows == 1) {
            nestedWidget->setFixedHeight(32);
        }
        
        return nestedLayout;
    }

    return nullptr;
}

void setColumnWidths(QGridLayout *layout);


void closeNest2(AutoGridLayout *l)
{
    QWidget *w = l->parentWidget();
    QLayout *l0 = w->property("layer").value<QLayout*>();

    if (w && l0) {
        int typ = w->property("typ").toInt();
        l0->addWidget(w);

        if (typ == 3) {
            setColumnWidths(l);
        }
    }
}



AutoGridLayout *Ribbon::addNest(const QString &tabName, int rows, int columns, int typ)
{
    RibbonTabContent *tab = static_cast<RibbonTabContent*>(getTab(tabName));
    if (tab) {
        QHBoxLayout *l = tab->ribbonHorizontalLayout;
        return addNest2(l, rows, columns, typ);
    }

    return nullptr;


/*
    QWidget *tab = static_cast<RibbonTabContent*>(getTab(tabName));

    if (tab) {
        QWidget *nestedWidget = new QWidget(tab);
        nestedWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

        nestedWidget->setProperty("class", QString("nest%1").arg(typ));
        nestedWidget->setProperty("typ", typ);

        AutoGridLayout *nestedLayout = new AutoGridLayout(rows, columns, nestedWidget);
        return nestedLayout;
    }

    return nullptr;
*/
}

void setColumnWidths(QGridLayout *layout);


void Ribbon::closeNest(AutoGridLayout *l)
{
    QWidget *w = l->parentWidget();

    int typ = w->property("typ").toInt();
    QLayout *l0 = w->property("layer").value<QLayout*>();

    if (w && l0) {
        l0->addWidget(w);

        if (typ == 3) {
            setColumnWidths(l);
        }
    }

/*
    QWidget *w = l->parentWidget();

    int typ = w->property("typ").toInt();

    if (w) {
        RibbonTabContent *tab = static_cast<RibbonTabContent*>(w->parentWidget());
        if (tab) {
            tab->ribbonHorizontalLayout->addWidget(w);

            if (typ == 3) {
                setColumnWidths(l);
            }
        }
    }
*/
}


QLayout *Ribbon::getTabLayout(const QString &tabName)
{
    QWidget *tab = getTab(tabName);
    if (tab != nullptr) {
        RibbonTabContent *ribbonTabContent = static_cast<RibbonTabContent*>(tab);
        if (ribbonTabContent) {
            return ribbonTabContent->ribbonHorizontalLayout;
        }
    }
    return nullptr;
}

void Ribbon::addButton(const QString &tabName, const QString &groupName, WToolButton *button, int typ)
{
    QWidget *tab = getTab(tabName);
//    RibbonTabContent *tab = static_cast<RibbonTabContent*>(getTab(tabName));

    if (tab != nullptr) {
        RibbonTabContent *ribbonTabContent = static_cast<RibbonTabContent*>(tab);
        ribbonTabContent->addButton(groupName, button, typ);
        button->setIconSize(QSize(48,48));
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setFixedHeight(70);
        button->setFixedWidth(70);

//        addButton(tabName, groupName, button, typ);
//        tab->ribbonHorizontalLayout->addWidget(button);
    }
    else {
        addTab(tabName);
        addButton(tabName, groupName, button, typ);
    }
}

void Ribbon::removeButton(const QString &tabName, const QString &groupName, WToolButton *button)
{
#if 0
  // Find ribbon tab
  QWidget *tab = nullptr;
  for (int i = 0; i < count(); i++)
  {
    if (tabText(i).toLower() == tabName.toLower())
    {
      tab = QTabWidget::widget(i);
      break;
    }
  }

  if (tab != nullptr)
  {
    // Tab found
    // Remove ribbon button
    RibbonTabContent *ribbonTabContent = static_cast<RibbonTabContent*>(tab);
    ribbonTabContent->removeButton(groupName, button);

    if (ribbonTabContent->groupCount() == 0)
    {
      removeTab(tabName);
    }
  }
#endif
}

#include <QMenu>
#include <QMessageBox>
#include <QSettings>

QString insertLineBreaks(const QString& text, int maxLineLength = 50);

QString ispr_text(const QString &text)
{
    QString atext = text;

//    atext = atext.replace(" ", "\n");

    int index = atext.indexOf('\t');
    if (index != -1) {
        atext = atext.left(index);
    }

    while (atext.endsWith('.')) {
        atext.chop(1);  // Удаляем последний символ
    }

    atext = insertLineBreaks(atext, 10);

    QStringList lines = atext.split('\n');

    // Убираем все строки после третьей
    atext = lines.mid(0, 3).join('\n');  // Берем первые 3 строки и объединяем их

    qDebug() << QString("|%1| |%2|").arg(text, atext);
//    atext = atext.replace("\n", " ");

    return atext;
}


void Ribbon::slot_SomethingChecked()
{
    QAction *a = qobject_cast<QAction *>(sender());
    if (a) {
        QString txt1 = a->data().toString();

        int ii = 0;

        for (auto &it : map_tab) {
            int i = it.first;
            auto v = it.second;
            QWidget *w = std::get<0>(v);
            QString txt = std::get<1>(v);
            bool on = std::get<2>(v);

            if (on) ii ++;

            if (txt == txt1) {
                map_tab[i] = std::tuple<QWidget*, QString, bool> (w, txt, !on);

                QSettings settings;
                settings.setValue(QString("ribbon/%1").arg(txt), !on);

                if (on) {
                    QTabWidget::removeTab(ii-1);
                }
                else {
                    this->insertTab(ii, w, txt);
                }
            }
        }
    }
}



void Ribbon::init()
{
    int ii = 0;
    for (int i = 0; i < count(); i++) {
        QString txt = tabText(i);
        QWidget *w = widget(i);
        map_tab[i] = std::tuple<QWidget*, QString, bool> (w, txt, true);
    }


    for (auto &it : map_tab) {
        int i = it.first;
        auto v = it.second;
        QWidget *w = std::get<0>(v);
        QString txt = std::get<1>(v);

        QSettings settings;
        bool on = settings.value(QString("ribbon/%1").arg(txt), true).toBool();
        if (on) ii ++;

        map_tab[i] = std::tuple<QWidget*, QString, bool> (w, txt, on);

        if (!on) {
            QTabWidget::removeTab(ii);
        }
    }

    for (int i = 0; i < count(); i++) {
        auto tab = QTabWidget::widget(i);
        RibbonTabContent *ribbonTabContent = static_cast<RibbonTabContent*>(tab);

        if (ribbonTabContent) {
            ribbonTabContent->adjustColumn();
        }
    }
}

void Ribbon::hide_show()
{
    if (!hidden) {
        hidden = true;
        this->setFixedHeight(this->tabBar()->sizeHint().height());
    }
    else {
        hidden = false;
//        this->setFixedHeight(QWIDGETSIZE_MAX);
        this->setFixedHeight(105);
    }

    QDockWidget *dockWidget = main_window->ribbon_dock;
    dockWidget->adjustSize();
}

void Ribbon::menu_on_off()
{

    QAction *senderAction = qobject_cast<QAction *>(sender());
    if (senderAction) {
        bool checked = main_window->menuBar()->isVisible();
        senderAction->setChecked(!checked);
        main_window->menuBar()->setVisible(!checked);
    }
}

void Ribbon::contextMenu(QPoint point)
{
    QMenu *menu = new QMenu(this);

    QAction *a = new QAction(tr("Главное меню"), this);
    a->setCheckable(true);
    a->setChecked(main_window->menuBar()->isVisible());

    connect(a, &QAction::triggered, this, &Ribbon::menu_on_off);
    menu->addAction(a);

    for (auto &it : map_tab) {
        auto v = it.second;
//        std::get<0>(v);
        QString txt = std::get<1>(v);
        bool on = std::get<2>(v);

        QAction *a = new QAction(txt, this);
        a->setCheckable(true);
        a->setChecked(on);
        a->setData(txt);
        connect(a, SIGNAL(triggered()), this, SLOT(slot_SomethingChecked()));
        menu->addAction(a);
    }
    menu->popup(this->mapToGlobal(point));
}
