#include "DataRepository.h"
#include <stdexcept>

DataRepository::DataRepository(QSqlDatabase* db) : m_db(db) {}

std::vector<std::pair<int, QString>> DataRepository::getNasTypes() {
    QSqlQuery query(*m_db);
    QString sql = "SELECT id, tip_nas FROM standardPumps ORDER BY tip_nas";  // Замените на ваш запрос
    if (!query.exec(sql)) {
        throw std::runtime_error("Ошибка в запросе Тип Насоса: " + query.lastError().text().toStdString());
    }

    std::vector<std::pair<int, QString>> result;
    while (query.next()) {
        int id = query.value("id").toInt();
        QString name = query.value("tip_nas").toString();
        result.emplace_back(id, name);
    }
    return result;
}

