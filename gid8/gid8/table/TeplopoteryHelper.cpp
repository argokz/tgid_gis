#include "qsqlerror.h"
#include "qsqlrecord.h"
#include <QList>
#include <QSqlQuery>


// Функция для выполнения SQL-запроса и получения данных
QList<int> teplopoteryExecuteQuery(const QString& sql, QSqlDatabase* db) {
    QList<int> results;
    QSqlQuery query(*db);

    if (!query.exec(sql)) {
        qWarning() << "Ошибка выполнения SQL-запроса:" << query.lastError().text();
        return results;
    }

    if (query.next()) {
        // Перебираем все колонки в текущей строке результата
        for (int i = 0; i < query.record().count(); ++i) {
            QVariant value = query.value(i);
            if (value.isValid() && !value.isNull() && value.canConvert<int>()) {
                results.append(value.toInt());
            } else {
                qWarning() << "Некорректное значение в поле" << query.record().fieldName(i);
            }
        }
    }

    return results;
}

// Функция для замены плейсхолдеров в JSON
QString teplopoteryReplacePlaceholders(const QString& jsonString, const QList<int>& queryResults) {
    QString updatedJson = jsonString;
    for (int i = 0; i < queryResults.size(); ++i) {
        QString placeholder = QString("$data%1$").arg(i + 1);
        updatedJson.replace(placeholder, QString::number(queryResults[i]));
    }
    return updatedJson;
}

