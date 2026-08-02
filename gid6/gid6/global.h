#pragma once

struct GLOBAL
{
    int is_ZULU_SM;
    int is_ZULU;
    int is_ZN0;
    int is_save_po;
    int is_save_po_yes;
    int iter;
    int g_equival;
    int is_kv;
    int g_is_avar;
    int is_dross_yes;
    int avtomat_yes;
    int is_tg;
    int tp_metod;
    int n_GWS;
    int n_GWS2;
    int n_trtp;
    int no_teplovyd;
    int no_teplopoter;
    int uf_calc;
    int is_mag_fragment;
    int is_char_sety;
    int is_no_balans;
    int is_veter;
    int is_no_current;
    int is_save_uf_new;
    int is_leto;
    int is_poteri;
    int is_dop;
    int is_fakt;
    int is_no_out;
    int Month;
    int n_nagr;
    double Tn;
    double Eps;
    double veter;
};

//map<int, GLOBAL> map_global;



bool read_calc_param(const CString& param, GLOBAL &global);
CString get_calc_param(GLOBAL& global);
GLOBAL *get_calc_param_global_id(int id);
