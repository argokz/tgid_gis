#include <QApplication>
#include <QFile>
#include <QRegularExpression>
#include <map>
#include <mystd.h>
#include <set>
#include "init_files.h"

static std::map<QString, std::map<QString, std::map<QString, CColName> > > map_map_col_txt;
static std::map<QString, std::map<QString, std::map<QString, ForeignTable> > > map_lookup;

static std::map<QString, std::map<QString, std::map<int, QString> > > map_lookup2;

bool is_POSTGRESQL();
bool is_MSSQL();


QString del_vbar(const QString & txt0)
{
    QString txt = txt0;

    int n = txt.indexOf("||");
    if (n != -1) {
        txt = txt.left(n);
    }
    return txt;    
}


const CColName *findColumnRusNameFull1(const QString & d0, const QString & table0, const QString & column0)
{
    QString d = d0.toLower();
    QString table = table0.toLower();
    QString column = column0.toLower();
    column = del_vbar(column);

    auto it1 = map_map_col_txt.find(d);
    
    if (it1 != map_map_col_txt.end()) {
        auto  it2 = it1->second.find(table);
        if (it2 != it1->second.end()) {

          auto it3 = it2->second.find(column);
          if (it3 != it2->second.end()) {
              return &it3->second;
          }
        }
    }

    int nn = table0.indexOf('.');

    if (nn != -1) {
        return findColumnRusNameFull1(d0, table0.mid(nn+1), column0);
    }


    return nullptr;
}


const CColName *findColumnRusNameFull(const QString & d, const QString & table, const QString & column)
{
    
    const CColName *cn = findColumnRusNameFull1(d, table, column);
    if (!cn) {
        cn = findColumnRusNameFull1(d, "?", column);
    }

    if (!cn && d != "gid") {
        cn = findColumnRusNameFull("gid", table, column);
    }

    return cn;
}



QString findColumnRusName(const QString & d, const QString & table, const QString & column)
{
    const CColName *cn = findColumnRusNameFull(d, table, column);

    if (cn) {
        return cn->name;
    }

    if (!cn && d != "gid") {
        return findColumnRusName("gid", table, column);
    }

    return column;
}



QString findColumnName1(const QString & d0, const QString & table0, const QString & rus_name)
{
    QString d = d0.toLower();
    QString table = table0.toLower();

    auto it1 = map_map_col_txt.find(d);
    
    if (it1 != map_map_col_txt.end()) {
        auto  it2 = it1->second.find(table);

        if (it2 == it1->second.end()) {
            int nn = table0.indexOf('.');

            if (nn != -1) {
                it2 = it1->second.find(table0.mid(nn+1));
            }
        }


        if (it2 != it1->second.end()) {
          for (auto & it3 : it2->second) {
            if (rus_name == it3.second.name)
              return it3.first;
          }
        }
    }
    return rus_name;
}

QString findColumnName(const QString & d, const QString & table, const QString & rus_name)
{
    QString cn = findColumnName1(d, table, rus_name);

    if (cn != rus_name) {
        return cn;
    }

    if (d != "gid") {
        return findColumnName1("gid", table, rus_name);
    }

    return rus_name;
}



void initColumnRusNameFile(const QString & database, const QString & klfn)
{

//    QString fn = QString("%1kls/sprav.txt").arg(argpath());
    QFile file(klfn);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        QTextStream in(&file);
//        in.setEncoding(QStringConverter::System);

        QString text_f1 = "";
        bool is_f1 = false;
        QString d = database.toLower();
        QString table = "";
        QString column = "";

        while (!file.atEnd()) {
//            QString s = in.readLine();
            QString s = readCP1251Line(file);

            if (!is_f1) {
                if (s.left(1) == "-") continue;

                if (s.left(3) == "<<<") {
                    text_f1 = "";
                    is_f1 = true;
                }
            }
            else {
                if (s.left(3) == ">>>") {
                    is_f1 = false;
                    map_map_col_txt[d][table][column].name_full = text_f1;
                }
                else {
                    if (s == "") {
                    }

                    if (text_f1 != "") text_f1 += " ";
                    text_f1 += s;
                }
                continue;
            }

            static QRegularExpression re("\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*(,\\s*\"(.+?)\")?");
            QRegularExpressionMatch match = re.match(s);
            if (match.hasMatch()) {
                QString name_e       = match.captured(1);
                QString name_col_e   = match.captured(2);
                QString name_col_r   = match.captured(3);
                QString name_full    = match.captured(5);

//                QString d = database.toLower();
                table = name_e.toLower();
                column = name_col_e.toLower();

                CColName col_name;
                col_name.name = name_col_r;
                col_name.name_full = name_full;
                col_name.name_full.replace("\\n", "\n");

                map_map_col_txt[d][table][column] = col_name;
            }
        }
    }
}

void initColumnRusName(const QString & database)
{
    QString klfn;

    klfn = QString("%1kls/%2.txt1").arg(argpath(), database);
    initColumnRusNameFile(database, klfn);

    klfn = QString("%1kls/%2.txt2").arg(argpath(), database);
    initColumnRusNameFile(database, klfn);

    klfn = QString("%1kls/%2.txt3").arg(argpath(), database);
    initColumnRusNameFile(database, klfn);

    klfn = QString("%1kls/%2.txt4").arg(argpath(), database);
    initColumnRusNameFile(database, klfn);


    klfn = QString("%1%2.txt2").arg(argpath_2(), database);
    initColumnRusNameFile(database, klfn);
}

QString ispr_include(QString & key);


void initLookup(const QString & database)
{
    QString klfn = QString("%1kls/%2.lookup").arg(argpath(), database);

    QFile file(klfn);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        QTextStream in(&file);
//        in.setEncoding(QStringConverter::System);

        while (!file.atEnd()) {
//            QString s = in.readLine();
            QString s = readCP1251Line(file);

            if (s.left(1) == "-") continue;

//            static QRegularExpression re("\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*\"([0-9]+?)\"(\\s*,\\s*\"(.+?)\")?");

//            static QRegularExpression re("\"(.+?)\",\"(.+?)\",\"(.+?)\",\"(.+?)\",\"(.+?)\",([0-9]+)(,\"(.+?)\"");
            static QRegularExpression re("\"(.+?)\",\"(.+?)\",\"(.+?)\",\"(.+?)\",\"(.+?)\",([0-9]+)(,\"(.+)\")?");

            QRegularExpressionMatch match = re.match(s);
            if (match.hasMatch()) {
                QString tn      = match.captured(1).toLower();
                QString fn      = match.captured(2).toLower();
                QString tn_l    = match.captured(3).toLower();
                QString s_id    = match.captured(4).toLower();
                QString s_name  = match.captured(5);
                int isSort  = match.captured(6).toInt();
                QString q       = match.captured(8);

                if (!is_MSSQL()) {
                    q.replace(" TOP 2000000 ", " ");
                }

                q.replace("\\n", "\n");

                q = ispr_include(q);

                QString d = database.toLower();

                ForeignTable ft;

                ft.field = fn;
                ft.id = s_id;
                ft.name = s_name;
                ft.table = tn_l;
                ft.cxema = d;
                ft.isSort = (isSort != 0);

                if (q != "") {
                  ft.q = q;
//                  ft.name = "name";
                }
                else {
                   ft.q = "";
                }
                map_lookup[d][tn][fn] = ft;
            }
        }
    }
}


void initLookup2(const QString & database)
{
    QString klfn = QString("%1kls/%2.lookup2").arg(argpath(), database);

    QFile file(klfn);

    QString fn = "";
    QString d = database.toLower();

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {

        while (!file.atEnd()) {
            QString s = readCP1251Line(file);

            if (s.left(1) == "-") continue;

            if (s.left(1) == " ") {
                static QRegularExpression re("^\\s+([0-9]+?)\\s+([^\\s].*?)\\s*$");

                QRegularExpressionMatch match = re.match(s);
                if (match.hasMatch()) {
                    int id        = match.captured(1).toInt();
                    QString txt   = match.captured(2);
                    map_lookup2[d][fn.toLower()][id] = txt;
                }
            }
            else {
                fn = s;
            }
        }
    }
}


const ForeignTable *findLookup(const QString & d0, const QString & table0, const QString & column0)
{
    ForeignTable ft;
    ft.id = "";
    ft.table = "";

    QString d = d0.toLower();
    QString table = table0.toLower();
    QString column = column0.toLower();
    column = del_vbar(column);

    std::map<QString, std::map<QString, std::map<QString, ForeignTable> > >::const_iterator it1 = map_lookup.find(d);
    
    if (it1 != map_lookup.end()) {
        auto it2 = it1->second.find(table);
        if (it2 != it1->second.end()) {

          auto  it3 = it2->second.find(column);
          if (it3 != it2->second.end()) {
              return &it3->second;
          }
        }
    }

    if (d != "gid" && d != "gid8") {
        return findLookup("gid8", table, column);
    }
    if (d == "gid8") {
        return findLookup("gid", table, column);
    }

    return nullptr;
}

std::map<int, QString> *findLookup2_(const QString & d0, const QString & fn0)
{
    QString d = d0.toLower();
    QString fn = fn0.toLower();

    auto it1 = map_lookup2.find(d);
    if (it1 == map_lookup2.end()) return nullptr;
    auto it2 = it1->second.find(fn);
    if (it2 == it1->second.end()) return nullptr;

    return &it2->second;
}

std::map<int, QString> *findLookup2(const QString & d, const QString & fn)
{
    std::map<int, QString> *p = findLookup2_(d, fn);

    if (!p && d != "gid") {
        p = findLookup2_("gid", fn);
    }
    return p;
}

const std::map<QString, ForeignTable> *findLookup(const QString & d0, const QString & table0)
{
    QString d = d0.toLower();
    QString table = table0.toLower();

    auto it1 = map_lookup.find(d);

    if (it1 == map_lookup.end()) {
//        QString dd = getGidAdoName();
//        dd.MakeLower();
      
//        if (d == dd) {
//            it1 = map_lookup.find("gid");
//        }
    }
    
    if (it1 != map_lookup.end()) {
        auto it2 = it1->second.find(table);
        if (it2 != it1->second.end()) {
            return &it2->second;
        }
    }

    if (d != "gid") {
        return findLookup("gid", table);
    }

    return nullptr;
}


//-------------------------------------------------

static std::map<QString, std::map<QString, QString> > map_map_txt;

QString findTableRusName(const QString & d0, const QString & n0)
{
    QString nn = n0;

    QString d = d0.toLower();
    QString n = n0.toLower();

    auto it1 = map_map_txt.find(d);
    if (it1 != map_map_txt.end()) {
        auto it2 = it1->second.find(n);
        if (it2 != it1->second.end()) {
            return it2->second;
        }
    }
    int n0n = n0.indexOf('.');

    if (n0n != -1) {
        return findTableRusName(d0, n0.mid(n0n+1));
        //        return findColumnRusNameFull1(d0, table0.mid(nn+1), column0);
    }


    if (d != "gid") {
        return findTableRusName("gid", n);
    }


    return nn;
}


QString findTableEngName(const QString & d0, const QString & n0)
{
    QString nn = n0;

    QString d = d0.toLower();
    QString n = n0.toLower();

    auto it1 = map_map_txt.find(d);
    if (it1 != map_map_txt.end()) {
        for (auto it2 = it1->second.begin(); it2 != it1->second.end(); it2++) {
            QString column = it2->second.toLower();
            if (column == n) {
                return it2->first;
            }
        }
    }

    if (d != "gid") {
        return findTableEngName("gid", n);
    }

    return nn;
}


/*
QString findTableRusNameGeo(const QString & n)
{
    Klassif *kls = m_geofile->m_kl_list->findKlN(n);
    if (kls) {
        return findTableRusName(kls->m_ado->m_schema, n);
    }
    return n;
}
*/


void initTableRusNameFile(const QString & database, const QString & klfn)
{
    QString d = database.toLower();
    QFile file(klfn);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        QTextStream in(&file);
//        in.setEncoding(QStringConverter::System);

        while (!file.atEnd()) {
//            QString s = in.readLine();
            QString s = readCP1251Line(file);

            if (s.left(1) == "-") continue;

            static QRegularExpression re("[0-9]+,\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*$");
            QRegularExpressionMatch match = re.match(s);
            if (match.hasMatch()) {
                QString name_e  = match.captured(1).toLower();
                QString name_r  = match.captured(2);
                map_map_txt[d][name_e] = name_r;
            }
        }
    }
}

void initTableRusName(const QString & database)
{
    QString klfn;

    klfn = QString("%1kls/%2.txt").arg(argpath(), database);
    initTableRusNameFile(database,  klfn);

    klfn = QString("%1%2.txt").arg(argpath_2(), database);
    initTableRusNameFile(database, klfn);
}


void addTableRusName(const QString & database, const QString & name_e, const QString & name_r)
{
    QString d = database.toLower();
    QString n = name_e.toLower();

    map_map_txt[d][n] = name_r;
}


QString findTableRusNameGid(const QString & n)
{
    return findTableRusName("gid", n);
}


static std::map<QString, QString> map_renamed_table;
static std::map<QString, std::map<QString, QString>> map_renamed_columns;

void initRenamedTables()
{
    QString klfn = QString("%1kls/renamed_tables.txt").arg(argpath());

    QFile file(klfn);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        QTextStream in(&file);
//        in.setEncoding(QStringConverter::System);

        while (!file.atEnd()) {
//            QString s = in.readLine();
            QString s = readCP1251Line(file);
//             static QRegularExpression re("\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*(,\\s*\"(.+?)\")?");
            static QRegularExpression re("\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*$");

            QRegularExpressionMatch match = re.match(s);

            if (match.hasMatch()) {
                QString old_name = match.captured(1).toLower();
                QString new_name = match.captured(2).toLower();

                map_renamed_table[new_name] = old_name;
            }
        }
    }
}


void initRenamedColumns()
{
    QString klfn = QString("%1kls/renamed_columns.txt").arg(argpath());

    QFile file(klfn);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        QTextStream in(&file);
//        in.setEncoding(QStringConverter::System);

        while (!file.atEnd()) {
//            QString s = in.readLine();
            QString s = readCP1251Line(file);
             static QRegularExpression re("\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*,\\s*\"(.+?)\"\\s*");

            QRegularExpressionMatch match = re.match(s);

            if (match.hasMatch()) {
                QString tn = match.captured(1).toLower();
                QString old_name = match.captured(2).toLower();
                QString new_name = match.captured(3).toLower();

                map_renamed_columns[tn][old_name] = new_name;
            }
        }
    }
}

QString findNewColumnName(const QString & tn, const QString & old_name)
{
    QString old_name_lower = old_name.toLower();

    auto it1 = map_renamed_columns.find(tn.toLower());
    
    if (it1 != map_renamed_columns.end()) {
        auto  it2 = it1->second.find(old_name_lower);
        if (it2 != it1->second.end()) {
            return it2->second;
        }
    }

    return old_name;
}

struct LinePoint {
    std::set<QString> set_line;
    std::set<QString> set_node;
    std::set<QString> set_node1;
    std::set<QString> set_node2;
};


static std::map<QString, LinePoint> map_line_point;

void initLinePoint()
{
    QString klfn = QString("%1kls/line_point.txt").arg(argpath());

    QFile file(klfn);

    int levels[256];
    levels[0] = 0;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        QTextStream in(&file);
//        in.setEncoding(QStringConverter::System);

        int level = 0;
        int ns0 = 0;
        QString cod = "";
        QString typ = "";

        while (!file.atEnd()) {
//            QString s = in.readLine();
            QString s = readCP1251Line(file);

            int ns = 0;
            for (; ns < s.length() && s.at(ns) == ' '; ns++) {
            }
            QString txt = s.mid(ns);

            if (ns > ns0) {
                level += 1;
                levels[level] = ns;
            }
            else if (ns < ns0) {
                for (level = 0; level < 256; level++) {
                    if (levels[level] == ns) {
                        break;
                    }
                }
                if (level == 256) {
                    break;
                }
            }

            if (level == 0) {
                cod = txt;
            }
            else if (level == 1) {
                typ = txt;
            }
            else if (level == 2) {
                LinePoint lp;

                if (typ == "line") {
                    lp.set_line.insert(txt);
                }
                else if (typ == "node") {
                    lp.set_node.insert(txt);
                }
                else if (typ == "node1") {
                    lp.set_node1.insert(txt);
                }
                else if (typ == "node2") {
                    lp.set_node2.insert(txt);
                }
            }


            ns0 = ns;

            if (s.left(1) == "-") continue;
        }
    }
}

static std::map<QString, QString> map_include;


void initInclude()
{
    QString klfn = QString("%1kls/GID.include").arg(argpath());

    QFile file(klfn);

    int levels[256];
    levels[0] = 0;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {

        int level = 0;
        int ns0 = 0;
        QString key = "";
        QString txt = "";

        while (!file.atEnd()) {
//            QString s = in.readLine();
            QString s = readCP1251Line(file);
            if (s.left(1) == "-") continue;
            if (s.left(1) == "#") {
                key = s.mid(1);
                txt = "";
                continue;
            }

            if (txt != "") txt += "\n";
            txt += s;
            map_include[key] = txt;
        }
    }
}


QString getInclude(QString & key) 
{
    auto it = map_include.find(key);
    if (it != map_include.end()) {
        return it->second;
    }
    return "";
}



QString ispr_include1(const QString & q0)
{
    QString q = q0;

    static QRegularExpression re("###(\\w+)###");
    QRegularExpressionMatchIterator i = re.globalMatch(q);

    // Замена найденных ключей на другие значения
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString key = match.captured(1);  // Получаем ключ (key1, key2, и т.д.)
        QString replacement = getInclude(key);

        if (replacement != "") {
            q.replace(match.captured(0), replacement);
        }
    }
    return q;
}


QString ispr_include(QString & q0)
{
    QString q = q0;

    for (int i = 0; i < 10; i++)
    {
        QString q2 = ispr_include1(q);
        if (q2 == q) return q;
        q = q2;
    }

    qDebug() << "Error in ispr_include";
    
    return q;
}

