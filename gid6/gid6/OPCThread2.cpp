// OutThrea.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "OPCThread2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void log(const char *podp, const char *s);


/////////////////////////////////////////////////////////////////////////////
// COPCThread

void AddProt(HWND hWnd, const TCHAR *Str, int lim);


IMPLEMENT_DYNCREATE(COPCThread2, CWinThread)

COPCThread2 *pOPCThread = 0;


CSemaphore  c_s;
CSemaphore  c_s_opc;


COPCThread2::COPCThread2()
{
  m_isExit = 0;
  m_isOff = 0;

  opc_server = 0;
  opc_gr = 0;

  event = new CEvent(FALSE, FALSE, "COPCThread2");
}

/*
void COPCThread2::setLst(const CArray<CString,CString> & lst)
{
  CSingleLock lock(&c_s);

  lock.Lock(1000);

  if (lock.IsLocked()) {
    for (int i = 0; i < lst.GetSize(); i++) {
      m_lst.insert(stringlst[i]);
    }
    lock.Unlock();
  }

  Tic();
//  read_opc();
}
*/


COPCThread2::~COPCThread2()
{
//    pclose(m_pF);
    delete event;
}


extern CLSID clsid;


void COPCThread2::openOPC() 
{
  char computer[256] = "";
  CString str;

  try {
    opc_server = new OPCServer(clsid);
    opc_gr = new OPCGroup(opc_server);
  }
  catch(...) {
  }
}


CString COPCThread2::getOpcVar(const char *name, double a)
{
  if (opc_gr && name[0]) {
    try {
      char zn[256];
      OPCItem item(opc_gr, name, a);
      item.Read(zn);
      return CString(zn);
    }
    catch (OPCError *err) {
      delete err;
      return CString(name);
    }
    catch (...) {
//      AfxMessageBox(name, MB_OK|MB_ICONINFORMATION);
      return CString("?");
    }
  }
  return CString(name);
}
/*
void COPCThread2::read_opc()
{
  CSingleLock lock1(&c_s_opc);

  lock1.Lock();

  set<CString>::const_iterator it;

  for (it = m_lst.begin(); it != m_lst.end(); ++it) {
    CString str = *it;
    if (!str.IsEmpty()) {
      m_val1[str] = getOpcVar(str);
    }
  }
  lock1.Unlock();
}

*/

BOOL COPCThread2::InitInstance()
{
  LocalInit2();
  openOPC();

//  LocalCleanup();

//  CSingleLock lockOPC(&c_s_opc);
  CSingleLock lock(&c_s);

//  lockOPC.Lock();

  
//  while (TRUE) {
  while (TRUE) {
    if (m_isExit) break;

    ::WaitForSingleObject( event->m_hObject, INFINITE );

    event->ResetEvent();

    if (m_isExit) break;

//      read_opc();
//      lockOPC.Lock(1000);
//      if (lockOPC.IsLocked()) {
        set<string>::const_iterator it;
        for (it = m_lst.begin(); it != m_lst.end(); ++it) {
          string str = *it;
          if (!str.empty()) {
            m_val1[str] = getOpcVar(str.c_str(), 1.);
          }
        }
//        lockOPC.Unlock();
//      }

      lock.Lock(1000);
      if (lock.IsLocked()) {
        map <string, string>::const_iterator it;
        m_val.clear();
        for (it = m_val1.begin(); it != m_val1.end(); ++it) {
          m_val[it->first] = it->second;
        }
        lock.Unlock();
      }
  }

//  AfxMessageBox("!", MB_OK|MB_ICONINFORMATION);

  m_isOff = 1;

  return ExitInstance();
}

int COPCThread2::ExitInstance()
{
  // TODO:  perform any per-thread cleanup here

  LocalCleanup();

  ExitThread(0);

  return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(COPCThread2, CWinThread)
  //{{AFX_MSG_MAP(COPCThread2)
    // NOTE - the ClassWizard will add and remove mapping macros here.
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()



void COPCThread2::Tic()
{
  event->SetEvent();
}

void COPCThread2::Exit() 
{
  m_isExit = 1; 
  Tic();
};

BOOL COPCThread2::isOff() 
{ 
  return m_isOff; 
}


CString COPCThread2::get_value(const char *name)
{
  if (name[0] == 0) return CString("");

  CString out = "?";
  CSingleLock lock(&c_s);

  lock.Lock(1000);

  if (lock.IsLocked()) {
    map <string, string>::const_iterator it = m_val.find(string(name));

    if (it != m_val.end()) {
      out = it->second.c_str();
    }
    else {
      m_lst.insert(name);
//      Tic();
    }
    lock.Unlock();
  }

  return out;

}
