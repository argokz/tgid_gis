#pragma once

#include <QList>
#include <QSqlDatabase>
#include <QString>

namespace tgid::repo {

struct ClosedConsumerCriteria final {
    int fragmentId = 0;
    QString searchText;
    int limit = 1000;
};

struct ClosedConsumerRow final {
    qint64 id = 0;
    qint64 sourceId = 0;
    QString classTable;
    int fragmentId = 0;
    QString externalCode;
    QString externalNodeName;
    QString consumerName;
    qint64 rowVersion = 0;
};

class ClosedConsumerRepository final {
public:
    [[nodiscard]] QList<ClosedConsumerRow> load(
        const QSqlDatabase& database,
        const ClosedConsumerCriteria& criteria,
        QString* error) const;
};

}  // namespace tgid::repo
