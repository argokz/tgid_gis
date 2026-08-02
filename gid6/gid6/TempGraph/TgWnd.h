#pragma once

//#include "graph_tempgraph2.h"
#include "ado.h"

struct GR_PARAM2
{
  double * x,*y;
  CPen * pen; 
  long kol;
  BOOL show;
};


class CTgWnd  : public CWnd
{
protected: // create from serialization only
  DECLARE_DYNCREATE(CTgWnd)

// Attributes
public:

  CTgWnd();
  void Copy();
  
  void init0(int id);
  void init();

  void SetLines(BOOL c1, BOOL c2, BOOL c3, BOOL c4, BOOL c5);

  int m_printPage;
  BOOL m_isPrinting;
  CRect m_drawRect;

//  CGraph_TempGraph2  m_grid;

protected:
  GR_PARAM2 array1[4];
  GR_PARAM2 arrayOT[4];
  CPen * DotPen;
  bool lButtonDown;
  BOOL showGraphOT;
  CClientDC * drawdc;
  int oldMode;
  CPoint oldp;
  HENHMETAFILE  mf;
  CFont* font;

  CRect bkgrnd;
  int llx,lly;
  short stepX, stepY;
    short nGridX, nGridY;
  short lX, lY, rX, rY;
    double minX, maxX, minY, maxY;
  double masX, masY;


  CString m_sezon, m_kod_ist;
  int m_kod, m_kod_tg;
  int m_id;

  BOOL  m_ch1;
  BOOL  m_ch2;
  BOOL  m_ch3;
  BOOL  m_ch4;

  BOOL  m_ch5;


  void Draw(CDC &dc);
  void SetMyFont(CDC &dc);
  void DrawEdge(CDC &dc);
  void DrawLine(CDC &dc, GR_PARAM2 &gr);
  void DrawLineDotted(CDC& dc, GR_PARAM2& gr);
  double FindMax( double *arr ,long kol);
  void FindMinMax(double &min,double &max, double *arr ,long kol);
  void MasSh(double  min, double max, short kol, double *mas );
  void MasShX(double  min, double max, short kol, double *mas );
  void MasShY(double  min, double max, short kol, double *mas );
  void InitArray();
  void InitArrayOT(CAdoFile* ado);
  void InitLine();
  void OnLine();
  void RubberBand(CPoint pos);


  public:
  protected:

  // Standard printing commands
  virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
  virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
  virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
  virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);

protected:
  // Generated message map functions
  //{{AFX_MSG(CTgWnd)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnPaint();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};