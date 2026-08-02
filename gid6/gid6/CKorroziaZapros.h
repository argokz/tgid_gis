#pragma once

#include "ado.h"


// CKorroziaZapros dialog

class CKorroziaZapros : public CDialogEx
{
	DECLARE_DYNAMIC(CKorroziaZapros)

public:
	CKorroziaZapros(CWnd* pParent, int m_sezon_korrozia);   // standard constructor
	virtual ~CKorroziaZapros();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_KORROZIA_ZAPROS };
#endif
    CAdoFile *m_ado;
    CString m_q;

    int m_sezon_korrozia;




protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_ctrl_year;
	virtual BOOL OnInitDialog();
    virtual void OnOK();
	CString m_year;
	CString m_year1;
	BOOL m_check;
	COleDateTime m_date_plan;
	afx_msg void OnClickedCheck1();
	CDateTimeCtrl m_date_plan_ctrl;
};
