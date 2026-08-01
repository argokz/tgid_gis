#pragma once

#include "repo/ObjectRepository.h"

#include <QList>
#include <QSqlDatabase>
#include <QString>

namespace tgid::repo {

struct SearchField final {
    QString name;
    QString displayName;
    QString unit;
    QString databaseType;
    QString editorKind;
    QList<ObjectFieldOption> options;
};

struct SearchCriteria final {
    QString classTable;
    QString fieldName;
    QString comparison;
    QString value;
    QString secondValue;
    bool includeArchived = false;
    int limit = 200;
};

struct SearchResult final {
    qint64 id = 0;
    QString fragmentId;
    QString value;
    qint64 rowVersion = 0;
    bool archived = false;
};

class SearchRepository final {
public:
    [[nodiscard]] QList<SearchField> loadFields(
        const QSqlDatabase& database,
        const QString& classTable,
        QString* error) const;

    [[nodiscard]] QList<SearchResult> search(
        const QSqlDatabase& database,
        const SearchCriteria& criteria,
        QString* error) const;
};

}  // namespace tgid::repo
