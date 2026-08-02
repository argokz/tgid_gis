#include "pumpgraphwidget.h"

#include "pumpgraphwidget.h"
#include "qevent.h"


PumpGraphWidget::PumpGraphWidget(QWidget* parent, QSqlQuery &query, int id)
    : QWidget(parent) {
    pumpGraph = PumpGraphTemplate();
    GRAPHR_PARAM grp;

    pumpGraph.setStandardPumps(query,grp, id);
    pumpGraph.initData(&grp.gr);
    pumpGraph.apH = APPROX_PARAM();
    pumpGraph.apHr = APPROX_PARAM();
    pumpGraph.apKPD = APPROX_PARAM();
    pumpGraph.apKPDr = APPROX_PARAM();
    pumpGraph.apN = APPROX_PARAM();
    pumpGraph.apNr = APPROX_PARAM();
    pumpGraph.GrApr = new GRAPHR_PARAM(grp);

}

PumpGraphWidget::PumpGraphWidget(QWidget* parent, QSqlQuery &query, int id, GRAPHR_PARAM grApr, int k_kol, bool isApproximate)
    : QWidget(parent){

    isCalcMode =true;
    approximateMode = isApproximate;
    pumpGraph = PumpGraphTemplate();
    pumpGraph.calcMode = true;
    GRAPHR_PARAM grp;

    pumpGraph.setStandardPumps(query,grp, id);

    grp.IsDnom = grApr.IsDnom;
    grp.IsFnom = grApr.IsFnom;

    grp.S = grApr.S;
    grp.Spr = grApr.Spr;

    grp.c_vr = grApr.c_vr;

    grp.diam = grApr.diam;
    grp.gr.k_kol = k_kol;

    if (approximateMode)
        pumpGraph.initData(&grp.gr);
    else
        pumpGraph.initData2(&grp.gr);
    pumpGraph.apH = APPROX_PARAM();
    pumpGraph.apHr = APPROX_PARAM();
    pumpGraph.apKPD = APPROX_PARAM();
    pumpGraph.apKPDr = APPROX_PARAM();
    pumpGraph.apN = APPROX_PARAM();
    pumpGraph.apNr = APPROX_PARAM();
    pumpGraph.GrApr = new GRAPHR_PARAM(grp);
}

void PumpGraphWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) { // Проверяем, что нажата правая кнопка мыши
        QPoint point = event->pos(); // Получаем координаты клика

        if (point.x() < pumpGraph.rX && point.x() > pumpGraph.lX && point.y() < pumpGraph.rY && point.y() > pumpGraph.lY) {
            pumpGraph.p = point;
            pumpGraph.rButtonDown = 1;
            pumpGraph.frst = 1;
            repaint();

            qDebug() << "Правая кнопка нажата в области графика:" << point;
        }
    }

    // Вызываем базовый обработчик (опционально)
    QWidget::mousePressEvent(event);
}

void PumpGraphWidget::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) { // Проверяем, что зажата левая кнопка мыши
        QPoint point = event->pos();

        if (point.x() >= pumpGraph.lX && point.x() <= pumpGraph.rX &&
            point.y() >= pumpGraph.lY && point.y() <= pumpGraph.rY) {

            pumpGraph.p = point; // Обновляем текущую точку
            pumpGraph.showGuidelines = true; // Включаем отображение линий
            repaint();
        }
    }
}

void PumpGraphWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) { // Отпускаем левую кнопку мыши
        pumpGraph.showGuidelines = false; // Выключаем отображение линий
        repaint();
    }
}

void PumpGraphWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    if (isCalcMode)
    {
        if (approximateMode)
            if (isReportMode)
                pumpGraph.DrawContent(painter);
            else
                pumpGraph.DrawCalc(painter);
        else{
            if (isReportMode)
                pumpGraph.DrawContent2(painter);
            else
                pumpGraph.DrawCalc2(painter);
        }
    }
    else
        pumpGraph.Draw(painter);


}

