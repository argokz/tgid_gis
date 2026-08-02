#include <QApplication>
#include <QtSql>
#include <iostream>
#include "db.h"

bool CAdoInsert::ado_insert(const QString & values)
{
    QSqlQuery query(*m_db);
    bool ret = true;

    if (values != "") {
//        if (head[0]) m_head = head;
        n_insert++;
        if (m_values != "") m_values += ",";
        m_values += "(";
        m_values += values;
        m_values += ")\n";
    }

    if (n_insert == 999 || values == "") {
        if (n_insert) {
//            long affected;
            QString sql = QString("INSERT INTO %1 (%2) VALUES %3").arg(m_tn, m_head, m_values);

            if (!query_exec(*m_db, query, sql)) {
                ret = false;
            }
        }
        n_insert = 0;
        m_values = "";
//        m_head = "";
    }
    return ret;
}

bool CAdoInsert::ado_insert_int1(int id)
{
    QString values = QString("%1").arg(id);
    return ado_insert(values);
}

bool CAdoInsert::ado_insert_int2(int id1, int id2)
{
    QString values = QString("%1,%2").arg(id1).arg(id2);
    return ado_insert(values);
}

bool execQ(QSqlDatabase & db, const QString & q, bool message)
{
    QSqlQuery query(db);

    bool res = query_exec(db, query, q);

    if (!res && message) {
        ErrorMessage(query);
    }

    return res;
}

#if 0
int execInsertQ(QSqlDatabase & db, QSqlQuery &query, const QString & q, bool message)
{
    if(db.driver()->hasFeature(QSqlDriver::LastInsertId)) {
        bool res = query_exec(db, query, q);

        if (!res) {
            ErrorMessage(query);
            return 0;
        }

//        Q_ASSERT(res);

        QVariant id = query.lastInsertId();
        Q_ASSERT(id.isValid() && !id.isNull());
        return id.toInt();
    }
    return 0;
}


int execInsertQ(QSqlDatabase & db, const QString & q, bool message)
{
    QSqlQuery query(db);

    if(db.driver()->hasFeature(QSqlDriver::LastInsertId)) {
        bool res = query_exec(db, query, q);

        if (!res) {
            ErrorMessage(query);
            return 0;
        }

//        Q_ASSERT(res);

        QVariant id = query.lastInsertId();
        Q_ASSERT(id.isValid() && !id.isNull());
        return id.toInt();
    }
    return 0;
}

#endif

bool isGeometry(const QString & s)
{
    return s.indexOf("geometry:") == 0 || s.indexOf("ST_GeomFromText") == 0;
}

int execInsertQ(QSqlDatabase & db, const QString & tn, std::map<QString, QVariant> & map_v, bool message, bool last)
{
    QString q;
    QString q1 = "";
    QString q2 = "";


    for (auto &it : map_v) {
        if (q1 != "") q1 += ",";
        q1 += it.first;

        if (q2 != "") q2 += ",";
        if (isGeometry(it.second.toString())) {
            q2 += it.second.toString();
        }
        else if (it.second.toString() == "$$$_NOW_$$$") {
            if (is_POSTGRESQL()) {
                q2 += "NOW()";
            }
            else if (is_MSSQL()) {  // MS SQL
                q2 += "GETDATE()";
            }
        }
        else {
            q2 += "?";
//            q2 += ":" + it.first;
        }
    }

    q = QString("INSERT INTO %1 (%2) VALUES (%3)").arg(tn, q1, q2);


    qDebug() << q;

    QSqlQuery query(db);
    query.prepare(q);

    for (auto &it : map_v) {
        if (!isGeometry(it.second.toString()) && (it.second.toString() != "$$$_NOW_$$$")) {
//            query.b1indValue(QString(":%1").arg(it.first), it.second);
            query.addBindValue(it.second);
            qDebug() << it.first << " : " << it.second;
        }
    }

    if(db.driver()->hasFeature(QSqlDriver::LastInsertId)) {
        bool res = query_exec(db, query);

        if (!res) {
            if (message) {
                ErrorMessage(query);
            }
            return 0;
        }

        Q_ASSERT(res);

        if (last) {
            QVariant id = query.lastInsertId();
            Q_ASSERT(id.isValid() && !id.isNull());
            return id.toInt();
        }
    }
    return 0;
}

bool execUpdateQ(QSqlDatabase & db, const QString & tn, int id, std::map<QString, QVariant> & map_v, bool message)
{
    QString q;
    QString q1 = "";

    if (map_v.size() == 0) return true;

    for (auto &it : map_v) {
        if (q1 != "") q1 += ",";
//        q1 += QString("%1=:%2").arg(it.first, it.first);
        q1 += QString("%1=?").arg(it.first);
    }

    q = QString("UPDATE %1 SET %2 WHERE id=%3").arg(tn, q1).arg(id);

    QSqlQuery query(db);
    query.prepare(q);

    for (auto &it : map_v) {
//        query.b1indValue(QString(":%1").arg(it.first), it.second);
        query.addBindValue(it.second);
    }

    bool res = query_exec(db, query);

    if (!res && message) {
        ErrorMessage(query);
        return false;
    }
    return true;
}
