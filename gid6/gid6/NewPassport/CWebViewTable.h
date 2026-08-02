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
class CWebViewTable : public CView
{

protected:
    CWebViewTable();
    DECLARE_DYNCREATE(CWebViewTable)

public:
    virtual void OnInitialUpdate();
    void setGrouping(CString s) { m_setGrouping = s; };


    CString mSql;
    CString tableName;
    CString titleName;

public:
    virtual void OnDraw(CDC* pDC);

protected:

    // Standard printing commands
    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);

    void OnMoveTo(int id);
    int addNew(BSTR str);
    int addNewOld(BSTR str);
    int updateData(int id, BSTR str);
    void OnMarkAccessibility();

    CString GetNextValues();
    CString GetQueryAppend();
    CString ObtainDataToStorage();
    CString SortData(CString colName, bool desc);
    CString GetEmptyColumns();
    CString GetAllValues();
    //возвращает данные от firstIndex включительно до endIndex включительно
    CString GetValuesByRange(int firstIndex, int endIndex);
    CString GetHeaderTable();


    CString m_setGrouping = "";

    int countFields = -1;
    long countAllRows = -1;
    int countRows = 0;
    int currentCount = 0;
    bool initDataSource = false;

    BOOL liteMode = false;
    BOOL customEditMode = false;

    CAdoFile* mAdo;
    WVAdoImpDataSource* impDataSource;
    std::map<long,CString> dataStorage;
    std::map<CString, vector<std::pair<int, std::any>>> sortDataStorage;
    vector<long> dataStorageId;
    int currentDataStorageIndex = 0;


    std::vector<CString> vField;
    AdoHead m_head;

	BOOL specialForTeplopotery = false;
	BOOL specialHeader = false;
	BOOL viewAll =false;
	int teplopoteryTableIndex = 7;

    std::unique_ptr<CWebViewBrowser> m_pWebBrowser;

    virtual void OnDestroy();


protected:

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    virtual LRESULT OnAdoUpdated(WPARAM wParam, LPARAM);

    DECLARE_MESSAGE_MAP()

};

