#pragma once

#include <QString>

namespace tgid::db {

struct DatabaseConfig final {
    QString host;
    int port = 5440;
    QString database;
    QString user;
    QString password;

    [[nodiscard]] static DatabaseConfig fromEnvironment();
    [[nodiscard]] bool isComplete() const;
    [[nodiscard]] QString missingFields() const;
    [[nodiscard]] QString displayName() const;
};

}  // namespace tgid::db
