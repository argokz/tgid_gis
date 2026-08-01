#include "repo/ObjectRepository.h"

#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QRegularExpression>
#include <QSet>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include <cmath>
#include <utility>

namespace tgid::repo {
namespace {

const QRegularExpression& safeIdentifier()
{
    static const QRegularExpression expression(
        QStringLiteral("^[a-z_][a-z0-9_]*$"));
    return expression;
}

bool tableIsPublished(
    const QSqlDatabase& database,
    const QString& classTable,
    QString* error)
{
    QSqlQuery query(database);
    query.prepare(QStringLiteral(
        "SELECT EXISTS ("
        "  SELECT 1 FROM meta.layer_catalog"
        "   WHERE schema_name = 'net' AND table_name = :table_name"
        ")"));
    query.bindValue(QStringLiteral(":table_name"), classTable);
    if (!query.exec() || !query.next()) {
        if (error != nullptr) {
            *error = query.lastError().text();
        }
        return false;
    }
    if (!query.value(0).toBool()) {
        if (error != nullptr) {
            *error =
                QStringLiteral("Таблица отсутствует в meta.layer_catalog");
        }
        return false;
    }
    return true;
}

QString castForDatabaseType(const QString& databaseType)
{
    static const QHash<QString, QString> casts = {
        {QStringLiteral("int2"), QStringLiteral("smallint")},
        {QStringLiteral("int4"), QStringLiteral("integer")},
        {QStringLiteral("int8"), QStringLiteral("bigint")},
        {QStringLiteral("float4"), QStringLiteral("real")},
        {QStringLiteral("float8"), QStringLiteral("double precision")},
        {QStringLiteral("numeric"), QStringLiteral("numeric")},
        {QStringLiteral("bool"), QStringLiteral("boolean")},
        {QStringLiteral("text"), QStringLiteral("text")},
        {QStringLiteral("varchar"), QStringLiteral("text")},
        {QStringLiteral("bpchar"), QStringLiteral("text")},
        {QStringLiteral("date"), QStringLiteral("date")},
        {QStringLiteral("timestamp"), QStringLiteral("timestamp")},
        {QStringLiteral("timestamptz"),
         QStringLiteral("timestamp with time zone")},
        {QStringLiteral("time"), QStringLiteral("time")},
        {QStringLiteral("timetz"), QStringLiteral("time with time zone")},
        {QStringLiteral("uuid"), QStringLiteral("uuid")},
        {QStringLiteral("json"), QStringLiteral("json")},
        {QStringLiteral("jsonb"), QStringLiteral("jsonb")},
    };
    return casts.value(databaseType);
}

QVariant valueForBinding(const AttributeChange& change)
{
    if (change.databaseType != QStringLiteral("bool")) {
        return change.value;
    }

    const QString value = change.value.trimmed().toLower();
    if (value == QStringLiteral("да") || value == QStringLiteral("yes")
        || value == QStringLiteral("1")) {
        return QStringLiteral("true");
    }
    if (value == QStringLiteral("нет") || value == QStringLiteral("no")
        || value == QStringLiteral("0")) {
        return QStringLiteral("false");
    }
    return value;
}

QString jsonValueToString(const QJsonValue& value)
{
    if (value.isNull() || value.isUndefined()) {
        return {};
    }
    if (value.isString()) {
        return value.toString();
    }
    if (value.isBool()) {
        return value.toBool() ? QStringLiteral("да") : QStringLiteral("нет");
    }
    if (value.isDouble()) {
        return QString::number(value.toDouble(), 'g', 15);
    }
    if (value.isArray()) {
        return QString::fromUtf8(
            QJsonDocument(value.toArray()).toJson(QJsonDocument::Compact));
    }
    return QString::fromUtf8(
        QJsonDocument(value.toObject()).toJson(QJsonDocument::Compact));
}

QList<ObjectFieldOption> loadLookupOptions(
    const QSqlDatabase& database,
    const QString& schemaName,
    const QString& tableName,
    const QString& valueColumn,
    const QString& labelColumn,
    QString* error)
{
    QList<ObjectFieldOption> result;
    if (!safeIdentifier().match(schemaName).hasMatch()
        || !safeIdentifier().match(tableName).hasMatch()
        || !safeIdentifier().match(valueColumn).hasMatch()
        || !safeIdentifier().match(labelColumn).hasMatch()) {
        if (error != nullptr) {
            *error = QStringLiteral("Недопустимые метаданные справочника");
        }
        return result;
    }

    const QSqlDriver* driver = database.driver();
    const QString escapedSchema =
        driver->escapeIdentifier(schemaName, QSqlDriver::TableName);
    const QString escapedTable =
        driver->escapeIdentifier(tableName, QSqlDriver::TableName);
    const QString escapedValue =
        driver->escapeIdentifier(valueColumn, QSqlDriver::FieldName);
    const QString escapedLabel =
        driver->escapeIdentifier(labelColumn, QSqlDriver::FieldName);

    QSqlQuery query(database);
    if (!query.exec(
            QStringLiteral(
                "SELECT %1::text AS option_value,"
                "       coalesce(%2::text, %1::text) AS option_label"
                "  FROM %3.%4"
                " WHERE %1 IS NOT NULL"
                " ORDER BY option_label, option_value")
                .arg(
                    escapedValue,
                    escapedLabel,
                    escapedSchema,
                    escapedTable))) {
        if (error != nullptr) {
            *error = query.lastError().text();
        }
        return result;
    }
    while (query.next()) {
        ObjectFieldOption option;
        option.value = query.value(0).toString();
        option.label = query.value(1).toString();
        result.append(std::move(option));
    }
    return result;
}

}  // namespace

ObjectDetails ObjectRepository::load(
    const QSqlDatabase& database,
    const QString& classTable,
    qint64 id) const
{
    ObjectDetails result;
    result.id = id;
    result.classTable = classTable;

    if (!safeIdentifier().match(classTable).hasMatch()) {
        result.error = QStringLiteral("Недопустимое имя таблицы");
        return result;
    }
    if (!tableIsPublished(database, classTable, &result.error)) {
        return result;
    }

    QSqlQuery capabilityQuery(database);
    capabilityQuery.prepare(QStringLiteral(
        "SELECT can_split, can_join"
        "  FROM meta.layer_catalog"
        " WHERE schema_name = 'net' AND table_name = :table_name"));
    capabilityQuery.bindValue(QStringLiteral(":table_name"), classTable);
    if (!capabilityQuery.exec() || !capabilityQuery.next()) {
        result.error = capabilityQuery.lastError().text();
        return result;
    }
    result.canSplit = capabilityQuery.value(0).toBool();
    result.canJoin = capabilityQuery.value(1).toBool();

    const QString escapedTable = database.driver()->escapeIdentifier(
        classTable, QSqlDriver::TableName);
    QSqlQuery objectQuery(database);
    objectQuery.prepare(
        QStringLiteral(
            "SELECT jsonb_object_agg(e.key, e.value)"
            "  FROM net.%1 AS t"
            " CROSS JOIN LATERAL"
            "       jsonb_each_text(to_jsonb(t) - 'geom') AS e(key, value)"
            " WHERE t.id = :id"
            " GROUP BY t.id")
            .arg(escapedTable));
    objectQuery.bindValue(QStringLiteral(":id"), id);
    if (!objectQuery.exec()) {
        result.error = objectQuery.lastError().text();
        return result;
    }
    if (!objectQuery.next()) {
        result.error = QStringLiteral("Объект не найден");
        return result;
    }

    QByteArray jsonBytes = objectQuery.value(0).toByteArray();
    if (jsonBytes.isEmpty()) {
        jsonBytes = objectQuery.value(0).toString().toUtf8();
    }
    QJsonParseError parseError;
    const QJsonDocument document =
        QJsonDocument::fromJson(jsonBytes, &parseError);
    if (parseError.error != QJsonParseError::NoError
        || !document.isObject()) {
        result.error =
            QStringLiteral("Не удалось разобрать атрибуты объекта: %1")
                .arg(parseError.errorString());
        return result;
    }
    const QJsonObject values = document.object();
    result.rowVersion =
        values.value(QStringLiteral("row_version")).toVariant().toLongLong();

    QSqlQuery columnQuery(database);
    columnQuery.prepare(QStringLiteral(
        "SELECT a.attname,"
        "       format_type(a.atttypid, a.atttypmod),"
        "       coalesce("
        "           nullif(fc.display_name, ''),"
        "           nullif(col_description(c.oid, a.attnum), ''),"
        "           a.attname"
        "       ),"
        "       t.typname,"
        "       a.attgenerated = ''"
        "       AND a.attidentity = ''"
        "       AND a.attname NOT IN ("
        "           'id', 'fragment_id', 'node_from', 'node_to',"
        "           'node_from_src', 'node_to_src', 'fileid_src', 'src_id',"
        "           'subtype_src_id',"
        "           'removed_at', 'row_version', 'updated_at', 'updated_by',"
        "           'coords_legacy'"
        "       )"
        "       AND t.typname IN ("
        "           'int2', 'int4', 'int8', 'float4', 'float8', 'numeric',"
        "           'bool', 'text', 'varchar', 'bpchar', 'date',"
        "           'timestamp', 'timestamptz', 'time', 'timetz', 'uuid',"
        "           'json', 'jsonb'"
        "       )"
        "       AND coalesce(fc.is_editable, true) AS editable,"
        "       coalesce(fc.unit, ''),"
        "       coalesce("
        "           fc.editor_kind,"
        "           CASE WHEN t.typname = 'bool' THEN 'boolean' ELSE 'text' END"
        "       ),"
        "       coalesce(fc.lookup_schema, ''),"
        "       coalesce(fc.lookup_table, ''),"
        "       coalesce(fc.lookup_value_column, ''),"
        "       coalesce(fc.lookup_label_column, ''),"
        "       coalesce(fc.group_name, '')"
        "  FROM pg_class c"
        "  JOIN pg_namespace n ON n.oid = c.relnamespace"
        "  JOIN pg_attribute a ON a.attrelid = c.oid"
        "  JOIN pg_type t ON t.oid = a.atttypid"
        "  LEFT JOIN meta.field_catalog fc"
        "    ON fc.table_schema = n.nspname"
        "   AND fc.table_name = c.relname"
        "   AND fc.column_name = a.attname"
        " WHERE n.nspname = 'net'"
        "   AND c.relname = :table_name"
        "   AND a.attnum > 0"
        "   AND NOT a.attisdropped"
        "   AND a.attname <> 'geom'"
        "   AND coalesce(fc.is_visible, true)"
        " ORDER BY coalesce(fc.display_order, 10000 + a.attnum), a.attnum"));
    columnQuery.bindValue(QStringLiteral(":table_name"), classTable);
    if (!columnQuery.exec()) {
        result.error = columnQuery.lastError().text();
        return result;
    }

    while (columnQuery.next()) {
        ObjectAttribute attribute;
        attribute.name = columnQuery.value(0).toString();
        attribute.dataType = columnQuery.value(1).toString();
        attribute.displayName = columnQuery.value(2).toString().trimmed();
        attribute.databaseType = columnQuery.value(3).toString();
        attribute.editable = columnQuery.value(4).toBool();
        attribute.unit = columnQuery.value(5).toString().trimmed();
        attribute.editorKind = columnQuery.value(6).toString().trimmed();
        const QString lookupSchema = columnQuery.value(7).toString();
        const QString lookupTable = columnQuery.value(8).toString();
        const QString lookupValueColumn = columnQuery.value(9).toString();
        const QString lookupLabelColumn = columnQuery.value(10).toString();
        attribute.groupName = columnQuery.value(11).toString().trimmed();
        const QJsonValue value = values.value(attribute.name);
        attribute.isNull = value.isNull() || value.isUndefined();
        if (attribute.name == QStringLiteral("removed_at")) {
            result.canArchive = true;
            result.archived = !attribute.isNull;
        }
        if (attribute.databaseType == QStringLiteral("bool")
            && !attribute.isNull && value.isString()) {
            attribute.value =
                value.toString() == QStringLiteral("true")
                    ? QStringLiteral("да")
                    : QStringLiteral("нет");
        } else {
            attribute.value = jsonValueToString(value);
        }
        if (attribute.editorKind == QStringLiteral("lookup")) {
            QString lookupError;
            attribute.options = loadLookupOptions(
                database,
                lookupSchema,
                lookupTable,
                lookupValueColumn,
                lookupLabelColumn,
                &lookupError);
            if (!lookupError.isEmpty()) {
                result.error =
                    QStringLiteral("Не удалось загрузить справочник для %1: %2")
                        .arg(attribute.name, lookupError);
                return result;
            }
        }
        result.attributes.append(std::move(attribute));
    }

    return result;
}

UpdateResult ObjectRepository::update(
    QSqlDatabase database,
    const QString& classTable,
    qint64 id,
    qint64 expectedVersion,
    const QList<AttributeChange>& changes) const
{
    UpdateResult result;
    result.rowVersion = expectedVersion;

    if (!database.isOpen()) {
        result.error = QStringLiteral("Соединение с БД не открыто");
        return result;
    }
    if (!safeIdentifier().match(classTable).hasMatch()) {
        result.error = QStringLiteral("Недопустимое имя таблицы");
        return result;
    }
    if (!tableIsPublished(database, classTable, &result.error)) {
        return result;
    }
    if (expectedVersion < 1) {
        result.error = QStringLiteral("Некорректная версия объекта");
        return result;
    }
    if (changes.isEmpty()) {
        result.success = true;
        return result;
    }

    QSqlQuery metadataQuery(database);
    metadataQuery.prepare(QStringLiteral(
        "SELECT a.attname, t.typname"
        "  FROM pg_class c"
        "  JOIN pg_namespace n ON n.oid = c.relnamespace"
        "  JOIN pg_attribute a ON a.attrelid = c.oid"
        "  JOIN pg_type t ON t.oid = a.atttypid"
        "  LEFT JOIN meta.field_catalog fc"
        "    ON fc.table_schema = n.nspname"
        "   AND fc.table_name = c.relname"
        "   AND fc.column_name = a.attname"
        " WHERE n.nspname = 'net'"
        "   AND c.relname = :table_name"
        "   AND a.attnum > 0"
        "   AND NOT a.attisdropped"
        "   AND a.attgenerated = ''"
        "   AND a.attidentity = ''"
        "   AND a.attname NOT IN ("
        "       'id', 'fragment_id', 'node_from', 'node_to',"
        "       'node_from_src', 'node_to_src', 'fileid_src', 'src_id',"
        "       'subtype_src_id',"
        "       'removed_at', 'row_version', 'updated_at', 'updated_by',"
        "       'coords_legacy', 'geom'"
        "   )"
        "   AND coalesce(fc.is_visible, true)"
        "   AND coalesce(fc.is_editable, true)"));
    metadataQuery.bindValue(QStringLiteral(":table_name"), classTable);
    if (!metadataQuery.exec()) {
        result.error = metadataQuery.lastError().text();
        return result;
    }

    QHash<QString, QString> editableColumns;
    while (metadataQuery.next()) {
        const QString name = metadataQuery.value(0).toString();
        const QString databaseType = metadataQuery.value(1).toString();
        if (!castForDatabaseType(databaseType).isEmpty()) {
            editableColumns.insert(name, databaseType);
        }
    }

    QStringList assignments;
    QList<QVariant> bindValues;
    for (const AttributeChange& change : changes) {
        if (!safeIdentifier().match(change.name).hasMatch()
            || !editableColumns.contains(change.name)
            || editableColumns.value(change.name) != change.databaseType) {
            result.error =
                QStringLiteral("Поле «%1» нельзя редактировать").arg(change.name);
            return result;
        }

        const QString escapedColumn = database.driver()->escapeIdentifier(
            change.name, QSqlDriver::FieldName);
        if (change.setNull) {
            assignments.append(
                QStringLiteral("%1 = NULL").arg(escapedColumn));
        } else {
            assignments.append(
                QStringLiteral("%1 = CAST(? AS %2)")
                    .arg(escapedColumn,
                         castForDatabaseType(change.databaseType)));
            bindValues.append(valueForBinding(change));
        }
    }

    const QString escapedTable = database.driver()->escapeIdentifier(
        classTable, QSqlDriver::TableName);
    QSqlQuery updateQuery(database);
    updateQuery.prepare(
        QStringLiteral(
            "UPDATE net.%1 SET %2"
            " WHERE id = ? AND row_version = ?"
            " RETURNING row_version")
            .arg(escapedTable, assignments.join(QStringLiteral(", "))));
    for (const QVariant& value : std::as_const(bindValues)) {
        updateQuery.addBindValue(value);
    }
    updateQuery.addBindValue(id);
    updateQuery.addBindValue(expectedVersion);

    if (!database.transaction()) {
        result.error = database.lastError().text();
        return result;
    }
    if (!updateQuery.exec()) {
        result.error = updateQuery.lastError().text();
        database.rollback();
        return result;
    }
    if (!updateQuery.next()) {
        result.conflict = true;
        result.error =
            QStringLiteral("Объект уже изменён другим пользователем или удалён");
        database.rollback();
        return result;
    }

    result.rowVersion = updateQuery.value(0).toLongLong();
    if (!database.commit()) {
        result.error = database.lastError().text();
        database.rollback();
        return result;
    }
    result.success = true;
    return result;
}

QList<ObjectVersion> ObjectRepository::loadVersions(
    const QSqlDatabase& database,
    const QString& classTable,
    const QList<qint64>& ids,
    QString* error) const
{
    QList<ObjectVersion> result;
    if (error != nullptr) {
        error->clear();
    }
    if (!database.isOpen()) {
        if (error != nullptr) {
            *error = QStringLiteral("Соединение с БД не открыто");
        }
        return result;
    }
    if (!safeIdentifier().match(classTable).hasMatch()
        || ids.isEmpty() || ids.size() > 500) {
        if (error != nullptr) {
            *error = QStringLiteral("Некорректный набор объектов");
        }
        return result;
    }
    QString publicationError;
    if (!tableIsPublished(database, classTable, &publicationError)) {
        if (error != nullptr) {
            *error = publicationError;
        }
        return result;
    }

    QSet<qint64> uniqueIds;
    QStringList placeholders;
    placeholders.reserve(ids.size());
    for (const qint64 id : ids) {
        if (id <= 0 || uniqueIds.contains(id)) {
            if (error != nullptr) {
                *error = QStringLiteral(
                    "ID объектов должны быть положительными и уникальными");
            }
            return {};
        }
        uniqueIds.insert(id);
        placeholders.append(QStringLiteral("CAST(? AS bigint)"));
    }

    const QString escapedTable = database.driver()->escapeIdentifier(
        classTable, QSqlDriver::TableName);
    QSqlQuery query(database);
    query.prepare(
        QStringLiteral(
            "SELECT id, row_version FROM net.%1"
            " WHERE removed_at IS NULL AND id IN (%2)"
            " ORDER BY id")
            .arg(escapedTable, placeholders.join(QStringLiteral(", "))));
    for (const qint64 id : ids) {
        query.addBindValue(id);
    }
    if (!query.exec()) {
        if (error != nullptr) {
            *error = query.lastError().text();
        }
        return {};
    }
    while (query.next()) {
        result.append({query.value(0).toLongLong(),
                       query.value(1).toLongLong()});
    }
    if (result.size() != ids.size()) {
        if (error != nullptr) {
            *error = QStringLiteral(
                "Не все выбранные объекты существуют или активны");
        }
        return {};
    }
    return result;
}

BatchUpdateResult ObjectRepository::batchUpdate(
    QSqlDatabase database,
    const QString& classTable,
    const QList<ObjectVersion>& objects,
    const AttributeChange& change) const
{
    BatchUpdateResult result;
    if (!database.isOpen()) {
        result.error = QStringLiteral("Соединение с БД не открыто");
        return result;
    }
    if (!safeIdentifier().match(classTable).hasMatch()
        || objects.size() < 2 || objects.size() > 500) {
        result.error = QStringLiteral(
            "Для массового изменения нужно от 2 до 500 объектов");
        return result;
    }
    if (!tableIsPublished(database, classTable, &result.error)) {
        return result;
    }

    QSet<qint64> uniqueIds;
    for (const ObjectVersion& object : objects) {
        if (object.id <= 0 || object.rowVersion <= 0
            || uniqueIds.contains(object.id)) {
            result.error = QStringLiteral(
                "ID и версии объектов должны быть положительными, ID — уникальными");
            return result;
        }
        uniqueIds.insert(object.id);
    }

    QSqlQuery metadataQuery(database);
    metadataQuery.prepare(QStringLiteral(
        "SELECT t.typname"
        "  FROM pg_class c"
        "  JOIN pg_namespace n ON n.oid = c.relnamespace"
        "  JOIN pg_attribute a ON a.attrelid = c.oid"
        "  JOIN pg_type t ON t.oid = a.atttypid"
        "  LEFT JOIN meta.field_catalog fc"
        "    ON fc.table_schema = n.nspname"
        "   AND fc.table_name = c.relname"
        "   AND fc.column_name = a.attname"
        " WHERE n.nspname = 'net'"
        "   AND c.relname = :table_name"
        "   AND a.attname = :column_name"
        "   AND a.attnum > 0 AND NOT a.attisdropped"
        "   AND a.attgenerated = '' AND a.attidentity = ''"
        "   AND a.attname NOT IN ("
        "       'id', 'fragment_id', 'node_from', 'node_to',"
        "       'node_from_src', 'node_to_src', 'fileid_src', 'src_id',"
        "       'subtype_src_id',"
        "       'removed_at', 'row_version', 'updated_at', 'updated_by',"
        "       'coords_legacy', 'geom')"
        "   AND coalesce(fc.is_visible, true)"
        "   AND coalesce(fc.is_editable, true)"));
    metadataQuery.bindValue(QStringLiteral(":table_name"), classTable);
    metadataQuery.bindValue(QStringLiteral(":column_name"), change.name);
    if (!metadataQuery.exec() || !metadataQuery.next()) {
        result.error = metadataQuery.lastError().text().isEmpty()
                           ? QStringLiteral("Поле нельзя редактировать")
                           : metadataQuery.lastError().text();
        return result;
    }
    const QString databaseType = metadataQuery.value(0).toString();
    const QString cast = castForDatabaseType(databaseType);
    if (!safeIdentifier().match(change.name).hasMatch()
        || databaseType != change.databaseType || cast.isEmpty()) {
        result.error =
            QStringLiteral("Поле «%1» нельзя редактировать").arg(change.name);
        return result;
    }

    QStringList expectedRows;
    expectedRows.reserve(objects.size());
    for (qsizetype index = 0; index < objects.size(); ++index) {
        expectedRows.append(QStringLiteral(
            "(CAST(? AS bigint), CAST(? AS bigint))"));
    }
    const QString escapedTable = database.driver()->escapeIdentifier(
        classTable, QSqlDriver::TableName);
    const QString escapedColumn = database.driver()->escapeIdentifier(
        change.name, QSqlDriver::FieldName);
    const QString assignment =
        change.setNull
            ? QStringLiteral("%1 = NULL").arg(escapedColumn)
            : QStringLiteral("%1 = CAST(? AS %2)")
                  .arg(escapedColumn, cast);
    QSqlQuery query(database);
    query.prepare(
        QStringLiteral(
            "UPDATE net.%1 AS target SET %2"
            " FROM (VALUES %3) AS expected(id, row_version)"
            " WHERE target.id = expected.id"
            "   AND target.row_version = expected.row_version"
            "   AND target.removed_at IS NULL"
            " RETURNING target.id")
            .arg(escapedTable,
                 assignment,
                 expectedRows.join(QStringLiteral(", "))));
    if (!change.setNull) {
        query.addBindValue(valueForBinding(change));
    }
    for (const ObjectVersion& object : objects) {
        query.addBindValue(object.id);
        query.addBindValue(object.rowVersion);
    }

    if (!database.transaction()) {
        result.error = database.lastError().text();
        return result;
    }
    if (!query.exec()) {
        result.error = query.lastError().text();
        database.rollback();
        return result;
    }
    while (query.next()) {
        ++result.updatedCount;
    }
    if (result.updatedCount != objects.size()) {
        result.conflict = true;
        result.error = QStringLiteral(
            "Хотя бы один объект уже изменён, архивирован или удалён");
        result.updatedCount = 0;
        database.rollback();
        return result;
    }
    if (!database.commit()) {
        result.error = database.lastError().text();
        result.updatedCount = 0;
        database.rollback();
        return result;
    }
    result.success = true;
    return result;
}

UpdateResult ObjectRepository::setArchived(
    QSqlDatabase database,
    const QString& classTable,
    qint64 id,
    qint64 expectedVersion,
    bool archived) const
{
    UpdateResult result;
    result.rowVersion = expectedVersion;

    if (!database.isOpen()) {
        result.error = QStringLiteral("Соединение с БД не открыто");
        return result;
    }
    if (!safeIdentifier().match(classTable).hasMatch()) {
        result.error = QStringLiteral("Недопустимое имя таблицы");
        return result;
    }
    if (!tableIsPublished(database, classTable, &result.error)) {
        return result;
    }

    QSqlQuery columnQuery(database);
    columnQuery.prepare(QStringLiteral(
        "SELECT EXISTS ("
        "  SELECT 1"
        "    FROM information_schema.columns"
        "   WHERE table_schema = 'net'"
        "     AND table_name = :table_name"
        "     AND column_name = 'removed_at'"
        ")"));
    columnQuery.bindValue(QStringLiteral(":table_name"), classTable);
    if (!columnQuery.exec() || !columnQuery.next()) {
        result.error = columnQuery.lastError().text();
        return result;
    }
    if (!columnQuery.value(0).toBool()) {
        result.error =
            QStringLiteral("Этот тип объекта не поддерживает архивирование");
        return result;
    }

    const QString escapedTable = database.driver()->escapeIdentifier(
        classTable, QSqlDriver::TableName);
    QSqlQuery query(database);
    query.prepare(
        QStringLiteral(
            "UPDATE net.%1"
            "   SET removed_at = CASE"
            "       WHEN CAST(? AS boolean) THEN clock_timestamp()"
            "       ELSE NULL END"
            " WHERE id = ?"
            "   AND row_version = ?"
            "   AND (removed_at IS NOT NULL) <> CAST(? AS boolean)"
            " RETURNING row_version")
            .arg(escapedTable));
    query.addBindValue(archived);
    query.addBindValue(id);
    query.addBindValue(expectedVersion);
    query.addBindValue(archived);

    if (!database.transaction()) {
        result.error = database.lastError().text();
        return result;
    }
    if (!query.exec()) {
        result.error = query.lastError().text();
        database.rollback();
        return result;
    }
    if (!query.next()) {
        result.conflict = true;
        result.error =
            QStringLiteral("Состояние или версия объекта уже изменились");
        database.rollback();
        return result;
    }

    result.rowVersion = query.value(0).toLongLong();
    if (!database.commit()) {
        result.error = database.lastError().text();
        database.rollback();
        return result;
    }
    result.success = true;
    return result;
}

QList<ObjectHistoryEntry> ObjectRepository::loadHistory(
    const QSqlDatabase& database,
    const QString& classTable,
    qint64 id,
    QString* error) const
{
    if (!safeIdentifier().match(classTable).hasMatch()) {
        if (error != nullptr) {
            *error = QStringLiteral("Недопустимое имя таблицы");
        }
        return {};
    }
    QString catalogError;
    if (!tableIsPublished(database, classTable, &catalogError)) {
        if (error != nullptr) {
            *error = catalogError;
        }
        return {};
    }

    QSqlQuery query(database);
    query.prepare(QStringLiteral(
        "SELECT row_version,"
        "       operation,"
        "       to_char(changed_at, 'YYYY-MM-DD HH24:MI:SSOF'),"
        "       changed_by,"
        "       coalesce(application_name, ''),"
        "       changed_fields::text"
        "  FROM meta.object_change_log"
        " WHERE schema_name = 'net'"
        "   AND table_name = :table_name"
        "   AND object_id = :object_id"
        " ORDER BY row_version DESC, id DESC"));
    query.bindValue(QStringLiteral(":table_name"), classTable);
    query.bindValue(QStringLiteral(":object_id"), id);
    if (!query.exec()) {
        if (error != nullptr) {
            *error = query.lastError().text();
        }
        return {};
    }

    QList<ObjectHistoryEntry> entries;
    while (query.next()) {
        ObjectHistoryEntry entry;
        entry.rowVersion = query.value(0).toLongLong();
        entry.operation = query.value(1).toString();
        entry.changedAt = query.value(2).toString();
        entry.changedBy = query.value(3).toString();
        entry.applicationName = query.value(4).toString();
        entry.changedFields = query.value(5).toString();
        entries.append(std::move(entry));
    }
    if (error != nullptr) {
        error->clear();
    }
    return entries;
}

QList<ArchivedObjectInfo> ObjectRepository::loadArchived(
    const QSqlDatabase& database,
    QString* error) const
{
    QSqlQuery tableQuery(database);
    if (!tableQuery.exec(QStringLiteral(
            "SELECT l.table_name,"
            "       upper(l.geometry_type) IN ('POINT', 'MULTIPOINT')"
            "  FROM meta.layer_catalog AS l"
            " WHERE l.schema_name = 'net'"
            "   AND EXISTS ("
            "       SELECT 1 FROM information_schema.columns AS c"
            "        WHERE c.table_schema = l.schema_name"
            "          AND c.table_name = l.table_name"
            "          AND c.column_name = 'removed_at'"
            "   )"
            " ORDER BY l.sort_order, l.table_name"))) {
        if (error != nullptr) {
            *error = tableQuery.lastError().text();
        }
        return {};
    }

    QStringList branches;
    while (tableQuery.next()) {
        const QString tableName = tableQuery.value(0).toString();
        if (!safeIdentifier().match(tableName).hasMatch()) {
            continue;
        }
        const QString escapedTable = database.driver()->escapeIdentifier(
            tableName, QSqlDriver::TableName);
        const QString isNode =
            tableQuery.value(1).toBool()
                ? QStringLiteral("true")
                : QStringLiteral("false");
        branches.append(
            QStringLiteral(
                "SELECT '%1'::text AS class_table,"
                "       t.id,"
                "       coalesce(nullif(to_jsonb(t)->>'fragment_id', '')::int,"
                "                0) AS fragment_id,"
                "       coalesce(nullif(to_jsonb(t)->>'nodename', ''),"
                "                nullif(to_jsonb(t)->>'name', ''),"
                "                nullif(to_jsonb(t)->>'memo', ''), '')"
                "           AS label,"
                "       to_char(t.removed_at, 'YYYY-MM-DD HH24:MI:SSOF')"
                "           AS archived_at,"
                "       %2 AS is_node"
                "  FROM net.%3 AS t"
                " WHERE t.removed_at IS NOT NULL")
                .arg(tableName, isNode, escapedTable));
    }

    if (branches.isEmpty()) {
        if (error != nullptr) {
            error->clear();
        }
        return {};
    }

    QSqlQuery query(database);
    if (!query.exec(
            QStringLiteral(
                "SELECT * FROM (%1) AS archived"
                " ORDER BY archived_at DESC, class_table, id"
                " LIMIT 10000")
                .arg(branches.join(QStringLiteral(" UNION ALL "))))) {
        if (error != nullptr) {
            *error = query.lastError().text();
        }
        return {};
    }

    QList<ArchivedObjectInfo> objects;
    while (query.next()) {
        ArchivedObjectInfo object;
        object.classTable = query.value(0).toString();
        object.id = query.value(1).toLongLong();
        object.fragmentId = query.value(2).toInt();
        object.label = query.value(3).toString();
        object.archivedAt = query.value(4).toString();
        object.isNode = query.value(5).toBool();
        objects.append(std::move(object));
    }
    if (error != nullptr) {
        error->clear();
    }
    return objects;
}

CreateObjectResult ObjectRepository::createPoint(
    QSqlDatabase database,
    const QString& classTable,
    int fragmentId,
    const QPointF& position) const
{
    CreateObjectResult result;
    if (!database.isOpen()) {
        result.error = QStringLiteral("Соединение с БД не открыто");
        return result;
    }
    if (!safeIdentifier().match(classTable).hasMatch()) {
        result.error = QStringLiteral("Недопустимое имя таблицы");
        return result;
    }
    if (fragmentId <= 0
        || !std::isfinite(position.x())
        || !std::isfinite(position.y())) {
        result.error = QStringLiteral("Некорректный фрагмент или координаты");
        return result;
    }
    if (!tableIsPublished(database, classTable, &result.error)) {
        return result;
    }

    QSqlQuery metadataQuery(database);
    metadataQuery.prepare(QStringLiteral(
        "SELECT upper(l.geometry_type), l.srid, l.is_editable,"
        "       EXISTS ("
        "           SELECT 1 FROM information_schema.columns AS c"
        "            WHERE c.table_schema = 'net'"
        "              AND c.table_name = l.table_name"
        "              AND c.column_name = 'fragment_id'"
        "       )"
        "  FROM meta.layer_catalog AS l"
        " WHERE l.schema_name = 'net'"
        "   AND l.table_name = :table_name"));
    metadataQuery.bindValue(QStringLiteral(":table_name"), classTable);
    if (!metadataQuery.exec() || !metadataQuery.next()) {
        result.error = metadataQuery.lastError().text();
        return result;
    }
    if (metadataQuery.value(0).toString() != QStringLiteral("POINT")
        || !metadataQuery.value(2).toBool()
        || !metadataQuery.value(3).toBool()) {
        result.error = QStringLiteral(
            "Создание поддерживается только для редактируемых "
            "точечных классов с fragment_id");
        return result;
    }
    const int srid = metadataQuery.value(1).toInt();
    if (srid <= 0) {
        result.error = QStringLiteral("Некорректный SRID слоя");
        return result;
    }

    QSqlQuery fragmentQuery(database);
    fragmentQuery.prepare(QStringLiteral(
        "SELECT EXISTS (SELECT 1 FROM net.fragment"
        "               WHERE id = :fragment_id"
        "                 AND removed_at IS NULL)"));
    fragmentQuery.bindValue(QStringLiteral(":fragment_id"), fragmentId);
    if (!fragmentQuery.exec() || !fragmentQuery.next()) {
        result.error = fragmentQuery.lastError().text();
        return result;
    }
    if (!fragmentQuery.value(0).toBool()) {
        result.error = QStringLiteral("Фрагмент не найден или архивирован");
        return result;
    }

    const QString escapedTable = database.driver()->escapeIdentifier(
        classTable, QSqlDriver::TableName);
    QSqlQuery insertQuery(database);
    insertQuery.prepare(
        QStringLiteral(
            "INSERT INTO net.%1 (fragment_id, geom)"
            " VALUES (?, ST_SetSRID("
            "     ST_MakePoint(CAST(? AS double precision),"
            "                  CAST(? AS double precision)), %2))"
            " RETURNING id, row_version")
            .arg(escapedTable)
            .arg(srid));
    insertQuery.addBindValue(fragmentId);
    insertQuery.addBindValue(position.x());
    insertQuery.addBindValue(position.y());

    if (!database.transaction()) {
        result.error = database.lastError().text();
        return result;
    }
    if (!insertQuery.exec() || !insertQuery.next()) {
        result.error = insertQuery.lastError().text();
        database.rollback();
        return result;
    }
    result.id = insertQuery.value(0).toLongLong();
    result.rowVersion = insertQuery.value(1).toLongLong();
    if (!database.commit()) {
        result.error = database.lastError().text();
        database.rollback();
        result.id = 0;
        return result;
    }
    result.success = true;
    return result;
}

CreateObjectResult ObjectRepository::createLine(
    QSqlDatabase database,
    const QString& classTable,
    int fragmentId,
    qint64 nodeFrom,
    qint64 nodeTo) const
{
    CreateObjectResult result;
    if (!database.isOpen()) {
        result.error = QStringLiteral("Соединение с БД не открыто");
        return result;
    }
    if (!safeIdentifier().match(classTable).hasMatch()) {
        result.error = QStringLiteral("Недопустимое имя таблицы");
        return result;
    }
    if (fragmentId <= 0 || nodeFrom <= 0 || nodeTo <= 0
        || nodeFrom == nodeTo) {
        result.error =
            QStringLiteral("Некорректный фрагмент или конечные узлы");
        return result;
    }
    if (!tableIsPublished(database, classTable, &result.error)) {
        return result;
    }

    QSqlQuery metadataQuery(database);
    metadataQuery.prepare(QStringLiteral(
        "SELECT upper(l.geometry_type), l.is_editable,"
        "       count(c.column_name) = 3"
        "  FROM meta.layer_catalog AS l"
        "  LEFT JOIN information_schema.columns AS c"
        "    ON c.table_schema = l.schema_name"
        "   AND c.table_name = l.table_name"
        "   AND c.column_name IN ('fragment_id', 'node_from', 'node_to')"
        " WHERE l.schema_name = 'net'"
        "   AND l.table_name = :table_name"
        " GROUP BY l.geometry_type, l.is_editable"));
    metadataQuery.bindValue(QStringLiteral(":table_name"), classTable);
    if (!metadataQuery.exec() || !metadataQuery.next()) {
        result.error = metadataQuery.lastError().text();
        return result;
    }
    if (metadataQuery.value(0).toString() != QStringLiteral("LINESTRING")
        || !metadataQuery.value(1).toBool()
        || !metadataQuery.value(2).toBool()) {
        result.error = QStringLiteral(
            "Создание поддерживается только для сетевых классов LineString");
        return result;
    }

    const QString escapedTable = database.driver()->escapeIdentifier(
        classTable, QSqlDriver::TableName);
    QSqlQuery insertQuery(database);
    insertQuery.prepare(
        QStringLiteral(
            "INSERT INTO net.%1 (fragment_id, node_from, node_to)"
            " VALUES (?, ?, ?)"
            " RETURNING id, row_version")
            .arg(escapedTable));
    insertQuery.addBindValue(fragmentId);
    insertQuery.addBindValue(nodeFrom);
    insertQuery.addBindValue(nodeTo);

    if (!database.transaction()) {
        result.error = database.lastError().text();
        return result;
    }
    if (!insertQuery.exec() || !insertQuery.next()) {
        result.error = insertQuery.lastError().text();
        database.rollback();
        return result;
    }
    result.id = insertQuery.value(0).toLongLong();
    result.rowVersion = insertQuery.value(1).toLongLong();
    if (!database.commit()) {
        result.error = database.lastError().text();
        database.rollback();
        result.id = 0;
        return result;
    }
    result.success = true;
    return result;
}

SplitLineResult ObjectRepository::splitLine(
    QSqlDatabase database,
    const QString& classTable,
    qint64 id,
    qint64 expectedVersion,
    const QPointF& position) const
{
    SplitLineResult result;
    if (!database.isOpen()) {
        result.error = QStringLiteral("Соединение с БД не открыто");
        return result;
    }
    if (!safeIdentifier().match(classTable).hasMatch()) {
        result.error = QStringLiteral("Недопустимое имя таблицы");
        return result;
    }
    if (id <= 0 || expectedVersion <= 0
        || !std::isfinite(position.x())
        || !std::isfinite(position.y())) {
        result.error = QStringLiteral("Некорректные параметры разрезания");
        return result;
    }
    if (!tableIsPublished(database, classTable, &result.error)) {
        return result;
    }
    if (!database.transaction()) {
        result.error = database.lastError().text();
        return result;
    }

    QSqlQuery query(database);
    query.prepare(QStringLiteral(
        "SELECT node_id, node_created, first_line_id, second_line_id,"
        "       split_fraction"
        "  FROM net.split_line("
        "      CAST(? AS text), CAST(? AS bigint), CAST(? AS bigint),"
        "      CAST(? AS double precision), CAST(? AS double precision)"
        "  )"));
    query.addBindValue(classTable);
    query.addBindValue(id);
    query.addBindValue(expectedVersion);
    query.addBindValue(position.x());
    query.addBindValue(position.y());
    if (!query.exec() || !query.next()) {
        result.error = query.lastError().text();
        result.conflict = result.error.contains(
            QStringLiteral("CONFLICT:"), Qt::CaseInsensitive);
        database.rollback();
        return result;
    }

    result.nodeId = query.value(0).toLongLong();
    result.nodeCreated = query.value(1).toBool();
    result.firstLineId = query.value(2).toLongLong();
    result.secondLineId = query.value(3).toLongLong();
    result.splitFraction = query.value(4).toDouble();
    if (result.nodeId <= 0 || result.firstLineId <= 0
        || result.secondLineId <= 0) {
        result.error = QStringLiteral(
            "Сервер не вернул созданные объекты разрезания");
        database.rollback();
        return result;
    }
    if (!database.commit()) {
        result.error = database.lastError().text();
        database.rollback();
        return result;
    }

    result.success = true;
    return result;
}

JoinLinesResult ObjectRepository::joinLines(
    QSqlDatabase database,
    const QString& classTable,
    qint64 firstId,
    qint64 firstExpectedVersion,
    qint64 secondId,
    qint64 secondExpectedVersion) const
{
    JoinLinesResult result;
    if (!database.isOpen()) {
        result.error = QStringLiteral("Соединение с БД не открыто");
        return result;
    }
    if (!safeIdentifier().match(classTable).hasMatch()) {
        result.error = QStringLiteral("Недопустимое имя таблицы");
        return result;
    }
    if (firstId <= 0 || secondId <= 0 || firstId == secondId
        || firstExpectedVersion <= 0 || secondExpectedVersion <= 0) {
        result.error = QStringLiteral("Некорректные параметры соединения");
        return result;
    }
    if (!tableIsPublished(database, classTable, &result.error)) {
        return result;
    }
    if (!database.transaction()) {
        result.error = database.lastError().text();
        return result;
    }

    QSqlQuery query(database);
    query.prepare(QStringLiteral(
        "SELECT joined_line_id, archived_node_id"
        "  FROM net.join_lines("
        "      CAST(? AS text), CAST(? AS bigint), CAST(? AS bigint),"
        "      CAST(? AS bigint), CAST(? AS bigint)"
        "  )"));
    query.addBindValue(classTable);
    query.addBindValue(firstId);
    query.addBindValue(firstExpectedVersion);
    query.addBindValue(secondId);
    query.addBindValue(secondExpectedVersion);
    if (!query.exec() || !query.next()) {
        result.error = query.lastError().text();
        result.conflict = result.error.contains(
            QStringLiteral("CONFLICT:"), Qt::CaseInsensitive);
        database.rollback();
        return result;
    }

    result.joinedLineId = query.value(0).toLongLong();
    result.archivedNodeId = query.value(1).toLongLong();
    if (result.joinedLineId <= 0 || result.archivedNodeId <= 0) {
        result.error = QStringLiteral(
            "Сервер не вернул результат соединения участков");
        database.rollback();
        return result;
    }
    if (!database.commit()) {
        result.error = database.lastError().text();
        database.rollback();
        return result;
    }

    result.success = true;
    return result;
}

}  // namespace tgid::repo
