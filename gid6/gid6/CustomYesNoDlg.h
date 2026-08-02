#pragma once


// CustomYesNoDlg.h
class CCustomYesNoDlg : public CDialogEx
{
  DECLARE_DYNAMIC(CCustomYesNoDlg)

public:
    enum Result { RES_YES, RES_NO, RES_SOMETHING, RES_CANCEL };

    CCustomYesNoDlg(CWnd* pParent, const CString &text, const CString &text2);
    enum { IDD = IDD_CUSTOM_YES_NO_SOMETHING };

    CString m_text;
    CString m_text2;

    Result m_result = RES_CANCEL;
protected:
    virtual void DoDataExchange(CDataExchange* pDX);

    afx_msg void OnBtnYes();
    afx_msg void OnBtnNo();
    afx_msg void OnBtnSomething();
    DECLARE_MESSAGE_MAP()
};