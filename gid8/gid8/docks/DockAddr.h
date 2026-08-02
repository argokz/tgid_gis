#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QSplitter>
#include <QListWidget>

#include "std.h"

class GidWidget;
class CCxema;

class DockAddr : public QWidget
{
    Q_OBJECT

public:
    DockAddr(QWidget *parent = 0);
//    ~DockAddr();
    void setCxema(CCxema *cxema) {m_cxema = cxema;};

    void init(GidWidget *view, const QString &baza);
    void init2(const QString &nazv, const QString &raj, const QString &str, const QString &dom)
    {
        m_nazv = nazv; m_raj = raj; m_str = str; m_dom = dom;
        init_find(str_fnd);
    };

    void init2_out(QString &nazv, QString &raj, QString &str, QString &dom)
    {
        nazv = m_nazv; raj = m_raj; str = m_str; dom = m_dom;
    };


signals:

public slots:

protected slots:
//    virtual void itemClicked( QTreeWidgetItem * item, int column);

protected:

private slots:
    void onSelectionChanged1();
    void onSelectionChanged2();
//    void onSelectionChanged3();
    void onItemClicked(QListWidgetItem *item);
    void onTextChanged(const QString &text);

private:
    void hideEvent(QHideEvent *event);
    void restoreSettings();
    void saveSettings();
    void init_find(const QString &txt);


    QIcon groupIcon;
    QIcon keyIcon;

    QTreeWidget *tree;
    CCxema *m_cxema;

    QSplitter *bottomSplitter;
    QSplitter *mainSplitter;
    QListWidget *list1;
    QListWidget *list2;
    QListWidget *list3;

    QLineEdit *fnd;

    QString m_nazv;
    QString m_raj;
    QString m_str;
    QString m_dom;
    QString str_fnd = "";

    GidWidget *m_view = nullptr;
};
