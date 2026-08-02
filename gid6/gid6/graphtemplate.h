#if !defined(AFX_GRAPHTEMPLATE_H__98C5DCA2_77CE_4D83_A9A5_95D1BACCB1CF__INCLUDED_)
#define AFX_GRAPHTEMPLATE_H__98C5DCA2_77CE_4D83_A9A5_95D1BACCB1CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// graphtemplate.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGraphTemplate dialog
struct GRAPH_PARAM
{ 
  double H[10]; //Напор в точках рабочей характеристики
  double Q[10]; //Расход в точках рабочей характеристики
  double N[10]; //Мощность в точках рабочей характеристики
    double KPD[10]; //КПД в точках рабочей характеристики
  double Q_min; //Расход в точке начала рабочей зоны
  double Q_max; //Расход в точке конца рабочей зоны
  CString Tip; //Тип насоса 
  CString Name; //Название насоса 
  int k_kol;//Количество насосов
};

struct GRAPHR_PARAM
{
  GRAPH_PARAM gr; //Точки характерные для насосов
  double c_vr, //частота вращения заданная
    diam, //Диаметр заданный
    S; //Гидравлическое сопротивление
  BOOL Spr;//Признак TRUE - Гидравлическое сопротивление учитывать, FALSE - не учитывать
  double diam1,//Диаметр номинальный
    c_vr1;//частота вращения номинальная
  double Nmin, //номинальная минимальная мощность
    Nmax; //номинальная максимальная мощность
  BOOL IsDnom;//использовать диаметр номинальный в расчете
  BOOL IsFnom;//использовать частоту вращения номинальную в расчете
  //  int pr;
  //  int men;
  //int prop1,prop2;
};
enum TTypeChart{H_chart,N_chart,KPD_chart};
struct APPROX_PARAM
{
  double pog; //погрешность аппроксимации
  double r0; //коэффиниент аппроксимации r0
  double r1; //коэффиниент аппроксимации r1
  double r2; //коэффиниент аппроксимации r2
  void SetToNullApprox()
  {
  pog=0;
  r0=0;
  r1=0;
  r2=0;
  }
};



/////////////////////////////////////////////////////////////////////////////
// DEFINE SECTION
#define _HQ_color RGB(255,0,0)
#define _KPDQ_color RGB( 128,128,255 )
#define _NQ_color RGB( 0, 213,106 )

#define _HzadQ_color RGB(205,0,0)
#define _KPDzadQ_color RGB( 108,108,205 )
#define _NzadQ_color RGB( 0, 113,6 )

#define _setka_color RGB( 180, 180,180 )
#define _rab_zone_color RGB( 255, 133, 64 )
#define _basic_color RGB(0,0,0)

class CGraphTemplate : public CDialog
{
// Construction
public:
  CGraphTemplate(GRAPH_PARAM * GR,UINT IDD,CWnd* pParent = NULL);   // standard constructor
// Dialog Data
  //{{AFX_DATA(CGraphTemplate)
  CButton m_copy_to;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CGraphTemplate)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void PostNcDestroy();
  //}}AFX_VIRTUAL

// Implementation
protected:
  HENHMETAFILE  mf;
  CFont* font;

  CRect bkgrnd;
  int llx,lly;
  int gr_rap; // График (0) - отчет (1)
  short stepX, stepY;
    short nGridX, nGridY;
  short lX, lY, rX, rY;
    double minX, maxX, minY, maxY;
  double masX, masY;
  BOOL error_nasos;
///////////////////////внешние переменные///////////////////////
  GRAPH_PARAM DiamGR;
/////////////////////////////////////////////////////////////////////
  void DrawGraphLine( double * H, double * Q,
           CDC* dc, short col, short row );

  void SetMyFont(CDC &dc);
  virtual void Draw(CDC &dc);
  double FindMax( double *arr );
  void DrawGraph( double r0, double r1, double r2, CDC* dc, short col, short row );
  void MasSh(double  min, double max, short kol, double *mas );
  void Appr( double &r0,double &r1,double &r2,double *arr1, double *arr2, double* pog );
  void SetTitle();
  virtual void DrawChart(CDC &dc,TTypeChart type,APPROX_PARAM * apr_par,BOOL prepared=FALSE);
  virtual void DrawEdge(CDC &dc);
  virtual void ChahgeDrawParams(TTypeChart type,APPROX_PARAM * apr_par,CPen * &_pen);
  virtual void inihq( double *n, double *mas, short kol ); //масштабирование к кол-ву насосов


  
  virtual void CopyRaport(char *cbuf);
  
  virtual CString GetCaptionStrt();

  // Generated message map functions
  //{{AFX_MSG(CGraphTemplate)
  virtual BOOL OnInitDialog();
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg void OnPrint();
  afx_msg void OnCopy();
  afx_msg void OnTofile();  
  afx_msg void OnPaint();
  afx_msg void OnRep();
  afx_msg void OnCopyTo();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHTEMPLATE_H__98C5DCA2_77CE_4D83_A9A5_95D1BACCB1CF__INCLUDED_)
