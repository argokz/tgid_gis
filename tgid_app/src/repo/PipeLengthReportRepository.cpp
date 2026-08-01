#include "repo/PipeLengthReportRepository.h"

#include <QHash>
#include <QSet>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>

namespace tgid::repo {

QList<PipeLengthReportRow> PipeLengthReportRepository::load(
    const QSqlDatabase& database,
    const PipeLengthReportCriteria& criteria,
    QString* error) const
{
    QList<PipeLengthReportRow> result;
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
          QStringLiteral("'Все трубопроводы'::text"),
          {}}},
        {QStringLiteral("diameter"),
         {QStringLiteral("coalesce(p.diametercondit::text, '')"),
          QStringLiteral(
              "coalesce(p.diametercondit::text || ' мм', 'Не указан')"),
          {}}},
        {QStringLiteral("tubing_type"),
         {QStringLiteral("coalesce(p.tubingtypeid::text, '')"),
          QStringLiteral("coalesce(tt.name::text, 'Не указан')"),
          QStringLiteral(
              "LEFT JOIN ref.tubingtypes tt ON tt.id = p.tubingtypeid")}},
        {QStringLiteral("insulation"),
         {QStringLiteral("coalesce(p.isolmaterialid::text, '')"),
          QStringLiteral("coalesce(im.name::text, 'Не указан')"),
          QStringLiteral(
              "LEFT JOIN ref.isolmaterials im ON im.id = p.isolmaterialid")}},
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
            "SELECT %1 AS group_key, %2 AS group_label,"
            "       count(*)::bigint,"
            "       coalesce(sum(p.pipesectlength), 0)::double precision,"
            "       coalesce(sum(ST_Length(p.geom)), 0)::double precision,"
            "       coalesce(sum(coalesce(p.pipesectlength, ST_Length(p.geom))), 0)::double precision,"
            "       count(*) FILTER (WHERE p.pipesectlength IS NULL)::bigint"
            "  FROM net.pipe_section p %3"
            " WHERE %4 AND (CAST(? AS integer) = 0 OR p.fragment_id = CAST(? AS integer))"
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
                       query.value(3).toDouble(),
                       query.value(4).toDouble(),
                       query.value(5).toDouble(),
                       query.value(6).toLongLong()});
    }
    return result;
}

}  // namespace tgid::repo
