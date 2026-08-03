#pragma once

#include <QWidget>
#include <QTreeWidget>

#include "QSqlDatabase"
#include "std.h"

class CCxema;
class GidWidget;


class DockPTS : public QWidget
{
    Q_OBJECT

public:
    DockPTS(QWidget *parent = 0);
//    ~DockPTS();
//    void setCxema(CCxema *cxema) {m_cxema = cxema;};
    bool init(GidWidget *view, QSqlDatabase &db, bool is_ms);

    void vydMag(int id);
    void vydMS(int id);
    void vydRS(int id);

    /** Текущий выбранный участок МС/РС в дереве: typ=1 МС, typ=2 РС. */
    bool currentMsRs(int &typ, int &id) const;

signals:

public slots:
    void onEdit();
    void onMoveTo();
    void onEditGraph();
    void onPassport();

    void onPropertyAccepted();
    
protected slots:
    virtual void itemClicked( QTreeWidgetItem * item, int column);
    virtual void customContextMenuRequested(const QPoint &pos);

    void onSelectionChanged1();
    void onSelectionChanged2();



protected:

private:
    void hideEvent(QHideEvent *event);

    QIcon groupIcon;
    QIcon keyIcon;

    int m_ms_rs = 1;
    QTreeWidget *tree;
    QListWidget *list1;
    QListWidget *list2;
    GidWidget *m_view = nullptr;
    CCxema *m_cxema;
};

