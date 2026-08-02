#include "MultiHeaderTable.h"
#include "ResizableTableView.h"

ResizableTableView::ResizableTableView(QWidget *parent) : QTableView(parent), resizing(false), resizeColumn(-1) {
    setMouseTracking(true);  // Включаем отслеживание движения мыши
    hoverDelegate = new HoverDelegate(this);
    setItemDelegate(hoverDelegate);
}

void ResizableTableView::mouseMoveEvent(QMouseEvent *event) {
    int row = indexAt(event->pos()).row();
    if (row != hoverDelegate->getHoverRow()) {
        hoverDelegate->setHoverRow(row);
        viewport()->update();
    }

    QTableView::mouseMoveEvent(event);

    if (resizing) {
        int delta = event->pos().x() - resizeStartPos;
        int newWidth = initialColumnWidth + delta;
        if (newWidth > 10) {
            horizontalHeader()->resizeSection(resizeColumn, newWidth);
        }
        return;
    }

    int column = columnAt(event->pos().x());
    if (column >= 0 && !horizontalHeader()->isVisible()) {
        QRect cellRect = visualRect(model()->index(0, column));
        int rightEdge = cellRect.right();
        if (qAbs(event->pos().x() - rightEdge) <= 5) {
            setCursor(Qt::SplitHCursor);
            resizeColumn = column;
        } else {
            setCursor(Qt::ArrowCursor);
            resizeColumn = -1;
        }
    }
}

void ResizableTableView::mousePressEvent(QMouseEvent *event) {
    // Передаём событие родительскому виджету
    if (auto parent = qobject_cast<MultiHeaderTable *>(this->parentWidget())) {
        parent->mousePressEvent(event);
    }

    // Вызов базового класса QTableView
    QTableView::mousePressEvent(event);
}

void ResizableTableView::mouseDoubleClickEvent(QMouseEvent *event) {
    // Передаём событие родительскому виджету
    if (auto parent = qobject_cast<MultiHeaderTable *>(this->parentWidget())) {
        parent->mouseDoubleClickEvent(event);
    }

    // Вызов базового класса QTableView
    QTableView::mouseDoubleClickEvent(event);
}

void ResizableTableView::mouseReleaseEvent(QMouseEvent *event) {
    // Передаём событие родительскому виджету
    if (auto parent = qobject_cast<MultiHeaderTable *>(this->parentWidget())) {
        parent->mousePressEvent(event);
    }

    // Вызов базового класса QTableView
    QTableView::mouseReleaseEvent(event);
}
