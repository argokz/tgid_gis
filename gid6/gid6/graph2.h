#pragma once

#include "graph.h"
#include "coordlis.h"

using namespace std;


enum TipPS {
    PS_PAV = 7,    ///  Павильон
    PS_TRP = 8,    ///  ТРП
    PS_TK = 9,     ///  Тепловая камера
    PS_POD = 10,   ///  Подземный узел
    PS_NAD = 11,   ///  Надземный узел
    PS_VV = 12,    ///  Ввод в здание
    PS_NEUST = 13, ///  Неустановленный узел
    PS_NS = 14,    ///  Насосная станция
    PS_IS = 15,    ///  Источник тепла

    // Добавили 2023-12-02

    PS_VP = 16,    ///  Вертикальный подъем/опуск трубопровода
    PS_VC = 17,    ///  Границы вертикальных компенсаторов
    PS_OP = 18,    ///  Камера опуска/подъёма
    PS_PD = 19,    ///  Переход диаметра
    PS_TP = 20,    ///  Тепловой пункт
    PS_UP = 21,    ///  Угол поворота трубопровода
    PS_UK = 22,    ///  Узел канала
};



typedef void CLINE2;

class CCxema;

class CGraph2;

class CLine2;
class CNode2;

typedef map<long, CNode2*> CMapGraph;

int inc(const CNode2 *n);
int inc_pod(const CNode2 *n);
int inc_obr(const CNode2 *n);
int incP(const CNode2 *n);

bool chline(CLine2 *l, double x, double y, double r);
CFPoint centroid(CLINE2 *line);

bool isMarked(const CNode2 *n);


class CLine2
{
friend CGraph2;

friend inline CLine2 *bline(const CLINE2 *l);
friend inline CNode2 *&where(const CLINE2 *l);
friend inline CNode2 *&other(const CLINE2 *l);
friend inline CLINE2 *&next(const CLINE2 *l);
friend inline bool IsBegin(const CLINE2 *l);

friend inline double &pihP(const CLINE2 *l);
friend inline double &pihO(const CLINE2 *l);
friend inline int &nom_nodegP(const CLINE2 *l);
friend inline int &nom_nodegO(const CLINE2 *l);


public:
  CLine2()
  {
  }
  ;

//  int typ;

//  CFRect rect;
//  CCoordList m_NP;

  CLine line;

//  bool isHide;

  CCoordList getCLall();
  CString getTable() const;
  CString getTableMySQL() const;
  CString getTableOut() const;
  CString getTableOutMySQL() const;

  CString getName() const;
  CString getNameFull() const;
  CString getNamePT() const;
  CString getNamePTfull() const;

private:
  int _nom_nodegP[2], _nom_nodegO[2];
  double _pihP[2], _pihO[2];
  CLINE2 *_next[2];
  CNode2 *_where[2];
};


inline int napr(const CLINE2 *l)
{
  unsigned int s = (unsigned int)l;

  return (s&0x1) ? -1 : 1;
}

int is_dubl(const CLINE2 *l);


inline CLine2 *bline(const CLINE2 *l)
{
  unsigned int s = (unsigned int)l;
  return (CLine2 *)(s&0xfffffffcu);
}

inline CNode2 *&where(const CLINE2 *l) 
{
  return bline(l)->_where[((unsigned int)l)&0x1];
}

inline CLINE2 *&next(const CLINE2 *l) 
{
  return bline(l)->_next[(unsigned int)l&0x1];
}

inline CNode2 *&other(const CLINE2 *l) 
{
  return bline(l)->_where[1-(unsigned int)l&0x1];
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



class CNode2
{
public:
  CNode2() :
    lines(NULL)
    {
    };

  CNode node;

//  int id_kod;
//  CString name;

//  int fileID;
//  int calculationID;
  
//  CFPoint coord;

  CLINE2 *lines;
//  CGraph2 *m_graph;
  CGidFile *getFile();
  CString getFileName();


  CString getTable() const;
  CString getTableMySQL() const;
  CString getTableOut() const;
  CString getTableOutMySQL() const;

  CString getKod() const;
  CString getKod(long id_kod) const;
  CString getName() const;
  CString getNameFull() const;
  CString getNamePT() const;
  CString getNamePTfull() const;

  int typPS() const;

  bool isKam() const
  {
//    return typPS() != PS_NAD && typPS() != PS_POD && typPS() != PS_NEUST && typPS() > 0;
    return typPS() != PS_NEUST && typPS() > 0;
  }


//  int typ;
//  bool isHide;
  
  long id;
};



class CGraph2
{
public:
  CGraph2(CCxema *cxema);
  virtual ~CGraph2();

  void clear();

  int getKodId(const char *kod);
  void getKodIds(const char *kod, set<int> &set_id);

  CNode2 * findNodeName(const char *kod, const char *name, const char p);
  CLINE2 * findLineName(const char *kod1, const char *name1, const char p1, const char *kod2, const char *name2, const char p2);


  CNode2 * findNodeName_vn(const char *kod_p, const char *name_p, const char p_p, const char *kod, const char *name, const char p);
  CLINE2 * findLineName_vn(const char *kod_p, const char *name_p, const char p_p, const char *kod1, const char *name1, const char p1, const char *kod2, const char *name2, const char p2);

  

  CLINE2 * findLine(int parent_id, CFPoint pt, double r);
  CLINE2 * findLineNoP1P2(int parent_id, CFPoint pt, double r);

  CNode2 * find(int parent_id, CFPoint pt, double r);
  CNode2 * find(int parent_id, const CCoordList &cl);
  CNode2 * find(int parent_id, int typ, const CCoordList &cl);

  void init_find_node_nom_pt_out();
  CNode2 *find_pt_out(int id);


  void init_find_line_nom_out(int typ);
  CLINE2 * find_line_nom_out(int id);

  CLINE2 * find_line_big(int id);


  void init_find_line_nom_hps();

  void init_find_line_nom();
  CLINE2 * find_line_typ(const CNode2 *c1, const CNode2 *c2, int typ);
  CLINE2 * find_line(const CNode2 *c1, const CNode2 *c2, char p, int typ = -1);

  CLINE2 * findLine(int id);

  CLINE2 * findLineOut(const char *tn, int id);
  CNode2 * findNodeOut(const char *tn, int id);

  CLINE2 * findLine2(int typ, int id2);
  CNode2 * findNode2(int typ, int id2);


  CLINE2 * findLineOut(int typ, int id);
  CNode2 * findNodeOut(int typ, int id);
  CNode2 * findPtOut(int id);

  CNode2 * find_ins(int id);
  CNode2 * find(int id);
  CNode2 * ins(int id);

  CNode2 * findPO(int id, char po);
  CNode2 * findPO(int id);

  void save_otkl();
  void restore_otkl();


  void save_pjezo();
  void restore_pjezo();
  void set_pjezo(bool yes);

  void resetBorder();

  bool getBorderMap(int fileID, int internalNodeID, CFRect &rect);

  bool inGraphNode(int typ, int parent_id, int fileID);
  bool inGraphLine(int typ, int parent_id, int fileID);

  bool inGraphLineVyd(int typ, int parent_id);
  bool inGraphNodeVyd(int typ, int parent_id);


  CLINE2 * find_line_nom(int id);
  CLINE2 * find_line_nom_hps(int id);

  CLINE2 * Link(const CNode2 *c1, const CNode2 *c2);
  CLine2 * insert_line(CNode2 *node1, CNode2 *node2, const CCoordList &NP = CCoordList(), bool paral = false);
  CLine2 * insert_line2(CNode2 *node1, CNode2 *node2, const CCoordList &NP = CCoordList(), bool paral = false);
  CFRect getBorder(int internalNodeID);
  CFRect getBorderFile(int file);
  CFRect getBorderParent(int file);

  CNode2 * findPS1(const char *name);
  CNode2 * findPS(const char *name);

  CNode2 * sameMarkNode();

  CNode2 * find2(const CNode2& n);

  void clearFile(set<int> & v);
  void clearVnutr(int id);

  void vyd_lines(set<int> & v, bool nodes = false);
  void vyd_nodes(set<int> & v);

  void clean_out(int fileID);

  bool GetMarkRect(CFRect& rect);

  void remove_node(CNode2 &node);
  void remove_line(CLINE2 *line);

  int getNodeCount(int parent_id);

  bool set_pipeSectionID_(int id);
  bool set_lineSetID(set<int> & set_l);

  void allVisible();
  void visibleMSRS(set<int> & set_ms, set<int> & set_rs);
  bool setMark(const CCoordList &NP, bool val);
  bool isMark();

  bool reset();
  bool vyd_all();

  CLINE2 * find_line_obr(const CLINE2 *line);

  void reset_len();

  void WS1(CNode2 *v, CNode2 *t, bool zakr = false);
  void WG1(CNode2 *v, CNode2 *t, bool zakr = false);
  void WS1_h(CNode2 *v, CNode2 *t, double dh);
  void WS1_2(CNode2 *v, CNode2 *t, bool zakr = false);

  void link(CNode2 *s, CNode2 *t);
  void link2(CNode2 *s, CNode2 *t, bool zakr);

  void change() {is_changed = true;}

  void swap(CLINE2 *ll);

  void setOnOff(bool on_off);

  deque<long> list_pjezo;
  deque<long> list_pjezo_min;

//  bool isMark();

  map<long, CNode2*> map_node;
  map<long, CString> map_kod;

  CCxema *m_cxema;

private:
  CFRect m_rect;

  bool is_changed;

  map<int, CNode2*> m_map_pt_out;

  map<int, CLINE2*> m_map_line;
  map<int, CLINE2*> m_map_line_hps;
  map<int, CLINE2*> m_map_line_out;
  map<int, map<int, CFRect> > m_map_border;

};



double rasst(CFPoint p1, CFPoint p2, CFPoint p);
double rasst(CLINE2 *l, CFPoint p);
