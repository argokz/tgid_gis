#pragma once

#include "qsqldatabase.h"
#include <list>
#include <map>
#include <set>


enum {
    MSSQL,
    POSTGRESQL,
    SQLITE
};


class CAdoInsert
{
public:
    CAdoInsert(QSqlDatabase *db, const QString & tn, const QString & head) {
        m_db = db;
        m_head = head;
        m_tn = tn;
        n_insert = 0;
        m_values = "";
    };
    ~CAdoInsert() {
        ado_insert();
    }
    bool ado_insert(const QString & values = "");
    bool ado_insert_int1(int id);
    bool ado_insert_int2(int id1, int id2);
private:
    int n_insert;
    QString m_values;
    QString m_head;
    QString m_tn;
    QSqlDatabase *m_db;
};


struct AdoField {
 QString TABLE_CATALOG;
 QString TABLE_SCHEMA;
 QString TABLE_NAME;
 QString COLUMN_NAME;
 int ORDINAL_POSITION;
 QString COLUMN_DEFAULT;
 QString IS_NULLABLE;
 QString DATA_TYPE;
 int CHARACTER_MAXIMUM_LENGTH;
 int CHARACTER_OCTET_LENGTH;
 int NUMERIC_PRECISION;
 int NUMERIC_PRECISION_RADIX;
 int NUMERIC_SCALE;
 int DATETIME_PRECISION;
 QString CHARACTER_SET_CATALOG;
// QString CHARACTER_SET_SCHEMA;
 QString CHARACTER_SET_NAME;
// QString COLLATION_CATALOG;
// QString COLLATION_SCHEMA;
 QString COLLATION_NAME;
// QString DOMAIN_CATALOG;
// QString DOMAIN_SCHEMA;
// QString DOMAIN_NAME;

};

bool get_table_columns(QSqlDatabase & db, const QString & tn, std::list<AdoField> & fields);
bool get_table_columns(QSqlDatabase & db, const QString & tn, std::map<QString, AdoField> & fields);


bool column_in_table(QSqlDatabase &db, const QString & table, const QString & column);
//int ExecuteInsert(QSqlDatabase & db, const QString & q);
QString readTableValue(QSqlDatabase &db, const QString & q);
QString readTableValue2(QSqlDatabase &db, const QString & q);

bool fetchFirstRow(QSqlQuery &query, QMap<QString, QVariant> &row);
bool fetchFirstRow(QSqlDatabase &db, const QString & q, QMap<QString, QVariant> &row);


QVariant readTableVariant(QSqlDatabase &db, const QString & q);



bool readTableValues(QSqlDatabase &db, const QString & q, int n, std::vector<QVariant> & vv);
bool readTableRowValues(QSqlDatabase &db, const QString & q, std::vector<QVariant> & vv);

bool readTableRowValuesMap(QSqlDatabase &db, const QString & q, std::map<QString, QVariant> & vv);




int readTableInt(QSqlDatabase &db, const QString & q);

double read_double_db(QSqlDatabase &db, const QString & q, const QString & fn, bool *ok);

bool connectSQL0(int rdmbs, const QString & host, int port, const QString & baza, const QString & user, const QString & password, QSqlDatabase & db);
bool connectSQL1(const QString & baza,  QSqlDatabase &m_db);
bool connectSqlite0(const QString & baza,  QSqlDatabase &m_db);


long sizeOfQ(QSqlDatabase &db, const QString & q);

void ErrorMessage(QSqlQuery &query);
void ErrorMessage(QSqlDatabase &db);

QString getDatabaseName(QSqlDatabase &db);
void readLookup(QSqlDatabase &db, const QString & table, const QString & sid, const QString & sval, std::map<int, QString> &map_lookup);

bool delObject(QSqlDatabase &db, const QString & tn, int id);

bool execQ(QSqlDatabase & db, const QString & q, bool message = false);

//int execInsertQ(QSqlDatabase & db, QSqlQuery &query, const QString & q, bool message = false);
//int execInsertQ(QSqlDatabase & db, const QString & q, bool message = false);

//int execInsertQ(QSqlDatabase & db, const QString & tn, std::map<QString, QVariant> & map_v, bool message = false, bool last = true);
bool execUpdateQ(QSqlDatabase & db, const QString & tn, int id, std::map<QString, QVariant> & map_v, bool message = false);

QString getTableNameFromSql(const QString & q);
QString column_table_out(QSqlDatabase &db, const QString & table, const QString & prefix);

class CGraph2;

bool create_vyd_line_table(QSqlDatabase &db, CGraph2* graph, QString& tn);
bool create_vyd_node_table(QSqlDatabase &db, CGraph2* graph, QString& tn);

QString get_table_columns_txt(QSqlDatabase & db, const QString & tn, const QString & filtr = "");

void copy_q(QSqlDatabase &db, const QString & q);

QString quot_text(const QString & txt);
QString br_text(const QString & txt);
QString SelectTop(const QString & q);
QString top100000();
int get_RDMS();
bool is_POSTGRESQL();
bool is_MSSQL();
bool is_SQLITE();
QString full_name(const QString & schema, const QString & table);


QString ispr_q(const QString & q);

bool query_exec(QSqlDatabase &db, QSqlQuery &query, const QString & q = "", bool alarm = true);
bool query_exec(QSqlDatabase &db, const QString & q, bool alarm = true);

bool check_db(QSqlDatabase &db);

int insertIntoDatabase(QSqlDatabase &db, const QString &tableName, const std::map<QString, QVariant> &data, const QString & geom_col = "");
bool updateDatabaseRow(QSqlDatabase &db, const QString &tableName, int id, const std::map<QString, QVariant> &data, const QString & geom_col = "");
void setRDBMS(int rdbms);

bool updateRow(QSqlDatabase &m_db, const QString &table, int id, const QMap<QString, QVariant> &data);

bool checkTableExists(QSqlDatabase &db, const QString &schema, const QString &table);


QString split_query(QSqlDatabase &db, const QString & q0);



QString get_now();

