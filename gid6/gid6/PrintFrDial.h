#if !defined(AFX_PRINTFRDIAL_H__AA41D15D_0AA9_4ACA_A26C_B8F24B8FE574__INCLUDED_)
#define AFX_PRINTFRDIAL_H__AA41D15D_0AA9_4ACA_A26C_B8F24B8FE574__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrintFrDial.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrintFrDial dialog

class CPrintFrDial : public CDialog
{
// Construction
public:
	CPrintFrDial(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPrintFrDial)
	enum { IDD = IDD_PRINT_FR };
	CString	m_mas;
	int		m_paper;
	int		m_orient;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintFrDial)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPrintFrDial)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTFRDIAL_H__AA41D15D_0AA9_4ACA_A26C_B8F24B8FE574__INCLUDED_)
