#pragma once

/////////////////////////////////////////////////////////////////////////////
// CUTP_diam_rasch dialog

class CUTP_diam_rasch : public CDialog
{
// Construction
public:
	CUTP_diam_rasch(CWnd* pParent = NULL);   // standard constructor
	double d;

// Dialog Data
	//{{AFX_DATA(CUTP_diam_rasch)
	enum { IDD = IDD_UTP_DIAM_RASCHET };
	CButton	m_HELP;
	CButton	Button1;
	CButton	Button2;
	CEdit	Edit2;
	CEdit	Edit1;
	CEdit	Edit3;
	CEdit	Edit4;	
	CEdit	Edit5;
	CComboBox	Combo6;
	CComboBox	Combo5;
	CComboBox	Combo4;
	CComboBox	Combo3;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUTP_diam_rasch)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
//	CWorkspace * wsp;
	CString Edit1_str,Edit2_str,Edit3_str,Edit4_str,Edit5_str;
	int Combo3_i,Combo4_i,Combo5_i,Combo6_i;
	// Generated message map functions
	//{{AFX_MSG(CUTP_diam_rasch)
//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void Raschet();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnCloseupCombo5();
	afx_msg void OnCloseupCombo6();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
