#include "repo/LayerCatalogRepository.h"

#include <QSqlError>
#include <QSqlQuery>

namespace tgid::repo {

QList<LayerInfo> LayerCatalogRepository::load(
    const QSqlDatabase& database,
    QString* error) const
{
    QList<LayerInfo> layers;
    QSqlQuery query(database);
    query.setForwardOnly(true);

    if (!query.exec(QStringLiteral(
            "SELECT l.schema_name, l.table_name, l.geometry_type, l.srid,"
            "       l.display_name_ru, l.is_editable,"
            "       EXISTS ("
            "           SELECT 1 FROM information_schema.columns AS c"
            "            WHERE c.table_schema = l.schema_name"
            "              AND c.table_name = l.table_name"
            "              AND c.column_name = 'fragment_id'"
            "       ),"
            "       l.kind"
            "  FROM meta.layer_catalog AS l"
            " ORDER BY l.sort_order, l.display_name_ru, l.table_name"))) {
        if (error != nullptr) {
            *error = query.lastError().text();
        }
        return layers;
    }

    while (query.next()) {
        LayerInfo layer;
        layer.schemaName = query.value(0).toString();
        layer.tableName = query.value(1).toString();
        layer.geometryType = query.value(2).toString();
        layer.srid = query.value(3).toInt();
        layer.displayName = query.value(4).toString();
        layer.editable = query.value(5).toBool();
        layer.supportsFragment = query.value(6).toBool();
        layer.kind = query.value(7).toString();
        layers.append(std::move(layer));
    }
    return layers;
}

}  // namespace tgid::repo
