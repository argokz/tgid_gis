#pragma once

/////////////////////////////////////////////////////////////////////////////
// CGraphR dialog
//#include "..\\CoolControls\\CoolButton.h"

#include "Graph.h"
#include "graphtemplate.h"

class CGraphR : public CGraphTemplate
{
// Construction
public:
  void Draw(CDC &dc);
  void DrawDiam(CDC &dc);
  CGraphR(GRAPHR_PARAM * Param,CWnd* pParent = NULL);   // standard constructor
  GRAPHR_PARAM GRP;
  double d_obt; // Диаметр обточки

// Dialog Data
  //{{AFX_DATA(CGraphR)
  enum { IDD = IDD_GRAPH_R };
  CButton m_CANCEL;
  CButton m_OK;
  CButton m_c_rep_but;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CGraphR)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void PostNcDestroy();
  //}}AFX_VIRTUAL

// Implementation
protected:
  double r0,r1,r2;
  CPen * dot_pen;

  char legX[5], legY[5];
  
  CPoint p, oldp, pmode;
  double qq2, hh2, //Точка на номинальной характеристике  
  del_p, //Полученная относительна величина обточки
  del_d;//Допустимая относительна величина обточки
  double kb;    // Коэффициент быстроходности
  double _rr2;  // Переменная аппроксимации (для пересчета диаметра обточки)
  
  int lButtonDown;// При нажатии левой кнопки мыши нужны они
  int fl,oldMode;// При нажатии левой кнопки мыши нужны они
  CClientDC * drawdc;// При нажатии левой кнопки мыши нужны они
  CPen * pen;// При нажатии левой кнопки мыши нужны они
  void RubberBand( CPoint pos,CDC* drawdc);// При нажатии левой кнопки мыши нужны они

  int frst;// При нажатии Правой кнопки мыши нужны они
  int rButtonDown;// При нажатии Правой кнопки мыши нужны они
  
  
  
  void prdim( CPoint p,CDC* drawdc);//для расчета диаметра обточенного колеса
  
  void CopyRaport(char *cbuf);//Заполнение текстового отчета
  
  CString GetCaptionStrt(); //Начало заголовка окна

  // Generated message map functions
  //{{AFX_MSG(CGraphR)
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRep();
  virtual void OnCancel();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


