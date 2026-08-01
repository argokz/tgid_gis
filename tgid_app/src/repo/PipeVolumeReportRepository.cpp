#include "repo/PipeVolumeReportRepository.h"

#include <QHash>
#include <QSet>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>

namespace tgid::repo {

QList<PipeVolumeReportRow> PipeVolumeReportRepository::load(
    const QSqlDatabase& database,
    const PipeVolumeReportCriteria& criteria,
    QString* error) const
{
    QList<PipeVolumeReportRow> result;
    if (error != nullptr) {
        error->clear();
    }
    if (!database.isOpen() || criteria.fragmentId < 0
        || criteria.pipeIds.size() > 500) {
        if (error != nullptr) {
            *error = QStringLiteral("Некорректные параметры отчёта");
        }
        return result;
    }

    struct GroupSql final {
        QString key;
        QString label;
        QString joins;
    };
    static const QHash<QString, GroupSql> groups = {
        {QStringLiteral("none"),
         {QStringLiteral("''::text"),
          QStringLiteral("'Вся сеть'::text"),
          {}}},
        {QStringLiteral("diameter"),
         {QStringLiteral("coalesce(p.diameterinternal::text, '')"),
          QStringLiteral(
              "CASE WHEN p.diameterinternal IS NULL THEN 'Не указан' "
              "ELSE trim(to_char(p.diameterinternal, 'FM999999990.###')) || ' мм' END"),
          {}}},
        {QStringLiteral("scheme"),
         {QStringLiteral("coalesce(p.externalsignlineid::text, '')"),
          QStringLiteral(
              "CASE p.externalsignlineid "
              "WHEN 1 THEN 'Подающий + обратный (×2)' "
              "WHEN 2 THEN 'Подающий (×1)' "
              "WHEN 3 THEN 'Обратный (×1)' "
              "ELSE 'Прочее (×1)' END"),
          {}}},
        {QStringLiteral("fragment"),
         {QStringLiteral("p.fragment_id::text"),
          QStringLiteral(
              "coalesce(nullif(f.nasel_point, '') || ' — ', '') || "
              "f.name || ' (#' || f.id::text || ')'"),
          QStringLiteral(
              "JOIN net.fragment f ON f.id = p.fragment_id")}},
    };
    if (!groups.contains(criteria.grouping)) {
        if (error != nullptr) {
            *error = QStringLiteral("Недопустимая группировка отчёта");
        }
        return result;
    }
    const GroupSql group = groups.value(criteria.grouping);
    const QString activeCondition =
        criteria.includeArchived
            ? QStringLiteral("TRUE")
            : QStringLiteral("p.removed_at IS NULL");
    QString selectedCondition = QStringLiteral("TRUE");
    QSet<qint64> uniqueIds;
    if (!criteria.pipeIds.isEmpty()) {
        QStringList placeholders;
        placeholders.reserve(criteria.pipeIds.size());
        for (const qint64 id : criteria.pipeIds) {
            if (id <= 0 || uniqueIds.contains(id)) {
                if (error != nullptr) {
                    *error = QStringLiteral(
                        "ID выбранных участков должны быть положительными и уникальными");
                }
                return result;
            }
            uniqueIds.insert(id);
            placeholders.append(QStringLiteral("CAST(? AS bigint)"));
        }
        selectedCondition = QStringLiteral("p.id IN (%1)")
                                .arg(placeholders.join(QStringLiteral(", ")));
    }

    QSqlQuery query(database);
    query.prepare(
        QStringLiteral(
            "WITH all_nodes AS ("
            "    SELECT id, internalnodeid FROM net.consumer_real"
            "    UNION ALL SELECT id, internalnodeid FROM net.consumer_general"
            "    UNION ALL SELECT id, internalnodeid FROM net.heat_source"
            "    UNION ALL SELECT id, internalnodeid FROM net.pump_station"
            "    UNION ALL SELECT id, internalnodeid FROM net.valve_3way"
            "    UNION ALL SELECT id, internalnodeid FROM net.connect_node"
            "    UNION ALL SELECT id, internalnodeid FROM net.heat_chamber"
            "    UNION ALL SELECT id, internalnodeid FROM net.refill_node"
            "    UNION ALL SELECT id, internalnodeid FROM net.node_plain)"
            "SELECT %1 AS group_key, %2 AS group_label,"
            "       count(*)::bigint,"
            "       count(*) FILTER ("
            "           WHERE p.diameterinternal IS NOT NULL"
            "             AND p.pipesectlength IS NOT NULL)::bigint,"
            "       coalesce(sum(p.pipesectlength), 0)::double precision,"
            "       coalesce(sum("
            "           power(p.diameterinternal / 1000.0, 2)"
            "           * p.pipesectlength"
            "           * CASE WHEN p.externalsignlineid = 1 THEN 2 ELSE 1 END"
            "           * pi() / 4.0), 0)::double precision,"
            "       count(*) FILTER ("
            "           WHERE p.diameterinternal IS NULL)::bigint,"
            "       count(*) FILTER ("
            "           WHERE p.pipesectlength IS NULL)::bigint"
            "  FROM net.pipe_section p"
            "  JOIN all_nodes start_node"
            "    ON start_node.id = p.node_from"
            "   AND start_node.internalnodeid IS NULL %3"
            " WHERE %4"
            "   AND (CAST(? AS integer) = 0"
            "        OR p.fragment_id = CAST(? AS integer))"
            "   AND (%5)"
            " GROUP BY %1, %2"
            " ORDER BY %2, %1")
            .arg(group.key, group.label, group.joins,
                 activeCondition, selectedCondition));
    query.addBindValue(criteria.fragmentId);
    query.addBindValue(criteria.fragmentId);
    for (const qint64 id : criteria.pipeIds) {
        query.addBindValue(id);
    }
    if (!query.exec()) {
        if (error != nullptr) {
            *error = query.lastError().text();
        }
        return result;
    }
    while (query.next()) {
        result.append({query.value(0).toString(),
                       query.value(1).toString(),
                       query.value(2).toLongLong(),
                       query.value(3).toLongLong(),
                       query.value(4).toDouble(),
                       query.value(5).toDouble(),
                       query.value(6).toLongLong(),
                       query.value(7).toLongLong()});
    }
    return result;
}

}  // namespace tgid::repo
