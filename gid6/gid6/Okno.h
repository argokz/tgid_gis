#pragma once

/////////////////////////////////////////////////////////////////////////////
// COkno window

class COkno : public CWnd
{
// Construction
public:
  COkno();

// Attributes
  public:
    CEdit m_edt;
    CFont m_font;

// Operations
public:

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(COkno)
  public:
  virtual BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID);
  //}}AFX_VIRTUAL

// Implementation
public:
  void SetText(const char *txt);
  virtual ~COkno();

  // Generated message map functions
protected:
  //{{AFX_MSG(COkno)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnClose();
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};

