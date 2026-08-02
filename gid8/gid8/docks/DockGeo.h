#pragma once

#include <QWidget>
#include <QTreeWidget>
#include "QSqlDatabase"

#include "geo/Klassif.h"
#include "std.h"

class GidWidget;
class CCxema;

class DockGeo : public QWidget
{
    Q_OBJECT

public:
    DockGeo(QWidget *parent = 0);
//    ~DockGeo();
    void setCxema(CCxema *cxema) {m_cxema = cxema;};
    bool init(GidWidget *view, const QString & baza);
    void reset();

signals:

public slots:
    
protected slots:
    virtual void itemClicked( QTreeWidgetItem * item, int column);
    virtual void contextMenu(QPoint point);

    void onKalssif();
    void onTable();
    void onAdd();
    void onAddr();
    void onAddrAccepted();

protected:

    Klassif *m_kls = nullptr;
    Klassif *getKls();
    bool setCheck(QTreeWidgetItem *item, bool yes);

private:
    void hideEvent(QHideEvent *event);

    QIcon groupIcon;
    QIcon keyIcon;

    QString m_raj = "";
    QString m_str = "";
    QString m_dom = "";


    QTreeWidget *tree;
//  std::map <QString, QUE3> map_q;
    GidWidget *m_view = nullptr;
    CCxema *m_cxema;
};

