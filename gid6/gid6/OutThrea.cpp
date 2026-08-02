// OutThrea.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "OutThrea.h"
#include "Prot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool isExit;
bool isExit2;

/////////////////////////////////////////////////////////////////////////////
// COutThread

void AddProt(HWND hWnd, const TCHAR *Str, int lim);


IMPLEMENT_DYNCREATE(COutThread, CWinThread)

COutThread::COutThread()
{
}

COutThread::~COutThread()
{
//    pclose(m_pF);
}

BOOL COutThread::InitInstance()
{
  char   psBuffer[1280];
  TCHAR psBuffer2[1280];
  int i, n, c;

  i = 0; n = 0;

//  CSingleLock sLock(&(m_Output->m_mutex));

  while (TRUE) {
    if (isExit) break;
    
//    sLock.Lock();

//    if (WaitForInputIdle(m_hProcess, INFINITE) == WAIT_OBJECT_0) {
//    }

    c = fgetc (m_pF);

    if (c == '\n' || c == EOF || i == 256) {
      psBuffer[i] = 0;
      i = 0;
      n ++;
      OemToChar(psBuffer, psBuffer2);
//      if (m_Output) m_Output->Add(psBuffer2, 20);
      if (m_Prot) m_Prot->Add(psBuffer2, 20);
//    if (m_Prot) AddProt(m_Prot->m_hWnd, psBuffer2, 20);
    }
    else {
      psBuffer[i++] = c;
      psBuffer[i] = 0;

//      CString s;
//      s.Format("%c %d\n%s", c, c, psBuffer);
//      s.OemToCharA();
//      AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);
    }

    if (c == EOF) break;

//    sLock.Unlock();
  }
//  sLock.Unlock();

//  if (m_Output) m_Output->End();
  if (m_Prot) m_Prot->end_proc();

  isExit = true;

  return ExitInstance();
}

int COutThread::ExitInstance()
{
  // TODO:  perform any per-thread cleanup here

//  if (WaitForSingleObject(m_hProcess, INFINITE) == WAIT_OBJECT_0) {
//  }

  ExitThread(0);

  return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(COutThread, CWinThread)
  //{{AFX_MSG_MAP(COutThread)
    // NOTE - the ClassWizard will add and remove mapping macros here.
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutThread message handlers

/*
COutThread::COutThread(COutput *output, FILE *f, HANDLE hProcess)
{
  m_pF = f;
  m_Prot = NULL;
  m_Output = output;
  m_hProcess = hProcess;
}
*/

COutThread::COutThread(CProt *prot, FILE *f, HANDLE hProcess)
{
  m_pF = f;
//  m_Output = NULL;
  m_Prot = prot;
  m_hProcess = hProcess;
}
