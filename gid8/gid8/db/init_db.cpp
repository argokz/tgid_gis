#include <QApplication>
#include <QtSql>
#include <iostream>
#include "db.h"
#include <mystd.h>
#include <cxema/csv.h>

static std::map<std::tuple<QString, QString, QString> , std::list<AdoField> > map_fields;

QString make_file_name(const QString & txt0)
{
    QString txt = txt0;        

    txt = txt.toLower().trimmed();
    txt = txt.replace(" " , "_");

    txt = txt.replace("!" , "_");
    txt = txt.replace("\"" , "_");
    txt = txt.replace("#" , "_");
    txt = txt.replace("$" , "_");
    txt = txt.replace("%" , "_");
    txt = txt.replace("&" , "_");
    txt = txt.replace("'" , "_");
    txt = txt.replace("*" , "_");
    txt = txt.replace("+" , "_");
    txt = txt.replace("/" , "_");
    txt = txt.replace(":" , "_");
    txt = txt.replace("<" , "_");
    txt = txt.replace("=" , "_");
    txt = txt.replace(">" , "_");
    txt = txt.replace("?" , "_");
    txt = txt.replace("@" , "_");
    txt = txt.replace("\\" , "_");
    txt = txt.replace("`" , "_");
    txt = txt.replace("{" , "_");
    txt = txt.replace("|" , "_");
    txt = txt.replace("}" , "_");

    return txt;
}

QString getTablesName(QSqlDatabase &db, const QString & table0, const QString & databasename0 = "")
{
    QString table = table0;
    QString databasename = databasename0;

    if (databasename == "") {
        databasename = getDatabaseName(db);
    }
    QString host = make_file_name(db.hostName());
    table = make_file_name(table);

    QString fn = QString("%1/tables/%2/%3/%4.txt").arg(argpath_2(), host, databasename, table);
    return fn;
}

void save_1_field(QTextStream & out, AdoField & field)
{
    out << "\"" << field.TABLE_CATALOG << "\",";
    out << "\"" << field.TABLE_SCHEMA << "\",";
    out << "\"" << field.TABLE_NAME << "\",";
    out << "\"" << field.COLUMN_NAME << "\",";
    out <<         field.ORDINAL_POSITION << ",";
    out << "\"" << field.COLUMN_DEFAULT << "\",";
    out << "\"" << field.IS_NULLABLE << "\",";
    out << "\"" << field.DATA_TYPE << "\",";
    out <<         field.CHARACTER_MAXIMUM_LENGTH << ",";
    out <<         field.CHARACTER_OCTET_LENGTH << ",";
    out <<         field.NUMERIC_PRECISION << ",";
    out <<         field.NUMERIC_PRECISION_RADIX << ",";
    out <<         field.NUMERIC_SCALE << ",";
    out <<         field.DATETIME_PRECISION << ",";
    out << "\"" << field.CHARACTER_SET_CATALOG << "\",";
//    out << "\"" << field.CHARACTER_SET_SCHEMA << "\",";
    out << "\"" << field.CHARACTER_SET_NAME << "\",";
//    out << "\"" << field.COLLATION_CATALOG << "\",";
//    out << "\"" << field.COLLATION_SCHEMA << "\",";
    out << "\"" << field.COLLATION_NAME << "\"\n";
//    out << "\"" << field.DOMAIN_CATALOG << "\",";
//    out << "\"" << field.DOMAIN_SCHEMA << "\",";
//    out << "\"" << field.DOMAIN_NAME << "\"\n";
}

bool save_fields(QSqlDatabase &db, const QString & table, std::list<AdoField> & fields)
{
    QString fn = getTablesName(db, table);

    mkdir_for_file(fn);

    QFile file(fn);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {

        QTextStream out(&file);

        for (auto & it : fields) {
            save_1_field(out, it);
        }
        file.close();
        return true;
    }
    return false;
}


bool read_fields(QSqlDatabase &db, const QString & table, std::list<AdoField> & fields)
{
    QString fn = getTablesName(db, table);

    fields.clear();

    QFile file(fn);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {

        QTextStream in(&file);

        while (!in.atEnd()) {
            AdoField field;

            QString line = in.readLine();
            std::vector<QString> v;

            v = csv_read(line);

            int i = 0;

            field.TABLE_CATALOG = v[i++];
            field.TABLE_SCHEMA = v[i++];
            field.TABLE_NAME = v[i++];
            field.COLUMN_NAME = v[i++];
            field.ORDINAL_POSITION = v[i++].toInt();
            field.COLUMN_DEFAULT = v[i++];
            field.IS_NULLABLE = v[i++];
            field.DATA_TYPE = v[i++];
            field.CHARACTER_MAXIMUM_LENGTH = v[i++].toInt();
            field.CHARACTER_OCTET_LENGTH = v[i++].toInt();
            field.NUMERIC_PRECISION = v[i++].toInt();
            field.NUMERIC_PRECISION_RADIX = v[i++].toInt();
            field.NUMERIC_SCALE = v[i++].toInt();
            field.DATETIME_PRECISION = v[i++].toInt();
            field.CHARACTER_SET_CATALOG = v[i++];
//            field.CHARACTER_SET_SCHEMA = v[i++];
            field.CHARACTER_SET_NAME = v[i++];
//            field.COLLATION_CATALOG = v[i++];
//            field.COLLATION_SCHEMA = v[i++];
            field.COLLATION_NAME = v[i++];
//            field.DOMAIN_CATALOG = v[i++];
//            field.DOMAIN_SCHEMA = v[i++];
//            field.DOMAIN_NAME = v[i++];

            fields.push_back(field);
        }

        file.close();
        return true;
    }
    return false;
}

#include <QProgressDialog>

// Прочитать все таблицы

void init_table_columns_sqlite(QSqlDatabase & db)
{
    QSqlQuery query(db);

    QString q = QString(

" SELECT\n"
" TABLE_CATALOG,\n"
" TABLE_SCHEMA,\n"
" TABLE_NAME,\n"
" COLUMN_NAME,\n"
" ORDINAL_POSITION,\n"
" COLUMN_DEFAULT,\n"
" IS_NULLABLE,\n"
" DATA_TYPE,\n"
" CHARACTER_MAXIMUM_LENGTH,\n"
" CHARACTER_OCTET_LENGTH,\n"
" NUMERIC_PRECISION,\n"
" NUMERIC_PRECISION_RADIX,\n"
" NUMERIC_SCALE,\n"
" DATETIME_PRECISION,\n"
" CHARACTER_SET_CATALOG,\n"
" CHARACTER_SET_SCHEMA,\n"
" CHARACTER_SET_NAME,\n"
" COLLATION_CATALOG,\n"
" COLLATION_SCHEMA,\n"
" COLLATION_NAME,\n"
" DOMAIN_CATALOG,\n"
" DOMAIN_SCHEMA,\n"
" DOMAIN_NAME\n"
" FROM INFORMATION_SCHEMA.COLUMNS col\n"
" ORDER BY TABLE_NAME, TABLE_SCHEMA, ordinal_position\n");

    q = 

"SELECT m.name as table_name, p.name as column_name, p.type as data_type\n"
"FROM sqlite_master m\n"
"left outer join pragma_table_info((m.name)) p\n"
"     on m.name <> p.name\n"
//"order by table_name, column_name\n"
;



    QString fn = "";
    QString fn0 = "";

    QFile *file = nullptr;

    QProgressDialog pc("Операция выполняется, подождите пожалуйста...", "Прервать", 0, 100, nullptr);
    pc.setWindowModality(Qt::WindowModal);
    pc.show();

    int i = 0;
    int n = db.tables(QSql::AllTables).size();

    if (query_exec(db, query, q)) {
        while (query.next()) {
            AdoField field;

            field.TABLE_CATALOG = query.value("TABLE_CATALOG").toString();
            field.TABLE_SCHEMA = query.value("TABLE_SCHEMA").toString();
            field.TABLE_NAME = query.value("TABLE_NAME").toString();
            field.COLUMN_NAME = query.value("COLUMN_NAME").toString();
            field.ORDINAL_POSITION = query.value("ORDINAL_POSITION").toInt();
            field.COLUMN_DEFAULT = query.value("COLUMN_DEFAULT").toString();
            field.IS_NULLABLE = query.value("IS_NULLABLE").toString();
            field.DATA_TYPE = query.value("DATA_TYPE").toString();
            field.CHARACTER_MAXIMUM_LENGTH = query.value("CHARACTER_MAXIMUM_LENGTH").toInt();
            field.CHARACTER_OCTET_LENGTH = query.value("CHARACTER_OCTET_LENGTH").toInt();
            field.NUMERIC_PRECISION = query.value("NUMERIC_PRECISION").toInt();
            field.NUMERIC_PRECISION_RADIX = query.value("NUMERIC_PRECISION_RADIX").toInt();
            field.NUMERIC_SCALE = query.value("NUMERIC_SCALE").toInt();
            field.DATETIME_PRECISION = query.value("DATETIME_PRECISION").toInt();
            field.CHARACTER_SET_CATALOG = query.value("CHARACTER_SET_CATALOG").toString();
//            field.CHARACTER_SET_SCHEMA = query.value("CHARACTER_SET_SCHEMA").toString();
            field.CHARACTER_SET_NAME = query.value("CHARACTER_SET_NAME").toString();
//            field.COLLATION_CATALOG = query.value("COLLATION_CATALOG").toString();
//            field.COLLATION_SCHEMA = query.value("COLLATION_SCHEMA").toString();
            field.COLLATION_NAME = query.value("COLLATION_NAME").toString();
//            field.DOMAIN_CATALOG = query.value("DOMAIN_CATALOG").toString();
//            field.DOMAIN_SCHEMA = query.value("DOMAIN_SCHEMA").toString();
//            field.DOMAIN_NAME = query.value("DOMAIN_NAME").toString();

            fn = getTablesName(db, field.TABLE_NAME, field.TABLE_CATALOG);

//            fn = QString("%1/tables/%2/%3/%4.txt").arg(argpath_2(), db.hostName(), field.TABLE_CATALOG, field.TABLE_NAME);
            if (fn != fn0) {
                pc.setValue(i*100/n);
                QApplication::processEvents();
                i++;
                mkdir_for_file(fn);
                fn0 = fn;
                if (file) {
                    file->close();
                    delete file;
                }    
                file = new QFile(fn);
                if (!file->open(QIODevice::WriteOnly | QIODevice::Text)) return;
            }

            QTextStream out(file);
            save_1_field(out, field);
        }
    }
    if (file) {
        file->close();
        delete file;
    }
}




void init_table_columns(QSqlDatabase & db)
{

    if (is_SQLITE()) {
        init_table_columns_sqlite(db);
        return;
    }
    
    
    QSqlQuery query(db);

    QString q = QString(

" SELECT\n"
" TABLE_CATALOG,\n"
" TABLE_SCHEMA,\n"
" TABLE_NAME,\n"
" COLUMN_NAME,\n"
" ORDINAL_POSITION,\n"
" COLUMN_DEFAULT,\n"
" IS_NULLABLE,\n"
" DATA_TYPE,\n"
" CHARACTER_MAXIMUM_LENGTH,\n"
" CHARACTER_OCTET_LENGTH,\n"
" NUMERIC_PRECISION,\n"
" NUMERIC_PRECISION_RADIX,\n"
" NUMERIC_SCALE,\n"
" DATETIME_PRECISION,\n"
" CHARACTER_SET_CATALOG,\n"
" CHARACTER_SET_SCHEMA,\n"
" CHARACTER_SET_NAME,\n"
" COLLATION_CATALOG,\n"
" COLLATION_SCHEMA,\n"
" COLLATION_NAME,\n"
" DOMAIN_CATALOG,\n"
" DOMAIN_SCHEMA,\n"
" DOMAIN_NAME\n"
" FROM INFORMATION_SCHEMA.COLUMNS col\n"
" ORDER BY TABLE_NAME, TABLE_SCHEMA, ordinal_position\n");

    QString fn = "";
    QString fn0 = "";

    QFile *file = nullptr;

    QProgressDialog pc("Операция выполняется, подождите пожалуйста...", "Прервать", 0, 100, nullptr);
    pc.setWindowModality(Qt::WindowModal);
    pc.show();
    pc.setValue(0);
    QApplication::processEvents();

    int i = 0;
    int n = db.tables(QSql::AllTables).size();

    if (query_exec(db, query, q)) {
        while (query.next()) {
            AdoField field;

            field.TABLE_CATALOG = query.value("TABLE_CATALOG").toString();
            field.TABLE_SCHEMA = query.value("TABLE_SCHEMA").toString();
            field.TABLE_NAME = query.value("TABLE_NAME").toString();
            field.COLUMN_NAME = query.value("COLUMN_NAME").toString();
            field.ORDINAL_POSITION = query.value("ORDINAL_POSITION").toInt();
            field.COLUMN_DEFAULT = query.value("COLUMN_DEFAULT").toString();
            field.IS_NULLABLE = query.value("IS_NULLABLE").toString();
            field.DATA_TYPE = query.value("DATA_TYPE").toString();
            field.CHARACTER_MAXIMUM_LENGTH = query.value("CHARACTER_MAXIMUM_LENGTH").toInt();
            field.CHARACTER_OCTET_LENGTH = query.value("CHARACTER_OCTET_LENGTH").toInt();
            field.NUMERIC_PRECISION = query.value("NUMERIC_PRECISION").toInt();
            field.NUMERIC_PRECISION_RADIX = query.value("NUMERIC_PRECISION_RADIX").toInt();
            field.NUMERIC_SCALE = query.value("NUMERIC_SCALE").toInt();
            field.DATETIME_PRECISION = query.value("DATETIME_PRECISION").toInt();
            field.CHARACTER_SET_CATALOG = query.value("CHARACTER_SET_CATALOG").toString();
//            field.CHARACTER_SET_SCHEMA = query.value("CHARACTER_SET_SCHEMA").toString();
            field.CHARACTER_SET_NAME = query.value("CHARACTER_SET_NAME").toString();
//            field.COLLATION_CATALOG = query.value("COLLATION_CATALOG").toString();
//            field.COLLATION_SCHEMA = query.value("COLLATION_SCHEMA").toString();
            field.COLLATION_NAME = query.value("COLLATION_NAME").toString();
//            field.DOMAIN_CATALOG = query.value("DOMAIN_CATALOG").toString();
//            field.DOMAIN_SCHEMA = query.value("DOMAIN_SCHEMA").toString();
//            field.DOMAIN_NAME = query.value("DOMAIN_NAME").toString();

            QString tn = full_name(field.TABLE_SCHEMA, field.TABLE_NAME);

            fn = getTablesName(db, tn, field.TABLE_CATALOG);

//            fn = QString("%1/tables/%2/%3/%4.txt").arg(argpath_2(), db.hostName(), field.TABLE_CATALOG, field.TABLE_NAME);
            if (fn != fn0) {
                pc.setValue(i*100/n);
                QApplication::processEvents();
                i++;
                mkdir_for_file(fn);
                fn0 = fn;
                if (file) {
                    file->close();
                    delete file;
                }    
                file = new QFile(fn);
                if (!file->open(QIODevice::WriteOnly | QIODevice::Text)) return;
            }

            QTextStream out(file);
            save_1_field(out, field);
        }
    }
    if (file) {
        file->close();
        delete file;
    }
}

bool get_table_columns(QSqlDatabase & db, const QString & tn, std::map<QString, AdoField> & map_fields)
{
    std::list<AdoField> fields_list;
    bool ret = get_table_columns(db, tn, fields_list);

    if (ret) {
        for (auto & it : fields_list) {
            map_fields[it.COLUMN_NAME.toLower()] = it;
        }
    }

    return ret;
}

// Для PG: карточки/экспорт читают net.v_* (tbl_sql), а интроспекция
// раньше смотрела только relkind='r' — view не находились, filtr из
// tab/*.txt оставался пустым, поля в PropertyDial без значений.
static QString resolve_relname_for_columns(const QString & tn)
{
    if (!is_POSTGRESQL()) {
        return tn;
    }
    QString resolved = tbl_sql(tn);
    if (resolved.startsWith(QLatin1Char('"'))) {
        resolved.remove(QLatin1Char('"'));
    }
    return resolved;
}

bool get_table_columns(QSqlDatabase & db, const QString & tn, std::list<AdoField> & fields)
{
    std::tuple<QString, QString, QString> key(db.hostName(), db.databaseName(), tn.toLower());

    auto it = map_fields.find(key);
    if (it != map_fields.end()) {
        fields = it->second;
        return true;
    }

    QString q = QString(

"SELECT\n"
"    s.name AS TABLE_SCHEMA,\n"
"    t.name AS TABLE_NAME,\n"
"    c.name AS COLUMN_NAME,\n"
"    ty.name AS DATA_TYPE,\n"
"    c.max_length AS CHARACTER_MAXIMUM_LENGTH,\n"
"    c.precision AS NUMERIC_PRECISION,\n"
"    c.scale AS NUMERIC_SCALE,\n"
"    c.is_nullable AS IS_NULLABLE,\n"
"    c.column_id AS ORDINAL_POSITION,\n"
"    d.definition AS COLUMN_DEFAULT,\n"
"    COLUMNPROPERTY(c.object_id, c.name, 'IsIdentity') AS IS_IDENTITY\n"
"FROM\n"
"    sys.schemas s\n"
"JOIN sys.tables t ON s.schema_id = t.schema_id\n"
"JOIN sys.columns c ON t.object_id = c.object_id\n"
"JOIN sys.types ty ON c.user_type_id = ty.user_type_id\n"
"LEFT JOIN sys.default_constraints d ON d.object_id=c.default_object_id\n"
"WHERE LOWER(t.name)=LOWER('%1')\n"
"ORDER BY\n"
"    s.name,\n"
"    t.name,\n"
"    c.column_id;\n").arg(tn);


    if (is_POSTGRESQL()) {
    const QString rel = resolve_relname_for_columns(tn);

    q = QString(
"SELECT\n"
"    n.nspname AS table_schema,\n"
"    c.relname AS table_name,\n"
"    a.attname AS column_name,\n"
"    pg_catalog.format_type(a.atttypid, a.atttypmod) AS data_type,\n"
"    a.attnotnull AS is_nullable,\n"
"    pg_catalog.col_description(a.attrelid, a.attnum) AS column_default,\n"
"    a.attnum AS ordinal_position,\n"
"    CASE\n"
"        WHEN pg_catalog.pg_get_serial_sequence(n.nspname || '.' || c.relname, a.attname) IS NOT NULL THEN 'YES'\n"
"        ELSE 'NO'\n"
"    END AS is_identity\n"
"FROM\n"
"    pg_catalog.pg_namespace n\n"
"    INNER JOIN pg_catalog.pg_class c ON n.oid = c.relnamespace\n"
"    INNER JOIN pg_catalog.pg_attribute a ON c.oid = a.attrelid\n"
"WHERE\n"
"    c.relkind IN ('r', 'v', 'm') AND\n"
"    a.attnum > 0 AND NOT a.attisdropped and \n"
"    (LOWER(n.nspname || '.' || c.relname) = LOWER('%1') OR \n"
"    LOWER(c.relname) = LOWER('%1'))\n"
"ORDER BY\n"
"    table_schema,\n"
"    table_name,\n"
                    "    ordinal_position;\n").arg(rel);
                    }

    QSqlQuery query(db);

    if (query_exec(db, query, q)) {
        while (query.next()) {
            AdoField field;

//            field.TABLE_CATALOG = query.value("TABLE_CATALOG").toString();
            field.TABLE_SCHEMA = query.value("TABLE_SCHEMA").toString();
            field.TABLE_NAME = query.value("TABLE_NAME").toString();
            field.COLUMN_NAME = query.value("COLUMN_NAME").toString();
            field.DATA_TYPE = query.value("DATA_TYPE").toString();
            field.IS_NULLABLE = query.value("IS_NULLABLE").toString();
            field.COLUMN_DEFAULT = query.value("COLUMN_DEFAULT").toString();
            field.ORDINAL_POSITION = query.value("ORDINAL_POSITION").toInt();

            // тут вставить is_identity

            /*

            field.CHARACTER_MAXIMUM_LENGTH = query.value("CHARACTER_MAXIMUM_LENGTH").toInt();
            field.CHARACTER_OCTET_LENGTH = query.value("CHARACTER_OCTET_LENGTH").toInt();
            field.NUMERIC_PRECISION = query.value("NUMERIC_PRECISION").toInt();
            field.NUMERIC_PRECISION_RADIX = query.value("NUMERIC_PRECISION_RADIX").toInt();
            field.NUMERIC_SCALE = query.value("NUMERIC_SCALE").toInt();
            field.DATETIME_PRECISION = query.value("DATETIME_PRECISION").toInt();
            field.CHARACTER_SET_CATALOG = query.value("CHARACTER_SET_CATALOG").toString();
//            field.CHARACTER_SET_SCHEMA = query.value("CHARACTER_SET_SCHEMA").toString();
            field.CHARACTER_SET_NAME = query.value("CHARACTER_SET_NAME").toString();
//            field.COLLATION_CATALOG = query.value("COLLATION_CATALOG").toString();
//            field.COLLATION_SCHEMA = query.value("COLLATION_SCHEMA").toString();
            field.COLLATION_NAME = query.value("COLLATION_NAME").toString();
//            field.DOMAIN_CATALOG = query.value("DOMAIN_CATALOG").toString();
//            field.DOMAIN_SCHEMA = query.value("DOMAIN_SCHEMA").toString();
//            field.DOMAIN_NAME = query.value("DOMAIN_NAME").toString();
            */

            fields.push_back(field);
        }

        map_fields[key] = fields;
//        save_fields(db, tn, fields);

        return true;
    }

    return false;


#if 0

    std::tuple<QString, QString, QString> key(db.hostName(), db.databaseName(), tn.toLower());

    auto it = map_fields.find(key);
    if (it != map_fields.end()) {
        fields = it->second;
        return true;
    }


    if (read_fields(db, tn, fields)) {
        return true;
    }

//    static bool is_init = false;
//    if (!is_init) {
        init_table_columns(db);
//        is_init = true;
//    }

    if (read_fields(db, tn, fields)) {
        return true;
    }


    QSqlQuery query(db);


    QString q = QString(

" SELECT\n"
" TABLE_CATALOG,\n"
" TABLE_SCHEMA,\n"
" TABLE_NAME,\n"
" COLUMN_NAME,\n"
" ORDINAL_POSITION,\n"
" COLUMN_DEFAULT,\n"
" IS_NULLABLE,\n"
" DATA_TYPE,\n"
" CHARACTER_MAXIMUM_LENGTH,\n"
" CHARACTER_OCTET_LENGTH,\n"
" NUMERIC_PRECISION,\n"
" NUMERIC_PRECISION_RADIX,\n"
" NUMERIC_SCALE,\n"
" DATETIME_PRECISION,\n"
" CHARACTER_SET_CATALOG,\n"
" CHARACTER_SET_SCHEMA,\n"
" CHARACTER_SET_NAME,\n"
" COLLATION_CATALOG,\n"
" COLLATION_SCHEMA,\n"
" COLLATION_NAME,\n"
" DOMAIN_CATALOG,\n"
" DOMAIN_SCHEMA,\n"
" DOMAIN_NAME\n"
" FROM INFORMATION_SCHEMA.COLUMNS col\n"
" WHERE TABLE_NAME='%1'\n"
//" AND TABLE_SCHEMA='dbo'\n"
" ORDER BY ordinal_position\n").arg(tn);

    if (query_exec(db, query, q)) {
        while (query.next()) {
            AdoField field;

            field.TABLE_CATALOG = query.value("TABLE_CATALOG").toString();
            field.TABLE_SCHEMA = query.value("TABLE_SCHEMA").toString();
            field.TABLE_NAME = query.value("TABLE_NAME").toString();
            field.COLUMN_NAME = query.value("COLUMN_NAME").toString();
            field.ORDINAL_POSITION = query.value("ORDINAL_POSITION").toInt();
            field.COLUMN_DEFAULT = query.value("COLUMN_DEFAULT").toString();
            field.IS_NULLABLE = query.value("IS_NULLABLE").toString();
            field.DATA_TYPE = query.value("DATA_TYPE").toString();
            field.CHARACTER_MAXIMUM_LENGTH = query.value("CHARACTER_MAXIMUM_LENGTH").toInt();
            field.CHARACTER_OCTET_LENGTH = query.value("CHARACTER_OCTET_LENGTH").toInt();
            field.NUMERIC_PRECISION = query.value("NUMERIC_PRECISION").toInt();
            field.NUMERIC_PRECISION_RADIX = query.value("NUMERIC_PRECISION_RADIX").toInt();
            field.NUMERIC_SCALE = query.value("NUMERIC_SCALE").toInt();
            field.DATETIME_PRECISION = query.value("DATETIME_PRECISION").toInt();
            field.CHARACTER_SET_CATALOG = query.value("CHARACTER_SET_CATALOG").toString();
//            field.CHARACTER_SET_SCHEMA = query.value("CHARACTER_SET_SCHEMA").toString();
            field.CHARACTER_SET_NAME = query.value("CHARACTER_SET_NAME").toString();
//            field.COLLATION_CATALOG = query.value("COLLATION_CATALOG").toString();
//            field.COLLATION_SCHEMA = query.value("COLLATION_SCHEMA").toString();
            field.COLLATION_NAME = query.value("COLLATION_NAME").toString();
//            field.DOMAIN_CATALOG = query.value("DOMAIN_CATALOG").toString();
//            field.DOMAIN_SCHEMA = query.value("DOMAIN_SCHEMA").toString();
//            field.DOMAIN_NAME = query.value("DOMAIN_NAME").toString();

            fields.push_back(field);
        }

        map_fields[key] = fields;
        save_fields(db, tn, fields);

        return true;
    }
    return false;
#endif
}

// Выдает строку из полей для запроса, кроме тех, которые в filtr

QString get_table_columns_txt(QSqlDatabase & db, const QString & tn, const QString & filtr0)
{
    QString filtr = filtr0;
    QString s = "";
    std::list<AdoField> fields;

    if (get_table_columns(db, tn, fields)) {
        filtr = QString(",%1,").arg(filtr);

        for (auto &i : fields) {
            if (filtr.indexOf(QString(",%1,").arg(i.COLUMN_NAME)) == -1) {
                if (s != "") s += ",";
                s += "[";
                s += i.COLUMN_NAME;
                s += "]";
            }
        }
    }
    return s;
}

bool column_in_table(QSqlDatabase &db, const QString & table, const QString & column)
{
    QString q = QString(
       "SELECT COLUMN_NAME FROM INFORMATION_SCHEMA.COLUMNS col"
       " WHERE TABLE_NAME='%1' AND TABLE_SCHEMA='dbo' AND COLUMN_NAME='%2'").arg(table, column);

    if (is_SQLITE()) {
        q = QString("SELECT cid FROM  pragma_table_info('%1') where name='%2'").arg(table, column);
    }
    if (is_POSTGRESQL()) {
        q = QString(
"SELECT attrelid::regclass AS tbl\n"
"     , attname            AS col\n"
"     , atttypid::regtype  AS datatype\n"
"       -- more attributes?\n"
"FROM   pg_attribute\n"
"WHERE  attrelid = '%1'::regclass\n"
"and    lower(attname)=lower('%2')\n"
"AND    attnum > 0\n"
"AND    NOT attisdropped\n"
"ORDER  BY attnum;\n").arg(table, column);
    }

    bool yes = false;

    QSqlQuery query(db);

    if (query_exec(db, query, q, false)) {
        while (query.next()) {
            yes = true;
            break;
        }
    }
    return yes;
}

QString column_table_out(QSqlDatabase &db, const QString & table, const QString & prefix)
{
    std::list<AdoField> fields;
    QString s = "";
    
    if (get_table_columns(db, table, fields)) {
        for (auto &field : fields) {
            QString cn = field.COLUMN_NAME;
            cn = cn.toLower();

            if (cn == "id") continue;
            if (cn == "shape") continue;
            if (cn == "nodeid") continue;
            if (cn == "lineid") continue;
            if (cn == "calculationid") continue;

            if (table.toLower() == "pt_out") {
                // Технология и кондиционирование
                if (cn == "dop13") continue;
                if (cn == "dop14") continue;
                if (cn == "dop15") continue;
                if (cn == "dop16") continue;
                if (cn == "a10") continue;
                if (cn == "a7") continue;
                if (cn == "a8") continue;
                if (cn == "a9") continue;
            }


            if (s != "") s += ",";

            if (cn == "externalsignlineid") {
                cn = QString(
               "case %1.externalSignLineID\n"
               "  when 1 then ' '\n"
               "  when 2 then 'Подача'\n"
               "  when 3 then 'Обратка'\n"
               "  when 4 then 'Подача - Обратка'\n"
               "  when 5 then 'Обратка - Подача'\n"
               "end as %2\n").arg(prefix, br_text("Признак"));
            }
            else if (cn == "externalsign") {
                cn = QString(
               "case %1.externalSign\n"
               "  when 1 then 'Подача'\n"
               "  when 2 then 'Обратка'\n"
               "end as %2\n").arg(prefix, br_text("Признак"));
            }
            else if (cn == "sos") {
//                cn = " as sos\n";
                cn = QString(
               "case %1.sos\n"
               "  when '1' then 'Открыт'\n"
               "  when '2' then 'Закрыт'\n"
               "  when '3' then 'Неактивен'\n"
               "end as sos\n").arg(prefix);
            }
            else if (cn == "ist") {
                cn = "hs.name as ist\n";
            }
            else if (field.DATA_TYPE == "double precision") {
//                cn = QString(%1 as %1).arg(cn);
                if (is_POSTGRESQL()) {
                    cn = QString(
                        "CASE \n"
                        "    WHEN %1 = 0 THEN 0\n"
                        "    WHEN %1 IS NULL THEN NULL\n"
                        "    ELSE round(%1::numeric, 3 - floor(log(abs(%1)))::int - 1)\n"
                        "END AS %1\n").arg(cn);
                }
                else {
                    cn = QString(
                        "CASE \n"
                        "    WHEN %1 = 0 THEN 0\n"
                        "    WHEN %1 IS NULL THEN NULL\n"
                        "    ELSE round(%1, 3 - floor(log10(abs(%1)))::int - 1)\n"
                        "END AS %1\n").arg(cn);
                }
            }
            else {
                s += prefix;
                s += ".";
            }
            s += cn;
        }
    }
    return s;
}
