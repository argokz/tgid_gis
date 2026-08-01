#pragma once

#include <QList>
#include <QSqlDatabase>
#include <QString>

namespace tgid::repo {

struct PipeLengthReportCriteria final {
    int fragmentId = 0;
    QString grouping = QStringLiteral("none");
    bool includeArchived = false;
    QList<qint64> pipeIds;
};

struct PipeLengthReportRow final {
    QString groupKey;
    QString groupLabel;
    qint64 pipeCount = 0;
    double passportLength = 0.0;
    double geometryLength = 0.0;
    double effectiveLength = 0.0;
    qint64 missingPassportCount = 0;
};

class PipeLengthReportRepository final {
public:
    [[nodiscard]] QList<PipeLengthReportRow> load(
        const QSqlDatabase& database,
        const PipeLengthReportCriteria& criteria,
        QString* error) const;
};

}  // namespace tgid::repo
