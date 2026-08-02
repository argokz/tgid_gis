#pragma once

#include "stdafx.h"

#include <wrl.h>
#include <wil/com.h>

#include <functional>
#include <map>
#include <string>
#include <wrl\client.h>

#include "HostObjectSample_h.h"

class HostObjectSample : public Microsoft::WRL::RuntimeClass<
    Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
    IHostObjectSample, IDispatch>
{
public:
    typedef std::function<void(int, int, BSTR)> CallbackAfterInsert;
    typedef std::function<void(int, BSTR)> CallbackAfterChange;
    typedef std::function<void(int, BSTR, BSTR)> CallbackAfterUtverdit;
    typedef std::function<void(BSTR, BSTR, int, BSTR)> CallbackAfterPickupFile;
    typedef std::function<void(BSTR strJson, CallbackAfterInsert)> CallbackInsert;
    typedef std::function<void(INT id, BSTR strJson, CallbackAfterChange)> CallbackUpdate;
    typedef std::function<void(INT id, BSTR strJson, CallbackAfterUtverdit)> CallbackUtverdit;

    typedef std::function<void(BSTR strData, int count)> CallbackPdf;

    typedef std::function<void(vector<int> ids, CallbackAfterChange)> CallbackDelete;
    typedef std::function<void(int id, BSTR type, BSTR value,  CallbackAfterChange)> CallbackMoveTo;
    typedef std::function<void(BSTR strField, CallbackAfterPickupFile)> CallbackPickupFile;
    typedef std::function<void(BSTR strJson, CallbackAfterChange)> CallbackOnExcel;
    typedef std::function<void(BSTR strJson, CallbackAfterChange)> CallbackOnSearch;

    HostObjectSample(
        CallbackInsert callbackInsert, 
        CallbackUpdate callbackUpdate, 
        CallbackDelete callbackDelete, 
        CallbackMoveTo callbackMoveTo, 
        CallbackOnExcel callbackOnExcel, 
        CallbackPickupFile callbackPickupFile,
        CallbackUtverdit callbackUtverdit,
        CallbackPdf callbackPdf,
        CallbackOnSearch callbackOnSearch
        );



    STDMETHODIMP InsertData(BSTR stringJson, IDispatch* callbackParameter) override;

    STDMETHODIMP OnExcel(BSTR stringJson, IDispatch* callbackParameter) override;

    STDMETHODIMP DeleteData(VARIANT *idArray, IDispatch* callbackParameter) override;

    STDMETHODIMP MoveTo(INT id, BSTR type, BSTR value, IDispatch* callbackParameter) override;

    STDMETHODIMP OnSearch(BSTR value, IDispatch* callbackParameter) override;

    STDMETHODIMP UploadFile(BSTR field, IDispatch* callbackParameter) override;

    STDMETHODIMP UpdateData(INT idParameter, BSTR stringJson, IDispatch* callbackParameter) override;

    STDMETHODIMP UtverditAction(INT idParameter, BSTR stringJson, IDispatch* callbackParameter) override;

    STDMETHODIMP CreatePdf(BSTR stringData, int count) override;


    // IDispatch implementation
    STDMETHODIMP GetTypeInfoCount(UINT* pctinfo) override;

    STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override;

    STDMETHODIMP GetIDsOfNames(
        REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) override;

    STDMETHODIMP Invoke(
        DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams,
        VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override;

private:
    std::wstring m_propertyValue;
    std::map<INT, std::wstring> m_propertyValues;
    wil::com_ptr<IDispatch> m_callback;

    CallbackInsert m_callbackInsert;
    CallbackUpdate m_callbackUpdate;
    CallbackDelete m_callbackDelete;
    CallbackMoveTo m_callbackMoveTo;
    CallbackOnExcel m_callbackOnExcel;
    CallbackPickupFile m_callbackPickupFile;
    CallbackUtverdit m_callbackUtverdit;
    CallbackPdf m_callbackPdf;
    CallbackOnSearch m_callbackOnSearch;

    wil::com_ptr<ITypeLib> m_typeLib;

    DATE m_date;
    WCHAR m_formattedTime[200];
    WCHAR m_formattedDate[200];
};
