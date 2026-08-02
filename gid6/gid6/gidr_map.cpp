#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
//#include "DbTable.h"
#include "InputDia.h"
#include "Input2.h"
#include "gidrView.h"
#include "dialog_pr.h"
#include "win.h"
#include "mmenu.h"

#include "wm_user.h"

#include <json.hpp>

#include "PropertyGrid\test\PropGridDlg.h"

extern CString m_pathMap;


int getMapYes();
void setMapYes(int mapYes);
CString getPathMap();

static int m_isInternet = 0;



nlohmann::json get_j(nlohmann::json& j, const string& key);
double get_j_float(nlohmann::json& j, const string& key);
string get_j_string(nlohmann::json& j, const string& key);



string methodTOHex(string s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring temp(buf);
    delete[] buf;
    LPCWSTR input = temp.c_str();

    string output = "";

    int cbNeeded = WideCharToMultiByte(CP_UTF8, 0, input, -1, NULL, 0, NULL, NULL);
    if (cbNeeded > 0) {
        char* utf8 = new char[cbNeeded];
        if (WideCharToMultiByte(CP_UTF8, 0, input, -1, utf8, cbNeeded, NULL, NULL) != 0) {
            for (char* p = utf8; *p; *p++) {
                char onehex[5];
                _snprintf(onehex, sizeof(onehex), "%%%02.2X", (unsigned char)*p);
                output += onehex;
            }
        }
        delete[] utf8;
    }

    return output;
}

CString toUTF8(const CString& qq);
char* utf8_to_a2(const char* buf1, bool kaz);


void CGidrView::OnMapSearch()
{
    CPropGridDlg* pm_cEditDlg = dialog_pr(this, "map_search", WM_USER_MAP_SEARCH, _TR("Поиск"), -1);

    if (pm_cEditDlg) {
        CString city = AfxGetApp()->GetProfileString(szSection, "map_city", "");
        pm_cEditDlg->m_ctrlGrid.SetNameValue("CITY", city);
        CString addr = AfxGetApp()->GetProfileString(szSection, "map_addr", "");
        pm_cEditDlg->m_ctrlGrid.SetNameValue("ADDRESS", addr);
    }

}
#if 0
BOOL SetConnectionOptions()
{
    INTERNET_PER_CONN_OPTION_LIST list;
    BOOL    bReturn;
    DWORD   dwBufSize = sizeof(list);

    // Fill the list structure.
    list.dwSize = sizeof(list);

    // NULL == LAN, otherwise connectoid name.
    list.pszConnection = NULL;

    // Set three options.
    list.dwOptionCount = 3;
    list.pOptions = new INTERNET_PER_CONN_OPTION[3];

    // Ensure that the memory was allocated.
    if(NULL == list.pOptions)
    {
        // Return FALSE if the memory wasn't allocated.
        return FALSE;
    }

    // Set flags.
    list.pOptions[0].dwOption = INTERNET_PER_CONN_FLAGS;
    list.pOptions[0].Value.dwValue = PROXY_TYPE_DIRECT |
        PROXY_TYPE_PROXY;

    // Set proxy name.
    list.pOptions[1].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
    list.pOptions[1].Value.pszValue = TEXT("https://proxy:80");

    // Set proxy override.
    list.pOptions[2].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
    list.pOptions[2].Value.pszValue = TEXT("local");

    // Set the options on the connection.
    bReturn = InternetSetOption(NULL,
        INTERNET_OPTION_PER_CONNECTION_OPTION, &list, dwBufSize);

    // Free the allocated memory.
    delete [] list.pOptions;

    return bReturn;
}


BOOL setProxy(LPWSTR proxy){
 
    INTERNET_PER_CONN_OPTION_LIST    List;
    INTERNET_PER_CONN_OPTION         Option[2];
    unsigned long                    nSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);
 
    Option[0].dwOption       = INTERNET_PER_CONN_AUTOCONFIG_URL;
    Option[0].Value.pszValue = proxy;
 
    Option[1].Value.dwValue  |= PROXY_TYPE_DIRECT; 
    Option[1].dwOption       = INTERNET_PER_CONN_FLAGS;
    Option[1].Value.dwValue  = PROXY_TYPE_AUTO_PROXY_URL;
    Option[1].Value.pszValue = proxy;
 
    List.dwSize              = sizeof(INTERNET_PER_CONN_OPTION_LIST);
    List.pszConnection       = NULL;
    List.dwOptionCount       = 2;
    List.dwOptionError       = 0;
    List.pOptions            = Option;
 
    if(!InternetSetOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &List, nSize)){
        printf("InternetQueryOption failed! (%d)\n", GetLastError());
        return FALSE;
    }
    
    InternetSetOption(0, INTERNET_OPTION_REFRESH, NULL, NULL);
 
    return TRUE;
}
#endif
#if 0 
#include <Wininet.h>
#include <Strsafe.h>

void setProxy()
{
    HINTERNET hConnect = NULL;
    HINTERNET hOpen = NULL;
    STRSAFE_PCNZCH strUsername = L"qq";
    STRSAFE_PCNZCH strPassword = L"11";


// strUsername is a string buffer of cchMax characters or less.
// It contains the proxy user name.
size_t cchMax = 80;
size_t cchUserLength, cchPasswordLength;
HRESULT hr = StringCchLength(strUsername, cchMax, &cchUserLength);

if (SUCCEEDED(hr))
{
   // hOpen is the HINTERNET handle created by InternetConnect.
   InternetSetOption(hConnect, INTERNET_OPTION_PROXY_USERNAME,
      strUsername, DWORD(cchUserLength)+1);
}
else
{
   // Insert error handling code here.
}

// strPassword is the string buffer that contains the proxy password.
hr = StringCchLength(strPassword, cchMax, &cchPasswordLength);

InternetSetOption(hOpen, INTERNET_OPTION_PROXY_PASSWORD,
    strPassword, DWORD(cchPasswordLength)+1);
}
#endif

LRESULT CGidrView::OnMapSearch2(WPARAM wParam, LPARAM)
{
    //    CInputDialog inD(this, _TR("Поиск"), _TR("Введите текст"), text);

    //    if (inD.DoModal() == IDOK) {
    //        text = inD.m_strInput;
    //        text.Replace(" ", "+");

    CString tmpn, url;
    //    CString city = getPropValue(WM_USER_MAP_SEARCH, "CITY");
    //    CString addr = getPropValue(WM_USER_MAP_SEARCH, "ADDRESS");

    CString city = "", addr = "";


    if (getPropValue(WM_USER_MAP_SEARCH, "CITY", city)) {
        AfxGetApp()->WriteProfileString(szSection, "map_city", city);
    }

    if (getPropValue(WM_USER_MAP_SEARCH, "ADDRESS", addr)) {
        AfxGetApp()->WriteProfileString(szSection, "map_addr", addr);
    }
    
    CString text = city + " " + addr;


//  text.Replace(" ", "&");

    text = methodTOHex((string(text))).c_str();

    url.Format("http://lan.avto-glass.kz/?action=osm&q=%s", text);
    url.Format("http://nominatim.openstreetmap.org/search?q=%s&format=geojson&polygon=1&addressdetails=1", text);

    tmpn.Format("%s\\elevation.tmp", getenv("TMP"));

//    InternetSetOption(hConnect, INTERNET_OPTION_PROXY_USERNAME,
//      strUsername, DWORD(cchUserLength)+1);



//    InternetSetOption(hConnect, INTERNET_OPTION_PROXY_USERNAME,
//      strUsername, DWORD(cchUserLength)+1);

    int ret = URLDownloadToFile(0, url, tmpn, 0, 0);

    if (ret == S_OK) {
        if (IsFile(tmpn)) {
            CString qq = readFile(tmpn);
            try {
                nlohmann::json j0;

                j0 = nlohmann::json::parse((const char*)qq);

                nlohmann::json j1 = get_j(j0, "features");

                int n = j1.size();

                CMMenu menu(this, IDD_MENU2_W);

                int ii = 0;

                if (n != 1) {

                    for (int i = 0; i < n; i++) {
                        string j2 = j1[i]["properties"]["display_name"];

                        char* a = utf8_to_a2(j2.c_str(), true);
                        menu.Add(a, (void*)i);
                        CoTaskMemFree(a);
                    }

                    if (menu.DoModal() != IDOK) return 0L;

                    ii = (int)menu.getV();
                }

                nlohmann::json jj = j1[ii]["bbox"];

                double x1 = jj[0];
                double y1 = jj[1];
                double x2 = jj[2];
                double y2 = jj[3];

                CFPoint get_google_to_coord(double x, double y);

                CFPoint pt1 = get_google_to_coord(x1, y1);
                CFPoint pt2 = get_google_to_coord(x2, y2);
                
                MoveRect(CFRect(pt1, pt2));

                return 0L;
            }
            catch (...) {
            }
        }
    }
    AfxMessageBox(_TR("Не нашел"), MB_OK|MB_ICONINFORMATION);

    return 0L;
}

void CGidrView::OnMapYes()
{
    setMapYes(!getMapYes());
    AfxGetApp()->WriteProfileInt(szSection, "isMapYes", getMapYes());

    Invalidate();
}

void CGidrView::OnUpdateMapYes(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(getMapYes());
}

void CGidrView::OnMap(UINT nID)
{
    if (nID != ID_NOMAP && !m_isInternet) {
        if (MessageBox(_TR("Использовать подключение к сети интернет для получения карт?"), "", MB_YESNO) == IDYES) {
            m_isInternet = 1;
        }
    }


    bool get_WMS();
    //      CFPoint get_WMS_XY();

    if (nID == ID_WMS_MAP) {

        CWaitCursor wait;

        if (!get_WMS()) {
            AfxMessageBox("Не могу подключиться к WMS-серверу!");
            return;
        }
        //          CFPoint pt = get_WMS_XY();

        //          if (AfxMessageBox(_TR("Перейти к карте?"), MB_YESNO) == IDYES) {
        //              moveXY(pt);
        //          }
    }

    m_map = nID;

    AfxGetApp()->WriteProfileInt(szSection, "GoogleMap", m_map);

    is_google_map = GOOGLE_ALWAYS || (nID != ID_NOMAP);
    m_cxema->is_google_map = is_google_map;

    InvalidateG();
}

//-----------------------------------------------

void CGidrView::OnUpdateMap(CCmdUI* pCmdUI)
{

    pCmdUI->SetCheck(m_map == pCmdUI->m_nID);
}

void CGidrView::OnRereadMap()
{
    m_reread = TRUE;
    InvalidateG();
}


void CGidrView::OnMapPath()
{
    char path1[256];
    CString path;

    //  HRESULT ret = SHGetSpecialFolderPath(NULL,path1,CSIDL_APPDATA, 1);

    path = path1;

    //    if (GetDirectory(m_pathMap)) {
    if (getDir(this, m_pathMap)) {
        AfxGetApp()->WriteProfileString(szSection, "path_map", m_pathMap);
    }
}

void CGidrView::OnUpdateGoogleMap(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_map != ID_NOMAP && m_map != ID_YANDEX_MAP && m_map != ID_YANDEX_PMAP && m_map != ID_YANDEX_SAT);
}

void get_google_coord(CFPoint pt, double& x, double& y);

fpos_t readFile(const char* fn, char** pBuf, char* szDT);

#include "jsmn.h"

void OsmInfo(CFPoint pt)
{
    CString str, url, tmpn;
    double x, y;
    tmpn.Format("%s\\elevation.tmp", getenv("TMP"));
    get_google_coord(pt, x, y);

    url.Format("https://nominatim.openstreetmap.org/search?q=%f,%f&format=geojson&polygon=1&addressdetails=1", y, x);

    int ret = URLDownloadToFile(0, url, tmpn, 0, 0);

    if (ret == S_OK) {
        if (IsFile(tmpn)) {
            CString buf = readFile(tmpn);
            DeleteFile(tmpn);

            nlohmann::json j0;

           j0 = nlohmann::json::parse((const char*)buf);

           nlohmann::json j1 = get_j(j0, "features");

           CString str = "";

           int n = j1.size();
           for (int i = 0; i < n; i++) {
               string j2 = j1[i]["properties"]["display_name"];

               char* a = utf8_to_a2(j2.c_str(), true);

               if (str != "") str += "\n";

               str += a;

               CoTaskMemFree(a);
           }

           if (str == "") str = "Нет информации";
           AfxMessageBox(str);
        }
    }
}


CString Elevation(CFPoint pt)
{
    CString elev = "Результат не получен";
    double x, y;
    get_google_coord(pt, x, y);
    CString str, url, tmpn;
    str.Format("%f %f", x, y);
    url.Format("http://maps.googleapis.com/maps/api/elevation/json?locations=%f,%f", y, x);
    url.Format("http://api.geonames.org/srtm3JSON?lat=%f&lng=%f&username=demo", y, x);
    url.Format("https://elevation-api-test.azurewebsites.net/api/elevation/%f,%f", y, x);

    url.Format("http://open.mapquestapi.com/elevation/v1/profile?key=MNtuOSulyjpOWnWvl4sAWCuBYexHUUzG&shapeFormat=raw&latLngCollection=%f,%f", y, x);

    // Qwrewr23434+123
    // QRKYGcAiMOHMZh1U9wLysa90asNmOGdc

    CString key = "QRKYGcAiMOHMZh1U9wLysa90asNmOGdc";
    url.Format("http://open.mapquestapi.com/elevation/v1/profile?key=%s&shapeFormat=raw&latLngCollection=%f,%f", key, y, x);

    url.Format("https://api.open-elevation.com/api/v1/lookup?locations=%f,%f", y, x);
    url.Format("http://45.132.85.23:8084/api/v1/lookup?locations=%f,%f", y, x);

    CString site = "https://api.open-elevation.com";
    site = AfxGetApp()->GetProfileString(szSection, "elevation_url", site);

    url.Format("%s/api/v1/lookup?locations=%f,%f", site, y, x);

    tmpn.Format("%s\\elevation.tmp", getenv("TMP"));


    int ret = URLDownloadToFile(0, url, tmpn, 0, 0);

    if (ret == S_OK) {
        if (IsFile(tmpn)) {

            char szDT[256];
            char* pBuf;

            fpos_t l = readFile(tmpn, &pBuf, szDT);
            DeleteFile(tmpn);

            int numtok = 200;
            jsmntok_t* t = (jsmntok_t*)malloc(numtok * sizeof(jsmntok_t));

            jsmn_parser p;
            jsmn_init(&p);
            int r = jsmn_parse(&p, pBuf, l, t, numtok);

            CString s2 = "";

            for (int i = 0; i < r; i++) {
                if (t[i].type == JSMN_STRING) {
                    CString s1(&pBuf[t[i].start], t[i].end - t[i].start);
                    //                            if (s1 == "elevation") {
                    if (s1 == "height") {
                        s2 = CString(&pBuf[t[i + 1].start], t[i + 1].end - t[i + 1].start);
                    }
                    if (s1 == "elevation") {
                        s2 = CString(&pBuf[t[i + 1].start], t[i + 1].end - t[i + 1].start);
                    }
                }
            }

            delete[] pBuf;
            free(t);

            return s2;
        }
    }
    else {
        elev.Format("Результат не получен, ошибка %d", ret);
    }

    return elev;
}

