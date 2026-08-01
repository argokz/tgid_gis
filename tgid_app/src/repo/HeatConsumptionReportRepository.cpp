#include "repo/HeatConsumptionReportRepository.h"

#include <QSet>
#include <QSqlError>
#include <QSqlQuery>

namespace tgid::repo {

HeatConsumptionReportResult HeatConsumptionReportRepository::load(
    const QSqlDatabase& database,
    const HeatConsumptionReportCriteria& criteria) const
{
    HeatConsumptionReportResult result;
    static const QSet<QString> modes = {
        QStringLiteral("all"),
        QStringLiteral("closed"),
        QStringLiteral("open"),
    };
    if (!database.isOpen() || criteria.fragmentId < 0
        || !modes.contains(criteria.mode)) {
        result.error = QStringLiteral("Некорректные параметры отчёта");
        return result;
    }

    QSqlQuery query(database);
    query.prepare(QStringLiteral(
        "WITH all_nodes AS ("
        "    SELECT src_id AS node_id, fragment_id, removed_at"
        "      FROM net.consumer_real"
        "    UNION ALL SELECT src_id, fragment_id, removed_at"
        "      FROM net.consumer_general"
        "    UNION ALL SELECT src_id, fragment_id, removed_at"
        "      FROM net.heat_source"
        "    UNION ALL SELECT src_id, fragment_id, removed_at"
        "      FROM net.pump_station"
        "    UNION ALL SELECT src_id, fragment_id, removed_at"
        "      FROM net.valve_3way"
        "    UNION ALL SELECT src_id, fragment_id, removed_at"
        "      FROM net.connect_node"
        "    UNION ALL SELECT src_id, fragment_id, removed_at"
        "      FROM net.heat_chamber"
        "    UNION ALL SELECT src_id, fragment_id, removed_at"
        "      FROM net.refill_node"
        "    UNION ALL SELECT src_id, fragment_id, removed_at"
        "      FROM net.node_plain),"
        "latest_calculation AS ("
        "    SELECT fileid AS fragment_id, max(id) AS calculation_id"
        "      FROM calc.calculation"
        "     GROUP BY fileid)"
        "SELECT coalesce(sum(output.qotz), 0)::double precision,"
        "       coalesce(sum(output.qotn), 0)::double precision,"
        "       coalesce(sum(output.dop12), 0)::double precision,"
        "       coalesce(sum(output.dop18), 0)::double precision,"
        "       coalesce(sum(output.dop19), 0)::double precision,"
        "       coalesce(sum(output.dop20), 0)::double precision,"
        "       coalesce(sum(output.dop17), 0)::double precision,"
        "       coalesce(sum(output.a4), 0)::double precision,"
        "       coalesce(sum(output.a5), 0)::double precision,"
        "       coalesce(sum(output.a6), 0)::double precision,"
        "       coalesce(sum(output.a12), 0)::double precision,"
        "       coalesce(sum(output.a13), 0)::double precision,"
        "       coalesce(sum(output.a14), 0)::double precision,"
        "       coalesce(sum(output.a15), 0)::double precision,"
        "       count(*)::bigint,"
        "       count(DISTINCT node.fragment_id)::bigint"
        "  FROM calc.pt_out AS output"
        "  JOIN all_nodes AS node"
        "    ON node.node_id = output.nodeid"
        "   AND node.removed_at IS NULL"
        "  JOIN latest_calculation AS latest"
        "    ON latest.fragment_id = node.fragment_id"
        "   AND latest.calculation_id = output.calculationid"
        " WHERE CAST(? AS integer) = 0"
        "    OR node.fragment_id = CAST(? AS integer)"));
    query.addBindValue(criteria.fragmentId);
    query.addBindValue(criteria.fragmentId);
    if (!query.exec() || !query.next()) {
        result.error = query.lastError().text();
        return result;
    }

    result.resultNodeCount = query.value(14).toLongLong();
    result.fragmentCount = query.value(15).toLongLong();
    if (result.resultNodeCount == 0) {
        return result;
    }
    const auto addRow = [&result, &query](
                            const QString& key,
                            const QString& label,
                            int heatColumn,
                            int flowColumn) {
        result.rows.append({key, label,
                            query.value(heatColumn).toDouble(),
                            query.value(flowColumn).toDouble()});
    };
    if (criteria.mode != QStringLiteral("open")) {
        addRow(QStringLiteral("heating_dependent"),
               QStringLiteral("Отопление, зависимое"), 0, 7);
        addRow(QStringLiteral("heating_independent"),
               QStringLiteral("Отопление, независимое"), 1, 8);
        addRow(QStringLiteral("ventilation"),
               QStringLiteral("Вентиляция"), 2, 9);
    }
    if (criteria.mode != QStringLiteral("closed")) {
        addRow(QStringLiteral("open_hws_supply"),
               QStringLiteral("Открытое ГВС из подачи"), 3, 10);
        addRow(QStringLiteral("open_hws_return"),
               QStringLiteral("Открытое ГВС из обратки"), 4, 11);
        addRow(QStringLiteral("open_hws_recirculation"),
               QStringLiteral("Рециркуляция открытого ГВС"), 5, 12);
    }
    if (criteria.mode != QStringLiteral("open")) {
        addRow(QStringLiteral("closed_hws"),
               QStringLiteral("Закрытое ГВС, суммарно"), 6, 13);
    }
    return result;
}

}  // namespace tgid::repo
