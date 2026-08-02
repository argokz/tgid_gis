// Cxema.h: interface for the CCxema class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CXEMA_H__016F4120_D166_4723_B143_B5E9B1C3DB56__INCLUDED_)
#define AFX_CXEMA_H__016F4120_D166_4723_B143_B5E9B1C3DB56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "graph.h"
#include "Graph2.h"

#include "GeoFile.h"

#include <io.h>
#include <sys/locking.h>

#include <excel.h>

struct PT_DEF {
   PT_DEF() {
       is_open = false;
   };

   void save()
   {
      AfxGetApp()->WriteProfileInt("PT", "is_open", is_open);
      AfxGetApp()->WriteProfileInt("PT", "calcTemperatureID", calcTemperatureID);
      AfxGetApp()->WriteProfileInt("PT", "specExpendID", specExpendID);
      AfxGetApp()->WriteProfileInt("PT", "varCoeffID", varCoeffID);
   };

   void restore()
   {
      is_open = AfxGetApp()->WriteProfileInt("PT", "is_open", 0);
      if (is_open) {
          calcTemperatureID = AfxGetApp()->GetProfileInt("PT", "calcTemperatureID", -1);
          specExpendID = AfxGetApp()->GetProfileInt("PT", "specExpendID", -1);
          varCoeffID = AfxGetApp()->GetProfileInt("PT", "varCoeffID", -1);
      }
   };


   bool is_open;

   map <int, map <CString, CString> > map_v;

   int calcTemperatureID; // int DEFAULT NULL, -- COMMENT 'Ссылка на код расчетных температур',
   int specExpendID; // int DEFAULT NULL, -- COMMENT 'Ссылка на код удельных расходов тепло/вода, для расчетного режима',
   int varCoeffID; //  int DEFAULT NULL, -- COMMENT 'Код группы коэффициентов вариации',
};


struct UT_DEF {
   UT_DEF() {
       is_open = false;
   };
   bool is_open;

   map <CString, CString> map_v;
   
   
   double diameterInternal; // float DEFAULT 1000, --COMMENT 'Диаметр внутренний',
   double diameterCondit; // float DEFAULT 1000, --COMMENT 'Диаметр условный',
   double diameterExternal; // float DEFAULT 1000, --COMMENT 'Диаметр наружный',
   double wallThickness; //  float DEFAULT 1, --COMMENT 'Толщина стенки',

   int varCoeffIDflow; // int DEFAULT NULL, --COMMENT 'Код группы коэффициентов вариации параметров по подающей трубе',
   int varCoeffIDret; // int DEFAULT NULL, --COMMENT 'Код группы коэффициентов вариации параметров по обратной трубе',
   int tubingTypeID; //  int DEFAULT 1, --COMMENT 'Вид прокладки',



};


extern UT_DEF ut_def;
extern PT_DEF pt_def;


class CGidrView;
class CGraph2;

struct RaiEx {
  int nomer_rn;
  CString name_rn;
  int nomer_sys;
  int manager;
  CString phone_controller;
  int pr_videlen;
};

struct MagAlma {
  CString kod;
};

struct MsAlma {
  CString kod;
};


struct RsAlma {
  CString kod;
};

struct Fragments {
  long m_id; // int(11) NOT NULL AUTO_INCREMENT,
  CString m_name; // varchar(50) DEFAULT NULL,
  CString m_name_sys; // varchar(50) DEFAULT NULL COMMENT 'Наименование системы теплоснабжения',
  CString m_nasel_point; // varchar(50) DEFAULT NULL COMMENT 'Населенный пункт (для определения многолетних климатических данных), ссылка тбл. 13_Климатические данные, Справочная',
  CString m_sezon; // varchar(50) DEFAULT NULL COMMENT 'Признак отопительного периода: З-зимний, Л-летний',
  CString m_year; // varchar(50) DEFAULT NULL COMMENT 'Календарные года отопительного периода',
  double m_t_or; // double DEFAULT -32 COMMENT 'Расчетная температура наружного воздуха для отопления отопительного сезона, Град.С , ссылка тбл. 13_Климатические данные, Справочная',
  double m_t_vr; // double DEFAULT -32 COMMENT 'Расчетная температура наружного воздуха для вентиляции, ссылка тбл. 13_Климатические данные, Справочная',
  double m_t_vnew; // double DEFAULT 8 COMMENT 'Температура наружного воздуха конца(начала) отопительного сезона, Град.С',
  double m_tx; // double DEFAULT 5 COMMENT 'Расчетная температура подпитки холодной воды, отопительный период Град.С',
  double m_tx_leto; // double DEFAULT 15 COMMENT 'Расчетная температура подпитки холодной воды, летний период Град.С',
  CString m_tip_Qgvs; // varchar(50) DEFAULT NULL COMMENT 'Задаваемое значение нагрузки горячего водоснабжения в системах горячего водоснабжения тепловых пунктах системы теплоснабжения',
//  `begin_year` DATE DEFAULT NULL COMMENT 'Дата начала отопительного периода',
//  `end_year` DATE DEFAULT NULL COMMENT 'Дата окончания отопительного периода',
  double m_tn_god; // double DEFAULT 0 COMMENT 'Среднесезонная температура наружного воздуха в отопительный период',
  double m_tg_god; // double DEFAULT 0 COMMENT 'Среднесезонная температура грунта в отопительный период',
  double m_tn_god_leto; // double DEFAULT 0 COMMENT 'Среднесезонная температура наружного воздуха в летний период',
  double m_tg_god_leto; // double DEFAULT 0 COMMENT 'Среднесезонная температура грунта в летний период',
  double m_a; // double DEFAULT 0.25 COMMENT 'Нормативное удельное значение потерь сетевой воды с утечкой из ТС',
//  `year_audit` DATE DEFAULT NULL COMMENT 'Отопительный сезон аудитной проверки',
  double m_tvn_1; // double DEFAULT 8 COMMENT 'Температура наружного воздуха конца(начала) отопительного сезона (1-ая характерная точка), Град.С',
  double m_tvn_2; // double DEFAULT 0 COMMENT 'Температура наружного воздуха нижней срезки (излома) графика (2-ая характерная точка), Град.С',
  double m_tvn_3; // double DEFAULT -15 COMMENT 'Промежуточная температура наружного воздуха между нижней и верхней срезками графика (3-ая характерная точка), Град.С',
  double m_tvn_4; // double DEFAULT -32 COMMENT 'Температура наружного воздуха верхней срезки (излома) графика (4-ая характерная точка), Град.С',
  double m_tvn_5; // double DEFAULT -32 COMMENT 'Расчетная температура наружного воздуха для отопления (5-ая характерная точка), Град.С , ссылка тбл. 13_Климатические данные, Справочная',


  CString m_phone_manager; // varchar(50) DEFAULT NULL COMMENT 'Телефон/факс руководителя: Ссылка на тблТелфонСправочн',
};



/*

struct RasCx {
  char kod_rs[20];
  char name_rs[256];
  int pc_object;
  int nomer_rn;
  int uchastok;
  char kod_ist[50];
  int master;
  int owner;
  char ispoln[256];
  CGidFile *file;
};
*/

struct RasCx {
  long id;
  CString name;

  long fileID;
  long belongMagistral; // Принадлежность магистрали
  long objectID; // Объект РС, ссылка на Вспомогательную таблицу B10_Виды объектов расчетной схемы',
  long heatSourceID; // Код источника тепла , ссылка на Источник
};



struct NAGR
{
  double d_otopl_zco;
  double d_otopl_nco;
  double d_ventil_cv;
  double d_otopl_pr;
  double d_gv_pr;
  double d_rez_pr;
  double d_otopl_sm;
  double d_gv_sm;
  double d_rez_sm;
  double d_otopl_ps;
  double d_gv_ps;
  double d_rez_ps;
  double d_otopl_pw;
  double d_gv_pw;
  double d_rez_pw;
  double d_gv_op;
  double d_gv_oo;
  double d_rez_op;


  double d_otoplz;
  double d_otopln;
  double d_otn_fs;
  double d_otopl_tp;
  double d_ventil;
  double d_dolja_vent;
  double d_kondiz;
  double d_txz;
  double d_txop;
  double d_txoo;
  double d_gvop;
  double d_gvoo;
  double d_gvpr;
  double d_gvsm;
  double d_gvps;
  double d_gvpw;
};

enum { 
  OP_DEL_NODE = 1,
  OP_DEL_LINE,
  OP_SWAP_LINE,
  OP_SET_XY,
  OP_NODE_RENAME,
  OP_LINE_RENAME,
  OP_ADD_NODE,
  OP_ADD_LINE,
  OP_EDIT_DATA,
  OP_EDIT_GR,
  OP_BEGIN,
  OP_END1,
  OP_DEL_GEO,
  OP_CHANGE_GEO
};

struct COper {
  int oper;
  int typ;
  void *data;
};

struct UCH2F
{
  int typ;
  TCHAR *table;
};

enum { 
    TIP_TEPLO,
    TIP_VODA,
    TIP_KANAL
};



struct NODEF
{
  int typ_of_program;
  int tr1, tr2;
  int typ;
  TCHAR *code;
  TCHAR *table;
  TCHAR *tableMySQL;
  TCHAR *table1;
  TCHAR *opis;
  TCHAR *out;
};

struct LINEF
{
  int typ_of_program;
  int tr1, tr2;
  int typ;
  TCHAR *code;
  TCHAR *table;
  TCHAR *tableMySQL;
  TCHAR *table1;
  TCHAR *opis;
  TCHAR *out;
  TCHAR *otkr;
  signed char n_sost;
  signed char n_diam;
  signed char n_dlina;
};

struct LINEFOUT
{
  char *q;
  TCHAR *table;
  int typ;
};

struct menuitem {
  char *name;
  char *kode;
  char *query;
  int node_line;
};

struct CText
{
  int n;
  long fileID;
  long internalNodeID;
  CString text;
  LOGFONT lf;
  CFPoint coord;

  bool operator==(const CText& mc) const 
    { return coord == mc.coord;};
  bool operator<(const CText& mc) const 
    { return coord < mc.coord;};

};


struct CGidFile
{
  CGidFile() {
    m_rect = CFRect(LONG_MAX, LONG_MAX, -LONG_MAX, -LONG_MAX);
    name = "";
    m_schema = "";
    id = 0;
  }

  long id;
  CString name;
  CString m_schema;
  CFRect m_rect;
  list<RasCx> m_list_rs;
};

struct CDelLine 
{
  CDelLine()
  {
    n1 = NULL;
    n2 = NULL;
  }

  int typ;
  
  CNode2 *n1;
  CNode2 *n2;

  int nomP;
  int nomO;

  map<CString, CString> map_varP;
  map<CString, CString> map_varO;
  CCoordList cl;
};

struct CMoveLine
{
  const CLINE2 *l;
  CCoordList cl;
};

struct CMoveNode 
{
  CNode2 *n;
  CFPoint pt;
};




extern menuitem menudoc[];
extern menuitem menudocres[];



#define ST_BP "Байпас"
#define ST_CT "Система теплоснабжения"
#define ST_DR "Диафрагма"
#define ST_EL "Элеватор"
#define ST_HC "Насосный агрегат"
#define ST_HS "Насосная станция"
#define ST_C3 "Клапан трехходовой"
#define ST_IS "Источник тепла"
#define ST_KL "Канал"
#define ST_KM "Компенсатор"
#define ST_KU "Калориферная установка"
#define ST_KV "Коэффициенты вариации"
#define ST_MT "Манометр"
#define ST_OK "Обратный клапан"
#define ST_OP "Опора"
#define ST_OR "Организация"
#define ST_PC "Расчетная схема"
#define ST_PO "Потребитель обобщенный"
#define ST_RO "Радиаторы системы отопления"
#define ST_PR "Потребитель реальный"
#define ST_RD "Регулятор давления"
#define ST_RP "Регулятор перепада давления"
#define ST_RN "Район эксплуатации"
#define ST_RR "Регулятор расхода"
#define ST_TE "Термометр"
#define ST_TO "Теплообменник"
#define ST_TL "Теплообменник пластинчатый"
#define ST_TT "Теплообменник трубчатый"
#define ST_UP "Узел подпитки"
#define ST_UR "Удельные расходы"
#define ST_US "Узел"
#define ST_US2 "Узел присоединения"
#define ST_UT "Участок теплопровода"
#define ST_UT_KAP1 "Капремонт Участок теплопровода исходный"
#define ST_UT_KAP2 "Капремонт Участок теплопровода после ремонта"

#define ST_UT_OPR1 "Опрессовка Участок теплопровода исходный"
#define ST_UT_OPR2 "Опрессовка Участок теплопровода после ремонта"


#define ST_V1 "Местные сопротивления участка трубопровода"
#define ST_VP "Водоразборный прибор"
#define ST_ZD "Задвижка"
#define ST_ZD2 "Регулирующая арматура"
#define ST_ZE "Здание"
#define ST_ZN "Узел с заданным напором"
#define ST_ORP "Объекты с измеряемыми параметрами"
#define ST_PRIBOR "Прибор учета тепловой энергии"
#define ST_ZP "ЦТП"

//#define STR_REMONT "Журнал_Ремонт_трубы_камеры"
#define STR_REMONT_DEFECT "Дефект"
#define STR_REMONT_DEFECT2 "Дефект2"
#define STR_REMONT_SHURF "Шурф"
#define STR_REMONT_SHURF2 "Шурф2"
#define STR_REMONT_OSMOTR "Осмотр"
#define STR_REMONT_KORROZ "Индикатор_коррозии"
//#define STR_REMONT_KORROZ "corrosionIndicators"

#define STR_REMONT_ISPYT  "Испытания"
#define STR_REMONT_KAPITAL "Капремонт_участки_ремонта"
#define STR_REMONT_CONTROL_TU "Контроль технического состояния"
//#define STR_REMONT_OPRESSOVKA "Опрессовка_участки_ремонта"

//#define STR_PASPORT_OBJ "Паспорт объекта теплоснабжения"
#define STR_PASPORT_OBJ "passports"

#define STR_TRUB_REKONSTR "Трубы реконструируемые"



extern UCH2F uch2f[];
extern NODEF nodef[];
extern LINEF linef[];
extern LINEFOUT linefout[];
extern int nodefsize;
extern int linefsize;
extern int linefoutsize;
extern int uch2fsize;


class CExcel;

class CAdoFile;

class CCxema
{
public:
  CCxema();
  virtual ~CCxema();

  bool openGid(CAdoFile *ado, const char *par, const char *user_gid);
  bool openOut(CAdoFile *ado, const char *par, const char *user_gid);
  bool openOut2(CAdoFile *ado, CGraph2 *graph, const char *par2);
  bool openCALCULATION(CAdoFile *ado, const char *par);
  bool openIst(CAdoFile *ado);
  bool openFragments(CAdoFile *ado);
  bool open_heatSystem(CAdoFile *ado);

  bool initGidFiles(CAdoFile *ado, set<int> &v);
  bool initGidFiles0(CAdoFile *ado, set<int> &v);
  bool openLine(CAdoFile *ado, const char *tn, const char *otkr, int typ, const char *par, bool dubl);

  bool openTexts(CAdoFile *ado, const char *tn, const char *par);

  bool openNode(CAdoFile *ado, const char *tn, int typ, const char *par);
  bool openNodeFile(CAdoFile *ado, const char *tn, int typ, const char *par);

  bool openNodesNew(CAdoFile *ado, const char *par, const char* par2, Percent2 *pc, const char *user_gid);
  bool openLinesNew(CAdoFile *ado, const char *par, const char* par2, Percent2 *pc, const char *user_gid);

  bool openLineOut(CAdoFile *ado, CGraph2 *graph, const char *tn, const char *par);
  bool openLineTypOut(CAdoFile *ado, CGraph2 *graph, const char *tn, const char *fn, int typ, const char *par);
  bool openNodeOut(CAdoFile *ado, CGraph2 *graph, const char *tn, const char *par);
  bool openPtOut(CAdoFile *ado, CGraph2 *graph, const char *tn, const char *par);
  bool openDrOut(CAdoFile *ado, CGraph2 *graph, const char *tn, const char *par);
  bool openZN(CAdoFile *ado, const char *tn);
  bool openUP(CAdoFile *ado, const char *tn);
  bool openVP(CAdoFile *ado, const char *tn);

  bool openCxemaODBC(int type_of_net, const char *server, int port, const char *database, const char *user, const char* user_gid, const char *password);
  bool openCxema0(CGidFile *gid_file, CNode *parent);
  bool reopenCxema(CGidFile *gid_file, CNode *parent);
  bool initGidTree();
  bool openPS(CGidFile *gid_file, int typ);
  bool openPS(CGidFile *gid_file);
  bool openMAG_RS(CGidFile *gid_file);
  bool clearOut(CGidFile *gid_file);
  bool openZN(CGidFile *fid_file, const CNode *parent);
  bool openNode(CGidFile *gid_file, const char *tn, int i, const CNode *parent);
  bool openLine(CGidFile *gid_file, const char *tn, int i, const CNode *parent);
  bool initCxema(CGidFile *gid_file, const CNode *parent);
  bool initC3(CGidFile *gid_file, const CNode *parent);
//  bool close();
//  bool close_main();
  bool exportMSSQL(const char *fn);
  
  bool exportAlma(const char *fn);
  bool exportAlma(CGidFile *file, const char *fn1, const char *fn2);

  
  bool exportCxe(const char *fn);
  bool importCxe(const char *fn);


  bool exportDxf(const char *fn, int m_isGeobaza);
  bool uch_pod_dxf(FILE *dxf, double dx1, double dy1, double dx2, double dy2, CLine *l);
  bool exportShape(const char *fn);

  bool chName(CGidFile *file, const char *fn);
  bool setValue(CGidFile *gid_file, int typ, const char *fn, const char *val, bool is_line = false);
  bool setValue(CGidFile *gid_file, int typ, const char *fn, int n, bool is_line = false);

//  bool setMark(const CCoordList &NP, bool val);
//  bool isMark();

  CString get_new_name(CGidFile *gid_file, const char *kod);

  CNode *addNode(int typ, CGidFile *gid_file, const CNodeName & nodename, CFPoint pt);
  CNode *addNode(CFPoint pt);
  bool addText(CFPoint pt, const char *txt, int internalNodeID, int fileID);
  bool delText(const CText *txt);
  bool addLine(CLINE2 *line, int typ, double diametr, const char *fn1 = NULL, const char *val1 = NULL, const char *fn2 = NULL, const char *val2 = NULL, const char *fn3 = NULL, const char *val3 = NULL, const char *fn4 = NULL, const char *val4 = NULL, const char *fn5 = NULL, const char *val5 = NULL);
  bool addLine(CNode *n1, CNode *n2, int typ, double diametr, const char *fn1 = NULL, const char *val1 = NULL, const char *fn2 = NULL, const char *val2 = NULL, const char *fn3 = NULL, const char *val3 = NULL, const char *fn4 = NULL, const char *val4 = NULL, const char *fn5 = NULL, const char *val5 = NULL);
  CLINE2 *addLine(CNode *n1, CNode *n2, const CCoordList &NP);
  
  bool check_node(const CNode *n1, const CNode *n2);

  bool uniteLine(CLINE2 *line);
  bool uniteLine2(CLINE2 *line);

  bool moveLineEnd(CNode *m_n1, CNode *node, CLINE2 *m_line);

  bool insertNode(CFPoint pt, CLine *l);

  bool delNode(CNode *n);
  bool delNode2(CNode *n);
  bool delLine(CLINE2 *line);
  bool delCxema(const CNode *node);
  bool delZN(const CNode *node);

  bool renameParent(const CNodeName &n1, const CNodeName &n2);

  int editLine(CWnd *wnd, CLINE2 *l, const char *fn);
  int editNode(CWnd *wnd, CNode *n, const char *fn);
  int editLineIgnat(CWnd *wnd, CLINE2 *l);
  int editLineIgnat1(CWnd *wnd, CLINE2 *l, int kk, char po);
  int editLineIgnat2(CWnd *wnd, CLINE2 *l, int kk1, int kk2);
  int editNodeIgnat(CWnd *wnd, CNode *n);

  int updateLineIgnat1(CWnd *wnd, CLINE2 *ll, const char *fn, int kk, char po);

  bool ch_name(CNode *n, const CNodeName &n1, const CNodeName &n2);
  bool setLength(CLINE2 *ll, bool ismap, bool ask);

  bool swapLine(const CLINE2 *line);

//  bool setXY(CNode2 *n, CFPoint pt);
  bool setXY1(CNode2 *n, CFPoint pt);
  bool setCoord(const CLINE2 *l, const CCoordList & cl);
  bool setCoord1(const CLINE2 *l, const CCoordList & cl);
  bool setZN(CNode2 *n);
  bool setORP(CNode2 *n);
  bool setPRIBOR(CNode2 *n);
  bool setUP(CNode2 *n);
  bool delZN(CNode2 *n);
  bool delORP(CNode2 *n);
  bool delPRIBOR(CNode2 *n);
  bool delUP(CNode2 *n);

//  bool rename(CNode2 *node);
//  bool rename(CLine2 *line);
//  bool rename(CNode2 *node, int m_Index);

  bool split(CLine2 *l);
  bool split(CNode2 *n);

  bool saveCxema();

  bool zima_leto(int fileID, bool zima);

  bool isGeobaza();

  bool vnutr(const CCoordList &NP);

  bool addFile(const char *fn);
  void ExportSHAPE(const char *fn, const char *tn, int typUS, int loc, const char *KOD);

  void PrPo();

  bool createTAG(const CTime &t, int cur);

  CString getMAG(const char *mag);
  CString getRS(const char *rs);
  long getIST(long);

  void operat_begin(int typ, void *p1 = NULL, void *p2 = NULL);
  void operat(int typ, void *p1 = NULL, void *p2 = NULL);
  void undo();

  bool TrioMove(CFPoint *m_trio_point);
  bool TrioMove2(CGidFile *gid_file, CFPoint *m_trio_point);

  bool isShow(const char *kod_rs);

  bool objed(CLine *l);

  BOOL isReadOnly();

  void ExcelPoteri();
  void Excel2(const char *path, const char *fn);
  void Excel2(CExcel &ex, const char *path, const char *fn);
  void Excel2List(const char *path, const char *fn);
  int isExcel2List(const char *path, const char *fn);
  void ExcelQ(CAdoFile *ado, CExcel &ex, const char *strQ, const char *xn, int nline, int ntab, const char *fn, list<CDop> *list_dop);
  void Excel2Pot(CExcel &ex, const char *path, const char *fn, const char *kod_ist);

  void create_vyd_table(CAdoFile *ado);
  void create_vyd_table2(CAdoFile *ado);

  CNode *findIST_Node(const char *name);

  CString getParAll();


//  CGidFile *getGidFile(const char *name);
  CGidFile *getGidFile(long id);

  CNode2 *findAll(const char *kod, const char *name);

  CGraph2 *m_graph;

  CFRect m_rect;
  double m_diametr;
  double m_diametr_usl;
  double m_diametr_vn;
  double m_tol;
  double m_average_len;
  CString m_name_typ;


  double m_Tn;
  CString m_out_name;
  COleDateTime m_out_date;


  double m_Tn_ot;  // Расчетная температура наружного воздуха для отопления

  CString m_city;

  map<int, int> m_map_fileID_mag;


  CString m_par;  // Список фрагментов в текстовом виде

  BOOL isAdminFile;

  const Fragments * getFragments(int id);
  const RasCx * getPC(int kod_rs);
  const RaiEx * getRE(int nomer_rn);

  int NodeChange(CGidFile *file, CNode *n, const char *fn, const char *kod, const char *name, char *p);

//  CNode *m_kam_parent;
//  CNodeName m_node_name;
  set<CText> set_text;
  map <CString, CString> map_rayon;
  map <CString, CString> map_mag;
  map <CString, CString> map_raspr;
  map <long, long> map_ist;
  map<long, RasCx> map_rs;


  map <CString, int> map_pc_show;
  map <CString, CNode2*> map_ist2;
  set<CString> set_ist;

  map<CString, MagAlma> map_MAG_ALMA;
  map<CString, MsAlma> map_MS_ALMA;
  map<CString, RsAlma> map_RS_ALMA;
  map<CString, RasCx> map_PC;
  map<int, RaiEx> map_RE;
  map<int, Fragments> map_fragments;

  map <int, int > map_init_UR;
  map <int, int > map_init_KV;
  map <int, int > map_init_TR;

  map <int, CString> map_UR;
  map <int, CString> map_KV;
  map <int, CString> map_TR;
  map <int, CString> map_TP;


  double init_diameterInternal;
  double init_diameterCondit;
  double init_diameterExternal;
  double init_wallThickness;



//  map<int, CString> map_kod0;
//  map<CString, int> map_kod0_1;

//  set<CCxema *> m_cxema_list;

//  set<CGidFile *> m_file_list;
  map<long, CGidFile *> m_file_map;

  BOOL m_bIsGidrInf;
  BOOL m_bIsInfo[20];

  BOOL is_google_map;
  int m_n_undo;
  int m_typop;

  int m_is_kod0;
  int m_AstanaMag;
  set<int> m_AstanaMagSet;

  CGidFile *m_cur_file;
  
//  CString getName() {
//    return m_strFileName;
//  }
  CDelLine del_line;
  CMoveLine move_line;
  CMoveNode move_node;

  CAdoFile *m_ado;

private:

  int m_n_max;
//  CString m_strFileName;
  double m_coef0;
  double m_coef;
};

CString getLineTable(int typ);
CString getLineCode(int typ);
CString getLineTableMySQL(int typ);
CString getLineTableOutMySQL(int typ);

CString getNodeTable(int typ);
CString getNodeCode(int typ);
CString getNodeTableMySQL(int typ);
CString getNodeTableOutMySQL(int typ);


int getLineTypOut(const char *tn);
int getNodeTypOut(const char *tn);




CString getLineOtkr(int typ);

bool create_vyd_line_table(CAdoFile *ado, CGraph2 *graph, CString &vyd);
bool create_vyd_node_table(CAdoFile *ado, CGraph2 *graph, CString &vyd);

bool create_vyd_line_tuple(CAdoFile *ado, CGraph2 *graph, CString &tuple);
bool create_vyd_node_tuple(CAdoFile *ado, CGraph2 *graph, CString &tuple);


CString getKodName(map<int, CString> & map_str, int id);


#endif // !defined(AFX_CXEMA_H__016F4120_D166_4723_B143_B5E9B1C3DB56__INCLUDED_)

