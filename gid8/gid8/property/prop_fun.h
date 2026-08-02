#pragma once

enum FunIndex {
    FUN_error = -1,
    FUN_average3 = 1,
    FUN_climat1,
    FUN_color,
    FUN_diameter_std,
    FUN_diameter_std2,
    FUN_Date,
    FUN_Dir,
    FUN_File,
    FUN_FileView,
    FUN_heatTestsCoeff,
    FUN_Hidden,
    FUN_hydroRes,
    FUN_ind_result,
    FUN_ind_save,
    FUN_length,
    FUN_Math,
    FUN_ms_rs,
    FUN_NagrSum,
    FUN_nagruz_ot,
    FUN_nagruz_vt,
    FUN_nodeName,
    FUN_nodes,
    FUN_nodes_pr,
    FUN_node_pick,
    FUN_node_pick1,
    FUN_node_pick2,
    FUN_NotNull,
    FUN_ReadOnly,
    FUN_schemeNum,
    FUN_standardDampLink,
    FUN_Table,
    FUN_TableQ,
    FUN_uf,
    FUN_uf150_70,
    FUN_utverdit,
    FUN_viewNsDb,
    FUN_dolya,
    FUN_mestn,

    FUN_nodes_new_baza,
    FUN_list_of_coluns,
    FUN_viewText,

    FUN_alsecoNagr,
    FUN_alseco_nagr,

    FUN_list_db,
    FUN_addr_nagruzki,
};





//------------------------------------------------
QVariant fun_average3(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_climat1(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_color(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_Date(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_diameter_std(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_diameter_std2(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_Dir(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_dolya(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_File(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_FileView(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_heatTestsCoeff(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_Hidden(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_hydroRes(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_ind_result(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_ind_save(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_length(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_Math(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_mestn(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_ms_rs(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_NagrSum(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_nagruz_ot(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_nagruz_vt(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_nodeName(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_nodes(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_nodes_pr(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_node_pick(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_node_pick1(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_node_pick2(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_NotNull(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_ReadOnly(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_schemeNum(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_standardDampLink(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_Table(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_TableQ(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_uf(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_uf150_70(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_utverdit(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_viewNsDb(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
//------------------------------------------------



QVariant fun_nodes_new_baza(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);

QVariant fun_list_of_coluns(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);

QVariant fun_viewText(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);

QVariant fun_alsecoNagr(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_alseco_nagr(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);


QVariant fun_list_db(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);
QVariant fun_addr_nagruzki(QWidget *widget, PropertyDial *prop, const QString & fun, PropertyStr &pr, const QString & key, QVariant value, int id, const QString & label, QSqlDatabase &db, bool *ok, bool *yes);


