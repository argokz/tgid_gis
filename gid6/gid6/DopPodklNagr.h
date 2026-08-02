#pragma once

/////////////////////////////////////////////////////////////////////////////
// CDopPodklNagr dialog

class CDopPodklNagr : public CDialog
{
// Construction
public:
//  CDopPodklNagr(CWnd* pParent = NULL);   // standard constructor
  CDopPodklNagr(CWnd* pParent, double dp_min, double g, double p1, double p2, double poteri, double qq, double t1, double t2);

// Dialog Data
  //{{AFX_DATA(CDopPodklNagr)
	enum { IDD = IDD_DOP_PODKL_NAGR };
  double  m_dp_min;
  double  m_g;
  double  m_p1;
  double  m_p2;
  double  m_poteri;
  double  m_qq;
  double  m_t1;
  double  m_t2;
	double	m_g2;
	//}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CDopPodklNagr)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CDopPodklNagr)
  virtual BOOL OnInitDialog();
	afx_msg void OnRas();
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};
