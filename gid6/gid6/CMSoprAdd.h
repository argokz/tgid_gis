#pragma once


// CMSoprAdd dialog

class CMSoprAdd : public CDialogEx
{
//	DECLARE_DYNAMIC(CMSoprAdd)

public:
	CMSoprAdd(CWnd* pParent, int id, const char *txt, double s_min, double s_max, double s, int n);   // standard constructor
	virtual ~CMSoprAdd();

// Dialog Data
  //{{AFX_DATA(CMSoprAdd)
	enum { IDD = IDD_UTP_MSOPROT_ADD };
  CString m_name;
  double m_s_min;
  double m_s_max;
  double m_s;
  int m_n;
  double m_s_itog;

	CEdit	Edit1;
	CEdit	Edit2;
	CEdit	Edit3;
	CEdit	Edit4;	
	CEdit	Edit5;
	CEdit	Edit6;

  bool m_init;

  //}}AFX_DATA




protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void OnChangeS();
  virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};
