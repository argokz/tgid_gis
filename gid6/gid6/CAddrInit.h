#pragma once

class Klassif;

// CAddrInit dialog

class CAddrInit : public CDialogEx
{
    DECLARE_DYNAMIC(CAddrInit)

public:
    CAddrInit(CWnd* pParent, const Klassif *kls, CString mkr, CString str, CString dom);   // standard constructor
    virtual ~CAddrInit();

    CComboBox m_combo1;
    CComboBox m_combo2;
    CComboBox m_combo3;

    int m_n1;
    int m_n2;
    int m_n3;

    CString m_s1;
    CString m_s2;
    CString m_s3;



// Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ADDR_INIT };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK();
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

    const Klassif *m_kls;
    map <int, CString> map_fields;
};
