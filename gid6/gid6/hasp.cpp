#include "stdafx.h"
#include "gid6.h"
#include "InputDia.h"

#include "hasp.h"

#include "hasp/hasp.h"

int demoversion = 0;
int no_raschet = 0;  /// Не для расчета
int readonly_version = 0;
int no_password = 0;


int is_UPDATE_ONLY_SPECIAL = 0;


int login();
int logout();

int read_word(int n);
int write_word(int n, int x);

void HaspError(int n, char* s);


//#define IDNUMBERKEMERVO   1273476647
//#define IDNUMBERVILNUS    1385597058
//#define IDNUMBERCURRENT    891141865
//#define IDNUMBERKOKCH   830952856
//#define IDNUMBER    1


// Караганда это HASP-4 NET

//#define POLYTECH_ONLY 1
//#define ALMATY_ONLY 1
//#define HASP_HL 1
//#define ONLY_ASTANA    1

// Тараз и Казнипиэнергопром
//#define PITER    1


//#define ONLY_KAVERIN    1


//#define UPDATE_HASP ID_ASTANA
//#define UPDATE_HASP ID_ALMATY_TEP
//#define UPDATE_HASP ID_KHARKIV
//#define UPDATE_HASP ID_KHARKIV_LOCAL
//#define UPDATE_HASP ID_SEMEY
//#define UPDATE_HASP ID_SEMEY_NOTEBOOK
// 
//#define UPDATE_HASP ID_ODESSA
#define UPDATE_HASP ID_ODESSA_NET
//#define UPDATE_HASP ID_LUTSK
//#define UPDATE_HASP ID_SHYMKENT
//#define UPDATE_HASP ID_TARAZ
//#define UPDATE_HASP ID_EKIBASTUZ
//#define UPDATE_HASP ID_KAZNIPIENERGOPROM
//#define UPDATE_HASP ID_POLYTECH
//#define UPDATE_HASP ID_AKTOBE

//#ifdef HASP_HL || PITER

#ifdef ONLY_ASTANA
    #define ASTANAGID "AstanaGID"
#endif 


#if defined(ALMATY_ONLY)
    #define ASTANAGID "AlmatyGID"
#endif

#if defined(POLYTECH_ONLY)
    #define ASTANAGID "KarGTU"
#endif

#if defined(HASP_HL) || defined(PITER)

#if UPDATE_HASP == ID_POLYTECH
//    #define NET      1
//    #define COUNTER  1
//    #define OST 1500
    #define ASTANAGID "KarGTU"

#elif UPDATE_HASP == ID_ALMATY_TEP
    #define NET      1
    #define COUNTER  1
    #define OST 1500
    #define ASTANAGID "AlmatyGID"

#elif UPDATE_HASP == ID_SHYMKENT
    #define NET      1
    #define COUNTER  1
    #define OST 1500
    #define ASTANAGID "ShymkentGID"

#elif UPDATE_HASP == ID_KHARKIV
    #define NET      1
    #define COUNTER  1
    #define OST 150
    #define ASTANAGID "KharkivGID"

#elif UPDATE_HASP == ID_KHARKIV_LOCAL
//    #define NET      1
    #define COUNTER  1
    #define OST 1500
    #define ASTANAGID "KharkivGID_local"

#elif UPDATE_HASP == ID_SEMEY
    #define NET      1
    #define COUNTER  1
    #define OST 1500
    #define ASTANAGID "SemeyGID"

#elif UPDATE_HASP == ID_AKTOBE
    #define NET      1
    #define COUNTER  1
    #define OST 1500
    #define ASTANAGID "AktobeGID"

#elif UPDATE_HASP == ID_SEMEY_NOTEBOOK
//    #define NET      1
    #define COUNTER  1
    #define OST 1500
    #define ASTANAGID "SemeyGID_Notebook"


#elif UPDATE_HASP == ID_ODESSA  
//    #define NET      0
    #define COUNTER  1
    #define OST 1500
    #define ASTANAGID "OdessaGID"

#elif UPDATE_HASP == ID_ODESSA_NET
    #define NET      1
    #define COUNTER  1
    #define OST 1500
    #define ASTANAGID "OdessaGID_net"

#elif UPDATE_HASP == ID_LUTSK
//    #define NET      0
    #define COUNTER  1
    #define OST 1500
    #define ASTANAGID "LutskGID"


#elif UPDATE_HASP == ID_TARAZ
    #define NET      1
    #define COUNTER  1
    #define OST 1500
    #define ASTANAGID "TarazGID"

#elif UPDATE_HASP == ID_KAZNIPIENERGOPROM
    #define NET      1
    #define COUNTER  1
    #define OST 1500
    #define ASTANAGID "KazNIIGID"

#elif UPDATE_HASP == ID_EKIBASTUZ
    #define NET      1
    #define COUNTER  1
    #define OST 1500
    #define ASTANAGID "Ekibastuz"

#else  
    #define COUNTER  1
    #define OST      150
    #define ASTANAGID ""
#endif
#endif

#define _IP "45.132.85.23";
#define _port 1433;

#ifdef ONLY_KAVERIN
#if UPDATE_HASP == ID_ODESSA
    #define ASTANAGID "KievGID_KAVERIN_ONLY"
    #define _bd_gid "KievGID";
    #define _bd_geo "";
    #define _user "Kiev_User";
    #define _password "tgid";
#endif    
#endif


CString getUpdateDir()
{
//#ifdef HASP_HL
#if defined(HASP_HL) || defined(PITER) || defined(ONLY_KAVERIN) || defined(ONLY_ASTANA) || defined(ALMATY_ONLY) || defined(POLYTECH_ONLY)
    return ASTANAGID;
#else
    return "";
#endif
}

void SetDemo2(CString & IP, CString & user, CString & password, int port, CString & bd_gid, CString & bd_geo)
{
#if defined(ONLY_KAVERIN)
      bd_gid = _bd_gid;
      bd_geo = _bd_geo;
      IP = _IP;
      user = _user;
      password = _password;
      port = _port;
#endif
}





#include "hasp_hl/hasp_hl.h"

#define GEMAI "27qqbQX9Z33r+KM0xoyFpbYInaElBgPPfANtmsQ1yrABNllW2ZQ+HJXEMrFJ+MV+EWW74XYxfxhk511FmyKD/0XQNWv75lusPC+JW2WaxYFUMyua+Vr7bSg03oRPMhABYY7KluOye6dhdFBzTZUSoeCYV96vmjpCQt76Kf1gb0Zo7eL09GerAImcoAhlQH7r9vCYdNOFrCX17AzRjOMfc/v3CDy5oVaAfjXk1HFVT4oVPanTAT3wziltKp2deYNJSZzC1ml1fce4qxutszuGI8rmgZ22jZe2M9DxGHH+z5XMsbG/By6gooWJ/58wajFg7GcXY/wBnjv1GLhPyVP1yhFL13ksvl+kOd4VbsGki/8QMsMDFvoY/xSVKk/xTSNWQl4ufJBzqVCSNLeHwwhhjx2ysg2XhYnOLy1ElUmpwd2VeKZGhV5rqxfAT4ZVGCPVtFfVUf5a8g7cnEoXX44Xzzg73S95vy3UWCPkBSOtBjCe7LewaXU9ecUdqEd9bo892qTAOUmrH2UUWUqyk/Neht5eneK7Df+ZJh005CL6DrC/odeszQS02nv84I3bAaWAAjzgQlhlcynluFdeMaACdUA143GEmfDLpwh9tUMRVWEXYF0SI5pO06vNIBj6caSSZU4glKKcAGSQBhve1oQfT9K85QXpkWm3txXuoYEVpRko/eegtmWR92xlR6DrPdSgijNDBnyWZwaTvU8PwPeTVjPwvfEBCw=="


void psw(int k, long l, char *s) {
  long n;
  int i;
  n = l*(k+1) & 0xFFFF;

  for (i = 0; n && i < 8; i++ ) {
    s[i] = 'A' + n%26;
    n = n/26;
  }
  s[i] = 0;
}

void psw(int k, char *s) {
  char ss[8];

  psw(k, 31991L, s);
  psw(k, 22259L, ss); strcat(s, ss);
  psw(k, 27011L, ss); strcat(s, ss);
  s[8] = 0;
}


int hasp_ost = 0;


int get_ost_hasp()
{
    return hasp_ost;
}


CString argpath_system32();
int IsFile(const char* fN);


BOOL Hasp()
{
#if ALMATY_ONLY || POLYTECH_ONLY
    
    is_UPDATE_ONLY_SPECIAL = UPDATE_HASP;

    CString path = argpath_system32();
    
    if (!IsFile(path+"\\wing.sys")) {
        CString str;
        str.Format("Ошибка, обратитесь к поставщику");
        AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
        return FALSE;
    }
#endif


#ifdef ONLY_KAVERIN
  is_UPDATE_ONLY_SPECIAL = UPDATE_HASP;
#endif


#ifdef ONLY_ASTANA
  is_UPDATE_ONLY_SPECIAL = UPDATE_HASP;
#endif 


#ifdef HASP_HL
  long ost;
  hasp_status_t ret;
  hasp_handle_t handle;
//  char buffer[256];
//  hasp_size_t fsize;
  CString name_of_hasp = "";
  CString str;

  unsigned char vendor_code[] = GEMAI;

  if (CString(vendor_code) == GEMAI) {
      name_of_hasp = "GEMAI";
  }
  else {
      str.Format("Ошибка, обратитесь к поставщику");
      AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
      return FALSE;
  }

  is_UPDATE_ONLY_SPECIAL = UPDATE_HASP;


//  ret = hasp_login(HASP_PROGNUM_DEFAULT_FID,(hasp_vendor_code_t) vendor_code, &handle);

  
#ifdef NET
  ret = hasp_login(HASP_PROGNUM_OPT_NO_LOCAL | HASP_PROGNUM_FEATURETYPE | HASP_PROGNUM_OPT_TS,(hasp_vendor_code_t) vendor_code, &handle);
#else
  ret = hasp_login(HASP_PROGNUM_OPT_NO_REMOTE | HASP_PROGNUM_FEATURETYPE,(hasp_vendor_code_t) vendor_code, &handle);
#endif

  if (ret != 0) {
    if (ret == 7) {
#ifdef NET
        str.Format("Для запуска этой программы необходимо, чтобы на одном из компьютеров в вашей сети был установлен ключ защиты HASP с кодом %s и запущен менеджер лицензий HASP", name_of_hasp);
#else
        str.Format("Для запуска этой программы необходимо, чтобы на вашем компьютере был установлен ключ защиты HASP с кодом %s и драйвер HASP", name_of_hasp);
#endif
    }
    else {
        str.Format("Ошибка HASP %d", ret);
    }
    AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
    return FALSE;
  } 

#ifdef STRICT_HASP
  int strict_hasp;

  ret =  hasp_read(handle, HASP_FILEID_MAIN, 0, 4, (void*)&strict_hasp);

  if (strict_hasp != STRICT_HASP) {
    char s[256];
    CString text;

//#if STRICT_HASP != ID_CHELAB_MARINA
#if 1
//    srand( (unsigned)time( NULL ) );

    int n = rand();
    sprintf(s, "Введите пароль для %d", n);

    CInputDialog inD(AfxGetMainWnd( ), _T(""), s, text);

    if (inD.DoModal() != IDOK) return FALSE;

    psw(n, s);

    if (lstrcmpi(s, inD.m_strInput)) {
      MessageBox(GetActiveWindow(), "Неверный пароль", "",  MB_OK);
      return FALSE;
    }

#endif
    strict_hasp = STRICT_HASP;
    ret = hasp_write(handle, HASP_FILEID_MAIN, 0, 4, (void*)&strict_hasp);
  }


#endif


//  ret = hasp_get_size(handle, HASP_FILEID_MAIN, &fsize);
#ifdef COUNTER
//  ret =  hasp_read(handle, HASP_FILEID_MAIN, 0, 4, (void*)&ost);
  ret =  hasp_read(handle, HASP_FILEID_MAIN, 4, 4, (void*)&ost);

  ost--;

  hasp_ost = ost;

  if (ost <= 0 || ost > 60000) {
    char s[80];
    CString text;

//    srand( (unsigned)time( NULL ) /2);

    int n = rand();
    sprintf(s, "Введите пароль для %d", n);

    CInputDialog inD(AfxGetMainWnd( ), _T(""), s, text);

    if (inD.DoModal() != IDOK) return FALSE;

    psw(n, s);

    if (lstrcmpi(s, inD.m_strInput)) {
      MessageBox(GetActiveWindow(), "Неверный пароль", "",  MB_OK);
      return FALSE;
    }
    ost = OST;
  }

  ret =  hasp_write(handle, HASP_FILEID_MAIN, 4, 4, (void*)&ost);

//  CString str;
  str.Format("У вас осталось %d запусков", ost);
//  AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);


  str.Format("HASP %d неправильный", handle);
//  AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);


#ifdef IDNUMBER

  unsigned long iDNumber = GetID();
  if (iDNumber != IDNUMBERKOKCH) {
//  if(iDNumber != IDNUMBERKEMERVO) {
//  if(iDNumber != IDNUMBERVILNUS) {
//  if(iDNumber != 891141865) {
    CString str;
    str.Format("HASP %d неправильный")
    MessageBox(GetActiveWindow(), "HASP key ID number is wrong", "",  MB_OK);
    return FALSE;
  }

#endif


#endif

  ret =  hasp_logout(handle);

  if (ret != 0) {
    CString str;
    str.Format("Ошибка выхода из HASP %d", ret);
    AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
    return FALSE;
  }


  return TRUE;

#endif


  
#ifdef PITER
/*
  Этот фрагмент предназначен для защиты NetHASP
  для Питера (Хачатуров) нужно определить  переменную PITER

*/

  is_UPDATE_ONLY_SPECIAL = UPDATE_HASP;


#ifdef NET

  int n;
  char s[256];

  if ((n = login()) != 0) {
    HaspError(n, s);
    MessageBox(GetActiveWindow(), s, "",  MB_OK);
    return FALSE;
  }
#else
  login();
#endif

  int x;

  if ((x = read_word(1)) == -1) {
    AfxMessageBox("Ошибка с HASP", MB_OK|MB_ICONINFORMATION);
    return FALSE;
  }

#ifdef IDNUMBER

  unsigned long iDNumber = GetID();
  if (iDNumber != IDNUMBERKOKCH) {
//  if(iDNumber != IDNUMBERKEMERVO) {
//  if(iDNumber != IDNUMBERVILNUS) {
//  if(iDNumber != 891141865) {
    CString str;
    str.Format("HASP %d неправильный")
    MessageBox(GetActiveWindow(), "HASP key ID number is wrong", "",  MB_OK);
    return FALSE;
  }

#endif
//unsigned long GetID();
//sprintf(s, "%lx %lu", GetID(), GetID());
//MessageBox(GetActiveWindow(), s, "",  MB_OK);


#ifdef COUNTER

//  sprintf(s, "Осталось %d запусков", x);
//  MessageBox(GetActiveWindow(), s, "",  MB_OK);

  if ( x != 30000 ) {

    x --;

    if( x <= 1 || x > 60000) {
      x = OST;

      char s[80];
      CString text;

//      srand( (unsigned)time( NULL ) );

      int n = rand();
      sprintf(s, "Введите пароль для %d", n);

      CInputDialog inD(AfxGetMainWnd( ), _T(""), s, text);

      if (inD.DoModal() != IDOK) return FALSE;

      psw(n, s);

      if (lstrcmpi(s, inD.m_strInput)) {
        MessageBox(GetActiveWindow(), "Неверный пароль", "",  MB_OK);
        return FALSE;
      }
    }


    if (write_word(1, x) == -1) return FALSE;
  }
#endif


#endif

  return TRUE;
}
