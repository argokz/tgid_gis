#pragma once

#include "ado.h"


// CKorroziaZapros2 dialog

class CKorroziaZapros2 : public CDialogEx
{
	DECLARE_DYNAMIC(CKorroziaZapros2)

public:
	CKorroziaZapros2(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CKorroziaZapros2();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_KORROZIA_ZAPROS2 };
#endif
    CAdoFile *m_ado;
    CString m_q;

    int m_y1;
    int m_y2;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK();
    virtual void OnChCombo();

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_ctrl_year1;
    CComboBox m_ctrl_year2;
	virtual BOOL OnInitDialog();
	CString m_year1;
    CString m_year2;
};
