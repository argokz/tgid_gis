#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
#include "InputDia.h"
#include "Input2.h"
#include "gidrView.h"

#include "geofile.h"
#include "win.h"

CString get_xml_path(const CString & tab1, const CString & tab2, const CString & name, const CString & defect, const CString & prefix)
{
    CString s;
    
     s.Format(
        "'%s' ="
"STUFF(\n"
"( SELECT concat(', ', rtt2.name) AS n FROM %s r2 \n"
"LEFT JOIN %s rt2 ON r2.id=rt2.objID\n"
"LEFT JOIN %s rtt2 ON rtt2.id=rt2.activityID\n"
"WHERE r2.id=%s.id \n"
"FOR XML PATH('')\n"
", TYPE\n"
").value('.', 'NVARCHAR(MAX)'), 1, 1, '')\n", name, defect, tab1, tab2, prefix);
        
    return s;
}


void copyVydGraph_new(CGraph2* graph1, CGraph2* graph2, bool is_po = false);
void sort_line_rs_new(CGraph2* graph, list<CNode2*>& list_sort, list<CLINE2*>& st_l, int fileID = -1);
bool isPSstrict_(CNode2* node)
{
    return false;
}




CString getLineMarkTable(CGraph2 *graph)
{

    bool is_vyd = false;

    CString s, ss = "", q = "";

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

    for (auto p : st_l) {
        CLINE2* l = p;

        CLINE2* lP = graph->find_line_nom(bline(l)->line.nomP);
        CLINE2* lO = graph->find_line_nom(bline(l)->line.nomO);

        if (lP || lO) {
            CNode2* n1 = where(l);
            CNode2* n2 = other(l);

            if (n1_old && (isPSstrict_(n1) || n1 != n2_old)) {
                for (int & it : list_ut_min) {
                    s.Format("(%d, %d)", nn++, it);
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
        s.Format("(%d, %d)", nn++, it);
        if (ss != "") ss += ",";
        ss += s;
        //        if (ins.ado_insert_int2(nn++, it)) ret = true;
    }






//    if (ss != "") {
//        q.Format("select id from (values %s) mark_lines(id)", ss);
//    }

    return ss;
}


CString createSelect(const CString& tn)
{
    CString q = "";

    q.Format("SELECT * FROM %s", tn);



    return q;

}

const map<CString, ForeignTable>* findLookup(CString d, CString n1);
ForeignTable findLookup(CString d, CString n1, CString n2);


FILE* fopen_tab(const char* tab, const char* tn, const char* ext);

bool parse_filtr(const CString &line, CString &tab1, CString &tab2, CString &title)
{
    std::vector<CString> match;
    int l = regex_match("\\$view_filtr\\$(.+)\\$(.+)\\$\\s+(.+)$", line, match);
    if (l > 0) {
        tab1 = match[1];
        tab2 = match[2];
        title = match[3];
        return true;
    }
    return false;
}


bool readTab(const CString &tn, std::list<CString> & lst, std::map<CString, CString> & map_names)
{
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
                CString ss = str, name = "";
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

    return ret;
}


bool is_onlyline(const char* tn);

CString getGeoSelect(Klassif *kls, CGraph2 *graph)
{
    CString tn = kls->nazv;
    CAdoFile *ado = kls->m_ado;

    CString join_mark = "";
    CString mark_line = "";
    
    if (is_onlyline(kls->nazv)) {
        mark_line = getLineMarkTable(graph);
    }

    log1(mark_line);

    if (mark_line == "") {
    join_mark.Format(

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
") TM ON TM.shape.STDistance(T0.shape.STPointN(1)) < 0.3\n"

);

    join_mark = "";
    }

    else {
    

    join_mark.Format(

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
"join (values %s) mark_lines(ord, id) on mark_lines.id=l.id\n"
"where l.removed=0\n"
") TM ON TM.shape.STDistance(T0.shape.STPointN(1)) < 0.3\n"

, mark_line);
    }


    CString par = "";
    CString par0 = "";
    CString joins = "";
    int n_join = 1;


//    const map<CString, ForeignTable> *map_lookup = findLookup(db, tn);

    if (par != "") par += ",\n";
    par += "T0.id";

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
        par0 += ",t.kod1 AS 'Код РС начального узла'";
        par0 += ",t.name1 AS 'Наименование начального узла'";
        par0 += ",t.kod2 AS 'Код РС конечного узла'";
        par0 += ",t.name2 AS 'Наименование конечного узла'";
    }


    int n = ado->NColsMySQL(tn);

    struct Filtr {
        CString tab1, tab2, title;
    };

    std::list<CString> lst;
    std::map<CString, CString> map_names;

    if (!readTab(tn, lst, map_names)) {
        for (int i = 0; i < n && i < 1000; i++) {
            MySQLColumns* col = ado->findMySQLColumnsOrd(tn, i + 1);
            if (col) {
                lst.push_back(col->strCOLUMN_NAME);
            }

            Filtr fltr;
//            CString tab1, tab2, title;

        }
    }
    else {
        lst.push_front("id");
    }

    std::set<CString> st;


    int n_dop = 2;

//    for (int i = 0; i < n && i < 1000; i++) {
//    MySQLColumns* col = ado->findMySQLColumnsOrd(tn, i + 1);
    for (auto & col1 : lst) {

        Filtr fltr;       
        CString tab1, tab2, title;
        if (parse_filtr(col1, tab1, tab2, title)) {
            if (par != "") par += ",\n";
            par += get_xml_path(tab1, tab2, title, tn, "T0");

            if (par0 != "") par0 += ",\n";
            par0 += "\"";
            par0 += title;
            par0 += "\"";

            continue;
        }

        
        MySQLColumns* col = ado->findMySQLColumns(tn, col1);



        if (col) {

            //            if (col->strDATA_TYPE != "geometry" && col->strDATA_TYPE != "ntext") {
            if (col->strDATA_TYPE == "geometry") continue;
            CString s_col = col->strCOLUMN_NAME;
            if (s_col == "id") continue;
            if (s_col == "lineID" && mark_line != "") continue;

//            s_col.Format("%s", col->strCOLUMN_NAME);

            const CColName* col_name = findColumnRusName(ado->m_schema, tn, s_col);

            ForeignTable ft = findLookup(ado->m_schema, tn, s_col);

            if (ft.table != "") {
                CString join;
                join.Format("\nLEFT JOIN [%s] T%d ON T%d.id=T0.[%s]", ft.table, n_join, n_join, s_col);
                joins += join;
                s_col.Format("T%d.%s", n_join, ft.name);
                n_join += 1;
            }
            else {
                s_col = "T0." + s_col;
            }

            if (par != "") par += ",\n";
            par += s_col;

            CString cname = col->strCOLUMN_NAME;

            if (col_name) {
                cname = col_name->name;
            }
            
            auto it = map_names.find(col->strCOLUMN_NAME);
            if (it != map_names.end()) {
                cname = it->second;
            }


            if (st.find(cname) != st.end()) {
                CString ss;
                ss.Format(" %d", n_dop++);
                cname += ss;
                st.insert(cname);
            }
            else {
                st.insert(cname);
            }


            CString s;
            s.Format(" AS '%s'", cname);
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
        par += "TM.shape.STDistance(T0.shape.STPointN(1)) as length,\nMIN(TM.shape.STDistance(T0.shape.STPointN(1))) OVER(PARTITION BY T0.id ) AS min_len";
    }

    CString dt = "1=1";

    CGidrView* pView = getView();

    if (pView && kls->label_date) {
        CString ds1 = pView->m_sezon_date1.Format(_T("%Y%m%d"));
        CString ds2 = pView->m_sezon_date2.Format(_T("%Y%m%d"));
        dt.Format(" (T0.[%s] BETWEEN '%s' AND '%s' OR T0.[%s] is NULL) ", kls->codlabel_date, ds1, ds2, kls->codlabel_date);
    }

    CString q;
    q.Format("SELECT %s\nFROM [%s] T0%s\nWHERE %s", par, tn, joins, dt);


    if (mark_line != "") {
        CString qq;
        qq.Format("SELECT\n%s\nFROM (\n%s\n) t\nwhere t.length = t.min_len\norder by t.ord", par0, q);
        q = qq;
    }

    log1(q);

    return q;
}
