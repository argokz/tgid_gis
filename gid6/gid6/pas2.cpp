#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
#include "mmenu.h"
#include "maptabs.h"
#include "sort_graph.h"
#include "win.h"
#include "ps_alma.h"
#include "css.h"
#include "html.h"
#include "ado.h"

#include <locale.h>
#include <fstream>
#include "klassif.h"
#include "coordlis.h"
#include <iostream>
#include <thread>
#include <future>

//#include "async-await/async_task.hpp"
#include "sstream"
using namespace std;
//using namespace zb;


CString readQ(CAdoFile* ado, const char* tn, const char* tn2, int fileID);
void make_big_ut(CGraph2* graph, list<CLINE2*>& st_l, list<CNode2*>& list_sort, list <list<int> >& list_ut);
void printTubes(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString  fragments_nums);
void printMechanicalZa(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString  fragments_nums);
void printMechanicalKk(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString  fragments_nums);
void printChannels(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString  fragments_nums);
void printChambers(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString  fragments_nums);
void printPavilions(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString  fragments_nums);
void printSupports(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString  fragments_nums);
void printSpecConstructions(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString  fragments_nums);
void printIsolations(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString  fragments_nums);
void printRemonts(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString  fragments_nums);
void printRespPersons(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString  fragments_nums);
void printDefects(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString  fragments_nums);
void printPits(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString  fragments_nums);
void printClippings(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString  fragments_nums);
void printTrials(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString  fragments_nums);
void printExamination(FILE* g, FILE* f, int ms_rs, int id, CAdoFile* mAdo, CString  fragments_nums);

int find_geo_line(CCoordList& cl, list<int>& list_geo, Klassif* kls)
{
    double md = LEN100;

    if (!kls) return -1;

    CFRect rectAll = cl.GetFRect();
    rectAll.InflateRect(md, md);

    CFRect rect, r;

//  m_geofile->m_kls000 = kls;

    int nn = kls->NObj();

    for (int i = 1; i <= nn; i++) {
        m_geofile->GetFRect(kls, i, rect);

        if (r.IntersectRect(rect, rectAll)) {

            //      CFPoint pt = cl.first();
            CFPoint pt = rect.BottomRight();
            double len = cl.Length2(pt, 2);

            if (len < md) {
                len = cl.Length2(pt, 2);
                list_geo.push_back(i);
            }
        }
    }
    return list_geo.size();
}

CNode2* find_zn0(CCxema* m_cxema, CNode2* node, double& length);

//const RS *getRS();
const MAG* getMAG();

// Пересчитывает в количество по участку

void upr(list<vector<CString> >& v1);

struct MAP_GEO
{
    map<CLINE2*, list<int> > map_geo;
};

CFRect getRect(CLINE2* l);

void print_sheet0_2(VYD_LINES& vyd_l);

void print_sheetRemont2(CGraph2* graph, FILE* g, list <list<int> >& list_ut);
void print_sheetShurf2(FILE* f, VYD_LINES& vyd_l, CNode2* nr, CNode2* nm);
//void print_sheetKapital(HTML &h, VYD_LINES &vyd_l, CNode *nr, CNode *nm);

void print_sheet3_2(CGraph2* graph, FILE* f, VYD_LINES& vyd_l, CNode2* nr, CNode2* nm);
void print_sheet4_2(CGraph2* graph, FILE* f, VYD_LINES& vyd_l, CNode2* nr, CNode2* nm);
void print_sheet5_2(CGraph2* graph, FILE* f, VYD_LINES& vyd_l, CNode2* nr, CNode2* nm);
void print_sheet6_2(CGraph2* graph, FILE* f, VYD_LINES& vyd_l, CNode2* nr, CNode2* nm);
void print_sheet7_2(CGraph2* graph, FILE* f, VYD_LINES& vyd_l, CNode2* nr, CNode2* nm);
void print_sheet8_2(CGraph2* graph, FILE* f, VYD_LINES& vyd_l, CNode2* nr, CNode2* nm);
void print_sheet9_2(CGraph2* graph, FILE* f, int nomer_uch);

void print_sheet3_2(CGraph2* graph, FILE* f, list <list<int> >& list_ut);
void print_sheet4_2(CGraph2* graph, FILE* g, list <list<int> >& list_ut);
void print_sheet5_2(CGraph2* graph, FILE* f, list<CNode2*>& list_sort);




void print_head(FILE* f, const char* title);
void print_index1(FILE* f, const char* title);
void print_index(FILE* f, const char* title, const char* fn);
void print_index2(FILE* f);
void print_doc_end(FILE* f);
void printTd(CAdoFile* ado, FILE* f, const char* param, const int type, int count = 1);
void printTd(CAdoFile* ado, FILE* f, int id, const int type, int count = 1);
FILE* print_doc(FILE* fi, const char* fn, const char* title, void (*filtr) (CString& s1) = NULL);

CGidrView* getView();

CString getV(map<CString, CString>& map_RE, CString id);
void htmlReplace(map<CString, CString>& map_val, CString& str, CString s1, CString tn, CString fn);

#include "join2.h"

FILE* print_doc_main(FILE* fi, const char* fn, const char* fn2, const char* title, const char* table, int is_rs, int kod);


void print_td_povt(HTML& h, int n);

void print_ut22(FILE* g, CLINE2* l)
{
    CNode2* n1 = where(l);
    CNode2* n2 = other(l);

    if (napr(l) * bline(l)->line.pod.q > 0) {
        CNode2* n = n1; n1 = n2; n2 = n;
    }

    fprintf(g, "<td>%s</td>", n1->getNamePT());
    fprintf(g, "<td>%s</td>", n2->getNamePT());
}

CString getTubingTypeID(int tubingTypeID);
void print_ut(FILE* g, CLINE2* l);

void print_ut_2(FILE* g, list<int>& lst_ut)
{

    /**
    fprintf(g, "<tr>");

        CNode2* n1 = where(l);
        CNode2* n2 = other(l);

        if (napr(l) * bline(l)->line.pod.q < 0) {
            CNode2* n = n1; n1 = n2; n2 = n;
        }

        fprintf(g, "<td>%s</td>", n1->getNamePT());
        fprintf(g, "<td>%s</td>", n2->getNamePT());

        //  double diamP = bline(l)->line.pod.diam_usl;
        //  double diamO = bline(l)->line.obr.diam_usl;

        double diamP = bline(l)->line.pod.diam_vn;
        double diamO = bline(l)->line.obr.diam_vn;

        fprintf(g, "<td>%g</td>", diamP);
        fprintf(g, "<td>%g</td>", bline(l)->line.pod.dl);
        fprintf(g, "<td>%g</td>", diamO);
        fprintf(g, "<td>%g</td>", bline(l)->line.obr.dl);

        fprintf(g, "<td>%g</td>", bline(l)->line.pod.tol);
        fprintf(g, "<td>%g</td>", bline(l)->line.obr.tol);
        fprintf(g, "<td>%g</td>", bline(l)->line.pod.v); // Исправил w на v
        fprintf(g, "<td>%g</td>", bline(l)->line.obr.v);
        fprintf(g, "<td>%s</td>", getTubingTypeID(bline(l)->line.tubingTypeID));


        fprintf(g, "</tr>");
    */
}

void copyVydGraph_new(CGraph2* graph1, CGraph2* graph2, bool is_po = false);
void sort_line_rs_new(CGraph2* graph, list<CNode2*>& list_sort, list<CLINE2*>& st_l, int fileID = -1);

CLINE2* get_max(CGraph2* graph, list<int>& list_ut_min, double& dlP, double& dlO, double& qP, double& qO);

bool get_first_last(CGraph2* graph, list<int>& list_ut_min, CNode2*& n_first, CNode2*& n_last);
bool get_first_last_cl(CGraph2* graph, list<int>& list_ut_min, CNode2*& n_first, CNode2*& n_last, CCoordList& cl);


static CSS css;


#include <errno.h>

void reset_ms_rs(CAdoFile* ado, CGraph2* graph, bool ms, int ms_rs);


bool RunProcessWithoutWait(const std::string& programPath);
int MyExec(const CString& commandLine, CString &error);

void Run(CWnd* win, const char* cmd, const char* prtn, int id);


void run_passport(CWnd *wnd, CCxema *m_cxema, int ms_rs, int id)
{
#if 0
    CString path, error, msrs = ms_rs == 0 ? "ms" : "rs";
    path.Format("%s\\passport\\p_%s_%d.xlsx", getenv("TEMP"), msrs, id);

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

    CString cmd;
    cmd.Format("\"%s\" \"D:\\Qt\\gid8\\python\\passport\\p.py\"   -rdbms MsSql -server %s -database %s -port %d -user %s -password %s   -id %d -type %s -fragments=%s  -out_file \"%s\"",
        python_exe(), pDoc->m_IP, pDoc->m_bd_gid, pDoc->m_port, pDoc->m_user, pDoc->m_password,
        
        id, msrs, m_cxema->m_par, path);

//    int rc = MyExec(cmd, error);

    Run(wnd, cmd, argpath_2() + "protocol.txt", ID_END_PASSPORT);

/**
    if (rc == 0) {
        HINSTANCE hRun = ShellExecute (wnd->GetSafeHwnd (), "open", "excel", path, NULL, SW_SHOWMAXIMIZED);
    }
    else {
        AfxMessageBox(error);
    }
*/

#endif
    return;
}

static CString m_path = "";

void CGidrView::OnEndPassport()
{
    if (IsFile(m_path)) {
        HINSTANCE hRun = ShellExecute (this->GetSafeHwnd (), "open", "excel", m_path, NULL, SW_SHOWMAXIMIZED);
    }
    else {
        AfxMessageBox("Ошибка");
    }
}

CString python_str(const CString& prog, const CString& args);


void CGidrView::PaspRSMS_2(CNode2* nl, CNode2* n1, int ms_rs, int id)
{

    CString args, cmd, path, error, msrs = ms_rs == 0 ? "ms" : "rs";
    path.Format("%s\\passport\\p_%s_%d.xlsx", getenv("TEMP"), msrs, id);

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

//    args.Format("-rdbms MsSql -server %s -database %s -port %d -user %s -password %s -id %d -type %s -fragments=%s -out_file \"%s\"",
//        pDoc->m_IP, pDoc->m_bd_gid, pDoc->m_port, pDoc->m_user, pDoc->m_password,
//        id, msrs, m_cxema->m_par, path);

    
    args.Format("-rdbms MsSql -server %s -database %s -port %d -user %s -id %d -type %s -fragments=%s -out_file \"%s\" -encoding cp866",
        pDoc->m_IP, pDoc->m_bd_gid, pDoc->m_port, pDoc->m_user, 
        id, msrs, m_cxema->m_par, path);

    const char* password = "";
    if (_putenv_s("tgid_password", pDoc->m_password) != 0) {
        log1("Ошибка при установке переменной окружения.");
    }

    cmd = python_str("passport\\p.py", args);

    log1(cmd);

//    cmd.Format("\"%s\" -u \"D:\\Qt\\gid8\\python\\passport\\p.py\"  -rdbms MsSql -server %s -database %s -port %d -user %s -password %s -id %d -type %s -fragments=%s -out_file \"%s\"",
//        python_exe(), pDoc->m_IP, pDoc->m_bd_gid, pDoc->m_port, pDoc->m_user, pDoc->m_password,
//        id, msrs, m_cxema->m_par, path);

    Run(this, cmd, argpath_2() + "protocol.txt", ID_END_PASSPORT);
    m_path = path;


//    run_passport(this, m_cxema, ms_rs, id);
    return;

    
    /**
    auto tk = task::async([] {
        this_thread::sleep_for(10000ms);
        std::stringstream ss;
        ss << std::this_thread::get_id();
        uint64_t id = std::stoull(ss.str());
        CString s1;
        s1.Format("I'll go second on thread %d", id);
        })
        .await([&]()->void {
            });
*/

    CWaitCursor wait;

    if (ms_rs == 0) {
        reset_ms_rs(m_cxema->m_ado, m_cxema->m_graph, true, id);
    }
    else if (ms_rs == 1) {
        reset_ms_rs(m_cxema->m_ado, m_cxema->m_graph, false, id);
    }

    CString tmpName;
    //  tmpName.Format("%s\\index.html", getenv("TMP"));
    //  tmpName.Format("%s\\html\\2\\index.html", argpath());
    tmpName.Format("%s\\index2.html", getenv("TMP"));

    //  if (!kod) return;

    //  if (m_id_MS) is_rs = 0;
    //  if (m_id_RS) is_rs = 1;


    FILE* f = fopen(tmpName, "w");
    if (!f) {
        //      AfxMessageBox(strerror(errno), MB_OK|MB_ICONINFORMATION);
        AfxMessageBox("Закройте предыдущий паспорт", MB_OK|MB_ICONINFORMATION);
        return;
    }

    setlocale(LC_NUMERIC, "");

    print_index1(f, "");

    FILE* g = 0;

    m_cxema->m_graph->save_pjezo();

    VYD_LINES vyd_l;
    vyd_l.init2(m_cxema->m_graph, false, false);


    list <list<int> > list_ut;
    list<CLINE2*> st_l;
    list<CNode2*> list_sort;

    CGraph2 graph2(NULL);

    m_cxema->m_graph->restore_pjezo();

    copyVydGraph_new(m_cxema->m_graph, &graph2);
    sort_line_rs_new(&graph2, list_sort, st_l);

    make_big_ut(m_cxema->m_graph, st_l, list_sort, list_ut);

    m_cxema->m_graph->restore_pjezo();

    /*
      map<int, CLINE2*>::const_iterator it = vyd_l.map_line.begin();
      for (; it != vyd_l.map_line.end(); it++) {
        CLINE2 *l = it->second;
        CNode2 *n1 = where(l);
        CNode2 *n2 = other(l);
      }
    */

    if (id > 0) {
        //    g = print_doc_main(f, "titul_kar0.html", "Common1.htm", _TR("Заголовок"), _TR("Участок РС"), ms_rs,  id);  // Распредсеть
        //    print_doc_end(g);

        //    g = print_doc_main(f, "titul_kar.html", "Common2.htm", _TR("Общая характеристика2"), _TR("Участок МС"), ms_rs,  id);  // Распредсеть
        //    print_doc_end(g);

        if (ms_rs == 0) {
            g = print_doc_main(f, "CommonA.htm", "Common3.htm", _TR("Общая хар-ка"), _TR("Участок МС"), ms_rs, id);
            print_doc_end(g);
        }
        else if (ms_rs == 1) {
            g = print_doc_main(f, "CommonB.htm", "Common4.htm", _TR("Общая хар-ка"), _TR("Участок РС"), ms_rs, id);  // Распредсеть
            print_doc_end(g);
        }
    }

    CString fragments_nums = m_cxema->getParAll();

    /*Форма Трубы*/
    printTubes(g, f, ms_rs, id, m_cxema->m_ado, fragments_nums);
    /*Форма Механическое оборудование запорная арматура*/
    printMechanicalZa(g, f, ms_rs, id, m_cxema->m_ado, fragments_nums);
    /*Форма Механическое оборудование колодцы компенсаторы*/
    printMechanicalKk(g, f, ms_rs, id, m_cxema->m_ado, fragments_nums);
    /*Форма Каналы*/
    printChannels(g, f, ms_rs, id, m_cxema->m_ado, fragments_nums);
    /*Форма  Камеры*/
    printChambers(g, f, ms_rs, id, m_cxema->m_ado, fragments_nums);
    /*Форма  Павильоны*/
    printPavilions(g, f, ms_rs, id, m_cxema->m_ado, fragments_nums);
    /*Форма  Опоры*/
    printSupports(g, f, ms_rs, id, m_cxema->m_ado, fragments_nums);
    /*Форма  Спец конструкции*/
    printSpecConstructions(g, f, ms_rs, id, m_cxema->m_ado, fragments_nums);
    /*Форма  Изоляция*/
    printIsolations(g, f, ms_rs, id, m_cxema->m_ado, fragments_nums);
    /*Форма  Ответственные лица*/
    printRespPersons(g, f, ms_rs, id, m_cxema->m_ado, fragments_nums);
    /*Форма  Ремонт*/
    printRemonts(g, f, ms_rs, id, m_cxema->m_ado, fragments_nums);
    /*Форма  Дефект*/
    printDefects(g, f, ms_rs, id, m_cxema->m_ado, fragments_nums);
    /*Форма  Шурфовки*/
    printPits(g, f, ms_rs, id, m_cxema->m_ado, fragments_nums);
    /*Форма  Вырезки*/
    printClippings(g, f, ms_rs, id, m_cxema->m_ado, fragments_nums);
    /*Форма  Испытания*/
    printTrials(g, f, ms_rs, id, m_cxema->m_ado, fragments_nums);
    /*Форма  Освилетельствование*/
    printExamination(g, f, ms_rs, id, m_cxema->m_ado, fragments_nums);
    print_index2(f);
    fclose(f);
    setlocale(LC_NUMERIC, "eng");
    //  tmpName.Format("%s\\html\\2\\index2.html", argpath());
    tmpName.Format("%s\\index2.html", getenv("TMP"));
    //  tmpName.Format("%s\\html\\2\\index.html", argpath());


    CString cssName;
    cssName.Format("%s\\style.css", getenv("TMP"));
    css.print(cssName);
    HINSTANCE hRun = ShellExecute(GetSafeHwnd(), "open", "excel", "\"" + tmpName + "\"", NULL, SW_SHOWMAXIMIZED);
}

void CGidrView::OnPasp2()
{
    //  if (m_ms_kod0 <= 0 && m_rs_kod0 <= 0) return;

    int id = 0, ms_rs = 0;

    if (m_ms_kod0 > 0) {
        id = m_ms_kod0;
        ms_rs = 0;
    }
    else if (m_rs_kod0 > 0) {
        id = m_rs_kod0;
        ms_rs = 1;
    }

    PaspRSMS_2(0, 0, ms_rs, id);
}


void CGidrView::OnPaspKam2()
{
    CString tmpName;

    tmpName.Format("%s\\html\\birka.html", argpath());


    HINSTANCE hRun = ShellExecute(GetSafeHwnd(), "open", "excel", "\"" + tmpName + "\"", NULL, SW_SHOWMAXIMIZED);
}

bool getVydRectAll(CGraph2* graph, CFRect& rect0);
void copyVydGraphAll(CGraph2* graph1, CGraph2* graph2, list<CNode*>& list_sort, list<CLINE2*>& list_l, int mag_typ, int mag_id);


double getLengthLines(CNode* n, CFPoint pt);
double getLengthLines2(CNode2* n, CFPoint pt, CLINE2* vl);

int find_geo_line(CLINE2* vl, list<int>& list_geo, Klassif* kls);
int find_geo_node(CNode2* node, list<int>& list_geo, Klassif* kls);

int print_geo(FILE* f, list<int>& list_geo, Klassif* kls);


int makeGeoTemp(CGraph2* graph, Klassif* kls, CString& vyd);

#if 0

void read_geo(list<int>& list_geo, Klassif* kls, vector<CString>& list_t, list<vector<CString> >& list_vv)
{
    list_vv.clear();

    CAdoFile* ado = kls->m_ado;


    CString vyd = "";


    if (ado->openTable0(kls->TableName())) {
        list<int>::const_iterator it = list_geo.begin();
        for (; it != list_geo.end(); it++) {
            vector<CString> list_v;
            if (ado->GotoKey(kls->geo4[*it - 1]->nom)) {
                for (int i = 0; i < list_t.size(); i++) {
                    CString v = ado->read(list_t[i]);
                    list_v.push_back(v);
                }
            }
            list_vv.push_back(list_v);
        }

        it = list_geo.begin();
        for (; it != list_geo.end(); it++) {
            vector<CString> list_v;
            if (ado->GotoKey(kls->geo4[*it - 1]->nom)) {
                for (int i = 0; i < list_t.size(); i++) {
                    CString v = ado->read(list_t[i]);
                    CString pod_obr = ado->read(_TR("Признак трубопровода"));
                    if (pod_obr == _TR("общий")) {
                        list_v.push_back(v);
                    }
                }
            }
            if (list_v.size() > 0) {
                list_vv.push_back(list_v);
            }
        }
        ado->closeTable();
}
    }
#endif



void read_geo(list<int>& list_geo, Klassif* kls, vector<CString>& list_t, list<vector<CString> >& list_vv);


void print_td_povt(FILE* f, int n);
void print_td_n(FILE* f, int n);
void print_td_n(FILE* f, int n, BOOL yes);


Klassif* findKlN(const char* txt);

int num_po(const char* po1);
int kolich(const char* po, const char* kol);

void initLookup2(CAdoFile* ado, const char* tn, const char* fn, map<long, CString>& map_v)
{
    CString q;

    q.Format("SELECT ID, %s FROM  %s", fn, tn);

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long("ID");
            CString name = ado->readStr(fn);
            map_v[id] = name;
            ado->MoveNext();
        }
    }
}


CString getReal2(map<long, CString>& map_v, const char* ids)
{
    long id;

    if (sscanf(ids, "%d", &id)) {
        auto it = map_v.find(id);
        if (it != map_v.end()) {
            return it->second;
        }
    }
    return "";
}



#if 0

void print_sheet3_2(CGraph2* graph, FILE* f, VYD_LINES& vyd_l, CNode2* nr, CNode2* nm)
{
#if 1
    Klassif* klsD = findKlN(transl("Дренажный кран"));
    Klassif* klsV = findKlN(transl("Воздушник"));
    Klassif* klsZ = findKlN(transl("Запорная арматура"));
    Klassif* klsK = findKlN(transl("Компенсатор"));

    if (!klsD || !klsV || !klsZ || !klsK) {
        //    AfxMessageBox(_TR("Нет файла геобазы"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    MFIELDS map1, map2;
    //  InitTab(klsK->TableName(), klsK->TableName(), false);
    getMap(klsK->TableName(), transl("Тип компенсатора"), map1);

    //  InitTab(klsZ->TableName(), klsZ->TableName(), false);
    getMap(klsZ->TableName(), transl("Назначение"), map2);

    CAdoFile* ado = getAdo(getPsAdoName());

    map<long, CString> map_z;

    initLookup2(ado, "naznachenie_zapornoy_armatury_setey_teplosnabzheniya", "naimenovanie", map_z);



    //  map<int, CLINE2*>::const_iterator it = vyd_l.map_num_line.begin();

    list<int> list_geoD;
    list<int> list_geoV;
    list<int> list_geoZ;
    list<int> list_geoK;

    CNode2* n0 = NULL;

    //for (; it != vyd_l.map_num_line.end(); it++) {

    for (map<int, CLINE2*>::const_iterator it = vyd_l.map_line.begin(); it != vyd_l.map_line.end(); it++) {
        CLINE2* vl = it->second;
        CNode2* n1 = where(vl);
        CNode2* n2 = other(vl);

        if (1 || n1->typPS() != -1 || inc(n1) != 2 || nr && n1->node.node_name == nr->node.node_name) {
            n0 = n1;
            list_geoD.clear();
            list_geoV.clear();
            list_geoZ.clear();
            list_geoK.clear();
        }

        int nD = find_geo_line(vl, list_geoD, klsD);
        int nV = find_geo_line(vl, list_geoV, klsV);
        int nZ = find_geo_line(vl, list_geoZ, klsZ);
        int nK = find_geo_line(vl, list_geoK, klsK);
        //    int nU = find_geo_line(vl, list_geoU, klsU);

        if (1 || n2->typPS() != -1 || inc(n2) != 2) {
            CString tZ[] = { transl("Назначение"), transl("Диаметр"), transl("Материал"), transl("Тип отключения"), transl("Признак трубопровода") };
            vector<CString> ttZ(tZ, tZ + 5);
            list<vector<CString> > list_vvZ;
            read_geo(list_geoZ, klsZ, ttZ, list_vvZ);

            CString tV[] = { transl("Диаметр"), transl("Признак трубопровода") };
            vector<CString> ttV(tV, tV + 2);
            list<vector<CString> > list_vvV;
            read_geo(list_geoV, klsV, ttV, list_vvV);

            CString tD[] = { transl("Диаметр"), transl("Признак трубопровода") };
            vector<CString> ttD(tD, tD + 2);
            list<vector<CString> > list_vvD;
            read_geo(list_geoD, klsD, ttD, list_vvD);

            CString tK[] = { transl("Тип компенсатора"), transl("Диаметр трубопровода"), transl("Признак трубопровода") };
            vector<CString> ttK(tK, tK + 3);
            list<vector<CString> > list_vvK;
            read_geo(list_geoK, klsK, ttK, list_vvK);

            upr(list_vvZ);
            upr(list_vvV);
            upr(list_vvD);
            upr(list_vvK);

            nZ = list_vvZ.size();
            nV = list_vvV.size();
            nK = list_vvK.size();
            nD = list_vvD.size();

            int nn = max(max(nZ, nK), max(nD, nV));

            list<vector<CString> >::const_iterator itD = list_vvD.begin();
            list<vector<CString> >::const_iterator itV = list_vvV.begin();
            list<vector<CString> >::const_iterator itZ = list_vvZ.begin();
            list<vector<CString> >::const_iterator itK = list_vvK.begin();

            for (int i = 0; i < nn; i++) {

                fprintf(f, "<tr>");

                if (i == 0) {
                    print_ut22(f, vl);
                }
                else {
                    fprintf(f, "<td>&nbsp;</td>");
                    fprintf(f, "<td>&nbsp;</td>");
                }

                //      int nnn =  (nO, nD, nV, nZ, nK, nU);

                     // Запорная арматура         

                if (itZ != list_vvZ.end()) {
                    CString ss = getReal2(map_z, (*itZ)[0]);
                    fprintf(f, "<td>%s</td>", ss);  // Назначение
                    fprintf(f, "<td>%s</td>", (*itZ)[1]);  // Условный диаметр, мм

                    CString sn = (*itZ)[4];

                    int nn = atoi(sn);

                    CString material = (*itZ)[2];
                    CString tip_otkl = (*itZ)[3];

                    int is_chugun = material.Find(transl("чугун")) != -1;

                    int typ = 1;

                    print_td_n(f, nn, is_chugun);   // Чугунных
                                                  // Стальных
                    print_td_n(f, nn, !is_chugun && typ == 1);   // с ручным приводом
                    print_td_n(f, nn, !is_chugun && typ == 2);   // электроприводом
                    print_td_n(f, nn, !is_chugun && typ == 3);   // с гидроприводом
                }
                else {
                    print_td_povt(f, 6);
                }
                // Компенсаторы   

                if (itK != list_vvK.end()) {
                    map<CString, CString>::const_iterator it = map1.map1.find((*itK)[0]);

                    if (it != map1.map1.end()) {
                        fprintf(f, "<td>%s</td>", it->second);  // Конструкция
                    }
                    else {
                        fprintf(f, "<td>%s</td>", (*itK)[0]);  // Конструкция
                    }
                    fprintf(f, "<td>%s</td>", (*itK)[1]);  // Условный диаметр,  мм
                    fprintf(f, "<td>%d</td>", kolich((*itK)[2], (*itK)[3]));  // Количество, шт.
                }
                else {
                    print_td_povt(f, 3);
                }

                // Дренажные краны

                if (itD != list_vvD.end()) {
                    fprintf(f, "<td>%s</td>", (*itD)[0]);  // Условный диаметр, мм
                    fprintf(f, "<td>%d</td>", kolich((*itD)[1], (*itD)[2]));  // Количество, шт
                }
                else {
                    print_td_povt(f, 2);
                }
                // Воздушники

                if (itV != list_vvV.end()) {
                    fprintf(f, "<td>%s</td>", (*itV)[0]);  // Условный диаметр, мм
                    fprintf(f, "<td>%d</td>", kolich((*itV)[1], (*itV)[2]));              // Количество, шт.
                }
                else {
                    print_td_povt(f, 2);
                }

                fprintf(f, "\n");

                if (itD != list_vvD.end()) itD++;
                if (itV != list_vvV.end()) itV++;
                if (itZ != list_vvZ.end()) itZ++;
                if (itK != list_vvK.end()) itK++;
        }
    }
}
#endif
}

#endif




void print_pav(FILE* f, const char* tn, CNode2* n);




void print_sheet6_2(CGraph2* graph, FILE* f, VYD_LINES& vyd_l, CNode* nr, CNode* nm)
{
#if 1
    Klassif* klsD = findKlN(transl("Дренажный кран"));
    Klassif* klsV = findKlN(transl("Воздушник"));
    Klassif* klsZ = findKlN(transl("Запорная арматура"));
    Klassif* klsK = findKlN(transl("Компенсатор"));

    if (!klsD || !klsV || !klsZ || !klsK) {
        //    AfxMessageBox(_TR("Нет файла геобазы"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    //  MFIELDS map1;
    //  InitTab(klsK->m_db->m_strFileName, klsK->TableName(), false);
    //  getMap(klsK->TableName(), _TR("Тип компенсатора"), map1);

    //  map<int, CNode*>::const_iterator it = vyd_l.map_num_node.begin();

    for (map<int, CNode2*>::const_iterator it = vyd_l.map_node.begin(); it != vyd_l.map_node.end(); it++) {
        CNode2* n = it->second;

        //    if (n->typPS != -1) {
        if (1 /*n->name_typ_alma*/) {
            //    if (1 || n->typPS == 3 || n->typPS == 4) {

            list<int> list_geoD;
            list<int> list_geoV;
            list<int> list_geoZ;
            list<int> list_geoK;

            int nD = find_geo_node(n, list_geoD, klsD);
            int nV = find_geo_node(n, list_geoV, klsV);
            int nZ = find_geo_node(n, list_geoZ, klsZ);
            int nK = find_geo_node(n, list_geoK, klsK);



            CString name1 = "&nbsp;";

            name1 = n->getNamePT();

            int nn = 0;


            CString tZ[] = { _TR("Назначение"), _TR("Диаметр"), _TR("Материал"), _TR("Тип отключения") };
            vector<CString> ttZ(tZ, tZ + 4);
            list<vector<CString> > list_vvZ;
            read_geo(list_geoZ, klsZ, ttZ, list_vvZ);

            nn = max(nn, list_vvZ.size());

            CString tV[] = { _TR("Диаметр") };
            vector<CString> ttV(tV, tV + 1);
            list<vector<CString> > list_vvV;
            read_geo(list_geoV, klsV, ttV, list_vvV);

            nn = max(nn, list_vvV.size());


            CString tD[] = { _TR("Диаметр") };
            vector<CString> ttD(tD, tD + 1);
            list<vector<CString> > list_vvD;
            read_geo(list_geoD, klsD, ttD, list_vvD);
            nn = max(nn, list_vvD.size());


            CString tK[] = { _TR("Тип компенсатора"), _TR("Диаметр трубопровода") };
            vector<CString> ttK(tK, tK + 2);
            list<vector<CString> > list_vvK;
            read_geo(list_geoK, klsK, ttK, list_vvK);
            nn = max(nn, list_vvK.size());


            list<vector<CString> >::const_iterator itZ = list_vvZ.begin();
            list<vector<CString> >::const_iterator itV = list_vvV.begin();
            list<vector<CString> >::const_iterator itD = list_vvD.begin();
            list<vector<CString> >::const_iterator itK = list_vvK.begin();

            for (int i = 0; i < nn; i++) {
                fprintf(f, "<tr>");

                if (i == 0) {
                    fprintf(f, "<td>%s</td>", n->getNamePT());
                }
                else {
                    fprintf(f, "<td>&nbsp;</td>");
                }

                //      int nnn =  (nO, nD, nV, nZ, nK, nU);

                     // Запорная арматура         

                if (itZ != list_vvZ.end()) {
                    //          fprintf(f, "<td>%s</td>", (*itZ)[0]);  // Назначение
                    fprintf(f, "<td>%s</td>", (*itZ)[1]);  // Условный диаметр, мм

                    CString material = (*itZ)[2];
                    CString tip_otkl = (*itZ)[3];

                    int is_chugun = material.Find(_TR("чугун")) != -1;

                    int typ = 1;

                    print_td_n(f, is_chugun);   // Чугунных
                                                  // Стальных
                    print_td_n(f, !is_chugun && typ == 1);   // с ручным приводом
                    print_td_n(f, !is_chugun && typ == 2);   // электроприводом
                    print_td_n(f, !is_chugun && typ == 3);   // с гидроприводом
                }
                else {
                    print_td_povt(f, 5);
                }
                // Компенсаторы   

                if (itK != list_vvK.end()) {
                    //          map<CString, CString>::const_iterator it = map1.map1.find((*itK)[0]);

                    //          if (it != map1.map1.end()) {
                    //            fprintf(f, "<td>%s</td>", it->second);  // Конструкция
                    //          }
                    //          else {
                    fprintf(f, "<td>%s</td>", (*itK)[0]);  // Конструкция
        //          }
                    fprintf(f, "<td>%s</td>", (*itK)[1]);  // Условный диаметр,  мм
                    fprintf(f, "<td>1</td>");  // Количество, шт.
                }
                else {
                    print_td_povt(f, 2);
                }

                // Дренажные краны

                if (itD != list_vvD.end()) {
                    fprintf(f, "<td>%s</td>", (*itD)[0]);  // Условный диаметр, мм
                    fprintf(f, "<td>1</td>");  // Количество, шт
                }
                else {
                    print_td_povt(f, 2);
                }
                // Воздушники

                if (itV != list_vvV.end()) {
                    fprintf(f, "<td>%s</td>", (*itV)[0]);  // Условный диаметр, мм
                    fprintf(f, "<td>1</td>");              // Количество, шт.
                }
                else {
                    print_td_povt(f, 2);
                }

                print_td_povt(f, 5);
                fprintf(f, "\n");

                if (itD != list_vvD.end()) itD++;
                if (itV != list_vvV.end()) itV++;
                if (itZ != list_vvZ.end()) itZ++;
                if (itK != list_vvK.end()) itK++;
            }

        }
    }
#endif
}


void print_sheet9_2(FILE* f, int nomer_uch)
{
#if 0
    CString tn;


    tn.Format("SELECT * FROM [Участки эксплуатации] U, [Начальники участков] M WHERE M.ID=U.[Начальник участка] AND U.ID=%d", nomer_uch);

    CDbFile dbf;


    if (!openPS1(&dbf)) return;

    if (dbf.openTable(tn)) {
        if (!dbf.rs->IsEOF()) {

            dbf.rs->MoveFirst();

            if (!dbf.rs->IsEOF()) {
                //      if (dbf.MoveFirst()) {
                CString s = dbf.read(2);
                fprintf(f, "<tr>");
                s = dbf.read(_TR("Номер приказа о назначении"));
                fprintf(f, "<td>%s</td>", s);
                s = dbf.read(_TR("Дата приказа о назначении"));
                fprintf(f, "<td>%s</td>", s);
                s = dbf.read(_TR("Должность"));
                fprintf(f, "<td>%s</td>", s);

                s = dbf.read("M.Начальник участка");
                fprintf(f, "<td>%s</td>", s);
                //        s = dbf.read(_TR("Контактный телефон"));
                //        fprintf(f, "<td>%s</td>", s);

                fprintf(f, "</tr>");

                dbf.rs->MoveNext();
                }
            }
        dbf.closeTable();
        }
#endif
    }



void read_geo_Remont(list<int>& list_geo, Klassif* kls, vector<CString>& list_t, list<vector<CString> >& list_vv);
void read_elem(int ID, Klassif* kls, const char* name);


struct EL
{
    int oP;
    int pP;
    CString mP;
    int zP;
    int iP;
    int oO;
    int pO;
    CString mO;
    int zO;
    int iO;
};

struct TR
{
    int oP;
    int pP;
    int iP;
    int oO;
    int pO;
    int iO;
};

static void ispr(CString& v, double f)
{
    CString q;
    q.Format(" %g", f);
    v.Replace(", м", q + " м,");
    v.Replace(", шт", q + " шт,");
}


void print_zz(HTML& h, CString txt1, CString txt2, CString kam1, CString kam2, CString d1, CString d2, bool& first);


void print_sheetRemont2(CGraph2* graph, FILE* g, list <list<int> >& list_ut)
{
    Klassif* klsD = findKlN("defekt2");

    if (!klsD) {
        return;
    }

    MFIELDS map1, map2;

    //  map<int, CLINE2*>::const_iterator it = vyd_l.map_num_line.begin();

    list<int> list_geoD;



    for (auto p1 : list_ut) {

        CCoordList cl;
        cl.clear();
        CNode2* n1 = nullptr, * n2 = nullptr;

        get_first_last_cl(graph, p1, n1, n2, cl);

        if (n1 && n2) {
            list<int> list_geoD;


            //            fprintf(g, "<tr>");

            double dlP = 0, dlO = 0, qP, qO;

            CLINE2* l = get_max(graph, p1, dlP, dlO, qP, qO);

            int nD = find_geo_line(cl, list_geoD, klsD);


            //            if (nD > 0) {
            //                list_geoD.clear();
            //                nD = find_geo_line(cl, list_geoD, klsD);
            //            }


            CString tD[] = { "data_nachala_remonta", "data_zaversheniya_remonta", "otchet_po_defektu" };
            vector<CString> ttD(tD, tD + 3);
            list<vector<CString> > list_vvD;
            read_geo(list_geoD, klsD, ttD, list_vvD);

            //            upr(list_vvD);

            nD = list_vvD.size();

            int nn = nD;

            list<vector<CString> >::const_iterator itD = list_vvD.begin();

            for (int i = 0; i < nn; i++) {

                fprintf(g, "<tr>");

                if (i == 0) {
                    fprintf(g, "<td>%s</td>", n1->getNamePTfull());
                    fprintf(g, "<td>%s</td>", n2->getNamePTfull());
                }
                else {
                    fprintf(g, "<td>&nbsp;</td>");
                    fprintf(g, "<td>&nbsp;</td>");
                }

                // 

                if (itD != list_vvD.end()) {
                    fprintf(g, "<td>%s</td>", (*itD)[0]);  //
                    fprintf(g, "<td>%s</td>", (*itD)[1]);  //
                    fprintf(g, "<td>&nbsp;</td>");
                    fprintf(g, "<td>%s</td>", (*itD)[2]);  //
                }
                else {
                    print_td_povt(g, 4);
                }

                fprintf(g, "\n");

                if (itD != list_vvD.end()) itD++;

                fprintf(g, "</tr>");
            }

        }
    }
}



void printTr(CAdoFile* ado, FILE* f, const char** params, const int* types, int size) {
    fprintf(f, "<tr>");
    for (int i = 0;i < size;i++) {
        switch (i) {
        case 0://empty
            fprintf(f, "<td>&nbsp;</td>");
            break;
        case 1://long
            fprintf(f, "<td>%d</td>", ado->read_long(params[i]));
            break;
        case 2://double
            fprintf(f, "<td>%f</td>", ado->read_double(params[i]));
            break;
        case 3://string
            fprintf(f, "<td>%s</td>", ado->readStr(params[i]));
            break;

        }
    }
    fprintf(f, "</tr>");
}

void print_sheetShurf2(FILE* f, VYD_LINES& vyd_l, CNode2* nr, CNode2* nm)
{
#if 0
    Klassif* klsD = findKlN(transl("Шурф"));

    HTML h(f, &css);

    if (!klsD) {
        //    AfxMessageBox(_TR("Нет файла геобазы"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    MFIELDS map1, map2;
    InitTab(klsD->m_db->m_strFileName, klsD->TableName(), false);
    getMap(klsD->TableName(), _TR("Тип компенсатора"), map1);


    //  map<int, CLINE2*>::const_iterator it = vyd_l.map_num_line.begin();

    list<int> list_geoD;

    CNode* n0 = NULL;

    //for (; it != vyd_l.map_num_line.end(); it++) {

    for (map<int, CLINE2*>::const_iterator it = vyd_l.map_line.begin(); it != vyd_l.map_line.end(); it++) {
        CLINE2* vl = it->second;
        CNode* n1 = where(vl);
        CNode* n2 = other(vl);

        if (1 || n1->typPS() != -1 || inc(n1) != 2 || nr && n1->node_name == nr->node_name) {
            n0 = n1;
            list_geoD.clear();
        }

        int nD = find_geo_line(vl, list_geoD, klsD);
        //    int nU = find_geo_line(vl, list_geoU, klsU);

        if (1 || n2->typPS() != -1 || inc(n2) != 2) {

            CString tD[] = {
      _TR("Шурф"),
      _TR("Дата осмотра"),
      _TR("Номер акта осмотра в шурфе"),
      _TR("Состояние канала"),
      _TR("Состояние конструкции канала"),
      _TR("Наличие гидроизоляции канала"),
      _TR("Состояние гидроизоляционной конструкции, подающий"),
      _TR("Состояние гидроизоляционной конструкции, обратный"),
      _TR("Состояние покровного слоя изоляции, подающий"),
      _TR("Состояние покровного слоя изоляции, обратный"),
      _TR("Состояние тепловой изоляции, подающий"),
      _TR("Состояние тепловой изоляции, обратный"),
      _TR("Состояние противокоррозионного покрытия, подающий"),
      _TR("Состояние противокоррозионного покрытия, обратный"),
      _TR("Наличие коррозии, подающий"),
      _TR("Наличие коррозии, обратный"),
      _TR("Максимальная глубина коррозии, подающий, мм"),
      _TR("Максимальная глубина коррозии, обратный, мм"),
      _TR("Диаметр каверн, подающий, мм"),
      _TR("Диаметр каверн, обратный, мм"),
      _TR("Предполагаемые причины коррозионного повреждения металла трубы"),
      _TR("Место расположения повреждения по периметру трубы, подающий"),
      _TR("Место расположения повреждения по периметру трубы, обратный"),
      _TR("Наличие разрыва стенки трубы, причины, подающий"),
      _TR("Наличие разрыва стенки трубы, причины, обратный"),
      _TR("Наличие разрыва сварного шва трубы, причины, подающий"),
      _TR("Наличие разрыва сварного шва трубы, причины, обратный"),
      _TR("Наличие свища трубы, причины, подающий"),
      _TR("Наличие свища трубы, причины, обратный")
            };

            vector<CString> ttD(tD, tD + sizeof(tD) / sizeof(tD[0]));
            list<vector<CString> > list_vvD;
            read_geo(list_geoD, klsD, ttD, list_vvD);

            upr(list_vvD);

            nD = list_vvD.size();

            int nn = nD;

            list<vector<CString> >::const_iterator itD = list_vvD.begin();

            int nnn = sizeof(tD) / sizeof(tD[0]);


            for (int i = 0; i < nn; i++) {

                h.print_tr();
                h.set_tr(5);

                if (i == 0) {
                    print_ut22_1(h, vl);
                }
                else {
                    h.print_td("");
                }
                // Дренажные краны

                if (itD != list_vvD.end()) {
                    h.print_td((*itD)[1]); // Дата осмотра
                    h.print_td((*itD)[0]); // Шурф
                    h.print_td((*itD)[2]); // Номер акта

                    CString ss = "";

                    for (int ii = 2; ii < nnn; ii++) {

                        CString fn = tD[ii];
                        CString val = (*itD)[ii];

                        if (val != "") {

                            val = getReal2(klsD->TableName(), fn, val);
                            if (val == "TRUE") val = _TR("Да");
                            if (val == "FALSE") val = _TR("Нет");


                            if (ss != "") ss += "<br/>";
                            ss += fn;
                            ss += " : ";
                            ss += "<b>";
                            ss += val;
                            ss += "</b>";
        }
    }

                    css.css.vertical_align = "top";
                    css.css.mso_number_format = "\\@";

                    h.print_td(ss); // Шурф

                }
                else {
                    print_td_povt(h, 2);
                }

                h.print_tr2();

                // Воздушники
                if (itD != list_vvD.end()) itD++;
            }
        }
    }
#endif
}




void print_zz2(HTML& h, CString dt1, CString dt2, CString isp1, CString isp2, CString ss, bool& first);

#if 0

CString read_kap_meropr(CDbFile& dbf2, int ID)
{
    CString qq = "", q2;

    CString tn_meropr = _TR("Капремонт_колодцы_опоры_надземный");

    q2.Format("SELECT * FROM [%s] WHERE [ID_ремонт]=%d", tn_meropr, ID);

    if (dbf2.openTable(q2)) {
        while (!dbf2.rs->IsEOF()) {

            CString i1 = dbf2.read("ID_колод_опоры_надз");
            CString i2 = dbf2.read("ID_колодц_опоры_мероп");
            long d2 = dbf2.read_long(_TR("Количество Протяженность факт, шт, м"));

            CString s1 = getReal2(tn_meropr, "ID_колод_опоры_надз", i1);  if (i1 == s1) s1 = "";
            CString s2 = getReal2(tn_meropr, "ID_колодц_опоры_мероп", i2);if (i2 == s2) s2 = "";

            CString ss;
            ss.Format("%s %s : %d", s1, s2, d2);

            if (qq != "") qq += "<br/>";
            qq += ss;

            dbf2.rs->MoveNext();
        }
    }
    return qq;
}


//CString read_kap_meropr(dbf2, ID, _TR("Капремонт_колодцы_опоры_надземный"), "ID_колод_опоры_надз", "ID_колодц_опоры_мероп")


CString read_kap_meropr(CDbFile& dbf2, int ID, CString tn_meropr, CString f1, CString f2)
{
    CString qq = "", q2;

    q2.Format("SELECT * FROM [%s] WHERE [ID_ремонт]=%d", tn_meropr, ID);

    if (dbf2.openTable(q2)) {
        while (!dbf2.rs->IsEOF()) {

            CString i1 = dbf2.read(f1);
            CString i2 = dbf2.read(f2);
            //              long d1 = dbf2.read_long(_TR("Количество Протяженность план, шт, м"));
            long d2 = dbf2.read_long(_TR("Количество Протяженность факт, шт, м"));

            CString s1 = getReal2(tn_meropr, f1, i1); if (i1 == s1) s1 = "";
            CString s2 = getReal2(tn_meropr, f2, i2); if (i2 == s2) s2 = "";

            CString ss;
            ss.Format("%s %s : %d", s1, s2, d2);

            if (qq != "") qq += "<br/>";
            qq += ss;

            dbf2.rs->MoveNext();
        }
    }
    return qq;
}

CString read_kap_node(CDbFile& dbf2, int ID, CGraph2* graph, double r)
{
    CString q2;


    q2.Format("SELECT * FROM [%s] WHERE [ID_ремонт]=%d", STR_REMONT_KAPITAL, ID);

    CString qq = "";
    if (dbf2.openTable(q2)) {
        while (!dbf2.rs->IsEOF()) {
            CCoordList cl;
            dbf2.readCoord(cl);

            CFPoint pt = cl.centroid2();

            CLINE2* l = graph->findLine(pt, r);
            if (l) {
                if (qq != "") qq += " ";
                qq += (where(l)->node_name.Name() + " " + other(l)->node_name.Name());
            }
            dbf2.rs->MoveNext();
        }
    }
    return qq;
}
#endif



void CGidrView::print_sheetKapital2(HTML& h, VYD_LINES& vyd_l, CNode2* nr, CNode2* nm)
{
#if 0

    int qq = CreateGeoTableKapital(m_cxema->m_graph, STR_REMONT_KAPITAL);

    CString mdb = getVyd();

    h.set_tr(8);

    CDbFile dbf;

    CString q, q2;
    CString tn1 = _TR("Капитальный_ремонт");
    q.Format("SELECT * FROM [%s] WHERE [ID] IN (SELECT [nom] FROM [%s].VYD)", tn1, mdb);

    Klassif* kls = m_geofile->m_kl_list->findKlN(transl(STR_REMONT_KAPITAL));

    if (kls) {

        InitTab(kls->FileName(), _TR("Капремонт_трубопр_изол"), false);
        InitTab(kls->FileName(), _TR("Капремонт_колодцы_опоры_подземный"), false);
        InitTab(kls->FileName(), _TR("Капремонт_колодцы_опоры_надземный"), false);

        //    RunTableMap(kls->m_database, tn, q, getHelp_Map(), -1, 0, FALSE, FALSE, FALSE, FALSE);

        if (dbf.openMDB(kls->FileName())) {

            CDbFile dbf2(&dbf);

            if (dbf.openTable(q)) {

                while (!dbf.rs->IsEOF()) {
                    CString ss;
                    ss = dbf.read("ID");
                    long ID = dbf.read_long("ID");

                    css.css.border_top = 1;
                    css.css.border_bottom = 0;
                    css.css.border_right = 1;
                    css.css.border_left = 1;

                    CString dt1 = dbf.read(_TR("Дата начала ремонта факт"));
                    CString dt2 = dbf.read(_TR("Дата завершения ремонта факт"));

                    CString isp1 = dbf.read(_TR("Ответственный руководитель Должность"));
                    CString isp2 = dbf.read(_TR("Ответственный руководитель ФИО"));

                    q2.Format("SELECT * FROM [Капремонт_колодцы_опоры_надземный] WHERE [ID_ремонт]=%d", ID);

                    bool first = true;

                    h.print_tr();

                    CString qq = read_kap_node(dbf2, ID, m_cxema->m_graph, masx * D5 / mas_otn);

                    h.print_td(qq);
                    h.print_td(dt1);
                    h.print_td(dt2);
                    h.print_td(_TR("Капитальный ремонт"));

                    CString qqN = read_kap_meropr(dbf2, ID, _TR("Капремонт_колодцы_опоры_надземный"), "ID_колод_опоры_надз", "ID_колодц_опоры_мероп");
                    CString qqP = read_kap_meropr(dbf2, ID, _TR("Капремонт_колодцы_опоры_подземный"), "ID_колод_опоры_подз", "ID_колодц_опоры_мероп");
                    CString qqT = read_kap_meropr(dbf2, ID, _TR("Капремонт_трубопр_изол"), "ID_капремонт_труб", "ID_капремонт_труб_меропр");

                    qq = qqN;
                    if (qq != "") qq += "<br/>"; qq += qqP;
                    if (qq != "") qq += "<br/>"; qq += qqT;

                    h.print_td(qq);

                    h.print_td(isp1);
                    h.print_td(isp2);



                    h.print_tr2();

                    dbf.rs->MoveNext();
                }

                dbf.closeTable();
            }
            dbf.closeMDB();
        }
    }
#endif
}


void CGidrView::print_sheetOpressovka2(HTML& h, VYD_LINES& vyd_l, CNode2* nr, CNode2* nm)
{
#if 0
    int qq = CreateGeoTableOpressovka(m_cxema->m_graph, STR_REMONT_OPRESSOVKA);

    CString mdb = getVyd();

    h.set_tr(8);

    CDbFile dbf;

    CString q, q2;
    CString tn1 = _TR("Опрессовка");
    q.Format("SELECT * FROM [%s] WHERE [ID] IN (SELECT [nom] FROM [%s].VYD)", tn1, mdb);

    Klassif* kls = m_geofile->m_kl_list->findKlN(transl(STR_REMONT_OPRESSOVKA));

    if (kls) {

        InitTab(kls->FileName(), _TR("Опрессовка_повреждения"), false);

        //    RunTableMap(kls->m_database, tn, q, getHelp_Map(), -1, 0, FALSE, FALSE, FALSE, FALSE);

        if (dbf.openMDB(kls->FileName())) {

            CDbFile dbf2(&dbf);

            if (dbf.openTable(q)) {

                while (!dbf.rs->IsEOF()) {
                    CString ss;
                    ss = dbf.read("ID");
                    long ID = dbf.read_long("ID");

                    css.css.border_top = 1;
                    css.css.border_bottom = 0;
                    css.css.border_right = 1;
                    css.css.border_left = 1;

                    CString dt1 = dbf.read(_TR("Дата проведения"));
                    CString dt2 = dbf.read(_TR("Дата проведения"));

                    CString isp1 = dbf.read(_TR("Представитель ОЭТС должность"));
                    CString isp2 = dbf.read(_TR("Представитель ОЭТС ФИО"));

                    //          q2.Format("SELECT * FROM [Капремонт_колодцы_опоры_надземный] WHERE [ID_ремонт]=%d", ID);

                    bool first = true;

                    h.print_tr();

                    CString qq = read_kap_node(dbf2, ID, m_cxema->m_graph, masx * D5 / mas_otn);

                    h.print_td(qq);
                    h.print_td(dt1);
                    h.print_td(dt2);
                    h.print_td(_TR("Опрессовка"));

                    CString qqN = read_opr_meropr(dbf2, ID, _TR("Опрессовка_повреждения"), "ID_опрессовка_повреждения", "ID_опрессовка_повреждения_меропр");

                    qq = qqN;
                    //          if (qq != "") qq += "<br/>"; qq += qqP;
                    //          if (qq != "") qq += "<br/>"; qq += qqT;

                    h.print_td(qq);

                    h.print_td(isp1);
                    h.print_td(isp2);



                    h.print_tr2();

                    dbf.rs->MoveNext();
        }

                dbf.closeTable();
    }
            dbf.closeMDB();
}
}
#endif
        }






void CGidrView::MagLineTabKanal2(FILE* f, CNode2* n, VYD_LINES& vyd_l)
{
    HTML h(f, &css);

    Klassif* kls = m_geofile->m_kl_list->findKlN(transl("Канал"));

    if (!kls) return;

    CAdoFile* ado = kls->m_ado;

    CNode2* nz = m_cxema->m_graph->sameMarkNode();
    if (!nz) return;

    map<int, CLINE2*>::const_iterator it = vyd_l.map_line.begin();

    double dlP = 0, dlO = 0, vP = 0, vO = 0;

    CString q;
    //    q.Format("SELECT * FROM %s", kls->nazv);

    q = "SELECT tk.id, tk.naimenovanie AS tk_name,  kanal.tip_kanala, kanal.* FROM kanal LEFT JOIN [tip_kanala_setey_teplosnabzheniya] tk ON CAST(tk.id  AS VARCHAR(11)) = kanal.tip_kanala";



    if (ado->openTable0(q)) {

        for (; it != vyd_l.map_line.end(); it++) {
            CLINE2* l = it->second;
            if (bline(l)->line.typ == TIP_UT) {
                CFPoint pt = centroid(bline(l));

                int ng = m_geofile->findXY3(pt, 0, LOC3, kls->nazv);

                if (ng > 0) {

                    h.print_tr();
                    h.set_tr(7);

                    h.print_td(AnyName(where(l)) + " - " + AnyName(other(l)));

                    if (ado->GotoKey(ng)) {
                        CString vs = ado->readStr(transl("Высота"));
                        if (vs == "") vs = ado->readStr(transl("Высота, мм"));

                        CString sh = ado->readStr(transl("Ширина"));
                        if (sh == "") sh = ado->readStr(transl("Ширина, мм"));

                        CString dl = ado->readStr(transl("Длина"));
                        if (dl == "") dl = ado->readStr(transl("Длина, мм"));

                        CString konstr = ado->readStr(transl("Конструкция"));

                        //                        CString typ = ado->readStr(transl("Тип канала"));


                        CString typ = ado->readStr(transl("tk_name"));

                        if (typ == "") {
                            typ = ado->readStr(transl("Тип канала"));
                        }

                        CString tol = ado->readStr(transl("Толщина стенки, мм"));;

                        h.print_td(typ);
                        h.print_td(vs);
                        h.print_td(sh);
                        h.print_td(tol);
                        h.print_td(konstr);

                        h.print_td(dl);
                    }

                    //        print_ut(f,l);
                    /*
                    dlP += bline(l)->pod.dl;
                    dlO += bline(l)->obr.dl;
                    vP += bline(l)->pod.v;
                    vO += bline(l)->obr.v;
                    */

                    h.print_tr2();

                }
            }
        }
    }
}



void print_sheet3_2_obraz(CGraph2* graph, FILE* g, list <list<int> >& list_ut)
{
    for (auto p1 : list_ut) {

        bool get_first_last(CGraph2 * graph, list<int> & list_ut_min, CNode2 * &n_first, CNode2 * &n_last);

        CNode2* n1 = nullptr, * n2 = nullptr;

        get_first_last(graph, p1, n1, n2);

        if (n1 && n2) {
            fprintf(g, "<tr>");
            fprintf(g, "<td>%s</td>", n1->getNamePTfull());
            fprintf(g, "<td>%s</td>", n2->getNamePTfull());

            double dlP = 0, dlO = 0, qP, qO;

            CLINE2* l = get_max(graph, p1, dlP, dlO, qP, qO);

            if (l) {
                double diamP = bline(l)->line.pod.diam_vn;
                double diamO = bline(l)->line.obr.diam_vn;

                fprintf(g, "<td>%g</td>", diamP);
                fprintf(g, "<td>%g</td>", dlP);
                fprintf(g, "<td>%g</td>", diamO);
                fprintf(g, "<td>%g</td>", dlO);

                fprintf(g, "<td>%g</td>", bline(l)->line.pod.tol);
                fprintf(g, "<td>%g</td>", bline(l)->line.obr.tol);
                fprintf(g, "<td>%g</td>", qP); // Исправил w на v
                fprintf(g, "<td>%g</td>", qO);
                fprintf(g, "<td>%s</td>", getTubingTypeID(bline(l)->line.tubingTypeID));
            }
            fprintf(g, "</tr>");
        }
    }
}


#if 1

void print_sheet3_2(CGraph2* graph, FILE* f, VYD_LINES& vyd_l, CNode2* nr, CNode2* nm)
{
#if 1
    Klassif* klsD = findKlN(transl("Дренажный кран"));
    Klassif* klsV = findKlN(transl("Воздушник"));
    Klassif* klsZ = findKlN(transl("Запорная арматура"));
    Klassif* klsK = findKlN(transl("Компенсатор"));

    if (!klsD || !klsV || !klsZ || !klsK) {
        //    AfxMessageBox(_TR("Нет файла геобазы"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    MFIELDS map1, map2;
    InitTab(klsK->TableName(), klsK->TableName(), false);
    getMap(klsK->TableName(), transl("Тип компенсатора"), map1);

    //  InitTab(klsZ->TableName(), klsZ->TableName(), false);
    //  getMap(klsZ->TableName(), transl("Назначение"), map2);
    //  getMap(klsZ->TableName(), "ValvePurpose", map2);




    CAdoFile* ado = getAdo(getPsAdoName());

    map<long, CString> map_z;
    //  initLookup2(ado, "naznachenie_zapornoy_armatury_setey_teplosnabzheniya", "naimenovanie", map_z);
    initLookup2(ado, "ValvePurpose", "name", map_z);



    //  map<int, CLINE2*>::const_iterator it = vyd_l.map_num_line.begin();

    list<int> list_geoD;
    list<int> list_geoV;
    list<int> list_geoZ;
    list<int> list_geoK;

    CNode2* n0 = NULL;

    //for (; it != vyd_l.map_num_line.end(); it++) {

    for (map<int, CLINE2*>::const_iterator it = vyd_l.map_line.begin(); it != vyd_l.map_line.end(); it++) {
        CLINE2* vl = it->second;
        CNode2* n1 = where(vl);
        CNode2* n2 = other(vl);

        if (1 || n1->typPS() != -1 || inc(n1) != 2 || nr && n1->node.node_name == nr->node.node_name) {
            n0 = n1;
            list_geoD.clear();
            list_geoV.clear();
            list_geoZ.clear();
            list_geoK.clear();
        }

        int nD = find_geo_line(vl, list_geoD, klsD);
        int nV = find_geo_line(vl, list_geoV, klsV);
        int nZ = find_geo_line(vl, list_geoZ, klsZ);
        int nK = find_geo_line(vl, list_geoK, klsK);
        //    int nU = find_geo_line(vl, list_geoU, klsU);

        if (1 || n2->typPS() != -1 || inc(n2) != 2) {
            CString tZ[] = { transl("Назначение"), transl("Диаметр"), transl("Материал"), transl("Тип отключения"), transl("Признак трубопровода") };
            vector<CString> ttZ(tZ, tZ + 5);
            list<vector<CString> > list_vvZ;
            read_geo(list_geoZ, klsZ, ttZ, list_vvZ);

            CString tV[] = { transl("Диаметр"), transl("Признак трубопровода") };
            vector<CString> ttV(tV, tV + 2);
            list<vector<CString> > list_vvV;
            read_geo(list_geoV, klsV, ttV, list_vvV);

            CString tD[] = { transl("Диаметр"), transl("Признак трубопровода") };
            vector<CString> ttD(tD, tD + 2);
            list<vector<CString> > list_vvD;
            read_geo(list_geoD, klsD, ttD, list_vvD);

            CString tK[] = { transl("Тип компенсатора"), transl("Диаметр трубопровода"), transl("Признак трубопровода") };
            vector<CString> ttK(tK, tK + 3);
            list<vector<CString> > list_vvK;
            read_geo(list_geoK, klsK, ttK, list_vvK);

            upr(list_vvZ);
            upr(list_vvV);
            upr(list_vvD);
            upr(list_vvK);

            nZ = list_vvZ.size();
            nV = list_vvV.size();
            nK = list_vvK.size();
            nD = list_vvD.size();

            int nn = max(max(nZ, nK), max(nD, nV));

            list<vector<CString> >::const_iterator itD = list_vvD.begin();
            list<vector<CString> >::const_iterator itV = list_vvV.begin();
            list<vector<CString> >::const_iterator itZ = list_vvZ.begin();
            list<vector<CString> >::const_iterator itK = list_vvK.begin();

            for (int i = 0; i < nn; i++) {

                fprintf(f, "<tr>");

                if (i == 0) {
                    print_ut22(f, vl);
                }
                else {
                    fprintf(f, "<td>&nbsp;</td>");
                    fprintf(f, "<td>&nbsp;</td>");
                }

                //      int nnn =  (nO, nD, nV, nZ, nK, nU);

                     // Запорная арматура         

                if (itZ != list_vvZ.end()) {
                    CString ss = getReal2(map_z, (*itZ)[0]);
                    fprintf(f, "<td>%s</td>", ss);  // Назначение
                    fprintf(f, "<td>%s</td>", (*itZ)[1]);  // Условный диаметр, мм

                    CString sn = (*itZ)[4];

                    int nn = atoi(sn);

                    CString material = (*itZ)[2];
                    CString tip_otkl = (*itZ)[3];

                    int is_chugun = material.Find(transl("чугун")) != -1;

                    int typ = 1;

                    print_td_n(f, nn, is_chugun);   // Чугунных
                                                  // Стальных
                    print_td_n(f, nn, !is_chugun && typ == 1);   // с ручным приводом
                    print_td_n(f, nn, !is_chugun && typ == 2);   // электроприводом
                    print_td_n(f, nn, !is_chugun && typ == 3);   // с гидроприводом
                }
                else {
                    print_td_povt(f, 6);
                }
                // Компенсаторы   

                if (itK != list_vvK.end()) {
                    map<CString, CString>::const_iterator it = map1.map1.find((*itK)[0]);

                    if (it != map1.map1.end()) {
                        fprintf(f, "<td>%s</td>", it->second);  // Конструкция
                    }
                    else {
                        fprintf(f, "<td>%s</td>", (*itK)[0]);  // Конструкция
                    }
                    fprintf(f, "<td>%s</td>", (*itK)[1]);  // Условный диаметр,  мм
                    fprintf(f, "<td>%d</td>", kolich((*itK)[2], (*itK)[3]));  // Количество, шт.
                }
                else {
                    print_td_povt(f, 3);
                }

                // Дренажные краны

                if (itD != list_vvD.end()) {
                    fprintf(f, "<td>%s</td>", (*itD)[0]);  // Условный диаметр, мм
                    fprintf(f, "<td>%d</td>", kolich((*itD)[1], (*itD)[2]));  // Количество, шт
                }
                else {
                    print_td_povt(f, 2);
                }
                // Воздушники

                if (itV != list_vvV.end()) {
                    fprintf(f, "<td>%s</td>", (*itV)[0]);  // Условный диаметр, мм
                    fprintf(f, "<td>%d</td>", kolich((*itV)[1], (*itV)[2]));              // Количество, шт.
                }
                else {
                    print_td_povt(f, 2);
                }

                fprintf(f, "\n");

                if (itD != list_vvD.end()) itD++;
                if (itV != list_vvV.end()) itV++;
                if (itZ != list_vvZ.end()) itZ++;
                if (itK != list_vvK.end()) itK++;
            }
        }
    }
#endif
}

#endif



void print_sheet3_2(CGraph2* graph, FILE* g, list <list<int> >& list_ut)
{
    Klassif* klsD = findKlN(transl("Дренажный кран"));
    Klassif* klsV = findKlN(transl("Воздушник"));
    Klassif* klsZ = findKlN(transl("Запорная арматура"));
    Klassif* klsK = findKlN(transl("Компенсатор"));

    if (!klsD || !klsV || !klsZ || !klsK) {
        return;
    }

    CAdoFile* ado = getAdo(getPsAdoName());

    MFIELDS map1; // , map2;
    //  InitTab(klsK->TableName(), klsK->TableName(), false);
    getMap(klsK->TableName(), transl("Тип компенсатора"), map1);

    //  InitTab(klsZ->TableName(), klsZ->TableName(), false);
//    getMap(klsZ->TableName(), transl("Назначение"), map2);


    map<long, CString> map_z;
    //  initLookup2(ado, "naznachenie_zapornoy_armatury_setey_teplosnabzheniya", "naimenovanie", map_z);
    initLookup2(ado, "ValvePurpose", "name", map_z);


    map<long, CString> map_k;
    initLookup2(ado, "CompensatorTypes", "name", map_k);


    //  map<int, CLINE2*>::const_iterator it = vyd_l.map_num_line.begin();

    CNode2* n0 = NULL;


    for (auto p1 : list_ut) {
        CNode2* n1 = nullptr, * n2 = nullptr;

        CCoordList cl;
        cl.clear();

        //        get_first_last(graph, p1, n1, n2);
        get_first_last_cl(graph, p1, n1, n2, cl);

        if (n1 && n2) {
            list<int> list_geoD;
            list<int> list_geoV;
            list<int> list_geoZ;
            list<int> list_geoK;


            //            fprintf(g, "<tr>");

            double dlP = 0, dlO = 0, qP, qO;

            CLINE2* l = get_max(graph, p1, dlP, dlO, qP, qO);

            int nD = find_geo_line(cl, list_geoD, klsD);
            int nV = find_geo_line(cl, list_geoV, klsV);
            int nZ = find_geo_line(cl, list_geoZ, klsZ);
            int nK = find_geo_line(cl, list_geoK, klsK);



            CString tZ[] = { transl("Назначение"), transl("Диаметр"), transl("Материал"), transl("Тип отключения"), transl("Признак трубопровода") };
            vector<CString> ttZ(tZ, tZ + 5);
            list<vector<CString> > list_vvZ;
            read_geo(list_geoZ, klsZ, ttZ, list_vvZ);

            CString tV[] = { transl("Диаметр"), transl("Признак трубопровода") };
            vector<CString> ttV(tV, tV + 2);
            list<vector<CString> > list_vvV;
            read_geo(list_geoV, klsV, ttV, list_vvV);

            CString tD[] = { transl("Диаметр"), transl("Признак трубопровода") };
            vector<CString> ttD(tD, tD + 2);
            list<vector<CString> > list_vvD;
            read_geo(list_geoD, klsD, ttD, list_vvD);

            CString tK[] = { transl("Тип компенсатора"), transl("Диаметр трубопровода"), transl("Признак трубопровода") };
            vector<CString> ttK(tK, tK + 3);
            list<vector<CString> > list_vvK;
            read_geo(list_geoK, klsK, ttK, list_vvK);

            upr(list_vvZ);
            upr(list_vvV);
            upr(list_vvD);
            upr(list_vvK);

            nZ = list_vvZ.size();
            nV = list_vvV.size();
            nK = list_vvK.size();
            nD = list_vvD.size();

            int nn = max(max(nZ, nK), max(nD, nV));

            list<vector<CString> >::const_iterator itD = list_vvD.begin();
            list<vector<CString> >::const_iterator itV = list_vvV.begin();
            list<vector<CString> >::const_iterator itZ = list_vvZ.begin();
            list<vector<CString> >::const_iterator itK = list_vvK.begin();

            for (int i = 0; i < nn; i++) {

                fprintf(g, "<tr>");

                if (i == 0) {
                    fprintf(g, "<td>%s</td>", n1->getNamePTfull());
                    fprintf(g, "<td>%s</td>", n2->getNamePTfull());
                }
                else {
                    fprintf(g, "<td>&nbsp;</td>");
                    fprintf(g, "<td>&nbsp;</td>");
                }

                //      int nnn =  (nO, nD, nV, nZ, nK, nU);

                     // Запорная арматура         

                if (itZ != list_vvZ.end()) {
                    CString ss = getReal2(map_z, (*itZ)[0]);
                    fprintf(g, "<td>%s</td>", ss);  // Назначение
                    fprintf(g, "<td>%s</td>", (*itZ)[1]);  // Условный диаметр, мм

                    CString sn = (*itZ)[4];

                    int nn = atoi(sn);

                    CString material = (*itZ)[2];
                    CString tip_otkl = (*itZ)[3];

                    int is_chugun = material.Find(transl("чугун")) != -1;

                    int typ = 1;

                    print_td_n(g, nn, is_chugun);   // Чугунных
                                                  // Стальных
                    print_td_n(g, nn, !is_chugun && typ == 1);   // с ручным приводом
                    print_td_n(g, nn, !is_chugun && typ == 2);   // электроприводом
                    print_td_n(g, nn, !is_chugun && typ == 3);   // с гидроприводом
                }
                else {
                    print_td_povt(g, 6);
                }
                // Компенсаторы   

                if (itK != list_vvK.end()) {
                    CString ss = getReal2(map_k, (*itK)[0]);
                    fprintf(g, "<td>%s</td>", ss);  // Конструкция
                    fprintf(g, "<td>%s</td>", (*itK)[1]);  // Условный диаметр,  мм
                    fprintf(g, "<td>%d</td>", kolich((*itK)[2], (*itK)[3]));  // Количество, шт.
                }
                else {
                    print_td_povt(g, 3);
                }

                // Дренажные краны

                if (itD != list_vvD.end()) {
                    fprintf(g, "<td>%s</td>", (*itD)[0]);  // Условный диаметр, мм
                    fprintf(g, "<td>%d</td>", kolich((*itD)[1], (*itD)[2]));  // Количество, шт
                }
                else {
                    print_td_povt(g, 2);
                }
                // Воздушники

                if (itV != list_vvV.end()) {
                    fprintf(g, "<td>%s</td>", (*itV)[0]);  // Условный диаметр, мм
                    fprintf(g, "<td>%d</td>", kolich((*itV)[1], (*itV)[2]));              // Количество, шт.
                }
                else {
                    print_td_povt(g, 2);
                }

                fprintf(g, "\n");

                if (itD != list_vvD.end()) itD++;
                if (itV != list_vvV.end()) itV++;
                if (itZ != list_vvZ.end()) itZ++;
                if (itK != list_vvK.end()) itK++;
                fprintf(g, "</tr>");
            }

        }
    }
}

#if 0

void print_sheet4_2(CGraph2* graph, FILE* f, VYD_LINES& vyd_l, CNode2* nr, CNode2* nm)
{
    //  InitTab(getPS(), tn);

#if 1

    map<long, CString> map_o;

    Klassif* klsO = findKlN(transl("Опора"));
    Klassif* klsU = findKlN(transl("Угол поворота трубопровода"));

    if (!klsO || !klsU || !klsP) {
        //    AfxMessageBox(_TR("Нет файла геобазы"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    CAdoFile* ado = getAdo(getPsAdoName());


    initLookup2(ado, "BuildingConstructions", "name", map_o);



    //  InitTab(klsO->m_db->m_strFileName, klsO->TableName(), false);

    //  MFIELDS map1;
    //  InitTab(klsO->m_db->m_strFileName, klsO->TableName(), false);
    //  getMap(klsO->TableName(), _TR("Строительная конструкция"), map1);

    //  map<int, CLINE2*>::const_iterator it = vyd_l.map_num_line.begin();

    list<int> list_geoO;
    list<int> list_geoU;

    CNode2* n0 = NULL;

    list<vector<CString> > list_vvPD;
    //  for (; it != vyd_l.map_num_line.end(); it++) {

    for (map<int, CLINE2*>::const_iterator it = vyd_l.map_line.begin(); it != vyd_l.map_line.end(); it++) {
        CLINE2* vl = it->second;
        CNode2* n1 = where(vl);
        CNode2* n2 = other(vl);

        if (1 || n1->typPS() != -1 || inc(n1) != 2 || nr && n1->node.node_name == nr->node.node_name) {
            n0 = n1;
            list_geoO.clear();
            list_geoU.clear();
            list_vvPD.clear();
        }
        else if (n2->typPS() == -1 && inc(n2) == 2) {
            CLINE2* vl2 = n2->lines;
            if (bline(vl2) == bline(vl)) vl2 = next(vl2);
            double diam1 = bline(vl)->line.nomP != -1 ? bline(vl)->line.pod.diam_usl : bline(vl)->line.obr.diam_usl;
            double diam2 = bline(vl2)->line.nomP != -1 ? bline(vl2)->line.pod.diam_usl : bline(vl2)->line.obr.diam_usl;
            if (diam1 != diam2) {
                CString str;
                vector<CString> vvPD;

                str.Format("%g", diam1);  vvPD.push_back(str);
                str.Format("%g", diam2);  vvPD.push_back(str);
                list_vvPD.push_back(vvPD);
            }
        }

        int nO = find_geo_line(vl, list_geoO, klsO);
        int nU = find_geo_line(vl, list_geoU, klsU);

        if (1 || n2->typPS() != -1 || inc(n2) != 2) {
            CString tU[] = { transl("Диаметр трубопровода"), transl("Угол поворота"), transl("Признак трубопровода") };
            vector<CString> ttU(tU, tU + 3);
            list<vector<CString> > list_vvU;
            read_geo(list_geoU, klsU, ttU, list_vvU);

            CString tO[] = { transl("Строительная конструкция"), transl("Примечание"), transl("Признак трубопровода") };
            vector<CString> ttO(tO, tO + 3);
            list<vector<CString> > list_vvO;
            read_geo(list_geoO, klsO, ttO, list_vvO);

            if (list_vvO.size() > 0) {
                read_geo(list_geoO, klsO, ttO, list_vvO);
            }

            upr(list_vvU);
            upr(list_vvO);
            nU = list_vvU.size();
            nO = list_vvO.size();

            int nPD = list_vvPD.size();

            int nn = max(nPD, max(nU, nO));

            list<vector<CString> >::const_iterator itU = list_vvU.begin();
            list<vector<CString> >::const_iterator itO = list_vvO.begin();
            list<vector<CString> >::const_iterator itPD = list_vvPD.begin();

            for (int i = 0; i < nn; i++) {

                fprintf(f, "<tr>");

                if (i == 0) {
                    print_ut22(f, vl);
                }
                else {
                    fprintf(f, "<td>&nbsp;</td>");
                    fprintf(f, "<td>&nbsp;</td>");
                }

                // Углы поворота                  
                if (itU != list_vvU.end()) {
                    fprintf(f, "<td>%s</td>", (*itU)[0]);  // Наружный диаметр, мм
                    fprintf(f, "<td>%s</td>", (*itU)[1]);  // Угол поворота
                    fprintf(f, "<td>%d</td>", kolich((*itU)[2], (*itU)[3]));              // Количество, шт.

                }
                else {
                    print_td_povt(f, 3);
                }

                // Опоры
                if (itO != list_vvO.end()) {
                    CString ss = getReal2(map_o, (*itO)[0]);
                    fprintf(f, "<td>%s</td>", ss);  // Конструкция

                    fprintf(f, "<td>%s</td>", (*itO)[1]);  // Примечание
                    fprintf(f, "<td>%d</td>", kolich((*itO)[2], (*itO)[3]));              // Количество, шт.
                }
                else {
                    print_td_povt(f, 3);
                }

                if (itPD != list_vvPD.end()) {
                    fprintf(f, "<td>%s</td>", (*itPD)[0]);
                    fprintf(f, "<td>%s</td>", (*itPD)[1]);
                    fprintf(f, "<td>1</td>");
                }
                else {
                    print_td_povt(f, 3);
                }

                fprintf(f, "\n");

                if (itU != list_vvU.end()) itU++;
                if (itO != list_vvO.end()) itO++;
                if (itPD != list_vvPD.end()) itPD++;
                }
            }
        }
#endif
    }

#endif





void print_sheet4_2(CGraph2* graph, FILE* g, list <list<int> >& list_ut)
{
    Klassif* klsO = findKlN(transl("Опора"));
    Klassif* klsU = findKlN(transl("Угол поворота трубопровода"));
    Klassif* klsP = findKlN(transl("Переход диаметра"));

    if (!klsO || !klsU || !klsP) {
        return;
    }

    map<long, CString> map_o;
    CAdoFile* ado = getAdo(getPsAdoName());

    initLookup2(ado, "BuildingConstructions", "name", map_o);


    //    map<long, CString> map_z;
    //    initLookup2(ado, "naznachenie_zapornoy_armatury_setey_teplosnabzheniya", "naimenovanie", map_z);
    //    map<long, CString> map_k;
    //    initLookup2(ado, "CompensatorTypes", "name", map_k);


    CNode2* n0 = NULL;

    for (auto p1 : list_ut) {
        CNode2* n1 = nullptr, * n2 = nullptr;

        CCoordList cl;
        cl.clear();

        //        get_first_last(graph, p1, n1, n2);
        get_first_last_cl(graph, p1, n1, n2, cl);

        if (n1 && n2) {

            list<int> list_geoO;
            list<int> list_geoU;
            list<int> list_geoP;


            double dlP = 0, dlO = 0, qP, qO;

            CLINE2* l = get_max(graph, p1, dlP, dlO, qP, qO);

            int nO = find_geo_line(cl, list_geoO, klsO);
            int nU = find_geo_line(cl, list_geoU, klsU);
            int nP = find_geo_line(cl, list_geoP, klsP);


            CString tU[] = { transl("Диаметр трубопровода"), transl("Угол поворота"), transl("Признак трубопровода") };
            vector<CString> ttU(tU, tU + 3);
            list<vector<CString> > list_vvU;
            read_geo(list_geoU, klsU, ttU, list_vvU);

            CString tO[] = { transl("Строительная конструкция"), transl("Примечание"), transl("Признак трубопровода") };
            vector<CString> ttO(tO, tO + 3);
            list<vector<CString> > list_vvO;
            read_geo(list_geoO, klsO, ttO, list_vvO);


            CString tP[] = { transl("Начальный диаметр"), transl("Конечный диаметр"), transl("Признак трубопровода") };
            vector<CString> ttP(tP, tP + 3);
            list<vector<CString> > list_vvP;
            read_geo(list_geoP, klsP, ttP, list_vvP);



            upr(list_vvO);
            upr(list_vvU);
            upr(list_vvP);

            nO = list_vvO.size();
            nU = list_vvU.size();
            nP = list_vvP.size();

            int nn = max(max(nO, nU), nP);

            list<vector<CString> >::const_iterator itO = list_vvO.begin();
            list<vector<CString> >::const_iterator itU = list_vvU.begin();
            list<vector<CString> >::const_iterator itP = list_vvP.begin();

            for (int i = 0; i < nn; i++) {

                fprintf(g, "<tr>");

                if (i == 0) {
                    fprintf(g, "<td>%s</td>", n1->getNamePTfull());
                    fprintf(g, "<td>%s</td>", n2->getNamePTfull());
                }
                else {
                    fprintf(g, "<td>&nbsp;</td>");
                    fprintf(g, "<td>&nbsp;</td>");
                }


                // Углы поворота                  
                if (itU != list_vvU.end()) {
                    fprintf(g, "<td>%s</td>", (*itU)[0]);  // Наружный диаметр, мм
                    fprintf(g, "<td>%s</td>", (*itU)[1]);  // Угол поворота
                    fprintf(g, "<td>%d</td>", kolich((*itU)[2], (*itU)[3]));              // Количество, шт.

                }
                else {
                    print_td_povt(g, 3);
                }

                // Опоры
                if (itO != list_vvO.end()) {
                    CString ss = getReal2(map_o, (*itO)[0]);
                    fprintf(g, "<td>%s</td>", ss);  // Конструкция

                    fprintf(g, "<td>%s</td>", (*itO)[1]);  // Примечание
                    fprintf(g, "<td>%d</td>", kolich((*itO)[2], (*itO)[3]));              // Количество, шт.
                }
                else {
                    print_td_povt(g, 3);
                }


                // Переход диаметра
                if (itP != list_vvP.end()) {
                    fprintf(g, "<td>%s</td>", (*itP)[0]);  // 
                    fprintf(g, "<td>%s</td>", (*itP)[1]);  // 
                    fprintf(g, "<td>%d</td>", kolich((*itP)[2], (*itP)[3]));              // Количество, шт.
                }
                else {
                    print_td_povt(g, 3);
                }



                fprintf(g, "\n");

                if (itU != list_vvU.end()) itU++;
                if (itO != list_vvO.end()) itO++;
                if (itP != list_vvP.end()) itP++;


                //        if (itPD != list_vvPD.end()) itPD ++;


                fprintf(g, "</tr>");
            }

        }
    }
}



// Камеры и павильоны

void print_sheet5_2(CGraph2* graph, FILE* f, list<CNode2*>& list_sort)
{
    //  CDbFile dbf1;
    CString tn1 = _TR("Павильон");
    CString tn2 = _TR("Тепловая камера");

    for (auto it : list_sort) {
        CNode2* n = it;

        if (7 <= n->node._typPS && n->node._typPS <= 15) {
            fprintf(f, "<tr><td>%s</td>", n->getNamePT());

            switch (n->node._typPS) {
            case 7: print_pav(f, "pavilions", n); break; //Павильон", "П", 7),
            case 8: print_pav(f, "trps", n); break; //ТРП", "ТРП", 8),
            case 9: print_pav(f, "heatChambers", n); break; //Тепловая камера", "КАМ", 9),
            case 10: print_pav(f, "undergroundNodes", n); break; //Подземный узел", "ПОДЗ", 10),
            case 11: print_pav(f, "overgroundNodes", n); break; //Надземный узел", "НАДЗ", 11),
            case 12: print_pav(f, "Ввод в здание", n); break; //Ввод здание", "ВВ", 12),
            case 13: print_pav(f, "uninstalledNodes", n); break; //Неустановленный узел", "?", 13),
            case 14: print_pav(f, "pumpStations", n); break; //Насосная станция", "НС", 14),
            case 15: print_pav(f, "heatSources", n); break; //Источник тепла", "ИСТ", 15);
            }
        }
        fprintf(f, "\n");
    }
}


#if 0
void CGidrView::RsLineTabIzol2(FILE* f, CNode2* node)
{
    HTML h(f, &css);

    //  RsLineIzolTabDb(node);

    //  CString mdb = getVyd();
    //  CString tn = _TR("Изоляция");
    CString q, vyd;

    if (!node) return;


    CAdoFile* ado = getAdo(getPsAdoName());



    CString tuple;

    //  create_vyd_line_table(ado, node->node.m_graph, vyd);
    create_vyd_line_tuple(ado, node->node.m_graph, tuple);



    //  RsLineIzolTabDb(node);

    q.Format("SELECT uzel1 AS [Начальный узел], uzel2 AS [Конечный узел] ,"
        " isolMaterialID AS [Теплоизоляционный материал], "
        " isolThickness AS [Толщина тепловой изоляции, мм], "
        " externMaterialID AS [Материал наружного покрытия], "
        " externCoverThick AS [Толщина наружного покрытия], "
        " anticorrMaterialID AS [Материал антикоррозионного покрытия] "
        " FROM [%s]", ST_UT);

    q.Format(
        "select l.id,\n"
        //" n1.externalNodeName as 'uzel1',\n"
        //" --case l.externalSignLineID    when 1 then ' '    when 2 then 'П'    when 3 then 'О'    when 4 then 'П'    when 5 then 'О' end as 'pr1',\n"
        " IIF(n1.nodeName IS NULL, n1.externalNodeName, n1.nodeName) AS name1,\n"
        //" ec2.name as 'kod2', \n"
        //" n2.externalNodeName as 'uzel2',\n"
        //" -- case l.externalSignLineID    when 1 then ' '    when 2 then 'П'    when 3 then 'О'    when 4 then 'О'    when 5 then 'П' end as 'pr2',\n"
        " IIF(n2.nodeName IS NULL, n2.externalNodeName, n2.nodeName) AS name2,\n"

        " im.name AS [Теплоизоляционный материал], \n"
        " isolThickness AS [Толщина тепловой изоляции, мм], \n"
        " em.name AS [Материал наружного покрытия], \n"
        " externCoverThick AS [Толщина наружного покрытия], \n"
        " am.name AS [Материал антикоррозионного покрытия] \n"


        "from heatPipeSections hps\n"
        "join linesobj l on l.id = hps.lineID\n"
        "join nodes n1 on n1.id = l.nodeID1\n"
        "join nodes n2 on n2.id = l.nodeID2\n"
        "join externalCodes ec1 on ec1.id = n1.externalCodeID\n"
        "join externalCodes ec2 on ec2.id = n2.externalCodeID\n"
        "join externalSigns es1 on es1.id = n1.externalSignID\n"
        "join externalSigns es2 on es2.id = n2.externalSignID\n"

        "left join isolMaterials im on im.id = hps.isolMaterialID\n"
        "left join externalMaterials em on em.id = hps.externMaterialID\n"
        "left join anticorrMaterials am on am.id = hps.anticorrMaterialID\n"

        //"WHERE /*n1.fileID=$fileID$ AND */n1.internalNodeID IS NULL\n"
        "WHERE n1.internalNodeID IS NULL\n"
        //"AND l.removed=0 AND l.id IN (SELECT ID from %s)\n", vyd);
        "AND l.removed=0 AND l.id IN (%s)\n", tuple);




    //  RunTable(mdb, tn, q, NULL, -1, 0, FALSE, TRUE, TRUE, FALSE);

    //  InitTab(mdb, ST_UT, false);

    if (ado->openTable0(q)) {

        while (!ado->isEOF()) {
            CString s;
            CString s1 = ado->readStr(1);
            CString s2 = ado->readStr(2);
            h.print_tr();
            h.set_tr(5);

            css.css.mso_number_format = "\\@";
            css.css.text_align = "left";

            if (s1 == "" && s2 == "") {
                h.print_td("");
                h.print_td("");
            }
            else {
                h.print_td(s1);
                h.print_td(s2);
            }

            s = ado->readStr(3);
            //          s = getReal2(ST_UT, "kod_izol",s);

            h.print_td(s);
            s = ado->readStr(4);
            h.print_td(s);
            s = ado->readStr(5);
            //          s = getReal2(ST_UT, _TR("Материал наружного покрытия"),s);

            h.print_td(s);
            s = ado->readStr(6);
            h.print_td(s);
            s = ado->readStr(7);

            //          s = getReal2(ST_UT, _TR("Материал антикоррозийного покрытия"),s);

            h.print_td(s);

            h.print_tr2();
            ado->MoveNext();
    }
}
}

#endif


void CGidrView::RsLineTabIzol2(FILE* f, CNode2* node)
{
    HTML h(f, &css);

    //  RsLineIzolTabDb(node);

    //  CString mdb = getVyd();
    //  CString tn = _TR("Изоляция");
    CString q, vyd;

    if (!node) return;


    CAdoFile* ado = getAdo(getPsAdoName());

    CString tuple;

    create_vyd_line_tuple(ado, node->node.m_graph, tuple);

    q = readQ(ado, "izol", "Изоляция", 0);

    q.Replace("$tuple$", tuple);


    //  RunTable(mdb, tn, q, NULL, -1, 0, FALSE, TRUE, TRUE, FALSE);

    //  InitTab(mdb, ST_UT, false);

    if (ado->openTable0(q)) {

        while (!ado->isEOF()) {
            CString s;
            CString s1 = ado->readStr(1);
            CString s2 = ado->readStr(2);
            h.print_tr();
            h.set_tr(5);

            css.css.mso_number_format = "\\@";
            css.css.text_align = "left";

            if (s1 == "" && s2 == "") {
                h.print_td("");
                h.print_td("");
            }
            else {
                h.print_td(s1);
                h.print_td(s2);
            }

            s = ado->readStr(3);
            //          s = getReal2(ST_UT, "kod_izol",s);

            h.print_td(s);
            s = ado->readStr(4);
            h.print_td(s);
            s = ado->readStr(5);
            //          s = getReal2(ST_UT, _TR("Материал наружного покрытия"),s);

            h.print_td(s);
            s = ado->readStr(6);
            h.print_td(s);
            s = ado->readStr(7);

            //          s = getReal2(ST_UT, _TR("Материал антикоррозийного покрытия"),s);

            h.print_td(s);

            h.print_tr2();
            ado->MoveNext();
        }
    }
}


void CGidrView::MagLineTabIzol32(FILE* f, CNode2* n)
{
    CNode2* nz = m_cxema->m_graph->sameMarkNode();

    if (nz) {
        RsLineTabIzol2(f, nz);
    }
}

