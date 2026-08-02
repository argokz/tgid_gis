#pragma once

class CAdoFile;

class COtopSezon : public CDialog
{
// Construction
public:
	COtopSezon(CWnd* pParent, const char *capt, int year);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COtopSezon)
	enum { IDD = IDD_OTOP_SEZON };
	COleDateTime    m_date1;
	COleDateTime    m_date2;
    CListBox m_combo1;
    int     m_sezon;
	BOOL	m_check;
	//}}AFX_DATA

    int m_year1;

    CString m_capt;
    CString m_tab;
    CAdoFile *m_ado;

    map<CString, pair<COleDateTime, COleDateTime> > m_map_date;

    void setTab(CAdoFile *ado, const char *tab);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COtopSezon)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COtopSezon)
    virtual void OnOK();
	virtual BOOL OnInitDialog();
    virtual void OnSelchangeCombo1();
    virtual void OnDblclk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
