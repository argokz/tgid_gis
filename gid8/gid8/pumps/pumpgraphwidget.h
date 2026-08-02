#ifndef PUMPGRAPHWIDGET_H
#define PUMPGRAPHWIDGET_H
#include "pumpgraphtemplate.h"
#include <QWidget>
#include <QPainter>

class PumpGraphWidget : public QWidget {
    Q_OBJECT

public:
    explicit PumpGraphWidget(QWidget* parent, QSqlQuery &query, int id);
    explicit PumpGraphWidget(QWidget* parent, QSqlQuery &query, int id, GRAPHR_PARAM grApr, int k_kol, bool isApproximate);


public:
    PumpGraphTemplate pumpGraph;


public:
    bool isCalcMode = false;
    bool approximateMode = true;
    bool isReportMode = false;

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // PUMPGRAPHWIDGET_H
