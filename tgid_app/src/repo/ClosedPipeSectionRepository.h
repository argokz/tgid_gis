#pragma once

#include <QList>
#include <QSqlDatabase>
#include <QString>

namespace tgid::repo {

struct ClosedPipeSectionCriteria final {
    int fragmentId = 0;
    QString searchText;
    int limit = 2000;
};

struct ClosedPipeSectionRow final {
    qint64 id = 0;
    qint64 sourceId = 0;
    int fragmentId = 0;
    QString nodeFromCode;
    QString nodeFromName;
    QString nodeToCode;
    QString nodeToName;
    double pipeLength = 0.0;
    bool pipeLengthIsNull = false;
    double internalDiameter = 0.0;
    bool internalDiameterIsNull = false;
    qint64 rowVersion = 0;
};

class ClosedPipeSectionRepository final {
public:
    [[nodiscard]] QList<ClosedPipeSectionRow> load(
        const QSqlDatabase& database,
        const ClosedPipeSectionCriteria& criteria,
        QString* error) const;
};

}  // namespace tgid::repo
