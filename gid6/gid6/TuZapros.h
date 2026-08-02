#if !defined(AFX_TUZAPROS_H__A2E2F3D0_9BA5_4251_97E4_3B5C9E654698__INCLUDED_)
#define AFX_TUZAPROS_H__A2E2F3D0_9BA5_4251_97E4_3B5C9E654698__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TuZapros.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTuZapros dialog

class CTuZapros : public CDialog
{
// Construction
public:
	CTuZapros(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTuZapros)
	enum { IDD = IDD_TU_ZAPROS };
	CCheckListBox	m_ctrl_ist;
	CCheckListBox	m_ctrl_raj;
	CCheckListBox	m_ctrl_year;
	BOOL	m_check_a;
	BOOL	m_check_d;
	BOOL	m_check_z;
	//}}AFX_DATA


  void AddYear(const char *text, void *v = NULL);
  void AddIst(const char *text, void *v = NULL);
  void AddRaj(const char *text, void *v = NULL);
  void *getV(const char *text);
  void *getV();

  CArray<CString,CString> list_year;
  CArray<CString,CString> list_raj;
  CArray<CString,CString> list_ist;

  CArray<CString,CString> list_year2;
  CArray<CString,CString> list_raj2;
  CArray<CString,CString> list_ist2;


  int c_year;
  int c_raj;
  int c_ist;

  CString m_q;

  
  map<CString,void *> map_menu;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTuZapros)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTuZapros)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
  virtual void OnChkChangeIst();
  virtual void OnChkChangeRaj();
  virtual void OnChkChangeYear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TUZAPROS_H__A2E2F3D0_9BA5_4251_97E4_3B5C9E654698__INCLUDED_)
