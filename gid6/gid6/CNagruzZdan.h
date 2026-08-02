#pragma once


// CNagruzZdan dialog

class CNagruzZdan : public CDialogEx
{
	DECLARE_DYNAMIC(CNagruzZdan)

public:
	CNagruzZdan(CWnd* pParent, int _is_otopl);   // standard constructor
	virtual ~CNagruzZdan();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NAGRUZ_ZDAN };
#endif


    CButton	m_Help;
    CEdit	m_e8;
	CEdit	m_e7;
	CEdit	m_e6;
	CEdit	m_e5;
	CEdit	m_e4;
	CEdit	m_e3;
	CEdit	m_e2;
    CComboBox	m_ctl_41;
    CButton	Button1;
	CButton	Button2;
	CString	m_nazn;
	int	m_god;
	double	m_v;
	double	m_udel;
	double	m_tn;
	double	m_tvn;
 	double	m_nagr;
    double  m_alpha;


  int m_is_otopl;



protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    virtual void OnHelp();
    virtual void Raschet();

	DECLARE_MESSAGE_MAP()
};
