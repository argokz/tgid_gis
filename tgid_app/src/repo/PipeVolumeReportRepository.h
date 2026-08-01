#pragma once

#include <QList>
#include <QSqlDatabase>
#include <QString>

namespace tgid::repo {

struct PipeVolumeReportCriteria final {
    int fragmentId = 0;
    QString grouping = QStringLiteral("none");
    bool includeArchived = false;
    QList<qint64> pipeIds;
};

struct PipeVolumeReportRow final {
    QString groupKey;
    QString groupLabel;
    qint64 pipeCount = 0;
    qint64 calculatedCount = 0;
    double passportLength = 0.0;
    double volume = 0.0;
    qint64 missingDiameterCount = 0;
    qint64 missingLengthCount = 0;
};

class PipeVolumeReportRepository final {
public:
    [[nodiscard]] QList<PipeVolumeReportRow> load(
        const QSqlDatabase& database,
        const PipeVolumeReportCriteria& criteria,
        QString* error) const;
};

}  // namespace tgid::repo
