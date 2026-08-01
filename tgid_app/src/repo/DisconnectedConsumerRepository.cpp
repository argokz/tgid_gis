#include "repo/DisconnectedConsumerRepository.h"

#include <QSqlError>
#include <QSqlQuery>

namespace tgid::repo {

QList<DisconnectedConsumerRow> DisconnectedConsumerRepository::load(
    const QSqlDatabase& database,
    const DisconnectedConsumerCriteria& criteria,
    QString* error) const
{
    QList<DisconnectedConsumerRow> result;
    if (error != nullptr) {
        error->clear();
    }
    if (!database.isOpen() || criteria.fragmentId < 0
        || criteria.limit < 1 || criteria.limit > 10000) {
        if (error != nullptr) {
            *error = QStringLiteral("Некорректные параметры списка");
        }
        return result;
    }

    QSqlQuery query(database);
    query.prepare(QStringLiteral(
        "WITH consumer AS ("
        "    SELECT nodeid, consumerstateid, name"
        "      FROM net.v_generalizedconsumers"
        "    UNION"
        "    SELECT nodeid, consumerstateid, name"
        "      FROM net.v_realconsumers),"
        "latest_calculation AS ("
        "    SELECT fileid AS fragment_id, max(id) AS calculation_id"
        "      FROM calc.calculation GROUP BY fileid),"
        "canonical AS ("
        "    SELECT id AS object_id, row_version,"
        "           'consumer_general'::text AS class_table"
        "      FROM net.consumer_general WHERE removed_at IS NULL"
        "    UNION ALL"
        "    SELECT id, row_version, 'consumer_real'::text"
        "      FROM net.consumer_real WHERE removed_at IS NULL)"
        "SELECT coalesce(canonical.object_id, 0), node.id,"
        "       coalesce(canonical.class_table, 'unclassified'),"
        "       node.fileid, code.name,"
        "       coalesce(node.externalnodename, ''),"
        "       coalesce(consumer.name, ''),"
        "       coalesce(latest.calculation_id, 0),"
        "       coalesce(canonical.row_version, 0)"
        "  FROM net.v_nodes AS node"
        "  JOIN ref.externalcodes AS code"
        "    ON code.id = node.externalcodeid"
        "  JOIN consumer ON consumer.nodeid = node.id"
        "  LEFT JOIN latest_calculation AS latest"
        "    ON latest.fragment_id = node.fileid"
        "  LEFT JOIN calc.pt_out AS output"
        "    ON output.nodeid = node.id"
        "   AND output.calculationid = latest.calculation_id"
        "  LEFT JOIN canonical ON canonical.object_id = node.id"
        " WHERE node.removed = 0 AND output.nodeid IS NULL"
        "   AND (CAST(? AS integer) = 0"
        "        OR node.fileid = CAST(? AS integer))"
        "   AND (CAST(? AS text) = ''"
        "        OR coalesce(code.name, '') ILIKE '%' || CAST(? AS text) || '%'"
        "        OR coalesce(node.externalnodename, '')"
        "             ILIKE '%' || CAST(? AS text) || '%'"
        "        OR coalesce(consumer.name, '')"
        "             ILIKE '%' || CAST(? AS text) || '%'"
        "        OR node.id::text = CAST(? AS text))"
        " ORDER BY (latest.calculation_id IS NULL) DESC,"
        "          lower(coalesce(consumer.name, '')) ,"
        "          lower(coalesce(node.externalnodename, '')) , node.id"
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
                       query.value(7).toLongLong(),
                       query.value(8).toLongLong()});
    }
    return result;
}

}  // namespace tgid::repo
