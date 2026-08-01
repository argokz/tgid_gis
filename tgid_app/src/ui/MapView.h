#pragma once

#include "repo/MapRepository.h"

#include <QHash>
#include <QPainterPath>
#include <QPoint>
#include <QSet>
#include <QWidget>

class QMouseEvent;
class QPaintEvent;
class QResizeEvent;
class QWheelEvent;

namespace tgid::ui {

struct SelectedMapObject final {
    qint64 id = 0;
    QString classTable;
    bool isNode = false;
};

class MapView final : public QWidget {
    Q_OBJECT

public:
    explicit MapView(QWidget* parent = nullptr);

    void setMapData(repo::MapData mapData);
    void clearMap();
    void setPointCreationMode(bool enabled);
    void setLineCreationMode(bool enabled);
    void setLineSplitMode(bool enabled);
    void setNodeMoveMode(
        bool enabled, qint64 nodeId = 0,
        const QString& classTable = {});
    void setLineJoinMode(
        bool enabled, qint64 firstId = 0,
        const QString& firstClassTable = {});
    [[nodiscard]] bool hasSelectedLine(
        qint64 id, const QString& classTable) const;
    [[nodiscard]] bool hasSelectedNode(
        qint64 id, const QString& classTable) const;
    [[nodiscard]] QList<SelectedMapObject> selectedObjects() const;

public slots:
    void fitToData();

signals:
    void objectSelected(qint64 id, QString classTable, bool isNode);
    void objectSelectionCleared();
    void multipleObjectsSelected(int count, QString classTable, bool isNode);
    void selectionLimitReached();
    void pointPlacementRequested(QPointF position);
    void lineStartSelected(qint64 id);
    void linePlacementRequested(qint64 nodeFrom, qint64 nodeTo);
    void lineEndpointMissed();
    void lineSplitRequested(QPointF position);
    void nodeMoveRequested(QPointF position);
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
    void selectObjectAt(
        const QPointF& screenPosition,
        Qt::KeyboardModifiers modifiers);
    void selectLineEndpointAt(const QPointF& screenPosition);
    void selectJoinLineAt(const QPointF& screenPosition);
    void updateObjectSelection(
        qint64 id, const QString& classTable, bool isNode, bool extend);
    [[nodiscard]] bool isObjectSelected(
        qint64 id, const QString& classTable, bool isNode) const;

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
    QList<SelectedMapObject> selectedObjects_;
    QSet<qint64> selectedObjectIds_;
    bool pointCreationMode_ = false;
    bool lineCreationMode_ = false;
    bool lineSplitMode_ = false;
    bool nodeMoveMode_ = false;
    bool lineJoinMode_ = false;
    qint64 lineStartNodeId_ = 0;
    qint64 joinFirstLineId_ = 0;
    QString joinFirstClassTable_;
};

}  // namespace tgid::ui
