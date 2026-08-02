#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QMessageBox>
#include <QRegularExpression>

#include "cxema/graph2.h"
#include "init/init_files.h"

#include <mystd.h>

#include "cxema/Graph.h"
#include <db/db.h>

#include <geo/Klassif.h>

QString st_distance1(const QString &e1, const QString &e2)
{
    QString s;
//    QString s = QString("%1.STDistance(%2.STPointN(1))").arg(e1, e2);
//    QString s = QString("ST_Distance(%1, ST_PointN(%2, 1))").arg(e1, e2);

    if (is_POSTGRESQL()) {
        s = QString("ST_Distance(%1, %2)").arg(e1, e2);
    }
    else {
        s = QString("%1.STDistance(%2.STPointN(1))").arg(e1, e2);
    }
    return s;
}

QString st_distance(const QString &e1, const QString &e2)
{
    QString s;

    if (is_POSTGRESQL()) {
        s = QString("ST_Distance(%1, %2)").arg(e1, e2);
    }
    else {
        s = QString("%1.STDistance(%2)").arg(e1, e2);
    }
    return s;
}



QString get_xml_path(const QString & tab1, const QString & tab2, const QString & name, const QString & defect, const QString & prefix)
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
                    ").value('.', 'NVARCHAR(MAX)'), 1, 1, '')\n").arg( name, defect, tab1, tab2, prefix);
        
    return s;
}


void copyVydGraph_new(CGraph2* graph1, CGraph2* graph2, bool is_po = false);
void sort_line_rs_new(CGraph2* graph, list<CNode2*>& list_sort, list<CLINE2*>& st_l, int fileID = -1);
bool isPSstrict_(CNode2* node)
{
    return false;
}


QString getNodeMarkTable(CGraph2 *graph)
{

    bool is_vyd = false;

    QString s, ss = "", q = "";


    CGraph2 graph2(NULL);
    list<CNode2*> list_sort;
    list<CLINE2*> st_l;
    list <list<int> > list_ut;
    list<int> list_ut_min;

    copyVydGraph_new(graph, &graph2, true);
    sort_line_rs_new(&graph2, list_sort, st_l);


    CNode2* n1_old = nullptr, * n2_old = nullptr;
    CNode2* n_old = nullptr;

    graph->init_find_line_nom();

    bool first = true;

    for (auto p : st_l) {
        CLINE2* l = p;

        CLINE2* lP = graph->find_line_nom(bline(l)->line.nomP);
        CLINE2* lO = graph->find_line_nom(bline(l)->line.nomO);

        if (lP || lO) {
            CNode2* n1 = where(l);
            CNode2* n2 = other(l);

            if (n1_old && (isPSstrict_(n1) || n1 != n2_old)) {
                for (int & it : list_ut_min) {
                    s = QString("(%1, %2)").arg(nn++).arg(it);
                    if (ss != "") ss += ",";
                    ss += s;

//                    if (ins.ado_insert_int2(nn++, it)) ret = true;
                }

                list_ut_min.clear();
            }

            list_ut_min.push_back(lP ? bline(lP)->line.nomP : bline(lO)->line.nomO);

            n1_old = n1;
            n2_old = n2;
        }
    }

    for (auto &it : list_ut_min) {
        s = QString("(%1, %2)").arg(nn++).arg(it);
        if (ss != "") ss += ",";
        ss += s;
        //        if (ins.ado_insert_int2(nn++, it)) ret = true;
    }


    return ss;
}




QString getLineMarkTable(CGraph2 *graph)
{

    bool is_vyd = false;

    QString s, ss = "", q = "";

#if 0

    CMapGraph::const_iterator p = graph->map_node.begin();
    while (p != graph->map_node.end()) {
        CNode2* n = p->second;
        for (CLINE2* l = n->lines; l; l = next(l)) {
            if (IsBegin(l) && bline(l)->line.isPjezo) {
                if (bline(l)->line.nomP > 0) {
                    s.Format("(%d)", bline(l)->line.nomP);
                    if (ss != "") ss += ",";
                    ss += s;
                }
                if (bline(l)->line.nomO > 0 && bline(l)->line.nomP != bline(l)->line.nomO) {
                    s.Format("(%d)", bline(l)->line.nomO);
                    if (ss != "") ss += ",";
                    ss += s;
                }
            }
        }
        p++;
    }
#endif

    CGraph2 graph2(NULL);
    list<CNode2*> list_sort;
    list<CLINE2*> st_l;
    list <list<int> > list_ut;
    list<int> list_ut_min;

    copyVydGraph_new(graph, &graph2, true);
    sort_line_rs_new(&graph2, list_sort, st_l);


    CNode2* n1_old = nullptr, * n2_old = nullptr;
    CNode2* n_old = nullptr;

    graph->init_find_line_nom();

    for (auto p : st_l) {
        CLINE2* l = p;

        CLINE2* lP = graph->find_line_nom(bline(l)->line.nomP);
        CLINE2* lO = graph->find_line_nom(bline(l)->line.nomO);

        if (lP || lO) {
            CNode2* n1 = where(l);
            CNode2* n2 = other(l);

            if (n1_old && (isPSstrict_(n1) || n1 != n2_old)) {
                for (int & it : list_ut_min) {
                    s = QString("(%1, %2)").arg(nn++).arg(it);
                    if (ss != "") ss += ",";
                    ss += s;

//                    if (ins.ado_insert_int2(nn++, it)) ret = true;
                }

                list_ut_min.clear();
            }

            list_ut_min.push_back(lP ? bline(lP)->line.nomP : bline(lO)->line.nomO);

            n1_old = n1;
            n2_old = n2;
        }
    }

    for (auto &it : list_ut_min) {
        s = QString("(%1, %2)").arg(nn++).arg(it);
        if (ss != "") ss += ",";
        ss += s;
        //        if (ins.ado_insert_int2(nn++, it)) ret = true;
    }


    return ss;
}


QString createSelect(const QString& tn)
{
    QString q = QString("SELECT * FROM %1").arg(tn);
    return q;
}

const map<QString, ForeignTable>* findLookup(const QString & d, const QString & n1);
//ForeignTable findLookup(const QString & d, const QString & n1, const QString & n2);
const map<QString, ForeignTable>* findLookup(const QString & d, const QString & n1);


FILE* fopen_tab(const char* tab, const char* tn, const char* ext);

bool parse_filtr(const QString &line, QString &tab1, QString &tab2, QString &title);
#if 0
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

#endif
void read_tab(const QString & table, std::list<QString> &lst);



bool readTab(const QString &tn, std::list<QString> & lst, std::map<QString, QString> & map_names)
{
    std::list<QString>  lst0;

    read_tab(tn, lst0);

    for (auto & str : lst0) {
        if (str.left(1) == " ") {
            str = str.trimmed();
            static QRegularExpression re("([^ ]+)\\s(.+)");
            QRegularExpressionMatch match = re.match(str);
            if (match.hasMatch()) {
                str = match.captured(1);
                QString name = match.captured(2);
                map_names[str] = name;
            }
            lst.push_back(str);
        }
    }

    return lst.size() > 0;


/*

    bool ret = false;

    FILE *f = fopen_tab("tab", tn, "txt");
    if (f) {
        char str[1024 + 1];

        while (!feof(f)) {
            if (fgets(str, 1024, f) == NULL) break;

            if (str[0] == ' ') {
                ret = true;
                trim(str);
                ltrim(str);
                QString ss = str, name = "";
                if (str[0] != '$') {
                    int pos = ss.Find(" ");
                    if (pos != -1) {
                        name = ss.Mid(pos +1);
                        ss = ss.Left(pos);

                        if (name != "") {
                            map_names[ss] = name;
                        }
                    }
                    
                }
                lst.push_back(ss);
            }
        }       
        fclose(f);
    }
*/
//    return ret;
}



bool is_onlyline(const QString & tn);
const CColName *findColumnRusNameFull1(const QString & d, const QString & table, const QString & column);

QString getGeoSelect(Klassif *kls, CGraph2 *graph)
{
    QString tn = kls->nazv;
    QSqlDatabase *db = kls->m_db;

    QString join_mark = "";
    QString mark_line = "";
    
    if (is_onlyline(kls->nazv)) {
        mark_line = getLineMarkTable(graph);
    }

    qDebug() << mark_line;

    if (mark_line == "") {
    join_mark = QString(

"\nleft join (\n"
"select \n"
"l.id,\n"
"ec1.name as kod1,\n"
"n1.externalNodeName as name1,\n"
"ec2.name as kod2,\n"
"n2.externalNodeName as name2,\n"
"l.shape \n"
"from linesobj l\n"
//"join heatPipeSections hps ON hps.lineID=l.id\n"
"--LEFT JOIN pipeSections pss ON pss.id = hps.pipeSectionID\n"

"join nodes n1 on n1.id=l.nodeID1\n"
"join nodes n2 on n2.id=l.nodeID2\n"
"join externalCodes ec1 on ec1.id=n1.externalCodeID\n"
"join externalCodes ec2 on ec2.id=n2.externalCodeID\n"
"where l.removed=0\n"
//") TM ON TM.shape.STDistance(T0.shape.STPointN(1)) < 0.3\n"
") TM ON %1 < 0.3\n"

                        ).arg(st_distance1("TM.shape", "T0.shape"));

    join_mark = "";
    }

    else {
    

    join_mark = QString(

"\njoin (\n"
"select \n"
"l.id,\n"
"ec1.name as kod1,\n"
"n1.externalNodeName as name1,\n"
"ec2.name as kod2,\n"
"n2.externalNodeName as name2,\n"
"mark_lines.ord,\n"
"l.shape \n"
"from linesobj l\n"
//"join heatPipeSections hps ON hps.lineID=l.id\n"
"--LEFT JOIN pipeSections pss ON pss.id = hps.pipeSectionID\n"

"join nodes n1 on n1.id=l.nodeID1\n"
"join nodes n2 on n2.id=l.nodeID2\n"
"join externalCodes ec1 on ec1.id=n1.externalCodeID\n"
"join externalCodes ec2 on ec2.id=n2.externalCodeID\n"
"join (values %1) mark_lines(ord, id) on mark_lines.id=l.id\n"
"where l.removed=0\n"
//") TM ON TM.shape.STDistance(T0.shape.STPointN(1)) < 0.3\n"
") TM ON %2 < 0.3\n"

).arg(mark_line, st_distance1("TM.shape", "T0.shape"));


    }


    QString par = "";
    QString par0 = "";
    QString joins = "";
    int n_join = 1;


//    const map<QString, ForeignTable> *map_lookup = findLookup(db, tn);

    if (par != "") par += ",\n";
//    par += "T0.id";
    par += QString("T0.%1").arg(kls->id);

    if (join_mark != "") {
        par += ",TM.kod1";
        par += ",TM.name1";
        par += ",TM.kod2";
        par += ",TM.name2";
    }

    if (mark_line != "") {
/*
        par += ",TM.kod1";
        par += ",TM.name1";
        par += ",TM.kod2";
        par += ",TM.name2";
*/

        par += ",TM.ord";

        if (par0 != "") par0 += ",\n";
        par0 += "t.id";
        par0 += ", row_number() over(order by ord) as id1";
        par0 += QString(",t.kod1 AS %1").arg(quot_text("Код РС начального узла"));
        par0 += QString(",t.name1 AS %1").arg(quot_text("Наименование начального узла"));
        par0 += QString(",t.kod2 AS %1").arg(quot_text("Код РС конечного узла"));
        par0 += QString(",t.name2 AS %1").arg(quot_text("Наименование конечного узла"));
    }


//    int n = ado->NColsMySQL(tn);

    struct Filtr {
        QString tab1, tab2, title;
    };

    std::list<QString> lst;
    std::map<QString, QString> map_names;

    std::list<AdoField> fields;
    std::map<QString, AdoField> map_fields;

    get_table_columns(*kls->m_db, kls->nazv, map_fields);
    get_table_columns(*kls->m_db, kls->nazv, fields);


//    int n =-

    if (!readTab(tn, lst, map_names)) {
        for (const auto& field : fields) {
            if (field.COLUMN_NAME == kls->shape) continue;
            if (field.DATA_TYPE == "geometry") continue;
            lst.push_back(field.COLUMN_NAME);
        }
    }
    else {
        lst.push_front(kls->id);
    }

    std::set<QString> st;


    int n_dop = 2;

//    for (int i = 0; i < n && i < 1000; i++) {
//    MySQLColumns* col = ado->findMySQLColumnsOrd(tn, i + 1);
    for (auto & col1 : lst) {

        Filtr fltr;       
        QString tab1, tab2, title;
        if (parse_filtr(col1, tab1, tab2, title)) {
            if (par != "") par += ",\n";
            par += get_xml_path(tab1, tab2, title, tn, "T0");

            if (par0 != "") par0 += ",\n";
            par0 += "\"";
            par0 += title;
            par0 += "\"";

            continue;
        }

        
//        MySQLColumns* col = ado->findMySQLColumns(tn, col1);



        auto it = map_fields.find(col1);


        if (it != map_fields.end()) {
            auto col = it->second;

            //            if (col->strDATA_TYPE != "geometry" && col->strDATA_TYPE != "ntext") {
            if (col.DATA_TYPE == "geometry") continue;
            QString s_col = col.COLUMN_NAME;
            if (s_col == kls->id) continue;
            if (s_col == "lineID" && mark_line != "") continue;

//            s_col.Format("%s", col->strCOLUMN_NAME);

            QString schema = getDatabaseName(*kls->m_db);

            const CColName* col_name = findColumnRusNameFull1(schema, tn, s_col);

            const ForeignTable *ft = findLookup(schema, tn, s_col);

            if (ft) {
                QString join;
                join = QString("\nLEFT JOIN %1 T%2 ON T%3.id=T0.%4")
                    .arg(br_text(ft->table)).arg(n_join).arg(n_join).arg(br_text(s_col));
                joins += join;
                s_col = QString("T%1.%2").arg(n_join).arg(ft->name);
                n_join += 1;
            }
            else {
                s_col = "T0." + s_col;
            }

            if (par != "") par += ",\n";
            par += s_col;

            QString cname = col.COLUMN_NAME;

            if (col_name) {
                cname = col_name->name;
            }
            
            auto it = map_names.find(col.COLUMN_NAME);
            if (it != map_names.end()) {
                cname = it->second;
            }


            if (st.find(cname) != st.end()) {
                QString ss = QString(" %1").arg(n_dop++);
                cname += ss;
                st.insert(cname);
            }
            else {
                st.insert(cname);
            }


            QString s = QString(" AS %1").arg(quot_text(cname));
            par += s;

            if (par0 != "") par0 += ",\n";
            par0 += "\"";
            par0 += cname;
            par0 += "\"";
        }
    }


    joins += join_mark;

    if (mark_line != "") {
//        joins += join_mark;

        if (par != "") par += ",\n";
//        par += "TM.shape.STDistance(T0.shape.STPointN(1)) as length,\nMIN(TM.shape.STDistance(T0.shape.STPointN(1))) OVER(PARTITION BY T0.id ) AS min_len";
        par += QString("%1 as length,\nMIN(%2) OVER(PARTITION BY T0.%3 ) AS min_len")
          .arg(st_distance1("TM.shape", "T0.shape"))
          .arg(st_distance1("TM.shape", "T0.shape"))
          .arg(kls->id)
          ;
    }

    QString dt = "1=1";

/*

    CGidrView* pView = getView();

    if (pView && kls->label_date) {
        QString ds1 = pView->m_sezon_date1.Format(_T("%Y%m%d"));
        QString ds2 = pView->m_sezon_date2.Format(_T("%Y%m%d"));
        dt = QString(" (T0.[%1] BETWEEN '%2' AND '%3' OR T0.[%4] is NULL) ").arg(kls->codlabel_date, ds1, ds2, kls->codlabel_date);
    }
*/

    QString q = QString("SELECT %1 %2\nFROM %3 T0%4\nWHERE %5").arg(top100000(), par, br_text(tn), joins, dt);


    if (mark_line != "") {
//        QString qq = QString("SELECT %1 \n%2\nFROM (\n%3\n) t\nwhere t.length = t.min_len\norder by t.ord").arg(top100000(), par0, q);
        QString qq = QString("SELECT %1 \n%2\nFROM (\n%3\n) t\nwhere t.length = t.min_len").arg(top100000(), par0, q);
        q = qq;
    }

    qDebug() << q;

    return q;
}
