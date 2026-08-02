#include <QtGui>
#include <QtSql>
#include <iostream>

#include <gidview/GidWidget.h>

#include "db.h"
//#include "insert.h"

void copyVydGraph_fileID(CGraph2* graph1, CGraph2* graph2, bool is_po, int fileID)
{
    auto p = graph1->map_node.begin();
    while (p != graph1->map_node.end()) {
        CNode2* n = p->second;
        //    if (n->node.isPjezo) {
        if (n->node.fileID == fileID) {
            CNode2* n2 = graph2->find_ins(n->id);
            if (n2) {
                n2->node = n->node;
            }
        }
        p++;
    }

    p = graph1->map_node.begin();

    while (p != graph1->map_node.end()) {
        CNode2* n = p->second;

        //    if (n->node.isPjezo) {
        if (n->node.fileID == fileID) {
            CNode2* n2 = graph2->find(n->id);

            if (n2) {
                for (CLINE2* l = n->lines; l; l = nextl(l)) {
                    //          if (bline(l)->line.isPjezo && IsBegin(l) && (!is_po || (bline(l)->line.nomP != -1 || bline(l)->line.nomO != -1) )  ) {
//                    if (IsBegin(l) && (!is_po || (bline(l)->line.nomP != -1 || bline(l)->line.nomO != -1))) {
                    if (IsBegin(l)) {
                        CNode2* o = other(l);
                        CNode2* o2 = graph2->find(o->id);
                        if (o2) {
                            CCoordList cl;

                            CLINE2* ll = graph2->insert_line(n2, o2, cl, true);
                            if (ll) {
                                bline(ll)->line = bline(l)->line;
                                //                bline(ll)->line.isPjezo = false;
                            }
                        }
                    }
                }
            }
        }
        p++;
    }
}


QString getTemp(QSqlDatabase &db)
{
    QString tn = "temp_table";
    return tn;
}


bool create_vyd_line_table0(QSqlDatabase &db, CGraph2* graph, const QString & tn)
{
//    CWaitCursor wait;
    bool ret = false;
//    long affected;
    QString sql;

    QSqlQuery query(db);

#if 0
    if (!db.tables().contains(tn)) {

//    sql = QString("DROP TABLE %1").arg(tn);
//    ret = query_exec(query, sql);
        sql = QString("CREATE TABLE %1 (ID INT NOT NULL, PRIMARY KEY (ID))").arg(tn);
        ret = query_exec(db, query, sql);
    }
    else {
#endif
        sql = QString("DELETE FROM %1").arg(tn);
        ret = query_exec(db, query, sql);
#if 0
    }
#endif

    CMapGraph::const_iterator p;

    CAdoInsert ins(&db, tn, "id");

    bool is_vyd = false;

    for(auto p : graph->map_node) {
        CNode2* n = p.second;
        for (CLINE2* l = n->lines; l; l = nextl(l)) {
            if (IsBegin(l) && bline(l)->line.isPjezo) {
                if (bline(l)->line.nomP > 0) {
                    if (ins.ado_insert_int1(bline(l)->line.nomP)) ret = true;
                }

                if (bline(l)->line.nomO > 0 && bline(l)->line.nomP != bline(l)->line.nomO) {
                    if (ins.ado_insert_int1(bline(l)->line.nomO)) ret = true;
                }
            }
        }
    }
    if (ins.ado_insert()) ret = true;
    return ret;
}


bool create_vyd_node_table0(QSqlDatabase &db, CGraph2* graph, const QString & tn)
{
//    CWaitCursor wait;
    bool ret = true;
    QString sql;

    QSqlQuery query(db);
#if 0
    if (!db.tables().contains( tn )) {
//        sql = QString("DROP TABLE %1").arg(tn);
//        ret = query_exec(db, query, sql);

        sql = QString("CREATE TABLE %1 (ID INT NOT NULL, PRIMARY KEY (ID))").arg(tn);
        ret = query_exec(db, query, sql);
    }
    else {
#endif
        sql = QString("DELETE FROM %1").arg(tn);
        ret = query_exec(db, query, sql);
#if 0
    }
#endif
    if (!ret) {
        ErrorMessage(query);
        return false;
    }

    CAdoInsert ins(&db, tn, "id");

    bool is_vyd = false;

    for(auto p : graph->map_node) {
        CNode2* n = p.second;
        if (n->node.isPjezo) {
            if (ins.ado_insert_int1(n->id)) ret = true;
        }
    }
    if (ins.ado_insert()) ret = true;
    return ret;
}



bool create_vyd_line_table(QSqlDatabase &db, CGraph2* graph, QString& tn)
{
//    tn = getTemp(db);

    tn = "temp_table1";
    return create_vyd_line_table0(db, graph, tn);
}


bool create_vyd_node_table(QSqlDatabase &db, CGraph2* graph, QString& tn)
{
//    tn = getTemp(db);
    tn = "temp_table1";
    return create_vyd_node_table0(db, graph, tn);
}


void copyVydGraph_new(CGraph2 *graph1, CGraph2 *graph2, bool is_po);
void sort_line_rs_new(CGraph2 *graph, list<CNode2*> &list_sort, list<CLINE2 *> &st_l, int fileID);


bool create_vyd_line_table0_new_simple(QSqlDatabase &db, CGraph2* graph, const QString & tn)
{
 //   CWaitCursor wait;
    bool ret = false;
//    long affected;
    QString sql;

    QSqlQuery query(db);

    sql = QString("DROP TABLE %1").arg(tn);
    ret = query_exec(db, query, sql);

//    sql = QString("CREATE TEMPORARY TABLE %1 (ID1 INT NOT NULL, ID2 INT NOT NULL, PRIMARY KEY (ID1))").arg(tn);
    sql = QString("CREATE TABLE %1 (ID1 INT NOT NULL, ID2 INT NOT NULL, PRIMARY KEY (ID1))").arg(tn);
    ret = query_exec(db, query, sql);

    if (!ret) {
        ErrorMessage(query);
        return false;
    }

    CGraph2 graph2(NULL);
    list<CNode2*> list_sort;
    list<CLINE2*> st_l;

    copyVydGraph_new(graph, &graph2, true);
    sort_line_rs_new(&graph2, list_sort, st_l, -1);

    CNode2* n1_old = nullptr, * n2_old = nullptr;
    CNode2* n_old = nullptr;

    list <list<int> > list_ut;
    list<int> list_ut_min;

    int nn = 1;

    CAdoInsert ins(&db, tn, "id1,id2");

    graph->init_find_line_nom();

    for (auto p : st_l) {
        CLINE2* l = p;

        CLINE2* lP = graph->find_line_nom(bline(l)->line.nomP);
        CLINE2* lO = graph->find_line_nom(bline(l)->line.nomO);

        if (lP || lO) {
            CNode2* n1 = where(l);
            CNode2* n2 = other(l);

            //        if (n1_old && (isPSstrict(n1) || n1 != n2_old)) {
            if (n1_old && (n1 != n2_old)) {
                for (auto it : list_ut_min) {
                    if (ins.ado_insert_int2(nn++, it)) ret = true;
                }

                list_ut_min.clear();
            }

            list_ut_min.push_back(lP ? bline(lP)->line.nomP : bline(lO)->line.nomO);

            n1_old = n1;
            n2_old = n2;
        }
    }

    for (auto it : list_ut_min) {
        if (ins.ado_insert_int2(nn++, it)) ret = true;
    }
    if (ins.ado_insert()) ret = true;

    return ret;
}

bool isPSstrict(CNode2* node)
{
    return true;
}

bool create_vyd_node_table0_BIG(QSqlDatabase &db, CGraph2* graph, const QString & tn, bool is_big)
{
//    CWaitCursor wait;
    bool ret = false;
    QString sql;

    QSqlQuery query(db);

    sql = QString("DROP TABLE %1").arg(tn);
    ret = query_exec(db, query, sql);

//    sql = QString("CREATE TEMPORARY TABLE %1 (ID1 INT NOT NULL, ID2 INT NOT NULL, fileID int, PRIMARY KEY (ID1))", tn);
    sql = QString("CREATE TABLE %1 (ID1 INT NOT NULL, ID2 INT NOT NULL, fileID int, PRIMARY KEY (ID1))").arg(tn);
    ret = query_exec(db, query, sql);

    if (!ret) {
        ErrorMessage(query);
        return false;
    }

    CGraph2 graph2(NULL);
    list<CNode2*> list_sort;
    list<CLINE2*> st_l;

    copyVydGraph_new(graph, &graph2, true);
    sort_line_rs_new(&graph2, list_sort, st_l, -1);
    CNode2* n1_old = nullptr, * n2_old = nullptr;
    CNode2* n_old = nullptr;

    list<list<int> > list_ut;
    list<int> list_ut_min;

    int nn = 1;

    auto it_first = list_sort.begin();
    auto it_last = list_sort.rbegin();

    CAdoInsert ins(&db, tn, "id1,id2,fileID");

    for (auto p : list_sort) {
        CNode2* n = graph->find(p->id);

        //        if (isPSstrict(p) || p == *it_first || p == *it_last) {

        if (isPSstrict(p) || !is_big) {
            sql = QString("%1,%2,%3").arg(nn++).arg(n->id).arg(n->node.fileID);
            if (ins.ado_insert(sql)) ret = true;
        }
    }
    if (ins.ado_insert()) ret = true;
    return ret;
}


bool create_vyd_line_table0_new_all(QSqlDatabase &db, CGraph2* graph, const QString & tn, int fileID)
{
//    CWaitCursor wait;
    bool ret = false;
    QString sql;

    QSqlQuery query(db);

    sql = QString("DROP TABLE %1").arg(tn);
    ret = query_exec(db, query, sql);

    sql = QString("CREATE TABLE %1 (ID1 INT NOT NULL, ID2 INT NOT NULL, PRIMARY KEY (ID1))").arg(tn);
    ret = query_exec(db, query, sql);

    if (!ret) {
        ErrorMessage(query);
        return false;
    }

    CGraph2 graph2(NULL);
    list<CNode2*> list_sort;
    list<CLINE2*> st_l;

    //    copyVydGraph_new(graph, &graph2, true);

    copyVydGraph_fileID(graph, &graph2, false, fileID);
    sort_line_rs_new(&graph2, list_sort, st_l, -1);

    CNode2* n1_old = nullptr, * n2_old = nullptr;
    CNode2* n_old = nullptr;

    list <list<int> > list_ut;
    list<int> list_ut_min;

    int nn = 1;

    graph->init_find_line_nom();

    CAdoInsert ins(&db, tn, "id1,id2");

    for (auto p : st_l) {
        CLINE2* l = p;

        CLINE2* lP = graph->find_line_nom(bline(l)->line.nomP);
        CLINE2* lO = graph->find_line_nom(bline(l)->line.nomO);

        if (lP && (bline(lP)->line.nomP == 16761968 || bline(lP)->line.nomO == 16761969)) {
            int qq;
            qq = 1;
        }
        if (lO && (bline(lO)->line.nomP == 16761968 || bline(lO)->line.nomO == 16761969)) {
            int qq;
            qq = 1;
        }


        if (lP || lO) {
            CNode2* n1 = where(l);
            CNode2* n2 = other(l);

            //        if (n1_old && (isPSstrict(n1) || n1 != n2_old)) {
            if (n1_old && (n1 != n2_old)) {
                for (auto it : list_ut_min) {
                    if (ins.ado_insert_int2(nn++, it)) ret = true;
                }

                list_ut_min.clear();
            }

            list_ut_min.push_back(lP ? bline(lP)->line.nomP : bline(lO)->line.nomO);
            if (lP && lO && bline(lP)->line.nomP != bline(lP)->line.nomO) list_ut_min.push_back(bline(lO)->line.nomO);


            n1_old = n1;
            n2_old = n2;
        }
    }

    for (auto it : list_ut_min) {
        if (ins.ado_insert_int2(nn++, it)) ret = true;
    }
    if (ins.ado_insert()) ret = true;
    return ret;
}


bool create_vyd_node_table0_BIG_all(QSqlDatabase &db, CGraph2* graph, const QString & tn, bool is_big, int fileID)
{
//    CWaitCursor wait;
    bool ret = false;
    QString sql;

    QSqlQuery query(db);

    sql = QString("DROP TABLE %1").arg(tn);
    ret = query_exec(db, query, sql);

    sql = QString("CREATE TABLE %1 (ID1 INT NOT NULL, ID2 INT NOT NULL, fileID int, PRIMARY KEY (ID1))").arg(tn);
    ret = query_exec(db, query, sql);

    if (!ret) {
        ErrorMessage(query);
        return false;
    }

    CGraph2 graph2(NULL);
    list<CNode2*> list_sort;
    list<CLINE2*> st_l;

    //    copyVydGraph_new(graph, &graph2, true);

    copyVydGraph_fileID(graph, &graph2, false, fileID);
    
    sort_line_rs_new(&graph2, list_sort, st_l, -1);
    CNode2* n1_old = nullptr, * n2_old = nullptr;
    CNode2* n_old = nullptr;

    list<list<int> > list_ut;
    list<int> list_ut_min;

    int nn = 1;

    auto it_first = list_sort.begin();
    auto it_last = list_sort.rbegin();

    CAdoInsert ins(&db, tn, "id1,id2,fileID");

    for (auto p : list_sort) {
        CNode2* n = graph->find(p->id);

        //        if (isPSstrict(p) || p == *it_first || p == *it_last) {

        if (isPSstrict(p) || !is_big) {
            sql = QString("%1,%2,%3").arg(nn++).arg(n->id).arg(n->node.fileID);
            if (ins.ado_insert(sql)) ret = true;
        }
    }
    if (ins.ado_insert()) ret = true;
    return ret;
}



bool create_vyd_line_table_new_simple(QSqlDatabase &db, CGraph2* graph, QString& tn)
{
    tn = getTemp(db);

    return create_vyd_line_table0_new_simple(db, graph, tn);
}

bool create_vyd_line_table_new_all(QSqlDatabase &db, CGraph2* graph, QString& tn, int fileID)
{
    tn = getTemp(db);

    return create_vyd_line_table0_new_all(db, graph, tn, fileID);
}

bool create_vyd_node_table_BIG(QSqlDatabase &db, CGraph2* graph, QString& tn, bool is_big)
{
    tn = getTemp(db);

    return create_vyd_node_table0_BIG(db, graph, tn, is_big);
}

bool create_vyd_node_table_BIG_all(QSqlDatabase &db, CGraph2* graph, QString& tn, bool is_big, int fileID)
{
    tn = getTemp(db);

    return create_vyd_node_table0_BIG_all(db, graph, tn, is_big, fileID);
}
