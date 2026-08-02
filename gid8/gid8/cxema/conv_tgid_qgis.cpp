#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QString>
#include <QSet>
#include <QList>
#include <QPair>

// Парсинг имени таблицы (схема и имя)
QPair<QString, QString> parseSchemaTable(const QString &name) {
    QStringList parts = name.split('.');
    if (parts.size() == 2) {
        return {parts[0], parts[1]};
    } else if (parts.size() == 1) {
        return {"public", parts[0]}; // По умолчанию схема public
    } else {
        throw std::invalid_argument("Неверный формат имени таблицы: " + name.toStdString());
    }
}

// Получение столбцов из таблицы
QSet<QString> getColumns(QSqlDatabase &conn, const QString &tableName, const QString &prefix = "") {
    auto [schema, table] = parseSchemaTable(tableName);
    qDebug() << schema << table;

    QSet<QString> columns;
    QSqlQuery query(conn);
    QString sql = QString(
        "SELECT column_name "
        "FROM information_schema.columns "
        "WHERE table_schema = '%1' AND table_name = '%2'"
        " AND column_name <> 'coords' AND column_name <> 'x' AND column_name <> 'y'"
        " AND column_name <> 'globalid'"
        " AND column_name <> 'gistable'"
        " AND column_name <> 'sync'"
        " AND column_name <> 'gis'"

        )
        .arg(schema, table);

    if (!query.exec(sql)) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        return columns;
    }

    QString newPrefix = prefix.isEmpty() ? "" : prefix + "_";
    while (query.next()) {
        columns.insert(newPrefix + query.value(0).toString());
    }

    return columns;
}

// Копирование строк между таблицами
void copyCommonRowsLines(QSqlDatabase &srcConn, QSqlDatabase &dstConn, 
                        const QString &srcTable, const QString &dstTable, 
                        const QString &lineNode) {
    QString linesObj, lineId;

    qDebug() << QString("%3: %1 -> %2")
        .arg(srcTable, dstTable, lineNode);

    // Определение linesObj и lineId в зависимости от lineNode
    if (lineNode == "l") {
        linesObj = "linesobj";
        lineId = "lineid";
    } else if (lineNode == "n" || lineNode == "n0") {
        linesObj = "nodes";
        lineId = "nodeid";
    } else {
        qDebug() << "Ошибка: неверное значение lineNode!";
        return;
    }

    // Получение столбцов из исходной таблицы и linesObj
    QSet<QString> srcCols = getColumns(srcConn, srcTable, srcTable);
    QSet<QString> srcColsL = getColumns(srcConn, linesObj, linesObj);
    srcCols.unite(srcColsL); // Объединение наборов столбцов

    QSet<QString> dstCols = getColumns(dstConn, dstTable, "");

    // Нахождение общих столбцов
    QSet<QString> commonColsSet = srcCols.intersect(dstCols);
    if (commonColsSet.isEmpty()) {
        qDebug() << "Нет общих столбцов.";
        return;
    }

    // Преобразование QSet в QStringList
    QStringList commonCols = commonColsSet.values();
    commonCols.sort(); // Сортировка для консистентности

    QString colList = commonCols.join(", ");
//    QString placeholders = QStringList(QList<QString>(commonCols.size(), "%1")).join(", ");
    QString placeholders = QStringList(QList<QString>(commonCols.size(), "?")).join(", ");

    // Формирование строки для SET в запросе ON CONFLICT
    QString sets;
    for (const QString &col : commonCols) {
        if (!sets.isEmpty()) {
            sets += ",";
        }
        sets += QString("\n    %1 = EXCLUDED.%1").arg(col);
    }

    // Чтение данных для удаления
    QSqlQuery srcQuery(srcConn);
    QString deleteQuery;
    if (lineNode == "n0") {
        deleteQuery = QString(
            "SELECT t.id FROM net.v_nodes t "
            "LEFT JOIN net.v_generalizedconsumers gc ON t.id = gc.nodeID "
            "LEFT JOIN net.v_realconsumers rc ON t.id = rc.nodeID "
            "LEFT JOIN net.v_heatsources hs ON t.id = hs.nodeID "
            "WHERE t.removed = 1 AND t.sync_tgid "
            "AND gc.id IS NULL "
            "AND rc.id IS NULL "
            "AND hs.id IS NULL");
    } else {
        deleteQuery = QString(
            "SELECT l.id FROM %1 t "
            "JOIN %2 l ON l.id = t.%3 "
            "WHERE l.sync_tgid AND l.removed = 1")
            .arg(srcTable, linesObj, lineId);
    }

    if (!srcQuery.exec(deleteQuery)) {
        qDebug() << "Ошибка при выполнении запроса на удаление:" << srcQuery.lastError().text();
        return;
    }

    QStringList delRows;
    while (srcQuery.next()) {
        delRows << srcQuery.value(0).toString();
    }

    QString delRowsStr = delRows.join(",");

    // Формирование списка столбцов с учетом префиксов
    QString colList0 = colList;
    colList0.replace(srcTable + "_", "t.");
    colList0.replace(linesObj + "_", "l.");
    colList0.replace("l.shape", "ST_SetSRID(l.shape, 9998) AS shape");
    colList0.replace("t.shape", "ST_SetSRID(t.shape, 9998) AS shape");

//    if (lineNode != "n0") {
    if (srcTable == "heatpipesections" ) {
        colList0.replace("t.fileid", "n1.fileid");
        colList0.replace("l.fileid", "n1.fileid");
    }


    // Чтение данных для вставки
    QString selectQuery;
    if (lineNode == "n0") {
        selectQuery = QString(
            "SELECT %1 FROM net.v_nodes t "
            "LEFT JOIN net.v_generalizedconsumers gc ON t.id = gc.nodeID "
            "LEFT JOIN net.v_realconsumers rc ON t.id = rc.nodeID "
            "LEFT JOIN net.v_heatsources hs ON t.id = hs.nodeID "
            "WHERE t.removed = 0 AND t.sync_tgid "
            "AND gc.id IS NULL "
            "AND rc.id IS NULL "
            "AND hs.id IS NULL")
            .arg(colList0);
    } else {

        if (srcTable == "heatpipesections" ) {
            selectQuery = QString(
                "SELECT %1 FROM %2 t "
                "JOIN %3 l ON l.id = t.%4 "
                "JOIN net.v_nodes n1 ON l.nodeid1 = n1.id "
                "WHERE l.removed = 0 AND l.sync_tgid")
                .arg(colList0, srcTable, linesObj, lineId);
        }
        else {
            selectQuery = QString(
                "SELECT %1 FROM %2 t "
                "JOIN %3 l ON l.id = t.%4 "
                "WHERE l.removed = 0 AND l.sync_tgid")
                .arg(colList0, srcTable, linesObj, lineId);
        }
    }

    qDebug() << selectQuery;


    if (!srcQuery.exec(selectQuery)) {
        qDebug() << "Ошибка при выполнении запроса на выборку:" << srcQuery.lastError().text();
        return;
    }

    QList<QList<QVariant>> rows;
    while (srcQuery.next()) {
        QList<QVariant> row;
        for (int i = 0; i < commonCols.size(); ++i) {
            row << srcQuery.value(i);
        }
        rows << row;
    }

    // Формирование запроса INSERT
    QString insertSql = QString(
        "INSERT INTO %1 (%2) "
        "VALUES (%3) "
        "ON CONFLICT (%4_id) DO UPDATE "
        "SET %5")
        .arg(dstTable, colList, placeholders, linesObj, sets);

    QSqlQuery dstQuery(dstConn);
    dstQuery.prepare(insertSql);

    qDebug() << insertSql;


    // Выполнение вставки
    for (const auto &row : rows) {
        for (int i = 0; i < row.size(); ++i) {
//            dstQuery.bindValue(i, row[i]);
            dstQuery.addBindValue(row[i]);
            qDebug() << row[i];
        }


        if (!dstQuery.exec()) {
            qDebug() << "Ошибка вставки:" << dstQuery.lastError().text();
            return;
        }
    }
    dstConn.commit();

    // Удаление строк
    if (!delRowsStr.isEmpty()) {
        QString deleteSql = QString("DELETE FROM %1 WHERE %2_id IN (%3)")
                            .arg(dstTable, linesObj, delRowsStr);
        if (!dstQuery.exec(deleteSql)) {
            qDebug() << "Ошибка удаления:" << dstQuery.lastError().text();
            return;
        }
        dstConn.commit();
    }

    qDebug() << QString("Скопировано %1 строк.").arg(rows.size());
}

#if 0

QSqlDatabase connectToAnotherDatabase(const QSqlDatabase &existingConn,
                                      const QString &newDatabaseName)
{
    if (!existingConn.isOpen()) {
        qDebug() << "Исходное соединение не открыто!";
        return QSqlDatabase();
    }

    // Генерируем уникальное имя соединения: "conn_dbname" + случайный суффикс
    QString connectionName = QString("conn_%1_%2")
                                 .arg(newDatabaseName)
                                 .arg(QRandomGenerator::global()->generate());

    QSqlDatabase newConn = QSqlDatabase::addDatabase(existingConn.driverName(), connectionName);

    // Копируем параметры
    newConn.setHostName(existingConn.hostName());
    newConn.setPort(existingConn.port());
    newConn.setUserName(existingConn.userName());
    newConn.setPassword(existingConn.password());

    // Ставим новое имя базы
    newConn.setDatabaseName(newDatabaseName);

    if (!newConn.open()) {
        qDebug() << "Ошибка подключения к базе" << newDatabaseName
                 << ":" << newConn.lastError().text();
        QSqlDatabase::removeDatabase(connectionName);
        return QSqlDatabase();
    }

    return newConn;
}

#endif 

QSqlDatabase connectToAnotherDatabase(const QSqlDatabase &existingConn,
                                      const QString &newDatabaseName)
{
    if (!existingConn.isOpen()) {
        qDebug() << "Исходное соединение не открыто!";
        return QSqlDatabase();
    }

    // Сгенерируем уникальное имя соединения
    QString connectionName = QString("conn_%1_%2")
                                 .arg(newDatabaseName)
                                 .arg(QRandomGenerator::global()->generate());

    QSqlDatabase newConn = QSqlDatabase::addDatabase("QODBC", connectionName);

    // Берём параметры из существующего подключения
    QString driver   = "PostgreSQL Unicode";  // или "PostgreSQL ANSI", смотря какой у вас
    QString server   = existingConn.hostName();
    int port         = existingConn.port();
    QString user     = existingConn.userName();
    QString password = existingConn.password();

    // Формируем connection string
    QString connStr = QStringLiteral(
        "Driver={%1};Server=%2;Port=%3;Database=%4;Uid=%5;Pwd=%6;")
            .arg(driver)
            .arg(server)
            .arg(port)
            .arg(newDatabaseName)
            .arg(user)
            .arg(password);

    newConn.setDatabaseName(connStr);

    if (!newConn.open()) {
        qDebug() << connStr;
        qDebug() << "Ошибка подключения к базе" << newDatabaseName
                 << ":" << newConn.lastError().text();
        QSqlDatabase::removeDatabase(connectionName);
        return QSqlDatabase();
    }

    return newConn;
}



bool conv_tgid_qgis(QSqlDatabase &db1, const QString &name)
{
    qDebug() << "Синхронизация " << name;

    // Проверка, открыто ли исходное соединение
    if (!db1.isOpen()) {
        qDebug() << "Исходное соединение не открыто!";
        return false;
    }

/*
    // Проверка, не занято ли имя соединения
    if (QSqlDatabase::contains(name)) {
        qDebug() << "Соединение с именем" << name << "уже существует!";
        return false;
    }
*/

    // Подключение к целевой базе данных
    QSqlDatabase db2 = connectToAnotherDatabase(db1, name);
    if (!db2.isValid() || !db2.isOpen()) {
        qDebug() << "Не удалось подключиться к целевой базе данных" << name;
        return false;
    }

    // Начало транзакции для db2
    if (!db2.transaction()) {
        qDebug() << "Не удалось начать транзакцию для" << name << ":" << db2.lastError().text();
        db2.close();
        QSqlDatabase::removeDatabase(name);
        return false;
    }

    // Вызов copyCommonRowsLines для каждой пары таблиц
    bool success = true;
    try {
        copyCommonRowsLines(db1, db2, "heatpipesections", "gid.heatpipesections", "l");

        copyCommonRowsLines(db1, db2, "generalizedconsumers", "gid.generalizedconsumers", "n");
        copyCommonRowsLines(db1, db2, "realconsumers", "gid.realconsumers", "n");
        copyCommonRowsLines(db1, db2, "heatsources", "gid.heatsources", "n");
        copyCommonRowsLines(db1, db2, "pumpstations", "gid.pumpstations", "n");
        
        copyCommonRowsLines(db1, db2, "nodes", "gid.nodes", "n0");

    } catch (const std::exception &e) {
        qDebug() << "Ошибка при копировании данных:" << e.what();
        success = false;
    }

    // Завершение транзакции
    if (success) {
        if (!db2.commit()) {
            qDebug() << "Ошибка при фиксации транзакции:" << db2.lastError().text();
            success = false;
        }
    } else {
        if (!db2.rollback()) {
            qDebug() << "Ошибка при откате транзакции:" << db2.lastError().text();
        }
    }

    QString connName = db2.connectionName();

    // Закрытие соединения db2
    db2.close();

    QSqlDatabase::removeDatabase(connName);

    return success;
}
