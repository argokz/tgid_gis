#if !defined(AFX_PROTEDIT_H__5B477822_AD9F_4CDB_9774_A826D144E5A7__INCLUDED_)
#define AFX_PROTEDIT_H__5B477822_AD9F_4CDB_9774_A826D144E5A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProtEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProtEdit window

class CProtEdit : public CEdit
{
// Construction
public:
	CProtEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProtEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CProtEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CProtEdit)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROTEDIT_H__5B477822_AD9F_4CDB_9774_A826D144E5A7__INCLUDED_)
