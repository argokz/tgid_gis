#if !defined(AFX_FINDTUDOG_H__298EE5F8_6176_4C8A_9D17_F6972A0F589E__INCLUDED_)
#define AFX_FINDTUDOG_H__298EE5F8_6176_4C8A_9D17_F6972A0F589E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindTUDOG.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindTUDOG dialog

class CFindTUDOG : public CDialog
{
// Construction
public:
	CFindTUDOG(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFindTUDOG)
	enum { IDD = IDD_PTS_ALMA_F1 };
	CString	m_edit_find_name;
	CString	m_edit_find_name_tu;
	CString	m_edit_find_nd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindTUDOG)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFindTUDOG)
	virtual BOOL OnInitDialog();
	afx_msg void OnFindNd();
	afx_msg void OnTu();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDTUDOG_H__298EE5F8_6176_4C8A_9D17_F6972A0F589E__INCLUDED_)
