#if !defined(AFX_DIALOG3_H__701BE5AE_4CD8_4D93_8389_C27A851F5E7D__INCLUDED_)
#define AFX_DIALOG3_H__701BE5AE_4CD8_4D93_8389_C27A851F5E7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dialog3.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialog3 dialog

class CDialog3 : public CDialog
{
// Construction
public:
	CDialog3(CWnd* pParent, int idd = IDD_DIALOG_3, double e1 = 0., double e2 = 0., double e3 = 0.);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialog3)
	enum { IDD = IDD_DIALOG_3 };
	CEdit	m_ctrl_sum;
	double	m_e1;
	double	m_e2;
	double	m_e3;
    double	m_sum;

    CEdit m_ctrl_e1;
    CEdit m_ctrl_e2;
    CEdit m_ctrl_e3;


    double	m_max_e1 = 1e90;
	double	m_max_e2 = 1e90;
	double	m_max_e3 = 1e90;
	
    void setMax(double e1, double e2, double e3);


    //}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialog3)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialog3)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEdit();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOG3_H__701BE5AE_4CD8_4D93_8389_C27A851F5E7D__INCLUDED_)
