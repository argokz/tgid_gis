#include <QtGui>
#include <QtWidgets>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include "OpenServer.h"
#include <edit/menu.h>
#include <db/db.h>

#include <mystd.h>

QString getListOfGidTables(const QString & user)
{
  QString q =

    "SET NOCOUNT ON\n"
    "DECLARE @AllTables TABLE\n"
    "        (\n"
    "         ServerName NVARCHAR(200)\n"
    "        ,DBName NVARCHAR(200)\n"
    "        ,SchemaName NVARCHAR(200)\n"
    "        ,TableName NVARCHAR(200)\n"
    "        )\n"
    "DECLARE @SearchSvr NVARCHAR(200)\n"
    "       ,@SearchDB NVARCHAR(200)\n"
    "       ,@SearchS NVARCHAR(200)\n"
    "       ,@SearchTbl NVARCHAR(200)\n"
    "       ,@SQL NVARCHAR(4000)\n"

    "SET @SearchSvr = NULL  --Search for Servers, NULL for all Servers\n"
    "SET @SearchDB = NULL  --Search for DB, NULL for all Databases\n"
    "SET @SearchS = NULL  --Search for Schemas, NULL for all Schemas\n"
    "SET @SearchTbl = NULL  --Search for Tables, NULL for all Tables\n"

    "SET @SQL = 'SELECT @@SERVERNAME\n"
    "        ,''?''\n"
    "        ,s.name\n"
    "        ,t.name\n"
    "         FROM [?].sys.tables t \n"
    "         JOIN sys.schemas s on t.schema_id=s.schema_id \n"
    "         WHERE @@SERVERNAME LIKE ''%' + ISNULL(@SearchSvr, '') + '%''\n"
    "         AND ''?'' LIKE ''%' + ISNULL(@SearchDB, '') + '%''\n"
    "         AND s.name LIKE ''%' + ISNULL(@SearchS, '') + '%''\n"
    "         AND t.name LIKE ''%' + ISNULL(@SearchTbl, '') + '%''\n"
    "         AND ''?'' NOT IN (''master'',''model'',''msdb'',''tempdb'',''SSISDB'')\n"
    "           '\n"
    "-- Remove the '--' from the last statement in the WHERE clause to exclude system tables\n"

    "INSERT  INTO @AllTables\n"
    "        (\n"
    "         ServerName\n"
    "        ,DBName\n"
    "        ,SchemaName\n"
    "        ,TableName\n"
    "        )\n"
    "        EXEC sp_MSforeachdb @SQL\n"
    "SET NOCOUNT OFF\n"
    // "SELECT  *\n"
    "SELECT  DBName\n"
    "FROM    @AllTables\n"
    "WHERE TableName = 'heatpipesections'\n"
    // "ORDER BY 1,2,3,4\n";
    "ORDER BY DBName\n";


    if (is_POSTGRESQL()) {
        q = "SELECT datname FROM pg_database where not datistemplate order by datname";
//        q = QString("SELECT datname FROM pg_database WHERE has_database_privilege('tgid', datname, 'CONNECT') AND datname NOT IN ('template0', 'template1', 'postgres') ORDER BY datname").arg(user);
        q = QString("SELECT datname\nFROM pg_database\nWHERE (has_database_privilege('%1', datname, 'CONNECT') AND NOT datname LIKE 'template%' AND datname NOT IN ('postgres', 'sprav'))").arg(user);
    }


    return q;

}


QString getDB(QWidget *wnd, ConnectStr cs, bool is_gid)
{
    QStringList lst;
    QSqlDatabase db;
    QString name = "";
    if (is_POSTGRESQL()) {
        name = "template1";
    }

    bool ok = connectSQL0(cs.rdbms, cs.host, cs.port, name, cs.user, cs.password, db);

    if (!ok) {
        ok = connectSQL0(cs.rdbms, cs.host, cs.port, cs.baza, cs.user, cs.password, db);
    }

    if (ok) {
        QSqlQuery query(db);
        query.setForwardOnly(true);

        QString q;

        if (is_gid) {
            q = getListOfGidTables(cs.user);
        }
        else {
            q = "SELECT name, database_id, create_date FROM sys.databases ORDER BY name";

            if (is_POSTGRESQL()) {
                q = "SELECT datname FROM pg_database where not datistemplate order by datname";
//                q = QString("SELECT datname\nFROM pg_database\nWHERE (has_database_privilege('%1', datname, 'CONNECT') AND datname NOT IN ('template0', 'template1', 'postgres', 'sprav'))").arg(cs.user);
                q = QString("SELECT datname\nFROM pg_database\nWHERE (has_database_privilege('%1', datname, 'CONNECT') AND NOT datname LIKE 'template%' AND datname NOT IN ('postgres', 'sprav'))").arg(cs.user);
            }
        }


//        QInputDialog::getMultiLineText(nullptr, "title", "label", q);

        query_exec(db, query, q);

        while (query.next()) {
            QString s = query.value(0).toString();
            if (s != "vsp" && s != "sprav" && s != "master" && s != "tempdb" && s != "model" && s != "msdb") {
                lst.push_back(s);
             }
        }

        lst.sort();

        MenuDial md(lst, QObject::tr("Базы данных"), 0);
        if (md.exec() == QDialog::Accepted) {
            return md.curText;
        }
    }
    return "";
}

static ConnectStr cs0;
static bool first = true;

void init_cs0(ConnectStr &cs)
{
    cs.rdbms = 1;   // postgreSQL
    cs.user = "Lifan";
    cs.password = "Danil228";
    cs.port = 1433;
    cs.port = 5432;
    cs.baza = "AstanaGID";
    cs.geo = "Astana";
    cs.baza = "demo_stand";
    cs.geo = "demo_stand_map";

    cs.host = "localhost";
    cs.user = "demo";
    cs.password = "itwin";

/*
    if (first) {
        first = false;
        QString fn = QString("%1kls/config.ini").arg(argpath());
        QSettings settings(fn, QSettings::IniFormat);

        cs.rdbms =    settings.value("cs/rdbms"    , QVariant(cs.rdbms)).toInt();
        cs.host =     settings.value("cs/host"    , QVariant(cs.host)).toString();
        cs.port =     settings.value("cs/port"    , QVariant(cs.port)).toInt();
        cs.user =     settings.value("cs/user"    , QVariant(cs.user)).toString();
        cs.password = settings.value("cs/password", QVariant(cs.password)).toString();
        cs.baza =     settings.value("cs/baza"    , QVariant(cs.baza)).toString();
        cs.geo =      settings.value("cs/geo"     , QVariant(cs.geo)).toString();
    }
*/
}

#include <config.h>

void init_cs(ConnectStr &cs)
{
//    init_cs0(cs);
    cs = get_ini()->cs;


    QSettings settings;

    cs.rdbms =    settings.value("cs/rdbms"   , QVariant(cs.rdbms)).toInt();
    cs.host =     settings.value("cs/host"    , QVariant(cs.host)).toString();
    cs.port =     settings.value("cs/port"    , QVariant(cs.port)).toInt();
    cs.user =     settings.value("cs/user"    , QVariant(cs.user)).toString();
    cs.password = settings.value("cs/password", QVariant(cs.password)).toString();
    cs.baza =     settings.value("cs/baza"    , QVariant(cs.baza)).toString();
    cs.geo =      settings.value("cs/geo"     , QVariant(cs.geo)).toString();
    cs.gid9 =     settings.value("cs/gid9"    , QVariant(cs.gid9)).toString();
}


COpenServer::COpenServer(QWidget *parent)
  : QDialog(parent)
{
    ui.setupUi(this);

    ui.edit_PASSWORD->setEchoMode(QLineEdit::Password);
    ui.edit_PORT->setValidator(new QIntValidator(1, 65535, this));

    init_cs(cs);

    write();

    connect(ui.pushButton_IDOK, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(ui.pushButton_IDCANCEL, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui.pushButton_BD_GID_BTN, SIGNAL(clicked()), this, SLOT(onBdGid()));
    connect(ui.pushButton_BD_GEO_BTN, SIGNAL(clicked()), this, SLOT(onBdGeo()));

    connect(ui.pushButton_CHECK, SIGNAL(clicked()), this, SLOT(onCheck()));
    connect(ui.pushButton_NEW_BAZA, SIGNAL(clicked()), this, SLOT(onNewBaza()));


}

void COpenServer::onCheck()
{
    read();

    QSqlDatabase m_db;
    QString name = "";
    if (is_POSTGRESQL()) {
        name = "template1";
    }


    bool ok = connectSQL0(cs.rdbms, cs.host, cs.port, name, cs.user, cs.password, m_db);

    if (!ok) {
        ok = connectSQL0(cs.rdbms, cs.host, cs.port, cs.baza, cs.user, cs.password, m_db);
    }
    
    if (ok) {
        QMessageBox::about(this, "", "OK!");
    }
    else {
        QMessageBox::warning(this, "", m_db.lastError().text());
    }
}



void COpenServer::write()
{
    ui.edit_IP->setText(cs.host);
    ui.edit_USER->setText(cs.user);
    ui.edit_PASSWORD->setText(cs.password);
    ui.edit_PORT->setText(QString("%1").arg(cs.port));
    ui.edit_BD_GID->setText(cs.baza);
    ui.edit_BD_GEO->setText(cs.geo);
}


void COpenServer::read()
{
    cs.host =     ui.edit_IP->text();
    cs.port =     ui.edit_PORT->text().toInt();
    cs.user =     ui.edit_USER->text();
    cs.password = ui.edit_PASSWORD->text();
    cs.baza =     ui.edit_BD_GID->text();
    cs.geo =      ui.edit_BD_GEO->text();
}

void saveConnectStr(ConnectStr &cs)
{
    QSettings settings;

    settings.setValue("cs/rdbms", cs.rdbms);

    settings.setValue("cs/host", cs.host);
    settings.setValue("cs/port", cs.port);
    settings.setValue("cs/user", cs.user);
    settings.setValue("cs/password", cs.password);
    settings.setValue("cs/baza", cs.baza);
    settings.setValue("cs/geo", cs.geo);
    settings.setValue("cs/gid9", cs.gid9);
}


void COpenServer::okClicked()
{
    read();

    saveConnectStr(cs);

    done(QDialog::Accepted);
}

void COpenServer::onBdGid()
{
    QString s = getDB(this, cs, true);

    if (s != "") {
        cs.baza = s;
        write();
    }
}

void COpenServer::onBdGeo()
{
    QString s = getDB(this, cs, false);

    if (s != "") {
        cs.geo = s;
        write();
    }
}

bool check_name_bd(const QString & bn)
{
    if (bn.size() <= 0) return false;

    QChar ch = bn.at(0);
    
    if ('0' <= ch && ch <= '9') return false;
    
    for (int i = 0; i < bn.size(); i++) {
        QChar ch = bn.at(i);
        if ('0' <= ch && ch <= '9') continue;
        if ('a' <= ch && ch <= 'z') continue;
        if ('A' <= ch && ch <= 'Z') continue;
        if (ch == '_') continue;
        return false;
    }
    return true;
}

bool isBaza(QSqlDatabase &db, const QString & baza)
{
    QString q;

    if (is_POSTGRESQL()) {
        q = QString("SELECT datname FROM pg_catalog.pg_database WHERE datname = '%1'").arg(baza);
    }
    else {
        q = QString("SELECT name, database_id, create_date FROM sys.databases WHERE name='%1'").arg(baza);
    }

    long n = sizeOfQ(db, q);
    return n > 0;
}

bool newBazaMsSql(QWidget *widget, ConnectStr &cs, QSqlDatabase &db0, const QString & baza, const QString & path, bool ask)
{
    QString s, q;

    if (isBaza(db0, baza)) {
        QMessageBox::warning(widget, "", QString(QObject::tr("База данных %1 уже существует")).arg(baza));
        return false;
    }

    if (ask && !QMessageBox::question(widget, "",  QString(QObject::tr("Создать Базу данных %1?")).arg(baza))) return false;

    {
        q = QString("CREATE DATABASE [%1] COLLATE Cyrillic_General_CI_AS").arg(baza);

        QSqlQuery query(db0);
        bool ret = query_exec(db0, query, q);
        if (!ret) {
            ErrorMessage(query);
            return false;
        }
    }

    q = readFile(path);

    QProgressDialog pc("Операция выполняется, подождите пожалуйста...", "Прервать", 0, 100, nullptr);
    pc.setWindowModality(Qt::WindowModal);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    pc.move(screenGeometry.center() - pc.rect().center());

    pc.show();

//    Percent2 pc;
    QString q0 = q;
    int nn = 0, n = 0;
    int i1 = 0;

    QString p1 = "\nGO\n";

    while (1) {
        int i2 = q0.indexOf(p1, i1);
        if (i2 == -1) break;
        i1 = i2 + p1.length();
        nn ++;
    }

    i1 = 0;

    QSqlDatabase db;

    bool ok = connectSQL0(cs.rdbms, cs.host, cs.port, baza, cs.user, cs.password, db);
    if (!ok) return false;

    db.open();

    QSqlQuery query(db);

    while (1) {
        pc.setValue(n*100./nn);
        QApplication::processEvents();

        int i2 = q.indexOf(p1, i1);
        if (i2 == -1) break;

        QString q1 = q.mid(i1, i2-i1);

        while (1) {
            static QRegularExpression re("[\\n]SET IDENTITY_INSERT [^\\n]+");
            QRegularExpressionMatch match = re.match(q1);
            if (!match.hasMatch()) {
                static QRegularExpression re("^SET IDENTITY_INSERT [^\\n]+");
                match = re.match(q1);
            }
            if (!match.hasMatch()) {
                break;
            }
            QString m = match.captured(0);
            q1.replace(m, "");
        }

        static QRegularExpression re("[\\n]INSERT \\[dbo\\]\\.\\[([^\\]]+)\\]");
        QRegularExpressionMatch match = re.match(q1);

        if (match.hasMatch()) {
            QString tn = match.captured(1);

            QString s1 = QString("SET IDENTITY_INSERT [dbo].[%1] ON\n").arg(tn);
            QString s2 = QString("SET IDENTITY_INSERT [dbo].[%1] OFF\n").arg(tn);

            q1 = s1 + q1 + "\n" + s2;
        }

        q1 = q1.trimmed();

        if (q1 != "" && q1 != "GO") {
            bool ret = query_exec(db, query, q1);
            if (!ret) {
                ErrorMessage(query);
                return false;
            }
        }

        i1 = i2 + p1.length();
        n ++;
    }

    if (ask) {
        QMessageBox::warning(widget, "", QObject::tr("Выполнено"));
    }
    return true;
}

bool updateUserRight(QSqlDatabase &db, const QString & user, int right);


bool newBaza(QWidget *widget, ConnectStr &cs, QSqlDatabase &db0, const QString & baza, const QString & path0, bool ask, bool tgid)
{
    QString path = path0;
    QString s, q;

    if (is_MSSQL()) {
        return newBazaMsSql(widget, cs, db0, baza, path, ask);
    }


    if (is_POSTGRESQL()) {
        path = "postgresql/" + path;

        if (isBaza(db0, baza) && (baza != "template_gid")) {
            QMessageBox::warning(widget, "", QString(QObject::tr("База данных %1 уже существует")).arg(baza));
            return false;
        }

        if (!isBaza(db0, baza) && (baza != "template_gid" && tgid)) {
            if (QMessageBox::question(0, "", "Создать новый шаблон Базы данных?") == QMessageBox::Yes) {
                if (!newBaza(widget, cs, db0, "template_gid", path0, false, true)) {
                    return false;
                }
            }
        }
    }


    if (ask && !QMessageBox::question(widget, "",  QString(QObject::tr("Создать Базу данных %1?")).arg(baza))) return false;

#if 1
    if (isBaza(db0, "template_gid") && baza != "template_gid" && tgid) {
        q = QString("CREATE DATABASE %1 WITH TEMPLATE = template_gid").arg(baza);

        QSqlQuery query(db0);
        bool ret = query_exec(db0, query, q);
        if (!ret) {
            ErrorMessage(query);
            return false;
        }

        query_exec(db0, QString("REVOKE CONNECT ON DATABASE %1 FROM PUBLIC").arg(baza));
//        query_exec(db0, QString("REVOKE CONNECT ON DATABASE %1 FROM ALL").arg(baza));

        query_exec(db0, QString("REVOKE ALL PRIVILEGES ON DATABASE %1 FROM public;").arg(baza));

        query_exec(db0, QString("GRANT ALL ON SCHEMA public TO %1;").arg(cs.user));
        query_exec(db0, QString("GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO %1;").arg(cs.user));
        query_exec(db0, QString("GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public TO %1;").arg(cs.user));
        query_exec(db0, QString("GRANT ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA public TO %1;").arg(cs.user));

//        updateUserRight(db0, "Администратор", 260);

        return true;
    }
#endif

    QProgressDialog pc("Операция выполняется, подождите пожалуйста...", "Прервать", 0, 100, widget);
    pc.setWindowModality(Qt::WindowModal);
    pc.show();
    pc.setValue(0);

    {
        QSqlQuery query(db0);

        if (baza == "template_gid") {
            query_exec(db0, query, QString("ALTER DATABASE %1 WITH ALLOW_CONNECTIONS true;").arg(baza), false);
            query_exec(db0, query, QString("ALTER DATABASE %1 WITH is_template = false;").arg(baza), false);
            query_exec(db0, query, QString("SELECT pg_terminate_backend(pg_stat_activity.pid) FROM pg_stat_activity WHERE datname = '%1';").arg(baza), false);
            query_exec(db0, query, QString("DROP DATABASE %1;").arg(baza), false);

            q = QString("CREATE DATABASE %1 WITH TEMPLATE = template1").arg(baza);
            q = QString("CREATE DATABASE %1 WITH TEMPLATE = template0").arg(baza);

            bool ret = query_exec(db0, query, q);
            if (!ret) {
                ErrorMessage(query);
                return false;
            }
        }
    }

    q = readFile(path);

    void split_sql_expressions(const QString & text, std::list<QString> &list_q);

    std::list<QString> list_q;

    split_sql_expressions(q, list_q);


    int nn = list_q.size();

    if (nn == 0) {
        QMessageBox::about(widget, "", "Ошибка");
        return false;
    }


    QSqlDatabase db;

    bool ok = connectSQL0(cs.rdbms, cs.host, cs.port, baza, cs.user, cs.password, db);
    if (!ok) return false;
    db.open();

//    QProgressDialog pc("Операция выполняется, подождите пожалуйста...", "Прервать", 0, 100, widget);
//    pc.setWindowModality(Qt::WindowModal);
//    pc.show();

    int n = 0;

    if (is_POSTGRESQL()) {
        query_exec(db, "CREATE EXTENSION postgis;");
        query_exec(db, "CREATE EXTENSION dblink;");
    }

    for (auto & q: list_q) {
//        QApplication::processEvents();
        pc.setValue(n*100./nn);
        query_exec(db, q);
        n += 1;
    }

    if (is_POSTGRESQL()) {
        if (baza == "template_gid" && tgid) {
            query_exec(db, QString("ALTER DATABASE %1 WITH is_template = true;").arg(baza));

            query_exec(db, "GRANT ALL ON SCHEMA public TO public;");
            query_exec(db, "GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO public;");
            query_exec(db, "GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public TO public;");
            query_exec(db, "GRANT ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA public TO public;");

            query_exec(db0, QString("ALTER DATABASE %1 WITH ALLOW_CONNECTIONS false;").arg(baza));
            query_exec(db0, QString("SELECT pg_terminate_backend(pg_stat_activity.pid) FROM pg_stat_activity   WHERE datname = '%1';").arg(baza));


        }
    }

    if (ask) {
        QMessageBox::warning(widget, "", QString(QObject::tr("Выполнено создание Базы Данных")).arg(baza));
    }
    return true;
}


bool getCoordSystem(QWidget *widget, QString &proj, QString & wkt, QString & crsName, QString & dbName, QString & sql);


void newBaza0(QWidget *widget, ConnectStr &cs)
{

#if 0
    QString proj, wkt, baza, crs, sql;



    bool ok = getCoordSystem(widget, proj, wkt, crs, baza, sql);

#else

    bool ok;
    QString baza = QInputDialog::getText(widget, QObject::tr("База данных"), QObject::tr("Введите название новой базы"), QLineEdit::Normal, "", &ok);

#endif

    if (!ok) return;


    if (!check_name_bd(baza)) {
        QMessageBox::warning(widget, "", QObject::tr("Название базы данных должно состоять из букв латинскогот алфавита, цифр, подчеркиваний и не содержать пробелов"));
        return;
    }

    QSqlDatabase db;

    QString name = "";
    if (is_POSTGRESQL()) {
        name = "template1";
    }

    ok = connectSQL0(cs.rdbms, cs.host, cs.port, name, cs.user, cs.password, db);

    if (ok) {
        bool b2 = true;
        bool b3 = true;

//        if (!isBaza(db,  "vsp")) {
//            b2 = newBaza(widget, cs, db, "vsp", "sql2/vsp.sql", false);
//        }

        if (b2 && !isBaza(db,  "sprav")) {
            b3 = newBaza(widget, cs, db, "sprav", "sql2/sprav.sql", false, false);
        }

        bool b1 = newBaza(widget, cs, db, baza, "sql2/full_tgid.sql", true, true);

#if 0


        bool retQ = execQ(db, sql); 
        if (!retQ) {
            QMessageBox::warning(widget, "", QObject::tr("Ошибка при создании Системы координат"));
        }
#endif

        if (b1 || b2 || b3) {
            QMessageBox::warning(widget, "", QObject::tr("Выполнено"));
        }
    }
    else {
        QMessageBox::warning(widget, "", db.lastError().text());
    }
}

void newBaza0(QWidget *widget)
{
    ConnectStr cs;
    init_cs(cs);
    newBaza0(widget, cs);
}

void COpenServer::onNewBaza()
{
    newBaza0(this, cs);
}

