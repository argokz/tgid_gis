#include <QApplication>
#include <QtGui>
#include <QtWidgets>
#include <gidview/GidWidget.h>
#include <mainwindow.h>

#include <property/PropertyDial.h>
#include <property/variantdelegate.h>
#include <db/db.h>

#include <any/undo.h>

#include "cxema/undo.h"
#include <any/rights.h>

bool isEditGeo(const QString & tn, bool isMessage);



QString findTableRusName(const QString & d, const QString & n);

void ErrorMessage(QSqlQuery &query);

void GidWidget::viewNode2()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CNode2 *n = (CNode2*)action->data().toLongLong();
        beginDraw(n);
    }
}

void GidWidget::viewLine2()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CLINE2 *ll = (CLINE2*) action->data().toLongLong();
        beginDraw(ll);
    }
}

void GidWidget::viewGeo2()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        KlGeo *klgeo = (KlGeo *) action->data().toLongLong();
        endDraw();
        m_CurKlassif = klgeo->kls;
        m_CurGeoObject = klgeo->gl;
    }
}

#include <tg/TGDialog.h>

void GidWidget::viewTG()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CNode2 *n = (CNode2*)action->data().toLongLong();
        if (n) {
            beginDraw(n);

            TGDialog tg(this, &m_cxema, m_cxema.m_db, n->id, n->node.id2);
            tg.exec();
        }
    }
}

void GidWidget::viewNode(CNode2 *n, bool autodelete)
{
    if (n) {
        beginDraw(n);

        QString frn = n->getFileName();
        QString title = n->getNameFullFile();


        if (isPR() && n->node.internalNodeID != 0) {
            int nom = max(n->node.nomP, n->node.nomO);

            QString q = QString(

"select us_out.id, us_out.externalsign, us_out.pih, us_out.t\n"
"from us_out\n"
"join nodes n on n.id=us_out.nodeid\n"
"join realconsumers rc on rc.nodeid =n.id\n"
"where n.removed = 0\n"
"and n.id=%1\n"
"and externalsign=%2\n"
"and us_out.calculationID = (select MAX(id) from calculation c where c.fileID=n.fileID)\n")
    .arg(m_parent_id).arg(nom);
            qDebug() << q;

            std::vector<QVariant> vv;

            if (readTableValues(m_cxema.m_db, q, 4, vv)) {
//                QString ss = QString("%1\n%2\n%3").arg(nom).arg(vv[2].toDouble()).arg(vv[3].toDouble());
                QString ss = QString("Пьезометрический напор %1 м.вод.ст\nТемпература %2°С").arg(vv[2].toDouble()).arg(vv[3].toDouble());
                QMessageBox::warning(this, "Узел", QString("%1").arg(ss));
            }
            else {
                QMessageBox::warning(this, "Узел", "");
            }
            return;
        }

        if (n->node.typ == TIP_US) {
//            QString title = n->getNameFull();
            info(m_cxema.m_db, title, "nodes", n->id, -1, pr_type_node0, "shape", "id", autodelete );
//            info(m_cxema.m_db, QString("Узел (%1)").arg(frn), "nodes", n->id, -1, pr_type_node0, "shape", "id", autodelete );
        }
        else {

            auto typ = pr_type_node;
            if (n->node.typ == TIP_US_VN) {
                typ = pr_type_any;
            }
            QString table = n->getTableMySQL();
//            QString title = findTableRusName("gid", table);
            info(m_cxema.m_db, title, table, n->id, -1, typ, "shape", "id", autodelete);
        }
    }
}


void GidWidget::viewNode()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CNode2 *n = (CNode2*)action->data().toLongLong();
        if (n) {
            viewNode(n);
        }
    }
}

void create_b5_new(QSqlDatabase &db, CNode2 *nP, CCxema* cxema);


void GidWidget::viewKam()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CNode2 *n = (CNode2*)action->data().toLongLong();
        if (n) {
            if ( (!n->node.isP && n->node.typ != TIP_PR) && (isEditGid() && QMessageBox::question(this, "", "Создать схему ?") == QMessageBox::Yes)) {
                if (initCxema(n)) {
                    n->node.isP = true;
                }
            }
            if (n->node.isP || n->node.typ == TIP_PR) {
                n->node.isP = true;
                if (n->node.typ == TIP_PR) {
                    create_b5_new(m_cxema.m_db, n, &m_cxema);
                }
                Kam(n->id);
            }
        }
    }
}


bool delCxema1(CCxema *m_cxema, const CNode2 *node, int m_user)
{
    if (node) {
        bool ret;
        QString q;

        int idRem = 1;

//        int idRem = addRemoved(m_cxema->m_ado, CH_T_DELETE_CXEMA_BEGIN, node->id, "");
//        idRem = addRemoved(m_cxema->m_ado, CH_T_DELETE_NODE, node->id, "");

        change_group_start(m_cxema->m_db, "Удаление внутренней схемы");

        q = QString("UPDATE nodes SET removed=1, idRemoved=%1, operatorID=%3, archiveChangeDate=%4, sync_tgid=true WHERE internalNodeID=%2")
            .arg(idRem).arg(node->id)
            .arg(m_user)
            .arg(get_now());
            ;

        QSqlQuery query(m_cxema->m_db);
        query.setForwardOnly(true);

        ret = query_exec(m_cxema->m_db, query, q);

        if (ret) {
//            q.Format("D-ELETE FROM linesobj WHERE nodeID1 in (SELECT id FROM nodes WHERE internalNodeID=%d)", node->id);

  //          idRem = addRemoved(m_cxema->m_ado, CH_T_DELETE_LINE, node->id, "");
            
            q = QString("UPDATE linesobj SET removed=1, idRemoved=%1, operatorID=%3, archiveChangeDate=%4, sync_tgid=true WHERE nodeID1 in (SELECT id FROM nodes WHERE internalNodeID=%2)")
                .arg(idRem).arg(node->id)
                .arg(m_user)
                .arg(get_now());
                
                ;

            ret = query_exec(m_cxema->m_db, query, q);
            if (ret) {
                m_cxema->m_graph->clearVnutr(node->id);
            }
        }
        change_group_end(m_cxema->m_db);
//        addRemoved(m_cxema->m_ado, CH_T_DELETE_CXEMA_END, node->id, "");
    }
    return true;
}


void GidWidget::delKam()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CNode2 *n = (CNode2*)action->data().toLongLong();
        if (n) {
            if (QMessageBox::question(this,  "", QString(tr("Вы хотите удалить внутреннюю схему в узле %1")).arg(n->getName())) == QMessageBox::Yes) {
                delCxema1(&m_cxema, n, m_user);
                n->node.isP = false;
                repaint();
            }
        }
    }
}

void GidWidget::viewNodeCalc()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CNode2 *n = (CNode2*)action->data().toLongLong();
        if (n) {
            beginDraw(n);

            QString title = n->getNameFullFile();

            if (n->node.typ == TIP_US) {
                info_gid(m_cxema.m_db, title, "nodes", "US_OUT", n->id, -1, pr_type_node0 );
//                info_gid(m_cxema.m_db, "Узел", "nodes", "US_OUT", n->id, -1, pr_type_node0 );
            }
            else {
                QString table = n->getTableMySQL();
//                QString title = findTableRusName("gid", table);
                QString table_out = getOutTable(table);
                info_gid(m_cxema.m_db, title, table, table_out, n->id, -1, pr_type_node  );
//                info_gid(m_cxema.m_db, title, table, table_out, n->id, -1, pr_type_node  );
            }
        }
    }
}


void GidWidget::viewDR()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CNode2 *n = (CNode2*)action->data().toLongLong();
        if (n) {
            beginDraw(n);

            if (n->node.typ == TIP_PR) {
                QString table = n->getTableMySQL();
//                QString title = n->getNameFullFile();
                QString title0 = "Дроссельные органы потребителей";


                QString fn = n->getFileName();
                QString title = QString("%1 %2 %3").arg(title0, n->getKod(),  n->node.name);

                if (fn != "") {
                    title = QString("%1 (%2)").arg(title    , fn);
                }

                info_gid(m_cxema.m_db, title, table, "DR_OUT", n->id, -1, pr_type_node  );
            }
        }
    }
}



bool vydel(CNode2* node);

void GidWidget::fragmentLine()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CLINE2 *ll = (CLINE2*) action->data().toLongLong();
        if (ll) {
            vydel(where(ll));
            repaint();
        }
    }
}

void GidWidget::fragmentNode()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CNode2 *n = (CNode2*)action->data().toLongLong();
        if (n) {
            vydel(n);
            repaint();
        }
    }
}


void GidWidget::viewZN(CNode2 *n)
{
     beginDraw(n);
    
    if (n->node.nZN == -1) {
        if (QMessageBox::question(this,  "", tr("Создать узел с заданным напором?")) == QMessageBox::Yes) {

//            QString q = QString("INSERT INTO %1 (nodeID) VALUES (%2)").arg("setPressNodes").arg(n->id);

            QSqlQuery query(m_cxema.m_db);

//            int id2 = ExecuteInsert(m_cxema.m_db, q);

            std::map<QString, QVariant> data;

            data["nodeID"] = (int)n->id;

//            int id2 = ExecuteInsert(m_cxema.m_db, q);

            int id2 = insertIntoDatabase(m_cxema.m_db, "setPressNodes", data);

            if (id2 == 0) {
                ErrorMessage(query);
                return;
            }
            else {
                n->node.nZN = id2;
                repaint();
            }
        }
    }
    else {
        QString table = "setPressNodes";
        QString title = findTableRusName("gid", table);
        info(m_cxema.m_db, title, table, n->id, -1, pr_type_node );
    }
}


void GidWidget::viewZN()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CNode2 *n = (CNode2*)action->data().toLongLong();
        if (n) {
            viewZN(n);
        }
    }
}

void GidWidget::viewUP()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CNode2 *n = (CNode2*)action->data().toLongLong();
        if (n) {
            beginDraw(n);

            if (n->node.nUP == -1) {
                if (QMessageBox::question(this,  "", tr("Создать узел подпитки?")) == QMessageBox::Yes) {

//                    QString q = QString("INSERT INTO %1 (nodeID) VALUES (%2)").arg("refillNodes").arg(n->id);

                    QSqlQuery query(m_cxema.m_db);

//                    int id2 = ExecuteInsert(m_cxema.m_db, q);

                    std::map<QString, QVariant> data;
                    data["nodeID"] = (int)n->id;
                    int id2 = insertIntoDatabase(m_cxema.m_db, "refillNodes", data);


                    if (id2 == 0) {
                        ErrorMessage(query);
                        return;
                    }
                    else {
                        n->node.nUP = id2;
                        repaint();
                    }
                }
            }

            if (n->node.nUP != -1) {
                QString table = "refillNodes";
                QString title = findTableRusName("gid", table);

                info(m_cxema.m_db, title, table, n->id, -1, pr_type_node );
            }
        }
    }
}

void GidWidget::delUP()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CNode2 *n = (CNode2*)action->data().toLongLong();
        if (n) {
            beginDraw(n);

            if (n->node.nUP != -1) {
                if (QMessageBox::question(this,  "", tr("Удалить узел подпитки?")) == QMessageBox::Yes) {

                    QString q = QString("DELETE FROM %1 WHERE nodeID=%2").arg("refillNodes").arg(n->id);

                    QSqlQuery query(m_cxema.m_db);

                    if (query_exec(m_cxema.m_db, query, q)) {
                        n->node.nUP = -1;
                        repaint();
                    }
                    else {
                        ErrorMessage(query);
                    }
                }
            }
        }
    }
}


void GidWidget::delZN()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CNode2 *n = (CNode2*)action->data().toLongLong();
        if (n) {
            beginDraw(n);

            if (n->node.nZN != -1) {
                if (QMessageBox::question(this,  "", tr("Удалить узел с заданным напором?")) == QMessageBox::Yes) {

                    QString q = QString("DELETE FROM %1 WHERE nodeID=%2").arg("setPressNodes").arg(n->id);

                    QSqlQuery query(m_cxema.m_db);

                    if (query_exec(m_cxema.m_db, query, q)) {
                        n->node.nZN = -1;
                        repaint();
                    }
                    else {
                        ErrorMessage(query);
                    }
                }
            }
        }
    }
}

void GidWidget::viewNodePodpis()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CNode2 *n = (CNode2*)action->data().toLongLong();
        if (n) {
            QString q = QString("update nodes set displaySign=%1 where id=%2").arg(n->node.isPodp).arg(n->id);

            QSqlQuery query(m_cxema.m_db);
            bool res = query_exec(m_cxema.m_db, query, q);

            if (!res) {
                ErrorMessage(query);
                return;
            }
            else {
                n->node.isPodp = !n->node.isPodp;
                repaint();
            }
        }
    }
}

void GidWidget::closePotr(CNode2 *n)
{
    if (n->node.typ == TIP_PR || n->node.typ == TIP_PO) {
        QString q = QString ("update %1 set consumerStateID=%2 where id=%3").arg(n->getTableMySQL()).arg(n->node.isOtkl ? 1 : 2).arg(n->node.id2);

        QSqlQuery query(m_cxema.m_db);
        bool res = query_exec(m_cxema.m_db, query, q);

        if (!res) {
            ErrorMessage(query);
            return;
        }
        else {
            n->node.isOtkl = !n->node.isOtkl;
            repaint();
        }
    }
}

void GidWidget::closePotr()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CNode2 *n = (CNode2*)action->data().toLongLong();
        if (n) {
            closePotr(n);
        }
    }
}




void GidWidget::viewLinePodpis()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CLINE2 *ll = (CLINE2*) action->data().toLongLong();
        if (ll) {
            CLine2 *line = bline(ll);
            QString q = QString("update linesobj set displaySign=%1 where id=%2 OR id=%3").arg(line->line.isPodp).arg(line->line.nomP).arg(line->line.nomO);

            QSqlQuery query(m_cxema.m_db);
            bool res = query_exec(m_cxema.m_db, query, q);

            if (!res) {
                ErrorMessage(query);
                return;
            }
            else {
                line->line.isPodp = !line->line.isPodp;
                repaint();
            }
        }
    }
}

/*

(1, "new_baza.uzel"),
(2, "new_baza.nasosn"),
(3, "new_baza.istoch"),
(4, "new_baza.real_potr"),
(5, "new_baza.general_potr")
*/



/*

TIP_US,  "US",  ST_US,   "",                       "Узел",                   "Ненагруженный узел", "US_OUT"},
TIP_PR,  "PR",  ST_PR,   "realConsumers",          "Потребитель реальный",   "Потребитель реальный", "PT_OUT"},
TIP_PO,  "PO",  ST_PO,   "generalizedConsumers",   "Потребитель обобщенный", "Потребитель обобщенный", "PT_OUT"},
TIP_IS,  "IS",  ST_IS,   "heatSources",            "Источник тепла",         "Источник тепла", "US_OUT"},
TIP_HS,  "HS",  ST_HS,   "pumpStations",           "Насосная станция",       "Насосная станция", "US_OUT"},
TIP_C3,  "C3",  ST_C3,   "threeWayValves",         "Клапан трехходовой",     "Клапан трехходовой", "US_OUT"},
TIP_US2, "US2", ST_US2,  "connectNodes",           "Узел",                   "Ненагруженный узел", "US_OUT"},
TIP_US_VN, "US_VN", "Узел внутр.", "new_baza.uzel_vn",            "Узел",                   "Ненагруженный внутренний", "US_OUT"},
*/




void  GidWidget::delText()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        int id = (int) action->data().toLongLong();
        if (id >= 0) {
            for (auto it = m_cxema.set_text.begin(); it != m_cxema.set_text.end(); ++it) {
                if (it->n == id) {
                    if (QMessageBox::question(this, "", tr("Удалить текст")) == QMessageBox::Yes) {
                        QString q = QString("delete from texts where id=%1").arg(it->n);
                        if (query_exec(m_cxema.m_db, q)) {
                            m_cxema.set_text.erase(it);
                        }
                    }
                    return;
                }
            }
        }
    }
}

void  GidWidget::editText()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        int id = (int) action->data().toLongLong();
        if (id >= 0) {
            for (auto it = m_cxema.set_text.begin(); it != m_cxema.set_text.end(); ++it) {
                if (it->n == id) {
                    bool ok;
                    QString text = it->text;
                    text = QInputDialog::getText(this,"", tr("Введите текст"), QLineEdit::Normal, text, &ok);
                    if (ok) {
//                        QString q = QString("update texts set label_text=:text where id=%1").arg(it->n);
                        QString q = QString("update texts set label_text=? where id=%1").arg(it->n);
                        QSqlQuery query(m_cxema.m_db);
                        query.prepare(q);
//                        query.b1indValue(":text", text);
                        query.addBindValue(text);
                        if (query_exec(m_cxema.m_db, query)) {
                            it->text = text;
                        }
                        else {
                        }
                    }
                    return;
                }
            }
        }
    }
}

QString mesto_col(int mesto)
{
    std::map<int, QString> map_pr = {
        //    elevatornuminst Номер элеватора
        //    diameternozzle Диаметр сопла элеватора

        {15, "diameternozzle"}, // Диаметр сопла элеватора
        
        
        {4, "b20"}, // Подпорная
        {5, "b14"}, // Вход теплового пункта
        {1, "b23"}, // Отопление
        {2, "b28"}, // Вентиляция (воздушного отопления)
        {3, "b32"}, // ГВС; закрытая
        {37, "b35"}, // На входе подающего трубопровода открытой ГВС
        {6, "b38"}, // Ограничительная в циркуляционном контуре ГВС
        {45, "b36"}, // Подпорно-циркуляционная в открытой ГВС
        //Задвижка, открыта/закрыта
        {7, "zd7"}, // Вход ТП
        {8, "zd8"}, // Выход ТП
        {27, "zd27"}, // Вход СО
        {48, "zd48"}, // Вход СВ
        {28, "zd28"}, // Вх.II ступен.ГВ
        //{83, "zd83"}, # Вх.II ступен.ГВ
        //{82, "zd82"}, # Вх.I ступен.ГВ
        //{84, "zd84"}, # Вх.I ступен.ГВ
        {36, "zd36"}, // ОткрГВ подача
        {38, "zd38"}, // ОткрГВ обратка
        {39, "zd39"}, // Летняя СО для открытой ГВС обратка
        {49, "zd49"}, // СО посл, предв
        {50, "zd50"}, // Летняя СО посл, предв
    };

    QString col = "";

    auto it = map_pr.find(mesto);
    if (it != map_pr.end()) {
        col = it->second;
    }
    return col;
}



QString get_b4_dest(int id);

void GidWidget::onPrAccepted()
{
    auto *dlg = qobject_cast<PropertyDial *>(sender());
    if (dlg) {
        CLINE2 *ll = (CLINE2*) dlg->getParam("l").toLongLong();;
        int pr_id = dlg->getParam("pr_id").toInt();

        if (ll && pr_id > 0) {
            CLine2 *l = bline(ll);
            if (l->line.typ == TIP_ZD) {
                int mesto = max(l->line.nomP, l->line.nomO);
                QString col = mesto_col(mesto);
                if (col != "") {
                    int sost = dlg->getIndex("sost");
                    if (sost == 1 || sost == 2) {
                        QString q = QString("UPDATE realConsumers SET %1=%2 WHERE nodeID=%3").arg(col).arg(sost).arg(pr_id);

                        if (query_exec(m_cxema.m_db, q)) {
                            l->line.pod.isOtkl = (sost == 2);
                            l->line.obr.isOtkl = (sost == 2);
                            repaint();
                        }
                    }
                }
            }

            else if (l->line.typ == TIP_DR || l->line.typ == TIP_EL) {
                int mesto = max(l->line.nomP, l->line.nomO);
                QString col = mesto_col(mesto);
                if (col != "") {
                    double diam = dlg->getValue("diam").toDouble();
                    QString q = QString("UPDATE realConsumers SET %1=%2 WHERE nodeID=%3").arg(col).arg(diam).arg(pr_id);

                    if (query_exec(m_cxema.m_db, q)) {
                        l->line.pod.diam = diam;
                        l->line.obr.diam = diam;
                        l->line.pod.dru_home = diam != 0 ? 1 : 0;
                        l->line.obr.dru_home = diam != 0 ? 1 : 0;
                        repaint();
                    }
                }
            }
        }
    }
}



void GidWidget::viewLineVnutr(CLINE2 *ll)
{
    CLine2 *l = bline(ll);
    int mesto = max(l->line.nomP, l->line.nomO);

    QString name = QString("%1 (%2)").arg(get_b4_dest(mesto)).arg(mesto);
    QString col = mesto_col(mesto);

    if (mesto > 470) {
        name = "Фиктивный участок теплопровода";
    }


    CNode2 *nc = m_cxema.m_graph->find(m_parent_id);

    if (!nc || nc->node.typ != TIP_PR) return; // Ошибка, такого быть не может

    QString val = "";

    if (col != "") {
        QString q = QString("SELECT %1 FROM realConsumers WHERE nodeID=%2").arg(col).arg(nc->id);
        val = readTableValue(m_cxema.m_db, q);
    }

    QString q = QString(
            "select ras, sopr from any2_out\n"
            "join nodes n on n.id=any2_out.nodeid \n"
            "join realconsumers rc on rc.nodeid =n.id\n"
            "where n.removed = 0\n"
            "and n.id=%1\n"
            "and mesto=%2\n"
            "and any2_out.calculationID = (select MAX(id) from calculation c where c.fileID=n.fileID)\n")
                    .arg(m_parent_id).arg(mesto);

    qDebug() << q;

    std::vector<QVariant> vv;

        if (!readTableValues(m_cxema.m_db, q, 2, vv)) {
        vv.push_back(0);
        vv.push_back(0);
    }

    if (l->line.typ == TIP_DR || l->line.typ == TIP_EL) {
        auto *dlg = new PropertyDial(this, "pr_dr", "", "shape", "id", -1, -1);

        std::map<QString, QVariant> map_val;

        map_val["diam"] = val.toDouble();
        map_val["sopr"] = vv[1].toDouble();
        map_val["ras"] = vv[0].toDouble();

        dlg->init_dialog(&m_cxema.m_db, name, "pr_dr", map_val);
        dlg->addParam("l",  QVariant((qlonglong)ll));
        dlg->addParam("pr_id",  QVariant(m_parent_id));
        connect(dlg, SIGNAL(accepted()), this, SLOT(onPrAccepted()));

        dlg->show();

    }
    else if (l->line.typ == TIP_ZD) {
        auto *dlg = new PropertyDial(this, "pr_zd", "", "shape", "id", -1, -1);

        std::map<QString, QVariant> map_val;

        map_val["sost"] = val.toInt();
        map_val["sopr"] = vv[1].toDouble();
        map_val["ras"] = vv[0].toDouble();

        dlg->init_dialog(&m_cxema.m_db, name, "pr_zd", map_val);
        dlg->addParam("l",  QVariant((qlonglong)ll));
        dlg->addParam("pr_id",  QVariant(m_parent_id));
        connect(dlg, SIGNAL(accepted()), this, SLOT(onPrAccepted()));

        dlg->show();
    }
    else {
        auto *dlg = new PropertyDial(this, "pr_any", "", "shape", "id", -1, -1);

        std::map<QString, QVariant> map_val;

        map_val["sopr"] = vv[1].toDouble();
        map_val["ras"] = vv[0].toDouble();

        dlg->init_dialog(&m_cxema.m_db, name, "pr_any", map_val);
        dlg->show();
    }

    return;
}

void GidWidget::viewLine(CLINE2 *ll, bool autodelete)
{
    if (ll) {
        CLine2 *l = bline(ll);
        beginDraw(ll);
        QString table = l->getTableMySQL();
 //       QString title = findTableRusName("gid", table);
        QString frn = where(l)->getFileName();
        QString title = l->getNameFull();

        CFPoint pt;
        if (isPR()) {
            viewLineVnutr(ll);
            return;
        }


        if (m_parent_id != 0 && m_vnutr_typ != 1 && m_vnutr_typ != 3) {  // Не потребитель и не старый внутренний
            
QString get_line_table_ps_vn(int typ);
            QString tn = get_line_table_ps_vn(l->line.typ);
            info(m_cxema.m_db, QString("%1 (%2)").arg(title, frn), tn, l->line.nomP, l->line.nomO, pr_type_any, "shape", "id", autodelete);
//            info(m_cxema.m_db, QString("%1 (%2)").arg(title, frn), table, l->line.nomP, l->line.nomO, pr_type_any);
        }
        else {
            info(m_cxema.m_db, QString("%1 (%2)").arg(title, frn), table, l->line.nomP, l->line.nomO, pr_type_line, "shape", "id", autodelete);
        }
    }
}

void GidWidget::viewLine()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CLINE2 *ll = (CLINE2*) action->data().toLongLong();
        if (ll) {
            viewLine(ll);
        }
    }
}

double getLengtn(CLINE2 *ll, bool ismap);


void GidWidget::setLength()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CLINE2 *ll = (CLINE2*) action->data().toLongLong();
        if (ll) {
//            double len = LengthG(n1->node.coord, n2->node.coord, ismap);
            double len = getLengtn(ll, true);

            if (QMessageBox::question(this, "", 
                QString("Установить длину выделенного участка %1 метров?")
                    .arg(len, 0, 'f', 2)
                ) == QMessageBox::Yes) {
                m_cxema.setLength(ll, true, true);
            }
        }
    }
}

void GidWidget::viewLinePts()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CLINE2 *ll = (CLINE2*) action->data().toLongLong();
        if (ll) {
            CLine2 *l = bline(ll);
            beginDraw(ll);

//            QString table = l->getTableMySQL();
//            QString title = findTableRusName("gid", table);
//            info_gid(m_cxema.m_db, title, table, l->line.nomP, l->line.nomO, pr_type_line);

            QString table = "pipeSections";
            QString title = findTableRusName("gid", table);
            info(m_cxema.m_db, title, table, l->line.pipeSectionID, -1, pr_type_any);
            }
    }
}


void GidWidget::viewLineCalc()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CLINE2 *ll = (CLINE2*) action->data().toLongLong();
        if (ll) {
            CLine2 *l = bline(ll);

            QString title = l->getNameFullFile();

            beginDraw(ll);
            QString table = l->getTableMySQL();
//            QString title = findTableRusName("gid", table);
            QString table_out = getOutTable(table);
            info_gid(m_cxema.m_db, title, table, table_out, l->line.nomP, l->line.nomO, pr_type_line);
        }
    }
}

QString getLineOtkr(int typ);

bool GidWidget::setState(CLINE2 *line, int state)
{
    CLine2 *l = bline(line);

    QString q;

    if (l->line.typ == TIP_UT) {
        q = QString("UPDATE %1 SET pipeSectStateIDflow=%2, pipeSectStateIDret=%3 WHERE ID=%4 OR ID=%5").arg(l->getTableMySQL()).arg(state).arg(state).arg(l->line.idP2).arg(l->line.idO2);
    }
    else {
        QString otkr = getLineOtkr(l->line.typ);
        if (otkr != "") {
            q = QString("UPDATE %1 SET %2=%3 WHERE ID=%4 OR ID=%5").arg(l->getTableMySQL()).arg(otkr).arg(state).arg(l->line.idP2).arg(l->line.idO2);
        }
    }

    //    q.Format("UPDATE %s SET state=%d WHERE ID=%d OR ID=%d",l->getTableMySQL(), state, l->line.idP2, l->line.idO2);

    QSqlQuery query(m_cxema.m_db);

       if (query_exec(m_cxema.m_db, query, q)) {
        bool otkl = (state == 2);
        l->line.pod.isOtkl = otkl;
        l->line.obr.isOtkl = otkl;

        repaint();
        return true;
    }
    return false;
}

// Закрыть линию


void GidWidget::closeLineVnutr(CLINE2 *ll)
{
    CLine2 *l = bline(ll);

    int pr_id = m_parent_id;

    if (l && l->line.typ == TIP_ZD) {
        int mesto = max(l->line.nomP, l->line.nomO);
        QString col = mesto_col(mesto);
        if (col != "") {
            int sost = l->line.pod.isOtkl ? 1 : 2;
            QString q = QString("UPDATE realConsumers SET %1=%2 WHERE nodeID=%3").arg(col).arg(sost).arg(pr_id);

            if (query_exec(m_cxema.m_db, q)) {
                l->line.pod.isOtkl = (sost == 2);
                l->line.obr.isOtkl = (sost == 2);
                repaint();
            }
        }
    }
}


void GidWidget::closeLine(CLINE2 *ll)
{
    CLine2 *l = bline(ll);
    int state = 1;

    CFPoint pt;
    if (isPR()) {
        closeLineVnutr(ll);
        return;
    }

    if ((!l->line.pod.isOtkl && l->line.nomP != -1) || (!l->line.obr.isOtkl && l->line.nomO != -1)) state = 2;
    setState(l, state);
    endDraw();
    repaint();
}

void GidWidget::closeLine()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CLINE2 *ll = (CLINE2*) action->data().toLongLong();
        closeLine(ll);
    }
}

// Сделать фрагмент активным

void GidWidget::activeFragmentLine()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CLINE2 *ll = (CLINE2*) action->data().toLongLong();
        if (ll) {
//            this->m_fileID = where(ll)->node.fileID;
            setFileID(where(ll)->node.fileID, false);
            repaint();
        }
    }
}

void GidWidget::activeFragmentNode()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CNode2 *n = (CNode2*)action->data().toLongLong();
        if (n) {
//            this->m_fileID = n->node.fileID;
            setFileID(n->node.fileID, false);
            repaint();
        }
    }
}

void GidWidget::ch_type_line()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CLINE2 *ll = (CLINE2*) action->data().toLongLong();
        ch_type_line(ll);
    }
}

void GidWidget::ch_type_node()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CNode2 *n = (CNode2*)action->data().toLongLong();
        ch_type_node(n);
    }
}

void GidWidget::delNode()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CNode2 *n = (CNode2*)action->data().toLongLong();
        delNode(n, true);
    }
}

void GidWidget::delLine()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CLINE2 *ll = (CLINE2*) action->data().toLongLong();
        delLine(ll, true);
    }
}

void GidWidget::moveLine()
{
    is_move1 = false;
    m_nRegim = gidrAction.aNodeMove;
    return;

    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CLINE2 *line = (CLINE2*) action->data().toLongLong();

        m_line_for_move = line;
        m_n_for_move = -11;
        regim = R_MOVE_LINE;
        is_move1 = false;

        repaint();
    }
}

double hausdorff_distance(CCoordList &cl1, CCoordList &cl2);
double frechet_distance(CCoordList &cl1, CCoordList &cl2);

bool isGeoNode(const QString & name)
{
    return name == "heat.pav_tep" 
        || name == "heat.pav_mag_tep" 
        || name == "heat.ctp" 
        || name == "heat.source"
        || name == "heat.ns_tep" 
        || name == "heat.vr_tep"
        ;
}

KlGeo findGeoNode(CFPoint pt, double md)
{
    list<KlGeo> geo_list;

    findXY(pt, md, geo_list);

    for (auto & gl: geo_list) {
        if (isGeoNode(gl.kls->nazv)) {
            return gl;
        }
    }

    findXY(pt, md, geo_list, true);

    for (auto & gl: geo_list) {
        if (isGeoNode(gl.kls->nazv)) {
            return gl;
        }
    }

    KlGeo klg;
    klg.kls = nullptr;
    klg.gl = nullptr;

    return klg;
}


int findGeoTube(const QString &name, const CLINE2 *ll, CCoordList &m_NP, double &dist_min,
    KlGeo &go_n1,
    KlGeo &go_n2,
    KlGeo &go_l

)
{
    go_n1.kls = nullptr;
    go_n2.kls = nullptr;
    go_l.kls = nullptr;
    
    int id = -1;
    
    Klassif *kls = m_kl_list.findKlN(name);
    if (kls) {
//        double dist_min = 1e20;
        dist_min = 1e20;

        CCoordList cl = bline(ll)->line.m_NP;
        cl.push_front(where(ll)->node.coord);
        cl.push_back(other(ll)->node.coord);

        CGeoObject *gl0;

        for (auto & gl: kls->geo4) {
            if (gl) {
//                double dist = frechet_distance(gl->cl, cl);
                double dist = hausdorff_distance(gl->cl, cl);

                if (dist < dist_min) {
                    dist_min = dist;
                    id = gl->nom;
                    gl0 = gl;

                    go_l.kls = kls;
                    go_l.gl = gl;
                }
                gl->text00000 = QString("%1").arg(dist);
            }
            nn += 1;
        }


        if (id != -1) {
            CFPoint p1 = gl0->cl.first();
            CFPoint p2 = gl0->cl.last();

            double md = 10;

            go_n1 = findGeoNode(p1, md);
            go_n2 = findGeoNode(p2, md);

            m_NP = gl0->cl;

            if (go_n1.kls) {
                m_NP.push_front(go_n1.gl->cl.centroid());
            }
            if (go_n2.kls) {
                m_NP.push_back(go_n2.gl->cl.centroid());
            }

            gl0->text00000 = QString("!!!%1").arg(dist_min);
        
        }

        return id;
    }

    return -1;
}


#include <cxema/undo.h>

void GidWidget::moveLineGeo()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CLINE2 *ll = (CLINE2*) action->data().toLongLong();
        if (ll) {
//            QString name = "heat.tr_tep";

            Klassif *kls_r = m_kl_list.findKlN("heat.tr_tep");
            Klassif *kls_m = m_kl_list.findKlN("heat.tr_mag_tep");
            Klassif *kls = nullptr;


            double dist_m = 1e10, dist_r = 1e10;
            int id_m, id_r;

            KlGeo node1, node2, line0;


            if (kls_r) {
                CCoordList m_NP;
                id_r = findGeoTube(kls_r->nazv, ll, m_NP, dist_r, node1, node2, line0);
            }
            if (kls_m) {
                CCoordList m_NP;
                id_m = findGeoTube(kls_m->nazv, ll, m_NP, dist_m, node1, node2, line0);
            }

            if (dist_r < dist_m) {
                kls = kls_r;
            }
            else {
                kls = kls_m;
            }

            if (kls) {
                CCoordList m_NP;
                double dist;
                int id = findGeoTube(kls->nazv, ll, m_NP, dist, node1, node2, line0);

                if (id != -1) {

                    CCoordList cl = bline(ll)->line.m_NP;
                    cl.push_front(where(ll)->node.coord);
                    cl.push_back(other(ll)->node.coord);

                    QString text = cl.saveStr();

                    CNode2 *n1 = where(ll);
                    CNode2 *n2 = other(ll);

                    CFPoint pt1 = m_NP.first();
                    CFPoint pt2 = m_NP.last();

                    if (Length(pt1, n1->node.coord) > Length(pt1, n2->node.coord)) {
                        CFPoint pt0 = pt1;
                        pt1 = pt2;
                        pt2 = pt0;
                        m_NP.reverse();
                    }

                    change_group_start(m_cxema.m_db, "Перенос участка");

//                    addRemoved(m_cxema.m_db, m_user, CH_T_BEGIN, 0, "Участок");

                    setXY(n1, pt1);
                    setXY(n2, pt2);

                    m_NP.pop_back();
                    m_NP.pop_front();

                    m_NP.remove(CFPoint(C_SPR, C_SPR));

                    setCoord(ll, m_NP);

                    change_group_end(m_cxema.m_db);
//                    addRemoved(m_cxema.m_db, m_user, CH_T_END, 0, "Участок");


                    CLine2 *l = bline(ll);


                    if (l->line.nomP > 0) {
//                        addRemoved(m_cxema.m_db, m_user, CH_T_MOVE_LINE_GEO, l->line.nomP, text);
                    }
                    if (l->line.nomO > 0 && l->line.nomO != l->line.nomP) {
//                        addRemoved(m_cxema.m_db, m_user, CH_T_MOVE_LINE_GEO, l->line.nomO, text);
                    }


                    repaint();
                }


//                endDraw();
//                m_CurKlassif = kls;
//                m_CurGeoObject = kls->getGeoObjectById(id);

//                QMessageBox::warning(this, "", QString("dist=%1 id=%2 nn=%3").arg(dist_min).arg(id).arg(nn));
            }



/*
//            double len = LengthG(n1->node.coord, n2->node.coord, ismap);
            double len = getLengtn(ll, true);

            if (QMessageBox::question(this, "", 
                QString("Установить длину выделенного участка %1 метров?")
                    .arg(len, 0, 'f', 2)
                ) == QMessageBox::Yes) {
                m_cxema.setLength(ll, true, true);
            }
*/
        }
    }
}



void GidWidget::moveLine1()
{
    is_move1 = true;
    m_nRegim = gidrAction.aNodeMove;

    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CLINE2 *line = (CLINE2*) action->data().toLongLong();
        
        if (bline(line)->line.nomP == -1 || bline(line)->line.nomO == -1) {
            is_move1 = false;
        }
    }
}

bool is_onlyline(const QString & tn);

void GidWidget::viewGeo(KlGeo *klgeo)
{
    endDraw();
    m_CurKlassif = klgeo->kls;
    m_CurGeoObject = klgeo->gl;
    CFPoint pt;

    CLINE2 *line = nullptr;

    if (is_onlyline(klgeo->kls->nazv)) {
        double delta = geom.masx*D5;
        pt = klgeo->gl->cl.first();
        line = m_cxema.graph()->findLine( m_parent_id, pt, delta);
    }

    if (line) {
        CLine2 *l = bline(line);
        QString table = l->getTableMySQL();
        info_with_line(*klgeo->kls->m_db, klgeo->kls->RusName(), klgeo->kls->nazv, klgeo->gl->nom, -1, table, l->line.nomP, l->line.nomO, pr_type_geo, klgeo->kls->shape, klgeo->kls->id);
    }
    else {
        info(*klgeo->kls->m_db, klgeo->kls->RusName(), klgeo->kls->nazv, klgeo->gl->nom, -1, pr_type_geo, klgeo->kls->shape, klgeo->kls->id);
    }
}


void GidWidget::markGeo()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        KlGeo *klgeo = (KlGeo *) action->data().toLongLong();

        if (klgeo) {
            klgeo->gl->mark = !klgeo->gl->mark;
            repaint();
        }
    }
}


void GidWidget::viewGeo()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        KlGeo *klgeo = (KlGeo *) action->data().toLongLong();

        if (klgeo) {
            viewGeo(klgeo);
        }

//        QMessageBox::about(this, "!", QString(tr("%1 %2")).arg(klgeo->kls->nazv).arg(klgeo->gl->nom));
    }
}

bool isDeletable(QSqlDatabase &db, const QString & fn1, int n, bool isMessage = true);

bool GidWidget::delGeoObj(const QString & tn, int id, bool ask)
{
//   if (!strcmp(tn, "indikator_korrozii")) {
//      return korrozia_del(tn, id);
//   }
    QString name = tn;

    if (!isEditGeo(tn, true)) {
        return false;
    }
    
    Klassif *kls = m_kl_list.findKlN(name);
    if (kls) {
        name = kls->RusName() ;
    }


    if (!isDeletable(*kls->m_db, tn, id, true)) return false;

    int ret = QMessageBox::Yes;

    if (ask) {
        QString str = QString(tr("Удалить %1 %2 ?")).arg(name).arg(id);

        ret = QMessageBox::question(this,  tr("Подтвердить удаление"), str);
    }

    if (ret == QMessageBox::Yes) {
        bool b = m_geo.delGeoObj(tn, id);

        if (b) {
            m_CurGeoObject = nullptr;
            repaint();
            return true;
        }
    }
    return false;
}

void GidWidget::delGeo()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        KlGeo *klgeo = (KlGeo *) action->data().toLongLong();
        endDraw();
        m_CurKlassif = klgeo->kls;
        m_CurGeoObject = klgeo->gl;

        delGeoObj(klgeo->kls->nazv, klgeo->gl->nom);
    }
}

void isprClLoc6(CCoordList &cl);
QString transl(const QString & s0);


int copyZdan(const QString &name1, int id, const QString & name2)
{
    Klassif* kls = m_kl_list.findKlN(name1);
    if (!kls) return -1;

    Klassif* kls2 = m_kl_list.findKlN(name2);
    if (!kls2) return -1;

    int id2 = -1;

//    CAdoFile* ado = kls->m_ado;

    CGeoObject* geo = kls->getGeoObjectById(id);

    if (!geo)  return -1;

    QSqlQuery query(*kls->m_db);
    query.setForwardOnly(true);

    QString q = QString("SELECT * FROM %1 WHERE ID=%2").arg(name1).arg(id);

    query_exec(*kls->m_db, query, q);

    if (query.next()) {

        CCoordList cl = geo->cl;

        QString gr = query.value(transl("Город")).toString();
        QString mk = query.value(transl("ID_ADR_MAS")).toString();
        QString st = query.value(transl("STREET_NAM")).toString();
        QString dm = query.value(transl("NUMBER_1")).toString();

        if (st == "" && dm == "") {
            gr = query.value(transl("Город")).toString();
            mk = query.value(transl("Микрорайон")).toString();
            st = query.value(transl("Улица")).toString();
            dm = query.value(transl("Дом")).toString();
        }

/*
        q = QString("INSERT INTO %1 (%2,%3,%4,%5,shape) VALUES ('%6','%7','%8','%9', %10)")
                .arg(
            transl(name2),
            transl("Город"),
            transl("Микрорайон"),
            transl("Улица"),
            transl("Дом"),
            gr, mk, st, dm,
            cl.getWKT(kls->loc, 0));

*/
//        id2 = ado_executeInsert(kls2->m_ado, q, &affected);

//        id2 = execInsertQ(*kls2->m_db, q);

        std::map<QString, QVariant> data;

        if (name2 != "zdaniya_2") {
            data[transl("Город")] = gr;
            data[transl("Микрорайон")] = mk;
            data[transl("Улица")] = st;
            data[transl("Дом")] = dm;
        }

        data[kls2->shape] = cl.getWKT(kls2->loc, 0);

        id2 = insertIntoDatabase(*kls2->m_db, name2, data, kls2->shape);

        if (id2 > 0) {
            CGeoObject* g = new CGeoObject(id2, cl);
            kls2->geo4.push_back(g);
        }
    }
    return id2;
}



void GidWidget::createTU()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        KlGeo *klgeo = (KlGeo *) action->data().toLongLong();
        endDraw();

//        m_geo_for_move = *klgeo;
//        m_cl_for_move = klgeo->gl->cl;


        copyZdan(klgeo->kls->nazv, klgeo->gl->nom, transl("Здания ТУ"));
        repaint();
    }
}

void GidWidget::createAlseco()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        KlGeo *klgeo = (KlGeo *) action->data().toLongLong();
        endDraw();

        copyZdan(klgeo->kls->nazv, klgeo->gl->nom, "zdaniya_2");
        repaint();
    }
}





void GidWidget::ismGeo()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        KlGeo *klgeo = (KlGeo *) action->data().toLongLong();
        endDraw();
//        m_CurKlassif = klgeo->kls;
//        m_CurGeoObject = klgeo->gl;

        m_geo_for_move = *klgeo;
        m_cl_for_move = klgeo->gl->cl;

        if (klgeo->kls->loc == LOC6) {
            isprClLoc6(m_cl_for_move);
        }
        m_nRegim = nullptr;
        repaint();
//        QMessageBox::about(this, "!", QString(tr("%1 %2")).arg(klgeo->kls->nazv).arg(klgeo->gl->nom));
    }
}

bool readNodeVn(CGraph2 *m_graph, QSqlDatabase& db, int id_table, int id)
{
//    CGraph2 *m_graph = cxema->m_graph;

    QString q = QString("select id, nodename, internalnodeid, externalSignID, x, y, displaysign from new_baza.uzel_vn n where n.typeid=%1 and n.internalnodeid=%2").arg(id_table).arg(id);

    QSqlQuery query(db);
    query.setForwardOnly(true);

    query_exec(db, query, q);
    
    while (query.next()) {
        long id = query.value("id").toInt();
        QString name = query.value("nodename").toString();
        int internalNodeID = query.value("internalNodeID").toInt();
        int externalSignID = query.value("externalSignID").toInt();
        double x = query.value("x").toDouble();
        double y = query.value("y").toDouble();
        int displaysign = query.value("displaysign").toInt();

//        CGraph2 *m_graph = new CGraph2(nullptr);

        int typ = TIP_US_VN;

        CNode2* node2 = m_graph->find_ins(id);
        if (node2) {
            node2->node.node_name = CNodeName("", name, S_OBA);
            node2->node.name = name;
            node2->node.internalNodeID = internalNodeID;
            node2->node.coord.x = x;
            node2->node.coord.y = y;
            node2->node.typ = typ;

            node2->node.externalSignID = externalSignID;
            node2->node.id_kod = 1;
            node2->node.fileID = 1;
            node2->node.isPodp = !displaysign;
        }
    }

    QString q2 = QString("select id, nodename, internalnodeid , x, y, displaysign from new_baza.uzel_vn n where n.typeid=%1 and n.internalnodeid=%2").arg(id_table).arg(id);


    q2 = QString(
"select l.id, l.nodeid1, l.nodeid2, l.coords from new_baza.line_vn l\n"
"join new_baza.uzel_vn n1 on l.nodeid1 = n1.id\n"
"where n1.typeid=%1 and n1.internalnodeid=%2").arg(id_table).arg(id);

    q2 = QString(
"select l.id, l.typ, l.nodeID1, l.nodeID2, l.coords from (\n"
"select id, %1 as typ, nodeID1, nodeID2, coords from new_baza.line_vn \n"
"union select id, %2, nodeID1, nodeID2, coords from new_baza.ns_vn\n"
"union select id, %3, nodeID1, nodeID2, coords from new_baza.rd_vn\n"
"union select id, %4, nodeID1, nodeID2, coords from new_baza.rr_vn\n"
"union select id, %5, nodeID1, nodeID2, coords from new_baza.zd_vn\n"
"union select id, %6, nodeID1, nodeID2, coords from new_baza.ok_vn\n"
") l\n"
"join new_baza.uzel_vn n1 on l.nodeid1 = n1.id\n"
"where n1.typeid=%7 and n1.internalnodeid=%8")
.arg(TIP_UT)
.arg(TIP_HC)
.arg(TIP_RD)
.arg(TIP_RR)
.arg(TIP_ZD)
//.arg(TIP_ZD2)
//.arg(TIP_BP)
//.arg(TIP_DR)
//.arg(TIP_EL)
//.arg(TIP_RO)
//.arg(TIP_TO)
//.arg(TIP_KU)
//.arg(TIP_RP)
.arg(TIP_OK)
.arg(id_table).arg(id);



    query_exec(db, query, q2);

    while (query.next()) {
        long id = query.value("id").toInt();
        long nodeID1 = query.value("nodeID1").toInt();
        long nodeID2 = query.value("nodeID2").toInt();
        long typ = query.value("typ").toInt();
        QString s_coord = query.value("coords").toString();

        CCoordList NP;
        NP.loadStr(s_coord.toLatin1().data());

        CNode2 *n1 = m_graph->find(nodeID1);
        CNode2 *n2 = m_graph->find(nodeID2);

        CLine2* line = m_graph->insert_line(n1, n2, NP, true);
        if (line) {
            line->line.typ = typ;
            line->line.nomP = id;
        }
    }


    return true;
}


void GidWidget::vnutrGeo()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        KlGeo *klgeo = (KlGeo *) action->data().toLongLong();

        int id = klgeo->gl->nom;
        QString tn = klgeo->kls->nazv;

        QSqlDatabase *db = klgeo->kls->m_db;

        QString q = readQ("sql5/vn.sql");
        q = QString("SELECT id FROM (%1) __ WHERE name='%2'").arg(q, tn);

        int id_table = readTableInt(*db, q);

        m_cxema.m_graph_old = m_cxema.m_graph;
        m_cxema.m_graph = new CGraph2(&m_cxema);

        if (id_table == 4) {
            QString schemeNum = readTableValue(*db, QString("select schemenum from %1 where id=%2").arg(tn).arg(id));

void init_b5(CGraph2 *m_graph, int id, const QString & schemeNum);
            init_b5(m_cxema.m_graph, id, schemeNum);
            m_vnutr_typ = 3;
        }
        else {
            readNodeVn(m_cxema.m_graph, *db, id_table, id);
            m_vnutr_typ = 2;
        }

        m_parent_id = id;
        geom_old = geom;
        CFRect rect = m_cxema.graph()->getBorder(m_parent_id);
    //    SetBorder(rect);
        geom.dmas = 5;
        ChMas(rect);
        repaint();
    }
}



bool setGeoCoord(QSqlDatabase &db, Klassif *kls, int id, CCoordList &cl);


void GidWidget::naprGeo()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        KlGeo *klgeo = (KlGeo *) action->data().toLongLong();

        if (klgeo->kls->label_angle) {
            double angle = klgeo->gl->cl.angle;

            klgeo->gl->cl.angle = (klgeo->gl->cl.angle + 180);
            while (klgeo->gl->cl.angle > 360) klgeo->gl->cl.angle -= 360;

            if (!setGeoCoord(m_cxema.m_db, klgeo->kls, klgeo->gl->nom, klgeo->gl->cl )) {
                klgeo->gl->cl.angle = angle;
            }
        }
        repaint();
    }
}



void GidWidget::addNodeMenuItem(QMenu * subMenu, CNode2 *n, const QString & text, const char* slot_name)
{
    QAction *action = new QAction(text,this);
    action->setData((qlonglong)n);
    connect(action ,SIGNAL(triggered()),this,slot_name);
    connect(action ,SIGNAL(hovered()),this,SLOT(viewNode2()));
    subMenu->addAction(action);
}

void GidWidget::addLineMenuItem(QMenu * subMenu, CLINE2 *l, const QString & text, const char* slot_name)
{
    QAction *action = new QAction(text,this);
    action->setData(QVariant((qlonglong)l));
    connect(action ,SIGNAL(triggered()),this,slot_name);
    connect(action ,SIGNAL(hovered()),this,SLOT(viewLine2()));
    subMenu->addAction(action);
}

void GidWidget::addGeoMenuItem(QMenu * subMenu, KlGeo &kg, const QString & text, const char* slot_name)
{
    QAction *action = new QAction(text,this);
    action->setData(QVariant((qlonglong)&kg));
    connect(action ,SIGNAL(triggered()),this,slot_name);
    connect(action ,SIGNAL(hovered()),this,SLOT(viewGeo2()));
    subMenu->addAction(action);
}


void GidWidget::vydMag()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CLINE2 *ll = (CLINE2*) action->data().toLongLong();
        if (ll) {
            main_window->m_ms->vydMag(bline(ll)->line.mag);
            repaint();
        }
    }
}

void GidWidget::vydMS()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CLINE2 *ll = (CLINE2*) action->data().toLongLong();
        if (ll) {
            main_window->m_ms->vydMS(bline(ll)->line.ms);
            repaint();
        }
    }
}

void GidWidget::vydRS()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CLINE2 *ll = (CLINE2*) action->data().toLongLong();
        if (ll) {
            main_window->m_rs->vydRS(bline(ll)->line.rs);
            repaint();
        }
    }
}

void GidWidget::RButMenu(QPoint qpt, CFPoint pt)
{
//    double md = geom.mas * D5 / mas_otn;
    double md = geom.masx * D5;

    std::list<CNode2 *> lst_node = m_cxema.m_graph->find_node_list(m_parent_id, pt, md);

    QString s;
    QMenu *menu = new QMenu( this);

    m_line_for_move = nullptr;
    m_geo_for_move.kls = nullptr;

    bool is_node = false;
    bool is_line = false;
    bool is_geo = false;
    bool is_text = false;

//    QAction *action = nullptr;


    bool is_pr = false;

    CNode2 *nc = m_cxema.m_graph->find(m_parent_id);
    if (nc && nc->node.typ == TIP_PR) is_pr = true;

    QPointF qpt2 = mapFromGlobal( qpt );

    for (auto & txt: m_cxema.set_text) {
        if (txt.rect.contains(qpt2) ) {
            QAction *action = new QAction(tr("Редактировать текст"),this);
            action->setData((qlonglong)txt.n);
            connect(action ,SIGNAL(triggered()),this, SLOT(editText()));

            menu->addAction(action);
            action = new QAction(tr("Удалить текст"),this);
            action->setData((qlonglong)txt.n);
            connect(action ,SIGNAL(triggered()),this, SLOT(delText()));

            menu->addAction(action);
            is_text = true;
        }
    }

    if (!is_text) {
        for (CNode2* n: lst_node) {
            is_node = true;

            QMenu *subMenu = menu->addMenu(n->getNameFull());


            if (is_pr) {  // Смотрим внутри потребителя
                addNodeMenuItem(subMenu, n, QString(tr("Свойства объекта %1 %2")).arg(n->getTable(), n->getName()), SLOT(viewNode()));
                continue;
            }
            
            
            if (n->node.nZN != -1) {
                addNodeMenuItem(subMenu, n, tr("Узел с заданным напором"), SLOT(viewZN()));
                if (isEditGid()) {
                    addNodeMenuItem(subMenu, n, tr("Удалить узел с заданным напором"), SLOT(delZN()));
                }
                subMenu->addSeparator();
            }

            if (n->node.nUP != -1) {
                addNodeMenuItem(subMenu, n, tr("Узел подпитки"), SLOT(viewUP()));
                if (isEditGid()) {
                    addNodeMenuItem(subMenu, n, tr("Удалить узел подпитки"), SLOT(delUP()));
                }
                subMenu->addSeparator();
            }


            addNodeMenuItem(subMenu, n, QString(tr("Свойства объекта %1 %2")).arg(n->getTable(), n->getName()), SLOT(viewNode()));

            if (n->node.typ == TIP_IS) {
                addNodeMenuItem(subMenu, n, tr("Температурный график"), SLOT(viewTG()));
            }
           

            addNodeMenuItem(subMenu, n, tr("Нагрузки"), SLOT(viewNodeNagr()));
            addNodeMenuItem(subMenu, n, tr("Результат расчета"), SLOT(viewNodeCalc()));
        
            if (n->node.typ == TIP_PR) {
                addNodeMenuItem(subMenu, n, tr("Результат расчета дроссельных органов"), SLOT(viewDR()));
            }
            subMenu->addSeparator();
            addNodeMenuItem(subMenu, n, tr("Фрагмент сети"), SLOT(fragmentNode()));

    //        menu.AppendMenu(MF_SEPARATOR);
    //        menu.AppendMenu(MF_STRING, ID_POPUP_VYD, _TR("Фрагмент сети"));

            if (isEditGid()) {
                subMenu->addSeparator();
                addNodeMenuItem(subMenu, n, tr("Изменить тип объекта"), SLOT(ch_type_node()));
                if (!UserRight::isNoDelete()) {
                    addNodeMenuItem(subMenu, n, tr("Удалить"), SLOT(delNode()));
                }
                addNodeMenuItem(subMenu, n, tr("Перенести"), SLOT(moveLine()));
            }

            QString s;
            if (n->node.isPodp) {
                s = tr("Не показывать надпись");
            }
            else {
                s = tr("Показывать надпись");
            }

            addNodeMenuItem(subMenu, n, s, SLOT(viewNodePodpis()));
            subMenu->addSeparator();

            if (n->node.nZN == -1) {
                addNodeMenuItem(subMenu, n, tr("Узел с заданным напором"), SLOT(viewZN()));
            }

            if (n->node.nUP == -1) {
                addNodeMenuItem(subMenu, n, tr("Узел подпитки"), SLOT(viewUP()));
            }

            subMenu->addSeparator();

            if (n->node.typ != TIP_PO) {
                if (n->node.isP || n->node.typ == TIP_PR) {
                    addNodeMenuItem(subMenu, n, tr("Открыть внутреннюю схему"), SLOT(viewKam()));
                }
                else {
                   if (isEditGid()) {
                        addNodeMenuItem(subMenu, n, tr("Создать внутреннюю схему"), SLOT(viewKam()));
                    }
                }
            }

            if (n->node.isP && n->node.typ != TIP_PR) {
               if (isEditGid()) {
                   if (!UserRight::isNoDelete()) {
                       addNodeMenuItem(subMenu, n, tr("Удалить внутреннюю схему"), SLOT(delKam()));
                   }
               }
            }

            if (n->node.typ == TIP_PR || n->node.typ == TIP_PO) {
                subMenu->addSeparator();

                if (n->node.isOtkl) {
                    s = tr("Открыть потребитель");
                }
                else {
                    s = tr("Закрыть потребитель");
                }

                if (isEditGid()) {
                    addNodeMenuItem(subMenu, n, s, SLOT(closePotr()));
                }
            }

            if (this->m_fileID != n->node.fileID) {
                subMenu->addSeparator();
                addNodeMenuItem(subMenu, n, QString(tr("Сделать активным %1")).arg(n->getFileName()), SLOT(activeFragmentNode()));
            }
        }

        if (lst_node.size() == 0) {
            std::list<CLINE2 *> lst_line = m_cxema.m_graph->find_line_list(m_parent_id, pt, geom.masx * D5 / mas_otn);

            for (CLINE2 *l: lst_line) {
                is_line = true;

                QString po = "";
                if (bline(l)->line.nomP != -1 && bline(l)->line.nomO == -1) po = tr("подающий");
                if (bline(l)->line.nomP == -1 && bline(l)->line.nomO != -1) po = tr("обратный");
                if (po != "") po = " " + po;

                QMenu *subMenu = menu->addMenu(bline(l)->getNameFull() + po);


                if (is_pr) {  // Смотрим внутри потребителя
                    addLineMenuItem(subMenu, l, QString(tr("Свойства объекта %1 %2 %3")).arg(bline(l)->getTable(), bline(l)->getName(), po), SLOT(viewLine()));
                    if (isEditGid()) {
                        if (bline(l)->line.typ == TIP_ZD) {
                            if ((bline(l)->line.nomP != -1 && bline(l)->line.pod.isOtkl) || (bline(l)->line.nomO != -1 && bline(l)->line.obr.isOtkl)) {
                                addLineMenuItem(subMenu, l, QString(tr("Открыть %1 %2")).arg(bline(l)->getTable(), bline(l)->getName()), SLOT(closeLine()));
                            }
                            else {
                                addLineMenuItem(subMenu, l, QString(tr("Закрыть %1 %2")).arg(bline(l)->getTable(), bline(l)->getName()), SLOT(closeLine()));
                            }
                        }
                    }
                    continue;
                }

                addLineMenuItem(subMenu, l, QString(tr("Свойства объекта %1 %2 %3")).arg(bline(l)->getTable(), bline(l)->getName(), po), SLOT(viewLine()));
                addLineMenuItem(subMenu, l, tr("Результат расчета"), SLOT(viewLineCalc()));
    //            addLineMenuItem(subMenu, l, tr("Участок ПТС"), SLOT(viewLinePts()));
                subMenu->addSeparator();
                addLineMenuItem(subMenu, l, tr("Фрагмент сети"), SLOT(fragmentLine()));

                if (isEditGid()) {
                    subMenu->addSeparator();
                    addLineMenuItem(subMenu, l, tr("Изменить тип объекта"), SLOT(ch_type_line()));
                    if (!UserRight::isNoDelete()) {
                        addLineMenuItem(subMenu, l, tr("Удалить"), SLOT(delLine()));
                    }
                    addLineMenuItem(subMenu, l, tr("Передвинуть линию"), SLOT(moveLine()));

            
                    if (bline(l)->line.nomP > 0 && bline(l)->line.nomO > 0 ) {    
                        addLineMenuItem(subMenu, l, tr("Разъединить линию"), SLOT(moveLine1()));
                    }

        //            addLineMenuItem(subMenu, l, tr("Создать узел"), SLOT(delLine()));
                    if (bline(l)->line.typ == TIP_UT) {
                        addLineMenuItem(subMenu, l, tr("Установить длину"), SLOT(setLength()));
                    }

                    if (bline(l)->line.isPodp) {
                        s = tr("Не показывать надпись");
                    }
                    else {
                        s = tr("Показывать надпись");
                    }
                    addLineMenuItem(subMenu, l, s, SLOT(viewLinePodpis()));

    //            addLineMenuItem(subMenu, l, tr("Создать узел"), SLOT(delLine()));

                    subMenu->addSeparator();

                    if (bline(l)->line.typ == TIP_UT) {
                        addLineMenuItem(subMenu, l, tr("Передвинуть по карте"), SLOT(moveLineGeo()));
                        subMenu->addSeparator();
                    }
                }


                if (bline(l)->line.mag > 0) addLineMenuItem(subMenu, l, tr("Выделить магистраль"), SLOT(vydMag()));
                if (bline(l)->line.ms > 0) addLineMenuItem(subMenu, l, tr("Выделить Участок МС"), SLOT(vydMS()));
                if (bline(l)->line.rs > 0) addLineMenuItem(subMenu, l, tr("Выделить Участок РС"), SLOT(vydRS()));

                if (bline(l)->line.mag > 0 || bline(l)->line.ms > 0 || bline(l)->line.rs > 0) {
                    subMenu->addSeparator();
                }


                if (isEditGid()) {
                    if ((bline(l)->line.nomP != -1 && bline(l)->line.pod.isOtkl) || (bline(l)->line.nomO != -1 && bline(l)->line.obr.isOtkl)) {
                        addLineMenuItem(subMenu, l, QString(tr("Открыть %1 %2")).arg(bline(l)->getTable(), bline(l)->getName()), SLOT(closeLine()));
                    }
                    else {
                        addLineMenuItem(subMenu, l, QString(tr("Закрыть %1 %2")).arg(bline(l)->getTable(), bline(l)->getName()), SLOT(closeLine()));
                    }
                }

                if (this->m_fileID != where(l)->node.fileID) {
                    subMenu->addSeparator();
                    addLineMenuItem(subMenu, l, QString(tr("Сделать активным %1")).arg(where(l)->getFileName()), SLOT(activeFragmentLine()));
                }
            }
        }

        if (m_bIsGeo) {
//            void findXY(CFPoint p, double md, list<KlGeo> & geo_list);
            findXY(pt, md, geo_list);

            bool isPoint(int loc);
            bool is1 = false;
            bool is2 = false;

            for (int lt : {1, 2, 3}) {
                for (KlGeo &kg: geo_list) {
                    if (!kg.kls->ins) continue;
                    is_geo = true;
                    bool yes = false;

                    if (lt == 1) {
                        yes = isPoint(kg.kls->loc);
                        if (yes) is1 = true;
                    }
                    else if (lt == 2) {
                        yes = !is1 && kg.kls->loc == LOC2;
                        if (yes) is2 = true;
                    }
                    else if (lt == 3) {
                        yes = !is1 && !is2 && kg.kls->loc == LOC3;
                    }

                    if (yes) {
                        QMenu *subMenu = menu->addMenu(QString("%1 %2").arg(kg.kls->RusName()).arg(kg.gl->nom));

                        addGeoMenuItem(subMenu, kg, tr("Свойства"), SLOT(viewGeo()));
                        if (!UserRight::isNoDelete()) {
                            addGeoMenuItem(subMenu, kg, tr("Удалить"), SLOT(delGeo()));
                        }

                        if (isEditGeo(kg.kls->nazv, false)) {
                            addGeoMenuItem(subMenu, kg, tr("Передвинуть"), SLOT(ismGeo()));
                        
                            Klassif* kls2 = m_kl_list.findKlN(transl("Здания ТУ"));
                            if (kls2 && kg.kls->loc == LOC3 && kg.kls != kls2) {
                                addGeoMenuItem(subMenu, kg, tr("Создать Техническое условие"), SLOT(createTU()));
                            }

                            Klassif* kls_als = m_kl_list.findKlN("zdaniya_2");
                            if (kls_als && kg.kls->loc == LOC3 && kg.kls != kls_als) {
                                addGeoMenuItem(subMenu, kg, tr("Создать Здание с нагрузками АЛСЕКО"), SLOT(createAlseco()));
    //                            addGeoMenuItem(subMenu, kg, tr("Добавить Объект (МЖД) АЛСЕКО"), SLOT(addAlseco1()));
    //                            addGeoMenuItem(subMenu, kg, tr("Добавить Объект (Прочие) АЛСЕКО"), SLOT(addAlseco2()));
                            }

                        }


                        if (kg.kls->nazv == "zdaniya_2") {
//                            menu.AppendMenu(MF_STRING, ID_POPUP_GEO_MARK, _TR("Выделить объект"));
                            QString txt;
                            if (kg.gl->mark) {
                                txt = tr("Снять выделение");
                            }
                            else {
                                txt = tr("Выделить объект");
                            }


                            addGeoMenuItem(subMenu, kg, txt, SLOT(markGeo()));
                        }

                        
                        if (kg.kls->nazv == "new_baza.real_potr" || kg.kls->nazv == "new_baza.uzel" || kg.kls->nazv == "new_baza.nasosn" || kg.kls->nazv == "new_baza.istoch") {
                            addGeoMenuItem(subMenu, kg, tr("Внутренняя схема"), SLOT(vnutrGeo()));
                        }
                        if (kg.kls->loc == LOC6 || kg.kls->label_angle) {
                            addGeoMenuItem(subMenu, kg, tr("Развернуть"), SLOT(naprGeo()));
                        }
                    }
                }
            }
        }

        if (!is_node && !is_line && !is_geo) {

    #if 0
            if (m_clExpl.size() > 0) {
                action = new QAction(tr("Снять выделение"),this);
    //            menu.AppendMenu(MF_STRING, ID_POPUP_GEO_SHOW, _TR("Снять выделение"));
                menu->addAction(action);
                menu->addSeparator();
            }
    #endif
            if (is_pr) {
                menu->addAction(gidrAction.aZoom); // Масштабировать
//                menu->addAction(gidrAction.aKam);  // Внешняя схема / Внутренняя схема
                if (IsMain()) {
                    menu->addAction(gidrAction.aKam);  // Внешняя схема / Внутренняя схема
                }
                else {
                    menu->addAction(gidrAction.aKamVnesh);  // Внешняя схема / Внутренняя схема
                }


            }
            else {
                menu->addAction(gidrAction.aZoom); // Масштабировать
                menu->addSeparator();
                if (isEditGid()/* || isEditPS()*/) {
                    menu->addAction(gidrAction.aNovisual);  // Системные параметры...
                    if (isEditGid()) {
                        menu->addSeparator();
                        menu->addAction(gidrAction.aNode); // Установить узел\tN
                        menu->addAction(gidrAction.aLine); // Линейный объект
                        menu->addAction(gidrAction.aNodeMove1);  // Разъединенить линии
                        menu->addAction(gidrAction.aRename); // Изменить тип объекта
                        if (!UserRight::isNoDelete()) {
                            menu->addAction(gidrAction.aDel); // Удалить объект\tD
                        }
                        menu->addAction(gidrAction.aNodeMove); // Перенести объект
                    }
                    menu->addSeparator();

                    menu->addAction(gidrAction.aPopupPodpOn); // Показать скрытые надписи
                    menu->addAction(gidrAction.aPopupPodpOff); // Не показывать скрытые надписи
                    menu->addSeparator();
                }

                menu->addAction(gidrAction.aEditFindNode);  // Найти узел
                menu->addAction(gidrAction.aProtocol);  // Протокол расчета

                menu->addSeparator();
                if (IsMain()) {
                    menu->addAction(gidrAction.aZn);  // Узел с заданным напором
                }

                if (IsMain()) {
                    menu->addAction(gidrAction.aKam);  // Внешняя схема / Внутренняя схема
                }
                else {
                    menu->addAction(gidrAction.aKamVnesh);  // Внешняя схема / Внутренняя схема
                }

                if (isEditGid()) {
                    menu->addSeparator();
                    menu->addAction(gidrAction.aNodeNeotr); // Установка конвертированных узлов

        #if 0
                    if (regim == R_BEGIN_LEN2 && m_NP.size() > 1) {
                        menu->addSeparator();

                        action = new QAction(tr("Перенести во внутреннюю схему"),this);
                        menu->addAction(gidrAction.);
        //                menu.AppendMenu(MF_STRING, ID_POPUP_VNUTR, _TR("Перенести во внутреннюю схему"));
                    }
        #endif
                }

            }
            CNode2 *nc = m_cxema.m_graph->find(m_parent_id);
            if (nc) {
//                addNodeMenuItem(menu, nc, QString(tr("Свойства объекта %1 %2")).arg(nc->getTable(), nc->getName()), SLOT(viewNode()));
            }
        }
    }

    menu->exec( qpt );

    delete menu;
    menu = NULL;
}
