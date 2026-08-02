#include "stdafx.h"
#include "CWebViewBrowser.h"
#include "HostObjectSampleImpl.h"
#include <wrl.h>
#include "wil/com.h"
#include "WebView2.h"
#include <WebView2EnvironmentOptions.h>
#include <sstream>
#include <iomanip>

#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include "shlobj.h"

#pragma comment(lib,"Version.lib")

#include <string>

using namespace Microsoft::WRL;

#define CHECK_FAILURE_STRINGIFY(arg)         #arg
#define CHECK_FAILURE_FILE_LINE(file, line)  ([](HRESULT hr){ CheckFailure(hr, "Failure at " CHECK_FAILURE_STRINGIFY(file) "(" CHECK_FAILURE_STRINGIFY(line) ")"); })
#define CHECK_FAILURE                        CHECK_FAILURE_FILE_LINE(__FILE__, __LINE__)
#define CHECK_FAILURE_BOOL(value)            CHECK_FAILURE((value) ? S_OK : E_UNEXPECTED)
#define MSG_RUN_ASYNC_CALLBACK 999

struct CWebBrowserImpl
{
	wil::com_ptr<ICoreWebView2Environment>    m_webViewEnvironment;
	wil::com_ptr<ICoreWebView2Environment2>   m_webViewEnvironment2;
	wil::com_ptr<ICoreWebView2>               m_webView;
	wil::com_ptr<ICoreWebView2_15>             m_webView2;
	wil::com_ptr<ICoreWebView2Controller>     m_webController;
	wil::com_ptr<ICoreWebView2Settings>       m_webSettings;
	wil::com_ptr<HostObjectSample>			  m_hostObject;
};

void ShowFailure(HRESULT hr, CString const& message)
{
	CString text;
	text.Format("%s (0x%08X)", (LPCTSTR)message, hr);

	::MessageBox(nullptr, static_cast<LPCTSTR>(text), "Îøèáêà", MB_OK);
}

void CheckFailure(HRESULT hr, CString const& message)
{
	if (FAILED(hr))
	{
		CString text;
		text.Format("%s : 0x%08X", (LPCTSTR)message, hr);

		// TODO: log text

		std::exit(hr);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWebBrowser

IMPLEMENT_DYNCREATE(CWebViewBrowser, CWnd)

/////////////////////////////////////////////////////////////////////////////
// CWebBrowser properties
BEGIN_MESSAGE_MAP(CWebViewBrowser, CWnd)
END_MESSAGE_MAP()

CWebViewBrowser::CWebViewBrowser() :m_pImpl(new CWebBrowserImpl())
{
	m_callbacks[CallbackType::CreationCompleted] = nullptr;
	m_callbacks[CallbackType::NavigationCompleted] = nullptr;
	m_PostMessageCalback = nullptr;
}

CWebViewBrowser::~CWebViewBrowser()
{
	SetWindowLongPtr(m_hWnd, GWLP_USERDATA, 0);
	CloseWebView();
	delete m_pImpl;
}

BOOL CWebViewBrowser::CreateHostWindow(
	LPCTSTR lpszClassName,
	LPCTSTR lpszWindowName,
	DWORD dwStyle,
	const RECT& rect,
	CWnd* pParentWnd,
	UINT nID)
{
	if (lpszClassName == nullptr)
		lpszClassName = GetWindowClass();

	if (!CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID))
		return FALSE;

	::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);

	return TRUE;
}

BOOL CWebViewBrowser::Create(
	LPCTSTR lpszClassName,
	LPCTSTR lpszWindowName,
	DWORD dwStyle,
	const RECT& rect,
	CWnd* pParentWnd,
	UINT nID,
	CCreateContext*)
{
	if (!CreateHostWindow(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID))
		return FALSE;

	InitializeWebView();

	return TRUE;
}

BOOL CWebViewBrowser::CreateAsync(
	DWORD dwStyle,
	const RECT& rect,
	CWnd* pParentWnd,
	UINT nID,
	CallbackFunc onCreated)
{
	if (!CreateHostWindow(nullptr, nullptr, dwStyle, rect, pParentWnd, nID))
		return FALSE;

	m_callbacks[CallbackType::CreationCompleted] = onCreated;

	InitializeWebView();

	return TRUE;
}

void CWebViewBrowser::RegisterCallback(CallbackType const type, CallbackFunc callback)
{
	m_callbacks[type] = callback;
}

void CWebViewBrowser::RegisterPostMessageCallback(CallbackPostMessageFunc callback)
{
	m_PostMessageCalback = callback;
}

void CWebViewBrowser::CloseWebView()
{
	if (m_pImpl->m_webView)
	{
		m_pImpl->m_webView->remove_NavigationCompleted(m_navigationCompletedToken);
		m_pImpl->m_webView->remove_NavigationStarting(m_navigationStartingToken);
		m_pImpl->m_webView->remove_WebMessageReceived(m_webMesageReceivedToken);
		m_pImpl->m_webView->remove_DocumentTitleChanged(m_documentTitleChangedToken);

		m_pImpl->m_webController->Close();

		m_pImpl->m_webController = nullptr;
		m_pImpl->m_webView = nullptr;
		m_pImpl->m_webView2 = nullptr;
		m_pImpl->m_webSettings = nullptr;
		m_pImpl->m_hostObject = nullptr;
	}

	m_pImpl->m_webViewEnvironment2 = nullptr;
	m_pImpl->m_webViewEnvironment = nullptr;
}

void CWebViewBrowser::InitializeWebView()
{
	CloseWebView();

	wstring appData = GetUserDataFolder();
	auto options = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();

	options->put_AdditionalBrowserArguments(L"--disable-web-security");

	HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(
		nullptr,
		appData.c_str(),
		options.Get(),
		Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
			this,
			&CWebViewBrowser::OnCreateEnvironmentCompleted).Get());

	if (!SUCCEEDED(hr))
	{
		CString text;
		if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			text = L"Cannot found the WebView2 component.";
		}
		else
		{
			text = L"Cannot create the webview environment.";
		}

		ShowFailure(hr, text);
	}
}

HRESULT CWebViewBrowser::OnCreateEnvironmentCompleted(
	HRESULT result,
	ICoreWebView2Environment* environment)
{
	CHECK_FAILURE(result);

	if (!environment)
		return E_FAIL;

	CHECK_FAILURE(environment->QueryInterface(IID_PPV_ARGS(&m_pImpl->m_webViewEnvironment)));
	CHECK_FAILURE(environment->QueryInterface(IID_PPV_ARGS(&m_pImpl->m_webViewEnvironment2)));

	CHECK_FAILURE(m_pImpl->m_webViewEnvironment->CreateCoreWebView2Controller(
		m_hWnd,
		Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
			this,
			&CWebViewBrowser::OnCreateWebViewControllerCompleted).Get()));

	return S_OK;
}

HRESULT CWebViewBrowser::OnCreateWebViewControllerCompleted(
	HRESULT result,
	ICoreWebView2Controller* controller)
{
	if (result == S_OK)
	{
		if (controller != nullptr)
		{
			m_pImpl->m_webController = controller;
			CHECK_FAILURE(controller->get_CoreWebView2(&m_pImpl->m_webView));

			if (!m_pImpl->m_webView)
				return E_FAIL;

			CHECK_FAILURE(m_pImpl->m_webView->QueryInterface(IID_PPV_ARGS(&m_pImpl->m_webView2)));

			CHECK_FAILURE(m_pImpl->m_webView->get_Settings(&m_pImpl->m_webSettings));

			RegisterEventHandlers();

			ResizeToClientArea();
		}

		auto callback = m_callbacks[CallbackType::CreationCompleted];
		if (callback != nullptr)
			RunAsync(callback);
	}
	else
	{
		ShowFailure(result, "Cannot create webview environment.");
	}

	return S_OK;
}

void CWebViewBrowser::RegisterEventHandlers()
{
	// NavigationCompleted handler
	CHECK_FAILURE(m_pImpl->m_webView->add_NavigationCompleted(
		Callback<ICoreWebView2NavigationCompletedEventHandler>(
			[this](
				ICoreWebView2*,
				ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT
			{
				m_isNavigating = false;

				BOOL success;
				CHECK_FAILURE(args->get_IsSuccess(&success));

				if (!success)
				{
					COREWEBVIEW2_WEB_ERROR_STATUS webErrorStatus{};
					CHECK_FAILURE(args->get_WebErrorStatus(&webErrorStatus));
					if (webErrorStatus == COREWEBVIEW2_WEB_ERROR_STATUS_DISCONNECTED)
					{
						// Do something here if you want to handle a specific error case.
						// In most cases this isn't necessary, because the WebView will
						// display its own error page automatically.
					}
				}

				wil::unique_cotaskmem_string uri;
				m_pImpl->m_webView->get_Source(&uri);

				if (wcscmp(uri.get(), L"about:blank") == 0)
				{
					uri = wil::make_cotaskmem_string(L"");
				}

				auto callback = m_callbacks[CallbackType::NavigationCompleted];
				if (callback != nullptr)
					RunAsync(callback);

				return S_OK;
			})
		.Get(),
				&m_navigationCompletedToken));

	// NavigationStarting handler
	CHECK_FAILURE(m_pImpl->m_webView->add_NavigationStarting(
		Callback<ICoreWebView2NavigationStartingEventHandler>(
			[this](
				ICoreWebView2* webView,
				ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT
			{
				wil::unique_cotaskmem_string uri;
				CHECK_FAILURE(args->get_Uri(&uri));

				if (m_pImpl->m_hostObject) {
					//! [AddHostObjectToScript]
					VARIANT remoteObjectAsVariant = {};
					m_pImpl->m_hostObject.query_to<IDispatch>(&remoteObjectAsVariant.pdispVal);
					remoteObjectAsVariant.vt = VT_DISPATCH;
					webView->AddHostObjectToScript(L"table", &remoteObjectAsVariant);
					remoteObjectAsVariant.pdispVal->Release();
				}
				

				m_isNavigating = true;

				return S_OK;
			}).Get(), &m_navigationStartingToken));

	CHECK_FAILURE(m_pImpl->m_webView->add_WebMessageReceived(
		Callback<ICoreWebView2WebMessageReceivedEventHandler>(
		[this]
	(ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
			PWSTR message;
			args->TryGetWebMessageAsString(&message);
			
			if (m_PostMessageCalback != nullptr)
			{
				m_PostMessageCalback(message);
			}
			webview->PostWebMessageAsString(message);
			CoTaskMemFree(message);
			return S_OK;
		}).Get(), &m_webMesageReceivedToken));

	// DocumentTitleChanged handler
	CHECK_FAILURE(m_pImpl->m_webView->add_DocumentTitleChanged(
		Callback<ICoreWebView2DocumentTitleChangedEventHandler>(
			[this](ICoreWebView2* sender, IUnknown* args) -> HRESULT {
				wil::unique_cotaskmem_string title;
				CHECK_FAILURE(sender->get_DocumentTitle(&title));

				m_strTitle = title.get();

				auto callback = m_callbacks[CallbackType::TitleChanged];
				if (callback != nullptr)
					RunAsync(callback);

				return S_OK;
			})
		.Get(), &m_documentTitleChangedToken));
}

void CWebViewBrowser::ResizeToClientArea()
{
	if (m_pImpl->m_webController)
	{
		RECT bounds;
		GetClientRect(&bounds);
		m_pImpl->m_webController->put_Bounds(bounds);
	}
}

RECT CWebViewBrowser::GetBounds()
{
	RECT rc{ 0,0,0,0 };
	if (m_pImpl->m_webController)
	{
		m_pImpl->m_webController->get_Bounds(&rc);
	}

	return rc;
}

void CWebViewBrowser::Resize(LONG const width, LONG const height)
{
	SetWindowPos(nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOREPOSITION);
}

CString CWebViewBrowser::GetLocationURL()
{
	CString url;
	if (m_pImpl->m_webView)
	{
		wil::unique_cotaskmem_string uri;
		m_pImpl->m_webView->get_Source(&uri);

		if (wcscmp(uri.get(), L"about:blank") == 0)
		{
			uri = wil::make_cotaskmem_string(L"");
		}

		url = uri.get();
	}

	return url;
}

CString CWebViewBrowser::NormalizeUrl(CString url)
{
	if (url.Find(_T("://")) < 0)
	{
		if (url.GetLength() > 1 && url[1] == ':')
			url = _T("file://") + url;
		else
			url = _T("http://") + url;
	}

	return url;
}

void CWebViewBrowser::NavigateTo(CString url)
{
	m_pImpl->m_webView->Navigate(NormalizeUrl(url).AllocSysString());
}

void CWebViewBrowser::ExecuteScript(LPCWSTR javaScript) {
	m_pImpl->m_webView->ExecuteScript(javaScript, nullptr);
}

HRESULT CWebViewBrowser::ExecuteScript(LPCWSTR javaScript, ICoreWebView2ExecuteScriptCompletedHandler* handler) {
	return m_pImpl->m_webView->ExecuteScript(javaScript,handler);
}

HRESULT CWebViewBrowser::CallDevToolsProtocolMethod() {
	return m_pImpl->m_webView->CallDevToolsProtocolMethod(L"Storage.clearDataForOrigin",
		L"{\"origin\": \"https://drive.google.com\", \"storageTypes\": \"all\"}",
		nullptr);
}

void CWebViewBrowser::Navigate(CString const& url, CallbackFunc onComplete)
{
	if (m_pImpl->m_webView)
	{
		m_callbacks[CallbackType::NavigationCompleted] = onComplete;
		NavigateTo(url);
	}
}

void CWebViewBrowser::Stop()
{
	if (m_pImpl->m_webView)
	{
		m_pImpl->m_webView->Stop();
	}
}

void CWebViewBrowser::Reload()
{
	if (m_pImpl->m_webView)
	{
		m_pImpl->m_webView->Reload();
	}
}

void CWebViewBrowser::DisablePopups()
{
	if (m_pImpl->m_webSettings)
	{
		m_pImpl->m_webSettings->put_AreDefaultScriptDialogsEnabled(FALSE);
	}
}

void CWebViewBrowser::DefaultSettings()
{
	if (m_pImpl->m_webSettings)
	{
		m_pImpl->m_webSettings->put_IsScriptEnabled(TRUE);
		m_pImpl->m_webSettings->put_AreDefaultScriptDialogsEnabled(TRUE);
		m_pImpl->m_webSettings->put_IsWebMessageEnabled(TRUE);
		m_pImpl->m_webSettings->put_AreDefaultContextMenusEnabled(false);
	}

	wil::com_ptr<ICoreWebView2_3> wvWnd3 = m_pImpl->m_webView.try_query<ICoreWebView2_3>();
	CString argpath();
	CString fStr;
	fStr.Format("%stableServer", argpath());
	wvWnd3->SetVirtualHostNameToFolderMapping(L"appassets.example",
		fStr.AllocSysString(),
		COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_ALLOW);
}


void CWebViewBrowser::HelpSettings()
{
	if (m_pImpl->m_webSettings)
	{
		m_pImpl->m_webSettings->put_IsScriptEnabled(TRUE);
		m_pImpl->m_webSettings->put_AreDefaultScriptDialogsEnabled(TRUE);
		m_pImpl->m_webSettings->put_IsWebMessageEnabled(TRUE);
		m_pImpl->m_webSettings->put_AreDefaultContextMenusEnabled(false);
	}

	wil::com_ptr<ICoreWebView2_3> wvWnd3 = m_pImpl->m_webView.try_query<ICoreWebView2_3>();
	CString argpath();
	CString fStr;
	fStr.Format("%shelpServer", argpath());
	wvWnd3->SetVirtualHostNameToFolderMapping(L"google.com",
		fStr.AllocSysString(),
		COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_ALLOW);
}

void CWebViewBrowser::setHostObject(
    CallbackInsert callbackInsert, 
    CallbackUpdate callbackUpdate, 
    CallbackDelete callbackDelete, 
    CallbackMoveTo callbackMoveTo,
    CallbackOnExcel callbackOnExcel, 
    CallbackPickupFile callbackPickupFile,
	CallbackUtverdit callbackUtverdit,
	CallbackPdf callbackPdf,
	CallbackOnSearch callbackOnSearch
	) {
	m_pImpl->m_hostObject = Microsoft::WRL::Make<HostObjectSample>(
		callbackInsert, 
        callbackUpdate, 
        callbackDelete, 
        callbackMoveTo, 
        callbackOnExcel,
        callbackPickupFile,
        callbackUtverdit,
		callbackPdf,
		callbackOnSearch
		);

}

PCTSTR CWebViewBrowser::GetWindowClass()
{
	static PCTSTR windowClass = []
	{
		static TCHAR const* className = "EdgeBrowserHost";

		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProcStatic;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = AfxGetInstanceHandle();
		wcex.hIcon = nullptr;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = className;
		wcex.hIconSm = nullptr;

		ATOM result = RegisterClassEx(&wcex);
		if (result == 0)
		{
			[[maybe_unused]] DWORD lastError = ::GetLastError();
		}

		return className;
	}();

	return windowClass;
}

LRESULT CALLBACK CWebViewBrowser::WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (auto app = (CWebViewBrowser*)::GetWindowLongPtr(hWnd, GWLP_USERDATA))
	{
		LRESULT result = 0;
		if (app->HandleWindowMessage(hWnd, message, wParam, lParam, &result))
		{
			return result;
		}
	}

	return ::DefWindowProc(hWnd, message, wParam, lParam);
}

bool CWebViewBrowser::HandleWindowMessage(
	HWND, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* result)
{
	*result = 0;

	switch (message)
	{
	case WM_SIZE:
	{
		if (lParam != 0)
		{
			ResizeToClientArea();
			return true;
		}
	}
	break;
	case MSG_RUN_ASYNC_CALLBACK:
	{
		auto* task = reinterpret_cast<CallbackFunc*>(wParam);
		(*task)();
		delete task;
		return true;
	}
	break;
	}

	return false;
}

void CWebViewBrowser::RunAsync(CallbackFunc callback)
{
	auto* task = new CallbackFunc(callback);
	PostMessage(MSG_RUN_ASYNC_CALLBACK, reinterpret_cast<WPARAM>(task), 0);
}

bool CWebViewBrowser::IsWebViewCreated() const
{
	return m_pImpl->m_webView != nullptr;
}

wstring CWebViewBrowser::GetUserDataFolder()
{
	CString argpath_2();


	CString cs = argpath_2();
	BSTR bstr = cs.AllocSysString();
	std::wstring ws(bstr, SysStringLen(bstr));

	return ws;
	
}
