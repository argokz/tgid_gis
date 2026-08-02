#if !defined(AFX_PRINTINI_H__DF063542_DC29_4C38_B8B0_A9827B7AEF9E__INCLUDED_)
#define AFX_PRINTINI_H__DF063542_DC29_4C38_B8B0_A9827B7AEF9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrintIni.h : header file
//

#include "coordlis.h"

/////////////////////////////////////////////////////////////////////////////
// CPrintIni dialog

class CPrintIni : public CDialog
{
// Construction
public:
  CPrintIni(CWnd* pParent, CFRect* rectPrint, CFRect *rectBorder, double* mas);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CPrintIni)
  enum { IDD = IDD_PRINT_INIT };
  CString m_strMas;
  double  m_x1;
  double  m_x2;
  double  m_y1;
  double  m_y2;
  //}}AFX_DATA

  CFRect *m_rectPrint;
  CFRect *m_rectBorder;
  double *m_mas;

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CPrintIni)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CPrintIni)
  virtual void OnOK();
  afx_msg void OnAll();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTINI_H__DF063542_DC29_4C38_B8B0_A9827B7AEF9E__INCLUDED_)
