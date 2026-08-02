#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include <map>

#include "PropertyDial.h"
#include "qmessagebox.h"
#include <db/db.h>
#include <mystd.h>

#include <QMessageBox>

//#include "variantdelegate.h"

typedef void (*Fun)(PropertyDial *prop, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & lable);

#include "prop_fun2.h"
std::map<
    QString, 
    FunIndex2
    > map_fun2 = {
  
  {"OpresTableAkt",                     FUN_OpresTableAkt},
  {"akt_na_osmotr_tepl",                FUN_akt_na_osmotr_tepl},
  {"akt_rasledovania",                  FUN_akt_rasledovania},
  {"control_tu",                        FUN_control_tu},
  {"defect_elem",                       FUN_defect_elem},
  {"diag_defect_list",                  FUN_diag_defect_list},
  {"diag_defect_new",                   FUN_diag_defect_new},
  {"faktory_riska_truboprovoda",        FUN_faktory_riska_truboprovoda},
  {"faktory_riska_truboprovoda_osmotr", FUN_faktory_riska_truboprovoda_osmotr},
  {"faktory_riska_truboprovoda_remont", FUN_faktory_riska_truboprovoda_remont},
  {"granizy_razdela_opr",               FUN_granizy_razdela_opr},
  {"ind_reset",                         FUN_ind_reset},
  {"ind_result",                        FUN_ind_result},
  {"ind_save",                          FUN_ind_save},
  {"kamery_list_for_defect",            FUN_kamery_list_for_defect},
  {"karta_povrezhaemosti",              FUN_karta_povrezhaemosti},
  {"komissia",                          FUN_komissia},
  {"list_PT",                           FUN_list_PT},
  {"list_US_1",                         FUN_list_US_1},
  {"list_US_2",                         FUN_list_US_2},
  {"list_ms",                           FUN_list_ms},
  {"list_rs",                           FUN_list_rs},
  {"list_uch",                          FUN_list_uch},
  {"ms_view",                           FUN_ms_view},
  {"opres_defect_list",                 FUN_opres_defect_list},
  {"opres_defect_new_pick",             FUN_opres_defect_new_pick},
  {"osmotr_defect_list",                FUN_osmotr_defect_list},
  {"osmotr_defect_new_pick",            FUN_osmotr_defect_new_pick},
  {"pipeSections_faktory_riska",        FUN_pipeSections_faktory_riska},
  {"pipeSections_osmotr_history",       FUN_pipeSections_osmotr_history},
  {"pipeSections_remont_history",       FUN_pipeSections_remont_history},
  {"remont_defect_list",                FUN_remont_defect_list},
  {"remont_defekt2",                    FUN_remont_defekt2},
  {"remont_shurf2",                     FUN_remont_shurf2},
  {"remont_utverdit",                   FUN_remont_utverdit},
  {"rs_view",                           FUN_rs_view},
  {"shurf_utverdit",                    FUN_shurf_utverdit},
  {"spisok_otkl_potr",                  FUN_spisok_otkl_potr},
  {"table_view",                        FUN_table_view},
  {"view_UT",                           FUN_view_UT},
  {"view_UTBIG",                        FUN_view_UTBIG},
  {"view_documents",                    FUN_view_documents},
  {"view_filtr",                        FUN_view_filtr},
  {"view_link",                         FUN_view_link},
  {"view_meropr",                       FUN_view_meropr},
  {"vyd_rabot",                         FUN_vyd_rabot},
  {"zhurnal_defect",                    FUN_zhurnal_defect},
  {"zhurnal_defect_word",               FUN_zhurnal_defect_word},
  {"zhurnal_remont",                    FUN_zhurnal_remont},
  {"zhurnal_shurfy",                    FUN_zhurnal_shurfy}
};


std::map<
    FunIndex2,
    Fun
    > map_fun2_id = {

    {FUN_OpresTableAkt,                     fun_OpresTableAkt  },
    {FUN_akt_na_osmotr_tepl,                fun_akt_na_osmotr_tepl  },
    {FUN_akt_rasledovania,                  fun_akt_rasledovania  },
    {FUN_control_tu,                        fun_control_tu  },
    {FUN_defect_elem,                       fun_defect_elem  },
    {FUN_diag_defect_list,                  fun_diag_defect_list  },
    {FUN_diag_defect_new,                   fun_diag_defect_new  },
    {FUN_faktory_riska_truboprovoda,        fun_faktory_riska_truboprovoda  },
    {FUN_faktory_riska_truboprovoda_osmotr, fun_faktory_riska_truboprovoda_osmotr  },
    {FUN_faktory_riska_truboprovoda_remont, fun_faktory_riska_truboprovoda_remont  },
    {FUN_granizy_razdela_opr,               fun_granizy_razdela_opr  },
    {FUN_ind_reset,                         fun_ind_reset  },
    {FUN_ind_result,                        fun_ind_result  },
    {FUN_ind_save,                          fun_ind_save  },
    {FUN_kamery_list_for_defect,            fun_kamery_list_for_defect  },
    {FUN_karta_povrezhaemosti,              fun_karta_povrezhaemosti  },
    {FUN_komissia,                          fun_komissia  },
    {FUN_list_PT,                           fun_list_PT  },
    {FUN_list_US_1,                         fun_list_US_1  },
    {FUN_list_US_2,                         fun_list_US_2  },
    {FUN_list_ms,                           fun_list_ms  },
    {FUN_list_rs,                           fun_list_rs  },
    {FUN_list_uch,                          fun_list_uch  },
    {FUN_ms_view,                           fun_ms_view  },
    {FUN_opres_defect_list,                 fun_opres_defect_list  },
    {FUN_opres_defect_new_pick,             fun_opres_defect_new_pick  },
    {FUN_osmotr_defect_list,                fun_osmotr_defect_list  },
    {FUN_osmotr_defect_new_pick,            fun_osmotr_defect_new_pick  },
    {FUN_pipeSections_faktory_riska,        fun_pipeSections_faktory_riska  },
    {FUN_pipeSections_osmotr_history,       fun_pipeSections_osmotr_history  },
    {FUN_pipeSections_remont_history,       fun_pipeSections_remont_history  },
    {FUN_remont_defect_list,                fun_remont_defect_list  },
    {FUN_remont_defekt2,                    fun_remont_defekt2  },
    {FUN_remont_shurf2,                     fun_remont_shurf2  },
    {FUN_remont_utverdit,                   fun_remont_utverdit  },
    {FUN_rs_view,                           fun_rs_view  },
    {FUN_shurf_utverdit,                    fun_shurf_utverdit  },
    {FUN_spisok_otkl_potr,                  fun_spisok_otkl_potr  },
    {FUN_table_view,                        fun_table_view  },
    {FUN_view_UT,                           fun_view_UT  },
    {FUN_view_UTBIG,                        fun_view_UTBIG  },
    {FUN_view_documents,                    fun_view_documents  },
    {FUN_view_filtr,                        fun_view_filtr  },
    {FUN_view_link,                         fun_view_link  },
    {FUN_view_meropr,                       fun_view_meropr  },
    {FUN_vyd_rabot,                         fun_vyd_rabot  },
    {FUN_zhurnal_defect,                    fun_zhurnal_defect  },
    {FUN_zhurnal_defect_word,               fun_zhurnal_defect_word  },
    {FUN_zhurnal_remont,                    fun_zhurnal_remont  },
    {FUN_zhurnal_shurfy,                    fun_zhurnal_shurfy  },
};




void PropertyDial::prop_fun2(const QString & cmd, QSqlDatabase &db, const QString & param1, const QString & param2, int id, const QString & label)
{
    auto it1 = map_fun2.find(cmd);
    if (it1 == map_fun2.end()) {
        return;
    }

    auto it = map_fun2_id.find(it1->second);

    if (it != map_fun2_id.end()) {
        it->second(this, db, param1, param2, id, label);
        return;
    }
    QMessageBox::warning(this, "", cmd);
}


