#include <QFile>
#include <QTextStream>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>


QString buildValuesFromCsv(const QString &csvContent, const QChar delimiter = ';')
{
    QStringList lines = csvContent.split('\n', Qt::SkipEmptyParts);
    QStringList rows;
    bool firstLine = true;

    for (QString line : lines) {
        line = line.trimmed();
        if (line.isEmpty())
            continue;

        QStringList parts = line.split(delimiter);

        if (firstLine) {
            firstLine = false; // пропускаем заголовок
            continue;
        }

        QStringList formatted;
        for (QString field : parts) {
            field = field.trimmed();

            bool ok;
            field.toDouble(&ok);  // если число → без кавычек
            if (ok || field.toLower() == "null") {
                formatted << field;
            } else {
                field.replace("'", "''"); // экранируем апостроф
                formatted << QString("'%1'").arg(field);
            }
        }

        rows << QString("(%1)").arg(formatted.join(", "));
    }

    return rows.join(", ");
}

#if 0
QString buildValuesFromCsv(const QString &csvPath)
{
    QFile file(csvPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Не удалось открыть файл" << csvPath;
        return {};
    }

    QTextStream in(&file);
    QStringList rows;
    bool firstLine = true;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        // допустим, csv = id;name_zd
        QStringList parts = line.split(';');

        if (firstLine) { // пропустить заголовок
            firstLine = false;
            continue;
        }

        if (parts.size() < 2)
            continue;

        QString id = parts[0].trimmed();
        QString name = parts[1].trimmed();

        // экранирование одинарных кавычек
        name.replace("'", "''");

        rows << QString("(%1, '%2')").arg(id, name);
    }

    return rows.join(", ");
}

#endif

#if 0

int main()
{
    QString values = buildValuesFromCsv("data.csv");
    if (values.isEmpty())
        return 1;

    // собираем SQL
    QString sql = QString(
        "SELECT MIN(id) AS id, name_zd "
        "FROM (VALUES %1) AS tab(id, name_zd) "
        "GROUP BY name_zd"
    ).arg(values);

    qDebug() << sql;

    // если хотите выполнить в БД:
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL"); // или QODBC/QSQLITE
    db.setDatabaseName("yourdb");
    db.setUserName("postgres");
    db.setPassword("password");
    if (!db.open()) {
        qWarning() << "Не удалось открыть БД";
        return 1;
    }

    QSqlQuery query(db);
    if (!query.exec(sql)) {
        qWarning() << "Ошибка SQL:" << query.lastError().text();
        return 1;
    }

    while (query.next()) {
        int id = query.value("id").toInt();
        QString name = query.value("name_zd").toString();
        qDebug() << id << name;
    }

    return 0;
}

#endif