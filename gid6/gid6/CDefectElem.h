#pragma once


// CDefectElem dialog

class CDefectElem : public CDialogEx
{
	DECLARE_DYNAMIC(CDefectElem)

public:
	CDefectElem(int id, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDefectElem();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEFECT_ELEM };
#endif

private:
    int m_id;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    virtual void OnDestroy();
    virtual void OnSize(UINT nType, int cx, int cy);
    virtual void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
    virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_tree;
    CImageList m_imgList;
    CString m_profile;
    CFont m_Font;
	virtual BOOL OnInitDialog();
};
