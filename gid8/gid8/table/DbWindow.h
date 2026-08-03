#pragma once

#include "qevent.h"
#include "qheaderview.h"
#include "qpushbutton.h"
#include <QDialog>
#include <QTableView>
#include <QSqlQueryModel>
#include <QItemDelegate>
#include <QEvent>
#include <QAbstractItemView>
#include <QAbstractItemDelegate>

#include <vector>
#include <set>

class QSqlTableModel;
class QSqlQueryModel;
class QTableView;
class QDataWidgetMapper;

class GidWidget;
class MyMain;

enum MOVE_TO {
  move_to_unknown = 0,
  move_to_node = 2,
  move_to_line = 3,
  move_to_line_big = 4,
  move_to_geo = 5,
  move_to_node_out = 7,
  move_to_line_out = 8
};


#include <QStyledItemDelegate>
#include <QtGui>


class CustomHeaderView : public QHeaderView {
    Q_OBJECT

public:
    CustomHeaderView(Qt::Orientation orientation, const QString &dbName, const QString &tableName, std::vector<QString> &old_names, QWidget *parent = nullptr)
        : QHeaderView(orientation, parent), m_tableName(tableName), m_dbName(dbName), m_old_names(old_names) {
        setMouseTracking(true);  // Включаем отслеживание движения мыши
    }

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override;
    QSize sectionSizeFromContents(int logicalIndex) const override;
    void mouseMoveEvent(QMouseEvent *event) override;

    void enterEvent(QEnterEvent  *event)  {
        qDebug() << "Mouse entered header";
        QHeaderView::enterEvent(event);
    }

    void leaveEvent(QEvent  *event)  {
        qDebug() << "Mouse left header";
        QHeaderView::leaveEvent(event);
    }
private:
    QString m_dbName; 
    QString m_tableName;  // Имя таблицы
    std::vector<QString> m_old_names;
};

class MyHeaderModel : public QAbstractItemModel
{
public:
    MyHeaderModel(QObject *parent = 0) : QAbstractItemModel(parent) {}
    int columnCount(const QModelIndex &parent = QModelIndex()) const { return 2; }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const { return QVariant(); }
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const { return QModelIndex(); }
    QModelIndex parent(const QModelIndex &index) const { return QModelIndex(); }
    int rowCount(const QModelIndex &parent = QModelIndex()) const { return 0; }
};

#if 0

class MyHeader : public QHeaderView
{
    Q_OBJECT
public:
    MyHeader(QHeaderView *header, QWidget *parent = 0) : QHeaderView(Qt::Horizontal, header), mainHeader(header)
    {
        setModel(new MyHeaderModel(this));
        // This example uses hardcoded groups, you can extend
        // this yourself to save the groups
        // Group 1 is 0-2 and Group 2 is 3-4
        resizeSection(0, getSectionSizes(0, 2));
        resizeSection(1, getSectionSizes(3, 4));
        connect(this, SIGNAL(sectionResized(int,int,int)), this, SLOT(updateSizes()));
        connect(((QTableWidget *)(mainHeader->parentWidget()))->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(updateOffset()));
        setGeometry(0, 0, header->width(), header->height());
        updateOffset();
        mainHeader->installEventFilter(this);
    }
public slots:
    void updateSizes()
    {
        setOffset(mainHeader->offset());
        mainHeader->resizeSection(2, mainHeader->sectionSize(2) + (sectionSize(0) - getSectionSizes(0, 2)));
        mainHeader->resizeSection(4, mainHeader->sectionSize(4) + (sectionSize(1) - getSectionSizes(3, 4)));
    }
    void updateOffset()
    {
        setOffset(mainHeader->offset());
    }
protected:
    bool eventFilter(QObject *o, QEvent *e)
    {
        if (o == mainHeader) {
            if (e->type() == QEvent::Resize) {
                setOffset(mainHeader->offset());
                setGeometry(0, 0, mainHeader->width(), mainHeader->height());
            }
            return false;
        }
        return QHeaderView::eventFilter(o, e);
    }
private:
    int getSectionSizes(int first, int second)
    {
        int size = 0;
        for (int a=first;a<=second;++a)
            size += mainHeader->sectionSize(a);
        return size;
    }
    QHeaderView *mainHeader;

};
#endif

// Создаём кастомный делегат
class HoverDelegate : public QStyledItemDelegate {
public:
    HoverDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent), hoverRow(-1) {}

    void setHoverRow(int row) {
        hoverRow = row;
    }

    int getHoverRow() const {
        return hoverRow;
    }

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        QStyleOptionViewItem opt = option;
        
        // Если строка совпадает с наведённой, меняем цвет фона
        if (index.row() == hoverRow) {
            opt.backgroundBrush = QBrush(Qt::lightGray);  // Задаём цвет строки при наведении
        }

        QStyledItemDelegate::paint(painter, opt, index);
    }

private:
    void drawMultiLineHeader(QPainter *painter, const QRect &rect, const QString &line1, const QString &line2, const QString &line3) const;

    int hoverRow;
};

class ButtonDelegate : public QItemDelegate {
    Q_OBJECT
public:
    ButtonDelegate(QObject *parent = nullptr) : QItemDelegate(parent) {}

    // Рисуем кнопку в ячейке
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    // Обработка клика по кнопке
    bool editorEvent(QEvent *event, QAbstractItemModel  *model, const QStyleOptionViewItem &option, const QModelIndex &index);
};


class DbWindow : public QTableView
{
    Q_OBJECT

public:
    DbWindow();

    void setMoveTo(MOVE_TO _move_to) {move_to = _move_to;};
    void setAdd(bool yes) { m_add = yes;};
    void setEdit(bool yes) { m_edit = yes;};
    void setDelete(bool yes) { m_delete = yes;};
    void setOpres(const QString &opres) { m_opres = opres;};
    void setDop(const QString &dop) { m_dop = dop;};

    void reset_q();
    void reset_q(const QString & q);
    
    void setGeo(const QString & _tn) {move_to = move_to_geo; this->tn = _tn; };

    void setGidWidget(GidWidget *_gid) { gid = _gid;};
    void init(QSqlDatabase &db, const QString & tn, const QString & q, const QString & title);

    void set_hide(std::set<int> &h);

    QString tn;

    void createToolBar(MyMain *main_window);

signals:
    void dataChanged();      // ← наш кастомный сигнал

public slots:
    void OnMoveTo();
    void OnExcel();
    void OnHide();
    void OnHide2();
    void OnAll();
    void OnAdd();
    void OnEdit();
    void OnDelete();
    void OnOpres();
    void OnFind();
    void OnFindAll();
    void onPropertyAccepted();
    void onPropertyRejected();

    void findText(const QString & text);

protected:
    bool save_excel(const QString & filename);
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void hideEvent(QHideEvent *event) override;


    void saveSettings();

    // Это для выделения строки
    void mouseMoveEvent(QMouseEvent* event) override {
        int row = indexAt(event->pos()).row();
        if (row != hoverDelegate->getHoverRow()) {
            hoverDelegate->setHoverRow(row);
            viewport()->update();  // Обновляем представление, чтобы перерисовать строку
        }
        QTableView::mouseMoveEvent(event);
    }

    void leaveEvent(QEvent* event) override {
        hoverDelegate->setHoverRow(-1);  // Сбрасываем выделение строки при уходе курсора
        viewport()->update();
        QTableView::leaveEvent(event);
    }

private slots:
    void closeTable();
//    void helpClicked();

private:
    bool m_add = false;
    bool m_edit = false;
    bool m_delete = false;
    int m_id = -1;
    QString m_title;
    QString m_opres = "";
    QString m_dop = "";

    int m_find_col = 0;

    MOVE_TO move_to = move_to_unknown;
    std::vector<QString> v_col;
//    QString tn;
    GidWidget *gid = nullptr;
    QSqlDatabase *m_db = nullptr;
    std::vector<QString> old_names;
//    QSqlTableModel *model;
//    QTableView *view;

    bool closed = true;

    HoverDelegate* hoverDelegate;
};


DbWindow *getTableView(QSqlDatabase &db, const QString & tn, const QString & q, const QString & title);

// Подбор ширины столбцов таблиц — общий для DbWindow и MultiHeaderTable.
//
// kWidthSampleRows: сколько строк обмерять при подборе. По умолчанию Qt
// меряет ВСЕ строки модели; на паспортных таблицах со 151 столбцом
// (heatpipesections) это и создавало задержку при открытии.
//
// Границы нужны, потому что подбор по содержимому даёт крайности:
// столбец с одним символом сжимается до нечитаемого, а столбец с
// длинным примечанием растягивается на пол-экрана и уводит соседние за
// край окна — на это и жаловались как на «неудобную ширину».
inline constexpr int kWidthSampleRows = 50;
inline constexpr int kMinColWidth = 60;
inline constexpr int kMaxColWidth = 320;
