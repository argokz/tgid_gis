#pragma once

#include <QList>
#include <QPointF>
#include <QSqlDatabase>
#include <QString>

namespace tgid::repo {

struct ObjectFieldOption final {
    QString value;
    QString label;
};

struct ObjectAttribute final {
    QString name;
    QString displayName;
    QString dataType;
    QString databaseType;
    QString value;
    QString unit;
    QString groupName;
    QString editorKind;
    QList<ObjectFieldOption> options;
    bool isNull = false;
    bool editable = false;
};

struct ObjectDetails final {
    qint64 id = 0;
    qint64 rowVersion = 0;
    QString classTable;
    bool archived = false;
    bool canArchive = false;
    bool canSplit = false;
    QList<ObjectAttribute> attributes;
    QString error;

    [[nodiscard]] bool isValid() const { return error.isEmpty(); }
};

struct AttributeChange final {
    QString name;
    QString databaseType;
    QString value;
    bool setNull = false;
};

struct UpdateResult final {
    bool success = false;
    bool conflict = false;
    qint64 rowVersion = 0;
    QString error;
};

struct ObjectHistoryEntry final {
    qint64 rowVersion = 0;
    QString operation;
    QString changedAt;
    QString changedBy;
    QString applicationName;
    QString changedFields;
};

struct ArchivedObjectInfo final {
    qint64 id = 0;
    int fragmentId = 0;
    QString classTable;
    QString label;
    QString archivedAt;
    bool isNode = false;
};

struct CreateObjectResult final {
    bool success = false;
    qint64 id = 0;
    qint64 rowVersion = 0;
    QString error;
};

struct SplitLineResult final {
    bool success = false;
    bool conflict = false;
    bool nodeCreated = false;
    qint64 nodeId = 0;
    qint64 firstLineId = 0;
    qint64 secondLineId = 0;
    double splitFraction = 0.0;
    QString error;
};

class ObjectRepository final {
public:
    [[nodiscard]] ObjectDetails load(
        const QSqlDatabase& database,
        const QString& classTable,
        qint64 id) const;

    [[nodiscard]] UpdateResult update(
        QSqlDatabase database,
        const QString& classTable,
        qint64 id,
        qint64 expectedVersion,
        const QList<AttributeChange>& changes) const;

    [[nodiscard]] UpdateResult setArchived(
        QSqlDatabase database,
        const QString& classTable,
        qint64 id,
        qint64 expectedVersion,
        bool archived) const;

    [[nodiscard]] QList<ObjectHistoryEntry> loadHistory(
        const QSqlDatabase& database,
        const QString& classTable,
        qint64 id,
        QString* error) const;

    [[nodiscard]] QList<ArchivedObjectInfo> loadArchived(
        const QSqlDatabase& database,
        QString* error) const;

    [[nodiscard]] CreateObjectResult createPoint(
        QSqlDatabase database,
        const QString& classTable,
        int fragmentId,
        const QPointF& position) const;

    [[nodiscard]] CreateObjectResult createLine(
        QSqlDatabase database,
        const QString& classTable,
        int fragmentId,
        qint64 nodeFrom,
        qint64 nodeTo) const;

    [[nodiscard]] SplitLineResult splitLine(
        QSqlDatabase database,
        const QString& classTable,
        qint64 id,
        qint64 expectedVersion,
        const QPointF& position) const;
};

}  // namespace tgid::repo
