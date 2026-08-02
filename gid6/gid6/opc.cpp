#include "stdafx.h"
#include "resource.h"
#include "opc.h"
#include "crack.h"


HRESULT __fastcall UnicodeToAnsi(LPCOLESTR pszW, LPSTR* ppszA);
HRESULT __fastcall AnsiToUnicode(LPCSTR pszA, LPOLESTR* ppszW);

static IMalloc *pIMalloc = NULL;

int OPCError::error_code;
char OPCError::error_txt[256];

OPCError::OPCError(const char *s, int code) {
  error_code = code;
  error_txt[0] = 0;
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, code, 0, error_txt, sizeof error_txt, NULL);
}


void ErrorMsg(const char *s, int code) { 
  CString str;
  static char buf[256];
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, code, 0, buf, sizeof buf, NULL);
  str.Format(_TR("Ошибка %d\n%s\n%s"), code, s, buf);
//  AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
  throw new OPCError(s, code);
//  throw 0;
}

void ErrorMsg1(const char *s, int code) { 
  CString str;
  static char buf[256];
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, code, 0, buf, sizeof buf, NULL);
  str.Format(_TR("Ошибка %d\n%s\n%s"), code, s, buf);
//  AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
}


//---------------------------------------------------------
// LocalInit                          z
// This is generic initialization for a task using COM
void LocalInit(void)
{
  HRESULT r1;
  // General COM initialization...
  //
  r1 = CoInitialize(NULL);
//  r1 = CoInitializeEx(NULL, COINIT_MULTITHREADED );

  if (FAILED(r1))
  {
    ErrorMsg("Error from CoInitialize", r1);
//    exit(1);
  }

  static int ini = 0;

  // This is for DCOM
  //
  if (!ini) {
  r1 = CoInitializeSecurity(
      NULL,   //Points to security descriptor 
      -1,     //Count of entries in asAuthSvc 
      NULL,   //Array of names to register 
      NULL,   //Reserved for future use 
      RPC_C_AUTHN_LEVEL_NONE,    //The default authentication level for proxies
      RPC_C_IMP_LEVEL_IMPERSONATE,//The default impersonation level for proxies
      NULL,                      //Reserved; must be set to  NULL 
      EOAC_NONE,                 //Additional client or server-side capabilities
      NULL                       //Reserved for future use 
      );
  if (FAILED(r1))
  {
    ErrorMsg("Error from CoInitializeSecurity", r1);
//    CoUninitialize();
//    exit(1);
  }
  ini = 1;
  }


  // Also get access to the COM memory manager
  //
  r1 = CoGetMalloc(MEMCTX_TASK, &pIMalloc);

  if (FAILED(r1))
  {
    ErrorMsg("GetMalloc failed", r1);
//    CoUninitialize();
//    exit(1);
  }
}


void LocalInit2(void)
{
  HRESULT r1;
  // General COM initialization...
  //
  r1 = CoInitialize(NULL);
//  r1 = CoInitializeEx(NULL, COINIT_MULTITHREADED );

  if (FAILED(r1))
  {
    ErrorMsg("Error from CoInitialize", r1);
//    exit(1);
  }

/*
  // This is for DCOM
  //
  r1 = CoInitializeSecurity(
      NULL,   //Points to security descriptor 
      -1,     //Count of entries in asAuthSvc 
      NULL,   //Array of names to register 
      NULL,   //Reserved for future use 
      RPC_C_AUTHN_LEVEL_NONE,    //The default authentication level for proxies
      RPC_C_IMP_LEVEL_IMPERSONATE,//The default impersonation level for proxies
      NULL,                      //Reserved; must be set to  NULL 
      EOAC_NONE,                 //Additional client or server-side capabilities
      NULL                       //Reserved for future use 
      );
  if (FAILED(r1))
  {
    ErrorMsg("Error from CoInitializeSecurity", r1);
//    CoUninitialize();
//    exit(1);
  }
*/
  // Also get access to the COM memory manager
  //
  r1 = CoGetMalloc(MEMCTX_TASK, &pIMalloc);

  if (FAILED(r1))
  {
    ErrorMsg("GetMalloc failed", r1);
//    CoUninitialize();
//    exit(1);
  }
}


void LocalCleanup(void)
{
  // Finally, release the memory manager
  // as well as COM
  //
  if (pIMalloc) pIMalloc->Release();
  CoUninitialize();
}

static COSERVERINFO sinp;
static COSERVERINFO *sinptr = 0;
static DWORD clsctx = CLSCTX_LOCAL_SERVER;

static COAUTHINFO ai;
static COAUTHIDENTITY aid;

void SetComputer(const char *computer, const char *domain, const char *name, const char *pwd)
{
  WCHAR *szNodeName;
  WCHAR *szDomain;
  WCHAR *szName;
  WCHAR *szPwd;

  if (computer && computer[0])
  {
    AnsiToUnicode(computer, &szNodeName);
    AnsiToUnicode(domain, &szDomain);
    AnsiToUnicode(name, &szName);
    AnsiToUnicode(pwd, &szPwd);
    sinptr = &sinp;
    sinp.dwReserved1 = 0;
    sinp.dwReserved2 = 0;
   
    sinp.pwszName = szNodeName;

    {
      ai.dwAuthnSvc = RPC_C_AUTHN_WINNT;
      ai.dwAuthzSvc = RPC_C_AUTHZ_NONE;
      ai.pwszServerPrincName = NULL;
      ai.dwAuthnLevel = RPC_C_AUTHN_LEVEL_DEFAULT ;
      ai.dwImpersonationLevel = RPC_C_IMP_LEVEL_IMPERSONATE;

      aid.User = (USHORT *)szName;
      aid.UserLength = strlen(name);;
      aid.Domain = (USHORT *)szDomain;
      aid.DomainLength = strlen(domain);
      aid.Password = (USHORT *)szPwd;
      aid.PasswordLength = strlen(pwd);;
      aid.Flags = 0x2;

      ai.pAuthIdentityData = &aid;
      ai.dwCapabilities = EOAC_NONE;
    }

    sinp.pAuthInfo = &ai;

//    sinp.pAuthInfo = 0;
    clsctx = CLSCTX_REMOTE_SERVER;
  } else
  {
    // If NODE is Nul then try local server
    sinptr = 0;   // pointer should be NULL if local
    clsctx = CLSCTX_LOCAL_SERVER;
  }
}

void DumpVariant(char *buf, VARIANT *v, double a)
{
  CString strRet;

  if (a != 1.) {
    double val = 0;
    switch(v->vt)
    {
    case VT_I2:
    case VT_UI2:
      val = v->iVal;
      break;
    case VT_UI4:
    case VT_I4:
      val = v->lVal;
      break;
    case VT_R4:
      val = v->fltVal;
      break;
    case VT_R8:
      val = v->dblVal;
      break;
    default:
      goto LABEL;
      break;
    }
    sprintf(buf, "%g", val*a);
    return;
  }

LABEL:

  switch(v->vt)
  {
  case VT_I2:
    sprintf(buf, "%d", v->iVal );
    break;
  case VT_UI2:
    sprintf(buf, "%d", v->iVal );
    break;
  case VT_I4:
    sprintf(buf, "%ld", v->lVal );
    break;
  case VT_UI4:
    sprintf(buf, "%ld", v->lVal );
    break;
  case VT_R4:
    sprintf(buf, "%g", v->fltVal );
    break;
  case VT_R8:
    sprintf(buf, "%g", v->dblVal );
    break;
  case VT_BSTR:
    sprintf(buf, "%ls", v->bstrVal );
    break;
  case VT_BOOL:
    sprintf(buf, "%s", v->boolVal ? "On" : "Off");
    break;
  case VT_DATE:
    strRet = COleDateTime(*v).Format(_T("%d.%m.%Y"));
    strcpy(buf, strRet);
    break;

  default:
    sprintf(buf, "vt%d", v->vt );
    break;
  }
}

void DumpVariant2(char *buf, VARIANT *v)
{
  switch(v->vt)
  {
  case VT_I2:
    sprintf(buf, "(VT_I2)   = %d ", v->iVal );
    break;
  case VT_I4:
    sprintf(buf, "(VT_I4)   = %ld ", v->lVal );
    break;
  case VT_R4:
    sprintf(buf, "(VT_R4)   = %.8f ", v->fltVal );
    break;
  case VT_R8:
    sprintf(buf, "(VT_R8)   = %.8f ", v->dblVal );
    break;
  case VT_BSTR:
    sprintf(buf, "(VT_BSTR) = %ls ", v->bstrVal );
    break;
  case VT_BOOL:
    sprintf(buf, "(VT_BOOL) = %s ", v->boolVal ? "On" : "Off");
    break;
  default:
    sprintf(buf, "(unknown type:%d) ", v->vt );
    break;
  }
}


OPCServer::OPCServer(const CLSID & clsid)
{
  gpOPCS = 0;
  gpOPCBA = 0;

  HRESULT r;
  MULTI_QI mqi[2];
  IEnumGUID *enumID = 0;

  // set up mqi
  //
  mqi[0].pIID = &IID_IOPCServer;
  mqi[0].hr = 0;
  mqi[0].pItf = 0;

  mqi[1].pIID = &IID_IOPCBrowseServerAddressSpace;
  mqi[1].hr = 0;
  mqi[1].pItf = 0;

  // Note you must define _WIN32_DCOM in 'Settings'
  r = CoCreateInstanceEx(clsid, NULL, clsctx, sinptr, 2, mqi);

  if (FAILED(r) || FAILED(mqi[0].hr) || FAILED(mqi[1].hr))
  {
    ErrorMsg("CoCreateInstanceEx ", r);
    return;
  }

  gpOPCS = (IOPCServer *)mqi[0].pItf;
  gpOPCBA = (IOPCBrowseServerAddressSpace *)mqi[1].pItf;

  r = CoSetProxyBlanket(gpOPCBA, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, 2, &aid, 0);
  r = CoSetProxyBlanket(gpOPCS, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, 2, &aid, 0);

  OPCSERVERSTATUS * pServerStatus;
  r = gpOPCS->GetStatus( &pServerStatus );

  if (FAILED(r))
  {
    ErrorMsg("CoCreateInstanceEx ", r);
    return;
  }


  if (pServerStatus) {
    char *s;
    UnicodeToAnsi(pServerStatus->szVendorInfo, &s);
//    if (s[0]) AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);

    CoTaskMemFree(s);
    CoTaskMemFree(pServerStatus);
  }

  OPCNAMESPACETYPE  NameSpaceType;
  r = gpOPCBA->QueryOrganization(&NameSpaceType);
}

OPCServer::~OPCServer()
{
  gpOPCS->Release();
  gpOPCBA->Release();
}

#include "mmenu.h"

void OPCServer::Menu2(void *tree)
{
  map_hn.clear();
  lst.RemoveAll();
  lst2.RemoveAll();
  CTreeCtrl *tree1 = (CTreeCtrl *) tree;
  write_tree(tree1, TVI_ROOT);
}


OPCGroup::OPCGroup(OPCServer *server)
{
  pOPC = server->gpOPCS;
  HRESULT r1;
  DWORD RevisedRate1;
  float DeadBand1 = (float)0.0;
  HRESULT r4, r6;
//  HRESULT r9;

  pGRP1U = 0;
  pGRP1GSM = 0;
  pGRP1PGSM = 0;
  pGRP1SIO = 0;
  pGRP1ASIO = 0;
  pGRP1IM = 0;
  pGRP1DO = 0;
//  pGRP1CPC = 0;

  if(pGRP1U == 0)
  {
//--    printf("First time through: Creating groups...\n");

    r1 = pOPC->AddGroup( L"TestGroup1", TRUE, 0, 0, 0, &DeadBand1,
      0, &hServerGroup1, &RevisedRate1, IID_IUnknown, 
      (LPUNKNOWN*)&pGRP1U);

    if (FAILED(r1))
    {
      ErrorMsg("Error from AddGroup", r1);
    } 
    else
    {
//--          printf("Checking Interfaces on Group 1...\n");

      r4 = CoSetProxyBlanket(pGRP1U, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, 2, &aid, 0);

//      r2 = pGRP1U->QueryInterface(IID_IOPCGroupStateMgt, (void**)&pGRP1GSM);
//      r3 = pGRP1U->QueryInterface(IID_IOPCPublicGroupStateMgt, (void**)&pGRP1PGSM);
      r4 = pGRP1U->QueryInterface(IID_IOPCSyncIO, (void**)&pGRP1SIO);
//      r5 = pGRP1U->QueryInterface(IID_IOPCAsyncIO, (void**)&pGRP1ASIO);
      r6 = pGRP1U->QueryInterface(IID_IOPCItemMgt, (void**)&pGRP1IM);
//      r8 = pGRP1U->QueryInterface(IID_IDataObject, (void**)&pGRP1DO);
    }
  }
}


OPCGroup::~OPCGroup()
{
  HRESULT r1;
  // When done
  // Release the Group interface 
  // and remove the group from the server
  // (Which should take care of deleting any items)
  //
  if(!pGRP1U)
  {
    return;
  }

  if(pGRP1GSM) pGRP1GSM->Release();
  if(pGRP1PGSM) pGRP1PGSM->Release();
  if(pGRP1SIO) pGRP1SIO->Release();
  if(pGRP1ASIO) pGRP1ASIO->Release();
  if(pGRP1IM) pGRP1IM->Release();
  if(pGRP1DO) pGRP1DO->Release();
//  if(pGRP1CPC) pGRP1CPC->Release();

  if(pGRP1U)
  {
    pGRP1U->Release();
    r1 = pOPC->RemoveGroup(hServerGroup1, FALSE);
    if (FAILED(r1))
    {
      ErrorMsg("Error from RemoveGroup", r1);
    } 
    pGRP1U = 0;
  }
}

void split(CString s, CString & s1, double &a);


OPCItem::OPCItem(OPCGroup *group, const char *fn, double a)
{
  _a = a;

  pIM = group->pGRP1IM;
  gr = group;

  HRESULT r1;
  OPCITEMDEF id[2];
  HRESULT *ih;
  OPCITEMRESULT *ir;

  r1 = CoSetProxyBlanket(pIM, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, 2, &aid, 0);


  n_item0 = n_item = 1;
  g_sh = new OPCHANDLE[n_item];
  otn = new int[n_item];

  LPOLESTR wstr;

  CString fn1;

  split(fn, fn1, _a);

  AnsiToUnicode(fn1, &wstr);

  // Define two items
  //
  for (int i = 0; i < n_item; i++) {
    id[i].szItemID = wstr;
    id[i].szAccessPath = L"";
    id[i].bActive = TRUE;
    id[i].hClient = 100;
    id[i].hClient = i;
    id[i].dwBlobSize = 0;
    id[i].pBlob = NULL;
    id[i].vtRequestedDataType = VT_EMPTY;
  }

  // Add then items and check the hresults
  //
  r1 = pIM->AddItems(1, id, &ir, &ih);
  if (FAILED(r1))
  {
    ErrorMsg("Error from AddItems", r1);
    return;
  } 
  else
  {
    // Check the results of each item
    int i;

    for(i=0; i < n_item; i++)
    {
      if (FAILED(ih[i]))
      {
        ErrorMsg("Error from AddItems", ih[i]);
        // zzz more robust error here would be nice 
        // (to prevent use of bad items later)
      } else
      {
        otn[i] = i;
        g_sh[i] = ir[i].hServer;
        if(ir[i].pBlob) pIMalloc->Free(ir[i].pBlob);
      }
    }

    // Free the returned HRESULTs and ITEMRESULTS
    //
    pIMalloc->Free(ir);
    pIMalloc->Free(ih);
  }
}


OPCItem::OPCItem(OPCGroup *group, const CArray<CString,CString> & fn)
{
  pIM = group->pGRP1IM;
  gr = group;

  n_item0 = n_item = fn.GetSize();
  g_sh = new OPCHANDLE[n_item];
  otn = new int[n_item];

  HRESULT r1;
  OPCITEMDEF *id = new OPCITEMDEF[n_item];
  HRESULT *ih;
  OPCITEMRESULT *ir;
  int i, j;

  LPOLESTR wstr;

  for ( i = 0; i < n_item; i++) {
    AnsiToUnicode(fn[i], &wstr);
    id[i].szItemID = wstr;
    id[i].szAccessPath = L"";
    id[i].bActive = TRUE;
    id[i].hClient = 100;
    id[i].dwBlobSize = 0;
    id[i].pBlob = NULL;
    id[i].vtRequestedDataType = VT_EMPTY;
  }

  // Add then items and check the hresults
  //
  r1 = pIM->AddItems(n_item, id, &ir, &ih);
  if (FAILED(r1))
  {
    ErrorMsg("Error from AddItems", r1);
    return;
  } 
  else
  {
    // Check the results of each item

    for(j = i = 0; i < n_item; i++)
    {
      if (FAILED(ih[i]))
      {
//        g_sh[i] = -1;
//        ErrorMsg("Error from AddItems", ih[i]);
        // zzz more robust error here would be nice 
        // (to prevent use of bad items later)
      } else
      {
//        g_sh[i] = ir[i].hServer;
        otn[j] = i;
        g_sh[j++] = ir[i].hServer;
        if(ir[i].pBlob) pIMalloc->Free(ir[i].pBlob);
      }
    }

    // Free the returned HRESULTs and ITEMRESULTS
    //
    pIMalloc->Free(ir);
    pIMalloc->Free(ih);
  }

  for ( i = 0; i < n_item; i++) {
    CoTaskMemFree(id[i].szItemID);
  }

  n_item = j;

  delete [] id;
}

OPCItem::~OPCItem()
{
  HRESULT r1;
  HRESULT *ih;

  r1 = pIM->RemoveItems(n_item, g_sh, &ih);
  delete [] g_sh;
  delete [] otn;

  if (FAILED(r1))
  {
    ErrorMsg("Error from RemoveItems", r1);
    return;
  } 
  else
  {
    // Check the results of each item
    int i;

    for(i=0; i < n_item; i++)
    {
      if (FAILED(ih[i]))
      {
        ErrorMsg("Error from RemoveItems", ih[i]);
      } 
    }

    // Free the returned HRESULTs
    //
    pIMalloc->Free(ih);   //acc001
  }
}


void OPCTimeToDateTime(FILETIME t)
{
//  TSystemTime st;

  SYSTEMTIME st;

  FileTimeToLocalFileTime(&t, &t);
  FileTimeToSystemTime(&t, &st);
//  return SystemTimeToDateTime(st);
}


void OPCItem::Read(char *buf)
{
  HRESULT r;
  HRESULT *hr;
  OPCITEMSTATE *is;

  buf[0] = 0;

  CoSetProxyBlanket(gr->pGRP1SIO, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, 2, &aid, 0);

  r = gr->pGRP1SIO->Read(OPC_DS_CACHE, n_item, g_sh, &is, &hr);
//  r = gr->pGRP1SIO->Read(OPC_DS_DEVICE, n_item, g_sh, &is, &hr);

  if (FAILED(r))
  {
    ErrorMsg("Error from Read", r);
  } 
  else
  {
    // if the read worked then show the results
    // (Which are assumed to be VT_R4 for now!)
    //
    for ( int i = 0; i < n_item; i++) {
      if(!FAILED(hr[i])) {
        char s[256];
        DumpVariant(s, &is[i].vDataValue, _a);

//        CString s = CCrack::strVARIANT(is[i].vDataValue);

        OPCTimeToDateTime(is[i].ftTimeStamp);

        
        strcat(buf, s);
        VariantClear(&is[i].vDataValue);
      }
      else {
        ErrorMsg("Error from Read", r);
      }
    }

    // Free returned items
    //
    pIMalloc->Free(hr);
//    VariantClear(&is[1].vDataValue);
    pIMalloc->Free(is);
  }
}


COleVariant OPCItem::Read()
{
  COleVariant var;
  HRESULT r;
  HRESULT *hr;
  OPCITEMSTATE *is;

//  var = COleVariant();

  CoSetProxyBlanket(gr->pGRP1SIO, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, 2, &aid, 0);

  r = gr->pGRP1SIO->Read(OPC_DS_CACHE, n_item, g_sh, &is, &hr);

  if (FAILED(r))
  {
    ErrorMsg("Error from Read", r);
  } 
  else
  {
    // if the read worked then show the results
    // (Which are assumed to be VT_R4 for now!)
    //
    for ( int i = 0; i < n_item; i++) {
      if(!FAILED(hr[i])) {
        var = is[i].vDataValue;
        VariantClear(&is[i].vDataValue);
      }
      else {
        ErrorMsg("Error from Read", r);
      }
    }

    // Free returned items
    //
    pIMalloc->Free(hr);
//    VariantClear(&is[1].vDataValue);
    pIMalloc->Free(is);
  }
  return var;
}


void OPCItem::Read(CArray<CString,CString> & fn)
{
  HRESULT r;
  HRESULT *hr;
  OPCITEMSTATE *is;
  char s[256];
  int i;

  fn.SetSize(n_item0);
  
  r = gr->pGRP1SIO->Read(OPC_DS_CACHE, n_item, g_sh, &is, &hr);

  if (FAILED(r))
  {
    ErrorMsg("Error from Read", r);
  } 
  else
  {
    // if the read worked then show the results
    // (Which are assumed to be VT_R4 for now!)
    //
    for ( i = 0; i < n_item0; i++) {
      fn[i] = "BAD";
    }
    for ( i = 0; i < n_item; i++) {
      if(!FAILED(hr[i])) {
        DumpVariant(s, &is[i].vDataValue, 1.);
        VariantClear(&is[i].vDataValue);
        fn[otn[i]] = s;
      }
    }

    // Free returned items
    //
    pIMalloc->Free(hr);
//    VariantClear(&is[1].vDataValue);
    pIMalloc->Free(is);
  }
}

void OPCItem::Write(COleVariant &var)
{
  HRESULT r;
  HRESULT *hr;
//  OPCITEMSTATE *is;
//  char s[256];
  int i;

  CoSetProxyBlanket(gr->pGRP1SIO, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, 2, &aid, 0);

  r = gr->pGRP1SIO->Write(n_item, g_sh, &var, &hr);

  if (FAILED(r))
  {
    ErrorMsg("Error from Write", r);
  }
  else {
    for ( i = 0; i < n_item; i++) {
      if(FAILED(hr[i])) {
        ErrorMsg("Error from Write", r);
      }
    }
  }
}


void OPCItem::Read(int n, int *nn, CArray<CString,CString> & fn)
{
  int i;
  HRESULT r;
  HRESULT *hr;
  OPCITEMSTATE *is;
  char s[256];
  OPCHANDLE *sh = new OPCHANDLE[n];

  if (n == 0) return;

  fn.SetSize(n_item);

  for ( i = 0; i < n; i++) {
    sh[i] = g_sh[nn[i]];
  }

  r = gr->pGRP1SIO->Read(OPC_DS_CACHE, n, sh, &is, &hr);

  if (FAILED(r))
  {
    ErrorMsg("Error from Read", r);
  } 
  else
  {
    // if the read worked then show the results
    // (Which are assumed to be VT_R4 for now!)
    //
    for ( i = 0; i < n; i++) {
      if(!FAILED(hr[i])) {
        DumpVariant(s, &is[i].vDataValue, 1.);
        fn[i] = s;
      }
      else {
        fn[i] = "BAD";
      }
      VariantClear(&is[i].vDataValue);
    }

    // Free returned items
    //
    pIMalloc->Free(hr);
    pIMalloc->Free(is);
  }

  delete [] sh;
}


void OPCServer::write_tree(CTreeCtrl *tree, HTREEITEM hParent)
{
  HRESULT r;
  HTREEITEM  h;
  char *s;

  IEnumString *enumString = 0;
  r = gpOPCBA->BrowseOPCItemIDs(OPC_BRANCH, L"", VT_EMPTY, 0, &enumString);
  if (FAILED(r)) {
    ErrorMsg("BrowseOPCItemIDs", r);
  }
  r = CoSetProxyBlanket(enumString, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, 2, &aid, 0);

  while (S_OK == r) {
    unsigned long n;
    LPOLESTR str;

    r = enumString->Next(1, &str, &n);
    if (FAILED(r) || n != 1) break;

    UnicodeToAnsi(str, &s);
//    menu->list.Add(s);

    h = tree->InsertItem(s, hParent);
    CoTaskMemFree(s);

    r = gpOPCBA->ChangeBrowsePosition(OPC_BROWSE_DOWN, str);
    if (FAILED(r)) {
//      ErrorMsg("ChangeBrowsePosition", r);
    }
    else {
      write_tree(tree, h);
    }

    r = gpOPCBA->ChangeBrowsePosition(OPC_BROWSE_UP, L"");
    if (FAILED(r)) {
      break;
//      ErrorMsg("ChangeBrowsePosition", r);

    }
  }
  if (enumString) enumString->Release();

  r = gpOPCBA->BrowseOPCItemIDs(OPC_LEAF, L"", VT_EMPTY, 0, &enumString);
  r = CoSetProxyBlanket(enumString, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, 2, &aid, 0);

  if (FAILED(r)) {
    ErrorMsg("BrowseOPCItemIDs", r);
  }
  while (S_OK == r) {
    unsigned long n;
    LPOLESTR str, wstr;
    r = enumString->Next(1, &str, &n);
    if (FAILED(r) || n != 1) break;

    UnicodeToAnsi(str, &s);
    h = tree->InsertItem(s, hParent);
    lst2.Add(s);
    map_hn[h] = lst.GetSize();

    CoTaskMemFree(s);

    r = gpOPCBA->GetItemID(str, &wstr);
    if (FAILED(r)) {
      ErrorMsg("GetItemID", r);
    }

    UnicodeToAnsi(wstr, &s);
    lst.Add(s);
    CoTaskMemFree(s);
    CoTaskMemFree(wstr);
  }
  if (enumString) enumString->Release();
}

bool GetServer(CLSID &clsid, CString &str)
{
  HRESULT r;
  MULTI_QI mqi;
  IEnumGUID *enumID = 0;
  CLSID catid = CATID_OPCDAServer20;

  mqi.pIID = &IID_IOPCServerList;
  mqi.hr = 0;
  mqi.pItf = 0;

  r = CoCreateInstanceEx(CLSID_OpcServerList, NULL, clsctx, sinptr, 1, &mqi);
  if (FAILED(r) || FAILED(mqi.hr))
  {
    ErrorMsg("CoCreateInstanceEx IOPCServerList", r);
    return false;
  }

  CMMenu menu(0, IDD_MENU6, _TR("Список OPC-серверов"));
  CArray<CLSID,CLSID> listCLSID;

  IOPCServerList *sl = 0;
  sl = (IOPCServerList *) mqi.pItf;

  r = CoSetProxyBlanket(sl, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, 2, &aid, 0);

  r = sl->EnumClassesOfCategories(1, &catid, 1, &catid, &enumID);

  if (FAILED(r))
  {
    ErrorMsg("EnumClassesOfCategories", r);
    return 0;
  }

  r = CoSetProxyBlanket(enumID, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, 2, &aid, 0);

  while (S_OK == r) {
    unsigned long n;
    GUID guid;

    r = enumID->Next(1, &guid, &n);

    if (FAILED(r) || n != 1) break;

    char *s;

    LPOLESTR pszProgID;
    LPOLESTR pszUserType;

    sl->GetClassDetails(guid, &pszProgID, &pszUserType);

//    UnicodeToAnsi(pszProgID, &s);
    UnicodeToAnsi(pszUserType, &s);
    menu.lst.Add(s);
    CoTaskMemFree(s);
    listCLSID.Add(guid);

    CoTaskMemFree(pszProgID);
    CoTaskMemFree(pszUserType);
  }

  enumID->Release();
  sl->Release();

  if (menu.DoModal() == IDOK) {
    str = menu.m_strText;
    clsid = listCLSID[menu.m_Index];
    return true;
  }

  return false;
}


void MachineIP(char* result)
{
  TCHAR  szTemp[1024] = {0};
  TCHAR  szIP[1024] = {0};
  char   szHostName[1024] = {0};

  CWaitCursor wait;

  CMMenu menu(0, IDD_MENU6, _TR("Список компьютеров"));

  DWORD dwScope = RESOURCE_CONTEXT;
  NETRESOURCE *NetResource = NULL;  
  HANDLE hEnum; 
  WNetOpenEnum( dwScope, NULL, NULL, NULL, &hEnum );  
  
  LPTSTR strFullName;
  LPTSTR szT;

  if ( hEnum )  
  {
    DWORD Count = 0xFFFFFFFF;
    DWORD BufferSize = 4096;
    LPVOID Buffer = new char[4096];
    WNetEnumResource( hEnum, &Count, Buffer, &BufferSize );

    NetResource = (NETRESOURCE*)Buffer;

    for ( unsigned int i = 0; i < BufferSize/sizeof(NETRESOURCE); i++, NetResource++ )
    { 
      if ( NetResource->dwUsage == RESOURCEUSAGE_CONTAINER && NetResource->dwType == RESOURCETYPE_ANY )
      {
        if ( NetResource->lpRemoteName )
        {
          szT = NetResource->lpRemoteName;
          strFullName = szT;
          if ( '\\' == szT[0] && '\\' == szT[1]) strFullName = szT + 2;
          menu.lst.Add(strFullName);
        }
      }
    }
    delete Buffer;
    WNetCloseEnum( hEnum );
  }

  if (menu.lst.GetSize() && menu.DoModal() == IDOK) {
    strcpy(result, menu.m_strText);
    return;
  }

  lstrcpy(result,"");
}
