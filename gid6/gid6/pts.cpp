#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
#include "InputDia.h"
#include "Input2.h"
#include "gidrView.h"

#include "geofile.h"

#include "win.h"

#include "mmenu.h"
#include "join2.h"

#include "ado.h"        
#include <fstream>

#include "RemontZapros2.h"
#include "NewPassport/WebPageVirtualTable.h"

#include "wm_user.h"

#include "electro.h"

bool create_vyd_line_table_new(CAdoFile* ado, CGraph2* graph, CString& tn);

CString readQ(CAdoFile* ado, const char* tn, const char* tn2, int fileID);
CString readQ(CAdoFile* ado, const char* tn, const char* tn2, const char* fileID);

void initPassport(CAdoFile* ado, CGraph2* graph, HWND CWnd, CString argPath);
static map<CString, Klassif*> map_pts;

static map<CString, CString> map_onlygeo;

static set<CString> set_pts;


void init_set_file(const char *fn0, set<CString> & set1)
{
    CString fn;
    fn.Format("%s\\kls\\%s.lst", argpath(), fn0);

    char s[1024];

    set1.clear();

    FILE *f = fopen(fn, "r");

    if (f) {
        while (!feof(f)) {
            if (fgets(s,1023,f) == NULL) break;
            trim(s);
            CString sl = s;
            sl.MakeLower();
            set1.insert(sl);
        }
        fclose(f);
    }
}


void init_set_file0(const char* fn1, set<CString>& set_file)
{
    CString fn;
    fn.Format("%s\\kls\\%s.lst", argpath(), fn1);

    char s[1024];

    FILE* f = fopen(fn, "r");

    if (f) {
        while (!feof(f)) {
            if (fgets(s, 1023, f) == NULL) break;
            trim(s);

            set_file.insert(s);
        }
        fclose(f);
    }
}





bool is_in_set(const char *tn, set<CString> & set1)
{
    CString sl= tn;
    sl.MakeLower();

    auto it = set1.find(sl);
    if (it != set1.end()) {
        return true;
    }
    return false;    
}


static set<CString> set_water_line;
static set<CString> set_water_node;

void get_water_node(set<CString>& _set_water_node)
{
    _set_water_node = set_water_node;
}


void get_water_line(set<CString>& _set_water_line)
{
    _set_water_line = set_water_line;
}


void init_water_list_file()
{
    init_set_file("water_line", set_water_line);
    init_set_file("water_node", set_water_node);
}

bool is_water_line(const char *tn)
{
    return is_in_set(tn, set_water_line);
}

bool is_water_node(const char *tn)
{
    return is_in_set(tn, set_water_node);
}


static set<CString> set_onlyline;
static set<CString> set_onlypts;

void init_onlyline_list_file()
{
    init_set_file("onlyline", set_onlyline);
    init_set_file("onlypts", set_onlypts);
}


void init_onlypts_list_file()
{
}



bool is_onlyline(const char *tn)
{
    return is_in_set(tn, set_onlyline);
}


bool is_onlypts(const char *tn)
{
    return is_in_set(tn, set_onlypts);
}


static set<CString> set_noend;

void init_noend_list_file()
{
    init_set_file("noend", set_noend);
}


bool is_noend(const char *tn)
{
    return is_in_set(tn, set_noend);
}


void init_onlygeo()
{
    CString fn;
    fn.Format("%s\\kls\\onlygeo.lst", argpath());

    char s[1024];

    CString tab1 = "";

    map_onlygeo.clear();

    FILE* f = fopen(fn, "r");

    if (f) {
        while (!feof(f)) {
            if (fgets(s, 1023, f) == NULL) break;
            trim(s);

            if (s[0] != ' ') {
                tab1 = s;
            }
            else {
                ltrim(s);
                CString sl = s;
                sl.MakeLower();
                map_onlygeo[sl] = tab1;
            }
        }
        fclose(f);
    }
}


CString getOnlygeo(const char* tn)
{
    CString sl = tn;
    sl.MakeLower();

    auto it = map_onlygeo.find(sl);
    if (it != map_onlygeo.end()) {
        return it->second;
    }
    return "";
}




bool CGidrView::pts_info(const CFPoint &point)
{
    CString str;
    if (isPot()) return false;
    if (!m_geofile) return false;

    bool ret = false;

    for (auto it : map_pts) {
        CString tn = it.second->nazv;
        int ngeo = m_geofile->findXY3(point, D5* geom.masx, 3, tn);

       if (ngeo) {
           GeoInfo(tn, ngeo, false);
           break;
       }
    }

    return ret;
}


bool CGidrView::pts_move(const CFPoint &point)
{
    CString str;
    if (isPot()) return false;
    if (!m_geofile) return false;

    bool ret = false;

    for (auto it : map_pts) {
        CString tn = it.second->nazv;
        int ngeo = m_geofile->findXY3(point, D5* geom.masx, 3, tn);

       if (ngeo) {
            if (isEditGeo(tn)) GeoIsm(tn, ngeo);
            break;
       }
    }

    return ret;
}



bool CGidrView::tab_kls(Klassif* kls)
{
    RunGeoTable(kls);
    return true;
}

bool CGidrView::pts_del(const CFPoint &point)
{
    CString str;
    if (!isEditGeo("")) return false;
    if (isPot()) return false;
    if (!m_geofile) return false;

    for (auto it : map_pts) {
        CString fn0 = it.first;
        CString fn = it.second->nazv;
        int n = m_geofile->findXY3(point, D5* geom.masx, 3, fn);

        if (n) {
            if (!isDeletable(fn, n)) return false;

            if (!isEditGeo(fn)) return false;

            if (n) {
                str.Format(_TR("Удалить %s?"), fn0);
                if (MessageBox(str, _TR("Подтвердить удаление"), MB_YESNO) == IDYES) {
                    delGeoObj(fn, n);
                    GetDocument()->SetModifiedFlag(true);
                    InvalidateG();
                }
            }
            break;
        }
    }
    return true;
}



void init_set_pts()
{
    init_set_file("pts", set_pts);
}

bool is_pts(const CString &tn)
{
    return is_in_set(tn, set_pts);
}


void init_pts_list_file(const char * fn1)
{
    CString fn;
    fn.Format("%s\\kls\\%s.lst", argpath(), fn1);

    char s[1024];

    map_pts.clear();

    FILE *f = fopen(fn, "r");

    if (f) {
        while (!feof(f)) {
            if (fgets(s,1023,f) == NULL) break;
            trim(s);

            Klassif* kls = m_geofile->m_kl_list->findKlN(s);
            if (kls) {
                map_pts[kls->RusName()] = kls;
                CString ss = s;
                ss.MakeLower();
                set_pts.insert(ss);
            }
        }
        fclose(f);
    }
}


bool init_pts_table(const char * tab)
{
    map_pts.clear();

    Klassif* kls = m_geofile->m_kl_list->findKlN(tab);
    if (kls) {
        map_pts[kls->RusName()] = kls;
        return true;
    }
    return false;
}

void get_map_pts(map<CString, Klassif*> & _map_pts)
{
    _map_pts = map_pts;
}


void init_pts_list_table(const char * tab)
{
   Klassif* kls = m_geofile->m_kl_list->findKlN(tab);
   if (kls) {
       map_pts[kls->RusName()] = kls;
   }
}


Klassif *menu_list_file(CWnd *wnd, const char *capt, const char * fn1)
{
    CMMenu menu(wnd, IDD_MENU1, capt);

    init_pts_list_file(fn1);

    for (auto it : map_pts) {
        menu.Add(it.first, (void*) it.second);
    }

    if (menu.DoModal() == IDOK) {
        return (Klassif*) menu.getV();
    }
    return NULL;
}



#if 0

Klassif *menu_electro(CWnd *wnd)
{
    char* pts_list[] = {
"liniya_elektroperedach",
"gilza_es",
"istochnik_elektrosnabzheniya",
"priemnik_elektrosnabzheniya",
"kabelnyy_kanal_es",
"opora_es",
"mufta"
   };


    return menu_list(wnd, "Объекты электрических сетей", sizeof(pts_list)/sizeof(pts_list[0]), pts_list);
}

Klassif *menu_remont(CWnd *wnd)
{
    CMMenu menu(wnd, IDD_MENU2, _TR("Объект ремонтов"));

    menu.Add("Дефекты",     (void*) m_geofile->m_kl_list->findKlN("defect"));
    menu.Add("Ремонт",      (void*) m_geofile->m_kl_list->findKlN("remont"));
    menu.Add("Диагностика", (void*) m_geofile->m_kl_list->findKlN("diag"));
    menu.Add("Испытания",   (void*) m_geofile->m_kl_list->findKlN("opressovka_defekt"));

    if (menu.DoModal() == IDOK) {
        return (Klassif*) menu.getV();
    }
    return NULL;
}



Klassif *menu_pts(CWnd *wnd)
{
    char* pts_list[] = {

"drenazhnyy_kran2",
"zapornaya_armatura2",
"vozdushnik2",
"vvod_v_zdanie",
"vertikalnyy_podem_opusk_truprovoda",
"vozdushnik",
"drenazhnyy_kran",
"zapornaya_armatura",
"kanal",
"kompensator",
"lyuki",
"opora",
"perehod_diametra",
"ugol_povorota_truboprovoda",
"granitsy_vertikalnyh_kompensatorov",
"kamera_opuska_ili_podema",
"kolodtsy",
"perehlesty_setey",
"peremychki",
"uzel_kanala",
"vvody_v_zdanie",
"tkamera",
"pavilion",
"tstrp",
"nasosnye_stantsii",
"istochniki_teplosnabzheniya",
"bridge_crossing",
"duker",
"shield"
    };

    return menu_list(wnd, "Объекты ПТС", sizeof(pts_list)/sizeof(pts_list[0]), pts_list);
}

#endif

void CGidrView::addKls(Klassif* kls, bool init)
{
    if (addGeo(kls, init)) {
        CString str;
        str.Format(_TR("Начинайте создавать %s"), kls->RusName());
        AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
        GetDocument()->SetModifiedFlag(true);
    }
}





void CGidrView::OnRemontsAddDefect()
{
    Klassif* kls = m_geofile->m_kl_list->findKlN("defect");

    if (kls) {
        addKls(kls);
    }
}

void CGidrView::OnRemontsAddDiag()
{
    Klassif* kls = m_geofile->m_kl_list->findKlN("diag");
    if (kls) addKls(kls);
}

void CGidrView::OnRemontsAddOpres()
{
    Klassif* kls = m_geofile->m_kl_list->findKlN("defect");
    if (kls) {
        CString str;
        str.Format(_TR("Начинайте создавать Повреждение для опрессовки"));
        AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);

/*
        CString str;
        str.Format(_TR("Начинайте создавать %s"), kls->RusName());
        AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
        addGeo(kls);
        GetDocument()->SetModifiedFlag(true);
*/

        m_kls = kls;
        regim = R_INIT;
        SetCur(m_nRegim);
        initGeoValue();
        AddGeoValue("remontTypeID", COleVariant(2L));
        m_nRegim = ID_NEW_GEO;

        GetDocument()->SetModifiedFlag(true);
    }
}


void CGidrView::OnRemontsAddRemont()
{
    Klassif* kls = m_geofile->m_kl_list->findKlN("remont");
    if (kls) addKls(kls);
}


void CGidrView::OnRemontsAdd()
{
//    Klassif *kls = menu_remont(this);
//    Klassif *kls = menu_list_file(this, "Объект ремонтов", "remont");

    Klassif* kls_defect = m_geofile->m_kl_list->findKlN("defect");
    Klassif* kls_diag = m_geofile->m_kl_list->findKlN("diag");
    Klassif* kls_remont = m_geofile->m_kl_list->findKlN("remont");
    
    CMMenu menu(this, IDD_MENU1, "Объект ремонтов");

    if (kls_defect) menu.Add(kls_defect->RusName(), (void*) kls_defect);
//    menu.Add(kls_diag->RusName(), (void*) kls_diag);

    if (kls_diag) {
        menu.Add("Диагностика Шурф", (void*) kls_diag);
        menu.Add("Диагностика Вырезка ", (void*) kls_diag);
        menu.Add("Диагностика Освидетельствование", (void*) kls_diag);
    }

    if (kls_remont) menu.Add(kls_remont->RusName(), (void*) kls_remont);

    if (menu.DoModal() == IDOK) {
        initGeoValue();
        
        Klassif *kls = (Klassif *) menu.getV();

        if (kls) {

        if (kls == kls_diag) {
            int sredstvoDiagID = -1;

            switch (menu.m_Index)
            {
            case 1: sredstvoDiagID = 1; break;
            case 2: sredstvoDiagID = 2; break;
            case 3: sredstvoDiagID = 3; break;
            default:
                break;
            }
            AddGeoValue("sredstvoDiagID", (long)sredstvoDiagID);
        }

        if (addGeo(kls, false)) {
           CString str;
           str.Format(_TR("Начинайте создавать %s"), kls->RusName());
           AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
        }
        GetDocument()->SetModifiedFlag(true);
        }

//        addKls(kls);
    }
}

void CGidrView::OnRemontsInfo()
{
    init_pts_list_file("remont");
    regim = R_INIT;
    m_nRegim = ID_PTS_INFO;
}

void CGidrView::OnRemontsDel()
{
    init_pts_list_file("remont");
    regim = R_INIT;
    m_nRegim = ID_PTS_DEL;
}

void CGidrView::OnRemontsTable()
{
    Klassif* kls = menu_list_file(this, "Объект ремонтов", "remont");
    if (kls) tab_kls(kls);
}

void CGidrView::OnRemontsZhurnal()
{
    Klassif* kls = menu_list_file(this, "Объект ремонтов", "remont");

    if (kls) {
        if (kls->nazv.CompareNoCase("defect") == 0) viewNizGeoQ("defect", "stateID", "data_osmotra", true);
        if (kls->nazv.CompareNoCase("diag")   == 0) viewNizGeoQ("diag", "sredstvoDiagID", "data_prov", true);
        if (kls->nazv.CompareNoCase("remont") == 0) viewNizGeoQ("remont", "stateID", "data_osmotra", true);
    }
}

void CGidrView::OnPtsAdd()
{
//    Klassif *kls = menu_pts(this);
    Klassif *kls = menu_list_file(this, "Объекты ПТС", "pts");

    if (kls) {
        if (kls->nazv == "kanal") {
            AfxMessageBox("Укажите две точки на участке");
            regim = R_INIT;
            m_nRegim = ID_BEGIN_CANAL_1;
            m_l_canal = nullptr;
            m_kls = kls;
        }
        else {
            addKls(kls);
        }
    }
}

void CGidrView::OnPtsInfo()
{
    init_pts_list_file("pts");
    regim = R_INIT;
    m_nRegim = ID_PTS_INFO;
}

void CGidrView::OnPtsDel()
{
    init_pts_list_file("pts");
    regim = R_INIT;
    m_nRegim = ID_PTS_DEL;
}

void CGidrView::OnPtsTable()
{
    Klassif* kls = menu_list_file(this, "Объекты ПТС", "pts");
    if (kls) tab_kls(kls);
}

void CGidrView::OnPtsZhurnal()
{
    Klassif* kls = menu_list_file(this, "Объекты ПТС", "pts");
    if (kls) {
//        if (strcmp(kls->TableName(), "heatPoint") == 0) {
        if (!strcmp(kls->TableName(), "heatPoint")) {
            initPassport(m_cxema->m_ado, m_cxema->m_graph, GetSafeHwnd(), argpath());
        }
        else {
            CString tn = kls->TableName();
//            Zhurnal(tn, "", nullptr, nullptr, true, kls->RusName());
            viewNizGeoQ(tn, "", "", true);
        }
    }
}

void CGidrView::OnKorrozAdd()
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(REM_INDICATOR);
    if (kls) addKls(kls);
}

void CGidrView::OnKorrozInfo()
{
    init_pts_list_table(REM_INDICATOR);
    regim = R_INIT;
    m_nRegim = ID_PTS_INFO;
}


void CGidrView::OnKorrozDel()
{
    init_pts_list_table(REM_INDICATOR);
    regim = R_INIT;
    m_nRegim = ID_PTS_DEL;
}

void CGidrView::OnAktDel()
{
    if (!init_pts_table("act")) {
        AfxMessageBox("Таблицы act не существует");
        return;
    }
    regim = R_INIT;
    m_nRegim = ID_PTS_DEL;
}

void CGidrView::OnAktInfo()
{
    if (!init_pts_table("act")) {
        AfxMessageBox("Таблицы act не существует");
        return;
    }

    regim = R_INIT;
    m_nRegim = ID_PTS_INFO;
}


void ispr_vyd(CAdoFile *ado, CGraph2 *graph, CString &q)
{
    CString q2 = "", q3 = "";

    if (graph->isMark()) {
        CString vyd;

        bool sort = true;

        if (create_vyd_line_table_new(ado, graph, vyd)) {
            q2.Format(" JOIN %s vyd ON vyd.id2=l.id", vyd);
            if (sort) {
                q3.Format(" ORDER BY t.orderID");
            }
        }
    }

    q.Replace("$j$", q2);
    q.Replace("$order$", q3);
}


void CGidrView::OnKorrozTable()
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(REM_INDICATOR);
    if (!kls) return;

    CAdoFile* ado = kls->m_ado;

    CString fStr;

    fStr.Format("%ssql\\objects\\defect\\indikator_korrozii.sql", argpath());
    ifstream f(fStr);
    if (f.good())
    {
        fStr.Format("objects\\defect\\indikator_korrozii");
        CString q = readQ(kls->m_ado, fStr, "", m_cxema->m_par);

        CGraph2 *graph = m_cxema->m_graph;

        q.Replace("$and_condition$", "");
//        q.Replace("$j$", "");

        ispr_vyd(ado, graph, q);

//        q.Replace("$j$", q2);
//        q.Replace("$order$", q3);

        CString s2015;
        s2015.Format("%d", m_sezon_korrozia);
        q.Replace("$YEAR1$", s2015);

        viewQ2(kls->m_ado, kls->m_database, q, kls->nazv, "", virt_data_geo, "", "");
    }
    else
    {
        AfxMessageBox("Нет файла indikator_po_godam.sql", MB_OK|MB_ICONINFORMATION);
    }
}

CString getParams(CAdoFile* ado, const char* tn);


void CGidrView::OnAktTable()
{
    Klassif* kls = m_geofile->m_kl_list->findKlN("act");
    if (!kls) return;

    CString q;
    CString params = getParams(kls->m_ado, kls->nazv);
    q.Format("SELECT %s FROM [%s]", params, kls->nazv);

    viewQ2(kls->m_ado, kls->m_database, q, kls->nazv, "", virt_data_geo, "", "");
}


int getGodQ(CWnd *wnd, CAdoFile *ado, const char *q, const char *capt)
{
//    CString q;
//    q.Format("SELECT DISTINCT YEAR(data_ustanovki) FROM indikator_korrozii_po_godam ORDER BY YEAR(data_ustanovki)");

//    CMMenu menu(this, IDD_MENU, _TR("Выберите год установки"));
    CMMenu menu(wnd, IDD_MENU, capt);

    long y = 2011;

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            CString s = ado->readStr(0);
            y = ado->read_long(0);
            if (y < 1900) {
                //                menu.Add(_TR("Без даты"), (void*)y);
            }
            else {
                menu.Add(s, (void*)y);
            }
            ado->MoveNext();
        }
        ado->closeTable();
    }

    int n = 0;

    if (menu.DoModal() != IDOK) return -1;

    long year = (long)menu.getV();

    return year;
}



int getSezonQ(CWnd *wnd, CAdoFile *ado, const char *q, const char *capt, bool first_year)
{
    CMMenu menu(wnd, IDD_MENU, capt);

    long y = 2011;

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            CString s = ado->readStr(0);
            y = ado->read_long(0);
            if (y < 1900) {
                //                menu.Add(_TR("Без даты"), (void*)y);
            }
            else {
                if (first_year) {
                    s.Format("%d %d", y, y + 1);
                }
                else {
                    s.Format("%d %d", y - 1, y);
                }
                menu.Add(s, (void*)y);
            }
            ado->MoveNext();
        }
        ado->closeTable();
    }

    int n = 0;

    if (menu.DoModal() != IDOK) return -1;

    long year = (long)menu.getV();

    return year;

}


#if 0

void CGidrView::OnKorrozTableGod()
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(REM_INDICATOR);
    if (!kls) return;


    CAdoFile* ado = kls->m_ado;

//    CString q = "SELECT DISTINCT YEAR(data_ustanovki) FROM indikator_korrozii_po_godam ORDER BY YEAR(data_ustanovki)";
//    int year = getGodQ(this, ado, q, _TR("Выберите год установки"));

    CString q = "SELECT DISTINCT YEAR(data_ustanovki) FROM indikator_korrozii_po_godam ORDER BY YEAR(data_ustanovki)";
    int year = getGodQ(this, ado, q, _TR("Выберите год установки"));

    if (year < 0) return;

    CString sy;
    sy.Format("%d", year);

    CString fStr;

    fStr.Format("%ssql\\objects\\defect\\indikator_po_godam.sql", argpath());
    ifstream f(fStr);
    if (f.good())
    {
        fStr.Format("objects\\defect\\indikator_po_godam");
        CString q = readQ(kls->m_ado, fStr, "", m_cxema->m_par);

        q.Replace("$YEAR$", sy);
        viewQ2(kls->m_database, q, kls->nazv, "", virt_data_geo);
    }
    else
    {
        AfxMessageBox("Нет файла indikator_po_godam.sql", MB_OK|MB_ICONINFORMATION);
    }
}

#endif


/**

void CGidrView::OnKorrozZhurnal()
{
//    Klassif* kls = m_geofile->m_kl_list->findKlN(REM_INDICATOR);
    Zhurnal(REM_INDICATOR, "objects\\defect\\indikator_korrozii");
}
*/

void CGidrView::OnElectroAdd()
{
//    Klassif *kls = menu_electro(this);
    Klassif *kls = menu_list_file(this, "Объекты электрической сети", "electro");

    if (kls) addKls(kls);
}

void CGidrView::OnElectroInfo()
{
    init_pts_list_file("electro");
    regim = R_INIT;
    m_nRegim = ID_PTS_INFO;
}

void CGidrView::OnElectroDel()
{
    init_pts_list_file("electro");
    regim = R_INIT;
    m_nRegim = ID_PTS_DEL;
}

void CGidrView::OnElectroTable()
{
//    Klassif *kls = menu_electro(this);
    Klassif *kls = menu_list_file(this, "Объекты электрических сетей", "electro");
    if (kls) RunGeoTable(kls);
}


void initPassportElectricalNetwork(CAdoFile* ado, HWND CWnd, CString argPath);
void CGidrView::OnElectroZhurnal()
{
//    Klassif *kls = menu_list_file(this, "Объекты электрических сетей", "electro");
    //electro_pas();
    initPassportElectricalNetwork(m_cxema->m_ado, GetSafeHwnd(), argpath());
}

CString getTabQ0(CAdoFile* ado, const char * tn);
CString getGeoQ(Klassif* kls);
CString getGeoQGraph(Klassif* kls, CGraph2* graph, bool sort);


void CGidrView::viewNizGeoQ(const char *tn, const char *typ_column, const char *date_column, bool is_line)
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);

    if (!kls) return;

    CAdoFile *ado = kls->m_ado;

    WebPageVirtualTable* a = (WebPageVirtualTable*)getDockablePane(IDD_PTS_VIRTUAL_TABLE_NIZ1);
    //CVirtualTable2 *a = (CVirtualTable2 *)getDockablePane(IDD_PTS_VIRTUAL_TABLE_NIZ);

    if (a) {
        kls->ins = 1;
        m_isGeobaza = TRUE;

        CString q;
        
        if (is_line) {
            q = getGeoQGraph(kls, m_cxema->m_graph, false);
        }
        else {
            q =  getTabQ0(ado, tn);
        }

        if (q != "") {
            if (date_column && date_column[0]) {
                CRemontZapros2 dlg(this);

                if (typ_column && typ_column[0]) {
                    ForeignTable ft = findLookup(ado->m_schema, tn, typ_column);
                    if (ft.table != "") {
                        dlg.setTab(ado, ft.table);
                    }
                }

                if (dlg.DoModal() == IDOK) {
                    CString qq;

                    CString ds1 = dlg.m_date1.Format(_T("%Y%m%d"));
                    CString ds2 = dlg.m_date2.Format(_T("%Y%m%d"));

                    int typ = dlg.m_type1;

                    qq.Format("SELECT * FROM (%s\n) _QWEQWWEE WHERE _TAB1____%s BETWEEN '%s' AND '%s' ", q, date_column, ds1, ds2);

                    if (typ > 0) {
                        CString ss;
                        ss.Format(" AND _TAB1____%s=%d", typ_column, typ);
                        qq += ss;
                    }


                    m_geofile->setVisibleAll(tn);
                    m_geofile->setVisible(kls, qq);
                    
                    RunTableNiz(ado, tn, qq, kls->RusName(), true, true, virt_data_geo);
                }
            }
            else {
                RunTableNiz(ado, tn, q, kls->RusName(), true, true, virt_data_geo);
            }
        }
        Invalidate();
    }
}


CString get_color_q(CString q, int sost)
{
    CString q_color = "";

    switch(sost) {
        case 1:
    q_color.Format(

    "SELECT top 2147483647\n"
    "id, 255 AS __color \n"

    "FROM (%s\n) _QQQQ\n", q);
    break;
            break;
        case 2:
    q_color.Format(

    "SELECT top 2147483647\n"
    "id,\n"
//    "convert(int, convert(varbinary, '0xB5D9FD', 1)) AS __color\n"
"CASE \n"
"WHEN \n"
"[Дата установки] <> '-'\n"
"THEN 16711680\n"
"ELSE 255\n"
"END AS __color\n"

    "FROM (%s\n) _QQQQ\n", q);
    break;
            break;
        case 3:
    q_color.Format(

    "SELECT top 2147483647\n"
    "id,\n"
//    "convert(int, convert(varbinary, '0xB5D9FD', 1)) AS __color\n"

"CASE \n"
"WHEN [Дата извлечения] <> '-' THEN 65280\n"
"WHEN [Дата установки] <> '-' THEN 16711680\n"
"ELSE 255\n"
"END AS __color\n"

    "FROM (%s\n) _QQQQ\n", q);
    break;
            break;
        case -1:



    q_color.Format(

    "SELECT top 2147483647\n"
    "id,\n"
//    "convert(int, convert(varbinary, '0xB5D9FD', 1)) AS __color\n"

"CASE \n"
"WHEN \n"
"[Дата планирования] <> '-'\n"
"AND [Дата установки] = '-'\n"
"AND [Дата извлечения] = '-'\n"
"THEN 255\n"
"WHEN \n"
"[Дата установки] <> '-'\n"
"AND [Дата извлечения] = '-'\n"
"THEN 16711680\n"
"WHEN \n"
"[Дата извлечения] <> '-'\n"
"THEN 65280\n"
"END AS __color\n"

    "FROM (%s\n) _QQQQ\n", q);
    break;
    }

    return q_color;
}


void CGidrView::ZhurnalQ(const char *tn0, const char *q, const char *title0, int sost)
{
    CString tn = tn0;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);

    if (!kls) return;

    CString title = kls->RusName();
    if (title0 && title0[0]) title = title0;

    kls->ins = 1;
    m_isGeobaza = TRUE;

    CString q_color = get_color_q(q, sost);

    if (q_color != "") {
        m_geofile->setVisibleColor(kls, q_color);
    }

    Invalidate();

    RunTableNiz(kls->m_ado, tn, q, title, true, false, virt_data_geo);
}

// Журналы без разбивки по цветам


void CGidrView::ZhurnalQ2(const char *tn0, const char *q, const char *title0, bool reset_visible, VIRT_DATA typ)
{
    CString tn = tn0;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);

    if (!kls) return;

    CString title = kls->RusName();
    if (title0 && title0[0]) title = title0;

    kls->ins = 1;
    m_isGeobaza = TRUE;

    RunTableNiz(kls->m_ado, tn, q, title, true, false, typ);

    if (reset_visible) {
      m_geofile->setVisibleAll(tn);
      m_geofile->setVisible(kls, q);
    }

    Invalidate();

}


#include "InputDialogDate.h"

void CGidrView::OnPopupKorroziaPlan()
{
    if (!isEditGeo(REM_INDICATOR)) return;
    if (isPot()) return;
    if (!m_geofile) return;

    Klassif* kls = m_geofile->m_kl_list->findKlN(REM_INDICATOR);
    if (!kls) return;

    int id = m_geofile->findXY3(m_menu_pt, D5 * geom.masx, 1, REM_INDICATOR);
    if (!id) return;

    CGeoObject *geo = kls->getGeoObjectById(id);
    
    if (geo->cl.color != getGeoColor(4)) {
        AfxMessageBox("Индикатор коррозии должен быть архивным", MB_OK|MB_ICONINFORMATION);
        return;
    }

    CInputDialogDate dlg(this, "", "Введите дату планирования");

    if (dlg.DoModal() == IDOK) {
        CString q;

        q.Format(
           "UPDATE obj\n"
           "set obj.sostoyanie = 1,\n"
           "obj.data_planirovaniya = '%s'\n"
           "from indikator_korrozii obj\n"
           "where obj.id = %d", dlg.m_date1.Format("%Y%m%d"), id);

        if (!kls->m_ado->Execute(q)) {
            AfxMessageBox(kls->m_ado->getError(), MB_OK|MB_ICONINFORMATION);
        }
        else {
            geo->cl.color = getGeoColor(1);
            Invalidate();
        }
    }
}


void CGidrView::OnZhurnalKorroziaGod()
{
    CString tn = REM_INDICATOR;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);

    if (!kls) return;

//    CString q = "SELECT DISTINCT YEAR(data_ustanovki) FROM indikator_korrozii_po_godam ORDER BY YEAR(data_ustanovki)";
//    int year = getGodQ(this, kls->m_ado, q, _TR("Выберите год установки"));

    CString q = "SELECT DISTINCT YEAR(data_ustanovki) FROM indikator_korrozii_po_godam ORDER BY YEAR(data_ustanovki)";
    int year = getGodQ(this, kls->m_ado, q, _TR("Выберите год установки"));


    if (year < 0) return;

    CString sy;
    sy.Format("%d", year);

    CString fn;
    fn = "objects\\defect\\indikator_po_godam";

    q = readQ(kls->m_ado, fn, "", m_cxema->m_par);
    q.Replace("$YEAR$", sy);

    CString s;

    s.Format("Индикаторы коррозии за %d год", year);


    ZhurnalQ(tn, q, s, -1);

    m_zhurnal = ID_KORROZIA_GOD;
}



void CGidrView::OnZhurnalKorroziaCurrent()
{
    CString tn = REM_INDICATOR;

    if (m_sezon_korrozia < 0) {
        setKorroziaSezonDlg();
        if (m_sezon_korrozia < 0) {
            return;
        }
    }


    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);

    if (!kls) return;
    
    CString fn;
    fn = "objects\\defect\\indikator_korrozii";

    CString qq = readQ(kls->m_ado, fn, "", m_cxema->m_par);


//    qq.Replace("$and_condition$"," AND (sostoyanie <> 4)");
    qq.Replace("$and_condition$","");

//    qq.Replace("$j$", "");
//    qq.Replace("$order$", "");

    ispr_vyd(kls->m_ado, m_cxema->m_graph, qq);


    CString s2015;
    s2015.Format("%d", m_sezon_korrozia);
    qq.Replace("$YEAR1$", s2015);

    m_zhurnal = ID_KORROZIA_CURRENT;

    CString s;
    s.Format("Индикаторы коррозии на %d-%d отопительный сезон", m_sezon_korrozia, m_sezon_korrozia+1);

    ZhurnalQ(tn, qq, s, -1);
}

void CGidrView::KorroziaZhurnal(int sost, const char *title)
{
    if (m_sezon_korrozia < 0) {
        setKorroziaSezonDlg();
        if (m_sezon_korrozia < 0) {
            return;
        }
    }

    CString tn = REM_INDICATOR;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);

    if (!kls) return;

    kls->ins = 1;
    m_isGeobaza = TRUE;
    
    CString fn;
    fn = "objects\\defect\\indikator_korrozii";

    CString qq = readQ(kls->m_ado, fn, "", m_cxema->m_par);

    CString qqq = "";

    switch(sost) {
        case 1:
            qqq = "";
            break;
        case 2:
            qqq = " AND obj_godam.sostoyanie IN (2,3)";
            break;
        case 3:
            qqq = " AND obj_godam.sostoyanie = 3";
            break;
    }


    qq.Replace("$and_condition$", qqq);
//    qq.Replace("$and_condition$", "");
//    qq.Replace("$j$", "");
//    qq.Replace("$order$", "");

    ispr_vyd(kls->m_ado, m_cxema->m_graph, qq);

    CString s2015;
    s2015.Format("%d", m_sezon_korrozia);
    qq.Replace("$YEAR1$", s2015);

    ZhurnalQ(tn, qq, title,sost);
}

int getCurYear()
{
    COleDateTime date = COleDateTime::GetCurrentTime();
    return date.GetYear();
}


void CGidrView::OnKorroziaZhurnal1()
{

    CString s;
    s.Format("Индикаторы коррозии планируемые на %d-%d отопительный сезон", m_sezon_korrozia, m_sezon_korrozia+1);

    KorroziaZhurnal(1, s);
    m_zhurnal = ID_KORROZIA_ZHURNAL1;
}
 
void CGidrView::OnKorroziaZhurnal2() 
{ 
    CString s;
    s.Format("Индикаторы коррозии анализ на %d-%d отопительный сезон", m_sezon_korrozia, m_sezon_korrozia+1); 

    KorroziaZhurnal(2, s); 
    m_zhurnal = ID_KORROZIA_ZHURNAL2; 
}

void CGidrView::OnKorroziaZhurnal3() 
{ 
    CString s;
    s.Format("Индикаторы коррозии после расчета на %d-%d отопительный сезон", m_sezon_korrozia, m_sezon_korrozia+1); 

    KorroziaZhurnal(3, s); 
    m_zhurnal = ID_KORROZIA_ZHURNAL3; 
} 

void CGidrView::OnKorroziaZhurnal4() 
{   
    CString s;
    s.Format("Индикаторы коррозии архивные на %d-%d отопительный сезон", m_sezon_korrozia, m_sezon_korrozia+1); 

    KorroziaZhurnal(4, s); 
    m_zhurnal = ID_KORROZIA_ZHURNAL4; 
}

void CGidrView::OnZhurnalKorroziaAll()
{
    CString tn = REM_INDICATOR;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);

    if (!kls) return;
    
    CString fn;
    fn = "objects\\defect\\indikator_korrozii";

    CString qq = readQ(kls->m_ado, fn, "", m_cxema->m_par);

    qq.Replace("$and_condition$","");
//    qq.Replace("$j$", "");
//    qq.Replace("$order$", "");

    ispr_vyd(kls->m_ado, m_cxema->m_graph, qq);


    m_zhurnal = ID_KORROZIA_ALL;

    ZhurnalQ(tn, qq, "Индикаторы коррозии все", -1);
}

void CGidrView::OnUpdateZhurnal(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_zhurnal == pCmdUI->m_nID);
}


void CGidrView::Zhurnal(const char *tn0, const char *fStr0, const char *typ_column, const char *date_column, bool is_line, const char *title0)
{
    CString tn = tn0;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);

    if (!kls) return;

    CString title = kls->RusName();
    if (title0 && title0[0]) title = title0;

    CAdoFile* ado = kls->m_ado;

//    CString fStr = "objects\\defect\\defect";
    CString fStr = fStr0;

    CString qq = readQ(kls->m_ado, fStr, "", m_cxema->m_par);

    CGraph2 *graph = m_cxema->m_graph;

    CString q2 = "", q3 = "";

    CRemontZapros2 dlg(this);

//    dlg.setTab(kls->m_ado, tn);

    if (typ_column && typ_column[0]) {
        ForeignTable ft = findLookup(ado->m_schema, tn, typ_column);
        if (ft.table != "") {
            dlg.setTab(ado, ft.table);
        }
    }

    int ret = dlg.DoModal();

    if (ret == IDOK) {
        if (graph->isMark()) {
            CString vyd;

            bool sort = true;

            if (create_vyd_line_table_new(ado, graph, vyd)) {
                q2.Format(" JOIN %s vyd ON vyd.id2=l.id", vyd);
                if (sort) {
                    q3.Format(" ORDER BY t.orderID");
                }
            }
        }

        qq.Replace("$j$", q2);
        qq.Replace("$order$", q3);

        kls->ins = 1;
        m_isGeobaza = TRUE;

        CString q4;

        CString ds1 = dlg.m_date1.Format(_T("%Y%m%d"));
        CString ds2 = dlg.m_date2.Format(_T("%Y%m%d"));

        int typ = dlg.m_type1;
        CString qc1 = "", qc2 = "";


        if (typ_column[0] && typ > 0) {
           qc1.Format("([%s] = %d)", typ_column, typ);
           q4 += qc1;
        }

        if (date_column[0]) {
           qc2.Format("([%s] BETWEEN '%s' AND '%s')", date_column, ds1, ds2);
           if (q4 != "") q4 += " AND ";
           q4 += qc2;
        }

        if (q4 != "") q4 = "AND ("+q4+")";

        qq.Replace("$and_condition$", q4);


        m_geofile->setVisible(kls, qq);

        kls->ins = 1;
        m_isGeobaza = TRUE;
        m_geofile->m_kl_list->save2();

        Invalidate();

        RunTableNiz(ado, tn, qq, title, true, false, virt_data_geo);
    }
}


void CGidrView::ZhurnalK(const char *tn0, const char *fStr0, const char *typ_column, const char *date_column, bool is_line, const char *title0)
{
    CString tn = tn0;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);

    if (!kls) return;

    CString title = kls->RusName();
    if (title0 && title0[0]) title = title0;

    CAdoFile* ado = kls->m_ado;

//    CString fStr = "objects\\defect\\defect";
    CString fStr = fStr0;

    CString qq = readQ(kls->m_ado, fStr, "", m_cxema->m_par);

    CGraph2 *graph = m_cxema->m_graph;

    CString q2 = "", q3 = "";

    CRemontZapros2 dlg(this);

//    dlg.setTab(kls->m_ado, tn);

    if (typ_column && typ_column[0]) {
        ForeignTable ft = findLookup(ado->m_schema, tn, typ_column);
        if (ft.table != "") {
            dlg.setTab(ado, ft.table);
        }
    }

    int ret = dlg.DoModal();

    if (ret == IDOK) {
        if (graph->isMark()) {
            CString vyd;

            bool sort = true;

            if (create_vyd_line_table_new(ado, graph, vyd)) {
                q2.Format(" JOIN %s vyd ON vyd.id2=l.id", vyd);
                if (sort) {
                    q3.Format(" ORDER BY t.orderID");
                }
            }
        }

        qq.Replace("$j$", q2);
        qq.Replace("$order$", q3);

        kls->ins = 1;
        m_isGeobaza = TRUE;

        CString q4;

        CString ds1 = dlg.m_date1.Format(_T("%Y%m%d"));
        CString ds2 = dlg.m_date2.Format(_T("%Y%m%d"));

        int typ = dlg.m_type1;
        CString qc1 = "", qc2 = "";


        if (typ_column[0] && typ > 0) {
           qc1.Format("([%s] = %d)", typ_column, typ);
           q4 += qc1;
        }

        if (date_column[0]) {
           qc2.Format("([%s] BETWEEN '%s' AND '%s')", date_column, ds1, ds2);
           if (q4 != "") q4 += " AND ";
           q4 += qc2;
        }

        if (q4 != "") q4 = "AND ("+q4+")";

        qq.Replace("$and_condition$", q4);

        CString s2015;
        s2015.Format("%d", m_sezon_korrozia);

        qq.Replace("$YEAR1$", s2015);

        CString q_color = get_color_q(qq, -1);

        m_geofile->setVisibleColor(kls, q_color);

        kls->ins = 1;
        m_isGeobaza = TRUE;
        m_geofile->m_kl_list->save2();

        Invalidate();

        RunTableNiz(ado, tn, qq, title, true, false, virt_data_geo);
    }
}




void CGidrView::OnZhurnalDefect()
{
    Zhurnal("defect", "objects\\defect\\defect", "stateID", "data_osmotra", true, "");
    m_zhurnal = ID_ZHURNAL_DEFECT;
}

void CGidrView::OnZhurnalDiag()
{
    Zhurnal("diag", "objects\\defect\\diag", "sredstvoDiagID", "data_prov", true, "");
    m_zhurnal = ID_ZHURNAL_DIAG;
//    viewNizGeoQ("diag", "sredstvoDiagID", "data_prov", true);
}

void CGidrView::OnZhurnalRemont()
{
    Zhurnal("remont", "objects\\defect\\remont", "stateID", "data_osmotra", true, "");
    m_zhurnal = ID_ZHURNAL_REMONT;
//    viewNizGeoQ("remont", "stateID", "data_osmotra", true);
}



void CGidrView::OnZhurnalElectro()
{
//    Zhurnal("liniya_elektroperedach", "objects\liniya_elektroperedach", "", "", false);
    CString tn = "liniya_elektroperedach";
    CString fStr = "objects\\liniya_elektroperedach";


    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);

    if (!kls) return;

    CAdoFile* ado = kls->m_ado;

    CString qq = readQ(kls->m_ado, fStr, "", m_cxema->m_par);
    RunTableNiz(ado, tn, qq, kls->RusName(), true, false, virt_data_geo);
    m_zhurnal = ID_ZHURNAL_ELECTRO;

    return;


//    viewNizGeoQ("liniya_elektroperedach", "", "", false);

#if 0

    CString tn = "liniya_elektroperedach";
    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);

    if (!kls) return;

    CAdoFile* ado = kls->m_ado;

    CString fStr = "objects\\liniya_elektroperedach";

    CString qq = readQ(kls->m_ado, fStr, "", 0);

    RunTableNiz(ado, tn, qq, kls->RusName(), true, false);
#endif

}

void CGidrView::OnZhurnalKorroz()
{
//    viewNizGeoQ(REM_INDICATOR, "sostoyanie", "data_ustanovki", true);
    Zhurnal(REM_INDICATOR, "objects\\defect\\indikator_korrozii", "sostoyanie", "", true, "Журнал индикаторов коррозии");
    m_zhurnal = ID_ZHURNAL_KORROZ;
}


#include "dialog_pr.h"

#include "CKorroziaZapros.h"

void setIndikator(int sost)
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(REM_INDICATOR);
    if (!kls) return;

    CAdoFile* ado = kls->m_ado;
    
    CString q;

    set<int> set_id;

    q.Format("SELECT id FROM indikator_korrozii WHERE sostoyanie = %d", sost);

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long("id");
            set_id.insert(id);

            ado->MoveNext();
        }
        ado->closeTable();
    }

    int i = 0, n = kls->geo4.size();

    for (auto it : set_id) {
        long id = it;

        for (; i < n && kls->geo4[i]->nom < id; i++) {
        }
        if (kls->geo4[i]->nom == id) {
            kls->geo4[i]->cl.color = getGeoColor(1);
        }
    }
}


void CGidrView::OnKorroziaUpdate()
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(REM_INDICATOR);
    if (!kls) return;

    CAdoFile* ado = kls->m_ado;

    CKorroziaZapros dlg(this, m_sezon_korrozia);

//    CString q = "SELECT DISTINCT YEAR(data_ustanovki) FROM indikator_korrozii_po_godam WHERE data_ustanovki <> '' AND NOT data_ustanovki IS NULL ORDER BY YEAR(data_ustanovki)";
    CString q = "SELECT DISTINCT CONCAT(YEAR(data_planirovaniya),'-',YEAR(data_planirovaniya)+1) AS y, YEAR(data_planirovaniya) AS y1 FROM indikator_korrozii_po_godam WHERE YEAR(data_planirovaniya) > 1980 ORDER BY y ";
//    CString q = "SELECT CONCAT(YEAR(date1), '-', YEAR(date2)) AS y, date1, date2 FROM heating_seasons ORDER BY y";

    dlg.m_ado = ado;
    dlg.m_q = q;

    if (dlg.DoModal() == IDOK) {
        CString q;
        CString sd;
        CString msg;

        if (dlg.m_year =="") {
            AfxMessageBox("Выберите отопительный сезон", MB_OK|MB_ICONINFORMATION);
            return;
        }

        msg.Format("Вы действительно хотите сделать все Индикаторы коррозии за %s отопительный сезон плановыми?", dlg.m_year);

#if 0
        q.Format(
       "UPDATE obj\n"
       "set obj.sostoyanie = 1,\n"
       "obj.data_planirovaniya = '%s'\n"
       "from indikator_korrozii obj\n"
       "JOIN indikator_korrozii_po_godam obj_godam on obj_godam.id_i = obj.id\n"
//       "where obj.sostoyanie = 4 and YEAR(obj_godam.data_ustanovki) = %s \n", dlg.m_date_plan.Format("%Y%m%d"), dlg.m_year);
       "where (YEAR(obj_godam.data_ustanovki) = %s OR YEAR(obj_godam.data_planirovaniya) = %s) \n", dlg.m_date_plan.Format("%Y%m%d"), dlg.m_year1, dlg.m_year1);

        q.Format(
            "UPDATE obj\n"
            "set obj.sostoyanie = 1,\n"
            "obj.data_planirovaniya = '%s'\n"

            "FROM indikator_korrozii obj\n"
            "JOIN indikator_korrozii_po_godam g1 ON obj.id = g1.id_i\n"
            "AND (YEAR(g1.data_planirovaniya) = %d OR YEAR(g1.data_ustanovki) = %d) \n"
            "LEFT JOIN indikator_korrozii_po_godam g2 ON obj.id = g1.id_i\n"
            "AND (YEAR(g2.data_planirovaniya) = %d OR YEAR(g2.data_ustanovki) = %d) \n"
            "WHERE g2.id IS NULL\n"
            , dlg.m_date_plan.Format("%Y%m%d"), dlg.m_year1, dlg.m_year1, dlg.m_year1, dlg.m_year1);
#endif

            int y1 = atoi(dlg.m_year1);
            int y2 = dlg.m_date_plan.GetYear();

q.Format(

"INSERT INTO indikator_korrozii_po_godam(id_i, data_planirovaniya, data_ustanovki, data_izvlecheniya, sredniy_ves_plastiny_pri_ustanovke__g,"
"poterya_massy_srednyaya_pri_kislotnoy_obraboke__g,\n"
"srednyaya_skorost_korrozii__mm_god,\n"
"otsenka_korrozionnogo_protsessa,\n"
"agressivnost_setevoy_vody,\n"
"sostoyanie,\n"

"nomer_indikatora_korrozii,\n"
"truboprovod,\n"
"kolichestvo_plastin_v_sborke,\n"
"radius_krugloy_plastiny__mm,\n"
"radius_vtulki__mm,\n"
"tolschina_plastiny__mm,\n"
"kolichestvo_dney_ispytaniy\n"
//"sredniy_ves_plastiny_posle_ispytaniy__g\n"

")\n"

"SELECT i1.id_i, '%s', NULL, NULL, NULL,NULL,NULL,NULL,NULL,1,"

"i1.nomer_indikatora_korrozii,\n"
"i1.truboprovod,\n"
"i1.kolichestvo_plastin_v_sborke,\n"
"i1.radius_krugloy_plastiny__mm,\n"
"i1.radius_vtulki__mm,\n"
"i1.tolschina_plastiny__mm,\n"
"i1.kolichestvo_dney_ispytaniy\n"
//"i1.sredniy_ves_plastiny_posle_ispytaniy__g\n"


"FROM indikator_korrozii_po_godam i1\n"
"LEFT JOIN indikator_korrozii_po_godam i2 ON  i1.id_i = i2.id_i  AND (YEAR(i2.data_planirovaniya) = %d OR YEAR(i2.data_ustanovki) = %d)\n"
"WHERE\n"
"(YEAR(i1.data_planirovaniya) = %d OR YEAR(i1.data_ustanovki) = %d) AND i2.id IS NULL\n"

, dlg.m_date_plan.Format("%Y%m%d"), y2, y2, y1, y1);

        CString q2;
        int yn = dlg.m_date_plan.GetYear();

       if (AfxMessageBox(msg, MB_YESNO) == IDYES) {
            if (!ado->Execute(q)) {
                AfxMessageBox(ado->getError(), MB_OK|MB_ICONINFORMATION);
            }
            else {
                setKorroziaSezon(y2, true);

        q2.Format(
"IF NOT EXISTS (SELECT id FROM heating_seasons WHERE YEAR(date1)=%d)\n"    
"BEGIN\n"
"INSERT INTO heating_seasons (date1, date2) VALUES ('%d1015','%d0415')\n"
"END", yn, yn, yn+1);
            if (!ado->Execute(q2)) {
                AfxMessageBox(ado->getError(), MB_OK|MB_ICONINFORMATION);
            }

                Invalidate();

                AfxMessageBox("Выполнено", MB_OK|MB_ICONINFORMATION);
            }
        }
    }

//    CPropGridDlg* pm_cEditDlg = dialog_pr(this, "korroz1", WM_KORROZIA_UPDATED, _TR("Коррозия"), -1);

//    if (pm_cEditDlg) {
//        pm_cEditDlg->m_ctrlGrid.SetNameValueInit("TG1", "60");
//    }
}

LRESULT CGidrView::OnKorroziaUpdate2(WPARAM wParam, LPARAM lParam)
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(REM_INDICATOR);
    if (!kls) return 0;

    CAdoFile* ado = kls->m_ado;

    CString s_data1, s_year;

    getPropValue(WM_KORROZIA_UPDATED, "data1", s_data1);
    getPropValue(WM_KORROZIA_UPDATED, "year", s_year);

    CString q;
    CString sd;

    q.Format(

   "UPDATE obj\n"
   "set obj.sostoyanie = 1,\n"
   "obj.data_planirovaniya = '%s'\n"
   "from indikator_korrozii obj\n"
   "JOIN indikator_korrozii_po_godam obj_godam on obj_godam.id_i = obj.id\n"
//   "where obj.sostoyanie = 4 and YEAR(obj_godam.data_ustanovki) = %s \n", s_data1, s_year);
   "where obj.sostoyanie = 4 and YEAR(obj_godam.data_ustanovki) = %s \n", s_data1, s_year);

    if (!ado->Execute(q)) {
    }

    set<int> set_id;

    q.Format("SELECT id FROM indikator_korrozii WHERE sostoyanie = 1");

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long("id");
            set_id.insert(id);

            ado->MoveNext();
        }
        ado->closeTable();
    }

    int i = 0, n = kls->geo4.size();

    for (auto it : set_id) {
        long id = it;

        for (; i < n && kls->geo4[i]->nom < id; i++) {
        }
        if (kls->geo4[i]->nom == id) {
            kls->geo4[i]->cl.color = getGeoColor(1);
        }
    }

    Invalidate();

    return 0;
}

#include "Filtr3.h"

void set_left_geo()
{
    CLeft1Dialog* left1 = (CLeft1Dialog*)getDockablePane(IDD_LEFT1);
    if (left1) {
        left1->setKlsOnOff();
    }
}



void CGidrView::OnRemontOnOff()
//void CGidrView::OnRemontPovrDefOtobr()
{
    CFiltr3 menu(this, "Выберите объекты ремонтов");

    init_pts_list_file("remont");

    for (auto it : map_pts) {
        Klassif* kls = it.second;
        menu.Add(it.first, (int) kls, kls->ins);
    }

    if (menu.DoModal() == IDOK) {

        for (auto it : menu.map_item) {
            Klassif *kls = (Klassif *) it.second.id;
            kls->ins = it.second.on;
        }

        set_left_geo();

        m_geofile->m_kl_list->save2();
        setKorroziaSezon(m_sezon_korrozia, true);

        Invalidate();
    }
}


void CGidrView::KorroziaOnOff(bool yes)
{

    Klassif *kls = m_geofile->m_kl_list->findKlN(REM_INDICATOR);

    if (kls) {
        kls->ins = yes;
        m_geofile->m_kl_list->save2();
        Invalidate();
    }



    CFiltr3 menu(this, "Выберите объекты Индикаторов Коррозии");

    init_pts_list_file("korrozia");

    for (auto it : map_pts) {
        Klassif* kls = it.second;
        menu.Add(it.first, (int) kls, kls->ins);
    }

    if (menu.DoModal() == IDOK) {
        for (auto it : menu.map_item) {
            Klassif *kls = (Klassif *) it.second.id;
            kls->ins = it.second.on;
        }
        m_geofile->m_kl_list->save2();
        Invalidate();
    }
}

void CGidrView::OnKorroziaOnOff()
{
    CFiltr3 menu(this, "Выберите объекты Индикаторов Коррозии");

    init_pts_list_file("korrozia");

    for (auto it : map_pts) {
        Klassif* kls = it.second;
        menu.Add(it.first, (int) kls, kls->ins);
    }

    if (menu.DoModal() == IDOK) {
        for (auto it : menu.map_item) {
            Klassif *kls = (Klassif *) it.second.id;
            kls->ins = it.second.on;
        }
        m_geofile->m_kl_list->save2();
        Invalidate();
    }
}

void CGidrView::OnUpdateKorroziaOnOff(CCmdUI* pCmdUI)
{
    Klassif *kls = m_geofile->m_kl_list->findKlN(REM_INDICATOR);

    if (kls) {
        pCmdUI->SetCheck(kls->ins);
    }
    else {
        pCmdUI->SetCheck(FALSE);
    }
}

void CGidrView::OnAktOnOff()
{
    CFiltr3 menu(this, "Выберите объекты Индикаторов Коррозии");

    init_pts_list_file("act");

    for (auto it : map_pts) {
        Klassif* kls = it.second;
        menu.Add(it.first, (int) kls, kls->ins);
    }

    if (menu.DoModal() == IDOK) {
        for (auto it : menu.map_item) {
            Klassif *kls = (Klassif *) it.second.id;
            kls->ins = it.second.on;
        }
        m_geofile->m_kl_list->save2();
        Invalidate();
    }
}

void CGidrView::OnUpdateAktOnOff(CCmdUI* pCmdUI)
{
    Klassif *kls = m_geofile->m_kl_list->findKlN("act");

    if (kls) {
        pCmdUI->SetCheck(kls->ins);
    }
    else {
        pCmdUI->SetCheck(FALSE);
    }
}



void CGidrView::OnElectroOnOff()
{
    CFiltr3 menu(this, "Выберите объекты электроснабжения");

    init_pts_list_file("electro");

    for (auto it : map_pts) {
        Klassif* kls = it.second;
        menu.Add(it.first, (int) kls, kls->ins);
    }

    if (menu.DoModal() == IDOK) {
        for (auto it : menu.map_item) {
            Klassif *kls = (Klassif *) it.second.id;
            kls->ins = it.second.on;
        }
        m_geofile->m_kl_list->save2();
        Invalidate();
    }
}


void CGidrView::OnGeoOnOff()
{
     CMainFrame* mf = (CMainFrame*)AfxGetMainWnd();
     mf->LeftBar(true);
     CLeft1Dialog *a = (CLeft1Dialog *)getDockablePane(IDD_LEFT1);
     if (a) {
         BOOL delay = TRUE;
         a->ShowPane(TRUE, delay, TRUE);
     }
/*


    CFiltr3 menu(this, "Выберите объекты Геобазы");

    for (auto & it : *m_geofile->m_kl_list) {
        Klassif* kls = &it.second;
        if (!ado_equal(kls->m_ado,m_cxema->m_ado)) {
            menu.Add(kls->RusName(), (int) kls, kls->ins);
        }
    }

    if (menu.map_item.size() == 0) {
        AfxMessageBox("Карта города не подключена");
        return;
    }


    if (menu.DoModal() == IDOK) {
        for (auto it : menu.map_item) {
            Klassif *kls = (Klassif *) it.second.id;
            kls->ins = it.second.on;
        }
        m_geofile->m_kl_list->save2();
        Invalidate();
    }
*/
}



void CGidrView::OnPassportOnOff()
{
    CFiltr3 menu(this, "Выберите объекты ПТС");

    init_pts_list_file("pts");

    for (auto it : map_pts) {
        Klassif* kls = it.second;
        menu.Add(it.first, (int) kls, kls->ins);
    }

    if (menu.DoModal() == IDOK) {
        for (auto it : menu.map_item) {
            Klassif *kls = (Klassif *) it.second.id;
            kls->ins = it.second.on;
        }
        m_geofile->m_kl_list->save2();
        Invalidate();
    }
}


void CGidrView::OnUpdateZhurnalExcel(CCmdUI* pCmdUI)
{
    //CVirtualTable2 *a = (CVirtualTable2 *)getDockablePane(IDD_PTS_VIRTUAL_TABLE_NIZ);
    WebPageVirtualTable* a = (WebPageVirtualTable*)getDockablePane(IDD_PTS_VIRTUAL_TABLE_NIZ);

    if (a) {
        pCmdUI->Enable(a->IsVisible());
    }
    else {
        pCmdUI->Enable(FALSE);
    }
}


void CGidrView::OnZhurnalExcel()
{

    //CVirtualTable2 *a = (CVirtualTable2 *)getDockablePane(IDD_PTS_VIRTUAL_TABLE_NIZ);
    WebPageVirtualTable* a = (WebPageVirtualTable*)getDockablePane(IDD_PTS_VIRTUAL_TABLE_NIZ);
    if (a) {
        //a->Excel();
    }
}

// Пересчитать журналы

void CGidrView::resetZhurnal(bool is_reset_visible)
{
m_is_reset_visible = is_reset_visible;

    switch(m_zhurnal) {
    case ID_KORROZIA_GOD:       OnZhurnalKorroziaGod(); break;
    case ID_KORROZIA_CURRENT:   OnZhurnalKorroziaCurrent(); break;
    case ID_KORROZIA_ZHURNAL1:  OnKorroziaZhurnal1(); break;
    case ID_KORROZIA_ZHURNAL2:  OnKorroziaZhurnal2(); break; 
    case ID_KORROZIA_ZHURNAL3:  OnKorroziaZhurnal3(); break; 
    case ID_KORROZIA_ZHURNAL4:  OnKorroziaZhurnal4(); break; 
    case ID_KORROZIA_ALL:       OnZhurnalKorroziaAll(); break;
    case ID_ZHURNAL_DEFECT:     OnZhurnalDefect(); break;
    case ID_ZHURNAL_DIAG:       OnZhurnalDiag(); break;
    case ID_ZHURNAL_REMONT:     OnZhurnalRemont(); break;
    case ID_ZHURNAL_ELECTRO:    OnZhurnalElectro(); break;
    case ID_ZHURNAL_KORROZ:     OnZhurnalKorroz(); break;
    }


    if (m_zhurnal == ID_REMONT_POVR_DEF_ZHUR) {
//        if (isOpenZhur()) OnRemontPovrDefZhur();
//        if (isOpenZhur()) OnRemontPovrDefZhurRemont();
//        if (isOpenZhur()) OnRemontShurfPlan();


if (isOpenZhur(1)) OnRemontShurfPlan();
if (isOpenZhur(2)) OnRemontShurfPredpis();
if (isOpenZhur(3)) OnRemontShurfNarush();
if (isOpenZhur(4)) OnRemontShurfZhurnal();
/*
if (isOpenZhur(5)) OnRemontPovrDefZhur();
if (isOpenZhur(6)) OnRemontPovrDefZhur1();
if (isOpenZhur(7)) OnRemontPovrDefZhur2();
if (isOpenZhur(8)) OnRemontPovrDefZhur3();
if (isOpenZhur(9)) OnRemontPovrDefZhurRemont();
*/

if (isOpenZhur(10)) OnShurfAll();
if (isOpenZhur(11)) OnShurfProcess();
if (isOpenZhur(12)) OnShurfVypolneno();

if (isOpenZhur(20)) OnDefectZhurnalExpluatacia();
if (isOpenZhur(21)) OnDefectZhurnalShurf();
if (isOpenZhur(22)) OnDefectZhurnalOsmotr();
if (isOpenZhur(23)) OnDefectZhurnalOpress();
if (isOpenZhur(24)) OnDefectZhurnalNarushenie();
if (isOpenZhur(25)) OnDefectZhurnalRemont();
if (isOpenZhur(26)) OnDefectZhurnalPosleremont();
if (isOpenZhur(27)) OnDefectZhurnalAll();

if (isOpenZhur(51)) OnOpresZdAll();



    }
m_is_reset_visible = true;
}

/**

void CGidrView::OnRemontShurfOtobr()
{
    CFiltr3 menu(this, "Выберите объекты Шурфовки");

    init_pts_list_file("shurfy");

    for (auto it : map_pts) {
        Klassif* kls = it.second;
        menu.Add(it.first, (int) kls, kls->ins);
    }

    if (menu.DoModal()) {
        for (auto it : menu.map_item) {
            Klassif *kls = (Klassif *) it.second.id;
            kls->ins = it.second.on;
        }
        m_geofile->m_kl_list->save2();
        Invalidate();
    }
}

*/