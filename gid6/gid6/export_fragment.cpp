#include "stdafx.h"
#include "gid6.h"

#include "ado.h"
#include "Win.h"
#include "gidrView.h"
#include "TaskProgressDlg.h"

#include "zip/zip.h"
#include "zip/unzip.h"

#include "csv.h"

#include "InputDia.h"

#include "json.h"


const map<CString, ForeignTable>* findLookup(CString d, CString n1);

bool isDate(int typ)
{
    if (typ == adDate) return true;
    if (typ == adDBDate) return true;
    if (typ == adDBTime) return true;
    if (typ == adDBTimeStamp) return true;

    return false;
}

bool isStr(int typ)
{
    if (typ == adGUID) return true;
    if (typ == adDate) return true;
    if (typ == adDBDate) return true;
    if (typ == adDBTime) return true;
    if (typ == adDBTimeStamp) return true;
    if (typ == adBSTR) return true;
    if (typ == adChar) return true;
    if (typ == adVarChar) return true;
    if (typ == adLongVarChar) return true;
    if (typ == adWChar) return true;
    if (typ == adVarWChar) return true;
    if (typ == adLongVarWChar) return true;

    return false;
}

CString ispr(CString s, int typ)
{
    if (isDate(typ)) {
        int d, m, y;
        if (sscanf(s, "%d.%d.%d", &d, &m, &y) == 3) {
            s.Format("\"%04d-%02d-%02d\"", y, m, d);
            s.Format("\"%04d%02d%02d\"", y, m, d);
        }
        else {
            s = "";
        }
    }
    else if (isStr(typ)) {
//        s.Replace("\"", "\\\"");
        s.Replace("\"", "\"\"");
        s = "\"" + s + "\"";
    }
    return s;
}

CString ispr2(CString s, int typ)
{
    if (s == NULL_VALUE) return "NULL";

    if (isStr(typ)) {
        s.Replace("'", "''");
        s = "N'" + s + "'";
    }
    else if (s == "") {
        return "NULL";
    }
    return s;
}

bool export_table(FILE* f, CAdoFile* ado, const char* tn, const char* q, const char* fieldID, bool isNode, CGraph2* graph)
{
    fprintf(f, "-------------------------\n");
    fprintf(f, "%s\n", tn);

    if (!ado->openTable0(q)) return false;

    int nf = ado->NFlds();

    vector<CAdoField> columns(nf);

    for (int i = 0; i < nf; i++) {  
        columns[i] = ado->Field(i);
    }

    CString s = "";
    for (int i = 0; i < nf; i++) {
        if (columns[i].name != "shape" && columns[i].name != "removed" && columns[i].name != "idRemoved" && columns[i].name != "id_old") {
            if (s != "") s += ",";
            s += ispr(columns[i].name, adVarWChar);
        }
    }
    fprintf(f, "%s\n", s);

    int id_new = 1;

    while (!ado->isEOF()) {
        CString s = "";

        long id = ado->read_long("id");
        
        bool yes = true;

        if (fieldID[0] && graph) {
            yes = false;
            long id2 = ado->read_long(fieldID);
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
            for (int i = 0; i < nf; i++) {
                if (columns[i].name != "shape" && columns[i].name != "removed" && columns[i].name != "idRemoved" && columns[i].name != "id_old") {
                    if (s != "") s += ",";
                    CString v = ado->readStr(i);
                    s += ispr(v, columns[i].typ);
                }
            }
            fprintf(f, "%s\n", s);
        }

        ado->MoveNext();
    }
    ado->closeTable();

    return true;
}

bool export_table_unvisual(FILE* f, CAdoFile* ado, const char* tn, int fileID, CGraph2* graph)
{
    CString q;
    q.Format("SELECT * FROM [%s] WHERE fileID=%d", tn, fileID);
    return export_table(f, ado, tn, q, "", false, graph);
}

bool export_table_node(FILE* f, CAdoFile* ado, const char* tn, int fileID, CGraph2* graph)
{
    CString q;

    q.Format(
        "SELECT obj.* FROM [%s] obj\n"
        "JOIN nodes n ON n.id = obj.nodeID\n"
        "WHERE n.fileID = %d AND n.removed = 0", tn, fileID);

    return export_table(f, ado, tn, q, "nodeID", true, graph);
}

bool export_table_line(FILE* f, CAdoFile* ado, const char* tn, int fileID, CGraph2* graph)
{
    CString q;

    q.Format(
        "SELECT obj.* FROM [%s] obj\n"
        "JOIN linesobj l ON l.id = obj.lineID\n"
        "JOIN nodes n1 ON n1.id = l.nodeID1\n"
        "JOIN nodes n2 ON n2.id = l.nodeID2\n"
        "WHERE n1.fileID = %d AND n2.fileID = %d AND l.removed = 0 AND n1.removed = 0 AND n2.removed = 0", tn, fileID, fileID);

    return export_table(f, ado, tn, q, "lineID", false, graph);
}

bool export_tgid(CAdoFile* ado, CString fn, int fileID, CGraph2 *graph)
{
    FILE* f = fopen(fn, "w");
    
    if (graph) {
       graph->init_find_line_nom();
    }


    if (!f) {
        AfxMessageBox("Не могу открыть файл для записи", MB_OK|MB_ICONINFORMATION);
        return false;
    }

    CString q;


    fprintf(f, "=========================\n");
    fprintf(f, "Server: %s\n", ado->m_server);
    fprintf(f, "Database: %s\n", ado->m_schema);
    fprintf(f, "User: %s\n", ado->m_user);

    COleDateTime vv = COleDateTime::GetCurrentTime();
    CString date = vv.Format("%d-%m-%Y %H:%M");
    fprintf(f, "Date: %s\n", date);

    q.Format("SELECT * FROM heatSystem");
    export_table(f, ado, "heatSystem", q, "", false, graph);

    q.Format("SELECT * FROM fragments WHERE id=%d", fileID);
    export_table(f, ado, "fragments", q, "", false, graph);

    export_table_unvisual(f, ado, "externalCodes", fileID, graph);

    //    export_table_unvisual(f, ado, "CALCULATION", fileID, graph);
    export_table_unvisual(f, ado, "calcTemperatures", fileID, graph);
    export_table_unvisual(f, ado, "calculations", fileID, graph);
    export_table_unvisual(f, ado, "gvsLoadGraphs", fileID, graph);
    export_table_unvisual(f, ado, "specExpends", fileID, graph);

    export_table_unvisual(f, ado, "texts", fileID, graph);

    export_table_unvisual(f, ado, "varCoefficients", fileID, graph);

    //    export_table_node(f, ado, "sortNodesForUchastok", fileID);
    //    export_table_unvisual(f, ado, "sortLinesForUchastok", fileID);


    q.Format("SELECT * FROM nodes WHERE fileID=%d AND removed=0", fileID);
    export_table(f, ado, "nodes", q, "id", true, graph);

    q.Format(
"SELECT DISTINCT pss.* FROM pipeSections pss\n"
"JOIN heatPipeSections hps ON hps.pipeSectionID=pss.id\n"
"JOIN linesobj l ON l.id=hps.lineID\n"
"JOIN nodes n1 ON n1.id=l.nodeID1\n"
"WHERE n1.fileID=%d AND l.removed=0"
, fileID);

    export_table(f, ado, "pipeSections", q, "lineID", false, graph);

    q.Format(
        "SELECT l.* FROM linesobj l\n"
        "JOIN nodes n1 ON n1.id=l.nodeID1\n"
        "JOIN nodes n2 ON n2.id=l.nodeID2\n"
        "WHERE n1.fileID=%d AND n1.fileID=n2.fileID\n"
        "AND l.removed=0 AND n1.removed=0 AND n2.removed=0"
        "AND (n1.internalNodeID=n2.internalNodeID OR (n1.internalNodeID IS NULL AND n2.internalNodeID IS NULL))"
        
        , fileID);

    export_table(f, ado, "linesobj", q, "ID", false, graph);

    q.Format("SELECT zn.* FROM setPressNodes zn JOIN nodes n ON n.id=zn.nodeID WHERE n.fileID=%d AND n.removed=0", fileID);

    export_table(f, ado, "setPressNodes", q, "nodeID", true, graph);

    export_table_unvisual(f, ado, "directions", fileID, graph);
    export_table_node(f, ado, "deployedDirections", fileID, graph);
    export_table_node(f, ado, "connectNodes", fileID, graph);

    // Узел для связи

    export_table_node(f, ado, "WDOdevices", fileID, graph);
    export_table_node(f, ado, "buildingEntries", fileID, graph);
    export_table_node(f, ado, "generalizedConsumers", fileID, graph);
    export_table_node(f, ado, "heatChambers", fileID, graph);
    export_table_node(f, ado, "heatSources", fileID, graph);
    export_table_node(f, ado, "realConsumers", fileID, graph);
    export_table_node(f, ado, "refillNodes", fileID, graph);
    export_table_node(f, ado, "threeWayValves", fileID, graph);

    q.Format("SELECT tg.* FROM deployedTempGraphs tg JOIN heatSources hs ON hs.id=tg.hSourceID JOIN nodes n ON n.id=hs.nodeID WHERE n.fileID=%d", fileID);

    export_table(f, ado, "deployedTempGraphs", q, "", false, graph);

    // для ПТС
    export_table_node(f, ado, "internalNodes", fileID, graph);
    export_table_node(f, ado, "overgroundNodes", fileID, graph);
    export_table_node(f, ado, "pavilions", fileID, graph);
    export_table_node(f, ado, "pumpStations", fileID, graph);
    export_table_node(f, ado, "trps", fileID, graph);
    export_table_node(f, ado, "undergroundNodes", fileID, graph);
    export_table_node(f, ado, "uninstalledNodes", fileID, graph);

    export_table_line(f, ado, "airHeaters", fileID, graph);
    export_table_line(f, ado, "bypass", fileID, graph);
    export_table_line(f, ado, "consumptRegulators", fileID, graph);
    export_table_line(f, ado, "dampers", fileID, graph);
    export_table_line(f, ado, "diaphragms", fileID, graph);
    export_table_line(f, ado, "elevators", fileID, graph);
    export_table_line(f, ado, "heatExchangers", fileID, graph);
    export_table_line(f, ado, "heatPipeSections", fileID, graph);
    export_table_line(f, ado, "localHydroResistances2", fileID, graph);
    export_table_line(f, ado, "opresDeployed", fileID, graph);
    export_table_line(f, ado, "pressDropRegulators", fileID, graph);
    export_table_line(f, ado, "pressRegulators", fileID, graph);
    export_table_line(f, ado, "pumps", fileID, graph);
    export_table_line(f, ado, "regulArmatures", fileID, graph);
    export_table_line(f, ado, "reverseValves", fileID, graph);
    export_table_line(f, ado, "systemRadiators", fileID, graph);

    fclose(f);

    return true;
}

CString ValidFileName(CString s)
{
    s.Replace("<", "_");
    s.Replace(">", "_");
    s.Replace(":", "_");
    s.Replace("\"", "_");
    s.Replace("/", "_");
    s.Replace("\\", "_");
    s.Replace("|", "_");
    s.Replace("?", "_");
    s.Replace("*", "_");
    s.Replace(".", "_");

    return s;
}

void Run(CWnd* win, const char* cmd, const char* prtn, int id);


void CGidrView::OnExportFragment()
{
    int nFragments(CAdoFile * ado, CCxema * cxema, CString & fragment);

    CString fragment;

    long fileID = nFragments(m_cxema->m_ado, m_cxema, fragment);

    if (fileID <= 0) {
        AfxMessageBox("Для экспорта должен быть открыт только один фрагмент", MB_OK|MB_ICONINFORMATION);
        return;
    }

    CFileDialog fd(FALSE, _T("tgid"), ValidFileName(fragment) + ".tgid",
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        _TR("*.tgid|*.tgid|"), this);

    if (fd.DoModal() == IDOK) {
        CString fn = fd.GetPathName();

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

    CString cmd;

    cmd.Format("\"%s\" \"%s\\unite\\export_tgid.py\" \"%s\" -rdbms MsSql -server %s -database %s -port %d -user %s -password %s -fileID %d", 
        python_exe(), argpath(), fn, pDoc->m_IP, pDoc->m_bd_gid, pDoc->m_port, pDoc->m_user, pDoc->m_password, fileID);

    CInputDialog inD(AfxGetMainWnd(), "", "?", cmd);
    inD.DoModal();

    Run(this, cmd, argpath_2() + "protocol.txt", ID_END_PYTHON);




/*
        CString tmpn;

        tmpn.Format("%s\\tempfileforexport.tmp", getenv("TEMP"));

        if ( export_tgid(m_cxema->m_ado, tmpn, fileID, nullptr)) {
            HZIP hz = CreateZip(fn,0);
            CString tn = GetTitle(fn)+".txt";
            tn = "tgid.txt";
            ZipAdd(hz, tn, tmpn);
            CloseZip(hz);
            AfxMessageBox("Выполнено", MB_OK|MB_ICONINFORMATION);
        }
*/


    }

}

void CGidrView::OnExportFragmentVyd()
{
    int nFragments(CAdoFile * ado, CCxema * cxema, CString & fragment);

    CString fragment;

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
        CString fn = fd.GetPathName();

        CString tmpn;

        tmpn.Format("%s\\tempfileforexport.tmp", getenv("TEMP"));

        if ( export_tgid(m_cxema->m_ado, tmpn, fileID, m_cxema->m_graph)) {
            HZIP hz = CreateZip(fn,0);
            CString tn = GetTitle(fn)+".txt";
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

    CString path;

    if (!getDir(this, path)) return;

    if (path == "") {
        AfxMessageBox("Выберите каталог");
        return;
    }

    Percent2 pc;

    for (auto it : m_cxema->m_file_map) {
        pc.SetValue(i*100/n);
        i++;

        CString fn;
        
        int fileID = it.second->id;
        CString name = ValidFileName(it.second->name);

        fn.Format("%s\\%s.tgid", path, name);

        CString tmpn;

        tmpn.Format("%s\\tempfileforexport.tmp", getenv("TEMP"));

        if ( export_tgid(m_cxema->m_ado, tmpn, fileID, nullptr)) {
            HZIP hz = CreateZip(fn,0);
            CString tn = GetTitle(fn)+".txt";
            tn = "tgid.txt";
            ZipAdd(hz, tn, tmpn);
            CloseZip(hz);
        }
    }
}



static map<CString, map<long, long> > map_num;
static CString m_fragment_name = "";

static map<CString, vector<CString> > map_col;
static map<CString, map<int, vector<CString> > > map_val;
static int n_rows;
static int i_rows;


bool open_tgid(const char* fn)
{
    FILE* f = fopen(fn, "r");

    if (!f) return false;

    map_num.clear();

    map_col.clear();
    map_val.clear();

    n_rows = 0;
    i_rows = 0;

    int n_fragment = 1;
    vector<CString> columns;

    CString tn = "";
    CString cols = "";

    char* str, * line;

    int N = 100000;

    str = new char[N+1];

    while (!feof(f)) {
        line = str;
        if (fgetstr(line, N, f) == NULL) break;

        if (line[0] == '=') {
            n_fragment += 1;
            if (fgetstr(line, N, f) == NULL) break;
            if (fgetstr(line, N, f) == NULL) break;
            if (fgetstr(line, N, f) == NULL) break;
            if (fgetstr(line, N, f) == NULL) break;
        }
        else if (line[0] == '-') {
            if (fgetstr(line, N, f) == NULL) break;
            tn = line;

            if (fgetstr(line, N, f) == NULL) break;
            columns = csv_read(line);
            map_col[tn] = columns;
        }
        else {
            vector<CString> l;

            l = csv_read(line);

            if (l.size() > 0) {
                long id = atoi(l[0]);
                map_val[tn][id] = l;
                n_rows ++;
                if (tn.CompareNoCase("nodes") == 0) n_rows ++;
            }
        }
    }
    delete [] str;

    fclose(f);

    return true;
}

static map<int, int> map_ist;


int open_table(Percent2 *pc, CAdoFile* ado, CString tn, int check_column = 0)
{
    CString tnl = tn;
    tnl.MakeLower();

    int id_new = -1;
//    const map<CString, ForeignTable>* map_l = findLookup(ado->m_schema, tn);
    const map<CString, ForeignTable>* map_l1 = findLookup("gid", tn);
    const map<CString, ForeignTable>* map_l = findLookup(ado->m_schema, tn);

    auto col_it = map_col.find(tn);

    if (col_it == map_col.end()) return 0;

    CString cols = "";
    for (auto c : col_it->second) {
        CString ss = c;
        if (ss == "" || ss[0] == '-') {
            return 0;
        }
        if (c != "id" && c != "shape" && !(tn.CompareNoCase("externalCodes") == 0 && c.CompareNoCase("belongMagistral") == 0) ) {
            if (cols != "")  cols += ",";
            cols += ("[" + c + "]");
        }
    }

    int n_internalNodeID = -1;
    int n_nodeID1 = -1;
    int n_nodeID2 = -1;
    int n_lineID = -1;

    if (check_column && tn.CompareNoCase("nodes") == 0) {
        int nf = col_it->second.size();

        for (int i = 0; i < nf; i++) {
            CString col = col_it->second[i];

            if (col.CompareNoCase("internalNodeID") == 0) {
                n_internalNodeID = i;
            }
            else if (col.CompareNoCase("nodeID1") == 0) {
                n_nodeID1 = i;
            }
            else if (col.CompareNoCase("nodeID2") == 0) {
                n_nodeID2 = i;
            }
            else if (col.CompareNoCase("lineID") == 0) {
                n_lineID = i;
            }
        }
    }

    auto val_it = map_val.find(tn);
    if  (val_it == map_val.end()) return 0;

    for (auto it : val_it->second) {
        if (pc) {
            pc->SetValue(i_rows*100/n_rows);
            i_rows ++;
        }


        vector<CString> l = it.second;
        int nf = l.size();

        if (check_column && tn.CompareNoCase("nodes") == 0 && n_internalNodeID >= 0) {
            CString internalNodeID = l[n_internalNodeID];
            if (check_column == 1 && internalNodeID != NULL_VALUE || check_column == 2 && internalNodeID == NULL_VALUE) {
                continue;
            }

            if (check_column == 2) {  // Проверяем, что internalNodeID есть
                bool yes = false;
                auto it2 = map_num.find("nodes");
                if (it2 != map_num.end()) {
                    auto it3 = it2->second.find(atoi(internalNodeID));
                    if (it3 != it2->second.end()) {
                        yes = true;
                    }
                }
                if (!yes) {
                    continue;
                }
            }
        }

        if (tn.CompareNoCase("linesobj") == 0 && n_nodeID1 >= 0 && n_nodeID2 >= 0) {
            CString nodeID1 = l[n_nodeID1];
            CString nodeID2 = l[n_nodeID2];

            auto it2 = map_num.find(tn);
            if (it2 != map_num.end()) {
                {
                    auto it3 = it2->second.find(atoi(nodeID1));
                    if (it3 == it2->second.end()) {
                        continue;
                    }
                }
                {
                    auto it4 = it2->second.find(atoi(nodeID2));
                    if (it4 == it2->second.end()) {
                        continue;
                    }
                }
            }
        }

        if (n_lineID >= 0) {
            CString lineID = l[n_lineID];

            auto it2 = map_num.find(tn);
            if (it2 != map_num.end()) {
                auto it3 = it2->second.find(atoi(lineID));
                if (it3 == it2->second.end()) {
                    continue;
                }
            }
        }


        long id = atoi(l[0]);

        CString s = "";


        for (int i = 0; i < nf; i++) {
            CString col = col_it->second[i];
            CString v = l[i];

            if (col.CompareNoCase("nodeID1") == 0 || col.CompareNoCase("nodeID2") == 0) {
            }

            if (col != "id" && col != "shape" && !(tn.CompareNoCase("externalCodes") == 0 && col.CompareNoCase("belongMagistral") == 0) ) {

                if (map_l && v != NULL_VALUE) {
                    CString col_l = col;
                    col_l.MakeLower();
                    auto it = map_l->find(col_l);


                    if (it != map_l->end()) {
                        ForeignTable ft = it->second;

                        CString tnl1 = ft.table;
                        tnl1.MakeLower();
                        auto it2 = map_num.find(tnl1);

                        int n = atoi(v);

                        if (col.CompareNoCase("nodeID1") == 0 || col.CompareNoCase("nodeID2") == 0 || col.CompareNoCase("nodeID") == 0
                             || col.CompareNoCase("lineID") == 0 || col.CompareNoCase("internalNodeID") == 0
                             || col.CompareNoCase("varCoeffID") == 0
                             || col.CompareNoCase("varCoeffIDflow") == 0 || col.CompareNoCase("varCoeffIDret") == 0
                             || col.CompareNoCase("heatSourceID") == 0 || col.CompareNoCase("exploitRegionID") == 0
//                             || ( col.Find("ID") == col.GetLength()-2 && col != "graphTypeID")

//                             || col == "" || col == ""
                             
                             ) {
                            v = NULL_VALUE;
                        }

                        if (it2 != map_num.end()) {
                            v = NULL_VALUE;
                            auto it3 = it2->second.find(n);
                            if (it3 != it2->second.end()) {
                                v.Format("%d", it3->second);
                            }
                        }
                    }
                }

                if (tn.CompareNoCase("pressRegulators") == 0 && col.CompareNoCase("h") == 0 && v == NULL_VALUE) {
                    v = "0";
                }

                if (tn.CompareNoCase("externalCodes") == 0 && col.CompareNoCase("heatSourceID") == 0) {
                    if (v != "" && v != NULL_VALUE) {
                        int heatSourceID = atoi(v);
                        map_ist[id] = heatSourceID;
                        v = "";
                    }
                }

                if (tn.CompareNoCase("externalCodes") == 0 && col.CompareNoCase("belongMagistral") == 0) {
                    v = "";
                }

                if (tn.CompareNoCase("fragments") == 0 && col.CompareNoCase("name") == 0) {
                    COleDateTime vv = COleDateTime::GetCurrentTime();
                    CString date = vv.Format("%d-%m-%Y %H:%M");
                    CString ss;

                    ss.Format(" импортирован %s", date);

                    if (v.GetLength()+ss.GetLength() > 50) {
                        v = v.Left(50-ss.GetLength());
                    }
                    
                    v += ss;

                    if (v.GetLength() > 80)  v = v.Left(80);

                    m_fragment_name = v;
                }

                if (s != "")  s += ",";
                s += ispr2(v, adVarWChar);
            }
        }

        CString ins;

        ins.Format("INSERT INTO [%s] (%s) VALUES (%s)", tn, cols, s);

        long affected;
        id_new = ado->ExecuteInsert(ins, &affected);

        if (id_new <= 0) {
//            CInputDialog inD(AfxGetMainWnd(), "Ошибка", ado->getError(), ins);
//            inD.DoModal();
            AfxMessageBox(ado->getError(), MB_OK|MB_ICONINFORMATION);
            break;
        }
        map_num[tnl][id] = id_new;
    }

    return id_new;
}

int import_tgid(CAdoFile *ado, CString fn)
{
    Percent2 pc;

    map_ist.clear();

    open_tgid(fn);

    if (!ado->StartTransaction()) return -1;

    int fileID = open_table(&pc, ado, "fragments");

    if (fileID == 0) return 0;

    open_table(&pc, ado, "externalCodes");
    open_table(&pc, ado, "calcTemperatures");
    open_table(&pc, ado, "calculations");
    open_table(&pc, ado, "gvsLoadGraphs");
    open_table(&pc, ado, "specExpends");
    open_table(&pc, ado, "varCoefficients");
    open_table(&pc, ado, "directions");

    //open_table(&pc, ado, "sortLinesForUchastok");
    //open_table(&pc, ado, "sortNodesForUchastok");

    open_table(&pc, ado, "nodes", 1);
    open_table(&pc, ado, "nodes", 2);

//    open_table(&pc, ado, "pipeSections");

    open_table(&pc, ado, "linesobj");

    open_table(&pc, ado, "deployedDirections");
//    open_table(&pc, ado, "deployedTempGraphs");
    open_table(&pc, ado, "setPressNodes");
    open_table(&pc, ado, "selectNodes");

    open_table(&pc, ado, "texts");

    // Узел дл связи

    open_table(&pc, ado, "connectNodes");
    open_table(&pc, ado, "WDOdevices");
    open_table(&pc, ado, "buildingEntries");
    open_table(&pc, ado, "generalizedConsumers");
    open_table(&pc, ado, "heatChambers");
    open_table(&pc, ado, "heatSources");
    open_table(&pc, ado, "realConsumers");
    open_table(&pc, ado, "refillNodes");
    open_table(&pc, ado, "threeWayValves");

    // для ПТС;
    open_table(&pc, ado, "internalNodes");
    open_table(&pc, ado, "overgroundNodes");
    open_table(&pc, ado, "pavilions");
    open_table(&pc, ado, "pumpStations");
    open_table(&pc, ado, "trps");
    open_table(&pc, ado, "undergroundNodes");
    open_table(&pc, ado, "uninstalledNodes");

    open_table(&pc, ado, "airHeaters");
    open_table(&pc, ado, "bypass");
    open_table(&pc, ado, "consumptRegulators");
    open_table(&pc, ado, "dampers");
    open_table(&pc, ado, "diaphragms");
    open_table(&pc, ado, "elevators");
    open_table(&pc, ado, "heatExchangers");
    open_table(&pc, ado, "heatPipeSections");
    open_table(&pc, ado, "localHydroResistances2");
    open_table(&pc, ado, "opresDeployed");
    open_table(&pc, ado, "pressDropRegulators");
    open_table(&pc, ado, "pressRegulators");
    open_table(&pc, ado, "pumps");
    open_table(&pc, ado, "regulArmatures");
    open_table(&pc, ado, "reverseValves");
    open_table(&pc, ado, "systemRadiators");

    open_table(&pc, ado, "deployedTempGraphs");

    CString qqq = "";

    for (auto& it : map_ist) {
        int id1 = it.first;
        int id2 = it.second;

        bool b1 = false, b2 = false;

        {
            auto it1 = map_num.find("externalcodes");
            if (it1 != map_num.end()) {
                auto it2 = it1->second.find(id1);
                if (it2 != it1->second.end()) {
                    id1 = it2->second;
                    b1 = true;
                }
            }
        }

        {
            auto it1 = map_num.find("heatsources");

            if (it1 != map_num.end()) {
                auto it2 = it1->second.find(id2);
                if (it2 != it1->second.end()) {
                    id2 = it2->second;
                    b2 = true;
                }
            }
        }

        if (b1 && b2) {
            CString qq;
            qq.Format("SELECT %d AS id1, %d AS id2", id1, id2);
            if (qqq != "") qqq += "\nUNION\n";
            qqq += qq;
        }
    }

    if (qqq != "") {
        CString q;
        q.Format("UPDATE ec SET heatSourceID = TBL.id2 FROM externalCodes ec JOIN (%s) TBL ON TBL.id1 = ec.id", qqq);
        if (!ado->Execute(q)) {
            AfxMessageBox(_TR("Ошибка 1"), MB_OK | MB_ICONINFORMATION);
        }
    }

    ado->FinishTransaction();

    return fileID;
}

static map<int, CString> map_tgid;

int CGidrView::import_tgid0(CAdoFile *ado, CString fn)
{
    CString q;

//    q.Format("DISABLE TRIGGER ALL ON DATABASE;");
//    ado->Execute(q);

    CString tmpn;

    tmpn.Format("%s\\tempfileforexport.tmp", getenv("TEMP"));

    HZIP hz = OpenZip(fn,0);
    if (!hz) {
        AfxMessageBox("Ошибка! Не могу открыть файл.", MB_OK|MB_ICONINFORMATION);
        return -1;
    }

    ZIPENTRY ze; 
    GetZipItem(hz, -1, &ze); 
    int numitems=ze.index;

    for (int i=0; i < numitems; i++) { 
        GetZipItem(hz,i,&ze);
//        UnzipItem(hz, i, ze.name);
        UnzipItem(hz, i, tmpn);
        break;
    }
    CloseZip(hz);

    int fileID = import_tgid(ado, tmpn);
    if (fileID <= 0) {
        AfxMessageBox("Ошибка! Не могу записать данные в БД\n"+ado->getError(), MB_OK|MB_ICONINFORMATION);
        return -1;
    }

//    return fileID;

    q.Format("UPDATE  l SET shape = NULL FROM linesobj l JOIN nodes n1 ON n1.id=l.nodeID1 WHERE n1.fileID=%d", fileID);
    ado->Execute(q);

    q.Format("UPDATE  l\n"
             "SET\n"
             "l.shape = geometry::STGeomFromText(CONCAT('LINESTRING(', n1.x/100, ' ',  -n1.y/100, ',',  "
             "dbo.str_coords(REPLACE(CONVERT(VARCHAR(MAX), l.coords), ',', ' ')),"
             "n2.x/100, ' ', -n2.y/100, ')'), 0).MakeValid()\n"

             "FROM linesobj l\n"
             "JOIN nodes n1 ON n1.id=l.nodeID1\n"
             "JOIN nodes n2 ON n2.id=l.nodeID2\n"
             "WHERE n1.internalNodeID IS NULL \n"
             "  AND (n1.x <> n2.x OR n1.y <> n2.y) \n"
             "  AND (n1.x <> 0 OR n1.y <> 0) \n"
             "  AND (n2.x <> 0 OR n2.y <> 0) \n"
             "  AND n1.fileID=%d\n"
             "  AND l.removed=0\n", fileID);

    if (!ado->Execute(q)) {
        AfxMessageBox(_TR("Ошибка 2"), MB_OK | MB_ICONINFORMATION);
        return -1;
    }
//    openFragment(fileID, m_fragment_name);

    return fileID;
}

CString readQ(CAdoFile* ado, const char* tn, const char* tn2, int fileID);
CString readQ(CAdoFile* ado, const char* tn, const char* tn2, const char* fileID);

void CGidrView::OnEndImport()
{
    CString outName;
    outName.Format("%s\\out.txt", getenv("TMP"));
    
    CString par = m_cxema->m_par;

    CAdoFile* ado = m_cxema->m_ado;


    FILE *f = fopen(outName, "r");
    if (f) {
        while(!feof(f)) {
            int id;
            if (fscanf(f, "%d", &id) == 1) {
                CString ss;
                ss.Format("%d", id);
                if (par != "") par += ",";
                par += ss;

                CGidFile* gid_file = new CGidFile;
                gid_file->id = id;
                gid_file->name = "!!";
                gid_file->m_schema = ado->m_schema;
                m_cxema->m_file_map[gid_file->id] = gid_file;
            }
        }
        fclose(f);
    }


    m_cxema->m_par = par;

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
    m_cxema->openGid(m_cxema->m_ado, m_cxema->m_par, pDoc->m_user_gid);
    m_cxema->openFragments(m_cxema->m_ado);

    m_cxema->initGidTree();
    Invalidate();


    AfxMessageBox(_TR("Выполнено"), MB_OK | MB_ICONINFORMATION);
}

void CGidrView::OnEndExport()
{
    AfxMessageBox(_TR("Выполнено"), MB_OK | MB_ICONINFORMATION);
}

void CGidrView::OnEndUnite()
{
//    AfxMessageBox(_TR("Выполнено"), MB_OK | MB_ICONINFORMATION);
    OnEndImport();
}


void CGidrView::OnImportFragment()
{
    map_tgid.clear();

    if (!isEditGid()) {
        AfxMessageBox(_TR("Редактирование запрещено!"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    CFileDialog fd(TRUE, _T("tgid"), NULL, OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT,
        _T("*.tgid|*.tgid|"), AfxGetMainWnd());


    DWORD MAXFILE = 2562*10; //2562 is the max
    fd.m_ofn.nMaxFile = MAXFILE;
    char* pc = new char[MAXFILE];
    fd.m_ofn.lpstrFile = pc;
    fd.m_ofn.lpstrFile[0] = NULL;

    if (fd.DoModal() != IDOK) {
        return;
    }
    else {
        POSITION pos = fd.GetStartPosition();

        CString batName, outName;
        batName.Format("%s\\sety.bat", getenv("TMP"));
        outName.Format("%s\\out.txt", getenv("TMP"));

        FILE* f = fopen(batName, "w");

        fprintf(f, "@echo off\n");
        fprintf(f, "@del \"%s\"\n", outName);

        if (f) {
            while (pos) {

                Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

                CString cmd;

                CString fn = fd.GetNextPathName(pos);

                cmd.Format("\"%s\" \"%s\\unite\\import_tgid.py\" \"%s\" -rdbms MsSql -server %s -database %s -port %d -user %s -password %s -out_file \"%s\"",
                    python_exe(), argpath(), fn, pDoc->m_IP, pDoc->m_bd_gid, pDoc->m_port, pDoc->m_user, pDoc->m_password, outName);

                cmd.CharToOemA();

                fprintf(f, "@%s\n", cmd);

                /*
                            int fileID = import_tgid0(m_cxema->m_ado, fd.GetNextPathName(pos));
                            map_tgid[fileID] = m_fragment_name;


                            for (int i = 1; i <= 5; i++) {
                                CString fn;
                                fn.Format("pipeSections\\copy_pipeSections_by_fragment%d", i);
                                CString qq = readQ(m_cxema->m_ado, fn, "", fileID);
                                if (!m_cxema->m_ado->Execute(qq)) {
                //                    AfxMessageBox(_TR("Ошибка 3"), MB_OK | MB_ICONINFORMATION);
                //                    return;
                                }
                            }
                */
            }
            fclose(f);
        }

        Run(this, batName, argpath_2() + "protocol.txt", ID_END_IMPORT);
    }
#if 0


    if (AfxMessageBox("Обновить индексы после выполнения импорта?", MB_YESNO) == IDYES) {
        CString tn1 = argpath() + "\\sql3\\update_index.sql";
        CString q = readFile(tn1);
        q.Replace("$BAZA$", m_cxema->m_ado->m_schema);

        if (m_cxema->m_ado->Execute(q)) {
    //        AfxMessageBox(_TR("Готово"), MB_OK | MB_ICONINFORMATION);
        }           
        else {
            AfxMessageBox(_TR("Ошибка 2"), MB_OK | MB_ICONINFORMATION);
        }
    }

//    for (auto &it : map_tgid) {
//        openFragment(it.first, it.second);
//    }


    CString par = "";
    for (auto &it : map_tgid) {
        CString ss;
        ss.Format("%d", it.first);
            
        if (par != "") par += ",";
        par +=  ss;
//        openFragment(it.first, it.second);
    }

    m_cxema->m_par = par;

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
    m_cxema->openGid(m_cxema->m_ado, m_cxema->m_par, pDoc->m_user_gid);
    m_cxema->initGidTree();

#endif  


#if 0
    if (fd.DoModal() == IDOK) {
        CAdoFile* ado = m_cxema->m_ado;

        CString fn = fd.GetPathName();

        CString tmpn;

        tmpn.Format("%s\\tempfileforexport.tmp", getenv("TEMP"));

        HZIP hz = OpenZip(fn,0);
        if (!hz) {
            AfxMessageBox("Ошибка! Не могу открыть файл.", MB_OK|MB_ICONINFORMATION);
            return;
        }

        ZIPENTRY ze; 
        GetZipItem(hz, -1, &ze); 
        int numitems=ze.index;

        for (int i=0; i < numitems; i++) { 
            GetZipItem(hz,i,&ze);
    //        UnzipItem(hz, i, ze.name);
            UnzipItem(hz, i, tmpn);
            break;
        }
        CloseZip(hz);

        int fileID = import_tgid(ado, tmpn);
        if (fileID <= 0) {
            AfxMessageBox("Ошибка! Не могу записать данные в БД\n"+ado->getError(), MB_OK|MB_ICONINFORMATION);
            return;
        }


        CString q;

        q.Format("UPDATE  l SET shape = NULL FROM linesobj l JOIN nodes n1 ON n1.id=l.nodeID1 WHERE n1.fileID=%d", fileID);
        ado->Execute(q);

        q.Format("UPDATE  l\n"
                 "SET\n"
                 "l.shape = geometry::STGeomFromText(CONCAT('LINESTRING(', n1.x/100, ' ',  -n1.y/100, ',',  dbo.str_coords(REPLACE(l.coords, ',', ' ')),n2.x/100, ' ', -n2.y/100, ')'), 0).MakeValid()\n"

                 "FROM linesobj l\n"
                 "JOIN nodes n1 ON n1.id=l.nodeID1\n"
                 "JOIN nodes n2 ON n2.id=l.nodeID2\n"
                 "WHERE n1.internalNodeID IS NULL \n"
                 "  AND (n1.x <> n2.x OR n1.y <> n2.y) \n"
                 "  AND (n1.x <> 0 OR n1.y <> 0) \n"
                 "  AND (n2.x <> 0 OR n2.y <> 0) \n"
                 "  AND n1.fileID=%d\n"
                 "  AND l.removed=0\n", fileID);

        ado->Execute(q);
        openFragment(fileID, m_fragment_name);
        AfxMessageBox(_TR("Выполнен импорт фрагмента.\nУстановите в объектах фрагмента Расчетная схема Источник тепла"), MB_OK|MB_ICONINFORMATION);
#endif
        //    }
}

CString getPar(set<int>& v);


void CGidrView::reopenFragments()
{
    CAdoFile* ado = m_cxema->m_ado;

    m_cxema->openFragments(ado);

    set<int> v;

    CString sect;
    sect.Format("DB\\%s", ado->m_schema);

    for (auto it : m_cxema->map_fragments) {
        bool q = false;

//        AfxMessageBox(it.second.m_name, MB_OK|MB_ICONINFORMATION);

        bool on = AfxGetApp()->GetProfileInt(sect, it.second.m_name, 1);

        if (on) {
            for (auto it2 : m_cxema->m_file_map) {
                if (it.first == it2.first) {
                    q = true;
                    break;
                }
            }
            if (!q) {
                v.insert(it.second.m_id);

                CGidFile* gid_file = new CGidFile;
                gid_file->id = it.second.m_id;
                gid_file->name = it.second.m_name;
                gid_file->m_schema = ado->m_schema;
                m_cxema->m_file_map[gid_file->id] = gid_file;

            }
        }
    }


    m_cxema->m_par = getPar(v);

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

    m_cxema->openGid(ado, m_cxema->m_par, pDoc->m_user_gid);

    m_cxema->initGidTree();

    CFRect rect = m_cxema->m_graph->getBorder(m_parent_id);
    SetBorder(rect);

    Invalidate();
    
}





void CGidrView::OnImportFragmentMDB()
{
    if (!isEditGid()) {
        AfxMessageBox(_TR("Редактирование запрещено!"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    CFileDialog fd(TRUE, _T("mdb"), NULL, OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT,
        _T("*.mdb|*.mdb|"), AfxGetMainWnd());


    DWORD MAXFILE = 2562*10; //2562 is the max
    fd.m_ofn.nMaxFile = MAXFILE;
    char* pc = new char[MAXFILE];
    fd.m_ofn.lpstrFile = pc;
    fd.m_ofn.lpstrFile[0] = NULL;

    if (fd.DoModal() == IDOK) {
        POSITION pos = fd.GetStartPosition();

        CString fn;
        CString path_temp = getenv("TMP");

        fn.Format("%s\\import_mdb\\config.json", path_temp);
        mkdir_for_file(fn);

        int type_of_net;
        CString server, user, password;
        int port;

        getAdoConnectParam(type_of_net, server, port, user, password);

        void write_system_coord(JSON &json);

        FILE* f = fopen(fn, "w");

        if (f) {
            JSON json;
            json.add("{");
            json.add("\"mssql\": {");
            json.add("host", server);
            json.add("user", user);
            json.add("password", password);
            json.add("port", port);
            json.add("db", getGidAdoName());

            json.add("}");

            json.add("\"coords\": {");
            write_system_coord(json);
            json.add("}");


            json.add("\"mdbs\": [");

            while (pos) {
                CString fn = fd.GetNextPathName(pos);
//                fn.Replace("\\", "/");
//                fn.Replace("\"", "\"\"");
//                fn = CString("\"") + fn + "\"";
                json.add1(fn);
            }

            json.add("]");

            json.add("}");
            fputs(json.str(), f);
            fclose(f);

            CString str, path;


            str.Format("\"%simport_mdb\\import_mdb.py\"", argpath());

            path.Format("%simport_mdb\\", argpath());

//            AfxMessageBox(path, MB_OK|MB_ICONINFORMATION);

            ChDir cd(path);


//            AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);

            int ret = _spawnlp(_P_WAIT, python_exe(), "python", str, NULL);
//            int ret = _execlp("cmd", "cmd", NULL);
//            int ret = _spawnlp(_P_WAIT , "cmd", "cmd", NULL);

//            HINSTANCE hRun = ShellExecute(AfxGetMainWnd()->GetSafeHwnd(), "open", "python", str, path, SW_SHOW);


            if (ret < 0) {
                if (errno == ENOENT) {
                    AfxMessageBox("Для конвертирования mdb-файлов должен быть установлен python", MB_OK|MB_ICONINFORMATION);
                }
                else {
                    AfxMessageBox("Ошибка запуска программы для конвертирования mdb-файлов", MB_OK|MB_ICONINFORMATION);
                }
        //        s.Format("%d", errno);
        //        AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);
            }
            else {
                    AfxMessageBox("Конвертирование закончено", MB_OK|MB_ICONINFORMATION);
//                    m_cxema->openFragments(m_cxema->m_ado);
                    reopenFragments();
            }
        }
    }
}

bool CGidrView::importFragmentMag(const char *fn)
{
    return true;
}


void CGidrView::OnImportFragmentMag()
{
    if (!isEditGid()) {
        AfxMessageBox(_TR("Редактирование запрещено!"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    CFileDialog fd(TRUE, _T("mag"), NULL, OFN_HIDEREADONLY,
        _T("*.mag|*.mag|"), AfxGetMainWnd());


    if (fd.DoModal() == IDOK) {

        CString mag = fd.GetPathName();

        CFileDialog fd2(TRUE, _T("cxe"), NULL, OFN_HIDEREADONLY,
            _T("*.cxe|*.cxe|"), AfxGetMainWnd());

        CString cxe = "";

        if (fd2.DoModal() == IDOK) {
            cxe = fd2.GetPathName();
        }

       CString fn;
        CString path_temp = getenv("TMP");

        fn.Format("%s\\import_mdb\\config.json", path_temp);
        mkdir_for_file(fn);

        int type_of_net;
        CString server, user, password;
        int port;

        CString tmpn;




        tmpn.Format("%s\\tmp12345.mdb", getenv("TEMP"));

        CString qq;
        CString cmd;
        cmd.Format("%sconv\\gidr3cnv.exe", argpath());


        if (cxe == "") {
            qq.Format("\"%sconv\\gidr3cnv.exe\" \"%s\" \"%s\"", argpath(), tmpn, mag);
        }
        else {
            qq.Format("\"%sconv\\gidr3cnv.exe\" \"%s\" \"%s\" \"%s\"", argpath(), tmpn, mag, cxe);
        }
        if (cxe == "") {
            qq.Format("\"%s\" \"%s\"", tmpn, mag);
        }
        else {
            qq.Format("\"%s\" \"%s\" \"%s\"", tmpn, mag, cxe);
        }
                   


///        int ret = system(qq);
        int ret = _spawnlp(_P_WAIT, cmd, "gidr3cnv.exe", qq, NULL);

        if (ret != 0) {
            AfxMessageBox(qq+"\n"+strerror(errno), MB_OK | MB_ICONINFORMATION);
            return;
        }


        getAdoConnectParam(type_of_net, server, port, user, password);

        void write_system_coord(JSON &json);

        FILE* f = fopen(fn, "w");

        if (f) {

            JSON json;
            json.add("{");
            json.add("\"mssql\": {");
            json.add("host", server);
            json.add("user", user);
            json.add("password", password);
            json.add("port", port);
            json.add("db", getGidAdoName());

            json.add("}");

            json.add("\"coords\": {");
            write_system_coord(json);
            json.add("}");


            json.add("\"mdbs\": [");


            CString fn = tmpn+".mdb"; // fd.GetNextPathName(pos);
            json.add1(fn);


            json.add("]");


            json.add("\"fragments\": [");

            //CString fn = tmpn; // fd.GetNextPathName(pos);
            json.add1(fd.GetFileTitle());

            json.add("]");


            json.add("}");
            fputs(json.str(), f);
            fclose(f);

            CString str, path;


            str.Format("\"%simport_mdb\\import_mdb.py\"", argpath());

            path.Format("%simport_mdb\\", argpath());

//            AfxMessageBox(path, MB_OK|MB_ICONINFORMATION);

            ChDir cd(path);


//            AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);

            int ret = _spawnlp(_P_WAIT, python_exe(), "python", str, NULL);
//            int ret = _execlp("cmd", "cmd", NULL);
//            int ret = _spawnlp(_P_WAIT , "cmd", "cmd", NULL);

//            HINSTANCE hRun = ShellExecute(AfxGetMainWnd()->GetSafeHwnd(), "open", "python", str, path, SW_SHOW);


            if (ret < 0) {
                if (errno == ENOENT) {
                    AfxMessageBox("Для конвертирования mdb-файлов должен быть установлен python", MB_OK|MB_ICONINFORMATION);
                }
                else {
                    AfxMessageBox("Ошибка запуска программы для конвертирования mdb-файлов", MB_OK|MB_ICONINFORMATION);
                }
        //        s.Format("%d", errno);
        //        AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);
            }
            else {
                    AfxMessageBox("Конвертирование закончено", MB_OK|MB_ICONINFORMATION);
//                    m_cxema->openFragments(m_cxema->m_ado);
                    reopenFragments();
            }
        }
    }
}



void CGidrView::OnImportPTS()
{
}


bool isOUT(const CString & tn)
{
    if (tn.Find("_OUT") != -1) {
        return true;
    }
    return false;
}

long sizeOfQ(CAdoFile* ado, const char* qq);


bool isShape(CAdoFile *ado, const CString& tn)
{
    CString q;

    q.Format("SELECT COLUMN_NAME FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_NAME = '%s' AND DATA_TYPE = 'geometry'", tn);

    long l = sizeOfQ(ado, q);


    return l > 0;
}



void CGidrView::OnExportPTS()
{
//    int nFragments(CAdoFile * ado, CCxema * cxema, CString & fragment);

    CString fragment;

    CAdoFile* ado = m_cxema->m_ado;


    list<CString> list_tn;

    CString q = "SELECT * FROM SYSOBJECTS WHERE xtype = 'U'";

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            CString tn = ado->readStr("name");

                list_tn.push_back(tn);
            ado->MoveNext();
        }

        ado->closeTable();
    }

    FILE* f = fopen("C:\\vc_projects\\qqq.txt", "w");

    if (f) {
        for (auto it : list_tn) {
            if (isOUT(it)) {
                fprintf(f, "= OUT ");
            }
            if (isShape(ado, it)) {
                fprintf(f, "= GEO ");
            }
            fprintf(f, "%s\n", it);
        }
        fclose(f);
    }





/*

    long fileID = nFragments(m_cxema->m_ado, m_cxema, fragment);

    if (fileID <= 0) {
        AfxMessageBox("Для экспорта должен быть открыт только один фрагмент", MB_OK|MB_ICONINFORMATION);
        return;
    }

    CFileDialog fd(FALSE, _T("tgid"), ValidFileName(fragment) + ".tgid",
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        _TR("*.tgid|*.tgid|"), this);

    if (fd.DoModal() == IDOK) {
        CString fn = fd.GetPathName();

        CString tmpn;

        tmpn.Format("%s\\tempfileforexport.tmp", getenv("TEMP"));

        if ( export_tgid(m_cxema->m_ado, tmpn, fileID, nullptr)) {
            HZIP hz = CreateZip(fn,0);
            CString tn = GetTitle(fn)+".txt";
            tn = "tgid.txt";
            ZipAdd(hz, tn, tmpn);
            CloseZip(hz);
            AfxMessageBox("Выполнено", MB_OK|MB_ICONINFORMATION);
        }
    }
*/
}
