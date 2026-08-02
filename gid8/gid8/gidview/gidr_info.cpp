#include <QApplication>
#include <QtGui>
#include <QtWidgets>
#include <gidview/GidWidget.h>

#include <property/PropertyDial.h>
#include <property/variantdelegate.h>
#include <db/db.h>

#include <any/rights.h>

QString findTableRusName(const QString & d, const QString & n);

void ErrorMessage(QSqlQuery &query);

QString lineQ();
QString nodeQ();


bool reread_line(QSqlDatabase &db, CGraph2 *graph, int id, const QString & table)
{
    bool ret = false;

    CLINE2 * l = graph->findLine(id);

    if (table != "linesobj") {
        int l_typ = getLineTypName(table);
        l = graph->findLine2(l_typ, id);
    }

    if (l) {
        QString q = lineQ();

        q += QString(" AND l.id in (%1, %2)").arg(bline(l)->line.nomP).arg(bline(l)->line.nomO);

        bline(l)->line.nomP = -1;
        bline(l)->line.nomO = -1;

        QSqlQuery query(db);
        if (query_exec(db, query, q)) {
            while (query.next()) {
                readLineNew(query, bline(l), bline(l)->line.typ, graph);
                ret = true;
            }
        }
    }

    return ret;
}

bool reread_node(QSqlDatabase &db, CGraph2 *graph, int id, const QString & table)
{
    CNode2 * n = graph->find(id);

    if (table != "nodes") {
        int l_typ = getNodeTypName(table);
        n = graph->findNode2(l_typ, id);
    }
    
    if (n) {
        QString q = nodeQ();
        q += QString(" AND n.id=%1").arg(n->id);

        QSqlQuery query(db);
        if (query_exec(db, query, q)) {
            if (query.next()) {
                readNodeNew(query, n, n->node.typ, graph);
                return true;
            }
        }
    }
    return false;
}


bool GidWidget::update_view(std::map<QString, std::map<int, std::map<QString, PropertyStr> > > &map_edited)
{
    bool ret = false;

    for (auto &it1: map_edited) {
        QString table = it1.first;
        for (auto &it2: it1.second) {
            QSqlDatabase *db = nullptr;

            for (auto &it3: it2.second) {
                PropertyStr pr = it3.second;
                db = pr.db;
            }
            if (!db) continue;

            int id = it2.first;

            // Тут ставим проверку, если изменилось что-то, что нужно отображать на экране

            int n_typ = getNodeTypName(table);
            int l_typ = getLineTypName(table);


            if (table == "nodes" || n_typ != -1) {
                ret = reread_node(*db, m_cxema.graph(), id, table);
            }
            else if (table == "linesobj" || l_typ != -1) {
                ret = reread_line(*db, m_cxema.graph(), id, table);
            }
            else {
                Klassif *kls = m_kl_list.findKlN(table);
//                QString q = QString("SELECT * FROM %1 WHERE id=%2").arg(table).arg(id);
                if (kls) {
                    CGeoObject *go = kls->getGeoObjectById(id);
                    if (go) {
                        for (auto &it3: it2.second) {
                            PropertyStr pr = it3.second;
                            if (pr.key.toLower() == kls->codlabel.toLower()) {
                                go->text00000 = VariantDelegate::displayText(pr.value);
                                repaint();
                            }
                        }
                        ret = true;
                    }
                }
            }
        }
    }
    return ret;
}


bool isAkt(const QString & tn)
{
    return tn.toLower() == "act";
}

bool containsIgnoreCase(const QStringList &list, const QString &value)
{
    for (const QString &item : list) {
        if (item.compare(value, Qt::CaseInsensitive) == 0) {
            return true;
        }
    }
    return false;
}

bool isRemont(const QString & tn)
{
    QStringList myList = {
        "defect",
        "remont",
        "remont2",
        "shurfy",
        "diag",
        "opres",
        "osmotr",
        "shurfy",
        "opressovka_sektsioniruyuschaya_zadvizhka"
    };

    return containsIgnoreCase(myList, tn);
}

bool isEditGeo(const QString & tn, bool isMessage);

#if 0

bool isEditGeo(const QString & tn, bool isMessage)
{
/*
    if (pView->isReadOnly()) {
        if (isMessage) {
            str.Format("Редактирование в данном режиме невозможно");
            AfxMessageBox(str, MB_OK | MB_ICONINFORMATION);
        }
        return FALSE;
    }
*/
    if (isAkt(tn)) {
        if (UserRight::isAct()) {
            return true;
        }
        else {
            if (isMessage) {
                QString str = QString("Пользователь не имеет прав на редактирование Актов раздела"); //.arg(pDoc->m_user_gid);
                QMessageBox::information(0, "", str);
            }
            return FALSE;
        }
    }

    if (tn.toLower() == "indikator_korrozii") {
        if (UserRight::isKorrozia()) {
            return true;
        }
        else {
            if (isMessage) {
                QString str = QString("Пользователь не имеет прав на редактирование Индикаторов коррозии"); //.arg(pDoc->m_user_gid);
                QMessageBox::information(0, "", str);
            }
            return FALSE;
        }
    }



    bool is_pts(const QString &tn);


    if (is_pts(tn)) {
        if (UserRight::isPTS()) {
            return true;
        }
        else {
            if (isMessage) {
//                str.Format("%s не имеет прав на редактирование объектов ПТС", pDoc->m_user_gid);
                QString str = QString("Пользователь не имеет прав на редактирование объектов ПТС"); //.arg(pDoc->m_user_gid);
                QMessageBox::information(0, "", str);
            }
            return FALSE;
        }
    }

    if (isRemont(tn)) {
        if (UserRight::isRemont()) {
            return true;
        }
        else {
            if (isMessage) {
                QString str = QString("Пользователь не имеет прав на редактирование объектов Ремонтов"); //.arg(pDoc->m_user_gid);
                QMessageBox::information(0, "", str);
            }
            return FALSE;
        }
    }

    Klassif* kls = m_kl_list.findKlN(tn);
    if (kls) {
        if (UserRight::isEditGeo()) {
            return true;
        }
        else {
            if (isMessage) {
                QString str = QString("Пользователь не имеет прав на редактирование объектов Геобазы"); //.arg(pDoc->m_user_gid);
                QMessageBox::information(0, "", str);
            }
            return FALSE;
        }
    }


}

#endif

//bool update_db_2(QSqlDatabase & db, const QString table, int id, std::map<QString, std::map<int, std::map<QString, PropertyStr> > > &map_edited);

void GidWidget::onPropertyAccepted()
{
    auto *dlg = qobject_cast<PropertyDial *>(sender());
    if (dlg) {
        bool right;

        Klassif* kls = m_kl_list.findKlN(dlg->m_table);
        if (kls) {
            right = isEditGeo(dlg->m_table, true);
            if (!right) return;
        }
        else {
            right = isEditGid();
        }

        if (!right) {
            QMessageBox::warning(this, "", QString("Редактирование запрещено!"));
            return;
        }

        if (update_db(dlg->map_edited, dlg->getUser())) {


#if 0
            int id = dlg->m_id1;
            update_db_2(m_cxema.m_db_2, dlg->m_table, id, dlg->map_edited);
#endif

            update_view(dlg->map_edited);
            repaint();
        }
    }
}

void GidWidget::onPropertyRejected()
{
    auto *dlg = qobject_cast<PropertyDial *>(sender());
    if (dlg) {
        if (dlg->get_autodelete()) {
// Удаляем новый объект, если его не сохранили
//            QMessageBox::warning(this, "", QString("%1 %2 %3\n%4").arg(dlg->m_table).arg(dlg->m_id1).arg(dlg->m_id2).arg(dlg->m_pr_type));

            endDraw();

            QString q;

            QSqlQuery query(m_cxema.m_db);
            CLINE2 *l;
            CNode2 *n;

            switch(dlg->m_pr_type) {
                case pr_type_node0:    // Узел
                    q = QString("delete from net.v_nodes where id in (%1,%2)").arg(dlg->m_id1).arg(dlg->m_id2);
                    query_exec(m_cxema.m_db, query, q);
                    n = m_cxema.m_graph->find(dlg->m_id1);
                    if (n) {
                        m_cxema.m_graph->remove_node(*n);
                    }
                    n = m_cxema.m_graph->find(dlg->m_id2);
                    if (n) {
                        m_cxema.m_graph->remove_node(*n);
                    }

                    break;

                case pr_type_node:     // Остальные точечные объекты
                    q = QString("delete from net.v_nodes where id in (%1,%2)").arg(dlg->m_id1).arg(dlg->m_id2);
                    query_exec(m_cxema.m_db, query, q);
                    q = QString("delete from %1 where nodeID in (%2,%3)").arg(dlg->m_table).arg(dlg->m_id1).arg(dlg->m_id2);
                    query_exec(m_cxema.m_db, query, q);

                    n = m_cxema.m_graph->find(dlg->m_id1);
                    if (n) {
                        m_cxema.m_graph->remove_node(*n);
                    }
                    n = m_cxema.m_graph->find(dlg->m_id2);
                    if (n) {
                        m_cxema.m_graph->remove_node(*n);
                    }
                    break;

                case pr_type_line:     // Линейные объекты
                    q = QString("delete from net.v_linesobj where id in (%1,%2)").arg(dlg->m_id1).arg(dlg->m_id2);
                    query_exec(m_cxema.m_db, query, q);
                    q = QString("delete from %1 where lineID in (%2,%3)").arg(dlg->m_table).arg(dlg->m_id1).arg(dlg->m_id2);
                    query_exec(m_cxema.m_db, query, q);

                    m_cxema.m_graph->init_find_line_nom();

                    l = m_cxema.m_graph->find_line_nom(dlg->m_id1);
                    if (l) {
                        m_cxema.m_graph->remove_line(l);
                    }
                    if (dlg->m_id2 != dlg->m_id1) {
                        l = m_cxema.m_graph->find_line_nom(dlg->m_id2);
                        if (l) {
                            m_cxema.m_graph->remove_line(l);
                        }
                    }

                    break;

                case pr_type_any:
                    q = QString("delete from %1 where id in (%2,%3)").arg(dlg->m_table).arg(dlg->m_id1).arg(dlg->m_id2);
                    query_exec(m_cxema.m_db, query, q);
                    break;

                case pr_type_node_out:
                case pr_type_line_out:
                    break;

                case pr_type_geo:
                    delGeoObj(dlg->m_table, dlg->m_id1, false);
                    break;
                }

                repaint();
        }
    }
}

#include "tu/TuInputDialog.h"

void GidWidget::info(QSqlDatabase &db, const QString & title, const QString & table, int id1, int id2, PrTypes pr_type, const QString & s_shape, const QString & s_id, bool autodelete)
{
    QString table_file = table;

    if (table.toLower() == "zdaniya_tu") {
        TuInputDialog dlg(this, db, title, table, id1, &m_cxema);
        if (dlg.exec() == QDialog::Accepted) {
        }

        return;
    }


    
    if (m_idMenu == 4 && (pr_type == pr_type_line || pr_type == pr_type_node || pr_type == pr_type_node)) {
        table_file = table+"_PTS";
    }

    auto *dlg = new PropertyDial(this, table, table_file, s_shape, s_id, id1, id2);
    dlg->setUser(m_user);
    dlg->init_gid(db, title, table, id1, id2, pr_type, &m_cxema, m_idMenu == 4);

    if (autodelete) {
        dlg->set_autodelete();
    }

    dlg->show();

    connect(dlg, SIGNAL(accepted()), this, SLOT(onPropertyAccepted()));
    connect(dlg, SIGNAL(rejected()), this, SLOT(onPropertyRejected()));
}

void GidWidget::info_with_line(QSqlDatabase &db, const QString & title, const QString & table, int id1, int id2, const QString & table2, int id1_2, int id2_2, PrTypes pr_type, const QString & s_shape, const QString & s_id, bool autodelete)
{
    QString table_file = table;
    
    if (m_idMenu == 4 && (pr_type == pr_type_line || pr_type == pr_type_node || pr_type == pr_type_node)) {
        table_file = table+"_PTS";
    }

    auto *dlg = new PropertyDial(this, table, table_file, s_shape, s_id, id1, id2);
    dlg->setUser(m_user);
    dlg->init_gid_with_line(db, title, table, id1, id2, id1_2, pr_type, &m_cxema, m_idMenu == 4);

    if (autodelete) {
        dlg->set_autodelete();
    }

    dlg->show();

    connect(dlg, SIGNAL(accepted()), this, SLOT(onPropertyAccepted()));
    connect(dlg, SIGNAL(rejected()), this, SLOT(onPropertyRejected()));
}



void GidWidget::info_gid(QSqlDatabase &db, const QString & title, const QString & table, const QString & out_table, int id1, int id2, PrTypes pr_type)
{
    auto *dlg = new PropertyDial(this, table, table, "shape", "id", id1, id2);
    dlg->setUser(m_user);
    dlg->init_out(db, title, table, out_table, id1, id2, pr_type);
    dlg->show();

    connect(dlg, SIGNAL(accepted()), this, SLOT(onPropertyAccepted()));
    connect(dlg, SIGNAL(rejected()), this, SLOT(onPropertyRejected()));
}

void GidWidget::viewGeo(Klassif * kls, int id)
{
    endDraw();

    m_CurKlassif = kls;

    m_CurGeoObject = nullptr;

    for (auto gl: kls->geo4) {
        if (gl->nom == id) {
            m_CurGeoObject = gl;
        }
    }

    if (m_CurGeoObject) {
        info(*kls->m_db, kls->RusName(), kls->nazv, id, -1, pr_type_geo, kls->shape, kls->id, true);
    }
}
