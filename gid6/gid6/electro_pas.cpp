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

#include <locale.h>

#include "css.h"


static CSS css;


void print_head(FILE* f, const char* title);
void print_index1(FILE* f, const char* title);
void print_index(FILE* f, const char* title, const char* fn);
void print_index2(FILE* f);
void print_doc_end(FILE* f);
FILE* print_doc(FILE* fi, const char* fn, const char* title, void (*filtr) (CString& s1) = NULL);

CString readQ(CAdoFile* ado, const char* tn, const char* tn2, int fileID);
CString readQ(CAdoFile* ado, const char* tn, const char* tn2, const char* fileID);

CString color_for_korrozia(CString fn0, CString vn)
{
    CString fn = fn0;
    fn.MakeLower();

    if (fn == "agressivnostname") {
             if (vn == "Низкая")      return "#FFFFCC";
        else if (vn == "Допустимая")  return "#3DEB3D";
        else if (vn == "Высокая")     return "red";
        else if (vn == "Аварийная")   return "fuchsia";
    }
    return "";
}

void drawTable(CAdoFile* ado, const char *q, FILE *g, bool border, CString(*color_for_table)(CString fn, CString vn) = nullptr )
{
//    AfxMessageBox(q, MB_OK|MB_ICONINFORMATION);
    if (ado->openTable0(q)) {
        int n = ado->NFlds();
        while (!ado->isEOF()) {
            fprintf(g, "<tr style='mso-height-source:userset;height:20.0pt'>\n");
            for (int i = 1; i < n; i++) {
                CString s = ado->readStr(i);


                CAdoField f = ado->Field(i);

                CString st = "border-top:none;border-left:none;width:70pt;vertical-align:middle;text-align:center";

                if (border) {
                    st = "border:.5pt solid black;height:24pt;width:70pt;vertical-align:middle;text-align:center";
                }

                if (f.typ == adDate || f.typ == adDBDate) {
                    st += ";mso-number-format:\"Short Date\"";
                }

                if (color_for_table) {
                    CString color = color_for_table(f.name, s);
                    CString s;

                    if (color != "") {
                        s.Format(";background: %s;mso-pattern:%s none;", color, color);
                        st += s;
                    }
                }

                fprintf(g, "<td style='%s'>%s</td>", st, s);
            }
            fprintf(g, "\n");
            fprintf(g, "</tr>\n");

            ado->MoveNext();
        }
        ado->closeTable();
    }
//    AfxMessageBox(ado->getError(), MB_OK|MB_ICONINFORMATION);
}


void CGidrView::electro_pas()
{
    FILE* g;

    CString tmpName;
    tmpName.Format("%s\\index2.html", getenv("TMP"));

    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) {
        return;
    }


    FILE* f = fopen(tmpName, "w");
    if (!f) {
//        AfxMessageBox(strerror(errno), MB_OK|MB_ICONINFORMATION);
        AfxMessageBox("Закройте предыдущий паспорт", MB_OK|MB_ICONINFORMATION);
        return;
    }

    CString q;

    setlocale(LC_NUMERIC, "");

    print_index1(f, "");

    g = print_doc(f, "electro\\a0.htm", _TR("Ф1.ЛЭП"));
    if (g) {
      q = readQ(ado, "electro\\liniya_elektroperedach", "", -1);
      drawTable(ado, q, g, true);
      print_doc_end(g);
    }

//    g = print_doc(f, "electro\\a1.htm", _TR("Форма_1._Приемники_электроснабжения"));
    g = print_doc(f, "electro\\a1.htm", _TR("Ф2.Приемники электроснабжения"));
    if (g) {
      q = readQ(ado, "electro\\priemnik_elektrosnabzheniya", "", -1);
      drawTable(ado, q, g, true);
      print_doc_end(g);
    }

/*

    g = print_doc(f, "electro\\transf0.htm", _TR("Трансформаторы"));
    if (g) {
      q = readQ(ado, "electro\\transf0", "", -1);
      drawTable(ado, q, g, true);
      print_doc_end(g);
    }
*/

//    g = print_doc(f, "electro\\a2.htm", _TR("Форма_2._Трансформаторы"));
    g = print_doc(f, "electro\\a2.htm", _TR("Ф3.Трансформаторы"));
    if (g) {
      q = readQ(ado, "electro\\transf2", "", -1);
      drawTable(ado, q, g, true);
      print_doc_end(g);
    }

//    g = print_doc(f, "electro\\a3.htm", _TR("Форма_3._Электродвигатели"));
    g = print_doc(f, "electro\\a3.htm", _TR("Ф4.Электродвигатели"));
    if (g) {
      q = readQ(ado, "electro\\edv", "", -1);
      drawTable(ado, q, g, true);
      print_doc_end(g);
    }

//    g = print_doc(f, "electro\\a4.htm", _TR("Форма_4._Дизель_генераторные_установки"));
    g = print_doc(f, "electro\\a4.htm", _TR("Ф5.Дизель-генер.установки"));
    if (g) {
      q = readQ(ado, "electro\\dgu", "", -1);
      drawTable(ado, q, g, true);
      print_doc_end(g);
    }

//    g = print_doc(f, "electro\\a5.htm", _TR("Форма_5._Грузоподъемное_оборудование"));
    g = print_doc(f, "electro\\a5.htm", _TR("Ф6.Грузоподъемное оборудование"));
    if (g) {
      q = readQ(ado, "electro\\gruzob", "", -1);
      drawTable(ado, q, g, true);
      print_doc_end(g);
    }

    print_index2(f);
    fclose(f);
    setlocale(LC_NUMERIC, "eng");

    CString cssName;
    cssName.Format("%s\\style.css", getenv("TMP"));
    css.print(cssName);

    HINSTANCE hRun = ShellExecute(GetSafeHwnd(), "open", "excel", "\"" + tmpName + "\"", NULL, SW_SHOWMAXIMIZED);
}



void CGidrView::korrozia_pas()
{
    FILE* g;

    CString tmpName;
    tmpName.Format("%s\\index2.html", getenv("TMP"));

    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) {
        return;
    }


    FILE* f = fopen(tmpName, "w");
    if (!f) {
        AfxMessageBox("Закройте предыдущий паспорт", MB_OK|MB_ICONINFORMATION);
        return;
    }

    CString q;

    setlocale(LC_NUMERIC, "");

    print_index1(f, "");

    g = print_doc(f, "korrozia\\k3.htm", _TR("Ф3"));
    if (g) {
      q = readQ(ado, "korrozia\\plan_ustanovki_indikatorov_korrozii", "", -1);
      drawTable(ado, q, g, true);
      print_doc_end(g);
    }

    g = print_doc(f, "korrozia\\k1.htm", _TR("Ф1"));
    if (g) {
      q = readQ(ado, "korrozia\\ocenka_korrozijnogo_processa_po_rezultatam_obrabotki_indikatorov_korrozii_slice_year", "", -1);
      drawTable(ado, q, g, true);
      print_doc_end(g);
    }

    g = print_doc(f, "korrozia\\k2.htm", _TR("Ф2"));
    if (g) {
      q = readQ(ado, "korrozia\\ocenka_korrozijnogo_processa_po_rezultatam_obrabotki_indikatorov_korrozii", "", -1);
      drawTable(ado, q, g, true);
      print_doc_end(g);
    }





    print_index2(f);
    fclose(f);
    setlocale(LC_NUMERIC, "eng");

    CString cssName;
    cssName.Format("%s\\style.css", getenv("TMP"));
    css.print(cssName);

    HINSTANCE hRun = ShellExecute(GetSafeHwnd(), "open", "excel", "\"" + tmpName + "\"", NULL, SW_SHOWMAXIMIZED);
}


FILE *print_doc_map(FILE *fi, const char *fn, const char *title, const map<CString, CString> &map_fn);


void CGidrView::pas_excel_1(const char *label, const char *sql_file, const char *html, const map<CString, CString> &map_fn, CString(*color_for_korrozia)(CString fn, CString vn))
{
    FILE* g;

    CString tmpName;
    tmpName.Format("%s\\index2.html", getenv("TMP"));

    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) {
        return;
    }


    FILE* f = fopen(tmpName, "w");
    if (!f) {
        AfxMessageBox("Закройте предыдущий паспорт", MB_OK|MB_ICONINFORMATION);
        return;
    }

    CString q;

    setlocale(LC_NUMERIC, "");

    print_index1(f, "");

    g = print_doc_map(f, html, label, map_fn);
    if (g) {
      q = readQ(ado, sql_file, "", m_cxema->m_par);

      for (auto it : map_fn) {
        q.Replace(it.first, it.second);
      }
      drawTable(ado, q, g, true, color_for_korrozia);
      print_doc_end(g);
    }

    print_index2(f);
    fclose(f);
    setlocale(LC_NUMERIC, "eng");

    CString cssName;
    cssName.Format("%s\\style.css", getenv("TMP"));
    css.print(cssName);

    HINSTANCE hRun = ShellExecute(GetSafeHwnd(), "open", "excel", "\"" + tmpName + "\"", NULL, SW_SHOWMAXIMIZED);
}

int getGodQ(CWnd* wnd, CAdoFile* ado, const char* q, const char* capt);

#include "CKorroziaZapros2.h"


void CGidrView:: OnKorrozDoc1()
{
    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) {
        return;
    }
//    CString q = "SELECT DISTINCT YEAR(data_ustanovki) FROM indikator_korrozii_po_godam ORDER BY YEAR(data_ustanovki)";
//    CString q = "SELECT DISTINCT YEAR(data_ustanovki) FROM indikator_korrozii_po_godam WHERE data_ustanovki <> '' AND NOT data_ustanovki IS NULL ORDER BY YEAR(data_ustanovki)";

    CString q = "SELECT CONCAT(YEAR(date1), '-', YEAR(date2)) AS name, YEAR(date1) AS y1, date1, date2 FROM heating_seasons ORDER BY date1";


    map<CString, CString> map_fn;

//    int year = getGodQ(this, ado, q, _TR("Выберите последний год установки"));
//    if (year < 0) return;


    CKorroziaZapros2 dlg(this);

    dlg.m_ado = ado;
    dlg.m_q = q;

    if (dlg.DoModal() != IDOK) return;

   int y1 = dlg.m_y1;
   int y2 = dlg.m_y2;

   CString s;
   CString zzz = "";
   CString years = "";
   CString numbs = "";
   CString cols = "";

    for (int y = y1; y <= y2; y++) {
//        s.Format("COALESCE(MAX(CASE WHEN t.year_number = '%d' THEN t.skk END), 0) as '%d',\n", y, y);

        s.Format("COALESCE(MAX(CASE WHEN t.year_number = '%d' THEN  IIF( t.stateIndID = 2, t.stateIndName, convert(varchar, t.skk)) END), '-') as 'Y%d',\n", y-1, y-1);


        zzz += s;
        s.Format("<td class=xl85>%d-%d</td>\n", y-1, y%100);
        years += s;

        s.Format("<col style='mso-width-source:userset;mso-width-alt:4000;'>\n");
        cols += s;
    }

    for (int y = 0; y < y2-y1+5+4 + 2; y++) {
        s.Format("<td class=xl82>%d</td>\n", y+1);
        numbs += s;
    }

    s.Format("%d", y1 - 1);
    map_fn["$Y1$"] = s;
    s.Format("%d", y1 % 100);
    map_fn["$YD1$"] = s;
    s.Format("%d", y2 - 1);
    map_fn["$Y2$"] = s;
    s.Format("%d", y2 % 100);
    map_fn["$YD2$"] = s;

    map_fn["$zzz$"] = zzz;
    map_fn["$YEARS$"] = years;
    map_fn["$NUMBS$"] = numbs   ;
    map_fn["$COLS$"] = cols;

    s.Format("%d", y2-y1+1);
    map_fn["$NYEARS$"] = s;

    s.Format("%d", y1);
    map_fn["$YEAR1$"] = s;
    s.Format("%d", y2);
    map_fn["$YEAR2$"] = s;
   
    pas_excel_1("Ф1", "korrozia\\ocenka_korrozijnogo_processa_po_rezultatam_obrabotki_indikatorov_korrozii_slice_year", "korrozia\\k1.htm", map_fn);
}

void CGidrView:: OnKorrozDoc2()
{
    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) {
        return;
    }

    CString q = "SELECT DISTINCT YEAR(data_ustanovki) FROM indikator_korrozii_po_godam ORDER BY YEAR(data_ustanovki)";

    map<CString, CString> map_fn;

//    int year = getGodQ(this, ado, q, _TR("Выберите год установки"));
//    if (year < 0) return;


    if (m_sezon_korrozia < 0) {
        setKorroziaSezonDlg();
        if (m_sezon_korrozia < 0) {
            return;
        }
    }
    int year = m_sezon_korrozia;

    CString s;
    s.Format("%d", year);
    map_fn["$YEAR1$"] = s;
    s.Format("%d", year+1);
    map_fn["$YEAR2$"] = s;

    pas_excel_1("Ф1", "korrozia\\ocenka_korrozijnogo_processa_po_rezultatam_obrabotki_indikatorov_korrozii", "korrozia\\k2.htm", map_fn, color_for_korrozia);
}

void CGidrView:: OnKorrozDoc3()
{
    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) {
        return;
    }
    CString q = "SELECT DISTINCT YEAR(data_ustanovki) FROM indikator_korrozii_po_godam ORDER BY YEAR(data_ustanovki)";

    map<CString, CString> map_fn;

//    COleDateTime v = COleDateTime::GetCurrentTime();

    if (m_sezon_korrozia < 0) {
        setKorroziaSezonDlg();
        if (m_sezon_korrozia < 0) {
            return;
        }
    }
    int year = m_sezon_korrozia;

//    int year = v.GetYear();


//    int year = getGodQ(this, ado, q, _TR("Выберите год планирования"));
//    if (year < 0) return;

    CString s;
    s.Format("%d", year);
    map_fn["$YEAR1$"] = s;
    s.Format("%d", year+1);
    map_fn["$YEAR2$"] = s;
    
    pas_excel_1("Ф3", "korrozia\\plan_ustanovki_indikatorov_korrozii", "korrozia\\k3.htm", map_fn);
}

void CGidrView:: OnKorrozDoc4()
{
}

void CGidrView:: OnKorrozDoc5()
{
}


