// gidrView.cpp : implementation of the CGidrView class
//

#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
//#include "DbTable.h"
#include "InputDia.h"
#include "Input2.h"
#include "gidrView.h"
#include "ado.h"

#include "mmenu.h"
#include "Win.h"

#include "TuFind.h"
#include "TuZapros.h"
#include "tu_p.h"

static CString PRIS = "Присоединенная нагрузка источников";
static CString kot = "Источники тепла";
static CString ZdaniaTU = "Здания ТУ";
static CString TechUsl = "Технические условия";


void moveGeoId(CGidrView *gidr, const char *tn, int id);
CString getParams(CAdoFile *ado, const char *tn);


void CGidrView::OnTuFind()
{
    CTuFind dlg;

    Klassif *kls = m_geofile->m_kl_list->findKlN(transl(ZdaniaTU));
    if (!kls || !kls->m_ado) {
        AfxMessageBox(_TR("Не подключен файл Технических условий"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    if (dlg.DoModal() == IDOK) {
        CString qq;
        qq.Format("%d %d", dlg.m_id_nagr, dlg.m_id_tu);

        if (dlg.m_id_nagr > 0) {
            moveGeoId(this, transl("Здания потребителей"), dlg.m_id_nagr);
        }
        else if (dlg.m_id_tu > 0) {
            moveGeoId(this, transl(ZdaniaTU), dlg.m_id_tu);
        }
        //    AfxMessageBox(qq, MB_OK|MB_ICONINFORMATION);
    }
}


void CGidrView::OnTuLine()
{
    if (!isEditGeo("")) return;

    Klassif *kls = m_geofile->m_kl_list->findKlN(transl(STR_TRUB_REKONSTR));

    if (!kls) {
        AfxMessageBox(_TR("Не подключен файл Технических условий"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    if (addGeo(kls)) {
        CString str;
        str.Format(_TR("Начинайте создавать %s"), kls->RusName());
        AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
    }
}


void findTuDog();

//#include "FindTUDOG.h"

void CGidrView::OnFindTuDog()
{
#ifdef COOLDOC
    //  findTuDog();


    CFindTUDOG dlg(this);

    int ret = dlg.DoModal();

    if (ret == 1) {
        Klassif *kls = m_geofile->m_kl_list->findKlN(transl("Здания потребителей"));
        if (!kls) return;

        CString q = "";

        if (dlg.m_edit_find_nd != "") {
            q.Format(
                "SELECT Организации.ID, Организации.[Номер договора], Организации.[Наименование объекта], Организации.[Адрес объекта], Организации.[Назначение объекта], Организации.[Наименование контрагента], Организации.Площадь, Организации.Этажность, Организации.[Нагрузка \"Отопление\"], Организации.[Нагрузка \"Вентиляция\"], Организации.[Нагрузка \"ГВС\"], Организации.[Нагрузка \"Пар\"], Организации.[Номер(а) прибора учета], Организации.[Эксплуатационный район по объекту], Организации.[Эксплуатационный участок по объекту], Организации.[Административный район по объекту] "
                "FROM Организации WHERE [Номер договора]='%s'", dlg.m_edit_find_nd);
        }
        else if (dlg.m_edit_find_name != "") {
            q.Format(
                "SELECT Организации.ID, Организации.[Номер договора], Организации.[Наименование объекта], Организации.[Адрес объекта], Организации.[Назначение объекта], Организации.[Наименование контрагента], Организации.Площадь, Организации.Этажность, Организации.[Нагрузка \"Отопление\"], Организации.[Нагрузка \"Вентиляция\"], Организации.[Нагрузка \"ГВС\"], Организации.[Нагрузка \"Пар\"], Организации.[Номер(а) прибора учета], Организации.[Эксплуатационный район по объекту], Организации.[Эксплуатационный участок по объекту], Организации.[Административный район по объекту] "
                "FROM Организации WHERE [Назначение объекта] LIKE '*%s*' OR  [Наименование контрагента] LIKE '*%s*'", dlg.m_edit_find_name, dlg.m_edit_find_name);
        }

        if (q != "") {
            RunTableMap(kls->m_database, _TR("Ненайденные организации"), q, getHelp_Map(), 0, 1, FALSE, FALSE, FALSE, FALSE);
        }
    }
    if (ret == 2) {

        Klassif *kls = m_geofile->m_kl_list->findKlN(transl(ZdaniaTU));
        if (!kls) return;

        CString q = "";

        /*
        if (m_edit_find_nd != "") {
        q.Format(
        "SELECT Организации.ID, Организации.[Номер договора], Организации.[Наименование объекта], Организации.[Адрес объекта], Организации.[Назначение объекта], Организации.[Наименование контрагента], Организации.Площадь, Организации.Этажность, Организации.[Нагрузка \"Отопление\"], Организации.[Нагрузка \"Вентиляция\"], Организации.[Нагрузка \"ГВС\"], Организации.[Нагрузка \"Пар\"], Организации.[Номер(а) прибора учета], Организации.[Эксплуатационный район по объекту], Организации.[Эксплуатационный участок по объекту], Организации.[Административный район по объекту] "
        "FROM Организации WHERE [Номер договора]='%s'", m_edit_find_nd);
        }
        else
        */
        if (dlg.m_edit_find_name_tu != "") {
            q.Format(
                "SELECT * "
                "FROM [Технические условия] WHERE [Наименование организации, запрашивающей ТУ] LIKE '*%s*' OR  [Наименование объекта] LIKE '*%s*'", dlg.m_edit_find_name_tu, dlg.m_edit_find_name_tu);
        }

        if (q != "") {
            RunTableMap(kls->m_database, _TR("ТУ"), q, getHelp_Map(), 0, 1, FALSE, FALSE, FALSE, FALSE);
        }
    }
#endif
}

#include "join2.h"

void CGidrView::OnTuTable()
{
    Klassif *kls = m_geofile->m_kl_list->findKlN(transl(ZdaniaTU));
    if (!kls) {
        AfxMessageBox(_TR("Не подключен файл Технических условий"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    CString params = getParams(kls->m_ado, transl(TechUsl));
    CString q;
    
    CString tn = transl(TechUsl);


    q.Format("SELECT %s FROM %s t LEFT JOIN stateTu st ON st.id=t.sostoyanie_dogovora", ppp, transl(TechUsl));
//    q.Format("SELECT %s FROM %s tu  LEFT JOIN stateTu ON stateTu.is=tu.sostoyanie_dogovora", params, transl(TechUsl));


//     Join2 j;
//     int n0 = j.initJoin(kls->m_ado, tn);
//     j.allJoin(kls->m_ado, tn, n0);
//     q = j.getQ_sokr();


    RunTableTU(kls->m_database, TechUsl, q, -1);
//    RunTableTU(kls->m_database, TechUsl, 0, -1);
}

void CGidrView::OnTuTrub()
{
    Klassif *kls = m_geofile->m_kl_list->findKlN(transl(STR_TRUB_REKONSTR));
    if (!kls) {
        AfxMessageBox(_TR("Не подключен файл Технических условий"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    RunTableMap(kls->m_database, STR_TRUB_REKONSTR, NULL, 0);
}


void CGidrView::OnTuTableNeiz()
{
    Klassif *kls = m_geofile->m_kl_list->findKlN(transl(ZdaniaTU));
    if (!kls) {
        AfxMessageBox(_TR("Не подключен файл Технических условий"), MB_OK|MB_ICONINFORMATION);
        return;
    }
    CString q;

//    q.Format( "SELECT * FROM %s WHERE (%s = 0 OR Здание IS NULL)", transl(TechUsl), transl("Здание"));
    q.Format("SELECT %s FROM %s t LEFT JOIN stateTu st ON st.id=t.sostoyanie_dogovora  WHERE (zdanie = 0 OR zdanie IS NULL)", ppp, transl(TechUsl));

    RunTableTU(kls->m_ado->m_schema, TechUsl, q, 0);
}



void CGidrView::OnTuYear()
{
  Klassif *kls = m_geofile->m_kl_list->findKlN(transl(ZdaniaTU));
  if (!kls) {
    AfxMessageBox(_TR("Не подключен файл Технических условий"), MB_OK|MB_ICONINFORMATION);
    return;
  }

  CString q;

  q.Format("SELECT DISTINCT YEAR(%s) FROM %s ORDER BY YEAR(%s)", transl("Дата выдачи ТУ"), transl(TechUsl), transl("Дата выдачи ТУ"));

  CMMenu menu(this, IDD_MENU, _TR("Выберите год"));

  CAdoFile *ado = kls->m_ado;
  
  if (ado->openTable0(q)) {
     while ( !ado->isEOF() ) {
        CString s = ado->readStr(0);
        long y = ado->read_long(0);
        menu.Add(s, (void*)y);

        ado->MoveNext();
    }
    ado->closeTable();
  }

  if (menu.DoModal() == IDOK) {
//    q.Format("SELECT * FROM %s WHERE YEAR(%s)=%ld", transl(TechUsl), transl("Дата выдачи ТУ"), (long) menu.getV());
    q.Format("SELECT %s FROM %s t LEFT JOIN stateTu st ON st.id=t.sostoyanie_dogovora WHERE YEAR(%s)=%ld", ppp, transl(TechUsl), transl("Дата выдачи ТУ"), (long) menu.getV());

//    RunTableTU(ado->m_schema, transl(TechUsl), q, 0);
    RunTableTU(ado->m_schema, TechUsl, q, 0);
  }
}

void CGidrView::OnTuIst()
{
  Klassif *kls = m_geofile->m_kl_list->findKlN(transl(ZdaniaTU));
  if (!kls) {
    AfxMessageBox(_TR("Не подключен файл Технических условий"), MB_OK|MB_ICONINFORMATION);
    return;
  }

  CString q = readFile(argpath()+"tu/1.sql");

  RunTableTU(kls->m_database, TechUsl, q, -1);



/*
  CString q;

  q.Format("SELECT DISTINCT (%s) FROM %s", transl("Источник"), transl(TechUsl));

  
  CMMenu menu(this, IDD_MENU2_1, _TR("Выберите Источник"));

  CAdoFile *ado = kls->m_ado;
  
  if (ado->openTable0(q)) {

      while ( !ado->isEOF() ) {

        CString s = ado->readStr(0);
        if (s != "") menu.Add(s);

        ado->MoveNext();
      }
    ado->closeTable();
  }

  if (menu.DoModal() == IDOK) {
    q.Format("SELECT * FROM %s WHERE %s='%s'", transl(TechUsl), transl("Источник"), menu.m_strText);
    RunTableTU(kls->m_database, TechUsl, q, -1);
  }
*/
}

void CGidrView::OnTuRay()
{
  Klassif *kls = m_geofile->m_kl_list->findKlN(ZdaniaTU);
  if (!kls) {
    AfxMessageBox(_TR("Не подключен файл Технических условий"), MB_OK|MB_ICONINFORMATION);
    return;
  }

  CString q;

  q.Format("SELECT DISTINCT ([Район эксплуатации]) FROM %s", transl(TechUsl));

  CMMenu menu(this, IDD_MENU2_1, _TR("Выберите Район эксплуатации"));

  CAdoFile *ado = kls->m_ado;
  
  if (ado->openTable0(q)) {

      while ( !ado->isEOF() ) {

        CString s = ado->readStr(0);
        if (s != "") menu.Add(s);

        ado->MoveNext();
      }
    ado->closeTable();
  }

  if (menu.DoModal() == IDOK) {
    q.Format("SELECT * FROM %s WHERE %s='%s'", transl(TechUsl), transl("Район эксплуатации"), menu.m_strText);
//    RunTableTU(kls->FileName(), _TR("Технические условия по Району эксплуатации") , q, getHelp_Map(), 0, MAP, FALSE, FALSE, FALSE, FALSE);
    RunTableTU(kls->m_database, TechUsl, q, -1);
  }
}

void CGidrView::OnTuKam()
{
  Klassif *kls = m_geofile->m_kl_list->findKlN(ZdaniaTU);
  if (!kls) {
    AfxMessageBox(_TR("Не подключен файл Технических условий"), MB_OK|MB_ICONINFORMATION);
    return;
  }

  CString q;

  q.Format("SELECT DISTINCT ([Камера]) FROM %s", transl(TechUsl));

  CMMenu menu(this, IDD_MENU2_1, _TR("Выберите камеру"));

  CAdoFile *ado = kls->m_ado;
  
  if (ado->openTable0(q)) {
      while ( !ado->isEOF() ) {

        CString s = ado->readStr(0);
        if (s != "") menu.Add(s);

        ado->MoveNext();
      }
    ado->closeTable();
  }

  if (menu.DoModal() == IDOK) {
    q.Format("SELECT * FROM %s WHERE %s='%s'", transl(TechUsl), transl("Камера"), menu.m_strText);
//    RunTableTU(kls->FileName(), _TR("Технические условия по камерам") , q, getHelp_Map(), 0, MAP, FALSE, FALSE, FALSE, FALSE);
    RunTableTU(kls->m_database, TechUsl, q, -1);
  }
}


//#include "TuZapros.h"
//#include "tu_p.h"


void CGidrView::OnTuSost()
{
    CTuZapros dlg(this);

    Klassif *kls = m_geofile->m_kl_list->findKlN(transl(ZdaniaTU));
    if (!kls) {
        AfxMessageBox(_TR("Не подключен файл Технических условий"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    CAdoFile *ado = kls->m_ado;

    CString q, qs;

    {
        q.Format("SELECT DISTINCT YEAR(%s) FROM %s ORDER BY YEAR(%s)", transl("Дата выдачи ТУ"), transl(TechUsl), transl("Дата выдачи ТУ"));

        if (ado->openTable0(q)) {
            while (!ado->isEOF()) {
                CString s = ado->readStr(0);
                long y = ado->read_long(0);

                if (s == "") {
                    dlg.AddYear(_TR("Не заданный"), 0);
                }
                else {
                    dlg.AddYear(s, (void*)y);
                }

                ado->MoveNext();
            }
            ado->closeTable();
        }
    }
    {
        q.Format("SELECT DISTINCT (%s) FROM %s ORDER BY %s", transl("Район эксплуатации"), transl(TechUsl), transl("Район эксплуатации"));

        if (ado->openTable0(q)) {

            while (!ado->isEOF()) {

                CString s = ado->readStr(0);
                if (s == "") {
                    dlg.AddRaj(_TR("Не заданный"));
                }
                else {
                    dlg.AddRaj(s);
                }

                ado->MoveNext();
            }
            ado->closeTable();
        }
    }

    {
        q.Format("SELECT DISTINCT (%s) FROM %s ORDER BY %s", transl("Источник"), transl(TechUsl), transl("Источник"));

        if (ado->openTable0(q)) {
            while (!ado->isEOF()) {

                CString s = ado->readStr(0);
                if (s == "") {
                    dlg.AddIst(_TR("Не заданный"));
                }
                else {
                    dlg.AddIst(s);
                }
                ado->MoveNext();
            }
            ado->closeTable();
        }
    }


    if (dlg.DoModal() == IDOK) {
        q.Format("SELECT %s FROM %s t LEFT JOIN stateTu st ON st.id=t.sostoyanie_dogovora WHERE %s", ppp, transl(TechUsl), dlg.m_q);
        qs.Format("SELECT %s FROM %s WHERE %s", ppp2, transl(TechUsl), dlg.m_q);

        CString qq;

        qq.Format("%s UNION ALL %s", q, qs);

//        RunTableTU(kls->m_ado->m_schema, transl(TechUsl), qq, 0);
        RunTableTU(kls->m_ado->m_schema, TechUsl, qq, 0);
    }
}


void CGidrView::OnTuIstO()
{
  Klassif *kls = m_geofile->m_kl_list->findKlN(ZdaniaTU);
  if (!kls) {
    AfxMessageBox(_TR("Не подключен файл Технических условий"), MB_OK|MB_ICONINFORMATION);
    return;
  }

  CString q;

  q.Format("SELECT DISTINCT YEAR(%s) FROM %s ORDER BY YEAR(%s)", transl("Дата выдачи ТУ"), transl(TechUsl), transl("Дата выдачи ТУ"));

  CAdoFile *ado = kls->m_ado;

  CMMenu menu(this, IDD_MENU, _TR("Выберите год"));
 
  if (ado->openTable0(q)) {
      while ( !ado->isEOF() ) {
        CString s = ado->readStr(0);
        long y = ado->read_long(0);
        if (s != "") menu.Add(s, (void*)y);

        ado->MoveNext();
      }
    ado->closeTable();
  }

  if (menu.DoModal() == IDOK) {

    CString qq = readFile(argpath()+"tu/tu.sql");

    qq.Replace("$YEAR$", menu.m_strText);

//    qq.Replace("$PS_STRUCT$", getPSAlma());

    if (ado->openTable0(qq)) {
      ado->closeTable();
      RunTableMap(kls->m_database, _TR("Баланс по источникам"), qq, -1);
    }
  }
}


int copyZdan(const char *name1, int id, const char *name2);


bool CGidrView::tu_new(const CFPoint &point)
{
  CString str;
  if (!isEditGeo("")) return false;
  if ( isPot() ) return false;
  if (!m_geofile) return false;

  CString tn = ZdaniaTU;

//  AfxMessageBox(tn, MB_OK|MB_ICONINFORMATION);

  int ngeo = m_geofile->findXY3(point, D5* geom.masx, 3, transl(tn));
  if (ngeo > 0) {
    return false;
  }

//  AfxMessageBox("!", MB_OK|MB_ICONINFORMATION);


  bool ret = false;
  char fn[256];

  m_num = m_geofile->findXY(point, D5* geom.masx, 3, fn);

  if (m_num) {
    copyZdan(fn, m_num, tn);
    ret = tu_info(point);
    InvalidateG();
  }
  else {
    Klassif *kls = m_geofile->m_kl_list->findKlN(transl(tn));

    if (kls) {
      if (addGeo(kls)) {
         CString str;
         str.Format(_TR("Начинайте создавать %s"), kls->RusName());
   //      AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
      }
      else {
        return false;
      }

      CDC *m_dc;


      regim = R_BEGIN_DRAW_LINE;
      setTypeLine(m_kls->typ);
      m_pt1 = m_pt2 = point;
      m_NP.push_back(m_pt1);

      if ((m_dc = BeginPaint()) != NULL) {
        XDraw(m_dc, false);
        XDraw(m_dc, true);
        DrawRect(m_dc, point);
        EndPaint(m_dc);
      }
    }
    return false;
  }
  return ret;
}


bool CGidrView::tu_info(const CFPoint &point)
{
    CString str;
    if (!isEditGeo("")) return false;
    if (isPot()) return false;
    if (!m_geofile) return false;

    CString tn = transl(ZdaniaTU);

    bool ret = false;
    int ngeo = m_geofile->findXY3(point, D5* geom.masx, 3, tn);
    if (ngeo) {
        GeoInfo(tn, ngeo, false);
    }
    return ret;
}

bool CGidrView::tu_del(const CFPoint &point)
{
    CString str;
    if (!isEditGeo("")) return false;
    if (isPot()) return false;
    if (!m_geofile) return false;


    CString fn0 = ZdaniaTU;
    CString fn = transl(fn0);



    int n = m_geofile->findXY3(point, D5* geom.masx, 3, fn);

    if (!n) return false;


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
    return true;
}


struct Nagr {
    double r_ov, r_gvs, r_s, poter, pr_o, pr_v, pr_g, pr_p;
    double ust;
};


void copyNagr(CAdoFile *ado, int year, int old_year)
{
    CString q;

    set<long> set_kot;
    set<long> set_kot_new;

    q.Format("SELECT * FROM %s", transl(kot));


    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long("ID");
            set_kot.insert(id);
            ado->MoveNext();
        }
        ado->closeTable();
    }

    map<long, Nagr> map_nagr;

    q.Format("SELECT * FROM %s WHERE god=%d", transl(PRIS), old_year);

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long("ID2");
            Nagr nagr;
            nagr.r_ov = ado->read_double(transl("Располагаемая мощность ОВ"));
            nagr.r_gvs = ado->read_double(transl("Располагаемая мощность ГВС средняя"));
            nagr.r_s = ado->read_double(transl("Располагаемая мощность суммарная"));
            nagr.poter = ado->read_double(transl("Нормативные тепловые потери"));
            nagr.pr_o = ado->read_double(transl("Присоединенная мощность Отопление"));
            nagr.pr_v = ado->read_double(transl("Присоединенная мощность Вентиляция"));
            nagr.pr_g = ado->read_double(transl("Присоединенная мощность ГВС максимальная"));
            nagr.pr_p = ado->read_double(transl("Присоединенная мощность Пар"));

            nagr.ust = ado->read_double("ustanovlennaya_moschnost");
            map_nagr[id] = nagr;
            ado->MoveNext();
        }
    }

    q.Format("SELECT * FROM %s WHERE god=%d", transl(PRIS), year);

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long("ID2");
            set_kot_new.insert(id);
            ado->MoveNext();
        }
    }

    set<long>::const_iterator it = set_kot.begin();
    for (; it != set_kot.end(); it++) {
        ado->AddNew(transl(PRIS));
        ado->write("ID2", *it);
        ado->write("god", (long)year);

        map<long, Nagr>::const_iterator it2 = map_nagr.find(*it);
        if (it2 != map_nagr.end()) {
            ado->write(transl("Располагаемая мощность ОВ"), it2->second.r_ov);
            ado->write(transl("Располагаемая мощность ГВС средняя"), it2->second.r_gvs);
            ado->write(transl("Располагаемая мощность суммарная"), it2->second.r_s);
            ado->write(transl("Нормативные тепловые потери"), it2->second.poter);
            ado->write(transl("Присоединенная мощность Отопление"), it2->second.pr_o);
            ado->write(transl("Присоединенная мощность Вентиляция"), it2->second.pr_v);
            ado->write(transl("Присоединенная мощность ГВС максимальная"), it2->second.pr_g);
            ado->write(transl("Присоединенная мощность Пар"), it2->second.pr_p);
            ado->write(transl("ustanovlennaya_moschnost"), it2->second.ust);
        }
        ado->Update();
    }
}

void copyNagr0(CAdoFile *ado, int year)
{
    CString q;

    set<long> set_kot;
    set<long> set_kot_new;


    if (ado->openTable0(kot)) {
        while (!ado->isEOF()) {
            long id = ado->read_long("ID");
            set_kot.insert(id);
            ado->MoveNext();
        }
        ado->closeTable();
    }


    q.Format("SELECT * FROM %s WHERE god=%d", transl(PRIS), year);
    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long("ID2");
            set_kot_new.insert(id);
            ado->MoveNext();
        }
    }

    set<long>::const_iterator it = set_kot.begin();
    for (; it != set_kot.end(); it++) {
        ado->AddNew(PRIS);
        ado->write("ID2", *it);
        ado->write("god", (long)year);
        ado->Update();
    }
}

void CGidrView::OnPrisNagr()
{
  CString s;
  CString q;

  Klassif *kls = m_geofile->m_kl_list->findKlN(transl(ZdaniaTU));
  if (!kls) {
      AfxMessageBox(_TR("Не подключен файл Технических условий"), MB_OK|MB_ICONINFORMATION);
      return;
  }

  CAdoFile *ado = kls->m_ado;


  q.Format("SELECT DISTINCT god FROM %s  ORDER BY god", transl(PRIS));


  long y = 2011;

  set<long> set_year;

  if (ado->openTable0(q)) {
      while ( !ado->isEOF() ) {
        y = ado->read_long(0);
        set_year.insert(y);
        ado->MoveNext();
    }
    ado->closeTable();
  }

  int n = 0;

      CString text;
      text.Format("%d", y+1);
      CInputDialog inD(this, "", _TR("Введите год"), text);

      if (inD.DoModal() == IDOK) {

    long year = atoi(inD.m_strInput);

    
    set<long>::const_iterator it = set_year.find(year);

    if (it != set_year.end()) {
      CString q;
      q.Format(_TR("Данные за %d год уже существуют"), year);
      AfxMessageBox(q, MB_OK|MB_ICONINFORMATION);
      return;
    }

    q.Format("SELECT K.ID, K.%s, K.%s, P.god, P.ustanovlennaya_moschnost, P.%s, P.%s, P.%s, P.%s, P.%s, P.%s, P.%s, P.%s FROM %s AS K INNER JOIN %s AS P ON K.ID = P.ID2 WHERE P.god=%d", 
    transl("Наименование"),
transl("Район эксплуатации"),
// "ustanovlennaya_moschnost",
transl("Располагаемая мощность ОВ"),
transl("Располагаемая мощность ГВС средняя"),
transl("Располагаемая мощность суммарная"),
transl("Нормативные тепловые потери"), 
transl("Присоединенная мощность Отопление"), 
transl("Присоединенная мощность Вентиляция"),
transl("Присоединенная мощность ГВС максимальная"),
transl("Присоединенная мощность Пар"),
transl(kot), 
transl(PRIS),
year);


    long n = getCountTable2(ado, q);


    if (n == 0) {
      if (AfxMessageBox(_TR("Копировать данные?"), MB_YESNO) == IDYES) {
        copyNagr(ado, year, y);
      }
      else {
        copyNagr0(ado, year);
      }
    }
    RunTableMap(kls->m_database, PRIS, q, -1);
  }
}

#include "Ultimate Grid\TableDialog.h"


void CGidrView::OnPrisNagrEdit()
{
  CString s;
  CString q;


  CMMenu menu(this, IDD_MENU_TU, _TR("Выберите год"));

  long y = 2011;

  Klassif *kls = m_geofile->m_kl_list->findKlN(transl(ZdaniaTU));
  if (!kls) {
      AfxMessageBox(_TR("Не подключен файл Технических условий"), MB_OK|MB_ICONINFORMATION);
      return;
  }

  CAdoFile *ado = kls->m_ado;

  q.Format("SELECT DISTINCT god FROM %s ORDER BY god", transl(PRIS));

  if (ado->openTable0(q)) {
      while ( !ado->isEOF() ) {
        s = ado->read(0);
        y = ado->read_long(0);
        menu.Add(s, (void*)y);

        ado->MoveNext();
    }
    ado->closeTable();
  }
  

  int n = 0;

  int ret = menu.DoModal();

  if (ret == IDOK) {

    long year = (long)menu.getV();

      int n_ret = 1;

      q.Format("SELECT * FROM %s WHERE god=%d", transl(PRIS), year);
      
//    q.Format("SELECT K.ID, K.naimenovanie, K.%s, P.god, P.ustanovlennaya_moschnost, P.%s, P.%s, P.%s, P.%s, P.%s, P.%s, P.%s, P.%s "
//    q.Format("SELECT P.ID, P.ID2, K.naimenovanie, P.ustanovlennaya_moschnost, P.%s, P.%s, P.%s, P.%s, P.%s, P.%s, P.%s, P.%s "
    q.Format("SELECT ID, ID2, ID2, ustanovlennaya_moschnost, %s, %s, %s, %s, %s, %s, %s, %s "
    " FROM %s "
//    " FROM %s AS P "
//    " INNER JOIN %s AS P ON K.ID = P.ID2"
    " WHERE god=%d", 

// transl("Район эксплуатации"),
transl("Располагаемая мощность ОВ"),
transl("Располагаемая мощность ГВС средняя"),
transl("Располагаемая мощность суммарная"),
transl("Нормативные тепловые потери"), 
transl("Присоединенная мощность Отопление"), 
transl("Присоединенная мощность Вентиляция"),
transl("Присоединенная мощность ГВС максимальная"),
transl("Присоединенная мощность Пар"),
transl(kot), 
transl(PRIS),
year);


    q.Format("SELECT P.ID, P.ID2, K.naimenovanie, P.ustanovlennaya_moschnost, P.%s, P.%s, P.%s, P.%s, P.%s, P.%s, P.%s, P.%s, P.god "
//    q.Format("SELECT ID, ID2, ID2, god, ustanovlennaya_moschnost, %s, %s, %s, %s, %s, %s, %s, %s "
    " FROM %s AS P "
    " INNER JOIN %s AS K ON K.ID = ID2"
    " WHERE god=%d", 

// transl("Район эксплуатации"),
transl("Располагаемая мощность ОВ"),
transl("Располагаемая мощность ГВС средняя"),
transl("Располагаемая мощность суммарная"),
transl("Нормативные тепловые потери"), 
transl("Присоединенная мощность Отопление"), 
transl("Присоединенная мощность Вентиляция"),
transl("Присоединенная мощность ГВС максимальная"),
transl("Присоединенная мощность Пар"),
transl(PRIS),
transl(kot), 
year);

      CTableDialog dlg(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), PRIS, q, n_ret, virt_data_edit_add_del);
      dlg.SetKey("god", year);
      dlg.setHead("", transl(PRIS));
      
      dlg.DoModal();

      return;



//    q.Format("SELECT K.ID, K.naimenovanie, K.%s, P.god, P.%s, P.%s, P.%s, P.%s, P.%s, P.%s, P.%s, P.%s "
//    " FROM %s AS K "
//    " INNER JOIN %s AS P ON K.ID = P.ID2"
//    " WHERE P.god=%d", 

    q.Format("SELECT K.ID, K.naimenovanie, K.%s, P.god, P.ustanovlennaya_moschnost, P.%s, P.%s, P.%s, P.%s, P.%s, P.%s, P.%s, P.%s "
    " FROM %s AS K "
    " INNER JOIN %s AS P ON K.ID = P.ID2"
    " WHERE P.god=%d", 

transl("Район эксплуатации"),
transl("Располагаемая мощность ОВ"),
transl("Располагаемая мощность ГВС средняя"),
transl("Располагаемая мощность суммарная"),
transl("Нормативные тепловые потери"), 
transl("Присоединенная мощность Отопление"), 
transl("Присоединенная мощность Вентиляция"),
transl("Присоединенная мощность ГВС максимальная"),
transl("Присоединенная мощность Пар"),
transl(kot), 
transl(PRIS),
year);

    long n = getCountTable2(ado, q);

    if (n == 0) {
      if (AfxMessageBox(_TR("Копировать данные?"), MB_YESNO) == IDYES) {
//      if (MessageBox("1", "2", MB_YESNO) == IDYES) {
        copyNagr(ado, year, y);
      }
    }

//    RunTableMap(kls->m_database, PRIS, q, -1);
    viewQ2(kls->m_ado, kls->m_database, q, transl(PRIS), PRIS, virt_data_edit, "", PRIS);
  }
  else if (ret == IDC_DEL_TU) {
    long year = (long)menu.getV();

    q.Format("DELETE FROM %s WHERE god=%d", transl(PRIS), year);

    ado->Execute(q);
  }
  else if (ret == IDC_ADD_TU) {
    OnPrisNagr();
  }
}

void CGidrView::OnPrisNagrDel()
{
  Klassif *kls = m_geofile->m_kl_list->findKlN(transl(ZdaniaTU));
  if (!kls) {
      AfxMessageBox(_TR("Не подключен файл Технических условий"), MB_OK|MB_ICONINFORMATION);
      return;
  }

  CAdoFile *ado = kls->m_ado;
  

  CString s;
  CString q;

  q.Format("SELECT DISTINCT god FROM %s", transl(PRIS));

  CMMenu menu(this, IDD_MENU, _TR("Выберите год"));


  long y = 2011;

  if (ado->openTable0(q)) {

      while ( !ado->isEOF() ) {

        s = ado->read(0);
        y = ado->read_long(0);
        menu.Add(s, (void*)y);

        ado->MoveNext();
      }
      ado->closeTable();
  }

  int n = 0;


  if (menu.DoModal() == IDOK) {

    long year = (long)menu.getV();

    if (AfxMessageBox(_TR("Подтвердите удаление"), MB_YESNO) == IDYES) {
      q.Format("DELETE FROM %s WHERE god=%d", PRIS, year);
      ado->Execute(q);
    }
  }
}

#include "ado.h"

void CGidrView::OnFindTuIst()
{
    Klassif *kls = m_geofile->m_kl_list->findKlN(transl(ZdaniaTU));
    if (!kls) {
        AfxMessageBox(_TR("Не подключен файл Технических условий"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    CString q;
    CMMenu menu(this, IDD_MENU2_1, _TR("Выберите источник тепла"));

    q.Format("SELECT DISTINCT %s FROM %s ORDER BY %s", transl("Источник"), transl(TechUsl), transl("Источник"));

    CAdoFile *ado = kls->m_ado;

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {

            while (!ado->isEOF()) {
                CString s = ado->readStr(0);
                if (s != "") menu.Add(s, 0);
                ado->MoveNext();
            }
        }
        ado->closeTable();
    }

    if (menu.DoModal() == IDOK) {

        q.Format("SELECT DISTINCT Z.ID FROM %s T, %s Z WHERE T.%s=Z.ID AND T.%s='%s'", transl(TechUsl), transl(ZdaniaTU), transl("Здание"), transl("Источник"), menu.m_strText);

        set<long> set_id;

        if (ado->openTable0(q)) {
            if (!ado->isEOF()) {
                while (!ado->isEOF()) {
                    int id = ado->read_long(0);
                    set_id.insert(id);
                    ado->MoveNext();
                }
            }
            ado->closeTable();
        }

        int i;
        for (i = 0; i < kls->geo4.size(); i++) {
            kls->geo4[i]->mark = false;
        }

        double x1 = 1e10, x2 = -1e10, y1 = 1e10, y2 = -1e10;

        for (i = 0; i < kls->geo4.size(); i++) {
            set<long>::const_iterator it = set_id.find(kls->geo4[i]->nom);
            if (it != set_id.end()) {
                kls->geo4[i]->mark = true;
                CFRect rect = kls->geo4[i]->rect;

                if (rect.left != 0 || rect.right != 0 || rect.top != 0 || rect.bottom != 0) {
                    x1 = min(x1, rect.left);
                    y1 = min(y1, -rect.bottom);
                    x2 = max(x2, rect.right);
                    y2 = max(y2, -rect.top);
                }

            }
        }
        Invalidate();
        if (x1 != 1e10) {
            MoveRect(CFRect(x1, -y2, x2, -y1));
            moveXY(CFPoint((x1 + x2) / 2, -(y1 + y2) / 2));
        }
    }
}

void CGidrView::OnQGvs()
{
    CString s = AfxGetApp()->GetProfileString(szSection, "coef24", "2.4");

    CInputDialog inD(this, _TR("Коэффициент перевода"), _TR("Коэффициент перевода k=Qгвmax/Qгвср"), s);
    if (inD.DoModal() == IDOK) {
        AfxGetApp()->WriteProfileString(szSection, "coef24", inD.m_strInput);
    }
}
