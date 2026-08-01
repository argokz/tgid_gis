#pragma once

#include <QList>
#include <QSqlDatabase>
#include <QString>

namespace tgid::repo {

struct HeatConsumptionReportCriteria final {
    int fragmentId = 0;
    QString mode = QStringLiteral("all");
};

struct HeatConsumptionReportRow final {
    QString metricKey;
    QString metricLabel;
    double heatLoad = 0.0;
    double massFlow = 0.0;
};

struct HeatConsumptionReportResult final {
    QList<HeatConsumptionReportRow> rows;
    qint64 resultNodeCount = 0;
    qint64 fragmentCount = 0;
    QString error;
};

class HeatConsumptionReportRepository final {
public:
    [[nodiscard]] HeatConsumptionReportResult load(
        const QSqlDatabase& database,
        const HeatConsumptionReportCriteria& criteria) const;
};

}  // namespace tgid::repo
