#include <QApplication>
#include <QtSql>
#include <iostream>
#include "db.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QRegularExpression>
#include <QElapsedTimer>


#include <QVariant>
#include <QString>
#include <map>

#if USE_ITWIN
//#include "version.h"
#else
//#include "version_tgid.h"
#endif

#include "version0.h"


static std::map <QString, QSqlDatabase> map_db_sqlite;

static std::map <std::tuple<QString, QString, int, QString, QString>, QSqlDatabase> map_db;

 


//static int RDBMS = MSSQL;
static int RDBMS = POSTGRESQL;


bool is_POSTGRESQL() 
{
    return RDBMS == POSTGRESQL;
}

bool is_SQLITE() 
{
    return RDBMS == SQLITE;
}


bool is_MSSQL() 
{
    return RDBMS == MSSQL;
}

int get_RDMS() 
{
    return RDBMS;
}

QString top100000()
{
    if (RDBMS == MSSQL) {
        return "TOP 1000000";
    }
    return "";
}

QString ispr_q(const QString & q)
{
    if (is_POSTGRESQL()) {
        static QRegularExpression re("^(.+)\\s+TOP\\s+[0-9]+\\s+(.+)$");
        QRegularExpressionMatch match = re.match(q);
        if (match.hasMatch()) {
            QString q1 = match.captured(1);
            QString q2 = match.captured(3);
            return q1 +" " + q2;
        }
    }
    return q;
}


QString get_id(const QString & tn)
{
    return "id";
}

QString get_shape(const QString & tn)
{
    return "shape";
}

QString get_now()
{
    if (is_POSTGRESQL()) {
        return "NOW()";
    }
    else if (is_MSSQL()) {  // MS SQL
        return "GETDATE()";
    }

    return "";
}


QString quot_text(const QString & txt)
{
    if (RDBMS == MSSQL) {
        return QString("'%1'").arg(txt);
    }
    if (RDBMS == POSTGRESQL) {
        return QString("\"%1\"").arg(txt);
    }
    return QString("'%1'").arg(txt);
}

QString br_text(const QString & txt0)
{
    QString txt = txt0;

    if (RDBMS == MSSQL) {
        txt.replace(".", "].[");
        return QString("[%1]").arg(txt);
    }
    if (RDBMS == POSTGRESQL) {
        txt.replace(".", "\".\"");

        return QString("\"%1\"").arg(txt.toLower());
    }
    return QString("[%1]").arg(txt);
}

QString full_name(const QString & schema, const QString & table0)
{
    QString table = table0;

    if (RDBMS == MSSQL) {
        if (schema != "dbo") {
            table = QString("%1.%2").arg(schema).arg(table);
        }
    }
    if (RDBMS == POSTGRESQL) {
        if (schema != "public") {
            table = QString("%1.%2").arg(schema).arg(table);
        }
    }
    return table;
}


QString SelectTop(const QString & q)
{
    if (RDBMS == MSSQL) {
        return QString("SELECT TOP 2147483647 * FROM (\n %1 \n) __").arg(q);
    }
    return q;
}


static QSqlDatabase *global_db = nullptr;


bool configureTgidPostgreSqlSession(QSqlDatabase &db)
{
    if (!db.isOpen() || RDBMS != POSTGRESQL) {
        return true;
    }

    QSqlQuery query(db);
    if (!query.exec(QString("SET application_name = '%1'")
                        .arg(VER_FILEDESCRIPTION_STR))) {
        qWarning() << "Не удалось задать application_name:"
                   << query.lastError().text();
        return false;
    }

    // compat содержит представления со старыми именами nodes/linesobj/...
    // только для gid8. Схема намеренно не включена в search_path самой БД:
    // QGIS, расчёт и новые клиенты продолжают работать с net/ref/calc.
    if (!query.exec(
            "SET search_path TO compat, public, net, ref, calc, "
            "meta, addr, doc, el, ops, org")) {
        qWarning() << "Не удалось настроить search_path gid8:"
                   << query.lastError().text();
        return false;
    }

    return true;
}


void setRDBMS(int rdbms)
{
    if (rdbms == 0) {
        RDBMS = MSSQL;
    }
    else if (rdbms == 1) {
        RDBMS = POSTGRESQL;
    }
    else if (rdbms == 2) {
        RDBMS = SQLITE;
    }
    else {
//        QMessageBox::warning(nullptr, "", "Ничего не понял");
//        return false;
    }
}


bool connectSQL0(int rdbms, const QString & host, int port, const QString & baza, const QString & user, const QString & password, QSqlDatabase &m_db)
{
    global_db = &m_db;

    qDebug() << "Connect " << rdbms << " " << host << " " << port << " " << baza << " " << user << " ";

    if (rdbms == 0) {
        RDBMS = MSSQL;
    }
    else if (rdbms == 1) {
        RDBMS = POSTGRESQL;
    }
    else if (rdbms == 2) {
        RDBMS = SQLITE;
    }
    else {
        QMessageBox::warning(nullptr, "", "Ничего не понял");
        return false;
    }


    qDebug() << "Connect!!";
    qDebug() << QSqlDatabase::drivers();


    std::tuple<QString, QString, int, QString, QString> key(host, baza, port, user, password);

//    auto it = map_db.find(baza);
    auto it = map_db.find(key);
    if (it != map_db.end()) {
        m_db = it->second;
        qDebug() << " m_db.open()";
        if (m_db.open()) {
            qDebug() << "!";
            return configureTgidPostgreSqlSession(m_db);
        }
        qDebug() << "+";
    }


//    QString connectString = QString("DRIVER={SQL Server};SERVER=%1,%2;DATABASE=%3;trustservercertificate=yes;")
//    QString connectString = QString("DRIVER={ODBC Driver 13 for SQL Server};SERVER=%1,%2;DATABASE=%3;trustservercertificate=yes;")

//    QString driver = "SQL Server";
//    QString driver = "ODBC Driver 17 for SQL Server";
    QString driver = "SQL Server Native Client 11.0";
    if (RDBMS == POSTGRESQL) {
#if _WIN32
        driver = "PostgreSQL Unicode(x64)";
#else
        driver = "PostgreSQL Unicode";
#endif

//        driver = "PostgreSQL Unicode";
    
    }
    
    QString connectString = QString("Driver={%1};").arg(driver);
//    connectString += "trustservercertificate=yes;";

    int timeout = 10;

    if (user == "" || password == "") {
        connectString += QString("Server=%1;").arg(host);
        connectString += "Trusted_Connection=yes;";
//        connectString += "Integrated Security=SSPI;";
    }
    else {
        if (RDBMS == POSTGRESQL) {
            connectString += QString("Server=%1;Port=%2;").arg(host).arg(port);
            connectString += QString("UserName=%1;Password=%2;").arg(user, password);

//            connectString += QString("LoginTimeout=%1;").arg(timeout);
        }
        else {
            connectString += QString("Server=%1,%2;").arg(host).arg(port);
//            connectString += QString("Connection Timeout=%1;").arg(timeout);
        }
    }

    if (baza != "") {
        connectString += QString("Database=%1;").arg(baza);
    }

//    connectString += QString("Application Name=%1;").arg(VER_FILEDESCRIPTION_STR);
    connectString += QString("ApplicationName=%1;").arg(VER_FILEDESCRIPTION_STR);

    bool sqlite = true;

    if (RDBMS == SQLITE) {
        connectString = "c:/data/baza.sqlite";
        driver = "SQLite3 ODBC Driver";
        m_db = QSqlDatabase::addDatabase("QSQLITE");
        m_db.setDatabaseName(connectString);
    }
    else {

        QString QODBC = "QODBC";
//        QODBC = "QPSQL";

        m_db = QSqlDatabase::addDatabase(QODBC, connectString);
        m_db.setHostName(host);
        m_db.setDatabaseName(connectString);

        if (RDBMS == POSTGRESQL) {
            qDebug() << "PostgreSQL ODBC:" << host << port << baza << user;
//            m_db.setConnectOptions("SQL_ATTR_LOGIN_TIMEOUT=5;SQL_ATTR_QUERY_TIMEOUT=5");
            m_db.setConnectOptions("SQL_ATTR_LOGIN_TIMEOUT=5");
            m_db.setPort(port);
        }

        if (user != "" && password != "") {

            m_db.setUserName(user);
            m_db.setPassword(password);
        }
    }
//    m_db = QSqlDatabase::addDatabase("QPSQL", connectString);


// Добавляем настройку таймаута для PostgreSQL


    bool ok = m_db.open();

    if (ok) {
        if (RDBMS == POSTGRESQL) {
            ok = configureTgidPostgreSqlSession(m_db);
        }

//        std::cout << "Connected\n";
    }
    else {
        QString error = m_db.lastError().text();
//        std::cout << error.toStdString() << std::endl;
        qDebug() << error.toStdString();
    }

    map_db[key] = m_db;
//    map_db[baza] = m_db;

    return ok;
}


bool connectSqlite0(const QString & baza, QSqlDatabase &m_db)
{
    global_db = &m_db;

    RDBMS = SQLITE;

    auto it = map_db_sqlite.find(baza);
    if (it != map_db_sqlite.end()) {
        m_db = it->second;
        if (m_db.open()) {
            return true;
        }
    }

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(baza);
    bool ok = m_db.open();

    if (ok) {
//        std::cout << "Connected\n";
    }
    else {
        QString error = m_db.lastError().text();
        std::cout << error.toStdString() << std::endl;
    }

    map_db_sqlite[baza] = m_db;

    return ok;
}





#include <mystd.h>

QString getDatabaseName(QSqlDatabase &db)
{
    QString databasename = db.databaseName();

    static QRegularExpression re("^.*Database=([^;]+);.*$");
    QRegularExpressionMatch match = re.match(databasename);
    if (match.hasMatch()) {
        return match.captured(1);
    }
    return "";
}

bool readTableValues(QSqlDatabase &db, const QString & q, int n, std::vector<QVariant> & vv)
{
    QSqlQuery query(db);
    query.setForwardOnly(true);

    if (query_exec(db, query, q)) {
        while (query.next()) {
            int nn = query.record().count();
            for (int i = 0; i < n && i < nn; i++) {
                QVariant v = query.value(i);
                vv.push_back(v);
            }
            return true;
        }
    }
    return false;
}


bool fetchFirstRow(QSqlQuery &query, QMap<QString, QVariant> &row)
{
    if (query.next()) {
        QSqlRecord rec = query.record();
        row.clear();
        for (int i = 0; i < rec.count(); i++) {
            row.insert(rec.fieldName(i), query.value(i));
        }
        return true;
    }
    return false; // строк нет
}

bool fetchFirstRow(QSqlDatabase &db, const QString & q, QMap<QString, QVariant> &row)
{
    QSqlQuery query(db);
    query.setForwardOnly(true);

    if (query_exec(db, query, q)) {
        if (query.next()) {
            QSqlRecord rec = query.record();
            row.clear();
            for (int i = 0; i < rec.count(); i++) {
                row.insert(rec.fieldName(i), query.value(i));
            }
            return true;
        }
    }
    return false; // строк нет
}



bool readTableRowValues(QSqlDatabase &db, const QString & q, std::vector<QVariant> & vv)
{
    QSqlQuery query(db);
    query.setForwardOnly(true);

    if (query_exec(db, query, q)) {
        QSqlRecord record = query.record();
        int columnCount = record.count();

        while (query.next()) {
            for (int i = 0; i < columnCount; i++) {
                QVariant v = query.value(i);
                vv.push_back(v);
            }
            return true;
        }
    }
    return false;
}

bool readTableRowValuesMap(QSqlDatabase &db, const QString & q, std::map<QString, QVariant> & vv)
{
    QSqlQuery query(db);
    query.setForwardOnly(true);

    if (query_exec(db, query, q)) {
        QSqlRecord record = query.record();
        int columnCount = record.count();

        while (query.next()) {
            for (int i = 0; i < columnCount; i++) {
                QVariant v = query.value(i);
                vv[record.fieldName(i)]= v;
            }
            return true;
        }
    }
    return false;
}




QVariant readTableVariant(QSqlDatabase &db, const QString & q)
{
    QVariant v = "";
    QSqlQuery query(db);

    if (query_exec(db, query, q)) {
        while (query.next()) {
            v = query.value(0);
            break;
        }
    }
    return v;
}


QString readTableValue(QSqlDatabase &db, const QString & q)
{
    QString v = "";
    QSqlQuery query(db);

    if (query_exec(db, query, q)) {
        while (query.next()) {
            v = query.value(0).toString();
            break;
        }
    }
    return v;
}

QString readTableValue2(QSqlDatabase &db, const QString & q)
{
    QString v = "";
    QSqlQuery query(db);

    if (query_exec(db, query, q)) {
        while (query.next()) {
            v = query.value(1).toString();
            break;
        }
    }
    return v;
}


int readTableInt(QSqlDatabase &db, QSqlQuery &query, const QString & q)
{
    int v = 0;

    if (query_exec(db, query, q)) {
        while (query.next()) {
            v = query.value(0).toInt();
            break;
        }
    }
    return v;
}


int readTableInt(QSqlDatabase &db, const QString & q)
{
    int v = 0;
    QSqlQuery query(db);

    if (query_exec(db, query, q)) {
        while (query.next()) {
            v = query.value(0).toInt();
            break;
        }
    }
    return v;
}


double read_double_db(QSqlDatabase &db, const QString & q, const QString & fn, bool *ok)
{
    *ok = false;
    double v = 0;
    QSqlQuery query(db);

    if (query_exec(db, query, q)) {
        while (query.next()) {
            v = query.value(fn).toDouble();
            *ok = true;
            break;
        }
    }
    return v;
}


void readLookup(QSqlDatabase &db, const QString & table, const QString & sid, const QString & sval, std::map<int, QString> &map_lookup)
{
    QString q = QString("SELECT %1,%2 FROM %3").arg(sid, sval, table);

    QSqlQuery query(db);

    if (query_exec(db, query, q)) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString value = query.value(1).toString();
            map_lookup[id] = value;
        }
    }
}

bool delObject(QSqlDatabase &db, const QString & tn, int id)
{
    if (tn == "") return true;
    QString q = QString("DELETE FROM %1 WHERE id=%2").arg(tn).arg(id);

    QSqlQuery query(db);
    bool ret = query_exec(db, query, q);

    return ret;
}


void ErrorMessageText(QSqlQuery &query, QString &q, QString &text)
{
    QSqlError e = query.lastError();
    q = query.lastQuery();
    text = e.text();

//    return QString("%1\n\n%2").arg(query.lastQuery(), e.text());
}


void ErrorMessage(QSqlQuery &query)
{
    QSqlError e = query.lastError();
    if (e.isValid()) {
        QMessageBox::warning(nullptr, "", QString("%1\n\n%2").arg(query.lastQuery(), e.text()));
    }
}

void ErrorMessage(QSqlDatabase &db)
{
    QSqlError e = db.lastError();
    if (e.isValid()) {
        QMessageBox::warning(nullptr, "", QString("%1").arg(e.text()));
    }
}



QString getTableNameFromSql(const QString & q)
{
    QString tn = "";

    static QRegularExpression re("FROM\\s+\\[?([^;\\s\\]]+)\\]?", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = re.match(q);
    if (match.hasMatch()) {
        tn = match.captured(1);

        if (tn[0] == '"') {
            tn = tn.mid(1, tn.length()-2);
        }


    }
    return tn;
}


long sizeOfQ(QSqlDatabase &db, const QString & q)
{
    QString qq = QString("SELECT count(*) FROM (%1) ____T____123").arg(q);

    QSqlQuery query(db);
    query.setForwardOnly(true);
    query_exec(db, query, qq);

    long nn = 0;
    if (query.next()) {
        nn = query.value(0).toInt();
    }
    return nn;
}

#include <QClipboard>
#include <property/variantdelegate.h>

void copy_q(QSqlDatabase &db, const QString & q)
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    QSqlQuery query(db);
    query.setForwardOnly(true);

    QString txt = "";

    if (query_exec(db,  query, q)) {
        while (query.next()) {
            QSqlRecord record = query.record();
            int cnt = record.count();
            QString row_txt = "";
            for (int c = 0; c < cnt; c++) {
                QSqlField field = record.field(c);
                QString s = VariantDelegate::displayText(field.value());
//                QString s = field.value().toString();
                if (c > 0) row_txt += "\t";
                row_txt += s;
            }

            if (txt != "") txt += "\r\n";
            txt += row_txt;
        }
    }
    clipboard->setText(txt);
}



bool check_db(QSqlDatabase &db)
{
    QSqlQuery query(db);

    if (!query.exec("select 1")) {
        if (QMessageBox::question(nullptr,  "", QObject::tr("Отключился сервер. Подключиться снова?")) == QMessageBox::Yes) {
            if ( db.open()) {
                return configureTgidPostgreSqlSession(db);
            }
        }
        return false;
    }
    return false;
}

bool query_exec(QSqlDatabase &db, QSqlQuery &query, const QString & q, bool alarm) 
{
    bool ret;
    qint64 elapsed;

//    qDebug() << "-----------------------------------\n" << q << "\n-----------------------------------";
    qDebug() << q;



    while (true) {
        QElapsedTimer timer;
        timer.start();
        
        if (q == "") {
            ret = query.exec();
        }
        else {
            ret = query.exec(q);
        }

        elapsed = timer.elapsed(); 

        if (ret) {
            int rowsAffected = query.numRowsAffected();

            qDebug() << QString("Время выполнения %1 миллисекунд").arg(elapsed);
            return true;
        }

        QSqlError e = query.lastError();

        QString msg = QString("[%1] %2\n[%3]\n%4")
            .arg(e.type())
            .arg(e.driverText())
            .arg(e.nativeErrorCode())
            .arg(e.databaseText())
//            .arg(e.text())
            
            ;

        if (check_db(db)) continue;

        if (alarm) {
            QInputDialog::getMultiLineText(nullptr, "Ошибка", msg, q);
        }

//        qWarning() << "-----------------------------------\n" << msg << "\n-----------------------------------";
        qWarning() << msg;

/*

        bool ret2 = query.exec("select 1");

        if (!ret2) {
            if (QMessageBox::question(nullptr,  "", QObject::tr("Отключилось. Подключиться снова?")) == QMessageBox::Yes) {
                if ( db.open()) {
                    continue;
                }
            }
        }
*/
        return false;

//            QString msg = QString("%1\n%2").arg(e.nativeErrorCode(), e.text());
//            QInputDialog::getMultiLineText(nullptr, "Ошибка", msg, q);

//            return ret;
    }

    return ret;
}

bool query_exec(QSqlDatabase &db, const QString & q, bool alarm) 
{
    QSqlQuery query(db);

    return query_exec(db, query, q, alarm);
}


bool query_exec2(QSqlDatabase &db, QSqlQuery &query, const QString & q, bool alarm) 
{
    return true;
}



#define NONE ""

QString & rtrim( QString & str ) 
{
    while( str.size() > 0 && str.at( str.size() - 1 ).isSpace() )
        str.chop( 1 );
    return str;
}


void split_sql_expressions(const QString & text, std::list<QString> &list_q)
{
    QString current = "";
    QString state = NONE;

//    for c in text:
    for (int i = 0; i < text.length(); i++) {
        QString c = text.mid(i, 1);

        if (state == NONE) {  // default state, outside of special entity
            current += c;
            if (c == "'" || c == "\"") {
                // quoted string
                state = c;
            }
            else if (c == "-") {
                // probably "--" comment
                state = "-";
            }
            else if (c == "$") {
                // probably $$"
                state = "$";
            }
            else if (c == "/") {
                // probably '/*' comment
                state = "/";
            }
            else if (c == ";") {
                // remove it from the statement
//                current = current[:-1].strip();
                current = current.left(current.length()-1).trimmed();
                // and save current stmt unless empty
                if (current != "") {
                    list_q.push_back(current);
                }
                current = "";
             }
        }
        else if (state == "-") {
            if (c != "-") {
                // not a comment
                state = NONE;
                current += c;
                continue;
            }
            // remove first minus
//            current = current[:-1]
              current = current.left(current.length()-1);

            // comment until end of line
            state = "--";
        }
        else if (state == "$") {
            current += c;
            if (c != "$") {
                // not a $$
                state = NONE;
            }
            else {
                state = "$$";
            }
        }
        else if (state == "--") {
            if (c == "\n") {
                // end of comment
                // and we do include this newline
                current += c;
                state = NONE;
            // else just ignore
            }
        }
        else if (state == "/") {
            if (c != "*") {
                state = NONE;
                current += c;
                continue;
            }
            // remove starting slash
            current = current.left(current.length()-1);
            // multiline comment
            state = "/*";

        }
        else if (state == "$$") {
            current += c;
            if (c == "$") {
                // probably end of $$
                state = "$$$";
            }
        }
        else if (state == "$$$") {
            current += c;

            if (c == "$") {
                state = NONE;
            }
            else {
                // not an end
                state = "$$";
            }
        }
        else if (state == "/*") {
            if (c == "*") {
                // probably end of comment
                state = "/**";
            }
        }
        else if (state == "/**") {
            if (c == "/") {
                state = NONE;
            }
            else {
                // not an end
                state = "/*";
            }
        }
        else if (state.left(1) == "'" ||  state.left(1) == "\"") {
            current += c;
            if (state.endsWith("\\")) {
                // prev was backslash, don't check for ender
                // just revert to regular state
                state = state.left(1);
                continue;
            }
            else if (c == "\\") {
                // don't check next char
                state += "\\";
                continue;
            }
            else if (c == state.left(1)) {
                // end of quoted string
                state = NONE;
            }
        }
        else {
            return;
//            raise Exception("Illegal state %s" % state);
        }
    }
    if (current != "") {
        while (state.endsWith(";")) state.chop(1);

        if (current != "") {
            list_q.push_back(current);
        }
    }
}

//void split_sql_expressions(CString text, std::list<CString> &list_q);

QString split_query(QSqlDatabase &db, const QString & q0)
{
    std::list<QString> list_q;
    QString q = q0;


    split_sql_expressions(q, list_q);

    if (list_q.size() > 1) {
        q = list_q.back();
        list_q.pop_back();

        for (auto& qq : list_q) {
            query_exec(db, qq);
        }
    }
    return q;
}




bool updateDatabaseRow(QSqlDatabase &db, const QString &tableName, int id, const std::map<QString, QVariant> &data, const QString & geom_col)
{
    // Проверяем, что база данных открыта
    if (!db.isOpen()) {
        if (!db.open()) {
            qWarning() << "Failed to open database: " << db.lastError().text();
            return false;
        }
    }

    // Формируем SQL-запрос для обновления
    QStringList setClauses;
    for (const auto &pair : data) {
        if (pair.first == geom_col) {
            setClauses.append(QString("%1 = %1").arg(pair.second.toString()));
        }
        else {
//            setClauses.append(QString("%1 = :%1").arg(pair.first));
            setClauses.append(QString("%1 = ?"));
        }
    }
//    QString sql = QString("UPDATE %1 SET %2 WHERE id = :id")
    QString sql = QString("UPDATE %1 SET %2 WHERE id = ?")
                  .arg(tableName)
                  .arg(setClauses.join(", "));

    QSqlQuery query(db);
    query.prepare(sql);

    // Привязываем параметры к запросу
    for (const auto &pair : data) {
        if (pair.first == geom_col) {
//            query.b1indValue(QString(":%1").arg(pair.first), pair.second);
        }
        else {
//            query.b1indValue(QString(":%1").arg(pair.first), pair.second);
            query.addBindValue(pair.second);
        }
    }
//    query.b1indValue(":id", id);
    query.addBindValue(id);

    // Выполняем запрос
    if (!query.exec()) {
        qWarning() << "Failed to execute query: " << query.lastError().text();
        return false;
    }

    return true;
}

int insertIntoDatabase(QSqlDatabase &db, const QString &tableName, const std::map<QString, QVariant> &data, const QString &geom_col) {
    // Проверяем, что база данных открыта
    if (!db.isOpen()) {
        if (!db.open()) {
            qWarning() << "Failed to open database: " << db.lastError().text();
            return -1;
        }
    }

    // Формируем SQL-запрос для вставки
    QStringList columns;
    QStringList placeholders;
    for (const auto &pair : data) {
        columns.append(pair.first);
        if (pair.first == geom_col && !geom_col.isEmpty()) {
            QString s = pair.second.toString();
//                QString("ST_GeomFromText(%1)").arg(pair.second.toString())
//                QString("ST_GeomFromText(%1)").arg(pair.second.toString())
//                ;


            placeholders.append(s);
        } else {
            if (pair.second.toString() == "$$$_NOW_$$$") {
                if (is_POSTGRESQL()) {
                    placeholders.append("NOW()");
                }
                else if (is_MSSQL()) {  // MS SQL
                    placeholders.append("GETDATE()");
                }
            }
            else {
//                placeholders.append(QString(":%1").arg(pair.first));
                placeholders.append("?");
            }
        }
    }

    QString sql = QString("INSERT INTO %1 (%2)\nVALUES (%3)\nRETURNING id")
                  .arg(tableName)
                  .arg(columns.join(", "))
                  .arg(placeholders.join(", "));

    if (columns.length() == 0) {
        sql = QString("INSERT INTO %1\nDEFAULT VALUES\nRETURNING id")
                          .arg(tableName);
    }

    QSqlQuery query(db);
    query.prepare(sql);

    // Привязываем параметры к запросу
    for (const auto &pair : data) {
        if (pair.first == geom_col && !geom_col.isEmpty()) {
        }
        else if (pair.second.toString() == "$$$_NOW_$$$") {
        }
        else {
//            query.bindValue(QString(":%1").arg(pair.first), pair.second);
            query.addBindValue(pair.second);
        }
    }

    // Выполняем запрос
    if (!query.exec()) {
        qWarning() << "Failed to execute query: " << query.lastError().text() << "SQL:" << sql;

        for (const auto &pair : data) {
            if (pair.first == geom_col && !geom_col.isEmpty()) {
            }
            else if (pair.second.toString() == "$$$_NOW_$$$") {
            }
            else {
                qWarning() << pair.first << " " << pair.second;
            }
        }


        return -1;
    }

    // Извлекаем возвращённый id
    if (query.next()) {
        bool ok;
        int insertedId = query.value("id").toInt(&ok);
        if (ok) {
            qDebug() << "Inserted record into" << tableName << "with id:" << insertedId;
            return insertedId;
        } else {
            qWarning() << "Failed to retrieve id from query result";
            return -1;
        }
    } else {
        qWarning() << "No id returned from query";
        return -1;
    }
}



#if 0

static int max_changed_object = 0;

int get_max_changed_object()
{
    return max_changed_object;
}

static bool m_cxema_updated = false;


void set_cxema_updated(bool yes)
{
    m_cxema_updated = yes;
}



void reset_max_changed_object(QSqlDatabase &db)
{
    max_changed_object = readTableValue(db, "select max(id) as id from changed_object").toInt();
//    set_cxema_updated(false);
}

#endif


static int max_changed_object = 0;

int get_max_changed_object()
{
    return max_changed_object;
}

static bool m_cxema_updated = false;


void set_cxema_updated(bool yes)
{
    m_cxema_updated = yes;
}



void reset_max_changed_object(QSqlDatabase &db)
{
    max_changed_object = readTableValue(db, "select max(log_id) as id from audit_log").toInt();
//    set_cxema_updated(false);
}



bool updateRow(QSqlDatabase &m_db, const QString &table, int id, const QMap<QString, QVariant> &data)
{
    if (data.isEmpty()) {
        qWarning() << "No data to update";
        return false;
    }

    // Формируем SET field1=?, field2=?, ...
    QStringList setParts;
    for (auto it = data.begin(); it != data.end(); ++it) {
        setParts << QString("%1 = ?").arg(it.key());
    }
    QString setClause = setParts.join(", ");

    QString sql = QString("UPDATE %1 SET %2 WHERE id = ?").arg(table, setClause);


    qDebug() << "-------------------------";
    qDebug() << sql;

    QSqlQuery query(m_db);
    query.prepare(sql);

    // Привязываем значения
    for (auto it = data.begin(); it != data.end(); ++it) {
        query.addBindValue(it.value());
        qDebug() << it.key() << " " << it.value();
    }

    qDebug() << "-------------------------";

    // Привязываем id
    query.addBindValue(id);

    if (!query.exec()) {
        qWarning() << "Update failed:" << query.lastError().text();
        return false;
    }

    return true;
}

/*

bool checkTableExists(QSqlDatabase &db, const QString &schema, const QString &table) {
    QSqlQuery query(db);
    query.prepare(R"(
        SELECT 1 
        FROM information_schema.tables 
        WHERE table_schema = :schema 
          AND table_name = :table
        LIMIT 1
    )");
    query.bindValue(":schema", schema);
    query.bindValue(":table", table);

    if (!query.exec()) {
        qWarning() << "Query error:" << query.lastError().text();
        return false;
    }
    return query.next(); // true если что-то вернул
}

*/


bool checkTableExists(QSqlDatabase &db, const QString &schema, const QString &table) 
{
    QSqlQuery query(db);

    QString driver = db.driverName().toUpper();

//    if (driver == "QPSQL") {
    if (is_POSTGRESQL()) {
//     
        // PostgreSQL
        query.prepare(R"(
            SELECT 1
            FROM information_schema.tables
            WHERE table_schema = :schema
              AND table_name = :table
            LIMIT 1
        )");
        query.bindValue(":schema", schema);
        query.bindValue(":table", table);
    }
    else if (is_MSSQL()) {
//    else if (driver == "QODBC" || driver == "QMSSQL") {
        // MS SQL
        query.prepare(R"(
            SELECT 1
            FROM sys.tables t
            JOIN sys.schemas s ON t.schema_id = s.schema_id
            WHERE s.name = :schema AND t.name = :table
        )");
        query.bindValue(":schema", schema);
        query.bindValue(":table", table);
    }
    else {
        qWarning() << "Unsupported driver:" << driver;
        return false;
    }

    if (!query.exec()) {
        qWarning() << "Query error:" << query.lastError().text();
        return false;
    }

    return query.next(); // true если вернулась хотя бы одна строка
}





#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlDatabase>
#include <QVariant>
#include <QDebug>

QVariant duplicateRow(const QString &tableName, int sourceId, const QString &keyField = "id")
{
    QSqlQuery query;

    // 1. Получаем исходную строку
    QString selectSql = QString("SELECT * FROM %1 WHERE %2 = :id")
                        .arg(tableName, keyField);
    query.prepare(selectSql);
    query.bindValue(":id", sourceId);

    if (!query.exec() || !query.next()) {
        qWarning() << "Не удалось получить строку:" << query.lastError().text();
        return QVariant();
    }

    QSqlRecord rec = query.record();

    // 2. Собираем список полей и значений (кроме ключевого)
    QStringList fieldNames;
    QList<QVariant> values;
    for (int i = 0; i < rec.count(); ++i) {
        QString fieldName = rec.fieldName(i);
        if (fieldName == keyField) continue;
        fieldNames << fieldName;
        values << rec.value(i);
    }

    // 3. Вставка
    QSqlDatabase db = QSqlDatabase::database();
    QString insertSql;

    if (db.driverName().startsWith("QPSQL")) {
        // PostgreSQL — используем RETURNING
        insertSql = QString("INSERT INTO %1 (%2) VALUES (%3) RETURNING %4")
                        .arg(tableName)
                        .arg(fieldNames.join(", "))
                        .arg(QString("?, ").repeated(fieldNames.size()).chopped(2))
                        .arg(keyField);
    } else {
        // SQLite, MySQL — без RETURNING
        insertSql = QString("INSERT INTO %1 (%2) VALUES (%3)")
                        .arg(tableName)
                        .arg(fieldNames.join(", "))
                        .arg(QString("?, ").repeated(fieldNames.size()).chopped(2));
    }

    query.prepare(insertSql);
    for (const QVariant &v : values)
        query.addBindValue(v);

    if (!query.exec()) {
        qWarning() << "Ошибка вставки:" << query.lastError().text();
        return QVariant();
    }

    // 4. Получаем id
    QVariant newId;
    if (db.driverName().startsWith("QPSQL")) {
        // в RETURNING уже есть id
        if (query.next())
            newId = query.value(0);
    } else if (db.driverName().startsWith("QSQLITE")) {
        QSqlQuery idQuery("SELECT last_insert_rowid()");
        if (idQuery.next())
            newId = idQuery.value(0);
    } else if (db.driverName().startsWith("QMYSQL")) {
        QSqlQuery idQuery("SELECT LAST_INSERT_ID()");
        if (idQuery.next())
            newId = idQuery.value(0);
    } else {
        // fallback — может сработать, если драйвер поддерживает
        newId = query.lastInsertId();
    }

    return newId;
}
