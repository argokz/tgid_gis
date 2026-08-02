#pragma once

// CTeploset dialog

class CTeploset : public CDialogEx
{
	DECLARE_DYNAMIC(CTeploset)

public:
	CTeploset(CWnd* pParent, bool is_CTeploset = false, bool second = false);   // standard constructor
	virtual ~CTeploset();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEPLOSET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
    CString m_profile;
    CImageList m_imgList;

    CTreeCtrl m_tree;
    CFont m_Font;
    bool m_second;
    bool m_is_CTeploset;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
    virtual void OnSize(UINT nType, int cx, int cy);
    virtual void OnDestroy();
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    virtual void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
    virtual void OnOK();

    std::set<int> m_set_ms;
    std::set<int> m_set_rs;

    bool m_all;
};
