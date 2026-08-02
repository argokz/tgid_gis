#include "rpdrawerwidget.h"
#include <gidview/GidWidget.h>

RPDrawerWidget::RPDrawerWidget(QWidget *parent)
    : QWidget{parent}
{
    setMinimumSize(100, 100);  // Устанавливаем минимальный размер для виджета
}

void RPDrawerWidget::paintEvent(QPaintEvent * /*event*/)
{

    QPainter painter(this);

    QObject *current = this->parent();
    while (current) {
        if (auto gidWidget = qobject_cast<GidWidget *>(current)) {

            gidWidget->draw_b5(&painter, schemeNumber);

            break;
        }
        current = current->parent();
    }

    if (!current) {
        qDebug() << "Родитель не найден среди GidWidget";
    }

}
