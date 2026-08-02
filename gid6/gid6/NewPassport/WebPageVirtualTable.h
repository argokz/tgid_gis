#pragma once

#include "stdafx.h"
#include "Ultimate Grid/AdoHead.h"
#include "WVAdoImpDataSource.h"
#include <wil/com.h>
#include "HostObjectSampleImpl.h"
#include "WebView2.h"
using Microsoft::WRL::ComPtr;


class WebPageVirtualTable : public CPaneDialog
{
	
protected:
	DECLARE_DYNCREATE(WebPageVirtualTable)

public:

	WebPageVirtualTable();

	bool init(int type_of_net, const char* db, const char* connect, const char* tn, const char* q, const char* title, bool is_head, VIRT_DATA _typ);

protected:
	
public:
	virtual ~WebPageVirtualTable();

protected:

	BOOL m_bInit;

	CString m_title;
	CString m_tn;
	CString m_q;
	VIRT_DATA m_typ;

protected:
	
	virtual LRESULT HandleInitDialog(WPARAM, LPARAM);
	virtual void OnEditFind();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnSetFocus(CWnd* pOldWnd);
	virtual BOOL OnEraseBkgnd(CDC* pDC);
	virtual void OnRefreshData();
	virtual void OnShowSql();

	virtual void OnShiftF1();
	virtual void OnDestroy();
	
	DECLARE_MESSAGE_MAP()

protected:
	void OnMoveTo(int id);
	void InitWebView();
	CString GetNextValues();
	CString GetQueryAppend(bool firstTime);
	CString GetHeaderTable();

	int addNew(BSTR str);
	int updateData(int id, BSTR str);
	
	int countFields = -1;
	long countAllRows = -1;
	int countRows = 0;
	int currentCount = 0;
	bool initDataSource = false;

	std::vector<CString> vField;
	AdoHead m_head;
	bool webViewInit = false;
	CAdoFile* mAdo;
	WVAdoImpDataSource* impDataSource;
	wil::com_ptr <ICoreWebView2> webviewWindow;
	wil::com_ptr <ICoreWebView2Controller> webviewController;

	wil::com_ptr<HostObjectSample> m_hostObject;

	EventRegistrationToken m_navigationStartingToken = {};
	EventRegistrationToken m_frameCreatedToken = {};

};


