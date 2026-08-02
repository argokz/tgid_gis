// Cxema.cpp: implementation of the CCxema class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gid6.h"
#include "Cxema.h"
#include "MainFrm.h"

#include "gid6Doc.h"
#include <math.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <share.h>

#include "mmenu.h"
#include "win.h"

#include "ado.h"


#define M_PI        3.14159265358979323846

//#include "../TeploCalc/ForTeploCalc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

long ado_executeInsert(CAdoFile *ado, const char *q, long * affected);
bool ado_execute(CAdoFile *ado, const char *q, long * affected);


void log(const char *podp, const char *s);

CString name_node(CNode *node, int is_ps);
double getLengthLine(CLINE2 *ll, BOOL ismap);

bool setMarkNode1Value(CAdoFile *ado, CGraph2 *graph, const char *fn, const char *val, bool all);


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern int demoversion;


UCH2F uch2f[] = {
  1,"Воздушник",
  2,"Дренаж",
  3,"Компенсатор",
  4,"Неподвижная опора",
  5,"Спусковое устройство",
};

NODEF nodef[] = {
    TIP_TEPLO, 1, 1, TIP_US,  "US",  ST_US,   "",                       "Узел",                   "Ненагруженный узел", "US_OUT",
    TIP_TEPLO, 0, 1, TIP_PR,  "PR",  ST_PR,   "realConsumers",          "Потребитель реальный",   "Потребитель реальный", "PT_OUT",
    TIP_TEPLO, 0, 1, TIP_PO,  "PO",  ST_PO,   "generalizedConsumers",   "Потребитель обобщенный", "Потребитель обобщенный", "PT_OUT",
//  0, 1, TIP_UP,  "UP",  ST_UP,   "",                       "Узел подпитки"),         "Узлы подпитки и прис.баков-аккумуляторов", NULL,
    TIP_TEPLO, 0, 1, TIP_IS,  "IS",  ST_IS,   "heatSources",            "Источник тепла",         "Источник тепла", "US_OUT",
    TIP_TEPLO, 0, 1, TIP_HS,  "HS",  ST_HS,   "pumpStations",           "Насосная станция",       "Насосная станция", "US_OUT",
    TIP_TEPLO, 0, 1, TIP_C3,  "C3",  ST_C3,   "threeWayValves",         "Клапан трехходовой",     "Клапан трехходовой", "US_OUT",
//  0, 1, TIP_ZP,  "ZP",  ST_ZP,   "",                       "ЦТП",                    "ЦТП", "US_OUT",
//    TIP_TEPLO, 1, 1, TIP_US2, "US2", ST_US2, "connectNodes",            "Узел",                   "Ненагруженный узел", "US_OUT",
    TIP_TEPLO, 1, 1, TIP_US2, "US2", ST_US2, "connectNodes",            "Узел присоединения",                   "Узел присоединения", "US_OUT",

/// Водопровод

    TIP_VODA, 0, 1, TIP_V_PT,  "V_PR",  "Потребитель водоснабжения",   "voda_POTREBIT",          "Потребитель водоснабжения",   "Потребитель водоснабжения", "PT_OUT",
    TIP_VODA, 0, 1, TIP_V_IS,  "V_IS",  "Источник водоснабжения",      "voda_ISTOC",             "Источник водоснабжения",      "Источник водоснабжения", "PT_OUT",




//    0, 1, TIP_V_PT,  "V_PR",  "Потребитель водопровода",   "voda_POTREBIT",          "Потребитель водопровода",   "Потребитель водопровода", "PT_OUT",

};

LINEF linef[] = {

    TIP_TEPLO, 1, 0, TIP_UT,  "UT",   ST_UT,     "heatPipeSections",     "Участок",     "Участок теплопровода", "UT_OUT", "", -1, -1, -1,
    TIP_TEPLO, 1, 0, TIP_HC,  "HC",   ST_HC,     "pumps",                "Насос",       "Насосы(насосные станции расчетной схемы", "NS_OUT", "stateID", -1, -1, -1,
    TIP_TEPLO, 1, 0, TIP_RD,  "RD",   ST_RD,     "pressRegulators",      "Регул. давл.", "Сетевые регуляторы давления", "RS_OUT", "regulatorStateID", -1, -1, -1,
    TIP_TEPLO, 1, 0, TIP_RR,  "RR",   ST_RR,     "consumptRegulators",   "Регул. расх.", "Сетевые регуляторы расхода", "RS_OUT", "regulatorStateID", -1, -1, -1,
    TIP_TEPLO, 1, 0, TIP_ZD,  "ZD",   ST_ZD,     "dampers",              "Задвижка",    "Задвижка", "ZD_OUT", "damperArmatureStateID", -1, -1, -1,
    TIP_TEPLO, 1, 0, TIP_ZD2, "ZD2",  ST_ZD2,    "regulArmatures",       "Регулирующая арматура", "Регулирующая арматура", "ZD2_OUT", "damperArmatureStateID", -1, -1, -1,
    TIP_TEPLO, 1, 0, TIP_BP,  "BP",   ST_BP,     "bypass",               "Байпас",      "Байпаc", "BP_OUT", "regulatorStateID", -1, -1, -1,
    TIP_TEPLO, 1, 0, TIP_DR,  "DR",   ST_DR,     "diaphragms",           "Диафрагма",   "Диафрагма", "DRO_OUT", "stateID", -1, -1, -1,
    TIP_TEPLO, 1, 0, TIP_EL,  "EL",   ST_EL,     "elevators",            "Элеватор",    "Элеватор", "ANY_OUT", "stateID", -1, -1, -1,
    TIP_TEPLO, 1, 0, TIP_RO,  "RO",   ST_RO,     "systemRadiators",      "Радиатор",    "Радиатор", "ANY_OUT", "stateID", -1, -1, -1,
    TIP_TEPLO, 1, 0, TIP_TO,  "TO",   ST_TO,     "heatExchangers",       "Теплообменник","Теплообменник", "ANY_OUT", "stateID", -1, -1, -1,
    TIP_TEPLO, 1, 0, TIP_KU,  "KU",   ST_KU,     "airHeaters",           "Калориферная установка",  "Калориферная установка", "ANY_OUT", "", -1, -1, -1,
    TIP_TEPLO, 1, 0, TIP_RP,  "RP",   ST_RP,     "pressDropRegulators",  "Регул. переп.", "Регулятор перепада давления", "RS_OUT", "regulatorStateID", -1, -1, -1,
    TIP_TEPLO, 1, 0, TIP_OK,  "OK",   ST_OK,     "reverseValves",        "Обратный клапан", "Обратный клапан", "OK_OUT", "stateID", -1, -1, -1,

/// Водопровод
    TIP_VODA, 1, 0, TIP_V_UT,  "V_UT",   "Участок водопровода",     "voda_UCH",     "Участок",     "Участок водопровода", "UT_OUT", "", -1, -1, -1,


//  1, 0, TIP_RM,  "RM",   ST_RM,     "",        "Расходомер", "Расходомер", "RM_OUT", "stateID", -1, -1, -1,



//  1, 0, TIP_KM, ST_KM, "Компенсатор", "Компенсатор", NULL, "", -1, -1, -1,
//  1, 0, TIP_OK, ST_OK, "Обратный клапан",         "Обратный клапан", NULL, "", -1, -1, -1,
//  1, 0, TIP_TL, ST_TL, "Теплообменник Пл.",       "Теплообменник пластинчатый", NULL, "", -1, -1, -1,
//  1, 0, TIP_TT, ST_TT, "Теплообменник Тр.",       "Теплообменник трубчатый", NULL, "", -1, -1, -1,
};


int nodefsize = sizeof(nodef)/sizeof(nodef[0]);
int linefsize = sizeof(linef)/sizeof(linef[0]);
int uch2fsize = sizeof(uch2f)/sizeof(uch2f[0]);


CString getLineTable(int typ)
{
  if (typ < 0 || typ >= linefsize) return "";
  return linef[typ].table;
}

CString getLineTableMySQL(int typ)
{
  if (typ < 0 || typ >= linefsize) return "";
  return linef[typ].tableMySQL;
}


CString getLineTableOut(int typ)
{
  if (typ < 0 || typ >= linefsize) return "";
  return linef[typ].out;
}

CString getLineTableOutMySQL(int typ)
{
  if (typ < 0 || typ >= linefsize) return "";
  return linef[typ].out;
}

CString getLineOtkr(int typ)
{
  if (typ < 0 || typ >= linefsize) return "";
  return linef[typ].otkr;
}

int getNodeTyp(const char *tn)
{
    for (int i = 0; i < nodefsize; i++) {
        if (!strcmp(nodef[i].code, tn)) return i;
    }
    return -1;
}

int getNodeTyp2(const char *tn)
{
    for (int i = 0; i < nodefsize; i++) {
        if (!_stricmp(nodef[i].tableMySQL, tn)) return i;
    }
    return -1;
}


int getLineTyp3(const char *tn)
{
    for (int i = 0; i < linefsize; i++) {
        if (!strcmp(linef[i].table, tn)) return i;
    }
    return -1;
}



int getLineTyp(const char *tn)
{
    for (int i = 0; i < linefsize; i++) {
        if (!strcmp(linef[i].code, tn)) return i;
    }
    return -1;
}

int getLineTyp2(const char *tn)
{
    for (int i = 0; i < linefsize; i++) {
        if (!_stricmp(linef[i].tableMySQL, tn)) return i;
    }
    return -1;
}

int getLineTypOut(const char *tn)
{
    for (int i = 0; i < linefsize; i++) {
        if (!_stricmp(linef[i].out, tn)) return i;
    }
    return -1;
}

int getNodeTypOut(const char *tn)
{
    for (int i = 0; i < nodefsize; i++) {
        if (!_stricmp(nodef[i].out, tn)) return i;
    }
    return -1;
}

CString getNodeCode(int typ)
{
  if (typ < 0 || typ >= nodefsize) return "";
  return nodef[typ].code;
}


CString getNodeTable(int typ)
{
  if (typ < 0 || typ >= nodefsize) return "";
  return nodef[typ].table;
}




CString getNodeTableMySQL(int typ)
{
  if (typ < 0 || typ >= nodefsize) return "";
  return nodef[typ].tableMySQL;
}

CString getNodeTableOut(int typ)
{
  if (typ < 0 || typ >= nodefsize) return "";
  return nodef[typ].out;
}

CString getNodeTableOutMySQL(int typ)
{
  if (typ < 0 || typ >= nodefsize) return "";
  return nodef[typ].out;
}

CCxema::CCxema()
{
  m_AstanaMag = -1;
  m_city = "";

  is_google_map = GOOGLE_ALWAYS;

  m_name_typ = _TR("К");

  m_diametr = 1000;
  m_diametr_usl = 1000;
  m_diametr_vn = 1000;
  m_tol = 0;
//  m_kam_parent = NULL;

  m_graph = NULL;
  
  m_average_len = 0.;
  m_n_max = 0;
  m_n_undo = 0;
  isAdminFile = 1;
  m_cur_file = NULL;
  m_is_kod0 = 0;

  init_diameterInternal = 1004;
  init_diameterCondit = 1000;
  init_diameterExternal = 1020;
  init_wallThickness = 8;



//  cl_ud;
}

CCxema::~CCxema()
{
  if (m_graph) {
    delete m_graph;
    m_graph = nullptr;
  }
}


//#include "DataEdit.h"

#define NEW_EDIT

CString CCxema::getMAG(const char *mag)
{
  map <CString, CString>::const_iterator it;
  
  it = map_mag.find(CString(mag));

  if (it != map_mag.end()) {
    return it->second;
  }
  return CString("");
}

CString CCxema::getRS(const char *rs) 
{
  map <CString, CString>::const_iterator it;
  
  it = map_raspr.find(CString(rs));

  if (it != map_raspr.end()) {
    return it->second;
  }
  return CString("");
}

long CCxema::getIST(long mag)
{
  map <long, long>::const_iterator it;
  
  it = map_ist.find(mag);

  if (it != map_ist.end()) {
    return it->second;
  }
  return -1;
}

const RasCx * CCxema::getPC(int kod_rs)
{
  map <long, RasCx>::const_iterator it = map_rs.find(kod_rs);

  if (it != map_rs.end()) {
    return &it->second;
  }
  return NULL;
}

const Fragments * CCxema::getFragments(int kod_rs)
{
  map <int, Fragments>::const_iterator it = map_fragments.find(kod_rs);

  if (it != map_fragments.end()) {
    return &it->second;
  }
  return NULL;
}



bool CCxema::isShow(const char *kod_rs)
{
  map <CString, int>::const_iterator it;
  
  it = map_pc_show.find(CString(kod_rs));

  if (it != map_pc_show.end()) {
    return (bool) it->second;
  }
  return false;
}


double sopr(double dH, double G)
{
  return G == 0 ? 0 : fabs(dH)/G/G;
}

/*
double get_kvs(double kvs_tr)
{
  double k1 = 0.7, k2 = 1.3;
  return 1.1*kvs_tr; 
//  return 10; 
}
*/

double getPnas(double dPru, double Gmax)
{
  return dPru;
}

CString CCxema::get_new_name(CGidFile *gid_file, const char *kod)
{
  return "";
}

bool CCxema::isGeobaza()
{
  extern int is_geobaza;

  is_geobaza = 1;
  
  return m_average_len > 10 || (m_average_len == 0 && is_geobaza) || is_google_map || m_graph->map_node.size() < 5;
}


extern TypMenu m_idMenu;

void closeRemont(const char *tn, int n);


void setTyp(char *cxema, CNode *node);
CString getPSTable(int n);

int editAlma(CGidrView *parent, CNode *n, const char *fn, int ro);

CString name_node(CNode *node, int is_ps);

//-----------------------------------------------
// Установить узел с заданным напором
//-----------------------------------------------

bool CCxema::setZN(CNode2 *n)
{
  if (!n) return false;

  CString q;

  q.Format("INSERT INTO %s (nodeID) VALUES (%d)", "setPressNodes", n->id);
  long affected;
  long id2 = ado_executeInsert(m_ado, q, &affected);

  if (id2 == 0) {
      AfxMessageBox(m_ado->getError(), MB_OK|MB_ICONINFORMATION);
  }
  else {
    n->node.nZN = id2;
  }

  return true;
}


bool CCxema::setPRIBOR(CNode2 *n)
{
  AfxMessageBox(_TR("Временно удалено 1"), MB_OK|MB_ICONINFORMATION);
  return true;
}


bool CCxema::delPRIBOR(CNode2 *n)
{
  AfxMessageBox(_TR("Временно удалено 2"), MB_OK|MB_ICONINFORMATION);
  return true;
}


//-----------------------------------------------
// Установить узел подпитки
//-----------------------------------------------

bool CCxema::setUP(CNode2 *n)
{
  if (!n) return false;

  CString q;

  q.Format("INSERT INTO %s (nodeID) VALUES (%d)", "refillNodes", n->id);
  long affected;
  long id2 = ado_executeInsert(m_ado, q, &affected);

  if (id2 == 0) {
      AfxMessageBox(m_ado->getError(), MB_OK|MB_ICONINFORMATION);
  }
  else {
    n->node.nUP = id2;
  }

  return true;
}

double getLengthLine(CLINE2 *ll, BOOL ismap)
{
  CLine2 *l = bline(ll);
  CNode2 *n1 = where(ll);
  CNode2 *n2 = other(ll);

  double l1, l2, len;

  if (l->line.m_NP.size() == 0) {
    len = LengthG(n1->node.coord, n2->node.coord, ismap);
  }
  else {
    CFPoint c1 = l->line.m_NP.first();
    CFPoint c2 = l->line.m_NP.last();

    len = l->line.m_NP.Length();
    l1 = LengthG(n1->node.coord, c1, ismap);
    l2 = LengthG(n2->node.coord, c2, ismap);
    len = len+l1+l2;
  }
  return len;
}

double getLengthLine(CFPoint p1, CFPoint p2, CCoordList &cl, BOOL ismap)
{
  double l1, l2, len;

  if (cl.size() == 0) {
    len = LengthG(p1, p2, ismap);
  }
  else {
    CFPoint c1 = cl.first();
    CFPoint c2 = cl.last();

    len = cl.Length();
    l1 = LengthG(p1, c1, ismap);
    l2 = LengthG(p2, c2, ismap);
    len = len+l1+l2;
  }
  return len;
}


bool CCxema::setLength(CLINE2 *ll, bool ismap, bool ask)
{
  CLine2 *l = bline(ll);
  CNode2 *n1 = where(ll);
  CNode2 *n2 = other(ll);


  double l1, l2, len;

  if (l->line.m_NP.size() == 0) {
    len = LengthG(n1->node.coord, n2->node.coord, ismap);
  }
  else {
    CFPoint c1 = l->line.m_NP.first();
    CFPoint c2 = l->line.m_NP.last();

    len = l->line.m_NP.Length();
    l1 = LengthG(n1->node.coord, c1, ismap);
    l2 = LengthG(n2->node.coord, c2, ismap);
    len = len+l1+l2;
  }

  long affected;
  CString q;

  q.Format("UPDATE %s SET pipeSectLength=%.1f WHERE id=%d OR id=%d", 
    l->getTableMySQL(), len, l->line.idP2, l->line.idO2);

  if (ado_execute(m_ado, q, &affected)) {
      l->line.pod.dl = l->line.obr.dl = len;

      if (ask) AfxMessageBox(_TR("Выполнено!"), MB_OK|MB_ICONINFORMATION);
  }


  return true;
}

bool LoadFont(const TCHAR *szSection, LOGFONT &lf, COLORREF &color);


bool CCxema::addText(CFPoint pt, const char *txt, int internalNodeID, int fileID)
{
  LOGFONT lf;
  COLORREF color = 0;

  memset(&lf, 0, sizeof(LOGFONT));
  strcpy(lf.lfFaceName, "Arial");
  lf.lfHeight = 10;

  LoadFont(_T("Font Gidr 2"), lf, color);

  long affected;
  CText text;

  CString q;

  q.Format("INSERT INTO texts (internalNodeID,label_text,font_name,font_size,x,y,fileID) "
  " VALUES (%d,'%s','%s',%d,%g,%g,%d)", internalNodeID, txt, lf.lfFaceName, lf.lfHeight, pt.x, pt.y, fileID);
  
  int id = m_ado->ExecuteInsert(q, &affected);

  if (id > 0) {
      text.lf = lf;
      text.text = txt;
      text.coord = pt;
      text.fileID = fileID;
      text.internalNodeID = internalNodeID;
      text.n = id;

      set_text.insert(text);
  }

  return true;
}

bool CCxema::delText(const CText *txt)
{
    long affected;
    CString q;
    q.Format("DELETE FROM texts WHERE id=%d", txt->n);
    bool ret = m_ado->Execute(q, &affected);
    if (ret) {
        set_text.erase(*txt);
    }
    
    return true;
}

int read_cxema(const char *cxema, const char *name1, char p1, const char *name2, char p2);


bool CCxema::zima_leto(int fileID, bool zima)
{
  AfxMessageBox(_TR("Временно удалено 3"), MB_OK|MB_ICONINFORMATION);
  return true;
}

CString add_pref(const char * fn, const char * name)
{
  CString str;

  if (name[0] == '#') {
    str.Format("#%s%s", fn, &name[1]);
  }
  else {
    str.Format("%s%s", fn, name);
  }
  str = str.Left(10);
  return str;
}

/*

bool CCxema::setMark(const CCoordList &NP, bool val)
{

  CLINE2 *l;

  CMapGraph::iterator p = m_graph->map_node.begin();

  while (p != m_graph->map_node.end()) {
    CNode2 *n = p->second;

    if (!n2->isHide && NP.find3(n->node.coord)) {
      n->node.isPjezo = val;

      for (l = n->lines; l; l = next(l)) {
        if (!IsBegin(l)) continue;
        CNode2 *n2 = other(l);
        if (n2 && !bline(l)->isHide && NP.find3(n2->node.coord)) {
          bline(l)->line.isPjezo = val;
        }
      }
    }
    p++;
  }

  return true;
}


bool CCxema::isMark()
{
  CMapGraph::iterator pp = m_graph->map_node.begin();

  while (pp != m_graph->map_node.end()) {
    if (pp->second->node.isPjezo) return true;
    pp++;
  }
  return false;
}

*/


bool CCxema::exportShape(const char *fn)
{
  ExportSHAPE(fn, ST_PO, TIP_PO, 1, "PO");
  ExportSHAPE(fn, ST_UT, TIP_UT, 2, "UT");
  ExportSHAPE(fn, ST_US, TIP_US, 1, "US");
  ExportSHAPE(fn, ST_PR, TIP_PR, 1, "PR");
//  ExportSHAPE(fn, ST_PO, TIP_PO, 1, "PO");

  ExportSHAPE(fn, ST_IS, TIP_IS, 1, "IS");

  return true;
}

struct PR_2 {
  char   sost;  //  Состояние потребителя
  char   cxema[6];  //  Номер расчетной схемы
  char   name_building[21];  //  Содержательное наименование здания
  double h;  //  Высота здания
  double geodz;  //  Геодезическая отметка низа трубы подвальной прокладки
  char   kodur[4];  //  Код удельных расходов тепло/вода
  char   kodtr[4];  //  Код расчетных температур
  char   kodkv[4];  //  Код группы коэффициентов вариации

  double otoplz;  //  Расчетная нагрузка на  отопление
  double otopln;  //  Расчетная нагрузка на  отопление
  double otopl_tp;  //  Расчетные внутренние тепловыделения
  double ventil;  //  Расчетная нагрузка  на вентиляцию
  double dolja_vent;  //  Доля расхода воды через КУ, при откл.её по возд.
  double gvop;  //  Средняя тепловая нагрузка на горячее водоснабжение
  double gvoo;  //  Средняя тепловая нагрузка на горячее водоснабжение
  double gvpr;  //  Средняя тепловая нагрузка на горячее водоснабжение
  double gvsm;  //  Средняя тепловая нагрузка на горячее водоснабжение
  double gvps;  //  Средняя тепловая нагрузка на горячее водоснабжение
  double gvpw;  //  Средняя тепловая нагрузка на горячее водоснабжение

  double uf;  //  Коэффициент  смешения  элеватора
  double gvs_max;  //  Коэфф.часовой неравномерности нагрузки горячего водоснабжения
  double rez;  //  Расч.тепл.нагрузка на компенсацию тепл.потерь в теплопр.цирк. ГВС
  double utechp;  //  Утечка из подающего теплопровода
  double utecho;  //  Утечка из обратного  теплопровода
  double Tr_tv_co;  //  Расчетная температура в отапливаемых помещений
  double Tr_tv_cv;  //  Расчетная температура нагретого воздуха после КУ
  double Tr_gvs;  //  Расчетная температура горячей воды
};

#include "Bgi.h"

#define N_OP 256
static COper m_oper[N_OP];
static int m_n1 = 0, m_n2 = 0;

void CCxema::operat_begin(int typop, void *p1, void *p2)
{
  
  m_geofile->m_n_undo = 0;

  m_typop = typop;

  if (typop == OP_DEL_LINE) {
    CLINE2 *l = (CLINE2 *) p1;
    m_n_undo = 1;
  }
  else if (typop == OP_SET_XY) {
    m_n_undo = 1;
  }
  else if (typop == OP_EDIT_GR) {
    m_n_undo = 1;
  }
  else {
    m_n_undo = 0;
  }
}

void CCxema::operat(int typop, void *p1, void *p2)
{
/*  

  COper oper;
  oper.oper = typop;

  m_n2 = (m_n2+1)%N_OP;

  m_oper[m_n2] = oper;

  CString str;

  str.Format("m_n2 = %d typ = %d", m_n2, m_oper[m_n2].oper);

//  AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
*/

}

void CCxema::undo()
{
/*
  CString str;

  if (m_n_undo) {
    if (m_typop == OP_DEL_LINE) {
      if (AfxMessageBox(_TR("Отменить удаление линии?"), MB_YESNO) == IDYES) {
        addLineUndo(del_line);
        m_n_undo = 0;
      }
    }
    else if (m_typop == OP_SET_XY) {
      if (AfxMessageBox(_TR("Отменить перенос узла?"), MB_YESNO) == IDYES) {
        setXY(move_node.n, move_node.pt);
        m_n_undo = 0;
      }
    }
    else if (m_typop == OP_EDIT_GR) {
      if (AfxMessageBox(_TR("Отменить перенос линии?"), MB_YESNO) == IDYES) {
        bline(move_line.l)->line.m_NP = move_line.cl;
        setCoord(move_line.l, move_line.cl);
        m_n_undo = 0;
      }
    }
    else {
      m_n_undo = 0;
    }
  }
*/
}

// Объединить подачу и обратку

bool ado_execute(CAdoFile *ado, const char *q, long * affected);


bool CCxema::uniteLine(CLINE2 *line)
{
  CLine2 *l = bline(line);

  CString q;
  long affected;

  if (l->line.nomP <= 0) return false;
  if (l->line.nomO <= 0) return false;
  if (l->line.nomP == l->line.nomO) return false;

  q.Format("UPDATE linesobj SET externalSignLineID=1 WHERE id=%d", l->line.nomP);

  if (ado_execute(m_ado, q, &affected)) {
      q.Format("DELETE FROM linesobj WHERE id=%d", l->line.nomO);
      if (ado_execute(m_ado, q, &affected)) {
          bline(line)->line.nomO = bline(line)->line.nomP;
      }
  }

  return false;  
}

// Объединить подачу и обратку для несовпадающих участков

bool CCxema::uniteLine2(CLINE2 *line1)
{
  CGraph2 *graph = where(line1)->node.m_graph;

  if (bline(line1)->line.nomP == -1) line1 = graph->find_line_obr(line1);

  CLINE2 *line2 = graph->find_line_obr(line1);

  CLine2 *l1 = bline(line1);
  CLine2 *l2 = bline(line2);

  if (l2 && (l2->line.nomP == -1 || l2->line.nomO == -1) && (l2->line.nomP != l1->line.nomP) && (l2->line.nomO != l1->line.nomO)) {

      CCoordList cl = l1->line.m_NP;
      CString coord = cl.saveStr();
      CString q, qq;
      long affected;

      q.Format("UPDATE linesobj SET coords='%s', nodeID1=%d, nodeID2=%d WHERE ID=%d OR ID=%d", coord, where(line1)->id, other(line1)->id, l2->line.nomP, l2->line.nomO);

      if (ado_execute(m_ado, q, &affected)) {

        qq.Format("l.id=%d OR l.id=%d", l2->line.nomP, l2->line.nomO);
        reset_shape_line(m_ado, qq);

        l1->line.m_NP = cl;
        if (l1->line.nomO == -1) {
            l1->line.nomO = l2->line.nomO;
            l1->line.idO2 = l2->line.idO2;
        }
        if (l1->line.nomP == -1) {
            l1->line.nomP = l2->line.nomP;
            l1->line.idP2 = l2->line.idP2;
        }

        m_graph->remove_line(l2);
        m_graph->change();

        l1->line.isPjezo = false;

        return true;
      }
  }

  return false;  
}


void set_time_local(char *s, time_t t) 
{
    struct tm *nt;
    nt = localtime( &t );
    sprintf(s, "%d-%02d-%02d %d:%02d:%02d", nt->tm_year+1900, nt->tm_mon+1, nt->tm_mday, nt->tm_hour, nt->tm_min, nt->tm_sec);
//--    printf( "%s\n", s);
}



bool CCxema::createTAG(const CTime &t, int cur)
{
  return true;
}


void preobr(const CFPoint *p1, const CFPoint *p2, double *x);

bool CCxema::TrioMove(CFPoint *m_trio_point)
{
  AfxMessageBox(_TR("Временно удалено 4"), MB_OK|MB_ICONINFORMATION);
  return true;  
}

bool CCxema::TrioMove2(CGidFile *gid_file, CFPoint *m_trio_point)
{
  AfxMessageBox(_TR("Временно удалено 5"), MB_OK|MB_ICONINFORMATION);
  return true;  
}


#if 1

void CCxema::create_vyd_table(CAdoFile *ado)
{
  long affected;
  CString sql;

  CString tn = "refill";

  sql.Format("DROP TABLE %s", tn);
  ado->Execute(sql, &affected);

  sql.Format("CREATE TABLE %s (id INT NOT NULL AUTO_INCREMENT, PRIMARY KEY (id))", tn);
  ado->Execute(sql, &affected );

  CMapGraph::const_iterator p;

  bool is_vyd = false;
  p = m_graph->map_node.begin();
  while (p != m_graph->map_node.end()) {
    CNode2 *n = p->second;
    if (n->node.isPjezo) {
      is_vyd = true;
      break;
    }
    p++;
  }

  p = m_graph->map_node.begin();

  while (p != m_graph->map_node.end()) {
    CNode2 *n = p->second;
    if (!is_vyd || n->node.isPjezo) {
      sql.Format("INSERT INTO %s (id) VALUES (%d)", tn, n->id);
      ado->ExecuteInsert(sql, &affected );
    }
    p++;
  }
}
#endif


BOOL CCxema::isReadOnly()
{
//  if (m_cxema && m_cxema->m_cur_file->m_dbf && m_cxema->m_cur_file->m_dbf->isReadOnly()) return TRUE;

//  if (m_cur_file && m_cur_file->id && m_cur_file->m_dbf->isReadOnly()) return TRUE;
  
  return FALSE;
}


CString getParFragments()
{
    CGidrView *pView = getView();

    if (pView) {
        return pView->m_cxema->m_par;
    }
    return "";
}


bool CCxema::exportCxe(const char *fn)
{
    return true;
}

bool CCxema::importCxe(const char *fn)
{
    return true;
}

