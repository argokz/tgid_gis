// Prot.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "Prot.h"
#include "win.h"
#include "OutThrea.h"
//#include "CoolDial.h"
  
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

FILE *popen(const TCHAR * cmd, const TCHAR * mode);
int pclose(FILE * f);

extern bool isExit;
extern bool isExit2;


/////////////////////////////////////////////////////////////////////////////
// CProt dialog



CProt::CProt()
  : CPaneDialog()
{
  //{{AFX_DATA_INIT(CProt)
  m_strEdit = "";
  //}}AFX_DATA_INIT

  m_id = 0;
}


void CProt::DoDataExchange(CDataExchange* pDX)
{
  CPaneDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CProt)
  DDX_Control(pDX, IDC_EDIT, m_edit);
  DDX_Text(pDX, IDC_EDIT, m_strEdit);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProt, CPaneDialog)
  //{{AFX_MSG_MAP(CProt)
    ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
    ON_WM_SIZE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProt message handlers

void CProt::resize()
{
  CRect sz;
  GetClientRect(sz);
  if (::IsWindow(m_edit.m_hWnd) )
    m_edit.MoveWindow(sz); // do not repaint

}

void CProt::OnSize(UINT nType, int cx, int cy) 
{
  CPaneDialog::OnSize(nType, cx, cy);

  resize();
}

void CProt::setFile(const char *fn)
{
  FILE *f;

  char s[1024];
  m_strEdit = "";
  m_edit.SetLimitText(300000);

  f = fopen(fn, "r");

  if (f) {
    while (!feof(f)) {
      fgetstr(s, 255, f);
      trim(s);
      m_strEdit += s;
      m_strEdit += "\r\n";
    }
        
    fclose(f);
  }
  UpdateData(FALSE);
}



HANDLE CProt::start_proc(CWnd *wnd, const TCHAR *cmd, const TCHAR *prtn, int id)
{
    m_id = id;
  m_pThread = 0;
  isExit = false;
  isExit2 = false;

  m_strEdit = "";
  UpdateData(FALSE);


  m_wnd = wnd;

  FILE *f;

  m_prtn = prtn;

  if (prtn) {
    f = fopen(prtn, "w");
    fclose(f);
  }

  if( (outf = popen( cmd, _T("r") )) == NULL )
      return 0;

  m_pThread = new COutThread(this, outf, m_hProcess);
  m_pThread->CreateThread();

//  UpdateData(TRUE);

   Add(_TR("Идет расчет, ожидайте"), 0);

  return outf;
//  return m_pThread->m_hThread;
}

int m_exit_code = 0;

void CProt::end_proc()
{
  int exit_code = pclose(outf);

      if (!exit_code) {
    Add(_TR("Расчет закончен"), 0);
//    AfxMessageBox(_TR("Расчет закончен"), MB_OK|MB_ICONINFORMATION);
  }
  else {
    CString str;
    if (exit_code == 2) {
//      AfxMessageBox(_TR("Расчет теплового режима не производится.\nСмотрите файл протокола"), MB_OK|MB_ICONINFORMATION);
      Add(_TR("Расчет закончен.\nРасчет теплового режима не производится.\r\nСмотрите файл протокола"), 0);
    }
    else if (exit_code == 3) {
//      AfxMessageBox(_TR("Расчет теплового режима не производится.\nСмотрите файл протокола"), MB_OK|MB_ICONINFORMATION);
      Add(_TR("Расчет закончен.\nРасчет тепловых потерь не производится.\r\nСмотрите файл протокола"), 0);
    }
    else {
//      AfxMessageBox(_TR("Расчет режима прерван.\nОбнаружены ошибки.\nСмотрите файл протокола"), MB_OK|MB_ICONINFORMATION);
      Add(_TR("Расчет режима прерван.\r\nОбнаружены ошибки.\r\nСмотрите файл протокола"), 0);
    }
  }


  isExit = TRUE;
  m_exit_code = exit_code;

//  m_wnd->SendMessage(WM_COMMAND, ID_END_DOIT);
//  m_wnd->PostMessage(WM_COMMAND, ID_END_DOIT);
  m_wnd->PostMessage(WM_COMMAND, m_id);
}


void CProt::Add(const TCHAR *Str, int lim) 
{
  if (!isExit) {
    CString str = Str;
    str.TrimRight();

    FILE *f = fopen(m_prtn, "a");
    if (f) {
      fprintf(f, "%s\n", (LPCTSTR)str);
      fclose(f);
    }

    CEdit *edt = (CEdit *) GetDlgItem(IDC_EDIT);
    if (edt) {
      int nLength = edt->GetWindowTextLength();
      edt->SetSel(nLength, nLength);
      edt->ReplaceSel("\r\n"+str);
    }
  }
}

void CProt::OnCancel() 
{

//extern CCoolDialogBar *pm_wndProtBar;
//extern CProt     *pm_cProt;
extern bool isExit;
extern bool isExit2;

  Cgid6App* pApp = (Cgid6App*) AfxGetApp();
  CMDIFrameWnd* pMainFrame = (CMDIFrameWnd *)pApp->m_pMainWnd;

//  pMainFrame->ShowControlBar(pm_wndProtBar, FALSE, FALSE);

//  CPaneDialog::OnCancel();
}


LRESULT CProt::HandleInitDialog(WPARAM, LPARAM)
{
  UpdateData(FALSE);

  CEdit *edt = (CEdit *)GetDlgItem(IDC_EDIT);
  if (edt) {
      edt->SetLimitText(0);
  }

  resize();
  
  return TRUE;
}


CTabbedPane* CProt::CreateTabbedPane()
{
    CTabbedPane* pTabbedBar = CDockablePane::CreateTabbedPane();

    if (pTabbedBar) {
      pTabbedBar->SetMinSize(CSize(200, 200)); //Here is that
    }

    return pTabbedBar;
}