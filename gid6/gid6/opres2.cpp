#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"

#include "gidrView.h"
#include "ado.h"
#include "mmenu.h"
#include "mmenu2.h"
#include "win.h"
#include "InputDia.h"
#include <locale.h>

#include "ado2.h"

static CString table_for_del_opred;
CString sezon_name(int y);


bool setOsmotr(CAdoFile* ado, CString opres, CCxema* m_cxema, int directionID)
{
    bool ret = false;
    CString q;

    if (opres == "osmotr" || opres == "remont2") {
        q.Format(
            "Select d.lineID, faktory_riska_truboprovoda.id AS fakt_id From %sDeployed d\n"
            "LEFT JOIN %s o ON o.id=d.directionID\n"
            "JOIN heatPipeSections hps ON hps.lineID=d.lineID\n"
            "JOIN pipeSections ps1 ON ps1.id=hps.pipeSectionID\n"
            "left join faktory_riska_truboprovoda on faktory_riska_truboprovoda.lineID = ps1.id and faktory_riska_truboprovoda.objID = %d and faktory_riska_truboprovoda.obj_type_faktory_riskaID = %d\n"

            "Where directionID = %d    \n", opres, opres, directionID,
            opres == "osmotr" ? 2 : 3,
            directionID
        );

    }
    else {
        q.Format("Select lineID From %sDeployed Where directionID = %d", opres, directionID);
    }

    m_cxema->m_graph->init_find_line_nom();


#if 0
    if (opres == "osmotr") {

"SELECT \n"
"f.id,\n"
"l.id\n"

"FROM osmotr o\n"
"JOIN osmotrDeployed d ON d.directionID=o.id\n"
"JOIN linesobj l ON l.id=d.lineID AND l.removed=0\n"
"JOIN heatPipeSections hps ON hps.lineID=l.id\n"
"JOIN nodes n1 ON n1.id=l.nodeID1\n"
"JOIN nodes n2 ON n2.id=l.nodeID2\n"
"LEFT JOIN faktory_riska_truboprovoda f ON f.objID=o.id AND f.lineID=hps.pipeSectionID AND f.obj_type_faktory_riskaID=2\n"

        return;
    }
#endif



    set<int> set_fileID;

    m_cxema->m_graph->reset();
    if (ado->openTable0(q)) {
        ret = true;
        while (!ado->isEOF()) {
            long id = ado->read_long("lineID");
            long prosm = ado->read_long("fakt_id");

            CLINE2* l = m_cxema->m_graph->find_line_nom(id);

            if (l) {
                bline(l)->line.isPjezo = true;
                where(l)->node.isPjezo = true;
                other(l)->node.isPjezo = true;

                bline(l)->line.isOsmotr = prosm;
            }
            ado->MoveNext();
        }
        ado->closeTable();
    }

    q.Format(

"SELECT DISTINCT\n"
//"n1.fileID \n"
"fr.name \n"
"FROM %sDeployed d\n"
"JOIN linesobj l ON l.id=d.lineID\n"
"JOIN nodes n1 ON n1.id=l.nodeID1\n"
"JOIN fragments fr ON fr.id=n1.fileID\n"
"Where directionID=%d AND n1.fileID NOT IN (%s)\n", opres, directionID, m_cxema->m_par);

    CString ss = "";

    if (ado->openTable0(q)) {
        ret = true;
        while (!ado->isEOF()) {
            CString name = ado->readStr("name");
            if (ss != "") ss += "\n";
            ss += name;
            ado->MoveNext();
        }
        ado->closeTable();
    }
    if (ss != "")  {
        AfxMessageBox("Часть участков контура не показана на экране, откройте фрагменты сети:\n"+ss);
    }
    return ret;
}

bool del_opres(int id, CString name)
{
    CAdoFile* ado = getAdo(getGidAdoName());

    if (!ado) return false;

    CString q;
    CString opres = table_for_del_opred;

    if (opres == "opres") {
        q.Format(_TR("Удалить Опрессовку %s?"), name);
    }
    else {
        q.Format(_TR("Удалить %s?"), name);
    }
    if (AfxMessageBox(q, MB_YESNO) == IDYES) {
        q.Format("DELETE FROM %s WHERE id=%d", opres, id);
        long affected;
        bool ret = ado->Execute(q, &affected);

        if (!ret) {
            AfxMessageBox(ado->getError());
        }

        return ret;
    }

    return false;
}

CString getPar(set<int>& v);

void delOsmotrOrRemont(CAdoFile* ado, CString opres, CString text, int directionID, bool ask)
{
    CString q;

    if (opres == "opres") {
        q.Format(_TR("Удалить Опрессовку %s?"), text);
    }
    else {
        q.Format(_TR("Удалить ремонт %s?"), text);
    }
    if (!ask || AfxMessageBox(q, MB_YESNO) == IDYES) {
        q.Format("DELETE FROM %s WHERE id=%d", opres, directionID);
        long affected;
        ado->Execute(q, &affected);
        q.Format("DELETE FROM %sDeployed WHERE directionID=%d", opres, directionID);
        ado->Execute(q, &affected);

        if (opres == "osmotr") {
            q.Format("DELETE FROM faktory_riska_truboprovoda WHERE objID=%d AND obj_type_faktory_riskaID=2", directionID);
            ado->Execute(q, &affected);
        }
        if (opres == "remont2") {
            q.Format("DELETE FROM faktory_riska_truboprovoda WHERE objID=%d AND obj_type_faktory_riskaID=3`", directionID);
            ado->Execute(q, &affected);
        }
    }
}

#if 0

void CGidrView::viewOsmotr(CString opres, int directionID)
{
    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) return;

    CString q;

    q.Format("Select lineID From %sDeployed Where directionID = %d", opres, directionID);

    m_cxema->m_graph->init_find_line_nom();

    m_cxema->m_graph->reset();

    if (setOsmotr(ado, opres, m_cxema, directionID)) {
        MoveVyd();
        Invalidate();
        viewOpresDialog(opres, false);
    }
    viewOpresProtokol(opres, directionID);
}

#endif


#include "NewPassport/CWebDialog.h"


void set_ms_rs_condition(CString& qq, std::set<int>& m_set_ms, std::set<int>& m_set_rs);
void sezon_condition(CString& q, int m_sezon_korrozia, COleDateTime m_sezon_date1, COleDateTime m_sezon_date2);

void CGidrView::ListOpres(CString opres, CString title, CString name_name, CString date_name, int& m_id_opr)
{
    CAdoFile* ado = m_cxema->m_ado;

    if (!ado) return;

    CString q;

    if (opres == "remont2") {
        q =
            "SELECT DISTINCT\n"
            "r.id,\n"
            "r.otchet_po_defektu AS 'Наименование/Адрес',\n"
            "r.data_nachala_plan as 'Дата начала ремонта плановая',\n"
            "r.data_okonchaniya_plan as 'Дата окончания ремонта плановая',\n"
            "r.data_nachala_remonta as 'Дата начала ремонтных работ',\n"
            "r.data_zaversheniya_remonta as 'Дата завершения ремонтных работ',\n"
            "CASE\n"
            "WHEN r.utverdit = 0 THEN 'Не утверждено'\n"
            "ELSE 'Утверждено'\n"
            "END AS 'Утверждение плана',\n"
            "rt.name as 'Вид ремонта',\n"
            "nach.fio as 'Ответственный за ремонт'\n"
            "FROM remont2 r\n"
            "LEFT JOIN remontTypes rt ON rt.id=r.remontTypeID\n"
            "LEFT JOIN nachalniki_uchastkov nach ON nach.id=r.responsibleID\n"
            "JOIN remont2Deployed depl ON depl.directionID=r.id\n"
            "JOIN linesobj l ON l.id=depl.lineID\n"
            "JOIN heatPipeSections hpss ON hpss.lineID=l.id $and_condition2$\n"
            "WHERE\n"
            "  (\n"
            "  (r.data_nachala_plan $season_condition$) OR\n"
            "  (r.data_nachala_remonta $season_condition$)\n"
            "  ) ORDER BY r.id DESC\n";
    }

    else if (opres == "osmotr") {
        q =
            "SELECT DISTINCT\n"
            "o.id,\n"
            "o.name,\n"
            "o.data_osmotra,\n"
            "nach.fio AS 'Ответственный'\n"
            "FROM osmotr o\n"
            "LEFT JOIN nachalniki_uchastkov nach ON nach.id = otvetstvennoe_lico_ID\n"
            "JOIN osmotrDeployed depl ON depl.directionID=o.id\n"
            "JOIN linesobj l ON l.id=depl.lineID\n"
            "JOIN heatPipeSections hpss ON hpss.lineID=l.id $and_condition2$\n"
            "WHERE\n"
            "  (\n"
            "  (o.data_osmotra $season_condition$) OR\n"
            "  (o.data_osmotra $season_condition$)\n"
            "  ) ORDER BY o.id DESC \n";
    }
    else if (opres == "opres") {
        q =
            "SELECT DISTINCT\n"
            "r.id,\n"
            "r.name AS 'Описание контура',\n"
            "r.date_opres as 'Дата опрессовки',\n"
            "CASE\n"
            "WHEN r.utverdit = 0 THEN 'Не утверждено'\n"
            "ELSE 'Утверждено'\n"
            "END AS 'Утверждение плана'\n"
            "FROM opres r\n"
            "LEFT JOIN nachalniki_uchastkov nach ON nach.id=r.responsibleID\n"
            "JOIN opresDeployed depl ON depl.directionID=r.id\n"
            "JOIN linesobj l ON l.id=depl.lineID\n"
            "JOIN heatPipeSections hpss ON hpss.lineID=l.id $and_condition2$\n"
            "WHERE\n"
            "  (\n"
            "  (r.data_nachala_plan $season_condition$) OR\n"
            "  (r.date_opres $season_condition$)\n"
            "  ) ORDER BY r.id DESC \n";
    }
    else {
        CString str;
        str.Format("Здесь нельзя использовать таблицу %s", opres);
        AfxMessageBox(str);
    }

    set_ms_rs_condition(q, m_set_ms, m_set_rs);
    sezon_condition(q, m_sezon_korrozia, m_sezon_date1, m_sezon_date2);

    int n_ret = 0;

    CWebDialog* dlg = new CWebDialog(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), opres, q, n_ret, virt_data_geo_edit, title, false);

    if (dlg != NULL) {
        if (opres == "remont2") {
            dlg->setUtverditMode(true);
            dlg->setUtverditKey("col_6");
        }
        if (opres == "opres") {
            dlg->setUtverditMode(true);
            dlg->setUtverditKey("col_3");
        }

        BOOL ret = dlg->Create(IDD_TABLE_DLG, this);

        if (!ret) {
            delete dlg;
            return;
        }
    }
}


void CGidrView::ListOpresQ(CString opres, CString title, CString q, CString utverdit_key)
{
    CAdoFile* ado = m_cxema->m_ado;

    if (!ado) return;

    int n_ret = 0;

    CWebDialog* dlg = new CWebDialog(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), opres, q, n_ret, virt_data_geo_del, title, false);

    if (dlg != NULL) {

        if (utverdit_key != "") {
            dlg->setUtverditMode(true);
            dlg->setUtverditKey(utverdit_key);
        }

/*
        if (opres == "remont2") {
            dlg->setUtverditMode(true);
            dlg->setUtverditKey("col_5");
        }
        if (opres == "shurfy") {
            dlg->setUtverditMode(true);
            dlg->setUtverditKey("col_4");
        }
*/
        BOOL ret = dlg->Create(IDD_TABLE_DLG, this);

        if (!ret) {
            delete dlg;
            return;
        }
    }
}


void CGidrView::LastOpres(CString opres, CString title, int& m_id_opr, int& m_id_opr_old, bool view_dialog)
{
    m_mark_changed = false;

    if (m_id_opr < 0) {
        m_id_opr = m_id_opr_old;
    }
    if (m_id_opr < 0) {
        CString str;
        str.Format("Выберите %s", title);
        AfxMessageBox(str);
        return;
    }
    int directionID = m_id_opr;
    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) return;

    if (directionID >= 0) {
        if (setOsmotr(ado, opres, m_cxema, directionID)) {
            if (view_dialog) {
                viewOpresDialog(opres, false);
            }
            else {
                MoveVyd();
                viewOpresProtokol(opres, directionID);
            }
        }
    }
    Invalidate();
}

//-----------------------------------------------

void CGidrView::OnListOpres()
{
    ListOpres("opres", _TR("Контуры опрессовок"), "name", "date_opres", m_id_opr);
}

void CGidrView::OnListRemont2()
{
    ListOpres("remont2", _TR("Контуры ремонта"), "otchet_po_defektu", "data_osmotra", m_id_remont2);
}

void CGidrView::OnListOsvidet2()
{
    ListOpres("osvidet2", _TR("Контуры освидетельствований"), "name", "date", m_id_osvidet2);
}

//-----------------------------------------------


void CGidrView::OnLastOsmotr2()
{
    LastOpres("osmotr", _TR("Контур осмотра"), m_id_osmotr2, m_id_osmotr2_old, true);
}

void CGidrView::OnLastOpres()
{
    LastOpres("opres", _TR("Контуры опрессовок"), m_id_opr, m_id_opr_old, true);
}

void CGidrView::OnLastRemont2()
{
    LastOpres("remont2", _TR("Контуры ремонта"), m_id_remont2, m_id_remont2_old, true);
}

void CGidrView::OnLastOsvidet2()
{
    LastOpres("osvidet2", _TR("Контуры освидетельствований"), m_id_osvidet2, m_id_osvidet2_old, true);
}

//-----------------------------------------------

void CGidrView::SaveOpresNew(CString opres, CString title, CString name_name, CString name_kontur, CString date_name, int id_nachalnik, int& m_id_opr, map<CString, CString>* map_param)
{
    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) return;

    if (!isEditGeo(opres, true)) return;

    if (opres == "remont2" || opres == "osmotr") {
       auto p = m_cxema->m_graph->map_node.begin();

       while (p != m_cxema->m_graph->map_node.end()) {
           CNode2* pp = p->second;
           for (CLINE2* l = pp->lines; l; l = next(l)) {
               if (IsBegin(l) && bline(l)->line.isPjezo) {
                   if (bline(l)->line.ms <= 0 && bline(l)->line.rs <= 0) {
                       CString str;
                       str.Format("Участок %s не принадлежит ни к какому магистральному или квартальному участку. Создать %s по нему невозможно", bline(l)->getName(), title);
                       AfxMessageBox(str);
                       return;
                   }

                   if (bline(l)->line.pipeSectionID <= 0) {
                       CString str;
                       str.Format("Участок %s не принадлежит ни к какому участку ПТС. Создать %s по нему невозможно", bline(l)->getName(), title);
                       AfxMessageBox(str);
                       return;
                   }
               }
           }
           p++;
       }
    }

    CString s;
    s.Format("Сохранить новый %s?", title);

    if (AfxMessageBox(s, MB_YESNO) != IDYES) return;

    ado->AddNew(opres);
    ado->write(name_name /*"name"*/, name_kontur);

    if (date_name != "") {
        COleDateTime dtValueDate = COleDateTime::GetCurrentTime();
        ado->write(date_name /*"date"*/, dtValueDate);
    }

    //    if (m_nachalnikID > 0) {
    //        ado->write("otvetstvennoe_lico_ID", m_nachalnikID);
    //    }
    if (id_nachalnik > 0) {
        ado->write("otvetstvennoe_lico_ID", id_nachalnik);
    }


    if (map_param) {
        for (auto& it : *map_param) {
            ado->write(it.first, it.second);
        }
    }

    m_id_opr = ado->Update();

    viewOpresDialog(opres, true);
    SaveOpres(opres, title, m_id_opr, true, true);
    viewUchPTS2();
}


void CGidrView::SaveOpres(CString opres, CString title, int& m_id_opr, bool yes, bool first_save)
{
    if (!yes) return;

    if (!isEditGeo(opres, true)) return;

    if (m_id_opr <= 0) {
        AfxMessageBox("Выберите контур");
        return;
    }

    if (!first_save) {
        CString s;

        s.Format("Обновить сохранение контура?\nВы уверены?");

        if (!m_cxema->m_graph->isMark()) {
            s.Format("Контур не выделен, после сохранения контура будет удален.\nВы уверены?");
        }


        if (AfxMessageBox(s, MB_YESNO) != IDYES) {
            return;
        }
    }

    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) return;

    //    if (AfxMessageBox("Сохранить контур опрессовок?", MB_YESNO) != IDYES) return;

    int directionID = m_id_opr;

    CString q;
    q.Format("DELETE FROM %sDeployed WHERE directionID=%d", opres, directionID);
    ado->Execute(q);

    map<long, CNode2*>::const_iterator p = m_cxema->m_graph->map_node.begin();

    CString opresDeployed;
    opresDeployed.Format("%sDeployed", opres);

    CString qq;
    bool first = true;

    q.Format("INSERT INTO %sDeployed (directionID,lineID) VALUES ", opres);

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* pp = p->second;

        if (pp->node.internalNodeID == 0) {
            for (CLINE2* l = pp->lines; l; l = next(l)) {
                if (IsBegin(l) && bline(l)->line.isPjezo) {
                    if (bline(l)->line.nomP > 0) {
                        if (!first) q += ",";  first = false;
                        qq.Format("(%d,%d)", directionID, bline(l)->line.nomP);
                        q += qq;
                    }
                    if (bline(l)->line.nomO > 0 && bline(l)->line.nomP != bline(l)->line.nomO) {
                        if (!first) q += ",";  first = false;
                        qq.Format("(%d,%d)", directionID, bline(l)->line.nomO);
                        q += qq;
                    }
                }
            }
        }
        p++;
    }

    if (!first) {
        long affected;
        long n = ado->ExecuteInsert(q, &affected);
    }

    CString s;
    s.Format("%s сохранено", title);
    AfxMessageBox(s, MB_OK | MB_ICONINFORMATION);
}


bool CGidrView::contur_changed()
{
    return m_mark_changed;
}

void CGidrView::OnUpdateOsmotrSave(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(m_cxema->m_graph->isMark() && m_id_osmotr2 > 0 && contur_changed());
}


void CGidrView::OnUpdateOpresSave(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(m_cxema->m_graph->isMark() && m_id_opr > 0 && contur_changed());
}

void CGidrView::OnUpdateRemontSave(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(m_cxema->m_graph->isMark() && m_id_remont2 > 0 && contur_changed());
}



void CGidrView::OnSaveOpresNew()
{
    SaveOpresNew("opres", "Опрессовка", "name", "Контур опрессовки", "date_opres", m_nachalnikID, m_id_opr);
}

void CGidrView::OnSaveOpres()
{
    SaveOpres("opres", "Опрессовка", m_id_opr, true, false);
}

void CGidrView::OnSaveRemont2New()
{
    SaveOpresNew("remont2", "Ремонт", "name", "Контур ремонта", "date", m_nachalnikID, m_id_remont2);
}

void CGidrView::OnSaveRemont2()
{
    SaveOpres("remont2", "Ремонт", m_id_remont2, true, false);
}

void CGidrView::OnSaveOsvidet2New()
{
    SaveOpresNew("osvidet2", "Освидетельствование", "name", "Освидетельствование", "date", m_nachalnikID, m_id_osvidet2);
}

void CGidrView::OnSaveOsvidet2()
{
    SaveOpres("osvidet2", "Освидетельствование", m_id_osvidet2, true, false);
}

// Начасть сохранять испытания

void CGidrView::viewOpresDialog(CString opres, bool autodelete)
{
    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) return;

    ado_infoInit(ado, opres);

    if (opres == "opres") {
        ado_infoView(this, ado, opres, m_id_opr, "Контур опрессовки", TP_OPRES, NULL, autodelete);
    }
    else if (opres == "remont2") {
        m_prop_grid = ado_infoView(this, ado, opres, m_id_remont2, "Контур ремонта", TP_REMONT, NULL, autodelete);
        setDefectForRemonts();
    }
    else if (opres == "osvidet2") {
        ado_infoView(this, ado, opres, m_id_osvidet2, "Контур освидетельствований", TP_OSVIDET, NULL, autodelete);
    }
    else if (opres == "osmotr") {
        ado_infoView(this, ado, opres, m_id_osmotr2, "Контур осмотра", TP_OSMOTR, NULL, autodelete);
    }
}

CString readQ(CAdoFile* ado, const char* tn, const char* tn2, int fileID);
CString readQ(CAdoFile* ado, const char* tn, const char* tn2, const char* fileID);
bool create_vyd_line_table_BIG(CAdoFile* ado, CGraph2* graph, CString& tn);
bool create_vyd_node_table_BIG(CAdoFile* ado, CGraph2* graph, CString& tn, bool is_big);


void CGidrView::viewUchPTS2()
{
    CAdoFile* ado = getAdo(getGidAdoName());
    if (ado) {
        CString vyd;
        CString name = _TR("Участки ПТС");

        CGraph2* graph = m_cxema->m_graph;

        if (create_vyd_line_table_BIG(ado, graph, vyd)) {
            CString q = readQ(ado, "UT_BIG", name, 0);
            if (q != "") {
                q.Replace("$vyd$", vyd);
                //                viewQ2(ado->m_schema, q, name, name, virt_data_line_big);
                //                viewNizGeoQ(tn, "", "", true);
                //                RunTableNiz(ado, name, q, name, bool refresh, bool is_head, virt_data_line_big)
                RunTableNiz(ado, name, q, name, true, false, virt_data_line_big);
            }
        }
    }
}


bool readTableValues(CAdoFile* ado, const char* q, int n, vector<CString>& vv);


void CGidrView::viewOpresProtokol(CString opres, int& m_opr_id)
{
    CAdoFile* ado = getAdo(getGidAdoName());
    if (ado) {
        CString vyd;

        CString name = "Участки";
        CString name1;
        CString q;

        q.Format("SELECT o.name, nach.fio FROM %s o LEFT JOIN nachalniki_uchastkov nach ON nach.id=o.otvetstvennoe_lico_ID WHERE o.id=%d", opres, m_opr_id);

        if (opres == "opres") {
            q.Format("SELECT o.name, nach.fio FROM %s o LEFT JOIN nachalniki_uchastkov nach ON nach.id=o.responsibleID WHERE o.id=%d", opres, m_opr_id);
        }
        if (opres == "remont2") {
            q.Format("SELECT o.otchet_po_defektu, nach.fio FROM %s o LEFT JOIN nachalniki_uchastkov nach ON nach.id=o.responsibleID WHERE o.id=%d", opres, m_opr_id);
        }

        vector<CString> v;

        if (readTableValues(ado, q, 2, v)) {
            name.Format("Участки %s. Ответственный: %s %d-%d отопительный сезон", v[0], v[1], m_sezon_korrozia, m_sezon_korrozia + 1);
        }

        CGraph2* graph = m_cxema->m_graph;

        CString s;
        s.Format("%d", m_opr_id);

        if (opres == "osmotr") {
            CString tn;
            tn = "objects\\osmotr\\osmotr_pipesection";
            //            tn = "objects\\opres2\\osvidet2";
            q = readQ(ado, tn, name, 0);
            q.Replace("$opres$", opres);
            q.Replace("$id$", s);

            if (q != "") {
                RunTableNiz(ado, name, q, name, true, false, virt_data_line_big);
            }
        }
        else if (opres == "remont2") {
            CString tn;
            tn = "objects\\osmotr\\remont_pipesections";
            q = readQ(ado, tn, name, 0);
            q.Replace("$opres$", opres);
            q.Replace("$id$", s);

            if (q != "") {
                RunTableNiz(ado, name, q, name, true, false, virt_data_line_big);
            }
        }
        else {
            CString tn;
            tn = "objects\\osmotr\\osmotr_pipesection";
            tn = "objects\\defect\\remonts_journal";
            tn = "objects\\opres2\\osvidet2";

            q = readQ(ado, tn, name, 0);
            q.Replace("$opres$", opres);
            q.Replace("$m_opr_id$", s);
            q.Replace("$id$", s);

            if (q != "") {
                q.Replace("$vyd$", vyd);
                RunTableNiz(ado, name, q, name, true, false, virt_data_line_big);
            }
        }
    }
}

void CGidrView::OnNaprTableOpres()
{
    viewOpresProtokol("opres", m_id_opr);
}

void CGidrView::OnNaprTableRemont2()
{
    viewOpresProtokol("remont2", m_id_remont2);
}

void CGidrView::OnNaprTableOsvidet2()
{
    viewOpresProtokol("osvidet2", m_id_osvidet2);
}

//------------------------------------

void CGidrView::OnListOsmotr2()
{
    //    ListOpres("osmotr", _TR("Контуры Осмотров"), "name", "data_osmotra", m_id_osmotr2);
    ListOpres("osmotr", _TR("Выберите контур для Осмотра"), "name", "data_osmotra", m_id_osmotr2);
}

void CGidrView::OnNaprTableOsmotr2()
{
    viewOpresProtokol("osmotr", m_id_osmotr2);
}


void CGidrView::OnSaveOsmotr2New()
{
    SaveOpresNew("osmotr", "Осмотр", "name", "Осмотр", "", m_nachalnikID, m_id_osmotr2);
    //    SaveOpresNew("osmotr", "Осмотр", "name", "Осмотр", "data_planiruemogo_osmotra", m_id_osmotr2);
}

void CGidrView::OnSaveOsmotr2()
{
    SaveOpres("osmotr", "Осмотр", m_id_osmotr2, true, false);
}

void CGidrView::OnOsmotrZapros4()
{
    AfxMessageBox("Запрос 4");
}

void CGidrView::OnOsmotrZapros5()
{
    AfxMessageBox("Запрос 5");
}

#if 0

void CGidrView::OnOsmotrZapros1()
{
    CAdoFile* ado = getAdo(getGidAdoName());
    if (ado) {
        CString vyd;

        CString name = "Поврежденные участки на выбранном контуре";
        CString name1;

        CGraph2* graph = m_cxema->m_graph;
        CString q = readQ(ado, "objects\\opres2\\table_osmotr_defect", name, 0);

        CString s;
        if (m_id_osmotr2 > 0) {
            s.Format("WHERE obj.id=%d", m_id_osmotr2);
        }
        else {
            s = "";
        }
        q.Replace("$where$", s);

        if (q != "") {

            CInputDialog inD(AfxGetMainWnd(), "", "?", q);
            inD.DoModal();


            RunTableNiz(ado, "defect", q, name, false, false, virt_data_geo_big_line);
        }
    }
}
#endif

int getNachalnik(CAdoFile* ado, CWnd* wnd, bool all);

#if 0
void CGidrView::OnOsmotrZapros2()
{
    CAdoFile* ado = getAdo(getGidAdoName());
    if (!ado) return;


    int id_nach = m_nachalnikID;

    if (id_nach < 0) {
        id_nach = getNachalnik(ado, this, false);
    }
    if (id_nach < 0) {
        return;
    }

    CString vyd;

    CString name = "Поврежденные участки на выбранном начальнике";
    CString name1;

    CGraph2* graph = m_cxema->m_graph;
    CString q = readQ(ado, "objects\\opres2\\table_osmotr_defect", name, 0);

    CString s;
    s.Format("WHERE nach.id=%d", id_nach);
    q.Replace("$where$", s);

    if (q != "") {
        CInputDialog inD(AfxGetMainWnd(), "", "?", q);
        inD.DoModal();
        RunTableNiz(ado, "defect", q, name, false, false, virt_data_geo_big_line);
    }
}
#endif


void CGidrView::OnOsmotrAkt()
{
    CString txt = readFile(argpath() + "\\html\\remont\\Акт0.htm");

    CString tmpName;

    tmpName.Format("%s\\act.html", getenv("TMP"));

    FILE* f = fopen(tmpName, "w");

    if (!f) {
        AfxMessageBox(strerror(errno), MB_OK | MB_ICONINFORMATION);
        return;
    }

    setlocale(LC_NUMERIC, "");
    fprintf(f, "%s", txt);
    fclose(f);

    setlocale(LC_NUMERIC, "eng");

    HINSTANCE hRun = ShellExecute(GetSafeHwnd(), "open", "winword", "\"" + tmpName + "\"", NULL, SW_SHOWMAXIMIZED);
}

void faktory_riska_truboprovoda_osmotr(CPropertyGrid* wnd, CString tn, CString title, int id, int obj_type_faktory_riskaID);

void CGidrView::OnLastFaktory()
{
    if (m_id_osmotr2 < 0) {
        m_id_osmotr2 = m_id_osmotr2_old;
    }
    if (m_id_osmotr2 < 0) {
        AfxMessageBox("Выберите контур осмотра");
        return;
    }

    int directionID = m_id_osmotr2;

    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) return;

    CString opres = "osmotr";

    if (directionID >= 0) {

        if (setOsmotr(ado, opres, m_cxema, directionID)) {
            MoveVyd();
            Invalidate();
            faktory_riska_truboprovoda_osmotr(0, opres, "Осмотр трубопроводов контура", directionID, 2);
            //            viewOpresDialog(opres, false);
        }
    }
}

// Все задвижки по опрессовке внизу

void CGidrView::OnOpresZdAll()
{
    if (m_id_opr < 0) {
        m_id_opr = m_id_opr_old;
    }
    if (m_id_opr < 0) {
        AfxMessageBox("Выберите контур опрессовки");
        return;
    }

    int directionID = m_id_opr;

    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) return;

    CString opres = "opres";

    if (directionID < 0) return;

//    CString fn = "objects\\opres2\\oprez_zd2";
    CString fn = "objects\\opres2\\oprez_us";

    CString s;
//    s.Format("Задвижки контура опрессовки");
    s.Format("Камеры контура опрессовки");

    map<CString, CString> map_fn;

    CString ss;
    ss.Format("%d", m_id_opr);

    map_fn["$ID$"] = ss;
    map_fn["$fileID$"] = m_cxema->m_par;

m_is_reset_visible = false;
    remontZhur(51, fn, "zapornaya_armatura", "", s, "", &map_fn, virt_data_node);
m_is_reset_visible = true;


//    remontZhur(51, fn, "zapornaya_armatura", "", s, "", &map_fn);
}


void CGidrView::OnOpresPlan()
{
    CString fn = "objects\\opres2\\plan_opres";

    CString s;
    s.Format("Контуры планов опрессовок. %s", sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
//    map_fn["$condition$"] = "(obj.remontTypeID IN (3))";
//    map_fn["$stateID$"] = "3";

    remontTable(fn, "opres", "", s, "", &map_fn, virt_data_geo);
}

void CGidrView::OnOpresProcess()
{
    CString fn = "objects\\opres2\\opres_journal";
    CString s;
//            MENUITEM "Завершенные опрессовки",      ID_OPRES_VYPOLNENO
//            MENUITEM "Отобразить все контура",      ID_OPRES_ALL2
    s.Format("Опрессовки в процессе выполнения. %s", sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
//    map_fn["$condition$"] = "(obj.remontTypeID IN (3))";
    map_fn["$stateID$"] = "2";

    remontTable(fn, "opres", "", s, "", &map_fn, virt_data_geo);
}

void CGidrView::OnOpresVypolneno()
{
    CString fn = "objects\\opres2\\opres_journal";
    CString s;
//            MENUITEM "Отобразить все контура",      ID_OPRES_ALL2

    s.Format("Завершенные опрессовки. %s", sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
//    map_fn["$condition$"] = "(obj.remontTypeID IN (3))";
    map_fn["$stateID$"] = "3";

    remontTable(fn, "opres", "", s, "", &map_fn, virt_data_geo);
}

void CGidrView::OnOpresAll2()
{
    CString fn = "objects\\opres2\\opres_journal";
    CString s;

    s.Format("Опрессовки. %s", sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
//    map_fn["$condition$"] = "(obj.remontTypeID IN (3))";
    map_fn["$stateID$"] = "1,2,3";

    remontTable(fn, "opres", "", s, "", &map_fn, virt_data_geo);
}

void PlanOpresProbation(CWnd* wnd, int m_sezon_korrozia);
void GraphOpresProbation(CWnd* wnd, int m_sezon_korrozia);

void CGidrView::OnOpresWord1()
{
    PlanOpresProbation(this, m_sezon_korrozia);
}

void CGidrView::OnOpresWord2()
{
    GraphOpresProbation(this, m_sezon_korrozia);
}

void CGidrView::OnOpresAnaliz2()
{
    CString fn = "objects\\opres2\\opres_journal";
    CString s;

    s.Format("Опрессовки. %s", sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
//    map_fn["$condition$"] = "(obj.remontTypeID IN (3))";
    map_fn["$stateID$"] = "1,2,3";

    remontTable(fn, "opres", "", s, "", &map_fn, virt_data_geo);
}

void CGidrView::OnOpresAnaliz()
{
    if (m_id_opr < 0) {
        m_id_opr = m_id_opr_old;
    }
    if (m_id_opr < 0) {
        CString str;
        str.Format("Выберите контур опрессовки");
        AfxMessageBox(str);
        return;
    }

    CString q;
    q.Format("SELECT name FROM opres WHERE id=%d", m_id_opr);

    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) return;

//    CString name = readTableValue(ado, q);

    CString fn = "objects\\osmotr\\opres_defect_list";
    CString s;

    s.Format("Анализ нарушений опрессовки контура. %s", sezon_name(m_sezon_korrozia));

    map<CString, CString> map_fn;
//    map_fn["$condition$"] = "(obj.remontTypeID IN (3))";

    CString ss;
    ss.Format("%d", m_id_opr);
    map_fn["$id$"] = ss;

//    remontZhur(61, fn, "defect", "", s, "", &map_fn);
    remontTable(fn, "defect", "", s, "", &map_fn, virt_data_geo);
}

