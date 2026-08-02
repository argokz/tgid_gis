#include "stdafx.h"

#include <json.hpp>
nlohmann::json get_j(nlohmann::json& j, const string& key);
double get_j_float(nlohmann::json& j, const string& key);
int get_j_int(nlohmann::json& j, const string& key);

#include "global.h"

bool read_calc_param(const CString& param, GLOBAL &global)
{
    nlohmann::json j0;

    try {


        j0 = nlohmann::json::parse((const char*)param);

        global.is_ZULU_SM = get_j_int(j0, "is_ZULU_SM");
        global.is_ZULU = get_j_int(j0, "is_ZULU");
        global.is_ZN0 = get_j_int(j0, "is_ZN0");
        global.is_save_po = get_j_int(j0, "is_save_po");
        global.is_save_po_yes = get_j_int(j0, "is_save_po_yes");
        global.iter = get_j_int(j0, "iter");
        global.g_equival = get_j_int(j0, "g_equival");
        global.is_kv = get_j_int(j0, "is_kv");
        global.g_is_avar = get_j_int(j0, "g_is_avar");
        global.is_dross_yes = get_j_int(j0, "is_dross_yes");
        global.avtomat_yes = get_j_int(j0, "avtomat_yes");
        global.is_tg = get_j_int(j0, "is_tg");
        global.tp_metod = get_j_int(j0, "tp_metod");
        global.n_GWS = get_j_int(j0, "n_GWS");
        global.n_GWS2 = get_j_int(j0, "n_GWS2");
        global.n_trtp = get_j_int(j0, "n_trtp");
        global.no_teplovyd = get_j_int(j0, "no_teplovyd");
        global.no_teplopoter = get_j_int(j0, "no_teplopoter");
        global.uf_calc = get_j_int(j0, "uf_calc");
        global.is_mag_fragment = get_j_int(j0, "is_mag_fragment");
        global.is_char_sety = get_j_int(j0, "is_char_sety");
        global.is_no_balans = get_j_int(j0, "is_no_balans");
        global.is_veter = get_j_int(j0, "is_veter");
        global.is_no_current = get_j_int(j0, "is_no_current");
        global.is_save_uf_new = get_j_int(j0, "is_save_uf_new");
        global.is_leto = get_j_int(j0, "is_leto");
        global.is_poteri = get_j_int(j0, "is_poteri");
        global.is_dop = get_j_int(j0, "is_dop");
        global.is_fakt = get_j_int(j0, "is_fakt");
        global.is_no_out = get_j_int(j0, "is_no_out");
        global.Month = get_j_int(j0, "Month");
        global.n_nagr = get_j_int(j0, "n_nagr");
        global.Tn = get_j_float(j0, "Tn");
        global.Eps = get_j_float(j0, "Eps");
        global.veter = get_j_float(j0, "veter");
    }
    catch (...) {
        return false;
    }

    return true;
}

CString get_calc_param_int(CString param, int n)
{
    CString s;
    s.Format(" -%s %d", param, n);
    return s;
}


CString get_calc_param(GLOBAL& global)
{
    CString s, str = "";

    if (global.is_ZULU_SM) str += " -is_ZULU_SM";
    if (global.is_ZULU) str += " -is_ZULU";
    if (global.is_ZN0) str += " -is_ZN0";
    if (global.is_save_po) str += " -is_save_po";
    if (global.is_save_po_yes) str += " -is_save_po_yes";
//    if (global.iter) str += " -iter";
    if (global.g_equival) str += " -g_equival";
    if (global.is_kv) str += " -is_kv";
    if (global.g_is_avar) str += " -g_is_avar";
    if (global.is_dross_yes) str += " -is_dross_yes";
    if (global.avtomat_yes) str += " -avtomat_yes";
    if (global.is_tg) str += " -is_tg";

    str += get_calc_param_int("tp_metod", global.tp_metod);
    str += get_calc_param_int("GWS", global.n_GWS);
    str += get_calc_param_int("GWS2", global.n_GWS2);

//    if (global.n_GWS) str += " -n_GWS";
//    if (global.n_GWS2) str += " -n_GWS2";

//    if (global.n_trtp) str += " -n_trtp";
    str += get_calc_param_int("trtp", global.n_trtp);

    if (global.no_teplovyd) str += " -no_teplovyd";
    if (global.no_teplopoter) str += " -no_teplopoter";
    if (global.uf_calc) str += " -uf_calc";
    if (global.is_mag_fragment) str += " -is_mag_fragment";
    if (global.is_char_sety) str += " -is_char_sety";
    if (global.is_no_balans) str += " -is_no_balans";
    if (global.is_veter) str += " -is_veter";
    if (global.is_no_current) str += " -is_no_current";
    if (global.is_save_uf_new) str += " -is_save_uf_new";
    if (global.is_leto) str += " -is_leto";
    if (global.is_poteri) str += " -is_poteri";
    if (global.is_dop) str += " -is_dop";
    if (global.is_fakt) str += " -is_fakt";
//    if (global.is_no_out) str += " -is_no_out";
//    if (global.Month) str += " -Month";
//    if (global.n_nagr) str += " -n_nagr";
    str += get_calc_param_int("nagr", global.n_nagr);


    if (global.Tn) {
        s.Format(" -Tn %g", global.Tn);
        str += s;
    }
//    if (global.Eps) str += " -Eps";
    if (global.veter) {
      s.Format(" -veter %g", global.Tn);
      str += s;
    }
    return str;
}



