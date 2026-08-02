#pragma once

#include <io.h>
#include <direct.h>
#include <sys/locking.h>

#define M_PI        3.14159265358979323846

//#define max(x,y)          (((x) > (y)) ? (x) : (y))
//#define min(x,y)          (((x) < (y)) ? (x) : (y))

//int _far _pascal MulDiv(int, int, int);

inline int imd(int x, int y, int z) {
  if (z == 0) return 1;
  return x*y/z;
}

class ClockMessage
{
public:
  ClockMessage(const char *_txt) 
  {
    txt = _txt;
    t1 = clock();
  }
  ~ClockMessage() 
  {
    CString str;
    clock_t t2 = clock();
    str.Format("%s %g секунд", txt, (double)(t2-t1) / CLOCKS_PER_SEC);
    AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
  }
private:
  clock_t t1;
  CString txt;
};

class CriticalSection
{
public:
  CriticalSection(CRITICAL_SECTION *_cs) 
  {
    cs = _cs;
    EnterCriticalSection(cs);
  }
  ~CriticalSection() 
  {
    LeaveCriticalSection(cs);
  }
private:
  CRITICAL_SECTION *cs;
};


class ChDir {
public:
  ChDir(const char *dir) {
    _getcwd(path, 255);
    drive = -1;
    if (dir[1] == ':') {
      drive = _getdrive();
      _chdrive(dir[0] >= 'a' ? dir[0]-'a' : dir[0]-'A');
    }
    _chdir(dir);
  }
  ~ChDir() {
    if (drive >= 0) _chdrive(drive);
    _chdir(path);
  }
private:
  int drive;
  char path[256];
};

class WaitCursor {
public :
  WaitCursor(LPCTSTR idc = IDC_WAIT) {
    hcur = ::SetCursor(::LoadCursor(NULL,idc));
  }
  ~WaitCursor() {
    SetCursor(hcur);
  }
private :
  HCURSOR hcur;
};


CString GetName(const char *path);
CString GetFName(const char *path);
CString GetTitle(CString path);




int IsFile(const TCHAR *fN);
int imd (int m1,int m2, int d);

CString argpath();
CString argpath_2();
CString python_exe();
CString pythonw_exe();


FILE *fopen_tabs(const TCHAR *mdb, const TCHAR *tn, const TCHAR *ext, const TCHAR *r);

FILE *fopenexe(const TCHAR *fN, const TCHAR *r);
FILE *fopenakt(const TCHAR *fN, const TCHAR *r);
FILE *fopendb(const TCHAR *fN, const TCHAR *r);
void trim(TCHAR *s);
void ltrim(TCHAR *s);
void toupper(TCHAR *s);

int fgetstr(TCHAR *s, int n, FILE *f);
CString fgetstr(FILE *f);

void skipbl(FILE *f);
void readbl(FILE *f, TCHAR *s);
void tmpfullname(TCHAR *tmpN);

void torus(TCHAR *s);

double okr3(double x, int dX, bool yes);

void OemToAnsiRus(TCHAR *s1, TCHAR *s2);
void AnsiToOemRus(TCHAR *s1, TCHAR *s2);
void AnsiUpperRus(TCHAR *s);

void Status(int n, LPCTSTR txt);


void SaveFont(const TCHAR *szSection, const LOGFONT &lf, COLORREF color);
bool LoadFont(const TCHAR *szSection, LOGFONT &lf, COLORREF &color);
bool LoadFont2(const TCHAR *szSection, LOGFONT &lf, COLORREF &color);

bool isNul(double q);

void log(const char *podp, const char *s);
void log2(const char *podp, int n, const char *txt);

#define LOG3(x) log2(__FILE__, __LINE__, x)
#define LOG2() log2(__FILE__, __LINE__, "")


CString GetTitle(CString path);
CString GetExt(CString path);
CString GetPath(CString path);


void d_plus();
void m_plus();
void disk_memo(int &d, int &m);

int rgb_l(int color);

CString NameForProfile(const char *fn);

CString readFile(const CString &fn);
void saveToFile(const CString &fn, const CString &txt);

BOOL GetDirectory(CString &strDir);
bool getDir(CWnd *wnd, CString &dir);

CString get8();
CString get10();
CString get11();
CString get15();

CString getMapSprav();

//CString getVyd();
//CString getHelp_Map();
//CString getHelp_OUT();
//CString getHelpVspomogat();
//CString getHelpGrafIstoch();
//CString getHelpOsnovn();
//CString getHelpSparvochn();
//CString getHelpStandObor();
//CString getEdinitsi();
//CString getOsnovn();
//CString getStandObor();
//CString getSpravochnaya();
//CString getVspomogat();
//CString getPsSprav();

bool isColorDark(unsigned long color);
CString cssColor(unsigned long color);


int get_tr();
int get_bk_color();
void setTransp(int tr, int bk_color);

int regex_match(const char *p, const char *buf, vector<CString> & match);

int WeekDays(int d, int m, int y);
int jdn(int d, int m, int y);
int jdn(const COleDateTime &Date);
void jdn2(int J, int &D, int &M, int &Y);

CString mon_name(int i);
int mon_days(int i, int y);

void savePosition(CWnd *wnd, const char *name);
void loadPosition(CWnd *wnd, const char *name);
double getScreenMas();

void mkdir_for_file(const char *fn);

void log1(const char* format, ...);
//void log1(CString text);
void set_log1(CString fn);

