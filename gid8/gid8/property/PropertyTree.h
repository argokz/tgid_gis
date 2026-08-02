#pragma once

#include "qsqldatabase.h"
#include <QIcon>
#include <QTimer>
#include <QTreeWidget>
#include <QSharedPointer>
#include <mystd.h>


QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

class PropertyTree : public QTreeWidget
{
    Q_OBJECT

public:
    using SettingsPtr = QSharedPointer<QSettings>;
//    using TypeCheckerPtr = QSharedPointer<TypeChecker>;

    PropertyTree(int id, QWidget *parent = nullptr);
    ~PropertyTree();

    void restore(const QString & table);

    void setSettingsObject(const SettingsPtr &settings);
    QSize sizeHint() const override;

public slots:
    void setAutoRefresh(bool autoRefresh);
    void setFallbacksEnabled(bool enabled);
    void maybeRefresh();
    void refresh();
    void onSectionResized(int logicalIndex, int oldSize, int newSize);

protected:
    bool event(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateSetting(QTreeWidgetItem *item);
//    void textChanged(const QString &);
//    void clicked();

private:
    void hideEvent(QHideEvent *event);
    void updateChildItems(QTreeWidgetItem *parent);
    QTreeWidgetItem *childAt(QTreeWidgetItem *parent, int index) const;
    int childCount(QTreeWidgetItem *parent) const;
    int findChild(QTreeWidgetItem *parent, const QString &text, int startIndex) const;
    void moveItemForward(QTreeWidgetItem *parent, int oldIndex, int newIndex);
    void adjustColumnWidths(int index = 0);

    SettingsPtr settings;
//    TypeCheckerPtr m_typeChecker;
    QTimer refreshTimer;
    QIcon groupIcon;
    QIcon keyIcon;
    bool autoRefresh = false;

public:

    QTreeWidgetItem *createItem(const QString &text, QTreeWidgetItem *parent,
                                int index);

    QString m_table = "";
    QString m_q = "";

private:
    int m_id = -1;
};
