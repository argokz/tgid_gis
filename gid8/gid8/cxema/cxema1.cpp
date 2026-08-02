#include <QCoreApplication>

#include "cxema/Graph.h"
#include "cxema/cxema_names.h"
#include "cxema1.h"

UCH2F uch2f[] = {
  {1,"Воздушник"},
  {2,"Дренаж"},
  {3,"Компенсатор"},
  {4,"Неподвижная опора"},
  {5,"Спусковое устройство"},
};

NODEF nodef[] = {
    {TIP_TEPLO, 1, 1, TIP_US,  "US",  ST_US,   "",                       "Узел",                   "Ненагруженный узел", "US_OUT"},
    {TIP_TEPLO, 0, 1, TIP_PR,  "PR",  ST_PR,   "realConsumers",          "Потребитель реальный",   "Потребитель реальный", "PT_OUT"},
    {TIP_TEPLO, 0, 1, TIP_PO,  "PO",  ST_PO,   "generalizedConsumers",   "Потребитель обобщенный", "Потребитель обобщенный", "PT_OUT"},
//  0, 1, TIP_UP,  "UP",  ST_UP,   "",                       "Узел подпитки"),         "Узлы подпитки и прис.баков-аккумуляторов", NULL,
    {TIP_TEPLO, 0, 1, TIP_IS,  "IS",  ST_IS,   "heatSources",            "Источник тепла",         "Источник тепла", "US_OUT"},
    {TIP_TEPLO, 0, 1, TIP_HS,  "HS",  ST_HS,   "pumpStations",           "Насосная станция",       "Насосная станция", "US_OUT"},
//    {TIP_TEPLO, 0, 1, TIP_C3,  "C3",  ST_C3,   "threeWayValves",         "Клапан трехходовой",     "Клапан трехходовой", "US_OUT"},
//  0, 1, TIP_ZP,  "ZP",  ST_ZP,   "",                       "ЦТП",                    "ЦТП", "US_OUT",
    {TIP_TEPLO, 1, 1, TIP_US2, "US2", ST_US2,  "connectNodes",           "Узел",                   "Ненагруженный узел", "US_OUT"},

//    {TIP_TEPLO, 1, 1, TIP_US_VN, "US_VN", "Узел внутр.", "new_baza.uzel_vn",            "Узел",                   "Ненагруженный внутренний", "US_OUT"},

/// Водопровод

//    {TIP_VODA, 0, 1, TIP_V_PT,  "V_PR",  "Потребитель водоснабжения",   "voda_POTREBIT",          "Потребитель водоснабжения",   "Потребитель водоснабжения", "PT_OUT"},
//    {TIP_VODA, 0, 1, TIP_V_IS,  "V_IS",  "Источник водоснабжения",      "voda_ISTOC",             "Источник водоснабжения",      "Источник водоснабжения", "PT_OUT"},




//    0, 1, TIP_V_PT,  "V_PR",  "Потребитель водопровода",   "voda_POTREBIT",          "Потребитель водопровода",   "Потребитель водопровода", "PT_OUT",

};

LINEF linef[] = {

    {TIP_TEPLO, 1, 0, TIP_UT,  "UT",   ST_UT,     "heatPipeSections",     "Участок",     "Участок теплопровода", "UT_OUT", "", -1, -1, -1},
    {TIP_TEPLO, 1, 0, TIP_HC,  "HC",   ST_HC,     "pumps",                "Насос",       "Насосы(насосные станции расчетной схемы", "NS_OUT", "stateID", -1, -1, -1},
    {TIP_TEPLO, 1, 0, TIP_RD,  "RD",   ST_RD,     "pressRegulators",      "Регул. давл.", "Сетевые регуляторы давления", "RS_OUT", "regulatorStateID", -1, -1, -1},
    {TIP_TEPLO, 1, 0, TIP_RR,  "RR",   ST_RR,     "consumptRegulators",   "Регул. расх.", "Сетевые регуляторы расхода", "RS_OUT", "regulatorStateID", -1, -1, -1},
    {TIP_TEPLO, 1, 0, TIP_ZD,  "ZD",   ST_ZD,     "dampers",              "Задвижка",    "Задвижка", "ZD_OUT", "damperArmatureStateID", -1, -1, -1},
    {TIP_TEPLO, 1, 0, TIP_ZD2, "ZD2",  ST_ZD2,    "regulArmatures",       "Регулирующая арматура", "Регулирующая арматура", "ZD2_OUT", "damperArmatureStateID", -1, -1, -1},
    {TIP_TEPLO, 1, 0, TIP_BP,  "BP",   ST_BP,     "bypass",               "Байпас",      "Байпаc", "BP_OUT", "regulatorStateID", -1, -1, -1},
    {TIP_TEPLO, 1, 0, TIP_DR,  "DR",   ST_DR,     "diaphragms",           "Диафрагма",   "Диафрагма", "DRO_OUT", "stateID", -1, -1, -1},
    {TIP_TEPLO, 1, 0, TIP_EL,  "EL",   ST_EL,     "elevators",            "Элеватор",    "Элеватор", "ANY_OUT", "stateID", -1, -1, -1},
    {TIP_TEPLO, 1, 0, TIP_RO,  "RO",   ST_RO,     "systemRadiators",      "Радиатор",    "Радиатор", "ANY_OUT", "stateID", -1, -1, -1},
    {TIP_TEPLO, 1, 0, TIP_TO,  "TO",   ST_TO,     "heatExchangers",       "Теплообменник","Теплообменник", "ANY_OUT", "stateID", -1, -1, -1},
    {TIP_TEPLO, 1, 0, TIP_KU,  "KU",   ST_KU,     "airHeaters",           "Калориферная установка",  "Калориферная установка", "ANY_OUT", "", -1, -1, -1},
    {TIP_TEPLO, 1, 0, TIP_RP,  "RP",   ST_RP,     "pressDropRegulators",  "Регул. переп.", "Регулятор перепада давления", "RS_OUT", "regulatorStateID", -1, -1, -1},
    {TIP_TEPLO, 1, 0, TIP_OK,  "OK",   ST_OK,     "reverseValves",        "Обратный клапан", "Обратный клапан", "OK_OUT", "stateID", -1, -1, -1},

/// Водопровод
    {TIP_VODA, 1, 0, TIP_V_UT,  "V_UT",   "Участок водопровода",     "voda_UCH",     "Участок",     "Участок водопровода", "UT_OUT", "", -1, -1, -1},


//  1, 0, TIP_RM,  "RM",   ST_RM,     "",        "Расходомер", "Расходомер", "RM_OUT", "stateID", -1, -1, -1,



//  1, 0, TIP_KM, ST_KM, "Компенсатор", "Компенсатор", NULL, "", -1, -1, -1,
//  1, 0, TIP_OK, ST_OK, "Обратный клапан",         "Обратный клапан", NULL, "", -1, -1, -1,
//  1, 0, TIP_TL, ST_TL, "Теплообменник Пл.",       "Теплообменник пластинчатый", NULL, "", -1, -1, -1,
//  1, 0, TIP_TT, ST_TT, "Теплообменник Тр.",       "Теплообменник трубчатый", NULL, "", -1, -1, -1,
};

struct menuitemMySQL {
    QString name;
    QString kode;
    QString query;
    QString nameMySQL;
    int node_line;
};



menuitem menudocUnvis[] = {
    //    ST_VP, "VP", NULL, "WDOdevices",1,      //  Водоразборный прибор 

    //  ST_ZE, "ZE", NULL, "",   //  Здание 
        ST_KV, "KV", NULL, "varCoefficients", 0, //  Коэффициенты вариации 
    //  ST_MT, "MT", NULL, "",   //  Манометр 
    //  ST_V1, "V1", NULL, "",   //  Местные сопротивления участка трубопровода 


    //   _TR("Объекты с измеряемыми параметрами"),_TR("Объекты с измеряемыми параметрами"), NULL, "",   //  Объекты с измеряемыми параметрами 
    //   ST_OR, "OR", NULL, "",   //  Организация 
    //   ST_PRIBOR, "PRIBOR", NULL, "",
    //   ST_RN, "RN", NULL, "",   //  Район эксплуатации 

       ST_PC, "PC", NULL, "externalCodes", 0,      //  Расчетная схема 
    //   ST_CT, "CT", NULL, "fragments", 0,        //  Система теплоснабжения 
    // ST_TE, "TE", NULL, "",   //  Термометр 
       ST_UR, "UR", NULL, "specExpends", 0,      //  Удельные расходы 
       ST_UP, "UP", NULL, "refillNodes", 1,   //  Узел подпитки 
       ST_ZN, "ZN", NULL, "setPressNodes", 1,    //  Узел с заданным напором 
    //   ST_ZP, "ZP", NULL, "",   //  ЦТП 
//       NULL, "", NULL, "",
};



int nodefsize = sizeof(nodef)/sizeof(nodef[0]);
int linefsize = sizeof(linef)/sizeof(linef[0]);
int uch2fsize = sizeof(uch2f)/sizeof(uch2f[0]);
int menuitemsize = sizeof(menudocUnvis)/sizeof(menudocUnvis[0]);


QString getOutTable(const QString & tn)
{
    if (!QString::compare("nodes", tn)) return "US_OUT";

    for (int i = 0; i < linefsize; i++) {
        if (!QString::compare(linef[i].tableMySQL, tn)) return linef[i].out;
    }
    for (int i = 0; i < nodefsize; i++) {
        if (!QString::compare(nodef[i].tableMySQL, tn)) return nodef[i].out;
    }
    return "";
}

QString getLineCode(int typ)
{
  if (typ < 0 || typ >= linefsize) return "";
  return linef[typ].code;
}

QString getLineTable(int typ)
{
  if (typ < 0 || typ >= linefsize) return "";
  return linef[typ].table;
}

QString getLineTableMySQL(int typ)
{
  if (typ < 0 || typ >= linefsize) return "";
  return linef[typ].tableMySQL;
}


QString getLineTableOut(int typ)
{
  if (typ < 0 || typ >= linefsize) return "";
  return linef[typ].out;
}

QString getLineTableOutMySQL(int typ)
{
  if (typ < 0 || typ >= linefsize) return "";
  return linef[typ].out;
}

QString getLineOtkr(int typ)
{
  if (typ < 0 || typ >= linefsize) return "";
  return linef[typ].otkr;
}

int getNodeTyp(const QString & tn)
{
    for (int i = 0; i < nodefsize; i++) {
        if (!QString::compare(nodef[i].code, tn)) return i;
    }
    return -1;
}


int getLineTyp(const QString & tn)
{
    for (int i = 0; i < linefsize; i++) {
        if (!QString::compare(linef[i].code, tn)) return i;
    }
    return -1;
}

int getLineTyp2(const QString & tn)
{
    for (int i = 0; i < linefsize; i++) {
        if (!QString::compare(linef[i].table, tn)) return i;
    }
    return -1;
}

int getLineTypName(const QString & tn)
{
    for (int i = 0; i < linefsize; i++) {
        if (!QString::compare(linef[i].tableMySQL, tn)) return i;
    }
    return -1;
}

int getNodeTypName(const QString & tn)
{
    for (int i = 0; i < nodefsize; i++) {
        if (!QString::compare(nodef[i].tableMySQL, tn)) return i;
    }
    return -1;
}


int getLineTypOut(const QString & tn)
{
    for (int i = 0; i < linefsize; i++) {
        if (!QString::compare(linef[i].out, tn)) return i;
    }
    return -1;
}

int getNodeTypOut(const QString & tn)
{
    for (int i = 0; i < nodefsize; i++) {
        if (!QString::compare(nodef[i].out, tn)) return i;
    }
    return -1;
}


QString getNodeCode(int typ)
{
  if (typ < 0 || typ >= nodefsize) return "";
  return nodef[typ].code;
}


QString getNodeTable(int typ)
{
  if (typ < 0 || typ >= nodefsize) return "";
  return nodef[typ].table;
}

QString getNodeTableMySQL(int typ)
{
  if (typ < 0 || typ >= nodefsize) return "";
  return nodef[typ].tableMySQL;
}

QString getNodeTableOut(int typ)
{
  if (typ < 0 || typ >= nodefsize) return "";
  return nodef[typ].out;
}

QString getNodeTableOutMySQL(int typ)
{
  if (typ < 0 || typ >= nodefsize) return "";
  return nodef[typ].out;
}

CCxema::CCxema()
{
    m_graph = new CGraph2(this);
    m_fileID = -1;
}

CCxema::~CCxema()
{
  if (m_graph) {
    delete m_graph;
    m_graph = nullptr;
  }
}


Fragment *CCxema::getFragment(int id)
{
    auto it = map_fragments.find(id);
    if (it != map_fragments.end()) {
        return &it->second;
    }
    return nullptr;

}

QString CCxema::getFileName(int fileID)
{
    Fragment * fr = this->getFragment(fileID);

    if (fr) {
        return fr->m_name;
    }
    return "";
}

QString CCxema::getFileName()
{
    return getFileName(m_fileID);
}

#include <db/db.h>

bool CCxema::addText(CFPoint pt, const QString &txt, int fileID, int parent_id)
{
    CText text;

    if (parent_id > 0) {
        CNode2 *node = m_graph->find(parent_id);
        if (node) {
            fileID = node->node.fileID;
        }
    }

/*
  QString q = QString("INSERT INTO texts (internalNodeID,label_text,font_name,font_size,x,y,fileID) "
  " VALUES (%1,'%2','%3',%4,%5,%6,%7)")
    .arg(parent_id)
    .arg(txt)
    .arg("Arial")
    .arg(14)
//    .arg(lf.lfFaceName)
//    .arg(lf.lfHeight)
    .arg(pt.x)
    .arg(pt.y)
    .arg(fileID);
*/
//  int id  = execInsertQ(m_db, q, true);


    //    .arg(lf.lfFaceName)
    //    .arg(lf.lfHeight)

    std::map<QString, QVariant> data;

    data["internalNodeID"] = parent_id;
    data["label_text"] = txt;
    data["font_name"] = "Arial";
    data["font_size"] = 14;
    data["x"] = pt.x;
    data["y"] = pt.y;
    data["fileID"] = fileID;

  int id = insertIntoDatabase(m_db, "texts", data);


  if (id > 0) {
//      text.lf = lf;
      text.text = txt;
      text.coord = pt;
      text.fileID = fileID;
      text.internalNodeID = parent_id;
      text.n = id;
      set_text.push_back(text);
//      set_text.insert(text);
      return true;
  }

  return false;
}


