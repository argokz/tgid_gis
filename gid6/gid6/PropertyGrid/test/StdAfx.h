// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A69CC70E_E4DE_4CB4_8240_753E64794368__INCLUDED_)
#define AFX_STDAFX_H__A69CC70E_E4DE_4CB4_8240_753E64794368__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define dbSeeChanges0 0
#define dbSeeChanges0 dbSeeChanges


#define _CRT_NON_CONFORMING_SWPRINTFS
//#define _CRT_SECURE_NO_WARNINGS

//#pragma warning(disable:4786)
//#pragma warning(disable:4503)
#pragma warning(disable:4244) // conversion from 'double' to 'int', possible loss of data
#pragma warning(disable:4018) // '<': signed/unsigned mismatch

#include <vector>
#include <list>
#include <map>
#include <set>
#include <stack>
#include <queue>
//#include <string>
#include <valarray>
using namespace std;

#define VC_EXTRALEAN    // Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxtempl.h>
#include <afxmt.h>   // MFC multithreading

typedef enum DataTypeEnum {
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
} DataTypeEnum;


//#include <atlstr.h>

//#ifndef _AFX_NO_DB_SUPPORT
//#include <afxdb.h>      // MFC ODBC database classes
//#endif // _AFX_NO_DB_SUPPORT

//#ifndef _AFX_NO_DAO_SUPPORT
//#include <afxdao.h>     // MFC DAO database classes
//#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>   // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#import "C:\Program Files\Common Files\System\ado\msado15.dll" rename("EOF", "adoEOF") rename("BOF", "adoBOF") rename("LockTypeEnum", "adoLockTypeEnum")
using namespace ADODB;

#if 0

#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>

#endif

static TCHAR BASED_CODE szSection[] = _T("Settings");

#define N3 5
#define N10 25

#define D5 8.
#define N8 8

#define NO_ALL_FIND

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A69CC70E_E4DE_4CB4_8240_753E64794368__INCLUDED_)
