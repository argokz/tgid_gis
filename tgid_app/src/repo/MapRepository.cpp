#include "repo/MapRepository.h"

#include "db/DatabaseConnection.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlError>
#include <QSqlQuery>

namespace tgid::repo {
namespace {

QVector<QPointF> parseLineString(const QByteArray& geoJson, QString* error)
{
    QJsonParseError parseError;
    const QJsonDocument document =
        QJsonDocument::fromJson(geoJson, &parseError);
    if (parseError.error != QJsonParseError::NoError
        || !document.isObject()) {
        if (error != nullptr) {
            *error = QStringLiteral("Некорректный GeoJSON: %1")
                         .arg(parseError.errorString());
        }
        return {};
    }

    const QJsonObject object = document.object();
    if (object.value(QStringLiteral("type")).toString()
        != QStringLiteral("LineString")) {
        if (error != nullptr) {
            *error = QStringLiteral("Ожидался LineString");
        }
        return {};
    }

    QVector<QPointF> points;
    const QJsonArray coordinates =
        object.value(QStringLiteral("coordinates")).toArray();
    points.reserve(coordinates.size());
    for (const QJsonValue& coordinateValue : coordinates) {
        const QJsonArray coordinate = coordinateValue.toArray();
        if (coordinate.size() < 2) {
            continue;
        }
        points.append(
            QPointF(coordinate.at(0).toDouble(), coordinate.at(1).toDouble()));
    }
    if (points.size() < 2 && error != nullptr) {
        *error = QStringLiteral("В линии меньше двух точек");
    }
    return points;
}

}  // namespace

MapData MapRepository::loadFragment(
    const db::DatabaseConfig& config,
    int fragmentId) const
{
    MapData result;
    result.fragmentId = fragmentId;

    db::DatabaseConnection connection;
    if (!connection.open(config, &result.error)) {
        return result;
    }

    QSqlQuery nodeQuery(connection.database());
    nodeQuery.setForwardOnly(true);
    nodeQuery.prepare(QStringLiteral(
        "SELECT id2, class_table, coalesce(type_txt, ''),"
        "       coalesce(nullif(nodename, ''),"
        "                nullif(externalnodename, ''), ''),"
        "       ST_X(geom), ST_Y(geom)"
        "  FROM net.v_map_nodes"
        " WHERE fileid = :fragment_id"
        " ORDER BY id2"));
    nodeQuery.bindValue(QStringLiteral(":fragment_id"), fragmentId);
    if (!nodeQuery.exec()) {
        result.error = nodeQuery.lastError().text();
        return result;
    }

    while (nodeQuery.next()) {
        MapNode node;
        node.id = nodeQuery.value(0).toLongLong();
        node.classTable = nodeQuery.value(1).toString();
        node.type = nodeQuery.value(2).toString();
        node.label = nodeQuery.value(3).toString();
        node.position =
            QPointF(nodeQuery.value(4).toDouble(), nodeQuery.value(5).toDouble());
        result.nodes.append(std::move(node));
    }

    QSqlQuery lineQuery(connection.database());
    lineQuery.setForwardOnly(true);
    lineQuery.prepare(QStringLiteral(
        "SELECT id2, class_table, coalesce(type_txt, ''),"
        "       ST_AsGeoJSON(geom, 3, 0)"
        "  FROM net.v_map_lines"
        " WHERE fileid = :fragment_id"
        " ORDER BY id2"));
    lineQuery.bindValue(QStringLiteral(":fragment_id"), fragmentId);
    if (!lineQuery.exec()) {
        result.error = lineQuery.lastError().text();
        return result;
    }

    while (lineQuery.next()) {
        MapLine line;
        line.id = lineQuery.value(0).toLongLong();
        line.classTable = lineQuery.value(1).toString();
        line.type = lineQuery.value(2).toString();
        QString geometryError;
        line.points =
            parseLineString(lineQuery.value(3).toByteArray(), &geometryError);
        if (!geometryError.isEmpty()) {
            result.error =
                QStringLiteral("Объект %1 (%2): %3")
                    .arg(line.id)
                    .arg(line.classTable, geometryError);
            return result;
        }
        result.lines.append(std::move(line));
    }

    return result;
}

}  // namespace tgid::repo
