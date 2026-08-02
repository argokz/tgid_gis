#include "sldrawerwidget.h"
#include <gidview/GidWidget.h>

SLDrawerWidget::SLDrawerWidget(QWidget *parent)
    : QWidget{parent}
{
    setMinimumSize(100, 100);  // Устанавливаем минимальный размер для виджета
}

void SLDrawerWidget::paintEvent(QPaintEvent * /*event*/)
{

    QPainter painter(this);

    QObject *current = this->parent();
    while (current) {
        if (auto gidWidget = qobject_cast<GidWidget *>(current)) {

            gidWidget->draw_example_style_line(&painter, this->mag_ras, this->tubingTypeID, this->open, this->q, this->m_ring);

            break;
        }
        current = current->parent();
    }

    if (!current) {
        qDebug() << "Родитель не найден среди GidWidget";
    }

}
