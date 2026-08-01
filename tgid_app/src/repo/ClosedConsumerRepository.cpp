#include "repo/ClosedConsumerRepository.h"

#include <QSqlError>
#include <QSqlQuery>

namespace tgid::repo {

QList<ClosedConsumerRow> ClosedConsumerRepository::load(
    const QSqlDatabase& database,
    const ClosedConsumerCriteria& criteria,
    QString* error) const
{
    QList<ClosedConsumerRow> result;
    if (error != nullptr) {
        error->clear();
    }
    if (!database.isOpen() || criteria.fragmentId < 0
        || criteria.limit < 1 || criteria.limit > 2000) {
        if (error != nullptr) {
            *error = QStringLiteral("Некорректные параметры списка");
        }
        return result;
    }

    QSqlQuery query(database);
    query.prepare(QStringLiteral(
        "WITH closed_consumer AS ("
        "    SELECT id, src_id, fragment_id, externalcodeid,"
        "           externalnodename, name, row_version,"
        "           'consumer_general'::text AS class_table"
        "      FROM net.consumer_general"
        "     WHERE removed_at IS NULL AND consumerstateid = 2"
        "    UNION ALL"
        "    SELECT id, src_id, fragment_id, externalcodeid,"
        "           externalnodename, name, row_version,"
        "           'consumer_real'::text"
        "      FROM net.consumer_real"
        "     WHERE removed_at IS NULL AND consumerstateid = 2)"
        "SELECT consumer.id, consumer.src_id, consumer.class_table,"
        "       consumer.fragment_id, code.name,"
        "       coalesce(consumer.externalnodename, ''),"
        "       coalesce(consumer.name, ''), consumer.row_version"
        "  FROM closed_consumer AS consumer"
        "  JOIN ref.externalcodes AS code"
        "    ON code.id = consumer.externalcodeid"
        " WHERE (CAST(? AS integer) = 0"
        "        OR consumer.fragment_id = CAST(? AS integer))"
        "   AND (CAST(? AS text) = ''"
        "        OR coalesce(code.name, '') ILIKE '%' || CAST(? AS text) || '%'"
        "        OR coalesce(consumer.externalnodename, '')"
        "             ILIKE '%' || CAST(? AS text) || '%'"
        "        OR coalesce(consumer.name, '')"
        "             ILIKE '%' || CAST(? AS text) || '%'"
        "        OR consumer.src_id::text = CAST(? AS text))"
        " ORDER BY lower(coalesce(consumer.name, '')),"
        "          lower(coalesce(consumer.externalnodename, '')) ,"
        "          consumer.src_id"
        " LIMIT CAST(? AS integer)"));
    query.addBindValue(criteria.fragmentId);
    query.addBindValue(criteria.fragmentId);
    for (int index = 0; index < 5; ++index) {
        query.addBindValue(criteria.searchText.trimmed());
    }
    query.addBindValue(criteria.limit);
    if (!query.exec()) {
        if (error != nullptr) {
            *error = query.lastError().text();
        }
        return result;
    }
    while (query.next()) {
        result.append({query.value(0).toLongLong(),
                       query.value(1).toLongLong(),
                       query.value(2).toString(),
                       query.value(3).toInt(),
                       query.value(4).toString(),
                       query.value(5).toString(),
                       query.value(6).toString(),
                       query.value(7).toLongLong()});
    }
    return result;
}

}  // namespace tgid::repo
