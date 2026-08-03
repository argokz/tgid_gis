#include <QCoreApplication>
#include "col_index.h"
#include <QLocale>
#include <QTranslator>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QProgressDialog>
#include <QFile>
#include <QMessageBox>
#include <QInputDialog>


#include <mystd.h>

#include <iostream>
#include <cxema/Graph.h>
#include <cxema/graph2.h>
#include <cxema/cxema1.h>

#include <db/db.h>


long sizeOfQ(QSqlDatabase &db, const QString & q);


const double coef_xy = 1.;


bool readLineNew(QSqlQuery &query, const ColIndex &C, CLine2* line, int typ, CGraph2* m_graph, bool is_out)
{
    long externalSignLineID = query.value(C("externalSignLineID")).toInt();
    long id = query.value(C("id")).toInt();
    long id2 = query.value(C("ID2")).toInt();
    long podp = query.value(C("displaySign")).toInt();
    long organizationID = query.value(C("organizationID")).toInt();
    long magistral = 0;
    long distSite = 0;
    long magistralSite = 0;
    long tubingTypeID = 0; // _TR("Н");
    long pipeSectStateIDflow = 1;
    long pipeSectStateIDret = 1;
    long crimpingQueSite = 0; // опрессовка
    long pipeSectionID = 0;

/*
    line->line.nomP = -1;
    line->line.idP2 = -1;
    line->line.nomgP = -1;
    line->line.nomO = -1;
    line->line.idO2 = -1;
    line->line.nomgO = -1;
    line->line.pod.q = 0;
    line->line.obr.q = 0;
*/
    UTNapr napr;

    pipeSectStateIDflow = query.value(C("pipeSectStateIDflow")).toInt();
    pipeSectStateIDret = query.value(C("pipeSectStateIDret")).toInt();


    if (typ == TIP_UT) {
        magistral = query.value(C("magistral")).toInt();

        distSite = query.value(C("distSite")).toInt();
        magistralSite = query.value(C("magistralSite")).toInt();


        tubingTypeID = query.value(C("tubingTypeID")).toInt();
        crimpingQueSite = query.value(C("crimpingQueSite")).toInt();  // опрессовка
        pipeSectionID = query.value(C("pipeSectionID")).toInt();  // Большой участок

        double diameterExternal = query.value(C("diameterExternal")).toDouble();
        double diameterInternal = query.value(C("diameterInternal")).toDouble();
        double diameterCondit = query.value(C("diameterCondit")).toDouble();
        double pipeSectLength = query.value(C("pipeSectLength")).toDouble();

        napr.dl = pipeSectLength;
        napr.diam = diameterInternal;
        napr.diam_usl = diameterCondit;
        napr.diam_vn = diameterExternal;
        napr.v = napr.dl * pow(napr.diam * 0.001, 2) * M_PI;
        napr.tol = query.value(C("wallThickness")).toDouble();
    }
    else if (typ == TIP_DR) {
//        napr.dru_home = query.value(C("diameterInternal")).toDouble();
        napr.dru_home = query.value(C("dru_home")).toDouble();
    }

    if (pipeSectStateIDflow == 0) pipeSectStateIDflow = 1;
    if (pipeSectStateIDret == 0) pipeSectStateIDret = 1;

    line->line.tubingTypeID = tubingTypeID; //  tubingTypeID != "" ? tubingTypeID[0] : 'Н';

    line->line.pipeSectionID = pipeSectionID;


    line->line.mag = magistral;
    line->line.ms = magistralSite;
    line->line.rs = distSite;

    line->line.typ = typ;
    line->line.owner = organizationID;
    line->line.m_opressovka = crimpingQueSite;


    if (externalSignLineID == SignLine_pp) {
        line->line.nomO = -1;
    }

    if (externalSignLineID == SignLine_po) {
        line->line.nomP = -1;
    }


    if (externalSignLineID == SignLine_ob || externalSignLineID == SignLine_pp || externalSignLineID == SignLine_po) {
        line->line.nomP = id;
        line->line.idP2 = id2;
        line->line.pod = napr;
        line->line.pod.isOtkl = (pipeSectStateIDflow == 2);
        line->line.isPodp = !podp;
    }
    if (externalSignLineID == SignLine_ob || externalSignLineID == SignLine_oo || externalSignLineID == SignLine_op) {
        line->line.nomO = id;
        line->line.idO2 = id2;
        line->line.obr = napr;
        line->line.obr.isOtkl = (pipeSectStateIDret == 2);
        line->line.isPodp = !podp;
    }
    line->line.typ = typ;

    if (externalSignLineID == SignLine_ob || externalSignLineID == SignLine_pp || externalSignLineID == SignLine_po) {
        line->line.pod.q = query.value(C("pod_q")).toDouble();        //  Расход сетевой воды на участке
        line->line.nomgP = query.value(C("nomgP")).toInt();
    }
    if (externalSignLineID == SignLine_ob || externalSignLineID == SignLine_oo || externalSignLineID == SignLine_op) {
        line->line.obr.q = query.value(C("obr_q")).toDouble();        //  Расход сетевой воды на участке
        line->line.nomgO = query.value(C("nomgO")).toInt();
    }

    if (typ == TIP_UT) {
        if (externalSignLineID == SignLine_ob || externalSignLineID == SignLine_pp || externalSignLineID == SignLine_po) {
            line->line.pod.poter = query.value(C("pod_poter")).toDouble();
            line->line.pod.w = query.value(C("pod_w")).toDouble();        //  Скорость потока сетевой воды
            line->line.pod.time1 = query.value(C("pod_time1")).toDouble();    //  Время прохождения потока воды
            line->line.pod.a14 = query.value(C("pod_a14")).toDouble();      //  Удельные линейные потери напора на участке
            line->line.pod.a15 = query.value(C("pod_a15")).toDouble();      //  Линейные потери напора на участке
            line->line.pod.a16 = query.value(C("pod_a16")).toDouble();      //  Местные потери напора на участке
            line->line.pod.a17 = query.value(C("pod_a17")).toDouble();      //  Общие потери напора на участке
            line->line.pod.tzam = query.value(C("pod_tzam")).toDouble();      //  Время остывания воды при остановке движения
            line->line.pod.tpot = query.value(C("pod_tpot")).toDouble();    //  Тепловые потери  теплопроводом


            line->line.pod.ql    = query.value(C("pod_b101")).toDouble();    //
            line->line.pod.ql_ot = query.value(C("pod_b102")).toDouble();    //
            line->line.pod.ql_v  = query.value(C("pod_b103")).toDouble();    //
            line->line.pod.ql_gv = query.value(C("pod_b104")).toDouble();    //

            line->line.pod.ql_gv_p = query.value(C("pod_b105")).toDouble();    //
            line->line.pod.ql_gv_o = query.value(C("pod_b106")).toDouble();    //


            line->line.obr.ql    = query.value(C("obr_b101")).toDouble();    //
            line->line.obr.ql_ot = query.value(C("obr_b102")).toDouble();    //
            line->line.obr.ql_v  = query.value(C("obr_b103")).toDouble();    //
            line->line.obr.ql_gv = query.value(C("obr_b104")).toDouble();    //

            line->line.obr.ql_gv_p = query.value(C("obr_b105")).toDouble();    //
            line->line.obr.ql_gv_o = query.value(C("obr_b106")).toDouble();    //


        }

        if (externalSignLineID == SignLine_ob || externalSignLineID == SignLine_oo || externalSignLineID == SignLine_op) {
            line->line.obr.poter = query.value(C("obr_poter")).toDouble();
            line->line.obr.w = query.value(C("obr_w")).toDouble();        //  Скорость потока сетевой воды
            line->line.obr.time1 = query.value(C("obr_time1")).toDouble();    //  Время прохождения потока воды
            line->line.obr.a14 = query.value(C("obr_a14")).toDouble();      //  Удельные линейные потери напора на участке
            line->line.obr.a15 = query.value(C("obr_a15")).toDouble();      //  Линейные потери напора на участке
            line->line.obr.a16 = query.value(C("obr_a16")).toDouble();      //  Местные потери напора на участке
            line->line.obr.a17 = query.value(C("obr_a17")).toDouble();      //  Общие потери напора на участке
            line->line.obr.tzam = query.value(C("obr_tzam")).toDouble();      //  Время остывания воды при остановке движения
            line->line.obr.tpot = query.value(C("obr_tpot")).toDouble();    //  Тепловые потери  теплопроводом
        }
    }

    return true;
}


QString lineQ()
{
    QString q = readQ("sql3/ut.sql");
    q.replace(QString("$user_gid$"), QString("Администратор"));
    return q;
}


bool CCxema::read_lines(const QString & par, QProgressDialog *percent, bool dubl)
{
    QString q = lineQ();

    if (q == "") {
        QMessageBox::warning(nullptr, "", "Не могу открыть файл sql3/ut.sql. Переустановите систему");
        return false;
    }

    if (par.left(1) == "(") {
        q += QString(" AND %1").arg(par);
    }
    else {
        if (!isFragmentList(par)) {
            QMessageBox::warning(nullptr, "",
                QObject::tr("Некорректный список фрагментов: %1").arg(par));
            return false;
        }
        q += QString(" AND n1.fileID IN (%1)").arg(par);
    }


//    QInputDialog::getMultiLineText(nullptr, "title", "label", q);
//    std::cout << q.toStdString() << std::endl;
//    bool dubl = false;

    QSqlQuery query(m_db);
    query.setForwardOnly(true);

    QString qq = QString("SELECT l.id FROM net.v_linesobj l JOIN net.v_nodes n1 ON n1.id=l.nodeID1 AND n1.removed=0 AND l.removed=0 AND n1.fileID IN (%1)").arg(par);

    if (par.left(1) == "(") {
        qq = QString("SELECT l.id FROM net.v_linesobj l JOIN net.v_nodes n1 ON n1.id=l.nodeID1 AND n1.removed=0 AND l.removed=0 AND %1").arg(par);
    }


//    QInputDialog::getMultiLineText(nullptr, "", "", qq);

    // Счёт строк нужен ТОЛЬКО как знаменатель полосы прогресса.
    //
    // Замер по журналу программы: этот count(*) — самый дорогой запрос
    // при открытии фрагмента, 1,42 с из 1,7 с всего SQL. Он повторяет
    // тот же тяжёлый JOIN двух представлений UNION ALL, что и сам
    // запрос чтения, то есть фрагмент читается фактически дважды.
    // Когда полосы прогресса нет, платить за неё незачем.
    long cnt = percent ? sizeOfQ(m_db, qq) : 0;

//    QInputDialog::getMultiLineText(nullptr, "", "", q);

    query_exec(m_db, query, q);
    // То же, что в read_nodes: 21,3 с на 80 614 участков уходило
    // на поиск имени колонки при каждом обращении.
    const ColIndex C(query);

    int i = 0;

    long id_old = -1;

    while (query.next()) {
        long id = query.value(C("id")).toInt();
//          doSomething(country);

        if (id == 452 || id == 453) {
            int qq;
            qq = 1;
        }



        long fileID = query.value(C("fileID")).toInt();

        QString type_txt = query.value(C("type_txt")).toString();

        int typ = getNodeTyp(type_txt);

        CCoordList NP;

        if (id != id_old) {
            id_old = id;

            long idn1 = query.value(C("nodeID1")).toInt();
            long idn2 = query.value(C("nodeID2")).toInt();

            QString type_txt = query.value(C("type_txt")).toString();

            int typ = getLineTyp(type_txt);

            if (idn1 != idn2 && idn1 > 0 && idn2 > 0) {
                CNode2* n1, * n2, * n1i = NULL, * n2i = NULL;

                n1 = m_graph->find(idn1);
                n2 = m_graph->find(idn2);

                if (n1 && n2) {
                    bool skip = false;

                    if (n1->node.internalNodeID != 0) {
                        n1i = m_graph->find(n1->node.internalNodeID);
                        if (!n1i) skip = true;
                    }
                    if (n2->node.internalNodeID != 0) {
                        n2i = m_graph->find(n2->node.internalNodeID);
                        if (!n2i) skip = true;
                    }

                    if (!skip) {
                        QString s_coord = query.value(C("coords")).toString();
                        NP.loadStr(s_coord.toLatin1().data());

                        if (coef_xy != 1.) {
                            for (auto it = NP.begin(); it != NP.end(); it++) {
                                it->x *= coef_xy;
                                it->y *= coef_xy;
                            }
                        }



                        CLINE2* l = m_graph->find_line_typ(n1, n2, linef[typ].typ);
                        CLine2* line = bline(l);

                        if (!line || line->line.m_NP != NP) {
                            line = m_graph->insert_line(n1, n2, NP, true);
                        }
                        else {
                            if (line) {
                                long sign_old = getExternalSignLineId(l);
                                long sign_new = query.value(C("externalSignLineID")).toInt();

                                if (!((sign_old == 2 && sign_new == 3) || (sign_old == 3 && sign_new == 2))) {

                                    int nomP_old = line->line.nomP;
                                    int nomO_old = line->line.nomO;


                                    //       if (dubl || (nomP_old == id || nomO_old)) {
                                    if (dubl) {
                                    }
                                    else {
                                        line = m_graph->insert_line(n1, n2, NP, true);
                                    }
                                }
                            }
                        }

                        if (line) {
                            readLineNew(query, C, line, typ, m_graph);
                        }
                    }
                }
            }
        }

        //           std::cout << str.toStdString() << std::endl;

//            if (i %1000 == 0) {
//           std::cout << i*100./cnt  << "%          \r";
//          }

        if (percent && cnt) {
            if (i%1000 == 0) {
                percent->setValue(i * 100. / cnt / 2 + 50);
                QApplication::processEvents();
            }
        }
        i++;
    }

    if (percent && cnt) {
        percent->setValue(100);
        QApplication::processEvents();
    }


    return true;
}
