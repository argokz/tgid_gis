// GeoFile.h: interface for the CGeoFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOFILE_H__BD7FFF15_62E2_11D4_B15B_AB099AB1734B__INCLUDED_)
#define AFX_GEOFILE_H__BD7FFF15_62E2_11D4_B15B_AB099AB1734B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)

typedef void CLINE2;


#include "Klassif.h"
#include "coordlis.h"
//#include "shp_dbf.h"

struct ScrollGeom;
class CScroll;
class CGidrView;
class CCxema;
class CGeoMenu;
class CAdoFile;
class Percent2;


struct QQQ {
  long n1, n2;
};


struct CDelObj
{
  CDelObj()
  {
  }

  Klassif *kls;
  map<CString, CString> map_var;
  CCoordList cl;
};


struct CMoveObj
{
  CMoveObj()
  {
  }

  int nom;
  Klassif *kls;
  CCoordList cl;
};


class CFieldList : public valarray<string>
{
public:
  CFieldList() : valarray<string>() {};
  CFieldList(int n) : valarray<string>(n) {};
};

class CGeoFile;

class CGeoFileIterator
{
public:
  CGeoFileIterator(const CGeoFile & gf);
public:
private:

  const CGeoFile &m_gf;
  int m_nCur;
};

class CGeoFile
{
public:
  CGeoFile();
  virtual ~CGeoFile();

//  bool GetCoord(const Klassif *kls, CCoordList& cl, CString &text, double dx = 0., double dy = 0., double coef = 1.);
  bool GetCoord(const Klassif *kls, CCoordList& cl, double dx = 0., double dy = 0., double coef = 1.);
//  bool GetCoord2(const Klassif *kls, CCoordList& cl, double dx = 0., double dy = 0., double coef = 1.);
  bool GetCoord(const Klassif *kls, int n, CCoordList& cl, double dx = 0., double dy = 0., double coef = 1.);
  
  bool GetFRect(const Klassif *kls, int n, CFRect& rect);

/**
  bool SetFRect(int n, const CFRect& rect) {
    if (n > 1) m_kls->geo4[n-1]->rect = rect;
    return true;
  };
  int IsMark(int n) { return m_kls->geo4[n-1]->mark; };
  int IsVisible(int n) { return m_kls->geo4[n-1]->is_visible; };
  bool GetFRect(CFRect& rect) { return GetFRect(m_nCur, rect);};
  bool SetFRect(const CFRect& rect) { return SetFRect(m_nCur, rect);};
*/
  bool SetFRect(const Klassif *kls, int n, const CFRect& rect);
  int IsMark(const Klassif *kls, int n);
  int IsVisible(const Klassif *kls, int n);
  bool GetFRect(const Klassif *kls, CFRect& rect);
  bool SetFRect(const Klassif *kls, const CFRect& rect);

  bool GetBorder(CFRect& rect);
  bool GetBorder2(CFRect& rect);
  bool GetBorder(const Klassif *kls, CFRect& rect);
//  int NObj() {return m_kls->geo4.size();};
  bool FieldName(int fH, string & str);
  bool FieldName1(int fH, string & str);

  bool TrioMove2(CFPoint *m_trio_point);

  int NMark(const char *fn, list<int> &list_id);
  int NMark(Klassif* kl, list<int> &list_id);

  int NMark(const char *fn);
  int NMark(Klassif* kl);
  int NMark();

  bool InitS(const char *tN, const char *qN = NULL);
  bool SetKls(const char *tN, const char *qN = NULL);

  bool open(const char * fN);
  bool close();
  bool GoToN(int n);
  bool readTable(int sl);
  bool readGEO();
  bool readGEO_Shape();
  bool readGEO_Geomedia();
  bool readGEO_Gidr(const char *name);
  bool readSHP(const char *fn);
  bool read(int fN, string & str);
  bool read(const char *fN, string & str);

  bool read_find(int fN, string & str);
  bool read_find(const char *fN, string & str);

//  void reset_geo(Klassif *m_kls);
//  void reset_geo();
//  void reset_geo(int i);

//  bool write(int fH, const char *str);
//  bool write(const char *fN, const char *str);

//  bool write_find(int fH, const char *str);
//  bool write_find(const char *fN, const char *str);

  bool readMark(string & str);
  bool reget();
  bool update();
  CFPoint find();
  int findN(const char *cod, int n);
  void query(const char *cod, const CCoordList& cl, int &nn, QQQ *& qqq);
  void check();
//  bool del(unsigned int nom);
//  bool del(Klassif *kls, unsigned int nom);
  bool addget(Klassif *kl);
  void FileAddDel(Klassif *kls, int add);

  void isprShape();
  void isprGeo();

  void isprObj(CCoordList &cl);
  int createObjPassportTP(const char *tN, const CCoordList& cl);
  int createObj(const char *tN, CCoordList& cl, map<CString, COleVariant> &m_map_geo_value);
  int createObj2(const char *tN, CCoordList& cl, map<CString, COleVariant> &m_map_geo_value);
  int createObjZd(const char *tN, const CCoordList& cl, const char *street, const char *nomer, int et, const char *naimen);
  bool updateObjField(const char* tn, int id, const char* fn, int value);
//  int updateObj(const char *tN, unsigned int nom, const CCoordList& cl);
  int createObjElPoint(const char *tN, const CCoordList &cl, map<CString, COleVariant> &m_map_geo_value);
//  void write_cl(const char *tn, const CCoordList &cl);
  int createObjRemont(const char *tN, CCoordList &cl, CLINE2 *l, map<CString, COleVariant> &m_map_geo_value);
  int createObjKapital(const char *tN, const CCoordList &cl, CLINE2 *l);
  int createObjKapital0(const char *tN, const CCoordList &cl, CLINE2 *l);

  int createObjOpressovka(const char *tN, const CCoordList &cl, CLINE2 *l);
  int createObjOpressovka0(const char *tN, const CCoordList &cl, CLINE2 *l);
  
  int createZhurnal(const char *tN, const CCoordList &cl, CLINE2 *l);

  int createText(CFPoint pt1, CFPoint pt2, const char *text);


  void DxfLayer_shp_dbf(FILE *dxf, const char *tn, CFRect & rectBorder);
  void DxfLayer(FILE *dxf, const char *tn, const char *in, CFRect & rectBorder);
  void Dxf(FILE *dxf, CFRect & rectBorder);
  void DxfKlass(FILE *dxf);

  void DxfKlass1(FILE *dxf);
  void DxfKlass2(FILE *dxf);


  void SetDbN(int n);
//  int GetLoc() { return m_spr.loc;};
  int GetLoc(const Klassif *kls) { return kls->loc;};
  int GetColor(const Klassif* kls) { return kls->coli;};
  int GetColor2(const Klassif* kls) { return kls->colb;};
  int GetColor3(const Klassif* kls) { return kls->col3;};
  int GetColorP(const Klassif* kls) {
    return kls->colb != 0xFFFFFF && kls->loc == LOC3 ? kls->coli : kls->coli;
  };

  bool GetMarkRect(const Klassif *kls, CFRect& rect);

  int GetNom(const Klassif *kls, int n);

/**
  int GetNom(int n) {
    return m_kls->geo4[n-1]->nom;
  };
*/

//  void writeNode(const char *tn, int nn, CNode *nn1, CNode *nn2);
//  void write(const char *tn, int nn, const char *fn, const char *val);
//  void write(const char *tn, int nn, const char *fn, double val);
//  void write(const char *tn, int nn, const char *fn, int val);

/*

  void SetMark(int n, bool mark = true) {
    m_kls->geo4[n-1]->mark = mark;
  };

  void SetNom2(int n, int nn) {m_kls->geo4[n-1]->nom2 = nn;};
  void RevMark(int n) {m_kls->geo4[n-1]->mark = !m_kls->geo4[n-1]->mark;};
*/
  void SetMark(Klassif *kls, int n, bool mark = true);
  void SetNom2(Klassif *kls, int n, int nn);
  void RevMark(Klassif *kls, int n);


  bool ClearMark();

  void ExportMIF(Klassif *kls);
  void ImportMIF(const char *fN, Klassif *kls);
  void ExportSHAPE(const Klassif *kls, int n_qq = 0, int *qq = NULL);
  void ExportSHAPE(int n);

  void Perez(const char *tN);

  void i2toi2(const char *fN, const char *kN);

  int findXYmin(CFPoint p, char *fn);

  int findXY(CFPoint p, double md, char *fn);
  int findXY(CFPoint p, double md, int loc, char *fn);
  int findXY_list(CFPoint p, double md, int loc, set<CString> &list_fn, char* fn);
  int findXY2(CFPoint p, double md, int loc, char *fn);
  int findXY3(CFPoint p, double md, int loc, const char *fn);
  int findXY3_bl(CFPoint p, double md, int loc, const char *fn);
  int findXYkls(CFPoint p, double md, int loc, Klassif **kl);


  bool findXY_end_exact(CFPoint p, CString fn, map<CString, set<int> > & set_out );
  bool findXY_end_list_exact(CFPoint p, set<CString> &list_fn, map<CString, set<int> > & set_out );


  int findAllXY(CView *viev, CFPoint p, double md/*, char *tn, int &num*/);
  int findAllXY(CGeoMenu *menu, CView *viev, CFPoint p, double md/*, char *tn, int &num*/);
  
  bool findString(const char *str, bool matchcase, bool matchwholeword, bool next, char *tN, int &n);
  bool findKadn(const char *fn, const char *str, bool matchcase, bool whole, bool next, char *tN, int &n);
  bool findQ(const char *q, bool next, char *tN, int &n);

  void OnDBF();
  unsigned int GetNumList(const Klassif *kls, unsigned int num);

  int ExpPer(const char *tn, unsigned int nom);

  bool GDB(const char *name);
  bool GDB_Oracle();
  bool Geomedia(const char *name);
  bool Code();
  bool openMySQL(int type_of_net, const char *server, int port, const char *database, const char *user, const char *password, bool special);
  bool readMySql_Shape(CAdoFile *ado);
  bool readMySql_Shape_table(CAdoFile *ado, Klassif *kls, Percent2 *pc = NULL, int p1 = 0, int p2 = 100);
  bool readMySql_Shape_table1(CAdoFile *ado, const char *q, Percent2 *pc = NULL, int p1 = 0, int p2 = 100);

  bool isRead(const Klassif *kls, int n);
  bool isReadLabel(const Klassif *kls, int n);

  CFPoint find(const char *tn, const char *coordn = NULL);
  CFPoint findNext();
//  int edit(CScroll *scr, CFPoint pt, int md);
  CDC *view_object(CScroll *scr, const char *tn, int n);
  void no_view_object(CScroll *scr, CDC *dc);

  void Draw(CDC *dc, CGidrView *scr, int loc, int isMarkOnly, int isText, bool is_GID);
//  void Draw(CDC *dc, CGidrView *scr, CRect rect, double m_bx, double m_by, double masx, double masy, int loc, int isMarkOnly, int isText, bool is_GID);
  void Draw(CDC* dc, CGidrView* view, CRect rect, ScrollGeom &geom, int loc, int isMarkOnly, int isText, bool is_GID);

  
  void DrawLoc(CDC *m_dc, CGidrView *scr, int loc, bool fill, bool label, int isMarkOnly, bool is_GID);
  bool Filtr();

  CFPoint findPoint(CFPoint pt);
  void findCl(const CFPoint &p1, const CFPoint &p2, const CCoordList &cl1, CCoordList &cl2);

  void initGeoTree(CTreeCtrl &m_ctlTree);
  void initGeoTree(const char * fn);
  void initGeoTree(const char * fn, CTreeCtrl &m_ctlTree, HTREEITEM i1);

  bool addLayer(const char *fn, Klassif *kls);
  bool delLayer(const char *fn, const char *text);

  void set_coord_geo(double x0, double y0, double alpha);
  void set_coord_geo_obr(double x0, double y0, double alpha);

  void reset_kl(const Klassif *kls);
  void klassif();

  void zad();

  bool setMark(CCoordList &cl);
  bool setMark(Klassif *kls, CCoordList &cl);
  bool undo();


  BOOL isNext();

  bool isOpen() {return m_isOpen;};
  void closeMark() { m_isOpen = false;};

  CCoordList Add(const Klassif *kls);

//  Klassif *m_kls;
  CString m_fn;
  CString m_name;

  int NFlds();

  int m_n_list;

  KlList *m_kl_list;

  CString m_tn;
  CString m_tn1;
  CString m_coordn;
  int m_nCur;            // Текущий объект
  int m_numCur;
  BOOL is_shape;
  BOOL is_shpdbf;
  BOOL is_gidr;
  BOOL is_odbc;
  BOOL is_geomedia;
  int m_old_remont;
  CString m_old_TN;

//  CGeoList geo4[256];
//  int m_n_sl;
  Cshp_dbf *m_shp;
  bool read_label(const Klassif *kls, int n, CString &text, CString &text_color, double &x, double &y, double &alpha, double &coef, short &align);
  bool read_label(const Klassif *kls, CString &text, CString &text_color, double &x, double &y, double &alpha, double &coef, short &align);
  bool read_label_MI(const Klassif *kls, CString &text, double &x, double &y, double &alpha, double &coef, short &align);
  void readXYText(double &x, double &y, double &a, double &coef, short &align);
//  void writeXYText(const char *tN, unsigned int nom, double x, double y, double a, double coef, short align);
  CString read_textstring();
  double read_angle();
  double read_size();

  void DrawObj1(CDC *m_dc, CScroll *scr, const Klassif *kls, int num, CCoordList &cl, bool redr, bool isMark, bool fill, bool label, bool no_mas);

  void DrawObj(CDC *m_dc, CScroll *scr, const Klassif *kls, int i, bool redr, bool fill, bool label, int isMarkOnly, bool no_mas);
  void DrawLabel1(CDC *m_dc, CScroll *scr, const CCoordList& cl);
  void DrawLabel1_MI(CDC *m_dc, CScroll *scr, const CCoordList& cl, Klassif* m_kls2);
  void DrawLabel2(CDC *m_dc, CScroll *scr, const CCoordList& cl);
  void DrawLabel2(CDC *m_dc, CPoint pt, CScroll *scr, const CCoordList& cl, const char *s);
  void DrawLabel3(CDC *m_dc, CScroll *scr, const CCoordList& cl, Klassif* m_kls2);

  void drawLayer(CDC *dc, CScroll *scr, const char *tn, bool label, int isMarkOnly);
  void drawLayer(CDC *m_dc, CScroll *scr, const char *tn, const char *in, bool fill, bool label, int isMarkOnly);

  void drawLayer_xy(CDC *m_dc, CScroll *scr, const char *tn, const char *in, bool label, int isMarkOnly);
  void drawLayer_shp_dbf(CDC *m_dc, CScroll *scr, const char *tn, bool label, int isMarkOnly);
  void setExit() {is_exit = TRUE;};

  CFPoint toGeo(CFPoint pt);
  BOOL isCoord() { return m_is_coord; };
  bool delGeoObj(const char *tn, int id);
  bool updateObj(const char *tn, int id, const CCoordList &cl);

  void setVisible(const char *tn, const char *q);
  void setVisible(const Klassif *kls, const char *q);

  void setVisibleColor(const Klassif* kls, const char* q);

  void setVisibleAll(const Klassif* kls);
  void setVisibleAll(const char *tn);
  void setVisibleAllNo(const Klassif* kls);
  void setVisibleAllNo(const char *tn);
  void setVisibleAllRemonts();

  void refreshRemont();
  void refreshElectro();


  int m_n_undo;

protected:

  void CheckTyp(Klassif *kls);
  void MkReport(Klassif *kls, const char *fN);

//  int m_cur_sl;
  double m_frmx1, m_frmx2, m_frmy1, m_frmy2;
  bool m_isOpen;
  BOOL m_bIsGeoGeodez;
  COleVariant m_bookmark;
  long m_abs_pos;
  CGidrView *m_view;
  BOOL is_exit;

  BOOL m_is_coord;
  double m_geo_x[6];

  int m_n_vys1, m_n_vys2;
  int n_teh_1, n_teh_2, n_teh_3, n_teh_4, n_teh_5, n_teh_6, n_teh_7, n_teh_8;

int n_coord, n_shape, n_point;
int n_MinGX, n_MinGY, n_MaxGX, n_MaxGY;
int n_EMinX, n_EMinY, n_EMaxX, n_EMaxY;

  int m_typop;
  CDelObj del_obj;
  CMoveObj move_obj;

friend CGeoFileIterator;
};

extern CGeoFile *m_geofile;

bool allGeoVisible(const char *name, bool v);



#endif // !defined(AFX_GEOFILE_H__BD7FFF15_62E2_11D4_B15B_AB099AB1734B__INCLUDED_)
