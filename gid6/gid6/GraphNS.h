#if !defined(AFX_GRAPH_H__4CA02E81_A2FD_11D6_983B_B3EA5BA9D621__INCLUDED_)
#define AFX_GRAPH_H__4CA02E81_A2FD_11D6_983B_B3EA5BA9D621__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Graph.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGraphNs dialog

//#include "..\\CoolControls\\CoolButton.h"
#include "graphtemplate.h"

class CGraphNs : public CGraphTemplate
{
// Construction
public:
  CGraphNs(GRAPH_PARAM  * graph_par,CWnd* pParent = NULL);   // standard constructor
  APPROX_PARAM apH,apN,apKPD;
  
// Dialog Data
  //{{AFX_DATA(CGraphNs)
  enum { IDD = IDD_GRAPH };
  CButton m_OK;
    // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CGraphNs)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  void Draw(CDC &dc);
  CString GetCaptionStrt();
  // Generated message map functions
  //{{AFX_MSG(CGraphNs)
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPH_H__4CA02E81_A2FD_11D6_983B_B3EA5BA9D621__INCLUDED_)
