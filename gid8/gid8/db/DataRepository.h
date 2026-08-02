#ifndef DATAREPOSITORY_H
#define DATAREPOSITORY_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <vector>
#include <QString>
#include <utility> // Для std::pair

class DataRepository {
public:
    explicit DataRepository(QSqlDatabase* db);

    std::vector<std::pair<int, QString>> getNasTypes();

private:
    QSqlDatabase* m_db;
};

#endif // DATAREPOSITORY_H
