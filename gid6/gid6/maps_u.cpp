// mapsView.cpp : implementation of the CMapsView class
//

#include "stdafx.h"
#include "gid6.h"
#include "gidrview.h"

#include "InputDia.h"

#include "win.h"
#include <math.h>
#include <Urlmon.h>
#include <Wininet.h>
#include <intshcut.h>
#include <process.h>    /* _beginthread, _endthread */
#include "MMenu.h"

#include "maps.h"

#include "geodez.h"

#include "wm_user.h"

#include <afxwin.h>      // MFC core and standard components
#include <afxinet.h>     // MFC Internet support

int utf8_to_a(const char* buf1, char* a);

bool DownloadFile(CString strURL, CString strFilePath)
{
    CInternetSession session;
    CStdioFile* pFile = nullptr;
    CHttpFile* pHttpFile = nullptr;
    bool bResult = false;

    try
    {
        pHttpFile = (CHttpFile*)session.OpenURL(strURL);
        if (pHttpFile)
        {
            pFile = new CStdioFile(strFilePath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
            char buffer[1024];
            UINT nRead = 0;
            while ((nRead = pHttpFile->Read(buffer, sizeof(buffer))) > 0)
            {
                pFile->Write(buffer, nRead);
            }
            bResult = true;
        }
    }
    catch (CInternetException* pEx)
    {
        pEx->Delete();
        bResult = false;
    }

    if (pHttpFile) pHttpFile->Close();
    if (pFile) pFile->Close();

    delete pHttpFile;
    delete pFile;

    session.Close();

    return bResult;
}



// http://localhost:8080/geoserver/cite/wms

CString rovno = "http://62.80.170.250:6080/arcgis/services/Rovno/Rovno/MapServer/WmsServer";
CString kiev = "http://62.80.170.250:6080/arcgis/services/Kiev/Borispol/MapServer/WmsServer";

//#define KHARKIV 1

//static double m_False_Easting = 0;
//static double m_Central_Meridian = 0;

static SystemCoord m_system_coord;


#define COEF 10000000

const double R = 6378137.0; // Радиус Земли для Web Mercator
const double geocoef = (R * M_PI / 180.0)/100000.;

#include "json.h"

void write_system_coord(JSON& json)
{
    json.add("False_Easting", m_system_coord.False_Easting);
    json.add("False_Northing", m_system_coord.False_Northing);
    json.add("Central_Meridian", m_system_coord.Central_Meridian);
    json.add("Latitude_Of_Origin", m_system_coord.Latitude_Of_Origin);
    json.add("Scale_Factor", m_system_coord.Scale_Factor);
    json.add("Angle", m_system_coord.Angle);
}

double x2lon_(double x);
double y2lat_(double y);


void get_google_coord(CFPoint pt, double& x, double& y)
{
    if (m_system_coord.isMercator()) {
        y = y2lat(-pt.y / COEF);
        x = x2lon(pt.x / COEF);
        return;
    }

    double X1 = -pt.y / 100.;
    double Y1 = pt.x / 100.;

    double xx1, yy1;
    m2m(m_system_coord, X1, Y1, xx1, yy1, true);

    x = x2lon_(yy1 / 100000.);
    y = -y2lat_(-xx1 / 100000.);
}

void m2d3(SystemCoord& system_coord, double x, double y, double& B, double& L);


void get_google_coord2(CFPoint pt, double& x, double& y)
{
    double X1 = -pt.y / 100.;
    double Y1 = pt.x / 100.;

    double xxx, yyy;

//    X1 = 5501629.7994;
//    Y1 = 345802.5080;


//5500445.07134272,562179.05405901

    m2d3(m_system_coord, X1, Y1, xxx, yyy);

    y = SK42_WGS84_Lat(xxx, yyy, 0);
    x = SK42_WGS84_Long(xxx, yyy, 0);
    y = xxx;
    x = yyy;

}

double lon2x_(double x);
double lat2y_(double y);


CFPoint get_google_to_coord(double x, double y)
{
    CFPoint pt;

    if (m_system_coord.isMercator()) {
        pt.y = -lat2y(y) * COEF;
        pt.x = lon2x(x) * COEF;

        return pt;
    }

    double yy1, xx1, X1, Y1;

    yy1 = lon2x_(x) * 100000;
    xx1 = -lat2y_(-y) * 100000;

    m2m2(m_system_coord, xx1, yy1, X1, Y1, true);

    pt.x = Y1 * 100;
    pt.y = -X1 * 100;

    return pt;
}

// Для html

CFPoint get_m_to_coord2(double x, double y)
{
    CFPoint pt;
    double yy1, xx1, X1, Y1;

    yy1 = x * 100000;
    xx1 = y * 100000;

    m2m2(m_system_coord, xx1, yy1, X1, Y1, true);

    pt.x = Y1 * 100;
    pt.y = -X1 * 100;

    return pt;
}

CString m_layer;
static CString m_wms = "";

void set_Central_Meridian(double _Central_Meridian)
{
    m_system_coord.Central_Meridian = _Central_Meridian;
}

void set_False_Easting(double _False_Easting)
{
    m_system_coord.False_Easting = _False_Easting;
}



void set_system_coord(double _False_Easting, double _False_Northing, double _Central_Meridian, double _Latitude_Of_Origin, double _Scale_Factor, double _Angle, double _dx, double _dy)
{
//    _Central_Meridian = 224.2/3.;

    if (_Central_Meridian == 0) {
        m_system_coord.False_Easting = 0;
        m_system_coord.False_Northing = 0;
        m_system_coord.Central_Meridian = 0;
        m_system_coord.Latitude_Of_Origin = 0;
        m_system_coord.Scale_Factor = 1;
        m_system_coord.Angle = 0;
        m_system_coord.dx = 0;
        m_system_coord.dy = 0;
    }
    else {
        m_system_coord.False_Easting = _False_Easting;
        m_system_coord.False_Northing = _False_Northing;
        m_system_coord.Central_Meridian = _Central_Meridian;
        m_system_coord.Latitude_Of_Origin = _Latitude_Of_Origin;
        m_system_coord.Scale_Factor = _Scale_Factor;
        m_system_coord.Angle = _Angle;
        m_system_coord.dx = _dx;
        m_system_coord.dy = _dy;
    }
/*
    m_system_coord.False_Easting = 7096.2717;
    m_system_coord.False_Northing = 8623.8408;
    m_system_coord.Central_Meridian = 77.0407772555556;
    m_system_coord.Latitude_Of_Origin = 43.3363979194444;
    m_system_coord.Scale_Factor = 1;
    m_system_coord.Angle = 0;
*/
}


void set_system_coord(SystemCoord sc)
{
    m_system_coord = sc;
}



CString getWMS_File_Name(CString r)
{
    r.Replace("/arcgis", "");
    r.Replace("/services", "");
    r.Replace("/MapServer", "");
    r.Replace("/WmsServer", "");
    r.Replace("http://", "");
    r.Replace("https://", "");
    r.Replace(":", "_");
    r.Replace("/", "_");
    r.Replace("\\", "_");
    r.Replace(".", "_");

    return r;
}

void setWMS(CString _rovno, CString _layer)
{
    rovno = _rovno;
    m_layer = _layer;
    m_wms = getWMS_File_Name(rovno);
}

static double wx1 = 0, wx2 = 0, wy1 = 0, wy2 = 0;
extern CString m_pathMap;


static int _width = 256;
static double MIN_LEN = 196.;
//static double MIN_LEN = 128.;

//#define  MIN_LEN 196.


double WGS84_SK42_Lat(double Bd, double Ld, double H);
double WGS84_SK42_Long(double Bd, double Ld, double H);
double SK42_WGS84_Lat(double Bd, double Ld, double H);
double SK42_WGS84_Long(double Bd, double Ld, double H);

//111319.49079327358

#if 0
void ispr_point(CFPoint point, CFPoint point2)
{
//111319.49079327358

    if (m_system_coord.isMercator()) {
        point2 = point;
        return;
    }

    double Y1 = point.x * COEF / 100;
    double X1 = -point.y * COEF / 100;

    double xxx1, yyy1;

    m2m(m_system_coord, X1, Y1, xxx1, yyy1, true);

    point2.x = yyy1 / 100000;
    point2.x = -xxx1 / 100000;
}

#endif

std::pair<double, double> mercatorToWgs84(double x_merc, double y_merc);
std::pair<double, double> wgs84ToMercator(double lat, double lon);


void ispr_rectf(CFRect rectf, CFRect& rectf2)
{
    if (m_system_coord.isMercator()) {
        rectf.mult(1./geocoef);
        rectf2 = rectf;
        return;
    }

    double Y1 = rectf.left * COEF / 100;
    double X2 = -rectf.top * COEF / 100;

    double Y2 = rectf.right * COEF / 100;
    double X1 = -rectf.bottom * COEF / 100;

    double xxx1, xxx2, xxx3, xxx4, yyy1, yyy2, yyy3, yyy4;

    m2m(m_system_coord, X1, Y1, xxx1, yyy1, true);
    m2m(m_system_coord, X1, Y2, xxx2, yyy2, true);
    m2m(m_system_coord, X2, Y2, xxx3, yyy3, true);
    m2m(m_system_coord, X2, Y1, xxx4, yyy4, true);

    rectf2.left = yyy1 / 100000;
    rectf2.right = yyy2 / 100000;

    rectf2.top = -xxx3 / 100000;
    rectf2.bottom = -xxx1 / 100000;
}

void ispr_rectf2(CFRect rectf, CFRect& rectf2)
{
    if (m_system_coord.isMercator()) {
        rectf.mult(geocoef);
        rectf2 = rectf;
        return;
    }

    double Y1 = rectf.left * 100000;
    double Y2 = rectf.right * 100000;
    double X2 = -rectf.top * 100000;
    double X1 = -rectf.bottom * 100000;

    double xxx1, xxx2, xxx3, xxx4, yyy1, yyy2, yyy3, yyy4;

    m2m2(m_system_coord, X1, Y1, xxx1, yyy1, true);
    m2m2(m_system_coord, X1, Y2, xxx2, yyy2, true);
    m2m2(m_system_coord, X2, Y2, xxx3, yyy3, true);
    m2m2(m_system_coord, X2, Y1, xxx4, yyy4, true);

    rectf2.left = yyy1 / 100000;
    rectf2.right = yyy2 / 100000;

    rectf2.top = -xxx4 / 100000;
    rectf2.bottom = -xxx1 / 100000;
}


/*
CFPoint GeoToFPoint(double B, double L)
{
    if (m_system_coord.isMercator()) {
        double y = -lat2y(B) * 10000000.;
        double x = L * 10000000.;
        CFPoint pt(x, y);
        return pt;
    }
    else {
        double x, y;

        d2m3(m_system_coord, B, L, x, y);
        CFPoint pt(y * 100, -x * 100);
        return pt;
    }
}

*/


CFPoint get_WMS_XY()
{
    return CFPoint(lon2x((wx1 + wx2)) / 2 * 10000000., -lat2y((wy1 + wy2) / 2) * 10000000.);
}


bool isJpeg(int typ)
{
    //  return typ == ID_YANDEX_SAT || typ == ID_GOOGLE_SAT;
    return typ == ID_YANDEX_SAT || typ == ID_GOOGLE_SAT || typ == ID_GOOGLE_HYBRID;
    //    return typ == ID_YANDEX_SAT || typ == ID_GOOGLE_SAT || typ == ID_GOOGLE_HYBRID || typ == ID_WMS_MAP;
}

bool isMap(int typ_map) {
    if (typ_map == ID_GOOGLE_MAP) return true;
    if (typ_map == ID_GOOGLE_SAT) return true;
    if (typ_map == ID_GOOGLE_HYBRID) return true;
    if (typ_map == ID_YANDEX_MAP) return true;
    if (typ_map == ID_2GIS_MAP) return true;
    if (typ_map == ID_YANDEX_PMAP) return true;
    if (typ_map == ID_YANDEX_SAT) return true;
    if (typ_map == ID_OPENSTREETMAP) return true;
    if (typ_map == ID_VISICOM_MAP) return true;
    if (typ_map == ID_WMS_MAP) return true;


    if (typ_map == ID_MAP_SEMEY) return true;
    if (typ_map == ID_MAP_SEMEY_TS) return true;


    return false;
}


void log(const char* podp, const char* s);


//static TCHAR BASED_CODE szSection[] = _T("Settings");



static int map_typ = ID_NOMAP;
static double map_mas = 1;
static CSemaphore c_s;
static int reread = FALSE;
static double dmas = 0;

//extern CMapsThread* m_pMapsThread = 0;



unsigned int nazv(double x, int n)
{
    unsigned int xx;
    x = (x + 180.) / 180;

    int j = 0;

    xx = 0;

    for (int i = 0; i < n - 1; i++) {
        xx *= 2;
        xx += (x > 1);
        x *= 2; if (x > 2.) x -= 2.;
    }
    return xx;
}

double coord(unsigned int xx, int n)
{
    return -180. + (360. * xx) / pow(2., n);
    /*
      unsigned int x = 0x80000 >> (20-n);
      double ff = -180, dd = 180;

      for (; x; x >>= 1) {
        if ( xx&x ) ff += dd;

        dd /= 2.;
      }
      return ff;
    */
}


void conv(unsigned int xx, unsigned int yy, char* s, int n)
{
    unsigned int x = 0x80000 >> (4 + 17 - n);
    char c;

    s[0] = 't';

    int j = 0;

    for (j = 1; x; x >>= 1) {
        if ((xx & x) == 0 && (yy & x)) c = 'q';
        else if ((xx & x) && (yy & x)) c = 'r';
        else if ((xx & x) == 0 && (yy & x) == 0) c = 't';
        else if ((xx & x) && (yy & x) == 0) c = 's';

        s[j++] = c;
    }
    s[j] = 0;
}

CString get_url(int nn, int xx, int yy)
{
    CString str;
    char abcd[] = "abc";
    static int ii = 0;

    long num = 156000000;

    num = 177290279;
    num = 0;

    switch (map_typ) {
    case ID_GOOGLE_MAP:
        str.Format("http://mt%d.google.com/vt/lyrs=m@%ld&hl=ru&x=%d&y=%d&z=%d&s=G", ii, num, xx, yy, nn);
        break;
    case ID_GOOGLE_SAT:
        str.Format("http://mt%d.google.com/vt/lyrs=s@%ld&hl=ru&x=%d&y=%d&z=%d&s=G", ii, num, xx, yy, nn);
        break;
    case ID_GOOGLE_HYBRID:
        str.Format("http://mt%d.google.com/vt/lyrs=y@%ld&hl=ru&x=%d&y=%d&z=%d&s=G", ii, num, xx, yy, nn);
        break;

    case ID_VISICOM_MAP:
    {
        int yy2 = (1 << nn) - 1;
        str.Format("http://tms%d.visicom.ua/2.0.0/planet3/base/%d/%d/%d.png", ii + 1, nn, xx, yy2 - yy);
    }
    break;

    case ID_MAP_SEMEY:
        str.Format("http://vkomap.kz/Proxy/Semey/Semey/MapServer/tile/%d/%d/%d.png", nn, yy, xx);
        break;

    case ID_MAP_SEMEY_TS:
        str.Format("http://vkomap.kz/Proxy/Semey/Heat/MapServer/tile/%d/%d/%d.png", nn, yy, xx);
        break;

    case ID_WMS_MAP:
    {
        //        int yy2 = (1 << nn)-1;
        //        int yy2 = (1 << nn);

        double x1, y1, x2, y2;

        //        x1 = 30.90,  x2 = x1 + 0.2;
        //        y1 = 50.33;  y2 = y1 + 0.2;

        // 5844993.78 3095123.03

        double n2 = pow(2., nn);


        x1 = (xx * 360. / n2) - 180;
        y1 = ((yy + 1) * 360. / n2) - 180;

        y1 = -y1;

        double dx = 360. / n2;

        x2 = x1 + dx;
        y2 = y1 + dx;


        x1 = x2lon(y1);
        x2 = x2lon(y2);

        y1 = y2lat(y1);
        y2 = y2lat(y2);

/*

        double xx1 = WGS84_SK42_Lat(x1, y1, 0);
        double yy1 = WGS84_SK42_Long(x1, y1, 0);

        double xx2 = WGS84_SK42_Lat(x2, y2, 0);
        double yy2 = WGS84_SK42_Long(x2, y2, 0);
        
        x1 = xx1;
        x2 = xx2;
        y1 = yy1;
        y2 = yy2;

*/


        //        m2d(xx2*1e6, yy2*1e6, x2, y2);

        //        CFPoint p = mercatorToGeo(CFPoint(xx1, yy1));

        if (x2 < wx1 || x1 > wx2 || y2 < wy1 || y1 > wy2) {
            str = "";
        }
        else {
            //            str.Format("%s?request=GetMap&service=WMS&version=1.3.0&layers=1&styles=default&crs=EPSG:4326&bbox=%lf,%lf,%lf,%lf&width=%d&height=%d&format=image/%s", rovno, y1, x1, y2, x2, _width, _width, isJpeg(map_typ) ? "jpeg" : "png");
//            str.Format("%s?request=GetMap&service=WMS&version=1.3.0&layers=%s&crs=EPSG:4326&bbox=%lf,%lf,%lf,%lf&width=%d&height=%d&format=image/%s", rovno, m_layer, y1, x1, y2, x2, _width, _width, isJpeg(map_typ) ? "jpeg" : "png");
            str.Format("%s?request=GetMap&service=WMS&version=1.3.0&layers=%s&crs=EPSG:4326&bbox=%.10lf,%.10lf,%.10lf,%.10lf&width=%d&height=%d&format=image/%s&styles", rovno, m_layer, y1, x1, y2, x2, _width, _width, isJpeg(map_typ) ? "jpeg" : "png");

//            str.Format("%s?request=GetMap&service=WMS&version=1.3.0&layers=%s&crs=EPSG:3857&bbox=%lf,%lf,%lf,%lf&width=%d&height=%d&format=image/%s", rovno, m_layer, y1, x1, y2, x2, _width, _width, isJpeg(map_typ) ? "jpeg" : "png");


//            x1 = 
//            x2 = 
//            y1 = 
//            y2 = 

//            str.Format("%s?request=GetMap&service=WMS&version=1.3.0&layers=%s&crs=EPSG:900913&bbox=%.10lf,%.10lf,%.10lf,%.10lf&width=%d&height=%d&format=image/%s&styles", rovno, m_layer, y1, x1, y2, x2, _width, _width, isJpeg(map_typ) ? "jpeg" : "png");




        }

        //        x1 = (xx*360./n2)-180;
        //        y1 = (yy*360./n2)-180;

        //void m2d(double x, double y, double &B, double &L);

        //        double xx1, yy1;
        //        m2d(x1*1e6, y1*1e6, xx1, yy1);


    }
    break;

    case ID_2GIS_MAP:
        str.Format("https://tile1.maps.2gis.com/tiles?x=%d&y=%d&z=%d&v=1.2", /*ii+1,*/ xx, yy, nn);
        break;


    case ID_YANDEX_MAP:
        str.Format("http://vec0%d.maps.yandex.net/tiles?l=map&v=2.4.2&hl=ru&x=%d&y=%d&z=%d", ii + 1, xx, yy, nn);
        str.Format("http://vec0%d.maps.yandex.net/tiles?l=map&x=%d&y=%d&z=%d", ii + 1, xx, yy, nn);
        break;

    case ID_YANDEX_SAT:
        str.Format("http://sat0%d.maps.yandex.net/tiles?l=sat&v=1.21.0&hl=ru&x=%d&y=%d&z=%d", ii + 1, xx, yy, nn);
        str.Format("http://sat0%d.maps.yandex.net/tiles?l=sat&x=%d&y=%d&z=%d", ii + 1, xx, yy, nn);
        break;

    case ID_YANDEX_PMAP:
        str.Format("http://0%d.pvec.maps.yandex.net/tiles?l=pmap&x=%d&y=%d&z=%d", ii + 1, xx, yy, nn);
        break;

    case ID_OPENSTREETMAP:
        str.Format("http://%c.tile.openstreetmap.org/%d/%d/%d.png", abcd[ii], xx, yy, nn);
        str.Format("http://tah.openstreetmap.org/Tiles/tile/%d/%d/%d.png", nn, xx, yy);
        str.Format("http://%c.tile.openstreetmap.org/%d/%d/%d.png", abcd[ii], nn, xx, yy);

        //      str.Format("http://tile3.opencyclemap.org/landscape/%d/%d/%d.png", /*abcd[ii], */ nn, xx, yy);


        break;
    }

    ii = (ii + 1) % 3;

    return str;
};


CString get_name(int nn, int xx, int yy)
{
    CString str;
    if (isJpeg(map_typ)) {
        str.Format("%d/%d/%d.jpg", nn, xx, yy);
    }
    else {
        str.Format("%d/%d/%d.png", nn, xx, yy);
    }
    return str;
};

CString get_path()
{
    CString path0 = "path1";
    CString path;

    switch (map_typ) {
    case ID_GOOGLE_MAP:
        path0 = "GOOGLE_MAP";
        break;
    case ID_GOOGLE_SAT:
        path0 = "GOOGLE_SAT";
        break;
    case ID_GOOGLE_HYBRID:
        path0 = "GOOGLE_HYBRID";
        break;
    case ID_2GIS_MAP:
        path0 = "2GIS_MAP";
        break;
    case ID_YANDEX_MAP:
        path0 = "YANDEX_MAP";
        break;
    case ID_YANDEX_SAT:
        path0 = "YANDEX_SAT";
        break;
    case ID_YANDEX_PMAP:
        path0 = "YANDEX_PMAP";
        break;
    case ID_VISICOM_MAP:
        path0 = "VISICOM_MAP";
        break;

    case ID_MAP_SEMEY:
        path0 = "SEMEY\\CITY";
        break;
    case ID_MAP_SEMEY_TS:
        path0 = "SEMEY\\TS";
        break;





    case ID_WMS_MAP:
        path0 = "WMS_MAP\\";
        path0 += m_wms;
        path0 += "\\";
//        path0 += m_layer;
        path0 += getWMS_File_Name(m_layer);
        path0 += "\\";

        break;
    case ID_OPENSTREETMAP:
        path0 = "OPENSTREETMAP";
        break;
    }

    //  char path1[256];
    //  HRESULT ret = SHGetSpecialFolderPath(NULL,path1,CSIDL_APPDATA, 1);


    path.Format("%s/Sirius/tgid/map/%s/", m_pathMap, path0);

    return path;
}


CString fn(const char* ffn)
{
    CString str;
    CString path = get_path();


    str.Format("%s%s", path, ffn);
    return str;
}

struct DOWNL {
    DOWNL(const char* _fn, const char* _url, unsigned long _xx, unsigned long _yy, int _nn) :
        ffn(_fn), url(_url), xx(_xx), yy(_yy), nn(_nn)
    {};
    DOWNL() :
        ffn(""), url(""), xx(0), yy(0), nn(0)
    {};
    CString ffn, url;
    unsigned long xx, yy;
    int nn;

    CString get_url()
    {
        return url;
    };

    CString fn()
    {
        return ::fn(ffn);
    }
};

static deque<DOWNL> stack_download;

void Status(int n, LPCTSTR txt);
void drawJPG(CDC* dc, CRect& rect, const char* fn, int tr, int bk_color);
void drawPNG(CDC* dc, CRect& rect, const char* fn, int tr, int m_bk_color);

void drawPic(CDC* dc, CRect& rect, const char* fn, int tr, int bk_color)
{
    if (isJpeg(map_typ)) {
        drawJPG(dc, rect, fn, tr, bk_color);
    }
    else {
        drawPNG(dc, rect, fn, tr, bk_color);
    }
}


void mkdir_for_file(const char* fn)
{
    char s[1024];

    for (int i = strlen(fn)-1; i >= 0; i--) {
        if (fn[i] == ':') break;
        if (fn[i] == '\\' || fn[i] == '/') {
            strcpy(s, fn);
            s[i] = 0;
            mkdir_for_file(s);

            if (!IsFile(s)) {
                _mkdir(s);
            }
            break;
        }
    }
}

static bool is_read_tile = false;

bool isReadTile()
{
    if (is_read_tile) {
        is_read_tile = false;
        return true;
    }
    return false;
}

extern HWND m_hWndMain;



bool get_map(int nn, int xx, int yy, CString& fn, bool is_read, bool is_stack, int &n_download)
{
    CString path, name, url, tmpn;

    n_download = 0;

    name = get_name(nn, xx, yy);
    fn = get_path() + name;
    url = get_url(nn, xx, yy);

    if (url == "") return false;

    if (is_read) {
        tmpn.Format("%s\\tempfileformap.tmp", getenv("TEMP"));



        int ret = URLDownloadToFile(0, url, tmpn, 0, 0);
        if (ret == S_OK) {

            
//        bool ret = DownloadFile(url, tmpn);
//        if (ret) {
            
            if (IsFile(tmpn)) {
                mkdir_for_file(fn);
                //        MoveFileEx(tmpn, fn, MOVEFILE_REPLACE_EXISTING);

//                log1(url + " + " + fn);

                ::CopyFile(tmpn, fn, false);
                n_download++;

                return true;
            }
        }
        DeleteFile(tmpn);
        return false;
    }
    else {
        if (IsFile(fn) && !reread) {
            return true;
        }
        else {
            if (is_stack) {

                CSingleLock lock(&c_s);
                lock.Lock(1000);

                if (lock.IsLocked()) {
                    stack_download.push_back(DOWNL(name, url, xx, yy, nn));
                    lock.Unlock();
                }
            }
        }
    }
    return false;
}


std::pair<double, double> mercatorToWgs84(double x_merc, double y_merc) {
    // Преобразование долготы
    double lon = (x_merc / R) * (180.0 / M_PI);
    
    // Преобразование широты
    double lat = (atan(sinh(y_merc / R))) * (180.0 / M_PI);
    
    return {lat, lon};
}

std::pair<double, double> wgs84ToMercator(double lat, double lon) {
    // Преобразование долготы
    double x_merc = lon * (R * M_PI / 180.0);
    
    // Преобразование широты
    double y_merc = log(tan((90.0 + lat) * M_PI / 360.0)) * R;
    
    return {x_merc, y_merc};
}


double lon2x(double x)
{
    return x*geocoef;
}

double x2lon(double x)
{
    return x/geocoef;
}


double lat2y(double lat)
{
//    x_merc = lon * (R * math.pi / 180)
//    R = 6378137.0
//    y_merc = math.log(tan((90 + y) * M_PI / 360)) * R

//    double lat = y / 180. * M_PI;

    double y_merc = log(tan((90.0 + lat) * M_PI / 360.0)) * R;
    return y_merc/ 100000.;

//    return log(tan(M_PI / 4 + lat / 2)) * 180 / M_PI * geocoef;
}

double y2lat(double y)
{
//    double y_merc = y * COEF;
//    double lat = (atan(sinh(y_merc / R))) * (180.0 / M_PI);
//    return lat;

    double lat = y / 180. * M_PI / geocoef;
    return (atan(exp(lat)) * 360 / M_PI - 90);
}

#define BI_JPEG       4L


void SetHeaderMap(LPBITMAPINFOHEADER p_bminfo, int w, int h, int size)
{
    p_bminfo->biSize = sizeof(BITMAPINFOHEADER);
    p_bminfo->biWidth = w;
    p_bminfo->biHeight = h;
    p_bminfo->biPlanes = 1;
    p_bminfo->biBitCount = 0;
    p_bminfo->biCompression = BI_JPEG;

    p_bminfo->biBitCount = 24;
    p_bminfo->biCompression = BI_RGB;
    p_bminfo->biSizeImage = w * h;

    p_bminfo->biSizeImage = size;
    p_bminfo->biXPelsPerMeter = 0;
    p_bminfo->biYPelsPerMeter = 0;
    p_bminfo->biClrUsed = 0;
    p_bminfo->biClrImportant = 0;

    //  p_bminfo->biCompression = BI_RGB;
    //  p_bminfo->biSizeImage = 0;
    //  p_bminfo->biBitCount = 8;
}


void ErrorMsg(const char* s, int code);
int jpeg_decode(const char* filename, char* buf);
int jpeg_decode_tr(const char* filename, char* buf, int tr, int m_bk_color);

int get_tr();
int get_bk_color();


//int reget(const char* pszUrl, const char* fn, DWORD off, DWORD size);

static int m_nTransp;
static int m_bk_color;


void drawJPG(CDC* dc, CRect& rect, const char* fn, int tr, int bk_color)
{
    HANDLE hndl;
    LPVOID buf;

    hndl = ::GlobalAlloc(GMEM_MOVEABLE, _width * _width * 3);
    buf = (LPBITMAPINFO)::GlobalLock(hndl);

    BITMAPINFO BitsInfo;
    memset(&BitsInfo, 0, sizeof(BitsInfo));
    SetHeaderMap(&BitsInfo.bmiHeader, _width, _width, 0);

//    dc->SetStretchBltMode(STRETCH_DELETESCANS);
    dc->SetStretchBltMode(STRETCH_HALFTONE);

    jpeg_decode_tr(fn, (char*)buf, tr, bk_color);

    int qq = StretchDIBits(
        dc->m_hDC, rect.left, rect.top,
        rect.right - rect.left + 1, rect.bottom - rect.top,
        0, 0, _width, _width,
        buf, &BitsInfo, DIB_RGB_COLORS, SRCCOPY);

    GlobalUnlock(hndl);
    GlobalFree(hndl);
}


#define PNG_NO_EXTERN 1

#include "png/png.h"


BOOL LoadPngFile(PTSTR pstrFileName, png_byte** ppbImageData,
    int* piWidth, int* piHeight, int* piChannels, png_color* pBkgColor);


int rgb_l(int color, int tr, int bk_color);


void drawPNG(CDC* dc, CRect& rect, const char* fn, int tr, int m_bk_color)
{
    //  HANDLE hndl;
    //  LPVOID buf;

    FILE* f;

    //  f = fopen("C:/Documents and Settings/User/Application Data/Sirius/tgid/map/OPENSTREETMAP/15/22876/12051.png" , "rb");
    f = fopen(fn, "rb");

    if (!f) return;

    BITMAPINFO BitsInfo;
    memset(&BitsInfo, 0, sizeof(BitsInfo));
    SetHeaderMap(&BitsInfo.bmiHeader, _width, _width, 0);
    //  SetPalette((RGBQUAD*) ((LPSTR)buf+sizeof(BITMAPINFOHEADER)), _width);

//    dc->SetStretchBltMode(STRETCH_DELETESCANS);
    dc->SetStretchBltMode(STRETCH_HALFTONE);

    // проверяем сигнатуру файла (первые number байт)

    const int number = 8;

    png_byte sig[number] = { 0 };

    fread(sig, sizeof(png_byte), number, f);

    if (!png_check_sig(sig, number)) {
        fclose(f);
        return;
    }


    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    png_ptr->io_ptr = f;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    // говорим библиотеке, что мы уже прочли number байт, когда проверяли сигнатуру
    png_set_sig_bytes(png_ptr, number);
    // читаем всю информацию о файле
    png_read_info(png_ptr, info_ptr);


    if (setjmp(png_ptr->jmpbuf))
    {
        /* Free all of the memory associated with the png_ptr and info_ptr */
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        fclose(f);
        /* If we get here, we had a problem reading the file */
        return;
    }


    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type;

    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
        &interlace_type, NULL, NULL);

    // преобразуем файл если он содержит палитру в нормальный RGB
    //  if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth <= 8) png_set_palette_to_rgb(png_ptr);

//;    if (color_type == PNG_COLOR_TYPE_GRAY &&
//        bit_depth < 8) png_set_gray_1_2_4_to_8(png_ptr);

    png_set_expand(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY) {
        png_set_gray_to_rgb(png_ptr);
    }


    //  hndl = ::GlobalAlloc(GMEM_MOVEABLE, _width*_width*3);
    //  buf = (LPBITMAPINFO)::GlobalLock(hndl);

    unsigned char* buf;

    buf = new unsigned char[_width * _width * 4 * 2];
    unsigned char** b;

    b = new unsigned char* [_width * 4];

    int i;

    int c3 = 3;

    if (color_type == PNG_COLOR_TYPE_RGB_ALPHA) c3 = 4;

    for (i = 0; i < _width; i++) {
        b[i] = &((unsigned char*)buf)[i * _width * c3];
    }

    png_read_image(png_ptr, b);

    fclose(f);

    char* cbuf = (char*)buf;

    if (color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
        for (i = 0; i < _width * _width; i++) {
            int r, g, b, a;
            r = cbuf[i * 4];
            g = cbuf[i * 4 + 1];
            b = cbuf[i * 4 + 2];
            a = cbuf[i * 4 + 3];

            if (a == 0 && r == 0 && g == 0 && b == 0) {
                r = g = b = 255;
            }

            unsigned char im1[4];

            im1[0] = r;
            im1[1] = g;
            im1[2] = b;
            im1[3] = a;

            long* ii = (long*)&im1;
            ii[0] = rgb_l(ii[0], tr, m_bk_color);

            r = im1[0];
            g = im1[1];
            b = im1[2];

            cbuf[i * 3 + 2] = r;
            cbuf[i * 3 + 1] = g;
            cbuf[i * 3] = b;
        }
    }
    else {
        for (i = 0; i < _width * _width; i++) {
            int r, g, b;
            r = cbuf[i * 3];
            g = cbuf[i * 3 + 1];
            b = cbuf[i * 3 + 2];


            unsigned char im1[4];

            im1[0] = r;
            im1[1] = g;
            im1[2] = b;

            long* ii = (long*)&im1;
            ii[0] = rgb_l(ii[0], tr, m_bk_color);

            r = im1[0];
            g = im1[1];
            b = im1[2];

            cbuf[i * 3 + 2] = r;
            cbuf[i * 3 + 1] = g;
            cbuf[i * 3] = b;
        }
    }


    int qq = StretchDIBits(
        dc->m_hDC, rect.left, rect.top,
//        rect.right - rect.left + 1, rect.bottom - rect.top,
        rect.right - rect.left + 1, rect.bottom - rect.top,
        0, 0, _width, _width,
        buf, &BitsInfo, DIB_RGB_COLORS, SRCCOPY);


    delete[] b;
    delete[] buf;

    //  GlobalUnlock(hndl);
    //  GlobalFree(hndl);
}



int drawMap(CMapsThread* m_pMapsThread, CGidrView* view, CDC* dc, CRect& m_rect, int nn, bool is_stack)
{
    if (!m_pMapsThread || m_pMapsThread->m_isExit) {
        log1("Exit m_pMapsThread 1");
        return 0;
    }

    int n_download = 0;

    CString str;
    CFRect rectf1 = view->ScreenToCoord(m_rect);
    CFRect rectf;

    long xx1, xx2, yy1, yy2, xx, yy;

    ispr_rectf(rectf1, rectf);

    xx1 = nazv(rectf.left, nn);
    xx2 = nazv(rectf.right, nn);
    yy2 = nazv(-rectf.top, nn);
    yy1 = nazv(-rectf.bottom, nn);

    double n2 = pow(2., nn);

    xx1 = (rectf.left + 180) * n2 / 360;
    xx2 = (rectf.right + 180) * n2 / 360;

    yy1 = (180 + rectf.top) * n2 / 360.;
    yy2 = (180 + rectf.bottom) * n2 / 360.;

    xx2 = min(xx2, (1 << nn) - 1);
    yy2 = min(yy2, (1 << nn) - 1);

    xx1 = max(xx1, 0);
    yy1 = max(yy1, 0);

    view->setcolor(dc, 0xFFFF00);

    double dh = 720. / pow(2., nn + 1);

//    log1("!");


    //  if (yy2-yy1 < 50 && xx2 - xx1 < 50)
    for (yy = yy1; yy <= yy2; yy++) {
        for (xx = xx1; xx <= xx2; xx++) {

            if (!m_pMapsThread || m_pMapsThread->m_isExit) {
                log1("Exit m_pMapsThread");
                return n_download;
            }

            double fx = coord(xx, nn);
            double fy = coord(yy, nn);
            CFRect rr2 = CFRect(fx, fy + dh, fx + dh, fy);
            CFRect rr;

            ispr_rectf2(rr2, rr);

            //      rr = rr2;

            CRect rect = view->CoordToScreen(rr);

            CString tmpn;
            int n_download1;

            CString ss;
            ss.Format("%d xx=%d yy=%d %s", nn, xx, yy, tmpn);

//            log1(ss);

            if (get_map(nn, xx, yy, tmpn, false, is_stack, n_download1)) {
                n_download += n_download1;
                //      if (get_map(nn, xx, yy, tmpn, true, is_stack)) {
                drawPic(dc, rect, tmpn, view->m_nTransp, view->m_bk_color);

                bool m_isSetka = false;
                if (m_isSetka) view->rectangle(dc, rect.left, rect.top, rect.right, rect.bottom);
            }
        }
    }

    return n_download;
}

#import <msxml3.dll> named_guids
using namespace MSXML2;

static int init_wms = 0;

void setInit()
{
    init_wms = 0;
}

CString bstr2a(_bstr_t b);


CString get_wms_xml_name(CString r)
{
    CString ini;
    ini.Format("%s/Sirius/tgid/map/WMS_MAP/%s/index.xml", m_pathMap, m_wms, getWMS_File_Name(r));

    //    ini = "C:/Users/gena1/AppData/Roaming/Sirius/tgid/map/WMS_MAP/localhost_8080_geoserver_cite_wms/i.xml";

    return ini;
}


bool read_ini(CString ini)
{
    CString tmpn, url;
    url.Format("%s?SERVICE=WMS&VERSION=1.3.0&REQUEST=GetCapabilities", rovno);
    tmpn.Format("%s\\tempfileformap.tmp", getenv("TEMP"));
    int ret = URLDownloadToFile(0, url, tmpn, 0, 0);
    if (ret != S_OK) return false;
    mkdir_for_file(ini);
    ::CopyFile(tmpn, ini, FALSE);
    if (!IsFile(ini)) return false;

    return true;
}


#include "lib\tinyxml2\tinyxml2.h"

CString get_Layer(const char* r)
{
    CString ini = get_wms_xml_name(r);

    if (!read_ini(ini)) return "";

    //    CMMenu menu(AfxGetMainWnd(), IDD_MENU);
    CMMenu menu(AfxGetMainWnd(), IDD_MENU2, _TR("Выберите слой"));

    tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();

    doc->LoadFile(ini);

    tinyxml2::XMLElement* WMS_Capabilities = doc->FirstChildElement("WMS_Capabilities");

    if (WMS_Capabilities) {
        tinyxml2::XMLElement* Capability = WMS_Capabilities->FirstChildElement("Capability");
        if (Capability) {
            tinyxml2::XMLElement* Layer0 = Capability->FirstChildElement("Layer");
            if (Layer0) {

                /*
                                tinyxml2::XMLElement* EX_GeographicBoundingBox = Layer0->FirstChildElement("EX_GeographicBoundingBox");
                                if (EX_GeographicBoundingBox) {
                                    wx1 = atof(EX_GeographicBoundingBox->FirstChildElement("westBoundLongitude")->GetText());
                                    wx2 = atof(EX_GeographicBoundingBox->FirstChildElement("eastBoundLongitude")->GetText());
                                    wy1 = atof(EX_GeographicBoundingBox->FirstChildElement("southBoundLatitude")->GetText());
                                    wy2 = atof(EX_GeographicBoundingBox->FirstChildElement("northBoundLatitude")->GetText());
                                }
                */



                tinyxml2::XMLElement* Layer = Layer0->FirstChildElement("Layer");

                if (!Layer) Layer = Layer0;

                while (Layer) {
                    const char* str = 0;

                    tinyxml2::XMLElement* Title = Layer->FirstChildElement("Title");
                    if (Title) {
                        char a[10240];
                        str = Title->GetText();
                        utf8_to_a(str, a);
                        str = a;
                    }
                    else {
                        tinyxml2::XMLElement* Name = Layer->FirstChildElement("Name");
                        if (Name) {
                            str = Name->GetText();
                        }
                    }

                    if (str) {
                        menu.Add(str, (void *)Layer);
                    }

                    Layer = Layer->NextSiblingElement();
                }
            }
        }
    }


    if (menu.DoModal() == IDOK) {
        tinyxml2::XMLElement* Layer = (tinyxml2::XMLElement*)menu.getV();

        tinyxml2::XMLElement* EX_GeographicBoundingBox = Layer->FirstChildElement("EX_GeographicBoundingBox");
        if (EX_GeographicBoundingBox) {
            wx1 = atof(EX_GeographicBoundingBox->FirstChildElement("westBoundLongitude")->GetText());
            wx2 = atof(EX_GeographicBoundingBox->FirstChildElement("eastBoundLongitude")->GetText());
            wy1 = atof(EX_GeographicBoundingBox->FirstChildElement("southBoundLatitude")->GetText());
            wy2 = atof(EX_GeographicBoundingBox->FirstChildElement("northBoundLatitude")->GetText());
        }

        const char* str = "";

        tinyxml2::XMLElement* Name = Layer->FirstChildElement("Name");
        if (Name) {
            str = Name->GetText();
        }

        return str;
    }

    return "";
}


bool get_WMS()
{
    //    if (init_wms) return true;

    CString ini = get_wms_xml_name(rovno);

    if (!read_ini(ini)) return false;

    CMMenu menu(AfxGetMainWnd(), IDD_MENU);

    tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();

    doc->LoadFile(ini);

    tinyxml2::XMLElement* WMS_Capabilities = doc->FirstChildElement("WMS_Capabilities");

    if (WMS_Capabilities) {
        tinyxml2::XMLElement* Capability = WMS_Capabilities->FirstChildElement("Capability");
        if (Capability) {
            tinyxml2::XMLElement* Layer0 = Capability->FirstChildElement("Layer");
            if (Layer0) {


                /*
                                tinyxml2::XMLElement* EX_GeographicBoundingBox = Layer0->FirstChildElement("EX_GeographicBoundingBox");
                                if (EX_GeographicBoundingBox) {
                                    wx1 = atof(EX_GeographicBoundingBox->FirstChildElement("westBoundLongitude")->GetText());
                                    wx2 = atof(EX_GeographicBoundingBox->FirstChildElement("eastBoundLongitude")->GetText());
                                    wy1 = atof(EX_GeographicBoundingBox->FirstChildElement("southBoundLatitude")->GetText());
                                    wy2 = atof(EX_GeographicBoundingBox->FirstChildElement("northBoundLatitude")->GetText());
                                }
                */


                tinyxml2::XMLElement* Layer = Layer0->FirstChildElement("Layer");

                if (!Layer) Layer = Layer0;


                while (Layer) {

                    tinyxml2::XMLElement* Name = Layer->FirstChildElement("Name");
                    if (Name) {
                        const char* str = 0;
                        str = Name->GetText();
                        if (!strcmp(str, m_layer)) {
                            tinyxml2::XMLElement* EX_GeographicBoundingBox = Layer->FirstChildElement("EX_GeographicBoundingBox");
                            if (EX_GeographicBoundingBox) {
                                wx1 = atof(EX_GeographicBoundingBox->FirstChildElement("westBoundLongitude")->GetText());
                                wx2 = atof(EX_GeographicBoundingBox->FirstChildElement("eastBoundLongitude")->GetText());
                                wy1 = atof(EX_GeographicBoundingBox->FirstChildElement("southBoundLatitude")->GetText());
                                wy2 = atof(EX_GeographicBoundingBox->FirstChildElement("northBoundLatitude")->GetText());
                                return true;
                            }
                        }
                    }
                    Layer = Layer->NextSiblingElement();
                }
            }
        }
    }

    return false;
}

bool redrawMap(CMapsThread* m_pMapsThread, CGidrView* view, CDC* dc, CRect& m_rect, int typ_map, int is_reread)
{
    map_typ = typ_map;
    reread = is_reread;

    if (!view || !::IsWindow(view->m_hWnd)) return false;

    if (typ_map == ID_NOMAP) return false;
    if (!isMap(typ_map)) return false;

    if (view->m_parent_id != 0) return false;

    double mas = view->geom.masx;

    view->geom.masx /= COEF;
    view->geom.masy /= COEF;

    CString str;

    CClientDC screenDC(NULL);
    dmas = screenDC.GetDeviceCaps(LOGPIXELSX) / 2.54;
    //  dmas = view->dmas;

    //  double nn1 = log(360./20000/view->masx)/log(2.);
    double nn = log(360. * dmas / MIN_LEN / (view->geom.masx * view->geom.dmas)) / log(2.);

    //  str.Format("%g", nn);
    //  Status(2, str);

    char buf[256];
    nn = atof(_gcvt(nn, 7, buf));

    //  nn1 = max(1., nn1);
    nn = max(1., nn);

    if (typ_map == ID_WMS_MAP) {
        _width = 512;
        MIN_LEN = 128;
        MIN_LEN = 256;

        //        _width = 512;
        //        MIN_LEN = 64;

                //    MIN_LEN = 64;
                //    get_WMS();
    }
    else {
        _width = 256;
        MIN_LEN = 196.;
    }


    if (typ_map == ID_WMS_MAP) {
        nn = min(nn, 21);
    }
    else if (typ_map == ID_2GIS_MAP) {
        nn = min(nn, 18);
    }
    else if (typ_map == ID_OPENSTREETMAP) {
        nn = min(nn, 19);
    }
    else if (typ_map == ID_MAP_SEMEY || typ_map == ID_MAP_SEMEY_TS) {
        nn = min(nn, 19);
    }
    else {
        nn = min(nn, 18);
    }

    if (typ_map == ID_YANDEX_MAP || typ_map == ID_YANDEX_SAT) {
        nn = min(nn, 17);
    }


    if (map_mas != view->geom.masx * view->geom.dmas) {

        CSingleLock lock(&c_s);
        lock.Lock(1000);

        if (lock.IsLocked()) {
            stack_download.clear();
            lock.Unlock();
        }
    }

    map_mas = view->geom.masx * view->geom.dmas;

    if (typ_map == ID_WMS_MAP && !init_wms) {
        if (!get_WMS()) {
            AfxMessageBox("Не могу подключиться к WMS-серверу!");
        }
        init_wms = 1;
    }


    int n_download = drawMap(m_pMapsThread, view, dc, m_rect, nn, true);

    //  for (int i = nn-1; i > 8; i--) drawMap(view, dc, m_rect, i, true);

    view->geom.masx = view->geom.masy = mas;
    if (m_pMapsThread) m_pMapsThread->Tic();
    reread = FALSE;

    return n_download > 0;
}


IMPLEMENT_DYNCREATE(CMapsThread, CWinThread)


CMapsThread::CMapsThread()
{
    m_isExit = 0;
    m_isOff = 0;
    threadno = 0;
}


CMapsThread::CMapsThread(void* t)
{
    m_isExit = 0;
    m_isOff = 0;
    threadno = 0;

    threadno = t;
}


CMapsThread::~CMapsThread()
{
    //    delete m_event;
    m_event = nullptr;
}


BOOL CMapsThread::InitInstance()
{
    m_event = new CEvent(FALSE, FALSE, "CMapsThread");

    while (TRUE) {
        if (m_isExit) {
            break;
        }

        ::WaitForSingleObject(m_event->m_hObject, INFINITE);

        if (m_isExit) {
            break;
        }

        if (m_event) m_event->ResetEvent();


        CGidrView* view = (CGidrView*)threadno;
        int online = 1;
#if 1
        ::Status(1, "OffLine");
#endif
        CString str;

        //    deque<DOWNL> stack_download1 = stack_download;
        //    stack_download.clear();
        //    while (stack_download.size() > 100) {
        //      DOWNL downl = stack_download.back();
        //      stack_download.pop_front();
        //    }

        int n_download = 0;

        while (!m_isExit) {
            int stack_size = 0;
            DOWNL downl;

            CString ss;
//            ss.Format("CMapsThread::InitInstance %d", this);
//            log1(ss);

            CSingleLock lock(&c_s);
            lock.Lock(1000);

            if (lock.IsLocked()) {
                stack_size = stack_download.size();
                if (stack_size != 0) {
                    downl = stack_download.back();
                    stack_download.pop_back();
                }
                lock.Unlock();
            }

            if (stack_size == 0) break;

            BOOL bIsConnection;
            DWORD dwConnectionState;
            bIsConnection = InternetGetConnectedState(&dwConnectionState, NULL);
            //С проверкой флажков dwConnectionState можно поиграться, 
            //хотя данная комбинация пока не подводила 
            if (!(bIsConnection && (dwConnectionState & INTERNET_CONNECTION_LAN ||
                dwConnectionState & INTERNET_CONNECTION_MODEM)))
            {
                if (online) {
#if 1
                    ::Status(1, "OffLine");
#endif


                    online = 0;
                }
            }
            else {
                online = 1;

                CString fnn = downl.ffn;
                str.Format("%d %d %s ", stack_size, fnn.GetLength(), fnn);
                CString fn = downl.fn();
                CString tmpn = fn;

                int nn = downl.nn;

                double mas = view->geom.masx / COEF;

                mas = map_mas;

                double nn0 = log(360. * dmas / MIN_LEN / mas) / log(2.);

                if (nn <= nn0) {
                    double dh = 720 / pow(2., nn + 1);
                    double fx = coord(downl.xx, nn);
                    double fy = coord(downl.yy, nn);
                    CFRect rr, rr2 = CFRect(fx, fy + dh, fx + dh, fy);

                    ispr_rectf2(rr2, rr);
                    rr.left *= COEF; rr.right *= COEF; rr.top *= COEF; rr.bottom *= COEF;

                    int isInternet = 1;

                    //          if (isInternet && isMap(typ_map)) {
                    if (isInternet) {
#if 1
                        ::Status(1, str);
#endif
                        if (view->m_parent_id == 0) {

                            int n_download1;

                            if (get_map(nn, downl.xx, downl.yy, tmpn, true, false, n_download1)) {
                                n_download += n_download1;
                                CRect rect = view->CoordToScreen(rr);

                                CClientDC dc(view);
                                drawPic(&dc, rect, fn, view->m_nTransp, view->m_bk_color);
//                                view->rectangle(&dc, rect.left, rect.top, rect.right, rect.bottom);
                            }
#if 1
                            ::Status(1, str + " +");
#endif
                        }
                    }
                }
            }
        }
        if (n_download) {
            n_download = 0;
            //                BOOL ret = ::PostMessage(m_hWndMain, WM_USER_REFRESH, 0, 0);
            log1("Refresh");
            view->PostMessage(WM_USER_REFRESH, 0, 0);
        }
    }

    m_isOff = 1;

    return ExitInstance();
}

int CMapsThread::ExitInstance()
{
    //    ExitThread(0);
    log1("ExitInstance!!!");
    return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CMapsThread, CWinThread)
    //{{AFX_MSG_MAP(CMapsThread)
      // NOTE - the ClassWizard will add and remove mapping macros here.
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CMapsThread::Tic()
{
    m_event->SetEvent();
}

void CMapsThread::Exit()
{
    log1("CMapsThread::Exit");
    m_isExit = 1;
    Tic();
//    m_event2 = event2;
}

BOOL CMapsThread::isOff()
{
    return m_isOff;
}
