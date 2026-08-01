#include "repo/ClosedPipeSectionRepository.h"

#include <QSqlError>
#include <QSqlQuery>

namespace tgid::repo {

QList<ClosedPipeSectionRow> ClosedPipeSectionRepository::load(
    const QSqlDatabase& database,
    const ClosedPipeSectionCriteria& criteria,
    QString* error) const
{
    QList<ClosedPipeSectionRow> result;
    if (error != nullptr) {
        error->clear();
    }
    if (!database.isOpen() || criteria.fragmentId < 0
        || criteria.limit < 1 || criteria.limit > 5000) {
        if (error != nullptr) {
            *error = QStringLiteral("Некорректные параметры списка");
        }
        return result;
    }

    QSqlQuery query(database);
    query.prepare(QStringLiteral(
        "SELECT pipe.id, pipe.src_id, node_from.fileid,"
        "       code_from.name,"
        "       coalesce(node_from.externalnodename, ''),"
        "       code_to.name,"
        "       coalesce(node_to.externalnodename, ''),"
        "       pipe.pipesectlength, pipe.diameterinternal,"
        "       pipe.row_version"
        "  FROM net.pipe_section AS pipe"
        "  JOIN net.v_nodes AS node_from"
        "    ON node_from.id = pipe.node_from AND node_from.removed = 0"
        "  JOIN ref.externalcodes AS code_from"
        "    ON code_from.id = node_from.externalcodeid"
        "  JOIN net.v_nodes AS node_to"
        "    ON node_to.id = pipe.node_to AND node_to.removed = 0"
        "  JOIN ref.externalcodes AS code_to"
        "    ON code_to.id = node_to.externalcodeid"
        " WHERE pipe.removed_at IS NULL"
        "   AND node_from.internalnodeid IS NULL"
        "   AND pipe.pipesectstateidflow = 2"
        "   AND pipe.pipesectstateidret = 2"
        "   AND (CAST(? AS integer) = 0"
        "        OR node_from.fileid = CAST(? AS integer))"
        "   AND (CAST(? AS text) = ''"
        "        OR coalesce(code_from.name, '')"
        "             ILIKE '%' || CAST(? AS text) || '%'"
        "        OR coalesce(node_from.externalnodename, '')"
        "             ILIKE '%' || CAST(? AS text) || '%'"
        "        OR coalesce(code_to.name, '')"
        "             ILIKE '%' || CAST(? AS text) || '%'"
        "        OR coalesce(node_to.externalnodename, '')"
        "             ILIKE '%' || CAST(? AS text) || '%'"
        "        OR pipe.src_id::text = CAST(? AS text))"
        " ORDER BY lower(coalesce(node_from.externalnodename, '')) ,"
        "          lower(coalesce(node_to.externalnodename, '')) ,"
        "          pipe.src_id"
        " LIMIT CAST(? AS integer)"));
    query.addBindValue(criteria.fragmentId);
    query.addBindValue(criteria.fragmentId);
    for (int index = 0; index < 6; ++index) {
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
                       query.value(2).toInt(),
                       query.value(3).toString(),
                       query.value(4).toString(),
                       query.value(5).toString(),
                       query.value(6).toString(),
                       query.value(7).toDouble(),
                       query.value(7).isNull(),
                       query.value(8).toDouble(),
                       query.value(8).isNull(),
                       query.value(9).toLongLong()});
    }
    return result;
}

}  // namespace tgid::repo
