#pragma once

#include "gid6Doc.h"

#include "scroll.h"
#include "cxema.h"
#include "Graph.h"
#include "Graph2.h"

#include "Podl.h"

#include <math.h>

#include "GeoFile.h"

#include "dxf/DXFI.H"

class WaitCursor;

//#include "Colors.h"

void setView(CView* pView0);
CGidrView *getGidView();
void setGidView(CGidrView *view);

void setMag(int typ, int mag);

struct VYD_LINES;
class HTML;
class CDialogNew2;
class CDialogNew3;
enum VIRT_DATA;

class CNode2;
class CLine2;

class CPropertyGrid;
class CTableDialog;

class CMapsThread;
struct MAG;
struct RS;

enum TypMenu
{
    TYP_MENU_REZHIM = 1,
    TYP_MENU_READONLY = 2,
    TYP_MENU_PTS = 4,
};


extern BOOL m_bIsMas;
extern TypMenu m_idMenu;
extern int m_bIsRezhim;

bool IsRezhim(const CNode2 *n);
bool IsRezhim(const CLINE2 *l);

BOOL isEditGeo(const char *tn, bool isMessage = true);
BOOL isDeletable(const char* fn, int n, bool isMessage = true);

struct KamState
{
    CFRect rect;
    int internalNodeID;
    int fileID;
};

struct BottomInfo
{
    BottomInfo()
    {
        stateID_name = "stateID";
        q = tn = title = "";
        typ = virt_data_geo;
    };

    CString stateID_name;
    CString q;
    CString tn;
    CString title;
    VIRT_DATA typ;
};


struct OPCvar
{
    OPCvar(const char *_str, int _x, int _y) :
        str(_str), x(_x), y(_y), is_on_off(false) {};
    OPCvar(const char *_str, CLine2 *_l, char _po, double _x1, double _y1, double _x2, double _y2) :
        str(_str), x1(_x1), y1(_y1), x2(_x2), y2(_y2), l(_l), po(_po), is_on_off(true)
    {};

    int x, y;
    CString str;

    CLine2 *l;
    double x1, y1, x2, y2;
    char po;
    bool is_on_off;
};


class CGidrView : public CScroll
{
protected: // create from serialization only
    CGidrView();
    DECLARE_DYNCREATE(CGidrView)

// Attributes
public:

//  CGidrView();
//  CGidrView(CGidrView *view, CNode2 *parent_kam);

    void init();

    Cgid6Doc* GetDocument();

    int m_parent_id;
    int m_parent_id_old;


//  CNode2 *m_parent_kam;
    CCxema *m_cxema;
//  CCxema *m_cxema_main;
//  CNodeName m_nn;

    CNode2 *m_node_first;
    CNode2 *m_node_last;

    CNode2 *m_node_pick1;
    CNode2 *m_node_pick2;

    bool m_is_reset_visible;

    bool m_is_geo;

    bool m_is_CTeploset;  // Выбирались ли тепловые сети

    BOOL m_bIsGeoGeodez;
    BOOL m_reread;

    int m_isMarkOnly;
    bool m_no_gid = false;

    int m_web_layer = 0;  // Слой для WEB

    CNode2 *cur_node;
    CString cur_geo_tn;
    int cur_geo_n;

    int m_zhurnal; // Журнал

    int m_id_RS;
    int m_id_MS;
    CString m_kod;
    int m_is_rs;
    int m_n_kapremont;

    bool m_isPasport;
    bool m_isOpressovaka;
    bool m_isNeOpr;

    int m_opressovka;
    int m_opressovka_uchastok;

    int m_mag, m_ms, m_rs;
    int m_mag_old, m_ms_old, m_rs_old;
//  CString m_kod0_vyd;
    int m_magistral_kod;
    int m_ms_kod;
    int m_rs_kod;

    int m_linePTS_ID;

    int m_pts_ut_kod;

    bool m_zadv_yes = false; // Использовались ли секцинирующие задвижки для опрессовки

    int m_sezon_korrozia;
    COleDateTime m_sezon_date1;
    COleDateTime m_sezon_date2;



    int m_magistral_kod0;
    int m_ms_kod0;
    int m_rs_kod0;

    BOOL m_isGeobaza;

    BOOL m_isPodpAll;

    BOOL m_isMS_RS_KOT_Yes;
    BOOL m_isOnlyPTSColor; // Выделять цветами только участки ПТС и чужие. остальные черным

    BottomInfo m_bottom_info;

// Operations
public:
    void BeginDrawNode(const CNode2 *node);
    void BeginDrawLine(const CLINE2 *Line);
    void open_back();
    void openGid(CNode2 *parent);
    void openGidODBC();
//  void open(const CNodeName &parent);

    void drawAll(CDC *dc, CRect rect);

    void setDate(CString tn, CString fn, CString name_mn);

    void setDefect();
    void setDefectForRemonts();

    void OnSetDate1();
    void OnSetDate2();
    void OnSetDate3();

    void OnSetRemontType();

    void initLeft();
    void drawPaint(CDC *m_dc);
    void drawPaint2(CDC *m_dc, bool is_gid = true, bool is_geo = true, bool is_map = true);
    void drawBorder(CDC *m_dc);
    void drawTexts(CDC *m_dc);

    void drawLoc1(CDC *dc, int isText, bool is_GID);
    void drawLoc2_REM(CDC *adc, int isText, bool is_GID);

    KamState Kam(int internalNodeID);
    void KamObr(KamState &kams);
    void KamObr();

    CString qGeoTable(Klassif *kls);

    void drawGid(CDC *m_dc, CGraph2 *m_graph, CFRect &rectScr);
    void drawOPC2();
    void redrawOnlyGid();
    bool setMark(const TCHAR *table);
    bool geo_edit(const char *fn, int n_geo);
    bool geo_edit6(const char *fn, int n_geo, CLine2 *l, const CFPoint &point);
    bool addGeo(Klassif *kls, bool init = true);
    bool addGeoTab(const char *fn);
    bool delGeoTab(Klassif *kls);
    bool tabGeo(Klassif *kls);
    bool tab_kls(Klassif* kls);
    bool tabGeoNeiz(Klassif *kls, int zd);
    bool findIS(const char *name);
    CNode2 *findIST(const char *name);
    bool setRS(const char *name, int ch);
    void MainCxema(const char *fn);
    BOOL reopen();
    BOOL reopen_all();
    BOOL isEditGid();
    BOOL isReadOnly();
    BOOL isEditPS();
//  BOOL isEditGeo(const char *tn);
//  BOOL isEditAkt();
    BOOL isAdminFile();
//  BOOL isReadOnly();
    void GeoNadpis(const char *tn, int num, BOOL b);
    void SetWindowText2();
    void move_line(CNodeName &n1, CNodeName &n2);
    bool reset();

    void moveRS(const char *rs);

    void setMagAll(MAG *mag);

    bool setMag(int typ_mag, int id, const char *name, int move);
    void beginRas(int id, const char *rn);
    void beginRas2(int id, const char *rn);
    void beginRasAll(int id, const char *rn);

    void beginMS1(int id, const char *rn);
    void beginMS2(int id, const char *rn);

    void setKorroziaSezonDlg();

    void setKorroziaSezon(int y, bool force);

    void geoEdit(CWnd *wnd, CString tn, int id);


    void addKls(Klassif* kls, bool init = true);
    void setOpresNull();

    void AlsecoNagr(int id);

//  void setRS(const char *strZn, const RS *rs);

    void MagUzelInfo(CNode2 *node);
    void MagUzelTab(CNode2 *node);
    void MagUzelTab(CNode2 *node, int Mag);

    bool MoveVydAll();
    bool MoveVyd();
    bool moveCurrent();

    bool MoveLinePTS();

    bool MoveVydIf();
    bool MoveMSRS(int mag_typ, int mag_id);

    int GeoInfo(const char *tn, int n, bool autodelete);

    void viewTable(const char *tn);
    void viewTable2(const char *db, const char *tn, const char *tn2, const char* caption, VIRT_DATA typ, bool all = false);
    void viewQ(const char *q, VIRT_DATA typ);
    void viewQ2(CAdoFile* ado, const char *db, const char *q, const char *tn, const char *head, VIRT_DATA typ, CString add_text, const char *title, bool all = false);
    void viewQ2key(const char *db, const char *q, const char *tn, const char *head, VIRT_DATA typ, CString add_text, CString key_name, int key_value);


    void viewQhead(const char *db, const char *q, const char *tn, VIRT_DATA typ);
    void viewTP(const char *db, const char *q, const char *tn, VIRT_DATA typ);

    void RunDoIt(const char* cmd1, const char* txt);
    bool RunTable(const TCHAR *table, const TCHAR *query, int nom, VIRT_DATA typ, CString add_text, const char *title);
    bool RunTableAdo(CAdoFile* ado, const TCHAR* table, const TCHAR* query, int nom, VIRT_DATA typ, CString add_text, const char * title);
    
    bool RunTableDB(const char *db, const TCHAR *table, const TCHAR *query, int nom, VIRT_DATA typ, CString add_text, const char *title);
    bool RunTableMap(const char *mdb, const TCHAR *table, const TCHAR *query, int nom);
    bool RunTableTU(const char *mdb, const TCHAR *table, const TCHAR *query, int nom);

    int CreateGeoTableKapital(CGraph2 *graph, const char *tn);
    int CreateGeoTableOpressovka(CGraph2 *graph, const char *tn);

    bool findNodeLine(const CFPoint &pt, CNode2 **node, CLINE2 **line, double m5);
    bool findNodeLineUSA(const CFPoint &pt, CNode2 **node, CLINE2 **line, double m5);

    void raionMaster(int id);

    bool info_gid(CCxema *cxema, const CNode2 *node, const CLINE2 *line, CWnd *wnd = nullptr);
    bool info_gid_DR(CCxema *cxema, const CNode2 *node);
    bool info(CCxema *cxema, CNode2 *node, CLINE2 *line, CWnd *wnd = nullptr);
    bool infoZN(CCxema *cxema, CNode2 *node);
    bool infoUP(CCxema *cxema, CNode2 *node);
    bool infoVP(CCxema *cxema, CNode2 *node);

    void setRS(int id);
    void setMS(int id);
    void setKod(int is_rs, const char *kod);
//  void setMSNew(const char *kod, const char *opis, int ms_rs, bool mv);
//  void setMagNew(int mag, const char *opis, bool mv);

//  void setMSNew(const char *kod, const char *opis, int ms_rs, bool mv);
    void setMSNew(int id, const char *opis, int ms_rs, bool mv);

    void setMagNew(int mag, int ms, int rs, const char *opis, bool mv);
    void setMagNomer(int mag, int ms, int rs);
    void setMSNew(int ms_rs, const char *title);

    CFRect getMaxRect();

    void moveUchRemont(const char *tn, int ID);

    void moveNode(CNode2 *node);

    void moveGeo(const char *tn, int ID);
    void editGeo(const char *tn, int ID);

    void moveNode(const char *mdb, const char *kod, const char *name);

    int getActiv();
    void setActiv(int fileID);
    bool setActiv0(int fileID);

    void viewGeoUzel(CWnd *parent, const char *tn, int nom, bool readonly);
    void viewCurGeoUzel(CWnd *parent, bool readonly);
    CNode2 *getGeoNode(const char *tn, int nom);

    void Zn(CNode2 *node);

    int Word1(const char *name);
    int Word2(CDialogNew2 *dlg, const char *name, const char *tn, const char *avar, const char *fn, const char *path0);
    int Word3(CDialogNew3 *dlg, const char *doc_name, const char *avar1, const char *tn, const char *fn, const char *path0);

    int WordPassport(CDialogNew3 *dlg, const char *doc_name, const char *avar1, const char *tn, const char *fn, const char *path0);
    int WordTrub(CDialogNew3 *dlg, const char *doc_name, const char *avar1, const char *tn, const char *fn, const char *path0);
    int WordF2(CDialogNew3 *dlg, const char *doc_name, const char *avar1, const char *tn, const char *fn, const char *path0);
    int WordF3(CDialogNew3 *dlg, const char *doc_name, const char *avar1, const char *tn, const char *fn, const char *path0);
    int WordF4(CDialogNew3 *dlg, const char *doc_name, const char *avar1, const char *tn, const char *fn, const char *path0);
    int WordF5(CDialogNew3 *dlg, const char *doc_name, const char *avar1, const char *tn, const char *fn, const char *path0);
    int WordF6(CDialogNew3 *dlg, const char *doc_name, const char *avar1, const char *tn, const char *fn, const char *path0);
    int WordF7(CDialogNew3 *dlg, const char *doc_name, const char *avar1, const char *tn, const char *fn, const char *path0);
    int WordF8(CDialogNew3 *dlg, const char *doc_name, const char *avar1, const char *tn, const char *fn, const char *path0);
    int WordF9(CDialogNew3 *dlg, const char *doc_name, const char *avar1, const char *tn, const char *fn, const char *path0);
    int WordF10(CDialogNew3 *dlg, const char *doc_name, const char *avar1, const char *tn, const char *fn, const char *path0);
    int WordF11(CDialogNew3 *dlg, const char *doc_name, const char *avar1, const char *tn, const char *fn, const char *path0);
    int WordF12(CDialogNew3 *dlg, const char *doc_name, const char *avar1, const char *tn, const char *fn, const char *path0);
    int WordFA3(CDialogNew3 *dlg, const char *doc_name, const char *avar1, const char *tn, const char *fn, const char *path0);
    int WordFA4(CDialogNew3 *dlg, const char *doc_name, const char *avar1, const char *tn, const char *fn, const char *path0);

    int WordKorroz(const char *doc_name, int y2, const char *path0);
    int WordOpr(CDialogNew2 *dlg, const char *doc_name, const char *avar1, const char *tn, const char *fn, const char *path0);

    void RsLineIzolTabDb(CNode2 *node);

    void changeTable(const char *fn, const char *tn);
//  void setMS(int typ, const char *kod0, const char *val);
//  void setMSNew(int ms_rs, const char *title);
//  void setOpr(int typ, const char *kod0, int id);

    void vydP(list<CNode2*> &list_node);

    int RunTableNiz(CAdoFile* ado, const TCHAR* tn0, const TCHAR* q0, const TCHAR* title0, bool refresh, bool is_head, VIRT_DATA typ);
    int RunTableNizCod(int cod);
    int CloseTableNizCod(int cod);

    void RunGeoTable(Klassif *kls);
    void RunGeoTable(CGraph2* graph, Klassif* kls);
    void RunGeoTable0(Klassif *kls);
    void RunGeoTableNew(Klassif *kls);

    void vydLineQ(CString q);


    void set_zd();

    void savePjezo();

    bool isOpressovka();
    bool isRemont2();
    bool isPassport();

    void PaspRSMS(CNode2 *nl, CNode2 *n1, int ms_rs, int id);
    void PaspRSMS2(CNode2 *nl, CNode2 *n1, int ms_rs, int id);

    bool isOpenZhur(int id);
    void Zhurnal(const char *tn, const char *fStr0, const char *typ_column, const char *date_column, bool is_line, const char *title);
    void ZhurnalK(const char *tn, const char *fStr0, const char *typ_column, const char *date_column, bool is_line, const char *title);


    void ZhurnalQ(const char *tn, const char *q, const char *title, int sost);
    void ZhurnalQ2(const char *tn, const char *q, const char *title, bool reset_visible, VIRT_DATA typ = virt_data_geo);

    void view_nodes();
    void view_PT();

    void vydGeo(const char *tn, CCoordList &m_NP);
    void korroziaPlan(const char *tn, CCoordList &m_NP);
    void korroziaAr(const char *tn, CCoordList &m_NP);
    bool korrozia_del(const char* tn, int id);

    int import_tgid0(CAdoFile *ado, CString fn);

    void UtZakr(bool all, const char *title);

    int m_prop_id;
    CPropertyGrid *m_prop_grid;
    CTableDialog *m_table_node;
    CString m_prop_table;
    CString m_prop_field;
    CString m_field_for_dist;
    CFPoint m_pt_object;

    CLINE2 *m_l_canal;
    CFPoint m_pt1_canal;
    CFPoint m_pt2_canal;


    CGidFile *curFile();

// pas2
    void MagLineTabIzol32(FILE *f, CNode2 *n);
    void MagLineTabKanal2(FILE *f, CNode2 *n, VYD_LINES &vyd_l);
    void OnPaspKam2();
    void PaspRSMS_2(CNode2 *nl, CNode2 *n1, int ms_rs, int id);
    void print_sheetKapital2(HTML &h, VYD_LINES &vyd_l, CNode2 *nr, CNode2 *nm);
    void print_sheetOpressovka2(HTML &h, VYD_LINES &vyd_l, CNode2 *nr, CNode2 *nm);
    void RsLineTab2(FILE *f, CNode2 *node);
    void RsLineTabIzol2(FILE *f, CNode2 *node);
// - pas2

    virtual void set_coord_gid(double x0, double y0, double alpha, bool convert = true);



//  void viewTG(int id);

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CGidrView)
public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual void OnInitialUpdate();
protected:
    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
    //}}AFX_VIRTUAL



// Implementation
public:
    virtual ~CGidrView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

//  CGraph2 *m_graph;

protected:
    HACCEL m_hAccel;                     // dialog accelerators

protected:
    int m_num;
    int m_ng;

    CString m_geo_name;

    BOOL m_bIsOpen;

    int m_n_txt;
    int m_nTimeOut;
//  BOOL m_bIsInfo[20];
//  BOOL m_bIsGidrInf;
    BOOL m_bOnlyGid;
    BOOL m_bIsPicture;
    BOOL m_bIsOpenPicture;
    BOOL m_bIsPribor;
    BOOL m_isKm;
    BOOL m_bIsAlma;

    CString m_strPicture;
    CString m_istName;
    bool m_zima;
    CNode2 *m_n1_neotr, *m_n2_neotr;
    CNode2 *m_n1, *m_n2;
    CLINE2 *m_line;
    CNode2 *m_napr1;
    CNode2 *m_CurNode;  // Выделенный узел
    CLINE2 *m_CurLine;  // Выделенный участок
//  CNode2 *m_CurNode1;  // Выделенный узел
//  CLINE2 m_CurLine1;  // Выделенный участок
    CString m_password;
    CPodlList *qqq_list;
//  CGeoFile m_geofile;
    int m_szY;
    Klassif *m_kls;
    Klassif *m_kls_for_del;
    Klassif *m_klsP;
    map<CString, CString> mapOPC;
    cdxf *m_dxf;
    CFPoint m_trio_point[6];
    int m_n_trio_point;
    int typ_uch2;
    int m_n_akt;
    CNode2 *m_menu_node;
    CLINE2 *m_menu_line;
    CString m_cur_file_name;
    CText *m_text_del;

    bool m_mark_changed = false; // Поменялось выделение на схеме 

    int m_niz_code; // Номер таблицы показываемой внизу


    map<CString, COleVariant> m_map_geo_value;

    void initGeoValue();
    void AddGeoValue(const char *name, const COleVariant & var);


    CFPoint m_menu_pt;
    int m_menu_ngeo;
    int m_menu_ngeo0;
    CString m_menu_tn;
    CString m_menu_tn0;
    int m_kod_vyd;
    int m_kod0_ms_rs;

    CString m_mkr = "";
    CString m_street = "";
    CString m_dom = "";

    CString m_sql_for_defect = "";

public:
    std::set<int> m_set_ms;
    std::set<int> m_set_rs;
protected:


//  COPCThread *m_pOPCThread;
    CMapsThread *m_pMapsThread;

    int m_menuType;
    BOOL m_bIsPodp;
    BOOL m_bIsFlag;
    BOOL m_bIsPodpNiz;
    BOOL m_bIsPodpVerh;
//  BOOL m_bIsRezhim;
    BOOL m_bIsPTS; // ‘еҐ¬  Џ’‘
    BOOL m_bIsPodpRes;
    double m_mmasx, m_mmasy, m_mbx, m_mby;
    char m_tmpPicN[256];
    UINT m_nTimer;
    UINT m_nTimer2;
    UINT m_nTimer3;
    list<OPCvar> opc_var;
    CString strQuery, strConnection, m_name;          ///
    CString m_Magistral1;
    int m_MagistralID1;
    CString m_Magistral2;
    int m_MagistralID2;

    CString m_Raspr;
    int m_RasprID;

    CString m_Magistral;
    int m_MagistralID;

    int m_MagLine;
    int m_MagistralTyp;
    CNode2 *m_MagLineNode;

    CFRect m_rect_old;

    int m_mag_n;
    vector<pair<CNode2 *, CNode2 *> > m_mag_node;
    vector<CNode2*> m_ras_node;

    int m_mark_regim;

    int m_fileID2;

    int m_nachalnikID;
    set<int> m_set_nachalnikID;

    double old_masx;
    double old_masy;
    double old_m_bx;
    double old_m_by;
    CFRect old_rect;

    void openBmp(const char *fn);
    void drawLineTypL(CLine2 *l, char po, BOOL redr, CDC *m_dc, double thickness, double x1, double y1, double x2, double y2, int typ, int color, int colorF, BOOL ris = TRUE, BOOL napr = TRUE, BOOL strelka = FALSE, BOOL isQ = TRUE);
    bool isPot();
    void move_gid(CGidFile *gid_file, const char *tn, int n);
    int move_geo(const char *tn, int n);
    bool find(const CFPoint &point);
    bool sfind(const CFPoint &point);
    bool findNodeLine2(const CFPoint &pt, CNode2 **node, CLINE2 **line, double m5);
    bool findUP(const CFPoint &point);
    bool del(const CFPoint &point);
    bool del_remont(const CFPoint &point);
    bool del_geo(const char *tn, int n);
    bool del_geo(const char *fn, const CFPoint &pt);
    bool find_remont(const CFPoint &pt, char *fn, int &n);
    bool info_remont(const CFPoint &pt);
    bool info_kapremont(const CFPoint &pt);

    bool delCxema(const CNode2 *node);

    bool dob_kapital(const CFPoint &pt);
    bool dob_opressovka(const CFPoint &pt);

    bool vydel(const CFPoint &point);
    bool vydel4(CNode2 *node, CLINE2 *l);
    bool vydel(CNode2 *node);
    bool info(const CFPoint &point);
    bool infoNode(CNode2 *node);
    bool infoLine(CLine2 *line);
    bool infoGeo(const char *tn, int num);


    bool tu_new(const CFPoint &point);
    bool tu_new_line(const CFPoint &point);
    bool tu_info(const CFPoint &point);
    bool pts_info(const CFPoint &point);
    bool pts_karta(const CFPoint &point);
    bool pts_move(const CFPoint &point);
    bool tu_del(const CFPoint &point);
    bool pts_del(const CFPoint &point);
    bool pts_del(const CFPoint &point, Klassif *kls);
    bool info_TGID(const CFPoint &point);
    bool info2(const CFPoint &point);
    bool wincc_info(const CFPoint &point);
    bool zd(const CFPoint &point);
    bool info_gid(const CFPoint &point);
    bool info_gid_node(const CNode2 *node);
    bool info_gid_line(const CLINE2 *line);
    bool info_gid_node_DR(const CNode2 *node);

    bool edit(const char *fn, int n);
    bool edit(const char *fn, const char *kod, const char *name);
    void ism(const CLINE2 *l);
    virtual bool updateObj();
    virtual bool mark_vyd();
    virtual bool MoveFragm(double dx, double dy);
    void copyAkt(int m_n_akt, CFPoint point);
    void redrawPaint(CDC *);
    void redrawPaint2(CDC *);
    void drawLinePodp(CDC *m_dc, const CLINE2 *l, double dx1, double dy1, double dx2, double dy2);
    void find_uch_pod(const CLINE2 *ll, double dx1, double dy1, double dx2, double dy2);

    void CopyBlockBMP2(CFRect rect);
    void CopyPic(const char *fn);

    void drawLine00(CDC *m_dc, const CLINE2 *ll, BOOL ris, double xx1, double yy1, double xx2, double yy2, char podobr, bool redr, int t, int typ, int vyd, bool is_hide);
    void drawLine1(CDC *m_dc, const CLINE2 *ll, bool redr, bool is_text, bool is_hide);
    void drawLine2(CDC *m_dc, const CLINE2 *l, BOOL ris, double xx1, double yy1, double xx2, double yy2, char podobr, bool redr, bool is_text, int t, bool is_hide);

    void uch_pod2(CDC *m_dc, const CLINE2 *ll, double dx1, double dy1, double dx2, double dy2);
    void drawNode0(CDC *m_dc, const CNode2 *node, bool redr);
    void drawNode(CDC *m_dc, const CNode2 *node, bool redr);

    void drawNode2(CDC *m_dc, const CNode2 *node, bool redr);


    void OPCpoint(const CNode2 *node);
    void initOPCVar();
    void EndDrawNode();
    void EndDrawLine();
    bool move_to(const CFPoint &point);
    bool neotr_node(CNode2 *node, CFPoint point);
    void checkPjezo();
    void drawMark(bool redr);
    CFont *initFont(CDC *dc);
    CFont *initFont2(CDC *dc, LOGFONT& lf);
    void closeFont(CDC *dc, CFont *oldfont);
    void OpenGeo();
    void OpenDop1();
    void OpenDop(const set <CString> &set_db);
    bool geoInfo(CFPoint pt);

//  bool IsRezhim(const CNode2 *n);
//  bool IsRezhim(const CLINE2 *l);

    CString Elevation2(CFPoint pt);

    void drawPodz(CDC *m_dc, const CLINE2 *ll);
    void drawPodpNode(CDC *m_dc, CNode2 *node);
    bool setCoord(CLINE2 *l, CCoordList &m_NP);
    bool setState(CLINE2 *line, int state);

    bool moveLineEnd(CNode2 *n1, CNode2 *node, CLINE2 *line);

    CLINE2 *moveLine(long id);
    CLINE2 *moveLineBig(long id);

    bool IsMain() {
        return m_parent_id == 0;
    };

    bool DR_otkr(CLine2 *l);
    bool ZD_otkr(CLine2 *l);

    void Otkr(CLINE2 *l);
    void Otkr(CNode2 *n);
    void Podp(CNode2 *node, CLine2 *line);

    void Out(BOOL anal);

    void OnList();
    bool QueryParam(const CString& q, CString& r);
    void Query(const CString& name, const CString& r);
    void OnQuery();

    CString getOPC(const char *str);

    void init_mas_otn(CDC *dc);

    int m_regim_geo; // режим что делать с гео объектам после того, как их выделили
    CString m_regim_geo_tn; // Таблица из предыдущего режима


    int m_regim_ras;
    int m_map;
    BOOL m_isInternet;
    BOOL m_isUkrup;
    void SetField(const char *n1, const char *n2, const char *n3);
    void deleteLeft();
    void deleteLeftAll();
    void setMenu(TypMenu isEdit);

    void addUt(const char *kv, const CCoordList &cl, set<CNode2*> &set_node);
    const CNode2 *vydelUT(const CNode2 *nn, CString kv, map <CString, CCoordList> &map_kv, int typ, CCxema **cxema);

    void FindFileList(const char *fn);
    bool addCity(const CFPoint &pt);

    void AddMagUzel(int id);
    void IsmMagUzel(int id);


    void GeoIsm(const char *tn, int num);
    void GeoIsmPodp(const char *tn, int num);
    void GeoDel(const char *tn, int num);
    void GeoTU(const char *tn, int num);

    void zad(const char *fn);

    void MagUzelDel(CNode2 *node);
    void MagUzelAdd(CNode2 *node);
    void MagUzelIsm(CNode2 *node);

    void MagRightReset();
    void MagLineInfo(CLINE2 *line);
    void MagLineTab(CLINE2 *line);
    void MagLineTab(CNode2 *node);

    void MagLineInfoPO(CLINE2 *line, char po);


    void MagLineTab2(CLINE2 *line);
    void MagLineTab2(CNode2 *node);
    void MagLineTab2(FILE *f, CNode2 *n);


    void RsLineTab(FILE *f, CNode2 *node);
    void RsLineTabIzol(FILE *f, CNode2 *node);
    void RsLineTab2(FILE *f, CNode2 *node, int typ);

    void RsLineTab(CNode2 *node);
    void RsLineTabDb(CNode2 *node);
    void RsLineIzolTab(CNode2 *node);
    void MagLineTabIzol3(FILE *f, CNode2 *n);
    void MagLineTabKanal(FILE *f, CNode2 *n, VYD_LINES &vyd_l);

    void saveNode(CNode2 *node);

    void MagTrub(CNode2 *node);

    void RasUzelAdd(CNode2 *node);
    void RasUzelAdd1(CNode2 *node);
    void RasUzelAdd2(CNode2 *node);

    int checkZD(const char *tn, int &diam);
    int checkZD(CLINE2 *l, const char *tn, int &diam);

    CNode2 *getRSMode();

    void savePNG(const char *fn);

    virtual void InvalidateG(BOOL bErase = TRUE);
    virtual double mas1();
    virtual double mas2();

    void drawNodeTyp4(CDC *m_dc, double x, double y, int typ, int color);
    inline bool setNodeColorById(int id, int color);

    const CNode2 *findNode(CCxema *m_cxema, const CNode2 *n);
//  void vydelRS(CNode2 *node);

    CNode2 *findZN(CNode2 *node);

    bool isPodp(CNode2 *node);
    void sw_p(CLINE2 *line);
    void SetUchRs1(CLINE2 *l);
    void initMapRS();

    void SetMs();

    void makeUT();
    void viewUT();
    void viewGeo(const char *tn);
    void pasport_US(CNode2 *n);
    void pasport_PR(CNode2 *n);
    void pasport_VV(CFPoint pt);

    void OnRemontZapros(const char *a);
    void OnKorrozZapros(const char *a);

    void KorroziaZhurnal(int sost, const char *title);

    void CreateElectro(const char *tn);

    void PopupOtklPotr(CLINE2 *line);

    bool addNode(CFPoint &point);
    bool addLine(CNode2 *n1, CNode2 *n2, CCoordList &cl);
    bool insertNode(CNode2 *n, CLINE2 *line, double r);

    bool setXY(CNode2 *n, CFPoint &pt);
    bool setXY1(CNode2 *n, CFPoint &pt);
    bool swap(CLINE2 *l);

    bool renameNode(CNode2 *node);
    bool renameLine(CLINE2 *line);
    bool renameNode(CNode2 *node, int m_Index);

    bool updateObj(const char *tN, int nom, const CCoordList &NP);
    bool delGeoObj(const char *tn, int nom);

    void addUchToPTS(CLINE2 *l1, CLINE2 *l2);


    bool delLine(CLINE2 *line, bool ask);
    bool delNode(CNode2 *node, bool ask);
    bool delNode2(CNode2 *node);
//  bool delLine(CLINE2 *line)

    void print_sheetKapital(HTML &h, VYD_LINES &vyd_l, CNode2 *nr, CNode2 *nm);
    void print_sheetOpressovka(HTML &h, VYD_LINES &vyd_l, CNode2 *nr, CNode2 *nm);

    void printUE(HTML *h, int ue, int ms_rs, const char *txt);
    void setMsNode(CNode2 *node, int first_last);

    void setOprNode(CNode2 *node, int first_last);
    bool initUS(const CNode2 *parent);
    bool initUS1(const CNode2* parent, const CNode2* n2);
    bool initCxema(CNode2 *parent);
    CNode2 * addUS1(int fileID, int id, int po, CFPoint &point);
    CNode2 * addUS2(int fileID, int id, int connectID, int po, CFPoint &point);


    void printMag(FILE *h, int ms_rs);
    void printMag(int ms_rs);

    void passportOpreesovka(FILE *f);

    CString table_to_q_for_remont(CString fn);

    bool isToolbar(int id);
//  void setPagesRemontElectro();
//  void setPages(int n);
    bool updateEl(Klassif *m_kls, CFPoint point, int loc);
    bool updateElEnd(Klassif *m_kls, CFPoint point, int loc);
    bool updateWaterNode(Klassif* m_kls, CFPoint point);
    bool updateWaterLine(Klassif* m_kls);
    bool createWaterLine(Klassif* m_kls, CFPoint point);
    void toPoint(CFPoint pt);
    void setNavigator(bool redr = false);

    void vyd_tubing(int tubingID);
    bool delMark();

    void openFragment(int fileID, const char * name);
    void closeFragment(int fileID);
    void saveRemont2();
    void reopenFragments();

    void viewNizQ(const char *q);
    void viewNizGeoQ(const char *tn, const char *typ_column, const char *date_column, bool is_line);

    void viewUchPTS();
    void viewUchPTS2();

    int m_ado_update_typ;
    int m_ado_update_id;

    void OnHttp8080();

    void RemontZapros(int typ, int stateID);
    void SetButtomInfo(int typ, int stateID, bool is_date, CTime date1, CTime date2);

    double okr(double x, int dx);

    void sortNode();
    void sortLine();

    void electro_pas();
    void korrozia_pas();

    void Undo(bool only_del);

    void pas_excel_1(const char *label, const char *q, const char *html, const map<CString, CString> &map_fn, CString (*color_for_table)(CString fn, CString vn) = nullptr);
    void KorroziaOnOff(bool yes);

    double getQG();
    double m_QG;  // Пересчет G в Q для Михаила из Алматы

    void setAvto(BOOL yes);

    void setNachalnik(int nachalnikID);

    void resetZhurnal(bool is_reset_visible);

    CString ispr_q_for_zhurnal(CString fn, CString date_name, const map<CString, CString>* map_fn);
    bool init_for_defect(int id, CString title, CString condition);
    bool exec_init_for_defect(CString fn1, CString condition);

    void remontZhur(int idd, CString fn, CString tn, CString date_name, CString title, CString cond, const map<CString, CString> *map_fn, VIRT_DATA typ = virt_data_geo);

    void remontTable(CString fn, CString tn, CString date_name, CString title, CString state_cond, const map<CString, CString>* map_fn, VIRT_DATA typ);

    bool contur_changed();
    void addMenuNode(CMenu &menu, CNode2 *node);
    void addMenuLine(CMenu& menu, CLINE2* line);
    void addMenuGeo(CMenu &menu, Klassif * kls, int ng);


// Generated message map functions
protected:
    //{{AFX_MSG(CGidrView)
    afx_msg void OnRegim(UINT nID);
    afx_msg void OnUpdateRegim(CCmdUI* pCmdUI);
    afx_msg void OnUpdateZhurnal(CCmdUI* pCmdUI);
    afx_msg void OnMap(UINT nID);
    afx_msg void OnSprav1(UINT nID);
    afx_msg void OnUpdateMap(CCmdUI* pCmdUI);
    afx_msg void OnUpdateZhurnalExcel(CCmdUI* pCmdUI);
    afx_msg void OnIst(UINT nID);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnInf();
    afx_msg void OnUpdateInf(CCmdUI* pCmdUI);
    afx_msg void OnFont();
    afx_msg void OnFontMag();
    afx_msg void OnFontPanel();
    afx_msg void OnEditFind();
    afx_msg void OnEditFindPs();
    afx_msg void OnViewDbAlma();
    afx_msg void OnViewDbPts();
    afx_msg void OnViewDbPS();
    afx_msg void OnViewDb();
    afx_msg void OnViewDbAll();
    afx_msg void OnDoIt();
    afx_msg void OnDoItDr();
    afx_msg void OnDoItDr1();
    afx_msg void OnDoItDr2();
    afx_msg void OnDoItList();
    afx_msg void OnDoItDrList();


    afx_msg void OnDoItAnal();
    afx_msg void OnEndDoIt();
    afx_msg void OnEndDoIt2();
    afx_msg void OnEndPython();
    afx_msg void OnEndImport();
    afx_msg void OnEndExport();
    afx_msg void OnEndUnite();



    afx_msg void OnDbList();

    afx_msg void OnFindPodl();
    afx_msg void OnGeoFile();
    afx_msg void OnPodlDir();
    afx_msg void OnUpdateNodeNeotr(CCmdUI* pCmdUI);
    afx_msg void OnGeobaza();
    afx_msg void OnUpdateGeobaza(CCmdUI* pCmdUI);

    afx_msg void OnColorMagRsGid();
    afx_msg void OnUpdateColorMagRsGid(CCmdUI* pCmdUI);

    afx_msg void OnColorOnlyPTSColor();
    afx_msg void OnUpdateOnlyPTSColor(CCmdUI* pCmdUI);

    afx_msg void OnUpdateGeo(CCmdUI* pCmdUI);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnOut();
    afx_msg void OnEsc();
    afx_msg void OnPjezo();
    afx_msg void OnUpdatePjezo(CCmdUI* pCmdUI);
    afx_msg void OnProtocol();
    afx_msg void OnUpdateProtocol(CCmdUI* pCmdUI);
    afx_msg void OnOpengl();
    afx_msg void OnZona();
    afx_msg void OnFindGeo();
    afx_msg void OnFon();
    afx_msg void OnUpdateKam(CCmdUI* pCmdUI);
    afx_msg void OnUpdateKam2(CCmdUI* pCmdUI);
    afx_msg void OnGeoGeodez();
    afx_msg void OnUpdateGeoGeodez(CCmdUI* pCmdUI);
    afx_msg void OnUkrup();
    afx_msg void OnUpdateUkrup(CCmdUI* pCmdUI);
    afx_msg void OnGidinf();
    afx_msg void OnUpdateGidinf(CCmdUI* pCmdUI);
    afx_msg void On11();
    afx_msg void OnNovisual();
    afx_msg void OnIstochnik();
    afx_msg void OnSaveCxema();
    afx_msg void OnAsyncCheck();
    afx_msg void OnSaveOut();
    afx_msg void OnZam();
    afx_msg void OnZapNewDoc();
    afx_msg void OnZap1();
    afx_msg void OnZap2();
    afx_msg void OnZap3();
    afx_msg void OnZap4();
    afx_msg void OnZap5();
    afx_msg void OnZap6();
    afx_msg void OnZap7();
    afx_msg void OnZap7_1();
    afx_msg void OnAnal();
    afx_msg void OnAnalizRezhima();
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown4(UINT nFlags, CPoint point);
    afx_msg void OnPopupMagUzel1(UINT nID);
    afx_msg void OnPopupMagUzelIsm1(UINT nID);

    afx_msg void OnPopupIspr();
    afx_msg void OnPopupActiv();
    afx_msg void OnPopupInfo();
    afx_msg void OnPopupInfoNode();
    afx_msg void OnPopupInfoLine();
    afx_msg void OnPopupInfoLineBig();
    afx_msg void OnPopupUnite();
    afx_msg void OnPopupUnite2();
    afx_msg void OnPopupOut();
    afx_msg void OnPopupOutDR();
    afx_msg void OnPopupDel();
    afx_msg void OnPopupDelZn();
    afx_msg void OnPopupDelUp();
    afx_msg void OnPopupDelVp();
    afx_msg void OnPopupDelNode();
    afx_msg void OnPopupDelLine();
    afx_msg void OnPopupDelGeo();
    afx_msg void OnPopupWinccInfo();
    afx_msg void OnPopupNewUzel();
    afx_msg void OnPopupRename();
    afx_msg void OnPopupOtkr();
    afx_msg void OnPopupOtkrPr();
    afx_msg void OnPopupPribor();
    afx_msg void OnPopupPriborDel();
    afx_msg void OnPopupORPDel();
    afx_msg void OnPopupZn();
    afx_msg void OnPopupUp();
    afx_msg void OnPopupORP();
    afx_msg void OnPopupKam();
    afx_msg void OnPopupVnutr();
    afx_msg void OnPopupVyd();
    afx_msg void OnUpdatePopupNewUzel(CCmdUI* pCmdUI);
    afx_msg void OnPopupMove();
    afx_msg void OnPopupCanal();
    afx_msg void OnPopupLength();
    afx_msg void OnPopupOpcParam();
    afx_msg void OnPopupOpcWrite();
    afx_msg void OnClear();
    afx_msg void OnUpdateClear(CCmdUI* pCmdUI);
    afx_msg void OnEditor(int id);
    afx_msg void OnEditor1();
    afx_msg void OnEditor2();
    afx_msg void OnEditor3();
    afx_msg void OnEditor4();
    afx_msg void OnUpdateEditor1(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditor2(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditor3(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditor4(CCmdUI* pCmdUI);



    afx_msg void OnUpdateEditor(CCmdUI* pCmdUI);
    afx_msg void OnPopupClear();
    afx_msg void OnPopupPodp();
    afx_msg void OnPopupPodpOn();
    afx_msg void OnPopupPodpOff();
    afx_msg void OnPopupObjed();
    afx_msg void OnPopupSwap();
    afx_msg void OnBmp();
    afx_msg void OnSetMSNew();
    afx_msg void OnSetRSNew();
    afx_msg void OnSetLength();
    afx_msg void OnExportDxf();
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnClearOut();
    afx_msg void OnPotNagr0();


    afx_msg LRESULT OnMyMoveTo(WPARAM wParam, LPARAM lParam);

    afx_msg LRESULT OnRefresh(WPARAM wParam, LPARAM lParam);



    afx_msg LRESULT OnFindNode(WPARAM wParam, LPARAM);
    afx_msg LRESULT OnFindLine(WPARAM wParam, LPARAM);
    afx_msg LRESULT OnFindLineBig(WPARAM wParam, LPARAM);
    afx_msg LRESULT OnFindLineBigOsmotr(WPARAM wParam, LPARAM);
    afx_msg LRESULT OnFindLineBig2(WPARAM wParam, LPARAM);
    afx_msg LRESULT OnFindGeo(WPARAM wParam, LPARAM);
    afx_msg LRESULT OnFindGeoTu(WPARAM wParam, LPARAM);
    afx_msg LRESULT OnFindGeoEdit(WPARAM wParam, LPARAM);
    afx_msg LRESULT OnFindGeoBigLine(WPARAM wParam, LPARAM);

    afx_msg LRESULT OnFindZh(WPARAM wParam, LPARAM);
    afx_msg LRESULT OnFindZd(WPARAM wParam, LPARAM);
    afx_msg LRESULT OnFindOrg(WPARAM wParam, LPARAM);
    afx_msg LRESULT OnFindSelGeoObject(WPARAM wParam, LPARAM);
    afx_msg void OnAddress();
    afx_msg void OnAddress2();
    afx_msg void OnColors();
    afx_msg void OnZapros();
    afx_msg void OnZima();
    afx_msg void OnUpdateZima(CCmdUI* pCmdUI);
    afx_msg void OnFont2();
    afx_msg void OnRep();
    afx_msg void OnRepR();
    afx_msg void OnRepW();
    afx_msg void OnUpdateSetLength(CCmdUI* pCmdUI);
    afx_msg void OnOut2();
    afx_msg void OnAddDopFile();
    afx_msg void OnEmptyTemplate();
    afx_msg void OnMainCxema();
    afx_msg void OnGeoFindNext();
    afx_msg void OnUpdateGeoFindNext(CCmdUI* pCmdUI);
    afx_msg void OnChName();
    afx_msg void OnGeodz();
    afx_msg void OnGeodz1();
    afx_msg void OnGeodz2();
    afx_msg void OnSetDiams();
    afx_msg void OnSetTubingType();
    afx_msg void OnSetLosesShare();
    afx_msg void OnSetOtv();
    afx_msg void OnSetOrg();
    afx_msg void OnSetOpenRez();
    afx_msg void OnSetOpenKoef();
    afx_msg void OnSetOpenRezT();
    afx_msg void OnSetOpenGvsT();
    afx_msg void OnSetUr();
    afx_msg void OnSetTp();
    afx_msg void OnSetTr();
    afx_msg void OnSetKvUt();
    afx_msg void OnSetKvPt();
    afx_msg void OnSetKvart();
    afx_msg void OnSetUf();
    afx_msg void OnSetSher();
    afx_msg void OnSetDate();
    afx_msg void OnSetKolChas();
    afx_msg void OnSetKti();
    afx_msg void OnSetUdobOt();
    afx_msg void OnSetUdobVent();
    afx_msg void OnSetPodpOn();
    afx_msg void OnSetPodpOff();
    afx_msg void OnSetPodpOnAll();
    afx_msg void OnSetPodpOffAll();
    afx_msg void OnSetKodRs();
    afx_msg void OnSetKodOpr();

    afx_msg void OnSetAvtoOn();
    afx_msg void OnSetAvtoOff();

    afx_msg void OnSetShaiba();


    afx_msg void OnSetCoordNull();

    afx_msg void OnMark();
    afx_msg void OnRezhim();
    afx_msg void OnUpdateRezhim(CCmdUI* pCmdUI);
    afx_msg void OnPodp();

    afx_msg void OnPodpNiz();
    afx_msg void OnPodpVerh();
    afx_msg void OnUpdatePodpNiz(CCmdUI* pCmdUI);
    afx_msg void OnUpdatePodpVerh(CCmdUI* pCmdUI);

    afx_msg void OnPodpPO();
    afx_msg void OnPodpRes();
    afx_msg void OnUpdatePodp(CCmdUI* pCmdUI);
    afx_msg void OnUpdatePodpPO(CCmdUI* pCmdUI);
    afx_msg void OnUpdatePodpRes(CCmdUI* pCmdUI);
    afx_msg void OnPicture();
    afx_msg void OnUpdatePicture(CCmdUI* pCmdUI);
    afx_msg void OnKlassif();
    afx_msg void OnNavigator();
    afx_msg void OnUpdateNavigator(CCmdUI* pCmdUI);
    afx_msg void OnLeftBar();
    afx_msg void OnUpdateLeftBar(CCmdUI* pCmdUI);
    afx_msg void OnNewHouse();
    afx_msg void OnUpdateNewHouse(CCmdUI* pCmdUI);
    afx_msg void OnNewUch2();
    afx_msg void OnUpdateNewUch2(CCmdUI* pCmdUI);
    afx_msg void OnUpdateVyd(CCmdUI* pCmdUI);
    afx_msg void OnUpdateVydRemont(CCmdUI* pCmdUI);
    afx_msg void OnUpdateGeodzInternet(CCmdUI* pCmdUI);
    afx_msg void OnExportShape();
    afx_msg void OnPrPo();
    afx_msg void OnTemp();
    afx_msg void OnEditUndo();
    afx_msg void OnEditUndoDel();
    afx_msg void OnRasprMag();
    afx_msg void OnRasprMagNew();
    afx_msg void OnRasprMag2();
    afx_msg void OnRenameRs();
    afx_msg void OnMagistr();
    afx_msg void OnRayon();
    afx_msg void OnOtklonenia();
    afx_msg void OnCheck();
    afx_msg void OnUpdateIsMas(CCmdUI* pCmdUI);
    afx_msg void OnIsMas();
    afx_msg void OnMasPic();
    afx_msg void OnMasPodp();
    afx_msg void OnUpdateNaprGid(CCmdUI* pCmdUI);
    afx_msg void OnNaprGid();
    afx_msg void OnMoveLineEnd();
    afx_msg void OnZapOtr();
    afx_msg void OnPtTempMin();
    afx_msg void OnZapZavozd();
    afx_msg void OnZapIzm();
    afx_msg void OnZapOrp();
    afx_msg void OnZapVnCx();
    afx_msg void OnZapNullGeo();
    afx_msg void OnZapNezak();
    afx_msg void OnIstSety();
    afx_msg void OnBalans();
    afx_msg void OnAnaliz2();
    afx_msg void OnTimePr();
    afx_msg void OnTrio();
    afx_msg void OnPoteri();
    afx_msg void OnWincc();
    afx_msg void OnClose();
    afx_msg void OnSavePjezo();
    afx_msg void OnListPjezo();

    afx_msg void OnNewOpres();


public:
    int m_id_opr;
    int m_id_osmotr2;

//protected:
    int m_id_opr_old;
    int m_id_remont2;
    int m_id_remont2_old;
    int m_id_osvidet2;
    int m_id_osvidet2_old;

    int m_id_osmotr2_old;


//-----------------------------

    void viewOpresDialog(CString opres, bool autodelete);

    void ListOpresQ(CString opres, CString title, CString q, CString utverdit_key);

    void ListOpres(CString opres, CString title, CString name_name, CString date_name, int &m_id_opr);
    void LastOpres(CString opres, CString title, int &m_id_opr, int &m_id_opr_old, bool view_dialog);
    void SaveOpres(CString opres, CString title, int & m_id_opr, bool yes, bool first);

    void applyNodeColor(const std::vector<int>& ids, int color);


    void applyNodeColors(const std::unordered_map<int, int>& idToColor);

public:    
    void SaveOpresNew(CString opres, CString title, CString name_name, CString kontur_name, CString date_name, int id_nachalnik, int & m_id_opr, map<CString, CString> *map_param = nullptr);
protected:
    void viewOpresProtokol(CString opres, int &m_opr_id);
    void viewOsmotr(CString opres, int directionID);


    bool importFragmentMag(const char* mdb2);


//-----------------------------
    afx_msg void OnListOpres();
    afx_msg void OnLastOpres();
    afx_msg void OnNaprTableOpres();
    afx_msg void OnSaveOpres();
    afx_msg void OnSaveOpresNew();
//-----------------------------
    afx_msg void OnListRemont2();
    afx_msg void OnLastRemont2();
    afx_msg void OnNaprTableRemont2();
    afx_msg void OnSaveRemont2();
    afx_msg void OnSaveRemont2New();
//-----------------------------
    afx_msg void OnListOsvidet2();
    afx_msg void OnLastOsvidet2();
    afx_msg void OnNaprTableOsvidet2();
    afx_msg void OnSaveOsvidet2();
    afx_msg void OnSaveOsvidet2New();
//-----------------------------

    afx_msg void OnListOsmotr2();
    afx_msg void OnLastOsmotr2();
    afx_msg void OnNaprTableOsmotr2();
    afx_msg void OnSaveOsmotr2();
    afx_msg void OnSaveOsmotr2New();
//-----------------------------

    afx_msg void OnOsmotrSave();
    afx_msg void OnOpresSave(); 
    afx_msg void OnRemontSave();



    afx_msg void OnLastFaktory();
    afx_msg void OnOsmotrAnaliz();
    afx_msg void OnOsmotrAnaliz2();

    afx_msg void OnPopupDefect1();
    afx_msg void OnPopupDefect2();

    afx_msg void OnIspravl();
    afx_msg void OnRereadMap();
    afx_msg void OnExcel2();
    afx_msg void OnExcel3();
    afx_msg void OnExcel4();
    afx_msg void OnMapPath();
    afx_msg void OnTeplopoteri();
    afx_msg void OnUpdateTeplopoteri(CCmdUI* pCmdUI);
    afx_msg void OnUpdateTU(CCmdUI* pCmdUI);
    afx_msg void OnUpdateIznos(CCmdUI* pCmdUI);
    afx_msg void OnTeplopoteriFakt();
    afx_msg void OnTeplopoteriF();
    afx_msg void OnTeplopoteriExcel();
    afx_msg void OnUpdateSavePjezo(CCmdUI* pCmdUI);
    afx_msg void OnUpdateSaveOpres(CCmdUI* pCmdUI);
    afx_msg void OnUpdateOpr(CCmdUI* pCmdUI);
    afx_msg void OnUpdateSaveCurentOpres(CCmdUI * pCmdUI);


    afx_msg void OnMoveVyd();
    afx_msg void OnColorLine();
    afx_msg void OnColorLine1();

    afx_msg void OnUpdateMoveVyd(CCmdUI* pCmdUI);
    afx_msg void OnPotDop();
    afx_msg void OnUpdatePotDop(CCmdUI* pCmdUI);
    afx_msg void OnUtNapr();
    afx_msg void OnUtNapr2();
    afx_msg void OnPribor();
    afx_msg void OnUpdatePribor(CCmdUI* pCmdUI);
    afx_msg void OnFlag();
    afx_msg void OnUpdateFlag(CCmdUI* pCmdUI);
    afx_msg void OnC3Del();
    afx_msg void OnC3Add();
    afx_msg void OnAktCreate();
    afx_msg void OnUpdateAktCreate(CCmdUI* pCmdUI);

    afx_msg void OnPassportTpCreate();
    afx_msg void OnUpdatePassportTpCreate(CCmdUI* pCmdUI);


    afx_msg void OnFindGeoAddr();
    afx_msg void OnGeoZdan();
    afx_msg void OnPopupGeoInfo();
    afx_msg void OnPopupGeoMark();
    afx_msg void OnPopupGeoDel();
    afx_msg void OnPopupGeoIsm();
    afx_msg void OnPopupGeoIsmPodp();
    afx_msg void OnPopupGeoNadpis();
    afx_msg void OnPopupGeoTU();
    afx_msg void OnPopupGeoTULine();
    afx_msg void OnPopupGeoTU2();
    afx_msg void OnPopupGeoZD2();
    afx_msg void OnPopupGeoNagr();
    afx_msg void OnPopupGeoHide();
    afx_msg void OnPopupGeoShow();
    afx_msg void OnPopupGeoNapr();
    afx_msg void OnPopupCopyAkt();
    afx_msg void OnPovorot2();
    afx_msg void OnUpdatePovorot2(CCmdUI* pCmdUI);
    afx_msg void OnResetGeo();
    afx_msg void OnResetGid();
    afx_msg void OnIsprLast();
    afx_msg void OnUsers();
    afx_msg void OnUt1();
    afx_msg void OnUt2();
    afx_msg void OnUt3();
    afx_msg void OnPotZd();
    afx_msg void OnZad();
    afx_msg void OnMapCity();
    afx_msg void OnAddCity();
    afx_msg void OnInvert();
    afx_msg void OnMagUzel();
    afx_msg void OnPopupMagUzel();
    afx_msg void OnPopupMagUzelDel();
    afx_msg void OnPopupMagUzelInfo();
    afx_msg void OnPopupMagLineInfo();
    afx_msg void OnPopupMagUzelAdd();
    afx_msg void OnPopupMagUzelIsm();
    afx_msg void OnPopupMagTrub();
    afx_msg void OnNextUzelZn();
    afx_msg void OnNextUzel(UINT nID);
    afx_msg void OnPopupRasUzelInfo();
    afx_msg void OnAllNode();
    afx_msg void OnProt();
    afx_msg void OnProtNew();
    afx_msg void OnProtNew2();
    afx_msg void OnVydRs();
    afx_msg void OnVydMs();

    afx_msg void OnVydRs2();
    afx_msg void OnVydMs2();


    afx_msg void OnVydMag();
    afx_msg void OnVydUchOpr();

    afx_msg void OnGeoPs();
    afx_msg void OnResetTG();
    afx_msg void OnProtMag();
    afx_msg void OnProtPassport();
    afx_msg void OnProtRs();
    afx_msg void OnProtKot();
    afx_msg void OnProtAll();
    afx_msg void OnPaspRS();
    afx_msg void OnPasp();
    afx_msg void OnPasp2();
    afx_msg void OnPaspKam();
    afx_msg void OnMapYes();
    afx_msg void OnUpdateMapYes(CCmdUI* pCmdUI);
    afx_msg void OnOwner();
    afx_msg void OnRS();
    afx_msg void OnMS();
    afx_msg void OnRemont();

    afx_msg void OnRemontWord1();
    afx_msg void OnRemontWord2();
    afx_msg void OnRemontWord3();
    afx_msg void OnRemontWord4();

    
    afx_msg void OnRemontView();
    afx_msg void OnRemontArhivView();
    afx_msg void OnPopupRemontClose();
    afx_msg void OnSaveMag();
    afx_msg void OnUpdateSaveMag(CCmdUI* pCmdUI);

    afx_msg void OnSaveMS();
    afx_msg void OnUpdateSaveMS(CCmdUI* pCmdUI);
    afx_msg void OnUpdateSaveRS(CCmdUI* pCmdUI);
    afx_msg void OnUpdatePasp(CCmdUI* pCmdUI);


    afx_msg void OnSaveUtPats();
    afx_msg void OnUpdateSaveUtPts(CCmdUI* pCmdUI);

    afx_msg void OnSaveRS();
    afx_msg void OnMagNo();
    afx_msg void OnMsNo();
    afx_msg void OnRsNo();
    afx_msg void OnMakeRaspr();
    afx_msg void OnCheckPO();
    afx_msg void OnPopupNagr();
    afx_msg void OnPopupNagr2();
    afx_msg void OnSetAddr();
    afx_msg void OnPopupAddZd();
    afx_msg void OnPopupAddObj();
    afx_msg void OnNagrZd();
    afx_msg void OnNagrOrg();
    afx_msg void OnNagrZdNeiz();
    afx_msg void OnNagrOrgNeiz();
    afx_msg void OnTuTrub();
    afx_msg void OnTuExcel();
    afx_msg void OnTuSvod();
    afx_msg void OnTuZhurnal();
    afx_msg void OnTuTable();
    afx_msg void OnTuZav();
    afx_msg void OnTuTableNeiz();
    afx_msg void OnTuYear();
    afx_msg void OnTuIst();
    afx_msg void OnTuRay();
    afx_msg void OnTuKam();
    afx_msg void OnTuSost();
    afx_msg void OnTuIstO();
    afx_msg void OnPopupMagLineDel();
    afx_msg void OnProverka();

    afx_msg void OnKm();
    afx_msg void OnMetr();
    afx_msg void OnUpdateKm(CCmdUI* pCmdUI);
    afx_msg void OnUpdateMetr(CCmdUI* pCmdUI);
    afx_msg void OnUpdateGoogleMap(CCmdUI* pCmdUI);
    afx_msg void OnMasAllAll();
    afx_msg void OnPrisNagr();
    afx_msg void OnPrisNagrEdit();
    afx_msg void OnPrisNagrDel();
    afx_msg void OnFindTuIst();
    afx_msg void OnTuFind();
    afx_msg void OnTuLine();
    afx_msg void OnSetUchRs();
    afx_msg void OnSetUchMs();
    afx_msg void OnAlma();
    afx_msg void OnUpdateAlma(CCmdUI* pCmdUI);
    afx_msg void OnUpdateSetUchMs(CCmdUI* pCmdUI);
    afx_msg void OnPopupPas();
    afx_msg void OnPopupPasVV();
    afx_msg void OnFindTuDog();
    afx_msg void OnSavePNG();
    afx_msg void OnExportGidTxt();
    afx_msg void OnHttp2();
    afx_msg void OnRemontAllAll();
    afx_msg void OnRemontAll();
    afx_msg void OnRemont1();
    afx_msg void OnRemont2();
    afx_msg void OnRemont3();
    afx_msg void OnShurfAll();
    afx_msg void OnShurf1();
    afx_msg void OnShurf2();
    afx_msg void OnShurf3();
    afx_msg void OnKapitalAll();
    afx_msg void OnKapital1();
    afx_msg void OnKapital2();
    afx_msg void OnKapital3();

    afx_msg void OnOpressovkaAll();
    afx_msg void OnOpressovka1();
    afx_msg void OnOpressovka2();
    afx_msg void OnOpressovka3();


    afx_msg void OnKorrozAll();
    afx_msg void OnKorroz1();
    afx_msg void OnKorroz2();
    afx_msg void OnKorroz3();

    afx_msg void OnKorrozOtchet();
    afx_msg void OnRemontOtchet();
    afx_msg void OnVydKod0();
    afx_msg void OnPopupOtklPotr();

    afx_msg void OnElectroIst();
    afx_msg void OnElectroPriem();
    afx_msg void OnElectroLine();
    afx_msg void OnElectroMufta();
    afx_msg void OnElectroOpora();
    afx_msg void OnElectroGil();
    afx_msg void OnElectroKanal();
    afx_msg void OnElectroRemont();

    afx_msg void OnUpdateElectro(CCmdUI* pCmdUI);

    afx_msg void OnPotrUR();
    afx_msg void OnPotrTR();

    afx_msg void OnPotrOtkl();
    afx_msg void OnDopPodklNagr();

    afx_msg void OnPopupAddKapital();
    afx_msg void OnDialog3();
    afx_msg void OnDestroy();
    afx_msg void OnMsPassport();

    afx_msg void OnUpdateSetMSNew(CCmdUI* pCmdUI);
    afx_msg void OnUpdateSetRSNew(CCmdUI* pCmdUI);
    afx_msg void OnSetOpressovka();
    afx_msg void OnSaveOpressovka();
    afx_msg void OnOpressovkaPassport();

    afx_msg void OnUpdateSetOpressovka(CCmdUI* pCmdUI);
    afx_msg void OnUpdateSaveOpressovka(CCmdUI* pCmdUI);
    afx_msg void OnMagNode();
    afx_msg void OnTextOpr();

    afx_msg void OnOprNeopr();
    afx_msg void OnOdnolin();
    afx_msg void OnRaznolin();

    afx_msg void OnCtrlAstr();
    afx_msg void OnSetLineID();
    afx_msg void OnRasList();
    afx_msg void OnPopupNormTemp();
    afx_msg void OnPopupTg();
    afx_msg void OnPopupTg2();

    afx_msg void OnPopupPr();


    afx_msg void OnTubing1();
    afx_msg void OnTubing2();
    afx_msg void OnTubing3();
    afx_msg void OnTubing4();
    afx_msg void OnTubing5();
    afx_msg void OnFindKti();
    afx_msg void OnSetIst();

    afx_msg void OnRemontUchList();
    afx_msg void OnWear();

    afx_msg LRESULT OnAdoUpdated(WPARAM wParam, LPARAM);
    afx_msg LRESULT OnAdoNoUpdated(WPARAM wParam, LPARAM);
    afx_msg LRESULT OnShurfUtverditALL(WPARAM wParam, LPARAM);
 
    afx_msg void OnShurfTable();

    afx_msg void OnPovrezhd();
    afx_msg void OnPopupMeropr();
    afx_msg void OnSetPsMap();

    afx_msg void OnCalcs();
    afx_msg void OnFileNew();

    afx_msg void OnSqlSave();
    afx_msg void OnSqlRestore();

    afx_msg void OnQGvs();

    afx_msg void OnSqlBackup();
    afx_msg void OnSqlBackupRestore();
    afx_msg void OnSqlCopyFragment();
    afx_msg void OnSqlDelFragment();

    afx_msg void OnAddFragment();

    afx_msg void OnTableDan();
    afx_msg void OnDaniil();
    afx_msg void OnMagFragment();
    afx_msg void OnMapSearch();
    afx_msg LRESULT OnMapSearch2(WPARAM wParam, LPARAM);

    afx_msg void OnMasMarkAll();
    afx_msg void OnUpdateMasMarkAll(CCmdUI* pCmdUI);

    afx_msg void OnAstr();
    afx_msg void OnUpdateAstr(CCmdUI* pCmdUI);

    virtual void On1000();
    virtual void On1001();
    virtual void On1002();
    virtual void OnDelText();
    virtual void OnEditText();
    virtual void OnFontText();
    virtual void OnCreateSortNode();
    LRESULT OnOpresUpdated(WPARAM wParam, LPARAM);
    LRESULT OnOpresEditUpdated(WPARAM wParam, LPARAM);

    afx_msg void OnRemontsAddDefect();
    afx_msg void OnRemontsAddRemont();
    afx_msg void OnRemontsAddDiag();
    afx_msg void OnRemontsAddOpres();

    afx_msg void OnGeoOnOff();
    afx_msg void OnRemontOnOff();
    afx_msg void OnPassportOnOff();
    afx_msg void OnElectroOnOff();
    afx_msg void OnKorroziaOnOff();
    afx_msg void OnAktOnOff();
    
    afx_msg void OnUpdateKorroziaOnOff(CCmdUI* pCmdUI);
    afx_msg void OnUpdateAktOnOff(CCmdUI* pCmdUI);

    afx_msg void OnRemontsAdd();
    afx_msg void OnRemontsInfo();
    afx_msg void OnRemontsDel();
    afx_msg void OnRemontsTable();
    afx_msg void OnRemontsZhurnal();
    afx_msg void OnPtsAdd();
    afx_msg void OnPtsInfo();
    afx_msg void OnPtsDel();
    afx_msg void OnPtsTable();
    afx_msg void OnPtsZhurnal();
    afx_msg void OnKorrozAdd();
    afx_msg void OnKorrozInfo();
    afx_msg void OnAktInfo();
    afx_msg void OnAktTable();
    afx_msg void OnKorrozDel();
    afx_msg void OnKorrozTable();
    afx_msg void OnAktDel();
//  afx_msg void OnKorrozTableGod();
    afx_msg void OnKorrozZhurnal();
    afx_msg void OnElectroAdd();
    afx_msg void OnElectroInfo();
    afx_msg void OnElectroDel();
    afx_msg void OnElectroTable();
    afx_msg void OnElectroZhurnal();

    afx_msg void OnZhurnalDefect();
    afx_msg void OnZhurnalDiag();
    afx_msg void OnZhurnalRemont();
    afx_msg void OnZhurnalElectro();
    afx_msg void OnZhurnalKorroz();
    afx_msg void OnZhurnalExcel();

    afx_msg void OnZhurnalDefectOpres();

    afx_msg void OnUpdateSaveCurentRemont2(CCmdUI* pCmdUI);
    afx_msg void OnUpdateSaveRemont2(CCmdUI* pCmdUI);

    afx_msg void OnCoord();
    afx_msg LRESULT OnCoordUpdated(WPARAM wParam, LPARAM);

    afx_msg void OnKorroziaZhurnal1();
    afx_msg void OnKorroziaZhurnal2();
    afx_msg void OnKorroziaZhurnal3();
    afx_msg void OnKorroziaZhurnal4();



    afx_msg void OnZhurnalKorroziaCurrent();
    afx_msg void OnZhurnalKorroziaAll();
    afx_msg void OnZhurnalKorroziaGod();
    afx_msg void OnKorroziaUpdate();
    afx_msg LRESULT OnKorroziaUpdate2(WPARAM wParam, LPARAM lParam);

    afx_msg void OnUpdateKorroziaCurrent(CCmdUI* pCmdUI);
    afx_msg void OnUpdateKorroziaAll(CCmdUI* pCmdUI);
    afx_msg void OnPopupKorroziaPlan();
    afx_msg void OnSetKorrozia();
    afx_msg void OnUpdateSetKorrozia(CCmdUI* pCmdUI);

    afx_msg void OnKorrozDoc1();
    afx_msg void OnKorrozDoc2();
    afx_msg void OnKorrozDoc3();
    afx_msg void OnKorrozDoc4();
    afx_msg void OnKorrozDoc5();

    afx_msg void OnExportPTS();
    afx_msg void OnImportPTS();


    afx_msg void OnExportFragment();
    afx_msg void OnExportFragmentVyd();
    afx_msg void OnImportFragment();
    afx_msg void OnImportFragmentMDB();
    afx_msg void OnImportFragmentMag();
    afx_msg void OnExportFragmentsMulty();

    afx_msg void OnSezon();
    afx_msg void OnBmpFind();
    afx_msg void OnUpdateBmpFind(CCmdUI* pCmdUI);

    afx_msg void OnUtZakr();
    afx_msg void OnUtZakrAll();

    afx_msg void OnNachalnik();
    afx_msg void OnSetOsvid();
    afx_msg void OnListOsvid();

    afx_msg void OnTest();
    afx_msg void OnOsmotrZapros1();
    afx_msg void OnOsmotrZapros2();
    afx_msg void OnOsmotrZapros3();
    afx_msg void OnOsmotrZapros4();
    afx_msg void OnOsmotrZapros5();
    afx_msg void OnOsmotrAkt();

    afx_msg void OnExportCxe();
    afx_msg void OnImportCxe();


    afx_msg void OnSetupProxy();


afx_msg void OnRemontPovrShurfAdd();
afx_msg void OnRemontPovrShurfAddNeplan();
afx_msg void OnRemontPovrShurfDel();
afx_msg void OnRemontPovrShurfMove();
afx_msg void OnRemontPovrShurfInfo();


afx_msg void OnRemontPovrDefPanel();

afx_msg void OnRemontPovrDefAdd();
afx_msg void OnRemontPovrDefDel();
afx_msg void OnRemontPovrDefMove();
afx_msg void OnRemontPovrDefInfo();
afx_msg void OnRemontPovrKarta();
afx_msg void OnRemontPovrKarta2();
afx_msg void OnRemontPovrOtop();
afx_msg void OnRemontPovrOtklPovr();
afx_msg void OnRemontPovrDefOtobr();
afx_msg void OnRemontShurfOtobr();

afx_msg void OnRemontPovrNach();
afx_msg void OnRemontPovrRs();
afx_msg void OnRemontPovrMs();
afx_msg void OnRemontPovrAll();

afx_msg void OnRemontPovrDefFind();
afx_msg void OnRemontPovrDefTable();
/*
afx_msg void OnRemontPovrDefZhur();
afx_msg void OnRemontPovrDefZhur1();
afx_msg void OnRemontPovrDefZhur2();
afx_msg void OnRemontPovrDefZhur3();
afx_msg void OnRemontPovrDefZhurRemont();
*/

afx_msg void OnRemontPovr_1();

afx_msg void OnRemontPovrPovr();
afx_msg void OnRemontPovrRemont();
afx_msg void OnRemontPovrPosleRemont();
afx_msg void OnRemontPovrRemontAll();

afx_msg void OnRemontPovr_2();

afx_msg void OnRemontPovrRemontTrub();
afx_msg void OnRemontPovrRazmZapl();
afx_msg void OnRemontPovrDlinaZamTrub();
afx_msg void OnRemontPovrRemIzol();
afx_msg void OnRemontPovrRemKanal();
afx_msg void OnRemontPovrRemAll();

afx_msg void OnRemontPovr_3();

afx_msg void OnRemontPovrDefEkspl();
afx_msg void OnRemontPovrDefOpr();
afx_msg void OnRemontPovrDefOsv();
afx_msg void OnRemontPovrDefShurfPlan();


afx_msg void OnRemontPovrDefAnalizTip();
afx_msg void OnRemontPovrDefAnalizPrichina();
afx_msg void OnRemontPovrDefAnalizElement();
afx_msg void OnRemontPovrDefAnalizTipProkl();
afx_msg void OnRemontPovrDefAnalizItd();
afx_msg void OnRemontPovrDefAnalizAll();
afx_msg void OnRemontPovrTeploSet();



afx_msg void OnRemontOsmotrPanel();
afx_msg void OnRemontOsmotrTeploSet();
afx_msg void OnRemontOsmotrSezon();
afx_msg void OnRemontOsmotrTable();
afx_msg void OnRemontOsmotrZhurnalOsmotr();
afx_msg void OnRemontOsmotrZhurnalPovr();
afx_msg void OnRemontOsmotrRegistr();
afx_msg void OnRemontOsmotrAkt();
afx_msg void OnRemontOsmotrDocument();
afx_msg void OnRemontOsmotrAdd();
afx_msg void OnRemontOsmotrDefAdd();
afx_msg void OnRemontOsmotrSaveOsmotr();
afx_msg void OnRemontOsmotrDefDel();
afx_msg void OnRemontOsmotrDefMove();
afx_msg void OnRemontOsmotrDefInfo();
afx_msg void OnRemontShurfPlan();
afx_msg void OnRemontShurfPredpis();
afx_msg void OnRemontShurfNarush();

afx_msg void OnShurfProcess();
afx_msg void OnShurfVypolneno();



afx_msg void OnRemontShurfPlanUtverdit();
afx_msg void OnRemontShurfPlanExcel();
afx_msg void OnRemontShurfPlanExcelMonth();
afx_msg void OnRemontShurfZhurnal();


afx_msg void OnDefectZhurnalExpluatacia    ();
afx_msg void OnDefectZhurnalShurf          ();
afx_msg void OnDefectZhurnalOsmotr         ();
afx_msg void OnDefectZhurnalOpress         ();
afx_msg void OnDefectZhurnalNarushenie     ();
afx_msg void OnDefectZhurnalRemont         ();
afx_msg void OnDefectZhurnalPosleremont    ();
afx_msg void OnDefectZhurnalAll            ();

afx_msg void OnDefectDocTable();

afx_msg void OnDefectWord1();
afx_msg void OnDefectWord2();

afx_msg void OnOsmotrAdd();

afx_msg void OnDefectAnaliz();

afx_msg void OnOpresAddPlan();

afx_msg void OnOpresTeploSet();

afx_msg void OnRemontAddPlan     ();
afx_msg void OnRemontAddCurrent  ();
//afx_msg void OnListRemont2       ();
//afx_msg void OnLastRemont2       ();
afx_msg void OnRemontTrub        ();
afx_msg void OnRemontPlan        ();
afx_msg void OnRemontCurrent     ();
afx_msg void OnRemontProcess     ();
afx_msg void OnRemontVypolneno   ();
afx_msg void OnRemontAll2        ();
afx_msg void OnRemontUtverdit    ();
afx_msg void OnOpresUtverdit    ();
//afx_msg void OnOpressovkaPassport();
afx_msg void OnRemontAnaliz2     ();
afx_msg void OnRemontAnaliz      ();


afx_msg void OnUpdateOsmotrSave(CCmdUI* pCmdUI);
afx_msg void OnUpdateOpresSave(CCmdUI* pCmdUI);
afx_msg void OnUpdateRemontSave(CCmdUI* pCmdUI);


afx_msg void OnRemontVyborNach();
afx_msg void OnRemontPlanUtverdit();

afx_msg void OnNoPts();

afx_msg void OnOpresZadvAdd();
afx_msg void OnCloseOpresZd();
afx_msg void OnOpresZdAll();
afx_msg void OnPopupOpenCloseStateId();


afx_msg void OnOpresPlan();
afx_msg void OnOpresProcess();
afx_msg void OnOpresVypolneno ();
afx_msg void OnOpresAll2();
afx_msg void OnOpresWord1();
afx_msg void OnOpresWord2();
afx_msg void OnOpresAnaliz2();
afx_msg void OnOpresAnaliz();

afx_msg void OnNodeZd();
afx_msg void OnMakeUtPTS();
afx_msg void OnRebuildIndexes();
afx_msg void OnGoogleElevationConfig();
afx_msg void OnOSMInfo();

afx_msg void OnDefectBezUch();
afx_msg void OnShurfBezUch();
afx_msg void OnSetMinPerepad();

afx_msg void OnAllPlanRemont2();
afx_msg void OnAllPlanShurfy();

afx_msg void OnInitSezon();
afx_msg void OnRemontIspr();
afx_msg void OnIndexes();

afx_msg void OnDelMark();
afx_msg void OnZdanieNagruzki();
afx_msg void OnNagrAlmaty();
afx_msg void OnNagrAlmatyTable();
afx_msg void OnNagrAlmaty2();
afx_msg void OnNagrAlmatyOnOff();
afx_msg void OnUpdateNagrAlmatyOnOff(CCmdUI* pCmdUI);
afx_msg void OnNagrAlmaXlsx();
afx_msg void OnNagrAlmaNenaid1();
afx_msg void OnNagrAlmaNenaid2();
afx_msg void OnNagrAlmaNenaid3();
afx_msg void OnSqlQuery();
afx_msg void OnNagrAlmatyAdd1();
afx_msg void OnNagrAlmatyAdd2();
afx_msg void OnDomaAlseco();
afx_msg void OnGeoAlseco1();
afx_msg void OnGeoAlseco2();

afx_msg void OnAddrConfig();

afx_msg void OnPtVydKv();
afx_msg void OnPtVydUr();
afx_msg void OnPtVydTr();


afx_msg void OnUpdateAlseco(CCmdUI* pCmdUI);
afx_msg void OnUpdateElevation2(CCmdUI* pCmdUI);

afx_msg LRESULT OnAlseco1Updated(WPARAM wParam, LPARAM);
afx_msg LRESULT OnAlseco2Updated(WPARAM wParam, LPARAM);

afx_msg void OnSetFocus(CWnd* pOldWnd);

afx_msg void OnEndPassport();


    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in gidrView.cpp
inline Cgid6Doc* CGidrView::GetDocument()
{
    return (Cgid6Doc*)m_pDocument;
}
#endif



CGidrView *getView();

//int RunTableNiz(CWnd* wnd, CAdoFile* ado, const TCHAR* tn, const TCHAR* q, const TCHAR* title, bool refresh, bool is_head, VIRT_DATA typ = virt_data_geo);
//int RunTableNiz(CWnd* wnd, CAdoFile* ado, const TCHAR* tn, const TCHAR* q, const TCHAR* title, bool refresh, bool is_head, VIRT_DATA typ);


