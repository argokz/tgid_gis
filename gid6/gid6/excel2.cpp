#include "stdafx.h"
#include <ole2.h> 

void ErrorMsg1(const char *s, int code);


// AutoWrap() - Automation helper function...
HRESULT AutoWrap(int autoType, VARIANT *pvResult, IDispatch *pDisp, LPOLESTR ptName, int cArgs...) {
    // Begin variable-argument list...
    va_list marker;
    va_start(marker, cArgs);

    if(!pDisp) {
//        MessageBox(NULL, "NULL IDispatch passed to AutoWrap()", "Error", 0x10010);
//        _exit(0);
        return -1;
    }

    // Variables used...
    DISPPARAMS dp = { NULL, NULL, 0, 0 };
    DISPID dispidNamed = DISPID_PROPERTYPUT;
    DISPID dispID;
    HRESULT hr;
    char buf[200];
    char szName[200];

    
    // Convert down to ANSI
    WideCharToMultiByte(CP_ACP, 0, ptName, -1, szName, 256, NULL, NULL);
    
    // Get DISPID for name passed...
    hr = pDisp->GetIDsOfNames(IID_NULL, &ptName, 1, LOCALE_USER_DEFAULT, &dispID);
    if(FAILED(hr)) {
        sprintf(buf, "IDispatch::GetIDsOfNames(\"%s\") failed w/err 0x%08lx", szName, hr);
        ErrorMsg1("", hr);
//        MessageBox(NULL, buf, "AutoWrap()", 0x10010);
//        _exit(0);
        return hr;
    }
    
    // Allocate memory for arguments...
    VARIANT *pArgs = new VARIANT[cArgs+1];
    // Extract arguments...
    for(int i=0; i<cArgs; i++) {
        pArgs[i] = va_arg(marker, VARIANT);
    }
    
    // Build DISPPARAMS
    dp.cArgs = cArgs;
    dp.rgvarg = pArgs;
    
    // Handle special-case for property-puts!
    if(autoType & DISPATCH_PROPERTYPUT) {
        dp.cNamedArgs = 1;
        dp.rgdispidNamedArgs = &dispidNamed;
    }
    
    // Make the call!
    hr = pDisp->Invoke(dispID, IID_NULL, LOCALE_SYSTEM_DEFAULT, autoType, &dp, pvResult, NULL, NULL);
    if(FAILED(hr)) {
        sprintf(buf, "IDispatch::Invoke(\"%s\"=%08lx) failed w/err 0x%08lx", szName, dispID, hr);
        ErrorMsg1("", hr);
//        MessageBox(NULL, buf, "AutoWrap()", 0x10010);
//        _exit(0);
//        return hr;
    }
    // End variable-argument section...
    va_end(marker);
    
    delete [] pArgs;
    
    return hr;
}
  

int excel(const char *path)
{
  
 // Initialize COM for this thread...
//  if (FAILED( CoInitialize(NULL))) return -1;

   // Get CLSID for our server...
   CLSID clsid;
   HRESULT hr = CLSIDFromProgID(L"Excel.Application", &clsid);
   VARIANT arr;
   bool isOpen = true;

   if(FAILED(hr)) {
//      ::MessageBox(NULL, "CLSIDFromProgID() failed", "Error", 0x10010);
      ErrorMsg1("CLSIDFromProgID() failed", hr);
      return -1;
   }

   // Start server and get IDispatch...
   IDispatch *pXlApp;
   hr = CoCreateInstance(clsid, NULL, CLSCTX_LOCAL_SERVER, IID_IDispatch, (void **)&pXlApp);
   if(FAILED(hr)) {
      ErrorMsg1("Excel не зарегистрирован", hr);
//      ::MessageBox(NULL, "Excel not registered properly", "Error", 0x10010);
      return -2;
   }

   // Make it visible (i.e. app.visible = 1)
   {

      VARIANT x;
      x.vt = VT_I4;
      x.lVal = 1;
      AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlApp, L"Visible", 1, x);
   }


   // Get Workbooks collection
   IDispatch *pXlBooks;
   {
      VARIANT result;
      VariantInit(&result);
      AutoWrap(DISPATCH_PROPERTYGET, &result, pXlApp, L"Workbooks", 0);
      pXlBooks = result.pdispVal;
   }

/*

   // Call Workbooks.Add() to get a new workbook...
   IDispatch *pXlBook;
   {
      VARIANT result;
      VariantInit(&result);
      AutoWrap(DISPATCH_PROPERTYGET, &result, pXlBooks, L"Add", 0);
      pXlBook = result.pdispVal;
   }
*/

   IDispatch *pXlBook;
   {
      VARIANT result;
      VariantInit(&result);
      COleVariant parm(path);
      hr = AutoWrap(DISPATCH_METHOD, &result, pXlBooks, L"Open", 1, parm);
      if(FAILED(hr)) {
        isOpen = false;
      }
      pXlBook = result.pdispVal;
   }

   if (isOpen) {

   // Create a 15x15 safearray of variants...
  
     int row = 18, col = 12;

     arr.vt = VT_ARRAY | VT_VARIANT;
     {
        SAFEARRAYBOUND sab[2];
        sab[0].lLbound = 1; sab[0].cElements = row;
        sab[1].lLbound = 1; sab[1].cElements = col;
        arr.parray = SafeArrayCreate(VT_VARIANT, 2, sab);
     }

     // Fill safearray with some values...
     for(int i=1; i<=row; i++) {
        for(int j=1; j<=col; j++) {
           long indices[] = {i,j};
           SafeArrayPutElement(arr.parray, indices, COleVariant((double)i*j*0.123));
        }
     }
 
     // Get ActiveSheet object
     IDispatch *pXlSheet;
     {
        VARIANT result;
        VariantInit(&result);
        AutoWrap(DISPATCH_PROPERTYGET, &result, pXlApp, L"ActiveSheet", 0);
        pXlSheet = result.pdispVal;
     }


     // Get Range object for the Range A1:O15...
     IDispatch *pXlRange;
     {
        VARIANT parm;
        parm.vt = VT_BSTR;
        parm.bstrVal = ::SysAllocString(L"F10:Q27");

        VARIANT result;
        VariantInit(&result);
        AutoWrap(DISPATCH_PROPERTYGET, &result, pXlSheet, L"Range", 1, parm);
        VariantClear(&parm);

        pXlRange = result.pdispVal;
     }

     // Set range with our safearray...
//     AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlRange, L"Value", 1, arr);


     // Wait for user...
  //   ::MessageBox(NULL, "All done.", "Notice", 0x10000);

     // Set .Saved property of workbook to TRUE so we aren't prompted
     // to save when we tell Excel to quit...
     {
        VARIANT x;
        x.vt = VT_I4;
        x.lVal = 1;
        AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlBook, L"Saved", 1, x);
     }

     // Tell Excel to quit (i.e. App.Quit)
  //   AutoWrap(DISPATCH_METHOD, NULL, pXlApp, L"Quit", 0);

     // Release references...
     pXlRange->Release();
     pXlSheet->Release();
     pXlBook->Release();
   }
   else {
     // Tell Excel to quit (i.e. App.Quit)
     AutoWrap(DISPATCH_METHOD, NULL, pXlApp, L"Quit", 0);
     CString str;
     str.Format(_TR("Файл %s не открывается"), path);
     AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
   }

   pXlBooks->Release();
   pXlApp->Release();
   VariantClear(&arr);

   // Uninitialize COM for this thread...
//   CoUninitialize();

  return 0;
}
