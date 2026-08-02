#include "property/PropertyDial.h"
#include <QApplication>
#include <QtGui>
#include <QInputDialog>

#include <mainwindow.h>
#include <gidview/GidWidget.h>
#include <db/db.h>
#include <dialog/MMenuDial.h>
#include <dialog/TableWindow.h>

QString getQForMarkLine(CGraph2 * graph, bool br);


void GidWidget::onZap1() // Объем сети
{
    double V1 = 0, V2 = 0;
    QString vyd;

    QString q = QString(

        "SELECT "
        "sum(POWER(diameterInternal/1000,2)*pipeSectLength*db*PI()/4) AS V "
        "FROM "
        "( "
        "SELECT "
        "hp.id, "
        "hp.diameterInternal,  "
        "hp.pipeSectLength, "
        "IIF(l.externalSignLineID=1, 2, 1) AS db "
        "FROM net.v_linesobj l "
        "JOIN heatPipeSections hp ON hp.lineID=l.id "
        "JOIN net.v_nodes n1 ON n1.id=l.nodeID1 "
        " @@ "
        "WHERE l.removed = 0 AND n1.internalNodeID IS NULL $$"
        ") _TTT"

    );


//    CAdoFile* ado = m_cxema->m_ado;

    QString q1 = q;
    QString s = QString("AND n1.fileID IN (%1)").arg(m_cxema.m_par);
    q1.replace("$$", s);
    q1.replace("@@", "");

    bool ok;
    V1 = read_double_db(m_cxema.m_db, q1, "V", &ok);

    if (m_cxema.m_graph->isMark()) {
        QString q1 = q;
        QString p1 = getQForMarkLine(m_cxema.m_graph, true);
        QString pp = QString("join (values %1) t(id) on t.id=l.id").arg(p1);
        q1.replace("@@", pp);
        q1.replace("$$", "");
        V2 = read_double_db(m_cxema.m_db, q1, "V", &ok);
    }


    QString str;

    if (V2 == 0)
        str = QString(tr("Объем сети, м3\nПо системе в целом. : %1")).arg(V1);
    else
        str = QString(tr("Объем сети, м3\nПо системе в целом. : %1\nПо выделенному фрагменту : %2")).arg(V1).arg(V2);

    QMessageBox::warning(this, "", str);
}
    

void GidWidget::onZap2() // Длина теплопроводов
{
    QString q1, q2, s, vyd;
    double lenP = 0, lenO = 0, lenP_m = 0, lenO_m = 0;

    QString q =

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


        "FROM net.v_linesobj l  "
        "JOIN heatPipeSections hp ON hp.lineID=l.id  "
        "JOIN net.v_nodes n1 ON n1.id=l.nodeID1  "
        "JOIN net.v_nodes n2 ON n2.id=l.nodeID2 "
        " @@ "

        "WHERE l.removed = 0 AND n1.internalNodeID IS NULL $$"
        ") _TTT "
        ;


    q1 = q;
    q1.replace("$$", QString("AND n1.fileID IN (%1)").arg(m_cxema.m_par));
    q1.replace("@@", "");

    bool ok;

    lenP = read_double_db(m_cxema.m_db, q1, "lenP", &ok);
    lenO = read_double_db(m_cxema.m_db, q1, "lenO", &ok);

    if (m_cxema.m_graph->isMark()) {
        QString q1 = q;
        QString p1 = getQForMarkLine(m_cxema.m_graph, true);
        QString pp = QString("join (values %1) t(id) on t.id=l.id").arg(p1);
        q1.replace("@@", pp);
        q1.replace("$$", "");

        
        lenP_m = read_double_db(m_cxema.m_db, q1, "lenP", &ok);
        lenO_m = read_double_db(m_cxema.m_db, q1, "lenO", &ok);
    }


    QString str;

    double len_m = lenP_m + lenO_m;
    double len = lenP + lenO;

    if (len_m == 0)
        str = QString(tr("Длина теплопроводов, м\nПо системе в целом. : %1  (подача : %2, обратка : %3) ")).arg(len).arg(lenP).arg(lenO);
    else
        str = QString(tr("Длина теплопроводов, м\nПо системе в целом. : %1  (подача : %2, обратка : %3) \nПо выделенному фрагменту : %4  (подача : %5, обратка : %6) ")).arg(len).arg(lenP).arg(lenO).arg(len_m).arg(lenP_m).arg(lenO_m);

    QMessageBox::warning(this, "", str);
}
    

void GidWidget::onZap7() // Длина теплопроводов по диаметрам
{
    QString vyd, s;
    QString q = QString(

//        "SELECT TOP 100000 "
        "SELECT "
        "ROW_NUMBER() OVER (ORDER BY diameterCondit) AS ID, "
        "diameterCondit, "
        "sum(pipeSectLength*db) AS len "

        "FROM ( "
        "SELECT  "
        "hp.id,  "
        "hp.diameterCondit,   "
        "hp.pipeSectLength,  "
        "IIF(l.externalSignLineID=1, 2, 1) AS db, "
        "n1.fileID "
        "FROM net.v_linesobj l "
        "JOIN heatPipeSections hp ON hp.lineID=l.id "
        "JOIN net.v_nodes n1 ON n1.id=l.nodeID1 "
        " @@ "

        "WHERE l.removed = 0 $$ AND n1.internalNodeID IS NULL"
        ") _TTT "
        "GROUP BY diameterCondit "
        "ORDER BY diameterCondit ");

    if (m_cxema.m_graph->isMark()) {
        QString p1 = getQForMarkLine(m_cxema.m_graph, true);
        QString pp = QString("join (values %1) t(id) on t.id=l.id").arg(p1);
        q.replace("@@", pp);
        q.replace("$$", "");
    }
    else {
        q.replace("$$", QString("AND n1.fileID IN (%1)").arg(m_cxema.m_par));
        q.replace("@@", "");
    }

//    QInputDialog::getMultiLineText(nullptr, "title", "label", q);


    TableWindow * tab = new TableWindow(m_cxema.m_db, "zap7", "Длины теплопроводов по диаметрам, м", q, this);
    tab->show();


#if 0

    MMenuDial menu(this, tr("Длины теплопроводов"));

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

    s = QString(tr("Всего\t\t%1")).arg(len_all);
    menu.lst.Add(s);

    menu.DoModal();
#endif
}
    

void GidWidget::onZap71() // Длина теплопроводов по диаметрам и способам прокладки
{
    QString vyd, s;
    QString q = QString(

//        "SELECT TOP 1000000 "
        "SELECT "
        "ROW_NUMBER() OVER (ORDER BY diameterCondit) AS ID, "
        "diameterCondit,  "
        "sum(IIF(tubingTypeID=1, len, 0)) AS K, "
        "sum(IIF(tubingTypeID=2, len, 0)) AS B, "
        "sum(IIF(tubingTypeID=4, len, 0)) AS N, "
        "sum(IIF(tubingTypeID=3, len, 0)) AS P, "
        "sum(IIF(tubingTypeID=5, len, 0)) AS O "
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
        "FROM net.v_linesobj l  "
        "JOIN heatPipeSections hp ON hp.lineID=l.id  "
        "JOIN net.v_nodes n1 ON n1.id=l.nodeID1  "
        " @@ "

        "WHERE l.removed = 0 AND n1.internalNodeID IS NULL $$"
        ") _TTT  "
        "GROUP BY tubingTypeID,diameterCondit  "
        ") _TTT2 "

        "GROUP BY diameterCondit "
        "ORDER BY diameterCondit "
    );

    if (m_cxema.m_graph->isMark()) {
        QString p1 = getQForMarkLine(m_cxema.m_graph, true);
        QString pp = QString("join (values %1) t(id) on t.id=l.id").arg(p1);
        q.replace("@@", pp);
        q.replace("$$", "");
    }
    else {
        q.replace("@@", "");
        q.replace("$$", QString("AND n1.fileID IN (%1)").arg(m_cxema.m_par));
    }

//    QInputDialog::getMultiLineText(nullptr, "title", "label", q);
    TableWindow * tab = new TableWindow(m_cxema.m_db, "zap7_1", "Длины теплопроводов по диаметрам и способам прокладки, м", q, this);
    tab->show();
}
    
QString getTime(double t)
{
    if (t > 1e70) return QString(QObject::tr("Нет движения воды по выбранному маршруту"));

    t = fabs(t);

    t /= 60;

    int h, m, s;

    h = t;
    m = (t - h) * 60;
    s = ((t - h) * 60 - m) * 60;

    return QString("%1 часов %2 минут %3 секунд").arg(h).arg(m).arg(s);
}


void GidWidget::onTimePr() // Время прохождения
{
    std::deque<long> list_pjezo = m_cxema.m_graph->list_pjezo;

    if (list_pjezo.size() < 2) {
        QMessageBox::warning(this, "", tr("Маршрут не выбран"));
        return;
    }

    double timeP = 0, timeO = 0;

    std::deque<long>::iterator p;
    p = list_pjezo.begin();
    CNode2 *n1 = m_cxema.m_graph->find(*p);
    p++;

    int k = 0;

    while (p != list_pjezo.end()) {
        CNode2 *n2 = m_cxema.m_graph->find(*p);

        CLINE2 *LP = m_cxema.m_graph->find_line(m_cxema.m_graph->findPO(n1->id, CPOD), m_cxema.m_graph->findPO(n2->id, CPOD), CPOD);
        CLINE2 *LO = m_cxema.m_graph->find_line(m_cxema.m_graph->findPO(n1->id, COBR), m_cxema.m_graph->findPO(n2->id, COBR), COBR);

        if (LP) {
            CLine2 *lP = bline(LP);
            if (lP->line.pod.q * napr(LP) * timeP < 0) timeP = 1e80;
            else timeP += lP->line.pod.time1 * napr(LP);
        }
        if (LO) {
            CLine2 *lO = bline(LO);
            if (lO->line.obr.q * napr(LO) * timeO > 0) timeO = 1e80;
            else timeO += lO->line.obr.time1 * napr(LO);
        }

        n1 = m_cxema.m_graph->find(*p);
        p++;
    }

    QString str = QString(tr("Время прохождения потока\nПодающий теплопровод : %1\nОбратный теплопровод : %2")).arg(getTime(timeP)).arg(getTime(timeO));
    QMessageBox::warning(this, "", str);
}
    

void GidWidget::onPoteri() // Тепловые потери
{
    double v = 0, v1 = 0, vP, vO;

    CGraph2* graph = m_cxema.m_graph;
    auto pp = graph->map_node.begin();

    while (pp != graph->map_node.end()) {
        CNode2* p = pp->second;
        CLINE2* ll;
        CLine2* l;
        for (ll = p->lines; ll; ll = nextl(ll)) {
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

    QString str;

    if (v1 == 0)
        str = QString(tr("Тепловые потери, Гкал/ч\nПо системе в целом. : %1")).arg(v);
    else
        str = QString(tr("Тепловые потери, Гкал/ч\nПо системе в целом. : %1\nПо выделенному фрагменту : %2")).arg(v).arg(v1);

    QMessageBox::warning(this, "", str);
}
    

void GidWidget::onZap3() // Теплопотребление полученное
{
    QString q = QString(
    
    R"SQL(
SELECT 
--  id,
  1 AS id,
  sum(Qotz)  AS n_otz,   --  /*  на отопление                   */
  sum(Qotn)  AS n_otn,   --  /*  на отопление                   */
  sum(dop12) AS n_vn,    --  /*  на вентиляцию                  */
  sum(dop18) AS n_gvop,  --  /*  г.в. открытое из подачи        */
  sum(dop19) AS n_gvoo,  --  /*  г.в. открытое из обратки       */
  sum(dop20) AS n_rez,   --  /*  рециркуляцион. нагрузка откр.ГВ*/
  sum(dop17) AS n_gvz,   --  /*  г.в. закрытое суммарное        */
  sum(a4)    AS q_otz,   --  /*  на отопление                   */
  sum(a5)    AS q_otn,   --  /*  на отопление                   */
  sum(a6)    AS q_vn,    --  /*  на вентиляцию                  */
  sum(a12)   AS q_gvop,  --  /*  г.в. открытое из подачи        */
  sum(a13)   AS q_gvoo,  --  /*  г.в. открытое из обратки       */
  sum(a14)   AS q_rez,   --  /*  рециркуляционный расход откр.ГВ*/
  sum(a15)   AS q_gvz    --  /*  г.в. закрытое суммарное        */

FROM PT_OUT

JOIN net.v_nodes n ON n.id=PT_OUT.nodeID and n.removed=0
LEFT JOIN 
(
SELECT 
    c.fileID,
    max(c.id) AS cid
    FROM CALCULATION c
    LEFT JOIN fragments fr ON fr.id=c.fileID
    GROUP BY c.fileID
) calc ON calc.fileID=n.fileID and calc.cid=PT_OUT.calculationID
WHERE calc.fileID IN (%1)

 )SQL").arg(m_cxema.m_par);

//    QInputDialog::getMultiLineText(nullptr, "title", "label", q);



    std::map<QString, QVariant> map_val;

    if (readTableRowValuesMap(m_cxema.m_db, q, map_val)) {
        auto *dlg = new PropertyDial(this, "tp0", "", "shape", "id", -1, -1);

        dlg->init_dialog(&m_cxema.m_db, "Теплопотребление полученное, ГКал/ч", "tp0", map_val);

        dlg->show();
    }



/*    
    
    TableWindow * tab = new TableWindow(m_cxema.m_db, "zap3", "Теплопотребление полученное, ГКал/ч", q, this);
    tab->show();
*/ 

}
    

void GidWidget::onZap4() // Теплопотребление в закрытых системах полученное
{
    QString q = QString(
    
    R"SQL(
SELECT 
--  id,
  1 AS id,
  sum(Qotz)  AS n_otz,   --  /*  на отопление                   */
  sum(Qotn)  AS n_otn,   --  /*  на отопление                   */
  sum(dop12) AS n_vn,    --  /*  на вентиляцию                  */
  sum(dop18) AS n_gvop,  --  /*  г.в. открытое из подачи        */
  sum(dop19) AS n_gvoo,  --  /*  г.в. открытое из обратки       */
  sum(dop20) AS n_rez,   --  /*  рециркуляцион. нагрузка откр.ГВ*/
  sum(dop17) AS n_gvz,   --  /*  г.в. закрытое суммарное        */
  sum(a4)    AS q_otz,   --  /*  на отопление                   */
  sum(a5)    AS q_otn,   --  /*  на отопление                   */
  sum(a6)    AS q_vn,    --  /*  на вентиляцию                  */
--  sum(a12)   AS q_gvop,  --  /*  г.в. открытое из подачи        */
--  sum(a13)   AS q_gvoo,  --  /*  г.в. открытое из обратки       */
--  sum(a14)   AS q_rez,   --  /*  рециркуляционный расход откр.ГВ*/
  sum(a15)   AS q_gvz    --  /*  г.в. закрытое суммарное        */

FROM PT_OUT

JOIN net.v_nodes n ON n.id=PT_OUT.nodeID and n.removed=0
LEFT JOIN 
(
SELECT 
    c.fileID,
    max(c.id) AS cid
    FROM CALCULATION c
    LEFT JOIN fragments fr ON fr.id=c.fileID
    GROUP BY c.fileID
) calc ON calc.fileID=n.fileID and calc.cid=PT_OUT.calculationID
WHERE calc.fileID IN (%1)

 )SQL").arg(m_cxema.m_par);

#if 0
//    QInputDialog::getMultiLineText(nullptr, "title", "label", q);
    TableWindow * tab = new TableWindow(m_cxema.m_db, "zap3", "Теплопотребление в закрытых системах, полученное, ГКал/ч", q, this);
    tab->show();
#endif
    std::map<QString, QVariant> map_val;

    if (readTableRowValuesMap(m_cxema.m_db, q, map_val)) {
        auto *dlg = new PropertyDial(this, "tpZ", "", "shape", "id", -1, -1);

        dlg->init_dialog(&m_cxema.m_db, "Теплопотребление в закрытых системах полученное, ГКал/ч", "tpZ", map_val);

        dlg->show();
    }
}
    

void GidWidget::onZap5() // Теплопотребление в открытых системах полученное
{
    QString q = QString(
    
    R"SQL(
SELECT 
--  id,
  1 AS id,
  sum(Qotz)  AS n_otz,   --  /*  на отопление                   */
  sum(Qotn)  AS n_otn,   --  /*  на отопление                   */
  sum(dop12) AS n_vn,    --  /*  на вентиляцию                  */
  sum(dop18) AS n_gvop,  --  /*  г.в. открытое из подачи        */
  sum(dop19) AS n_gvoo,  --  /*  г.в. открытое из обратки       */
  sum(dop20) AS n_rez,   --  /*  рециркуляцион. нагрузка откр.ГВ*/
  sum(dop17) AS n_gvz,   --  /*  г.в. закрытое суммарное        */
  sum(a4)    AS q_otz,   --  /*  на отопление                   */
  sum(a5)    AS q_otn,   --  /*  на отопление                   */
  sum(a6)    AS q_vn,    --  /*  на вентиляцию                  */
  sum(a12)   AS q_gvop,  --  /*  г.в. открытое из подачи        */
  sum(a13)   AS q_gvoo,  --  /*  г.в. открытое из обратки       */
  sum(a14)   AS q_rez,   --  /*  рециркуляционный расход откр.ГВ*/
  sum(a15)   AS q_gvz    --  /*  г.в. закрытое суммарное        */

FROM PT_OUT

JOIN net.v_nodes n ON n.id=PT_OUT.nodeID and n.removed=0
LEFT JOIN 
(
SELECT 
    c.fileID,
    max(c.id) AS cid
    FROM CALCULATION c
    LEFT JOIN fragments fr ON fr.id=c.fileID
    GROUP BY c.fileID
) calc ON calc.fileID=n.fileID and calc.cid=PT_OUT.calculationID
WHERE calc.fileID IN (%1)

 )SQL").arg(m_cxema.m_par);

//    QInputDialog::getMultiLineText(nullptr, "title", "label", q);
//    TableWindow * tab = new TableWindow(m_cxema.m_db, "zap3", "Теплопотребление в открытых системах,  полученное, ГКал/ч", q, this);
//    tab->show();

    std::map<QString, QVariant> map_val;

    if (readTableRowValuesMap(m_cxema.m_db, q, map_val)) {
        auto *dlg = new PropertyDial(this, "tpO", "", "shape", "id", -1, -1);

        dlg->init_dialog(&m_cxema.m_db, "Теплопотребление в открытых системах полученное, ГКал/ч", "tpO", map_val);

        dlg->show();
    }
}
    
