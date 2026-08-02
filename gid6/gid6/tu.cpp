#include "stdafx.h"
#include "gid6.h"
#include "MainFrm.h"
#include "gid6Doc.h"
#include "gidrView.h"

#include "ado.h"

#include <locale.h>


#include "mmenu.h"
#include "css.h"
#include "html.h"

static CSS css;


void print_index1(FILE *f, const char *title);
void print_index2(FILE *f);
void print_index(FILE *f, const char *title, const char *fn);
//FILE *print_doc(FILE *fi, const char *fn, const char *title);
FILE *print_doc(FILE *fi, const char *fn, const char *title, void(*filtr) (CString &s1));

void print_doc_end(FILE *f);

CString getParams(CAdoFile *ado, const char *tn);


static int m_y1 = 0, m_y2 = 0, m_y0 = 0;
static COleDateTime m_date;
static CString m_raion = "";


static double coef_2_4 = 2.4;

void filtr(CString &s1)
{
    CString s;

    s1.Replace("$ORG$", _TR("АО \"АлЭС\" и ТОО \"АТКЭ\""));
    s.Format("%d", m_y1);
    s1.Replace("$Y1$", s);
    s.Format("%d", m_y2);
    s1.Replace("$Y2$", s);
    s.Format("%d", m_y0);
    s1.Replace("$Y$", s);

    s.Format("%d.%02d.%4d", m_date.GetDay(), m_date.GetMonth(), m_date.GetYear());
    s1.Replace("$DATE$", s);
}

void filtr_zh(CString &s1)
{
    CString s;

    s.Format("%d", m_y0);
    s1.Replace("$YEAR$", s);
    s1.Replace("$RAION$", m_raion);
}

CString getB(int n)
{
    int nn = 'Z' - 'A';

    int a = n / nn;
    int b = n % nn;

    CString ss;
    if (a == 0) {
        ss.Format("%c", b + 'B');
    }
    else {
        ss.Format("%c%c", a - 1 + 'A', b + 'B');
    }

    return ss;
}

int print_summ(HTML &h, const char *name, int kk, int n1, int n2)
{
    h.print_tr();
    h.print_td();
    h.print_td();
    h.print_td("&nbsp;");

    css.css.bold = 1;
    css.css.text_align = "left";

    h.print_td(_TR("Итого по %s :"), name);  // D


    css.css.text_align = "right";
    css.css.mso_number_format = "0\\.000";

    h.print_td("=СУММ(E%d:E%d)", n1 + 1, n2); // E
    h.print_td("=СУММ(F%d:F%d)", n1 + 1, n2); // E
    h.print_td("=СУММ(G%d:G%d)", n1 + 1, n2); // E
    h.print_td("=СУММ(H%d:H%d)", n1 + 1, n2); // E
    h.print_td("=СУММ(I%d:I%d)", n1 + 1, n2); // E
    h.print_td("=СУММ(J%d:J%d)", n1 + 1, n2); // E
    h.print_td("=СУММ(K%d:K%d)", n1 + 1, n2); // E
    h.print_td("=СУММ(L%d:L%d)", n1 + 1, n2); // E
    h.print_td("=СУММ(M%d:M%d)", n1 + 1, n2); // E
    h.print_td("=СУММ(N%d:N%d)", n1 + 1, n2); // E
    h.print_td("=СУММ(O%d:O%d)", n1 + 1, n2); // E
    h.print_td("=СУММ(P%d:P%d)", n1 + 1, n2); // E
    h.print_td("=СУММ(Q%d:Q%d)", n1 + 1, n2); // E
    h.print_td("=СУММ(R%d:R%d)", n1 + 1, n2); // E
    h.print_td("=СУММ(S%d:S%d)", n1 + 1, n2); // E
    h.print_td("=СУММ(T%d:T%d)", n1 + 1, n2); // E
    h.print_td("=СУММ(U%d:U%d)", n1 + 1, n2); // E
    h.print_td("=СУММ(V%d:V%d)", n1 + 1, n2); // E
    h.print_td("=СУММ(W%d:W%d)", n1 + 1, n2); // E
    h.print_td("=СУММ(X%d:X%d)", n1 + 1, n2); // E
    h.print_td("=СУММ(Y%d:Y%d)", n1 + 1, n2); // E
    h.print_td("=СУММ(Z%d:Z%d)", n1 + 1, n2); // E

    css.css.mso_number_format = "";
    css.css.bold = 0;

    h.print_tr2();
    kk++;

    h.print_tr();
    h.print_td();
    h.print_td();
    h.print_td();
    h.print_td();
    h.print_tr2();

    kk++;
    return kk;
}

struct D7 {
    double d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11;

    D7() {
        d1 = d2 = d3 = d4 = d5 = d6 = d7 = d8 = d9 = d10 = d11 = 0.;
    }

    D7 operator + (D7 &d) const
    {
        D7 dd = *this;

        dd.d1 += d.d1;
        dd.d2 += d.d2;
        dd.d3 += d.d3;
        dd.d4 += d.d4;
        dd.d5 += d.d5;
        dd.d6 += d.d6;
        dd.d7 += d.d7;
        dd.d8 += d.d8;
        dd.d9 += d.d9;
        dd.d10 += d.d10;
        dd.d11 += d.d11;

        return dd;
    }

    void operator += (D7 &d)
    {
        d1 += d.d1;
        d2 += d.d2;
        d3 += d.d3;
        d4 += d.d4;
        d5 += d.d5;
        d6 += d.d6;
        d7 += d.d7;
        d8 += d.d8;
        d9 += d.d9;
        d10 += d.d10;
        d11 += d.d11;
    }
};




struct TU_DATA {
    double d1, d2, d3, d4;

    TU_DATA() {
        d1 = d2 = d3 = d4 = 0.;
    }
    void operator += (TU_DATA &d) {
        d1 += d.d1;
        d2 += d.d2;
        d3 += d.d3;
        d4 += d.d4;
    }

    TU_DATA operator - (TU_DATA &d) const
    {

        TU_DATA dd = *this;

        dd.d1 -= d.d1;
        dd.d2 -= d.d2;
        dd.d3 -= d.d3;
        dd.d4 -= d.d4;
        return dd;
    }


    double delta()
    {
        return d1 - (d2 + d3 + d4);
    }

    bool valid()
    {
        double dd = d2 + d3 + d4;

        if (fabs(d1 - dd) < 0.0001) {
            return true;
        }
        return false;
    }
};

struct TU_DATA3 {
    TU_DATA d, pr, a, pr_a, z, pr_z;
};


static map<CString, TU_DATA3> map_tu;


CString readSost(CAdoFile *ado, const char *fn)
{
    CString sost = "Д";
    long sostID = ado->read_long(transl(fn));

    switch (sostID) {
    case 1: sost = "Д"; break;
    case 2: sost = "А"; break;
    case 3: sost = "М"; break;
    case 4: sost = "З"; break;
    }
    return sost;
}


void read_pr(CAdoFile *db, int n, TU_DATA &d, TU_DATA &pr)
{
    CString s, ss;

    CString ist = db->readStr(transl("Источник"));
    CString nomer = db->readStr(transl("Номер"));
    CString org = db->readStr(transl("Наименование организации, запрашивающей ТУ"));
    CString name = db->readStr(transl("Наименование объекта"));
    CString addr = db->readStr(transl("Адрес объекта"));

    s.Format(_TR("Изменения продления %d"), n);
    s.Format(_TR("Дата изменения продления %d"), n);

    ss = db->read(s);

    if (ss == "") return;



    s.Format(_TR("Тепловые потоки, Гкал/ч %d"), n);
    d.d1 = db->read_double(transl(s));
    s.Format(_TR("В том числе отопление %d"), n);
    d.d2 = db->read_double(transl(s));
    s.Format(_TR("В том числе вентиляция %d"), n);
    d.d3 = db->read_double(transl(s));
    s.Format(_TR("В том числе ГВС макс %d"), n);
    d.d4 = db->read_double(transl(s));


    s.Format(_TR("Прирост нагрузки %d"), n);
    pr.d1 = db->read_double(transl(s));
    s.Format(_TR("В том числе прирост отопление %d"), n);
    pr.d2 = db->read_double(transl(s));
    s.Format(_TR("В том числе прирост вентиляция %d"), n);
    pr.d3 = db->read_double(transl(s));
    s.Format(_TR("В том числе прирост ГВС макс %d"), n);
    pr.d4 = db->read_double(transl(s));




    /*

    d.d1 = db->read_double(transl("Тепловая нагрузка по акту допуска (проекту) Гкал/ч"));
    d.d2 = db->read_double(transl("В том числе отопление по акту"));
    d.d3 = db->read_double(transl("В том числе вентиляция по акту"));
    d.d4 = db->read_double(transl("В том числе ГВС макс по акту"));
    */

    /*

    s.Format(_TR("Прирост нагрузки %d"), n);
    d.d5 = db->read_double(transl(s));
    s.Format(_TR("В том числе прирост отопление %d"), n);
    d.d6 = db->read_double(transl(s));
    s.Format(_TR("В том числе прирост вентиляция %d"), n);
    d.d7 = db->read_double(transl(s));
    s.Format(_TR("В том числе прирост ГВС макс %d"), n);
    d.d8 = db->read_double(transl(s));
    s.Format(_TR("Дополнительные технические мероприятия %d"), n);
    */
}


bool initTU()
{
    map_tu.clear();

    CString q;


    Klassif *kls = m_geofile->m_kl_list->findKlN(transl("Здания ТУ"));
    if (!kls || !kls->m_ado) {
        AfxMessageBox(_TR("Не подключен файл Технических условий"), MB_OK|MB_ICONINFORMATION);
        return false;
    }

    CAdoFile *db = kls->m_ado;


    CString params = getParams(db, transl("Технические условия"));

    q.Format("SELECT %s FROM %s WHERE YEAR(%s) >= %d AND YEAR(%s) <= %d", params, transl("Технические условия"), transl("Дата выдачи ТУ"), m_y1, transl("Дата выдачи ТУ"), m_y2);

    if (db->openTable0(q)) {
        while (!db->isEOF()) {
            CString name = db->readStr(transl("Источник"));
            CString nomer = db->readStr(transl("Номер ТУ"));

            // 
            // ID
            CString sost = readSost(db, transl("Состояние договора"));

            // Номер 
            int year = db->read_long(transl("Год"));
            // Номер ТУ
            // Дата выдачи ТУ
            // Наименование организации, запрашивающей ТУ
            // Наименование объекта
            // Адрес объекта Источник
            // Район эксплуатации

            TU_DATA d, pr;

            //  if (name == "13 воен/гор") {
            if (name != "" && name != _TR("АО \"АлЭС\" (ТЭЦ-1, ТЭЦ-2, ЗТК)")) {
                int qq;
                qq = 1;
            }

            COleDateTime dateZ = db->read_date(_TR("Дата выдачи акта допуска"));
            COleDateTime dateA = db->read_date(_TR("Дата аннулирования"));

            d.d1 = db->read_double(transl("Тепловые потоки, Гкал/ч"));
            d.d2 = db->read_double(transl("В том числе отопление"));
            d.d3 = db->read_double(transl("В том числе вентиляция"));
            d.d4 = db->read_double(transl("В том числе ГВС макс"));

            d.d1 = d.d2+d.d3+d.d4/coef_2_4;

            pr.d1 = db->read_double(transl("Прирост нагрузки"));
            pr.d2 = db->read_double(transl("В том числе прирост отопление"));
            pr.d3 = db->read_double(transl("В том числе прирост вентиляция"));
            pr.d4 = db->read_double(transl("В том числе прирост ГВС макс"));

            pr.d1 = pr.d2+pr.d3+pr.d4/coef_2_4;

            if (sost == "З") {
                pr.d1 = db->read_double(transl("Тепловая нагрузка по акту допуска (проекту) Гкал/ч"));
                pr.d2 = db->read_double(transl("В том числе отопление по акту"));
                pr.d3 = db->read_double(transl("В том числе вентиляция по акту"));
                pr.d4 = db->read_double(transl("В том числе ГВС макс по акту"));
                pr.d1 = pr.d2+pr.d3+pr.d4/coef_2_4;
            }

            // Камера

            // Дополнительные технические мероприятия
            // Срок действия ТУ

            /*
            read_pr(db, 1, d, pr);
            read_pr(db, 2, d, pr);
            read_pr(db, 3, d, pr);
            read_pr(db, 4, d, pr);
            read_pr(db, 5, d, pr);
            read_pr(db, 6, d, pr);
            read_pr(db, 7, d, pr);
            */
    
            TU_DATA3 data;

            map<CString, TU_DATA3>::const_iterator it = map_tu.find(name);

            data.d.d1 = d.d1;
            data.d.d2 = d.d2;
            data.d.d3 = d.d3;
            data.d.d4 = d.d4;
            data.pr.d1 = pr.d1;
            data.pr.d2 = pr.d2;
            data.pr.d3 = pr.d3;
            data.pr.d4 = pr.d4;

            TU_DATA z, pr_z;

            CString s;


            pr_z.d1 = db->read_double(transl("Тепловая нагрузка по акту допуска (проекту) Гкал/ч"));
            pr_z.d2 = db->read_double(transl("В том числе отопление по акту"));
            pr_z.d3 = db->read_double(transl("В том числе вентиляция по акту"));
            pr_z.d4 = db->read_double(transl("В том числе ГВС макс по акту"));
            pr_z.d1 = pr_z.d2+pr_z.d3+pr_z.d4/coef_2_4;


            if ((sost == "А" || sost == "М") && dateA.m_dt < m_date.m_dt) {
                data.a = d;
                data.pr_a = pr;
            }
            if (sost == "З") {
                double y1 = m_date.GetYear();
                double y2 = dateZ.GetYear();
                if (dateZ.m_dt < m_date.m_dt) {
                    data.z = d;
                    data.pr_z = pr;
                }
            }

            if (it != map_tu.end()) {
                TU_DATA3 data2 = it->second;
                data2.d += data.d;
                data2.pr += data.pr;
                data2.a += data.a;
                data2.pr_a += data.pr_a;
                data2.z += data.z;
                data2.pr_z += data.pr_z;
                map_tu[name] = data2;
            }
            else {
                map_tu[name] = data;
            }


            // Номер согласования ТС
            // Дата согласования ТС
            // Номер согласования ОВ
            // Дата согласования ОВ
            // Номер согласования ТП
            // Дата согласования ТП
            // Исполнение доп техн и энерг мероприятий в рамках ТУ 
            // Стадия строительства объектов 
            // Номер выдачи акта допуска 
            // Дата выдачи акта допуска
            // Тепловая нагрузка по акту допуска (проекту) Гкал/ч
            // В том числе отопление по акту
            // В том числе вентиляция по акту
            // В том числе ГВС макс по акту
            // Номер договора
            // Дата договора 
            // Здание


            db->MoveNext();
        }
        db->closeTable();
    }



    return true;
}

#include "TuItogInit.h"

static CString kot = _TR("Источники тепла");


bool isTuYear(int year)
{
    CString q;
    int n = 0;

    Klassif *kls = m_geofile->m_kl_list->findKlN(transl("Здания ТУ"));
    if (!kls || !kls->m_ado) return false;

    CAdoFile *ado = kls->m_ado;

    //  q.Format("SELECT K.ID, K.Наименование, K.[Район эксплуатации],  P.Год, P.[Располагаемая мощность ОВ], P.[Располагаемая мощность ГВС средняя], P.[Располагаемая мощность суммарная], P.[Нормативные тепловые потери], P.[Присоединенная мощность Отопление], P.[Присоединенная мощность Вентиляция], P.[Присоединенная мощность ГВС максимальная], P.[Присоединенная мощность Пар] FROM %s AS K INNER JOIN [Присоединенная нагрузка источников] AS P ON K.ID = P.ID WHERE K.ID=[P].[ID] AND P.Год=%d", kot, year);
    q.Format("SELECT K.id FROM %s AS K INNER JOIN %s AS P ON K.ID = P.ID2 WHERE P.god=%d", transl(kot),  transl("Присоединенная нагрузка источников"), year);

    long nc = getCountTable2(ado, q);

    return nc != 0;

/*
    
    if (ado->openTable0(q)) {
        n = ado->NRecs();
        ado->closeTable();
    }

    return n != 0;
*/
}


void print_row(HTML &h2, const char *ist, const char *name, const TU_DATA &d, const TU_DATA &pr, const char *color)
{
    h2.print_tr();
    h2.print_td();
    css.css.text_align = "center";
    css.css.bold = 1;

    h2.print_td(ist);
    h2.print_td(name);

    css.css.text_align = "right";
    css.css.bold = 0;

    css.css.bk_color = color;

    h2.print_td(d.d1);
    h2.print_td(d.d2);
    h2.print_td(d.d3);
    h2.print_td(d.d4);


    h2.print_td(pr.d1);
    h2.print_td(pr.d2);
    h2.print_td(pr.d3);
    h2.print_td(pr.d4);
    h2.print_td(pr.d4 / coef_2_4);

    css.css.bk_color = "";
    css.css.text_align = "left";

    h2.print_tr2();
}

void print_row1(HTML &h2, const char *ist, const char *name, double d)
{
    TU_DATA d1, d2;
    d2.d1 = d;

    print_row(h2, ist, name, d1, d2, "");
}

void CGidrView::OnTuExcel()
{
    Klassif *kls = m_geofile->m_kl_list->findKlN(transl("Здания ТУ"));
    if (!kls || !kls->m_ado) return;

    CAdoFile *ado = kls->m_ado;

    CTime t = CTime::GetCurrentTime();
    int yy = t.GetYear();

    CTuItogInit tuinit(yy);

    int ret;
    bool error = false;

    css.css.border = 1;

    do {
        ret = tuinit.DoModal();
        while (ret != IDOK) return;

        for (yy = tuinit.m_y1; yy <= tuinit.m_y2; yy++) {
            if (!isTuYear(yy)) {
                CString s;
                s.Format(_TR("Нет данных по источникам за %d год"), yy);
                AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);
                error = true;
            }
        }
        yy = tuinit.m_y_ist;
        if (yy < tuinit.m_y1 || yy > tuinit.m_y2) {
            if (!isTuYear(yy)) {
                CString s;
                s.Format(_TR("Нет данных по источникам за %d год"), yy);
                AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);
                error = true;
            }
        }
    } while (error);

    coef_2_4 = tuinit.m_coef24;

    m_y1 = tuinit.m_y1;
    m_y2 = tuinit.m_y2;
    m_y0 = tuinit.m_y_ist;
    m_date = tuinit.m_date;


    CString tmpName;
    tmpName.Format("%s\\tmp.html", getenv("TMP"));

    FILE *f = fopen(tmpName, "w");
    if (!f) {
        AfxMessageBox(_TR("Ошибка"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    if (!initTU()) return;

    setlocale(LC_NUMERIC, "");
    print_index1(f, "");

    FILE *g = print_doc(f, "tu\\balans.html", _TR("Баланс"), filtr);
    FILE *g2 = print_doc(f, "tu\\balans2.html", _TR("Данные к балансу"), filtr);

    HTML h1(g, &css), h2(g2, &css);

    h2.set_tr(12);

    double d;
    CString s, name = "", name_old = "";
    CString q;

//    q.Format("SELECT * FROM [Присоединенная нагрузка источников] INNER JOIN ([%s] K LEFT JOIN [Район эксплуатации] ON K.[Район эксплуатации] = [Район эксплуатации].ID) ON [Присоединенная нагрузка источников].ID = K.ID"
//        " WHERE ((([Присоединенная нагрузка источников].Год)=%d)) ORDER BY [Номер по порядку]", kot, m_y0);

    q.Format("SELECT *, PRN.ustanovlennaya_moschnost AS ust_m FROM %s PRN INNER JOIN (%s K LEFT JOIN %s RE ON K.%s = RE.id) ON PRN.id2 = K.id WHERE PRN.god=%d ORDER BY %s", 
        transl("Присоединенная нагрузка источников"),
        transl(kot),
        transl("Район эксплуатации"),
        transl("Район эксплуатации"),
        m_y0,
        transl("Номер по порядку")
        );


    if (ado->openTable0(q)) {
        int id = 1;
        int nn1 = 1;

        int nn0 = 6;
        int nn = nn0;

        while (!ado->isEOF()) {
            name = ado->readStr(transl("Наименование района эксплуатации источника тепла"));

            if (name != name_old) {
                if (nn != nn0) {
                    nn = print_summ(h1, name_old, nn, nn1, nn);
                }

                h1.print_tr();
                h1.print_td();
                h1.print_td();
                h1.print_td("");

                css.css.text_align = "left";
                css.css.bold = 1;
                h1.print_td("%s", name);  // D
                css.css.bold = 0;
                h1.print_tr2();

                name_old = name;
                nn++;
                nn1 = nn;
            }

            css.css.text_align = "left";

            h1.print_tr();
            h1.print_td();
            h1.print_td();


            css.css.mso_number_format = "";

            css.css.text_align = "center";

            h1.print_td("%d", id++);
            nn++;

            //          int id = ado->read_long(transl(1));
            CString ist = ado->readStr(transl("Наименование"));
            css.css.bold = 0;
            css.css.text_align = "left";
            h1.print_td("%s", ist);  // D
            css.css.bold = 0;

            css.css.text_align = "right";
            css.css.mso_number_format = "0\\.000";


            d = ado->read_double("ust_m");
            h1.print_td(d); // E

            d = ado->read_double(transl("Располагаемая мощность ОВ"));
            h1.print_td(d); // E

                            //          d = ado->read_double(transl("Располагаемая мощность ГВС максимальная"));
            d = ado->read_double(transl("Располагаемая мощность ГВС средняя"));
            h1.print_td(d); // F

            d = ado->read_double(transl("Располагаемая мощность суммарная"));
            h1.print_td(d); // G

            d = ado->read_double(transl("Присоединенная мощность Отопление"));
            h1.print_td(d); // H

            d = ado->read_double(transl("Присоединенная мощность Вентиляция"));
            h1.print_td(d); // I

            d = ado->read_double(transl("Присоединенная мощность ГВС максимальная"));
            h1.print_td(d); // J

            d = ado->read_double(transl("Присоединенная мощность ГВС средняя"));
            h1.print_td("=K%d/%g", nn, coef_2_4); // K

            d = ado->read_double(transl("Пар"));
            h1.print_td(d); // L

            d = ado->read_double(transl("Нормативные тепловые потери"));
            h1.print_td("=I%d+J%d+L%d+M%d", nn, nn, nn, nn); // M
            h1.print_td(d); // N
            h1.print_td("=H%d-N%d-O%d", nn, nn, nn); // O

            TU_DATA3 data;

            map<CString, TU_DATA3>::iterator it = map_tu.find(ist);
            if (it != map_tu.end()) {
                data = it->second;

                print_row(h2, ist, _TR("Всего"), data.d, data.pr, "");
                print_row(h2, "&nbsp", _TR("Аннулировано"), data.a, data.pr_a, "#FDE9D9");
                print_row(h2, "&nbsp", _TR("Подключено"), data.z, data.pr_z, "#EBF1DE");

                TU_DATA d1 = data.d - data.a - data.z;
                TU_DATA d2 = data.pr - data.pr_a - data.pr_z;

                print_row(h2, "&nbsp", _TR("Итого прирост, в т.ч:"), d1, d2, "");


                TU_DATA dd;

                print_row1(h2, "&nbsp", _TR("ОВ"), d2.d2 + d2.d3);
                print_row1(h2, "&nbsp", _TR("ГВС, макс"), d2.d4);
                print_row1(h2, "&nbsp", _TR("ГВС, ср"), d2.d4 / coef_2_4);

                h2.print_tr();
                h2.print_td_st(NULL, "");
                h2.print_tr2();


                d = 0;

                h1.print_td(data.pr.d2); // P
                h1.print_td(data.pr.d3); // Q  
                h1.print_td(data.pr.d4); // R   ='Данные к балансу'!K14
                h1.print_td("=S%d/%g", nn, coef_2_4); // S
                h1.print_td("=Q%d+R%d+T%d", nn, nn, nn); // T

                h1.print_td(data.pr_z.d2); // U
                h1.print_td(data.pr_z.d3); // V  
                h1.print_td(data.pr_z.d4); // W   ='Данные к балансу'!K14
                                           //            h1.print_td(d); // X
                h1.print_td("=X%d/%g", nn, coef_2_4); // X


                h1.print_td("=V%d+W%d+Y%d", nn, nn, nn); // Y


                h1.print_td(data.pr_a.d2); // Z
                h1.print_td(data.pr_a.d3); // AA  
                h1.print_td(data.pr_a.d4); // AB


                h1.print_td("=AC%d/%g", nn, coef_2_4); // AC
                h1.print_td("=AA%d+AB%d+AD%d", nn, nn, nn); // AD

                h1.print_td("=Q%d-V%d-AA%d", nn, nn, nn); // AE
                h1.print_td("=R%d-W%d-AB%d", nn, nn, nn); // AF
                h1.print_td("=S%d-X%d-AC%d", nn, nn, nn); // AG
                h1.print_td("=AH%d/%g", nn, coef_2_4); // AH
                h1.print_td("=AF%d+AG%d+AI%d", nn, nn, nn); // AI

//                h1.print_td("=F%d-I%d-J%d-Q%d-R%d+V%d+W%d", nn, nn, nn, nn, nn, nn, nn); // AJ
//                h1.print_td("=G%d-L%d-T%d+Y%d", nn, nn, nn, nn); // AK
//                h1.print_td("=H%d-N%d-U%d+Z%d-O%d", nn, nn, nn, nn, nn); // AL


                h1.print_td("=F%d-I%d-J%d-Q%d-R%d+V%d+W%d+AA%d+BB%d", nn, nn, nn, nn, nn, nn, nn, nn, nn); // AK
                h1.print_td("=G%d-L%d-T%d+Y%d+AD%d", nn, nn, nn, nn, nn); // AL
                h1.print_td("=AK%d+AL%d", nn, nn); // AM


//                h1.print_td("=H%d-N%d-U%d+Z%d-O%d", nn, nn, nn, nn, nn); // AN


// Убрал 2019.07.05

//                h1.print_td("=E%d-N%d-U%d+Z%d-O%d", nn, nn, nn, nn, nn); // AL


                css.css.text_align = "left";
                css.css.mso_number_format = "";

            }

            ado->MoveNext();

            h1.print_tr2();
        }

        nn = print_summ(h1, name_old, nn, nn1, nn);
        ado->closeTable();
    }

    fprintf(g, "</table>");

    print_doc_end(g);

    fprintf(g2, "</table>");
    print_doc_end(g2);

    print_index2(f);
    fclose(f);
    setlocale(LC_NUMERIC, "eng");

    CString cssName;
    cssName.Format("%s\\style.css", getenv("TMP"));
    css.print(cssName);

    HINSTANCE hRun = ShellExecute(GetSafeHwnd(), "open", "excel", tmpName, NULL, SW_SHOWMAXIMIZED);

}

void print_dop(HTML *h, CAdoFile *db, int ii)
{
    CString s, s0, s1, mer;
    double s2, s3, s4, s5, s6, s7, s8, s9;
    bool dop = false;

    for (int i = 7; i >= 1; i--) {

        s.Format(_TR("Изменения продления %d"), i);
        s0 = db->read(s);

        s.Format(_TR("Дата изменения продления %d"), i);
        s1 = db->read(s);
        s.Format(_TR("Тепловые потоки, Гкал/ч %d"), i);
        s2 = db->read_double(transl(s));
        s.Format(_TR("В том числе отопление %d"), i);
        s3 = db->read_double(transl(s));
        s.Format(_TR("В том числе вентиляция %d"), i);
        s4 = db->read_double(transl(s));
        s.Format(_TR("В том числе ГВС макс %d"), i);
        s5 = db->read_double(transl(s));
        s.Format(_TR("Прирост нагрузки %d"), i);
        s6 = db->read_double(transl(s));
        s.Format(_TR("В том числе прирост отопление %d"), i);
        s7 = db->read_double(transl(s));
        s.Format(_TR("В том числе прирост вентиляция %d"), i);
        s8 = db->read_double(transl(s));
        s.Format(_TR("В том числе прирост ГВС макс %d"), i);
        s9 = db->read_double(transl(s));
        s.Format(_TR("Дополнительные технические мероприятия %d"), i);
        mer = db->read(s);

        if (s1 != "" || s0 != "") {

            h->print_td("%s %s", s0, s1);
            h->print_td(s2);
            h->print_td(s6);
            h->print_td(mer);
            dop = true;
            break;
        }
    }
    if (!dop) {
        h->print_td(0.);
        h->print_td(0.);
        h->print_td(0.);
        h->print_td(0.);
    }
}


void CGidrView::OnTuSvod()
{
    CWaitCursor wait;

    CString tmpName;
    tmpName.Format("%s\\tmp.html", getenv("TMP"));

    Klassif *kls = m_geofile->m_kl_list->findKlN(transl("Здания ТУ"));
    if (!kls || !kls->m_ado) {
        AfxMessageBox(_TR("Не подключен файл Технических условий"), MB_OK|MB_ICONINFORMATION);
        return;
    }


    FILE *f = fopen(tmpName, "w");
    if (!f) {
        AfxMessageBox(_TR("Ошибка"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    css.css.border = 1;


    setlocale(LC_NUMERIC, "");

    print_index1(f, "");
    css.css.text_align = "left";

    CString q;

    map<int, HTML*> map_html;


    q.Format("SELECT * FROM %s ORDER BY %s", transl("Технические условия"), transl("Дата выдачи ТУ"));


    CAdoFile *db = kls->m_ado;

    HTML *h = NULL;


    if (db->openTable0(q)) {

        while (!db->isEOF()) {

            COleDateTime date = db->read_date(transl("Дата выдачи ТУ"));
            int y = date.GetYear();

            map<int, HTML*>::const_iterator it = map_html.find(y);
            if (it == map_html.end()) {
                CString s;
                s.Format("%d", y);

                if (y < 1900) {
                    s = _TR("Без даты");
                }


                FILE *g;

                g = print_doc(f, "tu\\svod.html", s, NULL);

                h = new HTML(g, &css);
                map_html[y] = h;
                if (h) {
                    h->set_tr(21);
                }

            }
            else {
                h = it->second;
            }


            if (h && h->f) {

                h->print_tr();

                CString sost = "";
                CString sost1 = readSost(db, transl("Состояние договора"));

                if (sost1 == "З") {
                    CString d = db->readStr(transl("Дата выдачи акта допуска"));
                    sost = _TR("Завершен ") + d;
                }
                else if (sost1 == "А") {
                    CString d = db->readStr(transl("Дата аннулирования"));
                    sost = _TR("Аннулирован ") + d;
                }
                else if (sost1 == "М") {
                    CString d = db->readStr(transl("Дата аннулирования"));
                    sost = _TR("Аннулирован мониторингу ") + d;
                }


                h->print_td_st(NULL, "&nbsp;");


                css.css.bk_color = "";
                if (sost1 == "М") css.css.bk_color = "#FCD5B4";
                if (sost1 == "З") css.css.bk_color = "#EBF1DE";
                if (sost1 == "А") css.css.bk_color = "#FDE9D9";

                h->print_td("%d", h->n_tr);

                CString nomer = db->readStr(transl("Номер ТУ"));
                CString data = db->readStr(transl("Дата выдачи ТУ"));


                h->print_td("%s %s", nomer, data);

                CString org = db->readStr(transl("Наименование организации, запрашивающей ТУ"));
                h->print_td("%s", org);

                CString name = db->readStr(transl("Наименование объекта"));
                h->print_td("%s", name);


                CString addr = db->readStr(transl("Адрес объекта"));
                h->print_td("%s", addr);


                double nagr = db->read_double(transl("Тепловые потоки, Гкал/ч"));
                h->print_td(nagr);


                double pr = db->read_double(transl("Прирост нагрузки"));
                h->print_td(pr);



                CString dop = db->readStr(transl("Дополнительные технические мероприятия"));
                h->print_td("%s", dop);

                h->print_td("%s", sost);

                print_dop(h, db, 1);


                CString nTS = db->readStr(transl("Номер согласования ТС"));
                CString dTS = db->readStr(transl("Дата согласования ТС"));
                h->print_td("%s", nTS + " " + dTS);

                CString nOV = db->readStr(transl("Номер согласования ОВ"));
                CString dOV = db->readStr(transl("Дата согласования ОВ"));
                h->print_td("%s", nOV + " " + dOV);

                CString nTP = db->readStr(transl("Номер согласования ТП"));
                CString dTP = db->readStr(transl("Дата согласования ТП"));
                h->print_td("%s", nTP + " " + dTP);


                CString isp = db->readStr(transl("Исполнение доп техн и энерг мероприятий в рамках ТУ"));
                h->print_td("%s", isp);

                CString stad = db->readStr(transl("Стадия строительства объектов"));
                h->print_td("%s", stad);


                CString nA = db->readStr(transl("Номер выдачи акта допуска"));
                CString dA = db->readStr(transl("Дата выдачи акта допуска"));
                h->print_td("%s", nA + " " + dA);

                double nagrA = db->read_double(transl("Тепловая нагрузка по акту допуска (проекту) Гкал/ч"));
                h->print_td(nagrA);

                CString nD = db->readStr(transl("Номер договора"));
                CString dD = db->readStr(transl("Дата договора"));
                h->print_td("%s", nD + " " + dD);


                /*
                CString addr = db->read("");
                h->print_td("%s", addr);
                CString addr = db->read("");
                h->print_td("%s", addr);
                */



                //          for (int i = 0; i < 7; i++) {
                //          }




                /*

                CString addr = db->read("");
                h->print_td("%s", addr);
                CString addr = db->read("");
                h->print_td("%s", addr);
                */

                h->print_tr2();

            }

            db->MoveNext();
        }
    }

    css.css.border = 0;


    map<int, HTML*>::const_iterator it = map_html.begin();
    for (; it != map_html.end(); it++) {
        HTML *h = it->second;
        FILE *g = it->second->f;
        if (g) {

            h->print_tr();  h->print_tr2();
            h->print_tr();  h->print_tr2();


            h->print_tr();
            h->print_td(0.);
            h->print_td(0.);
            h->print_td(0.);

            css.css.border = 1;

            css.css.bk_color = "#EBF1DE";
            h->print_td("");
            css.css.border = 0;
            css.css.bk_color = "";
            h->print_td(" - объекты, получившие акты допуска");

            h->print_tr2();

            h->print_tr();  h->print_tr2();

            h->print_tr();
            h->print_td(0.);
            h->print_td(0.);
            h->print_td(0.);

            css.css.border = 1;

            css.css.bk_color = "#FDE9D9";


            h->print_td("");
            css.css.border = 0;
            css.css.bk_color = "";
            h->print_td("-объекты, по которым ТУ аннулированы");


            h->print_tr2();

            h->print_tr();  h->print_tr2();

            h->print_tr();
            h->print_td(0.);
            h->print_td(0.);
            h->print_td(0.);

            css.css.border = 1;
            css.css.bk_color = "#FCD5B4";
            h->print_td("");
            css.css.border = 0;
            css.css.bk_color = "";
            h->print_td("- аннулировано по результатам мониторинга");
            h->print_tr2();


            fprintf(g, "</table>");
            print_doc_end(g);
        }
    }


    print_index2(f);
    fclose(f);
    setlocale(LC_NUMERIC, "eng");

    CString cssName;
    cssName.Format("%s\\style.css", getenv("TMP"));
    css.print(cssName);

    HINSTANCE hRun = ShellExecute(GetSafeHwnd(), "open", "excel", tmpName, NULL, SW_SHOWMAXIMIZED);
}


void CGidrView::OnTuZav()
{
    Klassif *kls = m_geofile->m_kl_list->findKlN(transl("Здания ТУ"));
    if (!kls || !kls->m_ado) {
        AfxMessageBox(_TR("Не подключен файл Технических условий"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    CString q;

    if (AfxMessageBox(_TR("Подтвердить операцию?"), MB_YESNO) != IDYES) return;

    long affected;

    q.Format("UPDATE [Технические условия] SET [Состояние договора] = 'З' WHERE NOT ([Дата выдачи акта допуска] IS NULL)");
    kls->m_ado->Execute(q, &affected);

    AfxMessageBox(_TR("Выполнено!"), MB_OK|MB_ICONINFORMATION);

}

void print_row_zh(HTML *h, const char *name, D7 d7)
{
    h->print_tr();
    h->print_td();
    h->print_td(0.);
    h->print_td(name);
    h->print_td(0.);
    h->print_td(0.);
    h->print_td(0.);
    h->print_td(d7.d1);
    h->print_td(d7.d2);
    h->print_td(d7.d3);
    h->print_td(d7.d4);
    h->print_td(d7.d5);
    h->print_td(d7.d6);
    h->print_td(d7.d7);
    h->print_td(d7.d8);
    h->print_td(d7.d9);
    h->print_td(d7.d10);
    h->print_td(d7.d11);

    h->print_tr2();
}


void print_row_zh2(HTML *h, const char *name, D7 d7)
{
    h->print_tr();
    h->print_td();
    h->print_td(0.);
    h->print_td(name);

    for (int j = 0; j < 8; j++)  h->print_td(0.);

    h->print_td(d7.d6);
    h->print_td(d7.d7);
    h->print_td(d7.d8);
    h->print_td(d7.d9);
    h->print_td(d7.d10);
    h->print_td(d7.d11);

    h->print_tr2();
}



void CGidrView::OnTuZhurnal()
{

    CString tmpName;
    tmpName.Format("%s\\tmp.html", getenv("TMP"));

    Klassif *kls = m_geofile->m_kl_list->findKlN(transl("Здания ТУ"));
    if (!kls || !kls->m_ado) {
        AfxMessageBox(_TR("Не подключен файл Технических условий"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    CAdoFile *ado = kls->m_ado;

    CString s;
    CString q;

    css.css.border = 1;

    q.Format("SELECT DISTINCT YEAR(%s) FROM %s ORDER BY YEAR(%s)", transl("Дата выдачи ТУ"), transl("Технические условия"), transl("Дата выдачи ТУ"));

    CMMenu menu(this, IDD_MENU, _TR("Выберите год"));

    long y = 2011;

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            s = ado->readStr(0);
            y = ado->read_long(0);
            if (y < 1900) {
                menu.Add(_TR("Без даты"), (void*)y);
            }
            else {
                menu.Add(s, (void*)y);
            }
            ado->MoveNext();
        }
        ado->closeTable();
    }

    int n = 0;

    if (menu.DoModal() != IDOK) return;

    long year = (long)menu.getV();
    m_y0 = year;

    CWaitCursor wait;


    FILE *f = fopen(tmpName, "w");
    if (!f) {
        AfxMessageBox(_TR("Ошибка"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    setlocale(LC_NUMERIC, "");

    print_index1(f, "");


    map<CString, HTML*> map_html;


    if (year) {
        q.Format("SELECT * FROM %s WHERE YEAR(%s)=%d ORDER BY %s", transl("Технические условия"), transl("Дата выдачи ТУ"), year, transl("Район эксплуатации"));
    }
    else {
        q.Format("SELECT * FROM %s WHERE %s IS NULL ORDER BY %s", transl("Технические условия"), transl("Дата выдачи ТУ"), transl("Район эксплуатации"));
    }


    HTML *h = NULL;

    D7 dd7;
    D7 dz7;
    D7 da7;


    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {

            CString raion = ado->readStr(transl("Район эксплуатации"));

            if (raion == "") {
                raion = _TR("Не задан");
            }

            map<CString, HTML*>::const_iterator it = map_html.find(raion);
            if (it == map_html.end()) {
                FILE *g;

                m_raion = raion;

                g = print_doc(f, "tu\\zhurnal.html", raion, filtr_zh);

                h = new HTML(g, &css);
                map_html[raion] = h;
                if (h) {
                    h->set_tr(21);
                }

            }
            else {
                h = it->second;
            }

            if (h && h->f) {

                h->print_tr();

                CString sost = "";
                CString sost1 = readSost(ado, transl("Состояние договора"));


                if (sost1 == "З") {
                    CString d = ado->readStr(transl("Дата выдачи акта допуска"));
                    sost = _TR("Завершен ") + d;
                }
                else if (sost1 == "А") {
                    CString d = ado->readStr(transl("Дата аннулирования"));
                    sost = _TR("Аннулирован ") + d;
                }
                else if (sost1 == "М") {
                    CString d = ado->readStr(transl("Дата аннулирования"));
                    sost = _TR("Аннулирован мониторингу ") + d;
                }

                h->print_td_st(NULL, "&nbsp;");

                css.css.bk_color = "";
                if (sost1 == "М") css.css.bk_color = "#FCD5B4";
                if (sost1 == "З") css.css.bk_color = "#EBF1DE";
                if (sost1 == "А") css.css.bk_color = "#FDE9D9";


                h->print_td("%d", h->n_tr);

                CString nomer = ado->readStr(transl("Номер ТУ"));
                CString data = ado->readStr(transl("Дата выдачи ТУ"));

                h->print_td("%s %s", nomer, data);

                CString org = ado->readStr(transl("Наименование организации, запрашивающей ТУ"));
                h->print_td("%s", org);

                CString name = ado->readStr(transl("Наименование объекта"));
                h->print_td("%s", name);


                CString addr = ado->readStr(transl("Адрес объекта"));
                h->print_td("%s", addr);


                double nagr = ado->read_double(transl("Тепловые потоки, Гкал/ч"));
                h->print_td(nagr);

                double nagrO = ado->read_double(transl("В том числе отопление"));
                h->print_td(nagrO);
                double nagrV = ado->read_double(transl("В том числе вентиляция"));
                h->print_td(nagrV);
                double nagrG = ado->read_double(transl("В том числе ГВС макс"));
                h->print_td(nagrG);
                h->print_td(nagrG / coef_2_4);

                double pr = ado->read_double(transl("Прирост нагрузки"));
                h->print_td(pr);
                double prO = ado->read_double(transl("В том числе прирост отопление"));
                h->print_td(prO);
                double prV = ado->read_double(transl("В том числе прирост вентиляция"));
                h->print_td(prV);
                double prG = ado->read_double(transl("В том числе прирост ГВС макс"));
                h->print_td(prG);
                h->print_td(prG / coef_2_4);

                h->print_td(0.);

                D7 d7;

                d7.d1 = nagr;
                d7.d2 = nagrO;
                d7.d3 = nagrV;
                d7.d4 = nagrG;
                d7.d5 = nagrG / coef_2_4;

                d7.d6 = pr;
                d7.d7 = prO;
                d7.d8 = prV;
                d7.d9 = prG;
                d7.d10 = prG / coef_2_4;

                dd7 += d7;

                if (sost1 == "М" || sost1 == "А") {
                    da7 += d7;
                }
                if (sost1 == "З") {
                    dz7 += d7;
                }

                CString kam = ado->readStr(transl("Камера"));
                h->print_td("%s", kam);

                CString dop = ado->readStr(transl("Дополнительные технические мероприятия"));
                h->print_td("%s", dop);

                h->print_td(0.);

                css.css.text_align = "left";

                CString srok = ado->readStr(transl("Срок действия ТУ"));
                h->print_td("%s", srok);
                h->print_td(0.);
                h->print_tr2();
            }
            ado->MoveNext();
        }
    }

    map<CString, HTML*>::const_iterator it = map_html.begin();
    for (; it != map_html.end(); it++) {
        HTML *h = it->second;
        FILE *g = h->f;
        if (g) {
            h->print_tr();
            h->print_tr2();

            D7 d7;

            print_row_zh(h, _TR("ИТОГО:"), dd7);
            print_row_zh(h, _TR("Аннулировано"), da7);
            print_row_zh(h, _TR("Подключено"), dz7);


/*

            print_row_zh(h, _TR("Итого прирост, в т.ч:"), d7);
            print_row_zh(h, _TR("ОВ"), d7);
            print_row_zh(h, _TR("ГВС, макс"), d7);
            print_row_zh(h, _TR("ГВС, ср"), d7);
*/

            print_row_zh2(h, _TR("Итого прирост, в т.ч:"), dd7);

/*
                d7.d7 = prO;
                d7.d8 = prV;
                d7.d9 = prG;
                d7.d10 = prG / coef_2_4;
*/


            h->print_tr();
            h->print_td();
            h->print_td(0.);
            h->print_td("ОВ");
            for (int j = 0; j < 8; j++) h->print_td(0.);
            h->print_td(dd7.d7+dd7.d8);
            h->print_tr2();


            h->print_tr();
            h->print_td();
            h->print_td(0.);
            h->print_td("ГВС, макс");
            for (int j = 0; j < 8; j++) h->print_td(0.);
            h->print_td(dd7.d9);
            h->print_tr2();


            h->print_tr();
            h->print_td();
            h->print_td(0.);
            h->print_td("ГВС, ср");
            for (int j = 0; j < 8; j++) h->print_td(0.);
            h->print_td(dd7.d10);
            h->print_tr2();



            fprintf(g, "</table>");

            print_doc_end(g);
        }
    }

    print_index2(f);
    fclose(f);
    setlocale(LC_NUMERIC, "eng");

    CString cssName;
    cssName.Format("%s\\style.css", getenv("TMP"));
    css.print(cssName);

    HINSTANCE hRun = ShellExecute(GetSafeHwnd(), "edit", "excel", tmpName, NULL, SW_SHOWMAXIMIZED);
}
