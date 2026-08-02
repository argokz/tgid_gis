#pragma once

#include "ado.h"


// CDefectElements dialog

class CDefectElements : public CDialogEx
{
	DECLARE_DYNAMIC(CDefectElements)

public:
	CDefectElements(int id, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDefectElements();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEFECT_ELEMENTS };
#endif

private:
    int m_id;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
    virtual void OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult) ;
	CListCtrl m_list1;
	CListCtrl m_list2;
};
