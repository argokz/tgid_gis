#pragma once


// CTeploset2 dialog

class CTeploset2 : public CDialogEx
{
	DECLARE_DYNAMIC(CTeploset2)

public:
	CTeploset2(CWnd* pParent, bool is_CTeploset);   // standard constructor
	virtual ~CTeploset2();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEPLOSET };
#endif

  int m_rs;
  int m_ms;
  int m_ms_rs;
  CString m_strName;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
    CString m_profile;
    CImageList m_imgList;

    CTreeCtrl m_tree;
    CFont m_Font;

    bool m_is_CTeploset;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
    virtual void OnSize(UINT nType, int cx, int cy);
    virtual void OnDestroy();
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    virtual void OnOK();

    std::set<int> m_set_ms;
    std::set<int> m_set_rs;
};
