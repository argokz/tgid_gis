// WebServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "WebServerDlg2.h"

#include "HTTPThrea.h"

#include "json.h"
#include "ado.h"

CString argpath();
CString argpath_2();

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool getDir(CWnd *wnd, CString &dir);


/////////////////////////////////////////////////////////////////////////////
// CWebServerDlg2 dialog

CWebServerDlg2::CWebServerDlg2(CWnd* pParent /*=NULL*/)
  : CDialog(CWebServerDlg2::IDD, pParent)
{
  //{{AFX_DATA_INIT(CWebServerDlg2)
  m_szHomeDir = "";
  m_szTilesDir = "";
  m_szDefIndex = _T("map.html");
  m_szIP = _T("localhost");
  m_szWebDir = "";
  m_Port = 80;
//  m_PortHttps = 443;
  m_PTO = 10;
  m_szStatus = "";
  m_HTTPS = 0;
  m_szPrivate = "";
  m_szPublic = "";
  //}}AFX_DATA_INIT
  // Note that LoadIcon does not require a subsequent DestroyIcon in Win32

  m_pThread = NULL;
  m_pThread80 = NULL;
  
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWebServerDlg2::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CWebServerDlg2)
//  DDX_Control(pDX, IDC_VISITORS, m_nVisitors);
//  DDX_Control(pDX, IDC_BYTESRECV, m_nBytesRecv);
//  DDX_Control(pDX, IDC_BYTESENT, m_nBytesSent);
//  DDX_Control(pDX, IDC_REQUESTS, m_nRequests);
//  DDX_Control(pDX, IDC_ACTIVECONN, m_nActiveConn);

  DDX_Text(pDX, IDC_PUBLIC, m_szPublic);
  DDX_Text(pDX, IDC_PRIVATE, m_szPrivate);
  
  DDX_Text(pDX, IDC_HOMEDIR, m_szHomeDir);
  DDX_Text(pDX, IDC_TILES_DIR, m_szTilesDir);
  DDX_Text(pDX, IDC_WEB_DIR, m_szWebDir);
  
  DDX_Text(pDX, IDC_DEFINDEXFILE, m_szIP);
//  DDX_Text(pDX, IDC_PORT_HTTPS, m_PortHttps);
  DDX_Text(pDX, IDC_PORT, m_Port);
  DDV_MinMaxInt(pDX, m_Port, 1, 65535);
  DDX_Text(pDX, IDC_PTO, m_PTO);
  DDV_MinMaxInt(pDX, m_PTO, 0, 10000);
  DDX_Text(pDX, IDC_STATUS, m_szStatus);
  DDX_Check(pDX, IDC_HTTPS, m_HTTPS);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWebServerDlg2, CDialog)
  //{{AFX_MSG_MAP(CWebServerDlg2)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_BN_CLICKED(IDC_START, OnStart)
  ON_BN_CLICKED(IDC_STOP, OnStop)
  ON_WM_CLOSE()
  ON_WM_TIMER()
  ON_BN_CLICKED(IDC_RESET, OnReset)
  ON_BN_CLICKED(IDC_HOMEDIRBROWSE, OnHomedirbrowse)
  ON_BN_CLICKED(IDC_TILE_DIR_BROWSE, OnTiledirbrowse)
  ON_BN_CLICKED(IDC_WEB_DIR_BROWSE, OnWebdirbrowse)

  
  ON_BN_CLICKED(IDC_PUBLICBROWSE, OnPublicbrowse)
  ON_BN_CLICKED(IDC_PRIVATEBROWSE, OnPivatebrowse)
  ON_WM_DESTROY()
  
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWebServerDlg2 message handlers

CString argpath();
CString argpath_2();


bool CopyDirTo( CString source_folder, CString target_folder )
{
//    CString new_sf = source_folder + "\\*";
    CString new_sf = source_folder;
    char sf[MAX_PATH+1];
    char tf[MAX_PATH+1];

    strncpy(sf, new_sf, MAX_PATH);
    strncpy(tf, target_folder, MAX_PATH);

    sf[lstrlen(sf)+1] = 0;
    tf[lstrlen(tf)+1] = 0;

    SHFILEOPSTRUCTA s = { 0 };
    s.wFunc = FO_COPY;
    s.pTo = tf;
    s.pFrom = sf;
    s.fFlags = FOF_SILENT | FOF_NOCONFIRMMKDIR | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NO_UI;
    int res = SHFileOperationA( &s );

    return res == 0;
}

#include <filesystem>


// Recursively copies all files and folders from src to target and overwrites existing files in target.
void CopyRecursive(const filesystem::path& src, const filesystem::path& target) noexcept
{
    try
    {
        filesystem::copy(src, target, filesystem::copy_options::overwrite_existing | filesystem::copy_options::recursive);
    }
    catch (std::exception& e)
    {
//        std::cout << e.what();
    }
}


BOOL CWebServerDlg2::OnInitDialog()
{
  CDialog::OnInitDialog();
  
  SetIcon(m_hIcon, TRUE);     // Set big icon
  SetIcon(m_hIcon, FALSE);    // Set small icon

  RestoreSettings();

//  CopyDirTo( argpath()+"\\ServerRoot2",  argpath_2()+"\\ServerRoot2");

/**
  filesystem::path src(argpath() + "\\ServerRoot2");
  filesystem::path dst(argpath_2() + "\\ServerRoot2");

  CopyRecursive(src, dst);
*/
  if (0) {
      OnStart();
  }
  
  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWebServerDlg2::OnSysCommand(UINT nID, LPARAM lParam)
{
  
  CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWebServerDlg2::OnPaint() 
{
  if (IsIconic())
  {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  }
  else
  {
    CDialog::OnPaint();
  }
}

int DeleteDirectory(const std::string &refcstrRootDirectory, bool bDeleteSubdirectories = true);


// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWebServerDlg2::OnQueryDragIcon()
{
  return (HCURSOR) m_hIcon;
}

void closeWeb();


void CWebServerDlg2::OnStart()
{
  if (m_pThread) {
    AfxMessageBox("Сервер уже запущен");
    return;
  }

//  closeWeb();

  UpdateData();

  CString s;

  s.Format("%s\\static\\config.json", m_szWebDir);
  
  FILE *f = fopen(s, "w");
  if (f) {
    JSON json;

    json.add("{");
    json.add("db", getGidAdoName());
    json.add("ps", getPsAdoName());

    CString s;

    if (m_HTTPS == 0) {
        s.Format("http://%s:%d", m_szIP, m_Port);
    }
    else {
        s.Format("https://%s:%d", m_szIP, m_Port);
    }

    json.add("ip", s);
    json.add("}");

    fprintf(f, "%s", json.str());

    fclose(f);
  }




//  if (AfxMessageBox("Создать новую карту?", MB_YESNO) == IDYES) {
//    DeleteDirectory(filename);
//  }



  UpdateData();
//  m_bRun = WebServer.Start(LPCTSTR(m_szHomeDir), LPCTSTR(m_szDefIndex), m_Port, m_PTO * 1000);

//  int port = m_HTTPS ? m_PortHttps : m_Port;
  int port = m_Port;

  m_pThread = new CHttpThread(this, 0, port, m_PTO, m_HTTPS);
  m_pThread->CreateThread();
  m_pThread->m_bAutoDelete = FALSE;

  if (m_pThread) {
    nTimerID = SetTimer(1, 5*1000, NULL);
  }
/*

  if (m_HTTPS) {
    m_pThread80 = new CHttpThread(this, 1, m_Port, m_PTO, 0);
    m_pThread80->CreateThread();
    m_pThread80->m_bAutoDelete = FALSE;
  }
*/
  m_szStatus = "Работает...";
  UpdateData(FALSE);
}

void CWebServerDlg2::OnStop() 
{
  if (m_pThread80) {
    m_pThread80->stop();
  }
  if (m_pThread) {
    m_pThread->stop();
  }
    
  if (m_pThread80) {
    WaitForSingleObject(m_pThread80->m_hThread, INFINITE); 
    delete m_pThread80;
    m_pThread80 = NULL;
  }                        
  
  if (m_pThread) {
    KillTimer(nTimerID);

    m_szStatus = "Останавливается.";
    UpdateData(FALSE);

    WaitForSingleObject(m_pThread->m_hThread, INFINITE); 
    delete m_pThread;
    m_pThread = NULL;

    m_szStatus = "Остановлен.";
    UpdateData(FALSE);
    return;
  }                        

  AfxMessageBox("Сервер не запущен");
}

void CWebServerDlg2::OnClose() 
{
  SaveSettings();
  if (m_pThread) OnStop();
  CDialog::OnClose();
}

void CWebServerDlg2::OnCancel() 
{
  SaveSettings();
  if (m_pThread) OnStop();
  CDialog::OnCancel();
}


void CWebServerDlg2::OnOK() 
{
  SaveSettings();
  if (m_pThread) OnStop();
  CDialog::OnOK();
}

bool is_cxema_updated();


void CWebServerDlg2::OnTimer(UINT nIDEvent) 
{
    if (nIDEvent == nTimerID)
    {
        m_updated = is_cxema_updated();
    }

/*
  StatisticsTag st;
  CString szTemp;
  
  WebServer.GetStats(st);

  szTemp.Format("%d", st.nClientsConnected);
  m_nActiveConn.SetWindowText(szTemp);

  szTemp.Format("%.1f", st.nTotalRecv);
  m_nBytesRecv.SetWindowText(szTemp);
  
  szTemp.Format("%.1f", st.nTotalSent);
  m_nBytesSent.SetWindowText(szTemp);

  szTemp.Format("%d", st.nTotalHits);
  m_nRequests.SetWindowText(szTemp);

  szTemp.Format("%d", st.nVisitors);
  m_nVisitors.SetWindowText(szTemp);
*/
  CDialog::OnTimer(nIDEvent);
}

void CWebServerDlg2::OnReset() 
{
  OnStop();
//  WebServer.Reset();
  UpdateData();
  OnStart();
}

bool SelDirDial(CWnd* pParentWnd, CString& cs);


void CWebServerDlg2::OnHomedirbrowse() 
{
//    getDir(this, m_szHomeDir);
    UpdateData(TRUE);
    SelDirDial(this, m_szHomeDir);
    UpdateData(FALSE);
}

void CWebServerDlg2::OnTiledirbrowse() 
{
//    getDir(this, m_szTilesDir);
    UpdateData(TRUE);
    SelDirDial(this, m_szTilesDir);
    UpdateData(FALSE);
}


void CWebServerDlg2::OnWebdirbrowse() 
{
//    getDir(this, m_szWebDir);
    UpdateData(TRUE);
    SelDirDial(this, m_szWebDir);
    UpdateData(FALSE);
}


void CWebServerDlg2::SaveSettings()
{
  CWinApp* pApp = AfxGetApp();
  UpdateData();

  pApp->WriteProfileString("WebServer2", "SSL Public", m_szPublic);
  pApp->WriteProfileString("WebServer2", "SSL Private", m_szPrivate);
  
  pApp->WriteProfileString("WebServer2", "Server Root", m_szHomeDir);
  pApp->WriteProfileString("WebServer2", "Tiles Dir", m_szTilesDir);
  pApp->WriteProfileString("WebServer2", "Web Dir", m_szWebDir);
  pApp->WriteProfileString("WebServer2", "IP", m_szIP);
  pApp->WriteProfileInt("WebServer2", "Port", m_Port);
//  pApp->WriteProfileInt("WebServer2", "PortHttps", m_PortHttps);
  pApp->WriteProfileInt("WebServer2", "PTO", m_PTO);
  pApp->WriteProfileInt("WebServer2", "HTTPS", m_HTTPS);
}

CString argpath();
CString argpath_2();


void CWebServerDlg2::RestoreSettings()
{
  CWinApp* pApp = AfxGetApp();

  m_szPublic = pApp->GetProfileString("WebServer2", "SSL Public", "");
  m_szPrivate = pApp->GetProfileString("WebServer2", "SSL Private", "");

  char path1[512];
  HRESULT ret = SHGetSpecialFolderPath(NULL,path1,CSIDL_APPDATA, 1);
  
  CString tiles = CString(path1)+"\\Sirius\\tgid\\tiles\\";

  m_szHomeDir = pApp->GetProfileString("WebServer2", "Server Root", "C:\\ServerRoot");
//  m_szHomeDir = pApp->GetProfileString("WebServer2", "Server Root", argpath_2()+"ServerRoot2");

//  m_szDefIndex = pApp->GetProfileString("WebServer2", "Defindex", "map.html");
  m_szIP = pApp->GetProfileString("WebServer2", "IP", "localhost");

//  m_szHomeDir = argpath()+"ServerRoot2";
  m_szHomeDir = argpath_2()+"ServerRoot2";
  m_szWebDir = argpath_2()+"ServerRoot2";
  m_szDefIndex = "map.html";
  
  m_szTilesDir = pApp->GetProfileString("WebServer2", "Tiles Dir", tiles);
//  m_szWebDir = pApp->GetProfileString("WebServer2", "Web Dir", "");
//  m_szHomeDir = argpath_2()+"ServerRoot2";


  m_szHomeDir = pApp->GetProfileString("WebServer2", "Server Root", m_szHomeDir);
  m_szWebDir = pApp->GetProfileString("WebServer2", "Web Dir", m_szWebDir);


  m_Port = pApp->GetProfileInt("WebServer2", "Port", 80);
//  m_PortHttps = pApp->GetProfileInt("WebServer2", "PortHttps", 443);
  m_PTO = pApp->GetProfileInt("WebServer2", "PTO", 10);
  m_HTTPS = pApp->GetProfileInt("WebServer2", "HTTPS", FALSE);
  UpdateData(FALSE);
}


void CWebServerDlg2::OnPublicbrowse()
{
  CFileDialog fd(TRUE, "", NULL, OFN_HIDEREADONLY,
    _T("Файлы ключей (*.*)|*.*|"), this);

  if (fd.DoModal() == IDOK) {
    m_szPublic = fd.GetPathName();

    UpdateData(FALSE);
  }
}

void CWebServerDlg2::OnPivatebrowse()
{
  CFileDialog fd(TRUE, "", NULL, OFN_HIDEREADONLY,
    _T("Файлы ключей (*.*)|*.*|"), this);

  if (fd.DoModal() == IDOK) {
    m_szPrivate = fd.GetPathName();

    UpdateData(FALSE);
  }
}


void CWebServerDlg2::OnDestroy()
{
    CDialog::OnDestroy();
//    if (!m_modal)
//      delete this; // Shown as non-modal, we'll clean up ourselves
}
