#if !defined(AFX_TIMEDIAL_H__028C605B_BD72_48C2_8772_20F3A4BA1F6C__INCLUDED_)
#define AFX_TIMEDIAL_H__028C605B_BD72_48C2_8772_20F3A4BA1F6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TimeDial.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTimeDial dialog

class CTimeDial : public CDialog
{
// Construction
public:
	CTimeDial(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTimeDial)
	enum { IDD = IDD_TIME };
	CTime	m_date;
	CTime	m_time;
	//}}AFX_DATA

        CTime m_time_data;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimeDial)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTimeDial)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMEDIAL_H__028C605B_BD72_48C2_8772_20F3A4BA1F6C__INCLUDED_)
