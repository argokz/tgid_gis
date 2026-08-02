#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
#include "gidrview.h"

#include "OpcParam.h"
#include "mmenu.h"

#include "InputDia.h"

#include "ado.h"

#include "electro.h"


//#include "Ant/modedit.h"

//#include "DialogNew2.h"
#include "win.h"

#include "ps_alma.h"

#include "DopPodklNagr.h"


//static CFPoint m_pt;
CString getPSTable(int n);

OPCServer* opc_server = 0;

bool ado_execute(CAdoFile* ado, const char* q, long* affected);
CString getRusName(const char* tn);


CString ubr(const char* s, const char* p);

BOOL isAkt(const char* tn);

const MAG* getMag1(CNode* node);
const MAG* getMag2(CNode* node);

void AddMagUzel(CGidrView* pView, CNode* node, int id, const MAG* mag1, const MAG* mag2, const RS* rs, int force);


void MagRightReset()
{
}

void CGidrView::MagRightReset()
{
    int oldT = m_MagistralTyp;

    ::MagRightReset();
    m_MagistralTyp = oldT;
}

//CNode *CGidrView::findZN(CNode *node)
//{
//  node = node->m_graph->findZN(node);
//  if (!node) return NULL;
//  return m_cxema->m_graph->find(node->node.coord, masx*D5/mas_otn);
//}

void CGidrView::OnRButtonDown4(UINT nFlags, CPoint point)
{
}

void addOpenClose(CMenu &menu, Klassif * kls, int ng)
{
    if (/*tn == "zapornaya_armatura" ||*/ kls->label_stateID) {
        CGeoObject* geo = kls->getGeoObjectById(ng);
        if (geo) {
            CCoordList cl = geo->cl;
            CString ss;

            if (cl.stateID == 1) {
                ss.Format("Закрыть %s", kls->RusName());
            }
            else {
                ss.Format("Открыть %s", kls->RusName());
            }
            menu.AppendMenu(MF_STRING, ID_POPUP_OPEN_CLOSE_STATE_ID, ss);
        }
    }
}


void CGidrView::addMenuGeo(CMenu &menu, Klassif * kls, int ng)
{
    CString qq;
//    Klassif* kls = m_kls;
    qq.Format(_TR("Свойства объекта %s"), kls->RusName());

    menu.AppendMenu(MF_STRING, ID_POPUP_GEO_INFO, qq);

    if (kls && !strcmp(kls->RusName(), _TR("Здания потребителей"))) {
        //      menu.AppendMenu(MF_STRING, ID_POPUP_INFO_NAGR, _TR("Нагрузки"));
        //      menu.AppendMenu(MF_STRING, ID_POPUP_INFO_TU, _TR("Технические условия"));
    }

    if (kls && !strcmp(kls->RusName(), _TR("Здания Карта"))) {
        menu.AppendMenu(MF_STRING, ID_POPUP_VV, _TR("Паспорт теплового пункта"));
    }

    if (kls) {
        CAddressLeft* a = (CAddressLeft*)getDockablePane(IDD_ADDRESS3);
        if (a) {
            CString tn,  str,  raj,  nomer;
            if (a->Addr(tn,  str,  raj,  nomer)) {
                raj = "id_adr_mas";
                str = "street_nam";
                nomer = "number_1";
                if (tn == kls->nazv) {
                    CAdoFile *ado = kls->m_ado;
                    CString q;

                    q.Format("SELECT %s, %s, %s, mkr2, street2, house2 FROM %s WHERE id=%d", raj, str, nomer, kls->nazv, ng);
                    if (ado->openTable0(q)) {
                        if ( !ado->isEOF() ) {

                          m_mkr = ado->readStr(3);
                          m_street = ado->readStr(4);
                          m_dom = ado->readStr(5);

                          if (m_street == "" && m_mkr == "") {
                              m_mkr = ado->readStr(0);
                              m_street = ado->readStr(1);
                              m_dom = ado->readStr(2);
                          }
                        }
                        ado->closeTable();
                    }
                    menu.AppendMenu(MF_STRING, ID_ZDANIE_NAGRUZKI, _TR("Нагрузки договорные АЛСЕКО"));
                }
            }
        }
    }

    if (kls && isEditGeo(kls->nazv, false)) {
        if (isEditGeo(kls->nazv)) {
            qq.Format(_TR("Удалить %s"), kls->RusName());

            menu.AppendMenu(MF_STRING, ID_POPUP_GEO_DEL, qq);

            if (!strcmp(kls->nazv, STR_REMONT_KAPITAL)) {
                menu.AppendMenu(MF_STRING, ID_POPUP_ADD_KAPITAL, _TR("Добавить участок к капитальному ремонту"));
            }

            CString tn = kls->nazv;

            if (isAkt(tn)) {
                menu.AppendMenu(MF_STRING, ID_POPUP_COPY_AKT, _TR("Копировать Акт"));
            }

            if (kls->loc == LOC6) {
                menu.AppendMenu(MF_STRING, ID_POPUP_GEO_ISM, _TR("Передвинуть объект"));
                menu.AppendMenu(MF_STRING, ID_POPUP_GEO_NAPR, _TR("Развернуть объект"));
            }
            else {
                menu.AppendMenu(MF_STRING, ID_POPUP_GEO_ISM, _TR("Передвинуть объект"));

                if (kls) {
                    if (!kls->codlabel.IsEmpty()) {
                        menu.AppendMenu(MF_STRING | (kls->label ? MF_CHECKED : 0), ID_POPUP_GEO_NADPIS, _TR("Надпись"));
                    }
                    if (kls->loc == LOC3) {
                        Klassif* klsN = m_geofile->m_kl_list->findKlN(transl("Здания потребителей"));
                        if (klsN) {
//                            menu.AppendMenu(MF_STRING, ID_POPUP_GEO_NAGR, _TR("Создать Договорные нагрузки"));
                        }
                        Klassif* kls2 = m_geofile->m_kl_list->findKlN(transl("Здания ТУ"));
                        if (kls2) {
                            menu.AppendMenu(MF_STRING, ID_POPUP_GEO_TU2, _TR("Создать Техническое условие"));
                        }

                    
                        kls2 = m_geofile->m_kl_list->findKlN(ZDANIYA_2);
                        if (kls2 && kls->nazv == "zdaniya_tu") {
                            menu.AppendMenu(MF_STRING, ID_POPUP_GEO_ZD2, _TR("Создать Здание с нагрузками АЛСЕКО"));

                            menu.AppendMenu(MF_STRING, ID_POPUP_GEO_ALSECO1, _TR("Добавить Объект (МЖД) АЛСЕКО"));
                            menu.AppendMenu(MF_STRING, ID_POPUP_GEO_ALSECO2, _TR("Добавить Объект (Прочие) АЛСЕКО"));

                        }
                    }

                    if (kls->nazv == ZDANIYA_2) {
                        menu.AppendMenu(MF_STRING, ID_POPUP_GEO_MARK, _TR("Выделить объект"));
                    }
                }
            }

            addOpenClose(menu, kls, ng);
        }
    }
    if (kls && kls->nazv == "defect") {
        menu.AppendMenu(MF_SEPARATOR);
        menu.AppendMenu(MF_STRING, ID_POPUP_DEFECT_1, _TR("Карта нарушения"));
        menu.AppendMenu(MF_STRING, ID_POPUP_DEFECT_2, _TR("Акт расследования"));
    }
}


void CGidrView::addMenuNode(CMenu &menu, CNode2 *node)
{
    CString str;

    //      CLINE2 *l;

    //      int i = 0;
    //      for ( l = node->node.lines; l && i < 20; l = next(l), i++) {
    //        CNode *n2 = other(l);
    //        str.Format(_TR("Перейти к %s-%s"), n2->node_name.kod, n2->node_name.name);
    //        menu.AppendMenu(MF_STRING, ID_NEXT_UZEL1+i, str);
    //      }

    if (!isPassport()) {
        if (node->node.nZN != -1) {
            menu.AppendMenu(MF_STRING, ID_POPUP_ZN, _TR("Узел с заданным напором"));
            menu.AppendMenu(MF_STRING, ID_POPUP_DEL_ZN, _TR("Удалить узел с заданным напором"));
            menu.AppendMenu(MF_SEPARATOR);
        }
    }

    //    menu.AppendMenu(MF_STRING|MF_CHECKED, ID_POPUP_INFO, _TR("Свойства объекта"));

    if (!isPassport()) {

#if 0
        void findNode(CGraph2 * graph, const CFPoint & pt, list<CNode2*> &list_node, double m5);

        list<CNode2*> list_node;
        findNode(m_cxema->m_graph, m_pt, list_node, D5 * masx);


        for (list<CNode2*>::const_iterator it = list_node.begin(); it != list_node.end(); it++) {
            const CNode2* node2 = *it;
            str.Format(_TR("Свойства объекта %s %s"), node2->getTable(), node2->getName());
            menu.AppendMenu(MF_STRING, ID_POPUP_INFO_NODE, str);
        }
#endif
        str.Format(_TR("Свойства объекта %s %s"), node->getTable(), node->getName());
        menu.AppendMenu(MF_STRING, ID_POPUP_INFO_NODE, str);

    }
    else {

        CString ss = nodef[node->node.typ].table1;
        if (node->typPS() > 0 && node->typPS() != PS_NEUST) {
            ss += " " + getPSTable(node->typPS());
        }
        str.Format(_TR("Свойства объекта %s"), ss);
        menu.AppendMenu(MF_STRING, ID_POPUP_INFO_NODE, str);
    }


    if (node->node.typ == TIP_IS) {
        menu.AppendMenu(MF_STRING, ID_POPUP_TG, _TR("Температурный график"));
        //      menu.AppendMenu(MF_STRING, ID_POPUP_TG2, _TR("Температурный график 2"));
        //      menu.AppendMenu(MF_STRING, ID_POPUP_INFO_NORM_TEMP, _TR("Нормативные температуры"));
    }

    if (node->node.typ == TIP_PR) {
        menu.AppendMenu(MF_STRING, ID_POPUP_PR, _TR("Схема"));
    }


    menu.AppendMenu(MF_STRING, ID_POPUP_NAGR, _TR("Нагрузки"));


   Klassif *kls3 = m_geofile->m_kl_list->findKlN(ZDANIYA_2);

    if (kls3 && (node->node.typ == TIP_PR || node->node.typ == TIP_PO)) {
        menu.AppendMenu(MF_STRING, ID_POPUP_DOMA_ALSECO, _TR("Подключенные дома АЛСЕКО"));
    }



    if (node->node.typ == TIP_PR || node->node.typ == TIP_PO) {
        menu.AppendMenu(MF_STRING, ID_POPUP_NAGR2, _TR("Камера присоединения"));
    }

    if (opc_server) {
        menu.AppendMenu(MF_STRING, ID_POPUP_OPC_PARAM, _TR("Параметры OPC"));
        menu.AppendMenu(MF_STRING, ID_POPUP_OPC_WRITE, _TR("Дистанционное управление"));
    }
    menu.AppendMenu(MF_STRING, ID_POPUP_OUT, _TR("Результат расчета"));
    if (node->node.typ == TIP_PR) {
        menu.AppendMenu(MF_SEPARATOR);
        menu.AppendMenu(MF_STRING, ID_POPUP_OUT_DR, _TR("Результат расчета дроссельных органов"));
    }
    if (node->node.typ == TIP_PR || node->node.typ == TIP_PO) {
        //      menu.AppendMenu(MF_STRING, ID_DOP_PODKL_NAGR, _TR("Расчет допустимой подключаемой нагрузки"));
    }
    menu.AppendMenu(MF_SEPARATOR);
    menu.AppendMenu(MF_STRING, ID_POPUP_VYD, _TR("Фрагмент сети"));

    //        menu.AppendMenu(MF_SEPARATOR);
    //        menu.AppendMenu(MF_STRING, ID_POPUP_PAS, _TR("Паспорт"));


    if (isEditGid() || isEditPS()) {
        menu.AppendMenu(MF_SEPARATOR);
        if (!isPassport()) {
            menu.AppendMenu(MF_STRING, ID_POPUP_RENAME, _TR("Изменить тип объекта"));
        }

        if (node->node.nUP != -1) {
            //        menu.AppendMenu(MF_STRING, ID_POPUP_DEL, _TR("Удалить узел подпитки"));
        }
        else if (node->node.nZN != -1) {
            //        menu.AppendMenu(MF_STRING, ID_POPUP_DEL, _TR("Удалить узел с заданным напором"));
        }
        else {
            if (!isPassport() && node->node.typ != TIP_US2 && !isNoDelete()) {
                str.Format(_TR("Удалить %s"), node->getTable());
                menu.AppendMenu(MF_STRING, ID_POPUP_DEL_NODE, str);
            }
        }

        if (!isPassport()) {
            menu.AppendMenu(MF_STRING, ID_POPUP_MOVE, _TR("Перенести узел"));
            if (node->node.isPodp)
                menu.AppendMenu(MF_STRING, ID_POPUP_PODP, _TR("Не показывать надпись по объекту"));
            else
                menu.AppendMenu(MF_STRING, ID_POPUP_PODP, _TR("Показывать надпись по объекту"));

            menu.AppendMenu(MF_SEPARATOR);

            if (node->node.nZN == -1) {
                menu.AppendMenu(MF_STRING, ID_POPUP_ZN, _TR("Узел с заданным напором"));
                menu.AppendMenu(MF_SEPARATOR);
            }

            menu.AppendMenu(MF_STRING, ID_POPUP_UP, _TR("Узел подпитки"));
            if (node->node.nUP != -1 && !isNoDelete()) {
                menu.AppendMenu(MF_STRING, ID_POPUP_DEL_UP, _TR("Удалить узел подпитки"));
            }

            if (0 && m_bIsPribor) {
                menu.AppendMenu(MF_SEPARATOR);

                if (node->node.nPRIBOR != -1) {
                    menu.AppendMenu(MF_STRING, ID_POPUP_PRIBOR, _TR("Редактировать Прибор учета"));
                }
                else {
                    menu.AppendMenu(MF_STRING, ID_POPUP_PRIBOR, _TR("Создать Прибор учета"));
                }
                if (node->node.nPRIBOR != -1 && !isNoDelete()) {
                    menu.AppendMenu(MF_STRING, ID_POPUP_PRIBOR_DEL, _TR("Удалить Прибор учета"));
                }
            }
            if (0) {

                menu.AppendMenu(MF_SEPARATOR);

                menu.AppendMenu(MF_STRING, ID_POPUP_ORP, _TR("Объект с измеряемыми параметрами"));
                if (node->node.nORP != -1) {
                    menu.AppendMenu(MF_STRING, ID_POPUP_DEL, _TR("Удалить Объект с измеряемыми параметрами"));
                    menu.AppendMenu(MF_STRING, ID_POPUP_WINCC_INFO, _TR("Измеряемые параметры"));
                }
            }

            menu.AppendMenu(MF_SEPARATOR);

            menu.AppendMenu(MF_STRING, ID_POPUP_KAM, _TR("Внутренняя схема"));
            if (node->node.isP && !isNoDelete()) {
                menu.AppendMenu(MF_STRING, ID_POPUP_CLEAR, _TR("Удалить внутреннюю схему"));
            }
//            if (node->node.isPodp) {

            if (node->node.typ == TIP_PR || node->node.typ == TIP_PO) {

                if (node->node.isOtkl) {
                    str.Format(_TR("Открыть потребитель"));
                }
                else {
                    str.Format(_TR("Закрыть потребитель"));
                }
                menu.AppendMenu(MF_SEPARATOR);
                menu.AppendMenu(MF_STRING, ID_POPUP_OTKR_PT, str);
            }
        }
    }

    if (1) {
        Klassif *kls = m_geofile->m_kl_list->findKlN("zapornaya_armatura");
        if (kls && isEditGeo(kls->nazv, false)) {
            
            CString q;
            q.Format("SELECT id FROM zapornaya_armatura WHERE nodeID_pts=%d", node->id);

            long id = readTableValueInt(kls->m_ado, q);
            if (id) {
                menu.AppendMenu(MF_SEPARATOR);
                menu.AppendMenu(MF_STRING, ID_NODE_ZD, _TR("Задвижки"));
            }
        }
    }
}

void CGidrView::addMenuLine(CMenu& menu, CLINE2* line)
{
    CString str;

    str.Format(_TR("Свойства объекта %s %s"), bline(line)->getTable(), bline(line)->getName());
    menu.AppendMenu(MF_STRING, ID_POPUP_INFO_LINE, str);


    if (bline(line)->line.typ == TIP_UT) {
        str.Format(_TR("Свойства объекта Участок ПТС"));
        menu.AppendMenu(MF_STRING, ID_POPUP_INFO_LINE_BIG, str);
    }

    if (opc_server) {
        menu.AppendMenu(MF_STRING, ID_POPUP_OPC_PARAM, _TR("Параметры OPC"));
        menu.AppendMenu(MF_STRING, ID_POPUP_OPC_WRITE, _TR("Дистанционное управление"));
    }
    menu.AppendMenu(MF_STRING, ID_POPUP_OUT, _TR("Результат расчета"));
    menu.AppendMenu(MF_SEPARATOR);
    menu.AppendMenu(MF_STRING, ID_POPUP_VYD, _TR("Фрагмент сети"));
    if (!isPassport() && isEditGid()) {
        menu.AppendMenu(MF_SEPARATOR);
        menu.AppendMenu(MF_STRING, ID_POPUP_RENAME, _TR("Изменить тип объекта"));
        if (!isNoDelete()) {
            str.Format(_TR("Удалить %s"), bline(line)->getTable());
        }
        menu.AppendMenu(MF_STRING, ID_POPUP_DEL_LINE, str);
        menu.AppendMenu(MF_STRING, ID_POPUP_MOVE, _TR("Передвинуть линию"));



        if (bline(line)->line.nomP != bline(line)->line.nomO && bline(line)->line.nomP != -1 && bline(line)->line.nomO != -1) {
            menu.AppendMenu(MF_STRING, ID_POPUP_UNITE, _TR("Объединить подающий и обратный участки"));
        }

        if (bline(line)->line.nomP == -1 || bline(line)->line.nomO == -1) {
            CLINE2* line2 = where(line)->node.m_graph->find_line_obr(line);

            if (line2 && (bline(line2)->line.nomP == -1 || bline(line2)->line.nomO == -1) && (bline(line2)->line.nomP != bline(line)->line.nomP) && (bline(line2)->line.nomO != bline(line)->line.nomO)) {
                menu.AppendMenu(MF_STRING, ID_POPUP_UNITE2, _TR("Объединить подающий и обратный участки"));
            }
        }


        menu.AppendMenu(MF_STRING, ID_POPUP_NEW_UZEL, _TR("Создать узел"));
        if (m_cxema->isGeobaza() && IsMain()) {
            menu.AppendMenu(MF_STRING, ID_POPUP_LENGTH, _TR("Установить длину"));
        }

        if (bline(line)->line.isPodp)
            menu.AppendMenu(MF_STRING, ID_POPUP_PODP, _TR("Не показывать надпись по объекту"));
        else
            menu.AppendMenu(MF_STRING, ID_POPUP_PODP, _TR("Показывать надпись по объекту"));

        menu.AppendMenu(MF_SEPARATOR);
    }

    if (bline(line)->line.mag > 0) menu.AppendMenu(MF_STRING, ID_VYD_MAG, _TR("Выделить магистраль"));
    if (bline(line)->line.ms > 0) menu.AppendMenu(MF_STRING, ID_VYD_MS, _TR("Выделить Участок МС"));
    if (bline(line)->line.rs > 0) menu.AppendMenu(MF_STRING, ID_VYD_RS, _TR("Выделить Участок РС"));

    if (bline(line)->line.m_opressovka > 0) menu.AppendMenu(MF_STRING, ID_VYD_OPR_UCH, _TR("Выделить Участок Опрессовки"));


    Klassif* klsU = m_geofile->m_kl_list->findKlN(transl("Здания ТУ"));
    if (klsU) {
        //          menu.AppendMenu(MF_SEPARATOR);
        //          menu.AppendMenu(MF_STRING, ID_POPUP_GEO_TU_LINE, _TR("Создать Техническое условие"));
    }

    //      menu.AppendMenu(MF_STRING, ID_POPUP_OTKL_POTR, _TR("Отключенные потребители"));


    Klassif* klsK = m_geofile->m_kl_list->findKlN("kanal");
    if (klsK) {
//        menu.AppendMenu(MF_SEPARATOR);
//        menu.AppendMenu(MF_STRING, ID_POPUP_CANAL, _TR("Создать канал по участку"));
    }



    if (!isPassport() && isEditGid()) {
        if (bline(line)->line.nomP != -1 && bline(line)->line.pod.isOtkl || bline(line)->line.nomO != -1 && bline(line)->line.obr.isOtkl) {
            str.Format(_TR("Открыть %s"), bline(line)->getTable());
        }
        else {
            str.Format(_TR("Закрыть %s"), bline(line)->getTable());
        }
        menu.AppendMenu(MF_SEPARATOR);
        menu.AppendMenu(MF_STRING, ID_POPUP_OTKR, str);
    }
}


void CGidrView::OnRButtonDown(UINT nFlags, CPoint point)
{
    CString str, qq;
    CNode2* node = NULL;
    CLINE2* line = NULL;
    char fn[256];
    int /*ng0 = 0,*/ ng = 0;
    int loc = 0;

    int m_is_ispr = 1;

    m_menu_node = NULL;
    m_menu_line = NULL;

    CMenu menu;

    VERIFY(menu.CreatePopupMenu());

    CFPoint m_pt = ScreenToCoordOb(point);

    m_menu_pt = m_pt;

    node = m_cxema->m_graph->find(m_parent_id, m_pt, geom.masx * D5 / mas_otn);
    line = m_cxema->m_graph->findLine(m_parent_id, m_pt, geom.masx * D5 / mas_otn);

    if (IsRezhim(node)) {
        return;
    }
    if (IsRezhim(line)) {
        return;
    }

    m_menu_ngeo = -1;
    m_menu_ngeo0 = -1;
    m_menu_tn = "";
    m_menu_tn0 = "";

    if (m_isGeobaza && m_parent_id == 0) {
        m_ng = ng = m_geofile->findXY(m_pt, D5 * geom.masx, 2, fn);

        if (ng > 0) m_geo_name = fn;

        m_kls = m_geofile->m_kl_list->findKlN(fn);
        if (m_kls && isPoint(m_kls->loc)) {
            line = NULL;
        }
    }

    m_menu_node = node;
    m_menu_line = line;

    CString sss;
    sss.Format("%d", m_idMenu);

    //  menu.AppendMenu(MF_STRING, ID_POPUP_ZN, sss);

    m_text_del = nullptr;

    if (isEditGid()) {
        set<CText>::iterator p = m_cxema->set_text.begin();

        while (p != m_cxema->set_text.end()) {
            double h = hypot(m_pt.x - p->coord.x, m_pt.y - p->coord.y);
            if (h < geom.masx * D5 && p->internalNodeID == m_parent_id) {
                CString str;
                str.Format(_TR("Удалить текст \"%s\"?"), p->text);
                menu.AppendMenu(MF_STRING, ID_POPUP_DEL_TEXT, str);

                str.Format(_TR("Изменить текст \"%s\"?"), p->text);
                //              menu.AppendMenu(MF_STRING, ID_POPUP_EDIT_TEXT, str);

                str.Format(_TR("Изменить шрифт для текста \"%s\"?"), p->text);
                //              menu.AppendMenu(MF_STRING, ID_POPUP_FONT_TEXT, str);


                m_text_del = (CText*)&*p;

                break;
            }
            p++;
        }
    }

    if (node) {
        addMenuNode(menu, node);
        if (ng) {
            if (m_kls && isEditGeo(m_kls->nazv, false)) {
                menu.AppendMenu(MF_SEPARATOR);
                addMenuGeo(menu, m_kls, ng);
            }
        }
    }
    else if (line) {
        addMenuLine(menu, line);
        if (ng) {
            Klassif* kls = m_kls;

            if (kls && isEditGeo(kls->nazv, false)) {
                str.Format(_TR("Свойства объекта %s"), kls->RusName());
                menu.AppendMenu(MF_STRING, ID_POPUP_GEO_INFO, str);
            }
        }
    }
    //  else if (ng && m_geofile->m_kls) {
    else if (ng && m_kls) {
        addMenuGeo(menu, m_kls, ng);
    }
    else {
        if (m_clExpl.size() > 0) {
            menu.AppendMenu(MF_STRING, ID_POPUP_GEO_SHOW, _TR("Снять выделение"));
            menu.AppendMenu(MF_SEPARATOR);
        }

        menu.AppendMenu(MF_STRING, ID_ZOOM, _TR("Масштабировать"));
        menu.AppendMenu(MF_SEPARATOR);
        if (isEditGid() || isEditPS()) {
            menu.AppendMenu(MF_STRING, ID_NOVISUAL, _TR("Характеристики системы"));
            menu.AppendMenu(MF_SEPARATOR);

            if (!isNoDelete()) {
                menu.AppendMenu(MF_STRING, ID_NODE, _TR("Установить узел"));
                menu.AppendMenu(MF_STRING, ID_LINE, _TR("Линейный объект"));
                menu.AppendMenu(MF_STRING, ID_NODE_MOVE1, _TR("Разъединенить линии"));
                menu.AppendMenu(MF_STRING, ID_RENAME, _TR("Изменить тип объекта"));
                menu.AppendMenu(MF_STRING, ID_DEL, _TR("Удалить объект"));
            }
            menu.AppendMenu(MF_STRING, ID_NODE_MOVE, _TR("Перенести объект"));
            menu.AppendMenu(MF_SEPARATOR);

            menu.AppendMenu(MF_STRING, ID_POPUP_PODP_ON, _TR("Показать скрытые надписи"));
            menu.AppendMenu(MF_STRING, ID_POPUP_PODP_OFF, _TR("Не показывать скрытые надписи"));
            menu.AppendMenu(MF_SEPARATOR);
        }

        menu.AppendMenu(MF_STRING, ID_EDIT_FIND_NODE, _TR("Найти узел"));
        menu.AppendMenu(MF_STRING, ID_PROTOCOL, _TR("Протокол расчета"));

        if (isEditGid()) {
            menu.AppendMenu(MF_SEPARATOR);
            menu.AppendMenu(MF_STRING, ID_ZN, _TR("Узел с заданным напором"));
            if (m_parent_id) {
                menu.AppendMenu(MF_STRING, ID_KAM, _TR("Внешняя схема"));
            }
            else {
                menu.AppendMenu(MF_STRING, ID_KAM, _TR("Внутренняя схема"));
            }
            menu.AppendMenu(MF_SEPARATOR);
            menu.AppendMenu(MF_STRING, ID_NODE_NEOTR, _TR("Установка конвертированных узлов"));

            if (regim == R_BEGIN_LEN2 && m_NP.size() > 1) {
                menu.AppendMenu(MF_SEPARATOR);
                menu.AppendMenu(MF_STRING, ID_POPUP_VNUTR, _TR("Перенести во внутреннюю схему"));
            }
        }
    }

    if (node || line) {
        if (line && !node) node = where(line);

        if (node && (!curFile() || node->node.fileID != m_cxema->m_cur_file->id)) {
            CString str;
            menu.AppendMenu(MF_SEPARATOR);
            str.Format(_TR("Сделать активной %s"), node->getFileName());
            menu.AppendMenu(MF_STRING, ID_POPUP_ACTIV, str);
        }
    }


    ClientToScreen(&point);

    menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);

    CBgi::OnRButtonDown(nFlags, point);
}

void CGidrView::OnPopupPribor()
{
    if (!isEditGid()) return;
    if (isPot()) return;

    CNode2* node = m_menu_node;

    if (node) {
        if (node->node.nPRIBOR == -1 && AfxMessageBox(_TR("Создать Прибор учета?"), MB_YESNO) == IDYES && m_cxema->setPRIBOR(node)) {
            GetDocument()->SetModifiedFlag(true);
            InvalidateG();
        }
        if (node->node.nPRIBOR != -1) {
            //      edit(ST_PRIBOR, node->nPRIBOR);
            AfxMessageBox(_TR("Временно удалено 701"), MB_OK | MB_ICONINFORMATION);
        }
    }
}

#include "Ultimate Grid/TableDialog.h"

void CGidrView::OnPopupMeropr()
{
    CString tn1 = m_geo_name;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn1);
    if (!kls) return;

    int id = m_ng;

    CAdoFile* ado = kls->m_ado;
    CString tn = "defekt2_meropriyatiya";
    CString q;
    int n_ret = 1;
    q.Format("SELECT d.id, d.defektID, d.povrezhdeniya, p.znachenie AS povr, d.meropriyatie, m.znachenie AS znach, plan FROM defekt2_meropriyatiya d"
        " LEFT JOIN opressovka_povrezhdeniya p ON p.id=d.povrezhdeniya "
        " LEFT JOIN opressovka_meropriyatiya_po_ustraneniyu_povrezhdeniy m ON m.id=d.meropriyatie "
        " WHERE defektID=%d"

        , id);

    CTableDialog dlg(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), _TR("Мероприятия"), q, n_ret, virt_data_edit_add_del);
    dlg.SetKey("defektID", id);
    dlg.setHead("", tn);

    dlg.DoModal();

}




void CGidrView::OnPopupPriborDel()
{
    AfxMessageBox(_TR("Временно удалено 702"), MB_OK | MB_ICONINFORMATION);
}

void CGidrView::OnPopupORPDel()
{
    AfxMessageBox(_TR("Временно удалено 703"), MB_OK | MB_ICONINFORMATION);
}


void CGidrView::OnPopupOpcParam()
{
    AfxMessageBox(_TR("Временно удалено 704"), MB_OK | MB_ICONINFORMATION);
}

void CGidrView::OnPopupOpcWrite()
{
    /*
      if (!isEditGid()) return;

      CNode *node = m_graph->find(m_pt, masx*D5/mas_otn);
      if (node) {
        try {
          OPCItem item(opc_gr, node->node.strOpc1);
          COleVariant var(1.567);
          item.Write(var);
          return;
        }
        catch(...) {
          return;
        }

        AfxMessageBox(node->node.strOpc1, MB_OK|MB_ICONINFORMATION);

      }
    */
}

// Перенос во внутрь

void CGidrView::OnPopupVnutr()
{
    AfxMessageBox(_TR("Временно удалено 705"), MB_OK | MB_ICONINFORMATION);
}



void CGidrView::Otkr(CLINE2* line)
{
    CLine2* l = bline(line);

    int state = 1;

    if (!l->line.pod.isOtkl && l->line.nomP != -1 || !l->line.obr.isOtkl && l->line.nomO != -1) state = 2;

    setState(l, state);
}


void CGidrView::OnPopupOtkr()
{
    if (!isEditGid()) return;

    CLINE2* line = m_menu_line;

    if (line) Otkr(line);
}


// Открыть или закрыть потребитель

void CGidrView::Otkr(CNode2* node)
{
    CString q;
    long affected;

    if (node && (node->node.typ == TIP_PR || node->node.typ == TIP_PO)) {
        q.Format("update %s set consumerStateID=%d where id=%d", node->getTableMySQL(), node->node.isOtkl ? 1 : 2, node->node.id2);

        if (ado_execute(m_cxema->m_ado, q, &affected)) {
            node->node.isOtkl = !node->node.isOtkl;
            GetDocument()->SetModifiedFlag(true);
            InvalidateG();
        }
    }
}

void CGidrView::OnPopupOtkrPr()
{
    if (!isEditGid()) return;

    CNode2* node = m_menu_node;
    if (node) Otkr(node);
}

void CGidrView::OnPopupUnite()
{
    if (!isEditGid()) return;

    if (AfxMessageBox(_TR("Объединить подающий и обратный участки? Проверьте, их параметры могут быть разными"), MB_YESNO) != IDYES) return;

    CLINE2* line = m_menu_line;

    m_CurNode = NULL;
    m_CurLine = NULL;

    m_cxema->uniteLine(line);
    bline(line)->line.isPjezo = false;

    Invalidate();
    AfxMessageBox(_TR("Выполнено"), MB_OK | MB_ICONINFORMATION);
}

void CGidrView::OnPopupUnite2()
{
    if (!isEditGid()) return;

    if (AfxMessageBox(_TR("Объединить подающий и обратный участки? Направление будет выбрано по подающему участку"), MB_YESNO) != IDYES) return;

    m_CurNode = NULL;
    m_CurLine = NULL;

    CLINE2* line = m_menu_line;

    m_cxema->uniteLine2(line);

    Invalidate();
    AfxMessageBox(_TR("Выполнено"), MB_OK | MB_ICONINFORMATION);
}

CString findTableRusName(CString d, CString n);


int CGidrView::getActiv()
{
    if (!m_cxema || !m_cxema->m_cur_file) return 0;

    return m_cxema->m_cur_file->id;
}

bool CGidrView::setActiv0(int fileID)
{
    CGidFile* file = m_cxema->getGidFile(fileID);
    if (file) {
        m_cxema->m_cur_file = file;
        return true;
    }

    return false;
}


void CGidrView::setActiv(int fileID)
{
    CGidFile* file = m_cxema->getGidFile(fileID);
    if (file) {
        m_cxema->m_cur_file = file;
        CFRect rect = m_cxema->m_graph->getBorderFile(fileID);


        if (rect.left == LONG_MAX) {
            SetBorder(CFRect(0. + 40000., 0. - 40000., 400. + 40000., 400. - 40000.));
        }
        else {
            rect.InflateRect(100, 100);
            SetBorder(rect);
        }
        InvalidateG();


        Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

        CString s;

        CString rus_name = findTableRusName(file->m_schema, file->name);

        s.Format("%s - %s", pDoc->m_bd_gid, rus_name);

        GetParentFrame()->SetWindowText(s);


        AfxGetApp()->WriteProfileInt(szSection, _T("fileID"), fileID);
    }
}


void CGidrView::OnPopupActiv()
{
    CNode2* node = m_menu_node;
    CLINE2* line = m_menu_line;

    if (!node && line) {
        if (m_menu_line) node = where(m_menu_line);
    }

    if (node) {
        setActiv(node->node.fileID);
    }
}


void CGidrView::OnPopupInfo()
{
    info(m_menu_pt);
}

void CGidrView::OnPopupInfoNode()
{
    CNode2* node = m_menu_node;

    if (IsRezhim(node)) return;
    //  setIsEdit(isEditGid());

    info(m_cxema, node, NULL);
}

void CGidrView::OnPopupInfoLine()
{
    CLINE2* line = m_menu_line;
    if (IsRezhim(line)) return;

    //  setIsEdit(isEditGid());
    info(m_cxema, NULL, line);
}


int ado_info_line_big2(CWnd* wnd, CAdoFile* ado, const char* tn2, int id2, int idN1, int idN2, const char* capt);

int createUT_PTS(CAdoFile *ado, CLINE2 *l)
{
    CString sql;

    double diam_usl = bline(l)->line.pod.diam_usl != 0 ? bline(l)->line.pod.diam_usl : bline(l)->line.obr.diam_usl;
    double dl = bline(l)->line.pod.diam_usl != 0 ? bline(l)->line.pod.dl : bline(l)->line.obr.dl;

    sql.Format("INSERT INTO pipeSections (nodeID1,nodeID2,pipeLength,magistral,magistralSite,distSite,tubingTypeID, DiamUslov)\n"
                "VALUES (%d,%d,%g,%d,%d,%d,%d,%d)",
              where(l)->id, other(l)->id, dl, bline(l)->line.mag, bline(l)->line.ms, bline(l)->line.rs, bline(l)->line.tubingTypeID, diam_usl);

    long affected;

    int pipeSectionID = ado->ExecuteInsert(sql, &affected);

    if (pipeSectionID > 0) {
        sql.Format("UPDATE heatPipeSections SET pipeSectionID=%d WHERE lineID IN (%d,%d)", pipeSectionID, bline(l)->line.nomP, bline(l)->line.nomO);
        if (ado->Execute(sql, &affected)) {
            bline(l)->line.pipeSectionID = pipeSectionID;
        }
    }

    return pipeSectionID;
}


void CGidrView::OnPopupInfoLineBig()
{
    CLINE2* line = m_menu_line;
    //  if (IsRezhim(line)) return;

    if (!line) return;

    CAdoFile* ado = getAdo(getGidAdoName());

    if (!ado) return;

    CString q;

    bool is_big = false;

    q.Format("SELECT pipeSectionID FROM heatPipeSections JOIN heatPipeSection  WHERE lineID=%d OR lineID=%d", bline(line)->line.nomP, bline(line)->line.nomO);
    q.Format("SELECT p.id, p.nodeID1, p.nodeID2 FROM pipeSections p JOIN heatPipeSections hps ON hps.pipeSectionID=p.id WHERE hps.lineID=%d OR hps.lineID=%d", bline(line)->line.nomP, bline(line)->line.nomO);

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            long id = ado->read_long("id");
            long nodeID1 = ado->read_long("nodeID1");
            long nodeID2 = ado->read_long("nodeID2");

            m_linePTS_ID = id;
            m_pts_ut_kod = id;

            Invalidate();

            ado_info_line_big2(this, ado, "pipeSections", id, nodeID1, nodeID2, _TR("Участок ПТС"));
            is_big = true;
        }
        ado->closeTable();
    }

    if (!is_big) {
//        AfxMessageBox("Нет участка ПТС", MB_OK | MB_ICONINFORMATION);
        if (AfxMessageBox("Нет участка ПТС. Создать его на основе выбранного участка", MB_YESNO) == IDYES) {
            if (bline(line)->line.ms == 0 && bline(line)->line.rs == 0) {
                AfxMessageBox("Выбранный участок не принадлежит ни к какой магистрали или распредсети. Создание участка ПТС на его основе невозможно");
                return;
            }

            int id = createUT_PTS(ado, line);
            if (id > 0) {
                long nodeID1 = where(line)->id;
                long nodeID2 = other(line)->id;

                m_linePTS_ID = id;
                m_pts_ut_kod = id;

                Invalidate();

                ado_info_line_big2(this, ado, "pipeSections", id, nodeID1, nodeID2, _TR("Участок ПТС"));
                is_big = true;
            }
        }
    }
}



void CGidrView::OnPopupOut()
{
    if (m_menu_node) info_gid_node(m_menu_node);
    else if (m_menu_line) info_gid_line(m_menu_line);
}

void CGidrView::OnPopupOutDR()
{
    if (m_menu_node) info_gid_node_DR(m_menu_node);
}

void CGidrView::OnPopupDel()
{
    del(m_menu_pt);
}


void CGidrView::OnPopupDelZn()
{
    CNode2* node = m_menu_node;
    if (!node) return;

    CString str;
    str.Format(_TR("Удалить Узел с заданным напорм?"));
    if (MessageBox(str, _TR("Подтвердить удаление"), MB_YESNO) != IDYES) return;

    long affected;
    CString q;
    q.Format("DELETE FROM %s WHERE NodeID=%d", "setPressNodes", node->id);
    if (ado_execute(m_cxema->m_ado, q, &affected)) {
        node->node.nZN = -1;
        Invalidate();
    }
}


void CGidrView::OnPopupDelUp()
{
    CNode2* node = m_menu_node;
    if (!node) return;

    CString str;
    str.Format(_TR("Удалить Узел подпитки?"));
    if (MessageBox(str, _TR("Подтвердить удаление"), MB_YESNO) != IDYES) return;

    long affected;
    CString q;
    q.Format("DELETE FROM %s WHERE NodeID=%d", "refillNodes", node->id);
    if (ado_execute(m_cxema->m_ado, q, &affected)) {
        node->node.nUP = -1;
        Invalidate();
    }
}

void CGidrView::OnPopupDelVp()
{
    CNode2* node = m_menu_node;
    if (!node) return;

    CString str;
    str.Format(_TR("Удалить Водоразборный прибор?"));
    if (MessageBox(str, _TR("Подтвердить удаление"), MB_YESNO) != IDYES) return;
    long affected;
    CString q;
    q.Format("DELETE FROM %s WHERE NodeID=%d", "WDOdevices", node->id);
    if (ado_execute(m_cxema->m_ado, q, &affected)) {
        node->node.nVP = -1;
        Invalidate();
    }
}



void CGidrView::OnPopupDelNode()
{
    if (!isEditGid()) return;
    if (isPot()) return;

    CNode2* node = m_menu_node;

    delNode(node, true);
}

void CGidrView::OnPopupDelLine()
{
    if (!isEditGid()) return;
    if (isPot()) return;

    CLINE2* line = m_menu_line;

    delLine(line, true);
}


void CGidrView::OnPopupDelGeo()
{
    del(m_menu_pt);
}


void CGidrView::OnPopupWinccInfo()
{
    wincc_info(m_menu_pt);
}

void CGidrView::OnPopupNewUzel()
{
    if (!isEditGid()) return;
    if (isPot()) return;

    addNode(m_menu_pt);
}

void CGidrView::OnPopupRename()
{
    if (!isEditGid()) return;
    if (isPot()) return;

    CNode2* node = m_menu_node;
    CLINE2* line = m_menu_line;

    if (node) renameNode(node);
    else if (line) renameLine(line);
}


void CGidrView::Zn(CNode2* node)
{
    if (!isEditGid()) return;
    if (isPot()) return;

    if (node) {
        if (node->node.isP) {
            AfxMessageBox("Нельзя создавать внутреннюю схему в узле с внутренней схемой", MB_OK | MB_ICONINFORMATION);
            return;
        }
        if (node->node.nZN == -1) {
            if (AfxMessageBox(_TR("Создать узел с заданным напором?"), MB_YESNO) == IDYES) {
                if (m_cxema->setZN(node)) {
                    GetDocument()->SetModifiedFlag(true);
                    InvalidateG();
                }
            }
        }

        if (node->node.nZN != -1) {
            infoZN(m_cxema, node);
        }
    }
}


void CGidrView::OnPopupZn()
{
    Zn(m_menu_node);
}


void CGidrView::OnPopupORP()
{
    AfxMessageBox(_TR("Временно удалено 707"), MB_OK | MB_ICONINFORMATION);
}


void CGidrView::OnPopupUp()
{
    CNode2* node = m_menu_node;

    if (!isEditGid()) return;
    if (isPot()) return;

    if (node) {
        if (node->node.nUP == -1) {
            if (AfxMessageBox(_TR("Создать узел подпитки?"), MB_YESNO) == IDYES) {
                if (m_cxema->setUP(node)) {
                    GetDocument()->SetModifiedFlag(true);
                    InvalidateG();
                }
            }
        }

        if (node->node.nUP != -1) {
            infoUP(m_cxema, node);
        }
    }
}


void CGidrView::OnPopupKam()
{
    CNode2* node = m_menu_node;

    if (node) {
        m_parent_id = node->id;
        old_masx = geom.masx;
        old_masy = geom.masy;
        old_m_bx = geom.m_bx;
        old_m_by = geom.m_by;
        old_rect = m_rectBorder;

        CFRect rect = m_cxema->m_graph->getBorder(m_parent_id);
        SetBorder(rect);
        ChMas(rect);
    }
    Invalidate();
}


void CGidrView::OnUpdatePopupNewUzel(CCmdUI* pCmdUI)
{
    CNode2* node = m_menu_node;
    pCmdUI->Enable(!node && isEditGid());
}

void CGidrView::OnPopupCanal()
{
    if (!isEditGid()) return;
    CNode2* node = m_menu_node;
    CLINE2* line = m_menu_line;

    if (line) {
        AfxMessageBox("Укажите две точки на участке");
        regim = R_INIT;
        m_nRegim = ID_BEGIN_CANAL_1;
        m_l_canal = line;
/*
        CString text = "1";
        
        CInputDialog inD(this, _TR("Ширина канала"), _TR("Введите ширину канала"), text);
        if (inD.DoModal() == IDOK) {
            text = inD.m_strInput;
            double width = atof(text);
            if (width > 0) {
                CCoordList cl = bline(line)->line.m_NP;

                cl.push_front(where(line)->node.coord);
                cl.push_back(other(line)->node.coord);

                cl = cl.createBuffer(width*100./2);
                int nn = m_geofile->createObj("kanal", cl, m_map_geo_value);
                Invalidate();
            }
        }
*/
    }
}

void CGidrView::OnPopupMove()
{
    if (!isEditGid()) return;
    CNode2* node = m_menu_node;
    CLINE2* line = m_menu_line;

    if (node) {
        m_n1 = node;
        m_nRegim = ID_NODE_MOVE_NEXT;
        BeginDrawNode(m_n1);
    }
    else if (line) {
        m_nRegim = ID_NODE_MOVE;
        ism(line);
    }
}

void CGidrView::OnPopupClear()
{
    if (!isEditGid()) return;
    if (isPot()) return;
    CNode2* node = m_menu_node;
    if (node) {
        CString str;
        str.Format(_TR("Вы хотите удалить внутреннюю схему в узле %s"), node->getName());
        if (AfxMessageBox(str, MB_YESNO) == IDYES) {
            delCxema(node);
            node->node.isP = false;
            InvalidateG();
            GetDocument()->SetModifiedFlag(true);
        }
    }
}

void CGidrView::OnPopupPodpOn()
{
    m_isPodpAll = TRUE;
    InvalidateG();
}

void CGidrView::OnPopupPodpOff()
{
    m_isPodpAll = FALSE;
    InvalidateG();
}

void CGidrView::Podp(CNode2* node, CLine2* line)
{
    long affected;
    CString q;
    if (!isEditGid()) return;
    if (isPot()) return;

    if (node) {
        q.Format("update nodes set displaySign=%d where id=%d", node->node.isPodp, node->id);

        if (ado_execute(m_cxema->m_ado, q, &affected)) {
            node->node.isPodp = !node->node.isPodp;
            GetDocument()->SetModifiedFlag(true);
            InvalidateG();
        }
    }
    else if (line) {
        q.Format("update linesobj set displaySign=%d where id=%d OR id=%d", line->line.isPodp, line->line.nomP, line->line.nomO);

        if (ado_execute(m_cxema->m_ado, q, &affected)) {
            line->line.isPodp = !line->line.isPodp;
            GetDocument()->SetModifiedFlag(true);
            InvalidateG();
        }
    }
}

void CGidrView::OnPopupPodp()
{
    CString str;
    if (!isEditGid()) return;
    if (isPot()) return;

    CNode2* node = m_menu_node;
    CLINE2* line = m_menu_line;

    Podp(node, bline(line));
}

// Развернуть узел 6

void CGidrView::OnPopupGeoNapr()
{
    if (m_ng <= 0) return;

    CString str;
    if (isPot()) return;
    if (!m_geofile) return;

    CString tn = m_geo_name;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (kls) {
        int id = m_ng;

        if (!isEditGeo(tn)) return;

        if (id >= 0) {
            CGeoObject* geo = kls->getGeoObjectById(id);
            if (geo) {
                CCoordList cl = geo->cl;

                CFPoint p1 = cl.first();
                CFPoint p2 = cl.second();

                p2 = p1 + (p1 - p2);

                cl.clear();
                cl.push_back(p1);
                cl.push_back(p2);

                updateObj(tn, id, cl);
            }
        }
    }
}

void CGidrView::OnPopupCopyAkt()
{
    CString str;
    //  if (!isEditGid()) return;
    if (isPot()) return;
    if (!m_geofile) return;

    if (m_ng) {
        regim = R_INIT;
        SetCur(m_nRegim);
        m_nRegim = ID_COPY_AKT;
        m_n_akt = m_ng;
    }
}


void CGidrView::OnPopupGeoInfo()
{
    CString str;
    if (isPot()) return;
    if (!m_geofile) return;

    double md = D5 * geom.masx;
    char tn[256];

    int n = m_geofile->findXY(m_menu_pt, md, tn);

    if (n) {
        Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
        if (kls)
        {
            int pod = -1, obr = -1;
            if (isPoint(kls->loc)) {
                CGeoObject* idCurrent = kls->getGeoObjectById(n);
                CGeoObject* idNext = kls->getGeoObjectById(n + 1);
                CGeoObject* idPrev = kls->getGeoObjectById(n - 1);
                /*проверка id рядом +-1, и координаты совпадают тогда есть подача и обратка у объекта*/
                if (idPrev
                    && idPrev->rect.left == idCurrent->rect.left
                    && idPrev->rect.right == idCurrent->rect.right
                    && idPrev->rect.bottom == idCurrent->rect.bottom
                    && idPrev->rect.top == idCurrent->rect.top)
                {
                    /*Так как подача в базу записывается первой, текущий объект является обраткой если больше предыдущего (idPrev)*/
                    pod = n - 1;
                    obr = n;
                }
                else if (idNext
                    && idNext->rect.left == idCurrent->rect.left
                    && idNext->rect.right == idCurrent->rect.right
                    && idNext->rect.bottom == idCurrent->rect.bottom
                    && idNext->rect.top == idCurrent->rect.top) {
                    /*Так как подача в базу записывается первой, текущий объект является подачей если меньше следующего (idNext)*/
                    obr = n + 1;
                    pod = n;
                }
            }
            if (pod != -1) {
                CMMenu menu(AfxGetMainWnd(), IDD_MENU);
                menu.lst.Add(_TR("Подающий"));
                menu.lst.Add(_TR("Обратный"));

                if (menu.DoModal() == IDOK) {
                    switch (menu.m_Index) {
                    case 0: GeoInfo(tn, pod, false);
                        break;
                    case 1: GeoInfo(tn, obr, false);
                        break;
                    }
                }
                return;
            }

        }
        GeoInfo(tn, n, false);
    }
}

void CGidrView::OnPopupGeoDel()
{
    CString str;
    if (isPot()) return;
    if (!m_geofile) return;

    char fn[256];

    int n = m_geofile->findXY(m_menu_pt, D5 * geom.masx, 2, fn);

    if (!isEditGeo(fn)) return;
    if (!isDeletable(fn, n)) return;

    if (n) {


        str.Format(_TR("Удалить %s?"), getRusName(fn));
        Klassif* kls = m_geofile->m_kl_list->findKlN(fn);
        if (kls)
        {
            CGeoObject* idCurrent = kls->getGeoObjectById(n);
            CGeoObject* idNext = kls->getGeoObjectById(n + 1);
            CGeoObject* idPrev = kls->getGeoObjectById(n - 1);
            int pod = -1, obr = -1;
            /*проверка id рядом +-1, и координаты совпадают тогда есть подача и обратка у объекта*/
            if (idPrev
                && idPrev->rect.left == idCurrent->rect.left
                && idPrev->rect.right == idCurrent->rect.right
                && idPrev->rect.bottom == idCurrent->rect.bottom
                && idPrev->rect.top == idCurrent->rect.top)
            {
                /*Так как подача в базу записывается первой, текущий объект является обраткой если больше предыдущего (idPrev)*/
                pod = n - 1;
                obr = n;
            }
            else if (idNext
                && idNext->rect.left == idCurrent->rect.left
                && idNext->rect.right == idCurrent->rect.right
                && idNext->rect.bottom == idCurrent->rect.bottom
                && idNext->rect.top == idCurrent->rect.top) {
                /*Так как подача в базу записывается первой, текущий объект является подачей если меньше следующего (idNext)*/
                obr = n + 1;
                pod = n;
            }
            if (pod != -1) {
                CMMenu menu(AfxGetMainWnd(), IDD_MENU);
                menu.lst.Add(_TR("Подающий"));
                menu.lst.Add(_TR("Обратный"));
                if (menu.DoModal() == IDOK) {
                    switch (menu.m_Index) {
                    case 0:
                        if (MessageBox(str, _TR("Подтвердить удаление"), MB_YESNO) != IDYES) return;
                        delGeoObj(fn, pod);
                        break;
                    case 1:
                        if (MessageBox(str, _TR("Подтвердить удаление"), MB_YESNO) != IDYES) return;
                        delGeoObj(fn, obr);
                        break;
                    }
                }
                GetDocument()->SetModifiedFlag(true);
                InvalidateG();
                return;
            }

        }
        if (MessageBox(str, _TR("Подтвердить удаление"), MB_YESNO) != IDYES) return;

        delGeoObj(fn, n);

        GetDocument()->SetModifiedFlag(true);
        InvalidateG();
    }
}

void CGidrView::GeoTU(const char* tn, int num)
{
    AfxMessageBox("GeoTU", MB_OK | MB_ICONINFORMATION);
    /*
      CString str;
      if (!isEditGeo(tn)) return;
      if ( isPot() ) return;
      if (!m_geofile) return;

      if (num >= 0) {
        if (m_geofile->InitS(tn) && m_geofile->GoToID(num)) {
          CCoordList cl;

          m_geofile->GetCoord(cl);
          m_geofile->createObj(_TR("Техническое_условие"), cl);
          InvalidateG();
        }
      }
    */
}



void CGidrView::GeoDel(const char* tn, int num)
{
    CString str;
    if (!isEditGeo(tn)) return;
    if (isPot()) return;
    if (!m_geofile) return;

    if (num >= 0) {
        str.Format(_TR("Удалить %s?"), tn);
        if (MessageBox(str, _TR("Подтвердить удаление"), MB_YESNO) != IDYES) return;
        delGeoObj(tn, num);
        GetDocument()->SetModifiedFlag(true);
        InvalidateG();
    }
}


void CGidrView::OnPopupGeoMark()
{
    CString str;
    if (isPot()) return;
    if (!m_geofile) return;

    CString tn = m_geo_name;

    int m_num = m_ng;

    if (m_num >= 0) {
        m_nRegim = ID_ISM_GEO;
        m_kls = m_geofile->m_kl_list->findKlN(tn);

        if (!m_kls) return;

        //    m_geofile->GetCoord(m_NP);

        int nn = m_kls->GetNom2(m_num);

        if (nn <= 0) return;

        m_kls->geo4[nn - 1]->mark = !m_kls->geo4[nn - 1]->mark;
        Invalidate();
    }
}


void CGidrView::GeoNadpis(const char* tn, int num, BOOL b)
{
    /*
      if (!isEditGeo(tn)) return;
      if ( isPot() ) return;
      if (!m_geofile) return;

      if (num >= 0) {
        if (m_geofile->InitS(tn)) {

          m_geofile->m_kls->label = b;
          if (m_geofile->m_kl_list) m_geofile->m_kl_list->save2(m_geofile->klass_i2);

          InvalidateG();
        }
      }
    */

    AfxMessageBox("GeoNadpis", MB_OK | MB_ICONINFORMATION);

}

void CGidrView::GeoIsm(const char* tn, int num)
{
    CString str;
    if (!isEditGeo(tn)) return;
    if (isPot()) return;
    if (!m_geofile) return;

    m_num = num;

    if (m_num >= 0) {
        m_nRegim = ID_ISM_GEO;
        m_kls = m_geofile->m_kl_list->findKlN(tn);

        if (!m_kls) return;

        //    m_geofile->GetCoord(m_NP);

        int nn = m_kls->GetNom2(num);

        if (nn <= 0) return;

        m_NP = m_kls->geo4[nn - 1]->cl;


        CCoordList::iterator it = m_NP.begin();

        m_locNP = m_kls->loc;

        bool is_noend(const char* tn);


        m_noend = is_noend(tn);

        if (!isPoint(m_locNP)) {
            CDC* m_dc;
            if ((m_dc = BeginPaint()) != NULL) {
                DrawLine(m_dc);
                EndPaint(m_dc);
            }
        }

        if (m_locNP == LOC1 || m_locNP == LOC11) {  // Именно 1 !!!
            regim = R_BEGIN_MOVE_POINT1;
            AfxMessageBox(_TR("Установите новое место объекта"), MB_OK | MB_ICONINFORMATION);
        }
        else if (m_locNP == LOC6) {  // Именно 6 !!!
            regim = R_BEGIN_MOVE_POINT6;
            AfxMessageBox(_TR("Установите новое место объекта"), MB_OK | MB_ICONINFORMATION);
        }
        else {
            regim = R_BEGIN_MOVE_LINE;
        }
    }
}



void CGidrView::OnPopupGeoIsm()
{
    CString str;
    if (isPot()) return;
    if (!m_geofile) return;

    char fn[256];

    m_num = m_geofile->findXY(m_menu_pt, D5 * geom.masx, 2, fn);

    if (!isEditGeo(fn)) return;

    GeoIsm(fn, m_num);
}

void CGidrView::GeoIsmPodp(const char* tn, int num)
{
    AfxMessageBox(_TR("Временно удалено 708"), MB_OK | MB_ICONINFORMATION);
}

void CGidrView::OnPopupGeoIsmPodp()
{
    AfxMessageBox(_TR("Временно удалено 709"), MB_OK | MB_ICONINFORMATION);
}

void CGidrView::OnPopupGeoNadpis()
{
    if (isPot()) return;
    if (!m_geofile) return;

    if (!m_kls) return;

    if (!isEditGeo(m_kls->nazv)) return;

    m_kls->label = !m_kls->label;
    if (m_geofile->m_kl_list) m_geofile->m_kl_list->save2();

    InvalidateG();
}

void CGidrView::OnPopupGeoHide()
{
    AfxMessageBox(_TR("Временно удалено 711"), MB_OK | MB_ICONINFORMATION);
}

void CGidrView::OnPopupGeoShow()
{
    AfxMessageBox(_TR("Временно удалено 712"), MB_OK | MB_ICONINFORMATION);
}


void CGidrView::OnPopupGeoTU()
{
    AfxMessageBox(_TR("Временно удалено 713"), MB_OK | MB_ICONINFORMATION);
}


int copyZdan(const char* name1, int id, const char* name2);
int copyZdan2(const char* name1, int id, const char* name2);


void CGidrView::OnPopupGeoTU2()
{
    if (!isEditGeo("")) return;
    if (isPot()) return;
    if (!m_geofile) return;

    char fn[256];
    int m_num = m_geofile->findXY(m_menu_pt, D5 * geom.masx, 2, fn);

    if (m_num) {
        copyZdan(fn, m_num, _TR("Здания ТУ"));
        InvalidateG();
    }
}


void CGidrView::OnPopupGeoZD2()
{
    if (!isEditGeo("")) return;
    if (isPot()) return;
    if (!m_geofile) return;

    char fn[256];
    int m_num = m_geofile->findXY(m_menu_pt, D5 * geom.masx, 2, fn);

    if (m_num) {
        copyZdan2(fn, m_num, ZDANIYA_2);
        InvalidateG();
    }
}


void CGidrView::OnPopupGeoNagr()
{
    if (!isEditGeo("")) return;
    if (isPot()) return;
    if (!m_geofile) return;

    char fn[256];
    int m_num = m_geofile->findXY(m_menu_pt, D5 * geom.masx, 2, fn);

    if (m_num) {
        copyZdan(fn, m_num, _TR("Здания потребителей"));
        InvalidateG();
    }
}



void CGidrView::OnPopupGeoTULine()
{
    AfxMessageBox(_TR("Временно удалено 714"), MB_OK | MB_ICONINFORMATION);
}


void CGidrView::OnPopupLength()
{
    CLINE2* line = m_menu_line;

    if (!isEditGid() || !m_cxema->isGeobaza() || !IsMain()) return;

    CString str;

    str.Format(_TR("Изменить длину участка %s"), bline(line)->getName());

    if (AfxMessageBox(str, MB_YESNO) == IDYES) {
        if (line) m_cxema->setLength(line, is_google_map & GOOGLE_LEN, true);
        GetDocument()->SetModifiedFlag(true);
        Invalidate();
    }
}


//int magUzelInfo(CWnd *wnd, CNode *node);

CString getPSTable(int n);
int getPSTyp(const char* name);
CString read_db(const char* mdb, const char* tn, const char* q, int nom, const char* fn);


int get_int(const char* s, int is_name)
{
    CString ms = "";
    int dig = 0, pt = 0;

    for (int i = 0; s[i]; i++) {
        if (!('0' <= s[i] && s[i] <= '9') && s[i] != '.' && dig) {
            break;
        }
        if ('0' <= s[i] && s[i] <= '9') {
            ms += s[i];
            dig = 1;
        }
        if (dig && s[i] == '.') {
            pt = 1;
        }
    }
    int m = -1;

    if (is_name && !pt) m *= 10;

    if (sscanf(ms, "%d", &m) == 1) {
        return m;
    }
    return -1;
}

CString getPSTable(int n);
CString getPSTableGeo(int n);

CString getName(const char* kod1, const char* name1)
{
    CString text, kod = kod1, name = name1;

    if (name.Find("(") >= 0) {
        return name;
    }

    kod.Replace(" ", "");
    kod.Replace(_TR("М"), "");
    kod.Replace("M", "");
    kod.Replace("#", "");

    text.Format("%s(%s)", name1, kod);

    return text;
}

void initRS0();

void CGidrView::OnPopupVyd()
{
    CNode2* node = m_menu_node;
    if (!node && m_menu_line) {
        node = where(m_menu_line);
    }
    if (node) vydel(node);
}

void CGidrView::OnNextUzel(UINT nID)
{
    CNode2* node = m_menu_node;
    if (!node) return;

    int n = nID - ID_NEXT_UZEL1;

    CLINE2* l;
    int i = 0;

    set <CNode2*> set_node;

    for (l = node->lines; l; l = next(l)) {
        CNode2* n2 = other(l);
        set_node.insert(n2);
    }
    set <CNode2*>::const_iterator it;

    for (it = set_node.begin(); it != set_node.end(); it++, i++) {
        if (i == n) {
            CNode2* n2 = *it;
            moveXY(n2->node.coord);
            break;
        }
    }
}

CNode2* find_next(CGraph2* graph, CNode2* n1, CNode2* n2, CNode2* n0)
{
    n1->node.isPjezo = true;

    if (inc(n1) != 2) return n1;
    if (n1->node.namePS != "") return n1;

    for (CLINE2* l = n1->lines; l; l = next(l)) {
        CNode2* o = other(l);
        if (o != n2 && o != n0) {
            bline(l)->line.isPjezo = true;
            return find_next(graph, o, n1, n0);
        }
    }
    return NULL;
}

CNode* find_next2(CGraph2* graph, CNode* n1)
{
    return NULL;
}

CString name_of_node(CNode* n)
{
    CString nm = n->namePS;

    if (nm == "") nm.Format("%s %s", n->node_name.kod, n->name);

    return nm;
}

CNode2* find_zn0(CGraph2* m_graph, CNode2* node, double& length)
{
    if (!node) return NULL;
    if (!m_graph) return NULL;

    CNode2* min_n = node;
    double min_len = 1e20;


    CMapGraph::iterator p;
    p = m_graph->map_node.begin();

    while (p != m_graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.coord.x != 0 || n->node.coord.y != 0) {
            double len = Length(n->node.coord, node->node.coord);
            if (len < min_len) {
                min_len = len;
                min_n = n;
            }
        }
        p++;
    }

    length = min_len;

    if (length == 1e20) {
        return NULL;
    }

    return min_n;
}

CNode2* find_zn0(CCxema* m_cxema, CNode2* node, double& length)
{
    if (!node) return NULL;

    CNode2* min_n = node;
    double min_len = 1e20;

    length = min_len;

    if (length == 1e20) {
        return NULL;
    }

    return min_n;
}

void findMag(int mag);
void findMs(int mag);
void findRs(int mag);
void findOpr(int opr);

int vydMagN(CGraph2* m_graph, int id);
int viewUchastok(CGraph2* m_graph, tuple<int, int> msrs, int type /*1 - Магистраль 2 Распредсеть 3 Магистраль и распредсеть*/, bool is_only_big_pipe);

void CGidrView::OnVydMag()
{
    CLINE2* line = m_menu_line;
    if (!line) return;

    m_ms = 0;
    m_rs = 0;
    m_mag = bline(line)->line.mag;

    vydMagN(m_cxema->m_graph, bline(line)->line.mag);
    findMag(bline(line)->line.mag);
    Invalidate();
}

void CGidrView::OnVydMs()
{
    CLINE2* line = m_menu_line;
    if (!line) return;

    m_ms = bline(line)->line.ms;
    m_rs = 0;
    m_mag = 0;

    m_cxema->m_graph->reset();
    viewUchastok(m_cxema->m_graph, make_tuple(bline(line)->line.ms, 0), 1, true);
    findMs(bline(line)->line.ms);
    Invalidate();
}

void CGidrView::OnVydRs()
{
    CLINE2* line = m_menu_line;
    if (!line) return;

    m_ms = 0;
    m_rs = bline(line)->line.rs;
    m_mag = 0;

    m_cxema->m_graph->reset();
    viewUchastok(m_cxema->m_graph, make_tuple(0, bline(line)->line.rs), 2, true);
    findRs(bline(line)->line.rs);
    Invalidate();
}


// что-то про опрессовку


void CGidrView::OnVydUchOpr()
{
    AfxMessageBox(_TR("Временно удалено 715"), MB_OK | MB_ICONINFORMATION);
}

// Камера присоединения

void CGidrView::OnPopupNagr2()
{
    CNode2* node = m_menu_node;
    if (!node) return;

    CMMenu menu(this, IDD_MENU2, "");

    while (node) {
        CString str;

        str.Format("%s", node->getName());

        menu.Add(str);

        CNode2* nn = node;
        node = NULL;

        for (CLINE2* l = nn->lines; l; l = next(l)) {
            CLine2* ll = bline(l);
            double naprP = ll->line.pod.q * napr(l);

            if (ll->line.nomP != -1 && naprP < 0) {
                node = other(l);
                break;
            }
        }
    }

    menu.DoModal();
}


void CGidrView::OnPopupPas()
{
    if (isPot()) return;

    CNode2* node = m_menu_node;

    if (node->node.typ == TIP_PR) {
        //    pasport_PR(node);
    }
    else {
        pasport_US(node);
    }
}


int vydRS(CGraph2* m_graph, const char* kod);


CNode2* first(CLINE2* l)
{
    if (bline(l)->line.pod.q * napr(l) > 0) return where(l);
    return other(l);
}

CNode2* second(CLINE2* l)
{
    if (bline(l)->line.pod.q * napr(l) > 0) return other(l);
    return where(l);
}

struct Nagr
{
    Nagr() : o(0), g(0), v(0) {};
    double o, g, v;
    void setZero() {
        o = g = v = 0.;
    }
};


void getNagr0(Nagr* nagr0, CLINE2* l1, CMMenu* m)
{
    Nagr nagr;

    CString str;
    int n_v = 0;

    CNode2* n1 = other(l1);
    if (bline(l1)->line.isPjezo) return;

    for (CLINE2* l = n1->lines; l; l = next(l)) {
        CLine2* ll = bline(l);
        double naprP = ll->line.pod.q * napr(l);

        if (ll->line.nomP != -1 && naprP > 0) {
            n_v++;
        }
    }

    for (CLINE2* l = n1->lines; l; l = next(l)) {
        CLine2* ll = bline(l);
        double naprP = ll->line.pod.q * napr(l);

        if (ll->line.nomP != -1 && naprP < 0) {
            if (ll != bline(l1)) return;
        }
    }

    bline(l1)->line.isPjezo = true;
    n1->node.isPjezo = true;

    if (n1->node.typ == TIP_PR || n1->node.typ == TIP_PO) {
        nagr.o = n1->node.Qot;
        nagr.g = n1->node.Qgvs;
        nagr.v = n1->node.Qvent;

        nagr0->o += nagr.o;
        nagr0->g += nagr.g;
        nagr0->v += nagr.v;

        str.Format("%s (%g %g %g)", n1->getName(), n1->node.Qot, n1->node.Qgvs, n1->node.Qvent);
        m->Add(str, (void*)n1);
    }

    for (CLINE2* l = n1->lines; l; l = next(l)) {
        CLine2* ll = bline(l);
        double naprP = ll->line.pod.q * napr(l);

        if (ll->line.nomP != -1 && naprP > 0) {
            getNagr0(nagr0, l, m);
        }
    }
}

double okr_d(double x)
{
    double dd = 100000.;

    x *= dd;
    x += 0.5;
    long l = x;

    x = l / dd;

    return x;
}

#include "ado.h"

void getNagr(CAdoFile* ado, Nagr* nagr0, CLINE2* l1, const char* tn)
{
    Nagr nagr;

    CString str;
    int n_v = 0;

    CNode2* n1 = other(l1);
    if (bline(l1)->line.isPjezo) return;

    CLINE2* l;

    for (l = n1->lines; l; l = next(l)) {
        CLine2* ll = bline(l);
        double naprP = ll->line.pod.q * napr(l);

        if (ll->line.nomP != -1 && naprP > 0) {
            n_v++;
        }
    }

    for (l = n1->lines; l; l = next(l)) {
        CLine2* ll = bline(l);
        double naprP = ll->line.pod.q * napr(l);

        if (ll->line.nomP != -1 && naprP < 0) {
            if (ll != bline(l1)) return;
        }
    }

    bline(l1)->line.isPjezo = true;
    n1->node.isPjezo = true;

    if (n1->node.typ == TIP_PR || n1->node.typ == TIP_PO) {
        nagr.o = n1->node.Qot;
        nagr.g = n1->node.Qgvs;
        nagr.v = n1->node.Qvent;

        nagr0->o += nagr.o;
        nagr0->g += nagr.g;
        nagr0->v += nagr.v;

        //    str.Format("%s %s (%g %g %g)", n1->node_name.kod, n1->name, );
        //    m->Add(str, (void*)n1);

        /*
            dbf.AddNew();
            dbf.write(1, n1->node_name.kod);
            dbf.write(2, n1->node_name.name);
            dbf.write(3, okr_d(n1->Qot));
            dbf.write(4, okr_d(n1->Qvent));
            dbf.write(5, okr_d(n1->Qgvs));
            dbf.write(6, n1->file->m_dbf->m_strFileName);
        */
        CString q;
        long affected;

        q.Format("INSERT INTO %s (kod, name, Otoplenie, Vent, gvs) VALUES ('', '', %g, %g, %g)", tn, n1->node.Qot, n1->node.Qvent, n1->node.Qgvs);

        long idv = ado->ExecuteInsert(q, &affected);
    }

    for (l = n1->lines; l; l = next(l)) {
        CLine2* ll = bline(l);
        double naprP = ll->line.pod.q * napr(l);

        if (ll->line.nomP != -1 && naprP > 0) {
            getNagr(ado, nagr0, l, tn);
        }
    }
}

CString getGidAdoName();

void CGidrView::PopupOtklPotr(CLINE2* line)
{
    if (!line) return;
    reset();
    Nagr nagr0;

#if 0
    long affected;
    CString sql, mdb = getVyd();
    CString tn = _TR("Отключенные потребители");

    CAdoFile* ado = getAdo(getGidAdoName());

    //    ado->delete_table(tn);

    sql.Format("DROP TABLE IF EXISTS %s", tn);
    ado->Execute(sql, &affected);

    sql.Format("CREATE TABLE [%s] ([kod] char(50), [uzel] char(255), [Отопление] DOUBLE, [Вентиляция] DOUBLE, [ГВС] DOUBLE, [Схема] char(255))", tn);
    sql.Format("CREATE TABLE [%s] ([kod] char(50), [uzel] char(255), [Отопление] char(50), [Вентиляция] char(50), [ГВС] char(50), [Схема] char(255))", tn);

    tn = "otkl_potr";

    sql.Format("CREATE TEMPORARY TABLE %s (id INT NOT NULL AUTO_INCREMENT, kod char(50), uzel char(255), Otoplenie DOUBLE, Vent DOUBLE, gvs DOUBLE, PRIMARY KEY (id))", tn);

    ado->Execute(sql, &affected);

    if (bline(line)->line.pod.q * napr(line) < 0) {
        line = sym(line);
    }

    getNagr(ado, &nagr0, line, tn);

    CString q;

    q.Format("INSERT INTO %s (Otoplenie, Vent, gvs) VALUES (%g, %g, %g)", tn, nagr0.o, nagr0.v, nagr0.g);

    //  long idv = ado_executeInsert(ado, q, &affected);
    long idv = ado->ExecuteInsert(q, &affected);

    Invalidate();

    q.Format("SELECT * FROM %s", tn);

    RunTableDB(ado->m_schema, tn, q, -1);

#endif
    //void RunTableBottom(const char *mdb, const char *tn, const char *q);
    //  RunTableBottom(mdb, tn, q);
}

void CGidrView::OnPopupOtklPotr()
{
    CLINE2* line = m_menu_line;
    if (!line) return;

    PopupOtklPotr(line);
}

void CGidrView::OnDopPodklNagr()
{
    AfxMessageBox(_TR("Расчет допустимой подключаемой нагрузки, не сделано"), MB_OK | MB_ICONINFORMATION);
}



void CGidrView::OnPopupNormTemp()
{
    AfxMessageBox("!!NormTemp", MB_OK | MB_ICONINFORMATION);
}

//#include "TempGraph/TgView.h"

void viewTG(CWnd* wnd, int id_ist);

void CGidrView::OnPopupTg()
{
    CNode2* node = m_menu_node;

    if (node) {
        if (0) {
            CString s;
            s.Format("%d", node->node.id2);
            //        CString path = argpath() + "tempGraphMain";
            //        HINSTANCE hRun = ShellExecute (this->GetSafeHwnd (), "open", path+"\\tempGraphMain.exe", s, path, SW_SHOW);

            Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

            void mk_config_json(Cgid6Doc * pDoc);
            mk_config_json(pDoc);

            CString path = argpath() + "tempGraphAstana";
            s.Format("tempGraphMain.py %d", node->node.id2);
            //        HINSTANCE hRun = ShellExecute (this->GetSafeHwnd (), "open", "pythonw", s, path, SW_SHOW);
            HINSTANCE hRun = ShellExecute(this->GetSafeHwnd(), "open", pythonw_exe(), s, path, SW_SHOW);

        }
        else {
            viewTG(this, node->node.id2);
        }
    }
}

void CGidrView::OnPopupTg2()
{
    CNode2* node = m_menu_node;

    if (node) {
        if (1) {
            CString s;
            s.Format("%d", node->node.id2);
            //        CString path = argpath() + "tempGraphMain";
            //        HINSTANCE hRun = ShellExecute (this->GetSafeHwnd (), "open", path+"\\tempGraphMain.exe", s, path, SW_SHOW);

            Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

            void mk_config_json(Cgid6Doc * pDoc);
            mk_config_json(pDoc);

            CString path = argpath() + "tempGraphAstana";
            s.Format("tempGraphMain.py %d", node->node.id2);
            HINSTANCE hRun = ShellExecute(this->GetSafeHwnd(), "open", pythonw_exe(), s, path, SW_SHOW);
        }
        else {
            viewTG(this, node->node.id2);
        }
    }
}




#include "TempGraph/tempgraph.h"

bool readTG(CAdoFile* ado);


#if 0
void CGidrView::viewTG(int id)
{
    CAdoFile* ado = getAdo(getGidAdoName());
    if (!ado) return;

    readTG(ado);
    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
    Cgid6App* theApp = (Cgid6App*)AfxGetApp();

    pDoc->m_cxema = m_cxema;
    //  pDoc->m_graph = m_cxema->m_graph;

    CFrameWnd* pWndSecondChild;
    CMultiDocTemplate* pDocTemplate;

    CTgView* pView;

    POSITION pos = pDoc->GetFirstViewPosition();
    while (pos != NULL)
    {
        pView = (CTgView*)pDoc->GetNextView(pos);
        if (pView->IsKindOf(RUNTIME_CLASS(CTgView))) {
            CMDIFrameWnd* pFrame = (CMDIFrameWnd*)theApp->m_pMainWnd;
            pFrame->MDIActivate(pView->GetParent());
            //            pView->initPjezo();
        }
    }

    pDocTemplate = theApp->GetDocTemplate(DOCTEMPLATE_TGRAPH);
    if (pDocTemplate != NULL)
    {
        pWndSecondChild = pDocTemplate->CreateNewFrame(pDoc, NULL);
        ASSERT(pWndSecondChild != NULL);
        pDocTemplate->InitialUpdateFrame(pWndSecondChild, pDoc);
    }
}
#endif

#if 0
LRESULT CGidrView::OnAdoUpdated(WPARAM wParam, LPARAM)
{
    //    init();

    CAdoFile* ado = getAdo(getGidAdoName());
    if (!ado) return 0;

    if (m_ado_update_typ == 1) {

#if 0
        CTempGraph tg(ado, m_ado_update_id);

        CString q;

        CString TableName1 = "deployedTempGraphs";
        q.Format("SELECT id,tn,Q_otn,t1,t2,t3,tv,t_bn,tg FROM %s WHERE hSourceID=%d ORDER BY tn DESC", TableName1, m_ado_update_id);
        viewQ2(ado->m_schema, q, TableName1, virt_data_tg);
        viewTG(m_ado_update_id);
#endif
    }

    m_ado_update_typ = -1;

    return 0;
}

#endif

#if 0
#include "Sheme\SHEME.H"

void CGidrView::OnPopupPr()
{
    //  AfxMessageBox("!", MB_OK|MB_ICONINFORMATION);
    RECT Clre;
    CSheme* Shem = new CSheme(argpath() + "/Схемы/6.3.con", Clre);

    //    m_Shema.GetClientRect(&Clre); 

    CWnd* m_Shema = this;

    if (!Shem->Create(NULL, _T("Hi"), WS_CHILD | WS_VISIBLE, Clre, m_Shema, 8888)) {
        Shem = NULL;
    }
    if (Shem != NULL)
    {
        //      UpdateSheme();      
    }
}



#endif


void CGidrView::OnDelText()
{
    if (m_text_del) {
        if (m_cxema->delText(m_text_del)) {
            GetDocument()->SetModifiedFlag(true);
            InvalidateG();
        }
    }
}


void CGidrView::OnEditText()
{
    if (m_text_del) {
        if (m_cxema->delText(m_text_del)) {
            GetDocument()->SetModifiedFlag(true);
            InvalidateG();
        }
    }
}

void CGidrView::OnFontText()
{
    if (m_text_del) {
        if (m_cxema->delText(m_text_del)) {
            GetDocument()->SetModifiedFlag(true);
            InvalidateG();
        }
    }
}

int color_open_close(int stateID);


bool getPodObrNum(CString tn, int id, int& pod, int& obr)
{
    pod = -1;
    obr = -1;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (kls)
    {
        CGeoObject* idCurrent = kls->getGeoObjectById(id);
        CGeoObject* idNext = kls->getGeoObjectById(id + 1);
        CGeoObject* idPrev = kls->getGeoObjectById(id - 1);
        /*проверка id рядом +-1, и координаты совпадают тогда есть подача и обратка у объекта*/
        if (idPrev
            && idPrev->rect.left == idCurrent->rect.left
            && idPrev->rect.right == idCurrent->rect.right
            && idPrev->rect.bottom == idCurrent->rect.bottom
            && idPrev->rect.top == idCurrent->rect.top)
        {
            /*Так как подача в базу записывается первой, текущий объект является обраткой если больше предыдущего (idPrev)*/
            pod = id - 1;
            obr = id;
            return true;
        }
        else if (idNext
            && idNext->rect.left == idCurrent->rect.left
            && idNext->rect.right == idCurrent->rect.right
            && idNext->rect.bottom == idCurrent->rect.bottom
            && idNext->rect.top == idCurrent->rect.top) {
            /*Так как подача в базу записывается первой, текущий объект является подачей если меньше следующего (idNext)*/
            obr = id + 1;
            pod = id;
            return true;
        }
    }

    pod = id;

    return false;
}


bool setColorZd(Klassif* kls, int id)
{
    if (id >= 0) {
        CGeoObject* geo = kls->getGeoObjectById(id);
        if (geo) {
            int stateIDNew = geo->cl.stateID == 1 ? 2 : 1;

            if (m_geofile->updateObjField(kls->nazv, id, kls->codlabel_stateID, stateIDNew)) {
                geo->cl.stateID = stateIDNew;
                geo->cl.color = color_open_close(stateIDNew);
                return true;
            }
        }
    }
    return false;
}


void CGidrView::OnPopupOpenCloseStateId()
{
    if (m_ng <= 0) return;

    CString str;
    if (isPot()) return;
    if (!m_geofile) return;

    CString tn = m_geo_name;

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (kls) {
        int id = m_ng;
        int pod = id, obr;
        getPodObrNum(tn, id, pod, obr);
        if (!isEditGeo(tn)) return;

        bool r1 = setColorZd(kls, pod);
        bool r2 = setColorZd(kls, obr);

        if (r1 || r2) {
            if (isOpenZhur(51))   // Обновить список задвижек внизу, если он открыт
                OnOpresZdAll();

            Invalidate();
        }
        else {
            AfxMessageBox("Ошибка");
        }
    }
}

int s_to_int(const char* s);


void CGidrView::OnNodeZd()
{
    CAdoFile* ado = m_cxema->m_ado;
    CString tn = "zapornaya_armatura";

    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);
    if (!kls) return;

    CNode2* node = m_menu_node;

    if (!node) return;

    CString q;

    q.Format(
"SELECT \n"
"zd_id AS id,\n"
"n.nodeName as nodeName,\n"
"n1.nodeName as name1,\n"
"n2.nodeName as name2,\n"
"es.name,\n"
"zd.stateID,\n"
"st.name,\n"
"zd.nomer2,\n"
"zd.diametr,\n"
"zd.primechanie\n"

"FROM (\n"
"SELECT DISTINCT\n"
"zd.id AS zd_id,\n"
"l.id AS l_id,\n"
"zd.nodeID_pts,\n"
"zd.priznak_truboprovoda,\n"
"zd.stateID,\n"
"zd.nomer2,\n"
"zd.diametr,\n"
"zd.primechanie,\n"
"zd.shape.STDistance(l.shape) AS len,\n"
"MIN(zd.shape.STDistance(l.shape)) OVER(PARTITION BY zd.id ) AS min_len\n,"
"MIN(l.id) OVER(PARTITION BY zd.id) AS min_id\n"
"FROM zapornaya_armatura zd\n"
"LEFT JOIN linesobj l ON zd.shape.STDistance(l.shape) < 0.1 AND l.removed=0\n"
"WHERE zd.nodeID_pts=%d\n"
") zd\n"
"LEFT JOIN damperArmatureStates st ON st.id=zd.stateID\n"
"LEFT JOIN externalSigns es ON es.id=zd.priznak_truboprovoda\n"
"LEFT JOIN heatPipeSections hps ON hps.lineID=l_id\n"
"LEFT JOIN pipeSections pss ON pss.id=hps.pipeSectionID\n"
"LEFT JOIN nodes n1 ON pss.nodeID1=n1.id\n"
"LEFT JOIN nodes n2 ON pss.nodeID2=n2.id\n"
"LEFT JOIN nodes n ON zd.nodeID_pts=n.id\n"

"WHERE len = min_len AND l_id = min_id\n"
//" AND n1.fileID IN (%s)\n"
"ORDER BY zd_id\n"
        , node->id
//        , m_cxema->m_par
    );

    int n_ret = 1;

    CTableDialog dlg(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, n_ret,
        virt_data_edit, "Задвижки", IDD_TABLE_DEFECT_NOADD_NODEL);

    dlg.setHead("", "zd_node");

    int ret = dlg.DoModal();

    q.Format(
        "SELECT zd.id, stateID FROM zapornaya_armatura zd\n"
        "WHERE zd.nodeID_pts=%d\n"
        , node->id
    );



    if (ret == IDOK) {
        if (ado->openTable0(q)) {
            while (!ado->isEOF()) {
                long id = ado->read_long("id");
                long stateID = ado->read_long("stateID");
                CGeoObject* geo = kls->getGeoObjectById(id);
                if (geo) {
                    geo->cl.stateID = stateID;
                    geo->cl.color = color_open_close(geo->cl.stateID);
                }
                ado->MoveNext();
            }
            ado->closeTable();
        }
    }
    else {
        set<int> set_zd;
        if (ado->openTable0(q)) {
            while (!ado->isEOF()) {
                long id = ado->read_long("id");
                set_zd.insert(id);
                ado->MoveNext();
            }
            ado->closeTable();
        }
        for (auto& it : set_zd) {
            CGeoObject* geo = kls->getGeoObjectById(it);
            if (geo) {
                CString q;
                q.Format("UPDATE zapornaya_armatura SET stateID=%d WHERE id=%d", geo->cl.stateID, it);
                ado->Execute(q);
            }
        }
    }


    Invalidate();

}
