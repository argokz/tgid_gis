#pragma once


struct CColName
{
    QString name;
    QString name_full;
};

struct ForeignTable {
    QString field;
    QString id;
    QString name;
    QString cxema;
    QString table;
    QString q;
    bool isSort;
};



QString findTableRusName(const QString & d, const QString & n);
QString findColumnRusName(const QString & d, const QString & n1, const QString & n2);
const CColName *findColumnRusNameFull(const QString & d, const QString & n1, const QString & n2);
const ForeignTable *findLookup(const QString & d, const QString & n1, const QString & n2);
const std::map<QString, ForeignTable> *findLookup(const QString & d, const QString & table);
std::map<int, QString> *findLookup2(const QString & d, const QString & fn);
