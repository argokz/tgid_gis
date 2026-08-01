#pragma once

#include <QList>
#include <QSqlDatabase>
#include <QString>

namespace tgid::repo {

struct DisconnectedConsumerCriteria final {
    int fragmentId = 0;
    QString searchText;
    int limit = 5000;
};

struct DisconnectedConsumerRow final {
    qint64 id = 0;
    qint64 sourceId = 0;
    QString classTable;
    int fragmentId = 0;
    QString externalCode;
    QString externalNodeName;
    QString consumerName;
    qint64 calculationId = 0;
    qint64 rowVersion = 0;

    [[nodiscard]] bool hasObjectCard() const
    {
        return id > 0 && classTable != QStringLiteral("unclassified");
    }

    [[nodiscard]] bool hasCalculation() const
    {
        return calculationId > 0;
    }
};

class DisconnectedConsumerRepository final {
public:
    [[nodiscard]] QList<DisconnectedConsumerRow> load(
        const QSqlDatabase& database,
        const DisconnectedConsumerCriteria& criteria,
        QString* error) const;
};

}  // namespace tgid::repo
