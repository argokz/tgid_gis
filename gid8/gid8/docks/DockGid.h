#pragma once

#include <QWidget>
#include <QTreeWidget>
#include "QSqlDatabase"

#include "std.h"

class GidWidget;
class CCxema;

class DockGid : public QWidget
{
    Q_OBJECT

public:
    DockGid(QWidget *parent = 0);
//    ~DockGid();
    bool init(GidWidget *view, CCxema *cxema);

signals:

public slots:

    
protected slots:
    virtual void itemClicked( QTreeWidgetItem * item, int column);
    virtual void contextMenu(QPoint point);
    virtual void onActive();

protected:

private:
    void hideEvent(QHideEvent *event);

    QIcon groupIcon;
    QIcon keyIcon;
 
    GidWidget *m_view = nullptr;
    QTreeWidget *tree;
//  std::map <QString, QUE3> map_q;
//    CCxema *m_cxema;
};

