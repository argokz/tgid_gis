#pragma once

#include "db/DatabaseConfig.h"

#include <QSqlDatabase>
#include <QString>

namespace tgid::db {

struct SchemaStatus final {
    bool valid = false;
    QString database;
    QString user;
    int version = 0;
    QString error;
};

class DatabaseConnection final {
public:
    static constexpr int MinimumSchemaVersion = 6;

    DatabaseConnection();
    ~DatabaseConnection();

    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;

    bool open(const DatabaseConfig& config, QString* error);
    void close();

    [[nodiscard]] bool isOpen() const;
    [[nodiscard]] QSqlDatabase database() const;
    [[nodiscard]] SchemaStatus inspectSchema() const;

private:
    QString connectionName_;
};

}  // namespace tgid::db
