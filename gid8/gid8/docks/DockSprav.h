#pragma once

#include <QWidget>
#include <QTreeWidget>

#include "std.h"

class CCxema;

struct QUE3 {
  QString q = "";
  QString tab = "";
  QString mdb = "";
  QString add = "";
};

class DockSprav : public QWidget
{
    Q_OBJECT

public:
    DockSprav(QWidget *parent = 0);
//    ~DockSprav();
    void setCxema(CCxema *cxema) {m_cxema = cxema;};

signals:

public slots:

protected slots:
    void onEdit();
    virtual void itemClicked( QTreeWidgetItem * item, int column);
    virtual void customContextMenuRequested(const QPoint &pos);

protected:

private:
    void hideEvent(QHideEvent *event);
    void edit_sprav(const QString & par, const QString & tn);

    QIcon groupIcon;
    QIcon keyIcon;

    QTreeWidget *tree;
    std::map <QString, QUE3> map_q;
    CCxema *m_cxema;
};
