#ifndef __ABOUTBOX_H__
#define __ABOUTBOX_H__
// aboutbox.h : header file
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1997 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.



/////////////////////////////////////////////////////////////////////////////
// CBigIcon window

#include "hlink.h"


class CBigIcon : public CButton
{
// Attributes
public:

// Operations
public:
  void SizeToContent();

// Implementation
protected:
  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

  //{{AFX_MSG(CBigIcon)
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CAboutBox dialog

class CAboutBox : public CDialog
{
// Construction
public:
  CAboutBox(CWnd* pParent = NULL);    // standard constructor

// Dialog Data
  //{{AFX_DATA(CAboutBox)
	enum { IDD = IDD_ABOUTBOX };
    CHyperlink m_mail;
    CHyperlink m_mail2;
	CString	m_warning;
	CString	m_name;
    CString	m_ostat;
    int m_ost;
	//}}AFX_DATA

// Implementation
protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  CBigIcon m_icon; // self-draw button

  // Generated message map functions
  //{{AFX_MSG(CAboutBox)
  virtual BOOL OnInitDialog();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
public:
};

/////////////////////////////////////////////////////////////////////////////
// CSplashWnd dialog

class CSplashWnd : public CDialog
{
// Construction
public:
  BOOL Create(CWnd* pParent);

// Dialog Data
  //{{AFX_DATA(CSplashWnd)
  enum { IDD = IDD_SPLASH };
    // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA

// Implementation
protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  CBigIcon m_icon; // self-draw button
  CFont m_font;   // light version of dialog font

  // Generated message map functions
  //{{AFX_MSG(CSplashWnd)
  virtual BOOL OnInitDialog();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif //__ABOUTBOX_H__