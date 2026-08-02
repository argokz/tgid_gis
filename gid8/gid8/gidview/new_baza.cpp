#include <QApplication>
#include <QFile>
#include <QRegularExpression>
#include <map>
#include <mystd.h>
#include <set>

#include "cxema/Graph.h"

struct LINEF2
{
    int typ;
    QString code;
    QString table_ps;
    QString table_mssql;
    QString table_ps_vn;
    QString name;
    QString table_out;
    QString state;
};

static LINEF2 linef[] = {

{TIP_UT,  "UT",   "new_baza.uchastok", "heatPipeSections",    "new_baza.line_vn", "Участок теплопровода",        "UT_OUT",  ""},
{TIP_HC,  "HC",   "new_baza.ns",       "pumps",               "new_baza.ns_vn",   "Насосы",                      "NS_OUT",  "stateID"},
{TIP_RD,  "RD",   "new_baza.rd",       "pressRegulators",     "new_baza.rd_vn",   "Сетевые регуляторы давления", "RS_OUT",  "regulatorStateID"},
{TIP_RR,  "RR",   "new_baza.rr",       "consumptRegulators",  "new_baza.rr_vn",   "Сетевые регуляторы расхода",  "RS_OUT",  "regulatorStateID"},
{TIP_ZD,  "ZD",   "new_baza.zd",       "dampers",             "new_baza.zd_vn",   "Задвижка",                    "ZD_OUT",  "damperArmatureStateID"},
{TIP_ZD2, "ZD2"   "new_baza.zd2",      "regulArmatures",      "new_baza.bp_zd2",  "Регулирующая арматура",       "ZD2_OUT", "damperArmatureStateID"},
{TIP_BP,  "BP",   "new_baza.bp",       "bypass",              "new_baza.bp_vn",   "Байпаc",                      "BP_OUT",  "regulatorStateID"},
{TIP_DR,  "DR",   "new_baza.dr",       "diaphragms",          "new_baza.bp_dr",   "Диафрагма",                   "DRO_OUT", "stateID"},
{TIP_EL,  "EL",   "",                  "elevators",           "new_baza.bp_el",   "Элеватор",                    "ANY_OUT", "stateID"},
{TIP_RO,  "RO",   "",                  "systemRadiators",     "new_baza.bp_ro",   "Радиатор",                    "ANY_OUT", "stateID"},
{TIP_TO,  "TO",   "",                  "heatExchangers",      "new_baza.bp_to",   "Теплообменник",               "ANY_OUT", "stateID"},
{TIP_KU,  "KU",   "",                  "airHeaters",          "new_baza.bp_ku",   "Калориферная установка",      "ANY_OUT", ""},
{TIP_RP,  "RP",   "",                  "pressDropRegulators", "new_baza.bp_rp",   "Регулятор перепада давления", "RS_OUT",  "regulatorStateID"},
{TIP_OK,  "OK",   "",                  "reverseValves",       "new_baza.bp_ok",   "Обратный клапан",             "OK_OUT",  "stateID"},

};

struct NODEF2
{
    int typ0;  // Номер для геобазы
    int typ;
    QString code;
    QString table_ps;
    QString table_mssql;
//    QString table_ps_vn;
    QString name;
    QString table_out;
//    QString state;
};


/*

(1, 'Узел'),
(2, 'Насосная станция'),
(3, 'Источник тепла'),
(4, 'Потребитель реальный'),
(5, 'Потребитель обобщенный')

*/


static NODEF2 nodef[] = {

{1, TIP_US,  "US",  "new_baza.uzel",          "",                       "Ненагруженный узел",     "US_OUT"},
{4, TIP_PR,  "PR",  "new_baza.real_potr",     "realConsumers",          "Потребитель реальный",   "PT_OUT"},
{5, TIP_PO,  "PO",  "new_baza.general_potr",  "generalizedConsumers",   "Потребитель обобщенный", "PT_OUT"},
{3, TIP_IS,  "IS",  "new_baza.istoch",        "heatSources",            "Источник тепла",         "US_OUT"},
{2, TIP_HS,  "HS",  "new_baza.nasosn",        "pumpStations",           "Насосная станция",       "US_OUT"},
{-1, TIP_C3,  "C3",  "",                       "threeWayValves",         "Клапан трехходовой",     "US_OUT"},
{-1, TIP_US2, "US2", "",                       "connectNodes",           "Ненагруженный узел",     "US_OUT"},
};

//static bool init = false;

static int nodefsize = sizeof(nodef)/sizeof(nodef[0]);
static int linefsize = sizeof(linef)/sizeof(linef[0]);

static map<int, LINEF2> map_LINEF2;
static map<int, NODEF2> map_NODEF2;

void init_line_node()
{
    for (int i = 0; i < linefsize; i++) {
        map_LINEF2[linef[i].typ] = linef[i];
    }

    for (int i = 0; i < nodefsize; i++) {
        map_NODEF2[nodef[i].typ] = nodef[i];
    }
}

QString get_line_table_ps(int typ)
{
    auto it = map_LINEF2.find(typ);
    if (it != map_LINEF2.end()) {
        return it->second.table_ps;
    }
    return "";
}

QString get_line_table_ps_vn(int typ)
{
    auto it = map_LINEF2.find(typ);
    if (it != map_LINEF2.end()) {
        return it->second.table_ps_vn;
    }
    return "";
}


QString get_node_table_ps(int typ)
{
    auto it = map_NODEF2.find(typ);
    if (it != map_NODEF2.end()) {
        return it->second.table_ps;
    }
    return "";
}


int get_node_typ0(const QString & tn)
{
    for (int i = 0; i < nodefsize; i++) {
        if (nodef[i].table_ps == tn) {
            return nodef[i].typ0;
        }
    }
    return -1;
}


