// gidrView.cpp : implementation of the CGidrView class
//

#include "stdafx.h"
#include "gid6.h"

#include "gid6Doc.h"
//#include "DbTable.h"
#include "gidrView.h"
#include "win.h"
#include "mmenu.h"
//#include "TeplopotrBox.h"
#include "InputDia.h"

#include "virtualnodelist.h"
#include "VirtualDlg.h"

#include "ado.h"

#include "TeplopotrBox.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct PT {
    void Init(CAdoFile* ado) {
    }

    void read(CAdoFile* ado) {
        a4 = ado->read_double("a4");  //  Полученный расход на отопление, зав.сх.
        a5 = ado->read_double("a5");  //  Полученный расход на отопление, незав.сх.
        a6 = ado->read_double("a6");  //  Полученный расход на вентиляцию
        a7 = ado->read_double("a7");  //  Полученный расход на кондиционирование
        a8 = ado->read_double("a8");  //  Полученный расход на технологию в закр.сист.
        a9 = ado->read_double("a9");  //  Полученный расход на техн. из под.трубопр. в откр сист.
        a10 = ado->read_double("a10");  //  Полученный расход потреб.на техн. из обр.труб.в откр.сист.
        a11 = ado->read_double("a11");  //  Полученный расход потребителя на ГВ в закр.системе
        a12 = ado->read_double("a12");  //  Полученный расход потреб.на ГВ из под.труб.в откр.сист.
        a13 = ado->read_double("a13");  //  Полученный расход потреб. на ГВ из обр.труб. в откр.сист.
        a14 = ado->read_double("a14");  //  Полученный расход потреб.в цирк.трубопроводе ГВС
        a15 = ado->read_double("a15");  //  Суммарный расход потребителя в закрытой системе
        a16 = ado->read_double("a16");  //  Сумм.расход потребителя из под.трубопр. в откр.системе
        a17 = ado->read_double("a17");  //  Сумм.расход потребителя из обр.трубопр. в откр.системе
        a18 = ado->read_double("a18");  //  Относительный расход сетевой воды в закрытой системе
        a19 = ado->read_double("a19");  //  Заданное значение утечки из подающего трубопровода
        a20 = ado->read_double("a20");  //  Заданное значение утечки из обратного трубопровода
        a21 = ado->read_double("a21");  //  Пьезометрический напор в подающем трубопроводе
        a22 = ado->read_double("a22");  //  Пьезометрический напор в обратном трубопроводе
        a23 = ado->read_double("a23");  //  Располагаемый напор в узле присоед.потребителя
        Gco = ado->read_double("Gco");  //  Расход воды на входе СО
        Gsm = ado->read_double("Gsm");  //  Расход воды в СО после узла смешения
        Gm = ado->read_double("Gm");  //  Расход воды на входе местной СО, незав.присоед.
        Gx = ado->read_double("Gx");  //  Расход водопроводной воды в закрытой ГВС
        Ggws = ado->read_double("Ggws");  //  Расход сет.воды на вторую ступень закрытой ГВС
        t1 = ado->read_double("t1");  //  Температура сет.воды на входе теплового пункта
        t2 = ado->read_double("t2");  //  Температура сет.воды на выходе теплового пункта
        t01 = ado->read_double("t01");  //  Температура в подающем трубопроводе СО
        t02 = ado->read_double("t02");  //  Температура в обратном трубопроводе СО
        t03 = ado->read_double("t03");  //  Температура после узла смешения в СО
        t2co = ado->read_double("t2co");  //  Температура сет.воды на выходе подогревателя СО
        t2gws = ado->read_double("t2gws");  //  Температура сет.воды на вых.подогревателя 1 ст.ГВС
        tv = ado->read_double("tv");  //  Температура внутри отапливаемых помещений
        tg = ado->read_double("tg");  //  Горячая вода на выходе водоразб.приборов в ГВС
        dop8 = ado->read_double("dop8");  //  Темп.нагретого воздуха в системе вентиляции
        dop9 = ado->read_double("dop9");  //  Темп. нагретого воздуха в системе кондиционирования
        Qotz = ado->read_double("Qotz");  //  Полученная тепл.нагрузка  на отопление, зав.сх.
        Qotn = ado->read_double("Qotn");  //  Полученная тепл.нагрузка  на отопление, незав.сх.
        dop12 = ado->read_double("dop12");  //  Полученная тепловая нагрузка потребителя на вентиляцию
        dop13 = ado->read_double("dop13");  //  Полученная тепл.нагрузка на кондиционирование
        dop14 = ado->read_double("dop14");  //  Получ.тепл.нагрузка на технологию в закрытой системе
        dop15 = ado->read_double("dop15");  //  Полученная тепл.нагрузка на техн. из под.труб.в откр.сист.
        dop16 = ado->read_double("dop16");  //  Полученная тепл.нагрузка на техн. из обр.труб.в откр. сист.
        dop17 = ado->read_double("dop17");  //  Полученная тепл.нагрузка на ГВ в закрытой системе
        dop18 = ado->read_double("dop18");  //  Полученная тепл.нагрузка на ГВ из под.труб.в откр.сист.
        dop19 = ado->read_double("dop19");  //  Полученная тепл.нагрузка на ГВ из обр.труб.в откр.сист.
        dop20 = ado->read_double("dop20");  //  Полученная тепл.нагрузка в цирк.трубопроводе ГВС
        Qsum_z = ado->read_double("Qsum_z");  //  Суммарная тепл.нагрузка  в закрытой системе
        dop22 = ado->read_double("dop22");  //  Сумм.тепл.нагрузка  из обр.трубопр.в откр.системе
        dop23 = ado->read_double("dop23");  //  Сумм.тепл.нагрузка  из обр.трубопр.в откр.системе
    }


    double a4;  //  Полученный расход на отопление, зав.сх.
    double a5;  //  Полученный расход на отопление, незав.сх.
    double a6;  //  Полученный расход на вентиляцию
    double a7;  //  Полученный расход на кондиционирование
    double a8;  //  Полученный расход на технологию в закр.сист.
    double a9;  //  Полученный расход на техн. из под.трубопр. в откр сист.
    double a10;  //  Полученный расход потреб.на техн. из обр.труб.в откр.сист.
    double a11;  //  Полученный расход потребителя на ГВ в закр.системе
    double a12;  //  Полученный расход потреб.на ГВ из под.труб.в откр.сист.
    double a13;  //  Полученный расход потреб. на ГВ из обр.труб. в откр.сист.
    double a14;  //  Полученный расход потреб.в цирк.трубопроводе ГВС
    double a15;  //  Суммарный расход потребителя в закрытой системе
    double a16;  //  Сумм.расход потребителя из под.трубопр. в откр.системе
    double a17;  //  Сумм.расход потребителя из обр.трубопр. в откр.системе
    double a18;  //  Относительный расход сетевой воды в закрытой системе
    double a19;  //  Заданное значение утечки из подающего трубопровода
    double a20;  //  Заданное значение утечки из обратного трубопровода
    double a21;  //  Пьезометрический напор в подающем трубопроводе
    double a22;  //  Пьезометрический напор в обратном трубопроводе
    double a23;  //  Располагаемый напор в узле присоед.потребителя
    double Gco;  //  Расход воды на входе СО
    double Gsm;  //  Расход воды в СО после узла смешения
    double Gm;  //  Расход воды на входе местной СО, незав.присоед.
    double Gx;  //  Расход водопроводной воды в закрытой ГВС
    double Ggws;  //  Расход сет.воды на вторую ступень закрытой ГВС
    double t1;  //  Температура сет.воды на входе теплового пункта
    double t2;  //  Температура сет.воды на выходе теплового пункта
    double t01;  //  Температура в подающем трубопроводе СО
    double t02;  //  Температура в обратном трубопроводе СО
    double t03;  //  Температура после узла смешения в СО
    double t2co;  //  Температура сет.воды на выходе подогревателя СО
    double t2gws;  //  Температура сет.воды на вых.подогревателя 1 ст.ГВС
    double tv;  //  Температура внутри отапливаемых помещений
    double tg;  //  Горячая вода на выходе водоразб.приборов в ГВС
    double dop8;  //  Темп.нагретого воздуха в системе вентиляции
    double dop9;  //  Темп. нагретого воздуха в системе кондиционирования
    double Qotz;  //  Полученная тепл.нагрузка  на отопление, зав.сх.
    double Qotn;  //  Полученная тепл.нагрузка  на отопление, незав.сх.
    double dop12;  //  Полученная тепловая нагрузка потребителя на вентиляцию
    double dop13;  //  Полученная тепл.нагрузка на кондиционирование
    double dop14;  //  Получ.тепл.нагрузка на технологию в закрытой системе
    double dop15;  //  Полученная тепл.нагрузка на техн. из под.труб.в откр.сист.
    double dop16;  //  Полученная тепл.нагрузка на техн. из обр.труб.в откр. сист.
    double dop17;  //  Полученная тепл.нагрузка на ГВ в закрытой системе
    double dop18;  //  Полученная тепл.нагрузка на ГВ из под.труб.в откр.сист.
    double dop19;  //  Полученная тепл.нагрузка на ГВ из обр.труб.в откр.сист.
    double dop20;  //  Полученная тепл.нагрузка в цирк.трубопроводе ГВС
    double Qsum_z;  //  Суммарная тепл.нагрузка  в закрытой системе
    double dop22;  //  Сумм.тепл.нагрузка  из обр.трубопр.в откр.системе
    double dop23;  //  Сумм.тепл.нагрузка  из обр.трубопр.в откр.системе


    int n_a4, n_a5, n_a6, n_a7, n_a8, n_a9, n_a10, n_a11, n_a12, n_a13, n_a14, n_a15,
        n_a16, n_a17, n_a18, n_a19, n_a20, n_a21, n_a22, n_a23, n_Gco, n_Gsm, n_Gm,
        n_Gx, n_Ggws, n_t1, n_t2, n_t01, n_t02, n_t03, n_t2co, n_t2gws, n_tv, n_tg,
        n_dop8, n_dop9, n_Qotz, n_Qotn, n_dop12, n_dop13, n_dop14, n_dop15, n_dop16,
        n_dop17, n_dop18, n_dop19, n_dop20, n_Qsum_z, n_dop22, n_dop23;
};

double read_double_db(CAdoFile* ado, const char* q, const char* fn);



// Объем сети

void CGidrView::OnZap1()
{
    double V1 = 0, V2 = 0;
    CString q, q1, s, vyd;

    q.Format(

        "SELECT "
        "sum(POWER(diameterInternal/1000,2)*pipeSectLength*db*PI()/4) AS V "
        "FROM "
        "( "
        "SELECT "
        "hp.id, "
        "hp.diameterInternal,  "
        "hp.pipeSectLength, "
        "IIF(l.externalSignLineID=1, 2, 1) AS db "
        "FROM linesobj l "
        "JOIN heatPipeSections hp ON hp.lineID=l.id "
        "JOIN nodes n1 ON n1.id=l.nodeID1 "

        "WHERE l.removed = 0 AND n1.internalNodeID IS NULL $$"
        ") _TTT"

    );


    CAdoFile* ado = m_cxema->m_ado;

    q1 = q;
    s.Format("AND n1.fileID IN (%s)", m_cxema->m_par);
    q1.Replace("$$", s);

    V1 = read_double_db(ado, q1, "V");

    if (m_cxema->m_graph->isMark()) {
        create_vyd_line_table(ado, m_cxema->m_graph, vyd);
        s.Format("AND l.id IN (SELECT ID from %s)", vyd);
        q1 = q;
        q1.Replace("$$", s);

        V2 = read_double_db(ado, q1, "V");
    }


    CString str;

    if (V2 == 0)
        str.Format(_TR("Объем сети, м3\nПо системе в целом. : %lg"), V1);
    else
        str.Format(_TR("Объем сети, м3\nПо системе в целом. : %lg\nПо выделенному фрагменту : %lg"), V1, V2);

    AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);
}


// Длина трубопроводов


CString get_SelectLine(int typ, const char* tn, const CNode* parent = NULL);


void dlina(CGraph2* m_graph, double& lenP, double& lenO, double& lenP_m, double& lenO_m)
{
    lenP = 0; lenO = 0; lenP_m = 0; lenO_m = 0;

    CMapGraph::iterator pp = m_graph->map_node.begin();

    while (pp != m_graph->map_node.end()) {
        CNode2* p = pp->second;
        CLINE2* ll;
        for (ll = p->lines; ll; ll = next(ll)) {
            CLine2* l = bline(ll);
            if (IsBegin(ll) && !IsRezhim(ll) && l->line.typ == TIP_UT) {
                int n = 1;
                double dlinaP = 0, dlinaO = 0;

                if (l->line.nomP > 0) dlinaP = l->line.pod.dl;
                if (l->line.nomO > 0) dlinaO = l->line.obr.dl;

                lenP += dlinaP * n;
                lenO += dlinaO * n;

                if (l->line.isPjezo) {
                    lenP_m += dlinaP * n;
                    lenO_m += dlinaO * n;
                }
            }
        }
        pp++;
    }
}

void CGidrView::OnZap2()
{

    CString q1, q2, s, vyd;
    double lenP = 0, lenO = 0, lenP_m = 0, lenO_m = 0;
    /*

      dlina(m_cxema->m_graph, lenP, lenO, lenP_m, lenO_m);

      double len = lenP+lenO;
      double len_m = lenP_m+lenO_m;

      CString str;

      if (len_m == 0)
          str.Format(_TR("Длина теплопроводов, м\nПо системе в целом. : %.8lg  (подача : %.8lg, обратка : %.8lg) "), len, lenP, lenO);
      else
          str.Format(_TR("Длина теплопроводов, м\nПо системе в целом. : %.8lg  (подача : %.8lg, обратка : %.8lg) \nПо выделенному фрагменту : %.8lg  (подача : %.8lg, обратка : %.8lg) "), len, lenP, lenO, len_m, lenP_m, lenO_m);

    */

    CString q =

        "SELECT  "
        "sum(lenP) AS lenP, "
        "sum(lenO) AS lenO, "
        "sum(lenP+lenO) AS len "

        "FROM ( "

        "SELECT  "
        "hp.id,  "
        "n1.externalNodeName AS name1, "
        "n2.externalNodeName AS name2, "
        "hp.diameterInternal,   "
        "IIF(l.externalSignLineID IN (1,2,4), hp.pipeSectLength, 0) AS lenP, "
        "IIF(l.externalSignLineID IN (1,3,5), hp.pipeSectLength, 0) AS lenO "


        "FROM linesobj l  "
        "JOIN heatPipeSections hp ON hp.lineID=l.id  "
        "JOIN nodes n1 ON n1.id=l.nodeID1  "
        "JOIN nodes n2 ON n2.id=l.nodeID2 "

        "WHERE l.removed = 0 AND n1.internalNodeID IS NULL $$"
        ") _TTT "
        ;


    CAdoFile* ado = m_cxema->m_ado;

    q1 = q;
    s.Format("AND n1.fileID IN (%s)", m_cxema->m_par);
    q1.Replace("$$", s);

    lenP = read_double_db(ado, q1, "lenP");
    lenO = read_double_db(ado, q1, "lenO");


    if (m_cxema->m_graph->isMark()) {
        create_vyd_line_table(ado, m_cxema->m_graph, vyd);
        s.Format("AND l.id IN (SELECT ID from %s)", vyd);
        q1 = q;
        q1.Replace("$$", s);

        lenP_m = read_double_db(ado, q1, "lenP");
        lenO_m = read_double_db(ado, q1, "lenO");
    }


    CString str;

    double len_m = lenP_m + lenO_m;
    double len = lenP + lenO;

    if (len_m == 0)
        str.Format(_TR("Длина теплопроводов, м\nПо системе в целом. : %.8lg  (подача : %.8lg, обратка : %.8lg) "), len, lenP, lenO);
    else
        str.Format(_TR("Длина теплопроводов, м\nПо системе в целом. : %.8lg  (подача : %.8lg, обратка : %.8lg) \nПо выделенному фрагменту : %.8lg  (подача : %.8lg, обратка : %.8lg) "), len, lenP, lenO, len_m, lenP_m, lenO_m);

    AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);
}


#if 0

struct PT {
    void Init(CDbFile* m_dbf) {
        n_a4 = m_dbf->FieldNum("a4");  //  Полученный расход на отопление, зав.сх.
        n_a5 = m_dbf->FieldNum("a5");  //  Полученный расход на отопление, незав.сх.
        n_a6 = m_dbf->FieldNum("a6");  //  Полученный расход на вентиляцию
        n_a7 = m_dbf->FieldNum("a7");  //  Полученный расход на кондиционирование
        n_a8 = m_dbf->FieldNum("a8");  //  Полученный расход на технологию в закр.сист.
        n_a9 = m_dbf->FieldNum("a9");  //  Полученный расход на техн. из под.трубопр. в откр сист.
        n_a10 = m_dbf->FieldNum("a10");  //  Полученный расход потреб.на техн. из обр.труб.в откр.сист.
        n_a11 = m_dbf->FieldNum("a11");  //  Полученный расход потребителя на ГВ в закр.системе
        n_a12 = m_dbf->FieldNum("a12");  //  Полученный расход потреб.на ГВ из под.труб.в откр.сист.
        n_a13 = m_dbf->FieldNum("a13");  //  Полученный расход потреб. на ГВ из обр.труб. в откр.сист.
        n_a14 = m_dbf->FieldNum("a14");  //  Полученный расход потреб.в цирк.трубопроводе ГВС
        n_a15 = m_dbf->FieldNum("a15");  //  Суммарный расход потребителя в закрытой системе
        n_a16 = m_dbf->FieldNum("a16");  //  Сумм.расход потребителя из под.трубопр. в откр.системе
        n_a17 = m_dbf->FieldNum("a17");  //  Сумм.расход потребителя из обр.трубопр. в откр.системе
        n_a18 = m_dbf->FieldNum("a18");  //  Относительный расход сетевой воды в закрытой системе
        n_a19 = m_dbf->FieldNum("a19");  //  Заданное значение утечки из подающего трубопровода
        n_a20 = m_dbf->FieldNum("a20");  //  Заданное значение утечки из обратного трубопровода
        n_a21 = m_dbf->FieldNum("a21");  //  Пьезометрический напор в подающем трубопроводе
        n_a22 = m_dbf->FieldNum("a22");  //  Пьезометрический напор в обратном трубопроводе
        n_a23 = m_dbf->FieldNum("a23");  //  Располагаемый напор в узле присоед.потребителя
        n_Gco = m_dbf->FieldNum("Gco");  //  Расход воды на входе СО
        n_Gsm = m_dbf->FieldNum("Gsm");  //  Расход воды в СО после узла смешения
        n_Gm = m_dbf->FieldNum("Gm");  //  Расход воды на входе местной СО, незав.присоед.
        n_Gx = m_dbf->FieldNum("Gx");  //  Расход водопроводной воды в закрытой ГВС
        n_Ggws = m_dbf->FieldNum("Ggws");  //  Расход сет.воды на вторую ступень закрытой ГВС
        n_t1 = m_dbf->FieldNum("t1");  //  Температура сет.воды на входе теплового пункта
        n_t2 = m_dbf->FieldNum("t2");  //  Температура сет.воды на выходе теплового пункта
        n_t01 = m_dbf->FieldNum("t01");  //  Температура в подающем трубопроводе СО
        n_t02 = m_dbf->FieldNum("t02");  //  Температура в обратном трубопроводе СО
        n_t03 = m_dbf->FieldNum("t03");  //  Температура после узла смешения в СО
        n_t2co = m_dbf->FieldNum("t2co");  //  Температура сет.воды на выходе подогревателя СО
        n_t2gws = m_dbf->FieldNum("t2gws");  //  Температура сет.воды на вых.подогревателя 1 ст.ГВС
        n_tv = m_dbf->FieldNum("tv");  //  Температура внутри отапливаемых помещений
        n_tg = m_dbf->FieldNum("tg");  //  Горячая вода на выходе водоразб.приборов в ГВС
        n_dop8 = m_dbf->FieldNum("dop8");  //  Темп.нагретого воздуха в системе вентиляции
        n_dop9 = m_dbf->FieldNum("dop9");  //  Темп. нагретого воздуха в системе кондиционирования
        n_Qotz = m_dbf->FieldNum("Qotz");  //  Полученная тепл.нагрузка  на отопление, зав.сх.
        n_Qotn = m_dbf->FieldNum("Qotn");  //  Полученная тепл.нагрузка  на отопление, незав.сх.
        n_dop12 = m_dbf->FieldNum("dop12");  //  Полученная тепловая нагрузка потребителя на вентиляцию
        n_dop13 = m_dbf->FieldNum("dop13");  //  Полученная тепл.нагрузка на кондиционирование
        n_dop14 = m_dbf->FieldNum("dop14");  //  Получ.тепл.нагрузка на технологию в закрытой системе
        n_dop15 = m_dbf->FieldNum("dop15");  //  Полученная тепл.нагрузка на техн. из под.труб.в откр.сист.
        n_dop16 = m_dbf->FieldNum("dop16");  //  Полученная тепл.нагрузка на техн. из обр.труб.в откр. сист.
        n_dop17 = m_dbf->FieldNum("dop17");  //  Полученная тепл.нагрузка на ГВ в закрытой системе
        n_dop18 = m_dbf->FieldNum("dop18");  //  Полученная тепл.нагрузка на ГВ из под.труб.в откр.сист.
        n_dop19 = m_dbf->FieldNum("dop19");  //  Полученная тепл.нагрузка на ГВ из обр.труб.в откр.сист.
        n_dop20 = m_dbf->FieldNum("dop20");  //  Полученная тепл.нагрузка в цирк.трубопроводе ГВС
        n_Qsum_z = m_dbf->FieldNum("Qsum_z");  //  Суммарная тепл.нагрузка  в закрытой системе
        n_dop22 = m_dbf->FieldNum("dop22");  //  Сумм.тепл.нагрузка  из обр.трубопр.в откр.системе
        n_dop23 = m_dbf->FieldNum("dop23");  //  Сумм.тепл.нагрузка  из обр.трубопр.в откр.системе
    }

    void read(CDbFile* m_dbf) {
        m_dbf->read(n_a4, a4);  //  Полученный расход на отопление, зав.сх.
        m_dbf->read(n_a5, a5);  //  Полученный расход на отопление, незав.сх.
        m_dbf->read(n_a6, a6);  //  Полученный расход на вентиляцию
        m_dbf->read(n_a7, a7);  //  Полученный расход на кондиционирование
        m_dbf->read(n_a8, a8);  //  Полученный расход на технологию в закр.сист.
        m_dbf->read(n_a9, a9);  //  Полученный расход на техн. из под.трубопр. в откр сист.
        m_dbf->read(n_a10, a10);  //  Полученный расход потреб.на техн. из обр.труб.в откр.сист.
        m_dbf->read(n_a11, a11);  //  Полученный расход потребителя на ГВ в закр.системе
        m_dbf->read(n_a12, a12);  //  Полученный расход потреб.на ГВ из под.труб.в откр.сист.
        m_dbf->read(n_a13, a13);  //  Полученный расход потреб. на ГВ из обр.труб. в откр.сист.
        m_dbf->read(n_a14, a14);  //  Полученный расход потреб.в цирк.трубопроводе ГВС
        m_dbf->read(n_a15, a15);  //  Суммарный расход потребителя в закрытой системе
        m_dbf->read(n_a16, a16);  //  Сумм.расход потребителя из под.трубопр. в откр.системе
        m_dbf->read(n_a17, a17);  //  Сумм.расход потребителя из обр.трубопр. в откр.системе
        m_dbf->read(n_a18, a18);  //  Относительный расход сетевой воды в закрытой системе
        m_dbf->read(n_a19, a19);  //  Заданное значение утечки из подающего трубопровода
        m_dbf->read(n_a20, a20);  //  Заданное значение утечки из обратного трубопровода
        m_dbf->read(n_a21, a21);  //  Пьезометрический напор в подающем трубопроводе
        m_dbf->read(n_a22, a22);  //  Пьезометрический напор в обратном трубопроводе
        m_dbf->read(n_a23, a23);  //  Располагаемый напор в узле присоед.потребителя
        m_dbf->read(n_Gco, Gco);  //  Расход воды на входе СО
        m_dbf->read(n_Gsm, Gsm);  //  Расход воды в СО после узла смешения
        m_dbf->read(n_Gm, Gm);  //  Расход воды на входе местной СО, незав.присоед.
        m_dbf->read(n_Gx, Gx);  //  Расход водопроводной воды в закрытой ГВС
        m_dbf->read(n_Ggws, Ggws);  //  Расход сет.воды на вторую ступень закрытой ГВС
        m_dbf->read(n_t1, t1);  //  Температура сет.воды на входе теплового пункта
        m_dbf->read(n_t2, t2);  //  Температура сет.воды на выходе теплового пункта
        m_dbf->read(n_t01, t01);  //  Температура в подающем трубопроводе СО
        m_dbf->read(n_t02, t02);  //  Температура в обратном трубопроводе СО
        m_dbf->read(n_t03, t03);  //  Температура после узла смешения в СО
        m_dbf->read(n_t2co, t2co);  //  Температура сет.воды на выходе подогревателя СО
        m_dbf->read(n_t2gws, t2gws);  //  Температура сет.воды на вых.подогревателя 1 ст.ГВС
        m_dbf->read(n_tv, tv);  //  Температура внутри отапливаемых помещений
        m_dbf->read(n_tg, tg);  //  Горячая вода на выходе водоразб.приборов в ГВС
        m_dbf->read(n_dop8, dop8);  //  Темп.нагретого воздуха в системе вентиляции
        m_dbf->read(n_dop9, dop9);  //  Темп. нагретого воздуха в системе кондиционирования
        m_dbf->read(n_Qotz, Qotz);  //  Полученная тепл.нагрузка  на отопление, зав.сх.
        m_dbf->read(n_Qotn, Qotn);  //  Полученная тепл.нагрузка  на отопление, незав.сх.
        m_dbf->read(n_dop12, dop12);  //  Полученная тепловая нагрузка потребителя на вентиляцию
        m_dbf->read(n_dop13, dop13);  //  Полученная тепл.нагрузка на кондиционирование
        m_dbf->read(n_dop14, dop14);  //  Получ.тепл.нагрузка на технологию в закрытой системе
        m_dbf->read(n_dop15, dop15);  //  Полученная тепл.нагрузка на техн. из под.труб.в откр.сист.
        m_dbf->read(n_dop16, dop16);  //  Полученная тепл.нагрузка на техн. из обр.труб.в откр. сист.
        m_dbf->read(n_dop17, dop17);  //  Полученная тепл.нагрузка на ГВ в закрытой системе
        m_dbf->read(n_dop18, dop18);  //  Полученная тепл.нагрузка на ГВ из под.труб.в откр.сист.
        m_dbf->read(n_dop19, dop19);  //  Полученная тепл.нагрузка на ГВ из обр.труб.в откр.сист.
        m_dbf->read(n_dop20, dop20);  //  Полученная тепл.нагрузка в цирк.трубопроводе ГВС
        m_dbf->read(n_Qsum_z, Qsum_z);  //  Суммарная тепл.нагрузка  в закрытой системе
        m_dbf->read(n_dop22, dop22);  //  Сумм.тепл.нагрузка  из обр.трубопр.в откр.системе
        m_dbf->read(n_dop23, dop23);  //  Сумм.тепл.нагрузка  из обр.трубопр.в откр.системе
    }


    double a4;  //  Полученный расход на отопление, зав.сх.
    double a5;  //  Полученный расход на отопление, незав.сх.
    double a6;  //  Полученный расход на вентиляцию
    double a7;  //  Полученный расход на кондиционирование
    double a8;  //  Полученный расход на технологию в закр.сист.
    double a9;  //  Полученный расход на техн. из под.трубопр. в откр сист.
    double a10;  //  Полученный расход потреб.на техн. из обр.труб.в откр.сист.
    double a11;  //  Полученный расход потребителя на ГВ в закр.системе
    double a12;  //  Полученный расход потреб.на ГВ из под.труб.в откр.сист.
    double a13;  //  Полученный расход потреб. на ГВ из обр.труб. в откр.сист.
    double a14;  //  Полученный расход потреб.в цирк.трубопроводе ГВС
    double a15;  //  Суммарный расход потребителя в закрытой системе
    double a16;  //  Сумм.расход потребителя из под.трубопр. в откр.системе
    double a17;  //  Сумм.расход потребителя из обр.трубопр. в откр.системе
    double a18;  //  Относительный расход сетевой воды в закрытой системе
    double a19;  //  Заданное значение утечки из подающего трубопровода
    double a20;  //  Заданное значение утечки из обратного трубопровода
    double a21;  //  Пьезометрический напор в подающем трубопроводе
    double a22;  //  Пьезометрический напор в обратном трубопроводе
    double a23;  //  Располагаемый напор в узле присоед.потребителя
    double Gco;  //  Расход воды на входе СО
    double Gsm;  //  Расход воды в СО после узла смешения
    double Gm;  //  Расход воды на входе местной СО, незав.присоед.
    double Gx;  //  Расход водопроводной воды в закрытой ГВС
    double Ggws;  //  Расход сет.воды на вторую ступень закрытой ГВС
    double t1;  //  Температура сет.воды на входе теплового пункта
    double t2;  //  Температура сет.воды на выходе теплового пункта
    double t01;  //  Температура в подающем трубопроводе СО
    double t02;  //  Температура в обратном трубопроводе СО
    double t03;  //  Температура после узла смешения в СО
    double t2co;  //  Температура сет.воды на выходе подогревателя СО
    double t2gws;  //  Температура сет.воды на вых.подогревателя 1 ст.ГВС
    double tv;  //  Температура внутри отапливаемых помещений
    double tg;  //  Горячая вода на выходе водоразб.приборов в ГВС
    double dop8;  //  Темп.нагретого воздуха в системе вентиляции
    double dop9;  //  Темп. нагретого воздуха в системе кондиционирования
    double Qotz;  //  Полученная тепл.нагрузка  на отопление, зав.сх.
    double Qotn;  //  Полученная тепл.нагрузка  на отопление, незав.сх.
    double dop12;  //  Полученная тепловая нагрузка потребителя на вентиляцию
    double dop13;  //  Полученная тепл.нагрузка на кондиционирование
    double dop14;  //  Получ.тепл.нагрузка на технологию в закрытой системе
    double dop15;  //  Полученная тепл.нагрузка на техн. из под.труб.в откр.сист.
    double dop16;  //  Полученная тепл.нагрузка на техн. из обр.труб.в откр. сист.
    double dop17;  //  Полученная тепл.нагрузка на ГВ в закрытой системе
    double dop18;  //  Полученная тепл.нагрузка на ГВ из под.труб.в откр.сист.
    double dop19;  //  Полученная тепл.нагрузка на ГВ из обр.труб.в откр.сист.
    double dop20;  //  Полученная тепл.нагрузка в цирк.трубопроводе ГВС
    double Qsum_z;  //  Суммарная тепл.нагрузка  в закрытой системе
    double dop22;  //  Сумм.тепл.нагрузка  из обр.трубопр.в откр.системе
    double dop23;  //  Сумм.тепл.нагрузка  из обр.трубопр.в откр.системе


    int n_a4, n_a5, n_a6, n_a7, n_a8, n_a9, n_a10, n_a11, n_a12, n_a13, n_a14, n_a15,
        n_a16, n_a17, n_a18, n_a19, n_a20, n_a21, n_a22, n_a23, n_Gco, n_Gsm, n_Gm,
        n_Gx, n_Ggws, n_t1, n_t2, n_t01, n_t02, n_t03, n_t2co, n_t2gws, n_tv, n_tg,
        n_dop8, n_dop9, n_Qotz, n_Qotn, n_dop12, n_dop13, n_dop14, n_dop15, n_dop16,
        n_dop17, n_dop18, n_dop19, n_dop20, n_Qsum_z, n_dop22, n_dop23;
};

#endif

/*
void CXEMA::ItPT(ITOG &itog) {

  for (n = 0; n < lst_n; n++) {
    a = lst_a[n];

    if (a) {
      for ( l = a->lines; l; l = l->next) {
        if (l->begin && l->uch->anal) {
          i = a->node->nom_g;
          if (IsPot(i) && i < l_np) {

      alfap=alfao=1;

      if (reg_r) {
        alfap=txop[i]+gvop[i];
        alfao=txoo[i]+gvoo[i];
        alfap=(alfap > 0.) ? qp_p[i]/alfap : alfap ;
        alfao=(alfao > 0.) ? qp_o[i]/alfao : alfao ;
      }

      alfa=otz[i]+otn[i]+vn[i]+kn[i]+txz[i]+gvz[i]+rez[i];

      if( s_rs[0].reg_r)
      {
        s=qp_z[i];
      }
      else s=alfa;

      alfa=(alfa > 0.) ? s/alfa : alfa ;
      if( svq[i] == -9999. || svq[i+l_np] == -9999.) alfa=0.;
      if( svq[i] == -9998. || svq[i+l_np] == -9998.) alfa=0.;
      kur = p_ur[i];


      v += otz[i]*alfa;
      q += otz[i]*alfa/tvur[kur].otoplz;

      itog.q_txop+=alfap*txop[i];
      itog.q_gvop+=alfap*gvop[i];
      itog.q_txoo+=alfao*txoo[i];
      itog.q_gvoo+=alfao*gvoo[i];

      itog.n_txop+=alfap*txop[i]/tvur[kur].txo;
      itog.n_gvop+=alfap*gvop[i]/tvur[kur].gvo;
      itog.n_txoo+=alfao*txoo[i]/tvur[kur].txo;
      itog.n_gvoo+=alfao*gvoo[i]/tvur[kur].gvo;

      itog.q_otz+=alfa*otz[i];
      itog.q_otn+=alfa*otn[i];
      itog.q_vn+=alfa*vn[i];
      itog.q_kn+=alfa*kn[i];
      itog.q_txz+=alfa*txz[i];
      itog.q_gvz+=alfa*gvz[i];
      itog.q_rez+=alfa*rez[i];

      itog.n_otz+=alfa*otz[i]/tvur[kur].otoplz;
      itog.n_otn+=alfa*otn[i]/tvur[kur].otopln;
      itog.n_vn+=alfa*vn[i]/tvur[kur].ventil;
      itog.n_kn+=alfa*kn[i]/tvur[kur].kondiz;
      itog.n_txz+=alfa*txz[i]/tvur[kur].txz;
      itog.n_gvz+=alfa*(gvps[i]/tvur[kur].gvps+gvpr[i]/tvur[kur].gvpr+gvsm[i]/tvur[kur].gvsm);
      itog.n_rez+=alfa*rez[i]/tvur[kur].rez;

          }
        }
      }
    }
  }

}
*/



CString getParOut(CAdoFile* ado, const char* par, const char* gid_user);


// Теплопотребление общее 

void Add(ITOG& itog, PT& pt)
{
    itog.n_otz += pt.Qotz;          /*  на отопление   Гкал           */
    itog.n_otn += pt.Qotn;          /*  на отопление                   */
    itog.n_vn += pt.dop12;           /*  на вентиляцию                  */
                                     //  itog.n_kn += pt.n_kn           /*  на кондиционирование           */
                                     //  itog.n_txz += pt.n_txz          /*  на технологию закрытую         */
                                     //  itog.n_txop += pt.n_txop         /*  технология откр. из подачи     */
                                     //  itog.n_txoo += pt.n_txoo         /*  технология откр. из обратки    */
    itog.n_gvop += pt.dop18;         /*  г.в. открытое из подачи        */
    itog.n_gvoo += pt.dop19;         /*  г.в. открытое из обратки       */
    itog.n_rez += pt.dop20;          /*  рециркуляцион. нагрузка откр.ГВ*/
    itog.n_gvz += pt.dop17;          /*  г.в. закрытое суммарное        */
                                     /*  расходы итоговые по расч.схеме */
    itog.q_otz += pt.a4;          /*  на отопление                   */
    itog.q_otn += pt.a5;          /*  на отопление                   */
    itog.q_vn += pt.a6;           /*  на вентиляцию                  */
                                  //  itog.q_kn += pt.q_kn           /*  на кондиционирование           */
                                  //  itog.q_txz += pt.q_txz          /*  на технологию закрытую         */
                                  //  itog.q_txop += pt.q_txop         /*  технология откр. из подачи     */
                                  //  itog.q_txoo += pt.q_txoo         /*  технология откр. из обратки    */
    itog.q_gvop += pt.a12;         /*  г.в. открытое из подачи        */
    itog.q_gvoo += pt.a13;         /*  г.в. открытое из обратки       */
    itog.q_rez += pt.a14;          /*  рециркуляционный расход откр.ГВ*/
    itog.q_gvz += pt.a15;          /*  г.в. закрытое суммарное        */
}



void CGidrView::OnZap3()
{
    if (!curFile() && m_parent_id == 0) {
        AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    ITOG itog, itog1;
    PT pt;

    CAdoFile* ado = m_cxema->m_ado;

    int fileID = m_cxema->m_cur_file->id;

    CString par;
    par.Format("%d", fileID);

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
    CString par2 = getParOut(ado, par, pDoc->m_user_gid);

    CString q;

    q.Format("SELECT * FROM PT_OUT WHERE calculationID IN (%s)", par2);
    q.Format("select n.id as n_id, PT_OUT.* from PT_OUT join nodes n ON n.id = PT_OUT.nodeID WHERE calculationID IN (%s)", par2);


    //  CDbFile *m_dbf = m_cxema->m_cur_file->m_dbf;

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            pt.Init(ado);

            while (!ado->isEOF()) {
                long id = ado->read_long("n_id");

                CNode2* n = m_cxema->m_graph->find(id);

                pt.read(ado);

                Add(itog, pt);
                if (n && isMarked(n)) {
                    Add(itog1, pt);
                }

                ado->MoveNext();
            }
        }
        ado->closeTable();
    }

    CTeplopotrBox box(this, itog, itog1, _TR("Итоговые значения параметров теплопотребления"));
    box.DoModal();
}


// Теплопотребление в закрытых системах

void CGidrView::OnZap4()
{
    if (!curFile() && m_parent_id == 0) {
        AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    ITOG itog, itog1;
    PT pt;

    CAdoFile* ado = m_cxema->m_ado;

    int fileID = m_cxema->m_cur_file->id;

    CString par;
    par.Format("%d", fileID);

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
    CString par2 = getParOut(ado, par, pDoc->m_user_gid);


    CString q;

    q.Format("SELECT * FROM PT_OUT WHERE calculationID IN (%s)", par2);
    q.Format("select n.id as n_id, PT_OUT.* from PT_OUT join nodes n ON n.id = PT_OUT.nodeID WHERE calculationID IN (%s)", par2);

    //  CDbFile *m_dbf = m_cxema->m_cur_file->m_dbf;

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            pt.Init(ado);

            while (!ado->isEOF()) {
                long id = ado->read_long("n_id");

                CNode2* n = m_cxema->m_graph->find(id);

                pt.read(ado);

                if (pt.a12 + pt.a13 == 0) {
                    Add(itog, pt);
                    if (n && isMarked(n)) {
                        Add(itog1, pt);
                    }
                }

                ado->MoveNext();
            }
        }
        ado->closeTable();
    }

    CTeplopotrBox box(this, itog, itog1, _TR("Итоговые значения параметров теплопотребления в закрытых системах"));
    box.DoModal();
}

// Теплопотребление в открытых системах

void CGidrView::OnZap5()
{
    if (!curFile() && m_parent_id == 0) {
        AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    ITOG itog, itog1;
    PT pt;

    CAdoFile* ado = m_cxema->m_ado;

    int fileID = m_cxema->m_cur_file->id;

    CString par;
    par.Format("%d", fileID);

    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();
    CString par2 = getParOut(ado, par, pDoc->m_user_gid);

    CString q;

    q.Format("SELECT * FROM PT_OUT WHERE calculationID IN (%s)", par2);
    q.Format("select n.id as n_id, PT_OUT.* from PT_OUT join nodes n ON n.id = PT_OUT.nodeID WHERE calculationID IN (%s)", par2);

    //  CDbFile *m_dbf = m_cxema->m_cur_file->m_dbf;

    if (ado->openTable0(q)) {
        if (!ado->isEOF()) {
            pt.Init(ado);

            while (!ado->isEOF()) {
                long id = ado->read_long("n_id");

                CNode2* n = m_cxema->m_graph->find(id);

                pt.read(ado);

                if (pt.a12 + pt.a13 > 0) {
                    Add(itog, pt);
                    if (n && isMarked(n)) {
                        Add(itog1, pt);
                    }
                }

                ado->MoveNext();
            }
        }
        ado->closeTable();
    }

    CTeplopotrBox box(this, itog, itog1, _TR("Итоговые значения параметров теплопотребления в открытых системах"));
    box.DoModal();
}

// Отключенные потребители




void CGidrView::OnZap6()
{
    CNode2* n;
    CString str;

    CMMenu menu(this, IDD_MENU2, _TR("Выберите закрытый потребитель"));

    CMapGraph::const_iterator p;
    p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* n = p->second;
        if (n->node.typ == TIP_PR || n->node.typ == TIP_PO) {
            if (n->node.isOtkl) {
                str = n->getName();
                menu.Add(str, (void*)n);
            }
        }
        p++;
    }


    if (menu.lst.GetSize() == 0) {
        AfxMessageBox(_TR("Нет закрытых потребителей"), MB_OK | MB_ICONINFORMATION);
    }
    else {
        if (menu.DoModal() == IDOK) {
            n = (CNode2*)menu.getV();
            if (n) {
                moveNode(n);
                BeginDrawNode(n);
            }
        }
    }
}

void CGidrView::OnPotNagr0()
{
    CNode2* n;
    CString str;

    CMMenu menu(this, IDD_MENU2, _TR("Выберите потребитель с нулевой нагрузкой"));

    CMapGraph::const_iterator p;
    p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* n = p->second;

        double Q = n->node.Qot + n->node.Qgvs + n->node.Qvent;

        if ((n->node.typ == TIP_PR || n->node.typ == TIP_PO) && Q == 0) {
            str = n->getName();
            menu.Add(str, (void*)n);
        }
        p++;
    }

    if (menu.lst.GetSize() == 0) {
        AfxMessageBox(_TR("Нет потребителей с нулевой нагрузкой"), MB_OK | MB_ICONINFORMATION);
    }
    else {
        if (menu.DoModal() == IDOK) {
            n = (CNode2*)menu.getV();
            if (n) {
                moveNode(n);
                BeginDrawNode(n);
            }
        }
    }
}

CString readQ(CAdoFile* ado, const char* tn, const char* tn2, const char* fileID);


void CGidrView::OnPotrTR()
{
    CAdoFile* ado = m_cxema->m_ado;
    CString q = readQ(ado, "find\\find_pot_tr", "", m_cxema->m_par);

    viewQ2(ado, ado->m_schema, q, "nodes", "", virt_data_node, "", "Потребители без ТР");


}

void CGidrView::OnPotrUR()
{
    CAdoFile* ado = m_cxema->m_ado;
    CString q = readQ(ado, "find\\find_pot_ur", "", m_cxema->m_par);

    viewQ2(ado, ado->m_schema, q, "nodes", "", virt_data_node, "", "Потребители без УР");

}


void CGidrView::OnPotrOtkl()
{
    CNode2* n;
    CMapGraph::const_iterator p;
    CMMenu menu(this, IDD_MENU2, _TR("Выберите отключенный потребитель по теплоносителю"));

    p = m_cxema->m_graph->map_node.begin();
    while (p != m_cxema->m_graph->map_node.end()) {
        const CNode2* nn = p->second;

        double Q = nn->node.Qot + nn->node.Qgvs + nn->node.Qvent;

        if ((nn->node.typ == TIP_PR || nn->node.typ == TIP_PO)
            && nn->node.qz == 0 && nn->node.qP == 0 && nn->node.qO == 0 && Q != 0) {
            CString s;
            s.Format("%s", nn->getName());
            menu.Add(s, (void*)nn->id);
        }
        p++;
    }

    if (menu.lst.GetSize() == 0) {
        AfxMessageBox(_TR("Нет отключенных потребителей по теплоносителю"), MB_OK | MB_ICONINFORMATION);
    }
    else {
        if (menu.DoModal() == IDOK) {
            int id = (int)menu.getV();

            n = m_cxema->m_graph->find(id);
            if (n) {
                moveXY(n->node.coord);
            }
        }
        Invalidate();
    }
}


// Длина трубопроводов по диаметрам

void CGidrView::OnZap7()
{
    CString q, vyd, s;
    q.Format(

        "SELECT  "

        "diameterCondit, "
        "sum(pipeSectLength*db) AS len "

        "FROM ( "
        "SELECT  "
        "hp.id,  "
        "hp.diameterCondit,   "
        "hp.pipeSectLength,  "
        "IIF(l.externalSignLineID=1, 2, 1) AS db, "
        "n1.fileID "
        "FROM linesobj l "
        "JOIN heatPipeSections hp ON hp.lineID=l.id "
        "JOIN nodes n1 ON n1.id=l.nodeID1 "

        "WHERE l.removed = 0 $$ AND n1.internalNodeID IS NULL"
        ") _TTT "
        "GROUP BY diameterCondit "
        "ORDER BY diameterCondit ");

    CAdoFile* ado = m_cxema->m_ado;


    if (m_cxema->m_graph->isMark()) {
        create_vyd_line_table(ado, m_cxema->m_graph, vyd);
        s.Format("AND l.id IN (SELECT ID from %s)", vyd);
        q.Replace("$$", s);
    }
    else {
        s.Format("AND n1.fileID IN (%s)", m_cxema->m_par);
        q.Replace("$$", s);
    }

    CMMenu menu(this, IDD_DIAMLEN, _TR("Длины теплопроводов"));

    double len_all = 0;

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            double diam = ado->read_double("diameterCondit");
            double len = ado->read_double("len");
            len_all += len;

            s.Format("%5lg \t\t%6lg", diam, len);
            menu.lst.Add(s);
            ado->MoveNext();
        }
        ado->closeTable();
    }

    s.Format(_TR("Всего\t\t%6lg"), len_all);
    menu.lst.Add(s);

    menu.DoModal();
}

void CGidrView::OnZapOtr()
{
    CLINE2* l;
    CMapGraph::iterator pp = m_cxema->m_graph->map_node.begin();

    while (pp != m_cxema->m_graph->map_node.end()) {
        CNode2* p = pp->second;
        if (p->node.data.pihP < p->node.data.pihO && p->node.data.pihP != 0 && p->node.data.pihO != 0) {
            p->node.isPjezo = 1;
            for (l = p->lines; l; l = next(l)) {
                bline(l)->line.isPjezo = 1;
            }
        }
        pp++;
    }
    Invalidate();
}




void CGidrView::OnZapVnCx()
{
    CMMenu menu(this, IDD_MENU2, _TR("Выберите узел"));

    CMapGraph::const_iterator pp = m_cxema->m_graph->map_node.begin();
    while (pp != m_cxema->m_graph->map_node.end()) {
        CNode2* p = pp->second;
        if (p->node.isP && (p->node.typ != TIP_PR && p->node.typ != TIP_PO)) {
            menu.Add(p->getName(), (void*)p->id);
        }
        pp++;
    }

    if (menu.DoModal() == IDOK) {
        int id = (int)menu.getV();

        CNode2* n = m_cxema->m_graph->find(id);
        if (n) {
            moveXYmas(n->node.coord);
        }
    }
    Invalidate();
}

CString getTime(double t)
{
    CString str;

    if (t > 1e70) return CString(_TR("Нет движения воды по выбранному маршруту"));

    t = fabs(t);

    t /= 60;

    int h, m, s;

    h = t;
    m = (t - h) * 60;
    s = ((t - h) * 60 - m) * 60;

    str.Format("%d часов %d минут %d секунд", h, m, s);

    return str;
}

void CGidrView::OnTimePr()
{
    deque<long> list_pjezo = m_cxema->m_graph->list_pjezo;

    if (list_pjezo.size() < 2) {
        AfxMessageBox(_TR("Маршрут не выбран"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    CNode2* n1, * n2;
    CLINE2* LO, * LP;
    CLine2* lP, * lO;
    double dlP = 0, dlO = 0;
    double timeP = 0, timeO = 0;

    deque<long>::iterator p;
    p = list_pjezo.begin();
    n1 = m_cxema->m_graph->find(*p);
    p++;

    int k = 0;

    while (p != list_pjezo.end()) {
        n2 = m_cxema->m_graph->find(*p);

        LP = m_cxema->m_graph->find_line(m_cxema->m_graph->findPO(n1->id, 'П'), m_cxema->m_graph->findPO(n2->id, 'П'), CPOD);
        LO = m_cxema->m_graph->find_line(m_cxema->m_graph->findPO(n1->id, 'О'), m_cxema->m_graph->findPO(n2->id, 'О'), COBR);

        if (LP) {
            lP = bline(LP);
            if (lP->line.pod.q * napr(LP) * timeP < 0) timeP = 1e80;
            else timeP += lP->line.pod.time1 * napr(LP);
        }
        if (LO) {
            lO = bline(LO);
            if (lO->line.obr.q * napr(LO) * timeO > 0) timeO = 1e80;
            else timeO += lO->line.obr.time1 * napr(LO);
        }

        n1 = m_cxema->m_graph->find(*p);
        p++;
    }

    CString str;

    str.Format(_TR("Время прохождения потока\nПодающий теплопровод : %s\nОбратный теплопровод : %s"), getTime(timeP), getTime(timeO));
    AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);

}

void CGidrView::OnPoteri()
{
    double v = 0, v1 = 0, vP, vO;

    CGraph2* graph = m_cxema->m_graph;
    CMapGraph::const_iterator pp = graph->map_node.begin();

    while (pp != graph->map_node.end()) {
        CNode2* p = pp->second;
        CLINE2* ll;
        CLine2* l;
        for (ll = p->lines; ll; ll = next(ll)) {
            l = bline(ll);
            if (IsBegin(ll) && l->line.typ == TIP_UT) {
                vP = l->line.pod.tpot;
                vO = l->line.obr.tpot;
                v += vP + vO;

                if (l->line.isPjezo) {
                    v1 += vP + vO;
                }
            }
        }
        pp++;
    }
    CString str;

    //  str.Format(_TR("ББ %g %g"), v, v1);

    if (v1 == 0)
        str.Format(_TR("Тепловые потери, Гкал/ч\nПо системе в целом. : %lg"), v);
    else
        str.Format(_TR("Тепловые потери, Гкал/ч\nПо системе в целом. : %lg\nПо выделенному фрагменту : %lg"), v, v1);

    AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);
}


void CGidrView::OnZapNullGeo()
{
    CMMenu menu(this, IDD_MENU2, _TR("Выберите узел"));

    CMapGraph::const_iterator pp = m_cxema->m_graph->map_node.begin();
    while (pp != m_cxema->m_graph->map_node.end()) {
        CNode2* p = pp->second;
        if (p->node.geoMarkTopTube == 0 && p->node.internalNodeID == m_parent_id) {
            menu.Add(p->getName(), (void*)p->id);
        }
        pp++;
    }

    if (menu.DoModal() == IDOK) {
        int id = (int)menu.getV();

        CNode2* n = m_cxema->m_graph->find(id);
        if (n) {
            moveXYmas(n->node.coord);
        }
    }
    Invalidate();
}


// Длина трубопроводов по диаметрам и способам прокладки

void CGidrView::OnZap7_1()
{

    CString q, vyd, s;
    q.Format(
        /*
        "SELECT  "

        "tubingTypeID, "
        "diameterCondit, "
        "sum(pipeSectLength*db) AS len "

        "FROM ( "
        "SELECT  "
        "hp.id,  "
        "hp.tubingTypeID, "
        "hp.diameterCondit,   "
        "hp.pipeSectLength,  "
        "IIF(l.externalSignLineID=1, 2, 1) AS db, "
        "n1.fileID "
        "FROM linesobj l "
        "JOIN heatPipeSections hp ON hp.lineID=l.id "
        "JOIN nodes n1 ON n1.id=l.nodeID1 "

        "WHERE l.removed = 0 $$"
        ") _TTT "
        "GROUP BY tubingTypeID,diameterCondit "
        "ORDER BY tubingTypeID,diameterCondit ");
        */

        "SELECT  "
        "diameterCondit,  "
        "sum(IIF(tubingTypeID=1, len, 0)) AS len1, "
        "sum(IIF(tubingTypeID=2, len, 0)) AS len2, "
        "sum(IIF(tubingTypeID=3, len, 0)) AS len3, "
        "sum(IIF(tubingTypeID=4, len, 0)) AS len4, "
        "sum(IIF(tubingTypeID=5, len, 0)) AS len5 "
        "  "
        "FROM ( "

        "SELECT   "

        "tubingTypeID,  "
        "diameterCondit,  "
        "sum(pipeSectLength*db) AS len "

        "FROM (  "
        "SELECT   "
        "hp.id,   "
        "hp.tubingTypeID,  "
        "hp.diameterCondit,    "
        "hp.pipeSectLength,   "
        "IIF(l.externalSignLineID=1, 2, 1) AS db,  "
        "n1.fileID  "
        "FROM linesobj l  "
        "JOIN heatPipeSections hp ON hp.lineID=l.id  "
        "JOIN nodes n1 ON n1.id=l.nodeID1  "

        "WHERE l.removed = 0 AND n1.internalNodeID IS NULL $$"
        ") _TTT  "
        "GROUP BY tubingTypeID,diameterCondit  "
        ") _TTT2 "

        "GROUP BY diameterCondit "
        "ORDER BY diameterCondit "
    );






    CAdoFile* ado = m_cxema->m_ado;


    if (m_cxema->m_graph->isMark()) {
        create_vyd_line_table(ado, m_cxema->m_graph, vyd);
        s.Format("AND l.id IN (SELECT ID from %s)", vyd);
        q.Replace("$$", s);
    }
    else {
        s.Format("AND n1.fileID IN (%s)", m_cxema->m_par);
        q.Replace("$$", s);
    }

    CMMenu menu(this, IDD_DIAMLEN1, _TR("Длины теплопроводов"));

    double len_all = 0;

    if (ado->openTable0(q)) {
        while (!ado->isEOF()) {
            double diam = ado->read_double("diameterCondit");
            double len1 = ado->read_double("len1");
            double len2 = ado->read_double("len2");
            double len3 = ado->read_double("len3");
            double len4 = ado->read_double("len4");
            double len5 = ado->read_double("len5");

            //    len_all += len;

            s.Format("%5lg\t%8lg\t%8lg\t%8lg\t%8lg", diam, len4, len2, len1, len3);
            menu.lst.Add(s);
            ado->MoveNext();
        }
        ado->closeTable();
    }


    menu.DoModal();

}

void CGidrView::OnZapNezak()
{
    /*

      CNodeListData *nd = new CNodeListData(m_cxema->m_graph, false);

      CVirtualDlg dlg(this, nd, -1, _TR("Список конвертированных узлов"), true);
      if (dlg.DoModal() == IDOK) {
        int id = dlg.m_id;
        CNode2 *n = m_cxema->m_graph->find(id);
        if (n) {
          moveXY(n->node.coord);
        }
      }
    */
    if (!isEditGid()) return;

    CMMenu menu(this, IDD_MENU2, "Список незаконченных узлов");
    CString str;
    CNode2* n;
    CLINE2* l;


    CMapGraph::const_iterator p = m_cxema->m_graph->map_node.begin();

    while (p != m_cxema->m_graph->map_node.end()) {
        CNode2* node = p->second;
        if (node && (node->node.coord.x != 0 || node->node.coord.y != 0)) {
            for (l = node->lines; l; l = next(l)) {
                n = other(l);
                if (n->node.coord.x == 0 && n->node.coord.y == 0) {
                    menu.Add(node->getName(), node);
                }
            }
        }
        p++;
    }

    if (menu.lst.GetSize() == 0) {
        AfxMessageBox("Все узлы отрисованы", MB_OK | MB_ICONINFORMATION);
        return;
    }
    if (menu.DoModal() == IDOK) {
        CNode2* n = (CNode2*)menu.getV();
        if (n) {
            moveNode(n);
        }
    }
}

//        MENUITEM _TR("Объект WinCC"),                ID_ZAP_ORP

void CGidrView::OnZapOrp()
{
    AfxMessageBox(_TR("Временно удалено 901"), MB_OK | MB_ICONINFORMATION);
}

//        MENUITEM _TR("Объект OPC"),                  ID_ZAP_IZM

void CGidrView::OnZapIzm()
{
    AfxMessageBox(_TR("Временно удалено 902"), MB_OK | MB_ICONINFORMATION);
}

void CGidrView::OnZapZavozd()
{
    CLINE2* l;
    CMapGraph::iterator pp = m_cxema->m_graph->map_node.begin();

    CMMenu menu(this, IDD_MENU2, _TR("Выберите узел"));


    while (pp != m_cxema->m_graph->map_node.end()) {
        CNode2* p = pp->second;
        if (p->node.typ == TIP_PR && !p->node.isNez && p->node.data.pihP < p->node.hz) {
            p->node.isPjezo = 1;

            menu.Add(p->getName(), (void*)p->id);

            for (l = p->lines; l; l = next(l)) {
                bline(l)->line.isPjezo = 1;
            }
        }
        pp++;
    }
    Invalidate();


    if (menu.DoModal() == IDOK) {
        int id = (int)menu.getV();

        CNode2* n = m_cxema->m_graph->find(id);
        if (n) {
            moveXYmas(n->node.coord);
        }
    }
}


#include "Ultimate Grid\TableDialog.h"


bool create_vyd_line_table_new_all(CAdoFile* ado, CGraph2* graph, CString& tn, int fileID);


void CGidrView::UtZakr(bool all, const char* title)
{
    if (!curFile() && m_parent_id == 0) {
        AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    CAdoFile* ado = m_cxema->m_ado;
    int fileID = m_cxema->m_cur_file->id;

    CString q;
    //    q.Format("SELECT * FROM heatPipeSections");
    int n_ret = 0;

    CString vyd;

    create_vyd_line_table_new_all(ado, m_cxema->m_graph, vyd, fileID);

    CString sq = "";

    if (all) {
        sq = "ut_out.id IS NULL OR";
    }


    q.Format(

        "SELECT top 2147483647 \n"
        "  l.id,\n"

        "  IIF(\n"
        //"    (ut_out.id IS NULL OR ut_out.externalSignLineID IN (1,2,4)) AND hps.pipeSectStateIDflow=2 AND\n"
        //"    (ut_out.id IS NULL OR ut_out.externalSignLineID IN (1,3,5)) AND hps.pipeSectStateIDret=2\n"


        "hps.pipeSectStateIDflow = 2 AND(ut_out.id IS NULL AND l.externalSignLineID IN(1, 2, 4))\n"
        "OR\n"
        "hps.pipeSectStateIDRet = 2 AND(ut_out.id IS NULL AND l.externalSignLineID IN(1, 3, 5))\n"



        ",     'закр', ''\n"
        "  ) AS 'Состояние',\n"

        "  ec1.name as 'Код РС начального узла', n1.externalNodeName as 'Наименование начального узла', \n"
        "  case IIF(ut_out.id IS NULL, l.externalSignLineID, ut_out.externalSignLineID)    when 1 then ' '    when 2 then 'П'    when 3 then 'О'    when 4 then 'П'    when 5 then 'О'  end as 'Признак нач.узла',\n"
        "  ec2.name as 'Код РС конечного узла', n2.externalNodeName as 'Наименование конечного узла',\n"
        "  case IIF(ut_out.id IS NULL, l.externalSignLineID, ut_out.externalSignLineID)    when 1 then ' '    when 2 then 'П'    when 3 then 'О'    when 4 then 'О'    when 5 then 'П'  end as 'Признак кон.узла',\n"

        "hps.pipeSectLength as 'Протяженность,м',\n"
        "hps.diameterInternal as 'Диаметр внутренний,мм',\n"

        "hs.sourceName AS 'Источник'\n,"
        "org.name AS 'Владелец'\n"

        ", row_number() OVER (ORDER  BY VYD.ID1)  AS '№'"



        " \n"
        "from linesobj l\n"
        "left join UT_OUT on l.id = ut_out.lineID\n"
        "join heatPipeSections hps ON hps.lineID=l.id\n"
        "join nodes n1 on n1.id = l.nodeID1\n"
        "join nodes n2 on n2.id = l.nodeID2\n"
        "join externalCodes ec1 on ec1.id = n1.externalCodeID\n"
        "join externalCodes ec2 on ec2.id = n2.externalCodeID\n"
        "left join nodes ni on ni.id = n1.internalNodeID\n"
        "left join externalCodes eci on eci.id = ni.externalCodeID\n"
        "left join externalSigns esi on esi.id = ni.externalSignID\n"
        "LEFT JOIN heatSources hs ON hs.id = ec1.heatSourceID\n"
        "left join organizations org on org.id = l.organizationID\n"
        "JOIN %s vyd ON vyd.id2=l.id\n"

        "WHERE n1.fileID=%d AND calculationID IS NULL\n"

        "AND n1.internalNodeID IS NULL\n"

        "AND \n"
        "(\n"
        //"ut_out.id IS NULL OR\n"
        "%s\n"

        "(hps.pipeSectStateIDflow=2 AND (ut_out.externalSignLineID IN (1,2,4) OR ut_out.id IS NULL)\n"
        "OR hps.pipeSectStateIDret=2 AND (ut_out.externalSignLineID IN (1,3,5) OR ut_out.id IS NULL)\n"
        ")\n"
        ")\n"
        "AND l.removed=0\n"
        "ORDER BY vyd.id1"
        //+7 705 199 70 50
        , vyd, fileID, sq);

    CTableDialog dlg(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), title, q, n_ret, virt_data_unknown);
    int ret = dlg.DoModal();
    if (ret == IDOK) {
        moveLine(dlg.m_id);
    }
}


void CGidrView::OnUtZakr()
{
    UtZakr(false, "Закрытые участки");
}

void CGidrView::OnUtZakrAll()
{
    UtZakr(true, "Отключенные участки");
}



bool getTG(long hSourceID, double tn, double& t1, double& t2, double& t3, double& tv);


void CGidrView::OnPtTempMin()
{
    CString str = AfxGetApp()->GetProfileString(szSection, "Tn", "-32");
    double Tn = atof(str);
    double t1, t2 = 0, t3, tv;

    bool readTG(CAdoFile * ado);

    CAdoFile* ado = m_cxema->m_ado;

    readTG(ado);

    CLINE2* l;
    CMapGraph::iterator pp = m_cxema->m_graph->map_node.begin();

    CMMenu menu(this, IDD_MENU2, _TR("Выберите потребитель с низкими температурами"));

    while (pp != m_cxema->m_graph->map_node.end()) {
        CNode2* p = pp->second;

        const RasCx* cx = m_cxema->getPC(p->node.id_kod);

        if (cx) {
            getTG(cx->heatSourceID, Tn, t1, t2, t3, tv);
        }

        double t1_ = p->node.data.tempP;

        if ((p->node.typ == TIP_PR || p->node.typ == TIP_PO) && t1_ < t2) {

            double Q = p->node.Qot + p->node.Qgvs + p->node.Qvent;

            if (!(p->node.qz == 0 && p->node.qP == 0 && p->node.qO == 0 && Q != 0)) {
                p->node.isPjezo = 1;
                CString str = p->getName();
                menu.Add(str, (void*)p);

                for (l = p->lines; l; l = next(l)) {
                    bline(l)->line.isPjezo = 1;
                }
            }
        }
        pp++;
    }
    if (menu.lst.GetSize() == 0) {
        AfxMessageBox(_TR("Нет потребителей с низкими температурами"), MB_OK | MB_ICONINFORMATION);
    }
    else {
        if (menu.DoModal() == IDOK) {
            CNode2 *n = (CNode2*)menu.getV();
            if (n) {
                moveNode(n);
                BeginDrawNode(n);
            }
        }
    }



    Invalidate();
}


void Run(CWnd * win, const char* cmd, const char* prtn, int id);
CString python_str(const CString& prog, const CString& args);

void CGidrView::OnZapNewDoc()
{

    CString cmd, args;
    CString tmpName;
    Cgid6Doc* pDoc = (Cgid6Doc*)GetDocument();

    cmd.Format("\"%s\" \"%s\\_new\\w.py\" -rdbms MsSql -server %s -database %s -port %d -user %s -password %s  -run  -fragments %s", 
        python_exe(), argpath(), pDoc->m_IP, pDoc->m_bd_gid, pDoc->m_port, pDoc->m_user, pDoc->m_password, m_cxema->m_par);

    args.Format("-rdbms MsSql -server %s -database %s -port %d -user %s -password %s  -run  -fragments %s", 
        pDoc->m_IP, pDoc->m_bd_gid, pDoc->m_port, pDoc->m_user, pDoc->m_password, m_cxema->m_par);


    cmd = python_str("_new\\w.py", args);

//    CInputDialog inD(AfxGetMainWnd(), "", "?", cmd);
//    inD.DoModal();

    log1(cmd);
    Run(this, cmd, argpath_2() + "protocol.txt", ID_END_PYTHON);
}

