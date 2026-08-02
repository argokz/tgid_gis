// Перенос gid6 temp.cpp: sortNodeDb / insertSortLinesToDb2 / copy_ut /
// get_first_last / isPS (CGidrView::OnCreateSortNode).
//
// Команда перестраивает ops.sortNodesForUchastok и ops.sortLinesForUchastok
// по текущему графу: для каждого участка МС/РС (ops.uchastok_ms,
// ops.uchastok_rs) граф обрезается до участка, сортируется
// (sort_line_rs_new), и в таблицы пишется порядок узлов и «больших линий»
// (подача+обратка между строгими узлами ПС).
//
// Отличия от gid6 продиктованы новой моделью «одна таблица — один объект»:
//  - legacy pipeSections больше не живое (attic-архив, compat.pipesections —
//    read-only view). Поэтому ветка gid6 «pipeSectionID==0 -> INSERT INTO
//    pipeSections + UPDATE heatPipeSections» убрана: pipesectionid в новой
//    модели системный (= id heatpipesection), при нуле в
//    sortLinesForUchastok пишется NULL;
//  - copyPTS (апдейт legacy pipeSections) и DELETE FROM iznos (attic) не
//    переносятся;
//  - ADO/CAdoFile заменены на QSqlQuery/CAdoInsert (db/db.h).

#include <QtGui>
#include <QtWidgets>

#include <gidview/GidWidget.h>
#include <db/db.h>
#include <any/rights.h>

void copyVydGraph_new(CGraph2* graph1, CGraph2* graph2, bool is_po = false);
void sort_line_rs_new(CGraph2* graph, list<CNode2*>& list_sort, list<CLINE2*>& st_l, int fileID = -1);


// Полная версия isPSstrict из gid6 PtsAlmaM2.cpp (в db/temp.cpp gid8 живёт
// заглушка isPSstrict() == true для create_vyd_node_table0_BIG, её трогать
// нельзя). Узел считается «строгим» ПС, если он типизирован, назван, или
// через него нельзя пройти насквозь (inc1 != 2).
static bool isPS_sort(CNode2* node)
{
    CLINE2* l;
    int inc1 = 0;

    for (l = node->lines; l; l = nextl(l)) {
        if (bline(l)->line.typ == TIP_UT) {
            inc1++;
        }
    }

    std::set<int> set_ms;
    std::set<int> set_rs;

    for (l = node->lines; l; l = nextl(l)) {
        set_ms.insert(bline(l)->line.ms);
        set_rs.insert(bline(l)->line.rs);
    }

    if (set_ms.size() + set_rs.size() > 2) inc1 = 3;

    bool lin1 = false;
    bool lin2 = false;

    for (l = node->lines; l; l = nextl(l)) {
        if (bline(l)->line.nomP == -1 || bline(l)->line.nomO == -1) {
            lin1 = true;
        }
        if (bline(l)->line.nomP != -1 && bline(l)->line.nomO != -1) {
            lin2 = true;
        }
    }
    if (lin1 && lin2) inc1 = 3;

    if (inc1 == 2) {
        CNode2* nn1 = nullptr, * nn2 = nullptr;
        for (l = node->lines; l; l = nextl(l)) {
            if (bline(l)->line.nomP == -1 || bline(l)->line.nomO != -1) {
                nn1 = other(l);
            }
            if (bline(l)->line.nomP != -1 && bline(l)->line.nomO == -1) {
                nn2 = other(l);
            }
        }
        if (nn1 && nn2 && nn1 == nn2) {
            inc1 = 1;
        }

        for (l = node->lines; l; l = nextl(l)) {
            if (bline(l)->line.nomP != -1 && bline(l)->line.nomO != -1
                && bline(l)->line.nomP != bline(l)->line.nomO) {
                inc1 = 3;
            }
        }
    }

    return (node->typPS() > 0 && node->typPS() != PS_NEUST) || node->node.namePS != "" || inc1 != 2;
}


struct BigLine
{
    int n_maxP = -1;
    int n_maxO = -1;
    CNode2* n_first = nullptr;
    CNode2* n_last = nullptr;
    double lengthP = 0;
    double lengthO = 0;
    list<int> list_l;
};


static bool get_first_last(CGraph2* graph, list<int>& list_ut_min, CNode2*& n_first, CNode2*& n_last)
{
    CNode2* n1_old = nullptr, * n2_old = nullptr;
    n_first = nullptr;
    n_last = nullptr;

    if (list_ut_min.size() == 2) {
        CLINE2* ll[2];
        int i = 0;
        for (auto it : list_ut_min) {
            ll[i] = graph->find_line_nom(it);
            i++;
        }
        if (ll[0] == ll[1]) {
            CNode2* n1 = where(ll[0]);
            CNode2* n2 = other(ll[0]);

            if (bline(ll[0])->line.nomO == -1) {
                bline(ll[0])->line.nomO = bline(ll[1])->line.nomO;
            }
            if (bline(ll[0])->line.nomP == -1) {
                bline(ll[0])->line.nomP = bline(ll[1])->line.nomP;
            }
            auto it = list_ut_min.rbegin();
            list_ut_min.remove(*it);

            n_first = n1;
            n_last = n2;

            return true;
        }
    }

    auto it_last = list_ut_min.rbegin();

    CLINE2* l_old = nullptr;

    for (auto it : list_ut_min) {
        CLINE2* l = graph->find_line_nom(it);
        if (l && l == l_old) {
            CNode2* n1 = where(l);
            CNode2* n2 = other(l);
            n_first = n1;
            n_last = n2;
            list_ut_min.remove(it);
            break;
        }
        if (l && l != l_old) {
            l_old = l;
            CNode2* n1 = where(l);
            CNode2* n2 = other(l);

            if (n1_old && n2_old) {
                if (!n_first) {
                    if (n1 == n1_old || n2 == n1_old) n_first = n2_old;
                    if (n1 == n2_old || n2 == n2_old) n_first = n1_old;
                }

                if (it == *it_last) { // последняя итерация
                    if (n1 == n1_old || n1 == n2_old) n_last = n2;
                    if (n2 == n1_old || n2 == n2_old) n_last = n1;
                }
            }

            if (it == *it_last && (!n_first || !n_last)) { // последняя итерация
                n_first = n1;
                n_last = n2;
            }

            n1_old = n1;
            n2_old = n2;
        }
    }
    return true;
}


// Схлопывает набор УТ (list_ut_min — nomP/nomO линий) в одну «большую
// линию»: суммарные длины, крайние узлы, самый длинный УТ (n_maxP) и
// список id heatpipesections (idP2/idO2).
static bool copy_ut(CGraph2* graph, list<int>& list_ut_min, BigLine& bigline)
{
    bool ret = false;
    double len_max = -1;

    CNode2* n_first = nullptr, * n_last = nullptr;

    get_first_last(graph, list_ut_min, n_first, n_last);

    int n_maxP = -1;

    double lengthP = 0, lengthO = 0;

    bigline.list_l.clear();

    for (auto it : list_ut_min) {
        CLINE2* ll = graph->find_line_nom(it);
        if (ll) {
            double lenP = bline(ll)->line.pod.dl;
            double lenO = bline(ll)->line.obr.dl;

            lengthP += lenP;
            lengthO += lenO;

            if (lenP > len_max || lenO > len_max) {
                n_maxP = it;
            }
            bigline.list_l.push_back(bline(ll)->line.idP2);
            bigline.list_l.push_back(bline(ll)->line.idO2);
        }
    }

    if (n_maxP > 0 && n_first && n_last) {
        bigline.n_maxP = n_maxP;
        bigline.n_first = n_first;
        bigline.n_last = n_last;
        bigline.lengthP = lengthP;
        bigline.lengthO = lengthO;
    }
    return ret;
}


// sortNodesForUchastok: порядок строгих узлов ПС вдоль участка.
static void sortNodeDb(QSqlDatabase &db, CGraph2* graph, bool ms, int id)
{
    CGraph2 graph2(NULL);
    list<CNode2*> list_sort;
    list<CLINE2*> st_l;

    QString tn = "sortNodesForUchastok";
    QString column = ms ? "uchastok_ms_id" : "uchastok_rs_id";

    query_exec(db, QString("DELETE FROM %1 WHERE %2=%3").arg(tn, column).arg(id));

    graph->reset();

    if (ms) {
        graph->vydMS(id);
    }
    else {
        graph->vydRS(id);
    }

    copyVydGraph_new(graph, &graph2, true);
    sort_line_rs_new(&graph2, list_sort, st_l);

    QString h = QString("orderID, nodeID, %1").arg(column);
    CAdoInsert ins(&db, tn, h);

    int nn = 1;

    for (auto p : list_sort) {
        CNode2* n = graph->find(p->id);

        if (n && isPS_sort(p)) {
            ins.ado_insert(QString("%1,%2,%3").arg(nn++).arg(n->id).arg(id));
        }
    }
    ins.ado_insert();
}


// sortLinesForUchastok: «большие линии» (подача/обратка) вдоль участка.
static void insertSortLinesToDb2(QSqlDatabase &db, CGraph2* graph, bool ms, int id)
{
    BigLine bigline;
    list<BigLine> bigline_list;
    CGraph2 graph2(NULL);
    list<CNode2*> list_sort;
    list<CLINE2*> st_l;

    QString tn = "sortLinesForUchastok";
    QString column = ms ? "uchastok_ms_id" : "uchastok_rs_id";

    query_exec(db, QString("DELETE FROM %1 WHERE %2=%3").arg(tn, column).arg(id));

    graph->reset();

    if (ms) {
        graph->vydMS(id);
    }
    else {
        graph->vydRS(id);
    }

    copyVydGraph_new(graph, &graph2);
    sort_line_rs_new(&graph2, list_sort, st_l);

    CNode2* n1_old = nullptr, * n2_old = nullptr;

    list<int> list_ut_min;

    graph->init_find_line_nom();

    for (auto p : st_l) {
        CLINE2* l = p;

        CLINE2* lP = graph->find_line_nom(bline(l)->line.nomP);
        CLINE2* lO = graph->find_line_nom(bline(l)->line.nomO);

        if (lP || lO) {
            CNode2* n1 = where(l);
            CNode2* n2 = other(l);

            if (n1_old && (isPS_sort(n1) || n1 != n2_old)) {
                copy_ut(graph, list_ut_min, bigline);
                bigline_list.push_back(bigline);
                list_ut_min.clear();
            }

            if (lP) {
                list_ut_min.push_back(bline(lP)->line.nomP);
            }
            if (lO && (lO != lP || bline(lP)->line.nomP != bline(lO)->line.nomO)) {
                list_ut_min.push_back(bline(lO)->line.nomO);
            }

            n1_old = n1;
            n2_old = n2;
        }
    }
    if (list_ut_min.size() > 0) {
        copy_ut(graph, list_ut_min, bigline);
        bigline_list.push_back(bigline);
    }

    if (bigline_list.size() > 0) {
        CNode2* n1 = nullptr;
        CNode2* n2 = nullptr;

        auto pp = bigline_list.rbegin();

        for (auto p = bigline_list.begin(); p != bigline_list.end(); p++) {
            if (&*pp != &*p) {
                auto p_next = std::next(p, 1);

                if (p->n_first == p_next->n_first || p->n_first == p_next->n_last) {
                    CNode2* nn = p->n_first; p->n_first = p->n_last; p->n_last = nn;
                }
            }

            if (p->n_last == n2) {
                CNode2* nn = p->n_first; p->n_first = p->n_last; p->n_last = nn;
            }

            n1 = p->n_first;
            n2 = p->n_last;
        }

        int nnn = 1;

        QString h = QString("orderID, pipeSectionID, lineID, nodeID1, nodeID2, totalLength, fileID, %1").arg(column);
        CAdoInsert ins_sort(&db, tn, h);

        for (auto& p : bigline_list) {
            if (isPS_sort(p.n_first) && isPS_sort(p.n_last)) {
                CLINE2* line = graph->find_line_nom(p.n_maxP);

                if (line) {
                    CLine2* l = bline(line);

                    double length = p.lengthP;

                    if (l->line.nomP == -1) {
                        length = p.lengthO;
                    }

                    // В новой модели pipesectionid системный (= id
                    // heatpipesection); ветка gid6 «создать pipeSection»
                    // не переносится — при нуле пишем NULL.
                    QString psid = l->line.pipeSectionID > 0
                        ? QString::number(l->line.pipeSectionID) : QString("NULL");

                    ins_sort.ado_insert(QString("%1,%2,%3,%4,%5,%6,%7,%8")
                        .arg(nnn++).arg(psid).arg(p.n_maxP)
                        .arg(p.n_first->id).arg(p.n_last->id)
                        .arg(length).arg(p.n_first->node.fileID).arg(id));

                    if (l->line.nomP != -1 && l->line.nomO != -1 && l->line.nomP != l->line.nomO) {
                        ins_sort.ado_insert(QString("%1,%2,%3,%4,%5,%6,%7,%8")
                            .arg(nnn++).arg(psid).arg(l->line.nomO)
                            .arg(p.n_first->id).arg(p.n_last->id)
                            .arg(p.lengthO).arg(p.n_first->node.fileID).arg(id));
                    }
                }
            }
        }
        ins_sort.ado_insert();
    }
    graph2.clear();
}


void GidWidget::onCreateSortNode() // Создание таблицы sortNodesForUchastok
{
    if (!UserRight::isAdmin()) {
        QMessageBox::warning(this, "", tr("Для выполнения этой операции у вас должны быть права администратора!"));
        return;
    }

    if (QMessageBox::question(this, "",
        tr("Перестроить таблицы sortNodesForUchastok и sortLinesForUchastok по текущему графу?")) != QMessageBox::Yes) {
        return;
    }

    QSqlDatabase &db = m_cxema.m_db;
    CGraph2* graph = m_cxema.m_graph;

    QList<int> ids_ms;
    QList<int> ids_rs;

    QSqlQuery query(db);
    query.setForwardOnly(true);
    if (query_exec(db, query, "SELECT id FROM uchastok_ms ORDER BY id")) {
        while (query.next()) ids_ms << query.value(0).toInt();
    }
    if (query_exec(db, query, "SELECT id FROM uchastok_rs ORDER BY id")) {
        while (query.next()) ids_rs << query.value(0).toInt();
    }

    const int sz = ids_ms.size() + ids_rs.size();
    if (sz == 0) {
        QMessageBox::information(this, "", tr("Нет участков МС/РС"));
        return;
    }

    QProgressDialog progress(tr("Сортировка участков..."), tr("Прервать"), 0, sz, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);

    // vydMS/vydRS работают через isPjezo — сохраняем состояние пьезометрии.
    graph->save_pjezo();

    int ii = 0;
    bool cancel = false;

    for (int id : ids_ms) {
        sortNodeDb(db, graph, true, id);
        insertSortLinesToDb2(db, graph, true, id);
        progress.setValue(++ii);
        if (progress.wasCanceled()) { cancel = true; break; }
    }

    if (!cancel) {
        for (int id : ids_rs) {
            sortNodeDb(db, graph, false, id);
            insertSortLinesToDb2(db, graph, false, id);
            progress.setValue(++ii);
            if (progress.wasCanceled()) { cancel = true; break; }
        }
    }

    progress.setValue(sz);

    graph->reset();
    graph->restore_pjezo();

    if (cancel) {
        QMessageBox::warning(this, "", tr("Прервано пользователем (обработано %1 из %2 участков)").arg(ii).arg(sz));
        return;
    }

    QMessageBox::information(this, "", tr("Готово"));
}
