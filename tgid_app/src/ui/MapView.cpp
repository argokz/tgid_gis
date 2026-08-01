#include "ui/MapView.h"

#include <QColor>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QWheelEvent>

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

namespace tgid::ui {
namespace {

QColor lineColor(const QString& classTable)
{
    if (classTable == QStringLiteral("pipe_section")) {
        return QColor(QStringLiteral("#3478c9"));
    }
    if (classTable == QStringLiteral("damper")) {
        return QColor(QStringLiteral("#e68619"));
    }
    if (classTable == QStringLiteral("pump")) {
        return QColor(QStringLiteral("#8b5cf6"));
    }
    if (classTable == QStringLiteral("regulator_press")) {
        return QColor(QStringLiteral("#d946ef"));
    }
    return QColor(QStringLiteral("#64748b"));
}

QColor nodeColor(const QString& classTable)
{
    if (classTable == QStringLiteral("consumer_real")) {
        return QColor(QStringLiteral("#16a34a"));
    }
    if (classTable == QStringLiteral("consumer_general")) {
        return QColor(QStringLiteral("#65a30d"));
    }
    if (classTable == QStringLiteral("heat_source")) {
        return QColor(QStringLiteral("#dc2626"));
    }
    if (classTable == QStringLiteral("pump_station")) {
        return QColor(QStringLiteral("#7c3aed"));
    }
    if (classTable == QStringLiteral("heat_chamber")) {
        return QColor(QStringLiteral("#0891b2"));
    }
    return QColor(QStringLiteral("#0f172a"));
}

double squaredDistanceToSegment(
    const QPointF& point,
    const QPointF& segmentStart,
    const QPointF& segmentEnd)
{
    const QPointF segment = segmentEnd - segmentStart;
    const double lengthSquared =
        segment.x() * segment.x() + segment.y() * segment.y();
    if (lengthSquared <= 0.000001) {
        const QPointF delta = point - segmentStart;
        return delta.x() * delta.x() + delta.y() * delta.y();
    }

    const QPointF fromStart = point - segmentStart;
    const double projection =
        std::clamp((fromStart.x() * segment.x()
                    + fromStart.y() * segment.y())
                       / lengthSquared,
                   0.0, 1.0);
    const QPointF closest = segmentStart + projection * segment;
    const QPointF delta = point - closest;
    return delta.x() * delta.x() + delta.y() * delta.y();
}

}  // namespace

MapView::MapView(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(420, 300);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::OpenHandCursor);
}

void MapView::setMapData(repo::MapData mapData)
{
    data_ = std::move(mapData);
    selectedId_ = 0;
    selectedClassTable_.clear();
    selectedIsNode_ = false;
    selectedObjects_.clear();
    selectedObjectIds_.clear();
    rebuildGeometry();
    fitToData();
}

void MapView::clearMap()
{
    data_ = {};
    linePaths_.clear();
    worldBounds_ = {};
    hasGeometry_ = false;
    selectedId_ = 0;
    selectedClassTable_.clear();
    selectedIsNode_ = false;
    selectedObjects_.clear();
    selectedObjectIds_.clear();
    pointCreationMode_ = false;
    lineCreationMode_ = false;
    lineSplitMode_ = false;
    lineJoinMode_ = false;
    lineStartNodeId_ = 0;
    joinFirstLineId_ = 0;
    joinFirstClassTable_.clear();
    setCursor(Qt::OpenHandCursor);
    update();
}

void MapView::setPointCreationMode(bool enabled)
{
    pointCreationMode_ = enabled && hasGeometry_;
    if (pointCreationMode_) {
        lineCreationMode_ = false;
        lineSplitMode_ = false;
        lineJoinMode_ = false;
        lineStartNodeId_ = 0;
    }
    panning_ = false;
    dragging_ = false;
    setCursor(pointCreationMode_ ? Qt::CrossCursor : Qt::OpenHandCursor);
    update();
}

void MapView::setLineCreationMode(bool enabled)
{
    lineCreationMode_ = enabled && hasGeometry_;
    if (lineCreationMode_) {
        pointCreationMode_ = false;
        lineSplitMode_ = false;
        lineJoinMode_ = false;
    }
    lineStartNodeId_ = 0;
    panning_ = false;
    dragging_ = false;
    setCursor(lineCreationMode_ ? Qt::CrossCursor : Qt::OpenHandCursor);
    update();
}

void MapView::setLineSplitMode(bool enabled)
{
    lineSplitMode_ = enabled && hasGeometry_
                     && selectedId_ != 0 && !selectedIsNode_;
    if (lineSplitMode_) {
        pointCreationMode_ = false;
        lineCreationMode_ = false;
        lineJoinMode_ = false;
        lineStartNodeId_ = 0;
    }
    panning_ = false;
    dragging_ = false;
    setCursor(lineSplitMode_ ? Qt::CrossCursor : Qt::OpenHandCursor);
    update();
}

void MapView::setLineJoinMode(
    bool enabled, qint64 firstId,
    const QString& firstClassTable)
{
    lineJoinMode_ = enabled && hasGeometry_
                    && selectedId_ == firstId && firstId != 0
                    && !selectedIsNode_
                    && selectedClassTable_ == firstClassTable;
    if (lineJoinMode_) {
        pointCreationMode_ = false;
        lineCreationMode_ = false;
        lineSplitMode_ = false;
        lineStartNodeId_ = 0;
        joinFirstLineId_ = firstId;
        joinFirstClassTable_ = firstClassTable;
    } else {
        joinFirstLineId_ = 0;
        joinFirstClassTable_.clear();
    }
    panning_ = false;
    dragging_ = false;
    setCursor(lineJoinMode_ ? Qt::CrossCursor : Qt::OpenHandCursor);
    update();
}

bool MapView::hasSelectedLine(qint64 id, const QString& classTable) const
{
    return selectedObjects_.size() == 1
           && selectedId_ == id && selectedClassTable_ == classTable
           && !selectedIsNode_;
}

QList<SelectedMapObject> MapView::selectedObjects() const
{
    return selectedObjects_;
}

void MapView::fitToData()
{
    if (!hasGeometry_) {
        update();
        return;
    }

    viewCenter_ = worldBounds_.center();
    const double width = std::max(worldBounds_.width(), 1.0);
    const double height = std::max(worldBounds_.height(), 1.0);
    const double availableWidth = std::max(this->width() - 48, 1);
    const double availableHeight = std::max(this->height() - 48, 1);
    pixelsPerUnit_ =
        std::clamp(std::min(availableWidth / width, availableHeight / height),
                   0.000001, 1000000.0);
    update();
}

void MapView::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(QStringLiteral("#f8fafc")));
    painter.setRenderHint(QPainter::Antialiasing, true);

    if (!hasGeometry_) {
        painter.setPen(QColor(QStringLiteral("#64748b")));
        painter.drawText(
            rect(), Qt::AlignCenter,
            QStringLiteral("Выберите фрагмент для загрузки схемы"));
        return;
    }

    QTransform worldTransform;
    worldTransform.translate(width() / 2.0, height() / 2.0);
    worldTransform.scale(pixelsPerUnit_, -pixelsPerUnit_);
    worldTransform.translate(-viewCenter_.x(), -viewCenter_.y());
    painter.setWorldTransform(worldTransform);

    for (auto it = linePaths_.constBegin(); it != linePaths_.constEnd(); ++it) {
        QPen pen(lineColor(it.key()), 1.35);
        pen.setCosmetic(true);
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(it.value());
    }

    for (const repo::MapLine& line : data_.lines) {
        if (line.points.isEmpty()
            || !isObjectSelected(line.id, line.classTable, false)) {
            continue;
        }
        QPainterPath selectedPath(line.points.first());
        for (qsizetype pointIndex = 1;
             pointIndex < line.points.size();
             ++pointIndex) {
            selectedPath.lineTo(line.points.at(pointIndex));
        }
        QPen selectionPen(QColor(QStringLiteral("#facc15")), 4.0);
        selectionPen.setCosmetic(true);
        selectionPen.setCapStyle(Qt::RoundCap);
        selectionPen.setJoinStyle(Qt::RoundJoin);
        painter.setPen(selectionPen);
        painter.drawPath(selectedPath);
    }

    painter.resetTransform();
    painter.setPen(QPen(QColor(QStringLiteral("#ffffff")), 0.8));
    for (const repo::MapNode& node : data_.nodes) {
        painter.setBrush(nodeColor(node.classTable));
        const QPointF screenPoint = worldToScreen(node.position);
        painter.drawEllipse(screenPoint, 3.2, 3.2);
    }

    for (const repo::MapNode& node : data_.nodes) {
        if (!isObjectSelected(node.id, node.classTable, true)) {
            continue;
        }
        const QPointF screenPoint = worldToScreen(node.position);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(QColor(QStringLiteral("#facc15")), 3.0));
        painter.drawEllipse(screenPoint, 7.0, 7.0);
    }

    if (lineCreationMode_ && lineStartNodeId_ != 0) {
        for (const repo::MapNode& node : data_.nodes) {
            if (node.id != lineStartNodeId_) {
                continue;
            }
            const QPointF screenPoint = worldToScreen(node.position);
            painter.setBrush(Qt::NoBrush);
            painter.setPen(QPen(QColor(QStringLiteral("#e11d48")), 3.0));
            painter.drawEllipse(screenPoint, 9.0, 9.0);
            break;
        }
    }

    painter.setPen(QColor(QStringLiteral("#475569")));
    painter.drawText(
        QRect(12, 10, width() - 24, 24),
        Qt::AlignLeft | Qt::AlignVCenter,
        QStringLiteral("Узлов: %1   Линий: %2")
            .arg(data_.nodes.size())
            .arg(data_.lines.size()));
    if (pointCreationMode_) {
        painter.setPen(QColor(QStringLiteral("#b42318")));
        painter.drawText(
            QRect(12, 36, width() - 24, 24),
            Qt::AlignLeft | Qt::AlignVCenter,
            QStringLiteral(
                "СОЗДАНИЕ: щёлкните место нового точечного объекта"));
    } else if (lineCreationMode_) {
        painter.setPen(QColor(QStringLiteral("#b42318")));
        painter.drawText(
            QRect(12, 36, width() - 24, 24),
            Qt::AlignLeft | Qt::AlignVCenter,
            lineStartNodeId_ == 0
                ? QStringLiteral(
                      "СОЗДАНИЕ ЛИНИИ: выберите начальный узел")
                : QStringLiteral(
                      "СОЗДАНИЕ ЛИНИИ: выберите конечный узел"));
    } else if (lineSplitMode_) {
        painter.setPen(QColor(QStringLiteral("#b42318")));
        painter.drawText(
            QRect(12, 36, width() - 24, 24),
            Qt::AlignLeft | Qt::AlignVCenter,
            QStringLiteral(
                "РАЗРЕЗАНИЕ: укажите точку на выбранной линии"));
    } else if (lineJoinMode_) {
        painter.setPen(QColor(QStringLiteral("#b42318")));
        painter.drawText(
            QRect(12, 36, width() - 24, 24),
            Qt::AlignLeft | Qt::AlignVCenter,
            QStringLiteral(
                "СОЕДИНЕНИЕ: выберите второй участок"));
    }
}

void MapView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton
        && (pointCreationMode_ || lineCreationMode_ || lineSplitMode_
            || lineJoinMode_)) {
        event->accept();
        return;
    }
    if (event->button() == Qt::LeftButton && hasGeometry_) {
        panning_ = true;
        dragging_ = false;
        pressMousePosition_ = event->position().toPoint();
        lastMousePosition_ = pressMousePosition_;
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void MapView::mouseMoveEvent(QMouseEvent* event)
{
    if (panning_) {
        const QPoint currentPosition = event->position().toPoint();
        if (!dragging_
            && (currentPosition - pressMousePosition_).manhattanLength() <= 3) {
            event->accept();
            return;
        }
        dragging_ = true;
        setCursor(Qt::ClosedHandCursor);
        const QPoint delta = currentPosition - lastMousePosition_;
        lastMousePosition_ = currentPosition;
        viewCenter_.rx() -= delta.x() / pixelsPerUnit_;
        viewCenter_.ry() += delta.y() / pixelsPerUnit_;
        update();
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void MapView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && pointCreationMode_) {
        emit pointPlacementRequested(screenToWorld(event->position()));
        event->accept();
        return;
    }
    if (event->button() == Qt::LeftButton && lineCreationMode_) {
        selectLineEndpointAt(event->position());
        event->accept();
        return;
    }
    if (event->button() == Qt::LeftButton && lineSplitMode_) {
        emit lineSplitRequested(screenToWorld(event->position()));
        event->accept();
        return;
    }
    if (event->button() == Qt::LeftButton && lineJoinMode_) {
        selectJoinLineAt(event->position());
        event->accept();
        return;
    }
    if (event->button() == Qt::LeftButton && panning_) {
        const bool wasDragging = dragging_;
        panning_ = false;
        dragging_ = false;
        setCursor(Qt::OpenHandCursor);
        if (!wasDragging) {
            selectObjectAt(event->position(), event->modifiers());
        }
        event->accept();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void MapView::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (pointCreationMode_ || lineCreationMode_ || lineSplitMode_
        || lineJoinMode_) {
        event->accept();
        return;
    }
    if (event->button() == Qt::LeftButton) {
        fitToData();
        event->accept();
        return;
    }
    QWidget::mouseDoubleClickEvent(event);
}

void MapView::selectLineEndpointAt(const QPointF& screenPosition)
{
    constexpr double nodeToleranceSquared = 10.0 * 10.0;
    const repo::MapNode* nearestNode = nullptr;
    double nearestDistance = nodeToleranceSquared;
    for (const repo::MapNode& node : data_.nodes) {
        const QPointF delta = screenPosition - worldToScreen(node.position);
        const double distance =
            delta.x() * delta.x() + delta.y() * delta.y();
        if (distance <= nearestDistance) {
            nearestDistance = distance;
            nearestNode = &node;
        }
    }
    if (nearestNode == nullptr) {
        emit lineEndpointMissed();
        return;
    }

    if (lineStartNodeId_ == 0) {
        lineStartNodeId_ = nearestNode->id;
        emit lineStartSelected(lineStartNodeId_);
        update();
        return;
    }
    if (nearestNode->id == lineStartNodeId_) {
        emit lineEndpointMissed();
        return;
    }

    const qint64 nodeFrom = lineStartNodeId_;
    const qint64 nodeTo = nearestNode->id;
    lineStartNodeId_ = 0;
    update();
    emit linePlacementRequested(nodeFrom, nodeTo);
}

void MapView::selectJoinLineAt(const QPointF& screenPosition)
{
    constexpr double lineToleranceSquared = 7.0 * 7.0;
    const repo::MapLine* nearestLine = nullptr;
    double nearestDistance = lineToleranceSquared;
    for (const repo::MapLine& line : data_.lines) {
        if ((line.id == joinFirstLineId_
             && line.classTable == joinFirstClassTable_)
            || line.points.size() < 2) {
            continue;
        }
        for (qsizetype pointIndex = 1;
             pointIndex < line.points.size(); ++pointIndex) {
            const double distance = squaredDistanceToSegment(
                screenPosition,
                worldToScreen(line.points.at(pointIndex - 1)),
                worldToScreen(line.points.at(pointIndex)));
            if (distance <= nearestDistance) {
                nearestDistance = distance;
                nearestLine = &line;
            }
        }
    }
    if (nearestLine == nullptr) {
        emit lineJoinMissed();
        return;
    }
    emit lineJoinRequested(nearestLine->id, nearestLine->classTable);
}

void MapView::wheelEvent(QWheelEvent* event)
{
    if (!hasGeometry_) {
        QWidget::wheelEvent(event);
        return;
    }

    const QPointF mousePosition = event->position();
    const QPointF anchorWorld = screenToWorld(mousePosition);
    const double steps = event->angleDelta().y() / 120.0;
    const double factor = std::pow(1.2, steps);
    pixelsPerUnit_ =
        std::clamp(pixelsPerUnit_ * factor, 0.000001, 1000000.0);

    viewCenter_.setX(
        anchorWorld.x() - (mousePosition.x() - width() / 2.0) / pixelsPerUnit_);
    viewCenter_.setY(
        anchorWorld.y() + (mousePosition.y() - height() / 2.0) / pixelsPerUnit_);
    update();
    event->accept();
}

QPointF MapView::worldToScreen(const QPointF& point) const
{
    return {
        (point.x() - viewCenter_.x()) * pixelsPerUnit_ + width() / 2.0,
        (viewCenter_.y() - point.y()) * pixelsPerUnit_ + height() / 2.0,
    };
}

QPointF MapView::screenToWorld(const QPointF& point) const
{
    return {
        viewCenter_.x() + (point.x() - width() / 2.0) / pixelsPerUnit_,
        viewCenter_.y() - (point.y() - height() / 2.0) / pixelsPerUnit_,
    };
}

void MapView::rebuildGeometry()
{
    linePaths_.clear();

    double minimumX = std::numeric_limits<double>::max();
    double minimumY = std::numeric_limits<double>::max();
    double maximumX = std::numeric_limits<double>::lowest();
    double maximumY = std::numeric_limits<double>::lowest();
    hasGeometry_ = false;

    const auto includePoint = [&](const QPointF& point) {
        minimumX = std::min(minimumX, point.x());
        minimumY = std::min(minimumY, point.y());
        maximumX = std::max(maximumX, point.x());
        maximumY = std::max(maximumY, point.y());
        hasGeometry_ = true;
    };

    for (const repo::MapLine& line : data_.lines) {
        if (line.points.isEmpty()) {
            continue;
        }
        QPainterPath& path = linePaths_[line.classTable];
        path.moveTo(line.points.first());
        includePoint(line.points.first());
        for (qsizetype pointIndex = 1;
             pointIndex < line.points.size();
             ++pointIndex) {
            path.lineTo(line.points.at(pointIndex));
            includePoint(line.points.at(pointIndex));
        }
    }

    for (const repo::MapNode& node : data_.nodes) {
        includePoint(node.position);
    }

    if (hasGeometry_) {
        worldBounds_ =
            QRectF(QPointF(minimumX, minimumY), QPointF(maximumX, maximumY))
                .normalized();
    } else {
        worldBounds_ = {};
    }
}

void MapView::selectObjectAt(
    const QPointF& screenPosition,
    Qt::KeyboardModifiers modifiers)
{
    constexpr double nodeToleranceSquared = 9.0 * 9.0;
    constexpr double lineToleranceSquared = 7.0 * 7.0;

    const repo::MapNode* nearestNode = nullptr;
    double nearestNodeDistance = nodeToleranceSquared;
    for (const repo::MapNode& node : data_.nodes) {
        const QPointF delta = screenPosition - worldToScreen(node.position);
        const double distance =
            delta.x() * delta.x() + delta.y() * delta.y();
        if (distance <= nearestNodeDistance) {
            nearestNodeDistance = distance;
            nearestNode = &node;
        }
    }

    if (nearestNode != nullptr) {
        updateObjectSelection(
            nearestNode->id,
            nearestNode->classTable,
            true,
            modifiers.testFlag(Qt::ControlModifier));
        return;
    }

    const repo::MapLine* nearestLine = nullptr;
    double nearestLineDistance = lineToleranceSquared;
    for (const repo::MapLine& line : data_.lines) {
        for (qsizetype pointIndex = 1;
             pointIndex < line.points.size();
             ++pointIndex) {
            const double distance = squaredDistanceToSegment(
                screenPosition,
                worldToScreen(line.points.at(pointIndex - 1)),
                worldToScreen(line.points.at(pointIndex)));
            if (distance <= nearestLineDistance) {
                nearestLineDistance = distance;
                nearestLine = &line;
            }
        }
    }

    if (nearestLine != nullptr) {
        updateObjectSelection(
            nearestLine->id,
            nearestLine->classTable,
            false,
            modifiers.testFlag(Qt::ControlModifier));
        return;
    }

    if (modifiers.testFlag(Qt::ControlModifier)) {
        return;
    }

    selectedObjects_.clear();
    selectedObjectIds_.clear();
    selectedId_ = 0;
    selectedClassTable_.clear();
    selectedIsNode_ = false;
    update();
    emit objectSelectionCleared();
}

void MapView::updateObjectSelection(
    qint64 id,
    const QString& classTable,
    bool isNode,
    bool extend)
{
    if (!extend || (!selectedObjects_.isEmpty()
                    && (selectedObjects_.first().classTable != classTable
                        || selectedObjects_.first().isNode != isNode))) {
        selectedObjects_.clear();
        selectedObjectIds_.clear();
    }

    const bool alreadySelected = selectedObjectIds_.contains(id);
    if (extend && !alreadySelected && selectedObjects_.size() >= 500) {
        emit selectionLimitReached();
        return;
    }
    qsizetype existingIndex = -1;
    if (alreadySelected) {
        for (qsizetype index = 0; index < selectedObjects_.size(); ++index) {
            if (selectedObjects_.at(index).id == id) {
                existingIndex = index;
                break;
            }
        }
    }
    if (extend && existingIndex >= 0) {
        selectedObjects_.removeAt(existingIndex);
        selectedObjectIds_.remove(id);
    } else {
        selectedObjects_.append({id, classTable, isNode});
        selectedObjectIds_.insert(id);
    }

    if (selectedObjects_.size() == 1) {
        const SelectedMapObject& selected = selectedObjects_.first();
        selectedId_ = selected.id;
        selectedClassTable_ = selected.classTable;
        selectedIsNode_ = selected.isNode;
        emit objectSelected(
            selectedId_, selectedClassTable_, selectedIsNode_);
    } else {
        selectedId_ = 0;
        selectedClassTable_.clear();
        selectedIsNode_ = false;
        if (selectedObjects_.isEmpty()) {
            emit objectSelectionCleared();
        } else {
            emit multipleObjectsSelected(
                static_cast<int>(selectedObjects_.size()),
                selectedObjects_.first().classTable,
                selectedObjects_.first().isNode);
        }
    }
    update();
}

bool MapView::isObjectSelected(
    qint64 id, const QString& classTable, bool isNode) const
{
    return !selectedObjects_.isEmpty()
           && selectedObjects_.first().classTable == classTable
           && selectedObjects_.first().isNode == isNode
           && selectedObjectIds_.contains(id);
}

}  // namespace tgid::ui
