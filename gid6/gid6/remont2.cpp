#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
#include "gidrView.h"

#include "geofile.h"

#include "CTeploset.h"

#include "InputDia.h"
#include "Filtr3.h"
#include "ado.h"
#include "MMenu.h"
#include "mmenu2.h"

#include "InputDialogDate.h"

static int m_colorID1 = 1;
static int m_colorID2 = 1;

void set_colorID1(int colorID1) { m_colorID1 = colorID1; }
void set_colorID2(int colorID2) { m_colorID2 = colorID2; }
int get_colorID1() { return m_colorID1; }
int get_colorID2() { return m_colorID2; }

CString readQ(CAdoFile* ado, const char* tn, const char* tn2, int fileID);
CString readQ(CAdoFile* ado, const char* tn, const char* tn2, const char* fileID);

void GrafikShurfovki(CWnd* wnd, int year1, int currentMonth);
void GrafikShurfovki(CWnd* wnd, int year1);
int vydMsRs(CGraph2* m_graph, int ms, int rs);
void NeOpisanMessage(int id_ms, int id_rs);


void CGidrView::OnRemontPovrDefPanel() {}

void CGidrView::OnRemontPovrDefAdd()
{
    Klassif* kls = m_geofile->m_kl_list->findKlN("defect");

    set_colorID1(1);
    set_colorID2(1);

    AddGeoValue("stateID", COleVariant(1L));
    AddGeoValue("remontTypeID", COleVariant(1L));

    if (kls) {
        addKls(kls);
    }
}

bool init_pts_table(const char* tab);


void CGidrView::OnRemontPovrDefDel()
{
    if (!init_pts_table("defect")) {
        AfxMessageBox("Таблицы defect не существует");
        return;
    }
    regim = R_INIT;
    m_nRegim = ID_PTS_DEL;

}

void CGidrView::OnRemontPovrDefMove()
{
    if (!init_pts_table("defect")) {
        AfxMessageBox("Таблицы defect не существует");
        return;
    }

    regim = R_INIT;
    m_nRegim = ID_PTS_MOVE;
}

void CGidrView::OnRemontPovrDefInfo()
{
    if (!init_pts_table("defect")) {
        AfxMessageBox("Таблицы defect не существует");
        return;
    }

    regim = R_INIT;
    m_nRegim = ID_PTS_INFO;
}

void CGidrView::OnRemontPovrShurfAdd()
{
    Klassif* kls = m_geofile->m_kl_list->findKlN("shurfy");
    if (kls) {
        initGeoValue();
        set_colorID2(1);
        set_colorID1(1);

        AddGeoValue("naznachenie_vskrID", COleVariant(1L));
        AddGeoValue("sostoyanie_shurfaID", COleVariant(1L));
        AddGeoValue("utverdit", COleVariant(0L));
        addKls(kls, false);
        //        OnRemontShurfPlan();
    }
}

void CGidrView::OnRemontPovrShurfAddNeplan()
{
    Klassif* kls = m_geofile->m_kl_list->findKlN("shurfy");
    if (kls) {
        initGeoValue();

        set_colorID2(3);
        set_colorID1(2);

        AddGeoValue("naznachenie_vskrID", COleVariant(3L));
        AddGeoValue("sostoyanie_shurfaID", COleVariant(2L));
        AddGeoValue("utverdit", COleVariant(3L));  // Утверждение 3
        addKls(kls, false);
        //        resetZhurnal();
        //        OnShurfAll();
    }
}


bool init_pts_table(const char* tab);


void CGidrView::OnRemontPovrShurfDel()
{
    if (!init_pts_table("shurfy")) {
        AfxMessageBox("Таблицы shurfy не существует");
        return;
    }
    regim = R_INIT;
    m_nRegim = ID_PTS_DEL;

}

void CGidrView::OnRemontPovrShurfMove()
{
    if (!init_pts_table("shurfy")) {
        AfxMessageBox("Таблицы shurfy не существует");
        return;
    }

    regim = R_INIT;
    m_nRegim = ID_PTS_MOVE;
}

void CGidrView::OnRemontPovrShurfInfo()
{
    if (!init_pts_table("shurfy")) {
        AfxMessageBox("Таблицы shurfy не существует");
        return;
    }

    regim = R_INIT;
    m_nRegim = ID_PTS_INFO;
}


CString argpath();


void CGidrView::OnRemontPovrKarta()
{
    CString str = argpath() + "excel2\\docx\\map_defect_template.docx";
    HINSTANCE hi = ShellExecute(m_hWnd, "open", str, NULL, NULL, SW_SHOWNORMAL);
}

void CGidrView::OnRemontPovrKarta2()
{
    CString str = argpath() + "excel2\\docx\\map_shurf_template.docx";
    HINSTANCE hi = ShellExecute(m_hWnd, "open", str, NULL, NULL, SW_SHOWNORMAL);
}




bool CGidrView::pts_karta(const CFPoint& point)
{
    CString str;
    if (isPot()) return false;
    if (!m_geofile) return false;

    CString tn = "defect";

    int ngeo = m_geofile->findXY3(point, D5 * geom.masx, 3, tn);
    if (ngeo) {
        GeoInfo(tn, ngeo, false);
    }
    return true;
}



void CGidrView::OnRemontPovrOtop()
{
    setKorroziaSezonDlg();
}

void CGidrView::OnRemontPovrOtklPovr() {}

void CGidrView::OnRemontPovrDefOtobr()
//void CGidrView::OnRemontOnOff()
{
    CFiltr3 menu(this, "Выберите объекты ремонтов");

    //    init_pts_list_file("remont");

    //    for (auto it : map_pts) {
    //        Klassif* kls = it.second;
    //        menu.Add(it.first, (int) kls, kls->ins);
    //    }
    Klassif* kls = m_geofile->m_kl_list->findKlN("defect");
    if (!kls) return;

    menu.Add(kls->RusName(), (int)kls, kls->ins);

    if (menu.DoModal() == IDOK) {
        for (auto it : menu.map_item) {
            Klassif* kls = (Klassif*)it.second.id;
            kls->ins = it.second.on;
        }
        m_geofile->m_kl_list->save2();

        setKorroziaSezon(m_sezon_korrozia, false);
        Invalidate();
    }
}

#if 0

void CGidrView::OnRemontShurfOtobr()
{
    CFiltr3 menu(this, "Выберите объекты ремонтов");

    //    init_pts_list_file("remont");

    //    for (auto it : map_pts) {
    //        Klassif* kls = it.second;
    //        menu.Add(it.first, (int) kls, kls->ins);
    //    }
    Klassif* kls = m_geofile->m_kl_list->findKlN("defect");
    if (!kls) return;

    menu.Add(kls->RusName(), (int)kls, kls->ins);

    if (menu.DoModal() == IDOK) {
        for (auto it : menu.map_item) {
            Klassif* kls = (Klassif*)it.second.id;
            kls->ins = it.second.on;
        }
        m_geofile->m_kl_list->save2();

        setKorroziaSezon(m_sezon_korrozia);
        Invalidate();
    }
}
#endif



//void CGidrView::OnRemontPovrNach(){}
//void CGidrView::OnRemontPovrRs(){}
//void CGidrView::OnRemontPovrMs(){}

void CGidrView::OnRemontPovrAll() {}


CString getPar(set<int>& v);


void setTeploSet(CAdoFile* ado, CString tn, set<int>& set_ms, set<int>& set_rs, int m_sezon_korrozia, COleDateTime m_sezon_date1, COleDateTime m_sezon_date2)
{
    CString s_rs = getPar(set_ms);
    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (!kls) return;

    CString q, ss;

    CString date_q = "";

    CString ds1 = m_sezon_date1.Format(_T("%Y%m%d"));
    CString ds2 = m_sezon_date2.Format(_T("%Y%m%d"));

    if (m_sezon_korrozia > 0) {
        if (tn == "shurfy") {
            date_q.Format(" WHERE (data_nachala_plan BETWEEN '%s' AND '%s' OR data_nachala BETWEEN '%s' AND '%s')", ds1, ds2, ds1, ds2);
        }
        else if (tn == "defect") {
            date_q.Format(" WHERE data_osmotra BETWEEN '%s' AND '%s' ", ds1, ds2);
        }
        else if (tn == "indikator_korrozii") {
            date_q.Format(" WHERE data_planirovaniya BETWEEN '%s' AND '%s' ", ds1, ds2);
        }

    }

    q.Format(
        "select top 2147483647\n"
        "-- '%s',\n"
        "t.obj_id as id\n"

        "from(\n"
        "   select distinct\n"
        "       MIN(l.shape.STDistance(d.shape)) OVER(PARTITION BY d.id ) AS min_len,\n"
        "       l.shape.STDistance(d.shape) as length,\n"
        "       l.id as lineID,\n"
        "       l.externalSignLineID,\n"
        "       d.id as obj_id\n"
        "   from %s d\n"
        "       JOIN linesobj l ON ( l.removed = 0 and l.shape.STDistance(d.shape) < 0.1)\n"
        "  %s   \n"
        ")t\n"
        "LEFT JOIN heatPipeSections hpss ON hpss.lineID=t.lineID\n"
        "where t.min_len = t.length\n"
        , tn, tn, date_q);


    if (set_ms.size() > 0 || set_rs.size() > 0) {
        q += " AND ";

        if (set_ms.size() > 0) {
            ss.Format(" (hpss.magistralSite IN (%s)\n", getPar(set_ms));
            q += ss;
        }
        else {
            q += " (1<>1 ";
        }

        q += " OR ";

        if (set_rs.size() > 0) {
            ss.Format(" hpss.distSite IN (%s))\n", getPar(set_rs));
            q += ss;
        }
        else {
            q += " 1<>1)";
        }
    }
    m_geofile->setVisibleAll(tn);
    m_geofile->setVisible(kls, q);
}



void CGidrView::OnRemontPovrTeploSet()
{
    CTeploset dlg(this);

    if (dlg.DoModal() == IDOK) {
        m_is_CTeploset = true;
        if (dlg.m_all) {
            m_set_ms.clear();
            m_set_rs.clear();
            m_cxema->m_graph->allVisible();
            m_geofile->setVisibleAll("defect");
            m_geofile->setVisibleAll("shurfy");
            m_geofile->setVisibleAll("indikator_korrozii");
        }
        else {
            m_set_ms = dlg.m_set_ms;
            m_set_rs = dlg.m_set_rs;

            m_cxema->m_graph->visibleMSRS(dlg.m_set_ms, dlg.m_set_rs);

            CAdoFile* ado = getAdo(getPsAdoName());
            if (ado) {
                setTeploSet(ado, "defect", dlg.m_set_ms, dlg.m_set_rs, m_sezon_korrozia, m_sezon_date1, m_sezon_date2);
                setTeploSet(ado, "shurfy", dlg.m_set_ms, dlg.m_set_rs, m_sezon_korrozia, m_sezon_date1, m_sezon_date2);
                setTeploSet(ado, "indikator_korrozii", dlg.m_set_ms, dlg.m_set_rs, m_sezon_korrozia, m_sezon_date1, m_sezon_date2);
            }
        }

        resetZhurnal(true);
        Invalidate();
    }
}


void CGidrView::OnOpresTeploSet()
{
    CTeploset dlg(this, true, m_is_CTeploset);

    if (dlg.DoModal() == IDOK) {
        m_is_CTeploset = true;

        for (auto &it : dlg.m_set_ms) {
            vydMsRs(m_cxema->m_graph, it, 0);
        }

        for (auto &it : dlg.m_set_rs) {
            vydMsRs(m_cxema->m_graph, 0, it);
        }
        Invalidate();
    }
}




void CGidrView::OnRemontPovrDefFind() {}
void CGidrView::OnRemontPovrDefTable() {}

CString set_to_text(set<int>& set_i)
{
    CString ss, s = "";

    for (auto& it : set_i) {
        if (s != "") s += ",";
        ss.Format("%d", it);
        s += ss;
    }
    return s;
}

int regex_match(const char* p, const char* buf, vector<CString>& match);

static int m_zhur_id = -1;

bool CGidrView::isOpenZhur(int id)
{
    return id == m_zhur_id;
}

void set_ms_rs_condition(CString& qq, std::set<int>& m_set_ms, std::set<int>& m_set_rs)
{
    CString ss, ms_rs_condition = "";

    if (m_set_ms.size() > 0 || m_set_rs.size() > 0) {

        ms_rs_condition += "AND (";

        if (m_set_ms.size() > 0) {
            CString s1 = set_to_text(m_set_ms);
            ss.Format(" hpss.magistralSite IN (%s)", s1);
            ms_rs_condition += ss;
        }
        if (m_set_rs.size() > 0) {
            if (m_set_ms.size() > 0) {
                ms_rs_condition += " OR ";
            }

            CString s1 = set_to_text(m_set_rs);
            ss.Format(" hpss.distSite IN (%s)", s1);
            ms_rs_condition += ss;
        }
        ms_rs_condition += ")";
    }
    qq.Replace("$and_condition2$", ms_rs_condition);
}

void sezon_condition(CString &q, int m_sezon_korrozia, COleDateTime m_sezon_date1, COleDateTime m_sezon_date2) 
{
    CString season_condition = " IS NULL OR 1=1";

    CString ds1 = m_sezon_date1.Format(_T("%Y%m%d"));
    CString ds2 = m_sezon_date2.Format(_T("%Y%m%d"));

    if (m_sezon_korrozia > 0) {
        season_condition.Format(" BETWEEN '%s' AND '%s' ", ds1, ds2);
    }
    q.Replace("$season_condition$", season_condition);
}


void regex_replace(CString& s, CString p1, CString p2);


CString CGidrView::ispr_q_for_zhurnal(CString fn, CString date_name, const map<CString, CString>* map_fn)
{
    CString ss;

    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) {
        return "";
    }

    CString qq = readQ(ado, fn, "", m_cxema->m_par);

    CString and_condition = "";
    CString season_condition = " IS NULL OR 1=1";

    CString ds1 = m_sezon_date1.Format(_T("%Y%m%d"));
    CString ds2 = m_sezon_date2.Format(_T("%Y%m%d"));

    CString and_condition0;
    and_condition0.Format(" AND n1.fileID in (%s) ", m_cxema->m_par);
    and_condition += and_condition0;

    if (m_sezon_korrozia > 0) {
        ss.Format(" AND %s BETWEEN '%s' AND '%s' ", date_name, ds1, ds2);
        and_condition += ss;
        ss.Format(" BETWEEN '%s' AND '%s' ", ds1, ds2);
        season_condition = ss;
    }

    CString ms_rs_condition = "";

    if (m_set_ms.size() > 0 || m_set_rs.size() > 0) {

        ms_rs_condition += "AND (";

        if (m_set_ms.size() > 0) {
            CString s1 = set_to_text(m_set_ms);
            ss.Format(" hpss.magistralSite IN (%s)", s1);
            ms_rs_condition += ss;
        }
        if (m_set_rs.size() > 0) {
            if (m_set_ms.size() > 0) {
                ms_rs_condition += " OR ";
            }

            CString s1 = set_to_text(m_set_rs);
            ss.Format(" hpss.distSite IN (%s)", s1);
            ms_rs_condition += ss;
        }
        ms_rs_condition += ")";

        and_condition += ms_rs_condition;
    }
    qq.Replace("$and_condition2$", ms_rs_condition);
    qq.Replace("$season_condition$", season_condition);

    qq.Replace("$and_condition$", and_condition);

    if (map_fn) {
        for (auto& it : *map_fn) {
            qq.Replace(it.first, it.second);
        }
    }

    CString and_n1_fileID;
    and_n1_fileID.Format(" AND n1.fileID in (%s) ", m_cxema->m_par);
    qq.Replace("$and_n1_fileID$", and_n1_fileID);

    regex_replace(qq, "\\$[a-zA-Z0-9_]+\\$", "");

    return qq;
}


void CGidrView::remontZhur(int idd, CString fn, CString tn, CString date_name, CString title, CString state_cond, const map<CString, CString>* map_fn, VIRT_DATA typ)
{
    CString qq = ispr_q_for_zhurnal(fn, date_name, map_fn);


    m_zhurnal = ID_REMONT_POVR_DEF_ZHUR;

//    CInputDialog inD(AfxGetMainWnd(), "", "?", qq);
//    inD.DoModal();

//    if (tn == "shurfy") {
//        CAdoFile* ado = getAdo(getPsAdoName());
//        if (ado) {
//            ado->Execute("ALTER INDEX [PK_Individual_ShurfyID] ON [dbo].[shurfy] REBUILD PARTITION = ALL WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON)");
//            ado->Execute("ALTER INDEX [shurfy_shape_idx] ON [dbo].[shurfy] REBUILD PARTITION = ALL WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON)");
//        }
//    }


    ZhurnalQ2(tn, qq, title, m_is_reset_visible, typ);
        m_zhur_id = idd;
}

// Сезоны если выбраны все поменять

CString sezon_name(int y)
{
    CString s;
    s.Format("Отопительный сезон %d-%d", y, y + 1);
    if (y == -1) {
        s.Format("Все отопительные сезоны");
    }
    return s;
}


void CGidrView::OnShurfAll()
{
    CString fn = "objects\\defect\\shurf_journal";
    CString s;
    s.Format("Все шурфы. %s", sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
    //    map_fn["$and_naznachenie_vskrID$"] = " AND d.naznachenie_vskrID = 1 ";
    //    map_fn["$and_naznachenie_vskrID$"] = " AND d.sostoyanie_shurfaID = 2 ";
    remontZhur(10, fn, "shurfy", "", s, "", &map_fn);
}

void CGidrView::OnShurfProcess()
{
    CString fn = "objects\\defect\\shurf_journal";
    CString s;
    s.Format("Шурфы в процессе. %s", sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
    //    map_fn["$and_naznachenie_vskrID$"] = " AND d.naznachenie_vskrID = 1 ";
    map_fn["$and_naznachenie_vskrID$"] = " AND d.sostoyanie_shurfaID = 2 ";
    remontZhur(11, fn, "shurfy", "t.data_osmotra", s, "", &map_fn);
}

void CGidrView::OnShurfVypolneno()
{
    CString fn = "objects\\defect\\shurf_journal";
    CString s;
    s.Format("Шурфы выполненные. %s", sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
    map_fn["$and_naznachenie_vskrID$"] = " AND d.sostoyanie_shurfaID = 3 ";
    remontZhur(12, fn, "shurfy", "t.data_osmotra", s, "", &map_fn);
}


void CGidrView::OnRemontShurfPlan()
{
    //    CString fn = "objects\\defect\\plan_shurf";
    CString fn = "objects\\defect\\shurf_journal";

    CString s;
    s.Format("План шурфов. %s", sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
    map_fn["$and_naznachenie_vskrID$"] = " AND d.naznachenie_vskrID = 1 ";
    remontZhur(1, fn, "shurfy", "t.data_osmotra", s, "", &map_fn);
}

void CGidrView::OnRemontShurfPredpis()
{
    CString fn = "objects\\defect\\shurf_journal";
    CString s;

    s.Format("Шурфы по предписанию. %s", sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
    map_fn["$and_naznachenie_vskrID$"] = " AND d.naznachenie_vskrID = 2 ";

    remontZhur(2, fn, "shurfy", "t.data_osmotra", s, "", &map_fn);
}

void CGidrView::OnRemontShurfNarush()
{
    CString fn = "objects\\defect\\shurf_journal";
    CString s;
    s.Format("Шурфы по нарушению. %s", sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
    map_fn["$and_naznachenie_vskrID$"] = " AND d.naznachenie_vskrID = 3 ";

    remontZhur(3, fn, "shurfy", "t.data_osmotra", s, "", &map_fn);
}


void CGidrView::OnShurfTable()
{
    CString tn = "shurfy";
    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);

    if (!kls) return;

    CString ss;

    CString date_name = "t.data_osmotra";
    CString fn = "objects\\defect\\shurf_journal";
    CString qq = readQ(kls->m_ado, fn, "", m_cxema->m_par);


    CString and_condition = "";
    CString season_condition = " IS NULL OR 1=1";

    CString ds1 = m_sezon_date1.Format(_T("%Y%m%d"));
    CString ds2 = m_sezon_date2.Format(_T("%Y%m%d"));

    CString and_condition0;
    and_condition0.Format(" AND n1.fileID in (%s) ", m_cxema->m_par);
    and_condition += and_condition0;

    if (m_sezon_korrozia > 0) {
        ss.Format(" AND %s BETWEEN '%s' AND '%s' ", date_name, ds1, ds2);
        and_condition += ss;
        ss.Format(" BETWEEN '%s' AND '%s' ", ds1, ds2);
        season_condition = ss;
    }

    CString ms_rs_condition = "";
    set_ms_rs_condition(qq, m_set_ms, m_set_rs);

    qq.Replace("$season_condition$", season_condition);
    qq.Replace("$and_condition$", and_condition);

    CString and_n1_fileID;
    and_n1_fileID.Format(" AND n1.fileID in (%s) ", m_cxema->m_par);
    qq.Replace("$and_n1_fileID$", and_n1_fileID);

    regex_replace(qq, "\\$[a-zA-Z0-9_]+\\$", "");

    //    m_zhurnal = ID_REMONT_POVR_DEF_ZHUR;
    void regex_replace(CString & s, CString p1, CString p2);
    regex_replace(qq, "\\$[a-zA-Z0-9_]+\\$", "");

    //    ZhurnalQ2(tn, qq, title);
    RunTable(tn, qq, -1, virt_data_geo, "", "");
}

static CString m_shurfy_nazn = "", m_shurfy_date = "";
static int m_shurf_month = -1;

void ispr_q(CString& q)
{
    CGidrView* view = getView();

    if (!view) return;

    CString ds1 = view->m_sezon_date1.Format(_T("%Y%m%d"));
    CString ds2 = view->m_sezon_date2.Format(_T("%Y%m%d"));

    CString ss;
    CString season_condition = " IS NULL OR 1=1";

    if (view->m_sezon_korrozia > 0) {
        ss.Format(" BETWEEN '%s' AND '%s' ", ds1, ds2);
        season_condition = ss;
    }

    q.Replace("$season_condition$", season_condition);
    q.Replace("$and_naznachenie_vskrID$", " AND d.naznachenie_vskrID = 1 ");
    set_ms_rs_condition(q, view->m_set_ms, view->m_set_rs);

//    ss.Format("AND d.naznachenie = '%s' AND d.data_utverzhdeniya_plana_shurfovok='%s'", m_shurfy_nazn, m_shurfy_date);
    ss.Format("AND d.naznachenie = '%s' ", m_shurfy_nazn);
    q.Replace("$and_name_date$", ss);

    ss.Format("AND MONTH(d.data_nachala_plan)=%d", m_shurf_month);
    q.Replace("$and_name_date_mon$", ss);

//    std::set<int> m_set_ms, m_set_rs;

    set_ms_rs_condition(q, view->m_set_ms, view->m_set_rs);

    CString and_n1_fileID;
    and_n1_fileID.Format(" AND n1.fileID in (%s) ", view->m_cxema->m_par);
    q.Replace("$and_n1_fileID$", and_n1_fileID);


}

bool qMenu(CWnd* wnd, CAdoFile* ado, CString q, CString caption)
{
    //    CFiltr3 dlg(this, ss, IDD_SHURF_LIST);
    CMMenu2 menu(wnd, IDD_MENU_MULTI, caption);

    int id = 0;

    menu.AddColumn(_TR("Наименование Плана шурфовок"));
    menu.AddColumn(_TR("Дата утверждения"));

    //    menu.setWidth(1, 400);
    //    menu.setWidth(2, 200);

    map<int, pair<CString, CString> > map_p;

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            CString s1 = ado->readStr(0);
            CString s2 = ado->readStr(1);
            menu.Add(1, s1);
            menu.Add(2, s2);

            map_p[id] = pair<CString, CString>(s1, s2);

            menu.Add((void*)id);

            id++;

            ado->MoveNext();
        }
        ado->closeTable();
    }

    if (menu.DoModal() == IDOK) {
        int id = (int)menu.getV();
        auto it = map_p.find(id);
        if (it != map_p.end()) {
            m_shurfy_nazn = it->second.first;
            m_shurfy_date = it->second.second;
            return true;
        }
    }
    return false;
}

int qMenu1(CWnd* wnd, CAdoFile* ado, CString q, CString caption)
{
    //    CFiltr3 dlg(this, ss, IDD_SHURF_LIST);
    CMMenu menu(wnd, IDD_MENU, caption);

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long(0);
            CString s2 = ado->readStr(1);
            menu.Add(s2, (void*)id);

            ado->MoveNext();
        }
        ado->closeTable();
    }

    if (menu.DoModal() == IDOK) {
        m_shurf_month = (int)menu.getV();;
        return m_shurf_month;
    }
    return -1;
}


void CGidrView::OnRemontShurfPlanExcel()
{
    if (m_sezon_korrozia < 0) {
        setKorroziaSezonDlg();
        if (m_sezon_korrozia < 0) {
            return;
        }
    }


    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) {
        return;
    }

    CString fn = "objects\\defect\\plan_shurf";

    map<CString, CString> map_fn;

    //    COleDateTime v = COleDateTime::GetCurrentTime();

    CString s;
    int year = m_sezon_korrozia;

    s.Format("%d", year);
    map_fn["$YEAR1$"] = s;
    s.Format("%d", year + 1);
    map_fn["$YEAR2$"] = s;

    CString season_condition = " IS NULL OR 1=1";

    if (m_sezon_korrozia > 0) {
        CString ds1 = m_sezon_date1.Format(_T("%Y%m%d"));
        CString ds2 = m_sezon_date2.Format(_T("%Y%m%d"));
        season_condition.Format(" BETWEEN '%s' AND '%s' ", ds1, ds2);
    }


    map_fn["$season_condition$"] = season_condition;
    map_fn["$and_utverdit$"] = "";


    CString q;
    q.Format(
        "select distinct \n"
        "d.naznachenie as 'Наименование Плана шурфовок',\n"
        "format(d.data_utverzhdeniya_plana_shurfovok,'dd/MM/yyyy')  AS 'Дата утверждения' \n"
//        "d.data_utverzhdeniya_plana_shurfovok  AS 'Дата утверждения' \n"
        "from shurfy d\n"
        "where (d.data_nachala_plan %s)  AND d.naznachenie_vskrID = 1 \n"
        "order by 'Дата утверждения' DESC, 'Наименование Плана шурфовок'", season_condition);

    CString ss;
    ss.Format("Планы шурфовок (документ). %s", sezon_name(m_sezon_korrozia));
    if (qMenu(this, ado, q, ss)) {
        GrafikShurfovki(this, m_sezon_korrozia);
    }

    //    pas_excel_1("Ф3", "objects\\defect\\plan_shurf", "remont\\plan_shurf.htm", map_fn);

}


void CGidrView::OnRemontShurfPlanExcelMonth()
{
    if (m_sezon_korrozia < 0) {
        setKorroziaSezonDlg();
        if (m_sezon_korrozia < 0) {
            return;
        }
    }


    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) {
        return;
    }

    CString fn = "objects\\defect\\plan_shurf";

    map<CString, CString> map_fn;

    //    COleDateTime v = COleDateTime::GetCurrentTime();

    CString s;
    int year = m_sezon_korrozia;

    s.Format("%d", year);
    map_fn["$YEAR1$"] = s;
    s.Format("%d", year + 1);
    map_fn["$YEAR2$"] = s;

    CString season_condition = " IS NULL OR 1=1";

    if (m_sezon_korrozia > 0) {
        CString ds1 = m_sezon_date1.Format(_T("%Y%m%d"));
        CString ds2 = m_sezon_date2.Format(_T("%Y%m%d"));
        season_condition.Format(" BETWEEN '%s' AND '%s' ", ds1, ds2);
    }


    map_fn["$season_condition$"] = season_condition;
    map_fn["$and_utverdit$"] = "";

    CString q;
    q.Format(
        "select \n"
        "distinct \n"
        "month(d.data_nachala_plan),\n"
        "choose(month(d.data_nachala_plan), 'январь', 'февраль', 'март', 'апрель', 'май', 'июнь', 'июль', 'август', 'сентябрь', 'октябрь', 'ноябрь', 'декабрь') as 'месяц'\n"
        "from shurfy d\n"
        "where (d.data_nachala_plan %s)  AND d.naznachenie_vskrID = 1 \n", season_condition);


    CString ss;
    ss.Format("Планы шурфовок (документ). %s", sezon_name(m_sezon_korrozia));
    int mon = qMenu1(this, ado, q, ss);
    if (mon >= 0) {
        GrafikShurfovki(this, m_sezon_korrozia, mon);
    }





    //    pas_excel_1("Ф3", "objects\\defect\\plan_shurf", "remont\\plan_shurf.htm", map_fn);

}


#include "dialog_pr.h"
#include "wm_user.h"

static CString m_str_list_shurf = "";


// Утвердить план

void CGidrView::OnRemontShurfPlanUtverdit()
{
    if (m_sezon_date1 == 0) {
        AfxMessageBox("Выберите отопительный сезон", MB_OK | MB_ICONINFORMATION);
        return;
    }
    CString str;
    str.Format(_TR("План шурфовок на отопительный сезон %d %d"), m_sezon_korrozia, m_sezon_korrozia + 1);

    CAdoFile* ado = m_cxema->m_ado;

    //    CString fn = "objects\\defect\\shurf_list";
    CString fn = "objects\\defect\\plan_shurf";
    CString q = readQ(ado, fn, "", -1);

    CString season_condition = " IS NULL OR 1=1";

    if (m_sezon_korrozia > 0) {
        CString ds1 = m_sezon_date1.Format(_T("%Y%m%d"));
        CString ds2 = m_sezon_date2.Format(_T("%Y%m%d"));
        season_condition.Format(" BETWEEN '%s' AND '%s' ", ds1, ds2);
    }

    q.Replace("$season_condition$", season_condition);
    q.Replace("$and_utverdit$", " AND d.utverdit = 0 ");
    q.Replace("$and_naznachenie_vskrID$", " AND d.naznachenie_vskrID = 1 ");

    CString and_n1_fileID;
    and_n1_fileID.Format(" AND n1.fileID in (%s) ", m_cxema->m_par);
    q.Replace("$and_n1_fileID$", and_n1_fileID);

    set_ms_rs_condition(q, m_set_ms, m_set_rs);

    CFiltr3 dlg(this, str, IDD_SHURF_LIST);

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            int id = ado->read_long(0);
            CString u1 = ado->readStr("uzel1");
            CString u2 = ado->readStr("uzel2");
            CString sa = ado->readStr("Адрес");
            CString spr = ado->readStr("Признак сети");
            CString sn = ado->readStr("Наименование участка");
            CString su = ado->readStr("Участок/район эксплуатации");
            CString nach = ado->readStr("Начальник участка");

            CString s = u1 + " - " + u2 + " " + sa + " " + spr + " " + sn + " " + su + " " + nach;

            dlg.Add(s, id, 1);

            ado->MoveNext();
        }
        ado->closeTable();
    }


    if (dlg.map_item.size() <= 0) {
        AfxMessageBox("Нет запланированных на выбранный сезон шурфовок");
        return;
    }


    int ret = dlg.DoModal();

    //    str.Format(_TR("Утвердить план шурфовок на отопительный сезон %d %d?"), m_sezon_korrozia, m_sezon_korrozia + 1);
        //    str.Format(_TR("Слишком близко к %s %s"), bline(line)->getTable(), bline(line)->getName());
    //    if (AfxMessageBox(str, MB_YESNO) == IDYES) {
    if (ret == IDOK && dlg.isNoEmpty()) {

        CPropGridDlg* pm_cEditDlg = dialog_pr(this, "shurf_utverdit", WM_USER_SHURF_UTVERDIT_ALL, _TR("Утверждение плана шурфовок"), -1);

        if (pm_cEditDlg) {
            CString str_naznachenie =""; // = AfxGetApp()->GetProfileString("shurf_utverdit", "naznachenie", "");

            CString str_fio_utverzhdaemogo = AfxGetApp()->GetProfileString("shurf_utverdit", "fio_utverzhdaemogo", "");
            CString str_dolzhnost_utverzhdaemogoID = AfxGetApp()->GetProfileString("shurf_utverdit", "dolzhnost_utverzhdaemogoID", "");
            CString str_sluzhba_utverzhdaemogoID = AfxGetApp()->GetProfileString("shurf_utverdit", "sluzhba_utverzhdaemogoID", "");
            CString str_fio_viziruemogo_1 = AfxGetApp()->GetProfileString("shurf_utverdit", "fio_viziruemogo_1", "");
            CString str_dolzhnost_viziruemogoID_1 = AfxGetApp()->GetProfileString("shurf_utverdit", "dolzhnost_viziruemogoID_1", "");
            CString str_data_utverzhdeniya_plana_shurfovok = AfxGetApp()->GetProfileString("shurf_utverdit", "data_utverzhdeniya_plana_shurfovok", "");

            setPropValueInit(pm_cEditDlg, WM_USER_SHURF_UTVERDIT_ALL, "naznachenie", str_naznachenie);

            setPropValueInit(pm_cEditDlg, WM_USER_SHURF_UTVERDIT_ALL, "fio_utverzhdaemogo", str_fio_utverzhdaemogo);
            setPropValueInit(pm_cEditDlg, WM_USER_SHURF_UTVERDIT_ALL, "dolzhnost_utverzhdaemogoID", str_dolzhnost_utverzhdaemogoID);
            setPropValueInit(pm_cEditDlg, WM_USER_SHURF_UTVERDIT_ALL, "sluzhba_utverzhdaemogoID", str_sluzhba_utverzhdaemogoID);
            setPropValueInit(pm_cEditDlg, WM_USER_SHURF_UTVERDIT_ALL, "fio_viziruemogo_1", str_fio_viziruemogo_1);
            setPropValueInit(pm_cEditDlg, WM_USER_SHURF_UTVERDIT_ALL, "dolzhnost_viziruemogoID_1", str_dolzhnost_viziruemogoID_1);
            setPropValueInit(pm_cEditDlg, WM_USER_SHURF_UTVERDIT_ALL, "data_utverzhdeniya_plana_shurfovok", str_data_utverzhdeniya_plana_shurfovok);
        }

        m_str_list_shurf = "";

        for (auto it : dlg.map_item) {
            if (it.second.on) {
                CString ss;
                ss.Format("%d", it.second.id);
                if (m_str_list_shurf != "") m_str_list_shurf += ",";
                m_str_list_shurf += ss;
            }
        }

#if 0
        CInputDialogDate dlg2(this, "", "Введите дату утверждения плана шурфовок");

        if (dlg2.DoModal() != IDOK) return;

        CString ds1 = m_sezon_date1.Format(_T("%Y%m%d"));
        CString ds2 = m_sezon_date2.Format(_T("%Y%m%d"));

        CString ds0 = dlg2.m_date1.Format("%Y%m%d"); // = m_sezon_date0.Format(_T("%Y%m%d"));

        q.Format("UPDATE d\n"
            "set\n"
            "d.utverdit = 1,\n"
            "d.data_utverzhdeniya_plana_shurfovok = '%s'\n"
            "from shurfy d\n"
            "where d.naznachenie_vskrID = 1 and d.data_nachala_plan BETWEEN '%s' AND '%s' AND d.id IN (%s) ", ds0, ds1, ds2, qq);

        if (ado->Execute(q, &affected)) {
            OnRemontShurfPlan();
        }
        else {
        }
#endif
    }
}

LRESULT CGidrView::OnShurfUtverditALL(WPARAM wParam, LPARAM)
{
    CString ds1 = m_sezon_date1.Format(_T("%Y%m%d"));
    CString ds2 = m_sezon_date2.Format(_T("%Y%m%d"));

    CString str_naznachenie;
    CString str_fio_utverzhdaemogo;
    CString str_dolzhnost_utverzhdaemogoID;
    CString str_sluzhba_utverzhdaemogoID;
    CString str_fio_viziruemogo_1;
    CString str_dolzhnost_viziruemogoID_1;
    CString str_data_utverzhdeniya_plana_shurfovok;


    if (getPropValue0(WM_USER_SHURF_UTVERDIT_ALL, "naznachenie", str_naznachenie))
        AfxGetApp()->WriteProfileString("shurf_utverdit", "naznachenie", str_naznachenie);

    if (getPropValue0(WM_USER_SHURF_UTVERDIT_ALL, "fio_utverzhdaemogo", str_fio_utverzhdaemogo))
        AfxGetApp()->WriteProfileString("shurf_utverdit", "fio_utverzhdaemogo", str_fio_utverzhdaemogo);

    if (getPropValue0(WM_USER_SHURF_UTVERDIT_ALL, "dolzhnost_utverzhdaemogoID", str_dolzhnost_utverzhdaemogoID))
        AfxGetApp()->WriteProfileString("shurf_utverdit", "dolzhnost_utverzhdaemogoID", str_dolzhnost_utverzhdaemogoID);

    if (getPropValue0(WM_USER_SHURF_UTVERDIT_ALL, "sluzhba_utverzhdaemogoID", str_sluzhba_utverzhdaemogoID))
        AfxGetApp()->WriteProfileString("shurf_utverdit", "sluzhba_utverzhdaemogoID", str_sluzhba_utverzhdaemogoID);

    if (getPropValue0(WM_USER_SHURF_UTVERDIT_ALL, "fio_viziruemogo_1", str_fio_viziruemogo_1))
        AfxGetApp()->WriteProfileString("shurf_utverdit", "fio_viziruemogo_1", str_fio_viziruemogo_1);

    if (getPropValue0(WM_USER_SHURF_UTVERDIT_ALL, "dolzhnost_viziruemogoID_1", str_dolzhnost_viziruemogoID_1))
        AfxGetApp()->WriteProfileString("shurf_utverdit", "dolzhnost_viziruemogoID_1", str_dolzhnost_viziruemogoID_1);

    if (getPropValue0(WM_USER_SHURF_UTVERDIT_ALL, "data_utverzhdeniya_plana_shurfovok", str_data_utverzhdeniya_plana_shurfovok))
        AfxGetApp()->WriteProfileString("shurf_utverdit", "data_utverzhdeniya_plana_shurfovok", str_data_utverzhdeniya_plana_shurfovok);


    CString ds0 = str_data_utverzhdeniya_plana_shurfovok;

    //    CString ds0 = dlg2.m_date1.Format("%Y%m%d"); // = m_sezon_date0.Format(_T("%Y%m%d"));

    CAdoFile* ado = m_cxema->m_ado;

    long affected;

    CString q;

#if 0
    q.Format("UPDATE d\n"
        "set\n"
        "d.utverdit = 1,\n"
        "d.fio_utverzhdaemogo = '%s',\n"
        "d.dolzhnost_utverzhdaemogoID = '%s',\n"
        "d.sluzhba_utverzhdaemogoID = '%s',\n"
        "d.fio_viziruemogo_1 = '%s',\n"
        "d.dolzhnost_viziruemogoID_1 = '%s',\n"

        "d.data_utverzhdeniya_plana_shurfovok = '%s'\n"
        "from shurfy d\n"
        "where d.naznachenie_vskrID = 1 and d.data_nachala_plan BETWEEN '%s' AND '%s' AND d.id IN (%s) "
        ,

        "str_fio_utverzhdaemogo,\n"
        "str_dolzhnost_utverzhdaemogoID,\n"
        "str_sluzhba_utverzhdaemogoID,\n"
        "str_fio_viziruemogo_1,\n"
        "str_dolzhnost_viziruemogoID_1,\n"

        "ds0, \n"
        "ds1, ds2, \n"
        "m_str_list_shurf);\n"
#endif

        q.Format(
            "update d\n"
            "set\n"
            "d.utverdit = 1,\n"
            //            "d.sostoyanie_shurfaID = 1,\n"
            "d.naznachenie = '%s',\n"
            //            "d.sostoyanie_shurfaID = 2,\n"
            "d.data_utverzhdeniya_plana_shurfovok='%s',\n"
            "d.fio_utverzhdaemogo='%s',\n"
            "d.fio_viziruemogo_1='%s',\n"
            "d.dolzhnost_utverzhdaemogoID = t.id_d_utv,\n"
            "d.sluzhba_utverzhdaemogoID = t.id_s_utv,\n"
            "d.dolzhnost_viziruemogoID_1 = t.id_d_v\n"
            "from shurfy d\n"
            "join\n"
            "(select d.id, dolz_utv.id as id_d_utv, subd_utv.id as id_s_utv, dolz_v.id as id_d_v  from shurfy d\n"
            "LEFT JOIN dolzhnosti dolz_utv ON dolz_utv.znachenie = '%s'\n"
            "LEFT JOIN dolzhnosti dolz_v ON dolz_v.znachenie = '%s'\n"
            "LEFT JOIN subdivisions subd_utv ON subd_utv.name = '%s'\n"
            "WHERE d.id IN (%s)\n"
            ") t\n"
            "on t.id = d.id\n"
            "WHERE d.id IN (%s)\n"

            
            ,

            str_naznachenie,
            str_data_utverzhdeniya_plana_shurfovok,
            str_fio_utverzhdaemogo,
            str_fio_viziruemogo_1,

            str_dolzhnost_utverzhdaemogoID,
            str_dolzhnost_viziruemogoID_1,
            str_sluzhba_utverzhdaemogoID,
            m_str_list_shurf,
            m_str_list_shurf

        );



    if (ado->Execute(q, &affected)) {
        OnRemontShurfPlan();
        AfxMessageBox("Утверждено");
    }
    else {
        AfxMessageBox("Ошибка");
    }
    return 0L;
}



void CGidrView::OnRemontShurfZhurnal()
{
    CString fn, s;
    fn = "objects\\defect\\shurf_journal";
    s.Format("Журнал шурфов за отопительный сезон %d-%d", m_sezon_korrozia, m_sezon_korrozia + 1);

    remontZhur(4, fn, "shurfy", "t.data_osmotra", s, "", nullptr);
}


#if 0
bool CGidrView::exec_init_for_defect(CString fn1, CString condition)
{
    return true;

//    CString fn1 = "objects\\defect\\defects_journal_init";
    map<CString, CString> map_fn;
    map_fn["$state_cond$"] = condition;

    CString qq = ispr_q_for_zhurnal(fn1, "", &map_fn);

    CAdoFile* ado = getAdo(getPsAdoName());

    if (ado) {
        m_sql_for_defect = qq;
        bool ret = ado->Execute(qq);
        if (ret) {
//            CString ss = readTableValue(ado, "SELECT TOP 1 * FROM tempdb..##InitialJoin");
        }
        return ret;
    }
    return false;
}

#endif

bool CGidrView::init_for_defect(int id, CString title, CString condition)
{
    CString fn = "objects\\defect\\defects_journal";

    map<CString, CString> map_fn;
    map_fn["$state_cond$"] = condition;
    remontZhur(id, fn, "defect", "", title, "", &map_fn);

    return true;
}

void CGidrView::OnRemontPovr_1() {}

void CGidrView::OnRemontPovrPovr() {}
void CGidrView::OnRemontPovrRemont() {}
void CGidrView::OnRemontPovrPosleRemont() {}
void CGidrView::OnRemontPovrRemontAll() {}

void CGidrView::OnRemontPovr_2() {}

void CGidrView::OnRemontPovrRemontTrub() {}
void CGidrView::OnRemontPovrRazmZapl() {}
void CGidrView::OnRemontPovrDlinaZamTrub() {}
void CGidrView::OnRemontPovrRemIzol() {}
void CGidrView::OnRemontPovrRemKanal() {}
void CGidrView::OnRemontPovrRemAll() {}

void CGidrView::OnRemontPovr_3() {}

void CGidrView::OnRemontPovrDefEkspl() {}
void CGidrView::OnRemontPovrDefOpr() {}
void CGidrView::OnRemontPovrDefOsv() {}
void CGidrView::OnRemontPovrDefShurfPlan() {}
//void CGidrView::OnRemontPovrAll(){}

void CGidrView::OnRemontPovrDefAnalizTip() {}
void CGidrView::OnRemontPovrDefAnalizPrichina() {}
void CGidrView::OnRemontPovrDefAnalizElement() {}
void CGidrView::OnRemontPovrDefAnalizTipProkl() {}
void CGidrView::OnRemontPovrDefAnalizItd() {}
void CGidrView::OnRemontPovrDefAnalizAll() {}





void CGidrView::OnRemontOsmotrPanel() {}
void CGidrView::OnRemontOsmotrTeploSet() {}

void CGidrView::OnRemontOsmotrSezon()
{
    setKorroziaSezonDlg();
}



void CGidrView::OnRemontOsmotrTable() {}
void CGidrView::OnRemontOsmotrZhurnalOsmotr() {}
void CGidrView::OnRemontOsmotrZhurnalPovr() {}
void CGidrView::OnRemontOsmotrRegistr() {}
void CGidrView::OnRemontOsmotrAkt() {}
void CGidrView::OnRemontOsmotrDocument() {}


void CGidrView::OnRemontOsmotrAdd()
{
    Klassif* kls = m_geofile->m_kl_list->findKlN("diag");
    if (kls) addKls(kls);
}


void CGidrView::OnRemontOsmotrDefAdd()
{
    OnRemontPovrDefAdd();
}

void CGidrView::OnRemontOsmotrSaveOsmotr() {}

void CGidrView::OnRemontOsmotrDefDel()
{
    //    OnRemontPovrDefDel();
}

void CGidrView::OnRemontOsmotrDefMove() {}
void CGidrView::OnRemontOsmotrDefInfo() {}


void CGidrView::OnRemontShurfOtobr()
{
    CFiltr3 menu(this, "Выберите объекты");

    //    init_pts_list_file("remont");

    //    for (auto it : map_pts) {
    //        Klassif* kls = it.second;
    //        menu.Add(it.first, (int) kls, kls->ins);
    //    }
    Klassif* kls = m_geofile->m_kl_list->findKlN("shurfy");
    if (!kls) return;

    menu.Add(kls->RusName(), (int)kls, kls->ins);

    if (menu.DoModal() == IDOK) {
        for (auto it : menu.map_item) {
            Klassif* kls = (Klassif*)it.second.id;
            kls->ins = it.second.on;
        }
        m_geofile->m_kl_list->save2();

        setKorroziaSezon(m_sezon_korrozia, true);
        Invalidate();
    }
}

#if 0

// Журнал повреждений
void CGidrView::OnRemontPovrDefZhur()
{
    CString fn, s;
    fn = "objects\\defect\\defects_journal";
    //    fn = "objects\\defect\\defects_journal_remontov (1)";
    s.Format("Журнал нарушений (все) за отопительный сезон %d-%d", m_sezon_korrozia, m_sezon_korrozia + 1);
    remontZhur(5, fn, "defect", "t.data_osmotra", s, "", nullptr);
}


void CGidrView::OnRemontPovrDefZhur1()
{
    CString fn, s, cond;
    fn = "objects\\defect\\defects_journal";
    //    fn = "objects\\defect\\defects_journal_remontov (1)";
    s.Format("Журнал нарушений (до ремонта) за отопительный сезон %d-%d", m_sezon_korrozia, m_sezon_korrozia + 1);

    cond.Format(" k.stateID = 1 AND ");
    remontZhur(6, fn, "defect", "t.data_osmotra", s, cond, nullptr);
}
void CGidrView::OnRemontPovrDefZhur2()
{
    CString fn, s, cond;
    fn = "objects\\defect\\defects_journal";
    //    fn = "objects\\defect\\defects_journal_remontov (1)";
    s.Format("Журнал нарушений (ремонт) за отопительный сезон %d-%d", m_sezon_korrozia, m_sezon_korrozia + 1);
    cond.Format(" k.stateID = 2 AND ");
    remontZhur(7, fn, "defect", "t.data_osmotra", s, cond, nullptr);
}
void CGidrView::OnRemontPovrDefZhur3()
{
    CString fn, s, cond;
    fn = "objects\\defect\\defects_journal";
    //    fn = "objects\\defect\\defects_journal_remontov (1)";
    s.Format("Журнал нарушений (после ремонта) за отопительный сезон %d-%d", m_sezon_korrozia, m_sezon_korrozia + 1);
    cond.Format(" k.stateID = 3 AND ");
    remontZhur(8, fn, "defect", "t.data_osmotra", s, cond, nullptr);
}



void CGidrView::OnRemontPovrDefZhurRemont()
{
    CString fn, s;
    //    fn = "objects\\defect\\defects_journal";
    fn = "objects\\defect\\defects_journal_remontov (1)";
    s.Format("Журнал ремонтов за отопительный сезон %d-%d", m_sezon_korrozia, m_sezon_korrozia + 1);
    remontZhur(9, fn, "defect", "t.data_osmotra", s, "", nullptr);
}

#endif


void CGidrView::OnDefectZhurnalExpluatacia()
{
    CString s;
    s.Format("Нарушения эксплуатации на тепловой сети. %s", sezon_name(m_sezon_korrozia));

    init_for_defect(20, s, " (remontTypeID = 1) AND ");
}

void CGidrView::OnDefectZhurnalShurf()
{
    CString fn = "objects\\defect\\shurf_defect_journal";
    CString s;
    s.Format("Нарушения в шурфе на тепловой сети. %s", sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
    map_fn["$state_cond$"] = " (remontTypeID = 3) AND ";
    //    remontZhur(21, fn, "defect", "", s, "", &map_fn);
    m_geofile->setVisibleAllNo("defect");

    remontZhur(21, fn, "shurfy", "", s, "", &map_fn);
}

void CGidrView::OnDefectZhurnalOsmotr()
{
    CString s;
    s.Format("Нарушения осмотра на тепловой сети. %s", sezon_name(m_sezon_korrozia));

    init_for_defect(22, s, " (remontTypeID = 6) AND ");
}

void CGidrView::OnDefectZhurnalOpress()
{
    CString s;
    s.Format("Нарушения опрессовки на тепловой сети. %s", sezon_name(m_sezon_korrozia));

    init_for_defect(23, s, " (remontTypeID = 2) AND ");
}

void CGidrView::OnDefectZhurnalNarushenie()
{
    CString s;
    s.Format("Выявленные нарушения на тепловой сети. %s", sezon_name(m_sezon_korrozia));

    init_for_defect(24, s, " (stateID = 1) AND ");
}

void CGidrView::OnDefectZhurnalRemont()
{
    CString s;
    s.Format("Нарушения в процессе ремонта на тепловой сети. %s", sezon_name(m_sezon_korrozia));

    init_for_defect(25, s, " (stateID = 2) AND ");
}

void CGidrView::OnDefectZhurnalPosleremont()
{
    CString s;
    s.Format("Нарушения после ремонта на тепловой сети. %s", sezon_name(m_sezon_korrozia));

    init_for_defect(26, s, " (stateID = 3) AND ");
}

CString readQ(CAdoFile* ado, const char* tn, const char* tn2, int fileID);


void CGidrView::OnDefectZhurnalAll()
{
    CString s;
    s.Format("Все нарушения эксплуатации на тепловой сети. %s", sezon_name(m_sezon_korrozia));

    init_for_defect(27, s, "");
}


void CGidrView::remontTable(CString fn, CString tn, CString date_name, CString title, CString state_cond, const map<CString, CString>* map_fn, VIRT_DATA typ)
{
//    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
//    if (!kls) return;

    CString ss;

    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) {
        return;
    }

    CString qq = readQ(ado, fn, "", m_cxema->m_par);
    CString and_condition = "";
    CString season_condition = " IS NULL OR 1=1";

    CString ds1 = m_sezon_date1.Format(_T("%Y%m%d"));
    CString ds2 = m_sezon_date2.Format(_T("%Y%m%d"));

    CString and_condition0;
    and_condition0.Format(" AND n1.fileID in (%s) ", m_cxema->m_par);
    and_condition += and_condition0;

    if (m_sezon_korrozia > 0) {
        ss.Format(" AND %s BETWEEN '%s' AND '%s' ", date_name, ds1, ds2);
        and_condition += ss;
        ss.Format(" BETWEEN '%s' AND '%s' ", ds1, ds2);
        season_condition = ss;
    }

    CString ms_rs_condition = "";

    if (m_set_ms.size() > 0 || m_set_rs.size() > 0) {

        ms_rs_condition += "AND (";

        if (m_set_ms.size() > 0) {
            CString s1 = set_to_text(m_set_ms);
            ss.Format(" hpss.magistralSite IN (%s)", s1);
            ms_rs_condition += ss;
        }
        if (m_set_rs.size() > 0) {
            if (m_set_ms.size() > 0) {
                ms_rs_condition += " OR ";
            }

            CString s1 = set_to_text(m_set_rs);
            ss.Format(" hpss.distSite IN (%s)", s1);
            ms_rs_condition += ss;
        }
        ms_rs_condition += ")";

        and_condition += ms_rs_condition;
    }
    qq.Replace("$and_condition2$", ms_rs_condition);
    qq.Replace("$season_condition$", season_condition);

    qq.Replace("$and_n1_fileID$", and_condition0);

    qq.Replace("$and_condition$", and_condition);
    //    qq.Replace("$j$", "");
    //    qq.Replace("$order$", "");
    //    qq.Replace("$and_utverdit$", "");

    //    qq.Replace("$state_cond$", state_cond);

    if (map_fn) {
        for (auto& it : *map_fn) {
            qq.Replace(it.first, it.second);
        }
    }
    m_zhurnal = ID_REMONT_POVR_DEF_ZHUR;
    regex_replace(qq, "\\$[a-zA-Z0-9_]+\\$", "");

//    CInputDialog inD(AfxGetMainWnd(), "", "?", qq);
//    inD.DoModal();

    RunTable(tn, qq, -1, typ /*virt_data_geo*/, "", title);
//    ZhurnalQ2(tn, qq, title, m_is_reset_visible);
}


void CGidrView::OnDefectAnaliz()
{
    CString fn = "objects\\defect\\defects_journal";
    CString s;
    s.Format("Все нарушения эксплуатации на тепловой сети. %s", sezon_name(m_sezon_korrozia));

    remontTable(fn, "defect", "", s, "", nullptr, virt_data_geo);
}

int WordDefectZhurnal(CString doc_name, CString fn, int id);


void CGidrView::OnDefectWord1()
{
    CAdoFile* ado = m_cxema->m_ado;

    CString tmpName;
    tmpName.Format("%s\\%s", getenv("TMP"), _TR("Журнал нарушений.docx"));

    CString fn = "objects\\defect\\defects_journal_doc";

    map<CString, CString> map_fn;
    CString q = ispr_q_for_zhurnal(fn, "", &map_fn);

/*
    CString q = readQ(ado, fn, "", "");
    q.Replace("$state_cond$", "");
    sezon_condition(q, m_sezon_korrozia, m_sezon_date1, m_sezon_date2);
    set_ms_rs_condition(q, m_set_ms, m_set_rs);

    CString and_n1_fileID;
    and_n1_fileID.Format(" AND n1.fileID in (%s) ", m_cxema->m_par);
    q.Replace("$and_n1_fileID$", and_n1_fileID);
*/
    WordDefectZhurnal(tmpName, q, -1);
}

void InfoAreaDoc(CWnd* wnd);

void CGidrView::OnDefectWord2()
{
    InfoAreaDoc(this);
}

void CGidrView::OnDefectDocTable()
{
    CString fn = "objects\\defect\\defect_doc_table";
    CString s;
    s.Format("Сведения по участку тепловой сети. %s", sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
    map_fn["$state_cond$"] = "";

    CString and_n1_fileID;
    and_n1_fileID.Format(" AND n1.fileID in (%s) ", m_cxema->m_par);
    map_fn["$and_n1_fileID$"] = and_n1_fileID;

    remontTable(fn, s, "", s, "", &map_fn, virt_data_line_big);
}

void CGidrView::OnOsmotrZapros1()
{
    CString fn = "objects\\osmotr\\1_q_2";
    CString s;
    s.Format("Осмотренные фрагменты сети. %s", sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
    map_fn["$state_cond$"] = "";
    remontTable(fn, s, "", s, "", &map_fn, virt_data_geo);
}    


void CGidrView::OnOsmotrZapros2()
{
    CString fn = "objects\\osmotr\\2_q";
    CString s;
    s.Format("Осмотренные трубопроводы контуров. %s", sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
    map_fn["$state_cond$"] = "";

    remontTable(fn, "osmotr_zapros_po_trubam", "", s, "", &map_fn, virt_data_line_big_osmotr);
}    

void CGidrView::OnOsmotrZapros3()
{
    CString fn = "objects\\osmotr\\3_q";
    CString s;
    s.Format("Несмотренные трубопроводы контуров. %s", sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
    map_fn["$state_cond$"] = "";
    remontTable(fn, "osmotr_zapros_po_trubam", "", s, "", &map_fn, virt_data_line_big_osmotr);
}    


void CGidrView::OnOsmotrAnaliz()
{
    if (m_id_osmotr2 < 0) {
        m_id_osmotr2 = m_id_osmotr2_old;
    }
    if (m_id_osmotr2 < 0) {
        AfxMessageBox("Выберите контур осмотра");
        return;
    }
    
    CString fn = "objects\\osmotr\\osmotr_pipesection";
    CString s, q;

    CAdoFile* ado = getAdo(getPsAdoName());

    q.Format("SELECT name FROM osmotr WHERE id=%d", m_id_osmotr2);

    CString name = readTableValue(ado, q);

    s.Format("Анализ осмотра трубопроводов контура: %s. %s", 
    
        name, 
        sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
    CString ss;
    ss.Format("%d", m_id_osmotr2);

    map_fn["$id$"] = ss;
    remontTable(fn, s, "", s, "", &map_fn, virt_data_geo);
}    


void CGidrView::OnOsmotrAnaliz2()
{
    CString fn = "objects\\osmotr\\osmotr_1";
    CString s;
    s.Format("Анализ осмотра контуров. %s", sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
//    CString ss;

//    sezon_condition(q, m_sezon_korrozia, m_sezon_date1, m_sezon_date2);
//    set_ms_rs_condition(q, m_set_ms, m_set_rs);
//void CGidrView::remontTable(CString fn, CString tn, CString date_name, CString title, CString state_cond, const map<CString, CString>* map_fn, VIRT_DATA typ)
    
    remontTable(fn, s, "", s, "", &map_fn, virt_data_geo);
}    


void AktRasledovania2(CWnd* wnd, int id);
void KartaPovrezhdaemosti2(CWnd* wnd, int id);

void CGidrView::OnPopupDefect1()
{
    int id = m_ng;
    KartaPovrezhdaemosti2(this, id);
}

void CGidrView::OnPopupDefect2()
{
    int id = m_ng;
    AktRasledovania2(this, id);
}

int is_ms(CGraph2 *graph)
{
    auto p = graph->map_node.begin();

    while (p != graph->map_node.end()) {
        CNode2* pp = p->second;
        for (CLINE2* l = pp->lines; l; l = next(l)) {
            if (IsBegin(l) && bline(l)->line.isPjezo) {
              if (bline(l)->line.ms > 0) {
                return 1;
              }
              if (bline(l)->line.pipeSectionID <= 0) {
                return 2;
              }
            }
        }
        p++;
    }
    return -1;
}

CString getPar(set<int>& v);


void CGidrView::OnOpresAddPlan()
{
    map<CString, CString> map_param;
    map_param["vid_ispytaniID"] = "1";
    map_param["sostoyanie_opresID"] = "1";
//    map_param["stateID"] = "1";
//    map_param["plan_flag"] = "1";
//    map_param["utverdit"] = "0";

    set<int> set_ms;
    set<int> set_rs;

    CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();
    while (p != m_cxema->m_graph->map_node.end()) {
        for (CLINE2 *l = p->second->lines; l; l = next(l)) {
            if (bline(l)->line.isPjezo) {
                if (bline(l)->line.ms > 0) {
                    set_ms.insert(bline(l)->line.ms);
                }
                if (bline(l)->line.rs > 0) {
                    set_rs.insert(bline(l)->line.rs);
                }
            }
        }
        p++;
    }

    CString sms = getPar(set_ms); if (sms == "") sms = "-1";
    CString srs = getPar(set_rs); if (srs == "") srs = "-1";

    CString q;

    CString sm = "";
    CString sr = "";


    q.Format("SELECT id AS id_ms, 0 AS id_rs, 1 AS type, ms.opisanie_uchastka_ms AS name FROM uchastok_ms ms WHERE id IN (%s)\n"
        "UNION\n"
        "SELECT 0, id, 2, rs.naimenovanie_uchastka_rs FROM uchastok_rs rs WHERE id IN (%s)", sms, srs);

    if (m_cxema->m_ado->openTable0(q)) {
        while (!m_cxema->m_ado->isEOF()) {
            long typ = m_cxema->m_ado->read_long("type");
            CString s1 = m_cxema->m_ado->readStr("name");

            if (typ == 1) {
                if (sm != "") sm += "\n";
                sm += s1;
            }
            if (typ == 2) {
                if (sr != "") sr += "\n";
                sr += s1;
            }

            m_cxema->m_ado->MoveNext();
        }
        m_cxema->m_ado->closeTable();
    }

//    CString tx = "Подвергаются испытаниям (давлением 16 кгс / см2) трубопроводы следующих";
    CString tx = "Подвергаются испытаниям фрагменты";
    if (sm != "") {
        tx += "\nтепломагистралей:\n" + sm;
    }
    if (sr != "") {
        tx += "\nраспредсетей:\n" + sr;
    }
//    tx.Replace("\n", "¶");
    tx.Replace("\n", "\r\n");
    map_param["opisaniye_kontura"] = tx;

    SaveOpresNew("opres", "Контур опрессовки", "name", "Контур опрессовки", "date_opres", -1, m_id_opr, &map_param);
}



void CGidrView::OnRemontAddPlan()
{
    map<CString, CString> map_param;
    map_param["remontTypeID"] = "1";
    map_param["stateID"] = "1";
    map_param["plan_flag"] = "1";
    map_param["utverdit"] = "0";

    SaveOpresNew("remont2", "Ремонт", "otchet_po_defektu", "Контур ремонта", "data_osmotra", -1, m_id_remont2, &map_param);
}

void CGidrView::OnRemontAddCurrent()
{
    map<CString, CString> map_param;
    map_param["remontTypeID"] = "3";
    map_param["stateID"] = "2";
//    map_param["plan_flag"] = "0";
    map_param["utverdit"] = "2";

    SaveOpresNew("remont2", "Ремонт", "otchet_po_defektu", "Контур ремонта", "data_osmotra", -1, m_id_remont2, &map_param);
}

/*

void CGidrView::OnListRemont2()
{
}

void CGidrView::OnLastRemont2()
{
}
*/

void CGidrView::OnRemontTrub()
{
}

void CGidrView::OnRemontPlan()
{
    CString fn = "objects\\defect\\remonts_journal";
    CString s;
    s.Format("Контуры капитального/инвестиционного ремонтов. %s", sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
//    map_fn["$remontTypeID$"] = "IN (1, 2)";
//    map_fn["$stateID$"] = "IN (1, 2)";
    map_fn["$condition$"] = "(obj.remontTypeID IN (1, 2))";

    remontTable(fn, "remont2", "", s, "", &map_fn, virt_data_line_big);
}

void CGidrView::OnRemontCurrent()
{
    CString fn = "objects\\defect\\remonts_journal";
    CString s;
    s.Format("Контуры текущих ремонтов. %s", sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
    map_fn["$condition$"] = "(obj.remontTypeID IN (3))";
//    map_fn["$stateID$"] = "3";

    remontTable(fn, "remont2", "", s, "", &map_fn, virt_data_line_big);
}

void CGidrView::OnRemontProcess()
{
    CString fn = "objects\\defect\\remonts_journal";
    CString s;
    s.Format("Ремонт в процессе выполнения. %s", sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
    map_fn["$condition$"] = "(obj.stateID = 2)";

    remontTable(fn, "remont2", "", s, "", &map_fn, virt_data_line_big);
}

void CGidrView::OnRemontVypolneno()
{
    CString fn = "objects\\defect\\remonts_journal";
    CString s;
    s.Format("Законченные ремонты. %s", sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
    map_fn["$condition$"] = "(obj.stateID = 3)";

    remontTable(fn, "remont2", "", s, "", &map_fn, virt_data_line_big);
}

void CGidrView::OnRemontAll2()
{
    CString fn = "objects\\defect\\remonts_journal";
    CString s;
    s.Format("Ремонты. %s", sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
    map_fn["$condition$"] = "(1=1)";

    remontTable(fn, "remont2", "", s, "", &map_fn, virt_data_line_big);
}

void CGidrView::OnRemontUtverdit()
{
}

/*
void CGidrView::OnOpressovkaPassport()
{
}
*/


CString CGidrView::table_to_q_for_remont(CString fn)
{
    CString qq = readQ(m_cxema->m_ado, fn, "", m_cxema->m_par);

    set_ms_rs_condition(qq, m_set_ms, m_set_rs);
    sezon_condition(qq, m_sezon_korrozia, m_sezon_date1, m_sezon_date2);

    return qq;
}

void Remont_docx1(CWnd* wnd, CAdoFile* ado, CString q, int m_sezon_korrozia);
void Remont_docx2(CWnd* wnd, CAdoFile* ado, CString q, int m_sezon_korrozia);
void Remont_docx3(CWnd* wnd, CAdoFile* ado, CString q, int m_sezon_korrozia);


void CGidrView::OnRemontWord1()
{
    CString fn = "objects\\defect\\grafik_group";
    CString q = table_to_q_for_remont(fn);
    map<CString, CString> map_fn;
    Remont_docx1(this, m_cxema->m_ado, q, m_sezon_korrozia);
}

void CGidrView::OnRemontWord2()
{
    CString fn = "objects\\defect\\plan_by_season";
    CString q = table_to_q_for_remont(fn);
    map<CString, CString> map_fn;
    Remont_docx2(this, m_cxema->m_ado, q, m_sezon_korrozia);
}

void CGidrView::OnRemontWord3()
{
    CString fn = "objects\\defect\\plan_by_month";
    CString q = table_to_q_for_remont(fn);
    map<CString, CString> map_fn;
    Remont_docx3(this, m_cxema->m_ado, q, m_sezon_korrozia);
}
void CGidrView::OnRemontWord4()
{
}


void CGidrView::OnRemontAnaliz2()
{
    CString fn = "objects\\defect\\remonts_journal";
    CString s;
    s.Format("Анализ контуров ремонта. %s", sezon_name(m_sezon_korrozia));

//    CString q = table_to_q_for_remont(fn);
    map<CString, CString> map_fn;
//    Remont_docx1(this, m_cxema->m_ado, q);
    map_fn["$condition$"] = "(1=1)";
    remontTable(fn, s, "Анализ контуров ремонта", s, "", &map_fn, virt_data_line_big);
}

void CGidrView::OnRemontAnaliz()
{
    CString fn = "objects\\defect\\grafik_group";
    CString s;
    s.Format("График выполнения ремонтов. %s", sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
    map_fn["$condition$"] = "(1=1)";
    remontTable(fn, s, "", s, "", &map_fn, virt_data_line_big);
}

#include "CTeploset2.h"


void CGidrView::OnRemontVyborNach()
{
    CTeploset2 dlg(this, m_is_CTeploset);

    int nachalnik = 0;

    if (dlg.DoModal() == IDOK) {
        m_cxema->m_graph->reset();
        vydMsRs(m_cxema->m_graph, dlg.m_ms, dlg.m_rs);

        if (!m_cxema->m_graph->isMark()) {
           NeOpisanMessage(dlg.m_ms, dlg.m_rs);
           return;
        }

        MoveVyd();
        Invalidate();
    }
}


// Утвердить ремонт

void CGidrView::OnRemontPlanUtverdit()
{
    if (m_sezon_date1 == 0) {
        AfxMessageBox("Выберите отопительный сезон", MB_OK | MB_ICONINFORMATION);
        return;
    }
    CString str;
    str.Format(_TR("План ремонтов на отопительный сезон %d %d"), m_sezon_korrozia, m_sezon_korrozia + 1);

    CAdoFile* ado = m_cxema->m_ado;

    //    CString fn = "objects\\defect\\shurf_list";
    CString fn = "objects\\defect\\remonts_utverdit";
    CString q = readQ(ado, fn, "", -1);

    q.Replace("$condition$", "plan_flag=1");

    ispr_q(q);

    CFiltr3 dlg(this, str, IDD_SHURF_LIST);

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            int id = ado->read_long(0);
            CString sa = ado->readStr("Наименование/Адрес");
//            CString spr = ado->readStr("Признак сети");
//            CString sn = ado->readStr("Наименование участка");
//            CString su = ado->readStr("Участок/район эксплуатации");
//            CString nach = ado->readStr("Начальник участка");

//            CString s = u1 + " - " + u2 + " " + sa + " " + spr + " " + sn + " " + su + " " + nach;
            
            CString s = sa;

            dlg.Add(s, id, 1);

            ado->MoveNext();
        }
        ado->closeTable();
    }


    if (dlg.map_item.size() <= 0) {
        AfxMessageBox("Нет запланированных ремонтов");
        return;
    }


    int ret = dlg.DoModal();

    //    str.Format(_TR("Утвердить план шурфовок на отопительный сезон %d %d?"), m_sezon_korrozia, m_sezon_korrozia + 1);
        //    str.Format(_TR("Слишком близко к %s %s"), bline(line)->getTable(), bline(line)->getName());
    //    if (AfxMessageBox(str, MB_YESNO) == IDYES) {
    if (ret == IDOK && dlg.isNoEmpty()) {
/**
        CPropGridDlg* pm_cEditDlg = dialog_pr(this, "shurf_utverdit", WM_USER_SHURF_UTVERDIT_ALL, _TR("Утверждение плана шурфовок"), -1);

        if (pm_cEditDlg) {
            CString str_naznachenie = AfxGetApp()->GetProfileString("shurf_utverdit", "naznachenie", "");

            CString str_fio_utverzhdaemogo = AfxGetApp()->GetProfileString("shurf_utverdit", "fio_utverzhdaemogo", "");
            CString str_dolzhnost_utverzhdaemogoID = AfxGetApp()->GetProfileString("shurf_utverdit", "dolzhnost_utverzhdaemogoID", "");
            CString str_sluzhba_utverzhdaemogoID = AfxGetApp()->GetProfileString("shurf_utverdit", "sluzhba_utverzhdaemogoID", "");
            CString str_fio_viziruemogo_1 = AfxGetApp()->GetProfileString("shurf_utverdit", "fio_viziruemogo_1", "");
            CString str_dolzhnost_viziruemogoID_1 = AfxGetApp()->GetProfileString("shurf_utverdit", "dolzhnost_viziruemogoID_1", "");
            CString str_data_utverzhdeniya_plana_shurfovok = AfxGetApp()->GetProfileString("shurf_utverdit", "data_utverzhdeniya_plana_shurfovok", "");

            setPropValueInit(pm_cEditDlg, WM_USER_SHURF_UTVERDIT_ALL, "naznachenie", str_naznachenie);

            setPropValueInit(pm_cEditDlg, WM_USER_SHURF_UTVERDIT_ALL, "fio_utverzhdaemogo", str_fio_utverzhdaemogo);
            setPropValueInit(pm_cEditDlg, WM_USER_SHURF_UTVERDIT_ALL, "dolzhnost_utverzhdaemogoID", str_dolzhnost_utverzhdaemogoID);
            setPropValueInit(pm_cEditDlg, WM_USER_SHURF_UTVERDIT_ALL, "sluzhba_utverzhdaemogoID", str_sluzhba_utverzhdaemogoID);
            setPropValueInit(pm_cEditDlg, WM_USER_SHURF_UTVERDIT_ALL, "fio_viziruemogo_1", str_fio_viziruemogo_1);
            setPropValueInit(pm_cEditDlg, WM_USER_SHURF_UTVERDIT_ALL, "dolzhnost_viziruemogoID_1", str_dolzhnost_viziruemogoID_1);
            setPropValueInit(pm_cEditDlg, WM_USER_SHURF_UTVERDIT_ALL, "data_utverzhdeniya_plana_shurfovok", str_data_utverzhdeniya_plana_shurfovok);
        }

        m_str_list_shurf = "";

        for (auto it : dlg.map_item) {
            if (it.second.on) {
                CString ss;
                ss.Format("%d", it.second.id);
                if (m_str_list_shurf != "") m_str_list_shurf += ",";
                m_str_list_shurf += ss;
            }
        }
*/
    }
}


void CGidrView::OnOpresZadvAdd()
{
    Klassif* kls = m_geofile->m_kl_list->findKlN("opressovka_sektsioniruyuschaya_zadvizhka");

//    set_colorID1(1);
//    set_colorID2(1);

//    AddGeoValue("stateID", COleVariant(1L));
//    AddGeoValue("remontTypeID", COleVariant(1L));

    if (kls) addKls(kls);
}
    


// Проверяет, что все факторы риска у ремонта сделаны

bool CheckFaktoryRiskaForRemont(int id)
{
    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return false;

    CString fn = "objects\\defect\\validate_remont";
    CString ss;
    ss.Format("%d", id);

    CString q = readQ(ado, fn, "", "");
    q.Replace("$ID$", ss);

    long ok = readTableValueInt(ado, q);
    return ok;
}

void CGidrView::OnDefectBezUch()
{
    CString tn = "defect";
    CString ss = "Нарушения без участков";
    CString qq = readQ(m_cxema->m_ado, "objects/defect/defect_null_hps", "", 0);

    m_sezon_korrozia = -1;
    SetWindowText2();
    Invalidate();
    RunTableNiz(m_cxema->m_ado, tn, qq, ss, true, false, virt_data_geo);
}

void CGidrView::OnShurfBezUch()
{
    CString tn = "shurfy";
    CString ss = "Шурфовки без участков";
    CString qq = readQ(m_cxema->m_ado, "objects/defect/shurfy_null_hps", "", 0);

    m_sezon_korrozia = -1;
    SetWindowText2();
    Invalidate();
    RunTableNiz(m_cxema->m_ado, tn, qq, ss, true, false, virt_data_geo);
}


void CGidrView::OnAllPlanRemont2()
{
    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) {
        return;
    }

    CString fn = "objects\\defect\\all_plan_remont2";
    CString q = readQ(ado, fn, "", -1);
    
    ListOpresQ("remont2", "Все планы ремонтов", q, "col_5");
}

void CGidrView::OnAllPlanShurfy()
{
    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) {
        return;
    }

    CString fn = "objects\\defect\\all_plan_shurfy";
//    CString q = readQ(ado, fn, "", -1);

    CString q = ispr_q_for_zhurnal(fn, "", nullptr);




    ListOpresQ("shurfy", "Все планы шурфовок", q, "");
//    ListOpresQ("shurfy", "Все планы шурфовок", q, "col_4");
}
