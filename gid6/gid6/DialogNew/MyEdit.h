#if !defined(AFX_MYEDIT_H__F6ED9283_2955_11D3_B153_A40D27506C56__INCLUDED_)
#define AFX_MYEDIT_H__F6ED9283_2955_11D3_B153_A40D27506C56__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MyEdit.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CMyControl window

class CMyControl
{
  
public:
  virtual ~CMyControl() {};

  virtual void GetText(CString& rString ) = 0;
  virtual void SetText(LPCTSTR lpszString) = 0;
  virtual void Move(LPCRECT lpRect) = 0;

};


/////////////////////////////////////////////////////////////////////////////
// CMyEdit window

class CMyEdit : public CEdit, public CMyControl
{

  COLORREF m_cr;
  COLORREF m_crTextColor;
  CBrush   m_br;

  HBRUSH CtlColor ( CDC* pDC, UINT nCtlColor )
  {
    pDC->SetBkColor(m_cr);
    pDC->SetTextColor(m_crTextColor); 
    return m_br;
  }
public:
  void SetTextColor(COLORREF cr)
  {
    m_crTextColor = cr; 
    RedrawWindow();
  }
  void SetBkColor(COLORREF cr)
  {
    m_cr = cr;
    m_br.CreateSolidBrush(cr);
    Invalidate();
  }



// Construction
public:
  CMyEdit()
  {
    m_crTextColor = 0;
    m_cr = 0xFFFFFF;
  }


//  virtual ~CMyEdit() {};

// Attributes
public:

// Operations
public:
  virtual void GetText(CString& rString );
  virtual void SetText(LPCTSTR lpszString);
  virtual void Move(LPCRECT lpRect);

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMyEdit)
  public:
//  virtual BOOL PreTranslateMessage(MSG* pMsg);
  //}}AFX_VIRTUAL

// Implementation
public:
//  virtual ~CMyEdit2();

  // Generated message map functions
protected:
  //{{AFX_MSG(CMyEdit2)
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};


class CMyStatic : public CStatic, public CMyControl
{

  COLORREF m_cr;
  COLORREF m_crTextColor;
  CBrush   m_br;

  HBRUSH CtlColor ( CDC* pDC, UINT nCtlColor )
  {
    pDC->SetBkColor(m_cr);
    pDC->SetTextColor(m_crTextColor); 
    return m_br;
  }
public:
  void SetTextColor(COLORREF cr)
  {
    m_crTextColor = cr; 
    RedrawWindow();
  }
  void SetBkColor(COLORREF cr)
  {
    m_cr = cr;
    m_br.CreateSolidBrush(cr);
    Invalidate();
  }



// Construction
public:
  CMyStatic()
  {
    m_crTextColor = 0;
    m_cr = 0xFFFFFF;
  }


//  virtual ~CMyEdit() {};

// Attributes
public:

// Operations
public:
  virtual void GetText(CString& rString );
  virtual void SetText(LPCTSTR lpszString);
  virtual void Move(LPCRECT lpRect);

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMyEdit)
  public:
//  virtual BOOL PreTranslateMessage(MSG* pMsg);
  //}}AFX_VIRTUAL

// Implementation
public:
//  virtual ~CMyEdit2();

  // Generated message map functions
protected:
  //{{AFX_MSG(CMyEdit2)
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};



class CMyComboBox : public CComboBox, public CMyControl
{
  COLORREF m_crTextColor;
  COLORREF m_cr;
  CBrush   m_br;

  HBRUSH CtlColor ( CDC* pDC, UINT nCtlColor )
  {
    pDC->SetBkColor(m_cr);
    pDC->SetTextColor(m_crTextColor); 
    return m_br;
  }
public:
  void SetTextColor(COLORREF cr)
  {
    m_crTextColor = cr; 
    RedrawWindow();
  }
  void SetBkColor(COLORREF cr)
  {
    m_cr = cr;
    m_br.CreateSolidBrush(cr);
    RedrawWindow();
//    Invalidate();
  }

  
  
  // Construction
public:
  CMyComboBox();
//  virtual ~CMyComboBox() {};







// Attributes
public:

// Operations
public:
  virtual void GetText(CString& rString );
  virtual void SetText(LPCTSTR lpszString);
  virtual void Move(LPCRECT lpRect);

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMyComboBox)
  public:
//  virtual BOOL PreTranslateMessage(MSG* pMsg);
  //}}AFX_VIRTUAL

// Implementation
public:
//  virtual ~CMyComboBox();
  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
  virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
  virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);

  // Generated message map functions
protected:
  CString str;

  //{{AFX_MSG(CMyComboBox)
  afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};


class CMyComboBox2 : public CComboBox, public CMyControl
{
// Construction
public:
  CMyComboBox2();


  COLORREF m_cr;
  CBrush   m_br;

  HBRUSH CtlColor ( CDC* pDC, UINT nCtlColor )
  {
    pDC->SetBkColor(m_cr);
    return m_br;
  }
public:
  void SetBkColor(COLORREF cr)
  {
    m_cr = cr;
    m_br.CreateSolidBrush(cr);
    Invalidate();
  }




// Attributes
public:

// Operations
public:
  virtual void GetText(CString& rString );
  virtual void SetText(LPCTSTR lpszString);
  virtual void Move(LPCRECT lpRect);

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMyComboBox)
  public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  //}}AFX_VIRTUAL

// Implementation
public:
//  virtual ~CMyComboBox();

  // Generated message map functions
protected:
  //{{AFX_MSG(CMyComboBox)
//  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};



class CMyButton : public CButton, public CMyControl
{
// Construction
public:
//  CMyButton();


  COLORREF m_cr;
  COLORREF m_cr1;
  CBrush   m_br;

  HBRUSH CtlColor ( CDC* pDC, UINT nCtlColor )
  {
    pDC->SetBkMode(TRANSPARENT);
    pDC->SetBkColor(m_cr);

    return m_br;
  }
public:
  void SetBkColor(COLORREF cr)
  {
    m_cr = cr;
    m_br.CreateSolidBrush(cr);
    Invalidate();
  }




// Attributes
public:

// Operations
public:
  virtual void GetText(CString& rString );
  virtual void SetText(LPCTSTR lpszString);
  virtual void Move(LPCRECT lpRect);

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMyComboBox)
  public:
//  virtual BOOL PreTranslateMessage(MSG* pMsg);
  //}}AFX_VIRTUAL

// Implementation
public:
//  virtual ~CMyComboBox();

  // Generated message map functions
protected:
  //{{AFX_MSG(CMyComboBox)
//  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};





/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYEDIT_H__F6ED9283_2955_11D3_B153_A40D27506C56__INCLUDED_)
