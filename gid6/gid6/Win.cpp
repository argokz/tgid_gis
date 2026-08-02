#include "stdafx.h"

#include "win.h"
#include "math.h"

void torus(char *s) {
  char b[] = "Ђ‚‘D…FGЌIJЉLЊNЋђQRS’UVW•YZ";
  char m[] = " bбdҐfghijklmn®аqrstuvwеyz";
  int i;

#ifndef LITVA
  for ( i = 0; s[i]; i++ ) {
    if ('A' <= s[i] && s[i] <= 'Z') s[i] = b[s[i]-'A'];
    if ('a' <= s[i] && s[i] <= 'z') s[i] = m[s[i]-'a'];
  }
#endif
}


CString GetPath(CString path) {
  CString str;
  int i, i1, i2;

  str = path;
  i1 = str.ReverseFind('\\');
  i2 = str.ReverseFind('/');

  i = max(i1, i2);

  if (i >= 0) {
    str = str.Left(i);
//    str += "\\";
  }
  return str;
}


CString GetTitle(CString path) {
  CString str;
  int i;

  str = path;
  i = str.ReverseFind('\\');
  if (i >= 0) str = str.Right(str.GetLength()-i-1);
  i = str.ReverseFind('.');
  if (i >= 0) str = str.Left(i);
  return str;
}

CString GetTitleB(CString path) 
{
  CString str = GetTitle(path);
  str.Replace(' ', '_');

  if (str.GetLength() > 31)   
        str = str.Left(31);

  return str;
}


CString GetFName(const char *path) {
  CString str;
  int i;

  str = path;
  i = str.ReverseFind('.');
  if (i >= 0) str = str.Left(i);
  return str;
}

CString GetName(const char *path) {
  CString str;
  int i;

  str = path;
  i = str.ReverseFind('\\');
  if (i < 0) i = str.ReverseFind('/');
  if (i >= 0) str = str.Right(str.GetLength()-i-1);
  return str;
}


CString GetExt(const char *path) {
  CString str;
  int i;

  str = path;
  i = str.ReverseFind('\\');
  if (i >= 0) str = str.Right(str.GetLength()-i-1);
  i = str.ReverseFind('.');
  if (i >= 0) str = str.Right(str.GetLength()-i-1);
  else str = "";
  return str;
}



CString GetExt(CString path) {
  CString str;
  int i;

  str = path;
  i = str.ReverseFind('\\');
  if (i >= 0) str = str.Right(str.GetLength()-i-1);
  i = str.ReverseFind('.');
  if (i >= 0) str = str.Right(str.GetLength()-i-1);
  else str = "";
  return str;
}


CString argpath_2()
{
    char path1[512];
    HRESULT ret = SHGetSpecialFolderPath(NULL, path1, CSIDL_APPDATA, 1);

    CString path;
    
    path.Format("%s\\Sirius\\tgid\\", path1);

    return path;
}

CString argpath_system32()
{
    char path1[512];
    HRESULT ret = SHGetSpecialFolderPath(NULL, path1, CSIDL_SYSTEM, 1);

    return path1;
}


CString argpath()
{
  int i;
  char path[_MAX_PATH+1];

  ::GetModuleFileName(NULL, path, _MAX_PATH);

  i = lstrlen(path);
  while (path[i-1] != '\\') i--; path[i] = 0;

  CString p = path;

  if (p.Find("\\gid6\\Debug\\") >= 0) {
      p.Replace("\\gid6\\Debug\\", "\\gid6\\gidr\\");
  }
  else if (p.Find("\\gid6\\Release\\") >= 0) {
      p.Replace("\\gid6\\Release\\", "\\gid6\\gidr\\");
  }
  if (p.Find("\\gid6\\gid6\\Debug\\") >= 0) {
      p.Replace("\\gid6\\gid6\\Debug\\", "\\gid6\\gidr\\");
  }
  else if (p.Find("\\gid6\\gid6\\Release\\") >= 0) {
      p.Replace("\\gid6\\gid6\\Release\\", "\\gid6\\gidr\\");
  }



#if _DEBUG
//  p.Replace("/Debug/", "/Release/");
//  p.Replace("\\Debug\\", "\\Release\\");
#endif

  return p;

}


CString python_exe()
{
    CString p = "python.exe";

    CString pp;
    pp.Format("%spython\\python.exe", argpath());

    if (IsFile(pp)) {
        return pp;
    }

    return p;
}


CString pythonw_exe()
{
    CString p = "pythonw.exe";

    CString pp;
    pp.Format("%spython\\pythonw.exe", argpath());

    if (IsFile(pp)) {
        return pp;
    }

    return p;
}


FILE *fopen_tabs(const TCHAR *mdb, const TCHAR *tn, const TCHAR *ext, const TCHAR *r) 
{
  FILE *f;
  CString s;

  s.Format("%stabs/%s/%s.%s", argpath(), GetTitle(mdb), tn, ext);
  f = _tfopen(s, r);
  if (f) return f;
  
  s.Format("%stabs/%s.%s", argpath(), tn, ext);
  f = _tfopen(s, r);
  if (f) return f;

  s.Format("%stabs3/%s.%s", argpath(), tn, ext);
  f = _tfopen(s, r);
  if (f) return f;



  s.Format("%stabs4/%s.%s", argpath(), tn, ext);
  f = _tfopen(s, r);
  if (f) return f;

  s.Format("%s/%s/%s.%s", GetPath(mdb), tn, tn, ext);
  f = _tfopen(s, r);
  if (f) return f;

  return f;

}


FILE *fopenexe(const TCHAR *fN, const TCHAR *r) {
  return _tfopen(argpath()+fN, r);
}


void ltrim(TCHAR *s) {
  int i;
  for (i = 0 ; s[i] == ' '; i++);
  if (s[i]) memmove(s, &s[i], (lstrlen(s)-i+1)*sizeof(TCHAR));
  trim(s);
}

void trim(TCHAR *s) {
  int i;
  if (s[0]) for (i = lstrlen(s)-1 ; i >= 0 && (s[i] == ' ' || s[i] == '\n' || s[i] == '\r') ; i--) s[i]=0;
}


void ug90(double x1, double y1, double x2, double y2, double &xt, double &yt) {
  if (x1 == x2 && y1 == y2) return;

  double a1, b1, c1, c2, d;

//  a1 = y2-y1; b1 = x1-x2; c1 = a1*xt+b1*yt;
//  a2 = x1-x2; b2 = y1-y2; c2 = a2*x2+b2*y2;

//  d = a1*b2-a2*b1;
//  xt = (b2*c1-b1*c2)/d;
//  yt = (a1*c2-a2*c1)/d;
//////////////////////////////

  a1 = y2-y1;
  b1 = x2-x1;

  c1 = a1*xt-b1*yt;
  c2 = b1*x2+a1*y2;

  d = a1*a1+b1*b1;
  xt = (a1*c1+b1*c2)/d;
  yt = (a1*c2-b1*c1)/d;
}

double ugol(double x1, double y1, double x2, double y2, double x3, double y3) {
  double U;

  if ((x3 == x1 && y3 == y1) || (x2 == x1 && y2 == y1)) return 0.;

  U = atan2(y3-y1, x3-x1) - atan2(y2-y1, x2-x1);
  if (U > M_PI) U -= (M_PI*2);
  if (U < -M_PI) U += (M_PI*2);
  return U;
}

int napr(int k, long *fil) {
  int i;
  double U;

  for ( U = 0., i = 1; i < k-1 ; i ++ ) {
    U += ugol(fil[0], fil[1], fil[i*2], fil[i*2+1],
                              fil[(i+1)*2], fil[(i+1)*2 + 1]);
  }
  if ( U < 0.) return -1;
  return 1;
}

int napr(int k, int *x, int *y) {
  int i;
  double U;

  for ( U = 0., i = 1; i < k-1 ; i ++ )
      U += ugol(x[0], y[0], x[i], y[i], x[i+1], y[i+1]);

  if ( U < 0.) return -1;
  return 1;
}

int napr(int k, double *x, double *y) {
  int i;
  double U;

  for ( U = 0., i = 1; i < k-1 ; i ++ )
      U += ugol(x[0], y[0], x[i], y[i], x[i+1], y[i+1]);

  if ( U < 0.) return -1;
  return 1;
}
/*
void OemToAnsiKaz(const char *s1, char *s2) {
  int i;
  unsigned char ss[2]; ss[1] = 0;

  for (i = 0; s1[i]; i++ ) {
    ss[0] = s1[i];

    switch ( ss[0] ) {
      case 240: ss[0] = 170; break;
      case 241: ss[0] = 165; break;
      case 242: ss[0] = 177; break;
      case 243: ss[0] = 179; break;
      case 244: ss[0] = 188; break;
      case 245: ss[0] = 161; break;
      case 246: ss[0] = 163; break;
      case 247: ss[0] = 174; break;

      case 248: ss[0] = 186; break;
      case 249: ss[0] = 166; break;
      case 250: ss[0] = 178; break;
      case 251: ss[0] = 180; break;
      case 252: ss[0] = 189; break;
      case 253: ss[0] = 162; break;
      case 254: ss[0] = 164; break;
      case 255: ss[0] = 175; break;
      case 'I': ss[0] = 190; break;
      case 'i': ss[0] = 191; break;

      case 'N': ss[0] = 185; break;

      default :
        OemToAnsi(ss, ss);
        break;
    }
    s2[i] = ss[0];
  }
  s2[i] = 0;
}

void AnsiToOemKaz(const char *s1, char *s2) {
  int i;
  unsigned char ss[2]; ss[1] = 0;

  for (i = 0; s1[i]; i++ ) {
    ss[0] = s1[i];
    switch ( ss[0] ) {
      case 170: ss[0] = 240; break;
      case 165: ss[0] = 241; break;
      case 177: ss[0] = 242; break;
      case 179: ss[0] = 243; break;
      case 188: ss[0] = 244; break;
      case 161: ss[0] = 245; break;
      case 163: ss[0] = 246; break;
      case 174: ss[0] = 247; break;

      case 186: ss[0] = 248; break;
      case 166: ss[0] = 249; break;
      case 178: ss[0] = 250; break;
      case 180: ss[0] = 251; break;
      case 189: ss[0] = 252; break;
      case 162: ss[0] = 253; break;
      case 164: ss[0] = 254; break;
      case 175: ss[0] = 255; break;
      case 190: ss[0] = 'I'; break;
      case 191: ss[0] = 'i'; break;

      case 185: ss[0] = 'N'; break;


      default :
        AnsiToOem(ss, ss);
        break;
    }
    s2[i] = ss[0];
  }
  s2[i] = 0;
}
*/

long Mul100(char *s) {
  long x, y;
  int i, k, l, c;

  for ( i = 0; s[i] && s[i] != '.' && s[i] != ','; i++ );
  c = s[i];
  if (c) {
    i++;
    for (l = 0; l < 2; l++, i++) if (s[i] == 0) s[i] = '0';
    s[i] = 0;
  }

  if (c == '.')
      k = sscanf(s, "%ld.%ld", &x, &y);
  else
      k = sscanf(s, "%ld,%ld", &x, &y);

  if (k == 0) return 0L;
  if (k == 1) return x*100;
  return x*100 + ((x > 0) ? y : (-y));
}


int IsFile(const char *fN) {
  return (_access(fN, 00) != -1);
}


int fgetstr(char *s, int n, FILE *f) {
  int i = 0, c;
  do {
    c = s[i++] = getc(f);
  } while ( c != 0 && c != '\n' && !feof(f) && i < n);
  s[i-1] = 0;
  return i-1;
}

CString fgetstr(FILE *f) 
{
  char s[10240]; 
  int n =  10240-1;
  s[0] = 0;

  int i = 0, c;
  do {
    c = s[i++] = getc(f);
  } while ( c != 0 && c != '\n' && !feof(f) && i < n);
  s[i-1] = 0;
  return s;
}


void skipbl(FILE *f) {
  int c;

  do {
    c = getc(f);
  } while ( !feof(f) && c != 12 );
  getc(f);
}

void readbl(FILE *f, char *s) {
  int k = 0, c;

  do {
    s[k++] = c = getc(f);
  } while ( !feof(f) && c != 12 );
  getc(f);
  s[k-1] = 0;
  s[k-2] = 0;
}

void SaveFont(const TCHAR *szSection, const LOGFONT &lf, COLORREF color) {
  AfxGetApp()->WriteProfileString(szSection, _T("FaceName"), lf.lfFaceName);

  AfxGetApp()->WriteProfileInt(szSection, _T("Height"),      lf.lfHeight     );
  AfxGetApp()->WriteProfileInt(szSection, _T("Width"),       lf.lfWidth      );
  AfxGetApp()->WriteProfileInt(szSection, _T("Escapement"),  lf.lfEscapement );
  AfxGetApp()->WriteProfileInt(szSection, _T("Orientation"), lf.lfOrientation);
  AfxGetApp()->WriteProfileInt(szSection, _T("Weight"),      lf.lfWeight     );
  AfxGetApp()->WriteProfileInt(szSection, _T("Italic"),         lf.lfItalic);
  AfxGetApp()->WriteProfileInt(szSection, _T("Underline"),      lf.lfUnderline);
  AfxGetApp()->WriteProfileInt(szSection, _T("StrikeOut"),      lf.lfStrikeOut);
  AfxGetApp()->WriteProfileInt(szSection, _T("CharSet"),        lf.lfCharSet);
  AfxGetApp()->WriteProfileInt(szSection, _T("OutPrecision"),   lf.lfOutPrecision);
  AfxGetApp()->WriteProfileInt(szSection, _T("ClipPrecision"),  lf.lfClipPrecision);
  AfxGetApp()->WriteProfileInt(szSection, _T("Quality"),        lf.lfQuality);
  AfxGetApp()->WriteProfileInt(szSection, _T("PitchAndFamily"), lf.lfPitchAndFamily);

  AfxGetApp()->WriteProfileInt(szSection, _T("Color"), color);
}

bool LoadFont(const TCHAR *szSection, LOGFONT &lf, COLORREF &color) 
{
  CString str = AfxGetApp()->GetProfileString(szSection, _T("FaceName"), "Arial");

  color = 0;
  memset(&lf, 0, sizeof(LOGFONT));

  lstrcpy(lf.lfFaceName, "Arial");
  
  lf.lfHeight = -10;
  lf.lfCharSet = DEFAULT_CHARSET;
  lf.lfEscapement = 0;
  lf.lfOrientation = 0;

  if (str == "") return false;

  lstrcpy(lf.lfFaceName, str);
  lf.lfHeight      =  AfxGetApp()->GetProfileInt(szSection, _T("Height"),      lf.lfHeight     );
  lf.lfWidth       =  AfxGetApp()->GetProfileInt(szSection, _T("Width"),       lf.lfWidth      );
  lf.lfEscapement  =  AfxGetApp()->GetProfileInt(szSection, _T("Escapement"),  lf.lfEscapement );
  lf.lfOrientation =  AfxGetApp()->GetProfileInt(szSection, _T("Orientation"), lf.lfOrientation);
  lf.lfWeight      =  AfxGetApp()->GetProfileInt(szSection, _T("Weight"),      lf.lfWeight     );
  lf.lfItalic      =  AfxGetApp()->GetProfileInt(szSection, _T("Italic"),         lf.lfItalic);
  lf.lfUnderline   =  AfxGetApp()->GetProfileInt(szSection, _T("Underline"),      lf.lfUnderline);
  lf.lfStrikeOut   =  AfxGetApp()->GetProfileInt(szSection, _T("StrikeOut"),      lf.lfStrikeOut);
  lf.lfCharSet     =  AfxGetApp()->GetProfileInt(szSection, _T("CharSet"),        lf.lfCharSet);
  lf.lfOutPrecision = AfxGetApp()->GetProfileInt(szSection, _T("OutPrecision"),   lf.lfOutPrecision);
  lf.lfClipPrecision= AfxGetApp()->GetProfileInt(szSection, _T("ClipPrecision"),  lf.lfClipPrecision);
  lf.lfQuality      = AfxGetApp()->GetProfileInt(szSection, _T("Quality"),        lf.lfQuality);
  lf.lfPitchAndFamily=AfxGetApp()->GetProfileInt(szSection, _T("PitchAndFamily"), lf.lfPitchAndFamily);

  lf.lfCharSet = RUSSIAN_CHARSET;


  color=AfxGetApp()->GetProfileInt(szSection, _T("Color"), color);

  return true;
}

bool LoadFont2(const TCHAR *szSection, LOGFONT &lf, COLORREF &color) 
{
  CString str = AfxGetApp()->GetProfileString(szSection, _T("FaceName"), "???");
  if (str == "???") return false;

  color = 0;
  memset(&lf, 0, sizeof(LOGFONT));
  lf.lfHeight = -10;
  lf.lfCharSet = DEFAULT_CHARSET;
  lf.lfEscapement = 0;
  lf.lfOrientation = 0;

  lstrcpy(lf.lfFaceName, str);
  lf.lfHeight      =  AfxGetApp()->GetProfileInt(szSection, _T("Height"),      lf.lfHeight     );
  lf.lfWidth       =  AfxGetApp()->GetProfileInt(szSection, _T("Width"),       lf.lfWidth      );
  lf.lfEscapement  =  AfxGetApp()->GetProfileInt(szSection, _T("Escapement"),  lf.lfEscapement );
  lf.lfOrientation =  AfxGetApp()->GetProfileInt(szSection, _T("Orientation"), lf.lfOrientation);
  lf.lfWeight      =  AfxGetApp()->GetProfileInt(szSection, _T("Weight"),      lf.lfWeight     );
  lf.lfItalic      =  AfxGetApp()->GetProfileInt(szSection, _T("Italic"),         lf.lfItalic);
  lf.lfUnderline   =  AfxGetApp()->GetProfileInt(szSection, _T("Underline"),      lf.lfUnderline);
  lf.lfStrikeOut   =  AfxGetApp()->GetProfileInt(szSection, _T("StrikeOut"),      lf.lfStrikeOut);
  lf.lfCharSet     =  AfxGetApp()->GetProfileInt(szSection, _T("CharSet"),        lf.lfCharSet);
  lf.lfOutPrecision = AfxGetApp()->GetProfileInt(szSection, _T("OutPrecision"),   lf.lfOutPrecision);
  lf.lfClipPrecision= AfxGetApp()->GetProfileInt(szSection, _T("ClipPrecision"),  lf.lfClipPrecision);
  lf.lfQuality      = AfxGetApp()->GetProfileInt(szSection, _T("Quality"),        lf.lfQuality);
  lf.lfPitchAndFamily=AfxGetApp()->GetProfileInt(szSection, _T("PitchAndFamily"), lf.lfPitchAndFamily);

  lf.lfCharSet = RUSSIAN_CHARSET;

  color=AfxGetApp()->GetProfileInt(szSection, _T("Color"), color);

  return true;
}


#define BIF_NEWDIALOGSTYLE 0x00000040
#define BIF_SHAREABLE 0x00008000


BOOL GetDirectory(CString &strDir)
{
//   ::CoInitialize(0);

   BROWSEINFO  BrInfo;
//   ITEMIDLIST pidlRoot;
//   pidlRoot.mkid

   ZeroMemory(&BrInfo, sizeof(BrInfo));
   BrInfo.hwndOwner = NULL;
   BrInfo.lpszTitle = _TR("Выберите папку");
//   BrInfo.pidlRoot = &pidlRoot;
//   BrInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX;
//   BrInfo.ulFlags |= BIF_NEWDIALOGSTYLE | BIF_SHAREABLE;


   LPITEMIDLIST pidlDestination = SHBrowseForFolder(&BrInfo);
   if(pidlDestination == NULL)
      return FALSE;

   SHGetPathFromIDList(pidlDestination, strDir.GetBuffer(256));
   strDir.ReleaseBuffer();

   IMalloc *pIMalloc;
   SHGetMalloc(&pIMalloc);
   pIMalloc->Free(pidlDestination);
   pIMalloc->Release();

//   ::CoUninitialize();

   return TRUE;
}


bool getDir(CWnd *wnd, CString &dir)
{
  bool b = false;
  CString szHomeDir = "";

  BROWSEINFO  bi;
  char folder_name[MAX_PATH];
  char dir_name[MAX_PATH];
  LPMALLOC lpMalloc; 
  
  bi.hwndOwner = wnd ? wnd->GetSafeHwnd() : 0;
  bi.pidlRoot = NULL;
  bi.pszDisplayName = folder_name;
  lstrcpy(bi.pszDisplayName, dir);

//  bi.lpszTitle = "Please Choose the folder Home Directory of Web Server";
  bi.lpszTitle = _TR("Выберите папку");
  bi.ulFlags = BIF_EDITBOX | BIF_STATUSTEXT;
  bi.lpfn = NULL;
  bi.lParam = NULL;
  bi.iImage = NULL;
  
  LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
  if(pidl)
  {
    SHGetPathFromIDList(pidl, dir_name);
    dir = dir_name;
    b = true;
  }
  
  if(!SHGetMalloc(&lpMalloc) && (lpMalloc != NULL)) 
  { 
    if(pidl != NULL) 
    { 
      lpMalloc->Free(pidl); 
    }  
    lpMalloc->Release(); 
  } 

//  if (dir == "") b = false;

  return b;
}




bool GetPaperSize(DEVMODE *devmode, int &lx, int &ly) {
  lx = devmode->dmPaperWidth;
  ly = devmode->dmPaperLength;

  CString qqq;
  qqq.Format("devmode->dmPaperSize=%d w=%d l=%d", 
  devmode->dmPaperSize ,
  devmode->dmPaperWidth,  
  devmode->dmPaperLength
  
  
  );
  //AfxMessageBox(qqq);


  switch (devmode->dmPaperSize) {
  case 0 :
    lx = devmode->dmPaperWidth;
    ly = devmode->dmPaperLength;
    break;

  case 131            : //   A0
  case 186            : //   A0
  case 191            : //   A0
  case 270            : //   A0
//  case 272            : //   A0
  case 622            : //   A0
  case 2059            : //   A0
  case 259 :  // A0 Ђ«¬ вл pdf
    lx = 8400; ly = 11840;  break;
  case 134 :  // A0 Ђ«¬ вл pdf
    lx = 8410; ly = 11888;  break;

  
  case 122            : //   A1
  case 132            : //   A1
  case 190            : //   A1
  case 271            : //   A1
  case 621            : //   A1
  case 2058            : //   A1
  case 162 :
  case 260 :  // A1 Ђ«¬ вл pdf
    lx = 5920; ly = 8400;   break;
  case 133 :  // A1 Ђ«¬ вл pdf
    lx = 5944; ly = 8410;   break;

  case 272            : //   A2
    lx = 4200; ly = 5920;   break;

  case 273            : //   A3
    lx = 2960; ly = 4200;   break;


  case DMPAPER_LETTER            : //  1  /* Letter 8 1/2 x 11 in               */
    lx = 2159; ly = 2794; break;
  case DMPAPER_LETTERSMALL       : //  2  /* Letter Small 8 1/2 x 11 in         */
    lx = 2159; ly = 2794; break;
  case DMPAPER_TABLOID           : //  3  /* Tabloid 11 x 17 in                 */
    lx = 2794; ly = 4318; break;
  case DMPAPER_LEDGER            : //  4  /* Ledger 17 x 11 in                  */
    lx = 4318; ly = 2794; break;
  case DMPAPER_LEGAL             : //  5  /* Legal 8 1/2 x 14 in                */
    lx = 2159; ly = 3556; break;
  case DMPAPER_STATEMENT         : //  6  /* Statement 5 1/2 x 8 1/2 in         */
    lx = 1397; ly = 2159; break;
  case DMPAPER_EXECUTIVE         : //  7  /* Executive 7 1/4 x 10 1/2 in        */
    lx = 1842; ly = 2667; break;
  case DMPAPER_A3                : //  8  /* A3 297 x 420 mm                    */
    lx = 2970; ly = 4200; break;
  case DMPAPER_A4                : //  9  /* A4 210 x 297 mm                    */
    lx = 2100; ly = 2970; break;
  case DMPAPER_A4SMALL           : // 10  /* A4 Small 210 x 297 mm              */
    lx = 2100; ly = 2970; break;
  case DMPAPER_A5                : // 11  /* A5 148 x 210 mm                    */
    lx = 1480; ly = 2100; break;
  case DMPAPER_B4                : // 12  /* B4 (JIS) 250 x 354                 */
    lx = 2500; ly = 3540; break;
  case DMPAPER_B5                : // 13  /* B5 (JIS) 182 x 257 mm              */
    lx = 1820; ly = 2570; break;
  case DMPAPER_FOLIO             : // 14  /* Folio 8 1/2 x 13 in                */
    lx = 2159; ly = 3302; break;
  case DMPAPER_QUARTO            : // 15  /* Quarto 215 x 275 mm                */
    lx = 2150; ly = 2750; break;
  case DMPAPER_10X14             : // 16  /* 10x14 in                           */
    lx = 2540; ly = 3556; break;
  case DMPAPER_11X17             : // 17  /* 11x17 in                           */
    lx = 2794; ly = 4318; break;
  case DMPAPER_NOTE              : // 18  /* Note 8 1/2 x 11 in                 */
    lx = 2159; ly = 2794; break;
  case DMPAPER_ENV_9             : // 19  /* Envelope #9 3 7/8 x 8 7/8          */
    lx = 984; ly = 2254; break;
  case DMPAPER_ENV_10            : // 20  /* Envelope #10 4 1/8 x 9 1/2         */
    lx = 1048; ly = 2413; break;
  case DMPAPER_ENV_11            : // 21  /* Envelope #11 4 1/2 x 10 3/8        */
    lx = 1143; ly = 2635; break;
  case DMPAPER_ENV_12            : // 22  /* Envelope #12 4 \276 x 11           */
    lx = 1206; ly = 2794; break;
  case DMPAPER_ENV_14            : // 23  /* Envelope #14 5 x 11 1/2            */
    lx = 1270; ly = 2921; break;
  case DMPAPER_CSHEET            : // 24  /* C size sheet                       */
    lx = 4318; ly = 5588; break;
  case DMPAPER_DSHEET            : // 25  /* D size sheet                       */
    lx = 5588; ly = 8636; break;
  case DMPAPER_ESHEET            : // 26  /* E size sheet                       */
    lx = 8636; ly = 11176; break;
  case DMPAPER_ENV_DL            : // 27  /* Envelope DL 110 x 220mm            */
    lx = 1100; ly = 2200; break;
  case DMPAPER_ENV_C5            : // 28  /* Envelope C5 162 x 229 mm           */
    lx = 1620; ly = 2290; break;
  case DMPAPER_ENV_C3            : // 29  /* Envelope C3  324 x 458 mm          */
    lx = 3240; ly = 4580; break;
  case DMPAPER_ENV_C4            : // 30  /* Envelope C4  229 x 324 mm          */
    lx = 2290; ly = 3240; break;
  case DMPAPER_ENV_C6            : // 31  /* Envelope C6  114 x 162 mm          */
    lx = 1140; ly = 1620; break;
  case DMPAPER_ENV_C65           : // 32  /* Envelope C65 114 x 229 mm          */
    lx = 1140; ly = 2290; break;
  case DMPAPER_ENV_B4            : // 33  /* Envelope B4  250 x 353 mm          */
    lx = 2500; ly = 3530; break;
  case DMPAPER_ENV_B5            : // 34  /* Envelope B5  176 x 250 mm          */
    lx = 1760; ly = 2500; break;
  case DMPAPER_ENV_B6            : // 35  /* Envelope B6  176 x 125 mm          */
    lx = 1760; ly = 1250; break;
  case DMPAPER_ENV_ITALY         : // 36  /* Envelope 110 x 230 mm              */
    lx = 1100; ly = 2300; break;
  case DMPAPER_ENV_MONARCH       : // 37  /* Envelope Monarch 3.875 x 7.5 in    */
    lx = 984; ly = 1905; break;
  case DMPAPER_ENV_PERSONAL      : // 38  /* 6 3/4 Envelope 3 5/8 x 6 1/2 in    */
    lx = 921; ly = 1651; break;
  case DMPAPER_FANFOLD_US        : // 39  /* US Std Fanfold 14 7/8 x 11 in      */
    lx = 3778; ly = 2794; break;
  case DMPAPER_FANFOLD_STD_GERMAN: // 40  /* German Std Fanfold 8 1/2 x 12 in   */
    lx = 2159; ly = 3048; break;
  case DMPAPER_FANFOLD_LGL_GERMAN: // 41  /* German Legal Fanfold 8 1/2 x 13 in */
    lx = 2159; ly = 3302; break;
  case DMPAPER_ISO_B4            : // 42  /* B4 (ISO) 250 x 353 mm              */
    lx = 2500; ly = 3530; break;
  case DMPAPER_JAPANESE_POSTCARD : // 43  /* Japanese Postcard 100 x 148 mm     */
    lx = 1000; ly = 1480; break;
  case DMPAPER_9X11              : // 44  /* 9 x 11 in                          */
    lx = 2286; ly = 2794; break;
  case DMPAPER_10X11             : // 45  /* 10 x 11 in                         */
    lx = 2540; ly = 2794; break;
  case DMPAPER_15X11             : // 46  /* 15 x 11 in                         */
    lx = 3810; ly = 2794; break;
  case DMPAPER_ENV_INVITE        : // 47  /* Envelope Invite 220 x 220 mm       */
    lx = 2200; ly = 2200; break;
  case DMPAPER_RESERVED_48       : // 48  /* RESERVED--DO NOT USE               */
    lx = 0; ly = 0; break;
  case DMPAPER_RESERVED_49       : // 49  /* RESERVED--DO NOT USE               */
    lx = 0; ly = 0; break;
  case DMPAPER_LETTER_EXTRA      : // 50  /* Letter Extra 9 \275 x 12 in        */
    lx = 2356; ly = 3048; break;
  case DMPAPER_LEGAL_EXTRA       : // 51  /* Legal Extra 9 \275 x 15 in         */
    lx = 2356; ly = 3810; break;
  case DMPAPER_TABLOID_EXTRA     : // 52  /* Tabloid Extra 11.69 x 18 in        */
    lx = 2969; ly = 4572; break;
  case DMPAPER_A4_EXTRA          : // 53  /* A4 Extra 9.27 x 12.69 in           */
    lx = 2355; ly = 3223; break;
  case DMPAPER_LETTER_TRANSVERSE : // 54  /* Letter Transverse 8 \275 x 11 in   */
    lx = 2102; ly = 2794; break;
  case DMPAPER_A4_TRANSVERSE     : // 55  /* A4 Transverse 210 x 297 mm         */
    lx = 2100; ly = 2970; break;
  case DMPAPER_LETTER_EXTRA_TRANSVERSE : // 56 /* Letter Extra Transverse 9\275 x 12 in */
    lx = 2356; ly = 3048; break;
  case DMPAPER_A_PLUS            : // 57  /* SuperA/SuperA/A4 227 x 356 mm      */
    lx = 2270; ly = 3560; break;
  case DMPAPER_B_PLUS            : // 58  /* SuperB/SuperB/A3 305 x 487 mm      */
    lx = 3050; ly = 4870; break;
  case DMPAPER_LETTER_PLUS       : // 59  /* Letter Plus 8.5 x 12.69 in         */
    lx = 2159; ly = 3223; break;
  case DMPAPER_A4_PLUS           : // 60  /* A4 Plus 210 x 330 mm               */
    lx = 2100; ly = 3300; break;
  case DMPAPER_A5_TRANSVERSE     : // 61  /* A5 Transverse 148 x 210 mm         */
    lx = 1480; ly = 2100; break;
  case DMPAPER_B5_TRANSVERSE     : // 62  /* B5 (JIS) Transverse 182 x 257 mm   */
    lx = 1820; ly = 2570; break;
  case DMPAPER_A3_EXTRA          : // 63  /* A3 Extra 322 x 445 mm              */
    lx = 3220; ly = 4450; break;
  case DMPAPER_A5_EXTRA          : // 64  /* A5 Extra 174 x 235 mm              */
    lx = 1740; ly = 2350; break;
  case DMPAPER_B5_EXTRA          : // 65  /* B5 (ISO) Extra 201 x 276 mm        */
    lx = 2010; ly = 2760; break;
  case DMPAPER_A2                : // 66  /* A2 420 x 594 mm                    */
    lx = 4200; ly = 5940; break;
  case DMPAPER_A3_TRANSVERSE     : // 67  /* A3 Transverse 297 x 420 mm         */
    lx = 2970; ly = 4200; break;
  case DMPAPER_A3_EXTRA_TRANSVERSE : // 68  /* A3 Extra Transverse 322 x 445 mm   */
    lx = 3220; ly = 4450; break;
#if(WINVER >= 0x0500)
  case DMPAPER_DBL_JAPANESE_POSTCARD : // 69 /* Japanese Double Postcard 200 x 148 mm */
    lx = 2000; ly = 1480; break;
  case DMPAPER_A6                : // 70  /* A6 105 x 148 mm                 */
    lx = 1050; ly = 1480; break;
  case DMPAPER_JENV_KAKU2        : // 71  /* Japanese Envelope Kaku #2       */
    lx = 0; ly = 0; break;
  case DMPAPER_JENV_KAKU3        : // 72  /* Japanese Envelope Kaku #3       */
    lx = 0; ly = 0; break;
  case DMPAPER_JENV_CHOU3        : // 73  /* Japanese Envelope Chou #3       */
    lx = 0; ly = 0; break;
  case DMPAPER_JENV_CHOU4        : // 74  /* Japanese Envelope Chou #4       */
    lx = 0; ly = 0; break;
  case DMPAPER_LETTER_ROTATED    : // 75  /* Letter Rotated 11 x 8 1/2 11 in */
    lx = 110; ly = 85; break;
  case DMPAPER_A3_ROTATED        : // 76  /* A3 Rotated 420 x 297 mm         */
    lx = 4200; ly = 2970; break;
  case DMPAPER_A4_ROTATED        : // 77  /* A4 Rotated 297 x 210 mm         */
    lx = 2970; ly = 2100; break;
  case DMPAPER_A5_ROTATED        : // 78  /* A5 Rotated 210 x 148 mm         */
    lx = 2100; ly = 1480; break;
  case DMPAPER_B4_JIS_ROTATED    : // 79  /* B4 (JIS) Rotated 364 x 257 mm   */
    lx = 3640; ly = 2570; break;
  case DMPAPER_B5_JIS_ROTATED    : // 80  /* B5 (JIS) Rotated 257 x 182 mm   */
    lx = 2570; ly = 1820; break;
  case DMPAPER_JAPANESE_POSTCARD_ROTATED : // 81 /* Japanese Postcard Rotated 148 x 100 mm */
    lx = 1480; ly = 1000; break;
  case DMPAPER_DBL_JAPANESE_POSTCARD_ROTATED : //82 /* Double Japanese Postcard Rotated 148 x 200 mm */
    lx = 1480; ly = 2000; break;
  case DMPAPER_A6_ROTATED        : // 83  /* A6 Rotated 148 x 105 mm         */
    lx = 1480; ly = 1050; break;
  case DMPAPER_JENV_KAKU2_ROTATED: // 84  /* Japanese Envelope Kaku #2 Rotated */
    lx = 0; ly = 0; break;
  case DMPAPER_JENV_KAKU3_ROTATED: // 85  /* Japanese Envelope Kaku #3 Rotated */
    lx = 0; ly = 0; break;
  case DMPAPER_JENV_CHOU3_ROTATED: // 86  /* Japanese Envelope Chou #3 Rotated */
    lx = 0; ly = 0; break;
  case DMPAPER_JENV_CHOU4_ROTATED: // 87  /* Japanese Envelope Chou #4 Rotated */
    lx = 0; ly = 0; break;
  case DMPAPER_B6_JIS            : // 88  /* B6 (JIS) 128 x 182 mm           */
    lx = 1280; ly = 1820; break;
  case DMPAPER_B6_JIS_ROTATED    : // 89  /* B6 (JIS) Rotated 182 x 128 mm   */
    lx = 1820; ly = 1280; break;
  case DMPAPER_12X11             : // 90  /* 12 x 11 in                      */
    lx = 3048; ly = 2794; break;
  case DMPAPER_JENV_YOU4         : // 91  /* Japanese Envelope You #4        */
    lx = 0; ly = 0; break;
  case DMPAPER_JENV_YOU4_ROTATED : // 92  /* Japanese Envelope You #4 Rotated*/
    lx = 0; ly = 0; break;
  case DMPAPER_P16K              : // 93  /* PRC 16K 146 x 215 mm            */
    lx = 1460; ly = 2150; break;
  case DMPAPER_P32K              : // 94  /* PRC 32K 97 x 151 mm             */
    lx = 970; ly = 1510; break;
  case DMPAPER_P32KBIG           : // 95  /* PRC 32K(Big) 97 x 151 mm        */
    lx = 970; ly = 1510; break;
  case DMPAPER_PENV_1            : // 96  /* PRC Envelope #1 102 x 165 mm    */
    lx = 1020; ly = 1650; break;
  case DMPAPER_PENV_2            : // 97  /* PRC Envelope #2 102 x 176 mm    */
    lx = 1020; ly = 1760; break;
  case DMPAPER_PENV_3            : // 98  /* PRC Envelope #3 125 x 176 mm    */
    lx = 1250; ly = 1760; break;
  case DMPAPER_PENV_4            : // 99  /* PRC Envelope #4 110 x 208 mm    */
    lx = 1100; ly = 2080; break;
  case DMPAPER_PENV_5            : // 100 /* PRC Envelope #5 110 x 220 mm    */
    lx = 1100; ly = 2200; break;
  case DMPAPER_PENV_6            : // 101 /* PRC Envelope #6 120 x 230 mm    */
    lx = 1200; ly = 2300; break;
  case DMPAPER_PENV_7            : // 102 /* PRC Envelope #7 160 x 230 mm    */
    lx = 1600; ly = 2300; break;
  case DMPAPER_PENV_8            : // 103 /* PRC Envelope #8 120 x 309 mm    */
    lx = 1200; ly = 3090; break;
  case DMPAPER_PENV_9            : // 104 /* PRC Envelope #9 229 x 324 mm    */
    lx = 2290; ly = 3240; break;
  case DMPAPER_PENV_10           : // 105 /* PRC Envelope #10 324 x 458 mm   */
    lx = 3240; ly = 4580; break;
  case DMPAPER_P16K_ROTATED      : // 106 /* PRC 16K Rotated                 */
    lx = 0; ly = 0; break;
  case DMPAPER_P32K_ROTATED      : // 107 /* PRC 32K Rotated                 */
    lx = 0; ly = 0; break;
  case DMPAPER_P32KBIG_ROTATED   : // 108 /* PRC 32K(Big) Rotated            */
    lx = 0; ly = 0; break;
  case DMPAPER_PENV_1_ROTATED    : // 109 /* PRC Envelope #1 Rotated 165 x 102 mm */
    lx = 1650; ly = 1020; break;
  case DMPAPER_PENV_2_ROTATED    : // 110 /* PRC Envelope #2 Rotated 176 x 102 mm */
    lx = 1760; ly = 1020; break;
  case DMPAPER_PENV_3_ROTATED    : // 111 /* PRC Envelope #3 Rotated 176 x 125 mm */
    lx = 1760; ly = 1250; break;
  case DMPAPER_PENV_4_ROTATED    : // 112 /* PRC Envelope #4 Rotated 208 x 110 mm */
    lx = 2080; ly = 1100; break;
  case DMPAPER_PENV_5_ROTATED    : // 113 /* PRC Envelope #5 Rotated 220 x 110 mm */
    lx = 2200; ly = 1100; break;
  case DMPAPER_PENV_6_ROTATED    : // 114 /* PRC Envelope #6 Rotated 230 x 120 mm */
    lx = 2300; ly = 1200; break;
  case DMPAPER_PENV_7_ROTATED    : // 115 /* PRC Envelope #7 Rotated 230 x 160 mm */
    lx = 2300; ly = 1600; break;
  case DMPAPER_PENV_8_ROTATED    : // 116 /* PRC Envelope #8 Rotated 309 x 120 mm */
    lx = 3090; ly = 1200; break;
  case DMPAPER_PENV_9_ROTATED    : // 117 /* PRC Envelope #9 Rotated 324 x 229 mm */
    lx = 3240; ly = 2290; break;
  case DMPAPER_PENV_10_ROTATED   : // 118 /* PRC Envelope #10 Rotated 458 x 324 mm */
    lx = 4580; ly = 3240; break;
#endif /* WINVER >= 0x0500 */
  default :
    return false;
  }

  if (devmode->dmOrientation == DMORIENT_LANDSCAPE) {
    int l = lx; lx = ly; ly = l;
  }

  return true;
}

double okr3(double x, int dX, bool yes) {
  if (dX == 0 || !yes) return x;

  if (x < 0) x -= dX;
  return (long)(x+dX/2)/dX*dX;
}


/*

double okr(double x, int dX) {
  if (dX == 0) return (long) x;

  if (x < 0) x -= dX;
  return (long)(x+dX/2)/dX*dX;
}
*/

void SaveFont(const TCHAR *szSection, const LOGFONT &lf) {
  AfxGetApp()->WriteProfileString(szSection, _T("FaceName"), lf.lfFaceName);

  AfxGetApp()->WriteProfileInt(szSection, _T("Height"),      lf.lfHeight     );
  AfxGetApp()->WriteProfileInt(szSection, _T("Width"),       lf.lfWidth      );
  AfxGetApp()->WriteProfileInt(szSection, _T("Escapement"),  lf.lfEscapement );
  AfxGetApp()->WriteProfileInt(szSection, _T("Orientation"), lf.lfOrientation);
  AfxGetApp()->WriteProfileInt(szSection, _T("Weight"),      lf.lfWeight     );
  AfxGetApp()->WriteProfileInt(szSection, _T("Italic"),         lf.lfItalic);
  AfxGetApp()->WriteProfileInt(szSection, _T("Underline"),      lf.lfUnderline);
  AfxGetApp()->WriteProfileInt(szSection, _T("StrikeOut"),      lf.lfStrikeOut);
  AfxGetApp()->WriteProfileInt(szSection, _T("CharSet"),        lf.lfCharSet);
  AfxGetApp()->WriteProfileInt(szSection, _T("OutPrecision"),   lf.lfOutPrecision);
  AfxGetApp()->WriteProfileInt(szSection, _T("ClipPrecision"),  lf.lfClipPrecision);
  AfxGetApp()->WriteProfileInt(szSection, _T("Quality"),        lf.lfQuality);
  AfxGetApp()->WriteProfileInt(szSection, _T("PitchAndFamily"), lf.lfPitchAndFamily);
}
#if 0
void Status(int n, LPCTSTR txt)
{
//    extern CStatusBarCtrl  *pm_ctlStatus;
//    pm_ctlStatus->SetText(txt, n, 0);

  CMainFrame *mf = (CMainFrame *) AfxGetMainWnd();
}
#endif

#define SIZEOF_ARRAY(ar)  (sizeof(ar)/sizeof((ar)[0]))

// - - - - - - - - - - - - - - - - - - - -

int CALLBACK SHBrowseForFolder_callback(HWND hwnd, UINT uMsg,
     LPARAM lParam, LPARAM lpData)  // in Help: BrowseCallbackProc
{
     TCHAR  tszStat[MAX_PATH];

     switch (uMsg)
     {
     case BFFM_INITIALIZED:
          ::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
          break;

     case BFFM_SELCHANGED:
          if (! SHGetPathFromIDList((LPITEMIDLIST) lParam, tszStat))
               tszStat[0] = 0;
          ::SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM) tszStat);
          break;

     }
     return 0;
}

// - - - - - - - - - - - - - - - - - - - -

bool SelDirDial(CWnd* pParentWnd, CString& cs)

// Это и есть нyжная пpогpамма. Ее можно вставить к себе почти без изменений
// и пpосто вызывать.
// 1-й паpам. - окно, котоpое должно быть pодительским для диалога.
// 2-й паpам. - на входе исходный каталог, на выходе выбpанный.
// Пpи каких-либо ошибках остается без изменения.

{
     bool b = false;
     LPMALLOC     pMalloc;
     BROWSEINFO   bi;
     TCHAR        tszGettingName[MAX_PATH];
     LPITEMIDLIST pBrItemList;
     LPITEMIDLIST pMyCompItemList;
     TCHAR        tszOldPath[MAX_PATH];

     if (SHGetMalloc(&pMalloc) != NOERROR)
          return false;

//     if (SHGetSpecialFolderLocation(pParentWnd->m_hWnd, CSIDL_DRIVES,
     if (SHGetSpecialFolderLocation(pParentWnd->m_hWnd, CSIDL_DESKTOP,
          &pMyCompItemList) != NOERROR)
     {
          pMalloc->Release();
          return false;
     }

     bi.hwndOwner      = pParentWnd->m_hWnd;
          // Здесь пpосто окно пpогpаммы, котоpое должно быть pодительским
          // для окна диалога
     bi.pidlRoot       = pMyCompItemList;
     bi.pszDisplayName = NULL;
     bi.lpszTitle      = NULL;
//     bi.ulFlags        = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
     bi.ulFlags        = BIF_RETURNONLYFSDIRS | BIF_EDITBOX;
     bi.lpfn           = SHBrowseForFolder_callback;
     bi.lParam         = (LPARAM) tszOldPath;
     _tcsncpy(tszOldPath, (LPCTSTR) cs, SIZEOF_ARRAY(tszOldPath));
     tszOldPath[SIZEOF_ARRAY(tszOldPath)-1] = 0;

     pBrItemList = SHBrowseForFolder(&bi);
     if (pBrItemList != NULL)
     {
          if (SHGetPathFromIDList(pBrItemList, tszGettingName)) {
               cs = tszGettingName;
               b = true;
          }
          pMalloc->Free(pBrItemList);
     }

     pMalloc->Free(pMyCompItemList);
     pMalloc->Release();

     return b;
}


void cyr2lat(const char *kod1, char *lcod)
{
  int i;
  char s[20];
  lcod[0] = 0;
  unsigned char c;

  char LAT[32][20] = {"A","B","W","G","D","E","G","Z","I","J","K","L","M","N","O","P","R","S","T","U","F","H","C","C","C","C","Y","Y","Y","E","U","YA"};
  char lat[32][20] = {"a","b","w","g","d","e","g","z","i","j","k","l","m","n","o","p","r","s","t","u","f","h","c","c","c","c","y","y","y","e","u","ya"};
  char *glas = "aeiouyuуеыаоэяиьюёъ";

  for ( i = 0; kod1[i]; i++) {
    c = (unsigned char) kod1[i];
    if ( strchr(glas, c)) {
      continue;
    }

    s[0] = c;
    s[1] = 0;
    if ( (unsigned char)'А' <= c && c <= (unsigned char)'Я') {
      strcat(lcod, LAT[c-(unsigned char)'А']);
    }
    else if ( (unsigned char)'а' <= c && c <= (unsigned char)'я') {
      strcat(lcod, lat[c-(unsigned char)'а']);
    }
    else if ( c == ' ') {
      strcat(lcod, "_");
    }
    else {
      strcat(lcod, s);
    }
  }
  lcod[8] = 0;
}


bool isNul(double q) 
{
//  return fabs(q) == 0;
  return fabs(q) < 1e-2;
}

void log4(const char *podp, const char *s)
{
//  FILE *f = fopen("c:\\ServerRoot\\teplo.log", "a");
//  if (f) {
//    fprintf(f, "%s %s\n", podp, s);
//    fclose(f);
//  }
}


void log2(const char *file, int n, const char *text)
{
//  FILE *f = fopen("d:\\teplo.log", "a");
//  fprintf(f, "%s %d %s\n", file, n, text);
//  fclose(f);
}


static CString log_file = "";

void log2(CString text)
{
    if (log_file == "") return;

    COleDateTime v = COleDateTime::GetCurrentTime();
    CString ss = v.Format("%Y-%m-%d %H:%M:%S");

    FILE *f = fopen(log_file, "a");
    if (f) {
        fprintf(f, "%s ", ss);
        if (text.Find("\n") != -1) {
            fprintf(f, "\n--------------------------------------------------------------------------------\n");
        }

        fprintf(f, "%s\n", text);
        
        if (text.Find("\n") != -1) {
            fprintf(f, "--------------------------------------------------------------------------------\n");
        }
        
        fclose(f);
    }
}


void log1(const char* format, ...) 
{
    if (log_file == "") return;

    va_list args;
    
    // Определяем размер необходимого буфера
    va_start(args, format);
    int size = vsnprintf(nullptr, 0, format, args) + 1; // +1 для нуль-терминатора
    va_end(args);

    if (size <= 0) {
        std::cerr << "Ошибка форматирования строки!" << std::endl;
        return;
    }

    // Выделяем необходимую память
    std::unique_ptr<char[]> buffer(new char[size]);

    // Форматируем строку
    va_start(args, format);
    vsnprintf(buffer.get(), size, format, args);
    va_end(args);

    log2(buffer.get());
}

void log1_clear()
{
    if (log_file == "") return;

    FILE *f = fopen(log_file, "w");
    if (f) {
        fprintf(f, "--------------------------\n");
        fclose(f);
    }
}

void set_log1(CString fn)
{
    log_file = fn;
    log1_clear();
}


void logMy(FILE* f, const char* text)
{
    if (f) {
        fprintf(f, "%s\n", text);
    }
}


//Copied from: http://articles.org.ru/cfaq/index.php?qid=335
//{ h-оттенок, l-яркость, s-насыщенность }

//{ Максимальные значения }
const int hlsmax = 240;
const int rgbmax = 255;
const int undefined = (hlsmax*2) / 3;

void rgbtohls(int r, int g, int b, int &h, int &l, int &s)
{
  int cmax,cmin;
  long rdelta, gdelta, bdelta;

  cmax = max( max(r,g), b);
  cmin = min( min(r,g), b);

  l = ( ((cmax+cmin)*hlsmax) + rgbmax )/(2*rgbmax);

  if (cmax == cmin) {
    s = 0; h = undefined;
  } 
  else {
    if (l <= (hlsmax/2)) 
      s = (((cmax-cmin)*hlsmax) + ((cmax+cmin)/2) ) / (cmax+cmin);
    else
      s = (((cmax-cmin)*hlsmax) + ((2*rgbmax-cmax-cmin)/2) ) / (2*rgbmax-cmax-cmin);

    rdelta = ( ((cmax-r)*(hlsmax/6)) + ((cmax-cmin)/2) ) / (cmax-cmin);
    gdelta = ( ((cmax-g)*(hlsmax/6)) + ((cmax-cmin)/2) ) / (cmax-cmin);
    bdelta = ( ((cmax-b)*(hlsmax/6)) + ((cmax-cmin)/2) ) / (cmax-cmin);

    if (r == cmax) h = (bdelta - gdelta);
    else 
      if (g == cmax) h = (hlsmax/3) + rdelta - bdelta;
    else 
      h =  ((2*hlsmax)/3) + gdelta - rdelta ;

    if (h < 0) h = h + hlsmax;
    if (h > hlsmax) h = h - hlsmax;

  }
  if (s < 0) s = 0; if (s > hlsmax) s = hlsmax;
  if (l < 0) l = 0; if (l > hlsmax) l = hlsmax;
}

long huetorgb(long n1, long n2, long hue)
{
  long result;

  if (hue < 0) hue = hue+hlsmax;
  if (hue > hlsmax) hue = hue -hlsmax;
  if (hue < (hlsmax/6)) 
    result = ( n1 + (((n2-n1)*hue+(hlsmax/12))/(hlsmax/6)) );
  else
    if (hue < (hlsmax/2)) result = n2;
    else
      if (hue < ((hlsmax*2)/3)) 
        result = ( n1 + (((n2-n1)*(((hlsmax*2)/3)-hue)+(hlsmax/12))/(hlsmax/6)));
      else 
        result = n1;

  return result;
};


void hlstorgb(int h, int l, int s, int &r, int &g, int &b)
{
  long magic1,magic2;

  if (s == 0) {
    b = (l*rgbmax)/hlsmax; 
    r = b; 
    g = b;
  } 
  else {

    if (l <= (hlsmax/2))
      magic2 = (l*(hlsmax + s) + (hlsmax/2))/hlsmax;
    else
      magic2 = l + s - ((l*s) + (hlsmax/2))/hlsmax;

    magic1 = 2*l-magic2;
    
    
    r = (huetorgb(magic1, magic2, h+(hlsmax/3)) * rgbmax + (hlsmax/2))/hlsmax;
    g = (huetorgb(magic1, magic2, h           ) * rgbmax + (hlsmax/2))/hlsmax;
    b = (huetorgb(magic1, magic2, h-(hlsmax/3)) * rgbmax + (hlsmax/2))/hlsmax;
  };
  if (r < 0) r = 0; if (r > rgbmax) r = rgbmax;
  if (g < 0) g = 0; if (g > rgbmax) g = rgbmax;
  if (b < 0) b = 0; if (b > rgbmax) b = rgbmax;
};


void shift_int(int &c, int d) 
{
  c += d;

  if (c >= hlsmax) c = hlsmax;
  if (c < 0) c = 0;
}


static int m_nTransp;
static int m_bk_color;

int get_tr() 
{
  return m_nTransp;
}

int get_bk_color() 
{
  return m_bk_color;
}


void setTransp(int tr, int bk_color)
{
  m_nTransp = tr;
  m_bk_color = bk_color;
}


int rgb_l(int color, int tr, int bk_color)
{
  int r, g, b;
  int rb, gb, bb;

//  int tr = m_nTransp;

  if (0 < tr && tr < 255) 
  {
    r = (color&0xFF0000) >> 16;
    g = (color&0x00FF00) >> 8;
    b = (color&0x0000FF);

    rb = (bk_color&0xFF0000) >> 16;
    gb = (bk_color&0x00FF00) >> 8;
    bb = (bk_color&0x0000FF);

/*
  rgbtohls(r, g, b, h, l, s);
  l *= 3;
  shift_int(l, 0);
  hlstorgb(h, l, s, r, g, b);
*/

    r = (rb*(255-tr)+r*tr)/255;
    g = (gb*(255-tr)+g*tr)/255;
    b = (bb*(255-tr)+b*tr)/255;

    color = r<<16|g<<8|b;
  }


  return color;
}



int rgb_l(int color)
{
  return rgb_l(color, m_nTransp, m_bk_color);
}


CString NameForProfile(const char *fn)
{
  CString str = fn;

  if (str.Find("\\\\", 0) == 0) {
    str.SetAt(0, 'N');
  }

  return str;
}


CString readFile(FILE *f)
{
  if (!f) return "";

  CString s = "";

  char str[1024+1];

  while (!feof(f)) {
    if (fgets(str,1024,f) == NULL) break;
    str[1024] = 0;
    s += str;
  }
  return s;
}

CString readFile(const CString &fn)
{
    CString s = "";

    FILE *f = fopen(fn, "r");
    if (f) {
        s = readFile(f);
        fclose(f);
    }
    return s;
}

void saveToFile(const CString &fn, const CString &txt)
{
    FILE *f = fopen(fn, "w");
    if (f) {
        fprintf(f, txt);
        fclose(f);
    }
}



#define PNG_NO_EXTERN 1

#include "png/png.h"


void savePNG(HDC &hdc, int nWidth, int nHeight, const char *fn)
{
  HDC hMemDC = NULL;

  BYTE *lpBitmapBits = NULL; 

  hMemDC = ::CreateCompatibleDC(hdc); 

  BITMAPINFO bi; 
  ZeroMemory(&bi, sizeof(BITMAPINFO));
  bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bi.bmiHeader.biWidth = nWidth;
  bi.bmiHeader.biHeight = nHeight;
  bi.bmiHeader.biPlanes = 1;
  bi.bmiHeader.biBitCount = 24;

  HBITMAP bitmap = ::CreateDIBSection(hMemDC, &bi, DIB_RGB_COLORS, (LPVOID*)&lpBitmapBits, NULL, 0);
  HGDIOBJ oldbmp = ::SelectObject(hMemDC, bitmap); 

  ::BitBlt(hMemDC, 0, 0, nWidth, nHeight, hdc, 0, 0, SRCCOPY);


  int bit_depth = 8;

  FILE *fp = fopen(fn, "wb");
  if (fp) {

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);

  if (png_ptr) {
    png_infop png_info;
    if (!(png_info = png_create_info_struct(png_ptr))) {
        goto destroy_write;
    }

    if (setjmp(png_ptr->jmpbuf)) {
        goto destroy_write;
    }

    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, png_info, nWidth, nHeight,
//                     bit_depth, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
        bit_depth, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
        

                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

//    png_byte trans[] = { 255, 128 }; // entry 0 opaque, entry 1 half alpha
//    int num_trans = 2;

//    png_set_tRNS(png_ptr, png_info, trans, num_trans, NULL);

    png_write_info(png_ptr, png_info);

    int i, k;
    unsigned char *rp2 = new unsigned char[png_ptr->width*png_ptr->height*4];
    unsigned char *rp;

    int NN3 = 3;

    for (i = 0, rp = &lpBitmapBits[png_ptr->width*(png_ptr->height-1)*3] ; i < png_ptr->height; i++, rp  -= png_ptr->width*3) {
      for (k = 0; k < png_ptr->width; k++) {
        unsigned char c = rp[k*3];

//        rp[k*3] = rp[k*3+2];
//        rp[k*3+2] = c;

        unsigned char r = rp[k*3];
        unsigned char g = rp[k*3+1];
        unsigned char b = rp[k*3+2];

        rp2[k*4] = rp[k*3+2];
        rp2[k*4+1] = rp[k*3+1];
        rp2[k*4+2] = c;
        rp2[k*4+3] = 255;

        if (r == 255 && g == 255 && b == 255) {
            rp2[k*4+3] = 0;
        }
      }
      png_write_row(png_ptr, rp2);
    }

    delete [] rp2;

    png_write_end(png_ptr, png_info);


  destroy_write:
      png_destroy_write_struct(&png_ptr, 0);
    }
    fclose(fp);

  }


  ::SelectObject(hMemDC, oldbmp);
  ::DeleteObject(bitmap);
  ::DeleteObject(hMemDC);
}


void savePNG4(HDC &hdc, int x0, int y0, int nWidth, int nHeight, const char *fn)
{
  HDC hMemDC = NULL;

  clock_t t1, t2, t3, t4, t5, t6, t999;

  t1 = clock();

  BYTE *lpBitmapBits = NULL; 

  hMemDC = ::CreateCompatibleDC(hdc); 

  BITMAPINFO bi; 
  ZeroMemory(&bi, sizeof(BITMAPINFO));
  bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bi.bmiHeader.biWidth = nWidth;
  bi.bmiHeader.biHeight = nHeight;
  bi.bmiHeader.biPlanes = 1;
  bi.bmiHeader.biBitCount = 24;

  HBITMAP bitmap = ::CreateDIBSection(hMemDC, &bi, DIB_RGB_COLORS, (LPVOID*)&lpBitmapBits, NULL, 0);
  HGDIOBJ oldbmp = ::SelectObject(hMemDC, bitmap); 

  ::BitBlt(hMemDC, 0, 0, nWidth, nHeight, hdc, x0, y0, SRCCOPY);

  int bit_depth = 8;

  t2 = clock();


  FILE *fp = fopen(fn, "wb");
  if (fp) {

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);

  if (png_ptr) {
    png_infop png_info;
    if (!(png_info = png_create_info_struct(png_ptr))) {
        goto destroy_write;
    }

    if (setjmp(png_ptr->jmpbuf)) {
        goto destroy_write;
    }

    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, png_info, nWidth, nHeight,
//                     bit_depth, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
        bit_depth, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
        

                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);


  t3 = clock();

//    png_byte trans[] = { 255, 128 }; // entry 0 opaque, entry 1 half alpha
//    int num_trans = 2;

//    png_set_tRNS(png_ptr, png_info, trans, num_trans, NULL);

    png_write_info(png_ptr, png_info);

    int i, k;
    unsigned char *rp2 = new unsigned char[png_ptr->width*png_ptr->height*4];
    unsigned char *rp;

    int NN3 = 3;

    for (i = 0, rp = &lpBitmapBits[png_ptr->width*(png_ptr->height-1)*3] ; i < png_ptr->height; i++, rp  -= png_ptr->width*3) {
      for (k = 0; k < png_ptr->width; k++) {
        unsigned char c = rp[k*3];

//        rp[k*3] = rp[k*3+2];
//        rp[k*3+2] = c;

        unsigned char r = rp[k*3];
        unsigned char g = rp[k*3+1];
        unsigned char b = rp[k*3+2];

        rp2[k*4] = rp[k*3+2];
        rp2[k*4+1] = rp[k*3+1];
        rp2[k*4+2] = c;
        rp2[k*4+3] = 255;

        if (r == 255 && g == 255 && b == 255) {
            rp2[k*4+3] = 0;
        }
      }
      png_write_row(png_ptr, rp2);
    }

    delete [] rp2;

    t4 = clock();

    png_write_end(png_ptr, png_info);


  destroy_write:
      png_destroy_write_struct(&png_ptr, 0);
    }
    fclose(fp);

  }

  t5 = clock();


  ::SelectObject(hMemDC, oldbmp);
  ::DeleteObject(bitmap);
  ::DeleteObject(hMemDC);

  t6 = clock();

/*
  log1("Делаю pdf всего %f секунд,  %.1f%%, %.1f%%, %.1f%%, %.1f%%, %.1f%% ",
      (double)(t6 - t1) / CLOCKS_PER_SEC,

      (double)(t2 - t1) / (t6-t1),
      (double)(t3 - t2) / (t6-t1),
      (double)(t4 - t3) / (t6-t1),
      (double)(t5 - t4) / (t6-t1),
      (double)(t6 - t5) / (t6-t1)
      );
*/

}



/* This algorithm is mentioned in the ISO C standard, here extended
   for 32 bits.  */
int rand_r (unsigned int *seed)
{
  unsigned int next = *seed;
  int result;

  next *= 1103515245;
  next += 12345;
  result = (unsigned int) (next / 65536) % 2048;

  next *= 1103515245;
  next += 12345;
  result <<= 10;
  result ^= (unsigned int) (next / 65536) % 1024;

  next *= 1103515245;
  next += 12345;
  result <<= 10;
  result ^= (unsigned int) (next / 65536) % 1024;

  *seed = next;

  return result;
}

/*
bool DirectoryRecursive(const char *path1)
{
  WIN32_FIND_DATA FindFileData;
  char path[MAX_PATH];
  BOOL ret;

  strcpy(path, path1);
  strcat(path,"\\*");

  HANDLE hFind = FindFirstFile(path, &FindFileData);
  do {
    if (strcmp(FindFileData.cFileName, ".") != 0 && strcmp(FindFileData.cFileName, "..") != 0) {
      char str[MAX_PATH];

      strcpy (str,path);
      str[strlen(path)-1] = '\0';
      strcat(str,FindFileData.cFileName);

      DWORD Attributes = GetFileAttributes(str);

      if (Attributes & FILE_ATTRIBUTE_DIRECTORY) {
        DirectoryRecursive(str);
//        _chdir(path1);
        ret = RemoveDirectory(str);
      }
      else {
        ret = DeleteFile(str);
      }

      if (!ret) {
//        DWORD dw = GetLastError();
        LPVOID lpMsgBuf;
        FormatMessage( 
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM | 
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            (LPTSTR) &lpMsgBuf,
            0,
            NULL 
        );

        int q;
        q = 1;
        LocalFree( lpMsgBuf );
      }
    }
  } while(FindNextFile(hFind, &FindFileData));

  return true;
}
*/


int DeleteDirectory(const std::string &refcstrRootDirectory,
                    bool              bDeleteSubdirectories = true)
{
  bool            bSubdirectory = false;       // Flag, indicating whether
                                               // subdirectories have been found
  HANDLE          hFile;                       // Handle to directory
  std::string     strFilePath;                 // Filepath
  std::string     strPattern;                  // Pattern
  WIN32_FIND_DATA FileInformation;             // File information


  strPattern = refcstrRootDirectory + "\\*.*";
  hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
  if(hFile != INVALID_HANDLE_VALUE)
  {
    do
    {
      if(FileInformation.cFileName[0] != '.')
      {
        strFilePath.erase();
        strFilePath = refcstrRootDirectory + "\\" + FileInformation.cFileName;

        if(FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
          if(bDeleteSubdirectories)
          {
            // Delete subdirectory
            int iRC = DeleteDirectory(strFilePath, bDeleteSubdirectories);
            if(iRC)
              return iRC;
          }
          else
            bSubdirectory = true;
        }
        else
        {
          // Set file attributes
          if(::SetFileAttributes(strFilePath.c_str(),
                                 FILE_ATTRIBUTE_NORMAL) == FALSE)
            return ::GetLastError();

          // Delete file
          if(::DeleteFile(strFilePath.c_str()) == FALSE)
            return ::GetLastError();
        }
      }
    } while(::FindNextFile(hFile, &FileInformation) == TRUE);

    // Close handle
    ::FindClose(hFile);

    DWORD dwError = ::GetLastError();
    if(dwError != ERROR_NO_MORE_FILES)
      return dwError;
    else
    {
      if(!bSubdirectory)
      {
        // Set directory attributes
        if(::SetFileAttributes(refcstrRootDirectory.c_str(),
                               FILE_ATTRIBUTE_NORMAL) == FALSE)
          return ::GetLastError();

        // Delete directory
        if(::RemoveDirectory(refcstrRootDirectory.c_str()) == FALSE)
          return ::GetLastError();
      }
    }
  }

  return 0;
}



/*

CString getVyd() 
{ 
  CString tmpName;
  tmpName.Format("%s\\vyd.mdb", getenv("TMP"));

  if (!IsFile(tmpName)) {
    CopyFile(argpath()+"vyd.mdb", tmpName, FALSE);
  }
  return tmpName;
//  return argpath()+"vyd.mdb"; 
}

*/


extern "C" {

#define PCRE_STATIC

#include "pcre-7.0/pcreposix.h"
#include "pcre-7.0/pcre_internal.h"
#include "pcre-7.0/pcre.h"
}

int regex_match(const char *p, const char *buf, vector<CString> & match)
{
  int rc;
  regex_t preg;
  int eflags = 0;
  regmatch_t *pmatch = NULL;
  int cflags = REG_EXTENDED;
  const char *bptr = buf;
  int maxMatches = 20;
  int maxGroups = 10;

  match.clear();

  pmatch = (regmatch_t *)malloc(sizeof(regmatch_t) * maxGroups);

  rc = regcomp(&preg, (char *)p, cflags);
  if (!rc) {
    for (int m = 0; m < maxMatches; m ++) {
      rc = regexec(&preg, (const char *)bptr, maxGroups, pmatch, eflags);
      if (rc) break;

      unsigned int g = 0;
      unsigned int offset = 0;
      for (g = 0; g < maxGroups; g++) {
        if (pmatch[g].rm_so == (size_t)-1) break;  // No more groups

        if (g == 0) offset = pmatch[g].rm_eo;

        char *cursorCopy = new char[strlen(bptr) + 1];
        strcpy(cursorCopy, bptr);
        cursorCopy[pmatch[g].rm_eo] = 0;

//        CString qq;
//        qq.Format("Match %u, Group %u: [%2u-%2u]: %s\n", m, g, pmatch[g].rm_so, pmatch[g].rm_eo, cursorCopy + pmatch[g].rm_so);

        match.push_back(cursorCopy + pmatch[g].rm_so);

        delete [] cursorCopy;
      }
      bptr += offset;
    }
  }

  free(pmatch);
  regfree(&preg);

  return match.size();
}

void regex_replace(CString &s, CString p1, CString p2)
{
    int l;
    do {
       vector<CString> match;
       l = regex_match(p1, s, match);

       if (l >= 1) {
           CString f1 = match[0];
           s.Replace(f1, p2);
       }
    } while (l >= 1);
}

CString cssColor(unsigned long color)
{
  int r = (color&0xFF0000) >> 16;
  int g = (color&0x00FF00) >> 8;
  int b = (color&0x0000FF);
  CString s;

  s.Format("#%-0.2x%-0.2x%-0.2x", b, g, r);

  return s;
}
  


bool isColorDark(unsigned long color)
{
  int r = (color&0xFF0000) >> 16;
  int g = (color&0x00FF00) >> 8;
  int b = (color&0x0000FF);
  
  double darkness = 1.-(0.299*r + 0.587*g + 0.114*b)/255;
  if (darkness < 0.5) {
    return false; // It's a light color
  }
  else{
    return true; // It's a dark color
  }
}


// День недели

int WeekDays(int d, int m, int y) 
{
  int weekday  = ((d += m < 3 ? y-- : y - 2, 23*m/9 + d + 4 + y/4- y/100 + y/400)+6)%7; 
  return weekday;
}

// Юлианский день

int jdn(int d, int m, int y)
{
  int JDN = (1461 * (y + 4800 + (m - 14)/12))/4 +(367 * (m - 2 - 12 * ((m - 14)/12)))/12 - (3 * ((y + 4900 + (m - 14)/12)/100))/4 + d - 32075;
  return JDN;
}

// Юлианский день


int jdn(const COleDateTime &Date)
{
  return jdn((int)Date.GetDay(), (int)Date.GetMonth(), (int)Date.GetYear());
}

// Дата по юлианскому дню

void jdn2(int J, int &D, int &M, int &Y)
{
  int y=4716,v=3,j=1401,u=5,m=2,s=153,n=12,w=2,r=4,B=274277,p=1461,C=-38;


  int f = J + j + (((4 * J + B) / 146097) * 3) / 4 + C;
  int e = r * f + v;
  int g = (e%p) / r;
  int h = u * g + w;
  
  D = ((h%s)) / u + 1;
  M = ((h / s + m)%n) + 1;
  Y = (e / p) - y + (n + m - M) / n;
}

const char *mons[] = {_TR("январь"),_TR("февраль"),_TR("март"),_TR("апрель"),_TR("май"),_TR("июнь"),_TR("июль"),_TR("август"),_TR("сентябрь"),_TR("октябрь"),_TR("ноябрь"),_TR("декабрь")};
const char *Mons[] = {_TR("Январь"),_TR("Февраль"),_TR("Март"),_TR("Апрель"),_TR("Май"),_TR("Июнь"),_TR("Июль"),_TR("Август"),_TR("Сентябрь"),_TR("Октябрь"),_TR("Ноябрь"),_TR("Декабрь")};

const char *mons2[] = {_TR("января"),_TR("февраля"),_TR("марта"),_TR("апреля"),_TR("мая"),_TR("июня"),_TR("июля"),_TR("августа"),_TR("сентября"),_TR("октября"),_TR("ноября"),_TR("декабрь")};


int m_days[] = {31,28,31,30,31,30,31,31,30,31,30,31};


CString mon_name(int i)
{
  if (i < 1 || i > 12) return "";
  return mons[i-1];
}

CString mon_name2(int i)
{
  if (i < 1 || i > 12) return "";
  return mons2[i-1];
}


CString Mon_name(int i)
{
  if (i < 1 || i > 12) return "";
  return Mons[i-1];
}

int mon_days(int i, int y)
{
  if (i < 1 || i > 12) return 0;
  if (i == 2 && y%4 == 0) return 29;
  return m_days[i-1];
}



#define PNG_NO_EXTERN 1

#include "png/png.h"


void savePNG(const char *fn, int nWidth, int nHeight, const char *buf)
{
  HDC hMemDC = NULL;

  int bit_depth = 8;

  FILE *fp = fopen(fn, "wb");
  if (fp) {

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);

  unsigned char *rp = new unsigned char [nWidth*4];

  if (png_ptr) {
    png_infop png_info;
    if (!(png_info = png_create_info_struct(png_ptr))) {
        goto destroy_write;
    }

    if (setjmp(png_ptr->jmpbuf)) {
        goto destroy_write;
    }

    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, png_info, nWidth, nHeight,
                     bit_depth, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, png_info);

    int i, k;

    for (i = 0 ; i < png_ptr->height; i++) {
      int kk = nWidth*(nHeight-i-1)*4;

      for (k = 0; k < png_ptr->width; k++) {
        rp[k*3+0] = buf[kk+k*4+2];
        rp[k*3+1] = buf[kk+k*4+1];
        rp[k*3+2] = buf[kk+k*4+0];
      }
      png_write_row(png_ptr, rp);
    }
    png_write_end(png_ptr, png_info);

  destroy_write:
      delete [] rp;
      png_destroy_write_struct(&png_ptr, 0);
    }
    fclose(fp);

  }
}


bool readDIB(const char *buf)
{
  BITMAPINFOHEADER *header;

  header = (BITMAPINFOHEADER *) buf;

  savePNG("E:\\Рисунки_Акты\\АРГ\\100 - есть\\qq.png" , header->biWidth, header->biHeight, buf+40);

  return true;
}



void savePNG(HDC &hdc, int nWidth, int nHeight, const char *fn);


bool CopyBuf(const char *fn)
{
  if (OpenClipboard(0)) {
    HANDLE hClipboardData = GetClipboardData(CF_DIB);

    if (hClipboardData) {

      char *pchData = (char*)GlobalLock(hClipboardData);

      if (pchData) {
//        readDIB(pchData);
        BITMAPINFOHEADER *header;
        header = (BITMAPINFOHEADER *) pchData;

        savePNG(fn, header->biWidth, header->biHeight, pchData+sizeof(BITMAPINFOHEADER));
      }
      GlobalUnlock(hClipboardData);
      CloseClipboard();    
      return true;
    }


    HENHMETAFILE hemf; 

    hemf = (HENHMETAFILE) GetClipboardData(CF_ENHMETAFILE);

    if (hemf ) {
      ENHMETAHEADER header;

      GetEnhMetaFileHeader(hemf, sizeof(ENHMETAHEADER), &header);

      int dx = header.rclFrame.right/20, dy = header.rclFrame.bottom/20;
      dx = (dx+7)/8*8;
      
      CClientDC oDC(AfxGetMainWnd( ));

      CDC dc;
      dc.CreateCompatibleDC(&oDC);
 
      CBitmap bitmap;
      bitmap.CreateCompatibleBitmap(&oDC, dx, dy);
      dc.SelectObject(bitmap);

      CRect rect(0, 0, dx, dy);
      dc.SetBkColor(0xFFFFFF);
      CBrush brush(0xFFFFFF);
      dc.FillRect(rect, &brush);

      PlayEnhMetaFile(dc.m_hDC, hemf, &rect);
      ::savePNG(dc.m_hDC, dx, dy, fn);

      CloseClipboard();
      return true;
    }
  }
  return false;
}

bool IsBufDib()
{   
  bool is_buf = false;

  if (OpenClipboard(0)) {
    HANDLE hClipboardData = GetClipboardData(CF_DIB);

    if (hClipboardData) {
      is_buf = true;
    }

    HENHMETAFILE hemf; 

    hemf = (HENHMETAFILE) GetClipboardData(CF_ENHMETAFILE);

    if (hemf ) {
      is_buf = true;
    }
    CloseClipboard();
  }
  return is_buf;
}



void savePosition(CWnd *wnd, const char *name)
{
  WINDOWPLACEMENT wpi;
  wnd->GetWindowPlacement(&wpi); 
  CString p = name;

  AfxGetApp()->WriteProfileInt(szSection, p+"_bx", wpi.rcNormalPosition.left);
  AfxGetApp()->WriteProfileInt(szSection, p+"_by", wpi.rcNormalPosition.top);
  AfxGetApp()->WriteProfileInt(szSection, p+"_cx", wpi.rcNormalPosition.right-wpi.rcNormalPosition.left);
  AfxGetApp()->WriteProfileInt(szSection, p+"_cy", wpi.rcNormalPosition.bottom-wpi.rcNormalPosition.top);
}

void loadPosition(CWnd *wnd, const char *name)
{
  CString p = name;
  
  int bx = AfxGetApp()->GetProfileInt(szSection, p+"_bx", 0);
  int by = AfxGetApp()->GetProfileInt(szSection, p+"_by", 0);
  int cx = AfxGetApp()->GetProfileInt(szSection, p+"_cx", 0);
  int cy = AfxGetApp()->GetProfileInt(szSection, p+"_cy", 0);

  if (cx > 0 && cy > 0) {
    wnd->SetWindowPos(&wnd->wndTopMost,bx,by,cx,cy,SWP_NOZORDER);
  }
}

static char _tr[10240];


const char *_TR(const char *key)
{
  return key;
  
  strcpy(_tr, key);  
  return transl(_tr);
}



int getNumb(const char *txt)
{
    vector<CString> match;
    int l = regex_match("^[^0-9]*([0-9]+)[^0-9]*$", txt, match);
    if (l >= 1) {
        CString t1 = match[1];
        int m = atoi(t1);
        return m;
    }
    return 0;
}

double getScreenMas()
{
  CClientDC screenDC(NULL);
  double m_h = screenDC.GetDeviceCaps(LOGPIXELSX);

  return m_h/96.;
}




bool RunProcessWithoutWait(const std::string& programPath)
{
    // Настраиваем структуру STARTUPINFO, чтобы скрыть консольное окно
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW; 
    si.wShowWindow = SW_HIDE; // Скрыть окно

    ZeroMemory(&pi, sizeof(pi));

    // Преобразуем строку пути программы в нужный формат
//    char cmd[MAX_PATH];
//    strcpy_s(cmd, programPath.c_str());

    // Запускаем процесс
    if (!CreateProcessA(
            NULL,    // Имя приложения
//            cmd,     // Командная строка
            (LPSTR)programPath.c_str(),     // Командная строка
            
            NULL,    // Защита процесса
            NULL,    // Защита потока
            FALSE,   // Унаследовать дескрипторы
            CREATE_NO_WINDOW, // Не показывать окно
            NULL,    // Использовать текущую переменную среды
            NULL,    // Использовать текущую директорию
            &si,     // Информация о старте
            &pi))    // Информация о процессе
    {
        // Если не удалось создать процесс, выводим ошибку
//        MessageBoxA(NULL, "Failed to create process", "Error", MB_OK);
        return false;
    }

    // Закрываем дескрипторы процесса и потока, чтобы не ждать завершения
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
}


std::string ReadFromPipe(HANDLE pipe)
{
    DWORD bytesRead;
    CHAR buffer[4096];
    BOOL success = FALSE;
    std::string result;

    // Читаем данные из канала и добавляем их в строку
    while (true) {
        success = ReadFile(pipe, buffer, sizeof(buffer) - 1, &bytesRead, nullptr);
        if (!success || bytesRead == 0) break; // Конец чтения
        buffer[bytesRead] = '\0'; // Добавляем нулевой символ для корректного вывода строки
        result += buffer; // Добавляем к результату
    }

    return result;
}



int MyExec(const CString& commandLine, CString &error)
{
    error = "";
    // Создаем анонимные каналы для перенаправления stdout и stderr
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE; // Позволяем наследовать дескрипторы
    saAttr.lpSecurityDescriptor = nullptr;

    HANDLE hStdOutRead, hStdErrRead, hStdOutWrite;
    HANDLE hStdErrWrite;
    if (!CreatePipe(&hStdOutRead, &hStdOutWrite, &saAttr, 0)) {
//        std::cerr << "Не удалось создать канал для stdout." << std::endl;
        error = "Не удалось создать канал для stdout.";
        log1(error);
        return 1;
    }
    if (!CreatePipe(&hStdErrRead, &hStdErrWrite, &saAttr, 0)) {
//        std::cerr << "Не удалось создать канал для stderr." << std::endl;
        error = "Не удалось создать канал для stderr.";
        log1(error);
        return 1;
    }

    // Устанавливаем дескрипторы, чтобы они не наследовались главным процессом
    SetHandleInformation(hStdOutRead, HANDLE_FLAG_INHERIT, 0);

    // Настраиваем STARTUPINFO, чтобы перенаправить stdout и stderr
    STARTUPINFO si = { sizeof(si) };
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = hStdOutWrite;
    si.hStdError = hStdErrWrite;

    PROCESS_INFORMATION pi;
//    LPCWSTR commandLine = L"\"C:\\Program Files\\My App\\myapp.exe\" \"argument with spaces\"";

    // Запускаем процесс
    if (!CreateProcess(nullptr, (LPSTR)(const char *)commandLine, nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
//        std::cerr << "Не удалось запустить процесс." << std::endl;
        error = "Не удалось создать канал для stderr.";
        log1(error);
        return 1;
    }

    // Закрываем ненужные дескрипторы
    CloseHandle(hStdOutWrite);
    CloseHandle(hStdErrWrite);

    // Чтение вывода из канала
    std::string output = ReadFromPipe(hStdOutRead);
    error = output.c_str();

    TCHAR *psBuffer2 = new TCHAR[error.GetLength()+1];
    OemToChar(error, psBuffer2);
    error = psBuffer2;
    delete [] psBuffer2;

    // Ждем завершения процесса
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Получение кода завершения процесса
    DWORD exitCode;
    if (!GetExitCodeProcess(pi.hProcess, &exitCode)) {
//        std::cerr << "Не удалось получить код завершения." << std::endl;
        error = "Не удалось получить код завершения.";
        log1(error);
        return 1;
    }

    // Закрываем дескрипторы
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hStdOutRead);

    // Выводим результат программы и код завершения
//    std::cout << "Вывод программы:\n" << output << std::endl;
//    std::cout << "Код завершения: " << exitCode << std::endl;

    return exitCode;
}

CString python_str(const CString & prog, const CString & args)
{
    CString cmd, pp;

    pp.Format("%s\\%s", argpath(), prog);

    if (!IsFile(pp)) {
        pp.Format("%s\\%sc", argpath(), prog);  // pyc-file

        if (!IsFile(pp)) {
            pp.Format("D:\\Qt\\gid8\\python\\%s", prog);
        }
        if (!IsFile(pp)) {
            pp.Format("D:\\Qt\\gid8\\python\\docs\\%s", prog);
        }
    }

    cmd.Format("\"%s\" -u \"%s\" %s", python_exe(), pp, args);

    return cmd;
}
