#pragma once

/////////////////////////////////////////////////////////////////////////////
// CGraphApr dialog
#include "graphtemplate.h"

class CGraphApr : public CGraphTemplate
{
// Construction
public:
  CGraphApr(GRAPHR_PARAM * Param,CWnd* pParent = NULL);   // standard constructor
  GRAPHR_PARAM GrApr; 
  APPROX_PARAM apH,apHr,apN,apNr,apKPD,apKPDr;
// Dialog Data
  //{{AFX_DATA(CGraphApr)
  enum { IDD = IDD_GRAPH_APR };
  CButton m_real_check;
  CButton m_nom_check;
  CButton m_c_rep_but;
  CButton m_OK;
  CButton m_CANCEL;
  int   m_IsFnom;
  int   m_IsDnom;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CGraphApr)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  void Draw(CDC &dc);
  //Признаки Показывать ли графики
  BOOL Show_H_chart, 
    Show_Hr_chart,
    Show_N_chart,
    Show_Nr_chart,
    Show_KPD_chart,
    Show_KPDr_chart;
  //Признаки Показывать ли графики
  void ChahgeDrawParams(TTypeChart type,APPROX_PARAM * apr_par,CPen * &_pen);
  CString GetCaptionStrt();
  void CopyRaport(char *cbuf);
  void CmChangeVal();//попытка изменить привод или диаметр
  // Generated message map functions
  //{{AFX_MSG(CGraphApr)
  afx_msg void OnRep();
  afx_msg void OnNomCheck();
  afx_msg void OnRealCheck();
  virtual BOOL OnInitDialog();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};
