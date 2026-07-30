#pragma once

#include <QList>
#include <QSqlDatabase>
#include <QString>

namespace tgid::repo {

struct LayerInfo final {
    QString schemaName;
    QString tableName;
    QString geometryType;
    int srid = 0;
    QString displayName;
    bool editable = false;
    bool supportsFragment = false;
    QString kind;
};

class LayerCatalogRepository final {
public:
    [[nodiscard]] QList<LayerInfo> load(
        const QSqlDatabase& database,
        QString* error) const;
};

}  // namespace tgid::repo
