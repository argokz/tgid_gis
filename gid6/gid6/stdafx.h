// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#include <boost/geometry.hpp>
#include <iostream>


#pragma once

#define _CRT_NON_CONFORMING_SWPRINTFS

#pragma warning(disable:4244) // conversion from 'double' to 'int', possible loss of data
#pragma warning(disable:4018) // '<': signed/unsigned mismatch

#pragma warning(disable:26495)  // Variable 'variable' is uninitialized. Always initialize a member variable (type.6).
//#pragma warning(disable:6031)  // Return value ignored: 'called-function' could return unexpected value

#pragma warning(disable:5033)  // 'register' is no longer a supported storage class

#pragma warning(disable:26812) // The enum type 'type-name' is unscoped. Prefer 'enum class' over 'enum' (Enum.3)
#pragma warning(disable:6284)  // Object passed as parameter when string is required in call to '*function*'

#pragma warning(disable:4477)  // 'function' : format string 'string' requires an argument of type 'type', but variadic argument number has type 'type'

#pragma warning(disable:33005) // VARIANT 'var' was provided as an _In_ or _InOut_ parameter but was not initialized (expression 'expr')




#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes


#define ZDANIYA_2 "zdaniya_2"


#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars


#include <vector>
#include <list>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include <string>
#include <valarray>
#include <afxwin.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxwin.h>
#include <afxwin.h>
#include <afxcontrolbars.h>
#include <afxdlgs.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
using namespace std;



typedef enum DataTypeEnum2 {
    dbBoolean = 1,
    dbByte = 2,
    dbInteger = 3,
    dbLong = 4,
    dbCurrency = 5,
    dbSingle = 6,
    dbDouble = 7,
    dbDate = 8,
    dbBinary = 9,
    dbText = 10,
    dbLongBinary = 11,
    dbMemo = 12,
    dbGUID = 15,
    dbBigInt = 16,
    dbVarBinary = 17,
    dbChar = 18,
    dbNumeric = 19,
    dbDecimal = 20,
    dbFloat = 21,
    dbTime = 22,
    dbTimeStamp = 23
} DataTypeEnum2;


enum REMONT_TYP {
  TIP_DEFECT2 = 1,
  TIP_SHURF2 = 2,
  TIP_KORROZIA = 3,
  TIP_OPRESSOVKA = 4,
  TIP_KAPITAL = 5
};

enum {
  RT_MS = 0,
  RT_RS,
  RT_KOTEL,
  RT_OPRESSOVKA
};

enum LAYERS {
    L_ALL = 0,
    L_GID = 1,
    L_GEO,
    L_PTS,
    L_ACT,
    L_IND,
    L_DEFECT,
    L_REMONT,
    L_SHURFY,
    L_DIAG,
    L_OPRESSOVKA_SEKTSIONIRUYUSCHAYA_ZADVIZHKA,
};

#import "C:\Program Files\Common Files\System\ado\msado15.dll" rename("EOF", "adoEOF") rename("BOF", "adoBOF") rename("LockTypeEnum", "adoLockTypeEnum")


using namespace ADODB;


static TCHAR BASED_CODE szSection[] = _T("Settings");

#define N3 5
#define N10 25

#define D5 8.
#define N8 8

#define NO_ALL_FIND

#define N65 20
#define LEN100 30


#define MIN_FONT 5.


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


CString transl(const char *txt);
CString transl_l(const char *txt);

extern int m_userRemoved;

const char *_TR(const char *key);

struct CIni
{
      CString m_IP;
      int m_port;
      CString m_user;
      CString m_password;
      CString m_bd_gid;
      CString m_bd_geo;
      CString m_update_server;
};


#pragma comment(lib, "OPENGL32.LIB")
#pragma comment(lib, "GLU32.LIB")
//#pragma comment(lib, "GLAUX.LIB")

#include <windows.h> 
 
//#include <gl\GLAUX.h>
#include <gl\GL.h>
#include <gl\GLU.h>

#define GOOGLE_ALWAYS FALSE
#define GOOGLE_LEN FALSE

//#define DEMO_KIEV_ARNIKA 1

//#define DEMO_KHARKIV 1

//#define DEMO_DNEPR 1

//#define DEMO_OMSK 1

//#define DEMO_TEPLOSERVIS_AKTOBE 1


#ifdef DEMO_KHARKIV
    #define DEMO_KAVERIN 1
    #define DemoGID "KharkivGID"
#endif

#ifdef DEMO_OMSK
    #define DEMO_KAVERIN 1
    #define DemoGID "OmskGID"
#endif



#ifdef DEMO_DNEPR
    #define DEMO_KAVERIN 1
    #define DemoGID "DneprGID"
#endif


#ifdef DEMO_KIEV_ARNIKA
    #define DEMO_KAVERIN 1
    #define DemoGID "KievGID"
#endif

#ifdef DEMO_TEPLOSERVIS_AKTOBE
    #define DEMO_KAVERIN 1
    #define DemoGID "Akotobe2024_10_15"
#endif




#ifdef DEMO_KAVERIN
//#define GOOGLE_ALWAYS TRUE
#define NO_UPDATE 1
#endif

