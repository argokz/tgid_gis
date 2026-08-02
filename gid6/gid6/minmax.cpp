// MFC application with a dialog that
// accepts two numbers (min/max) and a comment.
// The dialog allows choosing a value: minimum, maximum, or anything between.

#include <afxwin.h>
#include "resource.h"

class CMainDialog : public CDialogEx {
public:
    CMainDialog() : CDialogEx(IDD_MINMAX_DIALOG), m_min(0), m_max(100), m_value(0) {}

    int m_min;
    int m_max;
    int m_value;
    CString m_comment;

protected:
    virtual BOOL OnInitDialog() override {
        CDialogEx::OnInitDialog();
        SetDlgItemInt(IDC_EDIT_MIN, m_min);
        SetDlgItemInt(IDC_EDIT_MAX, m_max);
        SetDlgItemText(IDC_EDIT_COMMENT, m_comment);
        SetDlgItemInt(IDC_EDIT_VALUE, m_value);
        return TRUE;
    }

    afx_msg void OnBnClickedButtonMin() {
        m_value = m_min;
        SetDlgItemInt(IDC_EDIT_VALUE, m_value);
    }

    afx_msg void OnBnClickedButtonMax() {
        m_value = m_max;
        SetDlgItemInt(IDC_EDIT_VALUE, m_value);
    }

    afx_msg void OnEnChangeEditValue() {
        BOOL ok = FALSE;
        int v = GetDlgItemInt(IDC_EDIT_VALUE, &ok);
        if (ok) m_value = v;
    }

    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CMainDialog, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_MIN, &CMainDialog::OnBnClickedButtonMin)
    ON_BN_CLICKED(IDC_BUTTON_MAX, &CMainDialog::OnBnClickedButtonMax)
    ON_EN_CHANGE(IDC_EDIT_VALUE, &CMainDialog::OnEnChangeEditValue)
END_MESSAGE_MAP()

// Resource identifier for the dialog
//typedef enum { IDD_MAIN_DIALOG = 101 } RESOURCE_IDS;

/*
class CMfcSingleDialogApp : public CWinApp {
public:
    virtual BOOL InitInstance() override {
        CWinApp::InitInstance();

        // Передаем параметры извне
        int externalMin = 5;
        int externalMax = 50;
        CString externalComment = L"Число должно быть между 5 и 50";

        CMainDialog dlg;
        dlg.m_min = externalMin;
        dlg.m_max = externalMax;
        dlg.m_comment = externalComment;

        dlg.DoModal();
        return FALSE;
    }
};
*/
//CMfcSingleDialogApp theApp;
