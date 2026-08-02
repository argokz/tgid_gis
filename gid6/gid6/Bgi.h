#if !defined(AFX_BGI_H__D35DA322_199F_11D3_BAFC_444553540001__INCLUDED_)
#define AFX_BGI_H__D35DA322_199F_11D3_BAFC_444553540001__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Bgi.h : header file
//


#if !defined(__COLORS)
#define __COLORS

#define DPIPR 600

void log(const char *podp, const char *s);


enum COLORS {
    BLACK        = 0x000000,          /* dark colors */
    BLUE         = 0x7F0000,
    GREEN        = 0x007F00,
    CYAN         = 0x7F7F00,
    RED          = 0x00007F,
    MAGENTA      = 0x7F007F,
    BROWN        = 0x007F7F,
    LIGHTGRAY    = 0xBFBFBF,
    DARKGRAY     = 0x7F7F7F,           /* light colors */
    LIGHTBLUE    = 0xFF0000,
    LIGHTGREEN   = 0x00FF00,
    LIGHTCYAN    = 0xFFFF00,
    LIGHTRED     = 0x0000FF,
    LIGHTMAGENTA = 0xFF00FF,
    YELLOW       = 0x00FFFF,
    WHITE        = 0xFFFFFF
};
#endif

/////////////////////////////////////////////////////////////////////////////
// CBgi view



class Percent {
  public:
    Percent();
    ~Percent();
    void SetValue(int p);
  private:
    int pp;
    CProgressCtrl *indic;
};



class CBgi : public CView
{
protected:
//  CBgi();           // protected constructor used by dynamic creation
  DECLARE_DYNCREATE(CBgi)

// Attributes
public:
  CBgi();           // protected constructor used by dynamic creation
  virtual ~CBgi();

// Operations
public:

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CBgi)
  protected:
  virtual void OnDraw(CDC* pDC);      // overridden to draw this view
  virtual void OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView);
  //}}AFX_VIRTUAL

// Implementation
protected:
//  virtual ~CBgi();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

  // Generated message map functions
public:
  void SetDC(CDC *DC);
  void SetRect(CRect rect);
  CDC *BeginPaint();
  CDC *BeginMemoPaint(CDC *dc,int dx, int dy);
  void EndPaint(CDC *);
  void EndPaint2(CDC *);

  void Mxy(int x, int y);
//  void EvTimer(uint timerId);


  int getcolor(CDC*) 
  { 
    return color; 
  }
  void setcolor(CDC*,int c);
  void setcolor(CDC*,int c, int width);
  void setline(CDC*,int c, int l);
  void setline(CDC*,int c, double l);
  void setline(CDC*,int c, double l, int style);
  void setfillstyle(CDC*,int typ, int c);
  static void fillpoly(CDC*,int n, int *fil);
  static void fillpoly(CDC *m_dc, int n, CPoint *p); 
  static void putpixel(CDC*,int x, int y, int col);
  static void fillellipse(CDC*,int x, int y, int rx, int ry);
  static void ellipse(CDC*,int x, int y, int rx, int ry);
  static void drawpoly(CDC*,int n, int *fil);
  static void line(CDC*,int x1, int y1, int x2, int y2);
  static void line(CDC *m_dc, int x1, int y1, int x2, int y2, CRect rect);
  static void lineto(CDC*,int x, int y);
  static void moveto(CDC*,int x, int y);
  static void lineto(CDC*,POINT p);
  static void moveto(CDC*,POINT p);
  static void rectangle(CDC*,int x1, int y1, int x2, int y2);
  static void fillrectangle(CDC*,int x1, int y1, int x2, int y2);
  static void xrectangle(CDC*,int x1, int y1, int x2, int y2);
  static void xline(CDC*,int x1, int y1, int x2, int y2);
  static void xlineto(CDC*,int x, int y);
  static void tline(CDC*,int x1, int y1, int x2, int y2, int type);
  static void line(CDC*,double x1, double y1, double x2, double y2, int type);
  void typl(CDC*,int type, int &n, int &len, int &mlen, signed char *tp, signed char *xx, signed char *yy);

  void hline(CDC*,double x1, double y1, double x2, double y2);
  void lineR(CDC*,CPoint p1, CPoint p2);


  void pr(CDC *m_dc, int x0, int y0, const char *txt);
  void pr(CDC *m_dc, int x0, int y0, int x2, int y2, const char *txt);
  void word_size(CDC *m_dc, long x0, long y0, long x2, long y2, const char *txt, long &xmin, long &ymin, long &xmax, long &ymax);
  void xpr(CDC *m_dc, int x1, int y1, int x2, int y2, const char *s);

  void dline(CDC *m_dc, double x1, double y1, double x2, double y2);

  void drawLinePic(CDC *m_dc, int thickness, double x1, double y1, double x2, double y2, int color, int colorF, int bk_color, const int *pics, double coef, bool no_mas);



  BOOL MySetCursor(CWinApp *module, LPCTSTR resId);

  void MyTextOut(CDC *m_dc, int x, int y, const char *s);

//  CDC *m_dc;
//  CClientDC *m_pDC;
  CRect m_rect;
  double m_h;
  double m_ring = 1.;

  CPen *pen, *oldpen;
  CBrush *brush, *oldbrush;
  CFont *font, *oldfont;

protected:

  CWinApp   *CursorModule;
  LPCTSTR    CursorResId;
  HCURSOR    HCursor;
  Percent *m_pc;

  int dpi;
  int color;
  int is_paint;

  CWaitCursor *m_wait;


  //{{AFX_MSG(CBgi)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnDestroy();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BGI_H__D35DA322_199F_11D3_BAFC_444553540001__INCLUDED_)
