#pragma once

#include <QList>
#include <QSqlDatabase>
#include <QString>

namespace tgid::repo {

struct ZeroLoadConsumerCriteria final {
    int fragmentId = 0;
    QString searchText;
    int limit = 2000;
};

struct ZeroLoadConsumerRow final {
    qint64 id = 0;
    qint64 sourceId = 0;
    QString classTable;
    int fragmentId = 0;
    QString externalCode;
    QString externalNodeName;
    QString consumerName;
    qint64 rowVersion = 0;

    [[nodiscard]] bool hasObjectCard() const
    {
        return id > 0 && classTable != QStringLiteral("unclassified");
    }
};

class ZeroLoadConsumerRepository final {
public:
    [[nodiscard]] QList<ZeroLoadConsumerRow> load(
        const QSqlDatabase& database,
        const ZeroLoadConsumerCriteria& criteria,
        QString* error) const;
};

}  // namespace tgid::repo
