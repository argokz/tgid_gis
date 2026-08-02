#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
#include "geofile.h"
#include "ado.h"
#include "win.h"

#include <locale.h>
#include "electro.h"


void print_index1(FILE *f, const char *title);
void print_index2(FILE *f);
void print_index(FILE *f, const char *title, const char *fn);
void print_doc_end(FILE *f);
FILE *print_doc(FILE *fi, const char *fn, const char *title, void(*filtr) (CString &s1) = NULL);

CString getDefekt2Q()
{
    CString q = 
"SELECT _TAB1_.id AS id,_TAB2_.name AS 'Состояние',_TAB1_.data_osmotra,_TAB1_.vremya_osmotra,"
" re.naimenovanie_rayona_ekspluatatsii_istochnika_tepla AS 'Район эксплуатации',"
" ue.nomer_uchastka AS 'Номер участка',"
" mag.naimenovanie_magistrali AS 'Магистраль',"
" ms.opisanie_uchastka_ms AS 'Участок МС',"
" rs.[naimenovanie_uchastka_rs] AS 'Участок РС',"
" ec1.name AS 'Код узла 1',"
" n1.externalNodeName AS 'Наименование узла 1',"
" ec2.name AS 'Код узла 2',"
" n2.externalNodeName AS 'Наименование узла 2',"

" _TAB1_.otchet_po_defektu,_TAB1_.dlina_zamenennoy_truby,_TAB1_.data_nachala_remonta,_TAB1_.data_zaversheniya_remonta"
" FROM  [defekt2] _TAB1_  "
" LEFT JOIN statedefect _TAB2_ ON _TAB1_.stateID=_TAB2_.id"
" LEFT JOIN linesobj l ON l.id=_TAB1_.lineID"
" LEFT JOIN nodes n1 ON n1.id=l.nodeID1"
" LEFT JOIN nodes n2 ON n2.id=l.nodeID2"
" LEFT JOIN externalCodes ec1 ON ec1.id=n1.externalCodeID"
" LEFT JOIN externalCodes ec2 ON ec2.id=n2.externalCodeID"

" LEFT JOIN heatPipeSections hps ON hps.lineID=l.id"
" LEFT JOIN magistrali mag on mag.id=hps.magistral"
" LEFT JOIN uchastok_ms ms on ms.id=hps.magistralSite"
" LEFT JOIN uchastok_rs rs on rs.id=hps.distSite"
" LEFT JOIN uchastki_ekspluatatsii ue ON ue.id=rs.nomer_uchastka OR ue.id=ms.nomer_uchastka"
" LEFT JOIN rayon_ekspluatatsii re ON re.id=ue.rayon_ekspluatatsii";

    return q;
}

CString getShurf2Q()
{
    CString q = 

"SELECT _TAB1_.id AS id,_TAB2_.name AS 'Наименование',_TAB1_.data_osmotra,_TAB1_.vremya_osmotra,"
" re.naimenovanie_rayona_ekspluatatsii_istochnika_tepla AS 'Район эксплуатации',"
" ue.nomer_uchastka AS 'Номер участка',"
" mag.naimenovanie_magistrali AS 'Магистраль',"
" ms.opisanie_uchastka_ms AS 'Участок МС',"
" rs.[naimenovanie_uchastka_rs] AS 'Участок РС',"
" ec1.name AS 'Код начального узла',"
" n1.externalNodeName AS 'Наименование начального узла',"
" ec2.name AS 'Код конечного узла',"
" n2.externalNodeName AS 'Наименование конечного узла',"

" _TAB1_.otchet_po_defektu,_TAB1_.data_nachala_remonta,_TAB1_.data_zaversheniya_remonta , _TAB1_.stateID"
" FROM  [shurf2] _TAB1_  "
" LEFT JOIN statedefect _TAB2_ ON _TAB1_.stateID=_TAB2_.id"
" LEFT JOIN linesobj l ON l.id=_TAB1_.lineID"
" LEFT JOIN nodes n1 ON n1.id=l.nodeID1"
" LEFT JOIN nodes n2 ON n2.id=l.nodeID2"
" LEFT JOIN externalCodes ec1 ON ec1.id=n1.externalCodeID"
" LEFT JOIN externalCodes ec2 ON ec2.id=n2.externalCodeID"

" LEFT JOIN heatPipeSections hps ON hps.lineID=l.id"
" LEFT JOIN magistrali mag on mag.id=hps.magistral"
" LEFT JOIN uchastok_ms ms on ms.id=hps.magistralSite"
" LEFT JOIN uchastok_rs rs on rs.id=hps.distSite"
" LEFT JOIN uchastki_ekspluatatsii ue ON ue.id=rs.nomer_uchastka OR ue.id=ms.nomer_uchastka"
" LEFT JOIN rayon_ekspluatatsii re ON re.id=ue.rayon_ekspluatatsii";

    return q;
}

CString getKorrozQ()
{
    CString q = 

            "SELECT  _TAB1_.id AS id "
            //"--      ,sostoyanie "
            "    , si.name AS sostoyanie "
            "      ,nomer_indikatora_korrozii "
            //"--      ,truboprovod "
            "    ,es.name AS truboprovod "
            "      ,kolichestvo_plastin_v_sborke "
            "      ,sredniy_ves_plastiny_pri_ustanovke__g "
            "      ,radius_krugloy_plastiny__mm "
            "      ,tolschina_plastiny__mm "
            "      ,data_planirovaniya "
            "      ,data_ustanovki "
            "      ,data_izvlecheniya "
            "      ,sredniy_ves_plastiny_posle_ispytaniy__g "
            "      ,poterya_massy_srednyaya_pri_kislotnoy_obraboke__g "
            "      ,srednyaya_skorost_korrozii__mm_god "
            //"      ,otsenka_korrozionnogo_protsessa "
            "      ,cpm.name AS otsenka_korrozionnogo_protsessa"
            "      ,vneshniy_vid_plastin "
            "  FROM AstanaGID.dbo.indikator_korrozii _TAB1_ "
            "  LEFT JOIN stateIndicator si ON si.id=_TAB1_.sostoyanie "
            "  LEFT JOIN externalSigns es ON es.id=_TAB1_.truboprovod "
            "  LEFT JOIN corrosionProcessMarks cpm ON cpm.id=_TAB1_.otsenka_korrozionnogo_protsessa ";
    
    return q;
}


bool CGidrView::find_remont(const CFPoint &pt, char *fn, int &n)
{
    strcpy(fn, STR_REMONT_DEFECT);      n = m_geofile->findXY3(pt, D5*geom.masx, 3, transl(fn));  if (n > 0) return true;
    strcpy(fn, STR_REMONT_DEFECT2);     n = m_geofile->findXY3(pt, D5*geom.masx, 3, transl(fn));  if (n > 0) return true;
    strcpy(fn, STR_REMONT_SHURF);       n = m_geofile->findXY3(pt, D5*geom.masx, 3, transl(fn));  if (n > 0) return true;
    strcpy(fn, STR_REMONT_SHURF2);      n = m_geofile->findXY3(pt, D5*geom.masx, 3, transl(fn));  if (n > 0) return true;
    strcpy(fn, STR_REMONT_OSMOTR);      n = m_geofile->findXY3(pt, D5*geom.masx, 3, transl(fn));  if (n > 0) return true;
    strcpy(fn, STR_REMONT_KORROZ);      n = m_geofile->findXY3(pt, D5*geom.masx, 3, transl(fn));  if (n > 0) return true;
    strcpy(fn, STR_REMONT_ISPYT);       n = m_geofile->findXY3(pt, D5*geom.masx, 3, transl(fn));  if (n > 0) return true;
    strcpy(fn, STR_REMONT_KAPITAL);     n = m_geofile->findXY3(pt, D5*geom.masx, 3, transl(fn));  if (n > 0) return true;
    strcpy(fn, STR_REMONT_CONTROL_TU);  n = m_geofile->findXY3(pt, D5*geom.masx, 3, transl(fn));  if (n > 0) return true;

    return false;
}

bool CGidrView::del_remont(const CFPoint &pt)
{
    char fn[256];
    int n;

    if (find_remont(pt, fn, n)) {
        return del_geo(transl(fn), n);
    }
    return false;
}

bool CGidrView::info_remont(const CFPoint &pt)
{
    char fn[256];
    int n;

    if (find_remont(pt, fn, n)) {
        GeoInfo(transl(fn), n, false);
        return true;
    }
    return false;
}

/*
bool CGidrView::info_kapremont(const CFPoint &pt)
{
char fn[256];

strcpy(fn, STR_REMONT_KAPITAL);
int n = m_geofile->findXY3(pt, D5*masx, 3, fn);

if (n > 0) {
Klassif *kls = m_geofile->m_kl_list->findKlN(fn);
if (kls) {
int id = kls->GetNom2(n);
if (id > 0) {
int id2 = kls->geo4[id-1]->id_remont;

//        m_n_kapremont = id2;

if (vyd_kapremont(STR_REMONT_KAPITAL, id2)) {
m_geofile->m_old_remont = id2;

//          GeoInfo(fn, n);
refreshRemont();
Invalidate();
}
}
}
return true;
}
return false;
}
*/

bool vyd_kapremont(const char *tn, int id);

bool CGidrView::info_kapremont(const CFPoint &pt)
{
    char fn[256];

    strcpy(fn, STR_REMONT_KAPITAL);
    int n = m_geofile->findXY3(pt, D5* geom.masx, 3, fn);

    if (n == 0) {
        //    strcpy(fn, STR_REMONT_OPRESSOVKA);  
        //    n = m_geofile->findXY3(pt, D5*masx, 3, fn);  
    }

    if (n > 0) {
        Klassif *kls = m_geofile->m_kl_list->findKlN(fn);
        if (kls) {
            int id = kls->GetNom2(n);
            if (id > 0) {
                int id2 = kls->geo4[id - 1]->id_remont;

                if (vyd_kapremont(fn, id2)) {
                    m_geofile->m_old_remont = id2;
                    m_geofile->m_old_TN = fn;

                    //          GeoInfo(fn, n);
                    m_geofile->refreshRemont();
                    Invalidate();
                }
            }
        }
        return true;
    }
    return false;
}


#include "InputDia.h"


void CGidrView::OnKorrozOtchet()
{
//    korrozia_pas();
//    return;



    CTime theTime = CTime::GetCurrentTime();
    CString qt = theTime.Format("%Y");

    CInputDialog inD(AfxGetMainWnd(), "", _TR("Введите последний год обработки индикатора коррозии"), qt);
    if (inD.DoModal() == IDOK) {
        int y2;
        if (sscanf(inD.m_strInput, "%d", &y2) == 1) {
            CString tmpName;
            tmpName.Format("%s\\%s", getenv("TMP"), _TR("Индикатор коррозии.docx"));
            WordKorroz(tmpName, y2, "PATH_KORROZ");
        }
    }
    return;
}



void CGidrView::OnRemontOtchet()
{
    CString tmpName;
    tmpName.Format("%s\\index2.html", getenv("TMP"));

    CString tn = STR_REMONT_DEFECT;

    FILE* f = fopen(tmpName, "w");
    if (!f) return;

    setlocale(LC_NUMERIC, "");

    print_index1(f, "");

    CString file_name = "remont\\Ремонты.html";
    CString title = m_bottom_info.title;
    CString q = m_bottom_info.q;

    CAdoFile* ado = getAdo(getGidAdoName());

    FILE* g = print_doc(f, file_name, title);

    if (g) {
        if (ado->openTable0(q)) {
            int nf = ado->NFlds();

            while (!ado->isEOF()) {
                fprintf(g, "<tr>");
                CString s;
                for (int i = 2; i <= nf; i++) {
                    s = ado->readStr(i - 1);
                    fprintf(g, "<td class=xl67>%s\xA0</td>", s);
                }
                fprintf(g, "</tr>");
                ado->MoveNext();
            }
            ado->closeTable();
        }
        print_doc_end(g);
    }

    print_index2(f);
    fclose(f);
    setlocale(LC_NUMERIC, "eng");

    tmpName.Format("%s\\index2.html", getenv("TMP"));

    HINSTANCE hRun = ShellExecute(GetSafeHwnd(), "open", "excel", "\"" + tmpName + "\"", NULL, SW_SHOWMAXIMIZED);
}

bool vyd_kapremont(const char *tn, int id)
{
    AfxMessageBox(_TR("Временно удалено 802"), MB_OK|MB_ICONINFORMATION);
    return true;
}

void CGidrView::moveUchRemont(const char *tn, int ID)
{
    CFPoint pt;
    CString s = STR_REMONT_KAPITAL;
    if (!strcmp(tn, transl("Опрессовка"))) {
        //    s = STR_REMONT_OPRESSOVKA;
    }

    Klassif *kls = m_geofile->m_kl_list->findKlN(s);
    if (kls) {
        int i;
        int n = kls->geo4.size();

        for (i = 0; i < n; i++) {
            kls->geo4[i]->mark = false;
        }

        for (i = 0; i < n; i++) {
            if (kls->geo4[i]->id_remont == ID) {
                if (isLine(kls->loc)) {
                    double x1, y1, x2, y2;
                    kls->geo4[i]->cl.napr(x1, y1, x2, y2);
                    kls->geo4[i]->mark = true;
                    pt.x = (x1 + x2) / 2;
                    pt.y = (y1 + y2) / 2;
                }
                else {
                    pt = kls->geo4[i]->cl.centroid();
                }

                moveXY(pt);

                //        break;
            }
        }
    }
    Invalidate();
}

//#include "RemontZapros.h"

#include "RemontZapros.h"

CString readQ(CAdoFile *ado, const char *tn, const char *tn2, int fileID);
CString getJoinQ(CAdoFile *ado, const char *tn);


CString getTableDefect(int typ)
{
    switch (typ) {
    case ID_TU_ON_OFF:  /// Технические условия
        break;
    case ID_VIEW_TOOLBAR_ELECTRO:  /// Электрические сети
        return EL_LINE;
    case ID_VIEW_TOOLBAR_DEFECT:  /// Дефекты
        return "defekt2";
    case ID_VIEW_TOOLBAR_SHURF:  /// Дефекты
        return "shurf2";
    case TIP_DEFECT2:  /// Дефекты
        return "defekt2";
    case TIP_SHURF2:  /// Дефекты
        return "shurf2";
    case ID_VIEW_TOOLBAR_KORROZIA:  /// Индикаторы коррозии
    case TIP_KORROZIA:  /// Индикаторы коррозии
        return REM_INDICATOR;
    case ID_VIEW_TOOLBAR_OPRESSOVKA:  /// Опрессовка
        return "opressovka_defekt";
    case ID_VIEW_TOOLBAR_KAPREMONT:  /// Капитальный ремонт
        return "kapitalnyy_remont";
    case ID_VIEW_TOOLBAR_CONTROL_TU:  /// Контроль технического состояния
        return transl(STR_REMONT_CONTROL_TU);
    }

    return "";
}


void CGidrView::SetButtomInfo(int typ0, int stateID, bool is_date, CTime date1, CTime date2)
{
    CString tn = "", title = "", q = "";
    CString df = "";

    CAdoFile* ado = getAdo(getGidAdoName());

    switch (typ0) {
    case ID_TU_ON_OFF:  /// Технические условия
        title = "";
        tn = transl(title);
        q = readQ(ado, tn, tn, -1);
        break;

    case ID_VIEW_TOOLBAR_ELECTRO:  /// Электрические сети
        title = _TR("Линия электропередач");
        tn = transl(title);
        q = readQ(ado, "electro", tn, -1);
        break;

    case TIP_DEFECT2:  /// Дефекты
    case ID_VIEW_TOOLBAR_DEFECT:  /// Дефекты
        title = _TR("Повреждение");
        tn = "defekt2";
        q = getDefekt2Q();
        df = transl("Дата осмотра");
        break;

    case TIP_SHURF2:  /// Дефекты
    case ID_VIEW_TOOLBAR_SHURF:  /// Шурфы
        title = _TR("Шурф");
        tn = "shurf2";
        q = getShurf2Q();
        df = transl("Дата осмотра");
        break;

    case ID_VIEW_TOOLBAR_KORROZIA:  /// Индикаторы коррозии
    case TIP_KORROZIA:  /// Индикаторы коррозии
        tn = REM_INDICATOR;
        title = _TR("Индикатор корозии");
        q = getKorrozQ();
        df = transl("Дата установки");
        break;

    case ID_VIEW_TOOLBAR_OPRESSOVKA:  /// Опрессовка
        title = _TR("Опрессовка");
        tn = "opres";
        q = readQ(ado, tn, tn, -1);
        break;

    case ID_VIEW_TOOLBAR_KAPREMONT:  /// Капитальный ремонт
        tn = "kapitalnyy_remont";
        title = _TR("Капитальный ремонт");
        tn = transl(title);
        q = readQ(ado, tn, tn, -1);

        break;
    case ID_VIEW_TOOLBAR_CONTROL_TU:  /// Контроль технического состояния
        title = STR_REMONT_CONTROL_TU;
        tn = transl(title);
        q = readQ(ado, tn, tn, -1);
        break;
    }

    bool create_vyd_line_table(CAdoFile* ado, CGraph2* graph, CString& tn);
    if (m_cxema->m_graph->isMark()) {
        CString temp_name, qq;
        create_vyd_line_table(ado, m_cxema->m_graph, temp_name);
        qq.Format(" JOIN %s temp ON temp.id=_TAB1_.lineID", temp_name);
        q += qq;
    }

    CString s, wh = " WHERE 1=1  ";

    if (stateID > 0) {
        if (typ0 == ID_VIEW_TOOLBAR_KORROZIA || typ0 == TIP_KORROZIA) {
            s.Format(" AND sostoyanie=%d", stateID);
        }
        else {
            s.Format(" AND stateID=%d", stateID);
        }
        wh += s;
    }

    q = q + wh;

    if (is_date && df != "") {
        CString ds1 = date1.Format(_T("%Y%m%d"));
        CString ds2 = date2.Format(_T("%Y%m%d"));
        wh.Format(" AND '%s' <= %s AND %s <= '%s'", ds1, df, df, ds2);
        q = q + wh;
    }

    m_bottom_info.q = q;
    m_bottom_info.title = title;
    m_bottom_info.tn = tn;
    m_bottom_info.typ = virt_data_geo;

}



void CGidrView::RemontZapros(int typ, int stateID)
{
    CRemontZapros dlg(this);

    if (dlg.DoModal() == IDOK) {

        m_geofile->setVisibleAllRemonts();
        
        //    pm_cSheetBottom->m_remonts->zapros("", a, true, dlg.m_check, dlg.m_date1, dlg.m_date2);
        SetButtomInfo(typ, stateID, true, dlg.m_date1, dlg.m_date2);

        m_geofile->setVisible(m_bottom_info.tn, m_bottom_info.q);
        Invalidate();

        CAdoFile *ado = getAdo(getGidAdoName());

        RunTableNiz(ado, m_bottom_info.tn, m_bottom_info.q, m_bottom_info.title, true, true, m_bottom_info.typ);
    }
}



void CGidrView::OnRemontZapros(const char *aa)
{
}


void CGidrView::OnRemontAllAll()
{
    CTime date1, date2;

    CMainFrame *mf = (CMainFrame *)AfxGetMainWnd();
    int code = mf->getToolbarID();
    SetButtomInfo(code, 0, false, date1, date2);
    
    m_geofile->setVisibleAllRemonts();
    Invalidate();
}

void CGidrView::OnRemontAll()
{
    RemontZapros(TIP_DEFECT2, 0);
}

void CGidrView::OnRemont1()
{
    RemontZapros(TIP_DEFECT2, 1);
}

void CGidrView::OnRemont2()
{
    RemontZapros(TIP_DEFECT2, 2);
}

void CGidrView::OnRemont3()
{
    RemontZapros(TIP_DEFECT2, 3);
}

/*

void CGidrView::OnShurfAll()
{
    RemontZapros(TIP_SHURF2, 0);
}
*/

void CGidrView::OnShurf1()
{
    RemontZapros(TIP_SHURF2, 1);
}

void CGidrView::OnShurf2()
{
    RemontZapros(TIP_SHURF2, 2);
}

void CGidrView::OnShurf3()
{
    RemontZapros(TIP_SHURF2, 3);
}

void CGidrView::OnKapitalAll()
{
    RemontZapros(TIP_KAPITAL, 0);
}

void CGidrView::OnKapital1()
{
    RemontZapros(TIP_KAPITAL, 1);
}

void CGidrView::OnKapital2()
{
    RemontZapros(TIP_KAPITAL, 2);
}

void CGidrView::OnKapital3()
{
    RemontZapros(TIP_KAPITAL, 3);
}

/*
void CGidrView::OnKorrozZapros(const char *a)
{
CRemontZapros dlg(this);

if (dlg.DoModal() == IDOK) {
pm_cSheetBottom->m_korroz->zapros("", a, true, dlg.m_check, dlg.m_date1, dlg.m_date2);
}
}
*/

void CGidrView::OnOpressovkaAll()
{
    RemontZapros(TIP_OPRESSOVKA, 0);
}

void CGidrView::OnOpressovka1()
{
    RemontZapros(TIP_OPRESSOVKA, 1);
}

void CGidrView::OnOpressovka2()
{
    RemontZapros(TIP_OPRESSOVKA, 2);
}

void CGidrView::OnOpressovka3()
{
    RemontZapros(TIP_OPRESSOVKA, 3);
}

void CGidrView::OnKorrozZapros(const char *a)
{
    OnRemontZapros(_TR("После ремонта"));
}

void CGidrView::OnKorrozAll()
{
    RemontZapros(TIP_KORROZIA, 0);
}

void CGidrView::OnKorroz1()
{
    RemontZapros(TIP_KORROZIA, 1);
}

void CGidrView::OnKorroz2()
{
    RemontZapros(TIP_KORROZIA, 2);
}

void CGidrView::OnKorroz3()
{
    RemontZapros(TIP_KORROZIA, 3);
}


void CGidrView::OnPopupAddKapital()
{
    Klassif *kls = m_klsP;
    if (kls) {
        int id2 = kls->GetNom2(m_ng);
        if (id2 > 0) {
            m_geofile->m_old_remont = kls->geo4[id2 - 1]->id_remont;
            m_geofile->m_old_TN = STR_REMONT_KAPITAL;

            m_nRegim = ID_REMONT_KAPITAL;
            regim = R_INIT;
            SetCur(m_nRegim);
        }
    }
}

bool CGidrView::dob_kapital(const CFPoint &pt)
{
    //  if (m_geofile->m_old_TN == STR_REMONT_OPRESSOVKA) {
    //    return dob_opressovka(pt);
    //  }

    CLINE2 *line = NULL;
    CNode2 *node = NULL;

    findNodeLine2(pt, &node, &line, D5);

    if (line) {
        int n = m_geofile->findXY3(pt, D5* geom.masx, 2, STR_REMONT_KAPITAL);
        if (n) {
            //      AfxMessageBox("!", MB_OK|MB_ICONINFORMATION);
            return false;
        }

        Klassif *kls = m_geofile->m_kl_list->findKlN(transl(STR_REMONT_KAPITAL));
        if (kls) {
            CCoordList m_NP = bline(line)->getCLall();
            int n = m_geofile->createObjKapital(kls->nazv, m_NP, line);

            if (n) {
                kls->geo4[n - 1]->mark = true;
                GeoInfo(kls->nazv, kls->geo4[n - 1]->nom, true);
            }
        }
    }
    return false;
}

#if 0
bool CGidrView::dob_opressovka(const CFPoint &pt)
{
    CLINE2 *line = NULL;
    CNode *node = NULL;

    findNodeLine2(pt, &node, &line, D5);

    if (line) {
        int n = m_geofile->findXY3(pt, D5*masx, 2, STR_REMONT_OPRESSOVKA);
        if (n) {
            //      AfxMessageBox("!", MB_OK|MB_ICONINFORMATION);
            return false;
        }

        Klassif *kls = m_geofile->m_kl_list->findKlN(transl(STR_REMONT_OPRESSOVKA));
        if (kls) {
            CCoordList m_NP = bline(line)->getCLall();
            int n = m_geofile->createObjOpressovka(kls->nazv, m_NP, line);

            if (n) {
                kls->geo4[n - 1]->mark = true;
                m_geofile->edit(this, kls->nazv, kls->geo4[n - 1]->nom);
            }
        }
    }
    return false;
}

#endif


void CGidrView::OnOprNeopr()
{
    CString query;

    m_isNeOpr = true;
    Invalidate();

    query.Format("SELECT * FROM [%s] WHERE [Участок очереди опрессовки] <= 0 OR [Участок очереди опрессовки] IS NULL", ST_UT);

//    RunTable(ST_UT, query, -1);
}


#if 0

int CGeoFile::createObjRemont(const char *tN, const CCoordList &cl, CLINE2 *l) 
{ 
  return createObj(tN, cl);
}

#else 


#if 0

void copyUT_Remont(CDbFile *db1, CDbFile *m_db, int po, const char *fn1, const char *fn2)
{
    try {
        COleVariant v;
        db1->rs->GetFieldValue(fn1, v);

        CString fn = fn2;

        if (po == COBR) {
            fn.Replace(_TR("подающий"), _TR("обратный"));
        }
        m_db->rs->SetFieldValue(fn, v);
    }
    catch (CDaoException *e) {
        e->Delete();
    }
}


void copyUT_Remont(CDbFile *db1, CDbFile *m_db, int po)
{
    copyUT_Remont(db1, m_db, po, "dateNew", _TR("Год ввода в эксплуатацию"));
    copyUT_Remont(db1, m_db, po, _TR("Тип трубы"), _TR("Тип трубы"));
    copyUT_Remont(db1, m_db, po, _TR("Характеристика трубы"), _TR("Характеристика трубы"));
    copyUT_Remont(db1, m_db, po, _TR("Тип изоляции"), _TR("Конструкция изоляции, подающий"));

    copyUT_Remont(db1, m_db, po, _TR("Материал наружного покрытия"), _TR("Покровный слой материал, подающий"));
    copyUT_Remont(db1, m_db, po, _TR("Материал наружного покрытия"), _TR("Конструкция покровного слоя, подающий"));
    copyUT_Remont(db1, m_db, po, _TR("Толщина наружного покрытия, мм"), _TR("Толщина наружного покрытия, мм, подающий"));

    copyUT_Remont(db1, m_db, po, "kod_izol", _TR("Тепловая изоляция материал, подающий"));
    copyUT_Remont(db1, m_db, po, "thickizol", _TR("Толщина изоляции, мм, подающий"));

    copyUT_Remont(db1, m_db, po, "kod_owner", _TR("Организация"));
}
#endif



int CGeoFile::createObjRemont(const char *tN, CCoordList &cl, CLINE2 *l, map<CString, COleVariant>& m_map_geo_value)
{
  CString sZD = _TR("Здания");
  Klassif *klsz = m_geofile->m_kl_list->findKlN(transl(sZD));
  CString uu = "", nn = "", oo = "";
 
  Klassif *kls = m_geofile->m_kl_list->findKlN(transl(tN));

  if (!kls) return 0;

  if (klsz) {
    int n_zd = m_geofile->findXY3_bl(cl.first(), 0, 3,  sZD);
    if (n_zd > 0) {
/*
      if (m_geofile->InitS(sZD)) {
        if (m_geofile->GoToID(n_zd)) {
          uu = klsz->m_db->read("NAME_STREE");
          nn = klsz->m_db->read("NUMBER_1");
          oo = klsz->m_db->read("OWNER");
        }
      }
*/
    }
  }

  int id =  createObj(tN, cl, m_map_geo_value);


//  if (!InitS(tN)) return 0;

//  m_db->AddNew();

  CCoordList cl2 = cl;
//  cl2.y_minus();
//  write_cl(tN, cl2);

//  CNode *n1 = where(l);
//  CNode *n2 = other(l);

  double dl1 = 0, dl2 = 0;

//  CNode *nn1 = n1->isKam() ? n1 : find_next11(n1->m_graph, n1, n2, n1, dl1);
//  CNode *nn2 = n2->isKam() ? n2 : find_next11(n2->m_graph, n2, n1, n2, dl2);

//  m_db->writeLine(1, n1->node_name.kod, n1->node_name.name, ' ');
//  m_db->writeLine(2, n2->node_name.kod, n2->node_name.name, ' ');
//  int rasst(CLINE *l, CFPoint p)

/*
  if (nn1) {
    m_db->write(_TR("Код РС начальной камеры"), nn1->node_name.kod);
    m_db->write(_TR("Начальная камера"), nn1->node_name.name);
  }
  if (nn2) {
    m_db->write(_TR("Код РС конечной камеры"), nn2->node_name.kod);
    m_db->write(_TR("Конечная камера"), nn2->node_name.name);
  }

*/

  CFPoint p = cl.first();
  
//  double dl1_w = rasst(l, p);
//  double dl2_w = getLengthLine(l, false) - dl1_w;

//  dl1 += dl1_w;
//  dl2 += dl2_w;

//  CNode *nb = dl1 < dl2 ? nn1 : nn2;

  CAdoFile *ado = kls->m_ado;

  ado->Edit(tN, id);

  ado->write(transl("Участок МС"), (long)bline(l)->line.ms);
  ado->write(transl("Участок РС"), (long)bline(l)->line.rs);
  ado->write(transl("Магистраль"), (long)bline(l)->line.mag);

  ado->write(transl("Опрессовки участок очереди"), (long)bline(l)->line.m_opressovka);

  ado->Update();

#if 0
  m_db->write(_TR("Код РС ближайшей камеры"), nb->node_name.kod);
  m_db->write(_TR("Ближайшая камера"), nb->node_name.name);

  m_db->write(_TR("Расстояние до камеры, м"), min(dl1, dl2));

  m_db->write(_TR("Улица"), uu);
  m_db->write(_TR("Номер дома"), nn);


  m_db->write(_TR("Диаметр трубы подающий, условн, мм"), bline(l)->pod.diam_usl);
  m_db->write(_TR("Диаметр трубы обратный, условн, мм"), bline(l)->obr.diam_usl);

  m_db->write(_TR("Длина участка трубопровода, подающий, м"), bline(l)->pod.dl);
  m_db->write(_TR("Длина участка трубопровода, обратный, м"), bline(l)->obr.dl);

  char ss[2]; ss[0] = bline(l)->name_typ; ss[1] = 0;

  m_db->write(_TR("Вид прокладки"), ss);

  CString mag = getKod(l);
  m_db->write(_TR("Магистраль/Распредсеть"), mag);

  m_db->write(_TR("Район эксплуатации"), getRE(l));
  m_db->write(_TR("Участок эксплуатации"), getUch(l));
  m_db->write(_TR("Начальник участка"), getNach(l));



  CDbFile *db1 = where(l)->file->m_dbf;

  if (db1->openTable(linef[bline(l)->typ].table)) {
    if (bline(l)->nomP > 0 && db1->GotoKey(bline(l)->nomP)) {
      copyUT_Remont(db1, m_db, CPOD);
    }
    if (bline(l)->nomO > 0 && db1->GotoKey(bline(l)->nomO)) {
      copyUT_Remont(db1, m_db, COBR);
    }

    db1->closeTable();
  }

  CString ss1 = where(l)->m_graph->m_cxema->getIST(mag);

  m_db->write(_TR("Источник тепла"), ss1);

//  m_db->write(_TR("Вид сети"), where(l)->m_graph->isRS()? _TR("РС") : _TR("МС"));

//  m_db->write(_TR("Длина, м п"), bline(l)->pod.dl);
//  m_db->write(_TR("Диаметр внешний, мм"), bline(l)->pod.diam_usl+bline(l)->pod.tol*2);

  COleDateTime t = COleDateTime::GetCurrentTime();

  write_date(m_db, _TR("Дата осмотра"), t);
  write_date(m_db, _TR("Дата установки"), t);
  write_date(m_db, _TR("Дата контроля"), t);

  CString st;
  st.Format("%02d:%02d", t.GetHour(), t.GetMinute());
  m_db->write(_TR("Время осмотра"), st);

  m_db->Update();
  m_db->rs->SetBookmark( m_db->rs->GetLastModifiedBookmark( ) );

  int ID;

  m_db->read_first(ID);

  if ( n_shape != -1 ) {
    try {
      if (m_db->openTable(m_kls->xy_table.c_str())) {

        CFRect rect = cl.GetFRect();

        double gs = m_kls->IdxGridSize;

        double x1 = rect.left/gs/100;
        double x2 = rect.right/gs/100;
        double y1 = -rect.bottom/gs/100;
        double y2 = -rect.top/gs/100;

        m_db->rs->AddNew();
        m_db->rs->SetFieldValue(0, COleVariant((long)ID));
        m_db->rs->SetFieldValue(1, COleVariant((long)x1));
        m_db->rs->SetFieldValue(2, COleVariant((long)y1));
        m_db->rs->SetFieldValue(3, COleVariant((long)x2));
        m_db->rs->SetFieldValue(4, COleVariant((long)y2));

        m_db->rs->Update();

        m_db->closeTable();
      }
    }
    catch (CDaoException *e) {
      e->Delete();
    }
  }

  CGeoObject *g = new CGeoObject;

  g->nom = ID;
  g->nom2 = ID;
  g->mark = 0;
  g->rect = cl.GetFRect();
  m_kls->geo4.push_back(g);

  return m_kls->geo4.size();

#endif 

  return id;

}

#endif

CString argpath();

#include "CWear.h"

void CGidrView::OnWear()
{
    CString q;
    CAdoFile *ado = getAdo(getGidAdoName());

    if (!ado) return;
    long affected;
    CWear dlg(this);

    q.Format("SELECT DISTINCT DateRascheta AS dt FROM ps.CALCULATION_IZNOS ORDER BY DateRascheta DESC");
    if (ado->m_type_of_net == TYPE_OF_NET_MSSQL) {
        q.Replace(" ps.", " ");
    }

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            dlg.m_date = ado->read_date("dt");
        }
        ado->closeTable();
    }

    if (dlg.DoModal() != IDOK) return;

    CString str1 = dlg.m_date.Format(_T("%d.%m.%Y"));
    CString dt = dlg.m_date.Format(_T("%Y.%m.%d"));

    q.Format("SELECT DISTINCT id FROM ps.CALCULATION_IZNOS WHERE DateRascheta='%d'", dt);
    if (ado->m_type_of_net == TYPE_OF_NET_MSSQL) {
        q.Replace(" ps.", " ");
    }
    bool is_dt = false;

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            long id = ado->read_long(0);
            is_dt = true;
        }
        ado->closeTable();
    }
    //  q.Format("SELECT DISTINCT id iznos WHERE DateRascheta", dt);
    CString ss;

    ss.Format(_TR("Создать/обновить расчет износа за %s"), str1);

    if (!is_dt || AfxMessageBox(ss, MB_YESNO) == IDYES) {
        q.Format("DELETE FROM ps.CALCULATION_IZNOS WHERE DateRascheta='%s'", dt);
        if (ado->m_type_of_net == TYPE_OF_NET_MSSQL) {
            q.Replace(" ps.", " ");
        }
        ado->Execute(q, &affected);
        q.Format("INSERT INTO ps.CALCULATION_IZNOS (DateRascheta) VALUES ('%s')", dt);
        if (ado->m_type_of_net == TYPE_OF_NET_MSSQL) {
            q.Replace(" ps.", " ");
        }
        int idc = ado->ExecuteInsert(q, &affected);

        if (idc > 0) {
            q.Format("INSERT INTO ps.iznos(pipeSectionID,calculationID) SELECT id,%d FROM pipeSections", idc);
            if (ado->m_type_of_net == TYPE_OF_NET_MSSQL) {
                q.Replace(" ps.", " ");
            }
            ado->Execute(q, &affected);
        }
    }

    //  CString q = readFile(argpath()+"remont_sql/tgid_iznos_remont.sql");

    if (ado->m_type_of_net == TYPE_OF_NET_MSSQL) {
      q = readFile(argpath() + "remont_sql/tgid_iznos_MS.sql");
    }
    else {
      q = readFile(argpath() + "remont_sql/tgid_iznos.sql");
    }
    q.Replace("$DATE$", dt);
    viewQ2(ado, getGidAdoName(), q, _TR("Износ участков тепловой сети"), _TR("Износ участков тепловой сети"), virt_data_edit, "", "Износ участков тепловой сети");
}

void CGidrView::OnPovrezhd()
{
    AfxMessageBox("!", MB_OK|MB_ICONINFORMATION);
}



void CGeoFile::refreshRemont()
{
    CGidrView* pView = getView();

    if (pView) {
        CAdoFile* ado = getAdo(getGidAdoName());

//        if (m_sql_for_defect != "") {
//            bool ret = ado->Execute(m_sql_for_defect);
            pView->RunTableNiz(ado, pView->m_bottom_info.tn, pView->m_bottom_info.q, pView->m_bottom_info.title, false, true, pView->m_bottom_info.typ);
//            m_sql_for_defect = "";
//        }
    }
}

void CGeoFile::refreshElectro() 
{
}

