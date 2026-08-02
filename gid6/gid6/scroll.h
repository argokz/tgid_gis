#if !defined(AFX_SCROLL_H__FE364683_1BA5_11D3_BAFC_444553540001__INCLUDED_)
#define AFX_SCROLL_H__FE364683_1BA5_11D3_BAFC_444553540001__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Scroll.h : header file
//

#include "Bgi.h"

#pragma warning(disable:4786)
#include <vector>
#include <list>
using namespace std;

#include "coordlis.h"

class Klassif;

extern DEVMODE *m_devmode;
extern double m_MasPodp;

struct ScrollGeom
{
    double m_alpha = 0;
    double m_bx = 0, m_by = 0;
    double dmas = 1;
    double masx = 1, masy = 1;
};


/////////////////////////////////////////////////////////////////////////////
// CScroll view

enum REGIM {
  R_INIT,
  R_BEGIN_RECT,
  R_BEGIN_DRAW1,
  R_BEGIN_DRAW2,
  R_BEGIN_DRAW_COPY_BMP1,
  R_BEGIN_DRAW_COPY_BMP2,
  R_BEGIN_MOVE_VIEW,
  R_MOVE_VIEW,
  R_MOVE_VIEW_MIDDLE,
  R_MOVE_VIEW_VYD,
  R_BEGIN_DRAW_COPY1,
  R_BEGIN_DRAW_COPY2,
  R_BEGIN_MOVE_LINE,
  R_BEGIN_MOVE_POINT1,
  R_BEGIN_MOVE_POINT6,
  R_BEGIN_MOVE_POINT6_2,
  R_MOVE_LINE,
  R_MOVE_PODP,
  R_BEGIN_LEN1,
  R_BEGIN_LEN2,

  R_BEGIN_MOVE_PODP,


  R_BEGIN_LEN_OKR1,
  R_BEGIN_LEN_OKR2,
  R_BEGIN_LEN_OKR3,
  R_BEGIN_LEN_OKR4,

  R_BEGIN_LEN_OKR_VYD_1,
  R_BEGIN_LEN_OKR_VYD_2,

  R_BEGIN_LEN_OKR_VYD_GEO_1,
  R_BEGIN_LEN_OKR_VYD_GEO_2,


  R_BEGIN_MOVE_FRAGMENT_1,
  R_BEGIN_MOVE_FRAGMENT_2,
  R_FRAGM,

  R_BEGIN_DRAW_LINE,
  R_BEGIN_NODE_NEOTR,

  R_BEGIN_DRAW_TYPE,

  R_BEGIN_NEW_UCH2,

  R_BEGIN_MOVE_VYD,

  R_BEGIN_POVOROT_1,
  R_BEGIN_POVOROT_2,
  R_BEGIN_POVOROT_3,

  R_BEGIN_RECT_1,

  R_COPY_AKT,
  R_CITY,
  R_BEGIN_DRAW_TEXT1,
  R_BEGIN_DRAW_TEXT2,
  R_PTS_DEL,
  R_NODE_PICK,
  R_NODE_PICK1,
  R_NODE_PICK2,
  R_ALSECO_PICK,
  R_NODE_ADD


};


class CScroll : public CBgi
{
protected:
//  CScroll();           // protected constructor used by dynamic creation
  DECLARE_DYNCREATE(CScroll)

// Attributes
public:
  CScroll();           // protected constructor used by dynamic creation
  virtual ~CScroll();

// Operations
public:
  bool moveXY(const CFPoint& pt);
  bool moveXYmas(const CFPoint& pt);

  void DrawPolygon2(CDC *m_dc, const CCoordList &cl, int loc);
  void DrawPolygon3(CDC *m_dc, const CCoordList &cl);
  void DrawPolygon(CDC *m_dc, const CCoordList& cl, int loc, int typ);
  void DrawPolygonAdd(const CCoordList& cl, int loc);

  void Draw1(CDC *m_dc, CCoordList &cl, bool isMark, bool redr, int col, int kls_typ, bool no_mas);
  void Draw6(CDC *m_dc, CCoordList &cl, bool isMark, bool redr, int col, int kls_typ, double mas_otn, bool no_mas);
  void Draw23(CDC *m_dc, CCoordList &cl, int loc, bool isMark, bool redr, int col, int colb, int kls_typ, int kls_typ2);
  void Draw11(CDC *m_dc, CPoint pt, bool isMark, int col, int col2, int kls_typ, int po);
  void draw_tri(CDC *m_dc, CPoint pt, double r, double h, int col, double mas_otn);
  void draw_defect(CDC *m_dc, CPoint pt, double r, double h, int col, int col2, double mas_otn);
  void draw_quad(CDC *m_dc, CPoint pt, double r, double h, int col, double mas_otn);
  void draw_shurf(CDC *m_dc, CPoint pt, int col, int col2, double mas_otn);

  void DrawLabel3(CDC *m_dc, const CCoordList& cl, const char *text, double x, double y, double alpha, double coef, int loc, const Klassif *m_kls);
  void DrawLabel3_MI(CDC *m_dc, const CCoordList& cl, const char *text, double x, double y, double alpha, double coef, int loc, const Klassif *m_kls);
  void DrawLabel2(CDC *m_dc, CPoint pt, const char *text, short align, double x, double y, double alpha, double coef, int loc, const Klassif *m_kls);
  void DrawLabel2(CDC *m_dc, const CCoordList& cl, const char *text, short align, double x, double y, double alpha, double coef, const Klassif *m_kls);
  void DrawLabel22(CDC *m_dc, const CCoordList& cl, const char *text, short align, double x, double y, double alpha, double coef, const Klassif *m_kls);
  void DrawLabel2(CDC *m_dc, const CCoordList& cl, const char *text);


  void DrawLabelMulty(CDC *m_dc, const CCoordList& cl, const char *text, int v);
  void DrawLabelMulty1(CDC *m_dc, const CCoordList& cl, const char *text, int v);
  void DrawLabelMulty2(CDC *m_dc, CFPoint p1, CFPoint p2, const char *text, int v);

  void DrawLabelMultyZoom(CDC *m_dc, CFPoint p1, CFPoint p2, const char *text, int vv);

  
  void put_ras(CDC *m_dc, int x, int y, int typ, int col);


  CFRect ScreenToCoord(const CRect& rect);
  CRect CoordToScreen(const CFRect& rect);

  CFPoint CoordToScreenF(const CFPoint& point);
  CFPoint CoordToScreenF1(const CFPoint& point);
  
  CFPoint ScreenToCoord(const CPoint& point);
  CFPoint ScreenToCoordOb(const CPoint& point);
  CPoint CoordToScreen(const CFPoint& point);

  void ChMas2(CFRect rect);
  void SetMas2(double m, double bx0, double by0);
  void moveXY2(double bx0, double by0);

  void drawNodeNull(CDC*m_dc, double x, double y, int color);
  void drawNodeTyp(CDC *m_dc, double x, double y, int typ, int color, double markerPx = -1.0);
  void drawLineTyp(CDC *m_dc, double thickness, double x1, double y1, double x2, double y2, int typ, int color, int colorF, BOOL ris, BOOL napr, BOOL strelka, BOOL isQ, bool no_map);

  void drawLine(CDC *m_dc, double x1, double y1, double x2, double y2, int typ);

  void setTypeLine(int typ) { m_type_line = typ; }

  CFRect m_rectBorder;

  void Tic(bool tic) { m_bIsTic = tic; }


  void MoveRect(CFRect rect);
  BOOL isColor() {return m_isColor;};



// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CScroll)
  protected:
  virtual void OnDraw(CDC* pDC);      // overridden to draw this view
  virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
  virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
  virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
  virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
  //}}AFX_VIRTUAL

// Implementation
protected:
//  virtual ~CScroll();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

public:
    int regim;

protected:
    CCoordList m_NP;
    int m_locNP;
    bool m_noend;  // Не двигать конечные точки

    BOOL m_bScrollBar;
    int oldregim;

    int m_dX;

    int m_tecF;
    int m_nRegim;

    double m_otn;
    double m_coef;
    double m_xxx;
    double m_yyy;

    int m_type_line;

    bool m_off = false;

    BOOL m_is_scroll;
    BOOL m_is_print_fr;

    CSize m_print_size;
    int m_print_mas;

    CFPoint m_pt1;
    CFPoint m_pt2;

    CFPoint m_move_pt1;
    CFPoint m_move_pt2;

    CFPoint m_pt3;
    CFPoint m_pt1_2;
    CFPoint m_pt_ob;
    bool m_is_ob;
    CDC *mdc;
    CDC *m_dcSave;

    CRect m_rectMargin;
    CRect m_rectAllMargin;
    CFRect m_rectPrint;
    BOOL m_bIsNaprGid;
    BOOL m_bIsTic;
    BOOL m_bIsFragment;
    BOOL m_isRamka;
    double m_dPrintMas;
    int m_npx, m_npy;
//    DEVMODE *m_devmode;

    int m_isSavedImage;
    CDC *m_pDibDC;

    BOOL is_google_map;
    BOOL m_isColor;

    int m_dbl;
    bool m_is_xdraw;

    std::vector<CPoint> m_points;
    std::vector<int> m_pCounts;


public:
    COLORREF m_bk_color;
    int m_nTransp;
    int m_nTransp2;
    BOOL m_isPrint;

    unsigned long m_color_uzel;

    ScrollGeom geom;
    ScrollGeom geom_old;

    double mas_otn, mas_otn_geotext;
    double mas_otn0;
    double len_dot;

    CFPoint m_pt_alpha;
    LOGFONT m_lf;
    LOGFONT m_lf_mag;
    COLORREF m_color_mag;

    CString m_strText;


    CDC *SaveImage();
    void CloseImage(CDC *pDibDC);
    virtual void drawPaint(CDC *m_dc) {};

    void CopyBlockPng2(CDC &dc, double mas, double bx, double by, int dx, int dy, const char *fn, int xx, int yy, int zz);
    void CopyBlockPng4(CDC &dc, double masx, double masy, double bx, double by, int dx, int dy);

    virtual void set_coord_gid(double x0, double y0, double alpha, bool convert = true);


  // Generated message map functions
protected:
  void SetCur(int regim);
  void bord();
  void SetBar();
  void SetRange(long x1, long y1, long x2, long y2);
  void ScrollWindow (int dx, int dy);
  void MasPlus();
  void MasMinus();
  void MasAll();
  void SetMas(double m, double bx0, double by0);
  void toOb(const CFPoint& pt);
  void FindCoordU();
  void FindCoord();
  void FindCoordDeg();
  void CopyBlock(CFRect);
  void CopyBlockPng(CFRect);
  void CopyBlockPng(double mas, CFRect rect, const char *fn);
  void CopyBlockBMP(CFRect);
  void XDraw0(bool on);
  void XDraw(CDC*, bool on);
  void SetBorder(const CFRect& rect);
  void BorderRect(CDC *m_dc, CRect rect, CRect rect1);
//  bool ReadFrameHead();
  bool ReadFrameHead(const char *fN = NULL);
  void PrintFrame(CDC* pDC);

  CDC * BeginMoveW(CDC *m_dc);
  CDC * BeginMoveW(CDC *m_dc, CCoordList &cl);
  void EndMoveW(CDC *m_dc, CDC *mdc, int dx, int dy);
  void MoveW(CDC *m_dc, CDC *mdc, int dx, int dy);
  void MoveW(CDC *m_dc, CDC *mdc, int dx, int dy, CCoordList &cl);

  bool GetPrintSize(int &m_npx, int &m_npy, const char *fN = NULL);

  void find_point2(CFPoint pt);
  void find_podp2(CFPoint pt);

  CDC *BeginAlpha(CDC *);
  void EndAlpha(CDC *, CDC *);

  CDC *BeginAlpha2(CDC *m_dc);
  void EndAlpha2(CDC *, CDC *);
  void DrawAlpha2(CDC *m_dc, CDC *pDibDCAlpha, int nTransp);


  virtual void redrawPaint(CDC *) {};
  virtual void redrawPaint2(CDC *) {};
  virtual void redrawNew(CDC *) {};
  void DrawLine(CDC *m_dc);
  void DrawPodp(CDC *m_dc);
  void DrawRect(CDC *m_dc, CFPoint pt, int w=5, int h=5);

  void x_podp(CDC *m_dc, int x1, int y1, int x2, int y2, double otn);
  void ChMas(CFRect rect);

//  bool isFull() { return /*mas_otn < 3;/**/ masx*dmas < m_MasPodp;/**/};
//  bool isFull() { return true; };
//  bool isFull() { return /*mas_otn < 3;/**/ masx*dmas < 5000;/**/};
  bool isFull();

  virtual bool find(const CFPoint &p) { return false; };
  virtual bool sfind(const CFPoint &p) { return false; };
  virtual bool findUP(const CFPoint &p) { return false; };
  virtual bool createObj() { return false; };
  virtual bool updateObj() { return false; };
  virtual bool mark_vyd() { return false; };
  virtual bool MoveFragm(double dx, double dy) { return false; };
//  virtual bool createText(CFPoint pt1, CFPoint pt2, const char *text) { return false; };

  void hideU(CDC *m_dc, const CCoordList &cl);
  void hideU(CDC *m_dc, CRect rect);

  virtual void InvalidateG(BOOL bErase = TRUE);
  virtual double mas1();
  virtual double mas2();
  virtual void setNavigator() {};

  CFPoint to90(CFPoint pt);


  int scroll_type;
  UINT m_nTimerDbl;
  UINT m_nFlagsDbl; 
  CPoint m_pointDbl;

  CCoordList m_clExpl;


  //{{AFX_MSG(CScroll)
  afx_msg void OnPrintFr();
  afx_msg void OnPlus();
  afx_msg void OnMinus();
  afx_msg void OnMasAll();
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnF5();
  afx_msg void OnMasshtab();
  afx_msg void OnMoveW();
  afx_msg void OnFindcoord();
  afx_msg void OnFindcoordDeg();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnViewScrollbar();
  afx_msg void OnUpdateViewScrollbar(CCmdUI* pCmdUI);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown2(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnEsc();
  afx_msg void OnDelete();
  afx_msg void OnPageSetup();
  afx_msg void OnEditCopy();
  afx_msg void OnEditCopyPng();
  afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
  afx_msg void OnFragment();
  afx_msg void OnEditCopybmp();
  afx_msg void OnF4();
  afx_msg void OnReturn();
  afx_msg void OnCtrlReturn();
  afx_msg void OnTransp();
  afx_msg void OnF2();
  afx_msg void OnPrintInit();
//  afx_msg void OnAstr();
//  afx_msg void OnUpdateAstr(CCmdUI* pCmdUI);
  afx_msg void OnRamka();
  afx_msg void OnUpdateRamka(CCmdUI* pCmdUI);
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

/*
  Преобразовать координаты из экрана в карту
*/

inline CFPoint CScroll::ScreenToCoord(const CPoint& point) {
  return CFPoint(geom.masx*(geom.m_bx+point.x), geom.masy*(geom.m_by+point.y));
}

/*
  Преобразовать координаты из экрана в карту
  с учетом точки
*/


inline CFPoint CScroll::ScreenToCoordOb(const CPoint& point) {
  CFPoint pt;

  if (m_is_ob) {
    m_is_ob = false;
    return m_pt_ob;
  }
  return ScreenToCoord(point);
}


inline CPoint CScroll::CoordToScreen(const CFPoint& point) {
  return CPoint((int)(point.x/ geom.masx - geom.m_bx), (int)(point.y/ geom.masy- geom.m_by));
}

inline CFPoint CScroll::CoordToScreenF(const CFPoint& point) {
  return CFPoint((int)(point.x/ geom.masx- geom.m_bx), (int)(point.y/ geom.masy-geom.m_by));
}

inline CFPoint CScroll::CoordToScreenF1(const CFPoint& point) {
  return CFPoint(point.x/ geom.masx- geom.m_bx, point.y/ geom.masy- geom.m_by);
}




class CTic {
public :
  CTic(CScroll *scr) {
    m_scr = scr;
    m_scr->Tic(true);
  }
  ~CTic() {
    m_scr->Tic(false);
  }
private :
  CScroll *m_scr;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCROLL_H__FE364683_1BA5_11D3_BAFC_444553540001__INCLUDED_)
