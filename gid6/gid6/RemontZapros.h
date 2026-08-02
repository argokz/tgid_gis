#pragma once

/////////////////////////////////////////////////////////////////////////////
// CRemontZapros dialog

class CRemontZapros : public CDialog
{
// Construction
public:
	CRemontZapros(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRemontZapros)
	enum { IDD = IDD_REMONT_ZAPROS };
	CTime	m_date1;
	CTime	m_date2;
	BOOL	m_check;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRemontZapros)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRemontZapros)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
