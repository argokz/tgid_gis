// OutThrea.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "../win.h"
#include "HTTPThrea.h"
#include "WebServerDlg2.h"
#include "mongoose.h"
#include "httpparser.h"
#include "../zlib/zlib.h"
#include <sstream>
#include "json.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static string m_tiles = "";
const char *title = "map";

static struct mg_serve_http_opts s_http_server_opts;

static std::map<CString, CString> map_guid_file;
static std::map<CString, DWORD> map_guid_dwProcessId;


void ispr(string &s);


std::vector<int> ParseStringToNumbers(const CString& str)
{
    std::vector<int> numbers;
    CString delimiter = _T(",");
    int start = 0;

    CString token = str.Tokenize(delimiter, start);
    while (token != _T(""))
    {
        int number = _ttoi(token);  // Преобразуем строку в целое число
        numbers.push_back(number);  // Добавляем число в вектор
        token = str.Tokenize(delimiter, start);  // Получаем следующий токен
    }

    return numbers;
}


#include <windows.h>
#include <iostream>
#include <string>

#include <cstdlib>  // Для функции system()
#include <string>

#if 1

#include <windows.h>
#include <iostream>

bool IsWordInstalled()
{
    HKEY hKey;
    LONG result = RegOpenKeyEx(HKEY_CLASSES_ROOT, "Word.Application", 0, KEY_READ, &hKey);
    if (result == ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return true;
    }
    return false;
}

CString getGuid()
{
    CString ss;
    GUID guid;
    OLECHAR* guidString;
    HRESULT hr = CoCreateGuid(&guid);
    StringFromCLSID(guid, &guidString);
    ss = guidString;
    ::CoTaskMemFree(guidString);

    return ss;
}


bool IsPowerShellInstalled()
{
    HKEY hKey;
    // Попытка открыть ключ, где хранится путь к PowerShell
    LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\PowerShell\\3\\PowerShellEngine", 0, KEY_READ, &hKey);
    if (result == ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return true;
    }
    return false;
}


bool IsVBAEnabled()
{
    HKEY hKey;
    DWORD value;
    DWORD valueSize = sizeof(DWORD);

    // Открываем ключ реестра для Excel
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Office\\16.0\\Excel\\Security", 0, KEY_READ, &hKey);
    if (result == ERROR_SUCCESS) {
        result = RegQueryValueEx(hKey, "VBAWarnings", NULL, NULL, (LPBYTE)&value, &valueSize);
        RegCloseKey(hKey);

        if (result == ERROR_SUCCESS) {
            if (value == 2) {
                return true;  // VBA включен
            }
        }
    }

    return false;  // VBA отключен или не найден
}



#endif

#if 0

int MyExec(const CString & commandLine) 
{
    // Запускаем консольную программу

    DWORD exitCode;
    
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    if (CreateProcess(nullptr, (LPSTR)(const char *)commandLine, nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
        // Ждем завершения процесса
        WaitForSingleObject(pi.hProcess, INFINITE);

        // Получаем код завершения
        if (GetExitCodeProcess(pi.hProcess, &exitCode)) {
//            std::wcout << L"Код завершения: " << exitCode << std::endl;
        } 
        else {
//            std::cerr << "Не удалось получить код завершения." << std::endl;
        }

        // Закрываем дескрипторы
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
//        std::cerr << "Не удалось запустить процесс." << std::endl;
    }

    return exitCode;
}

#endif


int MyExec(const CString& commandLine, CString &error);
bool RunProcessWithoutWait(const std::string& programPath);


bool convert_doc_pdf(const CString& input1, const CString& output, CString &error)
{
    CString input = input1;

    input.Replace("/", "\\");

    std::string inputFile = input;
    std::string outputFile = output;
    std::string argp = argpath();

    // Формируем команду для запуска PowerShell скрипта
    std::string command = "powershell -ExecutionPolicy Bypass -File \"" 
        +  argp
        + "kls\\convert-to-pdf.ps1\" -InputFile \""
        + inputFile + "\" -OutputFile \"" + outputFile + "\"";

//    std::string command = L"cscript //nologo " + inputFile+ " \"%s\" \"  " + %s + \"", scriptPath, inputFile, outputFile
//    swprintf_s(commandLine, 512, L"cscript //nologo %s \"%s\" \"%s\"", scriptPath, inputFile, outputFile);
    
//    std::string command = "cscript //nologo \"" + argp + "kls\\convert-to-pdf.vba\" \"" + inputFile + "\" \"" + outputFile + "\"";

    log1(command.c_str());

    // Выполняем команду
//    int returnCode = system(command.c_str());
//    int returnCode = WinExec(command.c_str(), SW_HIDE);

    error = "";
    int returnCode = MyExec(command.c_str(), error);

//    CString ss;
//    ss.Format("returnCode = %d error=%s", returnCode, error);
//    log1(ss);

//  swprintf_s(commandLine, 512, L"cscript //nologo %s \"%s\" \"%s\"", scriptPath, inputFile, outputFile);
    
    return returnCode == 0;


//    return returnCode > 31;
}



COleDateTime GetFileCreationTime(const CString& filePath)
{
    struct _stat fileInfo;

    // Получаем информацию о файле
    if (_tstat(filePath, &fileInfo) == 0)
    {
        // Время последней модификации файла
        time_t modTime = fileInfo.st_mtime;

        // Преобразуем время в объект COleDateTime
        COleDateTime oleModTime(modTime);
        return oleModTime;
    }

    COleDateTime dt;

    return dt;
}


string getTiles()
{
    return m_tiles;
}

void setTiles(const char *t)
{
    m_tiles = t;
}



fpos_t readFile(const char *fn, char **pBuf, char *szDT)
{
  fpos_t lengthActual = 0, length = 0;

  FILE *f = fopen(fn, "rb");

  if (!f) return 0;

  // Retrive file size
  fseek(f, 0, SEEK_END);
  fgetpos(f, &lengthActual);
  fseek(f, 0, SEEK_SET);
  *pBuf = new char[lengthActual + 1];
  length = fread(*pBuf, 1, lengthActual, f);
  fclose(f);

  struct stat buf;
  int ret = ::stat(fn, &buf);
  if (!ret) {
    struct tm *newtime = gmtime(&buf.st_mtime);
//    struct tm *newtime = localtime(&buf.st_mtime);
    if (newtime) {
      strftime(szDT, 128, "%a, %d %b %Y %H:%M:%S GMT", newtime);
    }
  }
  return length;
}


string dateFile(const char *fn)
{
  char szDT[200];
  struct stat buf;
  int ret = ::stat(fn, &buf);
  if (!ret) {
    struct tm *newtime = gmtime(&buf.st_mtime);
    if (newtime) {
      strftime(szDT, 128, "%a, %d %b %Y %H:%M:%S GMT", newtime);
      return string(szDT);
    }
  }
  return string("");
}

//const char _frm[] = "%d%b%Y%H%M%S";
const char _frm[] = "%Y%m%d%H%M%S";

string dateFile2(const char *fn)
{
  char szDT[200];
  struct stat buf;
  int ret = ::stat(fn, &buf);
  if (!ret) {
    struct tm *newtime = gmtime(&buf.st_mtime);
    if (newtime) {
      strftime(szDT, 128, _frm, newtime);
      return string(szDT);
    }
  }
  return string("");
}

string curDate2()
{
  char szDT[200];
  time_t ltime;
  time(&ltime);
  struct tm *newtime = gmtime(&ltime);
  int n = strftime(szDT, 128, _frm, newtime);
  return string(szDT);
}

void parseGET1(map<string, string> &map_v, int len, const char *buf)
{
  string key;
  string val;

  for (int i = 0; i < len; i++) {
    if (buf[i] == '=') {
      key = string(buf, i);
      map_v[key] = string(&buf[i+1], len-i);
      break;
    }
  }
}

string parseGET(map<string, string> &map_v, const char *buf)
{
  int b = 0;
  string fileName = "";

  int i;

  for (i = 0; buf[i] && buf[i] != '?' && buf[i] != '&'; i++) {
  }
  fileName = string(buf, i);
  
  if (buf[i]) {
    i++;
    b = i;

    for (; buf[i]; i++) {
      if (buf[i] == '&' || buf[i] == '?') {
        parseGET1(map_v, i-b-1, &buf[b]);
        b = i+1;
      }
    }
    parseGET1(map_v, i-b-1, &buf[b]);
  }

  ispr(fileName);

  return fileName;
}

int regex_match(const char *p, const char *buf, vector<CString> & match);
int utf8_to_a(const char *buf1, char *a);

void parsePOST3(map<string, string> &map_v, const char *buf)
{
    vector<CString> match;
    int l = regex_match("\\s*(.+?)\\s*=\\s*\\\"(.+?)\\\"\\s*$", buf, match);

    if (l >= 2) {
        CString a1 = match[0];
        CString a2 = match[1];
        CString a3 = match[2];

        char a[1024];
        utf8_to_a(a3, a);
//        map_v[string(a2)] = string(a);
        map_v[string(a2)] = a3;
    }
}

void parsePOST2(map<string, string> &map_v, const char *buf)
{
    /**
        vector<CString> match;
        int l = regex_match("\\s*(.+?)\\s*=\\s*\\\"(.+)\\\"\\s*$", buf, match);

        if (l >= 2) {
            CString a1 = match[0];
            CString a2 = match[1];
            CString a3 = match[2];
        }
    */

    int b1 = 0, b2 = 0;

    for (int i = 0; buf[i]; i++) {
        if (buf[i] == ';') {
            CString ss = &buf[b1];
            ss = ss.Left(i - b1);
            parsePOST3(map_v, ss);
            b1 = i + 1;
        }
    }
    parsePOST3(map_v, &buf[b1]);
}

void parsePOST1(map<string, string> &map_v, const char *buf)
{
    vector<CString> match;
    int l = regex_match("(.+?)\\s*:\\s*(.+)$", buf, match);

    if (l >= 2) {
        CString a1 = match[0];
        CString a2 = match[1];
        CString a3 = match[2];
        parsePOST2(map_v, a3+";");
    }
}

int parsePOST(map<string, string> &map_v, int len, const char *buf, string &buf2)
{
    int b = 0, i = 0;

    if (len <= 0) return -1;
    CString s = "";

    string name = "name";
    string head = "";
    bool is_begin = true;
    int n0 = 0;
    int v0 = 0;

    for (i = 0; i < len; i++) {
        if (buf[i] == '\r' && buf[i + 1] == '\n') {
            string str = string(&buf[n0], min(i - n0 - 1, 100));
            n0 = i + 2;

            if (is_begin) {
                head = str;
                is_begin = false;
            }
            else {
                if (str == head) {
                    string val = string(&buf[v0], i - v0 - str.length()-3);
                    map_v[name] = val;
                }
                else {
                    parsePOST1(map_v, s);

                    map<string, string>::const_iterator it = map_v.find("name");
                    if (it != map_v.end()) {
                        name = it->second;
                    }

                    if (buf[i + 2] == '\r' && buf[i + 3] == '\n') {
                        i += 4;
                        v0 = i;
                    }
                }
            }
            i++;
            s = "";
        }
        else {
            s += buf[i];
        }
    }

    buf2 = string(&buf[v0], len - v0 - head.length() - 8+1);

    return v0;
    return i;
}


string getV(map<string, string> &map_v, const char *key)
{
  map<string, string>::const_iterator it = map_v.find(string(key));
  if (it != map_v.end()) {
    string s = it->second;
    ispr(s);
    return s;
  }
  return string("");
}

int  codX(char c) {
  if ('0' <= c && c <= '9') return c-'0';
  else if ('A' <= c && c <= 'F') return c-'A'+10;
  else if ('a' <= c && c <= 'f') return c-'A'+10;
  return 0;
}

char *utf8_to_a2(const char *buf1, bool kaz);

void ispr(string &s)
{
  const char *buf = s.c_str();

  string s2 = "";

  for (int i = 0; buf[i]; i++) {
    if (buf[i] == '%') {
      i++;
      int c1 = codX(buf[i]);
      i++;
      int c2 = codX(buf[i]);
      s2 += ((char) (c1*16+c2));

    }
    else {
      s2 += buf[i];
    }
  }
  char *a = utf8_to_a2(s2.c_str(), false);
  s = a;
  CoTaskMemFree(a);
}



int log_printf( const char *szFolder, const char *fmt, ... ) 
{
//  return FALSE;

//  EnterCriticalSection(&cs);

  va_list argptr;

  if (!strlen(szFolder)) return FALSE;

  char szLogFilePath[512];

  strcpy(szLogFilePath, m_tiles.c_str());

  if (szLogFilePath[0] != '\\') strcat(szLogFilePath, "\\");

  strcat(szLogFilePath, szFolder);

  FILE *f = fopen(szLogFilePath, "a");
  if (f) {
    va_start( argptr, fmt );
    vfprintf( f, fmt, argptr );
    va_end( argptr );

    fclose(f);
  }
//  LeaveCriticalSection(&cs);
  return FALSE;
}



/////////////////////////////////////////////////////////////////////////////
// CHttpThread

string getV(map<string, string> &map_v, const char *key);
string parseGET(map<string, string> &map_v, const char *buf);
void ispr(string &s);


CString html_max_changed_object();
CString html_cxema_updated();

CString http_search_node(const CString &text);

CString http_get_layers(int dpi);

CString http_get_q(const char *file, const char *q, int  id1, int id2, bool rus_name);
CString http_get_file_q(const char *db, const char *file, int  id1, int id2);


CString http_get_table(const char *file, const char *table, int ms, int rs, int  id1, int id2);
CString http_get_table_gid(const char *file, const char *table, const char *table2, int  id1, int id2);
CString http_get_list_analyse(const char *file, const char *table, const char *table2);


CString http_get_table_position(const char *file, const char *table, const char *type, int id);
CString http_get_table_gid_position(const char *file, const char *table, int  id);
CString http_get_table_list();
CString http_get_table_out_list();
CString http_get_table_analiz_list();


CString http_validate_user(const char *login, const char *user);
CString http_tree_gid();
CString http_tree_geo();

CString http_tree_ms();
CString http_tree_rs();


CString http_update(const char *q);
CString http_set_field(const char *q);


CString http_get_tiles_names(int dpi, int m, int x1, int y1, int x2, int y2, int internalNodeID, int layer, int gid, int geo);
CString http_delete_object(const char *tiles, const char *file, const char *table, int id);
CString http_add_geo_point(const char *tiles, const char *tn, double mas0, double x, double y);
CString http_move_object(const char *tiles, const char *tn, int id, double mas0, double x, double y);


CString http_get_dropdown(const char *file, const char *q);
CString http_get_map_info(int z, double x, double y, int internalNodeID, int geo);
CString http_get_pro_find(const char *tn, int loc, double x, double y);
CString http_get_geo(const char *tn, int id);
CString http_get_TU(const char *tn, int id);
CString http_get_node(const char *mdb, const char *tn, int id);
CString http_get_line(const char *file, const char *tn, int id);

CString http_get_street(const char *pat);
CString http_get_dom(const char *pat);
CString http_get_addr2(const char *pat);
bool http_make_png(const char *fn, int dpi, int m, int r, int c, int internalNodeID, int layer, int n_layers, int *layers, int gid, int geo, int ms, int rs);
bool http_make_zero_png(const char* fn);
bool http_get_border(int m, int &x1, int &y1, int &x2, int &y2, int internalNodeID);
CString http_get_gps(double x, double y);
CString http_get_pro(const char *file, const char *table, int ID);
CString http_get_pro_file();
CString http_get_pro_table(const char *mdb, const char *table);
CString http_get_pro_table_ID(const char *mdb, const char *table, int ID);
CString http_get_pro_update_ID(const char *mdb, const char *table, const char *field_list, int ID);
CString http_get_pro_table_objects_ID(const char *mdb, const char *table, int ID);
CString http_get_pro_query(const char *mdb, const char *query);
CString http_get_pro_query1(const char *mdb, const char *query);
CString http_get_pro_query2(const char *mdb, const char *query);
CString http_get_file(const char *mdb, const char *table);
CString http_get_remonts(const char *table);
CString http_get_map_rect(int internalNodeID, int width, int height);

CString http_get_screen();

CString http_get_node_id(int id);
CString http_get_node_xy(int mas, double x, double y, int internalNodeID);

CString http_get_ms(int id);
CString http_get_rs(int id);
CString http_get_fragments();
CString http_set_fragment(int id);

CString http_get_address_alma();

CString http_get_config();


CString http_get_geo_list();

CString get_tg_graph(int id);
CString get_tg_table(int id);

CString http_get_form(const char *table, int id);
CString http_set_form(const char *table, int id, const char *data);
CString http_delete_form(const char *table, int id, const char *data);

CString http_edit_field(const char *table, int id);

CString http_uploadFileForm(const char *path, const char *tmpn, const char *filename, const char *key, const char *tableName, int id);


IMPLEMENT_DYNCREATE(CHttpThread, CWinThread)

CHttpThread::CHttpThread()
{
}

static COleDateTime html_start;

void html_reset()
{
    html_start = COleDateTime::GetCurrentTime();
}

CString html_reset_cxema();

CHttpThread::CHttpThread(CWebServerDlg2 *dlg, int progr, int port, int pto, BOOL is_ssl)
{
  isExit = false;
  m_dlg = dlg;
  m_port = port;
  m_pto = pto;
  m_is_ssl = is_ssl;
  m_progr = progr;
  session = NULL;
  html_start = COleDateTime::GetCurrentTime();
}


CHttpThread::~CHttpThread()
{
//    pclose(m_pF);
}

string dateFile(const char *fn);
fpos_t readFile(const char *fn, char **pBuf, char *szDT);


class CRequest
{
public:

  CRequest() {
  };
  ~CRequest() {
    if (buf) delete [] buf;
  }

  string file_name = "";
  string status = "200 OK";
  string mime = "text/html";
  string date = "";
  string message = "";
  BOOL bKeepAlive = TRUE;
  BOOL gzip = FALSE;

  char *buf = NULL;
  int length = 0;
};


string getMime(string szFileName);


void readJson(CRequest *r, const string &message)
{
  r->status = "200 OK";
  r->mime = "application/json";

  r->length = message.length();
  r->buf = new char[r->length+1];
  memmove(r->buf, message.c_str(), r->length);
  r->buf[r->length] = 0;
}


void http_404(CRequest *req, const CString &message)
{
    JSON json;

    json.add("{");
    json.add("error", message);
    json.add("}");
    readJson(req, (const char *)json.str());

    req->status = "404 Resource not found";

    req->message = message;

}


bool readFile(CRequest *r, const char *filename, const char *last, bool is304)
{
    bool ret = false;
    string new_mod = "";
      
    if (last[0]) {
        new_mod = dateFile(filename);
    }

    if (is304 && last[0] && new_mod == string(last)) {
        r->status = "304 Not Modified";
        r->file_name = GetName(filename);
        r->date = last;
        r->mime = "";
    }
    else {
        char szDT2[200];
        r->length = readFile(filename, &r->buf, szDT2);

        if (r->length) {
            r->status = "200 OK";
            r->mime = getMime(filename);
            r->file_name = GetName(filename);
            r->date = szDT2;
            ret = true;
        }
        else
        {
            delete r->buf;
            r->buf = NULL;
            r->status = "404 Resource not found";
            r->mime = getMime("qq.html");
        }
    }
    return ret;
}

#define CHUNK 0x4000
#define windowBits 15
#define GZIP_ENCODING 16

bool gzip(int length, const char *buf, string &message)
{
  string m = "";
  int err = 0;

  unsigned char out[CHUNK];
  z_stream strm;
  strm.zalloc = Z_NULL;
  strm.zfree  = Z_NULL;
  strm.opaque = Z_NULL;
  err = deflateInit2 (&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, windowBits | GZIP_ENCODING, 8, Z_DEFAULT_STRATEGY);

  if (err < 0) return false;

  strm.next_in = (unsigned char *) buf;
  strm.avail_in = length;
  do {
      int have;
      strm.avail_out = CHUNK;
      strm.next_out = out;
      err = deflate (& strm, Z_FINISH);
      if (err < 0) return false;
      
      have = CHUNK - strm.avail_out;
      m += string((char*)out, have);
  } while (strm.avail_out == 0);
  deflateEnd (& strm);

  message = m;
  return true;
}

bool isGzip(const char *mime)
{
  if (!strncmp(mime, "text/", strlen("text/"))) return true;
  if (!strcmp(mime, "application/json")) return true;
  if (!strcmp(mime, "image/x-icon")) return true;
  if (!strcmp(mime, "application/javascript")) return true;

  return false;
}


void curDate(char *szDT)
{
  time_t ltime;
  time(&ltime);
  struct tm *newtime = gmtime(&ltime);
  int n = strftime(szDT, 128, "%a, %d %b %Y %H:%M:%S GMT", newtime);
}


void gzip(CRequest *r)
{
  string message;

  if (r->length <= 0) return;
  if (!isGzip(r->mime.c_str())) return;

  if (gzip(r->length, r->buf, message)) {
    delete [] r->buf;
    r->gzip = TRUE;
    r->buf = new char[message.length()+1];
    r->length = message.length();
    memmove(r->buf, message.c_str(), message.length());
  }
}


/* This is the name of the cookie carrying the session ID. */
#define SESSION_COOKIE_NAME "auth"
/* In our example sessions are destroyed after 30 seconds of inactivity. */
#define SESSION_TTL 300.0
#define SESSION_CHECK_INTERVAL 5.0

/* Session information structure. */
struct session {
  /* Session ID. Must be unique and hard to guess. */
  uint64_t id;
  /*
   * Time when the session was created and time of last activity.
   * Used to clean up stale sessions.
   */
  double created;
  double last_used; /* Time when the session was last active. */

  /* User name this session is associated with. */
  char *user;
  /* Some state associated with user's session. */
  int lucky_number;
};

/*
 * This example uses a simple in-memory storage for just 10 sessions.
 * A real-world implementation would use persistent storage of some sort.
 */
#define NUM_SESSIONS 100
struct session s_sessions[NUM_SESSIONS];

/*
 * Password check function.
 * In our example all users have password "password".
 */
static int check_pass(const char *user, const char *pass) {
  (void) user;
  return (strcmp(pass, "password") == 0);
}

bool check_login(const char *user, const char *password, int &right);


/*
 * Parses the session cookie and returns a pointer to the session struct
 * or NULL if not found.
 */
static struct session *get_session(struct http_message *hm) 
{
  struct mg_str *cookie_header = mg_get_http_header(hm, "cookie");
  if (cookie_header == NULL) {
    return NULL;
  }
  char ssid[21];
  if (!mg_http_parse_header(cookie_header, SESSION_COOKIE_NAME, ssid, sizeof(ssid))) {
    return NULL;
  }
//  uint64_t sid = strtoull(ssid, NULL, 16);
  uint64_t sid = 0;

  if (sscanf(ssid, "%I64x", &sid) == 1) {
    for (int i = 0; i < NUM_SESSIONS; i++) {
      if (s_sessions[i].id == sid) {
        s_sessions[i].last_used = mg_time();
        return &s_sessions[i];
      }
    }
  }
  return NULL;
}

/*
 * Destroys the session state.
 */
static void destroy_session(struct session *s) {
  free(s->user);
  memset(s, 0, sizeof(*s));
}

/*
 * Creates a new session for the user.
 */
static struct session *create_session(const char *user, const struct http_message *hm) 
{
  /* Find first available slot or use the oldest one. */
  struct session *s = NULL;
  struct session *oldest_s = s_sessions;
  for (int i = 0; i < NUM_SESSIONS; i++) {
    if (s_sessions[i].id == 0) {
      s = &s_sessions[i];
      break;
    }
    if (s_sessions[i].last_used < oldest_s->last_used) {
      oldest_s = &s_sessions[i];
    }
  }
  if (s == NULL) {
    destroy_session(oldest_s);
    log_printf("access.log", "Evicted %I64x/%s\n", oldest_s->id, oldest_s->user);
    s = oldest_s;
  }

  /* Initialize new session. */
  s->created = s->last_used = mg_time();
  s->user = strdup(user);
  s->lucky_number = rand();
  
  /* Create an ID by putting various volatiles into a pot and stirring. */
  cs_sha1_ctx ctx;
  cs_sha1_init(&ctx);
  cs_sha1_update(&ctx, (const unsigned char *) hm->message.p, hm->message.len);
  cs_sha1_update(&ctx, (const unsigned char *) s, sizeof(*s));
  unsigned char digest[20];
  cs_sha1_final(digest, &ctx);
  s->id = *((uint64_t *) digest);
  return s;
}



/*
 * If requested via GET, serves the login page.
 * If requested via POST (form submission), checks password and logs user in.
 */
static void login_handler(struct mg_connection *nc, int ev, void *p) 
{
  struct http_message *hm = (struct http_message *) p;
  if (mg_vcmp(&hm->method, "POST") != 0) {
    /* Serve login.html */
    mg_serve_http(nc, (struct http_message *) p, s_http_server_opts);
  } 
  else {
    /* Perform password check. */
    char user[50], pass[50];
    int ul = mg_get_http_var(&hm->body, "user", user, sizeof(user));
    int pl = mg_get_http_var(&hm->body, "pass", pass, sizeof(pass));
    if (ul > 0 && pl > 0) {

      int right;
      if (check_login(user, pass, right)) {

//      if (check_pass(user, pass)) {
        struct session *s = create_session(user, hm);
        char shead[100];
        snprintf(shead, sizeof(shead), "Set-Cookie: %s=%I64x; path=/", SESSION_COOKIE_NAME, s->id);
        mg_http_send_redirect(nc, 302, mg_mk_str("/"), mg_mk_str(shead));

        char szDT[200];
        curDate(szDT);

        log_printf("access.log", "[%s] %s logged in, sid %I64x\n", szDT, s->user, s->id);
      } 
      else {
//        mg_http_send_redirect(nc, 403, mg_mk_str("/bad_password.html"), mg_mk_str(NULL));
        mg_printf(nc, "HTTP/1.0 403 Unauthorized\r\n\r\n<p>Неправильный пароль.</p>\r\n");
      }
    } 
    else {
      mg_printf(nc, "HTTP/1.0 400 Bad Request\r\n\r\n<p>Требуется имя пользователя и пароль.</p>\r\n");
    }
    nc->flags |= MG_F_SEND_AND_CLOSE;
  }
  (void) ev;
}

/*
 * Logs the user out.
 * Removes cookie and any associated session state.
 */
static void logout_handler(struct mg_connection *nc, int ev, void *p) {
  struct http_message *hm = (struct http_message *) p;
  char shead[100];
  snprintf(shead, sizeof(shead), "Set-Cookie: %s=", SESSION_COOKIE_NAME);
//  mg_http_send_redirect(nc, 302, mg_mk_str("/"), mg_mk_str(shead));
//  mg_http_send_redirect(nc, 302, mg_mk_str("/login.html"), mg_mk_str(shead));
  mg_http_send_redirect(nc, 302, mg_mk_str("/index.html"), mg_mk_str(shead));
  struct session *s = get_session(hm);
  if (s != NULL) {
    char szDT[200];
    curDate(szDT);
    
    log_printf("access.log", "[%s] %s logged out, session %I64x destroyed\n", szDT, s->user, s->id);
    destroy_session(s);
  }
  nc->flags |= MG_F_SEND_AND_CLOSE;
  (void) ev;
}


int DeleteDirectory(const std::string &refcstrRootDirectory,
                    bool              bDeleteSubdirectories = true);


static void f5_handler(struct mg_connection *nc, int ev, void *p) 
{
  struct http_message *hm = (struct http_message *) p;

//  string filename = string(s_http_server_opts.document_root) + string("\\tiles");

  string filename =  m_tiles;

//  DirectoryRecursive(filename.c_str());
  DeleteDirectory(filename);

  mg_http_send_redirect(nc, 302, mg_mk_str("/"), mg_mk_str(NULL));
  nc->flags |= MG_F_SEND_AND_CLOSE;
  (void) ev;
}



/* Cleans up sessions that have been idle for too long. */
int check_sessions(void) {
  double threshold = mg_time() - SESSION_TTL;
  for (int i = 0; i < NUM_SESSIONS; i++) {
    struct session *s = &s_sessions[i];
    if (s->id != 0 && s->last_used < threshold) {
      log_printf("access.log", "Session %I64x (%s) closed due to idleness.\n",
              s->id, s->user);
      destroy_session(s);
      return 1;
    }
  }
  return 0;
}


#include "coordlis.h"


//CFRect getRsRect(int id);
//CFRect getMsRect(int id);

CString encode_rfc2231(CString txt)
{
    CString out = "", s;

    for (int i = 0; i < txt.GetLength(); i++) {
        char c = txt[i];
        s.Format("%c", c);

        if (c == ' ') s.Format("%%%02X", c);

        out += s;
    }
    return out;
}


CString tileName(int dpi, int m, int r, int c, int internalNodeID, int layer, int gid, int geo, int ms, int rs);
CString tileName(const CString &f);

/*
CString tileName(int m, int r, int c, int internalNodeID, int geo, int ms, int rs)
{
    CString filename;

    filename.Format("%s%d/%d/%d/%d/%d.png", m_tiles.c_str(), geo, internalNodeID, m, r, c);

    if (ms > 0) {
        CFRect r = getMsRect(ms);


        filename.Format("%sms/%d/%d/%d/%d/%d/%d.png", m_tiles.c_str(), ms, geo, internalNodeID, m, r, c);
    }
    else if (rs > 0) {
        filename.Format("%srs/%d/%d/%d/%d/%d/%d.png", m_tiles.c_str(), rs, geo, internalNodeID, m, r, c);
    }
    else {
        filename.Format("%s%d/%d/%d/%d/%d.png", m_tiles.c_str(), geo, internalNodeID, m, r, c);
    }

    return filename;
}

*/


CString getRealPath(CString code);


bool need_new_file(const CString &filename, int layer, bool gid)
{
    COleDateTime dt = GetFileCreationTime(filename);

    if (!IsFile(filename) || (dt < html_start && layer != L_GEO)) {
        return true;
    }
    return false;
}


static void ev_handler(struct mg_connection *c, int ev, void *p) 
{
  switch (ev) {
  case MG_EV_SSI_CALL: {
    /* Expand variables in a page by using session data. */
    const char *var = (const char *) p;
    CHttpThread *thread = (CHttpThread *) c->listener->mgr->user_data;
    
    const struct session *s = (const struct session *) thread->session;
    if (strcmp(var, "user") == 0) {
      mg_printf_html_escape(c, "%s", s->user);
    } 
    else if (strcmp(var, "lucky_number") == 0) {
      mg_printf_html_escape(c, "%d", s->lucky_number);
    }
    break;
  }
  case MG_EV_TIMER: {
    /* Perform session maintenance. */
    if (check_sessions()) {
//      logout_handler(c, ev, p);
    }
    mg_set_timer(c, mg_time() + SESSION_CHECK_INTERVAL);
    break;
  }
  case MG_EV_ACCEPT: {
    char addr[32];
    mg_sock_addr_to_str(&c->sa, addr, sizeof(addr), MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
//    log_printf("access.log", "%p: Connection from %s\r\n", c, addr);
    break;
  }
  case MG_EV_CLOSE: {
//    log_printf("access.log", "Connection %p closed\n", c);
    break;
  }
  
  case MG_EV_HTTP_REQUEST : {
    CHttpThread *thread = (CHttpThread *) c->listener->mgr->user_data;
    CWebServerDlg2 *dlg = thread->m_dlg;
    struct http_message *hm = (struct http_message *) p;

    CRequest req;
    CHttpParser parser(hm->message.p);
    string img = "/tiles/";

    string szFileName = parser.url;

#if 1

    struct session *s = get_session(hm);
    if (s) {
      s->last_used = mg_time();
    }

    if (!strcmp(parser.url.c_str(), "/") || szFileName.find(img) == 0) {
      if (s == NULL) {
        log_printf("access.log", "Redirect %s\n", szFileName.c_str());
//        mg_http_send_redirect(c, 302, mg_mk_str("/login.html"), mg_mk_str(NULL));
        mg_http_send_redirect(c, 302, mg_mk_str("/index.html"), mg_mk_str(NULL));
        c->flags |= MG_F_SEND_AND_CLOSE;
        break;
      }
      thread->session = s;
    }

#endif


    bool is_getFile = false;
    bool is_putFile = false;

    
    if(szFileName == "/") {
      szFileName = dlg->m_szDefIndex;
    }

    const char *ss = szFileName.c_str();

    map<string, string> mapGET;
    szFileName = parseGET(mapGET, ss);

    map<string, string> mapCookies;
    string cookies = parser.getV("Cookie");
    parseGET(mapCookies, cookies.c_str());

    string connection = parser.getV("Connection");
    _strlwr((char*)connection.c_str());

    req.bKeepAlive = (connection == string("keep-alive"));

    map<string, string> mapBody;
    string body = string(hm->body.p, hm->body.len);

    string body2 = "";

    int file_off = parsePOST(mapBody, body.length(), body.c_str(), body2);


    char szDT[200];
    curDate(szDT);

    string url = parser.url;
    ispr(url);

    string last1 = parser.getV("If-Modified-Since");
  
    string szResponse;
  
  //  log_printf(LOGFILENAME, "%s [%s] - [%s] GET %s ", address.c_str(), szDT,  last1.c_str(), url.c_str());


    if (szFileName == "/get_q") {
      string db = getV(mapGET, "db");
      string q = getV(mapGET,"q");
      int id1 = atoi(getV(mapGET, "id1").c_str());
      int id2 = atoi(getV(mapGET, "id2").c_str());

      string message = http_get_q(db.c_str(), q.c_str(), id1, id2, false);
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/get_config") {
      string message = http_get_config();
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/get_form") {
      string table = getV(mapGET,"table");
      int id = atoi(getV(mapGET, "id").c_str());
      string message = http_get_form(table.c_str(), id);
      readJson(&req, message.c_str());
    }

    else if (szFileName == "/edit_field") {
      string table = getV(mapGET,"table");
      int id = atoi(getV(mapGET, "id").c_str());
      string message = http_edit_field(table.c_str(), id);
      readJson(&req, message.c_str());
    }
    
    
    else if (szFileName == "/set_form") {
      string table = getV(mapGET,"table");
      int id = atoi(getV(mapGET, "id").c_str());
      string data = body;
      string message = http_set_form(table.c_str(), id, data.c_str());
      readJson(&req, message.c_str());
    }

    else if (szFileName == "/delete_form") {
      string table = getV(mapGET,"table");
      int id = atoi(getV(mapGET, "id").c_str());
      string data = body;
      string message = http_delete_form(table.c_str(), id, data.c_str());
      readJson(&req, message.c_str());
    }

    

    
    else if (szFileName == "/get_file_q") {
      string db = getV(mapGET, "db");
      string file = getV(mapGET,"file");
      int id1 = atoi(getV(mapGET, "id1").c_str());
      int id2 = atoi(getV(mapGET, "id2").c_str());

      string message = http_get_file_q(db.c_str(), file.c_str(), id1, id2);
      readJson(&req, message.c_str());
    }

    else if (szFileName == "/get_config") {
      string message = http_get_config();
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/get_table_position") {
      string db = getV(mapGET, "db");
      string q = getV(mapGET,"table");
      string type = getV(mapGET,"type");
      int id = atoi(getV(mapGET, "id").c_str());

      string message = http_get_table_position(db.c_str(), q.c_str(), type.c_str(), id);
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/get_table_gid_position") {
      string db = getV(mapGET, "db");
      string q = getV(mapGET,"table");
      int id = atoi(getV(mapGET, "id").c_str());

      string message = http_get_table_gid_position(db.c_str(), q.c_str(), id);
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/get_table_list") {
      string message = http_get_table_list();
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/get_table_out_list") {
      string message = http_get_table_out_list();
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/get_table_analiz_list") {
      string message = http_get_table_analiz_list();
      readJson(&req, message.c_str());
    }

    else if (szFileName == "/get_table_out_list") {
      string message = http_get_table_out_list();
      readJson(&req, message.c_str());
    }

    else if (szFileName == "/get_node_id") {
      int id = atoi(getV(mapGET, "id").c_str());
      string message = http_get_node_id(id);
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/get_node_xy") {
      double x = atof(getV(mapGET, "x").c_str());
      double y = atof(getV(mapGET, "y").c_str());
      double mas = atof(getV(mapGET, "mas").c_str());
      int internalNodeID = atoi(getV(mapGET,"internalNodeID").c_str());

      string message = http_get_node_xy(mas, x, y, internalNodeID);

      readJson(&req, message.c_str());
   }
    
    else if (szFileName == "/get_table") {
      string db = getV(mapGET, "db");
      string q = getV(mapGET,"table");
      int ms = atoi(getV(mapGET,"ms").c_str());
      int rs = atoi(getV(mapGET,"rs").c_str());

      int id1 = atoi(getV(mapGET, "id1").c_str());
      int id2 = atoi(getV(mapGET, "id2").c_str());

      string message = http_get_table(db.c_str(), q.c_str(), ms, rs, id1, id2);
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/get_table_gid") {
      string db = getV(mapGET, "db");
      string code = getV(mapGET,"code");
      string table = getV(mapGET,"table");
      int ms = atoi(getV(mapGET,"ms").c_str());
      int rs = atoi(getV(mapGET,"rs").c_str());

      int id1 = atoi(getV(mapGET, "id1").c_str());
      int id2 = atoi(getV(mapGET, "id2").c_str());

      string message = http_get_table_gid(db.c_str(), code.c_str(), table.c_str(), id1, id2);
      readJson(&req, message.c_str());
    }

    else if (szFileName == "/get_table_out") {
      string db = getV(mapGET, "db");
      string code = getV(mapGET,"code");
      string table = getV(mapGET,"table");
      int ms = atoi(getV(mapGET,"ms").c_str());
      int rs = atoi(getV(mapGET,"rs").c_str());

      int id1 = atoi(getV(mapGET, "id1").c_str());
      int id2 = atoi(getV(mapGET, "id2").c_str());

      string message = http_get_table_gid(db.c_str(), code.c_str(), table.c_str(), id1, id2);
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/get_list_analyse") {
      string db = getV(mapGET, "db");
      string code = getV(mapGET,"code");
      string table = getV(mapGET,"table");
      string message = http_get_list_analyse(db.c_str(), code.c_str(), table.c_str());
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/validate_user") {
      string login = getV(mapGET,"login");
      string password = getV(mapGET,"password");
      string message = http_validate_user(login.c_str(), password.c_str());
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/tree_gid") {
      string message = http_tree_gid();
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/tree_geo") {
//      string message = http_tree_geo();
//      readJson(&req, message.c_str());

        string message;
        CString filename = tileName("tree_geo.json");
        if (need_new_file(filename, L_GID, true)) {
            message = http_tree_geo();
            saveToFile(filename, message.c_str());
        }
        else {
            message = readFile(filename);
        }
        readJson(&req, message.c_str());
    }
    else if (szFileName == "/tree_ms") {
//      string message = http_tree_ms();
//      readJson(&req, message.c_str());
        string message;
        CString filename = tileName("tree_ms.json");
        if (need_new_file(filename, L_GID, true)) {
            message = http_tree_ms();
            saveToFile(filename, message.c_str());
        }
        else {
            message = readFile(filename);
        }
        readJson(&req, message.c_str());
    }
    else if (szFileName == "/tree_rs") {
//      string message = http_tree_rs();
        string message;
        CString filename = tileName("tree_rs.json");
        if (need_new_file(filename, L_GID, true)) {
            message = http_tree_rs();
            saveToFile(filename, message.c_str());
        }
        else {
            message = readFile(filename);
        }
        readJson(&req, message.c_str());
    }
    else if (szFileName == "/get_fragments") {
      string message = http_get_fragments();
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/set_fragment") {
      int id = atoi(getV(mapGET, "id").c_str());
      string message = http_set_fragment(id);
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/get_address") {
        string message;
        CString filename = tileName("address.json");
        if (need_new_file(filename, L_GID, true)) {
            message = http_get_address_alma();
            saveToFile(filename, message.c_str());
        }
        else {
            message = readFile(filename);
        }
//        string message = http_get_address_alma();
        readJson(&req, message.c_str());
    }
    else if (szFileName == "/get_ms_rs") {
      string type = getV(mapGET,"type");
      int id = atoi(getV(mapGET,"id").c_str());

      string message = "";

      if (type == "ms") {
        message = http_get_ms(id);
      }
      else if (type == "rs") {
        message = http_get_rs(id);
      }
      
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/reset") {
        html_reset();
    }
    else if (szFileName == "/reset_cxema") {
        html_reset();
        string message = html_reset_cxema();
        readJson(&req, message.c_str());
    }
    else if (szFileName == "/cxema_updated") {
        string message = html_cxema_updated();
        readJson(&req, message.c_str());
    }
    else if (szFileName == "/max_changed_object") {
        string message = html_max_changed_object();
        readJson(&req, message.c_str());
    }

    else if (szFileName == "/find") {
      double x = atof(getV(mapGET, "lng").c_str());
      double y = atof(getV(mapGET, "lat").c_str());

      int z = atoi(getV(mapGET, "mas").c_str());
      int internalNodeID = atoi(getV(mapGET,"internalNodeID").c_str());
      int geo = atoi(getV(mapGET,"geo").c_str());

      string message = http_get_map_info(z, x, y, internalNodeID, geo);

      readJson(&req, message.c_str());
    }
    else if (szFileName == "/get_tiles_names") {
      int m = atoi(getV(mapGET, "m").c_str());
      int x1 = atoi(getV(mapGET, "x1").c_str());
      int x2 = atoi(getV(mapGET, "x2").c_str());
      int y1 = atoi(getV(mapGET, "y1").c_str());
      int y2 = atoi(getV(mapGET, "y2").c_str());

      int internalNodeID = atoi(getV(mapGET,"internalNodeID").c_str());
      int geo = atoi(getV(mapGET,"geo").c_str());
      int gid = atoi(getV(mapGET,"gid").c_str());
      int layer = atoi(getV(mapGET,"layer").c_str());
      int dpi = atoi(getV(mapGET, "dpi").c_str());
      if (dpi == 0) dpi = 96;
      dpi = 96;

      string message = http_get_tiles_names(dpi, m, x1, y1, x2, y2, internalNodeID, layer, gid, geo);

      readJson(&req, message.c_str());
    }
    else if (szFileName == "/get_screen") {
      string message = http_get_screen();
      readJson(&req, message.c_str());
    }
    
    else if (szFileName == "/add_object") {
      double x = atof(getV(mapGET, "x").c_str());
      double y = atof(getV(mapGET, "y").c_str());
      
      double mas = atof(getV(mapGET, "mas").c_str());
      string table = getV(mapGET,"table");
      
      string message = http_add_geo_point(m_tiles.c_str(), table.c_str(), mas, x, y);
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/move_object") {
      double x = atof(getV(mapGET, "x").c_str());
      double y = atof(getV(mapGET, "y").c_str());
      double mas = atof(getV(mapGET, "mas").c_str());
      string table = getV(mapGET,"table");
      int id = atoi(getV(mapGET,"id").c_str());
      
      string message = http_move_object(m_tiles.c_str(), table.c_str(), id, mas, x, y);
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/delete_object") {
      string table = getV(mapGET,"table");
      string file = getV(mapGET,"db");
      int id = atoi(getV(mapGET,"id").c_str());
      
      string message = http_delete_object(m_tiles.c_str(), file.c_str(), table.c_str(), id);
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/find_street" || szFileName == "/find_dom" || szFileName == "/find_addr2") {
      string pat = getV(mapGET, "pat");

      string message = "";

      if (szFileName == "/find_street") {
        message = http_get_street(pat.c_str());
      }
      else if (szFileName == "/find_dom") {
        message = http_get_dom(pat.c_str());
      }
      else if (szFileName == "/find_addr2") {
        message = http_get_addr2(pat.c_str());
      }
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/F7") {
      string message = http_get_geo_list();
      readJson(&req, message.c_str());
    }

    

    else if (szFileName == "/get_map_init") {
      int internalNodeID = atoi(getV(mapGET,"internalNodeID").c_str());
      int width = atoi(getV(mapGET,"width").c_str());
      int height = atoi(getV(mapGET,"height").c_str());
      string message = http_get_map_rect(internalNodeID, width, height);
      readJson(&req, message.c_str());
    }


    else if (szFileName == "/get_rect") {
      int internalNodeID = atoi(getV(mapGET,"internalNodeID").c_str());
      string message = http_get_map_rect(internalNodeID, 0, 0);
      readJson(&req, message.c_str());
    }

    else if (szFileName == "/find_node") {
      string file = getV(mapGET,"db");
      string table = getV(mapGET,"table");
      int id = atoi(getV(mapGET,"id").c_str());

      string message = http_get_node(file.c_str(), table.c_str(), id);
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/search_node") {
      string text = getV(mapGET,"text");
      string message = http_search_node(text.c_str());
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/find_line") {
      string file = getV(mapGET,"db");
      string table = getV(mapGET,"table");
      int id = atoi(getV(mapGET,"id").c_str());

      string message = http_get_line(file.c_str(), table.c_str(), id);
      
      readJson(&req, message.c_str());
    }

    else if (szFileName == "/get_tg_graph") {
      int id = atoi(getV(mapGET,"id").c_str());

      string message = get_tg_graph(id);
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/get_tg_table") {
      int id = atoi(getV(mapGET,"id").c_str());

      string message = get_tg_table(id);
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/get_layers") {
      int dpi = atoi(getV(mapGET,"dpi").c_str());
      if (dpi == 0) dpi = 96;
      dpi = 96;

      string message = http_get_layers(dpi);
      readJson(&req, message.c_str());
    }

    else if (szFileName == "/find_geo") {
    //  CriticalSection cr(&thread->cs_find);
      string file = getV(mapGET,"db");
      string table = getV(mapGET,"table");
      int ID = atoi(getV(mapGET,"id").c_str());

      string message = http_get_geo(table.c_str(), ID);
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/update") {
        string j = getV(mapGET,"j");
        string message = http_update(j.c_str());
        readJson(&req, message.c_str());
    }
    else if (szFileName == "/setFields") {
        string j = getV(mapGET,"data");
        string message = http_set_field(j.c_str());
        readJson(&req, message.c_str());
    }
    else if (szFileName == "/get_tree_geo") {
    }
    else if (szFileName == "/get_tree_gid") {
    }
    else if (szFileName == "/get_dropdown") {
      string file = getV(mapGET,"db");
      string q = getV(mapGET,"id");
//      int ID = atoi(getV(mapGET,"ID").c_str());
//      string table = getV(mapGET,"table");
//      int ID = atoi(getV(mapGET,"ID").c_str());

      string message = http_get_dropdown(file.c_str(), q.c_str());
      readJson(&req, message.c_str());
    }
    else if (szFileName == "/gps") {
      double x = atof(getV(mapGET, "x").c_str());
      double y = atof(getV(mapGET, "y").c_str());
      string message = http_get_gps(x, y);
      readJson(&req, message.c_str());
    }
/*
    else if (szFileName == "/getFile") {
      string file = getV(mapGET,"db");
      readFile(&req, file.c_str(), last1.c_str());
    }
*/

#if 0
    else if (szFileName == "/getPassport") {
        int ms = atoi(getV(mapGET, "ms").c_str());
        int rs = atoi(getV(mapGET, "rs").c_str());
//        int id = atoi(getV(mapBody,"id").c_str());
        CString fragments = getV(mapGET, "fragments").c_str();
        CString guid = getV(mapGET, "guid").c_str();

        CString path;
        CString ms_rs = ms > 0 ? "ms" : "rs";
        int id = ms > 0 ? ms : rs;

#if 0
        path = "C:\\Users\\gena1\\AppData\\Local\\Temp\\passport\\p_ms_117.xlsx";
        readFile(&req, path, last1.c_str(), false);
        is_getFile = true;

#else

        bool first = true;

        if (guid != "") {
            auto & it = map_guid_file.find(guid);
            path = it->second;
            first = false;
        }
        else {
            guid = getGuid();
            path.Format("%s\\passport\\p_%s_%d_%s.xlsx", getenv("TEMP"), ms_rs, id, guid);
        }


        if (guid == "" || !IsFile(path)) {
            bool error = false;

            if (first) {
                CString commandLine;
                commandLine.Format("python \"D:\\Qt\\gid8\\python\\passport\\p.py\" -id %d -type \"%s\" -fragments=%s  -out_file \"%s\"",
                    id, ms_rs, fragments, path);

                bool ret = RunProcessWithoutWait((const char*)commandLine);

                if (ret) {
                    map_guid_file[guid] = path;
                }
                else {
                    http_404(&req, "Не могу запустить программу генерации паспортов");
                    error = true;
                }
            }

            if (!error) {
                CString msg;
                msg.Format("{\"guid\" : \"%s\"}", guid);
                readJson(&req, (const char *)msg);

                req.status = "202 Accepted";
            }
        }
        else {
//            path.Format("%skls\\excel\\p.xlsx", argpath());
//            path.Format("%skls\\excel\\p.xlsx", argpath());
            readFile(&req, path, last1.c_str(), false);
            is_getFile = true;
        }
#endif
    }
#endif


    else if (szFileName == "/getPassport") {
        int ms = atoi(getV(mapGET, "ms").c_str());
        int rs = atoi(getV(mapGET, "rs").c_str());
//        CString fragments = getV(mapGET, "fragments").c_str();
        CString guid = getV(mapGET, "guid").c_str();

        CString getParAll();

        CString fragments = getParAll();

//        log1("%d %d %s %s", ms, rs, fragments, guid);

        CString path;
        CString ms_rs = ms > 0 ? "ms" : "rs";
        int id = ms > 0 ? ms : rs;
        bool m202 = false;
        bool error = false;

        CMainFrame* mf = (CMainFrame*)AfxGetMainWnd();

        if (guid == "") {
            guid = getGuid();
            path.Format("%s\\passport\\p_%s_%d_%s.xlsx", getenv("TEMP"), ms_rs, id, guid);

            CString  fr = "";

            if (fragments != "") {
                fr.Format(" -fragments %s", fragments);
            }

            CString args = "";
            CString python_str(const CString & prog, const CString & args);

            CGidrView* pView = getView();
            Cgid6Doc* pDoc = (Cgid6Doc*)pView->GetDocument();

            args.Format("-rdbms MsSql -server %s -database %s -port %d -user %s -id %d -type %s %s  -out_file \"%s\"",
                pDoc->m_IP, pDoc->m_bd_gid, pDoc->m_port, pDoc->m_user, id, ms_rs, fr, path);
    

            const char* password = "";
            if (_putenv_s("tgid_password", pDoc->m_password) != 0) {
                log1("Ошибка при установке переменной окружения.");
            }


            CString commandLine = python_str("passport\\p.py", args);

            log1(commandLine);

//            bool ret = RunProcessWithoutWait((const char*)commandLine);
                
           
            DWORD ret = mf->RunProcessInBackground(commandLine);
            
            if (ret != -1) {
                map_guid_file[guid] = path;
                map_guid_dwProcessId[guid] = ret;
                m202 = true;
            }
            else {
                log1("Не могу запустить программу генерации паспортов");
                http_404(&req, "Не могу запустить программу генерации паспортов");
                error = true;
            }
        }
        else {
            {
                auto& it = map_guid_dwProcessId.find(guid);
                if (it == map_guid_dwProcessId.end()) {
                    http_404(&req, "Неизвестный GUID");
                    error = true;
                }
                else {
                    DWORD code = mf->ProcessCompleted(it->second);
                    if (code != -1 && code != 0) {
                        log1("");
                        http_404(&req, "Программа генерации паспортов завершилась с ошибкой");
                        error = true;
                    }
                }
            }

            if (!error) {

                auto& it = map_guid_file.find(guid);
                if (it == map_guid_file.end()) {
                    http_404(&req, "Неизвестный GUID");
                    error = true;
                }
                else {
                    path = it->second;

                    if (IsFile(path)) {
                        log1("Читаю файл %s", path);
                        readFile(&req, path, last1.c_str(), false);
                        is_getFile = true;
                    }
                    else {
                        m202 = true;
                    }
                }
            }
        }
        if (m202 && !error) {
            CString msg;
            msg.Format("{\"guid\" : \"%s\"}", guid);
            readJson(&req, (const char*)msg);
            req.status = "202 Accepted";
        }
    }


    else if (szFileName == "/getFile") {
        string file = getV(mapGET,"name");
        string path0 = getV(mapGET,"path");
        string format = getV(mapGET,"format");

        CString path = getRealPath(path0.c_str())+file.c_str();

        if (format != "pdf") {
            readFile(&req, path, last1.c_str(), false);
            is_getFile = true;
        }
        else {
            CString pdf_name;
            CString name = GetName(file.c_str());

            log1("pdf1");
            pdf_name.Format("%s\\pdf\\%s.pdf", getenv("TEMP"), name);
            mkdir_for_file(pdf_name);
            log1(pdf_name);
            CString error;

            if (convert_doc_pdf(path, pdf_name, error)) {
                readFile(&req, pdf_name, last1.c_str(), false);
                is_getFile = true;
            }
            else {
              http_404(&req, error);              

/*
              string message = error;
              log1(error);

              JSON json;

              json.add("{");
              json.add("error", message.c_str());
              json.add("}");
              readJson(&req, (const char *)json.str());

              req.status = "404 Resource not found";

              req.message = error;
*/
            }
        }
    }
    else if (szFileName == "/uploadFile") {
        if (file_off > 0) {
            string filename = getV(mapBody, "filename");
            const char *buf = &(body.c_str())[file_off];
            string path0 = getV(mapGET,"path");
            CString path = getRealPath(path0.c_str()) + filename.c_str();

            int len = body.length() - file_off;

            FILE *f = fopen(path, "wb");
            if (f) {
                fwrite(buf, len, 1, f);
                fclose(f);
            }
            //      putFile(&req, path, last1.c_str(), false);
            is_putFile = true;
        }
    }
    else if (szFileName == "/openFile") {
    }


    else if (szFileName == "/uploadFileForm") {
        if (file_off > 0) {
            map<string, string>::const_iterator it = mapBody.begin();

            string tableName = getV(mapBody, "tableName");
            int id = atoi(getV(mapBody,"id").c_str());

            string key = getV(mapBody, "key");
            string file = getV(mapBody, "file");
            string filename = getV(mapBody, "filename");
            string path = getV(mapBody, "filePath");

            CString tmpn;
            
            tmpn.Format("%s\\tempfileform.tmp", getenv("TEMP"));

            FILE *f = fopen(tmpn, "wb");
            if (f) {
                fwrite(body2.c_str(), body2.length(), 1, f);
                fclose(f);
                http_uploadFileForm(path.c_str(), tmpn, filename.c_str(), key.c_str(), tableName.c_str(), id);
            }
            //      putFile(&req, path, last1.c_str(), false);
            is_putFile = true;
        }
    }
    
    
    
    else if (szFileName == "/pro") {
      string message = "";
      
      if (!mg_vcmp(&hm->method, "GET") != 0) {
      }
      if (!mg_vcmp(&hm->method, "POST") != 0) {
        mapGET = mapBody;
      }

      string file = getV(mapGET,"db");
      string table = getV(mapGET,"table");
      int ID = atoi(getV(mapGET,"ID").c_str());
      string login = getV(mapBody,"login");
      string login2 = getV(mapBody,"login2");
      string action = getV(mapGET,"action");
      string query = getV(mapGET,"query");

      if (ID == 0) {
          ID = atoi(getV(mapGET,"id").c_str());
      }


      file = http_get_file(file.c_str(), table.c_str());

/*
      if (action == "UPDATE") {
        string field_list = getV(mapGET,"field_list");
        message = http_get_pro_update_ID(file.c_str(), table.c_str(), field_list.c_str(), ID);
      }
*/


/*
      else if (action == "findTube") {
        double x = atof(getV(mapGET, "x").c_str());
        double y = atof(getV(mapGET, "y").c_str());
        message = http_get_pro_find(ST_UT, 2, x, y);
      }
*/
      if (action == "remonts1") {
        message = http_get_remonts("Запорная арматура");
      }
      else if (action == "objects") {
        message = http_get_pro_table_objects_ID(file.c_str(), table.c_str(), ID);
      }
      else if (action == "query") {
        message = http_get_pro_query(file.c_str(), query.c_str());
      }
      else if (action == "query1") {
        message = http_get_pro_query1(file.c_str(), query.c_str());
      }
      else if (action == "query2") {
        message = http_get_pro_query2(file.c_str(), query.c_str());
      }
      else {
        if (file == string("")) {
          message = http_get_pro_file();
        }
        else {
          if (ID <= 0) {
            message = http_get_pro_table(file.c_str(), table.c_str());
          }
          else {
            message = http_get_pro_table_ID(file.c_str(), table.c_str(), ID);
          }
        }
      }
      readJson(&req, message.c_str());
/*
      if (!mg_vcmp(&hm->method, "POST") != 0) {
        string login = getV(mapBody,"login");
        string login2 = getV(mapBody,"login2");
        string find = getV(mapBody,"find");

        if (find == string("NS")) {
          int ID = atoi(getV(mapGET,"ID").c_str());

          string message = http_get_pro(find.c_str(), ID);
          readJson(&req, message.c_str());
        }
        else {
          std::ostringstream m;

          m << "{";
          m << "\"login1\" : \"" << login << "\",\n";
          m << "\"login2\" : \"" << login2 << "\"\n";

          m << "}";

          string message = m.str();;

  //        string message = string ("{\"file\" : \"")+file +string("\"}");
          readJson(&req, message.c_str());
        }
      }
*/
    }
/*
    else if (szFileName == "/test") {
      int rand_r (unsigned int *seed);
      unsigned int id = GetCurrentThreadId();
      char s[1024];

      seed = seed + id;
      int c1, r1, c2, r2;
      int m = (rand_r(&seed)%14)+1;
      http_get_border(m, c1, r1, c2, r2);

      int r = r1 + rand_r(&seed)%(r2-r1);
      int c = c1 + rand_r(&seed)%(c2-c1);
    
      sprintf(s, "%s\\tiles\\%d\\%d\\%d.png", m_HomeDir.c_str(), m, r, c);
      http_make_png(s, m, r, c);
      szResponse = writeFile(s, "", szDT, bKeepAlive);
    }
*/

    else if (szFileName == "/tiles") {
      int ms = atoi(getV(mapGET,"ms").c_str());
      int rs = atoi(getV(mapGET,"rs").c_str());

      int m = atoi(getV(mapGET,"m").c_str());
      int r = atoi(getV(mapGET,"row").c_str());
      int c = atoi(getV(mapGET,"col").c_str());
      int dpi = atoi(getV(mapGET,"dpi").c_str());
      if (dpi == 0) dpi = 96;
      dpi = 96;
      
      int internalNodeID = atoi(getV(mapGET,"internalNodeID").c_str());
      int geo = atoi(getV(mapGET,"geo").c_str());
      int gid = atoi(getV(mapGET,"gid").c_str());
      int layer = atoi(getV(mapGET,"layer").c_str());
      CString s_layers = getV(mapGET,"layers").c_str();
      std::vector<int> layers = ParseStringToNumbers(s_layers);

      CString filename = tileName(dpi, m, r, c, internalNodeID, layer, gid, geo, ms, rs);
      if (need_new_file(filename, layer, gid)) {
          CString ss;
          bool yes = http_make_png(filename, dpi, m, r, c, internalNodeID, layer, layers.size(), layers.data(), gid, geo, ms, rs);
          if (!yes) {
              filename = tileName("nodata.png");
              if (!IsFile(filename)) {
                  http_make_zero_png(filename);
              }
          }
      }
      else {
//        CString ss;
//        ss.Format("Копирую картинку %s", filename);
//        log1(ss);
      }

      readFile(&req, filename, last1.c_str(), true);
    }
    else {
        string filename = string(dlg->m_szHomeDir) + string("/") + szFileName;

        if (szFileName == "favicon.ico") {
            filename = argpath() + CString("/kls/favicon.ico");
        }

        readFile(&req, filename.c_str(), last1.c_str(), true);
    }

    gzip(&req);
    
    mg_printf(c,"HTTP/1.1 %s\r\n", req.status.c_str());

    mg_printf(c, "Date: %s\r\n", szDT);
    mg_printf(c, "Last-Modified: %s\r\n", req.date.c_str());

    CString server;
    server.LoadString(AFX_IDS_APP_TITLE);

//    mg_printf(c, "Server: TGid-7/0.1\r\n");
    mg_printf(c, "Server: %s\r\n", server);
    mg_printf(c, "Access-Control-Allow-Origin: *\r\n");
  
    if (req.mime != "") {
      mg_printf(c,"Content-Type: %s\r\n", req.mime.c_str());
    }
    mg_printf(c,"Content-Length: %d\r\n", req.length);
    if (req.bKeepAlive) {
      mg_printf(c, "Connection: Keep-Alive\r\n");
    }
    else {
      mg_printf(c, "Connection: close\r\n");
    }
//    if (req.message != "") {
//      mg_printf(c, "Erro: %s\r\n", req.message.c_str());
//    }

    if (req.gzip) {
      mg_printf(c, "Content-Encoding: gzip\r\n");
    }
    
    if (is_getFile && req.file_name != "") {
//        mg_printf(c, "Content-Disposition: attachment; filename=%s\r\n", encode_rfc2231(req.file_name.c_str()));
        mg_printf(c, "Content-Disposition: attachment; filename=\"%s\"\r\n", req.file_name.c_str());
    }


    mg_printf(c,"\r\n");
    
    if (req.length) mg_send(c, req.buf, req.length);

    char addr[32];
    mg_sock_addr_to_str(&c->sa, addr, sizeof(addr), MG_SOCK_STRINGIFY_IP);

    {
    //  CriticalSection cr(&thread->cs_log);
      log_printf("access.log", "%s [%s] - \"GET %s\" [%s] %d %s\n", addr, szDT, url.c_str(), req.status.c_str(), req.length, req.gzip ? "gzip" : "");
    }
    }
  }
}

static void ev_handler80(struct mg_connection *c, int ev, void *p) 
{
  if (ev == MG_EV_HTTP_REQUEST) {
    struct http_message *hm = (struct http_message *) p;
    CHttpParser parser(hm->message.p);
    string host = parser.getV("Host");
    
//    mg_printf(c,"HTTP/1.1 301 Moved Permanently\r\n");
//    mg_printf(c,"Content-Length: %d\r\n", 0);
//    mg_printf(c,"Location: https://%s\r\n", host.c_str());
//    mg_printf(c, "Connection: close\r\n");    
//    mg_printf(c,"\r\n");

    char s[1256];

    sprintf(s,"https://%s\r\n", host.c_str());

    mg_http_send_redirect(c, 301, mg_mk_str(s), mg_mk_str(NULL));
    c->flags |= MG_F_SEND_AND_CLOSE;


    string url = parser.url;
    ispr(url);
    char szDT[200];
    curDate(szDT);

    char addr[32];
    mg_sock_addr_to_str(&c->sa, addr, sizeof(addr), MG_SOCK_STRINGIFY_IP);

    log_printf("access.log", "%s [%s] - \"GET %s\" [%s]\n", addr, szDT, url.c_str(), "301==");
  }
}


//static const char *s_ssl_cert = "D:/vc_gid/mongoose-master/examples/simplest_web_server_ssl/server.pem";
//static const char *s_ssl_key = "D:/vc_gid/mongoose-master/examples/simplest_web_server_ssl/server.key ";

//static const char *s_ssl_cert = "C:/openssl/bin/1/cert.pem";
//static const char *s_ssl_key = "C:/openssl/bin/1/privateRSA.pem";
//static const char *ssl_ca_cert = "C:/Users/USER/AppData/Roaming/letsencrypt-win-simple/httpsacme-v01.api.letsencrypt.org/ca-0A0141420000015385736A0B85ECA708-crt.pem";


static const char *s_ssl_cert = "C:/Users/USER/AppData/Roaming/letsencrypt-win-simple/httpsacme-v01.api.letsencrypt.org/gena1967.ddns.net-crt.pem";
static const char *s_ssl_key = "C:/Users/USER/AppData/Roaming/letsencrypt-win-simple/httpsacme-v01.api.letsencrypt.org/gena1967.ddns.net-key.pem";
//static const char *ssl_ca_cert = "C:\\Users\\USER\\AppData\\Roaming\\letsencrypt-win-simple\\httpsacme-v01.api.letsencrypt.org\\ca-0A0141420000015385736A0B85ECA708-crt.pem";
static const char *ssl_ca_cert = "C:/Users/USER/AppData/Roaming/letsencrypt-win-simple/httpsacme-v01.api.letsencrypt.org/gena1967.ddns.net-chain.pem";


BOOL CHttpThread::InitInstance()
{
  char s_http_port[256];

//  char path1[512];
//  HRESULT ret = SHGetSpecialFolderPath(NULL,path1,CSIDL_APPDATA, 1);
//  m_tiles = string(path1)+"\\Sirius\\tgid\\";

  m_tiles = m_dlg->m_szTilesDir;

  sprintf(s_http_port, "%d", m_port);

  struct mg_mgr mgr;
  struct mg_connection *nc;
  struct mg_bind_opts bind_opts;
  const char *err;

  mg_mgr_init(&mgr, (void*) this);

  BOOL is_ssl = m_is_ssl;
  memset(&bind_opts, 0, sizeof(bind_opts));
  bind_opts.error_string = &err;

  if (is_ssl) {
//    bind_opts.ssl_ca_cert = ssl_ca_cert;
    bind_opts.ssl_cert = m_dlg->m_szPublic;
    bind_opts.ssl_key = m_dlg->m_szPrivate;
  }

  if (m_progr == 1) {
    nc = mg_bind_opt(&mgr, s_http_port, ev_handler80, bind_opts);
  }
  else {
    nc = mg_bind_opt(&mgr, s_http_port, ev_handler, bind_opts);
  }

  if (nc) {
/*
    InitializeCriticalSection(&cs_log);
    InitializeCriticalSection(&cs_png);
    InitializeCriticalSection(&cs_db);
    InitializeCriticalSection(&cs_find);
    InitializeCriticalSection(&cs_file);
*/

    mg_set_protocol_http_websocket(nc);
    s_http_server_opts.document_root = m_dlg->m_szHomeDir;

//    mg_register_http_endpoint(nc, "/login.html", login_handler);
    mg_register_http_endpoint(nc, "/index.html", login_handler);
//    mg_register_http_endpoint(nc, "/f5", f5_handler);
//    mg_register_http_endpoint(nc, "/logout", logout_handler);
    mg_set_timer(nc, mg_time() + SESSION_CHECK_INTERVAL);

    if (!is_ssl) {
    /* For each new connection, execute ev_handler in a separate thread */
//      mg_enable_multithreading(nc);
    }

    for (;!isExit;) {
      mg_mgr_poll(&mgr, m_pto*1000);
    }
/*
    DeleteCriticalSection(&cs_log);
    DeleteCriticalSection(&cs_png);
    DeleteCriticalSection(&cs_db);
    DeleteCriticalSection(&cs_find);
    DeleteCriticalSection(&cs_file);
*/
  }
  else {
    if (m_dlg->m_hWnd) {
        AfxMessageBox(err);
    }
  }
  mg_mgr_free(&mgr);

//  AfxMessageBox("Stop");

  return ExitInstance();
}

void CHttpThread::stop()
{
  isExit = TRUE;
}

int CHttpThread::ExitInstance()
{
  ExitThread(0);
  return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CHttpThread, CWinThread)
  //{{AFX_MSG_MAP(CHttpThread)
    // NOTE - the ClassWizard will add and remove mapping macros here.
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHttpThread message handlers


