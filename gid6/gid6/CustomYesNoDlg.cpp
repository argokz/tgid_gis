#include "stdafx.h"
#include "resource.h"

#include "CustomYesNoDlg.h"

IMPLEMENT_DYNAMIC(CCustomYesNoDlg, CDialogEx)

// CustomYesNoDlg.cpp
BEGIN_MESSAGE_MAP(CCustomYesNoDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BTN_YES,       &CCustomYesNoDlg::OnBtnYes)
    ON_BN_CLICKED(IDC_BTN_NO,        &CCustomYesNoDlg::OnBtnNo)
    ON_BN_CLICKED(IDC_BTN_SOMETHING, &CCustomYesNoDlg::OnBtnSomething)
END_MESSAGE_MAP()

CCustomYesNoDlg::CCustomYesNoDlg(CWnd* pParent, const CString &text, const CString &text2)
  : CDialogEx(IDD, pParent)
{
    m_text = text;
    m_text2 = text2;
}

void CCustomYesNoDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_TEXT, m_text);
//    DDX_Text(pDX, IDC_TEXT, m_text);
//    DDX_Control(pDX, IDC_LIST1, m_ist_list);
//  DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_date);
}



void CCustomYesNoDlg::OnBtnYes()       { m_result = RES_YES;       EndDialog(IDC_BTN_YES); }
void CCustomYesNoDlg::OnBtnNo()        { m_result = RES_NO;        EndDialog(IDC_BTN_NO); }
void CCustomYesNoDlg::OnBtnSomething() { m_result = RES_SOMETHING; EndDialog(IDC_BTN_SOMETHING); }