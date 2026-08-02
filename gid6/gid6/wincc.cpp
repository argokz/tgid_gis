//Copyright : Nick Gashevsky, 2004
#include "stdafx.h"
#include "stdio.h"
//#include "iostream.h"
#include "time.h"
#include "win.h"

using namespace ADODB;

#include "wincc.h"
        
CString dump_error(_com_error &e)
{
  CString str;
  _bstr_t bstrSource(e.Source());
  _bstr_t bstrDescription(e.Description());
  
  // Print Com errors.  
  str.Format(
    "Error\n"
    "Code = %08lx\n"
    "Code meaning = %s\n"
    "Source = %s\n"
    "Description = %s\n"
    , e.Error(), e.ErrorMessage(), (LPCSTR) bstrSource, (LPCSTR) bstrDescription);

//  AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
  return str;
}; 


void set_time(char *s, time_t t) 
{
    struct tm *nt;
    nt = gmtime( &t );
    sprintf(s, "%d-%02d-%02d %d:%02d:%02d.000", nt->tm_year+1900, nt->tm_mon+1, nt->tm_mday, nt->tm_hour, nt->tm_min, nt->tm_sec);
//--    printf( "%s\n", s);
}

CString wincc(const char *source, const char *catalog, const char *val)
{

  CString str = "";
  HRESULT hr;
//  if (FAILED( CoInitialize(NULL))) return str;

  _RecordsetPtr pRs = NULL;
  _CommandPtr pCmd = NULL;
  _ConnectionPtr pConn = NULL;

  char conn[256], cmd[256];

  CString WinCCOLEDBProvider = AfxGetApp()->GetProfileString(szSection, "WinCCOLEDBProvider", "WinCCOLEDBProvider");

  sprintf(conn, "Provider=%s;Catalog=%s;Data Source=%s\\WinCC", WinCCOLEDBProvider, catalog, source);
  _bstr_t strConn(conn);

  hr = pConn.CreateInstance(__uuidof(Connection));
  hr = pRs.CreateInstance(__uuidof(Recordset)); 
  hr = pCmd.CreateInstance(__uuidof(Command));

  pConn->ConnectionString=strConn;
  pConn->CursorLocation=adUseClient;
  pConn->Errors->Clear();
  hr = pConn->Open(pConn->ConnectionString,"","",adConnectUnspecified);

  if (hr != S_OK) return str;


  sprintf(cmd, "TAG:R, '%s','0000-00-00 00:02:00.000','0000-00-00 00:00:00'", val);


  _bstr_t strSQL(cmd);

  pCmd->CommandType = adCmdText;
  pCmd->ActiveConnection = pConn;
  pCmd->CommandText = strSQL;
  pConn->Errors->Clear();

  try {
    pRs = pCmd->Execute(NULL,NULL,adCmdText);
    pRs->MoveLast();
    str = (char*) ((_bstr_t)pRs->GetFields()->Item["RealValue"]->Value);
  }
  catch(_com_error &e) {
    dump_error(e);
    return CString("");
  }

  pRs->Close();
  pConn->Close();
  pConn->Errors->Clear();

  return str;
}

CWinCC::CWinCC()
{
  isOpen = false;
  m_strError = "";
  m_error = false;
}

CWinCC::~CWinCC()
{
  close();
}


bool CWinCC::open(const char *source, const char *catalog)
{
  HRESULT hr;
//  if (FAILED( CoInitialize(NULL))) return false;

  m_pConn = NULL;

  char conn[256];

  CString WinCCOLEDBProvider = AfxGetApp()->GetProfileString(szSection, "WinCCOLEDBProvider", "WinCCOLEDBProvider");

  sprintf(conn, "Provider=%s;Catalog=%s;Data Source=%s\\WinCC", WinCCOLEDBProvider, catalog, source);
  _bstr_t strConn(conn);

  hr = m_pConn.CreateInstance(__uuidof(Connection));

  try {
  m_pConn->ConnectionString=strConn;
  m_pConn->CursorLocation=adUseClient;
  m_pConn->Errors->Clear();
  hr = m_pConn->Open(m_pConn->ConnectionString,"","",adConnectUnspecified);
  }
  catch(_com_error &e) {
    m_error = true;
    CString ss = dump_error(e);
    AfxMessageBox(ss, MB_OK|MB_ICONINFORMATION);
    return false;
  }

  if (hr != S_OK) return false;

  m_pConn->Errors->Clear();

  isOpen = true;

  return true;
}


bool CWinCC::close()
{
  if (isOpen) {
    m_pConn->Close();
    m_pConn->Errors->Clear();
    isOpen = false;
  }

  return true;
}

/*
CString CWinCC::get_value(const char *val)
{
  CString str = "";
  char cmd[256];
  HRESULT hr;

  if (!isOpen) return str;

  if (m_error) return str;


//  static int qq = 0;
//  str.Format("%d", qq++);
//  return str;

  _RecordsetPtr pRs = NULL;
  _CommandPtr pCmd = NULL;

  hr = pCmd.CreateInstance(__uuidof(Command));
  
//  sprintf(cmd, "TAG:R, '%s','0000-00-00 00:10:00.000','0000-00-00 00:00:00.000'", val);
  sprintf(cmd, "TAG:R, '%s','0000-00-00 00:10:00.000','0000-00-00 00:00:00.000', TIMESTEP=600,2", val);
  sprintf(cmd, "TAG:R, '2010-02-17 10:20:30.000','2010-02-17 10:25:30.000' 'TIMESTEP=60,2'", val);
  _bstr_t strSQL(cmd);

  pCmd->CommandType = adCmdText;
  pCmd->ActiveConnection = m_pConn;
  pCmd->CommandText = strSQL;
  m_pConn->Errors->Clear();

  try {
    pRs = pCmd->Execute(NULL,NULL,adCmdText);
    pRs->MoveLast();
    str = (char*) ((_bstr_t)pRs->GetFields()->Item["RealValue"]->Value);

    str.Replace(",",".");
    double v = atof(str);

    str.Format("%.2f", v);
  }
  catch(_com_error &e) {
    m_error = true;
    dump_error(e);
    m_strError = (LPCSTR) e.Description();
    return str;
  }

  pRs->Close();

  return str;
}
*/


CString CWinCC::get_value(time_t t, const char *val)
{
  CString str = "";
  char cmd[256], s1[256], s2[256], *sss;
  HRESULT hr;
  double v;

  if (!isOpen) return str;

  if (m_error) return str;

//  static int qq = 0;
//  str.Format("%d", qq++);
//  return str;

  _RecordsetPtr pRs = NULL;
  _CommandPtr pCmd = NULL;

  hr = pCmd.CreateInstance(__uuidof(Command));

//  set_time(s1, t-600);
  set_time(s1, t-120);
  set_time(s2, t);

  //    _bstr_t strSQL("TAG:R,1,'2009-04-23 8:39:57','2009-04-23 8:40:00'");

  
//  sprintf(cmd, "TAG:R, '%s','0000-00-00 00:02:00.000','0000-00-00 00:00:00'", val);
//  sprintf(cmd, "TAG:R, '%s','0000-00-00 00:00:00','%s'", val, s2);
//  sprintf(cmd, "TAG:R, '%s','0000-00-00 00:02:00.000','0000-00-00 00:00:00'", val);


//  sprintf(cmd, "TAG:R, '%s','%s','%s'", val, s1, s2);
//  sprintf(cmd, "TAG:R, '%s','%s','0000-00-00 00:01:00', TIMESTEP=60,2", val, s1);

  sprintf(cmd, "TAG:R,'%s','%s','%s','TIMESTEP=60,2'", val, s1,s2);


  sprintf(cmd, "TAG:R,'%s','%s','%s'", val, s1,s2);


  sss = cmd;

//  strcpy(cmd, "TAG:R,'SystemArchive\AI_CH/OUM60TOO1.VB_OUT','2010-02-17 10:20:30.000','0000-00-00 00:00:00.000', 'TIMESTEP=300,2'");

  _bstr_t strSQL(cmd);  

  pCmd->CommandType = adCmdText;
  pCmd->ActiveConnection = m_pConn;
  pCmd->CommandText = strSQL;
  m_pConn->Errors->Clear();

  try {
//    ClockMessage m(val);
    pRs = pCmd->Execute(NULL,NULL,adCmdText);
    pRs->MoveLast();

//    long int NumberRecords = pRs->RecordCount;
//    str = (char*) ((_bstr_t)pRs->GetFields()->Item["RealValue"]->Value);

    long typ = pRs->GetFields()->Item[2L]->GetType();

    switch (typ) {
      case 5:
        v = (double) pRs->GetFields()->Item[2L]->Value;
        str.Format("%.2f", v);
        break;
      default:
        str = (char*) ((_bstr_t)pRs->GetFields()->Item[2L]->Value);
        break;
    }
  }
  catch(_com_error &e) {
//    m_error = true;
    dump_error(e);
 //   AfxMessageBox(dump_error(e), MB_OK|MB_ICONINFORMATION);

    m_strError = (LPCSTR) e.Description();
    return str;
  }

  pRs->Close();

  return str;
}

/**/
bool CWinCC::get_value(time_t t, const list<CString> & listTag, list<CString> & listVal)
{
  CString str = "";
  char s1[256], s2[256];
  CString cmd, lt;
  HRESULT hr;

  listVal.clear();

  if (!isOpen) return false;
  if (m_error) return false;

  list<CString>::const_iterator it;

  lt = "";

#if 1
  int qq = 0;
//  for ( it = listTag.begin(); it != listTag.end() && qq1 < 4; ++it, qq1++ ) {
  for ( it = listTag.begin(); it != listTag.end() && qq < 1; ++it, qq++ ) {
    lt += "'";
    lt += *it;
    lt += "';";
  }
#else
  int qq = 9;

  for ( it = listTag.begin(); it != listTag.end() && qq < 15 ; ++it) {
    CString qqq;
    qqq.Format("%d", qq++);
    lt += qqq;
    lt += ";";
  }
#endif

  if (lt.IsEmpty()) return false;

  lt = lt.Left(lt.GetLength()-1);




  _RecordsetPtr pRs = NULL;
  _CommandPtr pCmd = NULL;

  hr = pCmd.CreateInstance(__uuidof(Command));

  set_time(s1, t);

  set_time(s1, t-120);
  set_time(s2, t);

  cmd.Format("TAG:R,(%s),'%s','%s','TIMESTEP=60,2'", lt, s1, s2);
  cmd.Format("TAG:R,(%s),'2010-02-22 6:00:00.000','2010-02-22 6:02:00.000','TIMESTEP=60,2'", lt, s1, s2);

//  cmd = "TAG:R,('parametri\\@_Rashod_v_C2';'parametri\\@_davlenie_v_C2';'parametri\\@_temperatura_v_C2';'parametri\\@_Rashod_v_C1';'parametri\\@_davlenie_v_C1';'parametri\\@_temperatura_v_C1'),'2010-02-22 6:00:00.000','2010-02-22 6:02:00.000','TIMESTEP=60,2'";

  AfxMessageBox(cmd, MB_OK|MB_ICONINFORMATION);

  _bstr_t strSQL(cmd);

  pCmd->CommandType = adCmdText;
  pCmd->ActiveConnection = m_pConn;
  pCmd->CommandText = strSQL;
  m_pConn->Errors->Clear();

  try {
    {
    ClockMessage m(_TR("Готово"));
    pRs = pCmd->Execute(NULL,NULL,adCmdText);
    }

    double v;

    long int NumberRecords = pRs->RecordCount;

    pRs->MoveFirst();

    while (VARIANT_FALSE == pRs->GetadoEOF()) {
      str = "";
      long num = pRs->GetFields()->Item[0L]->Value;
      long typ = pRs->GetFields()->Item[2L]->GetType();
      switch (typ) {
        case 5:
          v = (double) pRs->GetFields()->Item[2L]->Value;
          str.Format("%.2f", v);
          break;
        default:
          str = (char*) ((_bstr_t)pRs->GetFields()->Item[2L]->Value);
          break;
      }

      listVal.push_back(str);

      pRs->MoveNext();
    }
  }
  catch(_com_error &e) {
    m_error = true;
    CString str = dump_error(e);
    AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
    m_strError = (LPCSTR) e.Description();
    return false;
  }
  catch(...) {
    AfxMessageBox(_TR("Неизвестная ошибка"), MB_OK|MB_ICONINFORMATION);
    return false;
  }
  pRs->Close();

  return true;
}
/**/

/*

void main()
{
  time_t t;

  time( &t );

  wincc("133-3", "cc_pid_cc_09_01_12_10_57_58r", "Level\\AI_Level", t-10000, t+10000);

  time( &t );

  tm *newtime = gmtime( &t );
  printf( "Coordinated universal time is %s\n", asctime( newtime ) );
}
*/