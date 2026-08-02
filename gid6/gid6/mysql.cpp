// Cxema.cpp: implementation of the CCxema class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gid6.h"
#include "Cxema.h"

#include "graph2.h"
#include "ado.h"
#include "gid6doc.h"
#include "gidrview.h"

#include "MainFrm.h"
#include "win.h"

#include "MdbList.h"
#include "TaskProgressDlg.h"

//static map<long, CGidFile*> map_gid;
//extern CLeftSheet *pm_cSheet;


//const double coef_xy = 3.048;
//const double coef_xy = 3.;
const double coef_xy = 1.;

bool is_column_in_table(CAdoFile* ado, const char* tn, const char* col)
{
    CString q;
    q.Format("SELECT 1 FROM sys.columns WHERE Name = N'%s' AND Object_ID = Object_ID(N'%s')", col, tn);
    bool yes = false;

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            yes = ado->read_long(0);
        }
        ado->closeTable();
    }
    return yes;
}



bool is_bezelevator(int n1, int n2)
{
    if (n1 == 1) {
        if (n2 == 1 || n2 == 2) {
            return true;
        }
    }
    else {
        if (n2 == 1 || n2 == 2 || n2 == 3 || n2 == 4) {
            return true;
        }
    }

    return false;
}


bool is_elevator(int n1, int n2)
{
    if (n1 == 1) {
        if (n2 == 3 || n2 == 4) {
            return true;
        }
    }
    else {
        if (n2 == 5 || n2 == 6 || n2 == 7 || n2 == 8) {
            return true;
        }
    }

    return false;
}


bool is_nezav(int n1, int n2)
{
    if (n1 == 1) {
        if (n2 == 5 || n2 == 6) {
            return true;
        }
    }
    else {
        if (n2 == 9 || n2 == 10 || n2 == 11 || n2 == 12) {
            return true;
        }
    }

    return false;
}






long getCountTable_(CAdoFile* ado, const char* q)
{
    long count = 0;

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            count = ado->read_long(0);
        }
        ado->closeTable();
    }
    return count;
}

CString getIdQ(const char* q)
{
    CString q0 = q;

    vector<CString> match;
    int l = regex_match("(.+)\\s+ORDER\\s+BY\\s+[a-z0-9_]+", q, match);

    if (l > 0) {
        q0 = match[1];
    }

    q0.Format("SELECT ID FROM (%s\n) ____T____123", q0);
    return q0;
}

CString getCountQ(const char* q)
{
    CString q0 = q;

    vector<CString> match;

    q0.Replace("\n", "(((n)))");
    q0.Replace("\r", "(((r)))");

    int l2 = regex_match("^(.+)[;]+\\s*$", q0, match);

    if (l2 > 0) {
        q0 = match[1];
    }

    /**
        int l = regex_match("(.+)\\s+ORDER\\s+BY\\s+[\\[\\]A-Za-z0-9_,\\.]+;?", q0, match);

        if (l > 0) {

            if (q0.Find("OVER(PARTITION") == -1) {
                q0 = match[1];
            }
        }
    */

    q0.Replace("(((n)))", "\n");
    q0.Replace("(((r)))", "\r");




    CString qq;
    qq.Format("SELECT count(*) FROM (%s\n) ____T____123", q0);
    return qq;
}


long getCountTable2(CAdoFile* ado, const char* q)
{
    CString q0 = getCountQ(q);
    return getCountTable_(ado, q0);
}


bool CCxema::initGidFiles(CAdoFile* ado, set<int>& v)
{
    CString sect;
    sect.Format("DB\\%s", ado->m_schema);

    v.clear();

    int cur_id = m_cur_file ? m_cur_file->id : 0;
    m_cur_file = NULL;

    CMdbList dlg(AfxGetMainWnd(), m_ado);

    int ret = dlg.DoModal();

    if (ret == IDOK) {

        m_file_map.clear();

        map<int, MdbItem>::const_iterator it = dlg.map_item.begin();

        for (int i = 0; it != dlg.map_item.end(); it++, i++) {
            MdbItem item = it->second;

            AfxGetApp()->WriteProfileInt(sect, item.name, item.on);

            if (item.on) {
                v.insert(item.id);

                CGidFile* gid_file = new CGidFile;
                gid_file->id = item.id;
                gid_file->name = item.name;
                gid_file->m_schema = ado->m_schema;
                m_file_map[item.id] = gid_file;

                if (cur_id == gid_file->id) {
                    m_cur_file = gid_file;
                }
            }
        }
        return true;
    }
    return false;
}

CGidFile* CCxema::getGidFile(long id)
{
    map<long, CGidFile*>::iterator it = m_file_map.find(id);

    if (it != m_file_map.end()) {
        return it->second;
    }
    return NULL;
}

double Ieee754SingleDigits2DoubleCheat(const char s[32])
{
    unsigned long  v;
    float f;
    unsigned i;
    char* p1 = (char*)&v, * p2 = (char*)&f;

    // Collect binary digits into an integer variable
    v = 0;
    for (i = 0; i < 32; i++)
        v = (v << 1) + (s[i] - '0');

    // Copy the bits from the integer variable to a float variable
    for (i = 0; i < sizeof(f); i++)
        *p2++ = *p1++;

    return f;
}

bool readLine(CAdoFile* ado, CLine2* line, int typ, CGraph2* m_graph, const char* tn, const char* otkr)
{
    long externalSignLineID = ado->read_long("externalSignLineID");
    long id = ado->read_long("ID");
    long id2 = ado->read_long("ID2");
    long podp = ado->read_long("displaySign");
    long organizationID = ado->read_long("organizationID");
    long magistral = 0;
    long distSite = 0;
    long magistralSite = 0;
    long tubingTypeID = 0; // _TR("Н");
    long pipeSectStateIDflow = 1;
    long pipeSectStateIDret = 1;
    long crimpingQueSite = 0; // опрессовка
    long pipeSectionID = 0;

    UTNapr napr;

    if (typ == TIP_UT) {
        magistral = ado->read_long("l2_magistral");

        distSite = ado->read_long("l2_distSite");
        magistralSite = ado->read_long("l2_magistralSite");

        if (typ == TIP_UT) {
            pipeSectStateIDflow = ado->read_long("pipeSectStateIDflow");
            pipeSectStateIDret = ado->read_long("pipeSectStateIDret");
        }
        else {
            if (otkr[0]) {
                pipeSectStateIDflow = pipeSectStateIDret = ado->read_long(otkr);
            }
            else {
                pipeSectStateIDflow = pipeSectStateIDret = ado->read_long("state");
            }
        }

        tubingTypeID = ado->read_long("tubingTypeID");
        crimpingQueSite = ado->read_long("crimpingQueSite");  // опрессовка
        pipeSectionID = ado->read_long("pipeSectionID");  // Большой участок

        double diameterExternal = ado->read_double("diameterExternal");
        double diameterInternal = ado->read_double("diameterInternal");
        double diameterCondit = ado->read_double("diameterCondit");
        double pipeSectLength = ado->read_double("pipeSectLength");

        napr.dl = pipeSectLength;
        napr.diam = diameterInternal;
        napr.diam_usl = diameterCondit;
        napr.diam_vn = diameterExternal;
        napr.v = napr.dl * pow(napr.diam * 0.001, 2) * M_PI;
        napr.tol = ado->read_double("wallThickness");
    }
    else if (typ == TIP_DR) {
//        napr.dru_home = ado->read_double("diameterInternal");
        napr.dru_home = ado->read_double("dru_home");
    }
    else {
        if (otkr[0]) {
            pipeSectStateIDflow = pipeSectStateIDret = ado->read_long(otkr);
        }
    }

    if (pipeSectStateIDflow == 0) pipeSectStateIDflow = 1;
    if (pipeSectStateIDret == 0) pipeSectStateIDret = 1;

    line->line.tubingTypeID = tubingTypeID; //  tubingTypeID != "" ? tubingTypeID[0] : 'Н';

    line->line.pipeSectionID = pipeSectionID;

    if (distSite == 392) {
        int qq;
        qq = 1;
    }

    line->line.mag = magistral;
    line->line.ms = magistralSite;
    line->line.rs = distSite;

    line->line.typ = typ;
    line->line.owner = organizationID;
    line->line.m_opressovka = crimpingQueSite;

    if (externalSignLineID == SignLine_ob || externalSignLineID == SignLine_pp || externalSignLineID == SignLine_po) {
        line->line.nomP = id;
        line->line.idP2 = id2;
        line->line.pod = napr;
        line->line.pod.isOtkl = (pipeSectStateIDflow == 2);
        line->line.isPodp = !podp;
    }
    if (externalSignLineID == SignLine_ob || externalSignLineID == SignLine_oo || externalSignLineID == SignLine_op) {
        line->line.nomO = id;
        line->line.idO2 = id2;
        line->line.obr = napr;
        line->line.obr.isOtkl = (pipeSectStateIDret == 2);
        line->line.isPodp = !podp;
    }
    line->line.typ = typ;
    return true;
}

CString getLineQ(int typ, const char* tn, const char* otkr, const char* qq)
{
    CString q;

    q.Format("SELECT l1.ID, l2.ID AS id2, l1.nodeID1, l1.nodeID2, l1.externalSignLineID, l1.coords, l1.organizationID, l1.operatorID, l1.displaySign "
        " FROM linesobj l1, %s l2, nodes "
        " WHERE l1.id=l2.lineID  AND l1.nodeID1=nodes.ID AND %s AND l1.removed = 0",
        tn, qq
    );

    if (typ == TIP_UT) {
        q.Format("SELECT l1.ID, l2.ID AS id2, l1.nodeID1, l1.nodeID2, l1.externalSignLineID, l1.coords, l1.organizationID, l1.operatorID, l1.displaySign, "
            " l2.pipeSectStateIDflow, l2.pipeSectStateIDret, l2.magistral AS l2_magistral, l2.distSite AS l2_distSite, l2.magistralSite AS l2_magistralSite, l2.tubingTypeID, l2.diameterInternal, l2.diameterExternal, l2.diameterCondit, l2.pipeSectLength, l2.wallThickness, l2.crimpingQueSite, l2.pipeSectionID "
            " FROM linesobj l1, %s l2, nodes "
            " WHERE l1.id=l2.lineID AND l1.nodeID1=nodes.ID AND %s AND l1.removed = 0",
            tn, qq
        );
    }
    else if (typ == TIP_DR) {
        if (otkr[0]) {
            q.Format("SELECT l1.ID, l2.ID AS id2, l1.nodeID1, l1.nodeID2, l1.externalSignLineID, l1.coords, l1.organizationID, l1.operatorID, l1.displaySign, l2.diameterInternal, l2.diameterInternal AS dru_home, "
                " l2.%s "
                " FROM linesobj l1, %s l2, nodes "
                " WHERE l1.id=l2.lineID AND l1.nodeID1=nodes.ID AND %s AND l1.removed = 0",
                otkr, tn, qq
            );
        }
    }
    else {
        if (otkr[0]) {
            q.Format("SELECT l1.ID, l2.ID AS id2, l1.nodeID1, l1.nodeID2, l1.externalSignLineID, l1.coords, l1.organizationID, l1.operatorID, l1.displaySign, "
                " l2.%s "
                " FROM linesobj l1, %s l2, nodes "
                " WHERE l1.id=l2.lineID AND l1.nodeID1=nodes.ID AND %s AND l1.removed = 0",
                otkr, tn, qq
            );
        }
    }
    return q;
}

long getExternalSignNodeId(CNode2* n)
{
    if (n->node.nomP > 0 && n->node.nomO > 0) {
        return 1;
    }
    if (n->node.nomP > 0) {
        return 2;
    }
    if (n->node.nomO > 0) {
        return 3;
    }
    return 0;
}


long getExternalSignLineId(CLINE2* line)
{
    CLine2* l = bline(line);

    if (l->line.nomP > 0 && l->line.nomO > 0) {
        return 1;
    }
    if (l->line.nomP > 0 && other(line)->node.nomP > 0) {
        return 2;
    }
    if (l->line.nomP > 0 && other(line)->node.nomP <= 0) {
        return 4;
    }
    if (l->line.nomO > 0 && other(line)->node.nomO > 0) {
        return 3;
    }
    if (l->line.nomO > 0 && other(line)->node.nomO <= 0) {
        return 5;
    }
    return 0;
}


bool CCxema::openLine(CAdoFile* ado, const char* tn, const char* otkr, int typ, const char* par, bool dubl)
{
    CCoordList NP;
    CGidFile* gid_file = m_cur_file;

    CString qq;
    qq.Format("nodes.fileID IN (%s)", par);

    if (par[0] == 0) qq = "1=1";

    CString q = getLineQ(typ, tn, otkr, qq);

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            //        long file = ado->read_long("fileID");

            long idn1 = ado->read_long("nodeID1");
            long idn2 = ado->read_long("nodeID2");

            if (idn1 != idn2 && idn1 > 0 && idn2 > 0) {
                CNode2* n1, * n2, * n1i = NULL, * n2i = NULL;

                n1 = m_graph->find(idn1);
                n2 = m_graph->find(idn2);

                if (n1 && n2) {
                    bool skip = false;

                    if (n1->node.internalNodeID != 0) {
                        n1i = m_graph->find(n1->node.internalNodeID);
                        if (!n1i) skip = true;
                    }
                    if (n2->node.internalNodeID != 0) {
                        n2i = m_graph->find(n2->node.internalNodeID);
                        if (!n2i) skip = true;
                    }

                    if (!skip) {
                        CString s_coord = ado->readStr("coords");
                        NP.loadStr(s_coord);

                        CLINE2* l = m_graph->find_line_typ(n1, n2, linef[typ].typ);
                        CLine2* line = bline(l);

                        if (!line || line->line.m_NP != NP) {
                            line = m_graph->insert_line(n1, n2, NP, true);
                        }
                        else {
                            if (line) {
                                long sign_old = getExternalSignLineId(l);
                                long sign_new = ado->read_long("externalSignLineID");

                                if (!(sign_old == 2 && sign_new == 3 || sign_old == 3 && sign_new == 2)) {
                                    if (dubl) {
                                    }
                                    else {
                                        line = m_graph->insert_line(n1, n2, NP, true);
                                    }
                                }
                            }
                        }

                        if (line) {
                            readLine(ado, line, typ, m_graph, tn, otkr);
                        }
                    }
                }
            }

            ado->MoveNext();
        }
        ado->closeTable();
    }
    else {
        AfxMessageBox(ado->getError(), MB_OK | MB_ICONINFORMATION);
        return false;
    }

    return true;
}



static bool read_UT(CAdoFile* ado, char p1, CLINE2* line, int id)
{
    CLine2* l = bline(line);
    UTNapr napr;

    if (p1 == CPOD) {
        napr = l->line.pod;
    }
    else {
        napr = l->line.obr;
    }

    napr.q = ado->read_double("a13");        //  Расход сетевой воды на участке
    napr.poter = ado->read_double("a14");
    //    napr.v = ado->read_double("a9");         //  Объем воды на участке теплопровода
    napr.w = ado->read_double("a10");        //  Скорость потока сетевой воды
    napr.time1 = ado->read_double("a11");    //  Время прохождения потока воды
//    napr.dl = ado->read_double("a7");        //  Длина участка теплопровода
//    napr.diam = ado->read_double("a8");      //  Внутренний диаметр трубы участка
    napr.a14 = ado->read_double("a14");      //  Удельные линейные потери напора на участке
    napr.a15 = ado->read_double("a15");      //  Линейные потери напора на участке         
    napr.a16 = ado->read_double("a16");      //  Местные потери напора на участке          
    napr.a17 = ado->read_double("a17");      //  Общие потери напора на участке            
    napr.tzam = ado->read_double("tzam");     //  Время остывания воды при остановке движения (временно Q для Миши)
    napr.tpot = ado->read_double("tpot");    //  Тепловые потери  теплопроводом

    napr.ql    = ado->read_double("b101");    //  Расчетная тепловая нагрузка, Гкал/ч                 
    napr.ql_ot = ado->read_double("b102");    //  Расчетная тепловая нагрузка на отопление, Гкал/ч    
    napr.ql_v  = ado->read_double("b103");    //  Расчетная тепловая нагрузка на вентиляцию, Гкал/ч   
    napr.ql_gv = ado->read_double("b104");    //  Расчетная тепловая нагрузка на ГВС, Гкал/ч          
                                                 
//    napr.ql_gv_p = ado->read_double("b105");    //  Расчетная тепловая нагрузка на ГВС, Гкал/ч          
//    napr.ql_gv_0 = ado->read_double("b106");    //  Расчетная тепловая нагрузка на ГВС, Гкал/ч          



    if (p1 == CPOD) {
        l->line.nomgP = id;
        l->line.pod = napr;
    }
    else {
        l->line.nomgO = id;
        l->line.obr = napr;
    }
    return false;
}

char po1(long externalSignLineID)
{
    char p1 = CPOD;

    if (externalSignLineID == 1 || externalSignLineID == 2 || externalSignLineID == 4) {
        p1 = CPOD;
    }
    if (externalSignLineID == 1 || externalSignLineID == 3 || externalSignLineID == 5) {
        p1 = COBR;
    }

    return p1;
}

bool CCxema::openLineOut(CAdoFile* ado, CGraph2* graph, const char* tn, const char* par)
{
    CString q;

    CGidFile* gid_file = m_cur_file;

    CString param = "*";

    //  q.Format("SELECT %s FROM %s", param, tn);

    //  q.Format("SELECT %s FROM %s WHERE calculationID IN (SELECT id FROM CALCULATION WHERE date1 IN (SELECT MAX(date1) AS id FROM CALCULATION WHERE fileID IN (%s) GROUP BY fileID))", param, tn, par);
    q.Format("SELECT %s FROM %s WHERE calculationID IN (%s)", param, tn, par);

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            //      long file = ado->read_long("fileID");
            long lineID = ado->read_long("lineID");
            long externalSignLineID = ado->read_long("externalSignLineID");
            long id = ado->read_long("ID");

            CLINE2* line = graph->find_line_nom(lineID);

            if (line) {
                char p1 = po1(externalSignLineID);
                read_UT(ado, p1, line, id);
            }

            ado->MoveNext();
        }
        ado->closeTable();
    }
    return true;
}

bool CCxema::openLineTypOut(CAdoFile* ado, CGraph2* graph, const char* tn, const char* fn, int typ, const char* par)
{
    //  ClockMessage m(tn);

    CString q;

    CGidFile* gid_file = m_cur_file;

    //  q.Format("SELECT * FROM %s",tn);
    //  q.Format("SELECT * FROM %s WHERE calculationID IN (SELECT id FROM CALCULATION WHERE date1 IN (SELECT MAX(date1) AS id FROM CALCULATION GROUP BY fileID))", tn);
    //  q.Format("SELECT * FROM %s WHERE calculationID IN (SELECT id FROM CALCULATION WHERE date1 IN (SELECT MAX(date1) AS id FROM CALCULATION WHERE fileID IN (%s) GROUP BY fileID))", tn, par);
    q.Format("SELECT * FROM %s WHERE calculationID IN (%s)", tn, par);

    int i = 0;

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            //      long file = ado->read_long("fileID");
            long lineID = ado->read_long("lineID");
            long externalSignLineID = ado->read_long("externalSignLineID");
            long id = ado->read_long("ID");

            i++;

            CLINE2* line = graph->find_line_nom(lineID);

            if (line) {
                char p1 = po1(externalSignLineID);
                double q = ado->read_double(fn);

                CLine2* l = bline(line);

                if (p1 == CPOD) {
                    l->line.nomgP = id;
                    l->line.pod.q = q;
                }
                else {
                    l->line.nomgO = id;
                    l->line.obr.q = q;
                }
            }

            ado->MoveNext();
        }
        ado->closeTable();
    }
    return true;
}

void setNodeOut(CNode2* node, int externalSignID, int id, double pih, double t)
{
    if (externalSignID == 1) {
        if (node->node.nomgP < 0 || pih > node->node.data.pihP) {
            node->node.nomgP = id;
            node->node.data.pihP = pih;
            node->node.data.tempP = t;
        }
    }
    if (externalSignID == 2) {
        if (node->node.nomgO < 0 || pih > node->node.data.pihO) {
            node->node.nomgO = id;
            node->node.data.pihO = pih;
            node->node.data.tempO = t;
        }
    }
}


struct NAGR2
{
    double d_otopl_zco;
    double d_otopl_nco;
    double d_ventil_cv;
    double d_otopl_pr;
    double d_gv_pr;
    double d_rez_pr;
    double d_otopl_sm;
    double d_gv_sm;
    double d_rez_sm;
    double d_otopl_ps;
    double d_gv_ps;
    double d_rez_ps;
    double d_otopl_pw;
    double d_gv_pw;
    double d_rez_pw;
    double d_gv_op;
    double d_gv_oo;
    double d_rez_op;


    double d_otoplz;
    double d_otopln;
    double d_otn_fs;
    double d_otopl_tp;
    double d_ventil;
    double d_dolja_vent;
    double d_kondiz;
    double d_txz;
    double d_txop;
    double d_txoo;
    double d_gvop;
    double d_gvoo;
    double d_gvpr;
    double d_gvsm;
    double d_gvps;
    double d_gvpw;
};


bool CCxema::openNodeOut(CAdoFile* ado, CGraph2* graph, const char* tn, const char* par)
{
    //  ClockMessage m(tn);

    CString q;

    //  q.Format("SELECT * FROM US_OUT");
    //  q.Format("SELECT * FROM %s WHERE calculationID IN (SELECT id FROM CALCULATION WHERE date1 IN (SELECT MAX(date1) AS id FROM CALCULATION WHERE fileID IN (%s) GROUP BY fileID))", tn, par);
    q.Format("SELECT * FROM %s WHERE calculationID IN (%s)", tn, par);

    q.Format(
        "SELECT US_OUT.*, n.externalNodeName, n.internalNodeID FROM %s\n"
        "JOIN nodes n ON n.id = US_OUT.nodeID\n"
        "WHERE calculationID IN (%s)", tn, par);


    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {

            //      long file = ado->read_long("fileID");

            //      long externalSignID = ado->read_long("externalSignID");

            long externalSignID = ado->read_long("externalSign");
            //      long internalNodeID = ado->read_long("internalNodeID");
            long calculationID = ado->read_long("calculationID");

            long id = ado->read_long("ID");
            int internalNodeID = ado->read_long("internalNodeID");

            long nodeID = ado->read_long("nodeID");
            double pih = ado->read_double("pih");
            double t = ado->read_double("t");

            CString ss = ado->readStr("externalNodeName");

            if (ss == "K_5") {
                int qq;
                qq = 1;
            }

            
            CNode2* node = graph->find(nodeID);

/*
            if (!node && internalNodeID) {
                node = graph->find(internalNodeID);
            }

*/
            if (node) {
                node->node.calculationID = calculationID;
                setNodeOut(node, externalSignID, id, pih, t);
                if (node->node.internalNodeID > 0 && node->node.typ == TIP_US2) {
                    CNode2* node_parent = m_graph->find(node->node.internalNodeID);
                    if (node_parent) {
                        setNodeOut(node_parent, externalSignID, id, pih, t);
                    }
                }
            }
            ado->MoveNext();
        }

        ado->closeTable();
    }

    return true;
}

bool CCxema::openPtOut(CAdoFile* ado, CGraph2* graph, const char* tn, const char* par)
{
    CString q;

    //    q.Format("SELECT * FROM %s WHERE calculationID IN (SELECT id FROM CALCULATION WHERE date1 IN (SELECT MAX(date1) AS id FROM CALCULATION WHERE fileID IN (%s) GROUP BY fileID))", tn, par);
    q.Format("SELECT * FROM %s WHERE calculationID IN (%s)", tn, par);

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long("ID");
            long nodeID = ado->read_long("nodeID");

            CNode2* node = graph->find(nodeID);

            if (node) {
                node->node.qz = ado->read_double("a15");
                node->node.qP = ado->read_double("a16");
                node->node.qO = ado->read_double("a17");

                if (node->node.qz == 0 && node->node.qP == 0 && node->node.qO == 0) {
                    CString s = ado->readStr("a3");

                    if (!strcmp(s, _TR("отключен"))) {
                        node->node.isZakr = true;
                    }
                }
                node->node.nomPT = id;
            }
            ado->MoveNext();
        }
        ado->closeTable();
    }
    return true;
}


bool CCxema::openDrOut(CAdoFile* ado, CGraph2* graph, const char* tn, const char* par)
{
    CString q;
    q.Format("SELECT * FROM %s WHERE calculationID IN (%s)", tn, par);

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {

            //            long file = ado->read_long("fileID");
            //            long internalNodeID = ado->read_long("internalNodeID");

            long id = ado->read_long("ID");
            long nodeID = ado->read_long("nodeID");

            CNode2* node = graph->find(nodeID);

            if (node) {
                node->node.nomDR = id;
            }
            ado->MoveNext();
        }
        ado->closeTable();
    }
    return true;
}



void setCurrentExternalCode(int fileID, int kod);


void setNodeCxema(CNode2* node2, const char* schemeNum)
{
    if (!schemeNum || !node2) return;

    int n1, n2;

    if (sscanf(schemeNum, "%d.%d", &n1, &n2) == 2) {
        node2->node.isEl = false;
        node2->node.isNez = false;
        if (n2 == 5 || n2 == 6 || n2 == 7 || n2 == 8 || !strcmp(schemeNum, "1.3") || !strcmp(schemeNum, "1.4")) {
            node2->node.isEl = true;
        }

        if (n2 == 9 || n2 == 10 || n2 == 11 || n2 == 12 || !strcmp(schemeNum, "1.5") || !strcmp(schemeNum, "1.6")) {
            node2->node.isNez = true;
            node2->node.isEl = false;
        }

    }
}

bool readNode(CAdoFile* ado, CNode2* node2, int typ, CGraph2* m_graph, const char* tn)
{
    long id = ado->read_long("ID");
    long fileID = ado->read_long("fileID");
    long internalNodeID = ado->read_long("internalNodeID");

    long externalCodeID = ado->read_long("externalCodeID");
    long externalSignID = ado->read_long("externalSignID");
    long podp = ado->read_long("displaySign");

    setCurrentExternalCode(fileID, externalCodeID);

    long connectID = 0;
    long consumerStateID = 1;
    CString schemeNum = "";

    if (typ == TIP_PR || typ == TIP_PO) {
        consumerStateID = ado->read_long("consumerStateID");
    }
    if (typ == TIP_PR) {
        schemeNum = ado->readStr("schemeNum");
    }
    if (typ == TIP_US2) {
        connectID = ado->read_long("connectID");
    }


    long nodeTypeID = ado->read_long("nodeTypeID");


    long id2 = 0;

    if (tn[0]) {
        id2 = ado->read_long("id2");
    }

    CString name = ado->readStr("externalNodeName");
    CString namePS = ado->readStr("nodeName");

    double x = ado->read_double("x");
    double y = ado->read_double("y");

    double geoMarkTopTube = ado->read_double("geoMarkTopTube");
    double geoMarkNodeArea = ado->read_double("geoMarkNodeArea");

    double hz = 0;

    if (typ == TIP_PO) {
        hz = ado->read_double("maxBuildingHeight");
    }

    if (typ == TIP_PR) {
        hz = ado->read_double("buildHeight");
    }

    NAGR nagr;
    memset(&nagr, 0, sizeof(NAGR));

    if (typ == TIP_PO) {
        nagr.d_otopl_zco = ado->read_double("calcHLdep");
        nagr.d_otopl_nco = ado->read_double("calcHLindep");
        nagr.d_ventil_cv = ado->read_double("calcHLventil");
        nagr.d_otopl_pr = ado->read_double("calcHLparall");
        nagr.d_gv_pr = ado->read_double("calcHLGVSparall");
        nagr.d_rez_pr = ado->read_double("avgHLcompParall");
        nagr.d_otopl_sm = ado->read_double("calcHLmix");
        nagr.d_gv_sm = ado->read_double("calcHLGVSmix");
        nagr.d_rez_sm = ado->read_double("avgHLcompMix");
        nagr.d_otopl_ps = ado->read_double("calcHLconseq");
        nagr.d_gv_ps = ado->read_double("calcHLGVSconseq");
        nagr.d_rez_ps = ado->read_double("avgHLcompconseq");
        nagr.d_otopl_pw = ado->read_double("calcHLpreON");
        nagr.d_gv_pw = ado->read_double("calcHLGVSpreON");
        nagr.d_rez_pw = ado->read_double("avgHLcompPreON");
        nagr.d_gv_op = ado->read_double("avgHLGVSopenSysFlow");
        nagr.d_gv_oo = ado->read_double("avgHLGVSopenSysRet");
        nagr.d_rez_op = ado->read_double("avgHLcompOpen");
    }

    node2->node.pribor_ucheta = 0;

    if (typ == TIP_PR) {
        node2->node.pribor_ucheta = (ado->read_long("WEmeteringDeviceID") == 2);

        nagr.d_otoplz = ado->read_double("calcHLdep");
        nagr.d_otopln = ado->read_double("calcHLindep");
        nagr.d_otn_fs = ado->read_double("relLoadFacade");
        nagr.d_otopl_tp = ado->read_double("calcInternHD");
        nagr.d_ventil = ado->read_double("calcHLventil");
        nagr.d_dolja_vent = ado->read_double("expendHWpart");
        nagr.d_kondiz = ado->read_double("avgHLcond");
        nagr.d_txz = ado->read_double("avgHLcloseSys");
        nagr.d_txop = ado->read_double("avgHLopenSysFlow");
        nagr.d_txoo = ado->read_double("avgHLopenSysRet");
        nagr.d_gvop = ado->read_double("avgHLGVSopenFlow");
        nagr.d_gvoo = ado->read_double("avgHLGVSopenRet");
        nagr.d_gvpr = ado->read_double("avgHLGVScloseParall");
        nagr.d_gvsm = ado->read_double("avgHLGVScloseMix");
        nagr.d_gvps = ado->read_double("avgHLGVScloseConseq");
        nagr.d_gvpw = ado->read_double("avgHLGVSclosePreON");
    }

    long automDegID = ado->read_long("automDegID");

    switch (automDegID) {
    case 1:  node2->node.pr_avtomat = ' '; break;
    case 2:  node2->node.pr_avtomat = 'А'; break;
    case 3:  node2->node.pr_avtomat = '3'; break;
    }

    node2->node.Qot = nagr.d_otoplz + nagr.d_otopln + nagr.d_otopl_zco + nagr.d_otopl_nco + nagr.d_otopl_pr + nagr.d_otopl_sm + nagr.d_otopl_ps;
    node2->node.Qgvs = nagr.d_gvop + nagr.d_gvoo + nagr.d_gvpr + nagr.d_gvsm + nagr.d_gvps + nagr.d_gvpw + nagr.d_gv_op + nagr.d_gv_oo + nagr.d_gv_pr + nagr.d_gv_sm + nagr.d_gv_ps + nagr.d_gv_pw;
    node2->node.Qvent = nagr.d_ventil + nagr.d_ventil_cv;

    node2->node.isValid = (typ != TIP_PO && typ != TIP_PR) || node2->node.Qot != 0 || (node2->node.Qvent != 0 && schemeNum == "2.1");

    node2->node.pP_fact = ado->read_double("calcPressFlow");
    node2->node.pO_fact = ado->read_double("calcPressRet");


    if (internalNodeID > 0) {
        CNode2* parent = m_graph->find_ins(internalNodeID);
        if (parent) {
            parent->node.isP = true;
        }
    }

    node2->node.nomP = node2->node.nomO = -1;
    if (externalSignID != SignNode_o) node2->node.nomP = id;
    if (externalSignID != SignNode_p) node2->node.nomO = id;

    node2->node.externalSignID = externalSignID;

    node2->node.id2 = id2;

    node2->node.connectID = connectID;


    if (name.GetLength() >= N10) name = name.Left(N10);

    node2->node.id_kod = externalCodeID;
    node2->node.node_name = CNodeName("", name, ' ');
    node2->node.coord.x = x;
    node2->node.coord.y = y;
    node2->node.typ = typ;
    node2->node.hz = hz;
    node2->node.internalNodeID = internalNodeID;

    node2->node._typPS = nodeTypeID;

    node2->node.schemeNum = schemeNum;


    setNodeCxema(node2, schemeNum);

    /*

        int n1, n2;

        if (sscanf(schemeNum, "%d.%d", &n1, &n2) == 2) {
            node2->node.isEl = false;
            node2->node.isNez = false;
            if (n2 == 5 || n2 == 6 || n2 == 7 || n2 == 8 || schemeNum == "1.3" || schemeNum == "1.4") {
              node2->node.isEl = true;
            }

            if (n2 == 9 || n2 == 10 || n2 == 11 || n2 == 12 || schemeNum == "1.5" || schemeNum == "1.6") {
              node2->node.isNez = true;
              node2->node.isEl = false;
            }
        }
    */

    node2->node.isPodp = !podp;

    node2->node.geoMarkTopTube = geoMarkTopTube;
    node2->node.geoMarkNodeArea = geoMarkNodeArea;

    node2->node.isOtkl = consumerStateID != 1;
    node2->node.name = name;
    node2->node.namePS = namePS;

    return true;
}

CString getNodeQ(int typ, const char* tn, const char* qq)
{
    CString q, par_pot;
    CString param0 = "!.externalSignID,!.fileID,!.externalCodeID,!.internalNodeID,!.externalNodeName,!.nodeName,!.x,!.y,!.geoMarkTopTube,!.geoMarkNodeArea,!.displaySign,!.nodeTypeID,!.calcPressFlow,!.calcPressRet";
    CString param = param0;
    param.Replace("!.", "nodes.");

    if (tn[0]) {
        q.Format("SELECT nodes.id,n2.id AS id2, %s FROM nodes,%s n2 WHERE nodes.id=n2.nodeID AND %s AND nodes.removed = 0", param, tn, qq);

        if (typ == TIP_PR) {
            par_pot = ",calcHLdep,calcHLindep,relLoadFacade,calcInternHD,calcHLventil,expendHWpart,avgHLcond,avgHLcloseSys,avgHLopenSysFlow,avgHLopenSysRet,avgHLGVSopenFlow,avgHLGVSopenRet,avgHLGVScloseParall,avgHLGVScloseMix,avgHLGVScloseConseq,avgHLGVSclosePreON,WEmeteringDeviceID";
            q.Format("SELECT nodes.id,n2.id AS id2, n2.consumerStateID, n2.schemeNum, buildHeight, automDegID, %s %s FROM nodes,%s n2 WHERE nodes.id=n2.nodeID AND %s AND nodes.removed = 0", param, par_pot, tn, qq);
        }
        else if (typ == TIP_PO) {
            par_pot = ",calcHLdep,calcHLindep,calcHLventil,calcHLparall,calcHLGVSparall,avgHLcompParall,calcHLmix,calcHLGVSmix,avgHLcompMix,calcHLconseq,calcHLGVSconseq,avgHLcompconseq,calcHLpreON,calcHLGVSpreON,avgHLcompPreON,avgHLGVSopenSysFlow,avgHLGVSopenSysRet,avgHLcompOpen";
            q.Format("SELECT nodes.id,n2.id AS id2, n2.consumerStateID, maxBuildingHeight, %s %s FROM nodes,%s n2 WHERE nodes.id=n2.nodeID AND %s AND nodes.removed = 0", param, par_pot, tn, qq);
        }
        else if (typ == TIP_US2) {
            q.Format("SELECT nodes.id,n2.id, connectID AS id2, %s FROM nodes,%s n2 WHERE nodes.id=n2.nodeID AND %s AND nodes.removed = 0", param, tn, qq);

            param = param0;
            param.Replace("!.externalNodeName", "n3.externalNodeName");
            param.Replace("!.nodeName", "n3.nodeName");
            param.Replace("!.", "nodes.");

            //        q.Format("SELECT nodes.id,n2.id AS id2, n2.connectID,  %s FROM nodes,%s n2 WHERE nodes.id=n2.nodeID AND %s AND nodes.removed = 0", param, tn, qq);
            //        q.Format("SELECT nodes.id,n2.id AS id2, n2.connectID, %s FROM nodes,%s n2 WHERE nodes.id=n2.nodeID AND %s AND nodes.removed = 0", param, tn, qq);

            q.Format("SELECT nodes.id,n2.id AS id2, n2.connectID, %s "
                "FROM %s n2 "
                "LEFT JOIN nodes ON nodes.id=n2.nodeID "
                "LEFT JOIN nodes n3 ON n3.id=n2.connectID "

                "WHERE %s AND nodes.removed = 0", param, tn, qq);
        }
    }
    else {
        q.Format("SELECT nodes.id,%s FROM nodes WHERE %s AND nodes.removed = 0", param, qq);
    }

    //  q += " AND (nodes.internalNodeID IS NULL OR nodes.internalNodeID = 0 OR )";

    return q;
}

bool CCxema::openNode(CAdoFile* ado, const char* tn, int typ, const char* qq)
{
    CString q = getNodeQ(typ, tn, qq);

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long("ID");
            long fileID = ado->read_long("fileID");

            CNode2* node2 = m_graph->find_ins(id);

            if (node2) {
                readNode(ado, node2, typ, m_graph, tn);
                node2->node.fileID = fileID;
            }
            ado->MoveNext();
        }
        ado->closeTable();
    }
    else {
        AfxMessageBox(ado->getError(), MB_OK | MB_ICONINFORMATION);
        return false;
    }
    return true;
}

/*

bool CCxema::openNodeFile(CAdoFile *ado, const char *tn, int typ, const char *par)
{
    CString q;
    q.Format("nodes.fileID IN (%s)", par);

    return openNode(ado, tn, typ, q);
}
*/



bool readTexts(CAdoFile* ado, CText& txt)
{
    memset(&txt.lf, 0, sizeof(LOGFONT));

    txt.n = ado->read_long("ID");
    txt.fileID = ado->read_long("fileID");
    txt.internalNodeID = ado->read_long("internalNodeID");


    txt.lf.lfCharSet = DEFAULT_CHARSET;

    txt.text = ado->readStr("label_text");
    CString font_name = ado->readStr("font_name");

    strcpy(txt.lf.lfFaceName, font_name);

    txt.lf.lfHeight = ado->read_long("font_size");

    txt.coord.x = ado->read_double("x");
    txt.coord.y = ado->read_double("y");


    return true;
}




bool CCxema::openTexts(CAdoFile* ado, const char* tn, const char* par)
{
    CString q;
    q.Format("SELECT * FROM texts WHERE fileID in (%s)", par);

    //  set_text.clear();

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            CText txt;
            readTexts(ado, txt);
            set_text.insert(txt);
            ado->MoveNext();
        }
        ado->closeTable();
    }
    else {
        //    AfxMessageBox(ado->getError(), MB_OK|MB_ICONINFORMATION);
        return false;
    }
    return true;
}


CString getKodName(map<int, CString>& map_str, int id)
{
    map<int, CString>::const_iterator it = map_str.find(id);
    if (it != map_str.end()) {
        return it->second;
    }
    return "";
}


bool openAnyKod(CAdoFile* ado, const char* tn, const char* fn, const char* fn0, const char* par, map<int, int>& map_v)
{
    CString q;
    q.Format("SELECT id, fileID, %s FROM %s WHERE fileID in (%s) AND %s='%s'", fn, tn, par, fn, fn0);


    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long(0);
            long fileID = ado->read_long(1);
            map_v[fileID] = id;
            ado->MoveNext();
        }
        ado->closeTable();
    }
    return true;
}

bool initAnyKod(CAdoFile* ado, const char* tn, const char* fn, const char* par, map<int, CString>& map_str, bool fileID_Yes = true)
{
    CString q;

    if (fileID_Yes) {
        q.Format("SELECT id, %s FROM %s WHERE fileID in (%s)", fn, tn, par);
    }
    else {
        q.Format("SELECT id, %s FROM %s", fn, tn);
    }

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long(0);
            CString s = ado->readStr(1);
            map_str[id] = s;
            ado->MoveNext();
        }
        ado->closeTable();
    }
    return true;
}

bool CCxema::openZN(CAdoFile* ado, const char* tn)
{
    CString q;

    q.Format("SELECT id, nodeID, pressFlow, pressRet, fragment_resultID FROM %s", tn);

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {

            long id = ado->read_long("ID");
            long nodeID = ado->read_long("nodeID");
            double h_p = ado->read_double("pressFlow");
            double h_o = ado->read_double("pressRet");

            int fileID2 = ado->read_long("fragment_resultID");


            CNode2* n = m_graph->find(nodeID);

            if (n) {
                n->node.nZN = id;
                if (fileID2 > 0) {
                    m_map_fileID_mag[n->node.fileID] = fileID2;
                }
            }



            ado->MoveNext();
        }
        ado->closeTable();
    }

    return true;
}

bool CCxema::openUP(CAdoFile* ado, const char* tn)
{
    CString q;

    q.Format("SELECT id, nodeID FROM %s", tn);

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {

            long id = ado->read_long("ID");
            long nodeID = ado->read_long("nodeID");

            CNode2* n = m_graph->find(nodeID);

            if (n) {
                n->node.nUP = id;
            }
            ado->MoveNext();
        }
        ado->closeTable();
    }
    return true;
}

bool CCxema::openVP(CAdoFile* ado, const char* tn)
{
    CString q;

    q.Format("SELECT id, nodeID FROM %s", tn);

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {

            long id = ado->read_long("ID");
            long nodeID = ado->read_long("nodeID");

            CNode2* n = m_graph->find(nodeID);

            if (n) {
                n->node.nVP = id;
            }
            ado->MoveNext();
        }
        ado->closeTable();
    }
    return true;
}

bool readTableMap(CAdoFile* ado, const char* q, const char* name_id, const char* name_name, map<long, CString>& map1)
{
    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long(name_id);
            CString name = ado->readStr(name_name);
            map1[id] = name;
            ado->MoveNext();
        }
        ado->closeTable();

        return true;
    }
    return false;
}

static CString ado_gid_name = "";

CString getGidAdoName()
{
    return ado_gid_name;
}

void setGidAdoName(const char* name)
{
    ado_gid_name = name;
}

CString getParOut(CAdoFile* ado, const char* par, const char* gid_user)
{
    CString par2 = "";
    CString q;
    q.Format("SELECT id FROM CALCULATION WHERE date1 IN (SELECT MAX(date1) AS id FROM CALCULATION WHERE user_gid='%s' AND fileID IN (%s) GROUP BY fileID)", gid_user, par);
    q.Format("SELECT id FROM CALCULATION WHERE id IN (SELECT MAX(id) AS id FROM CALCULATION WHERE user_gid='%s' AND fileID IN (%s) GROUP BY fileID)", gid_user, par);

    bool ret = ado->openTable0(q);

    if (!ret) {
        q.Format("SELECT id FROM CALCULATION WHERE date1 IN (SELECT MAX(date1) AS id FROM CALCULATION WHERE fileID IN (%s) GROUP BY fileID)", par);
        q.Format("SELECT id FROM CALCULATION WHERE id IN (SELECT MAX(id) AS id FROM CALCULATION WHERE fileID IN (%s) GROUP BY fileID)", par);
        ret = ado->openTable0(q);
    }


    if (ret) {
        while (!ado->isEOF()) {
            long id = ado->read_long("id");

            CString s;
            s.Format("%d", id);
            if (par2 != "") par2 += ",";
            par2 += s;
            ado->MoveNext();
        }
        ado->closeTable();
    }
    return par2;
}

int getOutID(CAdoFile* ado, int fileID)
{
    CString q;
    q.Format("SELECT id FROM CALCULATION WHERE date1 IN (SELECT MAX(date1) AS id FROM CALCULATION WHERE fileID=%d GROUP BY fileID)", fileID);
    q.Format("SELECT id FROM CALCULATION WHERE id IN (SELECT MAX(id) AS id FROM CALCULATION WHERE fileID=%d GROUP BY fileID)", fileID);

    int id = -1;

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            id = ado->read_long("id");
        }
        ado->closeTable();
    }
    return id;
}




bool CCxema::openOut(CAdoFile* ado, const char* par, const char* gid_user)
{
    //  SELECT MAX(id) AS id FROM CALCULATION GROUP BY fileID
    // SELECT id, date1 FROM CALCULATION WHERE date1 IN (SELECT MAX(date1) AS id FROM CALCULATION GROUP BY fileID)

    m_graph->init_find_line_nom();

    CString par2 = getParOut(ado, par, gid_user);


    if (par2 != "") {

        if (!openCALCULATION(ado, par2)) return false;

        if (!openLineOut(ado, m_graph, "UT_OUT", par2)) return false;

        //if (!openLineTypOut(ado, m_graph, "UT_OUT", "a13", TIP_UT, par)) return false;
        if (!openLineTypOut(ado, m_graph, "NS_OUT", "a14", TIP_HC, par2)) return false;
        if (!openLineTypOut(ado, m_graph, "RS_OUT", "a11", TIP_RD, par2)) return false;
        if (!openLineTypOut(ado, m_graph, "BP_OUT", "a13", TIP_BP, par2)) return false;
        if (!openLineTypOut(ado, m_graph, "ZD_OUT", "a9", TIP_ZD, par2)) return false;
        if (!openLineTypOut(ado, m_graph, "ZD2_OUT", "a9", TIP_ZD2, par2)) return false;
        if (!openLineTypOut(ado, m_graph, "DRO_OUT", "ras", TIP_DR, par2)) return false;
        if (!openLineTypOut(ado, m_graph, "ANY_OUT", "ras", TIP_RO, par2)) return false;
        if (!openLineTypOut(ado, m_graph, "ANY_OUT", "ras", TIP_TO, par2)) return false;
        if (!openLineTypOut(ado, m_graph, "ANY_OUT", "ras", TIP_EL, par2)) return false;
        if (!openLineTypOut(ado, m_graph, "ANY_OUT", "ras", TIP_KU, par2)) return false;
        if (!openLineTypOut(ado, m_graph, "OK_OUT", "ras", TIP_OK, par2)) return false;

        if (!openNodeOut(ado, m_graph, "US_OUT", par2)) return false;
        if (!openPtOut(ado, m_graph, "PT_OUT", par2)) return false;
        if (!openDrOut(ado, m_graph, "DR_OUT", par2)) return false;
    }
    return true;
}

#include "global.h"

static map<int, GLOBAL> map_global;


GLOBAL *get_calc_param_global_id(int id)
{
    auto it = map_global.find(id);
    if (it != map_global.end()) {
        return &it->second;
    }
    return nullptr;
}



CString get_calc_param_id(int id)
{
    auto it = map_global.find(id);
    if (it != map_global.end()) {
        return get_calc_param(it->second);
    }
    return "";
}



bool CCxema::openCALCULATION(CAdoFile* ado, const char* par2)
{
    CString q;

    q.Format("SELECT * FROM CALCULATION WHERE ID IN (%s)", par2);

    m_Tn = -300;

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            m_Tn = ado->read_double("Tn");
            m_out_name = ado->readStr("name");
            m_out_date = ado->read_date("date1");

            long id = ado->read_long("id");
            long fileID = ado->read_long("fileID");
            double Tn = ado->read_double("Tn");
            CString name = ado->readStr("name");
            COleDateTime date1 = ado->read_date("date1");
            CString user_gid = ado->readStr("user_gid");
            CString calc_params = ado->readStr("calc_params");
            GLOBAL global;

            long calc_plan = ado->read_long("calc_plan");

            if (read_calc_param(calc_params, global) && calc_plan) {
                map_global[fileID] = global;
            }

            ado->MoveNext();
        }
        ado->closeTable();
    }
    return true;

}


bool CCxema::openOut2(CAdoFile* ado, CGraph2* graph, const char* par2)
{
    //  SELECT MAX(id) AS id FROM CALCULATION GROUP BY fileID
    // SELECT id, date1 FROM CALCULATION WHERE date1 IN (SELECT MAX(date1) AS id FROM CALCULATION GROUP BY fileID)
    graph->init_find_line_nom();

    //    CString par2 = getParOut(ado, graph, par);

    if (!openCALCULATION(ado, par2)) return false;

    if (!openLineOut(ado, graph, "UT_OUT", par2)) return false;

    //if (!openLineTypOut(ado, graph, "UT_OUT", "a13", TIP_UT, par)) return false;
    if (!openLineTypOut(ado, graph, "NS_OUT", "a14", TIP_HC, par2)) return false;
    if (!openLineTypOut(ado, graph, "RS_OUT", "a11", TIP_RD, par2)) return false;
    if (!openLineTypOut(ado, graph, "BP_OUT", "a13", TIP_BP, par2)) return false;
    if (!openLineTypOut(ado, graph, "ZD_OUT", "a9", TIP_ZD, par2)) return false;
    if (!openLineTypOut(ado, graph, "ZD2_OUT", "a9", TIP_ZD2, par2)) return false;
    if (!openLineTypOut(ado, graph, "DRO_OUT", "ras", TIP_DR, par2)) return false;
    if (!openLineTypOut(ado, graph, "ANY_OUT", "ras", TIP_RO, par2)) return false;
    if (!openLineTypOut(ado, graph, "ANY_OUT", "ras", TIP_TO, par2)) return false;
    if (!openLineTypOut(ado, graph, "ANY_OUT", "ras", TIP_EL, par2)) return false;
    if (!openLineTypOut(ado, graph, "ANY_OUT", "ras", TIP_KU, par2)) return false;
    if (!openLineTypOut(ado, graph, "OK_OUT", "ras", TIP_OK, par2)) return false;

    if (!openNodeOut(ado, graph, "US_OUT", par2)) return false;
    if (!openPtOut(ado, graph, "PT_OUT", par2)) return false;
    if (!openDrOut(ado, graph, "DR_OUT", par2)) return false;

    return true;
}


bool CCxema::openIst(CAdoFile* ado)
{
    CString q;
    q.Format("Select * FROM externalCodes WHERE removed = 0");

    bool first = true;

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long ist_id = ado->read_long("heatSourceID");
            long id = ado->read_long("ID");
            map_ist[id] = ist_id;

            RasCx cx;

            cx.id = ado->read_long("id");
            cx.name = ado->readStr("name");

            cx.belongMagistral = ado->read_long("belongMagistral"); // Принадлежность магистрали
            cx.objectID = ado->read_long("objectID"); // Объект РС, ссылка на Вспомогательную таблицу B10_Виды объектов расчетной схемы',
            cx.heatSourceID = ado->read_long("heatSourceID"); // Код источника тепла , ссылка на Источник

            cx.fileID = ado->read_long("fileID");

            map_rs[id] = cx;

            //            if (first) {
            setCurrentExternalCode(cx.fileID, cx.id);
            first = false;
            //            }


                        //        map_ist[ist_id] = id;
                        //        map_pc_show[CString(kod_rs)] = show;
            ado->MoveNext();
        }
    }
    return true;
}

#include "geodez.h"

bool CCxema::open_heatSystem(CAdoFile* ado)
{
    CString q;
    q.Format("Select * FROM heatSystem");

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {

            int id = ado->read_long("id");
            CString name = ado->readStr("nasel_point");

            double Central_Meridian = ado->read_double("Central_Meridian");
            double Latitude_Of_Origin = ado->read_double("Latitude_Of_Origin");
            double False_Easting = ado->read_double("False_Easting");
            double False_Northing = ado->read_double("False_Northing");
            double Scale_Factor = ado->read_double("Scale_Factor");
            double Angle = ado->read_double("Angle");

            double dx = ado->read_double("dx");
            double dy = ado->read_double("dy");

            if (False_Easting == 0) False_Easting = Central_Meridian * 10000;

            set_system_coord(False_Easting, False_Northing, Central_Meridian, Latitude_Of_Origin, Scale_Factor, Angle, dx, dy);


            m_Tn_ot = ado->read_double("t_or");

            m_city = name;

            ado->MoveNext();
        }
    }
    return true;
}


bool CCxema::openFragments(CAdoFile* ado)
{
    map_fragments.clear();

    CString q;
    q.Format("Select * FROM fragments WHERE removed = 0");

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            Fragments fr;

            fr.m_id = ado->read_long("id");
            fr.m_name = ado->readStr("name");

            if (fr.m_name.Find(_TR("Магистрал")) == 0) {
                m_AstanaMag = fr.m_id;
                m_AstanaMagSet.insert(fr.m_id);
            }


            fr.m_name_sys = ado->readStr("name_sys");
            fr.m_nasel_point = ado->readStr("nasel_point");
            fr.m_sezon = ado->readStr("sezon");
            fr.m_year = ado->readStr("year");
            fr.m_t_or = ado->read_double("t_or");
            fr.m_t_vr = ado->read_double("t_vr");
            fr.m_t_vnew = ado->read_double("t_vnew");
            fr.m_tx = ado->read_double("tx");
            fr.m_tx_leto = ado->read_double("tx_leto");
            fr.m_tip_Qgvs = ado->readStr("tip_Qgvs");
            //  `begin_year` DATE DEFAULT NULL COMMENT 'Дата начала отопительного периода',
            //  `end_year` DATE DEFAULT NULL COMMENT 'Дата окончания отопительного периода',
            fr.m_tn_god = ado->read_double("tn_god");
            fr.m_tg_god = ado->read_double("tg_god");
            fr.m_tn_god_leto = ado->read_double("tn_god_leto");
            fr.m_tg_god_leto = ado->read_double("tg_god_leto");
            fr.m_a = ado->read_double("a");
            //  `year_audit` DATE DEFAULT NULL COMMENT 'Отопительный сезон аудитной проверки',
            fr.m_tvn_1 = ado->read_double("tvn_1");
            fr.m_tvn_2 = ado->read_double("tvn_2");
            fr.m_tvn_3 = ado->read_double("tvn_3");
            fr.m_tvn_4 = ado->read_double("tvn_4");
            fr.m_tvn_5 = ado->read_double("tvn_5");

            fr.m_phone_manager = ado->readStr("phone_manager");

            map_fragments[fr.m_id] = fr;

            ado->MoveNext();
        }
    }
    return true;
}

int getLineTyp(const char* tn);
int getNodeTyp(const char* tn);



bool readNodeNew(CAdoFile* ado, CNode2* node2, int typ, CGraph2* m_graph)
{
    long idgP = ado->read_long("usPid");
    long idgO = ado->read_long("usOid");

    long id = ado->read_long("ID");
    long fileID = ado->read_long("fileID");
    long internalNodeID = ado->read_long("internalNodeID");

    long externalCodeID = ado->read_long("externalCodeID");
    long externalSignID = ado->read_long("externalSignID");
    long podp = ado->read_long("displaySign");

    setCurrentExternalCode(fileID, externalCodeID);

    long consumerStateID = ado->read_long("consumerStateID");
    CString schemeNum = ado->readStr("schemeNum");

    long connectID = ado->read_long("connectID");

    long nodeTypeID = ado->read_long("nodeTypeID");

    long id2 = ado->read_long("id2");

    CString name = ado->readStr("externalNodeName");
    CString namePS = ado->readStr("nodeName");

    double x = ado->read_double("x");
    double y = ado->read_double("y");

    x *= coef_xy;
    y *= coef_xy;



    double geoMarkTopTube = ado->read_double("geoMarkTopTube");
    double geoMarkNodeArea = ado->read_double("geoMarkNodeArea");

    double hz = ado->read_double("hz");

    NAGR nagr;
    memset(&nagr, 0, sizeof(NAGR));

    if (typ == TIP_PO) {
        nagr.d_otopl_zco = ado->read_double("calcHLdep");
        nagr.d_otopl_nco = ado->read_double("calcHLindep");
        nagr.d_ventil_cv = ado->read_double("calcHLventil");
        nagr.d_otopl_pr = ado->read_double("calcHLparall");
        nagr.d_gv_pr = ado->read_double("calcHLGVSparall");
        nagr.d_rez_pr = ado->read_double("avgHLcompParall");
        nagr.d_otopl_sm = ado->read_double("calcHLmix");
        nagr.d_gv_sm = ado->read_double("calcHLGVSmix");
        nagr.d_rez_sm = ado->read_double("avgHLcompMix");
        nagr.d_otopl_ps = ado->read_double("calcHLconseq");
        nagr.d_gv_ps = ado->read_double("calcHLGVSconseq");
        nagr.d_rez_ps = ado->read_double("avgHLcompconseq");
        nagr.d_otopl_pw = ado->read_double("calcHLpreON");
        nagr.d_gv_pw = ado->read_double("calcHLGVSpreON");
        nagr.d_rez_pw = ado->read_double("avgHLcompPreON");
        nagr.d_gv_op = ado->read_double("avgHLGVSopenSysFlow");
        nagr.d_gv_oo = ado->read_double("avgHLGVSopenSysRet");
        nagr.d_rez_op = ado->read_double("avgHLcompOpen");
    }

    node2->node.pribor_ucheta = 0;

    if (typ == TIP_PR) {
        node2->node.pribor_ucheta = (ado->read_long("WEmeteringDeviceID") == 2);
        node2->node.nomDR = ado->read_long("nomDR");

        nagr.d_otoplz = ado->read_double("calcHLdep");
        nagr.d_otopln = ado->read_double("calcHLindep");
        nagr.d_otn_fs = ado->read_double("relLoadFacade");
        nagr.d_otopl_tp = ado->read_double("calcInternHD");
        nagr.d_ventil = ado->read_double("calcHLventil");
        nagr.d_dolja_vent = ado->read_double("expendHWpart");
        nagr.d_kondiz = ado->read_double("avgHLcond");
        nagr.d_txz = ado->read_double("avgHLcloseSys");
        nagr.d_txop = ado->read_double("avgHLopenSysFlow");
        nagr.d_txoo = ado->read_double("avgHLopenSysRet");
        nagr.d_gvop = ado->read_double("avgHLGVSopenFlow");
        nagr.d_gvoo = ado->read_double("avgHLGVSopenRet");
        nagr.d_gvpr = ado->read_double("avgHLGVScloseParall");
        nagr.d_gvsm = ado->read_double("avgHLGVScloseMix");
        nagr.d_gvps = ado->read_double("avgHLGVScloseConseq");
        nagr.d_gvpw = ado->read_double("avgHLGVSclosePreON");
    }


    node2->node.pP_fact = ado->read_double("calcPressFlow");
    node2->node.pO_fact = ado->read_double("calcPressRet");



    long automDegID = ado->read_long("automDegID");

    switch (automDegID) {
    case 1:  node2->node.pr_avtomat = ' '; break;
    case 2:  node2->node.pr_avtomat = 'А'; break;
    case 3:  node2->node.pr_avtomat = '3'; break;
    }

    node2->node.Qot = nagr.d_otoplz + nagr.d_otopln + nagr.d_otopl_zco + nagr.d_otopl_nco + nagr.d_otopl_pr + nagr.d_otopl_sm + nagr.d_otopl_ps + nagr.d_otopl_pw;
    node2->node.Qgvs = nagr.d_gvop + nagr.d_gvoo + nagr.d_gvpr + nagr.d_gvsm + nagr.d_gvps + nagr.d_gvpw + nagr.d_gv_op + nagr.d_gv_oo + nagr.d_gv_pr + nagr.d_gv_sm + nagr.d_gv_ps + nagr.d_gv_pw;
    node2->node.Qvent = nagr.d_ventil + nagr.d_ventil_cv;

    node2->node.isValid = (typ != TIP_PO && typ != TIP_PR) || node2->node.Qot != 0 || (node2->node.Qvent != 0 && schemeNum == "2.1");

    if (internalNodeID > 0) {
        CNode2* parent = m_graph->find_ins(internalNodeID);
        if (parent) {
            parent->node.isP = true;
        }
    }

    node2->node.nomP = node2->node.nomO = -1;
    if (externalSignID != SignNode_o) node2->node.nomP = id;
    if (externalSignID != SignNode_p) node2->node.nomO = id;

    node2->node.externalSignID = externalSignID;

    node2->node.id2 = id2;

    node2->node.connectID = connectID;


    if (name.GetLength() >= N10) name = name.Left(N10);

    node2->node.id_kod = externalCodeID;
    node2->node.node_name = CNodeName("", name, ' ');
    node2->node.coord.x = x;
    node2->node.coord.y = y;
    node2->node.typ = typ;
    node2->node.hz = hz;
    node2->node.internalNodeID = internalNodeID;

    node2->node._typPS = nodeTypeID;

    node2->node.schemeNum = schemeNum;





    setNodeCxema(node2, schemeNum);

    node2->node.isPodp = !podp;

    node2->node.geoMarkTopTube = geoMarkTopTube;
    node2->node.geoMarkNodeArea = geoMarkNodeArea;

    node2->node.isOtkl = consumerStateID != 1;
    node2->node.name = name;
    node2->node.namePS = namePS;


    node2->node.nomgP = idgP;
    node2->node.data.pihP = ado->read_double("pihP");
    node2->node.data.tempP = ado->read_double("tP1");

    node2->node.nomgO = idgO;
    node2->node.data.pihO = ado->read_double("pihO");
    node2->node.data.tempO = ado->read_double("tO1");

    if (typ == TIP_PR || typ == TIP_PO) {
        node2->node.qz = ado->read_double("a15");
        node2->node.qP = ado->read_double("a16");
        node2->node.qO = ado->read_double("a17");

        if (node2->node.qz == 0 && node2->node.qP == 0 && node2->node.qO == 0) {
            node2->node.isZakr = ado->readStr("isZakr");
        }
        node2->node.nomPT = ado->read_long("nomPT");
    }

    return true;
}



bool CCxema::openNodesNew(CAdoFile* ado, const char* par, const char* par2, Percent2* pc, const char* user_gid)
{
    CString tn1 = argpath() + "\\sql3\\us.sql";
    CString tn2 = argpath() + "\\sql3\\us2.sql";

    bool yes_user_gid = is_column_in_table(ado, "CALCULATION", "user_gid");
    if (yes_user_gid) {
        tn1 = argpath() + "\\sql3\\us_2022_03_12.sql";
        tn2 = argpath() + "\\sql3\\us2_2022_03_12.sql";
    }
    bool yes_vodoprovod = is_column_in_table(ado, "voda_UCH", "id");
    if (yes_vodoprovod) {
        tn1 = argpath() + "\\sql3\\us_voda_2022_03_12.sql";
        tn2 = argpath() + "\\sql3\\us2_voda_2022_03_12.sql";
    }


    CString q = readFile(tn1);
    q.Replace("$user_gid$", user_gid);

    CString qq;

//    qq.Format("%s AND n.fileID IN (%s) ORDER BY n.id", q, par);
    qq.Format("%s AND n.fileID IN (%s)", q, par);

    if (par2 && par2[0]) {
        qq.Format("%s AND n.fileID IN (%s) AND %s", q, par, par2);
    }


    long cnt = 0;

    if (pc) {
        CString q0;
        q0.Format("SELECT id FROM nodes n WHERE n.fileID IN (%s) AND n.removed=0", par);

        if (par2 && par2[0]) {
            q0.Format("SELECT id FROM nodes n WHERE n.fileID IN (%s) AND %s AND n.removed=0", par, par2);
//            qq.Format("%s AND n.fileID IN (%s) AND %s", q, par, par2);
        }

        
        cnt = getCountTable2(ado, q0);
    }

    if (ado->openTable0(qq)) {
        int i = 0;
        long id_old = -1;


        while (!ado->isEOF()) {
            if (pc && cnt) {
                pc->SetValue(i * 100 / cnt / 2);
                i++;
            }

            long id = ado->read_long("id");

            if (id == id_old) {
                int qqq;
                qqq = 1;
//                AfxMessageBox("!");
            }


            if (id != id_old) {
                id_old = id;

                long fileID = ado->read_long("fileID");

                CString type_txt = ado->readStr("type_txt");

                int typ = getNodeTyp(type_txt);

                CNode2* node2 = m_graph->find_ins(id);

                if (node2) {
                    readNodeNew(ado, node2, typ, m_graph);
                    node2->node.fileID = fileID;
                }
            }
            ado->MoveNext();
        }
        ado->closeTable();
    }
    return true;
}


bool readLineNew(CAdoFile* ado, CLine2* line, int typ, CGraph2* m_graph)
{
    long externalSignLineID = ado->read_long("externalSignLineID");
    long id = ado->read_long("ID");
    long id2 = ado->read_long("ID2");
    long podp = ado->read_long("displaySign");
    long organizationID = ado->read_long("organizationID");
    long magistral = 0;
    long distSite = 0;
    long magistralSite = 0;
    long tubingTypeID = 0; // _TR("Н");
    long pipeSectStateIDflow = 1;
    long pipeSectStateIDret = 1;
    long crimpingQueSite = 0; // опрессовка
    long pipeSectionID = 0;

    UTNapr napr;

    pipeSectStateIDflow = ado->read_long("pipeSectStateIDflow");
    pipeSectStateIDret = ado->read_long("pipeSectStateIDret");


    if (typ == TIP_UT) {
        magistral = ado->read_long("magistral");

        distSite = ado->read_long("distSite");
        magistralSite = ado->read_long("magistralSite");


        tubingTypeID = ado->read_long("tubingTypeID");
        crimpingQueSite = ado->read_long("crimpingQueSite");  // опрессовка
        pipeSectionID = ado->read_long("pipeSectionID");  // Большой участок

        double diameterExternal = ado->read_double("diameterExternal");
        double diameterInternal = ado->read_double("diameterInternal");
        double diameterCondit = ado->read_double("diameterCondit");
        double pipeSectLength = ado->read_double("pipeSectLength");

        napr.dl = pipeSectLength;
        napr.diam = diameterInternal;
        napr.diam_usl = diameterCondit;
        napr.diam_vn = diameterExternal;
        napr.v = napr.dl * pow(napr.diam * 0.001, 2) * M_PI;
        napr.tol = ado->read_double("wallThickness");
    }
    else if (typ == TIP_DR) {
//        napr.dru_home = ado->read_double("diameterInternal");
        napr.dru_home = ado->read_double("dru_home");
    }

    if (pipeSectStateIDflow == 0) pipeSectStateIDflow = 1;
    if (pipeSectStateIDret == 0) pipeSectStateIDret = 1;

    line->line.tubingTypeID = tubingTypeID; //  tubingTypeID != "" ? tubingTypeID[0] : 'Н';

    line->line.pipeSectionID = pipeSectionID;


    line->line.mag = magistral;
    line->line.ms = magistralSite;
    line->line.rs = distSite;

    line->line.typ = typ;
    line->line.owner = organizationID;
    line->line.m_opressovka = crimpingQueSite;

    if (externalSignLineID == SignLine_ob || externalSignLineID == SignLine_pp || externalSignLineID == SignLine_po) {
        line->line.nomP = id;
        line->line.idP2 = id2;
        line->line.pod = napr;
        line->line.pod.isOtkl = (pipeSectStateIDflow == 2);
        line->line.isPodp = !podp;
    }
    if (externalSignLineID == SignLine_ob || externalSignLineID == SignLine_oo || externalSignLineID == SignLine_op) {
        line->line.nomO = id;
        line->line.idO2 = id2;
        line->line.obr = napr;
        line->line.obr.isOtkl = (pipeSectStateIDret == 2);
        line->line.isPodp = !podp;
    }
    line->line.typ = typ;

    if (externalSignLineID == SignLine_ob || externalSignLineID == SignLine_pp || externalSignLineID == SignLine_po) {
        line->line.pod.q = ado->read_double("pod_q");        //  Расход сетевой воды на участке
        line->line.nomgP = ado->read_long("nomgP");
    }
    if (externalSignLineID == SignLine_ob || externalSignLineID == SignLine_oo || externalSignLineID == SignLine_op) {
        line->line.obr.q = ado->read_double("obr_q");        //  Расход сетевой воды на участке
        line->line.nomgO = ado->read_long("nomgO");
    }

    if (typ == TIP_UT) {
        if (externalSignLineID == SignLine_ob || externalSignLineID == SignLine_pp || externalSignLineID == SignLine_po) {
            line->line.pod.poter = ado->read_double("pod_poter");
            line->line.pod.w = ado->read_double("pod_w");        //  Скорость потока сетевой воды
            line->line.pod.time1 = ado->read_double("pod_time1");    //  Время прохождения потока воды
            line->line.pod.a14 = ado->read_double("pod_a14");      //  Удельные линейные потери напора на участке
            line->line.pod.a15 = ado->read_double("pod_a15");      //  Линейные потери напора на участке         
            line->line.pod.a16 = ado->read_double("pod_a16");      //  Местные потери напора на участке          
            line->line.pod.a17 = ado->read_double("pod_a17");      //  Общие потери напора на участке            
            line->line.pod.tzam = ado->read_double("pod_tzam");      //  Время остывания воды при остановке движения
            line->line.pod.tpot = ado->read_double("pod_tpot");    //  Тепловые потери  теплопроводом

            line->line.pod.ql    = ado->read_double("pod_b101");    //  
            line->line.pod.ql_ot = ado->read_double("pod_b102");    //  
            line->line.pod.ql_v  = ado->read_double("pod_b103");    //  
            line->line.pod.ql_gv = ado->read_double("pod_b104");    //  

            line->line.obr.ql    = ado->read_double("obr_b101");    //  
            line->line.obr.ql_ot = ado->read_double("obr_b102");    //  
            line->line.obr.ql_v  = ado->read_double("obr_b103");    //  
            line->line.obr.ql_gv = ado->read_double("obr_b104");    //  
        }

        if (externalSignLineID == SignLine_ob || externalSignLineID == SignLine_oo || externalSignLineID == SignLine_op) {
            line->line.obr.poter = ado->read_double("obr_poter");
            line->line.obr.w = ado->read_double("obr_w");        //  Скорость потока сетевой воды
            line->line.obr.time1 = ado->read_double("obr_time1");    //  Время прохождения потока воды
            line->line.obr.a14 = ado->read_double("obr_a14");      //  Удельные линейные потери напора на участке
            line->line.obr.a15 = ado->read_double("obr_a15");      //  Линейные потери напора на участке         
            line->line.obr.a16 = ado->read_double("obr_a16");      //  Местные потери напора на участке          
            line->line.obr.a17 = ado->read_double("obr_a17");      //  Общие потери напора на участке            
            line->line.obr.tzam = ado->read_double("obr_tzam");      //  Время остывания воды при остановке движения
            line->line.obr.tpot = ado->read_double("obr_tpot");    //  Тепловые потери  теплопроводом
        }
    }

    return true;
}



bool CCxema::openLinesNew(CAdoFile* ado, const char* par, const char* par2, Percent2* pc, const char *user_gid)
{
// проверяем наличие полей b101 в UT_OUT, если нет, то подключаем старый запрс

    bool yes_b101 = is_column_in_table(ado, "UT_OUT", "b101");
    bool yes_user_gid = is_column_in_table(ado, "CALCULATION", "user_gid");

    CString qn1 = "\\sql3\\ut.sql";  // Совсе старая версия
    CString qn2 = "\\sql3\\ut2.sql";  // Совсе старая версия
    if (yes_b101) {   // версия от 2021_12
        qn1 = "\\sql3\\ut_2021_12.sql";
        qn2 = "\\sql3\\ut2_2021_12.sql";
    }
    if (yes_user_gid) {   
        qn1 = "\\sql3\\ut_2022_03_12.sql";
        qn2 = "\\sql3\\ut2_2022_03_12.sql";
    }
    bool yes_vodoprovod = is_column_in_table(ado, "voda_UCH", "id");
    if (yes_vodoprovod) {
        qn1 = "\\sql3\\ut_voda_2022_03_12.sql";
        qn2 = "\\sql3\\ut2_voda_2022_03_12.sql";
    }


    CString q = readFile(argpath() + qn1);
    q.Replace("$user_gid$", user_gid);
    

    CString qq;
    CCoordList NP;

    bool dubl = false;

//    qq.Format("%s AND n1.fileID IN (%s) ORDER BY l.id", q, par);
    qq.Format("%s\nAND n1.fileID IN (%s) \nOPTION (RECOMPILE)", q, par);
    if (par2 && par2[0]) {
        qq.Format("%s\nAND n1.fileID IN (%s) AND %s", q, par, par2);
    }


    long cnt = 0;

    if (pc) {
        CString q0;
        q0.Format("SELECT l.id FROM linesobj l JOIN nodes n1 ON n1.id=l.nodeID1 and n1.removed=0  JOIN nodes n2 ON n2.id=l.nodeID2 and n2.removed=0 WHERE n1.fileID IN (%s) AND l.removed=0", par);
        if (par2 && par2[0]) {
            q0.Format("SELECT l.id FROM linesobj l JOIN nodes n1 ON n1.id=l.nodeID1 WHERE n1.fileID IN (%s) AND %s AND l.removed=0", par);
        }


        cnt = getCountTable2(ado, q0);
    }

    int i = 0;

//    ado->SetTimeout(60000);

    if (ado->openTable0(qq)) {

        long id_old = -1;

        while (!ado->isEOF()) {

            if (pc && cnt) {
                pc->SetValue(i * 100 / cnt / 2 + 50);
                i++;
            }
            long id = ado->read_long("id");

            if (id == id_old) {
                int qqq;
                qqq = 1;
//                AfxMessageBox("!");
            }

            if (id == 452 || id == 453) {
                int qq;
                qq = 1;
            }


            if (id != id_old) {
                id_old = id;

                long idn1 = ado->read_long("nodeID1");
                long idn2 = ado->read_long("nodeID2");

                //                if (idn1 == 468 && idn2 == 473) {
                if (idn1 == 95 && idn2 == 90) {
                    int qq;
                    qq = 1;
                }

                CString type_txt = ado->readStr("type_txt");

                int typ = getLineTyp(type_txt);

                if (idn1 != idn2 && idn1 > 0 && idn2 > 0) {
                    CNode2* n1, * n2, * n1i = NULL, * n2i = NULL;

                    n1 = m_graph->find(idn1);
                    n2 = m_graph->find(idn2);

                    if (n1 && n2) {
                        bool skip = false;

                        if (n1->node.internalNodeID != 0) {
                            n1i = m_graph->find(n1->node.internalNodeID);
                            if (!n1i) skip = true;
                        }
                        if (n2->node.internalNodeID != 0) {
                            n2i = m_graph->find(n2->node.internalNodeID);
                            if (!n2i) skip = true;
                        }

                        if (!skip) {
                            CString s_coord = ado->readStr("coords");
                            NP.loadStr(s_coord);

                            if (coef_xy != 1.) {
                                for (auto it = NP.begin(); it != NP.end(); it++) {
                                    it->x *= coef_xy;
                                    it->y *= coef_xy;
                                }
                            }



                            CLINE2* l = m_graph->find_line_typ(n1, n2, linef[typ].typ);
                            CLine2* line = bline(l);

                            if (!line || line->line.m_NP != NP) {
                                line = m_graph->insert_line(n1, n2, NP, true);
                            }
                            else {
                                if (line) {
                                    long sign_old = getExternalSignLineId(l);
                                    long sign_new = ado->read_long("externalSignLineID");

                                    if (!(sign_old == 2 && sign_new == 3 || sign_old == 3 && sign_new == 2)) {

                                        int nomP_old = line->line.nomP;
                                        int nomO_old = line->line.nomO;


                                        //                                        if (dubl || (nomP_old == id || nomO_old)) {
                                        if (dubl) {
                                        }
                                        else {
                                            line = m_graph->insert_line(n1, n2, NP, true);
                                        }
                                    }
                                }
                            }

                            if (line) {
                                readLineNew(ado, line, typ, m_graph);
                            }
                        }
                    }
                }

            }
            ado->MoveNext();
        }
        ado->closeTable();
    }
    else {
        AfxMessageBox(ado->getError(), MB_OK | MB_ICONINFORMATION);
        return false;
    }



    return true;
}

//#define OLD_OPEN 1



bool CCxema::openGid(CAdoFile* ado, const char* par, const char *user_gid)
{
    Percent2 pc;

    readTableMap(ado, "SELECT id,name FROM externalCodes WHERE removed = 0", "ID", "name", m_graph->map_kod);

#ifndef OLD_OPEN
    if (!openNodesNew(ado, par, 0, &pc, user_gid)) return false;
    if (!openLinesNew(ado, par, 0, &pc, user_gid)) return false;
#endif


    //  REBProgressManager pc;

#ifdef OLD_OPEN

    if (!openNodeFile(ado, "", TIP_US, par)) return false;

    pc.SetValue(1 * 100 / (nodefsize + linefsize));

    int i;
    for (i = 0; i < nodefsize; i++) {
        pc.SetValue(i * 100 / (nodefsize + linefsize));
        if (nodef[i].tableMySQL[0]) {
            if (!openNodeFile(ado, nodef[i].tableMySQL, nodef[i].typ, par)) return false;
        }
    }

    for (i = 0; i < linefsize; i++) {
        pc.SetValue((i + nodefsize) * 100 / (nodefsize + linefsize));
        openLine(ado, linef[i].tableMySQL, linef[i].otkr, linef[i].typ, par, false);
    }


#endif

    m_graph->init_find_line_nom();

    if (!openZN(ado, "setPressNodes")) return false;
    if (!openUP(ado, "refillNodes")) return false;
    if (!openVP(ado, "WDOdevices")) return false;

    CString q;

    q.Format("SELECT hSourceCode, nodeID FROM %s", "heatSources");

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {

            long nodeID = ado->read_long("nodeID");
            CString kod_ist = ado->readStr("hSourceCode");

            CNode2* n = m_graph->find(nodeID);
            if (n) {
                map_ist2[kod_ist] = n;
            }
            ado->MoveNext();
        }
        ado->closeTable();
    }

    open_heatSystem(ado);
    openFragments(ado);
    openIst(ado);

    openTexts(ado, "texts", par);


    openAnyKod(ado, "specExpends", "specExpendID", _TR("УР1"), par, map_init_UR);
    openAnyKod(ado, "varCoefficients", "kodkv", _TR("КВ1"), par, map_init_KV);
    openAnyKod(ado, "calcTemperatures", "calcTemperatureID", _TR("ТР1"), par, map_init_TR);

    initAnyKod(ado, "specExpends", "specExpendID", par, map_UR);
    initAnyKod(ado, "varCoefficients", "kodkv", par, map_KV);
    initAnyKod(ado, "calcTemperatures", "calcTemperatureID", par, map_TR);

    initAnyKod(ado, "heatPoint", "name", par, map_TP, false);

#ifdef OLD_OPEN

    openOut(ado, par);

#else 


    CString par2 = getParOut(ado, par, user_gid);
    if (!openCALCULATION(ado, par2)) return false;


#endif


    return true;
}

CString getPar(set<int>& v)
{
    CString par = "";

    set<int>::const_iterator it = v.begin();
    for (; it != v.end(); it++) {
        CString s;
        s.Format("%d", *it);
        if (par != "") par += ",";
        par += s;
    }
    return par;
}

CString CCxema::getParAll()
{
    set<int> v;

    map<long, CGidFile*>::iterator it = m_file_map.begin();

    for (; it != m_file_map.end(); it++) {
        v.insert(it->first);
    }
    return getPar(v);
}


static int max_changed_object = 0;

int get_max_changed_object()
{
    return max_changed_object;
}

    
void set_cxema_updated(bool yes);


void reset_max_changed_object(CAdoFile* ado)
{
    max_changed_object = readTableValueInt(ado, "select max(id) as id from changed_object");
//    set_cxema_updated(false);
}


bool CCxema::openCxemaODBC(int type_of_net, const char* server, int port, const char* database, const char* user, const char* user_gid, const char* password)
{
    log1("Начали openCxemaODBC");

    
    bool ret = false;

    void initDefault();

    initDefault();

    m_graph = new CGraph2(this);

    CGidFile* gid_file = new CGidFile;

    m_cur_file = gid_file;

    CAdoFile* ado = new CAdoFile;
    m_ado = ado;
    ado_gid_name = database;

    if (ado->openMySQL(type_of_net, server, port, database, user, password)) {
        static bool sp_updatestats = false;

        if (!sp_updatestats) {
            ado->Execute("EXEC sp_updatestats");
            sp_updatestats = true;
        }
        
        map<long, CString> map_mdb;

        readTableMap(ado, "SELECT id,name FROM externalCodes WHERE removed = 0", "ID", "name", m_graph->map_kod);

        set<int> v;

        if (initGidFiles(ado, v)) {
            if (v.size() > 0) {
                m_par = getPar(v);
                ret = openGid(ado, m_par, user_gid);
            }
        }
        reset_max_changed_object(ado);
    }
    else {
        AfxMessageBox(ado->getError(), MB_OK | MB_ICONINFORMATION);
    }
    //  initGidTree();

    log1("Закончили openCxemaODBC");

    return ret;
}


CString getSelectFragments();


bool CCxema::initGidTree()
{
    log1("Начали initGidTree");

    CLeft2Dialog* left2 = (CLeft2Dialog*)getDockablePane(IDD_LEFT2);
    if (left2) {
        CTreeCtrl* m_ctlTree = &left2->m_ctlTree;

        m_ctlTree->DeleteAllItems();


        CString q = getSelectFragments();
        //    CString q = "SELECT id, name FROM fragments WHERE removed = 0";

        if (m_ado->openTable0(q)) {
            while (!m_ado->isEOF()) {

                long id = m_ado->read_long("id");
                CString name = m_ado->readStr("name");

                map<long, CGidFile*>::const_iterator it = m_file_map.find(id);

                if (it != m_file_map.end()) {
                    HTREEITEM i2 = m_ctlTree->InsertItem(name, 5, 5, 0);
                    m_ctlTree->SetItemData(i2, id);
                }

                m_ado->MoveNext();
            }
            m_ado->closeTable();
        }



#if 0
        map<long, CGidFile*>::const_iterator it = m_file_map.begin();
        for (; it != m_file_map.end(); it++) {
            CGidFile* file = it->second;
            CString s = GetTitle(file->name);

            HTREEITEM i2 = m_ctlTree->InsertItem(s, 5, 5, 0);
            m_ctlTree->SetItemData(i2, it->first);

            /*

                  list<RasCx>::const_iterator it2 = file->m_list_rs.begin();
                  for (; it2 != file->m_list_rs.end(); it2++) {
                    s = it2->kod_rs;
                    HTREEITEM i3 = m_ctlTree->InsertItem(s, 5, 5, i2);
                  }
            */
        }
#endif  

    }

    m_par = getParAll();

    log1("Закончили initGidTree");
    
    return true;
}

#include "mmenu.h"
#include "virtlistView.h"
#include "adolist.h"
#include "VirtualDlg.h"

bool viewTable(CWnd* wnd)
{
    //  CAdoFile *ado = new CAdoFile;

    CAdoFile* ado = getAdo(getGidAdoName());

    if (ado) {
        CMMenu menu(wnd, IDD_MENU6, _TR("Выберите таблицу Стандартного оборудования"));

        if (ado->openSchema()) {
            while (!ado->isEOF()) {

                CString table_catalog = ado->readStr("TABLE_CATALOG");
                CString table_schema = ado->readStr("TABLE_SCHEMA");
                CString table_name = ado->readStr("TABLE_NAME");
                CString table_type = ado->readStr("TABLE_TYPE");
                CString table_guid = ado->readStr("TABLE_GUID");
                CString description = ado->readStr("DESCRIPTION");

                COleVariant var = ado->read("TABLE_NAME");

                if (table_type == "TABLE" && table_name.Find("standard") == 0) {
                    menu.lst.Add(table_name);
                }

                ado->MoveNext();
            }
        }
        if (menu.DoModal() == IDOK && menu.m_Index != -1) {


            CAdoListData* nd = new CAdoListData();

            CString q;
            q.Format("SELECT * FROM %s", menu.m_strText);


            nd->initQpat(ado, q);

            //        RunTableNew(wnd, linef[i].table, nd);

            CVirtualDlg dlg(wnd, nd, -1, menu.m_strText, true);
            if (dlg.DoModal() == IDOK) {
                //          text = dlg.m_text;
                //          id = dlg.m_id;
                //          return true;
            }
        }
    }
    return true;
}


#if 0

bool openPS()
{
    //  CAdoFile *ado = new CAdoFile;

    CAdoFile* ado = getAdo();

    if (ado) {
        CMMenu menu(wnd, IDD_MENU6, _TR("Выберите таблицу Стандартного оборудования"));

        if (ado->openSchema()) {
            while (!ado->isEOF()) {

                CString table_catalog = ado->readStr("TABLE_CATALOG");
                CString table_schema = ado->readStr("TABLE_SCHEMA");
                CString table_name = ado->readStr("TABLE_NAME");
                CString table_type = ado->readStr("TABLE_TYPE");
                CString table_guid = ado->readStr("TABLE_GUID");
                CString description = ado->readStr("DESCRIPTION");

                COleVariant var = ado->read("TABLE_NAME");

                if (table_type == "TABLE" && table_name.Find("standard") == 0) {
                    menu.lst.Add(table_name);
                }

                ado->MoveNext();
            }
        }
        if (menu.DoModal() == IDOK && menu.m_Index != -1) {


            CAdoListData* nd = new CAdoListData();

            CString q;
            q.Format("SELECT * FROM %s", menu.m_strText);


            nd->initQ(ado, q);

            //        RunTableNew(wnd, linef[i].table, nd);

            CVirtualDlg dlg(wnd, nd, "", true);
            if (dlg.DoModal() == IDOK) {
                //          text = dlg.m_text;
                //          id = dlg.m_id;
                //          return true;
            }
        }
    }
    return true;
}
#endif

