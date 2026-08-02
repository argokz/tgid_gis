#include "PropertyTree.h"
#include "variantdelegate.h"

#include <QApplication>
#include <QHeaderView>
#include <QScreen>
#include <QSettings>

#include <QtSql>
#include <QMessageBox>

#include <db/db.h>

QString argpath();
QString readCP1251Line(QFile &file);
QString findTableRusName(const QString & d, const QString & n);
QString findColumnRusName(const QString & d, const QString & n1, const QString & n2);


PropertyTree::PropertyTree(int id, QWidget *parent)
    : QTreeWidget(parent)
//    ,      m_typeChecker(new TypeChecker)
{
    m_id = id;
    autoRefresh = true;

//    setItemDelegate(new VariantDelegate(m_typeChecker, this));

//    setHeaderLabels({tr("Параметр"), tr("Подача"), tr("Обратка")});
    setHeaderLabels({tr(""), tr(""), tr("")});
    header()->setSectionResizeMode(0, QHeaderView::Interactive);
    header()->setSectionResizeMode(1, QHeaderView::Interactive);
    header()->setSectionResizeMode(2, QHeaderView::Interactive);
//    header()->setVisible(false);

    refreshTimer.setInterval(2000);

    groupIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),
                        QIcon::Normal, QIcon::Off);
    groupIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon),
                        QIcon::Normal, QIcon::On);
    keyIcon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));

    QString style = readFileUTF8("qss2/property.qss");
    this->setStyleSheet(style);

    connect(header(), &QHeaderView::sectionResized, this, &PropertyTree::onSectionResized);
    connect(&refreshTimer, &QTimer::timeout, this, &PropertyTree::maybeRefresh);
}

PropertyTree::~PropertyTree() = default;
  
void PropertyTree::setSettingsObject(const SettingsPtr &newSettings)
{
    settings = newSettings;
    clear();

    if (settings.isNull()) {
        refreshTimer.stop();
    } else {
        refresh();
        if (autoRefresh)
            refreshTimer.start();
    }
}

QSize PropertyTree::sizeHint() const
{
    const QRect availableGeometry = screen()->availableGeometry();
    return QSize(availableGeometry.width() * 2 / 3, availableGeometry.height() * 2 / 3);
}

void PropertyTree::setAutoRefresh(bool autoRefresh)
{
    this->autoRefresh = autoRefresh;
    if (!settings.isNull()) {
        if (autoRefresh) {
            maybeRefresh();
            refreshTimer.start();
        } else {
            refreshTimer.stop();
        }
    }
}

void PropertyTree::setFallbacksEnabled(bool enabled)
{
    if (!settings.isNull()) {
        settings->setFallbacksEnabled(enabled);

    // какое-то обновление, пока отключил
//        refresh();
    }
}

void PropertyTree::maybeRefresh()
{
    // Обновление по таймеру, пока отключил
//    if (state() != EditingState)
//        refresh();
}


#include <QFile>
#include <QHBoxLayout>
#include <QPushButton>

void PropertyTree::restore(const QString & table)
{
    m_table = table;

////////////////////////////////

    int nc = this->columnCount();

    QSettings settings;

    int w1 = settings.value(QString("PropertyTree/%1/%2/%3/%4").arg(table).arg(nc).arg(m_id).arg(1), 400).toInt();
    int w2 = settings.value(QString("PropertyTree/%1/%2/%3/%4").arg(table).arg(nc).arg(m_id).arg(2), 400).toInt();
    int w3 = settings.value(QString("PropertyTree/%1/%2/%3/%4").arg(table).arg(nc).arg(m_id).arg(3), 400).toInt();

    QTimer::singleShot(50, [=]() {
        this->setColumnWidth(0, w1);
        this->setColumnWidth(1, w2);
        this->setColumnWidth(2, w2);
    });


 // Авто-растяжение колонок, чтобы они занимали всю ширину
//    this->header()->setStretchLastSection(true); // Позволяет последней колонке занимать оставшееся пространство
//    this->header()->setSectionsStretchable(true); // Разрешает растягивание колонок

    adjustColumnWidths();

    for (int i = 0; i < this->topLevelItemCount(); i++) {
        QTreeWidgetItem * item = this->topLevelItem(i);                     
        if (item->childCount() > 0) {
            item->setExpanded(settings.value(QString("PropertyTree/%1/r/%2/%3").arg(table).arg(m_id).arg(i), true).toBool());
        }
    }
}

/*
void PropertyTree::textChanged(const QString & txt)
{
    PropertyStr pr = sender()->property("col").value<PropertyStr>();
    
//    pr.new_value = txt;:q


//  mapv_out[sender()->objectName()] = QVariant(txt);
}
*/


void PropertyTree::refresh()
{
//    if (settings.isNull())
//        return;

    disconnect(this, &QTreeWidget::itemChanged,
               this, &PropertyTree::updateSetting);

//    settings->sync();
//    updateChildItems(nullptr);

    connect(this, &QTreeWidget::itemChanged,
            this, &PropertyTree::updateSetting);
}

bool PropertyTree::event(QEvent *event)
{
    if (event->type() == QEvent::WindowActivate) {
        if (isActiveWindow() && autoRefresh)
            maybeRefresh();
    }
    return QTreeWidget::event(event);
}

void PropertyTree::updateSetting(QTreeWidgetItem *item)
{
    QString key = item->text(0);
    QTreeWidgetItem *ancestor = item->parent();
    while (ancestor) {
        key.prepend(ancestor->text(0) + QLatin1Char('/'));
        ancestor = ancestor->parent();
    }

//    PropertyStr pr = item->data(3, Qt::UserRole).value<PropertyStr>();


//    settings->setValue(key, item->data(2, Qt::UserRole));
    if (autoRefresh)
        refresh();
}

#

void PropertyTree::updateChildItems(QTreeWidgetItem *parent)
{
#if 0
#endif
}

QTreeWidgetItem *PropertyTree::createItem(const QString &text,
                                          QTreeWidgetItem *parent, int index)
{
    QTreeWidgetItem *after = nullptr;
    if (index != 0)
        after = childAt(parent, index - 1);

    QTreeWidgetItem *item;

    if (after) {
        if (parent)
            item = new QTreeWidgetItem(parent, after);
        else
            item = new QTreeWidgetItem(this, after);
    }
    else {
        if (parent)
            item = new QTreeWidgetItem(parent);
        else
            item = new QTreeWidgetItem(this);
    }

    item->setText(0, text);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    return item;
}

QTreeWidgetItem *PropertyTree::childAt(QTreeWidgetItem *parent, int index) const
{
    return (parent ? parent->child(index) : topLevelItem(index));
}

int PropertyTree::childCount(QTreeWidgetItem *parent) const
{
    return (parent ? parent->childCount() : topLevelItemCount());
}

int PropertyTree::findChild(QTreeWidgetItem *parent, const QString &text,
                            int startIndex) const
{
    for (int i = startIndex; i < childCount(parent); ++i) {
        if (childAt(parent, i)->text(0) == text)
            return i;
    }
    return -1;
}

void PropertyTree::moveItemForward(QTreeWidgetItem *parent, int oldIndex,
                                   int newIndex)
{
    for (int i = 0; i < oldIndex - newIndex; ++i)
        delete childAt(parent, newIndex);
}



void PropertyTree::hideEvent(QHideEvent *event)
{    
    int w1 = this->columnWidth(0);
    int w2 = this->columnWidth(1);
    int w3 = this->columnWidth(2);

    QSettings settings;

    int nc = this->columnCount();

    settings.setValue(QString("PropertyTree/%1/%2/%3/%4").arg(m_table).arg(nc).arg(m_id).arg(1), w1);
    settings.setValue(QString("PropertyTree/%1/%2/%3/%4").arg(m_table).arg(nc).arg(m_id).arg(2), w2);
    settings.setValue(QString("PropertyTree/%1/%2/%3/%4").arg(m_table).arg(nc).arg(m_id).arg(3), w3);


    for (int i = 0; i < this->topLevelItemCount(); i++) {
        QTreeWidgetItem * item = this->topLevelItem(i);
        if (item->childCount() > 0) {
            settings.setValue(QString("PropertyTree/%1/r/%2/%3").arg(m_table).arg(m_id).arg(i), item->isExpanded());
        }
    }


    QTreeWidget::hideEvent(event);
}


void PropertyTree::onSectionResized(int logicalIndex, int oldSize, int newSize)
{
    static bool resizing = false;

    if (resizing) {
        return; // Если уже изменяем, выходим
    }

    resizing = true; // Устанавливаем флаг
    adjustColumnWidths(0);

    resizing = false; // Сбрасываем флаг

//    qDebug() << "Column" << logicalIndex << "changed from" << oldSize << "to" << newSize;
}


void PropertyTree::adjustColumnWidths(int index )
{
    int availableWidth = viewport()->width();  // Доступная ширина внутри виджета
    int columnCount = this->columnCount();

    if (columnCount <= 1) columnCount = 2;

    int w1, w2;

    if (index == 0) {
        w1 = this->columnWidth(index);
        w2 = (availableWidth - w1) / (columnCount-1);
    }
    else {
        w2 = this->columnWidth(index);
        w1 = availableWidth - w2*(columnCount-1);
    }

    int w1_min = availableWidth/2;
    int w1_max = availableWidth*4/5;

    int w2_min = availableWidth/5;
    int w2_max = availableWidth/2;

    if (w1 < w1_min) w1 = w1_min;
    if (w1 > w1_max) w1 = w1_max;

    if (w2 < w2_min) w2 = w2_min;
    if (w2 > w2_max) w2 = w2_max;

    if (index == 0) {
        w2 = (availableWidth - w1) / (columnCount-1);
    }
    else {
        w1 = availableWidth - w2*(columnCount-1);
    }

    setColumnWidth(0, w1);
    setColumnWidth(1, w2);
    if (columnCount == 3) {
        setColumnWidth(2, w2);
    }
}

void PropertyTree::resizeEvent(QResizeEvent *event)
{
    QTreeWidget::resizeEvent(event);
    adjustColumnWidths(1);
}
