#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include <map>

#include "PropertyDial.h"
#include "cxema/cxema1.h"
#include "qmessagebox.h"
#include <db/db.h>
#include <mystd.h>

#include <QMessageBox>

//#include "variantdelegate.h"

#include <dialog/TableWindow.h>

void OpresTableAkt(QWidget* wnd, QSqlDatabase &db, int id);

void fun_OpresTableAkt(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    OpresTableAkt(prop, db, id);
}

void fun_akt_na_osmotr_tepl(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "akt_na_osmotr_tepl");
}

void fun_akt_rasledovania(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "akt_rasledovania");
}

void fun_control_tu(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "control_tu");
}

void fun_defect_elem(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "defect_elem");
}

void fun_diag_defect_list(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "diag_defect_list");
}

void fun_diag_defect_new(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "diag_defect_new");
}

void fun_faktory_riska_truboprovoda(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "faktory_riska_truboprovoda");
}

void fun_faktory_riska_truboprovoda_osmotr(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "faktory_riska_truboprovoda_osmotr");
}

void fun_faktory_riska_truboprovoda_remont(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "faktory_riska_truboprovoda_remont");
}

void fun_granizy_razdela_opr(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "granizy_razdela_opr");
}

void fun_ind_reset(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "ind_reset");
}

void fun_ind_result(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "ind_result");
}

void fun_ind_save(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "ind_save");
}

void fun_kamery_list_for_defect(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "kamery_list_for_defect");
}

void fun_karta_povrezhaemosti(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
//    QMessageBox::warning(prop, "", "karta_povrezhaemosti");

void KartaPovrezhdaemosti2(QWidget* wnd, QSqlDatabase &db, int id);
    KartaPovrezhdaemosti2(prop, db, id);
}

void fun_komissia(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "komissia");
}

void fun_list_PT(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "list_PT");
}

void fun_list_US_1(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "list_US_1");
}

void fun_list_US_2(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "list_US_2");
}

void fun_list_ms(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "list_ms");
}

void fun_list_rs(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "list_rs");
}

void fun_list_uch(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "list_uch");
}

void fun_ms_view(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "ms_view");
}

void fun_opres_defect_list(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "opres_defect_list");
}

void fun_opres_defect_new_pick(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "opres_defect_new_pick");
}

void fun_osmotr_defect_list(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "osmotr_defect_list");
}

void fun_osmotr_defect_new_pick(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "osmotr_defect_new_pick");
}

void fun_pipeSections_faktory_riska(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "pipeSections_faktory_riska");
}

void fun_pipeSections_osmotr_history(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "pipeSections_osmotr_history");
}

void fun_pipeSections_remont_history(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QString q = QString(
        "Select DISTINCT o.id,\n"
        "o.otchet_po_defektu AS %1,\n"
        "o.data_osmotra AS 'Дата осмотра'\n"
        "From remont2Deployed d\n"
        "LEFT JOIN remont2 o ON o.id=d.directionID\n"
        "JOIN net.v_linesobj l ON l.id=d.lineID\n"
        "JOIN heatPipeSections hps ON hps.lineID=l.id\n"
        "WHERE hps.pipeSectionID=%1\n").arg(quot_text("Наименование")).arg(id);

    TableWindow tab(db, "", "Ремонты", q, prop, -1, false, true, true);

    if (tab.exec()) {
    }
}

void fun_remont_defect_list(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "remont_defect_list");
}

void fun_remont_defekt2(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "remont_defekt2");
}

void fun_remont_shurf2(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "remont_shurf2");
}

void fun_remont_utverdit(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "remont_utverdit");
}

void fun_rs_view(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "rs_view");
}

void fun_shurf_utverdit(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "shurf_utverdit");
}

void fun_spisok_otkl_potr(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "spisok_otkl_potr");
}

void fun_table_view(PropertyDial *prop, QSqlDatabase &db, const QString & codeName, const QString & param2, int id, const QString & label)
{
    QString tn = codeName;

    if (codeName == "shurf") {
        tn = "shurfy";
    }
    else if (codeName == "cut_out" || codeName == "shurf" || codeName == "certification") {
        tn = "diag";
    }
    else if (codeName == "tk_diagnostika" || codeName == "pavilion_diagnostika") {
        tn = "diag";
    }
    else if (codeName == "heat_point_real_consumers") {
        tn = "realConsumers";
    }
    else if (codeName == "heat_point_list_contract") {
        tn = "realConsumers";
    }
    else if (codeName == "heat_point_buildings") {
        tn = "buildings";
    }

    QString fStr = QString("sql/field_list/%1.sql").arg(codeName);
    QString q = readQ(fStr);

    if (q == "") return;

    q.replace("$fileID$", QString("%1").arg(id));

    if (prop->getCxema()) {
        q.replace("$FragmentIds$", prop->getCxema()->m_par);
        q.replace("$fragments$", prop->getCxema()->m_par);
    }
    else {
        QMessageBox::warning(prop, "", "Ошибка");
        return;
    }

    TableWindow tab(db, "", label, q, prop, -1, false, true, false);

    if (tab.exec()) {
    }


//    viewTable(prop, db, label, fun, ok);

//    viewVirtualDlg(this, pItem->m_schema, q, id, pItem->m_label, txt, false, false, false, false, codeName);
}

void fun_view_UT(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "view_UT");
}

void fun_view_UTBIG(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "view_UTBIG");
}

#include <dialog/TableWindow.h>

void view_documents(PropertyDial *widget, QSqlDatabase &db, const QString & tab1, const QString & tab2, int id, const QString & label)
{
    QString q = QString("SELECT d.id, d.objID, d.remontDocumentTypeID, p.name, date_doc, path FROM %1 d"
        " LEFT JOIN %2 p ON p.id=d.remontDocumentTypeID "
        " WHERE objID=%3").arg(tab1, tab2).arg(id);

    TableWindow tab(db, tab1, label, q, widget, -1, false, true, true);
    tab.exec();
}

void fun_view_documents(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    view_documents(prop, db, param1, param2, id, label);
//    QMessageBox::warning(prop, "", "view_documents");
}

void fun_view_filtr(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "view_filtr");
}

void fun_view_link(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "view_link");
}

void fun_view_meropr(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "view_meropr");
}

void fun_vyd_rabot(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "vyd_rabot");
}

void fun_zhurnal_defect(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QString title = "Журнал нарушений на участке";
    QString tn = "defect";

    QString q = readFile("sql/objects/defect/history_defects_by_defect.sql");
    q.replace("$id$", QString("%1").arg(id));

    TableWindow tab(db, tn, title, q, prop, -1, false, true, true);

    if (tab.exec()) {
    }
}

void fun_zhurnal_defect_word(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "zhurnal_defect_word");
}

void fun_zhurnal_remont(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    int n_ret = 0;

    QString title = "Журнал ремонтов на участке";
    QString tn = "defect";

    QString q = readFile("objects/defect/defects_journal_remontov.sql");
    q.replace("$id$", QString("%1").arg(id));

    TableWindow tab(db, tn, title, q, prop, -1, false, true, true);

    if (tab.exec()) {
    }
//    CTableDialog dlg(wnd, ado->m_type_of_net, ado->m_schema, ado->getConnect(), tn, q, n_ret, virt_data_geo, title);
//    int ret = dlg.DoModal();
}

void fun_zhurnal_shurfy(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    QMessageBox::warning(prop, "", "zhurnal_shurfy");
}






