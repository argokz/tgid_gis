#include <QApplication>
#include <QtGui>

#include <mainwindow.h>
#include <gidview/GidWidget.h>
#include <dialog/MMenuDial.h>

#include <QInputDialog>
#include <any/rights.h>

#include <db/db.h>

#include <property/PropertyDial.h>
#include <any/MyMain.h>

#include <table/DbWindow.h>

#include "std.h"

void set_ms_rs_condition(QString& qq, std::set<int>& m_set_ms, std::set<int>& m_set_rs);
void sezon_condition(QString &q, int m_sezon_korrozia, QDateTime m_sezon_date1, QDateTime m_sezon_date2);



bool setOsmotr(QSqlDatabase &db, const QString & opres, CCxema* m_cxema, int directionID)
{
    bool ret = false;
    QString q;

    if (opres == "osmotr" || opres == "remont2") {
        q = QString(
            "Select d.lineID, faktory_riska_truboprovoda.id AS fakt_id From %1Deployed d\n"
            "LEFT JOIN %1 o ON o.id=d.directionID\n"
            "JOIN net.v_heatpipesections hps ON hps.lineID=d.lineID\n"
            "JOIN pipeSections ps1 ON ps1.id=hps.pipeSectionID\n"
            "left join faktory_riska_truboprovoda on faktory_riska_truboprovoda.lineID = ps1.id and faktory_riska_truboprovoda.objID = %2 and faktory_riska_truboprovoda.obj_type_faktory_riskaID = %3\n"

            "Where directionID = %4  \n").arg(opres).arg(directionID).arg(opres == "osmotr" ? 2 : 3).arg(directionID);

    }
    else {
        q = QString("Select lineID From %1Deployed Where directionID = %2").arg(opres).arg(directionID);
    }

    m_cxema->m_graph->init_find_line_nom();


#if 0
    if (opres == "osmotr") {

"SELECT \n"
"f.id,\n"
"l.id\n"

"FROM osmotr o\n"
"JOIN osmotrDeployed d ON d.directionID=o.id\n"
"JOIN net.v_linesobj l ON l.id=d.lineID AND l.removed=0\n"
"JOIN heatPipeSections hps ON hps.lineID=l.id\n"
"JOIN net.v_nodes n1 ON n1.id=l.nodeID1\n"
"JOIN net.v_nodes n2 ON n2.id=l.nodeID2\n"
"LEFT JOIN faktory_riska_truboprovoda f ON f.objID=o.id AND f.lineID=hps.pipeSectionID AND f.obj_type_faktory_riskaID=2\n"

        return;
    }
#endif



    set<int> set_fileID;

    m_cxema->m_graph->reset();

    QSqlQuery query(m_cxema->m_db);
    query.setForwardOnly(true);

    if (query_exec(m_cxema->m_db, query, q)) {
        ret = true;
        while (query.next()) {
            long id = query.value("lineID").toInt();
            long prosm = query.value("fakt_id").toInt();

            CLINE2* l = m_cxema->m_graph->find_line_nom(id);

            if (l) {
                bline(l)->line.isPjezo = true;
                where(l)->node.isPjezo = true;
                other(l)->node.isPjezo = true;

                bline(l)->line.isOsmotr = prosm;
            }
        }
    }

    q = QString(

"SELECT DISTINCT\n"
//"n1.fileID \n"
"fr.name \n"
"FROM %1Deployed d\n"
"JOIN net.v_linesobj l ON l.id=d.lineID\n"
"JOIN net.v_nodes n1 ON n1.id=l.nodeID1\n"
"JOIN fragments fr ON fr.id=n1.fileID\n"
            "Where directionID=%2 AND n1.fileID NOT IN (%3)\n").arg(opres).arg(directionID).arg(m_cxema->m_par);

    QString ss = "";

    if (query_exec(m_cxema->m_db, query, q)) {
        ret = true;
        while (query.next()) {
            QString name = query.value("name").toString();
            if (ss != "") ss += "\n";
            ss += name;
        }
    }
    if (ss != "")  {
        QMessageBox::warning(nullptr, "", "Часть участков контура не показана на экране, откройте фрагменты сети:\n"+ss);
    }
    return ret;
}

void GidWidget::viewOpresDialog(const QString & opres, int m_id_opr, bool autodelete)
{
    info(m_cxema.m_db, "", opres, m_id_opr, -1, pr_type_any, "shape", "id");

#if 0
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
#endif
}

void RunTableNiz(QSqlDatabase &db, const QString & tn, const QString & q, const QString & title, bool a1, bool a2)
{
    DbWindow *view = getTableView(db, tn, q, title);

    if (view) {
//        view->setMoveTo(st.node_or_line == 1 ? move_to_node : move_to_line);
//        view->setGidWidget(this);
        main_window->addWidgetTab(view, title, Qt::BottomDockWidgetArea);
    }
}


void GidWidget::viewOpresProtokol(const QString & opres, int& m_opr_id)
{
    QString vyd;

    QString name = "Участки";
    QString name1;
    QString q;

    q = QString("SELECT o.name, nach.fio FROM %1 o LEFT JOIN nachalniki_uchastkov nach ON nach.id=o.otvetstvennoe_lico_ID WHERE o.id=%2").arg(opres).arg(m_opr_id);

    if (opres == "opres") {
        q = QString("SELECT o.name, nach.fio FROM %1 o LEFT JOIN nachalniki_uchastkov nach ON nach.id=o.responsibleID WHERE o.id=%2").arg(opres).arg(m_opr_id);
    }
    if (opres == "remont2") {
        q = QString("SELECT o.otchet_po_defektu, nach.fio FROM %1 o LEFT JOIN nachalniki_uchastkov nach ON nach.id=o.responsibleID WHERE o.id=%2").arg(opres).arg(m_opr_id);
    }

    vector<QVariant> v;

    if (readTableValues(m_cxema.m_db, q, 2, v)) {
        name = QString("Участки %1. Ответственный: %2 %3-%4 отопительный сезон").arg(v[0].toString()).arg(v[1].toString()).arg(m_sezon_korrozia).arg(m_sezon_korrozia + 1);
    }

    CGraph2* graph = m_cxema.m_graph;

    QString s = QString("%1").arg(m_opr_id);

    if (opres == "osmotr") {
        QString tn;
        tn = "sql/objects/osmotr/osmotr_pipesection.sql";
        q = readQ(tn);
        q.replace("$opres$", opres);
        q.replace("$id$", s);

        if (q != "") {
            RunTableNiz(m_cxema.m_db, opres, q, name, true, false/*, virt_data_line_big*/);
        }
    }
    else if (opres == "remont2") {
        QString tn;
        tn = "sql/objects/osmotr/remont_pipesections.sql";
        q = readQ(tn);
        q.replace("$opres$", opres);
        q.replace("$id$", s);

        if (q != "") {
            RunTableNiz(m_cxema.m_db, opres, q, name, true, false/*, virt_data_line_big*/);
        }
    }
    else {
        QString tn;
        tn = "sql/objects/osmotr/osmotr_pipesection.sql";
        tn = "sql/objects/defect/remonts_journal.sql";
        tn = "sql/objects/opres2/osvidet2.sql";

        q = readQ(tn);
        q.replace("$opres$", opres);
        q.replace("$m_opr_id$", s);
        q.replace("$id$", s);

        if (q != "") {
            q.replace("$vyd$", vyd);
            RunTableNiz(m_cxema.m_db, opres, q, name, true, false/*, virt_data_line_big*/);
        }
    }
}




void GidWidget::LastOpres(const QString & opres, const QString & title, int& m_id_opr, int& m_id_opr_old, bool view_dialog)
{
    bool m_mark_changed = false;

    if (m_id_opr < 0) {
        m_id_opr = m_id_opr_old;
    }
    if (m_id_opr < 0) {
        QMessageBox::warning(nullptr, "", QString("Выберите %1").arg(title));
        return;
    }
    int directionID = m_id_opr;
//    CAdoFile* ado = m_cxema->m_ado;
//    if (!ado) return;

    if (directionID >= 0) {
        if (setOsmotr(m_cxema.m_db, opres, &m_cxema, directionID)) {
            if (view_dialog) {
                viewOpresDialog(opres, m_id_opr, false);
            }
            else {
                moveVyd();
                viewOpresProtokol(opres, directionID);
            }
        }
    }

    repaint();
}

void GidWidget::setOpres(const QString & opres, int id)
{

    if (opres == "opres") {
        m_id_opr = id;
    }
    else if (opres == "remont2") {
        m_id_remont2 = id;
    }
    else if (opres == "osvidet2") {
        m_id_osvidet2 = id;
    }
    else if (opres == "osmotr") {
        m_id_osmotr2 = id;
    }
//    QMessageBox::warning(nullptr, "", QString("%1 %2").arg(opres).arg(id));
}


void GidWidget::ListOpres(const QString & opres, const QString & title, const QString & name_name, const QString & date_name, int& m_id_opr)
{
    QString q;

    if (opres == "remont2") {
        q = R"SQL(
            SELECT DISTINCT TOP 1000000
            r.id,
            r.otchet_po_defektu AS 'Наименование/Адрес',
            r.data_nachala_plan as 'Дата начала ремонта плановая',
            r.data_okonchaniya_plan as 'Дата окончания ремонта плановая',
            r.data_nachala_remonta as 'Дата начала ремонтных работ',
            r.data_zaversheniya_remonta as 'Дата завершения ремонтных работ',
            CASE
            WHEN r.utverdit = 0 THEN 'Не утверждено'
            ELSE 'Утверждено'
            END AS 'Утверждение плана',
            rt.name as 'Вид ремонта',
            nach.fio as 'Ответственный за ремонт'
            FROM remont2 r
            LEFT JOIN remontTypes rt ON rt.id=r.remontTypeID
            LEFT JOIN nachalniki_uchastkov nach ON nach.id=r.responsibleID
            JOIN remont2Deployed depl ON depl.directionID=r.id
            JOIN net.v_linesobj l ON l.id=depl.lineID
            JOIN net.v_heatpipesections hpss ON hpss.lineID=l.id $and_condition2$
            WHERE
              (
              (r.data_nachala_plan $season_condition$) OR
              (r.data_nachala_remonta $season_condition$)
              ) ORDER BY r.id DESC
            )SQL";
    }

    else if (opres == "osmotr") {
        q = R"SQL(
            SELECT DISTINCT TOP 1000000
            o.id,
            o.name,
            o.data_osmotra,
            nach.fio AS 'Ответственный'
            FROM osmotr o
            LEFT JOIN nachalniki_uchastkov nach ON nach.id = otvetstvennoe_lico_ID
            JOIN osmotrDeployed depl ON depl.directionID=o.id
            JOIN net.v_linesobj l ON l.id=depl.lineID
            JOIN net.v_heatpipesections hpss ON hpss.lineID=l.id $and_condition2$
            WHERE
              (
              (o.data_osmotra $season_condition$) OR
              (o.data_osmotra $season_condition$)
              ) ORDER BY o.id DESC 
            )SQL";
    }
    else if (opres == "opres") {
        q = R"SQL(
            SELECT DISTINCT TOP 1000000
            r.id,
            r.name AS 'Описание контура',
            r.date_opres as 'Дата опрессовки',
            CASE
            WHEN r.utverdit = 0 THEN 'Не утверждено'
            ELSE 'Утверждено'
            END AS 'Утверждение плана'
            FROM opres r
            LEFT JOIN nachalniki_uchastkov nach ON nach.id=r.responsibleID
            JOIN opresDeployed depl ON depl.directionID=r.id
            JOIN net.v_linesobj l ON l.id=depl.lineID
            JOIN net.v_heatpipesections hpss ON hpss.lineID=l.id $and_condition2$
            WHERE
              (
              (r.data_nachala_plan $season_condition$) OR
              (r.date_opres $season_condition$)
              ) ORDER BY r.id DESC
            )SQL";
    
    
    }
    else {
        QString str = QString("Здесь нельзя использовать таблицу %1").arg(opres);
        QMessageBox::warning(this, "", str);
    }

    std::set<int> m_set_ms;
    std::set<int> m_set_rs;
//    QDateTime m_sezon_date1;
//    QDateTime m_sezon_date2;


//    QDateTime m_sezon_date1(QDate(m_sezon_korrozia, 10, 15), QTime());
//    QDateTime m_sezon_date2(QDate(m_sezon_korrozia+1, 4, 15), QTime());

    set_ms_rs_condition(q, m_set_ms, m_set_rs);
    sezon_condition(q, m_sezon_korrozia, m_sezon_date1, m_sezon_date2);

    int n_ret = 0;

//    QInputDialog::getMultiLineText(nullptr, "title", "label", q);

    DbWindow *table = getTableView(m_cxema.m_db, opres, q, tr("Опрессовки"));

    if (!table) {
        QMessageBox::warning(nullptr, "", QString("Ошибка"));
        return;
    }

    table->setEdit(true);
    table->setDelete(true);
    table->setOpres(opres);
    table->setGidWidget(this);

//        table->setGidWidget(this->m_view);
//        table->setGeo(kls->nazv);

    MyMain *pm = new MyMain(title, this);
    pm->setWidget(table);
    pm->show();



#if 0

    CWebDialog* dlg = new CWebDialog(this, ado->m_type_of_net, ado->m_schema, ado->getConnect(), opres, q, n_ret, virt_data_geo_del, title, false);

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
#endif
}


