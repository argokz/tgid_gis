#if !defined(AFX_TRANSPDIALOG_H__A7C0D5AB_DE0C_41F8_BC83_2F2B41808329__INCLUDED_)
#define AFX_TRANSPDIALOG_H__A7C0D5AB_DE0C_41F8_BC83_2F2B41808329__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TranspDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTranspDialog dialog

class CTranspDialog : public CDialog
{
// Construction
public:
	CTranspDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTranspDialog)
	enum { IDD = IDD_TRANSP };
	int		m_slider;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTranspDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTranspDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSPDIALOG_H__A7C0D5AB_DE0C_41F8_BC83_2F2B41808329__INCLUDED_)
