#include "stdafx.h"
#include "gid6.h"

#include "ado.h"

#include "gidrView.h"

#include "PotrCx.h"
#include "geofile.h"
#include "CGidResist.h"
#include "CZdanieNagruzki.h"
#include "MMenu.h"

#include "PropertyGrid\test\PropGridDlg.h"

bool saveAlseco(CPropertyGrid* wnd, CString m_prop_table, CString m_prop_field, CString name, double otop_z_el, double otop_z_be, double otop_n, double gvs_op, double gvs_oo, double gvs_pr, double gvs_sm, double gvs_ps, double gvs_pw, double vent)
{
    if (m_prop_table == "generalizedConsumers") {
        wnd->SetNameValue("calcHLdep", otop_z_el+ otop_z_be);
        wnd->SetNameValue("calcHLindep", otop_n);
        wnd->SetNameValue("connectionSchemeID", otop_z_el > 0 ? 2 : 1);

        if (name != "") {
            wnd->SetNameValue("name", name);
        }
        wnd->SetNameValue("avgHLGVSopenSysFlow", gvs_op);
        wnd->SetNameValue("calcHLventil", vent);
    }
    else {

        if (otop_z_el + otop_z_be > 0 && otop_n > 0) {
            AfxMessageBox("Выбранные дома имеют разные схемы включения СО.\nСохранение договорных нагрузок невозможно");
            return false;
        }

        if (name != "") {
            wnd->SetNameValue("name", name);
        }
        CString get_nom_cxema(int gvpr, int gvsm, int gvps, int gvpw, int rez, int gvop, int gvoo, int otopln, int otoplz, int ventil, int elevat);

        double rez = 0;

        CString schemeNum = get_nom_cxema(gvs_pr > 0, gvs_sm > 0, gvs_ps > 0, gvs_pw > 0, rez > 0, gvs_op > 0, gvs_oo > 0, otop_n > 0, (otop_z_el + otop_z_be) > 0, vent > 0, otop_z_el <= 0);

        wnd->SetNameValue("schemeNum", schemeNum);

        wnd->initCxema(schemeNum);

        wnd->SetNameValue("calcHLdep", otop_z_el + otop_z_be);
        wnd->SetNameValue("calcHLindep", otop_n);

        wnd->SetNameValue("avgHLGVSopenFlow", gvs_op);
        wnd->SetNameValue("avgHLGVSopenRet", gvs_oo);

        wnd->SetNameValue("avgHLGVScloseParall", gvs_pr);
        wnd->SetNameValue("avgHLGVScloseMix", gvs_sm);
        wnd->SetNameValue("avgHLGVScloseConseq", gvs_ps);
        wnd->SetNameValue("avgHLGVSclosePreON", gvs_pw);

        wnd->SetNameValue("calcHLventil", vent);
    }

    return true;
}

CString getAlsecoTxt(CString mkr2, CString str2, CString dom2, double otop, double vent, double gvs, double par)
{
    CString txt = "";

    CString ss;

    if (mkr2 != "") {
        if (txt != "") txt += " ";
        txt += mkr2;
    }
    if (str2 != "") {
        if (txt != "") txt += " ";
        txt += str2;
    }
    if (dom2 != "") {
        if (txt != "") txt += " ";
        txt += dom2;
    }

    if (otop) {
        ss.Format("Qот=%f", otop/1e6);
        if (txt != "") txt += "\r\n";
        txt += ss;
    }
    if (gvs) {
        ss.Format("Qгвс=%f", gvs/1e6);
        if (txt != "") txt += "\r\n";
        txt += ss;
    }

    if (vent) {
        ss.Format("Qвент=%f", vent/1e6);
        if (txt != "") txt += "\r\n";
        txt += ss;
    }
    if (par) {
        ss.Format("Qпар=%f", par/1e6);
        if (txt != "") txt += "\r\n";
        txt += ss;
    }

    double q = otop+gvs+vent+par;

    if (q) {
        ss.Format("Qсум=%f", q);
        if (txt != "") txt += "\r\n";
        txt += ss;
    }

    return txt;
}


void AlsecoNagr(CPropertyGrid* wnd, int id)
{
//       pView->AlsecoNagr(pItem->m_id_bd);

    Klassif* kls = m_geofile->m_kl_list->findKlN(ZDANIYA_2);
    if (!kls) return;

    CString mkr, street, dom, kod, name;

    wnd->GetNameValue("externalCodeID", kod);
    wnd->GetNameValue("externalNodeName", name);

    wnd->GetNameValue("id_adr_mas", mkr);
    wnd->GetNameValue("street_nam", street);
    wnd->GetNameValue("number_1", dom);

    CZdanieNagruzki dlg(false, false, wnd);

    dlg.init(mkr, street, dom);

    if (dlg.DoModal() == IDOK) {

        double q = (dlg.m_OTOP+dlg.m_GVS+dlg.m_VENT+dlg.m_PAR)/1e6;

        wnd->SetNameValue("otop",dlg.m_OTOP/1e6  );
        wnd->SetNameValue("gvs", dlg.m_GVS/1e6   );
        wnd->SetNameValue("vent",dlg.m_VENT/1e6  );
        wnd->SetNameValue("par", dlg.m_PAR/1e6   );
        wnd->SetNameValue("nagr",(dlg.m_OTOP+dlg.m_GVS+dlg.m_VENT+dlg.m_PAR)/1e6   );

        wnd->SetNameValue("mkr2", dlg.m_mkr2);
        wnd->SetNameValue("street2", dlg.m_street2);
        wnd->SetNameValue("house2", dlg.m_house2);

        CString txt = "";

        CString ss;

        if (dlg.m_mkr2 != "") {
            if (txt != "") txt += " ";
            txt += dlg.m_mkr2;
        }
        if (dlg.m_street2 != "") {
            if (txt != "") txt += " ";
            txt += dlg.m_street2;
        }
        if (dlg.m_house2 != "") {
            if (txt != "") txt += " ";
            txt += dlg.m_house2;
        }

        if (dlg.m_OTOP) {
            ss.Format("Qот=%f", dlg.m_OTOP/1e6);
            if (txt != "") txt += "\r\n";
            txt += ss;
        }
        if (dlg.m_GVS) {
            ss.Format("Qгвс=%f", dlg.m_GVS/1e6);
            if (txt != "") txt += "\r\n";
            txt += ss;
        }

        if (dlg.m_VENT) {
            ss.Format("Qвент=%f", dlg.m_VENT/1e6);
            if (txt != "") txt += "\r\n";
            txt += ss;
        }
        if (dlg.m_PAR) {
            ss.Format("Qпар=%f", dlg.m_PAR/1e6);
            if (txt != "") txt += "\r\n";
            txt += ss;
        }

        if (q) {
            ss.Format("Qсум=%f", q);
            if (txt != "") txt += "\r\n";
            txt += ss;
        }

//        if (dlg.m_street2 == "" && dlg.m_mkr2 == "") {
        if (q == 0 ) {
            wnd->GetNameValue("number_1", txt);
            wnd->SetNameValue("mkr2", "");
            wnd->SetNameValue("street2", "");
            wnd->SetNameValue("house2", "");
        }

        wnd->SetNameValue("txt", txt);

//        CString qq;
//        qq.Format("UPDATE zdaniya_2 SET potrebitel='%s %s' WHERE id=%d", kod, name, id);
//        kls->m_ado->Execute(qq);
    }
}


CString getString(list<int>& v);

bool readAlseco(CAdoFile *ado, CString qq, double &otop_z_el, double &otop_z_be, double &otop_n, double &gvs_op, double &gvs_oo, double &gvs_pr, double &gvs_sm, double &gvs_ps, double &gvs_pw, double &vent)
{
    CString q;

    q.Format(
       "SELECT \n"
       "sum(otop * IIF(otop_cxema = 1, 1, 0)) AS otop_z_el, \n"
       "sum(otop * IIF(otop_cxema = 2, 1, 0)) AS otop_z_be, \n"
       "sum(otop * IIF(otop_cxema = 3, 1, 0)) AS otop_n, \n"

       "sum(gvs * IIF(gvs_cxema = 1, 1, 0)) AS gvs_op, \n"
       "sum(gvs * IIF(gvs_cxema = 2, 1, 0)) AS gvs_oo, \n"

       "sum(gvs * IIF(gvs_cxema = 3, 1, 0)) AS gvs_pr, \n"
       "sum(gvs * IIF(gvs_cxema = 4, 1, 0)) AS gvs_sm, \n"
       "sum(gvs * IIF(gvs_cxema = 5, 1, 0)) AS gvs_ps, \n"
       "sum(gvs * IIF(gvs_cxema = 6, 1, 0)) AS gvs_pw, \n"

       "sum(vent) AS vent, sum(par) AS par, sum(nagr) AS nagr FROM zdaniya_2 WHERE id IN (%s)", qq);

    vector<CString> vv;

    bool ret = readTableValues(ado, q, 11, vv);

    otop_z_el = atof(vv[0]);
    otop_z_be = atof(vv[1]);
    otop_n = atof(vv[2]);
    gvs_op = atof(vv[3]);
    gvs_oo = atof(vv[4]);
    gvs_pr = atof(vv[5]);
    gvs_sm = atof(vv[6]);
    gvs_ps = atof(vv[7]);
    gvs_pw = atof(vv[8]);
    vent = atof(vv[9]);
    double par = atof(vv[10]);

    return ret;
}


CString alseco_text(double otop_z_el, double otop_z_be, double otop_n, double gvs_op, double gvs_oo, double gvs_pr, double gvs_sm, double gvs_ps, double gvs_pw, double vent)
{
    CString ss, str = "";
    
    if (otop_z_el > 0) { ss.Format("отопление, зависимое элеваторное: %f,\n", otop_z_el); str += ss; }
    if (otop_z_be > 0) { ss.Format("отопление, зависимое безэлеваторное: %f,\n", otop_z_be); str += ss; }
    if (otop_n > 0) { ss.Format("отопление, независимое: %f,\n", otop_n); str += ss; }
    if (gvs_op > 0) { ss.Format("гвс, открытое из подачи: %f,\n", gvs_op); str += ss; }
    if (gvs_oo > 0) { ss.Format("гвс, открытое из обратки: %f,\n", gvs_oo); str += ss; }

    if (gvs_pr > 0) { ss.Format("гвс, паралельное: %f,\n", gvs_pr); str += ss; }
    if (gvs_sm > 0) { ss.Format("гвс, смешанное: %f,\n", gvs_sm); str += ss; }
    if (gvs_ps > 0) { ss.Format("гвс, последовательное: %f,\n", gvs_ps); str += ss; }
    if (gvs_pw > 0) { ss.Format("гвс, предвключенное: %f,\n", gvs_pw); str += ss; }

    if (vent > 0) { ss.Format("вентиляция: %f,\n", vent); str += ss; }

    return str;
}

bool AlsecoNagrMark(CPropertyGrid* wnd, int id, CGidrView* view, CString table, CString column)
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(ZDANIYA_2);
    if (!kls) return false;
    
    int n = m_geofile->NMark(kls);

    if (n == 0) return false;
    
    CString str;

    CString kod, name;

    wnd->GetNameValue("externalCodeID", kod);
    wnd->GetNameValue("externalNodeName", name);

    list<int> list_id;

    m_geofile->NMark(kls, list_id);

    CString q;
    CString id_list = getString(list_id);

    double otop_z_el, otop_z_be, otop_n, gvs_op, gvs_oo, gvs_pr, gvs_sm, gvs_ps, gvs_pw, vent;

    if (!readAlseco(kls->m_ado, id_list, otop_z_el, otop_z_be, otop_n, gvs_op, gvs_oo, gvs_pr, gvs_sm, gvs_ps, gvs_pw, vent)) {
        return false;
    }

    str.Format("Ввести нагрузки из выбранных зданий?\n"
               "Всего выбрано %d зданий\n"
               "Нагрузка, Гкал/ч:\n", n);

    str += alseco_text(otop_z_el, otop_z_be, otop_n, gvs_op, gvs_oo, gvs_pr, gvs_sm, gvs_ps, gvs_pw, vent);

    if (AfxMessageBox(str, MB_YESNO) == IDYES) {
        saveAlseco(wnd, table, column, "", otop_z_el, otop_z_be, otop_n, gvs_op, gvs_oo, gvs_pr, gvs_sm, gvs_ps, gvs_pw, vent);

        CString q;

        q.Format("UPDATE zdaniya_2 SET potrebitel=NULL WHERE potrebitel='%s %s'", kod, name);
        kls->m_ado->Execute(q);

        q.Format("UPDATE zdaniya_2 SET potrebitel='%s %s' WHERE id IN (%s)", kod, name, id_list);
        kls->m_ado->Execute(q);
    }

    return true;
}


#include "dialog_pr.h"
#include "wm_user.h"



void CGidrView::OnNagrAlmatyAdd1()
{
    CPropGridDlg *pm_cEditDlg = dialog_pr(this, "alseco1", WM_USER_ALSECO1_UPDATED, "Новый объект (МЖД) Alseco", -1);

    if (pm_cEditDlg) {
    }
}

void CGidrView::OnNagrAlmatyAdd2()
{
    CPropGridDlg *pm_cEditDlg = dialog_pr(this, "alseco2", WM_USER_ALSECO1_UPDATED, "Новый объект (остальные) Alseco", -1);

    if (pm_cEditDlg) {
    }
}


LRESULT CGidrView::OnAlseco1Updated(WPARAM wParam, LPARAM)
{
    CString N_mkr, N_street, N_house, N_house2, N_addr, N_adm_rayon, N_rayon,
        N_ist, N_tg, N_uchastok, N_dogovor, N_owner, N_name;
    CString N_otop, N_gvs, N_vent, N_par;
    CString N_numb;
    
    getPropValue0(WM_USER_ALSECO1_UPDATED, "N_addr", N_addr);  //A Адрес объекта
    getPropValue0(WM_USER_ALSECO1_UPDATED, "N_adm_rayon", N_adm_rayon);  //A Административный район
    getPropValue0(WM_USER_ALSECO1_UPDATED, "N_dogovor", N_dogovor);  //A № договора
    getPropValue0(WM_USER_ALSECO1_UPDATED, "N_gvs", N_gvs);  //F Нагрузка гвс
    getPropValue0(WM_USER_ALSECO1_UPDATED, "N_house", N_house);  //A Номер дома
    getPropValue0(WM_USER_ALSECO1_UPDATED, "N_house2", N_house2);  //A Примечание???
    getPropValue0(WM_USER_ALSECO1_UPDATED, "N_ist", N_ist);  //A Тепловой источник
    getPropValue0(WM_USER_ALSECO1_UPDATED, "N_mkr", N_mkr);  //A Микрорайон
    getPropValue0(WM_USER_ALSECO1_UPDATED, "N_name", N_name);  //A Наименование объекта
    getPropValue0(WM_USER_ALSECO1_UPDATED, "N_numb", N_numb);  //A № объекта
    getPropValue0(WM_USER_ALSECO1_UPDATED, "N_otop", N_otop);  //F Нагрузка отопление
    getPropValue0(WM_USER_ALSECO1_UPDATED, "N_owner", N_owner);  //A Наименование контрагента
    getPropValue0(WM_USER_ALSECO1_UPDATED, "N_par", N_par);  //F Нагрузка Пар, Ккал/час
    getPropValue0(WM_USER_ALSECO1_UPDATED, "N_rayon", N_rayon);  //A Эксплуатационный район
    getPropValue0(WM_USER_ALSECO1_UPDATED, "N_street", N_street);  //A Улица
    getPropValue0(WM_USER_ALSECO1_UPDATED, "N_tg", N_tg);  //A Температурный график объекта
    getPropValue0(WM_USER_ALSECO1_UPDATED, "N_uchastok", N_uchastok);  //A Тепловой участок по объекту
    getPropValue0(WM_USER_ALSECO1_UPDATED, "N_vent", N_vent);  //F Нагрузка Вентиляция, Ккал/час


    CString q;

    q.Format(
        "INSERT INTO nagruzki (\n"
        "city,\n"
        "mkr,\n"
        "street,\n"
        "house,\n"
        "house2,\n"
        "addr,\n"
        "adm_rayon,\n"
        "rayon,\n"
        "ist,\n"
        "tg,\n"
        "uchastok,\n"
        "otop,\n"
        "gvs,\n"
        "vent,\n"
        "par,\n"
        "dogovor,\n"
        "numb,\n"
        "owner,\n"
        "name\n"
        ")\n"
        "VALUES (\n"
        "'%s', \n"
        "'%s',\n" //  N_mkr
        "'%s',\n" //  N_street
        "'%s',\n" //  N_house
        "'%s',\n" //  N_house2
        "'%s',\n" //  N_addr
        "'%s',\n" //  N_adm_rayon
        "'%s',\n" //  N_rayon
        "'%s',\n" //  N_ist
        "'%s',\n" //  N_tg
        "'%s',\n" //  N_uchastok
        "%g,\n" //  N_otop
        "%g,\n" //  N_gvs
        "%g,\n" //  N_vent
        "%g,\n" //  N_par
        "'%s',\n" //  N_dogovor
        "%d,\n" //  N_numb
        "'%s',\n" // N_owner
        "'%s'" // N_name
        ")"
        ,
        "г.Алматы",
        N_mkr,
        N_street,
        N_house,
        N_house2,
        N_addr,
        N_adm_rayon,
        N_rayon,
        N_ist,
        N_tg,
        N_uchastok,
        atof(N_otop),
        atof(N_gvs),
        atof(N_vent),
        atof(N_par),
        N_dogovor,
        atoi(N_numb),
        N_owner,
        N_name
    );

    Klassif* kls = m_geofile->m_kl_list->findKlN(ZDANIYA_2);
    if (!kls) return 0;

    long affected;

    int id2 = kls->m_ado->ExecuteInsert(q, &affected);

    AfxMessageBox("Выполнено");
    
    return 0;
}

LRESULT CGidrView::OnAlseco2Updated(WPARAM wParam, LPARAM)
{
    return 0;
}


// Коприрует из TO в АЛСЕКО

long ado_executeInsert(CAdoFile* ado, const char* q, long* affected);


int copyZdan2(const char* name1, int id, const char* name2)
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(transl(name1));
    if (!kls) return -1;

    Klassif* kls2 = m_geofile->m_kl_list->findKlN(transl(name2));
    if (!kls2) return -1;


    int id2 = -1;

    CAdoFile* ado = kls->m_ado;

    CGeoObject* geo = kls->getGeoObjectById(id);

    if (!geo)  return -1;

    CString q;
    q.Format("SELECT * FROM %s WHERE ID=%d", name1, id);

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            CCoordList cl = geo->cl;

            CString mk = ado->readStr(transl("Микрорайон"));
            CString st = ado->readStr(transl("Улица"));
            CString dm = ado->readStr(transl("Дом"));

            q.Format("INSERT INTO %s (%s,%s,%s,shape) VALUES ('%s','%s','%s', %s)",
                transl(name2),
                "id_adr_mas",
                "street_nam",
                "number_1",
                mk, st, dm,
                cl.getWKT(kls->loc, kls->m_ado->m_type_of_net));

            long affected;
            id2 = ado_executeInsert(kls2->m_ado, q, &affected);
            if (id2) {
                CGeoObject* g = new CGeoObject(id2, cl);
                kls2->geo4.push_back(g);
            }
        }
        ado->closeTable();
    }
    return id2;
}



void CGidrView::OnDomaAlseco()
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(ZDANIYA_2);
    if (!kls) return;

    CNode2* node = m_menu_node;

    CMMenu menu(this, IDD_MENU, node->getName());

    if (node) {
        m_geofile->ClearMark();
        
//        AfxMessageBox(node->getName());

        CString q;
//        q.Format("SELECT id FROM %s WHERE potrebitel='%s'", ZDANIYA_2, node->getName());
    
        q.Format("SELECT id, CONCAT(IIF(mkr2 IS NULL, '', CONCAT(mkr2, ' ')),  street2, ' ',  house2) FROM %s WHERE potrebitel='%s'", ZDANIYA_2, node->getName());

        if (kls->m_ado->openTable0(q)) {
            while (!kls->m_ado->isEOF()) {
                int id = kls->m_ado->read_long(0);
                CString str = kls->m_ado->readStr(1);

                menu.Add(str, (void*)id);

     
                int nn = kls->GetNom2(id);

                if (nn > 0) {
                    kls->geo4[nn - 1]->mark = 1;
                }

                kls->m_ado->MoveNext();
            }
            kls->m_ado->closeTable();
        }
        Invalidate();
        
        if (menu.lst.GetSize() == 0) {
            CString s;
            s.Format("Нет зданий подключенных к %s", node->getName());
            AfxMessageBox(s);
        }
        else {
            if (menu.DoModal() == IDOK) {
                int id = (int)menu.getV();

                CGeoObject* geo = kls->getGeoObjectById(id);

                if (geo) {
                    MoveRect(geo->cl.GetFRect());
                }
            }
        }
    }
}



void CGidrView::OnUpdateAlseco(CCmdUI* pCmdUI)
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(ZDANIYA_2);

    pCmdUI->Enable(kls != 0);
}

void read_nagr(int id, double &otop, double &vent, double &gvs)
{
    CString q;
    otop = vent = gvs = 0.;

    Klassif* kls = m_geofile->m_kl_list->findKlN("zdaniya_tu");
    if (!kls) return;

    CAdoFile *ado = kls->m_ado;

    q.Format(
      "SELECT \n"
      "sum(tu.v_tom_chisle_otoplenie) AS otop,\n"
      "sum(tu.v_tom_chisle_ventilyatsiya) AS vent,\n"
      "sum(tu.v_tom_chisle_gvs_maks) AS gvs\n"
      "FROM tehnicheskie_usloviya tu WHERE tu.zdanie=%d AND tu.sostoyanie_dogovora=4\n", id);

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            otop = ado->read_double(0);
            vent = ado->read_double(1);
            gvs = ado->read_double(2);
        }
    }

    return;
}

CString ftoa(double num);


void CGidrView::OnGeoAlseco1()
{
    if (m_ng <= 0) return;

    double otop = 0, vent = 0, gvs = 0;
    read_nagr(m_ng, otop, vent, gvs);

    CPropGridDlg *pm_cEditDlg = dialog_pr(this, "alseco1", WM_USER_ALSECO1_UPDATED, "Новый объект (МЖД) Alseco", -1);

    if (pm_cEditDlg) {
        setPropValueInit(pm_cEditDlg, WM_USER_ALSECO1_UPDATED, "N_otop", ftoa(otop));
        setPropValueInit(pm_cEditDlg, WM_USER_ALSECO1_UPDATED, "N_gvs", ftoa(gvs));
        setPropValueInit(pm_cEditDlg, WM_USER_ALSECO1_UPDATED, "N_vent", ftoa(vent));
    }
}

void CGidrView::OnGeoAlseco2()
{
    if (m_ng <= 0) return;
    double otop = 0, vent = 0, gvs = 0;

    read_nagr(m_ng, otop, vent, gvs);

    CPropGridDlg *pm_cEditDlg = dialog_pr(this, "alseco2", WM_USER_ALSECO1_UPDATED, "Новый объект (остальные) Alseco", -1);

    if (pm_cEditDlg) {
        setPropValueInit(pm_cEditDlg, WM_USER_ALSECO1_UPDATED, "N_otop", ftoa(otop));
        setPropValueInit(pm_cEditDlg, WM_USER_ALSECO1_UPDATED, "N_gvs", ftoa(gvs));
        setPropValueInit(pm_cEditDlg, WM_USER_ALSECO1_UPDATED, "N_vent", ftoa(vent));
    }
}

