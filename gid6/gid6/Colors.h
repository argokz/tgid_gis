#if !defined(AFX_COLORS_H__6BEA3D7C_D7B8_4EA9_85C7_DEB42B4FAFE7__INCLUDED_)
#define AFX_COLORS_H__6BEA3D7C_D7B8_4EA9_85C7_DEB42B4FAFE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Colors.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColors dialog

#include "colorsv.h"

class CColors : public CDialog
{
// Construction
public:
  CColors(CWnd* pParent, GID_COLORS *colors);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CColors)
  enum { IDD = IDD_COLORS };
  CColorsWindow m_w;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CColors)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  //}}AFX_VIRTUAL

// Implementation
protected:
   GID_COLORS *m_colors;
   CColorsWindow *w;

  // Generated message map functions
  //{{AFX_MSG(CColors)
  afx_msg void OnDefault();
  virtual BOOL OnInitDialog();
	afx_msg void OnFont();
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORS_H__6BEA3D7C_D7B8_4EA9_85C7_DEB42B4FAFE7__INCLUDED_)
