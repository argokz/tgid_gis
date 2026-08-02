#ifndef RIBBONTABWIDGET_H
#define RIBBONTABWIDGET_H

#include <QTabWidget>
#include <QToolButton>

#include "ribbontabcontent.h"
#include "WToolButton.h"

class AutoGridLayout;

class Ribbon : public QTabWidget
{
  Q_OBJECT
public:
    explicit Ribbon(QWidget *parent = 0);

    AutoGridLayout *addTab(const QString &tabName);
    void addTab(const QIcon &tabIcon, const QString &tabName);

    void removeTab(const QString &tabName);

    void addGroup(const QString &tabName, const QString &groupName);
    void addButton(const QString &tabName, const QString &groupName, WToolButton *button, int typ);
    QWidget *getTab(const QString &tabName);
    QLayout *getTabLayout(const QString &tabName);

    void removeButton(const QString &tabName, const QString &groupName, WToolButton *button);
    AutoGridLayout *addNest(const QString &tabName, int rows, int columns, int typ);
    void closeNest(AutoGridLayout *layer);

    void init();


protected slots:
    virtual void contextMenu(QPoint point);
    virtual void slot_SomethingChecked();
    virtual void onTabBarClicked(int index);

protected slots:

    void hide_show();

private:

    void menu_on_off();
    std::map<int, std::tuple<QWidget*, QString, bool> > map_tab;
    bool hidden = false;

};

#endif // RIBBONTABWIDGET_H
