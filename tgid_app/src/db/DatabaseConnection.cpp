#include "db/DatabaseConnection.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QUuid>

namespace tgid::db {

DatabaseConnection::DatabaseConnection()
    : connectionName_(
          QStringLiteral("tgid-app-%1").arg(QUuid::createUuid().toString()))
{
}

DatabaseConnection::~DatabaseConnection()
{
    close();
}

bool DatabaseConnection::open(const DatabaseConfig& config, QString* error)
{
    close();

    if (!config.isComplete()) {
        if (error != nullptr) {
            *error = QStringLiteral("Не заданы параметры: %1")
                         .arg(config.missingFields());
        }
        return false;
    }

    if (!QSqlDatabase::isDriverAvailable(QStringLiteral("QPSQL"))) {
        if (error != nullptr) {
            *error = QStringLiteral(
                "Драйвер QPSQL не найден. Доступные драйверы: %1")
                         .arg(QSqlDatabase::drivers().join(QStringLiteral(", ")));
        }
        return false;
    }

    QSqlDatabase db =
        QSqlDatabase::addDatabase(QStringLiteral("QPSQL"), connectionName_);
    db.setHostName(config.host);
    db.setPort(config.port);
    db.setDatabaseName(config.database);
    db.setUserName(config.user);
    db.setPassword(config.password);
    db.setConnectOptions(
        QStringLiteral("connect_timeout=5;application_name=tgid_app"));

    if (!db.open()) {
        if (error != nullptr) {
            *error = db.lastError().text();
        }
        db = {};
        QSqlDatabase::removeDatabase(connectionName_);
        return false;
    }

    return true;
}

void DatabaseConnection::close()
{
    if (!QSqlDatabase::contains(connectionName_)) {
        return;
    }

    {
        QSqlDatabase db = QSqlDatabase::database(connectionName_, false);
        if (db.isValid()) {
            db.close();
        }
    }
    QSqlDatabase::removeDatabase(connectionName_);
}

bool DatabaseConnection::isOpen() const
{
    if (!QSqlDatabase::contains(connectionName_)) {
        return false;
    }
    return QSqlDatabase::database(connectionName_, false).isOpen();
}

QSqlDatabase DatabaseConnection::database() const
{
    return QSqlDatabase::database(connectionName_, false);
}

SchemaStatus DatabaseConnection::inspectSchema() const
{
    SchemaStatus status;
    if (!isOpen()) {
        status.error = QStringLiteral("Соединение с БД не открыто");
        return status;
    }

    QSqlQuery identityQuery(database());
    if (!identityQuery.exec(
            QStringLiteral("SELECT current_database(), current_user"))
        || !identityQuery.next()) {
        status.error = identityQuery.lastError().text();
        return status;
    }
    status.database = identityQuery.value(0).toString();
    status.user = identityQuery.value(1).toString();

    QSqlQuery contractQuery(database());
    if (!contractQuery.exec(QStringLiteral(
            "SELECT to_regclass('meta.schema_version') IS NOT NULL,"
            "       to_regclass('meta.layer_catalog') IS NOT NULL,"
            "       to_regclass('net.node_reg') IS NOT NULL,"
            "       to_regclass('net.line_reg') IS NOT NULL,"
            "       to_regclass('net.v_map_nodes') IS NOT NULL,"
            "       to_regclass('net.v_map_lines') IS NOT NULL,"
            "       to_regclass('meta.object_change_log') IS NOT NULL,"
            "       to_regclass('meta.field_catalog') IS NOT NULL,"
            "       to_regclass('ref.externalcodes') IS NOT NULL,"
            "       to_regprocedure("
            "           'net.split_line(text,bigint,bigint,double precision,double precision)'"
            "       ) IS NOT NULL,"
            "       to_regprocedure("
            "           'net.join_lines(text,bigint,bigint,bigint,bigint)'"
            "       ) IS NOT NULL,"
            "       to_regprocedure("
            "           'net.move_node(text,bigint,bigint,double precision,double precision)'"
            "       ) IS NOT NULL,"
            "       to_regprocedure("
            "           'net.reclass_object(text,bigint,bigint,text)'"
            "       ) IS NOT NULL,"
            "       NOT EXISTS ("
            "           SELECT 1"
            "             FROM meta.layer_catalog layer"
            "             JOIN pg_namespace layer_namespace"
            "               ON layer_namespace.nspname = layer.schema_name"
            "             JOIN pg_class layer_table"
            "               ON layer_table.relnamespace = layer_namespace.oid"
            "              AND layer_table.relname = layer.table_name"
            "             JOIN pg_attribute layer_column"
            "               ON layer_column.attrelid = layer_table.oid"
            "              AND layer_column.attnum > 0"
            "              AND NOT layer_column.attisdropped"
            "             LEFT JOIN meta.field_catalog field_info"
            "               ON field_info.table_schema = layer.schema_name"
            "              AND field_info.table_name = layer.table_name"
            "              AND field_info.column_name = layer_column.attname"
            "            WHERE layer.schema_name = 'net'"
            "              AND layer_column.attname <> 'geom'"
            "              AND field_info.column_name IS NULL"
            "       ),"
            "       NOT EXISTS ("
            "           SELECT 1"
            "             FROM meta.field_catalog field_info"
            "            WHERE field_info.editor_kind = 'lookup'"
            "              AND ("
            "                  to_regclass(format('%I.%I',"
            "                      field_info.lookup_schema,"
            "                      field_info.lookup_table)) IS NULL"
            "                  OR NOT EXISTS ("
            "                      SELECT 1"
            "                        FROM pg_attribute lookup_col"
            "                       WHERE lookup_col.attrelid = to_regclass("
            "                                 format('%I.%I',"
            "                                     field_info.lookup_schema,"
            "                                     field_info.lookup_table))"
            "                         AND lookup_col.attname ="
            "                             field_info.lookup_value_column"
            "                         AND lookup_col.attnum > 0"
            "                         AND NOT lookup_col.attisdropped"
            "                  )"
            "                  OR NOT EXISTS ("
            "                      SELECT 1"
            "                        FROM pg_attribute lookup_col"
            "                       WHERE lookup_col.attrelid = to_regclass("
            "                                 format('%I.%I',"
            "                                     field_info.lookup_schema,"
            "                                     field_info.lookup_table))"
            "                         AND lookup_col.attname ="
            "                             field_info.lookup_label_column"
            "                         AND lookup_col.attnum > 0"
            "                         AND NOT lookup_col.attisdropped"
            "                  )"
            "              )"
            "       ),"
            "       EXISTS (SELECT 1 FROM public.spatial_ref_sys"
            "               WHERE srid = 9998),"
            "       EXISTS (SELECT 1 FROM information_schema.columns"
            "               WHERE table_schema = 'net'"
            "                 AND table_name = 'node_plain'"
            "                 AND column_name = 'row_version'),"
            "       EXISTS ("
            "           SELECT 1"
            "             FROM pg_trigger"
            "            WHERE tgrelid = 'net.node_plain'::regclass"
            "              AND tgname = 'object_insert_log'"
            "              AND NOT tgisinternal"
            "       ),"
            "       EXISTS ("
            "           SELECT 1"
            "             FROM pg_trigger"
            "            WHERE tgrelid = 'net.line_plain'::regclass"
            "              AND tgname = 'line_topology'"
            "              AND NOT tgisinternal"
            "       ),"
            "       EXISTS ("
            "           SELECT 1"
            "             FROM pg_trigger"
            "            WHERE tgrelid = 'net.node_plain'::regclass"
            "              AND tgname = 'node_geometry_propagate'"
            "              AND NOT tgisinternal"
            "       )"))
        || !contractQuery.next()) {
        status.error = contractQuery.lastError().text();
        return status;
    }

    static const QStringList contractNames = {
        QStringLiteral("meta.schema_version"),
        QStringLiteral("meta.layer_catalog"),
        QStringLiteral("net.node_reg"),
        QStringLiteral("net.line_reg"),
        QStringLiteral("net.v_map_nodes"),
        QStringLiteral("net.v_map_lines"),
        QStringLiteral("meta.object_change_log"),
        QStringLiteral("meta.field_catalog"),
        QStringLiteral("ref.externalcodes"),
        QStringLiteral("net.split_line"),
        QStringLiteral("net.join_lines"),
        QStringLiteral("net.move_node"),
        QStringLiteral("net.reclass_object"),
        QStringLiteral("meta.field_catalog coverage"),
        QStringLiteral("meta.field_catalog lookup targets"),
        QStringLiteral("SRID 9998"),
        QStringLiteral("net.*.row_version"),
        QStringLiteral("net.*.object_insert_log"),
        QStringLiteral("net.*.line_topology"),
        QStringLiteral("net.*.node_geometry_propagate"),
    };
    QStringList missing;
    for (int column = 0; column < contractNames.size(); ++column) {
        if (!contractQuery.value(column).toBool()) {
            missing.append(contractNames.at(column));
        }
    }
    if (!missing.isEmpty()) {
        status.error = QStringLiteral("Неполный контракт БД: %1")
                           .arg(missing.join(QStringLiteral(", ")));
        return status;
    }

    QSqlQuery versionQuery(database());
    if (!versionQuery.exec(
            QStringLiteral("SELECT max(version) FROM meta.schema_version"))
        || !versionQuery.next()) {
        status.error = versionQuery.lastError().text();
        return status;
    }
    status.version = versionQuery.value(0).toInt();
    if (status.version < MinimumSchemaVersion) {
        status.error =
            QStringLiteral("Версия схемы %1, требуется не ниже %2")
                .arg(status.version)
                .arg(MinimumSchemaVersion);
        return status;
    }

    status.valid = true;
    return status;
}

}  // namespace tgid::db
