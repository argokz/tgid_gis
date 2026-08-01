#include "repo/SearchRepository.h"

#include <QHash>
#include <QRegularExpression>
#include <QSet>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include <utility>

namespace tgid::repo {
namespace {

const QRegularExpression& safeIdentifier()
{
    static const QRegularExpression expression(
        QStringLiteral("^[a-z_][a-z0-9_]*$"));
    return expression;
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

QVariant normalizedValue(const QString& databaseType, const QString& value)
{
    if (databaseType != QStringLiteral("bool")) {
        QString normalized = value;
        if (databaseType == QStringLiteral("float4")
            || databaseType == QStringLiteral("float8")
            || databaseType == QStringLiteral("numeric")) {
            normalized.replace(',', '.');
        }
        return normalized;
    }
    const QString normalized = value.trimmed().toLower();
    if (normalized == QStringLiteral("да")
        || normalized == QStringLiteral("yes") || normalized == "1") {
        return QStringLiteral("true");
    }
    if (normalized == QStringLiteral("нет")
        || normalized == QStringLiteral("no") || normalized == "0") {
        return QStringLiteral("false");
    }
    return normalized;
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
    const QString schema =
        driver->escapeIdentifier(schemaName, QSqlDriver::TableName);
    const QString table =
        driver->escapeIdentifier(tableName, QSqlDriver::TableName);
    const QString value =
        driver->escapeIdentifier(valueColumn, QSqlDriver::FieldName);
    const QString label =
        driver->escapeIdentifier(labelColumn, QSqlDriver::FieldName);
    QSqlQuery query(database);
    if (!query.exec(
            QStringLiteral(
                "SELECT %1::text, coalesce(%2::text, %1::text)"
                "  FROM %3.%4 WHERE %1 IS NOT NULL"
                " ORDER BY 2, 1")
                .arg(value, label, schema, table))) {
        if (error != nullptr) {
            *error = query.lastError().text();
        }
        return result;
    }
    while (query.next()) {
        result.append(
            {query.value(0).toString(), query.value(1).toString()});
    }
    return result;
}

}  // namespace

QList<SearchField> SearchRepository::loadFields(
    const QSqlDatabase& database,
    const QString& classTable,
    QString* error) const
{
    QList<SearchField> result;
    if (error != nullptr) {
        error->clear();
    }
    if (!database.isOpen()
        || !safeIdentifier().match(classTable).hasMatch()) {
        if (error != nullptr) {
            *error = QStringLiteral("Некорректный класс поиска");
        }
        return result;
    }
    QSqlQuery query(database);
    query.prepare(QStringLiteral(
        "SELECT a.attname,"
        "       coalesce(nullif(fc.display_name, ''), a.attname),"
        "       coalesce(fc.unit, ''), t.typname,"
        "       coalesce(fc.editor_kind,"
        "           CASE"
        "             WHEN t.typname = 'bool' THEN 'boolean'"
        "             WHEN t.typname IN ('int2','int4','int8') THEN 'integer'"
        "             WHEN t.typname IN ('float4','float8','numeric') THEN 'decimal'"
        "             WHEN t.typname = 'date' THEN 'date'"
        "             WHEN t.typname IN ('timestamp','timestamptz') THEN 'datetime'"
        "             ELSE 'text' END),"
        "       coalesce(fc.lookup_schema, ''),"
        "       coalesce(fc.lookup_table, ''),"
        "       coalesce(fc.lookup_value_column, ''),"
        "       coalesce(fc.lookup_label_column, '')"
        "  FROM meta.layer_catalog layer"
        "  JOIN pg_namespace n ON n.nspname = layer.schema_name"
        "  JOIN pg_class c ON c.relnamespace = n.oid"
        "                 AND c.relname = layer.table_name"
        "  JOIN pg_attribute a ON a.attrelid = c.oid"
        "  JOIN pg_type t ON t.oid = a.atttypid"
        "  LEFT JOIN meta.field_catalog fc"
        "    ON fc.table_schema = n.nspname"
        "   AND fc.table_name = c.relname"
        "   AND fc.column_name = a.attname"
        " WHERE layer.schema_name = 'net'"
        "   AND layer.table_name = :table_name"
        "   AND a.attnum > 0 AND NOT a.attisdropped"
        "   AND a.attname <> 'geom'"
        "   AND coalesce(fc.is_visible, true)"
        " ORDER BY coalesce(fc.display_order, 10000 + a.attnum), a.attnum"));
    query.bindValue(QStringLiteral(":table_name"), classTable);
    if (!query.exec()) {
        if (error != nullptr) {
            *error = query.lastError().text();
        }
        return result;
    }
    while (query.next()) {
        SearchField field;
        field.name = query.value(0).toString();
        field.displayName = query.value(1).toString();
        field.unit = query.value(2).toString();
        field.databaseType = query.value(3).toString();
        field.editorKind = query.value(4).toString();
        if (castForDatabaseType(field.databaseType).isEmpty()) {
            continue;
        }
        if (field.editorKind == QStringLiteral("lookup")) {
            QString lookupError;
            field.options = loadLookupOptions(
                database,
                query.value(5).toString(),
                query.value(6).toString(),
                query.value(7).toString(),
                query.value(8).toString(),
                &lookupError);
            if (!lookupError.isEmpty()) {
                if (error != nullptr) {
                    *error = lookupError;
                }
                return {};
            }
        }
        result.append(std::move(field));
    }
    if (result.isEmpty() && error != nullptr) {
        *error = QStringLiteral("У класса нет доступных полей поиска");
    }
    return result;
}

QList<SearchResult> SearchRepository::search(
    const QSqlDatabase& database,
    const SearchCriteria& criteria,
    QString* error) const
{
    QList<SearchResult> result;
    if (error != nullptr) {
        error->clear();
    }
    QList<SearchCondition> requestedConditions = criteria.conditions;
    if (requestedConditions.isEmpty() && !criteria.fieldName.isEmpty()) {
        requestedConditions.append({criteria.fieldName,
                                    criteria.comparison,
                                    criteria.value,
                                    criteria.secondValue});
    }
    if (!database.isOpen()
        || !safeIdentifier().match(criteria.classTable).hasMatch()
        || requestedConditions.isEmpty() || requestedConditions.size() > 8
        || criteria.limit < 1 || criteria.limit > 1000) {
        if (error != nullptr) {
            *error = QStringLiteral("Некорректные параметры поиска");
        }
        return result;
    }

    static const QString metadataSql = QStringLiteral(
        "SELECT t.typname"
        "  FROM meta.layer_catalog layer"
        "  JOIN pg_namespace n ON n.nspname = layer.schema_name"
        "  JOIN pg_class c ON c.relnamespace = n.oid"
        "                 AND c.relname = layer.table_name"
        "  JOIN pg_attribute a ON a.attrelid = c.oid"
        "  JOIN pg_type t ON t.oid = a.atttypid"
        "  LEFT JOIN meta.field_catalog fc"
        "    ON fc.table_schema = n.nspname"
        "   AND fc.table_name = c.relname"
        "   AND fc.column_name = a.attname"
        " WHERE layer.schema_name = 'net'"
        "   AND layer.table_name = :table_name"
        "   AND a.attname = :field_name"
        "   AND a.attnum > 0 AND NOT a.attisdropped"
        "   AND a.attname <> 'geom'"
        "   AND coalesce(fc.is_visible, true)");

    static const QSet<QString> comparisons = {
        QStringLiteral("equals"), QStringLiteral("contains"),
        QStringLiteral("greater"), QStringLiteral("less"),
        QStringLiteral("between"), QStringLiteral("is_null"),
        QStringLiteral("not_null")};
    static const QSet<QString> orderedTypes = {
        QStringLiteral("int2"), QStringLiteral("int4"),
        QStringLiteral("int8"), QStringLiteral("float4"),
        QStringLiteral("float8"), QStringLiteral("numeric"),
        QStringLiteral("date"), QStringLiteral("timestamp"),
        QStringLiteral("timestamptz"), QStringLiteral("time"),
        QStringLiteral("timetz")};
    const QSqlDriver* driver = database.driver();
    const QString table = driver->escapeIdentifier(
        criteria.classTable, QSqlDriver::TableName);
    QStringList sqlConditions;
    QStringList displayFields;
    QSet<QString> displayedNames;
    QList<QVariant> values;
    for (const SearchCondition& requested : std::as_const(requestedConditions)) {
        if (!safeIdentifier().match(requested.fieldName).hasMatch()
            || !comparisons.contains(requested.comparison)) {
            if (error != nullptr) {
                *error = QStringLiteral("Некорректное условие поиска");
            }
            return result;
        }

        QSqlQuery metadata(database);
        metadata.prepare(metadataSql);
        metadata.bindValue(QStringLiteral(":table_name"), criteria.classTable);
        metadata.bindValue(QStringLiteral(":field_name"), requested.fieldName);
        if (!metadata.exec() || !metadata.next()) {
            if (error != nullptr) {
                *error = metadata.lastError().text().isEmpty()
                             ? QStringLiteral("Поле поиска недоступно: %1")
                                   .arg(requested.fieldName)
                             : metadata.lastError().text();
            }
            return result;
        }
        const QString databaseType = metadata.value(0).toString();
        const QString cast = castForDatabaseType(databaseType);
        if (cast.isEmpty()) {
            if (error != nullptr) {
                *error = QStringLiteral("Тип поля %1 не поддерживается поиском")
                             .arg(requested.fieldName);
            }
            return result;
        }
        if ((requested.comparison == QStringLiteral("greater")
             || requested.comparison == QStringLiteral("less")
             || requested.comparison == QStringLiteral("between"))
            && !orderedTypes.contains(databaseType)) {
            if (error != nullptr) {
                *error = QStringLiteral(
                    "Поле %1 не поддерживает сравнение порядка")
                             .arg(requested.fieldName);
            }
            return result;
        }

        const QString field = driver->escapeIdentifier(
            requested.fieldName, QSqlDriver::FieldName);
        QString sqlCondition;
        if (requested.comparison == QStringLiteral("is_null")) {
            sqlCondition = QStringLiteral("t.%1 IS NULL").arg(field);
        } else if (requested.comparison == QStringLiteral("not_null")) {
            sqlCondition = QStringLiteral("t.%1 IS NOT NULL").arg(field);
        } else if (requested.comparison == QStringLiteral("contains")) {
            sqlCondition = QStringLiteral(
                "CAST(t.%1 AS text) ILIKE '%' || CAST(? AS text) || '%'")
                               .arg(field);
            values.append(requested.value);
        } else if (requested.comparison == QStringLiteral("between")) {
            sqlCondition = QStringLiteral(
                "t.%1 BETWEEN CAST(? AS %2) AND CAST(? AS %2)")
                               .arg(field, cast);
            values.append(normalizedValue(databaseType, requested.value));
            values.append(
                normalizedValue(databaseType, requested.secondValue));
        } else {
            const QString operation =
                requested.comparison == QStringLiteral("greater")
                    ? QStringLiteral(">")
                    : requested.comparison == QStringLiteral("less")
                          ? QStringLiteral("<")
                          : QStringLiteral("=");
            sqlCondition = databaseType == QStringLiteral("json")
                               ? QStringLiteral(
                                     "CAST(t.%1 AS text) %2 CAST(? AS text)")
                                     .arg(field, operation)
                               : QStringLiteral("t.%1 %2 CAST(? AS %3)")
                                     .arg(field, operation, cast);
            values.append(normalizedValue(databaseType, requested.value));
        }
        sqlConditions.append(QStringLiteral("(%1)").arg(sqlCondition));
        if (!displayedNames.contains(requested.fieldName)) {
            displayedNames.insert(requested.fieldName);
            displayFields.append(
                QStringLiteral("'%1', to_jsonb(t.%2)")
                    .arg(requested.fieldName, field));
        }
    }

    const QString displayExpression = QStringLiteral(
        "jsonb_build_object(%1)::text").arg(displayFields.join(", "));

    QSqlQuery query(database);
    query.prepare(
        QStringLiteral(
            "SELECT t.id,"
            "       coalesce(to_jsonb(t)->>'fragment_id', ''),"
            "       %1,"
            "       t.row_version, t.removed_at IS NOT NULL"
            "  FROM net.%2 AS t"
            " WHERE %3 AND (%4)"
            " ORDER BY t.id LIMIT CAST(? AS integer)")
            .arg(displayExpression,
                 table,
                 criteria.includeArchived
                     ? QStringLiteral("TRUE")
                     : QStringLiteral("t.removed_at IS NULL"),
                 sqlConditions.join(QStringLiteral(" AND "))));
    for (const QVariant& value : std::as_const(values)) {
        query.addBindValue(value);
    }
    query.addBindValue(criteria.limit);
    if (!query.exec()) {
        if (error != nullptr) {
            *error = query.lastError().text();
        }
        return result;
    }
    while (query.next()) {
        result.append({query.value(0).toLongLong(),
                       query.value(1).toString(),
                       query.value(2).toString(),
                       query.value(3).toLongLong(),
                       query.value(4).toBool()});
    }
    return result;
}

}  // namespace tgid::repo
