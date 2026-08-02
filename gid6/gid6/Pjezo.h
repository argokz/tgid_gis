#if !defined(AFX_PJEZO_H__E463CEB0_F408_4E3E_9E41_121AAAC11350__INCLUDED_)
#define AFX_PJEZO_H__E463CEB0_F408_4E3E_9E41_121AAAC11350__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Pjezo.h : header file
//

#include "scroll.h"
#include "cxema.h"
#include "Graph.h"
#include "Graph2.h"

#include "Okno.h"

class CPjezoTable;

/////////////////////////////////////////////////////////////////////////////
// CPjezo view

struct CPNode1 {
  CNode2 *n;
  double pihP1, pihO1;
  double pihP2, pihO2;
  double pP_fact, pO_fact;
  double pP_old, pO_old;

  double pihP1_old, pihO1_old;
  double pihP2_old, pihO2_old;

  double tP, tO;
  double h;
  double geodz;
  double hz;
  double len, lenO, vO, lenP, vP;

  CPNode1() : pihP1(0), pihO1(0), pihP2(0), pihO2(0), h(0), geodz(0), hz(0), n(NULL), pP_fact(0), pO_fact(0)
  {}
};

struct CPLine1 {
  CLine2 *l;
  double len;
  double diam;
  double r;
  double g; // Расход
  double w; // Скорость
  double a14;  //  Удельные линейные потери напора на участке
  double a15;  //  Линейные потери напора на участке
  double a16;  //  Местные потери напора на участке
  double a17;  //  потери напора на участке

  CPLine1() : len(0), diam(0), r(0), g(0), l(NULL)
  {}
};

struct CPNode {
    CPNode1 rn[2];
};

struct CPLine {
    CPLine1 rl[2];
};




class CPjezo : public CScroll
{
 
friend CPjezoTable;

protected:
  CPjezo();           // protected constructor used by dynamic creation
  DECLARE_DYNCREATE(CPjezo)

// Attributes
public:
  CCxema *m_cxema;
  CGraph2 *m_graph;
  CGraph2 *m_graph2;
//  CGidFile *m_gid_file;
  double m_len;
  double min_pih;
  double max_pih;
  int m_mas;
  int m_n;

  BOOL m_napor, m_otmet, m_vysot, m_okno, m_stat, m_real, m_dbl;
  BOOL m_podacha, m_obratka, m_fact;



// Operations
public:

  void initPjezo();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CPjezo)
  public:
  virtual void OnInitialUpdate();
  protected:
  virtual void OnDraw(CDC* pDC);      // overridden to draw this view
  virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
  virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
  afx_msg void OnListPjezo();
  //}}AFX_VIRTUAL

// Implementation
protected:
  virtual ~CPjezo();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif


  // Generated message map functions
protected:
  int nkx, nky, x1, y1, x2, y2, width, height, w, h;
  double h_max;
  CFont fontH, fontV;
  deque<CNode2> list_pjezo;
  BOOL m_bIsInfo[5];

  int m_calculationID;
  int m_fileID;


  COkno okno;

  vector<CPLine> m_vlineP;
  vector<CPLine> m_vlineO;
  vector<CPNode> m_vnode;
  
  double getPih(int i, int typ, bool first, int nras);
  void redrawPaint(CDC *);
  void drawGr(CDC *, int color, int typ, int nras);
  void drawSetka(CDC *);
  void drawOkno(CDC *);
  void House(CDC *, int x, int y, int h);
  void readUS_OUT(CGraph2 *m_graph, int num, int nras);
  void CopyBlockP(CFRect rect);

  //{{AFX_MSG(CPjezo)
  afx_msg void OnFont();
  afx_msg void OnPlus();
  afx_msg void OnMinus();
  afx_msg void OnMasall();
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnUpdateNapor(CCmdUI* pCmdUI);
  afx_msg void OnNapr();
  afx_msg void OnNapor();
  afx_msg void OnOtmet();
  afx_msg void OnUpdateOtmet(CCmdUI* pCmdUI);

  afx_msg void OnPodacha();
  afx_msg void OnUpdatePodacha(CCmdUI* pCmdUI);
  afx_msg void OnObratka();
  afx_msg void OnUpdateObratka(CCmdUI* pCmdUI);
  afx_msg void OnFact();
  afx_msg void OnUpdateFact(CCmdUI* pCmdUI);
  
  afx_msg void OnOkno();
  afx_msg void OnUpdateOkno(CCmdUI* pCmdUI);
  afx_msg void OnVysot();
  afx_msg void OnUpdateVysot(CCmdUI* pCmdUI);
  afx_msg void OnDouble();
  afx_msg void OnUpdateDouble(CCmdUI* pCmdUI);
  afx_msg void OnEditCopy();
  afx_msg void OnUpdateStat(CCmdUI* pCmdUI);
  afx_msg void OnStat();
  afx_msg void OnTechinfo();
  afx_msg void OnRealP();
  afx_msg void OnExcel();
  afx_msg void OnUpdateRealP(CCmdUI* pCmdUI);
  afx_msg void OnSavePjezo();
  afx_msg void OnSavePjezo2();
  afx_msg void OnMoveTo();
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
  afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnInf();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PJEZO_H__E463CEB0_F408_4E3E_9E41_121AAAC11350__INCLUDED_)
