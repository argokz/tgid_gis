#pragma once

#include "repo/MapRepository.h"

#include <QHash>
#include <QPainterPath>
#include <QPoint>
#include <QWidget>

class QMouseEvent;
class QPaintEvent;
class QResizeEvent;
class QWheelEvent;

namespace tgid::ui {

class MapView final : public QWidget {
    Q_OBJECT

public:
    explicit MapView(QWidget* parent = nullptr);

    void setMapData(repo::MapData mapData);
    void clearMap();
    void setPointCreationMode(bool enabled);
    void setLineCreationMode(bool enabled);
    void setLineSplitMode(bool enabled);
    void setLineJoinMode(
        bool enabled, qint64 firstId = 0,
        const QString& firstClassTable = {});
    [[nodiscard]] bool hasSelectedLine(
        qint64 id, const QString& classTable) const;

public slots:
    void fitToData();

signals:
    void objectSelected(qint64 id, QString classTable, bool isNode);
    void objectSelectionCleared();
    void pointPlacementRequested(QPointF position);
    void lineStartSelected(qint64 id);
    void linePlacementRequested(qint64 nodeFrom, qint64 nodeTo);
    void lineEndpointMissed();
    void lineSplitRequested(QPointF position);
    void lineJoinRequested(qint64 secondId, QString classTable);
    void lineJoinMissed();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    [[nodiscard]] QPointF worldToScreen(const QPointF& point) const;
    [[nodiscard]] QPointF screenToWorld(const QPointF& point) const;
    void rebuildGeometry();
    void selectObjectAt(const QPointF& screenPosition);
    void selectLineEndpointAt(const QPointF& screenPosition);
    void selectJoinLineAt(const QPointF& screenPosition);

    repo::MapData data_;
    QHash<QString, QPainterPath> linePaths_;
    QRectF worldBounds_;
    QPointF viewCenter_;
    double pixelsPerUnit_ = 1.0;
    bool hasGeometry_ = false;
    bool panning_ = false;
    bool dragging_ = false;
    QPoint pressMousePosition_;
    QPoint lastMousePosition_;
    qint64 selectedId_ = 0;
    QString selectedClassTable_;
    bool selectedIsNode_ = false;
    bool pointCreationMode_ = false;
    bool lineCreationMode_ = false;
    bool lineSplitMode_ = false;
    bool lineJoinMode_ = false;
    qint64 lineStartNodeId_ = 0;
    qint64 joinFirstLineId_ = 0;
    QString joinFirstClassTable_;
};

}  // namespace tgid::ui
