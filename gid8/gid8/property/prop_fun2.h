#pragma once


enum FunIndex2 {
  FUN_OpresTableAkt,
  FUN_akt_na_osmotr_tepl,
  FUN_akt_rasledovania,
  FUN_control_tu,
  FUN_defect_elem,
  FUN_diag_defect_list,
  FUN_diag_defect_new,
  FUN_faktory_riska_truboprovoda,
  FUN_faktory_riska_truboprovoda_osmotr,
  FUN_faktory_riska_truboprovoda_remont,
  FUN_granizy_razdela_opr,
  FUN_ind_reset,
  FUN_ind_result,
  FUN_ind_save,
  FUN_kamery_list_for_defect,
  FUN_karta_povrezhaemosti,
  FUN_komissia,
  FUN_list_PT,
  FUN_list_US_1,
  FUN_list_US_2,
  FUN_list_ms,
  FUN_list_rs,
  FUN_list_uch,
  FUN_ms_view,
  FUN_opres_defect_list,
  FUN_opres_defect_new_pick,
  FUN_osmotr_defect_list,
  FUN_osmotr_defect_new_pick,
  FUN_pipeSections_faktory_riska,
  FUN_pipeSections_osmotr_history,
  FUN_pipeSections_remont_history,
  FUN_remont_defect_list,
  FUN_remont_defekt2,
  FUN_remont_shurf2,
  FUN_remont_utverdit,
  FUN_rs_view,
  FUN_shurf_utverdit,
  FUN_spisok_otkl_potr,
  FUN_table_view,
  FUN_view_UT,
  FUN_view_UTBIG,
  FUN_view_documents,
  FUN_view_filtr,
  FUN_view_link,
  FUN_view_meropr,
  FUN_vyd_rabot,
  FUN_zhurnal_defect,
  FUN_zhurnal_defect_word,
  FUN_zhurnal_remont,
  FUN_zhurnal_shurfy
};



void fun_OpresTableAkt(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_akt_na_osmotr_tepl(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_akt_rasledovania(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_control_tu(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_defect_elem(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_diag_defect_list(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_diag_defect_new(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_faktory_riska_truboprovoda(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_faktory_riska_truboprovoda_osmotr(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_faktory_riska_truboprovoda_remont(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_granizy_razdela_opr(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_ind_reset(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_ind_result(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_ind_save(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_kamery_list_for_defect(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_karta_povrezhaemosti(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_komissia(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_list_PT(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_list_US_1(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_list_US_2(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_list_ms(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_list_rs(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_list_uch(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_ms_view(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_opres_defect_list(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_opres_defect_new_pick(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_osmotr_defect_list(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_osmotr_defect_new_pick(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_pipeSections_faktory_riska(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_pipeSections_osmotr_history(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_pipeSections_remont_history(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_remont_defect_list(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_remont_defekt2(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_remont_shurf2(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_remont_utverdit(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_rs_view(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_shurf_utverdit(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_spisok_otkl_potr(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_table_view(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_view_UT(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_view_UTBIG(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_view_documents(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_view_filtr(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_view_link(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_view_meropr(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_vyd_rabot(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_zhurnal_defect(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_zhurnal_defect_word(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_zhurnal_remont(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
void fun_zhurnal_shurfy(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label);
