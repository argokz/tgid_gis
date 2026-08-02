#include "stdafx.h"
#include "gid6.h"
#include "scroll.h"

#include "gid6Doc.h"
#include "gid6View.h"
#include "gidrView.h"


#include "InputDia.h"

#include "win.h"
#include <math.h>
#include <Urlmon.h>
#include <Wininet.h>
#include <intshcut.h>
#include <process.h>    /* _beginthread, _endthread */
#include "MMenu.h"
#include "update.h"

//#include "hasp.h"

bool open_ini(CIni& ini);

#include <SYS\UTIME.H>

#include "TaskProgressDlg.h"


void KillExplorer(int id)
{
    HANDLE ps = OpenProcess(1, false, id);
    if (ps)
        TerminateProcess(ps, -9);
}

bool __stdcall EnumProc(HWND hWnd,/*LPARAM*/long/*lp*/)
{
    unsigned long* pPid;   //LPDWORD
    unsigned long result;      //DWORD
    void* hg;                  //HGLOBAL
    unsigned long id;
//    if (hWnd == NULL)
//        return false;
    hg = GlobalAlloc(GMEM_SHARE, sizeof(unsigned long));
    pPid = (unsigned long*)GlobalLock(hg);
    result = GetWindowThreadProcessId(hWnd, pPid);
    if (result) {
        char title[110];
        char className[95];
        GetClassName(hWnd, className, 95);
        GetWindowText(hWnd, title, 110);
        id = *pPid;
        CString str = "";
        str = title;


//        if (str.AnsiPos("Program Manager1") != 0)
//            KillExplorer(id);
//        Form1->ListBox1->Items->Add((AnsiString)totalStr);
    }
    else {
        GlobalUnlock(hg);
        GlobalFree(hg);
        return false;
    }
    GlobalUnlock(hg);
    GlobalFree(hg);
    return true;
}

#include <TlHelp32.h>


DWORD FindProcessId(const char* processname)
{
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    DWORD result = NULL;

    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hProcessSnap) return(FALSE);

    pe32.dwSize = sizeof(PROCESSENTRY32); // <----- IMPORTANT

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if (!Process32First(hProcessSnap, &pe32))
    {
        CloseHandle(hProcessSnap);          // clean the snapshot object
        printf("!!! Failed to gather information on system processes! \n");
        return(NULL);
    }

    do
    {
        printf("Checking process %ls\n", pe32.szExeFile);

        if (0 != strcmp("svchost.exe", pe32.szExeFile)) {
            if (0 == strcmp(processname, pe32.szExeFile))
            {
                result = pe32.th32ProcessID;
                break;
            }
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);

    return result;
}


void killProcessByName(const char* filename)
{
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes)
    {
        if (strcmp(pEntry.szExeFile, filename) == 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
                (DWORD)pEntry.th32ProcessID);
            if (hProcess != NULL)
            {
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
            }
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
}


bool isProcessByName(const char* filename)
{
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes)
    {
        if (strcmp(pEntry.szExeFile, filename) == 0)
        {
            return true;
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);

    return false;
}



Percent2* m_pc = NULL;



class CallbackHandler : public IBindStatusCallback
{
private:
    int m_percentLast;

public:
    CallbackHandler() : m_percentLast(0)
    {
    }

    // IUnknown

    HRESULT STDMETHODCALLTYPE
        QueryInterface(REFIID riid, void** ppvObject)
    {

        if (IsEqualIID(IID_IBindStatusCallback, riid)
            || IsEqualIID(IID_IUnknown, riid))
        {
            *ppvObject = reinterpret_cast<void*>(this);
            return S_OK;
        }

        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE
        AddRef()
    {
        return 2UL;
    }

    ULONG STDMETHODCALLTYPE
        Release()
    {
        return 1UL;
    }

    // IBindStatusCallback

    HRESULT STDMETHODCALLTYPE
        OnStartBinding(DWORD     /*dwReserved*/,
            IBinding* /*pib*/)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE
        GetPriority(LONG* /*pnPriority*/)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE
        OnLowResource(DWORD /*reserved*/)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE
        OnProgress(ULONG   ulProgress,
            ULONG   ulProgressMax,
            ULONG   ulStatusCode,
            LPCWSTR /*szStatusText*/)
    {
        switch (ulStatusCode)
        {
        case BINDSTATUS_FINDINGRESOURCE:
            //            tcout << _T("Finding resource...") << endl;
            break;
        case BINDSTATUS_CONNECTING:
            //            tcout << _T("Connecting...") << endl;
            break;
        case BINDSTATUS_SENDINGREQUEST:
            //            tcout << _T("Sending request...") << endl;
            break;
        case BINDSTATUS_MIMETYPEAVAILABLE:
            //            tcout << _T("Mime type available") << endl;
            break;
        case BINDSTATUS_CACHEFILENAMEAVAILABLE:
            //            tcout << _T("Cache filename available") << endl;
            break;
        case BINDSTATUS_BEGINDOWNLOADDATA:
            //            tcout << _T("Begin download") << endl;
            break;
        case BINDSTATUS_DOWNLOADINGDATA:
        case BINDSTATUS_ENDDOWNLOADDATA:
        {
            int percent = (int)(100.0 * static_cast<double>(ulProgress)
                / static_cast<double>(ulProgressMax));
            if (m_percentLast < percent)
            {
                //                LoadBar(percent, 100);
                m_pc->SetValue(percent);
                m_percentLast = percent;
            }
            if (ulStatusCode == BINDSTATUS_ENDDOWNLOADDATA)
            {
                //                tcout << endl << _T("End download") << endl;
            }
        }
        break;

        default:
        {
            //            tcout << _T("Status code : ") << ulStatusCode << endl;
        }
        }
        // The download can be cancelled by returning E_ABORT here
        // of from any other of the methods.
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE
        OnStopBinding(HRESULT /*hresult*/,
            LPCWSTR /*szError*/)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE
        GetBindInfo(DWORD*    /*grfBINDF*/,
            BINDINFO* /*pbindinfo*/)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE
        OnDataAvailable(DWORD      /*grfBSCF*/,
            DWORD      /*dwSize*/,
            FORMATETC* /*pformatetc*/,
            STGMEDIUM* /*pstgmed*/)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE
        OnObjectAvailable(REFIID    /*riid*/,
            IUnknown* /*punk*/)
    {
        return E_NOTIMPL;
    }
};

#include <iostream>

int readUrl(const char* url, int sz, char* buffer)
{
    //    ComInit init;

    // use CComPtr so you don't have to manually call Release()
    CComPtr<IStream> pStream;

    HRESULT hr = URLOpenBlockingStream(nullptr, url, &pStream, 0, nullptr);
    if (FAILED(hr))
    {
        return 1;
    }

    DWORD bytesRead = 0;
    hr = pStream->Read(buffer, sz, &bytesRead);

    if (bytesRead > 0) {
        buffer[bytesRead] = 0;
    }


    if (FAILED(hr))
    {
        //        std::cout << "ERROR: Download failed. HRESULT: 0x" << std::hex << hr << std::dec << "\n";
        return 2;
    }
    return 0;
}


bool unzip(const char* cInPath, const char* cOutPath)
{
    bool y = false;

    //    OleInitialize(NULL);

    //    char cInPath[] = "c:\\file.zip\\readme.txt";
    //    char cOutPath[] = "c:\\13";
    int nLen;
    BSTR bstrZip, bstrFol;
    HRESULT hres;
    long lCount;

    IShellDispatch* pSD = NULL;
    //Интерфейсы IID_Folder
    Folder* pZipFolder = NULL;
    Folder* pOutFolder = NULL;
    //Интерфейс IID_FolderItems
    FolderItems* pFolderItems = NULL;


    hres = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_SERVER, IID_IShellDispatch, (LPVOID*)&pSD);

    if (SUCCEEDED(hres))
    {
        VARIANT vZipDir, vOutDir, vOptions;

        VariantInit(&vZipDir);
        vZipDir.vt = VT_BSTR;
        nLen = (lstrlen(cInPath) + 1) * sizeof(WCHAR);
        bstrZip = SysAllocStringByteLen(NULL, nLen);

        MultiByteToWideChar(CP_ACP, 0, cInPath, -1, bstrZip, nLen);
        vZipDir.bstrVal = bstrZip;

        VariantInit(&vOutDir);
        vOutDir.vt = VT_BSTR;
        nLen = (lstrlen(cOutPath) + 1) * sizeof(WCHAR);
        bstrFol = SysAllocStringByteLen(NULL, nLen);

        MultiByteToWideChar(CP_ACP, 0, cOutPath, -1, bstrFol, nLen);
        vOutDir.bstrVal = bstrFol;

        if (SUCCEEDED(pSD->NameSpace(vOutDir, &pOutFolder)))
        {
            //Мы можем получить количество файлов в ZIP папке
            if (SUCCEEDED(pOutFolder->Items(&pFolderItems)))
            {
                pFolderItems->get_Count(&lCount);
            }

            VariantInit(&vOptions);
            vOptions.vt = VT_I4;
            vOptions.lVal = 0;

            hres = pOutFolder->CopyHere(vZipDir, vOptions);
            y = true;

            pOutFolder->Release();

            VariantClear(&vOptions);
        }

        pSD->Release();

        //SysFreeString(bstrZip);
        VariantClear(&vZipDir);

        //SysFreeString(bstrFol);
        VariantClear(&vOutDir);
    }
    return y;
}



#include "jsmn.h"

long getFileTime(const char* path)
{
    struct stat buf;
    int ret = ::stat(path, &buf);
    if (ret == -1) return -1;
    return buf.st_mtime;
}

long getTimeFromUrl(const char* url)
{
    char buffer[4000];

    int ret = readUrl(url, 4000, buffer);

    if (ret) {
//        AfxMessageBox("Нет доступа к серверу", MB_OK|MB_ICONINFORMATION);
        return -1;
    }

    long url_time = -1;

    int numtok = 20;
    jsmntok_t* t = (jsmntok_t*)malloc(numtok * sizeof(jsmntok_t));

    jsmn_parser p;
    jsmn_init(&p);
    int r = jsmn_parse(&p, buffer, strlen(buffer), t, numtok);

    for (int i = 0; i < r; i++) {
        if (t[i].type == JSMN_STRING) {
            CString s1(&buffer[t[i].start], t[i].end - t[i].start);
            if (s1 == "time") {
                CString ss = CString(&buffer[t[i + 1].start], t[i + 1].end - t[i + 1].start);
                url_time = atoi(ss);
            }
        }
    }

    free(t);

    return url_time;
}


extern int is_UPDATE_ONLY_SPECIAL;

CString getUpdateDir();


int UpdateProgr(bool force)
{

#ifdef NO_UPDATE
    return -1;
#endif 


//    AfxMessageBox("!", MB_OK|MB_ICONINFORMATION);

//    CString str, s1, p;
//    char path1[512];
//    HRESULT ret = SHGetSpecialFolderPath(NULL, path1, CSIDL_PROFILE, 1);
//   p.Format("%s\\AppData\\Local\\Programs\\update-manager\\update-manager.exe ", path1);
//    HINSTANCE hRun = ShellExecute (AfxGetMainWnd()->GetSafeHwnd (), "open", p, "", "", SW_SHOW);

    CString url, zipn, zipn2, exen, url_info;
//    CString AstanaGID = "SiriusGID";
    CString AstanaGID = "";

    CIni ini;

    if (open_ini(ini)) {
      AstanaGID = ini.m_update_server;
    }

    AstanaGID = AfxGetApp()->GetProfileString(szSection, "UpdateSetup", AstanaGID);
   
    if (is_UPDATE_ONLY_SPECIAL) {
        AstanaGID = getUpdateDir();
    }

    if (AstanaGID[0] == 0) return 0;

    WriteProfileString(szSection, "UpdateSetup", AstanaGID);


//    srand( (unsigned)time( NULL ) );
    int r = rand();

    url_info.Format("http://lan.avto-glass.kz/?action=updateInfo&db=%s&temp=%d", AstanaGID, r);
    url.Format("http://lan.avto-glass.kz/?action=tgid&db=%s&temp=%d", AstanaGID, r);
    zipn.Format("%s\\tgid.zip", getenv("TMP"));
    zipn2.Format("%s\\tgid.zip\\tgid-sql.exe", getenv("TMP"));
    exen.Format("%s\\tgid-sql.exe", getenv("TMP"));

      
    char path[_MAX_PATH + 1];
    ::GetModuleFileName(NULL, path, _MAX_PATH);

    long zip_time = getFileTime(zipn);
    long exe_time = getFileTime(path);
    long path_time = getFileTime(GetPath(path));
    long url_time = getTimeFromUrl(url_info);

    if (url_time == -1 || url_time == 0) return 0;

//    CString ss;
//    ss.Format("file = %d url = %d zip = %d zip-url = %d", exe_time, url_time, zip_time, zip_time - url_time);

    int ret = 0;
    
    bool ask = true;

    CString txt;
    
    txt.Format("Доступно обновление программы для %s, установить?", AstanaGID);

    if (force) {
        txt.Format("Повторить обновление программы для %s, установить?", AstanaGID);
    }

    if (zip_time == -1 || zip_time < url_time || force) {
//        ss.Format("zip_time-url_time = %d\n", zip_time - url_time);
        
//        if (MessageBox(AfxGetMainWnd()->GetSafeHwnd(), "Доступно обновление программы, установить?", "", MB_YESNO | MB_SYSTEMMODAL) == IDYES) {
        if (MessageBox(AfxGetMainWnd()->GetSafeHwnd(), txt, "", MB_YESNO | MB_SYSTEMMODAL) == IDYES) {
//            AfxGetMainWnd()->SendMessage(WM_CLOSE);
            Percent2 pc("Загрузка обновления выполняется...");

            m_pc = &pc;

            CallbackHandler callbackHandler;
            IBindStatusCallback* pBindStatusCallback = NULL;
            callbackHandler.QueryInterface(IID_IBindStatusCallback, reinterpret_cast<void**>(&pBindStatusCallback));
            ret = URLDownloadToFile(0, url, zipn, 0, pBindStatusCallback);
            ask = false;
        }
        else
            return 0;
    }

    if (ret == 0) {
        if (path_time < url_time || force) {
            if (!ask ||  MessageBox(AfxGetMainWnd()->GetSafeHwnd(), txt, "", MB_YESNO | MB_SYSTEMMODAL) == IDYES) {
                DeleteFile(exen);
                if (unzip(zipn2, getenv("TMP"))) {
                    HINSTANCE hRun = ShellExecute(AfxGetMainWnd()->GetSafeHwnd(), "open", exen, "", "", SW_SHOW);
                    CGidrView *pView = getView();

                    if (pView) {
                        Cgid6Doc *doc = (Cgid6Doc *)pView->GetDocument();
                        doc->m_ask_close = false;
                    }
                    AfxGetMainWnd()->SendMessage(WM_CLOSE);

//                    DWORD id = 0;
//                    do {
//                        id = FindProcessId("gid6.exe");
//                    } while (id);   


//                    EnumProc(0, 0L);


//                    AfxGetMainWnd()->SendMessage(WM_QUIT);

//                    AfxMessageBox(_TR("Закройте программу перед обновлением"), MB_OK|MB_ICONINFORMATION);
                }
            }
        }
        else {
            return 1;
//            AfxMessageBox("Обновление не требуется, установлена последняя версия", MB_OK|MB_ICONINFORMATION);
        }
    }
    else {
        return 2;
//        AfxMessageBox("Ошибка соединения с сервером", MB_OK|MB_ICONINFORMATION);
    }
    return 0;
}



IMPLEMENT_DYNCREATE(CUpdateThread, CWinThread)


CUpdateThread::CUpdateThread(void *t)
{
}


CUpdateThread::CUpdateThread()
{
}


CUpdateThread::~CUpdateThread()
{
}

bool check_protection();

BOOL CUpdateThread::InitInstance()
{
  srand( (unsigned)time( NULL ) );
  
  CoInitialize(NULL);

  #if 0
  int ret = UpdateProgr(false);
  #else
//  int ret2 = check_protection();
  #endif

  return ExitInstance();
}

int CUpdateThread::ExitInstance()
{

  ExitThread(0);

  CoUninitialize();

  return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CUpdateThread, CWinThread)
  //{{AFX_MSG_MAP(CUpdateThread)
    // NOTE - the ClassWizard will add and remove mapping macros here.
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()




