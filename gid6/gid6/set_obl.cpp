#include "stdafx.h"
#include "gid6.h"

#include "gidrView.h"

#include "InputDia.h"

#include "mmenu.h"
//#include "mmenu2.h"

#include "ado.h"
#include "ado.h"


//void setMS(CGraph *m_graph, int typ, const char *kod0, const char *val);


/*
  техников
  имя техника
  техники
*/

bool create_vyd_line_table(CAdoFile* ado, CGraph2* graph, CString& vyd);
bool create_vyd_node_table(CAdoFile* ado, CGraph2* graph, CString& vyd);

bool setMarkNodeLine2Value(CAdoFile* ado, CGraph2* graph, const char* tn, const char* fn, const char* val, bool all, bool isNode, const char* node_line_id)
{
    bool ret = false;
    long affected;
    CString tmpn = "sdgdfhgf234";
    CString q;

    if (all) {
        q.Format("UPDATE %s SET %s=%s", tn, fn, val);
        ret = ado->Execute(q, &affected);
    }
    else {
        bool ret;
        if (isNode) {
            ret = create_vyd_node_table(ado, graph, tmpn);
        }
        else {
            ret = create_vyd_line_table(ado, graph, tmpn);
        }

        if (ret) {
            q.Format("UPDATE %s SET %s=%s WHERE %s IN (SELECT ID FROM %s)", tn, fn, val, node_line_id, tmpn);
            ret = ado->Execute(q, &affected);
        }
    }
    return ret;
}



bool setMarkNode1Value(CAdoFile* ado, CGraph2* graph, const char* fn, const char* val, bool all)
{
    return setMarkNodeLine2Value(ado, graph, "nodes", fn, val, all, true, "ID");
}

bool setMarkNode2Value(CAdoFile* ado, CGraph2* graph, const char* tn, const char* fn, const char* val, bool all)
{
    return setMarkNodeLine2Value(ado, graph, tn, fn, val, all, true, "nodeID");
}


bool setMarkLine1Value(CAdoFile* ado, CGraph2* graph, const char* fn, const char* val, bool all)
{
    return setMarkNodeLine2Value(ado, graph, "linesobj", fn, val, all, false, "ID");
}

bool setMarkLine2Value(CAdoFile* ado, CGraph2* graph, const char* tn, const char* fn, const char* val, bool all)
{
    return setMarkNodeLine2Value(ado, graph, tn, fn, val, all, false, "lineID");
}


bool viewVirtualDlg(CWnd* wnd, const char* _bd, const char* q, long& id, const char* cap, CString& text, bool fnd = false, bool add = false, bool edit = false, bool del = false, CString tn = "", bool chk = false);


void CGidrView::OnSetOrg()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;

    CMMenu menu(this, IDD_MENU6);

    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) return;

    long id;
    CString text;
    bool ret = viewVirtualDlg(this, ado->m_schema, "SELECT id, name FROM organizations", id, _TR("Организации"), text);

    if (ret) {
        CString str;
        str.Format(_TR("Внимание!\nПри выполнении этой операции все организации будут установлены в %s\nВыполнить операцию?"), text);
        if (AfxMessageBox(str, MB_YESNO) == IDYES) {
            CString val;
            val.Format("%d", id);
            setMarkLine1Value(m_cxema->m_ado, m_cxema->m_graph, "organizationID", val, false);
            GetDocument()->SetModifiedFlag(true);
        }
    }
}



void CGidrView::OnSetOtv()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;

    CMMenu menu(this, IDD_MENU6);

    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) return;

    CString q;

    q.Format("SELECT id, name FROM responsibles");
    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long(0);
            CString s = ado->readStr(1);
            menu.Add(s, (void*)id);
            ado->MoveNext();
        }
        ado->closeTable();
    }

    if (menu.DoModal() == IDOK) {
        int id = (int)menu.getV();
        CString str;
        str.Format(_TR("Внимание!\nПри выполнении этой операции все техники будут установлены в %s\nВыполнить операцию?"), menu.m_strText);
        if (AfxMessageBox(str, MB_YESNO) == IDYES) {
            CString val;
            val.Format("%d", id);
            setMarkNode2Value(m_cxema->m_ado, m_cxema->m_graph, getNodeTableMySQL(TIP_PR), "responsibleID", val, false);
            GetDocument()->SetModifiedFlag(true);
        }
    }
}

bool get_diametr(CWnd* wnd, int standardID, double& d1, double& d2, double& d3, double& tol);

void CGidrView::OnSetDiams()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;

    CMMenu menu(this, IDD_MENU6);

    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) return;

    int standardID = 1;
    double d1, d2, d3, tol;
    //        GetNameValue(string("standardID"), standardID);

    if (get_diametr(this, standardID, d1, d2, d3, tol)) {

        CString str;
        str.Format(_TR("Внимание!\nПри выполнении этой операции все диаметры в выделенных участках будут установлены условный в %g мм, внешний в %g мм, внутренний в %g мм, толщина стенки в %g мм\nВыполнить операцию?"), d1, d2, d3, tol);
        if (AfxMessageBox(str, MB_YESNO) == IDYES) {
            CString v;
            v.Format("%g", d1);
            setMarkLine2Value(m_cxema->m_ado, m_cxema->m_graph, getLineTableMySQL(TIP_UT), "diameterCondit", v, false);
            v.Format("%g", d2);
            setMarkLine2Value(m_cxema->m_ado, m_cxema->m_graph, getLineTableMySQL(TIP_UT), "diameterExternal", v, false);
            v.Format("%g", d3);
            setMarkLine2Value(m_cxema->m_ado, m_cxema->m_graph, getLineTableMySQL(TIP_UT), "diameterInternal", v, false);
            v.Format("%g", tol);
            setMarkLine2Value(m_cxema->m_ado, m_cxema->m_graph, getLineTableMySQL(TIP_UT), "wallThickness", v, false);

            CMapGraph::const_iterator p = m_cxema->m_graph->map_node.begin();

            while (p != m_cxema->m_graph->map_node.end()) {
                CNode2* n = p->second;
                if (n->node.isPjezo) {

                    for (CLINE2* l = n->lines; l; l = next(l)) {
                        if (bline(l)->line.isPjezo) {
                            bline(l)->line.pod.diam_usl = d1;
                            bline(l)->line.pod.diam = d3;
                            bline(l)->line.pod.diam_vn = d2;
                            bline(l)->line.pod.tol = tol;

                            bline(l)->line.obr.diam_usl = d1;
                            bline(l)->line.obr.diam = d3;
                            bline(l)->line.obr.diam_vn = d2;
                            bline(l)->line.obr.tol = tol;
                        }
                    }
                }
                p++;
            }
            Invalidate();

            GetDocument()->SetModifiedFlag(true);
        }
    }
}



#if 0


void add(CMMenu2* menu, CMenuRow& row, void* pp1 = NULL, void* pp2 = NULL, void* pp3 = NULL, void* pp4 = NULL);
void del(CMMenu2* menu, CMenuRow& row, void* pp1 = NULL, void* pp2 = NULL, void* pp3 = NULL, void* pp4 = NULL);
void edit(CMMenu2* menu, CMenuRow& row, void* pp1 = NULL, void* pp2 = NULL, void* pp3 = NULL, void* pp4 = NULL);

#endif


void CGidrView::OnSetCoordNull()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;

    CString str;

    str.Format(_TR("Внимание!\nПри выполнении этой операции координаты всех выделенных узлов будут обнулены\nВыполнить операцию?"));
    if (AfxMessageBox(str, MB_YESNO) == IDYES) {

        setMarkNode1Value(m_cxema->m_ado, m_cxema->m_graph, "x", "0.", false);
        setMarkNode1Value(m_cxema->m_ado, m_cxema->m_graph, "y", "0.", false);

        setMarkLine1Value(m_cxema->m_ado, m_cxema->m_graph, "coords", "''", false);

        CMapGraph::const_iterator p = m_cxema->m_graph->map_node.begin();

        while (p != m_cxema->m_graph->map_node.end()) {
            CNode2* n = p->second;
            if (n->node.isPjezo) {
                n->node.coord.x = 0;
                n->node.coord.y = 0;

                for (CLINE2* l = n->lines; l; l = next(l)) {
                    bline(l)->line.m_NP.clear();
                }
            }
            p++;
        }

        GetDocument()->SetModifiedFlag(true);
        Invalidate();
    }
}



void CGidrView::OnSetKolChas()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;

    CString str, dt = "5000";

    CString text = dt;
    CInputDialog inD(this, _TR("Количество часов работы"), _TR("Введите Количество часов работы"), text);

    if (inD.DoModal() == IDOK) {
        int kv = atoi(inD.m_strInput);

        str.Format(_TR("Внимание!\nПри выполнении этой операции все Количество часов работы в %s\nВыполнить операцию?"), inD.m_strInput);

        if (AfxMessageBox(str, MB_YESNO) == IDYES) {
            CString v = kv > 5000 ? "1" : "0";

            //      m_cxema->setValue2(m_cxema->m_cur_file, TIP_UT, "kolwork", COleVariant((short)v, VT_BOOL), true);
            setMarkLine2Value(m_cxema->m_ado, m_cxema->m_graph, getLineTableMySQL(TIP_UT), "signNumWork", v, false);

            GetDocument()->SetModifiedFlag(true);
        }
    }

}


void CGidrView::OnSetUdobOt()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;


    CString str, dt = "1";

    CString text = dt;
    CInputDialog inD(this, _TR("Удельный объем системы отопления"), _TR("Введите удельный объем системы отопления"), text);

    if (inD.DoModal() == IDOK) {
        str.Format(_TR("Внимание!\nПри выполнении этой операции все Удельный объем системы отопления  будут установлены в %s\nВыполнить операцию?"), inD.m_strInput);
        if (AfxMessageBox(str, MB_YESNO) == IDYES) {
            setMarkNode2Value(m_cxema->m_ado, m_cxema->m_graph, getNodeTableMySQL(TIP_PR), "volWaterHS", (const char*)inD.m_strInput, false);
            setMarkNode2Value(m_cxema->m_ado, m_cxema->m_graph, getNodeTableMySQL(TIP_PO), "volWaterHS", (const char*)inD.m_strInput, false);
            GetDocument()->SetModifiedFlag(true);
        }
    }
}


void CGidrView::OnSetUdobVent()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;


    CString str, dt = "1";

    CString text = dt;
    CInputDialog inD(this, _TR("Удельный объем системы вентиляции"), _TR("Введите удельный объем системы отопления"), text);

    if (inD.DoModal() == IDOK) {
        str.Format(_TR("Внимание!\nПри выполнении этой операции все Удельный объем системы отопления в %s\nВыполнить операцию?"), inD.m_strInput);
        if (AfxMessageBox(str, MB_YESNO) == IDYES) {
            setMarkNode2Value(m_cxema->m_ado, m_cxema->m_graph, getNodeTableMySQL(TIP_PR), "volWaterVS", (const char*)inD.m_strInput, false);
            setMarkNode2Value(m_cxema->m_ado, m_cxema->m_graph, getNodeTableMySQL(TIP_PO), "volWaterVS", (const char*)inD.m_strInput, false);

            GetDocument()->SetModifiedFlag(true);
        }
    }
}


//Коэф. часовой неравномерности, откр.ГВС
//Расчетные тепловые потери в рециркуляц. контуре ГВС, откр.ГВС, %", ID_SET_OPEN_REZ
//Температура в рециркуляционном трубопроводе ГВС
//Расчетная температура горячей воды откр. ГВС



void CGidrView::OnSetOpenKoef()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;


    CString str, dt = "1.2";

    CString text = dt;
    CInputDialog inD(this, _TR("Коэф. часовой неравномерности, откр.ГВС"), _TR("Введите Коэф. часовой неравномерности, откр.ГВС"), text);

    if (inD.DoModal() == IDOK) {
        str.Format(_TR("Внимание!\nПри выполнении этой операции все Коэф. часовой неравномерности, откр.ГВС будут установлены в %s\nВыполнить операцию?"), inD.m_strInput);
        if (AfxMessageBox(str, MB_YESNO) == IDYES) {
            setMarkNode2Value(m_cxema->m_ado, m_cxema->m_graph, getNodeTableMySQL(TIP_PR), "hourIrregCoeff", (const char*)inD.m_strInput, false);
            setMarkNode2Value(m_cxema->m_ado, m_cxema->m_graph, getNodeTableMySQL(TIP_PO), "hourIrregCoeffOpen", (const char*)inD.m_strInput, false);

            GetDocument()->SetModifiedFlag(true);
        }
    }
}

void CGidrView::OnSetOpenRezT()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;


    CString str, dt = "40";

    CString text = dt;
    CInputDialog inD(this, _TR("Температура в рециркуляционном трубопроводе ГВС"), _TR("Введите Температура в рециркуляционном трубопроводе ГВС"), text);

    if (inD.DoModal() == IDOK) {
        str.Format(_TR("Внимание!\nПри выполнении этой операции все Температура в рециркуляционном трубопроводе ГВС будут установлены в %s\nВыполнить операцию?"), inD.m_strInput);
        if (AfxMessageBox(str, MB_YESNO) == IDYES) {
            setMarkNode2Value(m_cxema->m_ado, m_cxema->m_graph, getNodeTableMySQL(TIP_PR), "tempRecircPipe", (const char*)inD.m_strInput, false);
            setMarkNode2Value(m_cxema->m_ado, m_cxema->m_graph, getNodeTableMySQL(TIP_PO), "tempRecircPipeOpen", (const char*)inD.m_strInput, false);

            GetDocument()->SetModifiedFlag(true);
        }
    }
}

void CGidrView::OnSetOpenGvsT()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;


    CString str, dt = "60";

    CString text = dt;
    CInputDialog inD(this, _TR("Расчетная температура горячей воды откр. ГВС"), _TR("Введите Расчетная температура горячей воды откр. ГВС"), text);

    if (inD.DoModal() == IDOK) {
        str.Format(_TR("Внимание!\nПри выполнении этой операции все Расчетная температура горячей воды откр. ГВС будут установлены в %s\nВыполнить операцию?"), inD.m_strInput);
        if (AfxMessageBox(str, MB_YESNO) == IDYES) {
            setMarkNode2Value(m_cxema->m_ado, m_cxema->m_graph, getNodeTableMySQL(TIP_PR), "calcTempHWDO", (const char*)inD.m_strInput, false);
            setMarkNode2Value(m_cxema->m_ado, m_cxema->m_graph, getNodeTableMySQL(TIP_PO), "calcTempHWDOopen", (const char*)inD.m_strInput, false);

            GetDocument()->SetModifiedFlag(true);
        }
    }
}


void CGidrView::OnSetOpenRez()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;


    CString str, dt = "30";

    CString text = dt;
    CInputDialog inD(this, _TR("Расчетные тепловые потери в рециркуляц.кронтуре"), _TR("Введите Расчетные тепловые потери в рециркуляц.кронтуре"), text);

    if (inD.DoModal() == IDOK) {
        str.Format(_TR("Внимание!\nПри выполнении этой операции все Расчетные тепловые потери в рециркуляц.кронтуре  будут установлены в %s\nВыполнить операцию?"), inD.m_strInput);
        if (AfxMessageBox(str, MB_YESNO) == IDYES) {
            setMarkNode2Value(m_cxema->m_ado, m_cxema->m_graph, getNodeTableMySQL(TIP_PR), "circHLosOpen", (const char*)inD.m_strInput, false);
            setMarkNode2Value(m_cxema->m_ado, m_cxema->m_graph, getNodeTableMySQL(TIP_PO), "avgHLcompOpen", (const char*)inD.m_strInput, false);

            GetDocument()->SetModifiedFlag(true);
        }
    }
}




void CGidrView::OnSetSher()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;


    CString str, dt = "1";

    CString text = dt;
    CInputDialog inD(this, _TR("Эквивалентная шероховатость"), _TR("Введите Эквивалентную шероховатость"), text);

    if (inD.DoModal() == IDOK) {
        str.Format(_TR("Внимание!\nПри выполнении этой операции все Эквивалентные шероховатости в %s\nВыполнить операцию?"), inD.m_strInput);
        if (AfxMessageBox(str, MB_YESNO) == IDYES) {
            setMarkLine2Value(m_cxema->m_ado, m_cxema->m_graph, getLineTableMySQL(TIP_UT), "tubeRoughness", (const char*)inD.m_strInput, false);
            GetDocument()->SetModifiedFlag(true);
        }
    }

}

void CGidrView::OnSetTubingType()
{

    if (!isEditGid()) return;
    if (m_parent_id) return;

    CMMenu menu(this, IDD_MENU6, "Выберите тип прокладки");

    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) return;

    CString q;

    q.Format("SELECT id, name FROM tubingTypes");
    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long(0);
            CString s = ado->readStr(1);
            menu.Add(s, (void*)id);
            ado->MoveNext();
        }
        ado->closeTable();
    }

    if (menu.DoModal() == IDOK) {
        int id = (int)menu.getV();
        CString str;
        str.Format(_TR("Внимание!\nПри выполнении этой операции все выделенные участки будут иметь типа прокладки - %s\nВыполнить операцию?"), menu.m_strText);
        if (AfxMessageBox(str, MB_YESNO) == IDYES) {
            CString val;
            val.Format("%d", id);
            setMarkLine2Value(m_cxema->m_ado, m_cxema->m_graph, getLineTableMySQL(TIP_UT), "tubingTypeID", val, false);
            GetDocument()->SetModifiedFlag(true);
            CMapGraph::const_iterator p = m_cxema->m_graph->map_node.begin();

            while (p != m_cxema->m_graph->map_node.end()) {
                CNode2* n = p->second;
                if (n->node.isPjezo) {

                    for (CLINE2* l = n->lines; l; l = next(l)) {
                        if (bline(l)->line.isPjezo) {
                            bline(l)->line.tubingTypeID = id;
                        }
                    }
                }
                p++;
            }
            Invalidate();
            GetDocument()->SetModifiedFlag(true);
        }
    }

}

void CGidrView::OnSetLosesShare()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;

    CString str, dt = "0";

    CString text = dt;
    CInputDialog inD(this, _TR("Доля местных потерь"), _TR("Введите Долю местных потерь"), text);

    if (inD.DoModal() == IDOK) {
        str.Format(_TR("Внимание!\nПри выполнении этой операции все Доли местных потерь в выделенных участках будут установлены в %s\nВыполнить операцию?"), inD.m_strInput);
        if (AfxMessageBox(str, MB_YESNO) == IDYES) {
            setMarkLine2Value(m_cxema->m_ado, m_cxema->m_graph, getLineTableMySQL(TIP_UT), "localLosesShare", (const char*)inD.m_strInput, false);
            GetDocument()->SetModifiedFlag(true);
        }
    }

}



void CGidrView::OnSetPodpOnAll()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;


    CString str, dt = "1";

    str.Format(_TR("Показывать надписи по всем объектам, у которых они были скрыты?"));
    if (AfxMessageBox(str, MB_YESNO) == IDYES) {
        setMarkNode1Value(m_cxema->m_ado, m_cxema->m_graph, "displaySign", "0", true);
        setMarkLine1Value(m_cxema->m_ado, m_cxema->m_graph, "displaySign", "0", true);
        GetDocument()->SetModifiedFlag(true);
        Invalidate();
    }
}

void CGidrView::OnSetPodpOffAll()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;


    CString str, dt = "1";

    str.Format(_TR("Не показывать надписи по всем объектам, у которых они были скрыты?"));
    if (AfxMessageBox(str, MB_YESNO) == IDYES) {
        setMarkNode1Value(m_cxema->m_ado, m_cxema->m_graph, "displaySign", "1", true);
        setMarkLine1Value(m_cxema->m_ado, m_cxema->m_graph, "displaySign", "1", true);
        GetDocument()->SetModifiedFlag(true);
        Invalidate();
    }
}

void CGidrView::OnSetPodpOn()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;

    CString str, dt = "1";

    str.Format(_TR("Показать надписи в выделенном фрагменте?"));
    if (AfxMessageBox(str, MB_YESNO) == IDYES) {

        setMarkNode1Value(m_cxema->m_ado, m_cxema->m_graph, "displaySign", "0", false);
        setMarkLine1Value(m_cxema->m_ado, m_cxema->m_graph, "displaySign", "0", false);
        m_cxema->m_graph->setOnOff(true);
        GetDocument()->SetModifiedFlag(true);
        Invalidate();
    }
}

void CGidrView::OnSetPodpOff()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;

    CString str, dt = "1";

    str.Format(_TR("Скрыть надписи в выделенном фрагменте?"));
    if (AfxMessageBox(str, MB_YESNO) == IDYES) {
        setMarkNode1Value(m_cxema->m_ado, m_cxema->m_graph, "displaySign", "1", false);
        setMarkLine1Value(m_cxema->m_ado, m_cxema->m_graph, "displaySign", "1", false);
        m_cxema->m_graph->setOnOff(false);
        GetDocument()->SetModifiedFlag(true);
        Invalidate();
    }
}

#if 0

#include "MsSet.h"

//static int abort


void rewriteUT_MS(CDbFile* m_dbf, CLine* line, CMsSet* dlg)
{

    //      m_dbf->read(_TR("Вид сети"), x.pc_object);
    //      m_dbf->read(_TR("Район эксплуатации"), x.nomer_rn);
    CString s = m_dbf->read(_TR("Сеть"));
    //      m_dbf->read(_TR("Участок эксплуатации"), x.uchastok);

    CString pr1 = m_dbf->read("pr1");

    if (s != "") {
        //    AfxMessageBox("!", MB_ABORTRETRYIGNORE, MB_OK|MB_ICONINFORMATION);
    }

    m_dbf->Edit();
    m_dbf->write(_TR("Вид сети"), 1);
    m_dbf->write(_TR("Район эксплуатации"), dlg->m_id_raj);
    m_dbf->write(_TR("Сеть"), dlg->mag);
    m_dbf->write(_TR("Участок МС"), dlg->m_ms);
    m_dbf->Update();

    line->m_raj = dlg->m_id_raj;
    line->m_uchM = dlg->m_ms;
    line->m_vid = 1;
    strncpy(line->m_set, dlg->mag, N3);
    line->m_set[N3] = 0;

    if (line->nomP == line->nomO || pr1 != POD) {
        line->isPjezo = false;
        line->isPjezo2 = false;
    }
}

void CGidrView::SetMs()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;


    CString kod = "";

    CMapGraph::const_iterator p = m_cxema->m_graph->set_node.begin();

    while (p != m_cxema->m_graph->set_node.end() && kod == "") {
        for (CLINE* l = p->lines; l; l = next(l)) {
            if (bline(l)->isPjezo2 && !strcmp(where(l)->node_name.kod, other(l)->node_name.kod)) {
                kod = p->node_name.kod;
                break;
            }
        }
        p++;
    }



    CMsSet dlg(this, kod);

    if (dlg.DoModal() != IDOK) return;

    CString str;

    str.Format(_TR("Внимание!\nПри выполнении этой операции все выделенные участки!\nВыполнить операцию?"));
    if (AfxMessageBox(str, MB_YESNO) != IDYES) return;

    //  initMapRS();

    int typ = TIP_UT;
    CString tn = linef[typ].table;
    CGidFile* gid_file = m_cxema->m_cur_file;
    CDbFile* m_dbf = m_cxema->m_cur_file->m_dbf;
    CGraph* m_graph = m_cxema->m_graph;

    if (m_dbf->openTable(tn)) {

        if (!m_dbf->rs->IsEOF()) {
            m_dbf->rs->MoveFirst();

            while (!m_dbf->rs->IsEOF()) {
                char kod[40], name[256], p[2];
                char kod1[40], name1[256], p1[2];
                char kod2[40], name2[256], p2[2];

                m_dbf->readNode(kod, name, p);
                m_dbf->readLine(1, kod1, name1, p1);
                m_dbf->readLine(2, kod2, name2, p2);

                //        if (!strcmp(kod1, kod2)) {

                CNode* n1 = m_graph->find2(CNode(gid_file, CNodeName(kod1, name1, p1[0])));
                CNode* n2 = m_graph->find2(CNode(gid_file, CNodeName(kod2, name2, p2[0])));

                CLINE* line = m_graph->Link(n1, n2);

                if (line && bline(line)->isPjezo2) {
                    rewriteUT_MS(m_dbf, bline(line), &dlg);
                }
                //        }
                m_dbf->rs->MoveNext();
            }
        }
        m_dbf->closeTable();
    }
    Invalidate();

    GetDocument()->SetModifiedFlag(true);
    AfxMessageBox(_TR("Выполнено"), MB_OK | MB_ICONINFORMATION);
}


//static map<CString, RasCx> map_PC;

/*
void CGidrView::initMapRS()
{
  map_PC.clear();

  CDbFile *m_dbf = m_cxema->m_cur_file->m_dbf;


  m_dbf->openTable("SELECT * FROM [Расчетная схема]");
  if (!m_dbf->rs->IsEOF()) {
    m_dbf->rs->MoveFirst();

    while (!m_dbf->rs->IsEOF()) {
      RasCx pc;
      m_dbf->read("kod_rs", pc.kod_rs);
      m_dbf->read("name_rs", pc.name_rs);
      m_dbf->read("pc_object", pc.pc_object);
      m_dbf->read("nomer_rn", pc.nomer_rn);
      m_dbf->read("uchastok", pc.uchastok);
      m_dbf->read("kod_ist", pc.kod_ist);
      m_dbf->read("master", pc.master);
      m_dbf->read("ispoln", pc.ispoln);

      map_PC[CString(pc.kod_rs)] = pc;
//      m_cxema->m_cur_file->m_list_rs.push_back(pc);

      m_dbf->rs->MoveNext();
    }
  }
  m_dbf->closeTable();
}
*/


void rewriteUT(CDbFile* m_dbf, CLine* line, const char* kod1, map<CString, RasCx>& map_PC)
{
    map<CString, RasCx>::const_iterator it = map_PC.find(kod1);

    if (it != map_PC.end()) {
        RasCx x = it->second;

        if (x.pc_object != 1) {

            //      m_dbf->read(_TR("Вид сети"), x.pc_object);
            //      m_dbf->read(_TR("Район эксплуатации"), x.nomer_rn);
            CString s = m_dbf->read(_TR("Сеть"), x.kod_rs);
            //      m_dbf->read(_TR("Участок эксплуатации"), x.uchastok);

            m_dbf->Edit();
            m_dbf->write(_TR("Вид сети"), x.pc_object);
            m_dbf->write(_TR("Район эксплуатации"), x.nomer_rn);
            m_dbf->write(_TR("Сеть"), x.kod_rs);
            m_dbf->write(_TR("Участок РС"), x.uchastok);
            m_dbf->write("kod_owner", x.owner);
            m_dbf->Update();
            line->isPjezo = false;

            line->m_raj = x.nomer_rn;
            line->m_uchR = x.uchastok;
            line->m_vid = x.pc_object;
            line->owner = x.owner;
            strncpy(line->m_set, x.kod_rs, N3);
            line->m_set[N3] = 0;
        }
    }
}



void CGidrView::SetUchRs1(CLINE* line)
{
    CLine* l = bline(line);

    if (!l->isPjezo) {
        AfxMessageBox(_TR("Участок не выделен"), MB_OK | MB_ICONINFORMATION);
        return;
    }
    //  initMapRS();

    CNode* n1 = where(line);
    CNode* n2 = other(line);

    CMMenu menu(this);

    menu.Add(n1->node_name.kod);
    menu.Add(n2->node_name.kod);

    if (menu.DoModal() != IDOK) return;

    CDbFile* m_dbf = n1->file->m_dbf;

    int typ = TIP_UT;
    CString tn = linef[typ].table;

    if (m_dbf->openTable(tn)) {

        if (m_dbf->GotoKey(l->nomP)) {
            rewriteUT(m_dbf, l, menu.m_strText, m_cxema->map_PC);
        }
        else if (l->nomO != l->nomP && m_dbf->GotoKey(l->nomO)) {
            rewriteUT(m_dbf, l, menu.m_strText, m_cxema->map_PC);
        }
        m_dbf->closeTable();
    }
    Invalidate();
}


void CGidrView::OnSetUchRs()
{
    CGraph* m_graph = m_cxema->m_graph;

    if (!m_graph->isMark()) {
        AfxMessageBox(_TR("Нет выделенных участков трубопроводов"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    if (AfxMessageBox(_TR("Установит принадлежность выделенных трубопроводов к Участку эксплуатации?"), MB_YESNO) != IDYES) return;

    //  initMapRS();

    int typ = TIP_UT;
    CString tn = linef[typ].table;

    set<CGidFile*>::const_iterator it = m_cxema->m_file_list.begin();

    for (; it != m_cxema->m_file_list.end(); it++) {
        //  for (;) {
        //    CGidFile *gid_file = m_cxema->m_cur_file;
        //    CDbFile *m_dbf = m_cxema->m_cur_file->m_dbf;
        CGidFile* gid_file = *it;
        CDbFile* m_dbf = gid_file->m_dbf;

        if (m_graph->isMark(gid_file)) {
            if (m_dbf->openTable(tn)) {

                if (!m_dbf->rs->IsEOF()) {
                    m_dbf->rs->MoveFirst();

                    while (!m_dbf->rs->IsEOF()) {
                        char kod[40], name[256], p[2];
                        char kod1[40], name1[256], p1[2];
                        char kod2[40], name2[256], p2[2];

                        m_dbf->readNode(kod, name, p);
                        m_dbf->readLine(1, kod1, name1, p1);
                        m_dbf->readLine(2, kod2, name2, p2);

                        if (!strcmp(kod1, kod2)) {
                            CNode* n1 = m_graph->find2(CNode(gid_file, CNodeName(kod1, name1, p1[0])));
                            CNode* n2 = m_graph->find2(CNode(gid_file, CNodeName(kod2, name2, p2[0])));

                            CLINE* line = m_graph->Link(n1, n2);

                            if (line && bline(line)->isPjezo) {
                                rewriteUT(m_dbf, bline(line), kod1, m_cxema->map_PC);
                            }
                        }
                        m_dbf->rs->MoveNext();
                    }
                }
                m_dbf->closeTable();
            }
        }
    }
    Invalidate();

    GetDocument()->SetModifiedFlag(true);
    AfxMessageBox(_TR("Выполнено"), MB_OK | MB_ICONINFORMATION);
}



void CGidrView::OnUpdateSetUchMs(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_nRegim == ID_SET_UCH_MS);
}

void CGidrView::OnSetUchMs()
{
    CGraph* m_graph = m_cxema->m_graph;

    if (!m_graph->isMark()) {
        AfxMessageBox(_TR("Нет выделенных участков трубопроводов"), MB_OK | MB_ICONINFORMATION);
        return;
    }
    //  m_isPjezo2 = true;

    m_nRegim = ID_SET_UCH_MS;
    regim = R_INIT;
    //  regim = R_BEGIN_RED;

    AfxMessageBox(_TR("Выделите трубопроводы участка эксплуатации магистрали и завершите двойным щелчком левой клавишей мыши"), MB_OK | MB_ICONINFORMATION);

    //  if (AfxMessageBox(_TR("Выделите трубопроводы участка эксплуатации магистрали"), MB_YESNO) != IDYES) return;
}




#endif

int RunTableDialog(CWnd* wnd, CAdoFile* ado, const TCHAR* table, const TCHAR* query, const TCHAR* title);

#include "ado.h"
#include "graph2.h"

bool UpadateQ(CAdoFile* ado, const char* q, bool message = true)
{
    long affected;
    CWaitCursor wait;

    bool ret = ado->Execute(q, &affected);
    if (!ret) {
        AfxMessageBox(ado->getError(), MB_OK | MB_ICONINFORMATION);
    }
    else {
        if (message) AfxMessageBox("OK!", MB_OK | MB_ICONINFORMATION);
    }
    return ret;
}

bool Upadate1(CAdoFile* ado, const char* temp_name, const char* tn, const char* fn, const char* line_node_id, int kodv)
{
    CString q;
    q.Format(
        "ALTER TABLE nodes DISABLE TRIGGER all;\n"
        "UPDATE %s SET %s=%d WHERE %s IN (SELECT id FROM %s)\n"
        "ALTER TABLE nodes ENABLE TRIGGER all;\n"
        , tn, fn, kodv, line_node_id, temp_name);
    return UpadateQ(ado, q);
}

bool Upadate1_double(CAdoFile* ado, const char* temp_name, const char* tn, const char* fn, const char* line_node_id, double val)
{
    CString q;
    q.Format(
        "ALTER TABLE nodes DISABLE TRIGGER all;\n"
        "UPDATE %s SET %s=%g WHERE %s IN (SELECT id FROM %s)\n"
        "ALTER TABLE nodes ENABLE TRIGGER all;\n"
        , tn, fn, val, line_node_id, temp_name);
    return UpadateQ(ado, q);
}


bool Upadate1_s(CAdoFile* ado, const char* temp_name, const char* tn,  const char* line_node_id, const char *s)
{
    CString q;
    q.Format(
        "ALTER TABLE nodes DISABLE TRIGGER all;\n"
        "UPDATE %s SET %s WHERE %s IN (SELECT id FROM %s)\n"
        "ALTER TABLE nodes ENABLE TRIGGER all;\n"
        , tn, s, line_node_id, temp_name);
    return UpadateQ(ado, q);
}




void CGidrView::OnSetKvUt()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;

    if (!curFile()) {
        AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK | MB_ICONINFORMATION);
        return;
    }


    CAdoFile* ado = m_cxema->m_ado;
    if (ado) {
        CString tn = "varCoefficients";
        CString q;
        q.Format("SELECT id, kodkv FROM %s WHERE fileID=%d", tn, m_cxema->m_cur_file->id);

        int kodv = RunTableDialog(this, ado, tn, q, _TR("Выберите Коэффициент вариации"));

        if (kodv > 0) {
            CString str;
            CString kod = getKodName(m_cxema->map_KV, kodv);

            str.Format(_TR("Внимание!\nПри выполнении этой операции все коэффициенты вариации будут установлены в %s\nВыполнить операцию?"), kod);
            if (AfxMessageBox(str, MB_YESNO) == IDYES) {
                CString tn = "__temp__";
                bool ret = create_vyd_line_table(ado, m_cxema->m_graph, tn);
                if (ret) {
                    q.Format("UPDATE heatPipeSections SET varCoeffIDflow=%d, varCoeffIDret=%d WHERE lineID IN (SELECT id FROM %s)", kodv, kodv, tn);
                    UpadateQ(ado, q);
                }
            }
        }
    }
}


void CGidrView::OnSetKvPt()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;

    if (!curFile()) {
        AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK | MB_ICONINFORMATION);
        return;
    }


    CAdoFile* ado = m_cxema->m_ado;
    if (ado) {
        CString tn = "varCoefficients";
        CString q;
        q.Format("SELECT id, kodkv FROM %s WHERE fileID=%d", tn, m_cxema->m_cur_file->id);

        int kodv = RunTableDialog(this, ado, tn, q, _TR("Выберите Коэффициент вариации для потребителей"));

        if (kodv > 0) {
            CString str;
            CString kod = getKodName(m_cxema->map_KV, kodv);
            str.Format(_TR("Внимание!\nПри выполнении этой операции все коэффициенты вариации будут установлены в %s\nВыполнить операцию?"), kod);

            if (AfxMessageBox(str, MB_YESNO) == IDYES) {
                CString temp_name = "__temp__";
                bool ret = create_vyd_node_table(ado, m_cxema->m_graph, temp_name);
                if (ret) {
                    Upadate1(ado, temp_name, "generalizedConsumers", "varCoeffID", "nodeID", kodv);
                    Upadate1(ado, temp_name, "realConsumers", "varCoeffID", "nodeID", kodv);
                }
            }


        }
    }
}


#include "Uf.h"

void CGidrView::OnSetUf()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;

    if (!curFile()) {
        AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK | MB_ICONINFORMATION);
        return;
    }


    CAdoFile* ado = m_cxema->m_ado;
    if (ado) {                    

        CUf dlg(this, 150, 70, 95);
        if (dlg.DoModal() == IDOK && dlg.m_u > 0) {
            double uf = dlg.m_u;

            CString str;
            str.Format(_TR("Внимание!\nПри выполнении этой операции все коэффициенты смешения в выделенных потребителях будут установлены в %g\nВыполнить операцию?"), uf);

            if (AfxMessageBox(str, MB_YESNO) == IDYES) {
                CString temp_name = "__temp__";
                bool ret = create_vyd_node_table(ado, m_cxema->m_graph, temp_name);
                if (ret) {

                    CString ss;
                    ss.Format("mixFactCoeffIndep=%g,AMRdepParall=%g,AMRdepMix=%g,AMRdepConseq=%g,AMRdepPreON=%g", uf, uf, uf, uf, uf);
                    Upadate1_s(ado, temp_name, "generalizedConsumers", "nodeID", ss);
                    Upadate1_double(ado, temp_name, "realConsumers", "mixFactCoeff", "nodeID", uf);
                }
            }
        }
    }
}


// Тепловые пункты

void CGidrView::OnSetTp()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;

    /**
        if (!curFile()) {
          AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK|MB_ICONINFORMATION);
          return;
        }
    */

    CAdoFile* ado = m_cxema->m_ado;
    if (ado) {
        CString tn = "heatPoint";
        CString q;
        q.Format("SELECT id, name FROM %s", tn);

        int kodv = RunTableDialog(this, ado, tn, q, _TR("Выберите Тепловой пункт для реальных потребителей"));

        if (kodv > 0) {
            CString str;
            CString kod = getKodName(m_cxema->map_TP, kodv);

            str.Format(_TR("Внимание!\nПри выполнении этой операции все Тепловые пункты будут установлены в %s\nВыполнить операцию?"), kod);
            if (AfxMessageBox(str, MB_YESNO) == IDYES) {
                CString temp_name = "__temp__";
                bool ret = create_vyd_node_table(ado, m_cxema->m_graph, temp_name);
                if (ret) {
                    Upadate1(ado, temp_name, "realConsumers", "heatPointID", "nodeID", kodv);
                }
            }
        }
    }
}



void CGidrView::OnSetUr()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;

    if (!curFile()) {
        AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    CAdoFile* ado = m_cxema->m_ado;
    if (ado) {
        CString tn = "specExpends";
        CString q;
        q.Format("SELECT id, specExpendID FROM %s WHERE fileID=%d", tn, m_cxema->m_cur_file->id);

        int kodv = RunTableDialog(this, ado, tn, q, _TR("Выберите Код удельных расходов для реальных потребителей"));

        if (kodv > 0) {
            CString str;
            CString kod = getKodName(m_cxema->map_UR, kodv);
            str.Format(_TR("Внимание!\nПри выполнении этой операции все Коды удельных расходов будут установлены в %s\nВыполнить операцию?"), kod);
            if (AfxMessageBox(str, MB_YESNO) == IDYES) {
                CString temp_name = "__temp__";
                bool ret = create_vyd_node_table(ado, m_cxema->m_graph, temp_name);
                if (ret) {
                    Upadate1(ado, temp_name, "generalizedConsumers", "specExpendID", "nodeID", kodv);
                    Upadate1(ado, temp_name, "realConsumers", "specExpendID", "nodeID", kodv);
                }
            }
        }
    }
}

void CGidrView::OnSetTr()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;

    if (!curFile()) {
        AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    CAdoFile* ado = m_cxema->m_ado;
    if (ado) {
        CString tn = "calcTemperatures";
        CString q;
        q.Format("SELECT id, calcTemperatureID FROM %s WHERE fileID=%d", tn, m_cxema->m_cur_file->id);

        int kodv = RunTableDialog(this, ado, tn, q, _TR("Выберите Код расчетных температур"));

        if (kodv > 0) {
            CString str;
            CString kod = getKodName(m_cxema->map_TR, kodv);
            str.Format(_TR("Внимание!\nПри выполнении этой операции все Коды расчетных температур будут установлены в %s\nВыполнить операцию?"), kod);
            if (AfxMessageBox(str, MB_YESNO) == IDYES) {
                CString temp_name = "__temp__";
                bool ret = create_vyd_node_table(ado, m_cxema->m_graph, temp_name);
                if (ret) {
                    Upadate1(ado, temp_name, "generalizedConsumers", "calcTemperatureID", "nodeID", kodv);
                    Upadate1(ado, temp_name, "realConsumers", "calcTemperatureID", "nodeID", kodv);
                }
            }
        }
    }
}

void CGidrView::OnSetKti()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;

    CAdoFile* ado = m_cxema->m_ado;
    if (ado) {
        CString str, q, dt = "1";

        CString text = dt;
        CInputDialog inD(this, _TR("Коэффициент тепловых испытаний"), _TR("Введите Коэффициент тепловых испытаний"), text);

        if (inD.DoModal() == IDOK) {
            str.Format(_TR("Внимание!\nПри выполнении этой операции все Коэффициенты тепловых испытаний будут установлены в %s\nВыполнить операцию?"), inD.m_strInput);
            if (AfxMessageBox(str, MB_YESNO) == IDYES) {
                CString temp_name = "__temp__";
                bool ret = create_vyd_line_table(ado, m_cxema->m_graph, temp_name);
                if (ret) {
                    double _dt = atof(inD.m_strInput);
                    q.Format("UPDATE heatPipeSections SET heatTestsCoeff=%f WHERE lineID IN (SELECT id FROM %s)", _dt, temp_name);
                    UpadateQ(ado, q);
                }
            }
        }
    }
}



void CGidrView::setDate(CString tn, CString fn, CString name_mn)
{
    if (!isEditGid()) return;
    if (m_parent_id) return;

    CAdoFile* ado = m_cxema->m_ado;
    if (ado) {
        CTime tt = CTime::GetCurrentTime();

        CString str, dt = "01.02.2011", q;

        dt.Format("%d.%02d.%02d", tt.GetDay(), tt.GetMonth(), tt.GetYear());

        CString text = dt;
        CInputDialog inD(this, _TR("Дата"), _TR("Введите дату ") + name_mn, text);

        int d, m, y;

        if (inD.DoModal() == IDOK) {
            inD.m_strInput.Replace(".", " ");

            if (sscanf(inD.m_strInput, "%ld %ld %ld", &d, &m, &y) == 3) {

                str.Format(_TR("Внимание!\nПри выполнении этой операции все даты %s будут установлены в %s\nВыполнить операцию?"), name_mn, inD.m_strInput);
                if (AfxMessageBox(str, MB_YESNO) == IDYES) {
                    CString temp_name = "__temp__";
                    bool ret = create_vyd_line_table(ado, m_cxema->m_graph, temp_name);
                    if (ret) {
                        if (tn == "linesobj") {
                            q.Format("UPDATE %s SET %s='%d-%d-%d' WHERE ID IN (SELECT id FROM %s)", tn, fn, y, m, d, temp_name);
                        }
                        else {
                            q.Format("UPDATE %s SET %s='%d-%d-%d' WHERE lineID IN (SELECT id FROM %s)", tn, fn, y, m, d, temp_name);
                        }
                        UpadateQ(ado, q);

                        //                        if (fn == "firstPICdate") {
                        if (fn == "firstPICdateHP") {
                            //                            q.Format("UPDATE heatPipeSections SET lastTransDate='%d-%d-%d' WHERE lineID IN (SELECT id FROM %s)", y, m, d, temp_name);
                            q.Format("UPDATE heatPipeSections SET lastTransDate='%d-%d-%d' WHERE lineID IN (SELECT id FROM %s)", y, m, d, temp_name);
                            UpadateQ(ado, q, false);
                        }
                    }
                }
            }
        }
    }
}

void CGidrView::OnSetDate1()
{
    setDate("heatPipeSections", "lastTransDate", _TR("последней перекладки"));
}

void CGidrView::OnSetDate2()
{
    //    setDate("linesobj", "firstPICdate", _TR("первичного ввода в эксплуатацию"));
    setDate("heatPipeSections", "firstPICdateHP", _TR("первичного ввода в эксплуатацию"));
}

void CGidrView::OnSetDate3()
{
    setDate("heatPipeSections", "repairDatePlanTP", _TR("планируемого ремонта"));
}


void CGidrView::OnSetDate()
{
    //    setDate("linesobj", "lastTransDate", _TR("последней перекладки"));
    setDate("heatPipeSections", "lastTransDate", _TR("последней перекладки"));
}

void CGidrView::OnSetKodRs()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;

    if (!curFile()) {
        AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    CAdoFile* ado = m_cxema->m_ado;
    if (ado) {
        CString tn = "externalCodes";
        CString q;
        q.Format("SELECT id, name FROM %s WHERE fileID=%d AND removed = 0", tn, m_cxema->m_cur_file->id);

        int kodv = RunTableDialog(this, ado, tn, q, _TR("Выберите Код расчетной схемы"));

        if (kodv > 0) {
            CString str;
            str.Format(_TR("Внимание!\nПри выполнении этой операции все Коды расчетной схемы будут установлены в %s\nВыполнить операцию?"), "");
            if (AfxMessageBox(str, MB_YESNO) == IDYES) {
                CString temp_name = "__temp__";
                bool ret = create_vyd_node_table(ado, m_cxema->m_graph, temp_name);
                if (ret) {
                    if (Upadate1(ado, temp_name, "nodes", "externalCodeID", "ID", kodv)) {
                    }
                }
            }
        }
    }
}



void CGidrView::OnSetRemontType()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;

    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) return;

    long id;
    CString text;
    bool ret = viewVirtualDlg(this, ado->m_schema, "SELECT id, name FROM pipeRemontTypes", id, _TR("Признак ремонта"), text);

    if (ret) {
        CString str;
        str.Format(_TR("Внимание!\nПри выполнении этой операции все признаки ремонта будут установлены в %s\nВыполнить операцию?"), text);
        if (AfxMessageBox(str, MB_YESNO) == IDYES) {
            CString val;
            val.Format("%d", id);

            setMarkLine2Value(m_cxema->m_ado, m_cxema->m_graph, getLineTableMySQL(TIP_UT), "pipeRemontTypeID", val, false);


            GetDocument()->SetModifiedFlag(true);
        }
    }
}


#include "electro.h"


void CGidrView::OnUpdateSetKorrozia(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
    //    pCmdUI->Enable(m_NP.size() >= 3);
}


#include "InputDialogDate.h"


void CGidrView::OnSetKorrozia()
{
    CString s;

    Klassif* kls = m_geofile->m_kl_list->findKlN(REM_INDICATOR);
    if (!kls) return;

    CAdoFile* ado = kls->m_ado;

    CString str = m_NP.getWKT(3, ado->m_type_of_net);

    CString q;

    CInputDialogDate dlg(this, "Дата планировния", "");

    if (dlg.DoModal() == IDOK) {
        CString q;
        //        q.Format("SELECT * FROM %s obj WHERE obj.shape.STWithin (%s) = 1", REM_INDICATOR, str);
        CString tn = "__temp__";
        bool ret = create_vyd_line_table(ado, m_cxema->m_graph, tn);
        if (ret) {
            q.Format("UPDATE obj SET obj.sostoyanie=1, obj.data_planirovaniya='%s'\n"
                "FROM %s obj\n"
                //            "WHERE obj.shape.STWithin (%s) = 1"
                "JOIN linesobj l ON l.shape.STDistance(obj.shape) < 1\n"
                "JOIN %s tmp ON tmp.id=l.id"
                , dlg.m_date1.Format("%Y%m%d"), REM_INDICATOR, tn);

            //            CInputDialog inD(AfxGetMainWnd(), "", "?", q);
            //            inD.DoModal();

            if (!kls->m_ado->Execute(q)) {
                AfxMessageBox(kls->m_ado->getError(), MB_OK | MB_ICONINFORMATION);
            }
            else {

                void setIndikator(int sost);

                setIndikator(1);
                Invalidate();
            }

            //            q.Format("UPDATE heatPipeSections SET varCoeffIDflow=%d, varCoeffIDret=%d WHERE lineID IN (SELECT id FROM %s)", kodv, kodv, tn);
            //            UpadateQ(ado, q);
        }
    }
}



void CGidrView::setAvto(BOOL yes)
{
    if (!isEditGid()) return;
    if (m_parent_id) return;

    CString str;

    str.Format(_TR("Внимание!\nПри выполнении этой операции все выделенные реальные потребители станут %s\nВыполнить операцию?"), yes ? "автоматическими" : "неавтоматическими");
    if (AfxMessageBox(str, MB_YESNO) == IDYES) {
        setMarkNode2Value(m_cxema->m_ado, m_cxema->m_graph, getNodeTableMySQL(TIP_PR), "automDegID", yes ? "2" : "1", false);

        CMapGraph::const_iterator p = m_cxema->m_graph->map_node.begin();

        while (p != m_cxema->m_graph->map_node.end()) {
            CNode2* n = p->second;
            if (n->node.isPjezo) {
                n->node.pr_avtomat = yes ? 'А' : ' ';
            }
            p++;
        }

        GetDocument()->SetModifiedFlag(true);
        Invalidate();
    }
}

void CGidrView::OnSetAvtoOn()
{
    setAvto(TRUE);
}

void CGidrView::OnSetAvtoOff()
{
    setAvto(FALSE);
}



void CGidrView::OnSetShaiba()
{
    if (!isEditGid()) return;
    if (m_parent_id) return;

    CMMenu menu(this, IDD_MENU6, "Признак шайбы в потребителе");

    CAdoFile* ado = m_cxema->m_ado;
    if (!ado) return;

    CString q;

    q.Format("SELECT id, name FROM calcFerDiameterSIgns");
    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            long id = ado->read_long(0);
            CString s = ado->readStr(1);
            menu.Add(s, (void*)id);
            ado->MoveNext();
        }
        ado->closeTable();
    }

    if (menu.DoModal() == IDOK) {
        int id = (int)menu.getV();
        CString str;
        str.Format(_TR("Внимание!\nПри выполнении этой операции все Признаки шайбы будут установлены в %s\nВыполнить операцию?"), menu.m_strText);
        if (AfxMessageBox(str, MB_YESNO) == IDYES) {
            CString val;
            val.Format("%d", id);
            setMarkNode2Value(m_cxema->m_ado, m_cxema->m_graph, getNodeTableMySQL(TIP_PR), "calcFerDiameterSignID", val, false);
            GetDocument()->SetModifiedFlag(true);
        }
    }
}

