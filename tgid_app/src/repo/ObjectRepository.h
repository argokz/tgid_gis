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
    bool canJoin = false;
    bool canReclass = false;
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

struct ObjectVersion final {
    qint64 id = 0;
    qint64 rowVersion = 0;
};

struct BatchUpdateResult final {
    bool success = false;
    bool conflict = false;
    qsizetype updatedCount = 0;
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

struct JoinLinesResult final {
    bool success = false;
    bool conflict = false;
    qint64 joinedLineId = 0;
    qint64 archivedNodeId = 0;
    QString error;
};

struct MoveNodeResult final {
    bool success = false;
    bool conflict = false;
    qint64 rowVersion = 0;
    int connectedLines = 0;
    QString error;
};

struct ReclassTarget final {
    QString tableName;
    QString displayName;
};

struct ReclassResult final {
    bool success = false;
    bool conflict = false;
    bool isNode = false;
    QString targetTable;
    qint64 rowVersion = 0;
    int copiedFields = 0;
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

    [[nodiscard]] QList<ObjectVersion> loadVersions(
        const QSqlDatabase& database,
        const QString& classTable,
        const QList<qint64>& ids,
        QString* error) const;

    [[nodiscard]] BatchUpdateResult batchUpdate(
        QSqlDatabase database,
        const QString& classTable,
        const QList<ObjectVersion>& objects,
        const AttributeChange& change) const;

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

    [[nodiscard]] JoinLinesResult joinLines(
        QSqlDatabase database,
        const QString& classTable,
        qint64 firstId,
        qint64 firstExpectedVersion,
        qint64 secondId,
        qint64 secondExpectedVersion) const;

    [[nodiscard]] MoveNodeResult moveNode(
        QSqlDatabase database,
        const QString& classTable,
        qint64 id,
        qint64 expectedVersion,
        const QPointF& position) const;

    [[nodiscard]] QList<ReclassTarget> loadReclassTargets(
        const QSqlDatabase& database,
        const QString& sourceTable,
        QString* error) const;

    [[nodiscard]] ReclassResult reclassObject(
        QSqlDatabase database,
        const QString& sourceTable,
        qint64 id,
        qint64 expectedVersion,
        const QString& targetTable) const;
};

}  // namespace tgid::repo
