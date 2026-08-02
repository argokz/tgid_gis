#include "stdafx.h"
#include "gid6.h"
#include "Cxema.h"

#include "graph2.h"
#include "ado.h"

#include "MainFrm.h"
#include "win.h"

#include "Bgi.h"
#include "InputDia.h"

#include "TaskProgressDlg.h"

#include "init_files.h"

extern CGeoFile* m_geofile;

long getCountTable2(CAdoFile* ado, const char* q);

//#pragma pack(1)

//#define WKB_Point               1
//#define WKB_LineString          2
//#define WKB_Polygon             3
//#define WKB_MultiPoint          4
//#define WKB_MultiLineString     5 
//#define WKB_MultiPolygon        6
//#define WKB_GeometryCollection  7

//struct XY {
//  double x, y;
//};

//struct GEOM {
//  long SRID;
//  char order;
//  long typ;
//  char *buf;
//};

#pragma pack() 

void readWKB(char* buf, CCoordList& cl);
int readWKB(char* buf, int len, CCoordList& cl);

bool readWKB(COleVariant& var, CCoordList& cl)
{
    if (var.vt == VT_NULL) return false;

    COleSafeArray sa, sa1;

    sa = COleSafeArray((VARIANT)var);

    long r1, r2;

    DWORD dim = sa.GetDim();
    DWORD es = sa.GetElemSize();

    sa.GetLBound(1, &r1);
    sa.GetUBound(1, &r2);

    char* pb;
    sa.AccessData((LPVOID*)&pb);

    readWKB(pb, r2 - r1, cl);




    //  readWKB(pb+4, cl);

    sa.UnaccessData();

    return true;
}

void preobr(const CFPoint* p1, const CFPoint* p2, double* x);

int color_open_close(int stateID)
{
    if (stateID == 1) { // Открыта
        return BLACK;
    }
    if (stateID == 4) { // Частично открыта
        return BLUE;
    }
    return LIGHTRED;  // Закрыта
}


// Старая программа

bool CGeoFile::readMySql_Shape_table(CAdoFile* ado, Klassif* kl, Percent2* pc, int p1, int p2)
{
    CString q, q1;
    double x1, x2, y1, y2;

    CString qq = kl->nazv;

    if (qq == "linesobj") return false;
    if (qq == "nodes") return false;

    kl->geo4.resize(0);

    //    q.Format("SELECT COUNT(*) FROM %s", qq);
    q.Format("SELECT * FROM %s", qq);
    long count = getCountTable2(ado, q);

    CString wkb_str = "shape";
    if (ado->m_type_of_net == TYPE_OF_NET_MYSQL) {
        wkb_str = "ST_AsWKB(shape)";
    }
    else {
        wkb_str = "shape.STAsBinary()";
    }

    q.Format("SELECT id, %s AS __wkb FROM %s", wkb_str, qq);

    if (count > 0) {
        bool is_label = false;
        bool is_label_color = false;
        bool is_label_color3 = false;
        bool is_label_date = false;
        bool is_label_stateID = false;

        if (kl->label && kl->codlabel != "") {
            MySQLColumns* col = ado->findMySQLColumns(kl->nazv, kl->codlabel);

            if (col) {
                //          q.Format("SELECT id, %s AS __label, %s AS __wkb FROM %s", kl->codlabel, wkb_str, qq);

                q1.Format(", %s AS __label FROM ", kl->codlabel);
                q.Replace(" FROM ", q1);
                is_label = true;
            }
        }

        if (kl->label_color && kl->codlabel_color != "") {
            MySQLColumns* col = ado->findMySQLColumns(kl->nazv, kl->codlabel_color);

            if (col) {
                q1.Format(", %s AS __label_color FROM ", kl->codlabel_color);
                q.Replace(" FROM ", q1);
                is_label_color = true;
            }
        }


        if (kl->label_color3 && kl->codlabel_color3 != "") {
            MySQLColumns* col = ado->findMySQLColumns(kl->nazv, kl->codlabel_color3);

            if (col) {
                q1.Format(", IIF( %s IS NULL OR %s=0, 56, 57 ) AS __label_color3 FROM ", kl->codlabel_color3, kl->codlabel_color3);
                q.Replace(" FROM ", q1);
                is_label_color3 = true;
            }
        }



         if (kl->label_stateID && kl->codlabel_stateID != "") {
             MySQLColumns* col = ado->findMySQLColumns(kl->nazv, kl->codlabel_stateID);

             if (col) {
                q1.Format(", %s AS __stateID FROM ", kl->codlabel_stateID);
                q.Replace(" FROM ", q1);
                is_label_stateID = true;
             }
         }


        //      if (kl->nazv == "remont") {
        //            q1.Format(", %s AS __label_date FROM ", "data_osmotra");
        //            q.Replace(" FROM ", q1);
        //            is_label_date = true;
        //      }

        int n_loc1 = 0, n_loc2 = 0, n_loc3 = 0;

        if (ado->openTable0(q)) {
            int n = 0;
            kl->geo4.resize(count);

            int geo_size = kl->geo4.size();

            for (int i = 0; i < geo_size; i++) kl->geo4[i] = NULL;

            while (!ado->isEOF()) {

                if (pc) {
                    pc->SetValue((int)(100. * n / geo_size));
                }

                long id = ado->read_long("id");
                COleVariant var = ado->read("__wkb");

                CCoordList cl;
                readWKB(var, cl);

                if (cl.loc == 1) {
                    n_loc1++;
                }
                else if (cl.loc == 2) {
                    n_loc2++;
                }
                else if (cl.loc == 3) {
                    n_loc3++;
                }

                CString label = "";
                long label_color = 0;
                COleDateTime label_date;

                if (is_label) {
                    label = ado->readStr("__label");
                }

                cl.is_color = false;

                if (is_label_color) {
                    label_color = ado->read_long("__label_color");
                    cl.is_color = true;
                    cl.color = getGeoColor(label_color);
                }

                if (is_label_color3) {
                    label_color = ado->read_long("__label_color3");
                    cl.is_color = true;
                    cl.color = getGeoColor(label_color);
                }


                if (is_label_date) {
                    label_date = ado->read_date("__label_date");
                    cl.is_date = true;
                    cl.date = label_date;
                }

                if (is_label_stateID) {
                    cl.stateID = ado->read_long("__stateID");
                    if (cl.stateID) {
                        cl.is_color = true;
                        cl.color = color_open_close(cl.stateID);
                    }
                }

                CFRect rect = cl.GetFRect();

                if (cl.size() != 0) {
                    x1 = rect.left;
                    y1 = rect.top;
                    x2 = rect.right;
                    y2 = rect.bottom;

                    if (kl->loc == LOC6) {
                        CFPoint p = cl.first();
                        x1 = x2 = p.x;
                        y1 = y2 = p.y;
                    }

                    if (x2 > m_frmx2 || y2 > m_frmy2 || x1 < m_frmx1 || y1 < m_frmy1) {
                        int qq;
                        qq = 1;
                        rect = cl.GetFRect();
                        //                readWKB(var, cl);
                    }

                    if (ado->m_schema != getPsAdoName()) {
                        if (x1 < m_frmx1 && fabs(x1) < 1e10) m_frmx1 = x1;
                        if (y1 < m_frmy1 && fabs(y1) < 1e10) m_frmy1 = y1;

                        if (x2 > m_frmx2 && fabs(x2) < 1e10) m_frmx2 = x2;
                        if (y2 > m_frmy2 && fabs(y2) < 1e10) m_frmy2 = y2;
                    }

                    //            m_frmx1 = min(m_frmx1, x1);
                    //            m_frmy1 = min(m_frmy1, y1);
                    //            m_frmx2 = max(m_frmx2, x2);
                    //            m_frmy2 = max(m_frmy2, y2);

                    CGeoObject* g = new CGeoObject;

                    g->nom = id;
                    g->nom2 = id;
                    g->mark = 0;
                    g->rect = CFRect(x1, y1, x2, y2);

                    g->text00000 = label;
                    g->n_label = kl->n_label;

                    g->cl = cl;
                    g->is_read = true;

                    if (n < count) {
                        kl->geo4[n] = g;
                    }
                    else {
                        kl->geo4.push_back(g);
                    }
                    n++;
                }
                ado->MoveNext();
            }
            kl->geo4.resize(n);
        }
        ado->closeTable();

        //      if (kl->loc == 3 && n_loc2 > n_loc3 && kl->loc == 3) {
        //        kl->loc = 2;
        //      }
    }

    /*
        m_frmx1 = -580168.99328512396;
        m_frmy2 = 1524813.8542242101;
        m_frmx2 = 1573930.0000000000;
        m_frmy1 = -563195.60330578510;
    */

    return true;
}

// Новая функция

bool CGeoFile::readMySql_Shape_table1(CAdoFile* ado, const char* q, Percent2* pc1, int p1, int p2)
{
    CString q1;
    double x1, x2, y1, y2;

    long count0 = getCountTable2(ado, q);

    int n_loc1 = 0, n_loc2 = 0, n_loc3 = 0;

    CString old_name = "Неправильное название таблицы";
    Klassif* kls = nullptr;

    CString str;

    str.Format(_TR("Открывается %s, подождите пожалуйста..."), ado->m_schema);

    Percent2 pc(str);

//    CInputDialog inD(AfxGetMainWnd(), "", "?", CString(q));
//    inD.DoModal();

    if (ado->openTable0(q)) {
        int n = 0;

        while (!ado->isEOF()) {

            pc.SetValue((int)(100. * n / count0));

            CString name = ado->readStr("name");

            if (name != old_name) {
                kls = m_kl_list->findKlN(name);
                old_name = name;
            }

            if (!kls) continue;

            long count = kls->geo4.size();

            long id = ado->read_long("id");
            COleVariant var = ado->read("__wkb");

            CCoordList cl;
            readWKB(var, cl);

            if (cl.loc == 1) {
                n_loc1++;
            }
            else if (cl.loc == 2) {
                n_loc2++;
            }
            else if (cl.loc == 3) {
                n_loc3++;
            }

            cl.po = ado->read_long("__po");

            CString label = ado->readStr("__label");
            long label_color = ado->read_long("__label_color");
            long label_color2 = ado->read_long("__label_color2");
            long label_color3 = ado->read_long("__label_color3");
            COleDateTime label_date = ado->read_date("__label_date");

            if (label_color != -1) {
                cl.is_color = true;
                cl.color = getGeoColor(label_color);
            }
            if (label_color2 != -1) {
                cl.is_color2 = true;
                cl.color2 = getGeoColor(label_color2);
            }

            if (label_color3 != -1) {
                cl.is_color = true;
                cl.color = getGeoColor(label_color3);
            }


            cl.stateID = ado->read_long("__stateID");
            if (cl.stateID) {
                cl.is_color = true;
                cl.color = color_open_close(cl.stateID);
            }

            if (label_date.m_dt != 0) {
                cl.is_date = true;
                cl.date = label_date;
            }

            CFRect rect = cl.GetFRect();

            if (cl.size() != 0) {
                x1 = rect.left;
                y1 = rect.top;
                x2 = rect.right;
                y2 = rect.bottom;

                if (kls->loc == LOC6) {
                    CFPoint p = cl.first();
                    x1 = x2 = p.x;
                    y1 = y2 = p.y;
                }

                if (x2 > m_frmx2 || y2 > m_frmy2 || x1 < m_frmx1 || y1 < m_frmy1) {
                    int qq;
                    qq = 1;
                    rect = cl.GetFRect();
                    //                readWKB(var, cl);
                }

                if (ado->m_schema != getPsAdoName()) {
                    if (x1 < m_frmx1 && fabs(x1) < 1e10) m_frmx1 = x1;
                    if (y1 < m_frmy1 && fabs(y1) < 1e10) m_frmy1 = y1;

                    if (x2 > m_frmx2 && fabs(x2) < 1e10) m_frmx2 = x2;
                    if (y2 > m_frmy2 && fabs(y2) < 1e10) m_frmy2 = y2;
                }


                CGeoObject* g = new CGeoObject;

                g->nom = id;
                g->nom2 = id;
                g->mark = 0;
                g->rect = CFRect(x1, y1, x2, y2);

                g->text00000 = label;
                g->n_label = kls->n_label;

                g->cl = cl;
                g->is_read = true;

                if (kls->cnt < count) {
                    kls->geo4[kls->cnt] = g;
                }
                else {
                    kls->geo4.push_back(g);
                }
                kls->cnt++;
                n++;
            }
            ado->MoveNext();
        }
    }
    ado->closeTable();

    return true;
}

#include "InputDia.h"

// Старая функция, используется после изменения классификатора

bool CGeoFile::readMySql_Shape(CAdoFile* ado)
{
    if (m_n_list == 0) {
        m_frmx1 = LONG_MAX;
        m_frmy1 = LONG_MAX;
        m_frmx2 = LONG_MIN;
        m_frmy2 = LONG_MIN;
    }

    CString str;

    str.Format(_TR("Открывается %s, подождите пожалуйста..."), ado->m_schema);

    {
        set<CString> set_tab;

        CString q = "SELECT T.name FROM   sys.objects AS T JOIN sys.columns AS C ON T.object_id = C.object_id JOIN sys.types AS P ON C.user_type_id = P.user_type_id WHERE  T.type_desc = 'USER_TABLE' AND C.name = 'shape'";
        if (ado->openTable0(q)) {
            while (!ado->isEOF()) {
                CString s = ado->readStr(0);
                s.MakeLower();
                set_tab.insert(s);
                ado->MoveNext();
            }
            ado->closeTable();
        }

        CString wkb_str;
        if (ado->m_type_of_net == TYPE_OF_NET_MYSQL) {
            wkb_str = "ST_AsWKB(shape)";
        }
        else {
            wkb_str = "shape.STAsBinary()";
        }

        q = "";

        KlList::iterator it = m_kl_list->begin();

        for (; it != m_kl_list->end(); ++it) {
            Klassif* kl = &it->second;

            if (kl->m_ado == ado) {
                CString s = kl->nazv;
                s.MakeLower();
                auto it = set_tab.find(s);

                if (it == set_tab.end()) continue;

                //            MySQLTables* tab = kl->m_ado->findMySQLTable(kl->nazv);
                //            if (!tab) continue;

                if (q != "") q += " UNION ALL\n";
                CString qq;
                CString label = " '' AS __label";
                CString label_color = " -1 AS __label_color";
                CString label_color2 = " -1 AS __label_color2";
                CString label_color3 = " -1 AS __label_color3";
                CString label_date = " NULL AS __label_date";
                CString label_stateID = " NULL AS __stateID";

                bool is_stateID = false;
                bool is_show_map = false;
                bool is_remont_id = false;

                MySQLColumns* col = ado->findMySQLColumns(kl->nazv, "show_map");

                if (col) {
                    is_show_map = true;
                }


                if (kl->nazv == "defect") {
                   MySQLColumns* colr = ado->findMySQLColumns(kl->nazv, "remontID");
                   if (colr) {
                       is_remont_id = true;
                   }
                }

                if (kl->label_stateID && kl->codlabel_stateID != "") {
                    MySQLColumns* col = ado->findMySQLColumns(kl->nazv, kl->codlabel_stateID);

                    if (col) {
                        label_stateID.Format(" %s AS __stateID", kl->codlabel_stateID);
                        label_stateID.Format(" TRY_CONVERT(int, %s) AS __stateID", kl->codlabel_stateID);
                        is_stateID = true;
                    }
                }

                if (kl->label && kl->codlabel != "") {
                    MySQLColumns* col = ado->findMySQLColumns(kl->nazv, kl->codlabel);

                    if (col) {
                        label.Format(" %s AS __label ", kl->codlabel);
                        label.Format(" TRY_CONVERT(NVARCHAR(MAX), %s) AS __label", kl->codlabel);
//                        label.Format(" CONVERT(NVARCHAR(MAX), %s) AS __label ", kl->codlabel);
                        
                    }
                }

                if (kl->label_color && kl->codlabel_color != "") {
                    MySQLColumns* col = ado->findMySQLColumns(kl->nazv, kl->codlabel_color);

                    if (col) {
                        label_color.Format(" %s AS __label_color ", kl->codlabel_color);
                        label_color.Format(" TRY_CONVERT(int, %s) AS __label_color ", kl->codlabel_color);
                    }
                }

                if (kl->label_color2 && kl->codlabel_color2 != "") {
                    MySQLColumns* col = ado->findMySQLColumns(kl->nazv, kl->codlabel_color2);

                    if (col) {
                        label_color2.Format(" %s AS __label_color2 ", kl->codlabel_color2);
                        label_color2.Format(" TRY_CONVERT(int, %s) AS __label_color2 ", kl->codlabel_color2);
                    }
                }


                if (kl->label_color3 && kl->codlabel_color3 != "") {
                    MySQLColumns* col = ado->findMySQLColumns(kl->nazv, kl->codlabel_color3);

                    if (col) {
                        label_color3.Format(" IIF( %s IS NULL OR %s=0, 56, 57 ) AS __label_color3 ", kl->codlabel_color3, kl->codlabel_color3);
                    }
                }

                if (kl->label_date && kl->codlabel_date != "") {
                    MySQLColumns* col = ado->findMySQLColumns(kl->nazv, kl->codlabel_date);

                    if (col) {
                        label_date.Format(" %s AS __label_date ", kl->codlabel_date);
                        label_date.Format(" TRY_CONVERT(date, %s) AS __label_date ", kl->codlabel_date);
                    }
                }

                CString dop = "NULL";

                if (kl->nazv == "indikator_korrozii") {
                    dop = "truboprovod";
                }

                CString tnb;

                if (is_show_map) {
                    tnb.Format("(select * from %s WHERE show_map=1 OR show_map IS NULL) _%s", kl->nazv, kl->nazv);
                }

                if (kl->nazv == "zdaniya_tu") {
                    tnb.Format("(SELECT z.*, tu.sostoyanie_dogovora FROM %s z LEFT JOIN (SELECT id,sostoyanie_dogovora,zdanie FROM tehnicheskie_usloviya WHERE sostoyanie_dogovora=4) tu ON tu.zdanie=z.id) _%s", kl->nazv, kl->nazv);


tnb = 

"(SELECT z.*, t.code FROM zdaniya_tu z\n"
"LEFT JOIN (\n"
"SELECT\n"
"zdanie, \n"
"SUM(POWER(2, sostoyanie_dogovora-1)) AS code\n"
"FROM (\n"
"SELECT DISTINCT sostoyanie_dogovora, zdanie FROM tehnicheskie_usloviya WHERE sostoyanie_dogovora IS NOT NULL AND sostoyanie_dogovora > 0\n"
") tu\n"
"WHERE zdanie IS NOT NULL AND zdanie <> 0\n"
"GROUP BY zdanie\n"
") t ON t.zdanie=z.id\n ) _zdaniya_tu";



//                    CString codlabel_color = "sostoyanie_dogovora";
//                    label_color.Format(" %s AS __label_color ", codlabel_color);
//                    label_color.Format(" TRY_CONVERT(int, %s) AS __label_color ", codlabel_color);
//                    label_color = " IIF (sostoyanie_dogovora = 4, 1001, 1000)  AS __label_color";
//                    label_color = " IIF (code = 4, 1001, 1000)  AS __label_color";
                    label_color = " IIF (code IS NULL OR code = 0, 0, code)+2000  AS __label_color";
                }


//                else if (is_remont_id) {
//                    tnb.Format("(select * from %s WHERE remontID IS NULL) _%s", kl->nazv, kl->nazv);
//                }
                else {
                    tnb.Format("[%s]", kl->nazv);
                }

                if (is_show_map && is_remont_id) {
//                    tnb.Format("(select * from %s WHERE (show_map=1 OR show_map IS NULL) AND remontID IS NULL) _%s", kl->nazv, kl->nazv);
                }



                qq.Format("SELECT id,%s,%s,%s,%s,%s,%s,'%s' AS name,%s AS __wkb, %s AS __po FROM %s", label_stateID, label, label_color, label_color2, label_color3, label_date, kl->nazv, wkb_str, dop, tnb);

                q += qq;
            }
        }

        CString q2;

        {
            KlList::iterator it = m_kl_list->begin();
            for (; it != m_kl_list->end(); ++it) {
                Klassif* kl = &it->second;
                kl->cnt = -1;
            }
        }

        q2.Format("SELECT name, COUNT(*) AS cnt FROM (%s\n) _QQ GROUP BY name", q);

        if (ado->openTable0(q2)) {
            while (!ado->isEOF()) {
                CString name = ado->readStr("name");
                int cnt = ado->read_long("cnt");

                Klassif* kls = m_geofile->m_kl_list->findKlN(name);
                if (kls) {
                    kls->cnt = 0;
                    kls->geo4.resize(cnt);
                    for (int i = 0; i < cnt; i++) kls->geo4[i] = NULL;
                }

                ado->MoveNext();
            }
            ado->closeTable();
        }


        //      CInputDialog inD(AfxGetMainWnd(), "", "?", q);
        //      inD.DoModal();

        readMySql_Shape_table1(ado, q);

        {
            KlList::iterator it = m_kl_list->begin();
            for (; it != m_kl_list->end(); ++it) {
                Klassif* kl = &it->second;
                if (kl->cnt >= 0) {
                    kl->geo4.resize(kl->cnt);
                }
            }
        }
    }

    return true;

    Percent2 pc(str);
    int ii = 0;

    KlList::iterator it = m_kl_list->begin();

    for (; it != m_kl_list->end(); ++it) {
        Klassif* kl = &it->second;

        pc.SetValue((int)(100. * ii / m_kl_list->size()));
        ii++;

        if (kl->m_ado == ado) {
            readMySql_Shape_table(ado, kl);
        }
    }

    return true;
}


bool CGeoFile::openMySQL(int type_of_net, const char* server, int port, const char* database, const char* user, const char* password, bool special)
{
    CString fn = "MySQL";
    m_fn = fn;

    if (!database || database[0] == 0) return false;

    CString klfn, klfn_all, klfn2;
    CString database2 = database;

    void open_special_sk(const char* db_name);


    if (database == getGidAdoName()) {
        open_special_sk(database);
    }

    initStruct1();

    initTableRusName(database, database);
    initColumnRusName(database, database);
    initLookup(database, database);

    if (database == getGidAdoName()) {
        initTableRusName(database, "GID");
        initColumnRusName(database, "GID");
        initLookup(database, "GID");
    }

    initTableRusName(database, "OpenStreetMap");

    klfn.Format("%skls\\%s.kls2", argpath(), database);
    klfn_all.Format("%s%s.kls2", argpath_2(), database2);

    klfn2 = klfn;

    if (database == getGidAdoName()) {
        klfn2.Format("%skls\\GID.kls2", argpath());
    }

    special = FALSE;

    if (special) open_special(klfn);

    CString m_geo_coord = GetFName(fn) + ".coord";

    FILE* f = fopen(m_geo_coord, "r");
    if (f) {
        CFPoint p1[3], p2[3];

        for (int i = 0; i < 3; i++) {
            fscanf(f, "%lg", &p2[i].x);
            fscanf(f, "%lg", &p2[i].y);
            fscanf(f, "%lg", &p1[i].x);
            fscanf(f, "%lg", &p1[i].y);
        }
        preobr(p1, p2, m_geo_x);
        m_is_coord = TRUE;
        fclose(f);
    }

    if (!m_kl_list) m_kl_list = new KlList();

    CString ext = GetExt(fn);
    CString name = GetFName(fn);

    if (m_name == "") {
        m_name = name;
    }

    is_odbc = FALSE;

    CAdoFile* ado = new CAdoFile;

    if (ado->openMySQL(type_of_net, server, port, database, user, password)) {
        list<CString> list_tables;

        CString q = "SELECT s.name AS TABLE_SCHEMA, t.name AS TABLE_NAME, c.name AS COLUMN_NAME FROM sys.columns c JOIN sys.tables t ON t.object_id=c.object_id JOIN sys.schemas s ON s.schema_id=t.schema_id WHERE c.user_type_id=129";
        if (ado->m_type_of_net == TYPE_OF_NET_MYSQL) {
            //      q.Format("SELECT TABLE_NAME FROM information_schema.ST_GEOMETRY_COLUMNS WHERE lower(TABLE_SCHEMA)=lower('%s')", database);
            q.Format("SELECT TABLE_NAME FROM information_schema.COLUMNS WHERE TABLE_SCHEMA = '%s' AND COLUMN_TYPE = 'geometry'", database);
        }

        if (ado->openTable0(q)) {
            while (!ado->isEOF()) {

                CString table_name = ado->readStr("TABLE_NAME");
                CString ss = table_name;
                ss.MakeLower();

                if (ss != "nodes" && ss != "linesobj") {
                    bool yes = true;

                    if (special) {
                        if (find_special(ss)) {
                            yes = false;
                        }
                    }
                    if (yes) {
                        list_tables.push_back(table_name);
                    }
                }
                ado->MoveNext();
            }
            ado->closeTable();
        }

        list<CString>::const_iterator it = list_tables.begin();

        for (; it != list_tables.end(); it++) {
            CString table_name = *it;
            CString q;

            //      ado->MySQLTableInfo(table_name);
            //      MySQLColumns *col = ado->findMySQLColumns(table_name, "shape");

            //      if (col) {
            if (1) {
                //        if (fld.yes && (fld.typ == adLongVarBinary || fld.typ == adVarWChar)) {
                Klassif kl;
                kl.m_ado = ado;
                kl.m_database = database;

                kl.loc = 3;

                kl.nazv = GetTitle(table_name);
                kl.nazv_alias = kl.nazv;

                kl.prio = 1;
                kl.typ = 1;
                kl.ins = 1;
                kl.ins2 = 1;
                kl.punct = 0;
                kl.is_edit = 1;
                kl.is_potr = 0;
                kl.label = 0;
                kl.label_color = 0;
                kl.codlabel = "";
                kl.codlabel_color = "";
                kl.coord_name = "";
                kl.lf.lfFaceName[0] = 0;
                kl.coli = 0xFF0000;
                kl.colb = 0xFFFFFF;
                kl.col3 = 0x007F00;
                kl.ramka = 0;

                //          kl.m_db = NULL;
                kl.m_shp = NULL;
                kl.is_shpdbf = false;

                //          m_kl_list[kl.nazv] = kl;
                m_kl_list->insert(pair<CString, Klassif>(kl.nazv, kl));
            }
        }

        if (klfn2 != klfn) m_kl_list->open2(klfn2);

        Klassif* kls1 = m_kl_list->findKlN("indikator_korrozii");

        m_kl_list->open2(klfn);

        Klassif* kls2 = m_kl_list->findKlN("indikator_korrozii");

        m_kl_list->open2(klfn_all);

        Klassif* kls3 = m_kl_list->findKlN("indikator_korrozii");

        readMySql_Shape(ado);

        m_n_list++;
        m_isOpen = true;

        CLeft1Dialog* left1 = (CLeft1Dialog*)getDockablePane(IDD_LEFT1);
        if (left1) {
            left1->SetKls(m_geofile->m_kl_list);
            m_geofile->initGeoTree(left1->m_ctlTree);
        }
    }

    return true;
}
