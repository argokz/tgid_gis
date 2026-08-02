#if !defined(AFX_MYPAGE_H__4A8C26B5_ADB9_4729_BB1C_05EADB2FA434__INCLUDED_)
#define AFX_MYPAGE_H__4A8C26B5_ADB9_4729_BB1C_05EADB2FA434__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MYPAGE.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CMyPage dialog

class CMyPage : public CPropertyPage
{
// Construction
public:
  CMyPage();   // standard constructor
  CMyPage(UINT nIDTemplate, UINT nIDCaption = 0 );   // standard constructor
  CMyPage(UINT nIDTemplate, const char * strCaption );   // standard constructor

  virtual void Init0() {};

// Dialog Data
  //{{AFX_DATA(CMyPage)
  //}}AFX_DATA

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMyPage)
  public:
  virtual BOOL OnSetActive();
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CMyPage)
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYPAGE_H__4A8C26B5_ADB9_4729_BB1C_05EADB2FA434__INCLUDED_)
