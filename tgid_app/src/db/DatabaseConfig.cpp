#include "db/DatabaseConfig.h"

#include <QByteArray>
#include <QStringList>

namespace tgid::db {
namespace {

QString environmentValue(const char* name, const QString& fallback = {})
{
    const QByteArray value = qgetenv(name);
    return value.isEmpty() ? fallback : QString::fromUtf8(value);
}

}  // namespace

DatabaseConfig DatabaseConfig::fromEnvironment()
{
    DatabaseConfig config;
    config.host = environmentValue("TGID_HOST", QStringLiteral("localhost"));
    config.database = environmentValue("TGID_DB", QStringLiteral("tgid_prod"));
    config.user = environmentValue("TGID_USER");
    config.password = environmentValue("PGPASSWORD");

    bool portIsValid = false;
    const int environmentPort =
        environmentValue("TGID_PORT", QStringLiteral("5440")).toInt(&portIsValid);
    if (portIsValid && environmentPort > 0 && environmentPort <= 65535) {
        config.port = environmentPort;
    }

    return config;
}

bool DatabaseConfig::isComplete() const
{
    return !host.trimmed().isEmpty() && !database.trimmed().isEmpty()
        && !user.trimmed().isEmpty();
}

QString DatabaseConfig::missingFields() const
{
    QStringList fields;
    if (host.trimmed().isEmpty()) {
        fields.append(QStringLiteral("TGID_HOST"));
    }
    if (database.trimmed().isEmpty()) {
        fields.append(QStringLiteral("TGID_DB"));
    }
    if (user.trimmed().isEmpty()) {
        fields.append(QStringLiteral("TGID_USER"));
    }
    return fields.join(QStringLiteral(", "));
}

QString DatabaseConfig::displayName() const
{
    return QStringLiteral("%1@%2:%3/%4")
        .arg(user, host)
        .arg(port)
        .arg(database);
}

}  // namespace tgid::db
