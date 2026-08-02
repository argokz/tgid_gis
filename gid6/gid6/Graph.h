#pragma once

#include "coordlis.h"

using namespace std;

//#define CLINE2 void

typedef void CLINE2;

#define CPOD _T('П')
#define COBR _T('О')
#define POD _T("П")
#define OBR _T("О")


struct CGidFile;
class CCxema;

enum NODE_TYP2 {
  TIP2_US = 0,
  TIP2_TK,
  TIP2_PV
};


enum NODE_TYP {
  TIP_US = 0,
//  TIP_US_P,
//  TIP_US_O,
  TIP_PR,
  TIP_PO,
  TIP_IS,
  TIP_HS,

  TIP_C3,

//  TIP_ZP,
  TIP_US2,
  
// Водопровод

  TIP_V_PT,  // Потребитель водопровода
  TIP_V_IS,  // Источник водоснабжения




// Остальное

  TIP_UP,   /*     Узлы подпитки и прис.баков-аккумуляторов      - код УП"  */
  TIP_PODP,
  TIP_VODOR,
};

enum NODE_TYP3 {
    TIP3_ZN = 60,
    TIP3_ORP,
};




enum LINE_TYP {
  TIP_UT = 0, /*     Участки теплопроводов расчетной схемы         - код УТ"  */
  TIP_HC, /*     Насосы(насосные станции) расчетной схемы      - код НС"  */
  TIP_RD, /*     Сетевые регуляторы давления                   - код РД"  */
  TIP_RR, /*     Сетевые регуляторы расхода                    - код РР"  */
  TIP_ZD, /*     Задвижки на теплопроводах                     - код БП"  */
  TIP_ZD2, /*     Задвижки на теплопроводах                     - код БП"  */
  TIP_BP, /*     Байпасы наружных теплопроводов                - код БП"  */
//  TIP_DR, /*     Тепловой пункт                                           */
  TIP_DR,

  TIP_EL,
  TIP_RO,
  TIP_TO,
  TIP_KU,

  TIP_RP,  // Вставил только что

  TIP_OK,  // Переставил с KM

//  TIP_KM,
//  TIP_TL,
//  TIP_TT,
//  TIP_DRO,

// Водопровод

  TIP_V_UT, /*     Участки водопровода расчетной схемы         - код УТ"  */

// Остальное

};


class CGraph2;
class CLine;
class CNode;
class CNode2;

//typedef set<CNode> CMapGraph;

extern int nn;

void trim(TCHAR *s);



class CNodeName
{
public:
  CNodeName()
  {
    kod[0] = name[0] = 0;
    p = ' ';
  }
  CNodeName(const char *_kod, const char *_name, const char _p)
  {
    lstrcpy(kod, _kod);  trim(kod);
    lstrcpy(name, _name); trim(name);
    p = _p;
  }

  CNodeName(const char *_kod, const char *_name, const char *_p)
  {
    lstrcpy(kod, _kod);  trim(kod);
    lstrcpy(name, _name); trim(name);
    if (_p[0]) p = _p[0];
    else p = ' ';
  }

  CString Name() const
  {
    CString str;
//    str.Format("%*s %*s", -N3, kod, -N10, name);
    str.Format("%s %s", kod, name);
    return str;
  }
  
  bool operator==(const CNodeName& mc) const;
  bool operator!=(const CNodeName& mc) const { return !(*this == mc); };
  bool operator<(const CNodeName& mc) const;

  char kod[N3+1], name[N10+1], p;

private:
};

inline bool CNodeName::operator<(const CNodeName& mc) const
{
  int cmp;

  if ((cmp = strcmp(kod,mc.kod)) < 0) return true;
  if (cmp == 0) {
    if ((cmp = strcmp(name,mc.name)) < 0) return true;
    if (cmp == 0) {
      if (p < mc.p) return true;
    }
  }
  return false;
}

inline bool CNodeName::operator==(const CNodeName& mc) const
{
  return !strcmp(kod,mc.kod) && !strcmp(name,mc.name) && p == mc.p;
}

struct CNodeData
{
  CNodeData() : 
    tempP(0.), tempO(0.),
    pihP(0.), pihO(0.)
    {};
  double tempP, tempO;
  double pihP, pihO;

};


class CNode
{
friend CGraph2;
public:

  CNode() :
    fileID(0),
    calculationID(0),
    id_kod(-1),
    name(""),
      n(nn++), nomP(-1), nomO(-1),
    n_sort(0),
    externalSignID(1),
    nomgP(-1), nomgO(-1), 
    nomPT(-1),
    nomDR(-1),
    master(-1),
    kvartal(-1),
    Color(0),
    colorP(0xFFFFFFFF),
    namePS(""),
    name_typ_alma(1),
    _typPS(-1),

    idPS(0),
//    typPS(-1),
    isP(false), isPjezo(false), isPjezoSave(false), isHide(false), isValid(true), isValid_f(true), isPodp(true), isZakr(false),
//    pihP(0), pihO(0), tempP(0), tempO(0), 
    qz(0), qP(0), qO(0), pP_fact(0), pO_fact(0), Qot(0), Qgvs(0), Qvent(0),
    pP_old(0), pO_old(0),
    nZN(-1), nUP(-1), nVP(-1), nORP(-1), nPRIBOR(-1),
    strOpc1(""), strOpc2(""), strOpc3(""),
    aOpc1(1.), aOpc2(1.), aOpc3(1.),

    geoMarkTopTube(0),
    geoMarkNodeArea(0),
    hz(0.),

    pr_avtomat(' '),
    pribor_ucheta(0),
    m_graph(NULL),
    schemeNum(""),
    isOtkl(false),
    isMarkAnalyze(false)
    {
//      schemeNum[0] = 0;
    };
  
  
  CNode(CGidFile *_file, const CNodeName &p) : 
    file222(_file),
//    fileID(_file ? _file->id : 0),
    fileID(0),
    calculationID(0),
    id_kod(-1),
    name(""),
    node_name(p), 
//    lines(NULL), 
      n(nn++), nomP(-1), nomO(-1),
      connectID(0),
    n_sort(0),
    externalSignID(1),
    nomgP(-1), nomgO(-1), 
    nomPT(-1),
    nomDR(-1),
    master(-1),
    kvartal(-1),
    Color(0),
    colorP(0xFFFFFFFF),
    namePS(""),
    name_typ_alma(1),
    internalNodeID(0),
    _typPS(-1),

    id2(0),
    idPS(0),
//    typPS(-1),



    isP(false), isPjezo(false), isPjezoSave(false), isHide(false), isValid(true), isValid_f(true), isPodp(true), isZakr(false),
//    pihP(0), pihO(0), tempP(0), tempO(0), 
    qz(0), qP(0), qO(0), pP_fact(0), pO_fact(0), Qot(0), Qgvs(0), Qvent(0),
    pP_old(0), pO_old(0),
    nZN(-1), nUP(-1), nVP(-1), nORP(-1), nPRIBOR(-1),
    strOpc1(""), strOpc2(""), strOpc3(""),
    aOpc1(1.), aOpc2(1.), aOpc3(1.),

    geoMarkTopTube(0),
    geoMarkNodeArea(0),
    hz(0.),
    
    pr_avtomat(' '),
    m_graph(NULL),
    schemeNum(""),
    isOtkl(false),
    isMarkAnalyze(false)
    {
//      schemeNum[0] = 0;
    };


  int n;

  int connectID;

  int fileID;
  int calculationID;

  int id_kod;
  CString name;

  int n_sort;
  int Color; // для сортировки
//  int mag; // Магистраль из PS

  int len;
  int len2;
  CFPoint coord;
  CGidFile *file222;
  CNodeName node_name;
  CNodeData data;
  CString namePS;
  int idPS;
//  int typPS;
  int typ;

  bool isP, isPjezo, isPjezoSave, isHide, isValid, isValid_f, isPodp;
  bool isEl, isNez, isZakr;
  bool isOtkl;
  bool isMarkAnalyze;

  int _typPS;
  int nZN, nUP, nVP, nORP, nPRIBOR;
  int id2;
  int nomP, nomO;
  int nomgP, nomgO;
  int nomPT;
  int nomDR;
  int master;
  int kvartal;
  int internalNodeID;

  int externalSignID;

  double geoMarkTopTube;
  double geoMarkNodeArea;
  double hz;
  
  //  double pihP, pihO, tempP, tempO, 
  double qz, qP, qO, pP_fact, pO_fact, pP_old, pO_old;
  CString strOpc1, strOpc2, strOpc3;
  double aOpc1, aOpc2, aOpc3;
  double Qot, Qgvs, Qvent;
  unsigned long colorP;
  char name_typ_alma;
  char pr_avtomat;
  int pribor_ucheta;
//  char schemeNum[10];
  CString schemeNum;

  bool operator==(const CNode& mc) const 
    { 
      return fileID == mc.fileID && node_name == mc.node_name;
    };
  bool operator<(const CNode& mc) const 
    {
      if (fileID < mc.fileID) return true;
      if (fileID > mc.fileID) return false;
      return node_name < mc.node_name;
    };

//private:
//  CLINE2 *lines;
  CGraph2 *m_graph;

  bool isVyd(int m_mag, int m_ms, int m_rs);
  bool isVyd2(int m_mag, int m_ms, int m_rs, int isP);
};

struct UTNapr
{
  UTNapr() :
  isOtkl(false),
  sost(' '),
  q(0),
  dl(0),
  diam(0),
  diam_usl(0),
  diam_vn(0),
  tol(0),
  poter(0),
  a14(0),   //  Удельные линейные потери напора на участке      
  a15(0),   //  Линейные потери напора на участке               
  a16(0),   //  Местные потери напора на участке                
  a17(0),   //  Общие потери напора на участке                  
  time1(0),
  dru_home(0),
  v(0),  //  Объем
  w(0),  // Скорость потока сетевой воды
  tpot(0),
//  to(0) 
  tzam(0), // Температура замерзания, временно q ДЛЯ МИШИ

  ql(0),     //  Расчетная тепловая нагрузка, Гкал/ч               
  ql_ot(0),  //  Расчетная тепловая нагрузка на отопление, Гкал/ч  
  ql_v(0),   //  Расчетная тепловая нагрузка на вентиляцию, Гкал/ч 
  ql_gv(0)   //  Расчетная тепловая нагрузка на ГВС, Гкал/ч        




  {};
    
  void set_zero()
  {
     q = 0;
//     dl = 0;
//     diam = 0;
//     diam_usl = 0;
//     diam_vn = 0;
//     tol = 0;
     poter = 0;
     a14 = 0;   //  Удельные линейные потери напора на участке      
     a15 = 0;   //  Линейные потери напора на участке               
     a16 = 0;   //  Местные потери напора на участке                
     a17 = 0;   //  Общие потери напора на участке                  
     time1 = 0;
     dru_home = 0;
//     v = 0;  //  Объем
//     w = 0;  // Скорость потока сетевой воды
     w = 0;
     tpot = 0;
//     to = 0;
     tzam = 0;
  }

  bool operator<(const UTNapr& mc) const
  {
    if (diam_usl < mc.diam_usl) return true;
    if (diam_usl > mc.diam_usl) return false;
    if (tol < mc.tol) return true;
    if (tol > mc.tol) return false;

    return false;
  }

  bool isOtkl = false;     // Отключено 
  bool isOtklZD = false;   // Сюда сохраняется отключение от задвижки
  char sost;
  double q;
  double dl;
  double diam;
  double diam_usl;
  double diam_vn;
  double tol;
  double poter;
  double a14;      //  Удельные линейные потери напора на участке      
  double a15;      //  Линейные потери напора на участке               
  double a16;      //  Местные потери напора на участке                
  double a17;      //  Общие потери напора на участке                  
  double time1;
  double v;
  double w;
  double tpot;
  double tzam;
  double dru_home;
  double ql;
  double ql_ot;
  double ql_v;
  double ql_gv;
};


class CLine
{         
friend CGraph2;

//friend inline CLine *bline0(const CLINE2 *l);
//friend inline CNode *&where0(const CLINE2 *l);
//friend inline CNode *&other0(const CLINE2 *l);
friend inline CLINE2 *&next(const CLINE2 *l);
friend inline bool IsBegin(const CLINE2 *l);
friend inline int &nom_nodegP(const CLINE2 *l);
friend inline int &nom_nodegO(const CLINE2 *l);
friend inline int &nom_ZD(const CLINE2 *l);
friend inline int napr(const CLINE2 *l);

friend inline double &pihP(const CLINE2 *l);
friend inline double &pihO(const CLINE2 *l);


public:
  CLine() : 
    nomP(-1), nomO(-1), 
    idP2(-1), idO2(-1), 
    nomgP(-1), nomgO(-1), 
    n_sort(0),
    n_sort2(0),
    isPodp(true),
    typ(-1),
    tubingTypeID(1),
    pipeSectionID(0),
//    name_typ('К'),
//    isMag(false),
    colorP(0xFFFFFFFF), colorO(0xFFFFFFFF),

//    kod0(""),
    m_opressovka(-1),

    strOpc1(""), strOpc2(""),
    aOpc1(1.), aOpc2(1.),
    mag(0), ms(0), rs(0), owner(0),

      nZN(NULL),

     m_raj(-1),
     m_uchM(-1),
     m_uchR(-1),
     m_vid(-1),

      isOsmotr(false),
    isHide(false),
    isPjezo(false),
    isPjezo2(false),
    isPjezoSave(false)

    {
      _nom_nodegP[0] = _nom_nodegO[0] = _nom_nodegP[1] = _nom_nodegO[1] = -1;
      _pihP[0] = _pihO[0] = _pihP[1] = _pihO[1] = 0;
      _nom_ZD[0] = _nom_ZD[1] = 0;
       m_set[0] = 0;
    };

  bool isVyd(int m_mag, int m_ms, int m_rs);
  bool isVyd2(int m_mag, int m_ms, int m_rs, int isP);

  bool isLineOtkl() const;

  CFRect rect;
  CCoordList m_NP;
  unsigned long colorP, colorO;
  int typ;
  int nomP, nomO;
  int idP2, idO2;
  int nomgP, nomgO;
  int n_sort;
  int n_sort2;
  bool isPodp;
  bool isPjezo;
  bool isPjezo2;
  bool isPjezoSave;
  bool isHide;
  bool isOsmotr;

//  bool isMag;

  CCoordList getCLall();

//  char name_typ;

  int tubingTypeID;
  int pipeSectionID;

  char m_set[N3+1];    // Сеть
  int m_raj;           // Район эксплуатации
  int m_uchM;           // Участок эксплуатации
  int m_uchR;           // Участок эксплуатации
  int m_vid;           // Вид сети

  // направление (подача, обратка)
  UTNapr pod, obr;

  int mag, ms, rs, owner;
  int m_opressovka;

//  CString kod0;
  CString strOpc1, strOpc2;
  double aOpc1, aOpc2;
  CNode2 *nZN;

private:
  int _nom_nodegP[2], _nom_nodegO[2];
  int _nom_ZD[2];
  double _pihP[2], _pihO[2];
//  CLINE2 *_next[2];
//  CNode *_where[2];


};
/*
CLine *bline(const CLINE2 *l);
CNode *&where(const CLINE2 *l);
CNode *&other(const CLINE2 *l);
CLINE2 *next(const CLINE2 *l);
bool IsBegin(const CLINE2 *l);

inline CLine *bline0(const CLINE2 *l)
{
  unsigned int s = (unsigned int)l;
  return (CLine *)(s&0xfffffffcu);
}


inline CNode *&where0(const CLINE2 *l) 
{
  return bline0(l)->_where[((unsigned int)l)&0x1];
}

inline CNode *&other0(const CLINE2 *l) 
{
  return bline0(l)->_where[1-(unsigned int)l&0x1];
}

*/

#if 0

inline double &pihP(const CLINE2 *l) 
{
  return bline(l)->_pihP[((unsigned int)l)&0x1];
}

inline double &pihO(const CLINE2 *l) 
{
  return bline(l)->_pihO[((unsigned int)l)&0x1];
}


inline int &nom_nodegP(const CLINE2 *l) 
{
  return bline(l)->_nom_nodegP[((unsigned int)l)&0x1];
}

inline int &nom_nodegO(const CLINE2 *l) 
{
  return bline(l)->_nom_nodegO[((unsigned int)l)&0x1];
}

inline int &nom_ZD(const CLINE2 *l) 
{
  return bline(l)->_nom_ZD[((unsigned int)l)&0x1];
}


inline int napr(const CLINE2 *l)
{
  unsigned int s = (unsigned int)l;

  return (s&0x1) ? -1 : 1;
}





inline CLINE2 *&next(const CLINE2 *l) 
{
  return bline(l)->_next[(unsigned int)l&0x1];
}


inline CLINE2 *sym(const CLINE2 *l) 
{
  unsigned int s = (unsigned int)l;

  return (void*) (s^0x1);
}

bool IsBegin(const CLINE2 *l)
{
  unsigned int s = (unsigned int)l;

  return !(s&0x1);
}

#endif

CCoordList get_NP(const CLINE2 *l);
bool chline(CLine *l, double x, double y, double r);
//bool isMarked(const CNode *n);

#if 0
class CGraph2  
{
public:
  CGraph2(CCxema *cxema);
  virtual ~CGraph2();

  void clear();
  void clear(const CGidFile *gid_file);
  void clear(const set<CGidFile *> &gid_list);
  void clear_not(const set<CGidFile *> &gid_list);

  CNode * find(CCoordList const &cl);

  CNode * find_ins(const CNode &node);
  CNode * find(const CNode &node);
  CNode * find(CGidFile *file, const CNodeName &nn);
  
  CNode * findPO(const CNode &node);
  CNode * findPO(const CNode &node, char po);
  CNode * find2(const CNode &node);
  CNode * find2(CGidFile *file, const CNodeName &node);
  CNode * find2(CGidFile *file, const char *kod, const char *name);
  CNode * find(CFPoint pt, double r);

  CNode * findRS(CFPoint pt, double r);
  CNode * findRS(CNode *n);


  CNode * find(int typ, const CCoordList &cl);
  CLINE2 * findLine(int typ, const CFPoint pt, double r);

  CNode * findPS(const char *name);
  CNode * findPS1(const char *name);

  CNode * findZN(CNode *node);

  CNode * sameNode();
  CNode * sameMarkNode();
  

  CLINE2 * findLine(CFPoint pt, double r);

  CLine * insert_line(CNode *node1, CNode *node2, const CCoordList &NP = CCoordList(), bool paral = false);
  CLine * insert_line2(CNode *node1, CNode *node2, const CCoordList &NP = CCoordList(), bool paral = false);
//  CLine * insert_line(const CNode &node1, const CNode &node2);
  CLINE2 * Link(const CNode *c1, const CNode *c2);
  CLINE2 * find_line(const CNode *c1, const CNode *c2, const CCoordList &NP);
  CLINE2 * find_line(const CNode *c1, const CNode *c2, char p, int typ = -1);
  CLINE2 * find_lineG(const CNode *c1, const CNode *c2, char p, double len, double diam, int typ = -1);
  CLINE2 * find_lineG(const CNode *c1, const CNode *c2, int ng);
  CLINE2 * find_line_typ(const CNode *c1, const CNode *c2, int typ);

  CLINE2 * find_line_obr(const CLINE2 *l);

  CLINE2 * find_dubl(CLINE2 *l);
  
  void remove(CNode &node);
  void remove(CLINE2 *line);

  void swap(CLINE2 *line);

  void save_pjezo();
  void restore_pjezo();

  void ch_node(const CNode &n1, const CNode &n2);

  void reset_len();
  bool reset();
  void WS1(CNode *v, CNode *t);
  void WS1_2(CNode *v, CNode *t);
  void WS1PO(CNode *v, CNode *t, char po);
  void WS1_z(CNode *v, CNode *t);
  void WS1_z_zakr(CNode *v, CNode *t);
  void WS1_z2(CNode *v, CNode *t);
  void WG1(CNode *v, CNode *t);
  void WS2(CNode *v, CNode *t);
  void WS1_h(CNode *v, CNode *t, double dh);
  void link(CNode *v, CNode *t);
  void linkPO(CNode *v, CNode *t, char po);
  void link2(CNode *v, CNode *t);

  void link2_rs(CNode *v, CNode *t);


  bool isRS();
  void setRS();

  bool isMark();
  bool isMark(CGidFile *file);

  CFRect getBorder();
  CFRect getBorder(const CGidFile *gid_file);
  void initBorder(CGidFile *gid_file);
  int getNodeCount();

  CMapGraph set_node;
  CCxema *m_cxema;
  
  deque<CNode> list_pjezo;
  deque<CNode> list_pjezo_min;

private:

  bool m_is_rs;
  CFRect m_rect;

};

class PCLine
{
  PCLine();
private:
  CLine *line;
};
#endif



CString PSName(const CNode2 *n);
CString PodpisName(const CNode2 *n);
CString PSFullName(const CNode2 *n);
CString AnyName(const CNode2 *n);

CNode *next_p(CNode *n, CNode2 *nn);
double getLength(CLINE2 *l, CFPoint pt);
void Pjezoid(CGraph2 *graph, int m_mag, int m_ms, int m_rs);

CFPoint centroid(CLINE2 *l);
void SetCL(CLINE2 *l, const CCoordList &cl);

