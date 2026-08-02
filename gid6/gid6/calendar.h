#if !defined(AFX_CALENDAR_H__D8C6DC80_8421_4BF4_A224_BB7BAD9441F6__INCLUDED_)
#define AFX_CALENDAR_H__D8C6DC80_8421_4BF4_A224_BB7BAD9441F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// calendar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCalendar window

#define WM_CALENDAR (WM_USER + 2)


class CCalendar : public CMonthCalCtrl
{
// Construction
public:
	CCalendar();
	void InitCalendar(const CString &Date, int typ);
	void PositionCalendar(RECT PosR);

// Attributes
public:
  int m_typ;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalendar)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCalendar();

	// Generated message map functions
protected:
	BOOL CanSaveChange;
//	CWnd * EditGrid;
	//{{AFX_MSG(CCalendar)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALENDAR_H__D8C6DC80_8421_4BF4_A224_BB7BAD9441F6__INCLUDED_)
