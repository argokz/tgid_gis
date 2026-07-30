#pragma once

#include "db/DatabaseConfig.h"

#include <QList>
#include <QPointF>
#include <QString>
#include <QVector>

namespace tgid::repo {

struct MapNode final {
    qint64 id = 0;
    QString classTable;
    QString type;
    QString label;
    QPointF position;
};

struct MapLine final {
    qint64 id = 0;
    QString classTable;
    QString type;
    QVector<QPointF> points;
};

struct MapData final {
    int fragmentId = 0;
    QList<MapNode> nodes;
    QList<MapLine> lines;
    QString error;

    [[nodiscard]] bool isValid() const { return error.isEmpty(); }
};

class MapRepository final {
public:
    [[nodiscard]] MapData loadFragment(
        const db::DatabaseConfig& config,
        int fragmentId) const;
};

}  // namespace tgid::repo
