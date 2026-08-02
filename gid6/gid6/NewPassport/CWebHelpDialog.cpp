#pragma once
#include "stdafx.h"
#include "CWebHelpDialog.h"
#include "resource.h"
#include "wm_user.h"
#include "gidrview.h"
#include "crack.h"
#include <regex>
#include <nlohmann/json.hpp>
using nlohmann::json;

BEGIN_MESSAGE_MAP(CWebHelpDialog, CDialog)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_WM_SETFOCUS()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

CWebHelpDialog::CWebHelpDialog(CWnd* pParent)
    : CDialog(IDD_TABLE_DLG, pParent)
{
    

};


CWebHelpDialog::~CWebHelpDialog() {

}

BOOL CWebHelpDialog::OnInitDialog()
{
    UpdateData(FALSE);

    SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);

    int bx = AfxGetApp()->GetProfileInt(m_profile, "bx", 0);
    int by = AfxGetApp()->GetProfileInt(m_profile, "by", 0);
    int cx = AfxGetApp()->GetProfileInt(m_profile, "cx", 0);
    int cy = AfxGetApp()->GetProfileInt(m_profile, "cy", 0);

    if (cx > 0 && cy > 0) {
        SetWindowPos(&wndTopMost, bx, by, cx, cy, SWP_NOZORDER);
    }
    SetWindowText(m_caption);

    //this->ModifyStyleEx(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME, 0, 0);
    //this->ModifyStyle(WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME | WS_BORDER, 0, 0);
    m_pWebBrowser = std::make_unique<CWebViewBrowser>();

    if (m_pWebBrowser != nullptr)
    {
        CRect rectClient;
        GetClientRect(rectClient);

        m_pWebBrowser->CreateAsync(
            WS_VISIBLE | WS_CHILD,
            rectClient,
            this,
            1,
            [this]() {
                m_pWebBrowser->SetParent(this);
                m_pWebBrowser->HelpSettings();
                
                m_pWebBrowser->RegisterCallback(CWebViewBrowser::CallbackType::TitleChanged, [this]() {


                    CRect rectClient;
                    GetClientRect(&rectClient);
                    m_pWebBrowser->Resize(rectClient.Width(), rectClient.Height());

                    });

                //add callback post message
                m_pWebBrowser->RegisterPostMessageCallback([this](PWSTR message) {
                    if (wcscmp(message, L"OnPageFinished") == 0 || (wcscmp(message, L"OnPageBottom") == 0)) {

                    }
                    });
                m_pWebBrowser->CallDevToolsProtocolMethod();
                m_pWebBrowser->Navigate("https://google.com/index.html", nullptr);

            });
    }
    return true;
}


BOOL CWebHelpDialog::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UINT nID = LOWORD(wParam);

    if (nID == IDC_EDIT) {
    }

    return CDialog::OnCommand(wParam, lParam);
}


int CWebHelpDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDialog::OnCreate(lpCreateStruct) == -1)
        return -1;
    return 0;
}

void CWebHelpDialog::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    CRect rectClient;

    if (m_pWebBrowser != nullptr)
    {
        //m_staticBrowser.GetClientRect(rectClient);
        //m_staticBrowser.ClientToScreen(rectClient);
        //ScreenToClient(rectClient);
        GetClientRect(&rectClient);
        m_pWebBrowser->Resize(rectClient.Width(), rectClient.Height());
    }
}

void CWebHelpDialog::OnSetFocus(CWnd* pOldWnd)
{
    CDialog::OnSetFocus(pOldWnd);

}

BOOL CWebHelpDialog::OnEraseBkgnd(CDC* pDC)
{

    return CDialog::OnEraseBkgnd(pDC);
}


void setView(CView* pView0);

void CWebHelpDialog::OnDestroy()
{
    WINDOWPLACEMENT wpi;
    GetWindowPlacement(&wpi);

    AfxGetApp()->WriteProfileInt(m_profile, "bx", wpi.rcNormalPosition.left);
    AfxGetApp()->WriteProfileInt(m_profile, "by", wpi.rcNormalPosition.top);
    AfxGetApp()->WriteProfileInt(m_profile, "cx", wpi.rcNormalPosition.right - wpi.rcNormalPosition.left);
    AfxGetApp()->WriteProfileInt(m_profile, "cy", wpi.rcNormalPosition.bottom - wpi.rcNormalPosition.top);

    m_pWebBrowser.reset();

    CDialog::OnDestroy();
}

void CWebHelpDialog::OnCancel() {

    CDialog::OnCancel();
}

double to_double(COleVariant& var);


