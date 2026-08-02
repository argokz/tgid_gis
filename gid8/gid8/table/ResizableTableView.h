#ifndef RESIZABLETABLEVIEW_H
#define RESIZABLETABLEVIEW_H

#include <QTableView>
#include <QMouseEvent>
#include <QHeaderView>
#include "DbWindow.h"

class ResizableTableView : public QTableView {
    Q_OBJECT

public:
    HoverDelegate* hoverDelegate;
    bool resizing;
    int resizeColumn;
    int resizeStartPos;
    int initialColumnWidth;
    explicit ResizableTableView(QWidget *parent = nullptr);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent* event) override {
        hoverDelegate->setHoverRow(-1);  // Сбрасываем выделение строки при уходе курсора
        viewport()->update();
        QTableView::leaveEvent(event);
    }

private:

};


#endif // RESIZABLETABLEVIEW_H
