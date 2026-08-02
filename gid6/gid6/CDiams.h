#pragma once


// CDiams dialog

struct DIAMS
{
	int standID;
    double d1, d2, d3, tol;
};

DIAMS* diametr(CWnd* wnd, int standardID);


class CDiams : public CDialogEx
{
	DECLARE_DYNAMIC(CDiams)

public:
	CDiams(CWnd* pParent, int standardID);   // standard constructor
	virtual ~CDiams();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIAMS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

    vector<DIAMS> list_diam;
    vector<int> list_std;
    DIAMS m_diams;
    int m_standID;

    void show_std();
    void show_diam(const char *stand);

	afx_msg void OnSelchangeStd();
    afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	CListCtrl m_ctrlDiams;
	CComboBox m_ctrlStd;
    CFont m_font;
    int   m_nStd;
    int   m_standardID;
    CString m_standard;


	virtual void OnOK();
	afx_msg void OnClickedCalc();
};
