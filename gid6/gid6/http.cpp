// Scroll.cpp : implementation file
// Scroll.cpp : implementation file
//

//#define __ADO


#include "stdafx.h"
#include "resource.h"
#include "Scroll.h"
#include "InputDia.h"
#include "PrintIni.h"
#include "win.h"
#include <math.h>
#include "MapDlg.h"
#include "mmenu.h"
#include "geomenu.h"
#include "ps_alma.h"
#include "json.h"

#include "ado.h"
#include "win.h"
#include "maptabs.h"
#include "geodez.h"

CString getPSTable(int id);
CString getPSTableEng(int id);

//int log_printf( const char *szFolder, const char *fmt, ... );

CString findTableRusNameGid(CString n);
void get_google_rect_json(CFRect rect, int width, int height, JSON& json);


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//#include "http/WebServerDlg.h"
#include "http2/WebServerDlg2.h"

//const int DD = 512;
//const double mas_coef = 1.5;

struct HTMLLayers {
    long id;
    CString code;
    CString name;
};


static HTMLLayers layers[] =
{
    L_GID, "gid", "Гидравлика",
    L_GEO, "geo", "Геобаза",
    L_PTS, "pts", "ПТС",
    L_ACT, "act", "",
    L_IND, "indikator_korrozii", "",
    L_DEFECT, "defect", "",
    L_REMONT, "remont", "",
    L_SHURFY, "shurfy", "",
    L_DIAG, "diag", "",
    L_OPRESSOVKA_SEKTSIONIRUYUSCHAYA_ZADVIZHKA, "opressovka_sektsioniruyuschaya_zadvizhka", "",
};

static CString m_user_gid = "???";


const int DMAS96 = 96;

const int DX = 4;
const int DY = 4;


const int DD = 256;
const double mas_coef = 2;


CString tileName(const CString& f);
CString tileName(int dpi, int m, int r, int c, int internalNodeID, int layer, int gid, int geo, int ms, int rs);

std::pair<double, double> mercatorToWgs84(double x_merc, double y_merc);
std::pair<double, double> wgs84ToMercator(double lat, double lon);

void convert_coord(double x1, double y1)
{
    x1 = x2lon(x1);
    y1 = y2lat(y1);

}

double x2lon_(double x);
double y2lat_(double y);
void d2m(int zona, double B, double L, double& x, double& y);
CFPoint get_google_to_coord(double x, double y);



CFPoint get_m_to_coord2(double x, double y);


CFPoint tile_coord1(int zz, int xx, int yy)
{
    double n2 = pow(2., zz);

    double x1 = (xx * 360. / n2 - 180.);
    double y1 = -(yy * 360. / n2 - 180.);

    CFPoint pt = get_m_to_coord2(x1, y1);

    /*
        double yy1 = x1 * 100000, X1;
        double xx1 = y1 * 100000, Y1;

        m2m2(m_system_coord, xx1, yy1, X1, Y1, true);

        pt.x = Y1 * 100;
        pt.y = -X1 * 100;
    */
    return pt;
}

/*
void tile_coord(int dx, int dy, double dmas, int zz, int xx, int yy, double &mas, CFRect &r)
{
    CFPoint pt1 = tile_coord1(zz, xx, yy);
    CFPoint pt2 = tile_coord1(zz, xx + dx, yy + dy);

    double masx = (pt2.x - pt1.x) / DD / dx;
    double masy = (pt2.y - pt1.y) / DD / dy;

    mas = (masx+masy) * dmas / 2;

    r = {pt1.x, pt1.y, pt2.x, pt2.y};
}
*/

/*

void tile_coord(int dx, int dy, double dmas, int zz, int xx, int yy, double &masx, double &masy, CFRect &r)
{
    CFPoint pt1 = tile_coord1(zz, xx, yy);
    CFPoint pt2 = tile_coord1(zz, xx + dx, yy + dy);

    CFPoint pt3 = tile_coord1(zz, xx + dx, yy);
    CFPoint pt4 = tile_coord1(zz, xx, yy + dy);

    masx = (pt2.x - pt1.x) / DD / dx;
    masy = (pt2.y - pt1.y) / DD / dy;


    double x1 = (pt1.x + pt4.x) / 2;
    double x2 = (pt2.x + pt3.x) / 2;

    double y1 = (pt1.y + pt3.y) / 2;
    double y2 = (pt2.y + pt4.y) / 2;

    masx = (x2 - x1) / DD / dx;
    masy = (y2 - y1) / DD / dy;



//    mas = (masx+masy) * dmas / 2;

//    r = {pt1.x, pt1.y, pt2.x, pt2.y};
    r = { x1, y1, x2, y2 };
}

*/

CFPoint rotatePoint(const CFPoint& p, double theta) {
    CFPoint rotated;
    rotated.x = p.x * std::cos(theta) - p.y * std::sin(theta);
    rotated.y = p.x * std::sin(theta) + p.y * std::cos(theta);
    return rotated;
}


void tile_coord(int dx, int dy, int zz, int xx, int yy, double& masx, double& masy, double& theta, CFRect& r)
{
    CFPoint pt1 = tile_coord1(zz, xx, yy);
    CFPoint pt2 = tile_coord1(zz, xx + dx, yy);
    CFPoint pt3 = tile_coord1(zz, xx + dx, yy + dy);
    CFPoint pt4 = tile_coord1(zz, xx, yy + dy);

    theta = atan2(pt2.y - pt1.y, pt2.x - pt1.x);

    CFPoint pt0 = { (pt1.x + pt2.x + pt3.x + pt4.x) / 4, (pt1.y + pt2.y + pt3.y + pt4.y) / 4 };

    CFPoint pts[4];

    pts[0] = pt1 - pt0;
    pts[1] = pt2 - pt0;
    pts[2] = pt3 - pt0;
    pts[3] = pt4 - pt0;

    for (int i = 0; i < 4; i++) {
        pts[i] = rotatePoint(pts[i], theta);
    }

    double x1 = (pt1.x + pt4.x) / 2;
    double x2 = (pt2.x + pt3.x) / 2;

    double y1 = (pt1.y + pt2.y) / 2;
    double y2 = (pt3.y + pt4.y) / 2;


    masx = (x2 - x1) / DD / dx;
    masy = (y2 - y1) / DD / dy;

    r = { x1, y1, x2, y2 };
}


// Получить номер по координатам

//void coord_tile1(int dx, int dy, double dmas, int zz, int xx, int yy, double& masx, double& masy, double& theta, CFRect& r)

void coord_tile1(int dx, int dy, double dmas, int zz, double x, double y, int& xx, int& yy)
{

    double n2 = pow(2., zz);

    double x1 = (xx * 360. / n2 - 180.);
    double y1 = -(yy * 360. / n2 - 180.);

    CFPoint pt = get_m_to_coord2(x1, y1);

    /*

        CFPoint pt1 = tile_coord1(zz, xx, yy);
        CFPoint pt2 = tile_coord1(zz, xx + dx, yy);
        CFPoint pt3 = tile_coord1(zz, xx + dx, yy + dy);
        CFPoint pt4 = tile_coord1(zz, xx, yy + dy);

        theta = atan2(pt2.y - pt1.y, pt2.x - pt1.x);

        CFPoint pt0 = { (pt1.x + pt2.x + pt3.x + pt4.x) / 4, (pt1.y + pt2.y + pt3.y + pt4.y) / 4 };

        CFPoint pts[4];

        pts[0] = pt1 - pt0;
        pts[1] = pt2 - pt0;
        pts[2] = pt3 - pt0;
        pts[3] = pt4 - pt0;

        for (int i = 0; i < 4; i++) {
            pts[i] = rotatePoint(pts[i], theta);
        }

        double x1 = (pt1.x + pt4.x) / 2;
        double x2 = (pt2.x + pt3.x) / 2;

        double y1 = (pt1.y + pt2.y) / 2;
        double y2 = (pt3.y + pt4.y) / 2;


        masx = (x2 - x1) / DD / dx;
        masy = (y2 - y1) / DD / dy;

        r = { x1, y1, x2, y2 };
    */
}




CString http_get_pro_table_ID(const char* mdb, const char* table, int ID);

static CGidrView* gidr_view = 0;

CGidrView* getGidView()
{
    CString ss;
    ss.Format("getGidView %d", gidr_view);
    log1(ss);

    return gidr_view;
}


void setGidView(CGidrView* view)
{
    CString ss;
    ss.Format("setGidView %d", view);
    log1(ss);

    gidr_view = view;
}

static set <CString> set_street;
static CString m_street;
static CString m_dom;
static CString m_tab;

CString http_get_TU(const char* tn, int id);

void OpenList2(CAdoFile* ado, const char* q, list< vector<COleVariant> >& lst);

void init_street(CAdoFile* dbf, const char* tab, const char* street, const char* dom)
{
    CString q;

    q.Format("SELECT DISTINCT %s FROM %s", transl(street), transl(tab));

    set_street.clear();

    m_tab = tab;
    m_street = street;
    m_dom = dom;

    if (dbf->openTable0(q)) {
        while (!dbf->isEOF()) {
            CString s = dbf->readStr(0);
            if (s != "") set_street.insert(s);

            dbf->MoveNext();
        }
        dbf->closeTable();
    }
}

static map<CString, map<CString, CString> > map_podp;

CString get_podp(const char* tn1, const char* fn1)
{
    CString tn = tn1;
    CString fn = fn1;
    tn.MakeUpper();
    fn.MakeUpper();

    map<CString, map<CString, CString> >::const_iterator it1 = map_podp.find(tn);
    if (it1 != map_podp.end()) {
        map<CString, CString>::const_iterator it2 = it1->second.find(fn);
        if (it2 != it1->second.end()) {
            return it2->second;
        }
    }
    return fn1;
}

void init_podp()
{
}

static bool m_isWeb = false;

bool isWeb()
{
    return m_isWeb;
}


void CGidrView::OnHttp2()
{
    m_CurNode = NULL;
    m_CurLine = NULL;

    void init_mime();
    init_mime();

    init_podp();

    Klassif* kls = NULL;

    if (!kls) {
        kls = m_geofile->m_kl_list->findKlN(transl("Здания"));
        if (kls) {
            init_street(kls->m_ado, _TR("Здания"), "NAME_STREE", "NUMBER_1");
        }
    }

    if (!kls) {
        kls = m_geofile->m_kl_list->findKlN(transl("Здания Карта"));
        if (kls) {
            init_street(kls->m_ado, _TR("Здания Карта"), "STREET_NAM", "NUMBER_1");
        }
    }

    if (!kls) {
        kls = m_geofile->m_kl_list->findKlN(transl("Здания потребителей"));
        if (kls) {
            init_street(kls->m_ado, _TR("Здания потребителей"), _TR("Улица"), _TR("Дом"));
        }
    }


    gidr_view = this;

    m_isWeb = true;
    m_off = true;
    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
    if (pDoc) {
        m_user_gid = pDoc->m_user_gid;
    }

    CString http_get_address_alma();
    CString http_tree_geo();

    CString http_tree_ms();
    CString http_tree_rs();



    CString filename = tileName("address.json");
    CString message = http_get_address_alma();
    mkdir_for_file(filename);
    saveToFile(filename, message);

    filename = tileName("tree_ms.json");
    message = http_tree_ms();
    saveToFile(filename, message);

    filename = tileName("tree_rs.json");
    message = http_tree_rs();
    saveToFile(filename, message);

    filename = tileName("tree_geo.json");
    message = http_tree_geo();
    saveToFile(filename, message);



#if 1
    CWebServerDlg2 dlg(this);
    //  m_pMainWnd = &dlg;
    int nResponse = dlg.DoModal();
#else 
    CWebServerDlg2* dlg = new CWebServerDlg2(this);

    if (dlg != NULL) {
        BOOL ret = dlg->Create(IDD_WEBSERVER_DIALOG2, this);

        if (!ret) {
            AfxMessageBox(_T("Error creating Dialog"), MB_OK | MB_ICONINFORMATION);
            delete dlg;
            return;
        }
    }

#endif
    gidr_view = 0;
    m_isWeb = false;
    m_off = false;


    Invalidate();
}



void CGidrView::OnHttp8080()
{
    m_CurNode = NULL;
    m_CurLine = NULL;

    void init_mime();
    init_mime();

    init_podp();

    Klassif* kls = NULL;

    if (!kls) {
        kls = m_geofile->m_kl_list->findKlN(transl("Здания"));
        if (kls) {
            init_street(kls->m_ado, _TR("Здания"), "NAME_STREE", "NUMBER_1");
        }
    }

    if (!kls) {
        kls = m_geofile->m_kl_list->findKlN(transl("Здания Карта"));
        if (kls) {
            init_street(kls->m_ado, _TR("Здания Карта"), "STREET_NAM", "NUMBER_1");
        }
    }

    if (!kls) {
        kls = m_geofile->m_kl_list->findKlN(transl("Здания потребителей"));
        if (kls) {
            init_street(kls->m_ado, _TR("Здания потребителей"), _TR("Улица"), _TR("Дом"));
        }
    }


    gidr_view = this;

    m_isWeb = true;

#if 1
    CWebServerDlg2 dlg(this);
    //  m_pMainWnd = &dlg;
    int nResponse = dlg.DoModal();
#else 
    CWebServerDlg2* dlg = new CWebServerDlg2(this);

    if (dlg != NULL) {
        BOOL ret = dlg->Create(IDD_WEBSERVER_DIALOG2, this);

        if (!ret) {
            AfxMessageBox(_T("Error creating Dialog"), MB_OK | MB_ICONINFORMATION);
            delete dlg;
            return;
        }
    }

#endif
    gidr_view = 0;
    m_isWeb = false;

    Invalidate();
}






#if 0

#if 1

bool http_get_border(int m, int& x1, int& y1, int& x2, int& y2, int internalNodeID)
{
    if (!gidr_view) return false;

    KamState ks = gidr_view->Kam(internalNodeID);

    CFRect rect = gidr_view->getMaxRect();
    CFRect r = gidr_view->m_rectBorder;

    double dmas = gidr_view->dmas;

    double mas = 64000;
    //    double mas = 200000;

    for (int i = 1; i < m; i++) {
        mas /= mas_coef;
    }

    x1 = (r.left) * dmas / DD / mas;
    y1 = (r.top) * dmas / DD / mas;
    x2 = (r.right) * dmas / DD / mas;
    y2 = (r.bottom) * dmas / DD / mas;

    if (x2 <= x1) x2 = x1 + 1;
    if (y2 <= y1) y2 = y1 + 1;

    gidr_view->KamObr(ks);

    return true;
}

#else


bool http_get_border(int m, int& x1, int& y1, int& x2, int& y2, int internalNodeID)
{
    if (!gidr_view) return false;

    KamState ks = gidr_view->Kam(internalNodeID);

    CFRect rect = gidr_view->getMaxRect();
    CFRect r = gidr_view->m_rectBorder;

    double dmas = gidr_view->dmas;

    double mas = 64000;
    //    double mas = 200000;

    for (int i = 1; i < m; i++) {
        mas /= mas_coef;
    }

    x1 = (r.left) * dmas / DD / mas;
    y1 = (r.top) * dmas / DD / mas;
    x2 = (r.right) * dmas / DD / mas;
    y2 = (r.bottom) * dmas / DD / mas;

    if (x2 <= x1) x2 = x1 + 1;
    if (y2 <= y1) y2 = y1 + 1;

    gidr_view->KamObr(ks);

    return true;
}
#endif
#endif


int viewUchastok(CGraph2* m_graph, tuple<int, int> msrs, int type /*1 - Магистраль 2 Распредсеть 3 Магистраль и распредсеть*/, bool is_only_big_pipe);

void savePNG(HDC& hdc, int nWidth, int nHeight, const char* fn);
void savePNG4(HDC& hdc, int x0, int y0, int nWidth, int nHeight, const char* fn);
void draw_ramka(CDC& dc, const CString& txt, int dx, int dy);


void set_kls_layer(int layer)
{
    //    m_geofile->m_kl_list;

    Klassif* kls = nullptr;

    //    m_geofile->m_kl_list->zero_ins();

    if (layer == L_GEO) {
        for (auto& it : *m_geofile->m_kl_list) {
            Klassif* kls = &it.second;

            bool is_pts(const CString & tn);
            if (is_pts(kls->nazv)) {
                kls->ins = 0;
            }
        }
    }
    else {
        m_geofile->m_kl_list->zero_ins();
    }


    if (layer == L_PTS) {
        for (auto& it : *m_geofile->m_kl_list) {
            Klassif* kls = &it.second;

            bool is_pts(const CString & tn);
            if (is_pts(kls->nazv)) {
                kls->ins = 1;
            }
        }
    }


    for (int i = 0; i < sizeof(layers) / sizeof(layers[0]); i++) {
        if (layers[i].name == "") {
            kls = m_geofile->m_kl_list->findKlN(layers[i].code);
            if (kls) kls->ins = (layer == layers[i].id);
        }
    }
    /*
    kls = m_geofile->m_kl_list->findKlN("indikator_korrozii");
    if (kls) kls->ins = (layer == L_IND);
    kls = m_geofile->m_kl_list->findKlN("defect");
    if (kls) kls->ins = (layer == L_DEFECT);
    kls = m_geofile->m_kl_list->findKlN("act");
    if (kls) kls->ins = (layer == L_ACT);
    */


}


bool http_make_png(const char* fn1, int dpi, int zz, int yy, int xx, int internalNodeID, int layer, int n_layers, int* layers, int gid, int geo, int ms, int rs)
{
    clock_t t1 = clock();
    clock_t t1_1, t1_2, t1_3, t1_4, t2_1, t2_2, t3_1, t3_2, t4_1, t4_2;
    clock_t d_t1 = 0, d_t2 = 0, d_t3 = 0, t5_1, t5_2, t6_1, t6_2;


    if (!gidr_view) {
        CString ss;
        ss.Format("Ошибка при получении картинки %d %d %d", zz, xx, yy);

        log1(ss);
        return false;
    }

    KamState ks = gidr_view->Kam(internalNodeID);

    gidr_view->m_web_layer = layer;

    m_geofile->m_kl_list->save_ins();
    set_kls_layer(layer);

    CFRect rect = gidr_view->getMaxRect();
    //    double dmas = DMAS96;
    double dmas = dpi / 2.54;

    CFRect r;

    int dx = DX, dy = DY;

    int xx1 = xx / dx * dx, xx2 = xx1 + dx - 1;
    int yy1 = yy / dy * dy, yy2 = yy1 + dy - 1;

    //    tile_coord(dx, dy, dmas, zz, xx1, yy1, mas, r);
    double masx, masy, alpha;

    tile_coord(dx, dy, zz, xx1, yy1, masx, masy, alpha, r);

    bool yes = false;


    if (r.bottom >= rect.top && r.right >= rect.left && r.top < rect.bottom && r.left < rect.right) {

        CGraph2* graph = gidr_view->m_cxema->m_graph;

        if (layer == L_ALL || layer == L_GID) {
            if (ms) {
                graph->reset();
                viewUchastok(graph, make_tuple(ms, 0), 1, true);
            }
            else if (rs) {
                graph->reset();
                viewUchastok(graph, make_tuple(0, rs), 2, true);
            }
        }

        double dmas_old = gidr_view->geom.dmas;
        //        gidr_view->geom.dmas = DMAS96;
        gidr_view->geom.dmas = dmas;
        gidr_view->geom.m_alpha = -alpha;


        t1_1 = clock();

        gidr_view->set_coord_gid(r.center().x, r.center().y, gidr_view->geom.m_alpha, layer == L_ALL || layer == L_GID);
        t1_2 = clock();



        CClientDC oDC(gidr_view);

        CDC dc;
        dc.CreateCompatibleDC(&oDC);

        CBitmap bitmap;
        bitmap.CreateCompatibleBitmap(&oDC, DD * dx, DD * dy);
        dc.SelectObject(bitmap);

        CString ss, s_layer = "";

        for (int l = 0; l < layer; l++) {
            s_layer += "   ";
        }
        ss.Format("%-3d", layer);
        s_layer += ss;

        //        ss.Format("%d %d %d %g - %s", zz, xx1, yy1, alpha*180./M_PI, s_layer);
        ss.Format("1:%7.0f %3d %d %d %d - %s", masx * gidr_view->geom.dmas, dpi, zz, xx1, yy1, s_layer);

        t2_1 = clock();
        gidr_view->CopyBlockPng4(dc, masx, masy, r.left, r.top, DD * dx, DD * dy);
        draw_ramka(dc, ss, DD * dx, DD * dy);
        t2_2 = clock();


        double d_1 = 0;
        double d_2 = 0;

        t5_1 = clock();

        for (int yyy = yy1; yyy <= yy2; yyy += 1) {
            for (int xxx = xx1; xxx <= xx2; xxx += 1) {
                t6_1 = clock();
                CString fn = tileName(dpi, zz, yyy, xxx, internalNodeID, layer, gid, geo, ms, rs);
                t6_2 = clock();

                t4_1 = clock();
                mkdir_for_file(fn);
                t4_2 = clock();

                t3_1 = clock();
                ::savePNG4(dc.m_hDC, (xxx - xx1) * DD, (yyy - yy1) * DD, DD, DD, fn);
                t3_2 = clock();

                d_t1 += t6_2-t6_1;
                d_t2 += t4_2-t4_1;
                d_t3 += t3_2-t3_1;

            }
        }

        t5_2 = clock();


        gidr_view->geom.dmas = dmas_old;
        gidr_view->geom.m_alpha = 0;

        t1_3 = clock();
        gidr_view->set_coord_gid(0, 0, 0, layer == L_ALL || layer == L_GID);
        t1_4 = clock();

        if (layer == L_ALL || layer == L_GID) {
            if (ms || rs) {
                graph->reset();
            }
        }

        clock_t t2 = clock();

        log1(ss);

        yes = true;
    }

    gidr_view->m_web_layer = L_ALL;
    m_geofile->m_kl_list->restore_ins();

    gidr_view->KamObr(ks);


    clock_t t2 = clock();

    log1("Делаю картинку %s всего %f секунд, на пересчет %.1f%%, на рисование %.1f%% сохранение %.1f%% ( %.1f%%  %.1f%%   %.1f%%)",
        fn1,
        (double)(t2 - t1) / CLOCKS_PER_SEC,
        (double)(t1_2 - t1_1 + t1_4 - t1_3) / (t2 - t1) * 100,
        (double)(t2_2 - t2_1) / (t2 - t1) * 100,
        (double)(t5_2 - t5_1) / (t2 - t1) * 100,
        (double) d_t1 / (t2 - t1) * 100,
        (double) d_t2 / (t2 - t1) * 100,
        (double) d_t3 / (t2 - t1) * 100
    );


    return yes;
}

bool http_make_zero_png(const char* fn)
{
    log1("Делаю пустую картинку");

//    CString txt;
//    txt.Format("Нет Данных");

    CClientDC oDC(gidr_view);

    CDC dc;
    dc.CreateCompatibleDC(&oDC);

    CBitmap bitmap;
    bitmap.CreateCompatibleBitmap(&oDC, DD, DD);
    dc.SelectObject(bitmap);

    dc.SetBkColor(0xFFFFFF);
    CBrush brush(0xFFFFFF);
    CRect rect(0, 0, DD, DD);
    dc.FillRect(rect, &brush);

//    draw_ramka(dc, txt, DD, DD);
    ::savePNG(dc.m_hDC, DD, DD, fn);

    return true;
}




void findNode(CGraph2* graph, const CFPoint& pt, list<CNode2*>& list_node, double m5)
{
    CNode2* n0 = NULL;
    double h;

    CMapGraph::iterator p = graph->map_node.begin();

    while (p != graph->map_node.end()) {
        CNode2* node = p->second;
        h = hypot(pt.x - node->node.coord.x, pt.y - node->node.coord.y);
        if (h < m5) {
            list_node.push_back(node);
        }
        p++;
    }
}

void findLine(CGraph2* graph, const CFPoint& pt, list<CLINE2*>& list_line, double m5)
{
    CLINE2* l;

    CMapGraph::const_iterator p = graph->map_node.begin();

    while (p != graph->map_node.end()) {
        CNode2* node = p->second;

        if (node->node.coord.x != 0 && node->node.coord.y != 0) {
            for (l = node->lines; l; l = next(l)) {
                if (other(l)->node.coord.x != 0 && other(l)->node.coord.y != 0) {
                    if (IsBegin(l) && chline(bline(l), pt.x, pt.y, m5)) {
                        list_line.push_back(l);
                    }
                }
            }
        }
        p++;
    }
}

CLINE2* findLine1(CGraph2* graph, const CFPoint& pt, const char* tn, double m5)
{
    CLINE2* l;

    CMapGraph::const_iterator p = graph->map_node.begin();

    while (p != graph->map_node.end()) {

        CNode2* n = p->second;

        if (n->node.coord.x != 0 && n->node.coord.y != 0) {
            for (l = n->lines; l; l = next(l)) {
                if (other(l)->node.coord.x != 0 && other(l)->node.coord.y != 0) {
                    if (IsBegin(l) && chline(bline(l), pt.x, pt.y, m5)) {
                        return l;
                    }
                }
            }
        }
        p++;
    }
    return NULL;
}



#if 0
CString http_get_pro_find(const char* tn, int loc, double x, double y)
{
    if (!gidr_view) return "";

    double mas = 500;
    //    double dmas = gidr_view->dmas;
    double dmas = DMAS96;
    //  double dmas = 100;

    double masx_old = gidr_view->geom.masx, masy_old = gidr_view->geom.masy, bx_old = gidr_view->geom.m_bx, by_old = gidr_view->geom.m_by;
    gidr_view->geom.masx = gidr_view->geom.masy = mas / dmas;


    CFRect rect = gidr_view->getMaxRect();

    double x0 = x * 1000000 / dmas;
    double y0 = y * 1000000 / dmas;

    CFPoint pt = CFPoint(x0, y0);

    CGraph2* graph = gidr_view->m_cxema->m_graph;

    double md = D5 * mas / dmas;

    //  findNode(graph, pt, list_node, md*2);
    CLINE2* l = findLine1(graph, pt, tn, md);

    gidr_view->geom.masx = masx_old; gidr_view->geom.masy = masy_old;
    gidr_view->geom.m_bx = bx_old; gidr_view->geom.m_by = by_old;

    if (l) {
        return http_get_pro_table_ID(where(l)->getFileName(), tn, bline(l)->line.nomP);
    }

    return "";
}
#endif

CString http_get_map_rect(int internalNodeID, int width, int height)
{
    if (!gidr_view) return "";


    CFRect rect = gidr_view->getMaxRect();

    JSON json;

    get_google_rect_json(rect, width, height, json);

/*


    KamState ks = gidr_view->Kam(internalNodeID);

    //    double dmas = gidr_view->dmas;
    double dmas = DMAS96;
    CFRect rect = gidr_view->getMaxRect();
    CFRect rr;

    rr.left = (rect.left) * dmas / 1000000;
    rr.right = (rect.right) * dmas / 1000000;
    rr.top = (rect.top) * dmas / 1000000;
    rr.bottom = (rect.bottom) * dmas / 1000000;

    JSON json;

    json.add("{");
    json.add("x1", rect.left * dmas);
    json.add("x2", rect.right * dmas);
    json.add("y1", rect.top * dmas);
    json.add("y2", rect.bottom * dmas);

    json.add("xx1", rr.left);
    json.add("xx2", rr.right);
    json.add("yy1", rr.top);
    json.add("yy2", rr.bottom);

    json.add("}");

    gidr_view->KamObr(ks);
*/

    return json.str();
}

CFPoint get_google_to_coord(double x, double y);

double get_mas(double dmas, double z)
{
    double n2 = pow(2., z);
    const double R = 6378137.0; // Радиус Земли для Web Mercator
    double dH = 2. * M_PI * R / n2 * 100;
    double mas = dH / DD * dmas;

    return mas;
}

double get_z(double dmas, double mas)
{
    const double R = 6378137.0; // Радиус Земли для Web Mercator

    double dH = mas * DD / dmas;
    double n2 = (2. * M_PI * R * 100) / dH;

    double z = log(n2) / log(2.);

    return z;
}

CString http_get_map_info(int z, double x, double y, int internalNodeID, int geo)
{
    if (!gidr_view) return "";
    //    if (mas > 3000) return "";

    //    double dmas = gidr_view->dmas;
//    double dmas = DMAS96;

    double dmas = gidr_view->geom.dmas;

    KamState ks = gidr_view->Kam(internalNodeID);

    log1("http_get_map_info  z=%d xy = %f %f", z, x, y);

    CFPoint pt = get_google_to_coord(x, y);
    double mas = get_mas(dmas, z);

    double masx_old = gidr_view->geom.masx, masy_old = gidr_view->geom.masy, bx_old = gidr_view->geom.m_bx, by_old = gidr_view->geom.m_by;
    gidr_view->geom.masx = gidr_view->geom.masy = mas / dmas;

    CFRect rect = gidr_view->getMaxRect();

    CGraph2* graph = gidr_view->m_cxema->m_graph;

    double md = D5 * mas / dmas;

    list<CLINE2*> list_line;
    list<CNode2*> list_node;
    findNode(graph, pt, list_node, md * 2);
    findLine(graph, pt, list_line, md);

    JSON json;

    json.add("{");

    {
        json.add("\"lines\": [");
        for (list<CLINE2*>::const_iterator it = list_line.begin(); it != list_line.end(); it++) {
            CLINE2* line = *it;

            json.add("  {");
            json.add("db", where(line)->getFile()->m_schema);
            json.add("node1", where(line)->id);
            json.add("node2", other(line)->id);

            int nomgP = bline(line)->line.nomgP != -1 ? bline(line)->line.nomgP : bline(line)->line.nomgO;

            json.add("nomg", nomgP);

            int nomP = bline(line)->line.nomP != -1 ? bline(line)->line.nomP : bline(line)->line.nomO;
            json.add("id", nomP);

            int idP2 = bline(line)->line.idP2 != -1 ? bline(line)->line.idP2 : bline(line)->line.idO2;
            json.add("id2", idP2);

            json.add("table_rus", bline(line)->getTable());
            json.add("table", bline(line)->getTableMySQL());
            json.add("out", bline(line)->getTableOut());
            json.add("name", bline(line)->getName());
            json.add("}");
        }
        json.add("]");
    }
    {
        json.add("\"nodes\" : [");
        for (list<CNode2*>::const_iterator it = list_node.begin(); it != list_node.end(); it++) {
            CNode2* node = *it;
            json.add("{");
            json.add("db", node->getFile()->m_schema);
            json.add("nomgP", node->node.nomgP);
            json.add("nomgO", node->node.nomgO);
            json.add("id", node->id);
            json.add("id2", node->node.id2);
            json.add("internalNodeID", node->node.internalNodeID);
            json.add("table_rus", node->getTable());
            json.add("table", node->getTableMySQL());
            json.add("out", node->getTableOut());
            json.add("node", node->getName());
            json.add("name", node->getName());

            json.add("namePTS", node->getNamePT());
            json.add("nodeType", node->typPS());
            json.add("nodeTypeNameRus", getPSTable(node->typPS()));
            json.add("nodeTypeName", getPSTableEng(node->typPS()));

            json.add("}");
        }
    }
    json.add("]");

    if (internalNodeID == 0 && geo == 1) {
        CGeoMenu menu(gidr_view, IDD_GEO_MENU);
        m_geofile->findAllXY(&menu, gidr_view, pt, md);
        json.add("\"geo\" : [");

        {
            for (int i = 0; i < menu.list.GetSize(); i++) {
                //      m_List.AddString(list[i]->nazv.c_str());

                CString table = menu.list[i]->nazv;

                json.add("{");

                Klassif* kls = m_geofile->m_kl_list->findKlN(table);

                if (kls && kls->m_ado) {
                    json.add("db", kls->m_ado->m_schema);
                }

                CString ss = kls->RusName();

                CGeoObject* geo = kls->getGeoObjectById(menu.list_num[i]);

                if (geo && geo->text00000 != "") {
                    ss.Format("%s %s", kls->RusName(), geo->text00000);
                }

                json.add("table_rus", ss);
                json.add("table", table);
                json.add("id", menu.list_num[i]);
                json.add("}");
            }
        }

        json.add("]");
    }
    json.add("}");

    gidr_view->geom.masx = masx_old; gidr_view->geom.masy = masy_old;
    gidr_view->geom.m_bx = bx_old; gidr_view->geom.m_by = by_old;

    gidr_view->KamObr(ks);

    return json.str();
}

HRESULT __fastcall UnicodeToAnsi(LPCOLESTR pszW, LPSTR* ppszA);

#include "join.h"
#include "PropertyGrid\test\PropGridDlg.h"
int ado_add_table(CAdoFile* ado, JSON& json, Join& join, const char* cap);

extern CString m_error;

CString http_get_geo(const char* tn, int id)
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);

    if (!kls) return "";

    CAdoFile* ado = kls->m_ado;

    if (!ado) return "";

    ////    if (FAILED(::CoInitialize(NULL))) return "";

    bool readFun(const char* tab, const char* tn);

    readFun("tab", tn);


    JSON json;

    Join join(tn, "", id, ado);

    json.add("{");

    json.add("id", id);
    json.add("\"groups\" : [");


    MySQLColumns* col = ado->findMySQLColumns(tn, "lineID");
    if (col) {
        //      Join join_l(tn, id, ado);
        //      ado_add_table(ado, json, join_l, tn);
    }

    ado_add_table(ado, json, join, tn);

    json.add("]");
    json.add("}");

    return json.str();
}

CString http_get_dropdown(const char* file, const char* q)
{
    JSON json;

    CAdoFile* ado = getAdo(file);

    if (!ado) return "";

    list< vector<COleVariant> > lst;

    ////     if (FAILED(::CoInitialize(NULL))) return "";

    CString q0 = q;
    //    q0 += " LIMIT 100";

    OpenList2(ado, q0, lst);

    ////    ::CoUninitialize();

    json.add("[");

    list< vector<COleVariant> >::const_iterator it = lst.begin();

    for (; it != lst.end(); it++) {

        vector<COleVariant> v = *it;

        int ID = v[0].lVal;
        CString name2 = CCrack::strVARIANT3(v[1]);

        json.add("{");
        json.add("id", ID);
        json.add("name", name2);
        json.add("}");
    }

    json.add("]");






    return json.str();
}

void read_txt2(FILE* f, vector <CString>& tabs, vector <vector <CString> >& fields);

void init_tab(const char* tn)
{
    char s[256];

    sprintf(s, "tabs3/%s.txt", tn);
    FILE* f = fopenexe(s, "r");
    if (f) {
        vector <CString> tabs;
        vector <vector <CString> > fields;
#if NOMYSQL
        read_txt2(f, tabs, fields);
#endif
        fclose(f);
    }
}


int make_list_table(CAdoFile* ado, list<CGridData>& lst, const char* table, long id, const char* q0 = NULL);


#if 0

int ado_add_table(CAdoFile* ado, JSON& json, Join& join)
{
    long idt = 1;

    if (!join.is_open) return 0;

    CString tn = join.m_tn;
    int id = join.m_id;
    CString q = join.getQ();

    list<CGridData> lst;
    int n = make_list_table(ado, lst, tn, id, q);
    //  pm_cEditDlg->Populate(tn, lst);

    if (!n) return 0;

    //  HSECTION hs = cEditDlg->addSection(tn);

    list<CGridData>::const_iterator it = lst.begin();

    //    HITEM hi;

    for (; it != lst.end(); it++) {
        CGridData dg = *it;

        map<CString, CString>::const_iterator itf = join.map_foreign.find(dg.name);

        bool google_doc = false;

        if (itf != join.map_foreign.end()) {
            COleVariant value = dg.value;
            CString name = itf->second;

            it++;
            if (it == lst.end()) {
                break;
            }
            CGridData dg1 = *it;
            dg1.label = dg.label;
            dg1.name = dg.name;

            json.add("{");
            json.add("key", dg.label);
            json.add("name", dg.name);
            json.add("value", CCrack::strVARIANT3(dg1.value));

            long id = to_long(dg.value);

            json.add("idvalue", id);
            json.add("typ", strAdoFieldType(dg1.typ));
            json.add("DefinedSize", dg1.DefinedSize);
            json.add("DropDown", name);
            json.add("db", ado->m_schema);
            json.add("table", dg1.table);
            json.add("id", dg1.id);
            json.add("readonly", 0);
            json.add("google_doc", google_doc);
            json.add("}");
        }
        else {
            //      hi = cEditDlg->addDataGrid(hs, dg);

            json.add("{");
            json.add("key", dg.label);
            json.add("name", dg.name);
            json.add("value", CCrack::strVARIANT3(dg.value));
            json.add("typ", strAdoFieldType(dg.typ));
            json.add("DefinedSize", dg.DefinedSize);
            json.add("DropDown", "");
            json.add("db", ado->m_schema);
            json.add("table", dg.table);
            json.add("id", dg.id);
            json.add("readonly", 0);
            json.add("google_doc", google_doc);
            json.add("}");
        }

        //    cEditDlg->m_ctrlGrid.SetItemHelp(hi, string(dg.help));
        //    cEditDlg->m_ctrlGrid.SetItemTable(hi, string(ado->m_schema), string(tn), id);
    }

    return n;
}
#endif

CString getRusName(const char* tn);
bool readTab(const char* tn, const char* real_tn, int table_num, map <int, CString>& map_1, map <int, map <int, CString> >& map_2, map <CString, CString>& map_3);

int ado_add_table(CAdoFile* ado, JSON& json, Join& join, const char* cap0)
{
    INT NUMBER = 0;

    if (!join.is_open) return 0;

    CString tn = join.m_tn;
    CString tnn = getRusName(tn);

    CString cap = getRusName(cap0);


    int id = join.m_id;
    CString q = join.getQ();

    list<CGridData> lst;
    int n = make_list_table(ado, lst, tn, id, q);

    if (!n) return 0;

    //    CString tnn = tn;
    //    if (tn_txt && tn_txt[0]) tnn = tn_txt;

    map <int, CString> map_1;
    map <int, map <int, CString> > map_2;
    map <CString, CString> map_3;

    if (!readTab(tn, tnn, 0, map_1, map_2, map_3)) {
        if (cap)
            map_1[0] = cap;
        else
            map_1[0] = tnn;

        //        map_1[0] = _TR("Информация");

        list<CGridData>::const_iterator it = lst.begin();

        int n = 0;
        for (; it != lst.end(); it++) {
            CGridData dg = *it;

            map<CString, CString>::const_iterator itf = join.map_foreign.find(dg.name);

            if (dg.name != "shape") {
                map_2[0][n++] = dg.getIdent();
            }
            if (itf != join.map_foreign.end()) it++;
        }
    }

    //    readFun(tn);

    //    HITEM hi;
    //    HSECTION hs;

    bool google_doc = false;

    map <int, CString>::const_iterator it1 = map_1.begin();

    for (; it1 != map_1.end(); it1++) {
        CString s1 = it1->second;
        //        hs = cEditDlg->addSection(s1, number != 0, number++);

        json.add("{");
        json.add("name", s1);


        ///  секция открывается

        json.add("\"fields\" : [");


        map <int, map <int, CString> >::const_iterator it2 = map_2.find(it1->first);

        if (it2 != map_2.end()) {

            map <int, CString>::const_iterator it3 = it2->second.begin();

            for (; it3 != it2->second.end(); it3++) {
                CString s2 = it3->second;

                if (s2[0] == '$') {

                    map <CString, CString>::const_iterator it3 = map_3.find(s2);
                    if (it3 != map_3.end()) {

                        vector<CString> match;

                        int l = regex_match("\\$view_documents\\$(.+)\\$(.+)\\$", s2, match);
                        if (l >= 1) {
                            CString tab1 = match[1];
                            CString tab2 = match[2];

                            CString q;
                            q.Format("SELECT d.path, t.name FROM %s d LEFT JOIN %s t ON t.id=d.remontDocumentTypeID WHERE d.objID=%d ORDER BY t.id", tab1, tab2, id);

                            if (ado->openTable0(q)) {
                                while (!ado->isEOF()) {
                                    CString path = ado->readStr(0);
                                    CString name = ado->readStr(1);

                                    json.add("{");
                                    json.add("key", it3->second + " " + name);
                                    json.add("name", q);

                                    json.add("value", path);
                                    json.add("path", path);
                                    json.add("typ", "link");

                                    json.add("idvalue", id);
                                    //                        json.add("typ", strAdoFieldType(dg1.typ));
                                    //                        json.add("DefinedSize", dg1.DefinedSize);
                                    json.add("DropDown", "");
                                    json.add("db", ado->m_schema);
                                    //                        json.add("table", dg1.table);
                                    json.add("id", id);
                                    json.add("readonly", 0);
                                    json.add("google_doc", google_doc);
                                    json.add("}");


                                    ado->MoveNext();
                                }
                                ado->closeTable();
                            }
                        }
                    }
                }
                else {

                    list<CGridData>::const_iterator it = lst.begin();

                    for (; it != lst.end(); it++) {
                        CGridData dg = *it;

                        if (dg.name.CompareNoCase("calculationID") == 0) continue;
                        if (dg.name.CompareNoCase("nodeID") == 0 && _stricmp(tn, "pressRegulators")) continue;
                        if (dg.name.CompareNoCase("externalSign") == 0) continue;

                        map <CString, CString>::const_iterator it3 = map_3.find(dg.getIdent());
                        if (it3 != map_3.end()) {
                            dg.label = it3->second;
                        }

                        if (dg.getIdent() == s2) {
                            CString getFun(CString name);
                            bool funFileInfo0(CString fun, CString & ext, CString & filter, CString & path);

                            CString ext, filter, path = "";
                            CString fun = getFun(dg.name);

                            if (fun != "") {
                                if (funFileInfo0(fun, ext, filter, path)) {
                                }
                            }

                            map<CString, CString>::const_iterator itf = join.map_foreign.find(dg.name);

                            if (itf != join.map_foreign.end()) {
                                COleVariant value = dg.value;
                                CString name = itf->second;

                                it++;
                                if (it == lst.end()) {

                                    break;
                                }
                                CGridData dg1 = *it;
                                dg1.label = dg.label;
                                dg1.name = dg.name;

                                json.add("{");
                                json.add("key", dg.label);
                                json.add("name", dg.name);
                                json.add("value", CCrack::strVARIANT3(dg1.value));

                                long id = to_long(dg.value);

                                json.add("idvalue", id);
                                json.add("typ", strAdoFieldType(dg1.typ));
                                json.add("DefinedSize", dg1.DefinedSize);
                                json.add("DropDown", name);
                                json.add("db", ado->m_schema);
                                json.add("table", dg1.table);
                                json.add("id", dg1.id);
                                json.add("readonly", 0);
                                json.add("google_doc", google_doc);
                                json.add("}");
                            }
                            else {
                                //      hi = cEditDlg->addDataGrid(hs, dg);

                                json.add("{");
                                json.add("key", dg.label);
                                json.add("name", dg.name);

                                CString val = CCrack::strVARIANT3(dg.value);

                                if (fun != "") {
                                    val.Replace("\\\\", "\\");
                                    json.add("value", val);
                                    json.add("path", path);
                                    json.add("typ", "link");
                                }
                                else {
                                    json.add("value", val);
                                    json.add("path", "");
                                    json.add("typ", strAdoFieldType(dg.typ));
                                }


                                json.add("DefinedSize", dg.DefinedSize);
                                json.add("DropDown", "");
                                json.add("db", ado->m_schema);
                                json.add("table", dg.table);
                                json.add("id", dg.id);
                                json.add("readonly", 0);
                                json.add("google_doc", google_doc);
                                json.add("}");
                            }

                            //                        if (readonly) {
                            //                            cEditDlg->m_ctrlGrid.SetItemReadOnly(hi);
                            //                        }


                            //                        cEditDlg->m_ctrlGrid.SetItemHelp(hi, string(dg.help));
                            //                        cEditDlg->m_ctrlGrid.SetItemTable(hi, string(ado->m_schema), string(tn), id);

                            /*
                                                    map<CString, CString>::const_iterator it2 = map_fun.find(dg.name);
                                                    if (it2 != map_fun.end()) {
                                                        cEditDlg->m_ctrlGrid.SetItemFun(hi, string(it2->second));
                                                    }
                            */
                        }
                    }
                }
            }
        }


        ///   секция закрывается
        json.add("]");
        json.add("}");


    }
    return n;
}

int get_ID2(const char* name, const char* name2);


//int ado_add_table_PR(CAdoFile *ado, CPropGridDlg *cEditDlg, Join &join, const char *schemeNum1, const char *cap)
int ado_add_table_PR(CAdoFile* ado, JSON& json, Join& join, const char* cap0)
{
    char* ss[] = {
    "Основные",
    "Нагрузки",
    "Дроссели",
    "Оборудование",
    "Характерные точки",
    "Здание",
    "Перспектива",
    "Вывод на схему",
    "Строка заголовка окна",
    "Остальные"
    };
    bool google_doc = false;

    if (!join.is_open) return 0;

    CString tn = getRusName(join.m_tn);


    //    readFun(tn);

    int id = join.m_id;
    CString q = join.getQ();

    list<CGridData> lst;
    int n = make_list_table(ado, lst, tn, id, q);
    //  pm_cEditDlg->Populate(tn, lst);

    if (!n) return 0;

    CString schemeNum = "";

    for (list<CGridData>::const_iterator it = lst.begin(); it != lst.end(); it++) {
        CGridData dg = *it;

        if (dg.name == "schemeNum") {
            schemeNum = CCrack::strVARIANT2(dg.value);
            break;
        }
    }

    int number = 0;

    //    HSECTION hs;

    bool hs;

    bool first = true;

    for (int i = 1; i <= 10; i++) {
        hs = false;

        list<CGridData>::const_iterator it = lst.begin();

        for (; it != lst.end(); it++) {
            CGridData dg = *it;

            int ids = get_ID2(schemeNum, dg.name);
            if (dg.name == "schemeNum") ids = 1;
            if (ids == 8) ids = 1;
            //            if (ids <= 0) ids = 10;
            if (dg.name == "buildingTypeID") ids = 1;


            bool is_persp = false;
            bool is_cont = false;
            bool is___ = false;

            if (dg.name.Find("__") == 0) is___ = true;
            if (dg.name.Find("persp") == 0) is_persp = true;
            if (dg.name.Find("cont") == 0) is_cont = true;
            if (dg.name == "contractNumber") is_cont = false;


            if (ids == i && !is_cont && !is_persp && !is___) {

                //                if (hs == NULL) hs = cEditDlg->addSection(ss[i-1], number != 0, number++);
                if (!hs) {
                    if (!first) {
                        ///   секция закрывается
                        json.add("]");
                        json.add("}");
                    }
                    first = false;

                    json.add("{");
                    json.add("name", ss[i - 1]);

                    ///  секция открывается

                    json.add("\"fields\" : [");
                    hs = true;
                }

                map<CString, CString>::const_iterator itf = join.map_foreign.find(dg.name);

                if (itf != join.map_foreign.end()) {
                    COleVariant value = dg.value;
                    CString name = itf->second;

                    it++;
                    if (it == lst.end()) {

                        break;
                    }
                    CGridData dg1 = *it;
                    dg1.label = dg.label;
                    dg1.name = dg.name;

                    json.add("{");
                    json.add("key", dg.label);
                    json.add("name", dg.name);
                    json.add("value", CCrack::strVARIANT3(dg1.value));

                    long id = to_long(dg.value);

                    json.add("idvalue", id);
                    json.add("typ", strAdoFieldType(dg1.typ));
                    json.add("DefinedSize", dg1.DefinedSize);
                    json.add("DropDown", name);
                    json.add("db", ado->m_schema);
                    json.add("table", dg1.table);
                    json.add("id", dg1.id);
                    json.add("readonly", 0);
                    json.add("google_doc", google_doc);
                    json.add("}");
                }
                else {
                    //      hi = cEditDlg->addDataGrid(hs, dg);

                    json.add("{");
                    json.add("key", dg.label);
                    json.add("name", dg.name);
                    json.add("value", CCrack::strVARIANT3(dg.value));
                    json.add("typ", strAdoFieldType(dg.typ));
                    json.add("DefinedSize", dg.DefinedSize);
                    json.add("DropDown", "");
                    json.add("db", ado->m_schema);
                    json.add("table", dg.table);
                    json.add("id", dg.id);
                    json.add("readonly", 0);
                    json.add("google_doc", google_doc);
                    json.add("}");
                }


            }
        }

    }

    ///   секция закрывается
    json.add("]");
    json.add("}");

    return n;
}





CString http_get_node(const char* file, const char* tn2, int id1)
{
    CAdoFile* ado = getAdo(file);

    if (!ado) return "";

    ////    if (FAILED(::CoInitialize(NULL))) return "";

    CGraph2* graph = gidr_view->m_cxema->m_graph;
    CNode2* n = graph->find(id1);

    if (!n) return "";

    CString tn22 = n->getTableMySQL();
    int id2 = n->node.id2;

    JSON json;

    CString tn1 = "nodes";

    CString param = "externalCodeID, nodeName, externalSignID, internalNodeID, externalNodeName, addressID, geoMarkTopTube, geoMarkNodeArea, calcPressFlow, calcPressRet, PICdate, lastRepairDate, displaySign, archiveChangeDate, operatorID, x, y, scheme, memo, gpsCoords, belongHN, passport, inventNumber, nodeTypeID, isBusy, isLoaded, belongMagistralSite, belongDistSite, organizationID, magistralSite, distSite, fileID, registNumber";

    Join join(tn1, tn2, id1, param);
    join.addJoin("SELECT ID, name FROM externalCodes", "externalCodeID");
    join.addJoin("SELECT ID, name FROM organizations", "organizationID");
    join.addJoin("SELECT ID, name FROM fragments", "fileID");
    join.addJoin("SELECT ID, name FROM operators", "operatorID");
    join.addJoin("SELECT ID, name FROM externalSigns", "externalSignID");


    Join join2(tn22, "", id2, ado);

    json.add("{");

    json.add("id", id1);
    json.add("id2", id2);

    json.add("\"groups\" : [");


    ado_add_table(ado, json, join, _TR("Узел"));

    if (tn22 == "realConsumers") {
        ado_add_table_PR(ado, json, join2, tn22);
    }
    else {
        ado_add_table(ado, json, join2, tn22);
    }




    json.add("]");

    CString tn_out = n->getTableOutMySQL();

    int idgP = n->node.nomgP;
    int idgO = n->node.nomgO;


    if (idgP > 0) {
        json.add("\"OutFieldsP\" : [");
        Join joinOP(tn_out, "", idgP, ado);
        ado_add_table(ado, json, joinOP, _TR("Подающий"));
        json.add("]");
    }

    if (idgO > 0) {
        json.add("\"OutFieldsO\" : [");
        Join joinOO(tn_out, "", idgO, ado);
        ado_add_table(ado, json, joinOO, _TR("Обратный"));
        json.add("]");
    }

    json.add("calcP", idgP > 0);
    json.add("calcO", idgO > 0);

    json.add("}");

    return json.str();
}

CString http_get_line(const char* file, const char* tn2, int id1)
{
    CAdoFile* ado = getAdo(file);

    if (!ado) return "";

    CGraph2* graph = gidr_view->m_cxema->m_graph;
    CLINE2* l = graph->findLine(id1);

    if (!l) return "";

    int idN1 = where(l)->id;
    int idN2 = other(l)->id;
    int id2 = bline(l)->line.idP2;
    CString tn22 = bline(l)->getTableMySQL();


    ////    if (FAILED(::CoInitialize(NULL))) return "";

    //    Join join1("nodes", "", idN1, "externalCodeID, nodeName, externalSignID");
    Join join1("nodes", "", idN1, "externalCodeID, externalNodeName");

    join1.addJoin("SELECT ID, name FROM externalCodes", "externalCodeID");
    //    join1.addJoin("SELECT ID, name FROM externalSigns", "externalSignID");

    Join join2("nodes", "", idN2, "externalCodeID, externalNodeName");
    //Join join2("nodes", idN2, "externalCodeID, nodeName, externalSignID");
    join2.addJoin("SELECT ID, name FROM externalCodes", "externalCodeID");
    //    join2.addJoin("SELECT ID, name FROM externalSigns", "externalSignID");

    //    CString p = "externalSignLineID, hydroRes, organizationID, registNum, firstPICdate, lastMaintDate, podp, archiveChangeDate, operatorID, typ";
    CString p = "externalSignLineID, hydroRes, organizationID, registNum, archiveChangeDate, operatorID";

    Join join3("linesobj", "", id1, p);
    join3.addJoin("SELECT ID, name FROM externalSignLine", "externalSignLineID");
    join3.addJoin("SELECT ID, name FROM organizations", "organizationID");

    Join join4(tn22, "", id2, ado);

    JSON json;

    json.add("{");

    //    json.add("id", id1);
    //    json.add("id2", id2);

    json.add("\"groups\" : [");

    //    ado_add_table(ado, json, join1, _TR("Начальный узел"));
    //    ado_add_table(ado, json, join2, _TR("Конечный узел"));
    ado_add_table(ado, json, join3, _TR("Общая информация"));
    ado_add_table(ado, json, join4, tn22);

    json.add("]");

    CString tn_out = bline(l)->getTableOutMySQL();

    int idgP = bline(l)->line.nomgP;
    int idgO = bline(l)->line.nomgO;

    if (idgP > 0) {
        json.add("\"OutFieldsP\" : [");
        Join joinOP(tn_out, "", idgP, ado);
        ado_add_table(ado, json, joinOP, _TR("Подающий"));
        json.add("]");
    }

    if (idgO > 0) {
        json.add("\"OutFieldsO\" : [");
        Join joinOO(tn_out, "", idgO, ado);
        ado_add_table(ado, json, joinOO, _TR("Обратный"));
        json.add("]");
    }

    json.add("calcP", idgP > 0);
    json.add("calcO", idgO > 0);

    json.add("}");

#if 0

    CString con, q;
    map<CString, COleVariant> mapP;
    map<CString, COleVariant> mapO;
    list<CAdoField> lstFld;


    if (nomP > 0) {
        q.Format("SELECT * FROM `%s` WHERE id=%d", tn, nomP);
        OpenX(ado, q, mapP, lstFld);
    }

    if (nomO > 0 && nomP != nomO) {
        q.Format("SELECT * FROM `%s` WHERE id=%d", tn, nomO);
        OpenX(ado, q, mapO, lstFld);
    }

    InitTab(file, tn, true);

    ////    ::CoUninitialize();


    json.add("{");

    json.add("nomP", nomP);
    json.add("nomO", nomO);

    json.add("\"fields\" : [");

    list<CAdoField>::const_iterator it = lstFld.begin();

    for (; it != lstFld.end(); it++) {
        json.add("{");

        CString sk = get_podp(tn, it->name);

        json.add("key", sk);
        CString s = "";
        CString typ = "";

        if (nomP > 0) {
            map<CString, COleVariant>::const_iterator itP = mapP.find(it->name);
            if (itP != mapP.end()) {
                s = CCrack::strVARIANT3(itP->second);
            }
        }

        json.add("valueP", s);

        if (nomO > 0 && nomO != nomP) {
            map<CString, COleVariant>::const_iterator itO = mapO.find(it->name);
            if (itO != mapO.end()) {
                s = CCrack::strVARIANT3(itO->second);
            }
        }
        json.add("valueO", s);
        json.add("typ", strAdoFieldType(it->typ));
        json.add("DefinedSize", it->DefinedSize);
        json.add("}");
    }

    json.add("]");
    json.add("}");

#endif

    return json.str();
}


CString http_get_line_for_geo(const char* file, const char* tn2, int id1)
{
    CAdoFile* ado = getAdo(file);

    if (!ado) return "";

    CGraph2* graph = gidr_view->m_cxema->m_graph;
    CLINE2* l = graph->findLine(id1);

    if (!l) return "";

    int idN1 = where(l)->id;
    int idN2 = other(l)->id;
    int id2 = bline(l)->line.idP2;
    CString tn22 = bline(l)->getTableMySQL();


    Join join1("nodes", "", idN1, "externalCodeID, externalNodeName");

    join1.addJoin("SELECT ID, name FROM externalCodes", "externalCodeID");

    Join join2("nodes", "", idN2, "externalCodeID, externalNodeName");
    join2.addJoin("SELECT ID, name FROM externalCodes", "externalCodeID");

    CString p = "externalSignLineID, hydroRes, organizationID, registNum, archiveChangeDate, operatorID";

    Join join3("linesobj", "", id1, p);
    join3.addJoin("SELECT ID, name FROM externalSignLine", "externalSignLineID");
    join3.addJoin("SELECT ID, name FROM organizations", "organizationID");

    Join join4(tn22, "", id2, ado);

    JSON json;

    json.add("{");

    json.add("\"groups\" : [");

    ado_add_table(ado, json, join3, _TR("Общая информация"));
    ado_add_table(ado, json, join4, tn22);

    json.add("]");


    json.add("}");

    return json.str();
}



CString http_get_street(const char* pat1)
{
    CString pat = pat1;
    pat.TrimRight();
    pat.MakeUpper();

    list<CString> lst;

    {
        set<CString>::const_iterator it = set_street.begin();
        for (; it != set_street.end(); it++) {
            CString s = *it;
            s.MakeUpper();

            if (s.Find(pat) != -1) {
                lst.push_back(*it);
            }
        }
    }

    CString street = "";

    if (lst.size() == 1) {
        list<CString>::const_iterator it = lst.begin();
        CString s = *it;
        if (s == pat1) {
            street = s;
        }
    }

    JSON json;

    json.add("{");
    json.add("street", street);
    json.add("\"choices\" : [");
    list<CString>::const_iterator it = lst.begin();

    for (; it != lst.end(); it++) {
        CString s = *it;
        json.add1(s);
    }

    json.add("]");
    json.add("}");

    return json.str();
}

void decode_dom(const char* dom, int& n1, int& n2)
{
    n1 = n2 = 0;
    int i = 0;

    for (; dom[i] && !('0' <= dom[i] && dom[i] <= '9'); i++) {}
    if (dom[i]) {
        n1 = atoi(&dom[i]);
    }

    for (; dom[i] && ('0' <= dom[i] && dom[i] <= '9'); i++) {}
    for (; dom[i] && !('0' <= dom[i] && dom[i] <= '9'); i++) {}
    if (dom[i]) {
        n2 = atoi(&dom[i]);
    }
}

struct less_dom
{
    using result_type = bool;
    using first_argument_type = const CString&;
    using second_argument_type = const CString&;
    result_type operator() (const CString& arg1, const CString& arg2) const
    {
        int n11, n12, n21, n22;

        decode_dom(arg1, n11, n12);
        decode_dom(arg2, n21, n22);

        if (n11 < n21) return true;
        if (n11 > n21) return false;
        if (n12 < n22) return true;
        if (n12 > n22) return false;

        return arg1 < arg2;
    }
};

CString http_get_dom(const char* pat1)
{
    ////    if (FAILED(::CoInitialize(NULL))) return "";

    CString pat = pat1;
    pat.TrimRight();
    //  pat.MakeUpper();

    JSON json;

    Klassif* kls = m_geofile->m_kl_list->findKlN(transl(m_tab));
    if (kls) {
        CString ul = pat;
        list<COleVariant> lst;

        CString q;
        q.Format("SELECT DISTINCT %s FROM %s WHERE %s = '%s' ORDER BY %s", transl(m_dom), transl(m_tab), transl(m_street), ul, transl(m_dom));
        OpenList(kls->m_ado, q, lst);

        json.add("{");
        json.add("street", ul);
        json.add("\"choices\" : [");

        set<CString, less_dom> set_dom;

        {
            list<COleVariant>::const_iterator it = lst.begin();
            for (; it != lst.end(); it++) {
                CString s = CCrack::strVARIANT3(*it);
                set_dom.insert(s);
            }
        }

        set<CString, less_dom>::const_iterator it = set_dom.begin();

        for (; it != set_dom.end(); it++) {
            CString s = *it;
            json.add1(s);
        }

        json.add("]");
        json.add("}");
    }

    ////    ::CoUninitialize();

    return json.str();
}

void ShapeToCl(COleVariant& var, CCoordList& cl);

COleVariant get_V(map<CString, COleVariant>& map_v, CString id)
{
    COleVariant v;

    map<CString, COleVariant>::const_iterator it = map_v.find(id);
    if (it != map_v.end()) return it->second;
    return v;
}


CString http_get_addr2(const char* pat1)
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(transl(m_tab));
    if (!kls) {
        return "";
    }

    ////    if (FAILED(::CoInitialize(NULL))) return "";

    CString pat = pat1;
    pat.TrimRight();
    CString con, q;

    q.Format("SELECT ID FROM %s WHERE CONCAT(%s, ' ', %s) = '%s'", transl(m_tab), transl(m_street), transl(m_dom), pat);
    map<CString, COleVariant> lst;
    list<CAdoField> lstFld;

    //  AfxMessageBox("1", MB_OK|MB_ICONINFORMATION);

    OpenX(kls->m_ado, q, lst, lstFld);

    //  AfxMessageBox("2", MB_OK|MB_ICONINFORMATION);

////    ::CoUninitialize();

    COleVariant v = get_V(lst, "ID");

    JSON json;

    long id = (long)V_I4(&v);

    CGeoObject* geo = kls->getGeoObjectById(id);
    CFRect rr;

    if (geo) {
        CFRect r = geo->cl.GetFRect();
        /*

                CFRect rect = gidr_view->getMaxRect();
        //        double dmas = gidr_view->dmas;
                double dmas = DMAS96;

                rr.left = (r.left)*dmas / 1000000;
                rr.right = (r.right)*dmas / 1000000;
                rr.top = (r.top)*dmas / 1000000;
                rr.bottom = (r.bottom)*dmas / 1000000;

                json.add("{");
                json.add("x1", rr.left);
                json.add("x2", rr.right);
                json.add("y1", rr.top);
                json.add("y2", rr.bottom);
                json.add("}");
        */
        get_google_rect_json(r, 0, 0, json);
    }
    return json.str();
}


CString http_get_gps(double x, double y)
{
    CFPoint pt(x, y);

    pt = m_geofile->toGeo(pt);

    CFRect r(pt.x - 10000, pt.y - 10000, pt.x + 10000, pt.y + 10000);
    CFRect rr;

    CFRect rect = gidr_view->getMaxRect();
    //    double dmas = gidr_view->dmas;
    double dmas = DMAS96;

    rr.left = (r.left) * dmas / 1000000;
    rr.right = (r.right) * dmas / 1000000;
    rr.top = (r.top) * dmas / 1000000;
    rr.bottom = (r.bottom) * dmas / 1000000;

    JSON json;

    json.add("{");
    json.add("x1", rr.left);
    json.add("x2", rr.right);
    json.add("y1", rr.top);
    json.add("y2", rr.bottom);
    json.add("}");

    return json.str();
}

/*
CString http_get_list(const char *obj)
{

Klassif *kls = m_geofile->m_kl_list->findKlN(transl("Насосные_станции"));
if (!kls) {
return "";
}

////if ( FAILED(::CoInitialize(NULL)) ) return "";

CString con, q;
con.Format("Provider=Microsoft.ACE.OLEDB.12.0;Data Source=%s;Persist Security Info=False;", kls->m_db->m_strFileName);

list< vector<COleVariant> > lst;

q.Format("SELECT ID, СХЕМА FROM [Насосные_станции]");

OpenList2(kls->m_db->m_strFileName, q, lst);

JSON json;

json.add("{");
json.add("\"list\" : [");

list< vector<COleVariant> >::const_iterator it = lst.begin();

for ( ; it != lst.end(); it++) {

vector<COleVariant> v = *it;

int ID = v[0].lVal;
CString name2 = CCrack::strVARIANT3( v[1] );

//    CString name;
//            name.Format(_TR("Насосная станция %d"), ID);
//    name.Format(_TR("НС %d"), ID);

json.add("{");
json.add("ID", ID );
json.add("name", name2 );
json.add("}");
}

json.add("]");
json.add("}");

return json.str();
}
*/


CString http_get_pro(const char* mdb, const char* table, int ID)
{
    JSON json;
    CGraph2* graph = gidr_view->m_cxema->m_graph;

    ////    if (FAILED(::CoInitialize(NULL))) return "";

    CString con, q;
    con.Format("Provider=Microsoft.ACE.OLEDB.12.0;Data Source=%s;Persist Security Info=False;", mdb);

    list< vector<COleVariant> > lst;

    if (ID <= 0) {

        q.Format("SELECT nomer, uzel FROM [%s]", table);

        OpenList2(getAdo(mdb), q, lst);

        //    json.add("{");
        //    json.add("\"list\" : [");
        json.add("[");

        list< vector<COleVariant> >::const_iterator it = lst.begin();

        for (; it != lst.end(); it++) {

            vector<COleVariant> v = *it;

            int ID = v[0].lVal;
            CString name2 = CCrack::strVARIANT3(v[1]);

            json.add("{");
            json.add("ID", ID);
            json.add("name", name2);
            json.add("}");
        }

        json.add("]");
        //    json.add("}");
    }
    else {

        map<CString, COleVariant> lst;
        list<CAdoField> lstFld;

        q.Format("SELECT * FROM [%s] WHERE nomer=%d", table, ID);


        OpenX(getAdo(mdb), q, lst, lstFld);

        ///    json.add("{");
        //    json.add("\"list\" : [");

        json.add("[");

        list<CAdoField>::const_iterator it = lstFld.begin();

        for (; it != lstFld.end(); it++) {
            json.add("{");
            json.add("key", it->name);

            CString s = "";

            map<CString, COleVariant>::const_iterator itP = lst.find(it->name);
            if (itP != lst.end()) {
                s = CCrack::strVARIANT3(itP->second);
            }

            json.add("value", s);
            json.add("}");
        }

        json.add("]");
        //    json.add("}");
    }

    ////    ::CoUninitialize();

    return json.str();
}


CString http_get_pro_file()
{
    if (!gidr_view) return "";

    if (!gidr_view->m_cxema->m_cur_file) return "";

    JSON json;

    json.add("{");
    json.add("db", gidr_view->m_cxema->m_cur_file->m_schema);
    json.add("}");

    return json.str();
}

CString http_get_pro_table(const char* mdb, const char* table)
{
    JSON json;
    CGraph2* graph = gidr_view->m_cxema->m_graph;

    ////    if (FAILED(::CoInitialize(NULL))) return "";

    CString con, q;
    con.Format("Provider=Microsoft.ACE.OLEDB.12.0;Data Source=%s;Persist Security Info=False;", mdb);

    list< vector<COleVariant> > lst;


    q.Format("SELECT nomer, uzel FROM [%s]", table);

    OpenList2(getAdo(mdb), q, lst);

    //  json.add("{");
    //  json.add("\"list\" : [");

    json.add("[");

    list< vector<COleVariant> >::const_iterator it = lst.begin();

    for (; it != lst.end(); it++) {

        vector<COleVariant> v = *it;

        int ID = v[0].lVal;
        CString name2 = CCrack::strVARIANT3(v[1]);

        json.add("{");
        json.add("ID", ID);
        json.add("name", name2);
        json.add("}");
    }

    json.add("]");
    //  json.add("}");

////    ::CoUninitialize();

    return json.str();
}

CString http_get_pro_table_ID(const char* mdb, const char* table, int ID)
{
    JSON json;
    CGraph2* graph = gidr_view->m_cxema->m_graph;

    ////    if (FAILED(::CoInitialize(NULL))) return "";

    CString con, q;
    con.Format("Provider=Microsoft.ACE.OLEDB.12.0;Data Source=%s;Persist Security Info=False;", mdb);

    map<CString, COleVariant> lst;
    list<CAdoField> lstFld;

    q.Format("SELECT * FROM [%s] WHERE nomer=%d", table, ID);

    OpenX(getAdo(mdb), q, lst, lstFld);

    //  json.add("{");
    //  json.add("\"list\" : [");
    json.add("[");


    list<CAdoField>::const_iterator it = lstFld.begin();

    for (; it != lstFld.end(); it++) {
        json.add("{");
        json.add("key", it->name);

        CString s = "";

        map<CString, COleVariant>::const_iterator itP = lst.find(it->name);
        if (itP != lst.end()) {
            s = CCrack::strVARIANT3(itP->second);
        }

        json.add("value", s);
        json.add("}");
    }

    json.add("]");
    //  json.add("}");

////    ::CoUninitialize();

    return json.str();
}


CString http_get_pro_table_objects_ID(const char* mdb, const char* table, int ID)
{
    JSON json;

    json.add("[");
    json.add1(_TR("Один"));
    json.add1(_TR("Два"));
    json.add("]");

    return json.str();
}

CString http_get_pro_query(const char* mdb, const char* query)
{
    CString con;
    con.Format("Provider=Microsoft.ACE.OLEDB.12.0;Data Source=%s;Persist Security Info=False;", mdb);

    ////    if (FAILED(::CoInitialize(NULL))) return "";
    CString js = OpenXJSon(getAdo(mdb), query);
    ////    ::CoUninitialize();

    return js;
}


CString http_get_pro_query1(const char* mdb, const char* query)
{
    CString con;
    con.Format("Provider=Microsoft.ACE.OLEDB.12.0;Data Source=%s;Persist Security Info=False;", mdb);

    ////    if (FAILED(::CoInitialize(NULL))) return "";
    CString js = OpenXJSon1(getAdo(mdb), query);
    ////    ::CoUninitialize();

    return js;
}

CString http_get_pro_query2(const char* mdb, const char* query)
{
    CString con;
    con.Format("Provider=Microsoft.ACE.OLEDB.12.0;Data Source=%s;Persist Security Info=False;", mdb);

    ////    if (FAILED(::CoInitialize(NULL))) return "";
    CString js = OpenXJSon2(getAdo(mdb), query);
    ////    ::CoUninitialize();

    return js;
}

CString http_get_pro_graph(const char* mdb, const char* tn)
{
    CString con;
    con.Format("Provider=Microsoft.ACE.OLEDB.12.0;Data Source=%s;Persist Security Info=False;", mdb);

    ////    if (FAILED(::CoInitialize(NULL))) return "";

    CString js = OpenXJSon2(getAdo(mdb), tn);
    ////    ::CoUninitialize();

    return js;
}


CString http_get_file(const char* mdb1, const char* table)
{
    CString mdb = mdb1;

    CGraph2* graph = gidr_view->m_cxema->m_graph;

    if (!mdb1 || !mdb1[0]) {
        if (!gidr_view->m_cxema->m_cur_file) return "";
        return gidr_view->m_cxema->m_cur_file->m_schema;
    }

    if (!strcmp(mdb1, "geo")) {
        Klassif* kls = m_geofile->m_kl_list->findKlN(table);
        if (kls) {
            return kls->m_ado->m_schema;
        }
    }

    mdb.Replace("$path$", argpath());

    return mdb;
}

CString OpenXJSonCoord1(CAdoFile* ado, const char* tn, CFRect rect, double dmas);

CString http_get_remonts(const char* tn)
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);

    if (kls) {
        CString con;

        ////        if (FAILED(::CoInitialize(NULL))) return "";

        CFRect rect = gidr_view->getMaxRect();
        //        double dmas = gidr_view->dmas;
        double dmas = DMAS96;

        CString js = OpenXJSonCoord1(kls->m_ado, tn, rect, dmas);
        ////        ::CoUninitialize();

        return js;
    }
    return "";
}

CString getProfileString(const char* section, const char* reg);

void writeTU(JSON& json, int nn, const list<CAdoField>& lstFld2, const map< int, map<CString, COleVariant> >& mapP2)
{
    for (int n = 0; n < nn; n++) {
        list<CAdoField>::const_iterator it = lstFld2.begin();

        for (; it != lstFld2.end(); it++) {
            CString s = getVstr(mapP2, n, it->name);
            CString name = it->name;

            if (s != "" && name != _TR("Здание")) {
                json.add("{");
                json.add("key", it->name);

                if (name == _TR("Договор") || name == _TR("Акт") || name.Find(_TR("Технические условия")) != -1) {

                    CString path = getProfileString(szSection, "PATH_AKT_TU");

                    json.add("value", path + CString("\\") + s);
                    json.add("typ", "Link");
                }
                else {
                    json.add("value", s);
                    json.add("typ", strAdoFieldType(it->typ));
                }
                json.add("DefinedSize", it->DefinedSize);
                json.add("}");
            }
        }
    }
}

CString http_get_TU(const char* tn, int id)
{

#if 0
    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);

    if (!kls) return "";

    ////    if (FAILED(::CoInitialize(NULL))) return "";

    CString con, q;
    con.Format("Provider=Microsoft.ACE.OLEDB.12.0;Data Source=%s;Persist Security Info=False;", kls->m_db->m_strFileName);

    q.Format("SELECT * FROM [%s] WHERE ID=%d", tn, id);

    list<CAdoField> lstFld1;

    map<CString, COleVariant> mapP;

    list<CAdoField> lstFld2A;
    list<CAdoField> lstFld2D;
    list<CAdoField> lstFld2Z;

    map< int, map<CString, COleVariant> > mapP2A;
    map< int, map<CString, COleVariant> > mapP2D;
    map< int, map<CString, COleVariant> > mapP2Z;

    OpenX(kls->m_ado, q, mapP, lstFld1);

    q.Format("SELECT * FROM [Технические условия] WHERE [Здание]=%d AND [Состояние договора]='А'", id);
    int nnA = OpenX2(kls->m_ado, q, mapP2A, lstFld2A);

    q.Format("SELECT * FROM [Технические условия] WHERE [Здание]=%d AND [Состояние договора]='Д'", id);
    int nnD = OpenX2(kls->m_ado, q, mapP2D, lstFld2D);

    q.Format("SELECT * FROM [Технические условия] WHERE [Здание]=%d AND [Состояние договора]='З'", id);
    int nnZ = OpenX2(kls->m_ado, q, mapP2A, lstFld2Z);

    ////    ::CoUninitialize();

    JSON json;

    json.add("{");
    json.add("\"fields\" : [");

    list<CAdoField>::const_iterator it = lstFld1.begin();

    for (; it != lstFld1.end(); it++) {
        CString s = getVstr(mapP, it->name);

        if (s != "") {
            json.add("{");
            json.add("key", it->name);

            json.add("value", s);
            json.add("typ", strAdoFieldType(it->typ));
            json.add("DefinedSize", it->DefinedSize);
            json.add("}");
        }
    }

    writeTU(json, nnD, lstFld2D, mapP2D);
    writeTU(json, nnA, lstFld2A, mapP2A);
    writeTU(json, nnZ, lstFld2Z, mapP2Z);

    json.add("]");
    json.add("}");

    return json.str();
#endif

    return "";
}

bool delete_tile(const char* tiles, double dx, double dy, double dmas)
{
#if 0
    int m0 = 1;
    double mas = 64000;
    //    double mas = 200000;


    for (int m = 1; m <= 18; m++) {
        if (m >= m0) {
            int x1 = dx * dmas / DD / mas;
            int y1 = dy * dmas / DD / mas;

            //           CString s = tileName(m, y1, x1, 0, geo, 0, 0);
            //           s.Format("%s\\%d\\%d\\%d.png", tiles, m, y1, x1);
            //           log_printf("access.log", "DeleteFile %s\n", s);

            CString s;
            s = tileName(m, y1, x1, 0, 1, 1, 1, 0, 0);

            DeleteFile(s);
        }
        mas /= mas_coef;
    }
#endif
    return true;
}

void getProject(CLINE2* line, CFPoint point, CCoordList& m_NP, int loc);


CString http_add_geo_point(const char* tiles, const char* tn, double mas0, double x, double y)
{
    JSON json;

    if (!gidr_view) return "";
    if (mas0 > 3000) return "";
    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (!kls) return "";

    CFRect rect = gidr_view->getMaxRect();

    //    double dmas = gidr_view->dmas;
    double dmas = DMAS96;
    double dx = x * 1000000 / dmas;
    double dy = y * 1000000 / dmas;

    double x0 = dx;
    double y0 = dy;

    CFPoint pt = CFPoint(x0, y0);

    CCoordList cl;
    cl.push_back(pt);

    CNode2* node = NULL;
    CLINE2* line = NULL;

    gidr_view->findNodeLine(pt, &node, &line, D5);
    if ((kls->loc == LOC6 || kls->loc == LOC11) && line) { // По участку
        getProject(line, pt, cl, 6);
    }
    //    if (kls->loc == LOC6) { // По участку
    //    }

    map<CString, COleVariant> map_geo_value;

    int nn = m_geofile->createObj(kls->nazv, cl, map_geo_value);

    if (nn > 0) {
        delete_tile(tiles, dx, dy, dmas);

        json.add("{");
        json.add("}");
    }

    return json.str();
}


CString http_delete_object(const char* tiles, const char* file, const char* tn, int id)
{
    JSON json;

    if (!gidr_view) return "";
    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (!kls) return "";

    CFRect rect = gidr_view->getMaxRect();
    CGeoObject* geo = kls->getGeoObjectById(id);

    if (!geo) return "";

    double dx = geo->rect.left;
    double dy = geo->rect.top;

    //    double dmas = gidr_view->dmas;
    double dmas = DMAS96;
    //    double dx = x * 1000000 / dmas;
    //    double dy = y * 1000000 / dmas;

    bool ret = m_geofile->delGeoObj(tn, id);

    if (ret) {
        delete_tile(tiles, dx, dy, dmas);

        json.add("{");
        json.add("}");
    }

    return json.str();
}

string dateFile2(const char* fn);
string curDate2();

CString http_get_tiles_names(int dpi, int m, int x1, int y1, int x2, int y2, int internalNodeID, int layer, int gid, int geo)
{
    JSON json;

    json.add("[");

    for (int y = y1; y <= y2; y++) {
        json.add("[");
        for (int x = x1; x <= x2; x++) {
            //            CString fn;
            //           fn.Format("%s\\%d\\%d\\%d.png", tiles, m, y, x);

            CString fn = tileName(dpi, m, y, x, internalNodeID, layer, gid, geo, 0, 0);

            string date = dateFile2(fn);

            if (date == "") {
                date = curDate2();
            }

            json.add1(date.c_str());
        }
        json.add("]");
    }
    json.add("]");
    return json.str();
}



#include "jsmn.h"

string getJsonName(const char* pBuf, jsmntok_t* t, int i);

CString http_get_geo_list()
{
    JSON json;

    //  json.add("{");
    //  json.add("\"list\" : [");
    json.add("[");

    KlList::const_iterator it = m_geofile->m_kl_list->begin();

    for (; it != m_geofile->m_kl_list->end(); it++) {
        const Klassif* kl = &it->second;
        //    db_list[sl] = kl->m_db ? kl->m_db->db : NULL;
        //    if (db_list[sl] != db_list[0]) m_multy = 1;
        json.add1(kl->nazv);
    }

    json.add("]");
    // json.add("}");

    return json.str();
}


CString fullName(int type_of_net, CString schema, CString table);


//CString http_get_pro_update_ID(const char *mdb, const char *table2, const char *pBuf, int ID)
CString http_update(const char* pBuf)
{
    //    char szDT[200];

    long l = strlen(pBuf);
    int r;
    int numtok = 200;
    jsmntok_t* t = NULL;

    jsmn_parser p;

    do {
        t = (jsmntok_t*)malloc(numtok * sizeof(jsmntok_t));
        jsmn_init(&p);
        r = jsmn_parse(&p, pBuf, l, t, numtok);
        if (r == -1) {
            free(t);
            numtok *= 2;
            t = (jsmntok_t*)malloc(numtok * sizeof(jsmntok_t));
        }
    } while (r < 0 && numtok < 10000);

    int id_data = -1;
    int id_data_row = -1;

    bool first = true;
    string s_table, s_file, s_name, s_value, s_idValue, s_id;

    CString ss = "";

    map<CString, map<CString, map <CString, map <CString, CString> > > > mp;

    for (int i = 0; i < r; i++) {
        string s, sp, spp;

        int ip = t[i].parent;
        int ipp = t[i].parent >= 0 ? t[t[i].parent].parent : -1;

        spp = getJsonName(pBuf, t, ipp);
        sp = getJsonName(pBuf, t, ip);
        s = getJsonName(pBuf, t, i);

        if (t[i].type == JSMN_ARRAY && sp == "data") {
            id_data = i;
        }


        if (ip >= 0 && t[i].type == JSMN_OBJECT && ip == id_data) {
            if (!first) {
                CString s;
                s.Format("%s %s %s %s %s %s", s_table.c_str(), s_file.c_str(), s_name.c_str(), s_value.c_str(), s_idValue.c_str(), s_id.c_str());

                if (s_idValue != "") s_value = s_idValue;
                mp[s_file.c_str()][s_table.c_str()][s_id.c_str()][s_name.c_str()] = s_value.c_str();

                if (ss != "") ss += "\n";
                ss += s;
            }
            id_data_row = i;
            s_table = s_file = s_name = s_value = s_idValue = s_id = "";
            first = false;
        }

        if (t[i].type == JSMN_STRING || t[i].type == JSMN_PRIMITIVE) {
            if (ipp >= 0 && ipp == id_data_row) {
                if (sp == "table") s_table = s;
                else if (sp == "db") s_file = s;
                else if (sp == "name") s_name = s;
                else if (sp == "value") s_value = s;
                else if (sp == "idValue") s_idValue = s;
                else if (sp == "id") s_id = s;
            }
        }
    }

    if (!first) {
        if (s_idValue != "") s_value = s_idValue;
        mp[s_file.c_str()][s_table.c_str()][s_id.c_str()][s_name.c_str()] = s_value.c_str();
    }

    free(t);
    //    AfxMessageBox(ss, MB_OK|MB_ICONINFORMATION);

    map<CString, map<CString, map <CString, map <CString, CString> > > >::const_iterator it1 = mp.begin();

    CString file, table, id, name, val;

    CString q;
    CString qq = "";

    bool ret = true;


    ////    if (FAILED(::CoInitialize(NULL))) return "";

    for (; it1 != mp.end(); it1++) {
        file = it1->first;
        CAdoFile* ado = getAdo(file);
        if (ado) {
            map<CString, map <CString, map <CString, CString> > >::const_iterator it2 = it1->second.begin();

            for (; it2 != it1->second.end(); it2++) {
                CString s2 = "";
                table = it2->first;
                map <CString, map <CString, CString> >::const_iterator it3 = it2->second.begin();
                for (; it3 != it2->second.end(); it3++) {
                    id = it3->first;

                    map <CString, CString>::const_iterator it4 = it3->second.begin();
                    for (; it4 != it3->second.end(); it4++) {
                        name = it4->first;
                        val = it4->second;

                        MySQLColumns* c = ado->findMySQLColumnsS(file, table, name);

                        if (c) {
                            CString s;
                            //                            if (c->strDATA_TYPE == "str") {
                            if (c->strDATA_TYPE == "varchar" || c->strDATA_TYPE == "date" || c->strDATA_TYPE == "datetime") {
                                val.Format("\"%s\"", val);
                            }

                            s.Format("%s=%s", name, val);

                            if (s2 != "") s2 += ",";
                            s2 += s;
                        }
                    }
                    q.Format("UPDATE %s SET %s WHERE ID=%s", fullName(ado->m_type_of_net, file, table), s2, id);

                    long affected;
                    ado->Execute(q, &affected);
                }
            }
        }
    }

    ////    ::CoUninitialize();
    JSON json;
    json.error(ret, _TR("Ошибка"));
    return json.str();
}

void getProject(CLINE2* line, CFPoint point, CCoordList& m_NP, int loc);


CString http_move_object(const char* tiles, const char* tn, int id, double mas0, double x, double y)
{
    JSON json;

    if (!gidr_view) return "";
    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (!kls) {
        json.error(false, _TR("Нет таблицы"));
        return json.str();
    }

    CFRect rect = gidr_view->getMaxRect();
    CGeoObject* geo = kls->getGeoObjectById(id);

    if (!geo) {
        json.error(false, _TR("Ошибка"));
        return json.str();
    }

    double dx = geo->rect.left;
    double dy = geo->rect.top;

    //    double dmas = gidr_view->dmas;
    double dmas = DMAS96;

    double dx2 = x * 1000000 / dmas;
    double dy2 = y * 1000000 / dmas;

    double x2 = dx2;
    double y2 = dy2;

    CFPoint pt = CFPoint(x2, y2);


    CNode2* node;
    CLINE2* line;

    gidr_view->findNodeLine(pt, &node, &line, D5);

    CCoordList cl;
    if (line) { // По участку
        getProject(line, pt, cl, 6);
    }
    else {
        json.error(false, _TR("Нужно установить на трубу"));
        return json.str();
    }


    if (cl.size() == 0) {
        cl.push_back(pt);

        pt = CFPoint(pt.x + 100, pt.y);
        cl.push_back(pt);
    }


    bool ret = m_geofile->updateObj(tn, id, cl);

    if (ret) {
        delete_tile(tiles, dx, dy, dmas);
        delete_tile(tiles, dx2, dy2, dmas);
    }

    json.error(ret, _TR("Не могу перенести"));
    return json.str();
}


CString http_tree_gid()
{
    JSON json;

    return json.str();
}

void add_kls(JSON& json, const Klassif* kls, int id)
{
    json.add("{");
    json.add("id", id);
    json.add("text", kls->RusName());
    json.add("value", kls->nazv);
    json.add("db", kls->m_ado->m_schema);
    json.add("icon", "fas fa-file icon-state-default");
    json.add("opened", false);
    json.add("selected", false);
    json.add("disabled", false);
    json.add("loading", false);
    json.add("}");
}

void add_name(JSON& json, const CString& name, int id)
{
    json.add("id", id++);
    json.add("text", name);
    json.add("value", name);

    json.add("icon", "fa fa-folder icon-state-main");
    json.add("opened", false);
    json.add("selected", false);
    json.add("disabled", false);
    json.add("loading", false);
}

/**
void add_tree_geo(const CString &database, )
{
}
*/


#include "Win.h"

CString http_tree_geo()
{
    JSON json;

    if (!gidr_view) return "";

    json.add("{\"data\": [");

    set<CString> fn;
    set<CString> set_table;

    KlList::iterator itk;

    for (itk = m_geofile->m_kl_list->begin(); itk != m_geofile->m_kl_list->end(); ++itk) {
        const Klassif* kl = &itk->second;
        CString str;
        str = kl->m_database;
        fn.insert(str);
    }
    int id = 0;

    set<CString>::const_iterator it;
    for (it = fn.begin(); it != fn.end(); ++it) {
        CString database = *it;
        CString database0 = database;
        if (database0 == getGidAdoName()) {
            database0 = "Гидравлика";
        }

        json.add("{");
        add_name(json, database0, id++);

        json.add("\"children\": [");

        {
//            set<CString>::const_iterator it;
//            for (it = fn.begin(); it != fn.end(); ++it) {
//                CString database = *it;

                CString lstn;
                lstn.Format("%skls\\%s.lst", argpath(), database);
                //  lstn = GetFName(fn)+".lst";
                if (database == getGidAdoName()) {
                    lstn.Format("%skls\\GID.lst", argpath());
                }

                FILE* f = fopen(lstn, "r");

                if (f) {
                    bool children2_first = false;
                    bool children2 = false;


                    int i3 = 0;

                    static int id = 1;
                    static int num = 0;

                    CString text = "Таблицы";

                    while (!feof(f)) {
                        char s[2560];

                        fgetstr(s, 255, f);
                        if (s[0] == '=') {
                            break;
                        }

                        if (s[0] && s[0] != '-') {
                            CString fn = "";
                            fn = &s[1];

                            if (s[0] != ' ') {
                                text = s;
                                if (children2) {
                                    json.add("]}");
                                }
                                children2 = false;
                                children2_first = true;
                            }
                            else {
                                Klassif* kls = m_geofile->m_kl_list->findKlN(transl(fn));
                                if (kls && kls->ins && kls->m_database == database) {
                                    if (children2_first) {
                                        json.add("{");
                                        add_name(json, text, id++);
                                        json.add("\"children\": [");
                                    }
                                    children2_first = false;
                                    children2 = true;

                                    add_kls(json, kls, id++);
                                    set_table.insert(kls->nazv);
                                }
                            }
                        }
                    }

                    if (children2) {
                        json.add("]}");
                    }

                    fclose(f);
                }

                for (auto& it : *m_geofile->m_kl_list) {
                    Klassif* kls = &it.second;
                    if (set_table.find(kls->nazv) == set_table.end()) {
                        if (kls->ins && kls->m_ado->m_schema == database) {
                            add_kls(json, kls, id++);
                        }
                    }
                }

//            }
        }

        json.add("]");
        json.add("}");
    }
    json.add("]}");

    return json.str();
}

bool check_login(const char* user, const char* password, int& right);
void rightDecode(int right, int* r);


CString http_validate_user(const char* user, const char* password)
{
    JSON json;

    int right;

    if (check_login(user, password, right)) {
        int rr[256];

        rightDecode(right, rr);

        json.add("{");
        json.add("status", true);
        json.add("user", user);
        json.add("right", right);
        json.add("admin", rr[R_admin]);
        json.add("regim", rr[R_regim]);
        json.add("akt", rr[R_akt]);
        json.add("geo", rr[R_geo]);
        json.add("neud", rr[R_neud]);
        json.add("proiz", rr[R_proiz]);
        json.add("indikator", rr[R_indikator]);
        json.add("web_read", rr[R_web_read]);
        json.add("web_write", rr[R_web_write]);
        json.add("remont", rr[R_remont]);
        json.add("}");
    }
    else {
        json.add("{");
        json.add("status", false);
        json.add("}");
    }

    return json.str();
}


CString findTableRusName(CString d, CString n);


CString http_get_q(const char* file, const char* q, int r1, int r2, bool rus_name)
{
    CAdoFile* ado = getAdo(file);
    if (!ado) return "";

    JSON json;

    CString q0 = q;

    CString getTableNameFromSQL(CString q);
    CString tn = getTableNameFromSQL(q0);

    CString tnr = findTableRusNameGid(tn);

    if (tn == "nodes") tnr = _TR("Узел");

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (kls) {
        tnr = kls->RusName();
    }

    MySQLColumns* col = ado->findMySQLColumns(tn, "id");
    MySQLColumns2* col2 = ado->findMySQLColumns2(tnr, "id");

    if (ado->openTable0(q0)) {

        json.add("{");
        json.add("\"data\" : [");


        map<int, CString> map_title;

        int nf = ado->NFlds();

        for (long i = 0; i < nf; i++) {
            CAdoField f = ado->Field(i);
            CString name = f.name;
            CString title = name;

            MySQLColumns* col = ado->findMySQLColumns(tn, name);
            MySQLColumns2* col2 = ado->findMySQLColumns2(tnr, name);


            MySQLColumns2* colOld = ado->findMySQLOldColumns(tnr, name);

            map_title[i] = "";

            if (!col || col->strDATA_TYPE != "geometry" || f.typ != adLongVarBinary) {
                if (rus_name) {

                    if (colOld) {
                        if (colOld->nm != "")  title = colOld->nm;
                        if (colOld->stand_razmer != "") {
                            title += ", ";
                            title += colOld->stand_razmer;
                        }
                    }

                    if (col) {
                        if (col->strCOLUMN_COMMENT != "") {
                            title = col->strCOLUMN_COMMENT;
                        }
                    }

                    if (col2) {
                        title = col2->hlp;
                    }
                }
                title.Replace(".", " ");

                map_title[i] = title;
            }
        }


        int kk = 0;

        while (!ado->isEOF()) {

            kk++;

            json.add("{");

            CString strID = ado->readStr(0);

            for (long i = 0; i < nf; i++) {
                CString str = ado->readStr(i);

                auto it = map_title.find(i);
                CString title = it->second;

                if (title != "") {
                    json.add(title, (const char*)str);
                }
            }

            json.add("}");

            ado->MoveNext();
        }
        ado->closeTable();

        json.add("]");
        json.add("}");
        return json.str();
    }

    json.add("{");
    json.add("\"data\" : [");
    json.add("]");
    json.add("}");
    return json.str();
}


CString http_get_file_q(const char* db, const char* file, int r1, int r2)
{
    CAdoFile* ado = getAdo(db);
    if (!ado) return "";

    CString q = readFile(argpath() + file);

    return http_get_q(db, q, r1, r2, true);
}

CString getParams(CAdoFile* ado, const char* tn);


CString http_get_table(const char* file, const char* table, int ms, int rs, int r1, int r2)
{
    CString q;
//    int ms = 116;
//    int rs = 0;

    q.Format("SELECT * FROM %s", table);

    CAdoFile* ado = getAdo(file);
    if (ado) {
        CString par = getParams(ado, table);
        q.Format("SELECT %s FROM %s", par, table);

        Klassif *kls = m_geofile->m_kl_list->findKlN(table);
        if (kls && gidr_view) {

            CString getGeoSelect(Klassif *kls, CGraph2 *graph);
            
            CGraph2* graph = gidr_view->m_cxema->m_graph;

            graph->reset();
            if (ms) {
                viewUchastok(graph, make_tuple(ms, 0), 1, true);
            }
            else if (rs) {
                viewUchastok(graph, make_tuple(0, rs), 2, true);
            }

            q = getGeoSelect(kls, graph);
//           q = gidr_view->qGeoTable(kls);
        }
    }

    return http_get_q(file, q, r1, r2, true);
}

CString readQ(CAdoFile* ado, const char* tn, const char* tn2, int fileID);
CString readQ_select(CAdoFile* ado, CGraph2* graph, const char* tn, const char* tn2);
int getOutID(CAdoFile* ado, int fileID);

CString http_get_table_gid(const char* file, const char* code, const char* table, int r1, int r2)
{
    CString fn;

    if (!gidr_view || !gidr_view->m_cxema || !gidr_view->m_cxema->m_cur_file) return "";


    long fileID = gidr_view->m_cxema->m_cur_file->id;

    CString q = "";

    CAdoFile* ado = getAdo(file);
    if (ado) {

        if (gidr_view->m_cxema->m_graph->isMark()) {
            q = readQ_select(ado, gidr_view->m_cxema->m_graph, code, table);
        }
        else {
            q = readQ(NULL, code, table, fileID);
        }

        CString s;
        int calculationID = getOutID(ado, fileID);
        s.Format("%d", calculationID);
        q.Replace("$calculationID$", s);
    }

    return http_get_q(file, q, r1, r2, true);
}

void get_google_coord(CFPoint pt, double& x, double& y);


void write_xy(JSON& json, CFPoint pt)
{
    /*
        if (!gidr_view) return;

        CFRect rect = gidr_view->getMaxRect();
    //    double dmas = gidr_view->dmas;
        double dmas = DMAS96;

        pt.x = (pt.x)*dmas / 1000000;
        pt.y = (pt.y)*dmas / 1000000;

        json.add("{");
        json.add("x", pt.x);
        json.add("y", pt.y);
        json.add("}");
    */
    double lon, lat;

    get_google_coord(pt, lon, lat);

    json.add("{");
    json.add("lng", lon);
    json.add("lat", lat);
    json.add("}");
    log1("write_xy  %f %f", lat, lon);
}


CString http_get_list_analyse(const char* file, const char* code, const char* table)
{
    JSON json;

    CAdoFile* ado = getAdo(file);

    if (!ado) return "";

    Join join(code, "", -1, ado);

    json.add("{");
    json.add("\"data\" : [");
    ado_add_table(ado, json, join, "table");
    json.add("]");
    json.add("}");

    return json.str();
}

//const double dx = 4000, dy = 4000;

const double dx = 500, dy = 500;

void get_google_rect_json(CFPoint pt, CFRect rect, int width, int height, JSON &json)
{
    if (rect.Width() < dx*2) {
        rect.left = pt.x - dx;        
        rect.right = pt.x - dx;        
    }
    if (rect.Height() < dy*2) {
        rect.top = pt.x-dy;
        rect.bottom = pt.x+dy;
    }

    double lon1, lat1, lon2, lat2, lon, lat;

    get_google_coord(pt, lon, lat);
    get_google_coord(rect.TopLeft(), lon1, lat1);
    get_google_coord(rect.BottomRight(), lon2, lat2);

    json.add("{");
    if (width > 0 && height > 0) {
        double masx = rect.Width() / width;
        double masy = rect.Height() / height;

        double masxy = max(masx, masy);

        double dmas = 96;
        int zz = get_z(dmas, masxy * dmas);

        json.add("mas", zz);
    }

    json.add("lng", lon);
    json.add("lat", lat);
    
    json.add("lng1", lon1);
    json.add("lng2", lon2);

    json.add("lat1", lat2);
    json.add("lat2", lat1);
    json.add("}");
}

void get_google_rect_json(CFRect rect, int width, int height, JSON &json) 
{
    get_google_rect_json(rect.center(), rect, width, height, json);
}


void get_google_point_json(CFPoint pt, JSON &json)
{
    CFRect rect(pt.x-dx, pt.y-dy, pt.x+dx, pt.y+dy);

    get_google_rect_json(pt, rect, 0, 0, json);

/*

//    get_google_rect_json(, json);

    double lon1, lat1, lon2, lat2, lon, lat;

    get_google_coord(pt, lon, lat);
    get_google_coord(rect.TopLeft(), lon1, lat1);
    get_google_coord(rect.BottomRight(), lon2, lat2);

    json.add("{");
    json.add("lng", lon);
    json.add("lng", lon);
    
    json.add("lng1", lon1);
    json.add("lng2", lon2);

    json.add("lat1", lat2);
    json.add("lat2", lat1);
    json.add("}");
*/
}



CString http_get_table_position(const char* file, const char* table, const char* _type, int id)
{
    if (!gidr_view) return "";

    CString type = _type;

    CGraph2* graph = gidr_view->m_cxema->m_graph;

    JSON json;

    if (type == "node") {
        CNode2* n = graph->find(id);
        if (n) {
//            write_xy(json, n->node.coord);
//            get_google_rect_json(CFRect(n->node.coord.x-100, n->node.coord.y-100, n->node.coord.x+100, n->node.coord.y+100), json);
            get_google_point_json(n->node.coord, json);
        }
    }
    else if (type == "line") {
        CLINE2* l = graph->findLine(id);
        if (l) {
            get_google_rect_json(bline(l)->line.m_NP.centroid(), bline(l)->line.rect, 0, 0, json);
//            write_xy(json, bline(l)->line.m_NP.centroid());
        }
    }
    else if (type == "out") {
        CNode2* n = NULL;

        if (!strcmp(table, "PT_OUT")) {
            n = graph->findPtOut(id);
        }
        else {
            n = graph->findNodeOut(table, id);
        }
        if (n) {
//            write_xy(json, n->node.coord);
            get_google_point_json(n->node.coord, json);
        }
        else {
            CLINE2* l = graph->findLineOut(table, id);
            if (l) {
//                write_xy(json, bline(l)->line.m_NP.centroid());
                get_google_rect_json(bline(l)->line.m_NP.centroid(), bline(l)->line.rect, 0, 0, json);
            }
        }
    }
    else {
        Klassif* kls = m_geofile->m_kl_list->findKlN(table);

        if (!kls) return "";

        CGeoObject* geo = kls->getGeoObjectById(id);

        if (geo) {
            if (isPoint(kls->loc)) {
//                get_google_rect_json(geo->cl.GetFRect(), json);
                CFPoint pt = geo->cl.first();
//                get_google_rect_json(CFRect(pt.x-100, pt.y-100, pt.x+100, pt.y+100), json);
                get_google_point_json(pt, json);
            }
            else {
//                pt = geo->cl.centroid();
                get_google_rect_json(geo->cl.centroid(), geo->cl.GetFRect(), 0, 0, json);
            }
//            write_xy(json, pt);
        }
    }

    return json.str();
}


CString http_get_table_list()
{
    CString db = getGidAdoName();
    CAdoFile* ado = getAdo(db);
    if (!ado) return "";

    JSON json;
    json.add("{");
    json.add("\"data\": [");

    for (int i = 0; i < nodefsize; i++) {
        json.add("{");
        json.add("db", db);
        json.add("code", nodef[i].code);
        json.add("name", nodef[i].table);
        json.add("type", "node");

        if (nodef[i].tableMySQL && nodef[i].tableMySQL[0]) {
            json.add("table", nodef[i].tableMySQL);
        }
        else {
            json.add("table", "nodes");
        }

        json.add("}");
    }
    for (int i = 0; i < linefsize; i++) {
        json.add("{");
        json.add("db", db);
        json.add("code", linef[i].code);
        json.add("name", linef[i].table);
        json.add("type", "line");

        if (linef[i].tableMySQL && linef[i].tableMySQL[0]) {
            json.add("table", linef[i].tableMySQL);
        }
        else {
            json.add("table", "linesobj");
        }
        json.add("}");
    }

    json.add("]");
    json.add("}");

    /**
        int tsize = sizeof(menudocUnvis) / sizeof(menudocUnvis[0]);

        for (i = 0; i < tsize; i++) {
            if (menudocUnvis[i].name) {
    //            menu.Add(menudocUnvis[i].name, (void*)k); k++;
            }
        }
    */
    return json.str();
}


CString http_get_table_out_list()
{
    CString db = getGidAdoName();
    CAdoFile* ado = getAdo(db);
    if (!ado) return "";

    JSON json;
    json.add("{");
    json.add("\"data\": [");

    for (int i = 0; menudocres[i].name; i++)
    {
        json.add("{");
        json.add("db", db);
        json.add("code", menudocres[i].name);
        json.add("name", menudocres[i].kode);
        json.add("table", menudocres[i].name);
        json.add("type", "out");

        json.add("}");
    }

    json.add("]");
    json.add("}");

    return json.str();
}


CString http_get_table_analiz_list()
{
    CString db = getGidAdoName();
    CAdoFile* ado = getAdo(db);
    if (!ado) return "";

    JSON json;
    json.add("{");
    json.add("\"data\": [");


    struct MenuItemOut {
        char* name;
        char* out;
        char* query;
        int node_line;
    };

    MenuItemOut menudoc[] = {
      "Участки теплопроводов", "UT_OUT", NULL, 2,
      "Потребители", "PT_OUT", NULL, 1,
      "Насосные агрегаты", "NS_OUT", NULL, 2,
      "Сетевые регуляторы", "RS_OUT", NULL, 2,
      "Байпасы наружных теплопроводов", "BP_OUT", NULL, 2,
      "Задвижки", "ZD_OUT", NULL, 2,
      "Регулирующая арматура", "ZD2_OUT", NULL, 2,
      "Узлы", "US_OUT", NULL, 1,
    NULL, NULL, NULL,
    };

    for (int i = 0; menudoc[i].name; i++)
    {
        json.add("{");
        json.add("db", db);
        json.add("code", menudoc[i].out);
        json.add("name", menudoc[i].name);
        json.add("table", menudoc[i].out);
        json.add("type", "out");

        json.add("}");
    }

    json.add("]");
    json.add("}");

    return json.str();
}





CString http_get_table_gid_position(const char* file, const char* table, int id)
{
    if (!gidr_view) return "";

    Klassif* kls = m_geofile->m_kl_list->findKlN(table);

    if (!kls) return "";

    CGeoObject* geo = kls->getGeoObjectById(id);

    JSON json;

    if (geo) {
        CFPoint pt = geo->cl.centroid();

        write_xy(json, pt);
        /*
                CFRect rect = gidr_view->getMaxRect();
        //        double dmas = gidr_view->dmas;
                double dmas = DMAS96;

                pt.x = (pt.x)*dmas / 1000000;
                pt.y = (pt.y)*dmas / 1000000;

                json.add("{");
                json.add("x", pt.x);
                json.add("y", pt.y);
                json.add("}");
        */
    }

    return json.str();
}

CString get_minmax(const char* name, const char* s_min, const char* s_max)
{
    CString ss1 = "", ss2 = "";
    if (s_min[0])  ss1.Format("%s >= %s", name, s_min);
    if (s_max[0])  ss2.Format("%s <= %s", name, s_max);

    if (ss1 != "" && ss2 != "") return ss1 + " AND " + ss2;
    return ss1 + ss2;

}

CString http_set_field(const char* pBuf)
{
    //    char szDT[200];

    long l = strlen(pBuf);
    int r;
    int numtok = 200;
    jsmntok_t* t = NULL;

    jsmn_parser p;

    do {
        t = (jsmntok_t*)malloc(numtok * sizeof(jsmntok_t));
        jsmn_init(&p);
        r = jsmn_parse(&p, pBuf, l, t, numtok);
        if (r == -1) {
            free(t);
            numtok *= 2;
            t = (jsmntok_t*)malloc(numtok * sizeof(jsmntok_t));
        }
    } while (r < 0 && numtok < 10000);

    int id_fields = -1;

    int id_data = -1;
    int id_data_row = -1;

    bool first = true;
    string s_max = "", s_min = "", s_name1 = "";
    string s_db = "", s_code = "", s_name = "", s_table = "", s_type = "";

    CString ss = "";

    CString qq;
    CString qq0;

    qq0.Format("");
    CString q = qq0;

    bool is_info = false;

    for (int i = 0; i < r; i++) {
        string s, sp, spp;

        int ip = t[i].parent;
        int ipp = t[i].parent >= 0 ? t[t[i].parent].parent : -1;

        spp = getJsonName(pBuf, t, ipp);
        sp = getJsonName(pBuf, t, ip);
        s = getJsonName(pBuf, t, i);


        if (t[i].type == JSMN_OBJECT && sp == "info") {
            is_info = true;
        }

        if (t[i].type == JSMN_ARRAY && sp == "fields") {
            id_fields = i;
        }

        if (ip >= 0 && t[i].type == JSMN_OBJECT && ip == id_fields) {
            if (!first) {
                qq = get_minmax(s_name1.c_str(), s_min.c_str(), s_max.c_str());
                if (qq != "") {
                    q += " AND ";
                    q += qq;
                }
            }
            id_data_row = i;
            s_name1 = s_min = s_max = "";
            first = false;
        }

        //        if (t[i].type == JSMN_STRING || t[i].type == JSMN_PRIMITIVE) {
        if (t[i].type == JSMN_STRING) {
            if (is_info) {
                if (sp == "db") s_db = s;
                else if (sp == "code") s_code = s;
                else if (sp == "name") s_name = s;
                else if (sp == "table") s_table = s;
                else if (sp == "type") s_type = s;
            }

            if (ipp >= 0 && ipp == id_data_row) {
                if (sp == "name") s_name1 = s;
                else if (sp == "max") s_max = s;
                else if (sp == "min") s_min = s.c_str();
            }
        }
    }
    qq = get_minmax(s_name1.c_str(), s_min.c_str(), s_max.c_str());
    if (qq != "") {
        q += " AND ";
        q += qq;
    }

    free(t);

    int ret = 0;
    qq.Format("SELECT * FROM %s WHERE 1=1 %s", s_code.c_str(), q);
    return http_get_q(s_db.c_str(), qq, 1, -1, true);
}



CString http_get_node_id(int id)
{
    JSON json;
    return json.str();
}

CString http_get_node_xy(int mas, double x, double y, int internalNodeID)
{
    if (!gidr_view) return "";
    if (mas > 3000) return "";

    KamState ks = gidr_view->Kam(internalNodeID);

    //    double dmas = gidr_view->dmas;
    double dmas = DMAS96;
    //  double dmas = 100;

    double masx_old = gidr_view->geom.masx, masy_old = gidr_view->geom.masy, bx_old = gidr_view->geom.m_bx, by_old = gidr_view->geom.m_by;
    gidr_view->geom.masx = gidr_view->geom.masy = mas / dmas;

    CFRect rect = gidr_view->getMaxRect();

    double x0 = x * 1000000 / dmas;
    double y0 = y * 1000000 / dmas;

    CFPoint pt = CFPoint(x0, y0);


    CGraph2* graph = gidr_view->m_cxema->m_graph;

    double md = D5 * mas / dmas;

    list<CLINE2*> list_line;
    list<CNode2*> list_node;
    findNode(graph, pt, list_node, md * 2);

    JSON json;

    json.add("{");

    {
        json.add("\"nodes\" : [");
        for (list<CNode2*>::const_iterator it = list_node.begin(); it != list_node.end(); it++) {
            CNode2* node = *it;
            json.add("{");
            json.add("db", node->getFile()->m_schema);
            json.add("nomgP", node->node.nomgP);
            json.add("nomgO", node->node.nomgO);
            json.add("id", node->id);
            json.add("internalNodeID", node->node.internalNodeID);
            json.add("id2", node->node.id2);
            json.add("table_rus", node->getTable());
            json.add("table", node->getTableMySQL());
            json.add("out", node->getTableOut());
            json.add("node", node->getName());
            json.add("name", node->getName());
            json.add("}");
        }
    }
    json.add("]");

    json.add("}");

    gidr_view->geom.masx = masx_old; gidr_view->geom.masy = masy_old;
    gidr_view->geom.m_bx = bx_old; gidr_view->geom.m_by = by_old;

    gidr_view->KamObr(ks);

    return json.str();
}

CString http_get_fragments()
{
    if (!gidr_view) return "";

    JSON json;

    CString file_name = "";
    int fileID = gidr_view->getActiv();

#if 0

    CString db = getGidAdoName();
    CAdoFile* ado = getAdo(db);
    if (!ado) return "";

    if (ado->openTable0("SELECT id,name FROM fragments")) {

        json.add("{\"data\": [");

        while (!ado->isEOF()) {
            long id = ado->read_long("id");
            CString name = ado->readStr("name");
            json.add("{");
            json.add("id", id);
            json.add("name", name);

            if (id == fileID) {
                file_name = name;
            }


            json.add("}");

            ado->MoveNext();
        }

        json.add("]");


        json.add("\"currentfile\" : {");
        json.add("fileID", fileID);
        json.add("name", file_name);
        json.add("}");


        json.add("}");

        ado->closeTable();
    }
#endif

    map<long, CGidFile*>::const_iterator it = gidr_view->m_cxema->m_file_map.begin();

    json.add("{\"data\": [");

    for (; it != gidr_view->m_cxema->m_file_map.end(); it++) {
        //    return &it->second;

        json.add("{");
        json.add("id", it->first);
        json.add("name", it->second->name);

        if (it->first == fileID) {
            file_name = it->second->name;
        }

        json.add("}");
    }
    json.add("]");

    json.add("\"currentfile\" : {");
    json.add("fileID", fileID);
    json.add("name", file_name);
    json.add("}");


    json.add("}");

    return json.str();
}

CString http_set_fragment(int id)
{
    if (!gidr_view) return "";

    JSON json;
    gidr_view->setActiv0(id);

    return json.str();
}


CString http_get_address_alma()
{
    Klassif* kls = NULL;

    if (!kls) {
        kls = m_geofile->m_kl_list->findKlN(transl("Здания"));
        if (kls) {
            CString q = "SELECT DISTINCT CONCAT(name_stree, ' ', number_1) AS name FROM zdaniya zk WHERE NOT name_stree IS NULL ORDER BY CONCAT(name_stree, ' ', number_1)";
            return http_get_q(kls->m_database, q, 0, 100, true);
        }
    }

    /**
        if (!kls) {
            kls = m_geofile->m_kl_list->findKlN(transl("Здания потребителей"));
            if (kls) {
            }
        }
    */

    if (!kls) {
        kls = m_geofile->m_kl_list->findKlN(transl("Здания Карта"));
        if (kls) {
            CString q = "SELECT DISTINCT CONCAT(street_nam, ' ', number_1) AS name FROM zdaniya_karta zk WHERE NOT street_nam IS NULL ORDER BY CONCAT(street_nam, ' ', number_1)";
            return http_get_q(kls->m_database, q, 0, 100, true);
        }
    }

    JSON json;
    return json.str();
}

CString get_tg_graph(int id)
{
    return "";
}

CString get_tg_table(int id)
{
    if (!gidr_view) return false;

    CString fn;
    CGraph2* graph = gidr_view->m_cxema->m_graph;

    CNode2* n = graph->find(id);

    if (!n) return  "";

    CString tn = "deployedTempGraphs";

    CString q;

    //    q.Format("SELECT id,tn,Q_otn,t1,t2,t3,tv,t_bn,tg FROM %s hs JOIN nodes ON nodes.id=hs.nodeID WHERE nodes.id=%d ORDER BY tn DESC", tn, id);

    q.Format(

        "SELECT hs.id,tn,Q_otn,t1,t2,t3,tv,t_bn,tg "
        " FROM deployedTempGraphs gr "
        " JOIN heatSources hs ON gr.hSourceID=hs.id "
        " JOIN nodes ON nodes.id=hs.nodeID WHERE nodes.id=%d ORDER BY tn DESC",

        id);

    CString file = getGidAdoName();

    return http_get_q(file, q, 0, 100, true);
}



CString http_get_config()
{
    JSON json;

    json.add("{");
    json.add("db", getGidAdoName());
    json.add("ps", getPsAdoName());
    json.add("}");

    return json.str();

}

string getTiles();
void setTiles(const char* t);
CFRect getRsRect(int id);
CFRect getMsRect(int id);

CString tileName(const CString& f)
{
    CString filename;
    string m_tiles = getTiles();

    if (m_tiles == "") {
        char path1[512];
        HRESULT ret = SHGetSpecialFolderPath(NULL, path1, CSIDL_APPDATA, 1);
        m_tiles = CString(path1) + "\\Sirius\\tgid\\tiles\\";
        setTiles(m_tiles.c_str());
    }
    filename.Format("%s%s", m_tiles.c_str(), f);
    return filename;
}

CString row_col_name(int r, int c)
{
    CString ss;
    ss.Format("%d/%d", c, r);

    return ss;
}


CString tileName(int dpi, int m, int r, int c, int internalNodeID, int layer, int gid, int geo, int ms, int rs)
{
    int id = 0;
    CString filename;
    string m_tiles = getTiles();

    if (m_tiles == "") {
        char path1[512];
        HRESULT ret = SHGetSpecialFolderPath(NULL, path1, CSIDL_APPDATA, 1);
        m_tiles = CString(path1) + "\\Sirius\\tgid\\tiles\\";
        setTiles(m_tiles.c_str());
    }

    CString gidgeo = "";

    if (gid) gidgeo += "gid";
    if (geo) gidgeo += "geo";

    CString row_col = row_col_name(r, c);


    if (layer > 0) {
        gidgeo = "error";
        for (int i = 0; i < sizeof(layers) / sizeof(layers[0]); i++) {
            if (layers[i].id == layer) {
                gidgeo = layers[i].code;
            }
        }
    }


    filename.Format("%s%s/%d/%d/%d/%s.png", m_tiles.c_str(), gidgeo, dpi, internalNodeID, m, row_col);

    if ((ms > 0 || rs > 0) && (layer == L_ALL || layer == L_GID)) {
        CString ms_rs = "";
        CFRect r0;

        if (ms > 0) {
            id = ms;
            ms_rs = "ms";
            r0 = getMsRect(ms);
        }
        else {
            id = rs;
            ms_rs = "rs";
            r0 = getRsRect(rs);
        }

        CFRect rect = gidr_view->getMaxRect();
        double dmas = DMAS96;

        double masx, masy, alpha;
        CFRect rr;

        tile_coord(1, 1, m, c, r, masx, masy, alpha, rr);

        CFRect rrr;

        if (rrr.IntersectRect(rr, r0)) {
            filename.Format("%s%s/%d/%s/%d/%d/%d/%d/%s.png", m_tiles.c_str(), gidgeo, dpi, ms_rs, id, geo, internalNodeID, m, row_col);
        }
    }

    return filename;
}



CString http_get_screen()
{
    double dmas = DMAS96;

    if (gidr_view) {
        //        dmas = gidr_view->dmas;
        dmas = DMAS96;
    }
    JSON json;

    json.add("{");
    json.add("dmas", dmas);
    json.add("}");


    return json.str();
}


#include "http2/HTTPThrea.h"
#include "http2/WebServerDlg2.h"


CHttpThread* m_pThread8080 = NULL;

void closeWeb()
{
    if (!m_pThread8080) return;

    m_pThread8080->stop();

    //  KillTimer(nTimerID);

    WaitForSingleObject(m_pThread8080->m_hThread, INFINITE);
    delete m_pThread8080;
    m_pThread8080 = NULL;
}


void startWeb(CGidrView* view)
{
    if (m_pThread8080) return;

    int port = 8080;

    CWebServerDlg2* dlg = new CWebServerDlg2;

    dlg->m_hWnd = 0;
    dlg->m_szDefIndex = _T("map.html");
    dlg->m_szHomeDir = argpath_2() + "ServerRoot2";
    dlg->m_szPrivate = "";
    dlg->m_szPublic = "";

    char path1[512];
    HRESULT ret = SHGetSpecialFolderPath(NULL, path1, CSIDL_APPDATA, 1);
    CString tiles = CString(path1) + "\\Sirius\\tgid\\tiles\\";

    dlg->m_szTilesDir = AfxGetApp()->GetProfileString("WebServer2", "Tiles Dir", tiles);

    m_pThread8080 = new CHttpThread(dlg, 0, port, 10, 0);

    if (m_pThread8080) {
        m_pThread8080->CreateThread();
        m_pThread8080->m_bAutoDelete = FALSE;
        gidr_view = view;
    }

    //  if (m_pThread8080) {
    //    long nTimerID = SetTimer(1, 500, NULL);
    //  }
}


CString findTableRusName(CString d, CString n);


CString http_get_layers(int dpi)
{
    JSON json;

    json.add("{");

    json.add("\"layers\" : [");
    for (int i = 0; i < sizeof(layers) / sizeof(layers[0]); i++) {
        CString name = layers[i].name;
        Klassif* kls = nullptr;

        int mas1 = 0, mas2 = 0, mas_text1 = 0, mas_text2 = 0;
        int z1 = 0, z2 = 22;


        if (layers[i].code == "geo") {
            if (false) {
                continue;
            }
        }


        if (name == "") {
            name = findTableRusName(getGidAdoName(), layers[i].code);
            kls = m_geofile->m_kl_list->findKlN(layers[i].code);

            if (kls) {
                mas1 = kls->mas1;
                mas2 = kls->mas2*3;
                mas_text1 = kls->mas_text1;
                mas_text2 = kls->mas_text2;
            }
        }
        if (layers[i].code == "pts") {
            mas1 = 0;
            mas2 = 5000;
        }

        double dmas = dpi / 2.54;

        if (mas1 > 0) {
            z2 = get_z(dmas, mas1) + 0;
        }
        if (mas2 > 0) {
            z1 = get_z(dmas, mas2) - 0;
        }

        if (kls && kls->ins || layers[i].name != "") {
            json.add("{");
            json.add("id", layers[i].id);

            json.add("name", name);
            json.add("mas1", mas1);
            json.add("mas2", mas2);
            json.add("z1", z1);
            json.add("z2", z2);
            //            json.add("mas_text1", mas_text1);
            //            json.add("mas_text2", mas_text2);

            json.add("}");
        }
    }
    json.add("]");
    json.add("}");

    return json.str();
}


#include "TaskProgressDlg.h"


void CGidrView::OnSavePNG()
{
    CString str;

    int dpi = 96;

    m_CurNode = NULL;
    m_CurLine = NULL;

    setGidView((CGidrView*)this);

    CWaitCursor cur;

    CFRect rectMax = getMaxRect();

    CString m_szTilesDir = AfxGetApp()->GetProfileString("WebServer2", "Tiles Dir", "");

    m_off = true;

    long dt1 = 0, dt0 = 0, dt2 = 0;

    clock_t t0_1 = clock();

    int nnn = 0;
    int mm = 5;

    CFRect rect = getMaxRect();

    double x01, y01, x02, y02;

    get_google_coord(rect.TopLeft(), x01, y01);
    get_google_coord(rect.BottomRight(), x02, y02);

    double lon2x_(double x);
    double lat2y_(double y);

    x01 = lon2x_(x01);
    y01 = lat2y_(y01);
    x02 = lon2x_(x02);
    y02 = lat2y_(y02);

    double dmas = DMAS96;

    Percent2 pc;

    int dx = DX, dy = DY;
    int zz1 = 10, zz2 = 16;

    for (int zz = zz1; zz <= zz2; zz++) {

        CString ss;
        ss.Format("%d", zz);

        pc.SetText(ss);

        double n2 = pow(2., zz);

        int x1 = (x01 + 180) * n2 / 360;
        int y1 = (-y01 + 180) * n2 / 360;
        int x2 = (x02 + 180) * n2 / 360;
        int y2 = (-y02 + 180) * n2 / 360;

        int n_t = (y2 - y1 + dy - 1) * (x2 - x1 + dx - 1) / dx / dy * 2;

        nnn = 0;

        for (int row = y1; row <= y2; row += dy) {
            for (int col = x1; col <= x2; col += dx) {
                if (pc.SetValue(nnn * 100 / n_t)) goto QUIT;
                nnn++;


                CString fn = tileName(dpi, zz, row, col, 0, L_GEO, 1, 0, 0, 0);

                CFRect r;

                int xx1 = col / dx * dx, xx2 = xx1 + dx - 1;
                int yy1 = row / dy * dy, yy2 = yy1 + dy - 1;

                double masx, masy, alpha;
                tile_coord(dx, dy, zz, xx1, yy1, masx, masy, alpha, r);

                if (r.bottom >= rect.top && r.right >= rect.left && r.top < rect.bottom && r.left < rect.right) {
                    CClientDC oDC(this);

                    CDC dc;
                    dc.CreateCompatibleDC(&oDC);

                    CBitmap bitmap;
                    bitmap.CreateCompatibleBitmap(&oDC, DD * dx, DD * dy);
                    dc.SelectObject(bitmap);

                    m_web_layer = L_GEO;
                    double dmas_old = geom.dmas;
                    geom.dmas = dpi / 2.54;
                    geom.m_alpha = -alpha;

                    set_coord_gid(r.center().x, r.center().y, geom.m_alpha);

                    CString ss;

                    CopyBlockPng4(dc, masx, masy, r.left, r.top, DD * dx, DD * dy);
                    draw_ramka(dc, ss, DD * dx, DD * dy);

                    int internalNodeID = 0;

                    for (int yyy = yy1; yyy <= yy2; yyy += 1) {
                        for (int xxx = xx1; xxx <= xx2; xxx += 1) {
                            CString fn = tileName(dpi, zz, yyy, xxx, internalNodeID, L_GEO, 0, 0, 0, 0);
                            mkdir_for_file(fn);
                            ::savePNG4(dc.m_hDC, (xxx - xx1) * DD, (yyy - yy1) * DD, DD, DD, fn);
                        }
                    }
                    m_web_layer = L_ALL;
                    geom.dmas = dmas_old;
                    geom.m_alpha = 0;
                    set_coord_gid(0, 0, 0);
                }
            }
        }
    }

QUIT:

    dt0 = clock() - t0_1;

    str.Format("%g\n%g секунд\n%g секунд\n%g на одну", (double)dt0 / CLOCKS_PER_SEC, (double)dt1 / CLOCKS_PER_SEC, (double)dt2 / CLOCKS_PER_SEC, ((double)dt0) / nnn / CLOCKS_PER_SEC);
    AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);


    m_off = false;

    return;

    //    graph->restore_pjezo();
}

void resetGid(CCxema* m_cxema, const CString& m_user_gid);


int get_max_changed_object();

static bool m_cxema_updated = false;

void set_cxema_updated(bool yes)
{
    m_cxema_updated = yes;
}

bool is_cxema_updated()
{
    if (!gidr_view) {
        log1("Ошибка в bool is_cxema_updated()");
        return false;
    }

    if (!m_cxema_updated) {
        CAdoFile* ado = gidr_view->m_cxema->m_ado;

        int max_changed_object = get_max_changed_object();
        int max_changed_object_current = readTableValueInt(ado, "select max(id) as id from changed_object");

        m_cxema_updated = max_changed_object_current > max_changed_object;

        CString ss;
        ss.Format("is_cxema_updated c=%d %d (%d)", max_changed_object_current, max_changed_object, m_cxema_updated);

        log1(ss);
    }

    return m_cxema_updated;
}



CString html_cxema_updated()
{
    JSON json;

    //    CAdoFile *ado = gidr_view->m_cxema->m_ado;

    //    int max_changed_object = get_max_changed_object();
    //    int max_changed_object_new = readTableValueInt(ado, "select max(id) as id from changed_object");

    //    json.add("{");
    //    json.add("max_changed_object", get_max_changed_object());
    //    json.add("updated", max_changed_object_new > max_changed_object);
    //    json.add("}");

    //    log1("CString html_cxema_updated()");

    json.add("{");
    json.add("updated", m_cxema_updated);
    json.add("}");


    return json.str();
}


CString html_max_changed_object()
{
    JSON json;

    CAdoFile* ado = gidr_view->m_cxema->m_ado;

    int max_changed_object0 = readTableValueInt(ado, "select max(id) as id from changed_object");


    json.add("{");
    //    json.add("max_changed_object", get_max_changed_object());
    json.add("max_changed_object", max_changed_object0);
    json.add("}");

    return json.str();
}

CString html_reset_cxema()
{
    log1("html_reset_cxema()");

    resetGid(gidr_view->m_cxema, m_user_gid);

    return html_max_changed_object();
}

CString http_search_node(const CString &text1)
{
    CGraph2* graph = gidr_view->m_cxema->m_graph;
    CMapGraph::iterator p = graph->map_node.begin();

    CString text = text1;
    text.MakeUpper();

    JSON json;

    json.add("[");

    int nn = 0;

    while (p != graph->map_node.end()) {
        CNode2* n = p->second;

        if ((n->node.coord.x != 0 || n->node.coord.y != 0) 
           && (n->node.internalNodeID == 0) ) {

            CString name, name_pts = n->getNamePT();

            name.Format("%s", n->getNameFull());

            name.MakeUpper();
            name.MakeUpper();
            name_pts.MakeUpper();

            if (name.Find(text) != -1 || name_pts.Find(text) != -1) {
                json.add("{");
                json.add("id", n->id);

                json.add("kod", n->getKod());
                json.add("name", n->node.name);
                json.add("pts", n->getNamePT());

                double lon, lat;

                get_google_coord(n->node.coord, lon, lat);

                json.add("lng", lon);
                json.add("lat", lat);

                json.add("}");

                nn += 1;

                if (nn > 10000) break;
            }
        }
        p++;
    }
    json.add("]");

    return json.str();
}
