#include "stdafx.h"
#include <ole2.h> 
#include "crack.h"

void ErrorMsg1(const char *s, int code);

// AutoWrap() - Automation helper function...
HRESULT AutoWrap(int autoType, VARIANT *pvResult, IDispatch *pDisp, LPOLESTR ptName, int cArgs...);

#include "excel.h"

int CExcel::init_arr(int _row, int _col)
{
 // Create a 15x15 safearray of variants...

   row = _row, col = _col;

   arr.vt = VT_ARRAY | VT_VARIANT;
   {
      SAFEARRAYBOUND sab[2];
      sab[0].lLbound = 1; sab[0].cElements = row;
      sab[1].lLbound = 1; sab[1].cElements = col;
      arr.parray = SafeArrayCreate(VT_VARIANT, 2, sab);
   }

   return 1;
}

int CExcel::set_arr_str(CRange r, CString var)
{
  return set_arr(r, COleVariant(var));
}


int CExcel::set_arr(CRange r, COleVariant & var)
{
  set_range(r);

  if (var.vt == VT_BSTR) {
    CString str = CCrack::strVARIANT2(& var);
    if (str.IsEmpty()) return 1;
    var = COleVariant(str);
  }

  long indices[] = {r.r1,r.c1};



  SafeArrayPutElement(arr.parray, indices, &var);

//  set_typ2(r, var.vt);
  
  return 1;
}


int CExcel::set_arr2(CRange r, COleVariant & var)
{
  set_range(r);

  if (var.vt == VT_BSTR) {
    CString str = CCrack::strVARIANT2(& var);
    if (str.IsEmpty()) return 1;
    var = COleVariant(str);
  }

  long indices[] = {r.r1,r.c1};
  SafeArrayPutElement(arr.parray, indices, &var);
  return 1;
}

CString NtoA(int n)
{
  if (n < 1) return "";
    
  char AA[10];
  if (n <= 26) {
    AA[0] = 'A'+n-1;
    AA[1] = 0;
  }
  else {
    AA[0] = 'A'+(n-1)/26-1;
    AA[1] = 'A'+(n-1)%26;
    AA[2] = 0;
  }
  return AA;
}



void NtoA(int n, wchar_t *A)
{
  if (n <= 26) {
    A[0] = L'A'+n-1;
    A[1] = 0;
  }
  else {
    A[0] = L'A'+(n-1)/26-1;
    A[1] = L'A'+(n-1)%26;
    A[2] = 0;
  }
}


void AtoN(const char *A, int &r, int &c)
{
  c = A[0]-'A'+1;
  sscanf(&A[1], "%d", &r);
}


//HRESULT AutoWrap(int autoType, VARIANT *pvResult, IDispatch *pDisp, LPOLESTR ptName, int cArgs...) {


HRESULT put_long(IDispatch *pDisp, LPOLESTR ptName, long v) 
{
  VARIANT x;
  x.vt = VT_I4;
  x.lVal = v;

  HRESULT ret = AutoWrap(DISPATCH_PROPERTYPUT, NULL, pDisp, ptName, 1, x);
  
  return ret;
}

HRESULT put_val(IDispatch *pDisp, LPOLESTR ptName, COleVariant &val) 
{
  HRESULT ret = AutoWrap(DISPATCH_PROPERTYPUT, NULL, pDisp, ptName, 1, val);

  ErrorMsg1("", ret);
  
  return ret;
}



HRESULT get_long(IDispatch *pDisp, LPOLESTR ptName, long &v) 
{
  VARIANT result;
  VariantInit(&result);

  HRESULT ret = AutoWrap(DISPATCH_PROPERTYGET, &result, pDisp, ptName, 0);

  v = result.lVal;

  return ret;
}



IDispatch *get_property(IDispatch *pDisp, LPOLESTR ptName) 
{

  VARIANT result;
  VariantInit(&result);
  HRESULT ret = AutoWrap(DISPATCH_PROPERTYGET, &result, pDisp, ptName, 0);

  return result.pdispVal;
}


VARIANT get_val(IDispatch *pDisp, LPOLESTR ptName) 
{

  VARIANT result;
  VariantInit(&result);
  HRESULT ret = AutoWrap(DISPATCH_PROPERTYGET, &result, pDisp, ptName, 0);

  return result;
}


IDispatch *get_item(IDispatch *pDisp, long index) 
{
  VARIANT x;
  x.vt = VT_I4;
  x.lVal = index;

  VARIANT result;
  VariantInit(&result);

  HRESULT ret = AutoWrap(DISPATCH_PROPERTYGET, &result, pDisp, L"Item", 1, x);

  return result.pdispVal;
}



void run_method(IDispatch *pDisp, LPOLESTR ptName) 
{
  AutoWrap(DISPATCH_METHOD, NULL, pDisp, ptName, 0);
}

IDispatch *CExcel::getRange(int r1, int c1, int r2, int c2)
{
   // Get Range object for the Range A1:O15...
   IDispatch *pXlRange;
   VARIANT parm;
   parm.vt = VT_BSTR;

   wchar_t addr[256], l1[5], l2[5];

   NtoA(c1, l1);
   NtoA(c2, l2);
   swprintf(addr, 256, L"%s%d:%s%d", l1, r1, l2, r2);

   parm.bstrVal = ::SysAllocString(addr);

   VARIANT result;
   VariantInit(&result);
   AutoWrap(DISPATCH_PROPERTYGET, &result, pXlSheet, L"Range", 1, parm);
   VariantClear(&parm);

   pXlRange = result.pdispVal;

   return pXlRange;
}


IDispatch *getCell(IDispatch *pXlSheet, int r1, int c1)
{
   IDispatch *pXlCell;
   VARIANT parm[2];


   parm[0].vt = VT_I2;
   parm[0].iVal = r1;
   parm[1].vt = VT_I2;
   parm[1].iVal = c1;


   VARIANT result;
   VariantInit(&result);


   AutoWrap(DISPATCH_PROPERTYGET, &result, pXlSheet, L"Cells", 2, parm);

   pXlCell = result.pdispVal;

   return pXlCell;
}

IDispatch *getFont(IDispatch *pCell)
{
    return get_property(pCell, L"Font");
}



int CExcel::open(const char *path)
{
   isOpen = true;

   
   // Get CLSID for our server...
   CLSID clsid;
   HRESULT hr = CLSIDFromProgID(L"Excel.Application", &clsid);

   if(FAILED(hr)) {
//      ::MessageBox(NULL, "CLSIDFromProgID() failed", "Error", 0x10010);
      ErrorMsg1("CLSIDFromProgID() failed", hr);
      return -1;
   }

   // Start server and get IDispatch...
   hr = CoCreateInstance(clsid, NULL, CLSCTX_LOCAL_SERVER, IID_IDispatch, (void **)&pXlApp);
   if(FAILED(hr)) {
      ErrorMsg1("Excel не зарегистрирован правильно", hr);
//      ::MessageBox(NULL, "Excel not registered properly", "Error", 0x10010);
      return -2;
   }

   put_long(pXlApp, L"Visible", 0) ;
   pXlBooks = get_property(pXlApp, L"Workbooks");
   
   
   if (path) {
//    pXlBook = get_property(pXlBooks, L"Add");

      VARIANT result;
      VariantInit(&result);
      COleVariant parm(path);
      hr = AutoWrap(DISPATCH_METHOD, &result, pXlBooks, L"Open", 1, parm);
      if(FAILED(hr)) {
        
        isOpen = false;

  CString str;
  static char buf[256];
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 0, buf, sizeof buf, NULL);
  str.Format(_TR("Ошибка %d\n%s"), hr, buf);
//  AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);



      }
      pXlBook = result.pdispVal;

   }
   else {
    pXlBook = get_property(pXlBooks, L"Add");
   }

    setTab(-1);


   if (!isOpen) {
     run_method(pXlApp, L"Quit");

     if (path) {
       CString str;
       str.Format(_TR("Файл %s не открывается\nошибка %x"), path, hr);
       AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
     }
   }



  return 0;
}


CExcel::~CExcel()
{
   put_long(pXlApp, L"Visible", -1L) ;

   
   if (pXlBook) pXlBook->Release();
   if (pXlBooks) pXlBooks->Release();
   if (pXlApp) pXlApp->Release();
   VariantClear(&arr);

   delete wait;

   ShowExcel();
}


int CExcel::setTab(int ntab)
{
  if (!isOpen) return 0;

  if (ntab >= 0) {
    IDispatch *pXlWorksheets = get_property(pXlBook, L"Worksheets");
    IDispatch *pXlWorksheet = get_item(pXlWorksheets, ntab);
    pXlSheet = pXlWorksheet;
  }
  else {
    pXlSheet = get_property(pXlApp, L"ActiveSheet");
  }

  return 0;
}


int CExcel::copy(CRange r, int ntab)
{
  setTab(ntab);

   {
//     IDispatch *pXlRange = getRange(1, 1, 1, 1);
     IDispatch *pXlRange = getRange(r.r1, r.c1, r.r2, r.c2);
  //     IDispatch *pXlRange = getCell(1, 1);
  //     run_method(pXlRange, L"Paste");
  //     run_method(pXlSheet, L"Paste");

     AutoWrap(DISPATCH_METHOD, NULL, pXlRange, L"Select", 0);
     AutoWrap(DISPATCH_METHOD, NULL, pXlSheet, L"Paste", 0);

     if (pXlRange) pXlRange->Release();
   }

   put_long(pXlBook, L"Saved", 1) ;

   pXlSheet->Release();
 
   return 1;
}


void ShowExcel()
{
   HWND wnd = ::FindWindowEx(0, 0, "XLMAIN", 0);      
   
   if (wnd) {
//    WINDOWPLACEMENT wpi;
//    ::GetWindowPlacement(wnd, &wpi); 

//    if (wpi.showCmd != SW_RESTORE) {
       ::ShowWindow(wnd, SW_MINIMIZE);
       ::ShowWindow(wnd, SW_RESTORE);
//     }
   }
}

int CExcel::example2(CRange r, int ntab)
{
  if (!isOpen) return 0;


  if (ntab >= 0) {
    IDispatch *pXlWorksheets = get_property(pXlBook, L"Worksheets");
    IDispatch *pXlWorksheet = get_item(pXlWorksheets, ntab);
    pXlSheet = pXlWorksheet;
  }
  else {
    pXlSheet = get_property(pXlApp, L"ActiveSheet");
  }


  {
    IDispatch *pXlRange = getRange(r.r1+1, r.c1+1, row2+r.r1, col+r.c1);
    // Set range with our safearray...
    AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlRange, L"Value", 1, arr);
    if (pXlRange) pXlRange->Release();
  }


   if (0) {
      IDispatch *pXlRange = getRange(19, 1, row, col);

      IDispatch *pXlBorders = get_property(pXlRange, L"Borders");

      IDispatch *pXlItem = get_item(pXlBorders, 2);
      if (pXlItem) {
        put_long(pXlItem, L"LineStyle", 1) ;
        pXlItem->Release();
      }

      pXlBorders->Release();

      pXlRange->Release();
   }



   // Set .Saved property of workbook to TRUE so we aren't prompted
   // to save when we tell Excel to quit...
    put_long(pXlBook, L"Saved", 1) ;

   // Tell Excel to quit (i.e. App.Quit);
//   run_method(pXlApp, L"Quit") 

   // Release references...
   pXlSheet->Release();
   
   return 1;
}

int CExcel::set_str(CRange r, CString str)
{
  COleVariant var = COleVariant(str);
  return set_var(r, var);
}

void CExcel::set_range(CRange r)
{
  row1 = min(row1, r.r1);
  row2 = max(row1, r.r2);
  col1 = min(col1, r.c1);
  col2 = max(col1, r.c2);
}

int CExcel::set_typ(CRange r, int typ)
{
//  pXlSheet = get_property(pXlApp, L"ActiveSheet");

  IDispatch *pXlRange = getRange(r.r1, r.c1, r.r2, r.c2);
  if (pXlRange) {

    CString f = "General";

    switch(typ) {
    case dbText:
      f = "@";
      break;
    case dbDouble:
    case dbLong:
//      f = "0";
      break;
    case dbDate:
      f = _TR("ДД.ММ.ГГГГ");
      break;
    }
    put_val(pXlRange, L"NumberFormat", COleVariant(f));
    pXlRange->Release();
  }
  return 0;
}


int CExcel::set_typ2(CRange r, int typ)
{
  IDispatch *pXlRange = getRange(r.r1, r.c1, r.r2, r.c2);
  if (pXlRange) {

    CString f = "General";

    switch (typ) {
    case adVarWChar:
      f = "@";
      break;
    case adDouble:
    case adSingle:
    case adInteger:
      //      f = "0";
      break;
    case adDate:
    case adDBDate:
      f = _TR("ДД.ММ.ГГГГ");
      break;
    }
    put_val(pXlRange, L"NumberFormat", COleVariant(f));
    pXlRange->Release();
  }
  return 0;
}

int CExcel::set_v_align(CRange r, int typ)
{
  IDispatch *pXlRange = getRange(r.r1, r.c1, r.r2, r.c2);
  if (pXlRange) {
    put_val(pXlRange, L"VerticalAlignment", COleVariant((long) typ));
    pXlRange->Release();
  }
  
  return 0;
}

int CExcel::set_h_align(CRange r, int typ)
{
  IDispatch *pXlRange = getRange(r.r1, r.c1, r.r2, r.c2);
  if (pXlRange) {
    put_val(pXlRange, L"HorizontalAlignment", COleVariant((long) typ));
    pXlRange->Release();
  }
  
  return 0;
}

int CExcel::set_border(CRange r, int typ)
{
    IDispatch *pXlRange = getRange(r.r1, r.c1, r.r2, r.c2);
    if (pXlRange) {
        IDispatch *pXlBorders = get_property(pXlRange, L"Borders");

        if (pXlBorders) {
            put_long(pXlBorders, L"LineStyle", 1);
            pXlBorders->Release();
        }

        pXlRange->Release();
    }
    return 0;
}


int CExcel::set_bold(CRange r)
{
  IDispatch *pXlRange = getRange(r.r1, r.c1, r.r2, r.c2);
  if (pXlRange) {
    IDispatch *pXlFont = getFont(pXlRange);
    if (pXlFont) {
        put_val(pXlFont, L"Bold", COleVariant((long) 1));
        pXlFont->Release();
    }
    pXlRange->Release();
  }
  return 0;
}


int CExcel::set_hv_align(CRange r, int typH, int typV)
{
  IDispatch *pXlRange = getRange(r.r1, r.c1, r.r2, r.c2);
  if (pXlRange) {
    put_val(pXlRange, L"HorizontalAlignment", COleVariant((long) typH));
    put_val(pXlRange, L"VerticalAlignment", COleVariant((long) typV));
    pXlRange->Release();
  }
  
  return 0;
}


int CExcel::set_val(CRange r, const wchar_t *field, COleVariant &val )
{
  IDispatch *pXlRange = getRange(r.r1, r.c1, r.r2, r.c2);
  if (pXlRange) {
    put_val(pXlRange, (LPOLESTR)field, val);
    pXlRange->Release();
  }
  
  return 0;
}





/*

int CExcel::set_typ2(CRange r, int typ)
{
  //  pXlSheet = get_property(pXlApp, L"ActiveSheet");

  IDispatch *pXlRange = getRange(r.r1, r.c1, r.r2, r.c2);
  if (pXlRange) {

    CString f = "General";

    switch (typ) {
    case VT_BSTR:
      f = "@";
      break;
    case VT_R8:
    case VT_R4:
    case VT_I4:
      //      f = "0";
      break;
    case VT_DATE:
      f = _TR("ДД.ММ.ГГГГ");
      break;
    }
    put_val(pXlRange, L"NumberFormat", COleVariant(f));


//  AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlRange, L"NumberFormat", 1, COleVariant(f));

    pXlRange->Release();
  }
  return 0;
}
*/


int CExcel::set_var(CRange r, COleVariant & var)
{
  set_range(r);

  VARIANT arr_v;

  arr_v.vt = VT_ARRAY | VT_VARIANT;
  {
     SAFEARRAYBOUND sab[2];
     sab[0].lLbound = 1; sab[0].cElements = 1;
     sab[1].lLbound = 1; sab[1].cElements = 1;
     arr_v.parray = SafeArrayCreate(VT_VARIANT, 2, sab);
  }
  
  long indices[] = {1,1};

  SafeArrayPutElement(arr_v.parray, indices, &var);

//  pXlSheet = get_property(pXlApp, L"ActiveSheet");

  {
     IDispatch *pXlRange = getRange(r.r1, r.c1, r.r2, r.c2);
    // Set range with our safearray...

    AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlRange, L"Value", 1, arr_v);


    if (pXlRange) pXlRange->Release();
  }

  return 1;
}


/*

int CExcel::property(Crange r)
{
  if (!isOpen) return 0;


   pXlSheet = get_property(pXlApp, L"ActiveSheet");

   {
      IDispatch *pXlRange = getRange(1, 1, row, col);
     // Set range with our safearray...
     AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlRange, L"Value", 1, arr);
     if (pXlRange) pXlRange->Release();
   }

   {
      IDispatch *pXlRange = getRange(row, col, row, col);


      IDispatch *pCell = getCell(pXlApp, row, col);

      IDispatch *pComment = get_property(pXlRange, L"Comment");
      IDispatch *pShape = get_property(pComment, L"Shape");
      IDispatch *pFill = get_property(pShape, L"Fill");

      IDispatch *pForeColor = get_property(pFill, L"ForeColor");
      IDispatch *pBackColor = get_property(pFill, L"BackColor");


      IDispatch *pUserPicture = get_property(pFill, L"UserPicture");
      IDispatch *pUserTextured = get_property(pFill, L"UserTextured");


      COleVariant pat  = get_val(pFill, L"Pattern");
      COleVariant v = get_val(pFill, L"UserPicture");


        VARIANT parm;
        parm.vt = VT_BSTR;
        parm.bstrVal = ::SysAllocString(L"E:\\!БД_Алматы_2016\\Документы\\УТ-12.3.jpg");


      put_long(pFill, L"Visible", 0L);
      put_val(pFill, L"UserPicture", COleVariant(parm));
      put_long(pFill, L"Visible", -1L);


//      COleVariant rgb = get_val(pBackColor, L"RGB");

//      long r1, r2;

//      get_long(pBackColor, L"RGB", r1);
//      get_long(pForeColor, L"RGB", r2);

//      put_long(pBackColor, L"RGB", 0x00FF00);
//      put_long(pForeColor, L"RGB", 0x00FF00);

      
//      IDispatch *pForeColor = get_property(pFill, L"ForeColor");
//      COleVariant vv = get_val(pShape, L"AlternativeText");

      pXlRange->Release();
   }



   // Set .Saved property of workbook to TRUE so we aren't prompted
   // to save when we tell Excel to quit...
    put_long(pXlBook, L"Saved", 1) ;

   // Tell Excel to quit (i.e. App.Quit);
//   run_method(pXlApp, L"Quit") 

   // Release references...
   pXlSheet->Release();

   return 1;
}

*/

CRange::CRange(const char * A)
{
   AtoN(A, r1, c1);
   r2 = r1; c2 = c1;
}
