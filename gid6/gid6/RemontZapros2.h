#pragma once

class CAdoFile;

class CRemontZapros2 : public CDialog
{
// Construction
public:
	CRemontZapros2(CWnd* pParent, const char *capt = "");   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRemontZapros2)
	enum { IDD = IDD_REMONT_ZAPROS2 };
	CTime	m_date1;
	CTime	m_date2;
    CComboBox m_combo1;
    int     m_type1;
	BOOL	m_check;
	//}}AFX_DATA

    CString m_capt;
    CString m_tab;
    CAdoFile *m_ado;

    void setTab(CAdoFile *ado, const char *tab);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRemontZapros2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRemontZapros2)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
