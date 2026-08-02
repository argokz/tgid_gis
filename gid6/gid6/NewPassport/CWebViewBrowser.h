#pragma once

#include "stdafx.h"
#include <EventToken.h>
#include <functional>
#include <map>


struct ICoreWebView2Environment;
struct ICoreWebView2ExecuteScriptCompletedHandler;
struct ICoreWebView2Controller;
struct CWebBrowserImpl;
class CView;

class CWebViewBrowser : public CWnd
{
public:
    enum class CallbackType
    {
        CreationCompleted,
        NavigationCompleted,
        TitleChanged,
        PostWebMessage
    };
    typedef std::function<void(int, int, BSTR)> CallbackAfterInsert;
    typedef std::function<void(int, BSTR)> CallbackAfterChange;
    typedef std::function<void(int, BSTR, BSTR)> CallbackAfterUtverdit;
    typedef std::function<void(BSTR, BSTR, int, BSTR)> CallbackAfterPickupFile;
    typedef std::function<void(BSTR strJson, CallbackAfterInsert)> CallbackInsert;
    typedef std::function<void(INT id, BSTR strJson, CallbackAfterChange)> CallbackUpdate;
    typedef std::function<void(vector<int> ids, CallbackAfterChange)> CallbackDelete;
    typedef std::function<void(INT id, BSTR strJson, CallbackAfterUtverdit)> CallbackUtverdit;
    typedef std::function<void(BSTR strData, int count)> CallbackPdf;
    typedef std::function<void(BSTR value, CallbackAfterChange)> CallbackOnSearch;

    typedef std::function<void(int id, BSTR type, BSTR value, CallbackAfterChange)> CallbackMoveTo;
    typedef std::function<void(BSTR strField, CallbackAfterPickupFile)> CallbackPickupFile;
    typedef std::function<void(BSTR strJson, CallbackAfterChange)> CallbackOnExcel;


    using CallbackFunc = std::function<void()>;
    using CallbackPostMessageFunc = std::function<void(PWSTR message)>;

public:
    CWebViewBrowser();
    virtual ~CWebViewBrowser();

    virtual BOOL Create(
        LPCTSTR lpszClassName,
        LPCTSTR lpszWindowName,
        DWORD dwStyle,
        const RECT& rect,
        CWnd* pParentWnd,
        UINT nID,
        CCreateContext* = NULL) override;

    BOOL CreateAsync(
        DWORD dwStyle,
        const RECT& rect,
        CWnd* pParentWnd,
        UINT nID,
        CallbackFunc onCreated);

    void RegisterCallback(CallbackType const type, CallbackFunc callback);
    void RegisterPostMessageCallback(CallbackPostMessageFunc callback);

    RECT GetBounds();
    void SetBounds(LONG const width, LONG const height) { Resize(width, height); }
    void Resize(LONG const width, LONG const height);

    CString GetLocationURL();

    void Navigate(CString const& url, CallbackFunc onComplete);
    void ExecuteScript(LPCWSTR javaScript);
    HRESULT ExecuteScript(LPCWSTR javaScript, ICoreWebView2ExecuteScriptCompletedHandler* handler);
    HRESULT CallDevToolsProtocolMethod();
    void Reload();
    void Stop();
    bool IsNavigating() const { return m_isNavigating; }
    void DisablePopups();
    void DefaultSettings();
    void HelpSettings();
    void setHostObject(CallbackInsert callbackInsert, 
                       CallbackUpdate callbackUpdate, 
                       CallbackDelete callbackDelete, 
                       CallbackMoveTo callbackMoveTo, 
                       CallbackOnExcel callbackOnExcel, 
                       CallbackPickupFile callbackPickupFile,
                       CallbackUtverdit callbackUtverdit,
                       CallbackPdf callbackPdf,
                       CallbackOnSearch callbackOnSearch
                       );

    CString GetTitle() const { return m_strTitle; }

    void SetParent(CWnd* pParent) { m_pParent = pParent; }
    bool IsWebViewCreated() const;

protected:
    DECLARE_DYNCREATE(CWebViewBrowser)
    DECLARE_MESSAGE_MAP()

private:
    CWebBrowserImpl* m_pImpl;
    std::map<CallbackType, CallbackFunc> m_callbacks;
    CallbackPostMessageFunc m_PostMessageCalback;

    EventRegistrationToken m_navigationCompletedToken = {};
    EventRegistrationToken m_navigationStartingToken = {};
    EventRegistrationToken m_documentTitleChangedToken = {};
    EventRegistrationToken m_webMesageReceivedToken = {};

    bool m_isNavigating = false;
    CWnd* m_pParent = nullptr;
    CString m_strTitle;

private:
    void RunAsync(CallbackFunc callback);

    void CloseWebView();
    void RegisterEventHandlers();
    void ResizeToClientArea();
    void NavigateTo(CString url);
    CString NormalizeUrl(CString url);

    static wstring GetUserDataFolder();

    void InitializeWebView();
    HRESULT OnCreateEnvironmentCompleted(HRESULT result, ICoreWebView2Environment* environment);
    HRESULT OnCreateWebViewControllerCompleted(HRESULT result, ICoreWebView2Controller* controller);

    static PCTSTR GetWindowClass();
    static LRESULT CALLBACK WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    bool HandleWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* result);

    BOOL CreateHostWindow(
        LPCTSTR lpszClassName,
        LPCTSTR lpszWindowName,
        DWORD dwStyle,
        const RECT& rect,
        CWnd* pParentWnd,
        UINT nID);
};
