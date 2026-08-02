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


CNode2 *find_zn0(CCxema* m_cxema, CNode2 *node, double &length);

//const RS *getRS();
const MAG *getMAG();

// Пересчитывает в количество по участку


void upr(list<vector<CString> > & v1) 
{
  map<vector<CString>, int> map_v;
  
  list<vector<CString> >::const_iterator it = v1.begin();

  for (; it != v1.end(); it++) {
    map<vector<CString>, int>::const_iterator itm = map_v.find(*it);
    if (itm == map_v.end()) {
      map_v[*it] = 1;
    }
    else {
      map_v[*it] ++;
    }
  }

  v1.clear();
  {
    map<vector<CString>, int>::const_iterator it = map_v.begin();

    for (; it != map_v.end(); it++) {
      vector<CString> vv2;

      for (int i = 0; i < it->first.size(); i++) {
        vv2.push_back(it->first[i]);
      }
      CString str;
      str.Format("%d", it->second);
      vv2.push_back(str);

      v1.push_back(vv2);
    }
  }
}

struct MAP_GEO
{
  map<CLINE2*, list<int> > map_geo;
};

CFRect getRect(CLINE2 *l);

void print_sheet0(VYD_LINES &vyd_l);

void print_sheetRemont(FILE *f, VYD_LINES &vyd_l, CNode2 *nr, CNode2 *nm);
void print_sheetShurf(FILE *f, VYD_LINES &vyd_l, CNode2 *nr, CNode2 *nm);
//void print_sheetKapital(HTML &h, VYD_LINES &vyd_l, CNode *nr, CNode *nm);

void print_sheet3(CGraph2 *graph, FILE *f, VYD_LINES &vyd_l, CNode2 *nr, CNode2 *nm);
void print_sheet4(CGraph2 *graph, FILE *f, VYD_LINES &vyd_l, CNode2 *nr, CNode2 *nm);
void print_sheet5(CGraph2 *graph, FILE *f, VYD_LINES &vyd_l, CNode2 *nr, CNode2 *nm);
void print_sheet6(CGraph2 *graph, FILE *f, VYD_LINES &vyd_l, CNode2 *nr, CNode2 *nm);
void print_sheet7(CGraph2 *graph, FILE *f, VYD_LINES &vyd_l, CNode2 *nr, CNode2 *nm);
void print_sheet8(CGraph2 *graph, FILE *f, VYD_LINES &vyd_l, CNode2 *nr, CNode2 *nm);
void print_sheet9(CGraph2 *graph, FILE *f, int nomer_uch);

void print_head(FILE *f, const char *title);
void print_index1(FILE *f, const char *title);
void print_index(FILE *f, const char *title, const char *fn);
void print_index2(FILE *f);
void print_doc_end(FILE *f);
FILE *print_doc(FILE *fi, const char *fn, const char *title, void (*filtr) (CString &s1) = NULL);


CString d2a(double n) 
{
  CString s;
  s.Format("%g", n);
  return s;
}

//CGidrView *getView();

CString getV(map<CString, CString>& map_RE, CString id)
{
  map<CString, CString>::iterator it = map_RE.find(id);
  if (it != map_RE.end()) return it->second;
  return "";
}


void htmlReplace(map<CString, CString> &map_val, CString &str, CString s1, CString tn, CString fn)
{
    fn = transl(fn);
    fn.MakeLower();



    if (str.Find(s1) != -1) {

        if (s1 == "$owner$") {
            int qq;
            qq = 1;
        }


        CString val = getV(map_val, fn);

        val = getReal(tn, fn, val);

        str.Replace(s1, val); // Балансовая стоимость
    }
}

#include "join2.h"

FILE *print_doc_main(FILE *fi, const char *fn, const char *fn2, const char *title, const char *table, int is_rs, int kod)
{
    if (!kod) return NULL;

    CGidrView *pView = getView();
    if (!pView) return 0;
    CCxema *cxema = pView->m_cxema;

    CString q;

    CString tn1 = transl(table);

    q.Format("SELECT * FROM %s WHERE ID=%d", transl(table), kod);

    CAdoFile *ado = cxema->m_ado;
    map<CString, CString> map_val;

    if (ado) {
        Join2 j;
        int n0 = j.initJoin(ado, tn1);
        j.allJoin(ado, tn1, n0);
        q = j.getQ();
        CString s;
        s.Format(" WHERE _TAB1_.ID=%d", kod);
        q += s;



        //    InitTab(db.m_strFileName, table, false);

        if (ado->openTable0(q)) {
            if (!ado->isEOF()) {
                int n = ado->NFlds();

                for (int i = 0; i < n; i++) {

                    CString s = ado->FieldName(i);
                    CString v = ado->readStr(i);
                    if (s != _TR("Ответственное лицо")) {
                        v = getReal(table, s, v);
                    }
                    s.MakeLower();
                    map_val[s] = v;
                }

                CString v = ado->readStr(_TR("Магистраль"));
                v = getReal(table, _TR("Источник тепла"), v);
                map_val[transl("Источник тепла")] = v;

                v = ado->readStr("kod_istochnika");
                map_val["istochniki_teplosnabzheniya"] = v;


                v = ado->readStr(14);
                map_val["owner"] = v;

                ado->MoveNext();
            }
            ado->closeTable();
        }
    }




    FILE *f, *g;

    print_index(fi, title, fn);

    CString tmpName;
    //  tmpName.Format("%s\\html\\2\\%s", argpath(), fn2);
    tmpName.Format("%s\\%s", getenv("TMP"), fn);

    g = fopen(tmpName, "w");

    if (!g) return NULL;

    f = fopenexe(CString("html\\") + fn, "r");

    if (!f) return NULL;

    while (!feof(f)) {
        CString str;
        char s[10240];
        fgetstr(s, 10230, f);
        str = s;

        //    CString ss1 = getV(map_val, "name_rs");
        //    CString ss2 = getV(map_val, "kod_rs");

        //    CString ss2 = getV(map_val, "kod_rs");

        //    if (ss1 == "") ss1 = ss2;



        //    str.Replace("$namePS$", ss1);
        //    str.Replace("$name$", ss1);   // Описание участка МС

//        htmlReplace(map_val, str, "$name$", table, _TR("Магистраль")); // Год постройки
        htmlReplace(map_val, str, "$name$", table, "naimenovanie_magistrali");

        htmlReplace(map_val, str, "$name_rs$", table, "uzel_podklyucheniya"); 



        htmlReplace(map_val, str, "$god_postr$", table, _TR("Год постройки")); // Год постройки
        htmlReplace(map_val, str, "$god_ekspl$", table, _TR("Год ввода в эксплуатацию")); // Год ввода в эксплуатацию

        htmlReplace(map_val, str, "$vid_seti$", table, _TR("Вид сети")); // Вид сети
        htmlReplace(map_val, str, "$rabsr$", table, _TR("Рабочая среда")); // Вид сети

        htmlReplace(map_val, str, "$inv_nom$", table, _TR("Инвентарный номер"));  // Инвентарный номер
        htmlReplace(map_val, str, "$proekt$", table, _TR("Проектная организация")); // Проектная организация
    //    mag.proekt = getReal(tn,_TR("Проектная организация"), getV(map_val, _TR("Проектная организация")));

        htmlReplace(map_val, str, "$proekt$", table, _TR("Проектная организация")); // Проектная организация
        htmlReplace(map_val, str, "$kadastr$", table, _TR("Кадастровый номер"));

        htmlReplace(map_val, str, "$balans$", table, _TR("Балансовая стоимость")); // Балансовая стоимость
        htmlReplace(map_val, str, "$dlina$", table, _TR("Общая длина трассы")); // Общая длина трассы
        htmlReplace(map_val, str, "$balans$", table, _TR("Балансовая стоимость")); // Балансовая стоимость
        htmlReplace(map_val, str, "$god_balans$", table, _TR("Год оценки балансовой стоимости")); // Год оценки балансовой стоимости
        htmlReplace(map_val, str, "$rabsr$", table, _TR("Рабочая среда"));  // Рабочая среда
        htmlReplace(map_val, str, "$rab_dav$", table, _TR("Рабочее давление")); // Рабочее давление
        htmlReplace(map_val, str, "$rab_temp$", table, _TR("Рабочая температура")); // Рабочая температура
        htmlReplace(map_val, str, "$nomer_pr$", table, _TR("Номер проекта")); // Номер проекта
        htmlReplace(map_val, str, "$nomer_pas$", table, _TR("Номер паспорта")); // Номер паспорта
        htmlReplace(map_val, str, "$energos$", table, _TR("Энергосистема")); // Энергосистема
        htmlReplace(map_val, str, "$uch$", table, _TR("Участок")); // Участок
//        htmlReplace(map_val, str, "$nazn$", table, _TR("Назначение"));     // Назначение РС
        htmlReplace(map_val, str, "$nazn$", table, "naznachenie_rs");     // Назначение РС


        htmlReplace(map_val, str, "$date_zap$", table, _TR("Дата заполнения")); // Дата заполнения
    //    htmlReplace(map_val, str, "$ist$", table, "kod_ist"); // Источник
//        htmlReplace(map_val, str, "$ist$", table, "naimenovanie_istochnika"); // Источник
        htmlReplace(map_val, str, "$ist$", table, "istochniki_teplosnabzheniya"); // Источник





//        htmlReplace(map_val, str, "$ist$", table, _TR("Источник тепла")); // Источник

        htmlReplace(map_val, str, "$owner$", table, "owner");    // Предприятие владелец



//        htmlReplace(map_val, str, "$owner$", table, "naimenovanie");    // Предприятие владелец
//        htmlReplace(map_val, str, "$owner$", table, "predpriyatie_vladelets");    // Предприятие владелец

        
        htmlReplace(map_val, str, "$adres$", table, _TR("Адрес предприятия владельца"));    // Адрес предприятия владельца
        htmlReplace(map_val, str, "$reg_num$", table, _TR("Регистрационный номер"));  // Регистрационный номер
        htmlReplace(map_val, str, "$otv$", table, _TR("Ответственное лицо"));  // Ответственное лицо

        fprintf(g, "%s\n", str);
    }
    fclose(f);

    return g;
}
  


#if 0
FILE *print_doc_main(FILE *fi, const char *fn, const char *fn2, int is_rs, int m_rs, const char *kod)
{
  if (!kod) return NULL;

  CGidrView *pView = getView();
  if (!pView) return 0;
  CCxema *cxema = pView->m_cxema;
  CGidFile *file = NULL;

  map<CString, RasCx>::const_iterator it2 =  cxema->map_PC.begin();

  for (; it2 != cxema->map_PC.end(); it2++) {
    RasCx pc = it2->second;
    if (pc.kod_rs == CString(kod)) {
      file = pc.file;
    }
  }

  if (!file) return 0;

  CString q;

  q.Format("SELECT * FROM [%s] WHERE kod_rs='%s'", ST_PC, kod);

  CDbFile *db = file->m_dbf;
  map<CString, CString> map_val;

  InitTab(file->name, ST_PC, false);

  if (db->openTable(q)) {
    if (!db->rs->IsEOF()) {
      int n = db->NFlds();

      for (int i = 0; i < n; i++) {
        char s[256];

        db->FieldName(i+1, s);
        CString v = db->read(i+1);
        if (strcmp(s, _TR("Ответственное лицо"))) {
          v = getReal(ST_PC, s, v);
        }
        map_val[s] = v;        
      }

      db->rs->MoveNext();
    }
    db->closeTable();
  }

  FILE *f, *g;

  print_index(fi, _TR("Общая характеристика"), fn);

  CString tmpName;
//  tmpName.Format("%s\\html\\2\\%s", argpath(), fn2);
  tmpName.Format("%s\\%s", getenv("TMP"), fn);
  
  g = fopen(tmpName, "w");

  if (!g) return NULL;

  f = fopenexe(CString("html\\")+fn, "r");

  if (!f) return NULL;

  while (!feof(f)) {
    CString str;
    char s[10240];
    fgetstr(s, 10230, f);
    str = s;

    CString ss1 = getV(map_val, "name_rs");
    CString ss2 = getV(map_val, "kod_rs");
    if (ss1 == "") ss1 = ss2;


    str.Replace("$namePS$", ss1);
    str.Replace("$name$", ss1);   // Описание участка МС

    htmlReplace(map_val, str, "$god_postr$", _TR("Год постройки")); // Год постройки
    htmlReplace(map_val, str, "$god_ekspl$", _TR("Год ввода в эксплуатацию")); // Год ввода в эксплуатацию
    
    htmlReplace(map_val, str, "$vid_seti$", _TR("Вид сети")); // Вид сети
    htmlReplace(map_val, str, "$rabsr$", _TR("Вид сети")); // Вид сети

    htmlReplace(map_val, str, "$inv_nom$", _TR("Инвентарный номер"));  // Инвентарный номер
    htmlReplace(map_val, str, "$proekt$", _TR("Проектная организация")); // Проектная организация
//    mag.proekt = getReal(tn,_TR("Проектная организация"), getV(map_val, _TR("Проектная организация")));

    htmlReplace(map_val, str, "$proekt$", _TR("Проектная организация")); // Проектная организация


    htmlReplace(map_val, str, "$balans$", _TR("Балансовая стоимость")); // Балансовая стоимость
    htmlReplace(map_val, str, "$dlina$", _TR("Общая длина трассы")); // Общая длина трассы
    htmlReplace(map_val, str, "$balans$", _TR("Балансовая стоимость")); // Балансовая стоимость
    htmlReplace(map_val, str, "$god_balans$", _TR("Год оценки балансовой стоимости")); // Год оценки балансовой стоимости
    htmlReplace(map_val, str, "$rabsr$", _TR("Рабочая среда"));  // Рабочая среда
    htmlReplace(map_val, str, "$rab_dav$", _TR("Рабочее давление")); // Рабочее давление
    htmlReplace(map_val, str, "$rab_temp$", _TR("Рабочая температура")); // Рабочая температура
    htmlReplace(map_val, str, "$nomer_pr$", _TR("Номер проекта")); // Номер проекта
    htmlReplace(map_val, str, "$nomer_pas$", _TR("Номер паспорта")); // Номер паспорта
    htmlReplace(map_val, str, "$energos$", _TR("Энергосистема")); // Энергосистема
    htmlReplace(map_val, str, "$uch$", _TR("Участок")); // Участок
    htmlReplace(map_val, str, "$nazn$", _TR("Назначение"));     // Назначение РС
    htmlReplace(map_val, str, "$date_zap$", _TR("Дата заполнения")); // Дата заполнения
    htmlReplace(map_val, str, "$ist$", "kod_ist"); // Источник

    htmlReplace(map_val, str, "$owner$", _TR("Предприятие владелец"));    // Предприятие владелец
    htmlReplace(map_val, str, "$adres$", _TR("Адрес предприятия владельца"));    // Адрес предприятия владельца
    htmlReplace(map_val, str, "$reg_num$", _TR("Регистрационный номер"));  // Регистрационный номер
    htmlReplace(map_val, str, "$otv$", _TR("Ответственное лицо"));  // Ответственное лицо
    

/*

    if (is_rs) {
      const RS *rs = getRS(m_rs);
      str.Replace("$namePS$", rs->namePS);
      str.Replace("$inv_nom$", rs->inv_nom);  // Инвентарный номер
      str.Replace("$opis$", rs->opis);     // Наименование участка РС
      str.Replace("$namePS1$", rs->namePS1);  // Узел подключения
      str.Replace("$namePS2$", rs->namePS2);  // Конечный узел
      str.Replace("$kod2$", rs->kod2);     // kod2
      str.Replace("$uzel2$", rs->uzel2);    // uzel2
      str.Replace("$date_inv$", rs->date_inv); // Дата присвоения инвентарного номера


CString sss = rs->date_zap+"\xA0";
//sss.Replace(".",":");


      str.Replace("$date_zap$", sss); // Дата заполнения

//      str.Replace("$date_zap$", rs->date_zap); // Дата заполнения
      str.Replace("$reg_num$", rs->reg_num);  // Регистрационный номер
      str.Replace("$owner$", rs->owner);    // Предприятие владелец
      str.Replace("$adres$", rs->adres);    // Адрес предприятия владельца
      str.Replace("$nazn$", rs->nazn);     // Назначение РС
      str.Replace("$rabsr$", rs->rab_sr);  // Рабочая среда
      str.Replace("$rab_dav$", rs->rab_dav); // Рабочее давление
      str.Replace("$rab_temp$", rs->rab_temp); // Рабочая температура
      str.Replace("$srok$", rs->srok);     // Расчетный срок службы
      str.Replace("$resurs$", rs->resurs);   // Расчетный ресурс
      str.Replace("$pusk$", rs->pusk);     // Расчетное число пусков
      str.Replace("$pr_doc$", rs->pr_doc);   // Проектные документы
    }
    else {
//      const MAG *mag = getMAG();
      const MAG *mag = getMag2(m_rs);
      str.Replace("$name$", mag->name);   // Описание участка МС
CString sss = mag->date_zap+"\xA0";
//sss.Replace(".",":");


//AfxMessageBox(mag->date_zap, MB_OK|MB_ICONINFORMATION);
      str.Replace("$date_zap$", sss); // Дата заполнения
      str.Replace("$energos$", mag->energos); // Энергосистема
      str.Replace("$raion$", mag->raion); // Район
      str.Replace("$nazn$", mag->nazn);     // Назначение РС
      str.Replace("$nomer_pas$", mag->nomer_pas); // Номер паспорта
      str.Replace("$vid_seti$", mag->vid_seti); // Вид сети
      str.Replace("$proekt$", mag->proekt); // Проектная организация
      str.Replace("$nomer_pr$", mag->nomer_pr); // Номер проxекта
      str.Replace("$dlina$", mag->dlina); // Общая длина трассы
      str.Replace("$rab_dav$", mag->rab_dav); // Рабочее давление
      str.Replace("$rab_temp$", mag->rab_temp); // Рабочая температура
      str.Replace("$god_postr$", mag->god_postr); // Год постройки
      str.Replace("$god_ekspl$", mag->god_ekspl); // Год ввода в эксплуатацию
      str.Replace("$balans$", mag->balans); // Балансовая стоимость
      str.Replace("$god_balans$", mag->god_balans); // Год оценки балансовой стоимости

      str.Replace("$uch$", mag->uch); // Участок

      str.Replace("$resurs$", mag->resurs); 
      str.Replace("$pusk$", mag->pusk); 



      const MAG *mag1 = getMag1(mag->id_mag);

      if (mag1) {
        str.Replace("$ist$", mag1->ist); // Источник
      }
    }
*/

    fprintf(g, "%s\n", str);
  }
  fclose(f);

  return g;
}
#endif


#if 0

void print_ut(FILE *g, CDbFile &dbf)
{
  char kod1[40], name1[256], p1[2];
  char kod2[40], name2[256], p2[2];

  
  dbf.readLine(1, kod1, name1, p1);
  dbf.readLine(2, kod2, name2, p2);

  double diam = dbf.read_double("diametr_usl");
  double dlina = dbf.read_double("dlina");

  fprintf(g, "<tr>");

  fprintf(g, "<td>%s %s %s</td>", kod1, name1, p1);
  fprintf(g, "<td>%s %s %s</td>", kod2, name2, p2);
  
  fprintf(g, "<td>%g</td>", diam);
  fprintf(g, "<td>%g</td>", dlina);
  
  fprintf(g, "</tr>");
}

#endif

void print_ut2_1(HTML &h, CLINE2 *l);
void print_ut2(HTML &h, CLINE2 *l);
void print_td_povt(HTML &h, int n);


void print_ut2(FILE *g, CLINE2 *l)
{
  CNode2 *n1 = where(l);
  CNode2 *n2 = other(l);

  if (napr(l)*bline(l)->line.pod.q > 0) {
    CNode2 *n = n1; n1= n2; n2 = n;
  }

  fprintf(g, "<td>%s</td>", n1->getNamePTfull());
  fprintf(g, "<td>%s</td>", n2->getNamePTfull());
}

CString getTubingTypeID(int tubingTypeID)
{
  switch (tubingTypeID) {
  case 1 : return "канальная";
  case 2 : return "бесканальная";
  case 3 : return "подвальная";
  case 4 : return "надземная";
  case 5 : return "обвязка";
  }

  return "";
}


void print_ut(FILE *g, CLINE2 *l)
{
  fprintf(g, "<tr>");

  CNode2 *n1 = where(l);
  CNode2 *n2 = other(l);

  if (napr(l)*bline(l)->line.pod.q < 0) {
    CNode2 *n = n1; n1 = n2; n2 = n;
  }

  fprintf(g, "<td>%s</td>", n1->getNamePTfull());
  fprintf(g, "<td>%s</td>", n2->getNamePTfull());

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
}

static CSS css;


//void CGidrView::PaspRSMS(int is_rs, CNode *nl, CNode *n1, int nomer_uch)

#include <errno.h>

void CGidrView::PaspRSMS(CNode2 *nl, CNode2 *n1, int ms_rs, int id)
{
  CWaitCursor wait;

  CString tmpName;
//  tmpName.Format("%s\\index.html", getenv("TMP"));
//  tmpName.Format("%s\\html\\2\\index.html", argpath());
  tmpName.Format("%s\\index2.html", getenv("TMP"));

//  if (!kod) return;

//  if (m_id_MS) is_rs = 0;
//  if (m_id_RS) is_rs = 1;


  FILE *f = fopen(tmpName, "w");
  if (!f) {
      AfxMessageBox(strerror(errno), MB_OK|MB_ICONINFORMATION);
      return;
  }

  setlocale(LC_NUMERIC, "");

  print_index1(f, "");

  FILE *g = 0;

  VYD_LINES vyd_l;
  vyd_l.init(m_cxema->m_graph, false, false);

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
      g = print_doc_main(f, "CommonA.htm", "Common3.htm", _TR("Общая характеристика"), _TR("Участок МС"), ms_rs, id); 
      print_doc_end(g);
    }
    else if (ms_rs == 1) {
      g = print_doc_main(f, "CommonB.htm", "Common4.htm", _TR("Общая характеристика"), _TR("Участок РС"), ms_rs,  id);  // Распредсеть
      print_doc_end(g);
    }
  }


  g = print_doc(f, "a1.html", _TR("Трубы")); 

  map<int, CLINE2*>::const_iterator it = vyd_l.map_line.begin();

  double dlP = 0, dlO = 0, vP = 0, vO = 0;

  for (; it != vyd_l.map_line.end(); it++) {
    CLINE2 *l = it->second;
    if (bline(l)->line.typ == TIP_UT) {
      print_ut(g, l);
      dlP += bline(l)->line.pod.dl;
      dlO += bline(l)->line.obr.dl;
      vP += bline(l)->line.pod.v;
      vO += bline(l)->line.obr.v;
    }
  }

  fprintf(g, "<tr>");
  fprintf(g, "<td>Всего</td>");
  fprintf(g, "<td>&nbsp;</td>");
  
  fprintf(g, "<td>&nbsp;</td>");
  fprintf(g, "<td>%g</td>", dlP);
  fprintf(g, "<td>&nbsp;</td>");
  fprintf(g, "<td>%g</td>", dlO);

  fprintf(g, "<td>&nbsp;</td>");
  fprintf(g, "<td>&nbsp;</td>");
  fprintf(g, "<td>%g</td>", vP);
  fprintf(g, "<td>%g</td>", vO);
  fprintf(g, "<td>&nbsp;</td>");
  fprintf(g, "</tr>");

  print_doc_end(g);

/*
  g = print_doc(f, "a6.html", _TR("Мех. оборудование камер")); 
  if (g) {
    print_sheet6(g, vyd_l, nl, n1);
    print_doc_end(g);
  }
*/
  g = print_doc(f, "a3.html", _TR("Механическое оборудование")); 
  if (g) {
    print_sheet3(m_cxema->m_graph, g, vyd_l, nl, n1);
    print_doc_end(g);
  }

/*
  g = print_doc(f, "f10.html", _TR("Вскрытия"));  //шурфы
  if (g) {
//    print_sheet3(g, vyd_l, nl, n1);
    print_sheetShurf(g, vyd_l, nl, n1);
    print_doc_end(g);
  }
*/


  g = print_doc(f, "a7.html", _TR("Каналы")); 
  if (g) {
    MagLineTabKanal(g, nl, vyd_l);
    
    print_doc_end(g);
  }

  g = print_doc(f, "a5.html", _TR("Камеры и павильоны")); 
  if (g) {
    print_sheet5(m_cxema->m_graph, g, vyd_l, nl, n1);
    print_doc_end(g);
  }

  g = print_doc(f, "a4.html", _TR("Опоры")); 
  if (g) {
    print_sheet4(m_cxema->m_graph, g, vyd_l, nl, n1);
    print_doc_end(g);
  }

  g = print_doc(f, "f06.html", _TR("Спец.констр.")); 
  if (g) {
//    print_sheet4(g, vyd_l, nl, n1);
    print_doc_end(g);
  }

  g = print_doc(f, "a2.html", _TR("Изоляция труб")); 
  if (g) {
    MagLineTabIzol3(g, nl);
    print_doc_end(g);
  }
  
//  if (g) {
//    print_sheet2(g, vyd_l, nl, n1);
//    print_doc_end(g);
//  }
//  MagLineTabIzol3(g, nl);


// Спец констр



/*
  g = print_doc(f, "a2.html", _TR("Изоляция труб")); 
  MagLineTabIzol3(g, nl);
*/  




// Ответственный
  g = print_doc(f, "a10.html", _TR("Ответственное лицо")); 
  if (g) {
//    print_sheet9(g, nomer_uch);
    print_doc_end(g);
  }


/*
  g = print_doc(f, "a1-kaprem.html", _TR("Опрессовки")); 
  if (g) {
    HTML h(g, &css); 
    print_sheetOpressovka(h, vyd_l, nl, n1);
    print_doc_end(g);
  }
*/

  g = print_doc(f, "a1-rem.html", _TR("Дефект"));  // Изменение в оборудовании
  if (g) {
    print_sheetRemont(g, vyd_l, nl, n1);
    print_doc_end(g);
  }


  g = print_doc(f, "a1-kaprem.html", _TR("Кап.ремонт"));   // Изменение в оборудовании
  if (g) {
    HTML h(g, &css); 
    print_sheetKapital(h, vyd_l, nl, n1);
    print_doc_end(g);
  }

  g = print_doc(f, "f10.html", _TR("Вскрытия"));  //шурфы
  if (g) {
    print_sheetShurf(g, vyd_l, nl, n1);
    print_doc_end(g);
  }

  g = print_doc(f, "f11.html", _TR("Опрессовки"));  //Опрессовка
  if (g) {
//    HTML h(g, &css); 
//    print_sheetKapital(h, vyd_l, nl, n1);
    print_doc_end(g);
  }

  g = print_doc(f, "f12.html", _TR("Освидетель."));  
  if (g) {
//    HTML h(g, &css); 
//    print_sheetKapital(h, vyd_l, nl, n1);
    print_doc_end(g);
  }


/*

  g = print_doc(f, "a7.html", _TR("Каналы")); print_doc_end(g);
  g = print_doc(f, "a8.html", _TR("Ремонтные работы")); print_doc_end(g);
  
*/

  print_index2(f);
  fclose(f);
  setlocale(LC_NUMERIC, "eng");


//  tmpName.Format("%s\\html\\2\\index2.html", argpath());
  tmpName.Format("%s\\index2.html", getenv("TMP"));
//  tmpName.Format("%s\\html\\2\\index.html", argpath());


  CString cssName;
  cssName.Format("%s\\style.css", getenv("TMP"));
  css.print(cssName);

  HINSTANCE hRun = ShellExecute (GetSafeHwnd (), "open", "excel", "\""+tmpName+"\"", NULL, SW_SHOWMAXIMIZED);
}

void CGidrView::OnPasp()
{
//  if (m_ms_kod0 <= 0 && m_rs_kod0 <= 0) return;

  int id = 0,  ms_rs = 0;

  if (m_ms_kod0 > 0) {
    id = m_ms_kod0;
    ms_rs = 0;
  }
  else if (m_rs_kod0 > 0) {
    id = m_rs_kod0;
    ms_rs = 1;
  }
  
  PaspRSMS(0, 0, ms_rs, id);
}


void CGidrView::OnPaspKam()
{
  CString tmpName;

  tmpName.Format("%s\\html\\birka.html", argpath());

  HINSTANCE hRun = ShellExecute (GetSafeHwnd (), "open", "excel", "\""+tmpName+"\"", NULL, SW_SHOWMAXIMIZED);

}

bool getVydRectAll(CGraph2 *graph, CFRect &rect0);
void copyVydGraphAll(CGraph2 *graph1, CGraph2 *graph2, list<CNode*> &list_sort, list<CLINE2 *> &list_l, int mag_typ, int mag_id);


double getLengthLines(CNode *n, CFPoint pt);
double getLengthLines2(CNode2 *n, CFPoint pt, CLINE2 *vl);


int find_geo_line(CLINE2 *vl, list<int> &list_geo, Klassif *kls);


int find_geo_node(CNode2 *node, list<int> &list_geo, Klassif *kls)
{
    if (!kls) return -1;

  double md = 500;
  
  CFRect rectAll(node->node.coord.x, node->node.coord.y, node->node.coord.x, node->node.coord.y);
  rectAll.InflateRect(md, md);

  CFRect rect, r;

//  m_geofile->m_kls000 = kls;

  int nn = kls->NObj();

  for ( int i = 1; i <= nn; i++ ) {
    m_geofile->GetFRect(kls, i, rect);

    if (r.IntersectRect(rect, rectAll)) {
    
      CCoordList cl;
      m_geofile->GetCoord(kls, i, cl);
      CFPoint pt = cl.first();
      double len = Length(node->node.coord, pt);

      int qq = kls->geo4[i-1]->nom;

      if (len < md) {
        list_geo.push_back(i);
      }
    }
  }
  return list_geo.size();
}


int print_geo(FILE *f, list<int> &list_geo, Klassif *kls)
{
  int n = list_geo.size();
  
  if (n > 0) {
    fprintf(f, "  %s : ", kls->nazv);

    list<int>::const_iterator it = list_geo.begin();
    for ( ; it != list_geo.end(); it++) {
      int i = *it;
      int qq = kls->geo4[i-1]->nom;
      
      fprintf(f, " %d ", qq);
    }

    fprintf(f, "\n");
  }
  return n;
}

int makeGeoTemp(CGraph2 *graph, Klassif *kls, CString &vyd);

#if 0

void read_geo(list<int> &list_geo, Klassif *kls, vector<CString> &list_t, list<vector<CString> > & list_vv)
{
  list_vv.clear();

  CAdoFile *ado = kls->m_ado;


  CString vyd = "";


  if (ado->openTable0(kls->TableName())) {
    list<int>::const_iterator it = list_geo.begin();
    for ( ; it != list_geo.end(); it++) {
      vector<CString> list_v;
      if (ado->GotoKey(kls->geo4[*it-1]->nom)) {
        for (int i = 0; i < list_t.size(); i++) {
          CString v = ado->read(list_t[i]);
          list_v.push_back(v);
        }
      }
      list_vv.push_back(list_v);
    }

    it = list_geo.begin();
    for ( ; it != list_geo.end(); it++) {
      vector<CString> list_v;
      if (ado->GotoKey(kls->geo4[*it-1]->nom)) {
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



void read_geo(list<int> &list_geo, Klassif *kls, vector<CString> &list_t, list<vector<CString> > & list_vv)
{
    list_vv.clear();

    CAdoFile *ado = kls->m_ado;

    CString vyd = "";
//    int qq = makeGeoTemp(graph, kls, vyd);


    CString lst = "";

    list<int>::const_iterator it = list_geo.begin();
    for (; it != list_geo.end(); it++) {
        CString s;
        if (lst != "") lst += ",";
        s.Format("%d", kls->geo4[*it - 1]->nom);
        lst += s;
    }




    if (lst != "") {
        CString q;
        CString par = "id";

        for (vector<CString>::const_iterator it = list_t.begin(); it != list_t.end(); it++) {
            par += ",";
            par += transl(*it);
        }

//        q.Format("SELECT %s FROM %s WHERE ID IN (SELECT id from %s)", par, kls->nazv, vyd);
        q.Format("SELECT %s FROM %s WHERE ID IN (%s)", par, kls->nazv, lst);


//        Join2 j;
//        int n0 = j.initJoin(ado, kls->nazv);
//        j.allJoin(ado, kls->nazv, n0);
//        q = j.getQ_as();

        if (ado->openTable0(q)) {
            while (!ado->isEOF()) {
                vector<CString> list_v;
                for (int i = 0; i < list_t.size(); i++) {
                    CString v = ado->readStr(list_t[i]);
                    list_v.push_back(v);
                }
                list_vv.push_back(list_v);
                ado->MoveNext();
            }
            ado->closeTable();
        }
    }
}


void print_td_povt(FILE *f, int n)
{
  for (int i = 0; i < n; i++) { 
    fprintf(f, "<td>&nbsp</td>");
  }
}


void print_td_n(FILE *f, int n)
{
  if (n > 0) { 
    fprintf(f, "<td>%d</td>", n);
  }
  else {
    fprintf(f, "<td>&nbsp;</td>");
  }
}

void print_td_n(FILE *f, int n, BOOL yes)
{
  if (yes) { 
    fprintf(f, "<td>%d</td>", n);
  }
  else {
    fprintf(f, "<td>&nbsp;</td>");
  }
}


Klassif *findKlN(const char *txt)
{
  Klassif *kls = m_geofile->m_kl_list->findKlN(txt);

  if (!kls) {
    CString str;
    str.Format(_TR("Нет данных по объекту \"%s\""), txt);
    AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
  }
  return kls;
}

int num_po(const char *po1) 
{
    if (po1[0] == '1') return 2;
    return 1;
}

int kolich(const char* po, const char* kol)
{
    int n = 0;

    if (sscanf(kol, "%d", &n) == 1) {
        if (po[0] == '1') return n * 2;
    }
    return n;
}

void initLookup(CAdoFile *ado, const char * tn, const char *fn, map<long, CString> & map_v)
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


CString getReal(map<long, CString>& map_v, const char *ids)
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


void print_sheet3(CGraph2 *graph, FILE *f, VYD_LINES &vyd_l, CNode2 *nr, CNode2 *nm)
{
#if 1
  Klassif *klsD = findKlN(transl("Дренажный кран"));
  Klassif *klsV = findKlN(transl("Воздушник"));
  Klassif *klsZ = findKlN(transl("Запорная арматура"));
  Klassif *klsK = findKlN(transl("Компенсатор"));

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

//  initLookup(ado, "naznachenie_zapornoy_armatury_setey_teplosnabzheniya", "naimenovanie", map_z);
  initLookup(ado, "ValvePurpose", "name", map_z);



//  map<int, CLINE2*>::const_iterator it = vyd_l.map_num_line.begin();

  list<int> list_geoD;
  list<int> list_geoV;
  list<int> list_geoZ;
  list<int> list_geoK;

  CNode2 *n0 = NULL;

  //for (; it != vyd_l.map_num_line.end(); it++) {

  for (map<int, CLINE2*>::const_iterator it = vyd_l.map_line.begin(); it != vyd_l.map_line.end(); it++) {
    CLINE2 *vl = it->second;
    CNode2 *n1 = where(vl);
    CNode2 *n2 = other(vl);

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
      vector<CString> ttZ(tZ, tZ+5);
      list<vector<CString> > list_vvZ;
      read_geo(list_geoZ, klsZ, ttZ, list_vvZ);

      CString tV[] = { transl("Диаметр"), transl("Признак трубопровода") };
      vector<CString> ttV(tV, tV+2);
      list<vector<CString> > list_vvV;
      read_geo(list_geoV, klsV, ttV, list_vvV);

      CString tD[] = { transl("Диаметр"), transl("Признак трубопровода") };
      vector<CString> ttD(tD, tD+2);
      list<vector<CString> > list_vvD;
      read_geo( list_geoD, klsD, ttD, list_vvD);

      CString tK[] = { transl("Тип компенсатора"), transl("Диаметр трубопровода"), transl("Признак трубопровода") };
      vector<CString> ttK(tK, tK+3);
      list<vector<CString> > list_vvK;
      read_geo(list_geoK, klsK, ttK, list_vvK);

      upr (list_vvZ);
      upr (list_vvV);
      upr (list_vvD);
      upr (list_vvK);

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
          print_ut2(f, vl);
        }
        else {
          fprintf(f, "<td>&nbsp;</td>"); 
          fprintf(f, "<td>&nbsp;</td>"); 
        }

  //      int nnn =  (nO, nD, nV, nZ, nK, nU);

       // Запорная арматура         

        if (itZ != list_vvZ.end()) {
          CString ss = getReal(map_z, (*itZ)[0]);
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

        if (itD != list_vvD.end()) itD ++;
        if (itV != list_vvV.end()) itV ++;
        if (itZ != list_vvZ.end()) itZ ++;
        if (itK != list_vvK.end()) itK ++;
      }
    }
  }
#endif
}

void print_sheet4(CGraph2 *graph, FILE *f, VYD_LINES &vyd_l, CNode2 *nr, CNode2 *nm)
{
//  InitTab(getPS(), tn);

#if 1

  map<long, CString> map_o;
  
  Klassif *klsO = findKlN(transl("Опора"));
  Klassif *klsU = findKlN(transl("Угол поворота трубопровода"));

  if (!klsO || !klsU) {
//    AfxMessageBox(_TR("Нет файла геобазы"), MB_OK|MB_ICONINFORMATION);
    return;
  }

  CAdoFile* ado = getAdo(getPsAdoName());


  initLookup(ado, "BuildingConstructions", "name", map_o);



//  InitTab(klsO->m_db->m_strFileName, klsO->TableName(), false);

//  MFIELDS map1;
//  InitTab(klsO->m_db->m_strFileName, klsO->TableName(), false);
//  getMap(klsO->TableName(), _TR("Строительная конструкция"), map1);

//  map<int, CLINE2*>::const_iterator it = vyd_l.map_num_line.begin();

  list<int> list_geoO;
  list<int> list_geoU;

  CNode2 *n0 = NULL;

  list<vector<CString> > list_vvPD;
//  for (; it != vyd_l.map_num_line.end(); it++) {

  for (map<int, CLINE2*>::const_iterator it = vyd_l.map_line.begin(); it != vyd_l.map_line.end(); it++) {
    CLINE2 *vl = it->second;
    CNode2 *n1 = where(vl);
    CNode2 *n2 = other(vl);

    if (1 || n1->typPS() != -1 || inc(n1) != 2 || nr && n1->node.node_name == nr->node.node_name) {
      n0 = n1;
      list_geoO.clear();
      list_geoU.clear();
      list_vvPD.clear();
    }
    else if (n2->typPS() == -1 && inc(n2) == 2) {
      CLINE2 *vl2 = n2->lines;
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
      vector<CString> ttU(tU, tU+3);
      list<vector<CString> > list_vvU;
      read_geo(list_geoU, klsU, ttU, list_vvU);

      CString tO[] = { transl("Строительная конструкция"), transl("Примечание"), transl("Признак трубопровода") };
      vector<CString> ttO(tO, tO+3);
      list<vector<CString> > list_vvO;
      read_geo(list_geoO, klsO, ttO, list_vvO);

      if (list_vvO.size() > 0) {
        read_geo(list_geoO, klsO, ttO, list_vvO);
      }

      upr (list_vvU);
      upr (list_vvO);
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
          print_ut2(f, vl);
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
          CString ss = getReal(map_o, (*itO)[0]);
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

        if (itU != list_vvU.end()) itU ++;
        if (itO != list_vvO.end()) itO ++;
        if (itPD != list_vvPD.end()) itPD ++;
      }
    }
  }
#endif
}

CString bool_conv(CString s)
{
  if (s == "TRUE") return _TR("Есть");
  return _TR("Нет");
}


void print_pav(FILE* f, const char* tn, CNode2* n)
{
#if 1

    CAdoFile* ado = getAdo(getPsAdoName());
    CString q;

    q.Format("SELECT * FROM [%s] hh JOIN nodes n ON hh.nodeID=n.id WHERE hh.nodeID=%d", tn, n->id);
    q.Format("SELECT * FROM [%s] hh WHERE hh.nodeID=%d", tn, n->id);

    CString str;
    int v = 0;

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            v = 1;
            str = ado->readStr("internalHeight"    /*_TR("Внутренняя высота камеры, мм")*/ );   fprintf(f, "<td>%s</td>", str);
            str = ado->readStr("internalWidth"     /*_TR("Внутренняя ширина камеры, мм")*/  );   fprintf(f, "<td>%s</td>", str);
            str = ado->readStr("internalLength"     /*_TR("Внутренняя длина камеры, мм")*/ );   fprintf(f, "<td>%s</td>", str);
            str = ado->read(_TR("Толщина стенки, мм"));             fprintf(f, "<td>%s</td>", str);
            int idK = ado->read_long(_TR("Конструкция перекрытия"));
            CString* konstr = get_Vmap(ps_alma.map_Konstr, idK);

            fprintf(f, "<td>%s</td>", konstr ? *konstr : "");

            str = ado->readStr(_TR("Наличие неподвижных опор"));   str = bool_conv(str);    fprintf(f, "<td>%s</td>", str);
            str = ado->readStr(_TR("Наличие гидроизоляции"));      str = bool_conv(str);    fprintf(f, "<td>%s</td>", str);
            str = ado->readStr(_TR("Наличие дренажа"));            str = bool_conv(str);    fprintf(f, "<td>%s</td>", str);
            str = ado->readStr(_TR("Материал стен"));   fprintf(f, "<td>%s</td>", str);
        }
        ado->closeTable();
    }
    if (!v) {
        print_td_povt(f, 9);
    }
#endif
}


// Камеры и павильоны


void print_sheet5(CGraph2 *graph, FILE *f, VYD_LINES &vyd_l, CNode2 *nr, CNode2 *nm)
{
//  CDbFile dbf1;
  CString tn1 = _TR("Павильон");
  CString tn2 = _TR("Тепловая камера");

//  if (!openPS1(&dbf1)) return;
//  if (!dbf2.openMDB(&dbf1)) return;

//  CDbFile dbf2(&dbf1);

//  if (!dbf1.openTable(tn1)) return;
//  if (!dbf2.openTable(tn2)) return;

//  map<int, CNode*>::const_iterator it = vyd_l.map_num_node.begin();

//  for (; it != vyd_l.map_num_node.end(); it++) {
  for (map<int, CNode2*>::const_iterator it = vyd_l.map_node.begin(); it != vyd_l.map_node.end(); it++) {
    CNode2 *n = it->second;
 
//    if (1 || n->typPS == 3 || n->typPS == 4) {

//    if (n->node.name_typ_alma) {
    if (7 <= n->node._typPS && n->node._typPS <= 15) {


      fprintf(f, "<tr><td>%s</td>", n->getNamePTfull());

/*

      if (n->node._typPS == 7) {  // Павильон
        print_pav(f, "pavilions", n);
      }
      else if (n->node._typPS == 9) { // Тепловая камера
        print_pav(f, "heatChambers", n);
      }
      else if (n->node._typPS == 10) { //  узел
        print_pav(f, "", n);
      }
*/

      switch (n->node._typPS) {
      case 7 : print_pav(f, "pavilions", n); break; //Павильон", "П", 7),
      case 8 : print_pav(f, "trps", n); break; //ТРП", "ТРП", 8),
      case 9 : print_pav(f, "heatChambers", n); break; //Тепловая камера", "КАМ", 9),
      case 10 : print_pav(f, "undergroundNodes", n); break; //Подземный узел", "ПОДЗ", 10),
      case 11 : print_pav(f, "overgroundNodes", n); break; //Надземный узел", "НАДЗ", 11),
      case 12 : print_pav(f, "Ввод в здание", n); break; //Ввод здание", "ВВ", 12),
      case 13 : print_pav(f, "uninstalledNodes", n); break; //Неустановленный узел", "?", 13),
      case 14 : print_pav(f, "pumpStations", n); break; //Насосная станция", "НС", 14),
      case 15 : print_pav(f, "heatSources", n); break; //Источник тепла", "ИСТ", 15);
      }


//      else {
//        print_td_povt(f, 9);
//      }



/*
      if (n->typPS == 3) {
        print_pav(f, dbf1, n->idPS);
      }
      else if (n->typPS == 4) {
        print_pav(f, dbf2, n->idPS);
      }
*/
    }
    fprintf(f, "\n");

  }
//  dbf1.closeTable();
//  dbf2.closeTable();
//  dbf1.closeMDB();
//  dbf2.closeMDB();
}


void print_sheet6(CGraph2 *graph, FILE *f, VYD_LINES &vyd_l, CNode *nr, CNode *nm)
{
#if 1
  Klassif *klsD = findKlN(transl("Дренажный кран"));
  Klassif *klsV = findKlN(transl("Воздушник"));
  Klassif *klsZ = findKlN(transl("Запорная арматура"));
  Klassif *klsK = findKlN(transl("Компенсатор"));

  if (!klsD || !klsV || !klsZ || !klsK) {
//    AfxMessageBox(_TR("Нет файла геобазы"), MB_OK|MB_ICONINFORMATION);
    return;
  }

//  MFIELDS map1;
//  InitTab(klsK->m_db->m_strFileName, klsK->TableName(), false);
//  getMap(klsK->TableName(), _TR("Тип компенсатора"), map1);

//  map<int, CNode*>::const_iterator it = vyd_l.map_num_node.begin();

  for (map<int, CNode2*>::const_iterator it = vyd_l.map_node.begin(); it != vyd_l.map_node.end(); it++) {
    CNode2 *n = it->second;

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

      name1 = n->getNamePTfull();

      int nn = 0;


      CString tZ[] = {_TR("Назначение"), _TR("Диаметр"), _TR("Материал"), _TR("Тип отключения")  };
      vector<CString> ttZ(tZ, tZ+4);
      list<vector<CString> > list_vvZ;
      read_geo(list_geoZ, klsZ, ttZ, list_vvZ);

      nn = max (nn, list_vvZ.size());

      CString tV[] = {_TR("Диаметр") };
      vector<CString> ttV(tV, tV+1);
      list<vector<CString> > list_vvV;
      read_geo(list_geoV, klsV, ttV, list_vvV);

      nn = max (nn, list_vvV.size());


      CString tD[] = {_TR("Диаметр") };
      vector<CString> ttD(tD, tD+1);
      list<vector<CString> > list_vvD;
      read_geo(list_geoD, klsD, ttD, list_vvD);
      nn = max (nn, list_vvD.size());


      CString tK[] = {_TR("Тип компенсатора"), _TR("Диаметр трубопровода") };
      vector<CString> ttK(tK, tK+2);
      list<vector<CString> > list_vvK;
      read_geo(list_geoK, klsK, ttK, list_vvK);
      nn = max (nn, list_vvK.size());


      list<vector<CString> >::const_iterator itZ = list_vvZ.begin();
      list<vector<CString> >::const_iterator itV = list_vvV.begin();
      list<vector<CString> >::const_iterator itD = list_vvD.begin();
      list<vector<CString> >::const_iterator itK = list_vvK.begin();

      for (int i = 0; i < nn; i++) {
        fprintf(f, "<tr>");

        if (i == 0) {
          fprintf(f, "<td>%s</td>", n->getNamePTfull()); 
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

        if (itD != list_vvD.end()) itD ++;
        if (itV != list_vvV.end()) itV ++;
        if (itZ != list_vvZ.end()) itZ ++;
        if (itK != list_vvK.end()) itK ++;
      }

    }
  }
#endif
}


void print_sheet9(FILE *f, int nomer_uch)
{
#if 0
    CString tn;
  

  tn.Format("SELECT * FROM [Участки эксплуатации] U, [Начальники участков] M WHERE M.ID=U.[Начальник участка] AND U.ID=%d", nomer_uch);

  CDbFile dbf;
  
  
  if (!openPS1(&dbf)) return;

  if (dbf.openTable(tn)) {
    if (!dbf.rs->IsEOF()) {

      dbf.rs->MoveFirst();

      if ( !dbf.rs->IsEOF() ) {
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



void read_geo_Remont(list<int> &list_geo, Klassif *kls, vector<CString> &list_t, list<vector<CString> > & list_vv)
{
#if 0
  list_vv.clear();

  CDbFile *db = kls->m_db;

  if (db->openTable(kls->TableName())) {
    list<int>::const_iterator it = list_geo.begin();
    for ( ; it != list_geo.end(); it++) {
      vector<CString> list_v;
      if (db->GotoKey(kls->geo4[*it-1]->nom)) {
        for (int i = 0; i < list_t.size(); i++) {
          CString v = db->read(list_t[i]);
          list_v.push_back(v);
        }
      }
      list_vv.push_back(list_v);
    }

    it = list_geo.begin();
    for ( ; it != list_geo.end(); it++) {
      vector<CString> list_v;
      if (db->GotoKey(kls->geo4[*it-1]->nom)) {
        for (int i = 0; i < list_t.size(); i++) {
          CString v = db->read(list_t[i]);
          CString pod_obr = db->read(_TR("Признак трубопровода"));
          if (pod_obr == _TR("общий")) {
            list_v.push_back(v);
          }
        }
      }
      if (list_v.size() > 0) {
        list_vv.push_back(list_v);
      }
    }
    db->closeTable();
  }
#endif
}


void read_elem(int ID, Klassif *kls, const char *name)
{
#if 0
  CDbFile *db = kls->m_db;
  CString q;
  q.Format("SELECT * FROM [%s] WHERE [Дефект]=%d", name, ID);

  if (db->openTable(q)) {
    if (!db->rs->IsEOF()) {
      db->rs->MoveFirst();

      while ( !db->rs->IsEOF() ) {
        int n = db->read_long(_TR("Элемент"));

        db->rs->MoveNext();
      }
    }
    db->closeTable();
  }
#endif
}

/*

void print_sheetRemont(HTML &h, VYD_LINES &vyd_l, CNode2 *nr, CNode2 *nm)
{
      Klassif *klsD = findKlN(transl("Дефект"));

  if (!klsD) {
    return;
  }

  MFIELDS map1, map2;

//  map<int, CLINE2*>::const_iterator it = vyd_l.map_num_line.begin();

  list<int> list_geoD;

  for (map<int, CLINE2*>::const_iterator it = vyd_l.map_line.begin(); it != vyd_l.map_line.end(); it++) {
    CLINE2 *vl = it->second;
    int nD = find_geo_line(vl, list_geoD, klsD);
  }

  CString tD[] = {
    _TR("Дата начала ремонта"), 
    _TR("Дата завершения ремонта"), 
    _TR("Начальная камера"), 
    _TR("Конечная камера"), 
    _TR("Ремонт по восстановлению канала"),
    _TR("Ремонт изоляционной конструкции, подающий"),
    _TR("Ремонт изоляционной конструкции, обратный")
//    _TR("Ремонт по трубопроводу, подающий"),
//    _TR("Ремонт по трубопроводу, обратный")
  };
  vector<CString> ttD(tD, tD+sizeof(tD)/sizeof(tD[0]));
  list<vector<CString> > list_vvD;
  read_geo(list_geoD, klsD, ttD, list_vvD);

  list<vector<CString> >::const_iterator itD = list_vvD.begin();
  h.set_tr(9);
  css.css.mso_number_format = "\\@";

  for (; itD != list_vvD.end(); itD++) {
    CString d1 = (*itD)[0];
    CString d2 = (*itD)[1];
    CString vid = (*itD)[4]+"<br/><br/>"+(*itD)[5]+"<br/><br/>"+(*itD)[6];

    h.print_tr();
    h.print_td((*itD)[2]);
    h.print_td((*itD)[3]);
    css.css.vertical_align = "justify";
    h.print_td(d1);
    h.print_td(d2);
    h.print_td(vid);
    h.print_tr2();
  }
  fprintf(h.f, "</table>");
}
*/


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

static void ispr(CString &v, double f)
{
  CString q;
  q.Format(" %g", f);
  v.Replace(", м", q+" м,");
  v.Replace(", шт", q+" шт,");
}


void print_zz(HTML &h, CString txt1, CString txt2, CString kam1, CString kam2, CString d1, CString d2, bool &first)
{
  css.css.border_top = 0;
  css.css.border_bottom = 0;
  css.css.border_right = 1;
  css.css.border_left = 1;

  
  if (txt2 != "") {
    txt2.Replace("\n", "<br/>");
    h.print_tr();

    if (first) {
      css.css.border_top = 1;
      h.print_td(kam1);
      h.print_td(kam2);
      h.print_td(d1);
      h.print_td(d2);
      first = false;
    }
    else {
      css.css.border_top = 0;
      h.print_td("&nbsp;");
      h.print_td("&nbsp;");
      h.print_td("&nbsp;");
      h.print_td("&nbsp;");
    }
    h.print_td(txt1);
    h.print_td(txt2);
    h.print_tr2();
  }
}




void print_sheetRemont(FILE *f, VYD_LINES &vyd_l, CNode2 *nr, CNode2 *nm)
{
#if 0
  HTML h(f, &css);

  Klassif *klsD = findKlN(transl("Дефект"));

  if (!klsD) {
    return;
  }


  MFIELDS map1, map2;

//  map<int, CLINE2*>::const_iterator it = vyd_l.map_num_line.begin();

  list<int> list_geoD;

  for (map<int, CLINE2*>::const_iterator it = vyd_l.map_line.begin(); it != vyd_l.map_line.end(); it++) {
    CLINE2 *vl = it->second;
    int nD = find_geo_line(vl, list_geoD, klsD);
  }

  CString p = "";

  list<int>::const_iterator it2 = list_geoD.begin();
  for ( ; it2 != list_geoD.end(); it2++) {
    int i = *it2;
    if (i > 0) {
      CString s;
      s.Format("%d", klsD->geo4[i-1]->nom);
      if (p != "") p += ",";
      p += s;
    }
  }


  h.set_tr(9);

  CString def = _TR("Дефект");
  
  CString q, q_tr, q_el, q4;

  q.Format("SELECT * FROM [%s] WHERE ID in (%s) ORDER BY [Дата завершения ремонта]", _TR("Дефект"), p);
  q_tr.Format("SELECT * FROM [%s_Трубы] WHERE [Дефект] in (%s) AND ([Исправлено на подаче] > 0 OR [Осмотрено на обратке] > 0)", _TR("Дефект"), p);
  q_el.Format("SELECT * FROM [%s_Элементы] WHERE [Дефект] in (%s)", _TR("Дефект"), p);

//  q4.Format("SELECT * FROM ([%s] LEFT JOIN [%s_Трубы] ON [%s].ID = [%s_Трубы].[Дефект]) LEFT JOIN [%s_Элементы] ON [%s].ID = [%s_Элементы].Дефект WHERE [%s].ID IN (%s) ORDER BY [Дата завершения ремонта]", def, def, def, def, def, def, def, def, p);

  CDbFile *db = klsD->m_db;

  InitTab(db->m_strFileName, _TR("Дефект"), false);
  InitTab(db->m_strFileName, _TR("Дефект_Трубы"), false);
  InitTab(db->m_strFileName, _TR("Дефект_Элементы"), false);



  map<int, map<int, EL> > map_el;
  map<int, map<int, TR> > map_tr;

  if (db->openTable(q_el)) {
    if (!db->rs->IsEOF()) {

      db->rs->MoveFirst();

      while ( !db->rs->IsEOF() ) {

        EL el;

        int id1 = db->read_long("ID");
        int id2 = db->read_long(_TR("Дефект"));
        int id3 = db->read_long(_TR("Элемент"));

        el.oP = db->read_double(_TR("Осмотрено на подаче"));
        el.pP = db->read_double(_TR("Повреждено на подаче"));
        el.mP = db->read(_TR("Мероприятие на подаче"));
        el.zP = db->read_double(_TR("Запланировано на подаче"));
        el.iP = db->read_double(_TR("Исправлено на подаче"));
        el.oO = db->read_double(_TR("Осмотрено на обратке"));
        el.pO = db->read_double(_TR("Повреждено на обратке"));
        el.mP = db->read(_TR("Мероприятие на обратке"));
        el.zO = db->read_double(_TR("Запланировано на обратке"));
        el.iO = db->read_double(_TR("Исправлено на обратке"));

        map_el[id2][id3] = el;

        db->rs->MoveNext();
      }
    }
    db->closeTable();
  }

  if (db->openTable(q_tr)) {
    if (!db->rs->IsEOF()) {

      db->rs->MoveFirst();

      while ( !db->rs->IsEOF() ) {

        TR tr;

        int id1 = db->read_long("ID");
        int id2 = db->read_long(_TR("Дефект"));
        int id3 = db->read_long(_TR("Элемент"));

        tr.oP = db->read_double(_TR("Осмотрено на подаче"));
        tr.pP = db->read_double(_TR("Повреждено на подаче"));
        tr.iP = db->read_double(_TR("Исправлено на подаче"));
        tr.oO = db->read_double(_TR("Осмотрено на обратке"));
        tr.pO = db->read_double(_TR("Повреждено на обратке"));
        tr.iO = db->read_double(_TR("Исправлено на обратке"));

        map_tr[id2][id3] = tr;

        db->rs->MoveNext();
      }
    }
    db->closeTable();
  }

//  InitTab(getPsSprav(), _TR("Дефект_Трубы"), false);
//  InitTab(getPsSprav(), _TR("Дефект_Элементы"), false);

  if (db->openTable(q)) {
    if (!db->rs->IsEOF()) {
      db->rs->MoveFirst();

      while ( !db->rs->IsEOF() ) {
//        db->read(1, s);

        int id = db->read_long("ID");

        CString d1 = db->read(_TR("Дата начала ремонта"));
        CString d2 = db->read(_TR("Дата завершения ремонта"));
        CString kam1 = db->read(_TR("Начальная камера"));
        CString kam2 = db->read(_TR("Конечная камера"));
        CString d3 = db->read(_TR("Ремонт по восстановлению канала"));
        CString d4 = db->read(_TR("Ремонт изоляционной конструкции, подающий"));
        CString d5 = db->read(_TR("Ремонт изоляционной конструкции, обратный"));
//        int tr = db->read_long(_TR("Дефект_Трубы.Элемент"));
//        int el = db->read_long(_TR("Дефект_Элементы.Элемент"));


/*
        h.print_tr();
        h.print_td(kam1);
        h.print_td(kam2);
        css.css.vertical_align = "justify";
        h.print_td(d1);
        h.print_td(d2);
//        h.print_tr2();
*/

        CString v_elP = "", v_trP = "";
        CString v_elO = "", v_trO = "";

        map<int, map<int, EL> >::const_iterator ite = map_el.find(id);;

        if (ite != map_el.end()) {
          
          for (map<int, EL>::const_iterator it2 = ite->second.begin(); it2 != ite->second.end(); it2++) {

            CString v = getReal(_TR("Дефект_Элементы"), _TR("Элемент"), it2->first);
            
            EL el = it2->second;
            if (el.iP > 0) {
              ispr(v, el.iP);
              if (v_elP != "") v_elP += "\n";  v_elP += el.mP; v_elP += " "; v_elP += v;
            }
            if (el.iO > 0) {
              ispr(v, el.iO);
              if (v_elO != "") v_elO += "\n";  v_elO += el.mO; v_elO += " "; v_elO += v;
            }
          }
        }

        map<int, map<int, TR> >::const_iterator itt = map_tr.find(id);;

        if (itt != map_tr.end()) {
          itt->second;
          for (map<int, TR>::const_iterator it2 = itt->second.begin(); it2 != itt->second.end(); it2++) {
            CString v = getReal(_TR("Дефект_Трубы"), _TR("Элемент"), it2->first);
            
            TR tr = it2->second;
            if (tr.iP > 0) {
              ispr(v, tr.iP);
              if (v_trP != "") v_trP += "\n";  v_trP += v;
            }
            if (tr.iO > 0) {
              ispr(v, tr.iO);
              if (v_trO != "") v_trO += "\n";  v_trO += v;
            }
          }
        }

        bool first = true;

        print_zz(h, _TR("Канал"), d3, kam1, kam2, d1, d2, first);
        print_zz(h, _TR("Теплоизоляционная конструкция, подающий"), d4, kam1, kam2, d1, d2, first);
        print_zz(h, _TR("Теплоизоляционная конструкция, обратный"), d5, kam1, kam2, d1, d2, first);

        print_zz(h, _TR("Трубопроводы, подающий"), v_trP, kam1, kam2, d1, d2, first);
        print_zz(h, _TR("Трубопроводы, обратный"), v_trO, kam1, kam2, d1, d2, first);
        print_zz(h, _TR("Элементы, подающий"), v_elP, kam1, kam2, d1, d2, first);
        print_zz(h, _TR("Элементы, обратный"), v_elO, kam1, kam2, d1, d2, first);
//        print_zz(h, "", ".", kam1, kam2, d1, d2, first);

        db->rs->MoveNext();
      }
    }
    db->closeTable();
  }
#endif
}

void print_sheetShurf(FILE *f, VYD_LINES &vyd_l, CNode2 *nr, CNode2 *nm)
{
#if 0
  Klassif *klsD = findKlN(transl("Шурф"));

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

  CNode *n0 = NULL;

  //for (; it != vyd_l.map_num_line.end(); it++) {

  for (map<int, CLINE2*>::const_iterator it = vyd_l.map_line.begin(); it != vyd_l.map_line.end(); it++) {
    CLINE2 *vl = it->second;
    CNode *n1 = where(vl);
    CNode *n2 = other(vl);

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
      
      vector<CString> ttD(tD, tD+sizeof(tD)/sizeof(tD[0]));
      list<vector<CString> > list_vvD;
      read_geo(list_geoD, klsD, ttD, list_vvD);

      upr (list_vvD);

      nD = list_vvD.size();
     
      int nn = nD;

      list<vector<CString> >::const_iterator itD = list_vvD.begin();
     
      int nnn = sizeof(tD)/sizeof(tD[0]);


      for (int i = 0; i < nn; i++) {

        h.print_tr();
        h.set_tr(5);
        
        if (i == 0) {
          print_ut2_1(h, vl);
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

              val = getReal(klsD->TableName(), fn, val);
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
        if (itD != list_vvD.end()) itD ++;
      }
    }
  }
#endif
}




void print_zz2(HTML &h, CString dt1, CString dt2, CString isp1, CString isp2, CString ss, bool &first)
{
  css.css.border_top = 0;
  css.css.border_bottom = 0;
  css.css.border_right = 1;
  css.css.border_left = 1;
  
//    txt2.Replace("\n", "<br/>");
  h.print_tr();

  h.print_td("&nbsp;");
  h.print_td("&nbsp;");

  if (first) {
    css.css.border_top = 1;
    h.print_td(dt1);
    h.print_td(dt2);
    h.print_td(_TR("Капитальный ремонт"));
  }
  else {
    css.css.border_top = 0;
    h.print_td("&nbsp;");
    h.print_td("&nbsp;");
    h.print_td("&nbsp;");
  }
  h.print_td(ss);

  if (first) {
    h.print_td(isp1);
    h.print_td(isp2);
  }
  else {
    h.print_td("&nbsp;");
    h.print_td("&nbsp;");
  }

  h.print_tr2();

  first = false;
}

#if 0

CString read_kap_meropr(CDbFile &dbf2, int ID)
{
  CString qq = "", q2;

  CString tn_meropr = _TR("Капремонт_колодцы_опоры_надземный");

  q2.Format("SELECT * FROM [%s] WHERE [ID_ремонт]=%d", tn_meropr, ID);

  if (dbf2.openTable(q2)) {
    while ( !dbf2.rs->IsEOF() ) {

      CString i1 = dbf2.read("ID_колод_опоры_надз");
      CString i2 = dbf2.read("ID_колодц_опоры_мероп"); 
      long d2 = dbf2.read_long(_TR("Количество Протяженность факт, шт, м"));

      CString s1 = getReal(tn_meropr, "ID_колод_опоры_надз", i1);  if (i1 == s1) s1 = "";
      CString s2 = getReal(tn_meropr, "ID_колодц_опоры_мероп", i2);if (i2 == s2) s2 = "";

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


CString read_kap_meropr(CDbFile &dbf2, int ID, CString tn_meropr, CString f1, CString f2)
{
  CString qq = "", q2;

  q2.Format("SELECT * FROM [%s] WHERE [ID_ремонт]=%d", tn_meropr, ID);

  if (dbf2.openTable(q2)) {
    while ( !dbf2.rs->IsEOF() ) {

      CString i1 = dbf2.read(f1);
      CString i2 = dbf2.read(f2 );
      //              long d1 = dbf2.read_long(_TR("Количество Протяженность план, шт, м"));
      long d2 = dbf2.read_long(_TR("Количество Протяженность факт, шт, м"));

      CString s1 = getReal(tn_meropr, f1, i1); if (i1 == s1) s1 = "";
      CString s2 = getReal(tn_meropr, f2, i2); if (i2 == s2) s2 = "";

      CString ss;
      ss.Format("%s %s : %d", s1, s2, d2);

      if (qq != "") qq += "<br/>";
      qq += ss;

      dbf2.rs->MoveNext();
    }
  }
  return qq;
}

CString read_kap_node(CDbFile &dbf2, int ID, CGraph2 *graph, double r)
{
  CString q2;


  q2.Format("SELECT * FROM [%s] WHERE [ID_ремонт]=%d", STR_REMONT_KAPITAL, ID);

  CString qq = "";
  if (dbf2.openTable(q2)) {
    while ( !dbf2.rs->IsEOF() ) {
      CCoordList cl;
      dbf2.readCoord(cl);

      CFPoint pt = cl.centroid2();

      CLINE2 *l = graph->findLine(pt, r);
      if (l) {
        if (qq != "") qq += " ";
        qq += (where(l)->node_name.Name()+ " "+ other(l)->node_name.Name());
      }
      dbf2.rs->MoveNext();
    }
  }
  return qq;
}
#endif



void CGidrView::print_sheetKapital(HTML &h, VYD_LINES &vyd_l, CNode2 *nr, CNode2 *nm)
{
#if 0

  int qq = CreateGeoTableKapital(m_cxema->m_graph, STR_REMONT_KAPITAL);

  CString mdb = getVyd();

  h.set_tr(8);

  CDbFile dbf;

  CString q, q2;
  CString tn1 = _TR("Капитальный_ремонт");
  q.Format("SELECT * FROM [%s] WHERE [ID] IN (SELECT [nom] FROM [%s].VYD)", tn1, mdb);

  Klassif *kls = m_geofile->m_kl_list->findKlN(transl(STR_REMONT_KAPITAL));

  if (kls) {

    InitTab(kls->FileName(), _TR("Капремонт_трубопр_изол"), false);
    InitTab(kls->FileName(), _TR("Капремонт_колодцы_опоры_подземный"), false);
    InitTab(kls->FileName(), _TR("Капремонт_колодцы_опоры_надземный"), false);
    
    //    RunTableMap(kls->m_database, tn, q, getHelp_Map(), -1, 0, FALSE, FALSE, FALSE, FALSE);

    if (dbf.openMDB(kls->FileName())) {
    
      CDbFile dbf2(&dbf);
      
      if (dbf.openTable(q)) {

        while ( !dbf.rs->IsEOF() ) {
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

          CString qq = read_kap_node(dbf2, ID, m_cxema->m_graph, masx*D5/mas_otn);

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

#if 0

CString read_opr_meropr(CDbFile &dbf2, int ID, CString tn_meropr, CString f1, CString f2)
{
  CString qq = "", q2;

  q2.Format("SELECT * FROM [%s] WHERE [ID_опрессовка]=%d", tn_meropr, ID);

  if (dbf2.openTable(q2)) {
    while ( !dbf2.rs->IsEOF() ) {

      CString i1 = dbf2.read(f1);
      CString i2 = dbf2.read(f2 );
      //              long d1 = dbf2.read_long(_TR("Количество Протяженность план, шт, м"));
      long d2 = dbf2.read_long(_TR("Количество Протяженность, шт, м"));

      CString s1 = getReal(tn_meropr, f1, i1); if (i1 == s1) s1 = "";
      CString s2 = getReal(tn_meropr, f2, i2); if (i2 == s2) s2 = "";

      CString ss;
      ss.Format("%s %s : %d", s1, s2, d2);

      if (qq != "") qq += "<br/>";
      qq += ss;

      dbf2.rs->MoveNext();
    }
  }
  return qq;
}

#endif

void CGidrView::print_sheetOpressovka(HTML &h, VYD_LINES &vyd_l, CNode2 *nr, CNode2 *nm)
{
#if 0
  int qq = CreateGeoTableOpressovka(m_cxema->m_graph, STR_REMONT_OPRESSOVKA);

  CString mdb = getVyd();

  h.set_tr(8);

  CDbFile dbf;

  CString q, q2;
  CString tn1 = _TR("Опрессовка");
  q.Format("SELECT * FROM [%s] WHERE [ID] IN (SELECT [nom] FROM [%s].VYD)", tn1, mdb);

  Klassif *kls = m_geofile->m_kl_list->findKlN(transl(STR_REMONT_OPRESSOVKA));

  if (kls) {

    InitTab(kls->FileName(), _TR("Опрессовка_повреждения"), false);
    
    //    RunTableMap(kls->m_database, tn, q, getHelp_Map(), -1, 0, FALSE, FALSE, FALSE, FALSE);

    if (dbf.openMDB(kls->FileName())) {
    
      CDbFile dbf2(&dbf);
      
      if (dbf.openTable(q)) {

        while ( !dbf.rs->IsEOF() ) {
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

          CString qq = read_kap_node(dbf2, ID, m_cxema->m_graph, masx*D5/mas_otn);

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



void CGidrView::MagLineTabIzol3(FILE *f, CNode2 *n)
{
  CNode2 *nz = m_cxema->m_graph->sameMarkNode();

  if (nz) {
    RsLineTabIzol(f, nz);
  }
}



void CGidrView::RsLineTab(FILE *f, CNode2 *node)
{
#if 0
  RsLineTabDb(node);

  CDbFile dbf;
  CString mdb = getVyd();
  CString tn = _TR("Трубы");

  CString q;

  q.Format("SELECT uzel1 AS [Начальный узел], uzel2 AS [Конечный узел] ,"
    " diamP AS [Условный диаметр, подача], dlinaP AS [Длина, подача], "
    " diamO AS [Условный диаметр, обратка], dlinaO AS [Длина, обратка], "
    " tolP AS [Толщина стенки, подача], tolO AS [Толщина стенки, обратка], "
    " vP, vO, "
    " typ_pr "
    " FROM [%s]", tn);

//  RunTable(mdb, tn, q, NULL, -1, 0, FALSE, TRUE, TRUE, FALSE);

  if (dbf.openMDB(mdb)) {
    if (dbf.openTable(q)) {

      if (!dbf.rs->IsEOF()) {
        dbf.rs->MoveFirst();

        while ( !dbf.rs->IsEOF() ) {
          CString s;
          CString s1 = dbf.read(1);
          CString s2 = dbf.read(2);
          fprintf(f, "<tr>");
          if (s1 == "" && s2 == "") {
            fprintf(f, "<td>&nbsp;</td><td>&nbsp;</td>");
          }
          else {
            fprintf(f, "<td>%s</td><td>%s</td>", s1, s2);
          }

          s = dbf.read(3);
          fprintf(f, "<td>%s</td>", s);
          s = dbf.read(4);
          fprintf(f, "<td>%s</td>", s);
          s = dbf.read(5);
          fprintf(f, "<td>%s</td>", s);
          s = dbf.read(6);
          fprintf(f, "<td>%s</td>", s);
          s = dbf.read(7);
          fprintf(f, "<td>%s</td>", s);
          s = dbf.read(8);
          fprintf(f, "<td>%s</td>", s);

          s = dbf.read(9);
          fprintf(f, "<td>%s</td>", s);
          s = dbf.read(10);
          fprintf(f, "<td>%s</td>", s);

          s = dbf.read(11);
          fprintf(f, "<td>%s</td>", s);


          fprintf(f, "</tr>\n");
          dbf.rs->MoveNext();
        }
      }
    }
  }
#endif
}


void CGidrView::RsLineTabIzol(FILE *f, CNode2 *node)
{
  HTML h(f, &css); 

//  RsLineIzolTabDb(node);

//  CString mdb = getVyd();
//  CString tn = _TR("Изоляция");
  CString q, vyd;

  if (!node) return;

  CAdoFile *ado = m_cxema->m_ado;

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

        while ( !ado->isEOF() ) {
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
//          s = getReal(ST_UT, "kod_izol",s);

          h.print_td(s);
          s = ado->readStr(4);
          h.print_td(s);
          s = ado->readStr(5);
//          s = getReal(ST_UT, _TR("Материал наружного покрытия"),s);

          h.print_td(s);
          s = ado->readStr(6);
          h.print_td(s);
          s = ado->readStr(7);

//          s = getReal(ST_UT, _TR("Материал антикоррозийного покрытия"),s);
          
          h.print_td(s);

          h.print_tr2();
          ado->MoveNext();
      }
    }



#if 0

  RsLineIzolTabDb(node);

  CDbFile dbf;
  CString mdb = getVyd();
  CString tn = _TR("Изоляция");
  CString q;

  RsLineIzolTabDb(node);

  q.Format("SELECT uzel1 AS [Начальный узел], uzel2 AS [Конечный узел] ,"
    " mat AS [Теплоизоляционный материал], "
    " tol AS [Толщина тепловой изоляции, мм], "
    " pokr_mat AS [Материал наружного покрытия], "
    " pokr_tol AS [Толщина наружного покрытия], "
    " ant AS [Материал антикоррозионного покрытия] "
    " FROM [%s]", tn);


//  RunTable(mdb, tn, q, NULL, -1, 0, FALSE, TRUE, TRUE, FALSE);

  InitTab(mdb, ST_UT, false);

  if (dbf.openMDB(mdb)) {
    if (dbf.openTable(q)) {

      if (!dbf.rs->IsEOF()) {
        dbf.rs->MoveFirst();

        while ( !dbf.rs->IsEOF() ) {
          CString s;
          CString s1 = dbf.read(1);
          CString s2 = dbf.read(2);
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

          s = dbf.read(3);
          s = getReal(ST_UT, "kod_izol",s);

          h.print_td(s);
          s = dbf.read(4);
          h.print_td(s);
          s = dbf.read(5);
          s = getReal(ST_UT, _TR("Материал наружного покрытия"),s);

          h.print_td(s);
          s = dbf.read(6);
          h.print_td(s);
          s = dbf.read(7);

//          s = getReal(ST_UT, _TR("Материал антикоррозийного покрытия"),s);
          
          h.print_td(s);

          h.print_tr2();
          dbf.rs->MoveNext();
        }
      }
    }
  }
#endif
}

void CGidrView::MagLineTabKanal(FILE *f, CNode2 *n, VYD_LINES &vyd_l)
{
    HTML h(f, &css);

    Klassif *kls = m_geofile->m_kl_list->findKlN(transl("Канал"));

    if (!kls) return;

    CAdoFile *ado = kls->m_ado;

    CNode2 *nz = m_cxema->m_graph->sameMarkNode();
    if (!nz) return;

    map<int, CLINE2*>::const_iterator it = vyd_l.map_line.begin();

    double dlP = 0, dlO = 0, vP = 0, vO = 0;

    CString q;
//    q.Format("SELECT * FROM %s", kls->nazv);

    q = "SELECT tk.id, tk.naimenovanie AS tk_name,  kanal.tip_kanala, kanal.* FROM kanal LEFT JOIN [tip_kanala_setey_teplosnabzheniya] tk ON CAST(tk.id  AS VARCHAR(11)) = kanal.tip_kanala";



    if (ado->openTable0(q)) {

        for (; it != vyd_l.map_line.end(); it++) {
            CLINE2 *l = it->second;
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
