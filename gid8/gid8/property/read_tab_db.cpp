#include <QApplication>
#include <QHeaderView>
#include <QScreen>

#include <QtSql>
#include <QMessageBox>

#include <db/db.h>
#include <mystd.h>

QString argpath();
QString readCP1251Line(QFile &file);
QString findTableRusName(const QString & d, const QString & n);
QString findColumnRusName(const QString & d, const QString & n1, const QString & n2);

QString get_xml_path(const QString & tab1, const QString & tab2, const QString & name, const QString & defect, const QString & prefix);
#if 0
{
    QString s = QString(
        "'%1' ="
"STUFF(\n"
"( SELECT concat(', ', rtt2.name) AS n FROM %2 r2 \n"
"LEFT JOIN %3 rt2 ON r2.id=rt2.objID\n"
"LEFT JOIN %4 rtt2 ON rtt2.id=rt2.activityID\n"
"WHERE r2.id=%5.id \n"
"FOR XML PATH('')\n"
", TYPE\n"
").value('.', 'NVARCHAR(MAX)'), 1, 1, '')\n").arg(name, defect, tab1, tab2, prefix);
        
    return s;
}

#endif

bool parse_filtr(const QString & line, QString &tab1, QString &tab2, QString &title)
{
    static QRegularExpression re("\\$view_filtr\\$(.+)\\$(.+)\\$\\s+(.+)$");

    QRegularExpressionMatch match = re.match(line);
    if (match.hasMatch()) {
        tab1 = match.captured(1);
        tab2 = match.captured(2);
        title = match.captured(3);
        return true;
    }
    return false;
}

#include <init/init_files.h>


bool open_tab_file1(QFile &file, const QString & path, const QString & table, const QString & suffix)
{
    file.setFileName(QString("%1%2/%3.%4").arg(argpath(), path, table, suffix));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return true;
    }
    return false;
}


bool open_tab_file(QFile &file, const QString & table, const QString & suffix, QString &path)
{
    std::list<QString> lst = {"tab/gid8", "tab/gid8/pts", "tab/gid8/analiz", "tab", "tab/ps", "tab/remont", "dialog"};
    
    for (auto & it : lst) {
        file.setFileName(QString("%1%2/%3.%4").arg(argpath(), it, table, suffix));
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            path = it;
            return true;
        }
    }
    return false;
}


void read_tab(const QString & table, std::list<QString> &lst)
{
    lst.clear();
    QFile file;
    QString path;
    if (!open_tab_file(file, table, "txt", path)) return;

    while (!file.atEnd()) {
        QString line = readCP1251Line(file);
//        if (line.left(1) == "-") continue;
        lst.push_back(line);
    }
    return;
}

int headSpace(const QString & txt)
{
    for (int i = 0; i < txt.size(); i++) {
        if (txt.at(i) != ' ') return i;
    }
    return 0;
}

void read_disable(const QString & table, const QString & ext, std::map <QString, std::map<QString, std::list<QString> > > &map_disable)
{
    QFile file;
    QString path;
    if (!open_tab_file(file, table, ext, path)) return;

    int nn_old = 0;
    int level = 0;
//    int id = 0;
    QString id = "";

    int level_n[256];
    level_n[0] = 0;
    QString fn = "";

    while (!file.atEnd()) {
        QString line = readCP1251Line(file);
        if (line.left(1) == "-") continue;

        int nn = headSpace(line);

        if (nn > nn_old) {
            level += 1;
            if (level > 10) break;
            level_n[level] = nn;
        }
        else if (nn < nn_old) {
            for (int j = level-1; j >= 0; j--) {
                if (level_n[j] == nn) {
                    level = j;
                    break;
                }
            }
        }
        nn_old = nn;

        QString s = line.trimmed();
        int ls = s.indexOf(" ");

        if (ls != -1) {
            s = s.left(ls);
        }

        switch (level) {
            case 0:
                fn = s.toLower();
                break;
            case 1: 
//                id = s.toInt();                
                id = s.toLower();
                break;
            case 2: 
                map_disable[fn][id].push_back(s.toLower());
                break;
        }
    }
    return;
}

void read_fun(const QString & table, const QString & ext, std::map<QString, QString> &map_fun)
{
    QFile file;
    QString path;
    if (!open_tab_file(file, table, ext, path)) return;

    while (!file.atEnd()) {
        QString line = readCP1251Line(file);
        if (line.left(1) == "-") continue;
        static QRegularExpression re("^([^ ]+)\\s+([^\\s].*)\\s*$");

        QRegularExpressionMatch match = re.match(line);
        if (match.hasMatch()) {
            QString s1 = match.captured(1);
            QString s2 = match.captured(2);
            map_fun[s1] = s2;
        }
    }
    return;
}


void read_tab(const QString & table, std::list<QString> & filtr, std::list<AdoField> & fields)
{
    QFile file;

    QString path;
    if (!open_tab_file(file, table, "txt", path)) return;

    while (!file.atEnd()) {
        QString line = readCP1251Line(file);

        if (line.left(1) == "-") continue;
        if (line.left(1) == " ") {
            static QRegularExpression re("^\\s+([^ \\r\\n]+)");
            QRegularExpressionMatch match = re.match(line);
            if (match.hasMatch()) {
                QString txt = match.captured(1);

                for (auto & field : fields) {
                    if (QString::compare(field.COLUMN_NAME, txt, Qt::CaseInsensitive) == 0) {
                        filtr.push_back(txt);
                        break;
                    }
                }
            }
        }
        else if (line.left(1) == "$") {
            filtr.push_back(line);
        }
        else {
        }
    }
    return;
}

void get_3_param(QSqlDatabase & db, const QString & tn, const QString & table_file, const QString & prefix, int &ii, QString &par1, QString &par2, QString &joins, std::list<QString> & filtr, bool _read_tab, bool rus_name, const QString & s_shape, const QString & s_id)
{
    std::list<QString> columns;
    std::list<AdoField> fields;
    get_table_columns(db, tn, fields);

    for (auto & field : fields) {
        columns.push_back(field.COLUMN_NAME);
    }

    if (_read_tab) {
        read_tab(table_file, filtr, fields);
    }

    if (filtr.size() == 0) {
        for (auto & field : fields) {
            filtr.push_back(field.COLUMN_NAME);
        }
    }

    for (auto & c : filtr) {
        bool eq = false;
        for (auto & field : fields) {
            if (QString::compare(field.COLUMN_NAME, c, Qt::CaseInsensitive) == 0) {
                eq = true;
                break;
            }
        }
    }

    for (auto & c : filtr) {

        if (c == s_shape) continue;
        if (c == s_id) continue;

        if (c.left(1) == "$") {
            QString tab1, tab2, title;
            if (parse_filtr(c, tab1, tab2, title)) {
                if (par1 != "") par1 += ",\n";
                par1 += get_xml_path(tab1, tab2, title, tn, prefix);
            }
        }
        else {
            QString text = findColumnRusName(getDatabaseName(db), tn, c);
            const ForeignTable *ft = findLookup(getDatabaseName(db), tn, c);
            
            QString txt = c;
            QString tn_txt = QString("%1||%2").arg(txt, prefix);
//            QString tn_txt = QString("%1").arg(txt);
            if (prefix == "T") {
                tn_txt = QString("%1").arg(txt);
            }

            if (rus_name) {
                tn_txt = text;
            }




            if (par1 != "") par1 += ",\n";
            if (ft) {
                std::map<int, QString> *l2 = findLookup2(getDatabaseName(db), ft->table);

                if (l2) {
                    par1 += QString("CASE %1.%2\n").arg(prefix, txt);
                    for (auto &it : *l2) {
                        par1 += QString("WHEN %1 THEN '%2'\n").arg(it.first).arg(it.second);
                    }
//                    par1 += QString("END AS '%1'").arg(tn_txt);
                    par1 += QString("END AS %1").arg(quot_text(tn_txt));
                }
                else {
                    QString ft_name = ft->name;
                    if (ft->q == "") {

                        
                        joins += QString("LEFT JOIN %1 %2%3 ON %4%5.%6 = %7.%8\n")
                                     .arg(tbl_sql(ft->table)).arg(prefix).arg(ii).arg(prefix).arg(ii).arg(ft->id).arg(prefix, c);
                    }
                    else {
                        QString ftq = ft->q;
                        if (ft->table == "nodes") {
                            ft_name = "name";
                            ftq = QString(
"SELECT n.ID,\n"
"CONCAT(IIF(np.id IS NULL, '', CONCAT('(', ecp.name, ' ', np.externalNodeName, ') ')), t2.name, ' ', n.externalNodeName, ' ', IIF(n.externalSignID = 2, '(П)', IIF(n.externalSignID = 3, '(О)', ''))) AS name\n"
"FROM net.v_nodes n\n"
"LEFT JOIN externalCodes t2 ON t2.ID = n.externalCodeID\n"
"LEFT JOIN net.v_nodes np ON np.id = n.internalNodeID\n"
"LEFT JOIN externalCodes ecp ON ecp.ID = np.externalCodeID\n"
//"WHERE n.fileID =  %1\n"
                            );
                        }

                        joins += QString("LEFT JOIN (\n%1\n) %2%3 ON %4%5.%6 = %7.%8\n")
                            .arg(ftq).arg(prefix).arg(ii).arg(prefix).arg(ii).arg(ft->id).arg(prefix, c);
                    }

                    par1 += QString("%1%2.%3").arg(prefix).arg(ii).arg(ft_name);
                    par1 += QString(" AS %1").arg(quot_text(tn_txt));

                }
                if (!rus_name) {
                    par2 += QString(",\n%1.%2").arg(prefix, br_text(c));
//                    par2 += QString(" AS '%1||'").arg(tn_txt);
                    par2 += QString(" AS %1").arg(quot_text(tn_txt+"||"));
                }
            }
            else {
                par1 += QString("%1.%2").arg(prefix, br_text(c));
//                par1 += QString(" AS '%1'").arg(tn_txt);
                par1 += QString(" AS %1").arg(quot_text(tn_txt));
            }
        }
        ii += 1;
    }
}

QString create_select(QSqlDatabase & db, const QString & tn, const QString & s_shape, const QString & s_id)
{
    int i = 0;
    QString par1 = "", par2 = "", joins = "";

//    (i, par1, par2, joins) = get_3_param(cursor, tn, "T", i, par1, par2, joins, None);

//    q = f'SELECT TOP 10\nT.id,\n{par1}{par2}\nFROM {tn} T\n{joins}';

    std::list<QString> filtr;

    get_3_param(db, tn, tn, "T", i, par1, par2, joins, filtr, true, false, s_shape, s_id);

    QString q = QString("SELECT T.%1,%2\n%3\nFROM %4 T\n%5").arg(s_id, par1, par2, tbl_sql(tn), joins);
    return q;
}


QString create_select_node(QSqlDatabase & db, const QString & tn)
{
    int i = 0;
    QString par1 = "", par2 = "", joins = "";

    std::list<QString> filtr;

    filtr.push_back("externalCodeID");
    filtr.push_back("externalNodeName");
    filtr.push_back("externalSignID");
    filtr.push_back("geoMarkTopTube");
    filtr.push_back("geoMarkNodeArea");

    filtr.push_back("nodeTypeID");
    filtr.push_back("organizationID");
    filtr.push_back("inventNumber");
    filtr.push_back("registNumber");

    filtr.push_back("PICdate");
    filtr.push_back("lastRepairDate");
    filtr.push_back("archiveChangeDate");
    filtr.push_back("operatorID");

    filtr.push_back("fileID");

    joins += "LEFT JOIN net.v_nodes N ON N.id=T.nodeID\n";
    get_3_param(db, "nodes", "nodes", "N", i, par1, par2, joins, filtr, false, false, "shape", "id");
    filtr.clear();
    get_3_param(db, tn, tn, "T", i, par1, par2, joins, filtr, true, false, "shape", "id");

    QString q = QString("SELECT T.id,n.id as N_ID,%1\n%2\nFROM %3 T\n%4").arg(par1, par2, tbl_sql(tn), joins);
    return q;
}

QString create_select_line(QSqlDatabase & db, const QString & tn, const QString & table_file)
{
    int i = 0;
    QString par1 = "", par2 = "", joins = "";

    std::list<QString> filtr, filtr_n, filtr_l;

    filtr.push_back("externalSignLineID");
    filtr.push_back("organizationID");
    filtr.push_back("archiveChangeDate");
    filtr.push_back("operatorID");

    bool uuid = column_in_table(db, "linesobj", "globalid");

//    if (uuid) {
//        filtr.push_back("gistable");
//        filtr.push_back("globalid");
//    }
    
    filtr.push_back("hydroRes");
//    filtr.push_back("archiveChangeDateoperatorID");

    joins += "LEFT JOIN net.v_linesobj L ON T.lineID=L.id\n";
    par2 += ",\nL.id AS L_ID";
    get_3_param(db, "linesobj", "linesobj", "L", i, par1, par2, joins, filtr, false, false, "shape", "id");

    filtr_n.push_back("externalCodeID");
    filtr_n.push_back("externalNodeName");
    filtr_n.push_back("fileID");

    joins += "LEFT JOIN net.v_nodes N1 ON N1.id=L.nodeID1\n";
    par2 += ",\nN1.id AS N1_ID";
    get_3_param(db, "nodes", "nodes", "N1", i, par1, par2, joins, filtr_n, false, false, "shape", "id");

    joins += "LEFT JOIN net.v_nodes N2 ON N2.id=L.nodeID2\n";
    par2 += ",\nN2.id AS N2_ID";
    get_3_param(db, "nodes", "nodes", "N2", i, par1, par2, joins, filtr_n, false, false, "shape", "id");

    get_3_param(db, tn, table_file, "T", i, par1, par2, joins, filtr_l, true, false, "shape", "id");

    QString q = QString("SELECT T.id,%1\n%2\nFROM %3 T\n%4").arg(par1, par2, tbl_sql(tn), joins);

    return q;
}


QString create_select_all(QSqlDatabase & db, const QString & table, const QString & table_file, int id1, int id2, PrTypes pr_type, const QString & s_shape, const QString & s_id)
{
    QString q2;

    if (pr_type == pr_type_line) {
        q2 = create_select_line(db, table, table_file);
        q2 += QString("\nWHERE T.lineID IN (%1, %2) AND NOT L.id IS NULL").arg(id1).arg(id2);
    }
    else if (pr_type == pr_type_node) {
        q2 = create_select_node(db, table);
        q2 += QString("\nWHERE T.nodeID IN (%1, %2) AND NOT N.id IS NULL").arg(id1).arg(id2);
    }
    else if (pr_type == pr_type_line_out) {
        q2 = create_select_line(db, table, table_file);
//        q2 += QString("\nWHERE T.lineID IN (%1, %2) AND T.calculationID = (SELECT MAX(calculationID) FROM %3 WHERE lineID IN (%1,%2) )\nORDER BY T.externalSignLineID").arg(id1).arg(id2).arg(br_text(table));
        q2 += QString("\nWHERE T.lineID IN (%1, %2) AND NOT L.id IS NULL AND T.calculationID = (select MAX(id) from calculation c where c.fileID=N1.fileID)\nORDER BY T.externalSignLineID").arg(id1).arg(id2);
    }
    else if (pr_type == pr_type_node_out) {
        q2 = create_select_node(db, table);
//        q2 += QString("\nWHERE T.nodeID IN (%1, %2) AND T.calculationID = (SELECT MAX(calculationID) FROM %3 WHERE nodeID IN (%1,%2) )").arg(id1).arg(id2).arg(br_text(table));
        q2 += QString("\nWHERE T.nodeID IN (%1, %2) AND T.calculationID = (select MAX(id) from calculation c where c.fileID=N.fileID)").arg(id1).arg(id2);
    }
    else {
        q2 = create_select(db, table, s_shape, s_id);
        q2 += QString("\nWHERE T.%1 IN (%2, %3)").arg(s_id).arg(id1).arg(id2);
    }
    return q2;
}


QString create_select_rus(QSqlDatabase & db, const QString & tn, const QString & s_shape, const QString & s_id)
{
    int i = 0;
    QString par1 = "", par2 = "", joins = "";


    std::list<QString> filtr;

    get_3_param(db, tn, tn, "T", i, par1, par2, joins, filtr, false, true, s_shape, s_id);

    QString q = QString("SELECT T.%1,%2\n%3\nFROM %4 T\n%5").arg(s_id, par1, par2, tbl_sql(tn), joins);
    return q;
}
