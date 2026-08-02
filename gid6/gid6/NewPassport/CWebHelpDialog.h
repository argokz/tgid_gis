#pragma once
#include "stdafx.h"
#include "CWebViewBrowser.h"
#include "Ultimate Grid/AdoCug.h"
#include <vector>
#include "WVAdoImpDataSource.h"
#include <functional>
#include <memory>

#include "HostObjectSampleImpl.h"
class CWebHelpDialog : public CDialog
{
public:

	CWebHelpDialog(CWnd* wnd);


public:
	virtual ~CWebHelpDialog();

protected:
	CString m_profile, m_caption;

	std::unique_ptr<CWebViewBrowser> m_pWebBrowser;
	afx_msg BOOL OnInitDialog();

	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnSetFocus(CWnd* pOldWnd);
	virtual BOOL OnEraseBkgnd(CDC* pDC);
	virtual void OnDestroy();
	virtual void OnCancel();

	bool initDataSource = false;
	


	DECLARE_MESSAGE_MAP()
};

