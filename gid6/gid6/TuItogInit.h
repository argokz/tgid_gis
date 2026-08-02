#if !defined(AFX_TUITOGINIT_H__FE2F6928_4392_465C_93E4_F1AD660A4257__INCLUDED_)
#define AFX_TUITOGINIT_H__FE2F6928_4392_465C_93E4_F1AD660A4257__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TuItogInit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTuItogInit dialog

class CTuItogInit : public CDialog
{
// Construction
public:
  CTuItogInit(int y, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CTuItogInit)
  enum { IDD = IDD_TU_ITOG };
  CSpinButtonCtrl m_s3;
  CSpinButtonCtrl m_s2;
  CSpinButtonCtrl m_s1;
  int   m_y1;
  int   m_y2;
  int   m_y_ist;
  double m_coef24;
  COleDateTime  m_date;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CTuItogInit)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CTuItogInit)
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TUITOGINIT_H__FE2F6928_4392_465C_93E4_F1AD660A4257__INCLUDED_)
