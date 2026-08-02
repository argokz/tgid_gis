#include <QCoreApplication>
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QInputDialog>

#include <QtSql>

#include <QFile>
#include <cxema/graph2.h>
#include <QProgressDialog>
#include <QRegularExpression>
#include <QMessageBox>


#include "csv.h"
#include "db/db.h"


const std::list<QString> list_tab = {
 "fragments",
 "externalCodes",
 "nodes",

 "linesobj",

 "calcTemperatures",
 "gvsLoadGraphs",
 "specExpends",
 "varCoefficients",

 "directions",

 "calculations",
 "deployedDirections",
 "setPressNodes",
// "selectNodes",

 "texts",

 "connectNodes",
 "WDOdevices",
 "buildingEntries",
 "generalizedConsumers",
 "heatChambers",
 "heatSources",
 "realConsumers",
 "refillNodes",
 "threeWayValves",

    // для ПТС;
 "internalNodes",
 "overgroundNodes",
 "pavilions",
 "pumpStations",
 "trps",
 "undergroundNodes",
 "uninstalledNodes",

 "airHeaters",
 "bypass",
 "consumptRegulators",
 "dampers",
 "diaphragms",
 "elevators",
 "heatExchangers",
 "heatPipeSections",
 "localHydroResistances2",
 "opresDeployed",
 "pressDropRegulators",
 "pressRegulators",
 "pumps",
 "regulArmatures",
 "reverseValves",
 "systemRadiators",

 "deployedTempGraphs"
};



struct ForeignTable
{
    QString field;
    QString id;
    QString name;
    QString cxema;
    QString table;
    QString q;
    bool isSort;
};

const int TYPE_OF_NET_MSSQL = 1;
const int TYPE_OF_NET_MYSQL = 2;
const int m_type_of_net = TYPE_OF_NET_MSSQL;

//#define ISNO 1

bool isNo(const QString & tn, const QString & fn)
{
    if (fn == "shape") return true;
    if (fn == "id_old") return true;

#ifdef ISNO

    if (fn == "coords") return true;
    if (fn == "idRemoved") return true;
    if (fn == "removed") return true;
    if (fn == "organizationID") return true;

    if (tn == "nodes") {
        if (fn == "nodeName") return true;
        if (fn == "x") return true;
        if (fn == "y") return true;
    }
    if (tn == "dampers") {
        if (fn == "nodeName") return true;
        if (fn == "dispatcherSwitch") return true;
    }

#endif
    return false;
}

bool isNoTable(const QString & tn)
{
#ifdef ISNO
    if (tn == "") return true;
    if (tn == "localHydroResistances2") return true;
    if (tn == "opresDeployed") return true;
    if (tn == "deployedDirections") return true;
    if (tn == "texts") return true;
    if (tn == "pipeSections") return true;

#endif

    return false;
}



QString correctQ(const QString & q0)
{
    QString q = q0;

    if (m_type_of_net == TYPE_OF_NET_MSSQL) {
      q.replace("`", "");
    }

    if (m_type_of_net == TYPE_OF_NET_MYSQL) {
      q.replace("[", "`");
      q.replace("]", "`");
      q.replace(".dbo.", ".");
    }


    if (m_type_of_net == TYPE_OF_NET_MSSQL) {
      q.replace(" IF(", "IIF(");
      q.replace(",IF(", ",IIF(");
      q.replace("CREATE TEMPORARY TABLE ", "CREATE TABLE ");
    }

    if (m_type_of_net == TYPE_OF_NET_MSSQL) {
        q.replace(" ps.", "");
    }
    return q;
}


/*

int ExecuteInsert(QSqlDatabase & db, const QString & q)
{
    if(db.driver()->hasFeature(QSqlDriver::LastInsertId)){
        QSqlQuery insert(db);
        bool ret = query_exec(db, insert, q);
//        Q_ASSERT(res);
        if (!ret) return 0;

        QVariant id = insert.lastInsertId();
        Q_ASSERT(id.isValid() && !id.isNull());
        return id.toInt();
    }
    return 0;
}

*/


bool isDate(int typ)
{
    if (typ == QMetaType::QDate) return true;
    if (typ == QMetaType::QDateTime) return true;
    if (typ == QMetaType::QTime) return true;

    /*
    if (typ == adDBDate) return true;
    if (typ == adDBTime) return true;
    if (typ == adDBTimeStamp) return true;
*/
    return false;
}

bool isStr(int typ)
{
    if (isDate(typ)) return true;

    if (typ ==  QMetaType::QString) return true;
    if (typ ==  QMetaType::QChar) return true;

    if (typ ==  QMetaType::QReal || typ ==  QMetaType::Int || typ ==  QMetaType::UInt) {
        return false;
    }


    /*
    if (typ == adGUID) return true;
    if (typ == adBSTR) return true;
    if (typ == QMetaType::QChar) return true;
    if (typ == adVarChar) return true;
    if (typ == adLongVarChar) return true;
    if (typ == adWChar) return true;
    if (typ == adVarWChar) return true;
    if (typ == adLongVarWChar) return true;

    return false;
*/
    return true;
}

QString ispr(const QString & s0, int typ)
{
    QString s = s0;

    if (isDate(typ)) {
        static QRegularExpression re("^([0-9]+)\\-([0-9][0-9])\\-([0-9][0-9])");
        QRegularExpressionMatch match = re.match(s);
        if (match.hasMatch()) {
            int y = match.captured(1).toInt();
            int m = match.captured(2).toInt();
            int d = match.captured(3).toInt();
//            s = QString("\"%1-%2-%3\"").arg(y, 4, 10, QChar('0')).arg(m, 2, 10, QChar('0')).arg(d, 2, 10, QChar('0'));
            s = QString("\"%1%2%3\"").arg(y, 4, 10, QChar('0')).arg(m, 2, 10, QChar('0')).arg(d, 2, 10, QChar('0'));
        }
        else {
            s = "";
        }
    }
    else if (isStr(typ)) {
        s = s.replace("\"", "\"\"");
        s = "\"" + s + "\"";
    }
    return s;
}

//QString ispr2(const QString & s, int typ)
QString ispr2(const QString & s0, const QString & typ)
{
    QString s = s0;

    if (s == NULL_VALUE) return "NULL";

//    if (isStr(typ)) {

    if ((s == "" && typ == "date") || typ == "timestamp without time zone")
    {
        return "NULL";
    }

    if (typ == "integer") {
        if (s == "TRUE") return "1";
        if (s == "FALSE") return "0";
    }

    if (typ == "date" || typ == "text" || typ == "character varying"
        || typ == "nvarchar" || typ == "ntext" || typ == "timestamp without time zone")

    {
        s = s.replace("'", "''");
        if (is_MSSQL()) {
            s = "N'";
        }
        s = "'" + s + "'";
    }
    else if (s == "") {
        return "NULL";
    }


    return s;
}

bool export_table(QTextStream &out, QSqlDatabase & db, const QString & tn, const QString & q, const QString & fieldID, bool isNode, CGraph2* graph)
{
    if (isNoTable(tn)) return false;

    out << "-------------------------\n";
    out << tn << Qt::endl;

    QSqlQuery query(db);
    query.setForwardOnly(true);

    query_exec(db, query, q);

    QSqlRecord rec = query.record();

//    if (!db->openTable0(q)) return false;

    int nf = rec.count();

    std::vector<QSqlField> columns(nf);

    for (int i = 0; i < nf; i++) {
        columns[i] = rec.field(i);
    }

    QString s = "";
    for (int i = 0; i < nf; i++) {
        if (/*rec.fieldName(i) != "shape" && */ !isNo(tn, rec.fieldName(i))) {
            if (s != "") s += ",";
            s += ispr(rec.fieldName(i), QMetaType::QString);
        }
    }
    out << s <<  Qt::endl;

    int id_new = 1;

    while (query.next()) {
        QString s = "";

        long id = query.value("id").toInt();

        bool yes = true;

        if (fieldID != "" && graph) {
            yes = false;
            long id2 = query.value(fieldID).toInt();
            CNode2 *n = nullptr;
            if (isNode) {
                n = graph->find(id2);
                if (n && n->node.isPjezo) yes = true;

            }
            else {
                CLINE2 *l = graph->find_line_nom(id2);
                if (l) {
                    if (bline(l)->line.isPjezo) yes = true;
                    n = where(l);
                }
            }
            if (!yes && n) {
                n = graph->find(n->node.internalNodeID);
                if (n && n->node.isPjezo) yes = true;
            }
        }
        if (yes) {

            if (id == 6808) {
                int qq;
                qq = 1;
            }
            for (int i = 0; i < nf; i++) {
                if (/*rec.fieldName(i) != "shape"*/ !isNo(tn, rec.fieldName(i))) {
                    if (s != "") s += ",";
                    QVariant val= query.value(i);

                    QString v = val.toString();
                    if (val.isNull()) {
                        v = "";
                    }

                    int typ = rec.field(i).metaType().id();
//                    s += ispr(v, columns[i] typ);
                    s += ispr(v, typ);
                }
            }
            out << s << Qt::endl;
        }
    }

    return true;
}

bool export_table_unvisual(QTextStream &out, QSqlDatabase & db, const QString & tn, int fileID, CGraph2* graph)
{
    QString q = QString("SELECT * FROM %1 WHERE fileID=%2").arg(tbl_sql(tn)).arg(fileID);
    return export_table(out, db, tn, q, "", false, graph);
}

bool export_table_node(QTextStream &out, QSqlDatabase & db, const QString & tn, int fileID, CGraph2* graph)
{
    QString q = QString(
        "SELECT obj.* FROM %1 obj\n"
        "JOIN net.v_nodes n ON n.id = obj.nodeID\n"
        "WHERE n.fileID = %2 AND n.removed = 0").arg(tbl_sql(tn)).arg(fileID);

    return export_table(out, db, tn, q, "nodeID", true, graph);
}

bool export_table_line(QTextStream &out, QSqlDatabase & db, const QString & tn, int fileID, CGraph2* graph)
{
    QString q = QString(
    "SELECT obj.* FROM %1 obj\n"
        "JOIN net.v_linesobj l ON l.id = obj.lineID\n"
        "JOIN net.v_nodes n1 ON n1.id = l.nodeID1\n"
        "JOIN net.v_nodes n2 ON n2.id = l.nodeID2\n"
        "WHERE n1.fileID = %2 AND n2.fileID = %3 AND l.removed = 0 AND n1.removed = 0 AND n2.removed = 0").arg(tbl_sql(tn)).arg(fileID).arg(fileID);

    return export_table(out, db, tn, q, "lineID", false, graph);
}


bool export_tgid(QSqlDatabase & db, const QString & fn, int fileID, CGraph2 *graph)
{
//    FILE* f = fopen(fn, "w");
    QFile file(fn);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "", "Не могу открыть файл для записи");
        return false;
    }

    QTextStream out(&file);

    if (graph) {
       graph->init_find_line_nom();
    }

    QString q;

    out << "=========================\n";
    out << "Version: 2.0" << Qt::endl;
    out << "CodePage: utf-8" << Qt::endl;
    out << "Server: " << db.hostName() << Qt::endl;
    out << "Database: " << getDatabaseName(db) << Qt::endl;
    
//    db.databaseName() << Qt::endl;
    out << "User: " << db.userName() << Qt::endl;

    QString date = QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm");

    out << "Date: " << date << Qt::endl;

    q = "SELECT * FROM heatSystem";
    export_table(out, db, "heatSystem", q, "", false, graph);

    q = QString("SELECT * FROM fragments WHERE id=%1").arg(fileID);
    export_table(out, db, "fragments", q, "", false, graph);

    export_table_unvisual(out, db, "externalCodes", fileID, graph);

    //    export_table_unvisual(f, db, "CALCULATION", fileID, graph);
    export_table_unvisual(out, db, "calcTemperatures", fileID, graph);
    export_table_unvisual(out, db, "calculations", fileID, graph);
    export_table_unvisual(out, db, "gvsLoadGraphs", fileID, graph);
    export_table_unvisual(out, db, "specExpends", fileID, graph);

    export_table_unvisual(out, db, "texts", fileID, graph);

    export_table_unvisual(out, db, "varCoefficients", fileID, graph);

    //    export_table_node(out, db, "sortNodesForUchastok", fileID);
    //    export_table_unvisual(out, db, "sortLinesForUchastok", fileID);


    q = QString("SELECT * FROM net.v_nodes WHERE fileID=%1 AND removed=0").arg(fileID);
    export_table(out, db, "nodes", q, "id", true, graph);

    q = QString(
"SELECT pss.* FROM pipeSections pss\n"
"JOIN heatPipeSections hps ON hps.pipeSectionID=pss.id\n"
"JOIN net.v_linesobj l ON l.id=hps.lineID\n"
"JOIN net.v_nodes n1 ON n1.id=l.nodeID1\n"
"WHERE n1.fileID=%1 AND l.removed=0"
).arg(fileID);

    export_table(out, db, "pipeSections", q, "lineID", false, graph);


    q = QString(
        "SELECT l.* FROM net.v_linesobj l\n"
        "JOIN net.v_nodes n1 ON n1.id=l.nodeID1\n"
        "JOIN net.v_nodes n2 ON n2.id=l.nodeID2\n"
        "WHERE n1.fileID=%1 AND n1.fileID=n2.fileID\n"
        "AND l.removed=0 AND n1.removed=0 AND n2.removed=0\n"
        "AND (n1.internalNodeID=n2.internalNodeID OR (n1.internalNodeID IS NULL AND n2.internalNodeID IS NULL))"

        ).arg(fileID);

    export_table(out, db, "linesobj", q, "id", false, graph);

    q = QString("SELECT zn.* FROM setPressNodes zn JOIN net.v_nodes n ON n.id=zn.nodeID WHERE n.fileID=%1 AND n.removed=0").arg(fileID);

    export_table(out, db, "setPressNodes", q, "nodeID", true, graph);

    export_table_unvisual(out, db, "directions", fileID, graph);
    export_table_node(out, db, "deployedDirections", fileID, graph);
    export_table_node(out, db, "connectNodes", fileID, graph);

    // Узел для связи

    export_table_node(out, db, "WDOdevices", fileID, graph);
    export_table_node(out, db, "buildingEntries", fileID, graph);
    export_table_node(out, db, "generalizedConsumers", fileID, graph);
    export_table_node(out, db, "heatChambers", fileID, graph);
    export_table_node(out, db, "heatSources", fileID, graph);
    export_table_node(out, db, "realConsumers", fileID, graph);
    export_table_node(out, db, "refillNodes", fileID, graph);
    export_table_node(out, db, "threeWayValves", fileID, graph);

    q = QString("SELECT tg.* FROM deployedTempGraphs tg JOIN net.v_heatsources hs ON hs.id=tg.hSourceID JOIN net.v_nodes n ON n.id=hs.nodeID WHERE n.fileID=%1").arg(fileID);

    export_table(out, db, "deployedTempGraphs", q, "", false, graph);

    // для ПТС
    export_table_node(out, db, "internalNodes", fileID, graph);
    export_table_node(out, db, "overgroundNodes", fileID, graph);
    export_table_node(out, db, "pavilions", fileID, graph);
    export_table_node(out, db, "pumpStations", fileID, graph);
    export_table_node(out, db, "trps", fileID, graph);
    export_table_node(out, db, "undergroundNodes", fileID, graph);
    export_table_node(out, db, "uninstalledNodes", fileID, graph);

    export_table_line(out, db, "airHeaters", fileID, graph);
    export_table_line(out, db, "bypass", fileID, graph);
    export_table_line(out, db, "consumptRegulators", fileID, graph);
    export_table_line(out, db, "dampers", fileID, graph);
    export_table_line(out, db, "diaphragms", fileID, graph);
    export_table_line(out, db, "elevators", fileID, graph);
    export_table_line(out, db, "heatExchangers", fileID, graph);
    export_table_line(out, db, "heatPipeSections", fileID, graph);
    export_table_line(out, db, "localHydroResistances2", fileID, graph);
    export_table_line(out, db, "opresDeployed", fileID, graph);
    export_table_line(out, db, "pressDropRegulators", fileID, graph);
    export_table_line(out, db, "pressRegulators", fileID, graph);
    export_table_line(out, db, "pumps", fileID, graph);
    export_table_line(out, db, "regulArmatures", fileID, graph);
    export_table_line(out, db, "reverseValves", fileID, graph);
    export_table_line(out, db, "systemRadiators", fileID, graph);

    return true;
}

QString ValidFileName(const QString & s0)
{
    QString s = s0;

    s.replace("<", "_");
    s.replace(">", "_");
    s.replace(":", "_");
    s.replace("\"", "_");
    s.replace("/", "_");
    s.replace("\\", "_");
    s.replace("|", "_");
    s.replace("?", "_");
    s.replace("*", "_");
    s.replace(".", "_");

    return s;
}
#if 0
void CGidrView::OnExportFragment()
{
    int nFragments(CAdoFile * db, CCxema * cxema, QString & fragment);

    QString fragment;

    long fileID = nFragments(m_cxema->m_ado, m_cxema, fragment);

    if (fileID <= 0) {
        QMessageBox::warning(nullptr, "", "Для экспорта должен быть открыт только один фрагмент");
        return;
    }

    CFileDialog fd(FALSE, _T("tgid"), ValidFileName(fragment) + ".tgid",
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        _TR("*.tgid|*.tgid|"), this);

    if (fd.DoModal() == IDOK) {
        QString fn = fd.GetPathName();

        QString tmpn;

        tmpn.Format("%s\\tempfileforexport.tmp", QDir::tempPath());

        if ( export_tgid(m_cxema->m_ado, tmpn, fileID, nullptr)) {
            HZIP hz = CreateZip(fn,0);
            QString tn = GetTitle(fn)+".txt";
            tn = "tgid.txt";
            ZipAdd(hz, tn, tmpn);
            CloseZip(hz);
            AfxMessageBox("Выполнено", MB_OK|MB_ICONINFORMATION);
        }
    }
}

void CGidrView::OnExportFragmentVyd()
{
    int nFragments(CAdoFile * db, CCxema * cxema, QString & fragment);

    QString fragment;

    long fileID = nFragments(m_cxema->m_ado, m_cxema, fragment);

    if (fileID <= 0) {
        AfxMessageBox("Для экспорта должен быть открыт только один фрагмент", MB_OK|MB_ICONINFORMATION);
        return;
    }

    if (!m_cxema->m_graph->isMark()) {
        AfxMessageBox("Для экспорта должен быть выделен фрагмент сети", MB_OK|MB_ICONINFORMATION);
        return;
    }

    CFileDialog fd(FALSE, _T("tgid"), ValidFileName(fragment) + ".tgid",
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        _TR("*.tgid|*.tgid|"), this);

    if (fd.DoModal() == IDOK) {
        QString fn = fd.GetPathName();

        QString tmpn;

        tmpn.Format("%s\\tempfileforexport.tmp", QDir::tempPath());

        if ( export_tgid(m_cxema->m_ado, tmpn, fileID, m_cxema->m_graph)) {
            HZIP hz = CreateZip(fn,0);
            QString tn = GetTitle(fn)+".txt";
            tn = "tgid.txt";
            ZipAdd(hz, tn, tmpn);
            CloseZip(hz);
            AfxMessageBox("Выполнено", MB_OK|MB_ICONINFORMATION);
        }
    }
}



void CGidrView::OnExportFragmentsMulty()
{

    int n = m_cxema->m_file_map.size();
    int i = 0;

    QString path;

    if (!getDir(this, path)) return;


    Percent2 pc;

    for (auto it : m_cxema->m_file_map) {
        pc.SetValue(i*100/n);
        i++;

        QString fn;

        int fileID = it.second->id;
        QString name = ValidFileName(it.second->name);

        fn.Format("%s\\%s.tgid", path, name);

        QString tmpn;

        tmpn.Format("%s\\tempfileforexport.tmp", QDir::tempPath());

        if ( export_tgid(m_cxema->m_ado, tmpn, fileID, m_cxema->m_graph)) {
            HZIP hz = CreateZip(fn,0);
            QString tn = GetTitle(fn)+".txt";
            tn = "tgid.txt";
            ZipAdd(hz, tn, tmpn);
            CloseZip(hz);
        }
    }
}
#endif


static std::map<QString, std::map<long, long> > map_num;
static QString m_fragment_name = "";

static std::map<QString, std::vector<QString> > map_col;
static std::map<QString, std::map<int, std::vector<QString> > > map_val;
static int n_rows;
static int i_rows;

QString get_fragment_name()
{
    return m_fragment_name;
}


bool open_tgid(const QString & fn)
{
    QFile file(fn);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

//    QTextStream in(&file);
//    in.setEncoding(QStringConverter::System);

    map_num.clear();

    map_col.clear();
    map_val.clear();

    n_rows = 0;
    i_rows = 0;

    int n_fragment = 1;
    std::vector<QString> columns;

    QString tn = "";
    QString cols = "";

    QString line;

    int N = 100000;

    QString code = "cp1251";

    while (!file.atEnd()) {
//        line = in.readLine();
//        line = readLineCode(file);

//        if (fgetstr(line, N, f) == NULL) break;
        line = readLineCode(file, code);
//        line = in.readLine();
        if (line == "") break;

        if (line.left(1) == "=") {
            n_fragment += 1;
//            line = in.readLine();
//            line = in.readLine();
//            line = in.readLine();
//            line = in.readLine();

            line = readLineCode(file, code);

            if (line.left(strlen("Version")) == "Version") {
                line = readLineCode(file, code);
                line = readLineCode(file, code);
                code = "utf-8";
            }

            line = readLineCode(file, code);
            line = readLineCode(file, code);
            line = readLineCode(file, code);
        }
        else if (line.left(1) == "-") {
//            line = in.readLine();
            line = readLineCode(file, code);
            if (line == "") break;
            tn = line;
//            line = in.readLine();
            line = readLineCode(file, code);
            if (line == "") break;
            columns = csv_read(line);
            map_col[tn.toLower()] = columns;
        }
        else {
            std::vector<QString> l = csv_read(line);

            if (l.size() > 0) {
                long id = l[0].toInt();
                map_val[tn.toLower()][id] = l;
                n_rows ++;
                if (tn == "nodes") n_rows ++;
            }
        }
    }


    return true;
}

const map<QString, ForeignTable>* findLookup(const QString & d, const QString & n1);

static map<int, int> map_ist;




bool renumerate_db(QSqlDatabase & db, const QString & tn, const QString & tn0, const QString & col)
{
/*
    QString q = QString(
       "update %1 set fileID = f.id\n"
       "from %1 n\n"
//       "join __%1 n2 on n.id=n2.id\n"
       "join %2 f on f.id_old=n.%3\n"
       "where f.id_old is not null and n.id=%1.id\n"
       ).arg(tn, tn0, col);
*/

    QString q = QString(
       "update %1 set %3 = f.id\n"
       "from %1 n\n"
       "join __%1 n2 on n.id_old=n2.id\n"
       "join %2 f on f.id_old=n2.%3\n"
       "where f.id_old is not null and n.id=%1.id\n"
       ).arg(tn, tn0, col);



    bool ret = query_exec(db, q, false);

    return ret;
}

const std::map<QString , QString> map_qq =
{
    {"fileid", "fragments"},
    {"externalcodeid","externalcodes"},
    {"nodeid", "nodes"},
    {"nodeid1", "nodes"},
    {"nodeid2", "nodes"},
    {"connectid", "nodes"},
    {"lineid", "linesobj"},
    {"directionid", "directions"},
    {"calctemperatureid", "calctemperatures"},
    {"gvsloadgraphid", "gvsloadgraphs"},
    {"specexpendid", "specexpends"},

    {"varcoeffid", "varcoefficients"},
    {"varcoeffidflow", "varcoefficients"},
    {"varcoeffidret", "varcoefficients"}
};


bool check_import(QSqlDatabase & db, const QString & tn, std::map<QString, std::map<QString, std::set<int>  > > & map_error)
{
    const map<QString, ForeignTable>* map_l = findLookup(getDatabaseName(db), tn);

    if (!map_l) return true;
//        auto it_l = map_l->find(cl);
//        lookup = (it_l != map_l->end());

    std::map<QString, AdoField> map_fields;

    get_table_columns(db, tn, map_fields);

    auto col_it = map_col.find(tn.toLower());
    if (col_it == map_col.end()) {
        return false;
    }

    QString joins = "";
    QString cols2 = "";
    QString cols3 = "";

    int nj = 1;

    for (auto & c : col_it->second) {
        QString cl = c.toLower();
        auto it = map_fields.find(cl);

        if (cl == "id") continue;
        if (cl == "internalnodeid") continue;
        if (cl == "heatsourceid" && tn.toLower() == "externalcodes") continue;

        if (it == map_fields.end()) continue;

        bool lookup = true;

        auto it_l = map_l->find(cl);
        lookup = (it_l != map_l->end());

        auto itqq = map_qq.find(cl);

        if (itqq == map_qq.end() && lookup) {

            if (cols2 != "") cols2 += ",";

            cols2 += QString("(t.%1 IS NOT DISTINCT FROM t%2.id) as %1, t.%1 as v%2").arg(br_text(cl)).arg(nj);

            if (cols3 != "") cols3 += " or ";
            cols3 += QString("not %1").arg(br_text(cl));

            joins += QString("\nleft join %1 t%2 on t%2.id=t.%3").arg(tbl_sql(it_l->second.table)).arg(nj).arg(c);
            nj ++;
        }
    }

    if (nj == 1) return true;

    QString q = QString("select %1\nfrom __%2 t%3").arg(cols2, tn, joins );

    q = QString("select distinct * from (\n%1\n) _\nwhere %2").arg(q, cols3);


    QSqlQuery query(db);

    bool error = false;

    std::map<QString, std::set<int>  > map_id;

    if (query_exec(db, query, q)) {
     
        QSqlRecord record = query.record();
        int nf = record.count();
        
        while (query.next()) {
            for (int i = 0; i < nf; i += 2) {
                QSqlField field = record.field(i);
                bool y = query.value(i).toBool();
                if (!y) {
                    int id = query.value(i+1).toInt();
                    if (id > 0) {
                        map_id[field.name()].insert(id);
                        error = true;
                    }
                }
            }
        }
    }

    if (error) {

        map_error[tn.toLower()] = map_id;

        QString s = "";

        for (auto & [key, set_id]: map_id) {
            s += key;
            s += ":";
            auto it_l = map_l->find(key.toLower());
            if (it_l != map_l->end()) {
                s += it_l->second.table;
                s += ":";
            }
                

            for( auto &id: set_id) {
                s += QString(",%1").arg(id);
            }
            s += "\n";
        }
//        QMessageBox::warning(nullptr, "", s);
///        QInputDialog::getMultiLineText(nullptr, "", "", q);
    }

    return true;

}


bool add_id_old_column(QSqlDatabase & db, const QString & tn)
{
    bool ret;
    QString q;
    q = QString("ALTER TABLE %1 ADD id_old int DEFAULT 0").arg(tn);
    ret = query_exec(db, q, false);

    q = QString("UPDATE %1 SET id_old=0").arg(tn);
    ret = query_exec(db, q);

    return ret;
}


bool create_temp_table(const QString & fn, QSqlDatabase & db, const QString & tn, bool add_id_old = false)
{
    QString q;

    if (add_id_old) {
        add_id_old_column(db, tn);
     }

    q = QString("DROP TABLE IF EXISTS __%1").arg(tn);

    query_exec(db, q);

    q = QString("CREATE TABLE IF not exists __%1 AS SELECT * FROM %1 limit 0;").arg(tn);

    query_exec(db, q);

    auto val_it = map_val.find(tn.toLower());
    if  (val_it == map_val.end()) {
        return 0;
    }

    auto col_it = map_col.find(tn.toLower());
    if (col_it == map_col.end()) {
        return 0;
    }

    std::map<QString, AdoField> map_fields;

    get_table_columns(db, tn, map_fields);

    QString cols = "";
    for (auto & c : col_it->second) {
        auto it = map_fields.find(c.toLower());

        if (it != map_fields.end()) {
            if (cols != "")  cols += ",";
            cols += br_text(c);
        }
        else {
            QMessageBox::warning(nullptr, "", QString("Куда-то девалось поле %1 из таблицы %2").arg(c, tn));
        }
    }

    QString ins = QString("insert into __%1 (%2) values\n").arg(tn, cols);

    bool first = true;

    for (auto & it : val_it->second) {

        QString vals = "";

        vector<QString> l = it.second;
        int nf = l.size();

        for (int i = 0; i < nf; i++) {
            QString col = col_it->second[i];
            QString col_l = col.toLower();

            QString v = l[i];


            if (col == "name" && tn == "fragments") {
//                v = QString("Добавил %1").arg(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm"));
                v = QString("%1 %2").arg(fn).arg(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm"));
            }

            auto it = map_fields.find(col_l);

            if (it != map_fields.end()) {
                if (vals != "") vals += ",";
                vals += ispr2(v, it->second.DATA_TYPE);
            }
        }

        if (!first) ins += ",\n";
        first = false;

        ins += "(";
        ins += vals;
        ins += ")";
    }

    bool ret = query_exec(db, ins);

    return ret;
}

bool copy_temp_table(QSqlDatabase & db, const QString & tn, bool add_id_old = false)
{
    QString joins = "";
    QString cols1 = "";
    QString cols2 = "";

    std::map<QString, AdoField> map_fields;

    get_table_columns(db, tn, map_fields);

    auto col_it = map_col.find(tn.toLower());
    if (col_it == map_col.end()) {
        return false;
    }

    int nj = 1;

    if (add_id_old) {
        cols1 = "id_old",
        cols2 = "t.id";
    }

    for (auto & c : col_it->second) {
        QString cl = c.toLower();
        auto it = map_fields.find(cl);

        if (cl == "id") continue;
        if (cl == "internalnodeid") continue;
        if (cl == "heatsourceid" && tn.toLower() == "externalcodes") continue;

        if (it == map_fields.end()) continue;

        if (cols1 != "") cols1 += ",";
        if (cols2 != "") cols2 += ",";

        cols1 += br_text(cl);

        bool lookup = true;

        const map<QString, ForeignTable>* map_l = findLookup(getDatabaseName(db), tn);

        if (map_l) {
            auto it_l = map_l->find(cl);
            lookup = (it_l != map_l->end());
        }

        auto itqq = map_qq.find(cl);

        if (itqq != map_qq.end() && lookup) {
           joins += QString("\nLEFT JOIN %1 t%2 ON t%2.id_old=t.%3").arg(itqq->second).arg(nj).arg(c);
           cols2 += QString("t%1.id").arg(nj);
           nj ++;
        }
        else {
           cols2 += QString("t.%1").arg(br_text(cl));
        }
    }

    QString q = QString("insert into %1 (%2)\nselect %3\nfrom __%1 t%4").arg(tn, cols1, cols2, joins );

    if (tn.toLower() == "heatpipesections") {
//        QInputDialog::getMultiLineText(nullptr, "", "", q);
    }
    bool ret = query_exec(db, q);

    return ret;
}


/*
int open_table(QSqlDatabase & db, const QString & tn, bool add_id_old = false)
{
    std::map<QString, std::map<QString, std::set<int>  > > map_error;

    create_temp_table(db, tn, add_id_old);
    check_import(db, tn, map_error);
    copy_temp_table(db, tn, add_id_old );

    return 0;
}
*/

bool ispr_nodes(QSqlDatabase & db)
{
    QString q =
       "update nodes set internalnodeid=n.id\n"
       "from net.v_nodes nn\n"
       "join __nodes on __nodes.id=nn.id_old\n"
       "join net.v_nodes n on n.id_old = __nodes.internalnodeid\n"
       "where nodes.id=nn.id";

    bool ret = query_exec(db, q);
    return ret;
}

bool ispr_externalCodes(QSqlDatabase & db)
{
    QString q =

       "update externalcodes\n"
       "set heatsourceid = hs.id\n"
       "from externalcodes ec\n"
       "join __externalcodes ec2 on ec2.id=ec.id_old \n"
       "join __heatsources hs2 on hs2.id=ec2.heatsourceid \n"
       "join net.v_heatsources hs on hs.id_old=hs2.id\n"
       "join __nodes n2 on n2.id=hs2.nodeid \n"
       "where ec2.fileid = n2.fileid\n"
       "and ec.id=externalcodes.id\n";

    bool ret = query_exec(db, q);
    return ret;
}



bool add_id_old(const QString & tn)
{
    for (const auto& [key, value] : map_qq) {
       if (value.toLower() == tn.toLower()) {
           return true;
       }
    }
    return false;
}

QString findTableRusName(const QString & d, const QString & n);
QString findColumnRusName(const QString & d, const QString & table, const QString & column);



int import_tgid(QSqlDatabase & db, const QString & fn, const QString & tgid_name)
{
    map_ist.clear();
    open_tgid(fn);

    std::map<QString, std::map<QString, std::set<int>  > > map_error;

//    if (!db->StartTransaction()) return -1;

    for (auto &tn: list_tab) {
        create_temp_table(QFileInfo(tgid_name).baseName(), db, tn, add_id_old(tn));
    }

    for (auto &tn: list_tab) {
        check_import(db, tn, map_error);
    }

    QString s = "";

    for (auto & [tn, map_id] : map_error) {
        QString tnr = findTableRusName("gid", tn);
        const map<QString, ForeignTable>* map_l = findLookup(getDatabaseName(db), tn);

        if (s != "") s+= "\n";
        s += QString("%1 %2").arg(tn, tnr);

        for (auto & [col, set_id] : map_id) {
           QString colr = findColumnRusName("gid", tn, col);

           QString tn_l = "";

           auto it_l = map_l->find(col.toLower());
           if (it_l != map_l->end()) {
                tn_l = it_l->second.table;
           }

           s += QString("\n   %1 %2 %3\n       ").arg(col, colr, tn_l);

           QString ss = "";

           for (auto & id : set_id) {
                if (ss != "") ss += ",";
                ss += QString("%1").arg(id);
           }
           s += ss;
        }
    }

    if (s != "") {
        QMessageBox::warning(nullptr, "", s);
    }

    for (auto &tn: list_tab) {
        copy_temp_table(db, tn, add_id_old(tn));
    }

    ispr_nodes(db);
    ispr_externalCodes(db);  // Исправили источники в

    QMessageBox::warning(nullptr, "", "Готово!!");

    return 0;
}


bool isOUT(const QString & tn)
{
    if (tn.indexOf("_OUT", Qt::CaseInsensitive) != -1) {
        return true;
    }
    return false;
}

long sizeOfQ(QSqlDatabase &db, const QString & q);


bool isShape(QSqlDatabase & db, const QString& tn)
{
    QString q = QString("SELECT COLUMN_NAME FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_NAME = '%1' AND DATA_TYPE = 'geometry'").arg(tn);

    long l = sizeOfQ(db, q);

    return l > 0;
}
