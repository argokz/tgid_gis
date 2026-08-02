#pragma once
#include "stdafx.h"
#include "CWebViewBrowser.h"
#include "Ultimate Grid/AdoCug.h"
#include <vector>
#include "WVAdoImpDataSource.h"
#include <functional>
#include <memory>
#include "SearchUtility.h"
#include "HostObjectSampleImpl.h"
class CWebDialog : public CDialog
{
public:

	CWebDialog(CWnd* wnd, int type_of_net, const char* db, const char* connect, const char* tn, const char* q, int n_ret, VIRT_DATA m_typ, const char* caption, bool modal);

	CString mSql, mDb, mConnect;
	int mTypeOfNet;
	CString tableName;
    CString title;
	CString pickupFileField;
	std::function<void(BSTR, BSTR, int, BSTR)> gCallbackPickupFile;

public:
	virtual ~CWebDialog();
	void addDefaultKey(CString key, int value);
	void addDefaultKeys(vector<CString> keys, vector<int> values);
	void setTransIDMode(bool value);
    void setUtverditMode(bool value);
	void setColorMode(CString colorKey);
	void setHead(const char* db, const char* name);
    void setGrouping(CString s) { m_setGrouping = s;};
    void setUtverditKey(CString s) { m_setUtverditKey = s;};


protected:
	CString m_profile, m_caption;

	std::unique_ptr<CWebViewBrowser> m_pWebBrowser;
	afx_msg BOOL OnInitDialog();

	vector<CString> mDataKey;
	vector<int> mDataValue;
	bool transIDMode = false;
    bool utverditMode = false;
	CString colorKey;
	bool colorMode = false;

	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnSetFocus(CWnd* pOldWnd);
	virtual BOOL OnEraseBkgnd(CDC* pDC);
	virtual void OnDestroy();
	virtual void OnCancel();
    virtual LRESULT OnAdoUpdated(WPARAM wParam, LPARAM);
	virtual LRESULT OnOpenFileDialog(WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnAdoNoUpdated(WPARAM wParam, LPARAM);
    virtual LRESULT OnRemontUtverdit(WPARAM wParam, LPARAM);
    virtual LRESULT OnOpresUtverdit(WPARAM wParam, LPARAM);
    virtual void OnOK();
	CString GetNextValues();
	CString ObtainDataToStorage();
	CString GetQueryAppend();
	CString GetHeaderTable();

	int addNew(BSTR str);
	int updateData(int id, BSTR str);
    int utverdit(int id, BSTR str);
	void OnMoveTo(int id);

	BOOL liteMode = true;

	int countFields = -1;
	long countAllRows = -1;
	int countRows = 0;
	int currentCount = 0;
    CString m_setGrouping = "";
    CString m_setUtverditKey = "";


    int m_code;
    BSTR m_str;

    bool m_is_modal;

	bool initDataSource = false;
	std::vector<CString> vField;
	AdoHead m_head;
	CAdoFile* mAdo;
	WVAdoImpDataSource* impDataSource;
	std::map<long, CString> dataStorage;
	vector<long> dataStorageId;
	int currentDataStorageIndex = 0;

	DECLARE_MESSAGE_MAP()
};

