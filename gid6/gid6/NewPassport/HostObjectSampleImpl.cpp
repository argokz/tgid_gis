// Copyright (C) Microsoft Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "stdafx.h"

#include <wil/com.h>


#include "HostObjectSampleImpl.h"

HostObjectSample::HostObjectSample(
        HostObjectSample::CallbackInsert callbackInsert, 
        HostObjectSample::CallbackUpdate callbackUpdate, 
        HostObjectSample::CallbackDelete callbackDelete, 
        HostObjectSample::CallbackMoveTo callbackMoveTo, 
        HostObjectSample::CallbackOnExcel callbackOnExcel, 
        HostObjectSample::CallbackPickupFile callbackPickupFile, 
        HostObjectSample::CallbackUtverdit callbackUtverdit,
        HostObjectSample::CallbackPdf callbackPdf,
        HostObjectSample::CallbackOnSearch callbackOnSearch
        )
    :   m_callbackInsert(callbackInsert), 
        m_callbackUpdate(callbackUpdate), 
        m_callbackDelete(callbackDelete), 
        m_callbackMoveTo(callbackMoveTo), 
        m_callbackOnExcel(callbackOnExcel), 
        m_callbackPickupFile(callbackPickupFile),
        m_callbackUtverdit(callbackUtverdit),
        m_callbackPdf(callbackPdf),
        m_callbackOnSearch(callbackOnSearch)

{
}


STDMETHODIMP HostObjectSample::InsertData(BSTR stringJson, IDispatch* callbackParameter) {
    wil::com_ptr<IDispatch> callbackParameterForCapture = callbackParameter;
    
    m_callbackInsert(stringJson, [callbackParameterForCapture](int id, int code, BSTR message) -> void {
        VARIANTARG* varrr = new VARIANTARG[3];
      
        varrr[2].vt = VT_I4;
        varrr[2].lVal = id;
        varrr[1].vt = VT_I4;
        varrr[1].lVal = code;
        varrr[0].vt = VT_BSTR;
        varrr[0].bstrVal = message;
        DISPPARAMS myParams = {  varrr };
        myParams.cArgs = 3;
        myParams.cNamedArgs = 0;

        callbackParameterForCapture->Invoke(
            DISPID_UNKNOWN, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &myParams, nullptr,
            nullptr, nullptr);
        });
    return S_OK;
}

STDMETHODIMP HostObjectSample::DeleteData(VARIANT* idsParameter, IDispatch* callbackParameter) {
    wil::com_ptr<IDispatch> callbackParameterForCapture = callbackParameter;

    vector<int> myIds;

    SAFEARRAY* pSafeArray = (*idsParameter).parray;

    VARIANT* arrayAccess = NULL;
    HRESULT h = SafeArrayAccessData(pSafeArray, (void**)&arrayAccess);
       int size = pSafeArray->rgsabound[0].cElements;
      for (int i = 0; i < size; ++i)
    {
       
        myIds.push_back(arrayAccess[i].lVal);

    }
  
    m_callbackDelete(myIds, [callbackParameterForCapture](int code, BSTR message) -> void {
        VARIANTARG* varrr = new VARIANTARG[2];

        varrr[1].vt = VT_I4;
        varrr[1].lVal = code;
        varrr[0].vt = VT_BSTR;
        varrr[0].bstrVal = message;
        DISPPARAMS myParams = { varrr };
        myParams.cArgs = 2;
        myParams.cNamedArgs = 0;

        callbackParameterForCapture->Invoke(
            DISPID_UNKNOWN, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &myParams, nullptr,
            nullptr, nullptr);
        });
    return S_OK;
}


STDMETHODIMP HostObjectSample::UpdateData(INT idParameter, BSTR stringJson, IDispatch* callbackParameter) {
    wil::com_ptr<IDispatch> callbackParameterForCapture = callbackParameter;
    m_callbackUpdate(idParameter, stringJson, [callbackParameterForCapture](int code, BSTR message) -> void {
        VARIANTARG* varrr = new VARIANTARG[2];

        varrr[1].vt = VT_I4;
        varrr[1].lVal = code;
        varrr[0].vt = VT_BSTR;
        varrr[0].bstrVal = message;
        DISPPARAMS myParams = { varrr };
        myParams.cArgs = 2;
        myParams.cNamedArgs = 0;

        callbackParameterForCapture->Invoke(
            DISPID_UNKNOWN, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &myParams, nullptr,
            nullptr, nullptr);
        });
    return S_OK;
}

STDMETHODIMP HostObjectSample::UtverditAction(INT idParameter, BSTR stringJson, IDispatch* callbackParameter) {
    wil::com_ptr<IDispatch> callbackParameterForCapture = callbackParameter;
    m_callbackUtverdit(idParameter, stringJson, [callbackParameterForCapture](int code, BSTR stringJson, BSTR message) -> void {
        VARIANTARG* varrr = new VARIANTARG[2];

        varrr[2].vt = VT_I4;
        varrr[2].lVal = code;

        varrr[1].vt = VT_BSTR;
        varrr[1].bstrVal = stringJson;
        
        varrr[0].vt = VT_BSTR;
        varrr[0].bstrVal = message;

        DISPPARAMS myParams = { varrr };
        myParams.cArgs = 3;
        myParams.cNamedArgs = 0;

        callbackParameterForCapture->Invoke(
            DISPID_UNKNOWN, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &myParams, nullptr,
            nullptr, nullptr);
        });
    return S_OK;
}



STDMETHODIMP HostObjectSample::OnExcel( BSTR stringJson, IDispatch* callbackParameter) {
    wil::com_ptr<IDispatch> callbackParameterForCapture = callbackParameter;
    m_callbackOnExcel(stringJson, [callbackParameterForCapture](int code, BSTR message) -> void {
        VARIANTARG* varrr = new VARIANTARG[2];

        varrr[1].vt = VT_I4;
        varrr[1].lVal = code;
        varrr[0].vt = VT_BSTR;
        varrr[0].bstrVal = message;
        DISPPARAMS myParams = { varrr };
        myParams.cArgs = 2;
        myParams.cNamedArgs = 0;

        callbackParameterForCapture->Invoke(
            DISPID_UNKNOWN, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &myParams, nullptr,
            nullptr, nullptr);
        });
    return S_OK;
}



STDMETHODIMP HostObjectSample::CreatePdf(BSTR stringData, int count) {

    m_callbackPdf(stringData, count);
    return S_OK;
}

STDMETHODIMP HostObjectSample::UploadFile(BSTR field, IDispatch* callbackParameter) {
    wil::com_ptr<IDispatch> callbackParameterForCapture = callbackParameter;
    m_callbackPickupFile(field, [callbackParameterForCapture](BSTR path, BSTR field, int code, BSTR message) -> void {
        VARIANTARG* varrr = new VARIANTARG[2];
        varrr[3].vt = VT_BSTR;
        varrr[3].bstrVal = path;
        varrr[2].vt = VT_BSTR;
        varrr[2].bstrVal = field;
        varrr[1].vt = VT_I4;
        varrr[1].lVal = code;
        varrr[0].vt = VT_BSTR;
        varrr[0].bstrVal = message;
        DISPPARAMS myParams = { varrr };
        myParams.cArgs = 4;
        myParams.cNamedArgs = 0;

        callbackParameterForCapture->Invoke(
            DISPID_UNKNOWN, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &myParams, nullptr,
            nullptr, nullptr);
        });
    return S_OK;
}

STDMETHODIMP HostObjectSample::MoveTo(INT id, BSTR type, BSTR value, IDispatch* callbackParameter) {
    wil::com_ptr<IDispatch> callbackParameterForCapture = callbackParameter;
    m_callbackMoveTo(id, type, value, [callbackParameterForCapture](int code, BSTR message) -> void {
        VARIANTARG* varrr = new VARIANTARG[2];

        varrr[1].vt = VT_I4;
        varrr[1].lVal = code;
        varrr[0].vt = VT_BSTR;
        varrr[0].bstrVal = message;
        DISPPARAMS myParams = { varrr };
        myParams.cArgs = 2;
        myParams.cNamedArgs = 0;

        callbackParameterForCapture->Invoke(
            DISPID_UNKNOWN, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &myParams, nullptr,
            nullptr, nullptr);
        });
    return S_OK;
}

STDMETHODIMP HostObjectSample::OnSearch(BSTR value, IDispatch* callbackParameter) {
    wil::com_ptr<IDispatch> callbackParameterForCapture = callbackParameter;
    m_callbackOnSearch(value, [callbackParameterForCapture](int code, BSTR message) -> void {
        VARIANTARG* varrr = new VARIANTARG[2];

        varrr[1].vt = VT_I4;
        varrr[1].lVal = code;
        varrr[0].vt = VT_BSTR;
        varrr[0].bstrVal = message;
        DISPPARAMS myParams = { varrr };
        myParams.cArgs = 2;
        myParams.cNamedArgs = 0;

        callbackParameterForCapture->Invoke(
            DISPID_UNKNOWN, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &myParams, nullptr,
            nullptr, nullptr);
        });
    return S_OK;
}

STDMETHODIMP HostObjectSample::GetTypeInfoCount(UINT* pctinfo)
{
    *pctinfo = 1;
    return S_OK;
}

STDMETHODIMP HostObjectSample::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
{
    if (0 != iTInfo)
    {
        return TYPE_E_ELEMENTNOTFOUND;
    }
    if (!m_typeLib)
    {
        RETURN_IF_FAILED(LoadTypeLib(L"gid6.tlb", &m_typeLib));
    }
    return m_typeLib->GetTypeInfoOfGuid(__uuidof(IHostObjectSample), ppTInfo);
}

STDMETHODIMP HostObjectSample::GetIDsOfNames(
    REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)
{
    wil::com_ptr<ITypeInfo> typeInfo;
    RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));
    return typeInfo->GetIDsOfNames(rgszNames, cNames, rgDispId);
}

STDMETHODIMP HostObjectSample::Invoke(
    DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams,
    VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
    wil::com_ptr<ITypeInfo> typeInfo;
    RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));
    return typeInfo->Invoke(
        this, dispIdMember, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}