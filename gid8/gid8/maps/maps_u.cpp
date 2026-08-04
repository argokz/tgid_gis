#include <QtGui>
#include <QMessageBox>
#include <QSettings>
#include <QCache>
#include <gidview/GidWidget.h>
#include <algorithm>
#include <math.h>
#include <QPointF>
#include <QtXml>

#include "maps_id.h"

#include "geodez/geodez.h"
#include "cxema/coordlis.h"


double y2lat(double y);
double lat2y(double y);
//double WGS84_SK42_Lat(double Bd, double Ld, double H);
//double WGS84_SK42_Long(double Bd, double Ld, double H);
//double SK42_WGS84_Lat(double Bd, double Ld, double H);
//double SK42_WGS84_Long(double Bd, double Ld, double H);

void m2d3(SystemCoord& system_coord, double x, double y, double &B, double &L);


#if 0

// Метры из 42 года 3 градусной в метры WGS84 исправленное
void m2m_42_WGS84_(SystemCoord& system_coord, double X1, double Y1, double &xx, double &yy)
{
    double xxx, yyy;

    m2d3(system_coord, X1, Y1, xxx, yyy);

    xx = SK42_WGS84_Lat(xxx, yyy, 0);
    yy = SK42_WGS84_Long(xxx, yyy, 0);

    xx = xxx;
    yy = yyy;


    yy = yy * 100000;
    xx = lat2y(xx) * 100000;
}

// Метры из WGS84 в метры 42 года 3 градусной  исправленное

void m2m_WGS84_42_(SystemCoord& system_coord, double X1, double Y1, double &xx, double &yy)
{
    double xxx, yyy;


    yy = Y1 / 100000;
    xx = y2lat(X1/100000) ;

    xxx = WGS84_SK42_Lat(xx, yy, 0);
    yyy = WGS84_SK42_Long(xx, yy, 0);

    xxx = xx;
    yyy = yy;

    d2m3(system_coord, xxx, yyy, xx, yy);
}

#endif




// http://localhost:8080/geoserver/cite/wms

//QString rovno = "http://62.80.170.250:6080/arcgis/services/Rovno/Rovno/MapServer/WmsServer";
//QString rovno = "http://45.132.85.23:8085/geoserver/tgid/wms";
//QString kiev = "http://62.80.170.250:6080/arcgis/services/Kiev/Borispol/MapServer/WmsServer";

//#define KHARKIV 1

//static double m_False_Easting = 0;
//static double m_Central_Meridian = 0;

static SystemCoord m_system_coord;

SystemCoord *get_system_coord()
{
    return &m_system_coord;
}


#define COEF 10000000

//#include "json.h"
/*
void write_system_coord(JSON& json)
{
    json.add("False_Easting", m_system_coord.False_Easting);
    json.add("False_Northing", m_system_coord.False_Northing);
    json.add("Central_Meridian", m_system_coord.Central_Meridian);
    json.add("Latitude_Of_Origin", m_system_coord.Latitude_Of_Origin);
    json.add("Scale_Factor", m_system_coord.Scale_Factor);
    json.add("Angle", m_system_coord.Angle);
}
*/

void get_google_coord(CFPoint pt, double& x, double& y)
{
    if (m_system_coord.isMercator()) {
        y = y2lat(-pt.y / COEF);
        x = pt.x / COEF;
        return;
    }

    double X1 = -pt.y / 100;
    double Y1 = pt.x / 100;

    double xx1, yy1;
    m2m_42_WGS84(m_system_coord, X1, Y1, xx1, yy1);

    x = yy1 / 100000;
    y = -y2lat(-xx1 / 100000);
}

CFPoint get_google_to_coord(double x, double y)
{
    CFPoint pt;

    if (m_system_coord.isMercator()) {
        pt.y = -lat2y(y) * COEF;
        pt.x = x * COEF;

        return pt;
    }

    double yy1, xx1, X1, Y1;

    yy1 = x * 100000;
    xx1 = -lat2y(-y) * 100000;

    m2m_WGS84_42(m_system_coord, xx1, yy1, X1, Y1);

    pt.x = Y1 * 100;
    pt.y = -X1 * 100;

    return pt;
}

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

    if (m_system_coord.Scale_Factor == 0) {
        m_system_coord.Scale_Factor = 1.;
    }

//    m_system_coord.False_Easting = 40405;
//    m_system_coord.False_Northing = -5671188;
//    m_system_coord.Central_Meridian = 72;
//    m_system_coord.Latitude_Of_Origin = 0;
//    m_system_coord.Scale_Factor = 1;
//    m_system_coord.Angle = 0;


}


void set_system_coord(SystemCoord sc)
{
    m_system_coord = sc;
}

//static double wx1 = 0, wx2 = 0, wy1 = 0, wy2 = 0;
//extern QString m_pathMap;
QString m_pathMap;


static int _width = 256;
//static double MIN_LEN = 196.;

static double MIN_LEN = 128.;

//#define  MIN_LEN 196.




void ispr_point(CFPoint point, CFPoint point2)
{
    if (m_system_coord.isMercator()) {
        point2 = point;
        return;
    }

    double Y1 = point.x * COEF / 100;
    double X1 = -point.y * COEF / 100;

    double xxx1, yyy1;

    m2m_42_WGS84(m_system_coord, X1, Y1, xxx1, yyy1);

    point2.x = yyy1 / 100000;
    point2.x = -xxx1 / 100000;
}



void ispr_rectf(CFRect rectf, CFRect& rectf2)
{
    if (m_system_coord.isMercator()) {
        rectf2 = rectf;
        return;
    }

    double Y1 = rectf.left * COEF / 100;
    double X2 = -rectf.top * COEF / 100;

    double Y2 = rectf.right * COEF / 100;
    double X1 = -rectf.bottom * COEF / 100;

    double xxx1, xxx2, xxx3, xxx4, yyy1, yyy2, yyy3, yyy4;

    m2m_42_WGS84(m_system_coord, X1, Y1, xxx1, yyy1);
    m2m_42_WGS84(m_system_coord, X1, Y2, xxx2, yyy2);
    m2m_42_WGS84(m_system_coord, X2, Y2, xxx3, yyy3);
    m2m_42_WGS84(m_system_coord, X2, Y1, xxx4, yyy4);

    rectf2.left = yyy1 / 100000;
    rectf2.right = yyy2 / 100000;

    rectf2.top = -xxx3 / 100000;
    rectf2.bottom = -xxx1 / 100000;
}

void ispr_rectf2(CFRect rectf, CFRect& rectf2)
{
    if (m_system_coord.isMercator()) {
        rectf2 = rectf;
        return;
    }

    double Y1 = rectf.left * 100000;
    double Y2 = rectf.right * 100000;
    double X2 = -rectf.top * 100000;
    double X1 = -rectf.bottom * 100000;

    double xxx1, xxx2, xxx3, xxx4, yyy1, yyy2, yyy3, yyy4;

    m2m_WGS84_42(m_system_coord, X1, Y1, xxx1, yyy1);
    m2m_WGS84_42(m_system_coord, X1, Y2, xxx2, yyy2);
    m2m_WGS84_42(m_system_coord, X2, Y2, xxx3, yyy3);
    m2m_WGS84_42(m_system_coord, X2, Y1, xxx4, yyy4);

    rectf2.left = yyy1 / 100000;
    rectf2.right = yyy2 / 100000;

    rectf2.top = -xxx4 / 100000;
    rectf2.bottom = -xxx1 / 100000;
}

void ispr_rectf2(CFRect rectf, CFRect& rectf2, double & xxx1, double & yyy1, double & xxx2, double & yyy2, double & xxx3, double & yyy3, double & xxx4, double & yyy4)
{
    if (m_system_coord.isMercator()) {
        rectf2 = rectf;
        return;
    }

    double Y1 = rectf.left * 100000;
    double Y2 = rectf.right * 100000;
    double X2 = -rectf.top * 100000;
    double X1 = -rectf.bottom * 100000;

//    double xxx1, xxx2, xxx3, xxx4, yyy1, yyy2, yyy3, yyy4;

    m2m_WGS84_42(m_system_coord, X1, Y1, xxx1, yyy1);
    m2m_WGS84_42(m_system_coord, X1, Y2, xxx2, yyy2);
    m2m_WGS84_42(m_system_coord, X2, Y2, xxx3, yyy3);
    m2m_WGS84_42(m_system_coord, X2, Y1, xxx4, yyy4);


    rectf2.left = yyy1 / 100000;
    rectf2.right = yyy2 / 100000;

    rectf2.top = -xxx4 / 100000;
    rectf2.bottom = -xxx1 / 100000;

    xxx1 = -xxx1/100000;
    xxx2 = -xxx2/100000;
    xxx3 = -xxx3/100000;
    xxx4 = -xxx4/100000;

    yyy1 = yyy1/100000;
    yyy2 = yyy2/100000;
    yyy3 = yyy3/100000;
    yyy4 = yyy4/100000;
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

CFPoint get_WMS_XY()
{
    return CFPoint((wx1 + wx2) / 2 * 10000000., -lat2y((wy1 + wy2) / 2) * 10000000.);
}

*/

bool isJpeg(int typ)
{
    //  return typ == ID_YANDEX_SAT || typ == ID_GOOGLE_SAT;
    return typ == ID_YANDEX_SAT || typ == ID_GOOGLE_SAT || typ == ID_GOOGLE_HYBRID
        || typ == ID_MAPTILER_STREETS || typ == ID_MAPTILER_TOPO
        || typ == ID_MAPTILER_HYBRID || typ == ID_MAPTILER_OSM;
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

    if (typ_map == ID_ESRI_SATELLITE) return true;

    if (typ_map == ID_MAPTILER_STREETS) return true;
    if (typ_map == ID_MAPTILER_TOPO) return true;
    if (typ_map == ID_MAPTILER_HYBRID) return true;
    if (typ_map == ID_MAPTILER_OSM) return true;


    if (typ_map == ID_MAP_SEMEY) return true;
    if (typ_map == ID_MAP_SEMEY_TS) return true;

    return false;
}


void log(const char* podp, const char* s);

//static int map_typ = ID_NOMAP;
static double map_mas = 1;
//static CSemaphore c_s;
static bool reread = false;
static int dmas = 0;


unsigned int nazv(double x, int n)
{
    unsigned int xx;
    x = (x + 180.) / 180;

//    int j = 0;

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

QString maptiler_key()
{
    // Ключ MapTiler в исходники не попадает. В этом проекте уже был
    // случай, когда в коммит чуть не уехал токен Google (gid6/dop/
    // converter_old32), поэтому порядок такой же, как у пароля БД:
    // сначала настройки программы, затем переменная окружения.
    // Пустой ключ означает "карта не настроена" — get_url вернёт пустую
    // строку, и провайдер просто не будет запрашиваться.
    static QString key;
    static bool loaded = false;

    if (!loaded) {
        loaded = true;
        QSettings settings;
        key = settings.value("maps/maptiler_key").toString().trimmed();
        if (key.isEmpty()) {
            key = qEnvironmentVariable("MAPTILER_KEY").trimmed();
        }
        if (key.isEmpty()) {
            qWarning() << "MapTiler: ключ не задан "
                          "(настройка maps/maptiler_key или MAPTILER_KEY)";
        }
    }
    return key;
}

// Растровые тайлы MapTiler — обычного разрешения, 256x256.
//
// Суффикс @2x убран по требованию заказчика: карта грузилась слишком
// долго. Замер это подтверждал — плитка @2x весит 94 КБ против 36 КБ
// обычной (в 2,7 раза больше), на экран 1920x1080 их нужно около 54,
// то есть смена масштаба тянула ~5 МБ вместо ~1,9 МБ. При этом MapTiler
// отдаёт с ОДНОГО хоста, тогда как Google и OSM — с трёх поддоменов, а
// Qt держит не больше шести одновременных соединений на хост: разница
// в объёме превращалась в разницу во времени почти напрямую.
//
// Попытки вернуть @2x «по плотности экрана» здесь быть не должно:
// решение принято по результату работы, а не по характеристикам
// монитора.
//
// Формат — JPEG для всех четырёх стилей. Замер на одной и той же
// плитке: streets-v4 36 КБ в PNG против 17 КБ в JPEG, topo-v4 37
// против 17, openstreetmap 36 против 23, hybrid 43 против 26. Для
// подложки под сеть потеря качества несущественна, а объём вдвое
// меньше. Вместе с отказом от @2x это 94 КБ -> 17 КБ на плитку.
static QString maptiler_url(const QString &style, const QString &ext,
                            int nn, int xx, int yy)
{
    QString key = maptiler_key();
    if (key.isEmpty()) return QString();

    return QString("https://api.maptiler.com/maps/%1/256/%2/%3/%4.%5?key=%6")
            .arg(style).arg(nn).arg(xx).arg(yy).arg(ext).arg(key);
}

QString get_url(int nn, int xx, int yy, int map_typ)
{
    QString str;
    char abcd[] = "abc";
    static int ii = 0;

    long num = 156000000;

    num = 177290279;
    num = 0;

    switch (map_typ) {
    case ID_GOOGLE_MAP:
        str = QString("http://mt%1.google.com/vt/lyrs=m@%2&hl=ru&x=%3&y=%4&z=%5&s=G").arg(ii).arg(num).arg(xx).arg(yy).arg(nn);
        break;
    case ID_GOOGLE_SAT:
        str = QString("http://mt%1.google.com/vt/lyrs=s@%2&hl=ru&x=%3&y=%4&z=%5&s=G").arg(ii).arg(num).arg(xx).arg(yy).arg(nn);
        break;
    case ID_GOOGLE_HYBRID:
        str = QString("http://mt%1.google.com/vt/lyrs=y@%2&hl=ru&x=%3&y=%4&z=%5&s=G").arg(ii).arg(num).arg(xx).arg(yy).arg(nn);
        break;

#if !USE_ITWIN

    case ID_VISICOM_MAP:
    {
        int yy2 = (1 << nn) - 1;
        str = QString("http://tms%1.visicom.ua/2.0.0/planet3/base/%2/%3/%4.png").arg(ii + 1).arg(nn).arg(xx).arg(yy2 - yy);
    }
    break;

    case ID_MAP_SEMEY:
        str = QString("http://vkomap.kz/Proxy/Semey/Semey/MapServer/tile/%1/%2/%3.png").arg(nn).arg(yy).arg(xx);
        break;

    case ID_MAP_SEMEY_TS:
        str = QString("http://vkomap.kz/Proxy/Semey/Heat/MapServer/tile/%1/%2/%3.png").arg(nn).arg(yy).arg(xx);
        break;
#endif

#if 1


    case ID_WMS_MAP:
    {
#if 0
        double x1, y1, x2, y2;

        double n2 = pow(2., nn);

        x1 = (xx * 360. / n2) - 180.;
        y1 = ((yy + 1.) * 360. / n2) - 180.;

        y1 = -y1;

        double dx = 360. / n2;

        x2 = x1 + dx;
        y2 = y1 + dx;

        y1 = y2lat(y1);
        y2 = y2lat(y2);

        if (x2 < wx1 || x1 > wx2 || y2 < wy1 || y1 > wy2) {
            str = "";
        }
        else {
            str = QString("%1?request=GetMap"
                          "&service=WMS&version=1.3.0"
                          "&layers=%2"
                          "&crs=EPSG:4326"  //  WGS84 - World Geodetic System 1984, used in GPS
//                          "&crs=EPSG:3395"  // WGS 84 / World Mercator
//                          "&crs=EPSG:3857"  // Google Maps
//                          "&crs=EPSG:4024" // Unknown datum based upon the Krassowsky 1940 ellipsoid
                          "&bbox=%3,%4,%5,%6"
                          "&width=%7&height=%8"
                          "&format=image/%9")
                    .arg(rovno).arg(m_layer)
                    .arg(y1, 0, 'f', 10)
                    .arg(x1, 0, 'f', 10)
                    .arg(y2, 0, 'f', 10)
                    .arg(x2, 0, 'f', 10)
                    .arg(_width).arg(_width).arg(isJpeg(map_typ) ? "jpeg" : "png");
        }
#endif
    }
    break;
#endif

    case ID_2GIS_MAP:
        str = QString("https://tile1.maps.2gis.com/tiles?x=%1&y=%2&z=%3&v=1.2").arg( /*ii+1,*/ xx).arg(yy).arg(nn);
        break;

    case ID_ESRI_SATELLITE:
        str = QString("https://services.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer/tile/%3/%2/%1").arg(xx).arg(yy).arg(nn);
        break;

    case ID_MAPTILER_STREETS:
        str = maptiler_url("streets-v4", "jpg", nn, xx, yy);
        break;

    case ID_MAPTILER_TOPO:
        str = maptiler_url("topo-v4", "jpg", nn, xx, yy);
        break;

    case ID_MAPTILER_HYBRID:
        str = maptiler_url("hybrid-v4", "jpg", nn, xx, yy);
        break;

    case ID_MAPTILER_OSM:
        str = maptiler_url("openstreetmap", "jpg", nn, xx, yy);
        break;


    case ID_YANDEX_MAP:
        str = QString("http://vec0%1.maps.yandex.net/tiles?l=map&v=2.4.2&hl=ru&x=%2&y=%3&z=%4").arg(ii + 1).arg(xx).arg(yy).arg(nn);
        str = QString("http://vec0%1.maps.yandex.net/tiles?l=map&x=%2&y=%3&z=%4").arg(ii + 1).arg(xx).arg(yy).arg(nn);

//https://core-renderer-tiles.maps.yandex.net/vmap2/tiles?lang=ru_KZ&x=23387&y=12035&z=15&zmin=16&zmax=16&v=24.12.17-1-b241215200100&ads=enabled&client_id=yandex-web-maps&experimental_ranking_mode_name=default-web-ranking&experimental_data_hd=vegetation_model_exp
//curl ^"https://core-renderer-tiles.maps.yandex.net/vmap2/tiles?lang=ru_KZ^&x=23387^&y=12035^&z=15^&zmin=16^&zmax=16^&v=24.12.17-1-b241215200100^&ads=enabled^&client_id=yandex-web-maps^&experimental_ranking_mode_name=default-web-ranking^&experimental_data_hd=vegetation_model_exp^" ^

//    https://core-renderer-tiles.maps.yandex.net/vmap2/tiles?x=23387&y=12032&z=15

//    https://core-renderer-tiles.maps.yandex.net/vmap2/tiles?x=23387&y=12032&z=15

        break;

    case ID_YANDEX_SAT:
        str = QString("http://sat0%1.maps.yandex.net/tiles?l=sat&v=1.21.0&hl=ru&x=%2&y=%3&z=%4").arg(ii + 1).arg(xx).arg(yy).arg(nn);
        str = QString("http://sat0%1.maps.yandex.net/tiles?l=sat&x=%2&y=%3&z=%4").arg(ii + 1).arg(xx).arg(yy).arg(nn);
        break;

    case ID_YANDEX_PMAP:
        str = QString("http://0%1.pvec.maps.yandex.net/tiles?l=pmap&x=%2&y=%3&z=%4").arg(ii + 1).arg(xx).arg(yy).arg(nn);
        break;


    case ID_OPENSTREETMAP:
//        str = QString("http://%1.tile.openstreetmap.org/%2/%3/%4.png").arg(abcd[ii]).arg(nn).arg(xx).arg(yy);
        str = QString("https://%1.tile.openstreetmap.org/%2/%3/%4.png").arg(abcd[ii]).arg(nn).arg(xx).arg(yy);
        qDebug() << str;


//https://aisgzk.kz/aisgzk/Proxy/aisgzkZem2/MapServer/export?dpi=96&transparent=true&format=png8&layers=show%3A125&bbox=8136861.78634192%2C6415065.878826487%2C8137304.730795675%2C6415646.288800373&bboxSR=3857&imageSR=3857&size=783%2C1026&_ts=1734433615311&f=image
//https://core-renderer-tiles.maps.yandex.net/tiles?l=mapj&v=24.12.17-1-b241215200100&x=9907&y=5136&z=14&scale=1&lang=ru_RU&experimental_disable_toponym_hotspots=true&callback=x_9907_y_5136_z_14_l_mapj
        break;
    }

    ii = (ii + 1) % 3;

    return str;
};

struct LayerUrl {
    QString name;
    QString url;
    QString fn;
};


std::list<LayerUrl> get_url_wms(const QString & rovno, std::list<Layer> & m_layers, int nn, int xx, int yy, int map_typ)
{
    QString str;
    char abcd[] = "abc";
    static int ii = 0;

    long num = 156000000;

    num = 177290279;
    num = 0;

    double x1, y1, x2, y2;

    double n2 = pow(2., nn);

    x1 = (xx * 360. / n2) - 180.;
    y1 = ((yy + 1.) * 360. / n2) - 180.;

    y1 = -y1;

    double dx = 360. / n2;

    x2 = x1 + dx;
    y2 = y1 + dx;

    y1 = y2lat(y1);
    y2 = y2lat(y2);

    std::list<LayerUrl> ll;

    for (auto m_layer: m_layers) {
        if (x2 < m_layer.wx1 || x1 > m_layer.wx2 || y2 < m_layer.wy1 || y1 > m_layer.wy2) {
        }
        else {
            LayerUrl lu;

            lu.name = m_layer.name;

            lu.url = QString("%1?request=GetMap"
                          "&service=WMS&version=1.3.0"
                          "&layers=%2"
                          "&crs=EPSG:4326"  //  WGS84 - World Geodetic System 1984, used in GPS
                          //                          "&crs=EPSG:3395"  // WGS 84 / World Mercator
                          //                          "&crs=EPSG:3857"  // Google Maps
                          //                          "&crs=EPSG:4024" // Unknown datum based upon the Krassowsky 1940 ellipsoid
                          "&bbox=%3,%4,%5,%6"
                          "&width=%7&height=%8"
                          "&transparent=TRUE"
                          "&format=image/%9"
                          )
                      .arg(rovno).arg(m_layer.name)
                      .arg(y1, 0, 'f', 10)
                      .arg(x1, 0, 'f', 10)
                      .arg(y2, 0, 'f', 10)
                      .arg(x2, 0, 'f', 10)
                      .arg(_width).arg(_width).arg(isJpeg(map_typ) ? "jpeg" : "png");

            lu.url.replace("/gwc/service/wmts", "/wms");

//            qDebug() << lu.url;

            ll.push_back(lu);
        }
    }

    ii = (ii + 1) % 3;

    return ll;

}



QString get_name(int nn, int xx, int yy, int map_typ)
{
    QString str;
    if (isJpeg(map_typ)) {
        str = QString("%1/%2/%3.jpg").arg(nn).arg(xx).arg(yy);
    }
    else {
        str = QString("%1/%2/%3.png").arg(nn).arg(xx).arg(yy);
    }
    return str;
};

QString get_path(int map_typ)
{
    QString path0 = "path1";

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

    case ID_ESRI_SATELLITE:
        path0 = "ESRI_SATELLITE";
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
        path0 = "SEMEY/CITY";
        break;
    case ID_MAP_SEMEY_TS:
        path0 = "SEMEY/TS";
        break;

    case ID_WMS_MAP:
        path0 = "WMS_MAP/";
//        path0 += m_wms;
//        path0 += "/";
//        path0 += m_layer;
//        path0 += getWMS_File_Name(m_layer);
//        path0 += "/";

        break;
    case ID_OPENSTREETMAP:
        path0 = "OPENSTREETMAP";
        break;

    case ID_MAPTILER_STREETS:
        path0 = "MAPTILER_STREETS";
        break;
    case ID_MAPTILER_TOPO:
        path0 = "MAPTILER_TOPO";
        break;
    case ID_MAPTILER_HYBRID:
        path0 = "MAPTILER_HYBRID";
        break;
    case ID_MAPTILER_OSM:
        path0 = "MAPTILER_OSM";
        break;
    }

    //  char path1[256];
    //  HRESULT ret = SHGetSpecialFolderPath(NULL,path1,CSIDL_APPDATA, 1);

//#ifdef Q_WS_WIN

//QString appDir = argpath_2()+"map/"

//QString appDir = QDir::homePath() + "/Application Data/Sirius/tgid/map/";
//#elif defined(Q_WS_X11)
//QString appDir = QDir::homePath() + "/.config/yourapp/";
//#endif

    QString path = QString("%1/map/%2/").arg(argpath_2()).arg(path0);

    return path;
}

/*
QString fn(const char* ffn)
{
    QString str;
    QString path = get_path();


    str = QString("%s%s", path, ffn);
    return str;
}
*/
#if 0

struct DOWNL {
    DOWNL(const QString & _fn, const QString &  _url, unsigned long _xx, unsigned long _yy, int _nn) :
        ffn(_fn), url(_url), xx(_xx), yy(_yy), nn(_nn)
    {};
    DOWNL() :
        ffn(""), url(""), xx(0), yy(0), nn(0)
    {};
    QString ffn, url;
    unsigned long xx, yy;
    int nn;

    QString get_url()
    {
        return url;
    };

    QString fn()
    {
        return get_path()+ffn;
    }
};

static std::deque<DOWNL> stack_download;

#endif

void drawText1(QPainter* painter, double x, double y, int flags, const QString & str, double angle);


void drawRamka(QPainter *painter, CFRect& rect, const QString & str)
{
    return;
//    int flags = Qt::AlignCenter|Qt::AlignHCenter;
//    int flags = Qt::AlignLeft|Qt::AlignTop|Qt::TextDontClip;
    int flags = Qt::AlignCenter|Qt::AlignTop|Qt::TextDontClip;

    painter->setPen(QPen(Qt::black, 1.));

    painter->drawRect(rect.left, rect.top, rect.Width(), rect.Height());

//    painter->drawLine(rect.left, rect.bottom, rect.right, rect.top);
    double cx = (rect.left+rect.right)/2;
    double cy = (rect.bottom+rect.top)/2;

//    painter->drawLine(rect.left, rect.bottom, cx,cy);
//    painter->drawLine(rect.left, rect.top, cx,cy);

#if 1
    QFont font("Tahoma", 10);

    painter->setFont(font);
    
    QRectF rect1 = QRectF(cx, cy, 0, 0);

    painter->boundingRect ( rect1, flags, str);
    painter->drawText(rect1, flags, str, &rect1);
//    drawText1(painter, cx, cy, flags, str, 0);
#endif
}


// Кэш скачанных тайлов.
//
// Раньше это был std::map без какого-либо вытеснения: каждая once
// показанная плитка оставалась в памяти до конца работы программы.
// Пока тайлы были 256x256, это сходило с рук; с @2x плитка 512x512 в
// разобранном виде занимает около мегабайта, и прогулка по городу
// набирает гигабайты. QCache вытесняет давно не использованные записи
// сам, стоимость записи считаем в килобайтах.
//
// Второй кэш — уже отмасштабированные под экран плитки. drawPic делал
// image.scaled(..., SmoothTransformation) на КАЖДУЮ перерисовку каждой
// плитки; при сдвиге карты целевой размер не меняется, поэтому
// пересчёт был чистой потерей времени, вчетверо подорожавшей с @2x.
static const int kTileCacheKb = 256 * 1024;    // 256 МБ исходных
static const int kScaledCacheKb = 128 * 1024;  // 128 МБ масштабированных

static QCache<QString, QImage> images(kTileCacheKb);
static QCache<QString, QImage> scaled_images(kScaledCacheKb);

static int image_cost_kb(const QImage &im)
{
    // Не меньше единицы: запись нулевой стоимости QCache не вытесняет.
    return qMax(1, int(im.sizeInBytes() / 1024));
}

static std::set<QString> set_downloads;

QImage getImage(const QString & fn, bool &yes)
{
    if (reread) {
        // Перечитывание: плитка устарела. Масштабированные варианты
        // сбрасываются один раз в начале прохода (см. redrawMap), а не
        // здесь: getImage вызывается на каждую плитку, и чистка всего
        // кэша в этом месте повторялась бы десятки раз за кадр.
        images.remove(fn);
        yes = false;
        return QImage();
    }

    if (QImage *cached = images.object(fn)) {
        yes = true;
        // QImage неявно разделяемый: копия здесь не копирует пиксели.
        // Возвращаем именно значение, а не указатель, — вставка в кэш
        // ниже по коду может вытеснить и удалить объект.
        return *cached;
    }

    if (QFile::exists(fn)) {
        QImage image(fn);
        if (!image.isNull()) {
            images.insert(fn, new QImage(image), image_cost_kb(image));
            yes = true;
            return image;
        }
    }

    yes = false;
    return QImage();
}


bool yesImage(const QString & fn)
{
    if (reread) return false;

    if (images.contains(fn)) {
        return true;
    }

    if (QFile::exists(fn)) {
        return true;
    }
    return false;
}


QImage adjustBrightness(const QImage &image, int brightnessPercent) {
    QImage result = image.convertToFormat(QImage::Format_ARGB32);
    int adjustment = (255 * brightnessPercent) / 100;

    for (int y = 0; y < result.height(); ++y) {
        QRgb *line = reinterpret_cast<QRgb *>(result.scanLine(y));
        for (int x = 0; x < result.width(); ++x) {
            QColor color = QColor::fromRgb(line[x]);
            color.setRed(qBound(0, (color.red() * adjustment) / 255, 255));
            color.setGreen(qBound(0, (color.green() * adjustment) / 255, 255));
            color.setBlue(qBound(0, (color.blue() * adjustment) / 255, 255));
            line[x] = color.rgba();
        }
    }

    return result;
}

// На сколько уровней вверх искать уже загруженную плитку. Каждый шаг
// растягивает картинку вдвое; после пяти уровней это уже мыло крупнее
// экрана, от которого пользы меньше, чем от пустого места.
static const int kParentDepth = 5;

// Пока плитка нужного масштаба качается, на её месте оставался белый
// прямоугольник. Карты так не делают: они показывают кусок уже
// загруженной плитки более крупного масштаба, растянутый на это место,
// и подменяют его, когда придёт настоящая.
static bool drawParentTile(QPainter *painter, CFRect &rect, int map_typ,
                           int nn, int xx, int yy)
{
    // При перечитывании родительские плитки такие же устаревшие, и
    // getImage их ещё и вычистит из кэша — показывать нечего.
    if (reread) return false;

    for (int k = 1; k <= kParentDepth && nn - k >= 0; ++k) {
        const int pnn = nn - k;
        const int pxx = xx >> k;
        const int pyy = yy >> k;

        bool yes = false;
        QImage parent = getImage(get_path(map_typ)
                                 + get_name(pnn, pxx, pyy, map_typ), yes);
        if (!yes || parent.isNull()) continue;

        // Искомая плитка занимает 1/2^k ширины и высоты родительской;
        // её место внутри родителя — остаток от деления координат.
        const int div = 1 << k;
        const double sw = double(parent.width()) / div;
        const double sh = double(parent.height()) / div;
        const double sx = (xx - (pxx << k)) * sw;
        const double sy = (yy - (pyy << k)) * sh;

        painter->drawImage(
            QRectF(rect.left, rect.top, rect.Width(), rect.Height() + 2),
            parent, QRectF(sx, sy, sw, sh));
        return true;
    }
    return false;
}

// Обратный случай: масштаб уменьшили, и плиток более крупного масштаба
// ещё нет — зато есть четыре дочерние с прошлого вида. Собираем место
// из них. Глубже одного уровня не идём: там уже 16 плиток на одно
// место, и собирать дороже, чем дождаться загрузки.
static bool drawChildTiles(QPainter *painter, CFRect &rect, int map_typ,
                           int nn, int xx, int yy)
{
    if (reread) return false;

    const double w = rect.Width() / 2.;
    const double h = (rect.Height() + 2) / 2.;

    bool drawn = false;

    for (int dy = 0; dy < 2; ++dy) {
        for (int dx = 0; dx < 2; ++dx) {
            bool yes = false;
            QImage child = getImage(
                get_path(map_typ)
                + get_name(nn + 1, xx * 2 + dx, yy * 2 + dy, map_typ), yes);
            if (!yes || child.isNull()) continue;

            painter->drawImage(
                QRectF(rect.left + dx * w, rect.top + dy * h, w, h), child);
            drawn = true;
        }
    }
    return drawn;
}

void drawPic(QPainter *painter, CFRect& rect, const QString & fn, int nTransp, int bk_color, int i, int k, int nn, int map_typ)
{
    bool yes;
    QImage image = getImage(fn, yes);
//    if (nTransp < 100) {
//        image = adjustBrightness(image, nTransp);
//    }

    if (!yes) {
        // Настоящей плитки ещё нет. Сначала пробуем растянуть кусок
        // плитки более крупного масштаба (так бывает при увеличении),
        // затем — собрать место из четырёх дочерних (при уменьшении).
        if (!drawParentTile(painter, rect, map_typ, nn, i, k)) {
            drawChildTiles(painter, rect, map_typ, nn, i, k);
        }
        return;
    }

    if (yes) {
        const int w = int(rect.Width());
        const int h = int(rect.Height()) + 2;

        if (w > 0 && h > 0) {
            // Масштабирование с плавным сглаживанием — самая дорогая
            // операция отрисовки карты, а при сдвиге целевой размер не
            // меняется, и раньше оно повторялось на каждый кадр для
            // каждой плитки. Результат кэшируем по имени файла и
            // размеру: сдвиг теперь попадает в кэш целиком, пересчёт
            // остаётся только при смене масштаба.
            const QString key = QString("%1|%2x%3").arg(fn).arg(w).arg(h);

            QImage ready;
            if (QImage *cached = scaled_images.object(key)) {
                ready = *cached;
            }
            else {
                ready = image.scaled(w, h, Qt::IgnoreAspectRatio,
                                     Qt::SmoothTransformation);
                // Вставка может не удаться (плитка дороже всего кэша) —
                // тогда QCache удалит переданный объект, поэтому рисуем
                // из локальной копии, а не из указателя кэша.
                scaled_images.insert(key, new QImage(ready),
                                     image_cost_kb(ready));
            }

            painter->drawImage(QPointF(rect.left, rect.top), ready);
        }

//        qDebug() << "Рисую " << i << " " << k;

//        QFileInfo fileInfo(fn);
//        QString baseName = fileInfo.baseName(); 

//        yy1 = (180 + rectf.top) * n2 / 360.;
     
        double n2 = pow(2., nn);
        
        double fx = (i*360.)/n2-180;
        double fy = -y2lat((k*360.)/n2-180);

        drawRamka(painter, rect, QString("%1 %2\n%3 %4").arg(i).arg(k).arg(fx, 0, 'f', 5).arg(fy, 0, 'f', 5));
    }
}

QImage makeWhiteTransparent(const QImage &image)
{
    QImage result = image.convertToFormat(QImage::Format_ARGB32);
    QColor transparentColor(255, 255, 255); // Белый цвет

    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            if (result.pixelColor(x, y) == transparentColor) {
                result.setPixelColor(x, y, QColor(255, 255, 255, 0)); // Установить альфа-канал в 0
            }
        }
    }
    return result;
}


void drawPic_wms(QPainter *painter, CFRect& rect, std::list<QString> & list_fn, int nTransp, int bk_color, int i, int k, int nn)
{
    QRectF target(rect.left, rect.bottom, rect.Width(), rect.Height());

    QString ffn = "";

    for (auto &fn : list_fn) {
        bool yes;
        QImage image = getImage(fn, yes);

        if (yes) {
//            image = makeWhiteTransparent(image);

    //        painter->setRenderHint(QPainter::SmoothPixmapTransform);
//            image = image.scaled(rect.Width(), rect.Height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            image = image.scaled(rect.Width(), rect.Height(), Qt::IgnoreAspectRatio, Qt::FastTransformation);
            painter->drawImage(QPointF(rect.left, rect.top), image);
            qDebug() << "Нарисовал " << fn;

            ffn = fn;
        }
    }

    drawRamka(painter, rect, QString("%1 %2\n%3").arg(i).arg(k).arg(nn));

//    qDebug() << "Рамка";
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

//extern HWND m_hWndMain;


static QSet<QString> set_d;

void remove_file(DownloadManager &download_manager, const QString &fn)
{
    if (QFile::exists(fn)) {
        qDebug() << "remove " << fn;
        QFile file(fn);
        file.remove();
    }
    // Файл плитки удалён — вместе с ним должны уйти и разобранная
    // копия, и все её масштабированные варианты, иначе на экране
    // останется картинка, которой на диске уже нет.
    images.remove(fn);
    scaled_images.clear();
    download_manager.erase(fn);
    qDebug() << "remove " << fn;
}

bool get_map(DownloadManager &download_manager, int map_typ, int nn, int xx, int yy, int &n_download, QString &fn)
{
    n_download = 0;

    QString name = get_name(nn, xx, yy, map_typ);
    fn = get_path(map_typ) + name;

    if (reread) {
        remove_file(download_manager, fn);
    }



    if (!reread) {
        if ((download_manager.contains(fn) || QFile::exists(fn))) {
            return true;
        }
    }

    QString _url = get_url(nn, xx, yy, map_typ);

    if (_url == "") return false;
    set_d.insert(_url);
    QUrl url = QUrl::fromEncoded(_url.toLocal8Bit());

    download_manager.doDownload(url, fn);

    n_download += 1;

    return true;
}

QString getWMS_File_Name(const QString & r);

int get_map_wms(const QString & rovno, std::list<Layer> &m_layers, DownloadManager &download_manager, int map_typ, int nn, int xx, int yy, int &n_download, std::list<QString> &list_fn)
{
    n_download = 0;

    std::list<LayerUrl> lu = get_url_wms(rovno, m_layers, nn, xx, yy, map_typ);

    for ( auto & it : lu) {
        QString _url = it.url;
        QString name = it.name + "/" + get_name(nn, xx, yy, map_typ);

        QString fn = get_path(map_typ) + getWMS_File_Name(rovno) + "/" + name;

        if (reread) {
            remove_file(download_manager, fn);
        }

        if (!download_manager.contains(fn) && !yesImage(fn) || reread) {
            set_d.insert(_url);
            QUrl url = QUrl::fromEncoded(_url.toLocal8Bit());
            download_manager.doDownload(url, fn);

            n_download += 1;
        }
        else {
            list_fn.push_back(fn);
        }
    }

    return n_download;
}



double lat2y(double y)
{
    double lat = y / 180. * M_PI;
    return log(tan(M_PI / 4 + lat / 2)) * 180 / M_PI;
}

double y2lat(double B)
{
    double lat = B / 180. * M_PI;
    return (atan(exp(lat)) * 360. / M_PI - 90.);
}


void ErrorMsg(const char* s, int code);

int get_tr();
int get_bk_color();


//static int m_nTransp;
//static int m_bk_color;




int GidWidget::drawMap(QPainter* dc, CFRect& m_rect, int map_typ, int nn)
{
    int n_download = 0;

    CFRect rectf1 = ScreenToCoord(m_rect);
    CFRect rectf;

    double xx1, xx2, yy1, yy2;

    ispr_rectf(rectf1, rectf);

    double n2 = pow(2., nn);

    xx1 = (rectf.left + 180) * n2 / 360;
    xx2 = (rectf.right + 180) * n2 / 360;

    yy1 = (180 + rectf.top) * n2 / 360.;
    yy2 = (180 + rectf.bottom) * n2 / 360.;


//    qDebug() << "= drawMap1" << xx1 << " - " << xx2 << " " << yy1 << " - " << yy2;
//    qDebug() << m_rect.left << " " << m_rect.right << " " << m_rect.top << " " << m_rect.bottom;
//    qDebug() << rectf1.left << " " << rectf1.right << " " << rectf1.top << " " << rectf1.bottom;
//    qDebug() << rectf.left << " " << rectf.right << " " << rectf.top << " " << rectf.bottom;


    xx2 = fmin(xx2, (1 << nn) - 1);
    yy2 = fmin(yy2, (1 << nn) - 1);

    xx1 = fmax(xx1, 0.);
    yy1 = fmax(yy1, 0.);


//    qDebug() << "= drawMap2" << xx1 << " - " << xx2 << " " << yy1 << " - " << yy2;


    double dh = 720. / pow(2., nn + 1);


    for (int yy = yy1; yy <= yy2; yy ++) {
        for (int xx = xx1; xx <= xx2; xx ++) {
            double fx = coord(xx, nn);
            double fy = coord(yy, nn);
            CFRect rr2 = CFRect(fx, fy, fx + dh, fy + dh);
            CFRect rr;

//            ispr_rectf2(rr2, rr);
            double xxx1, yyy1, xxx2, yyy2, xxx3, yyy3, xxx4, yyy4;
            ispr_rectf2(rr2, rr, xxx1, yyy1, xxx2, yyy2, xxx3, yyy3, xxx4, yyy4);
            CFRect rect = CoordToScreen(rr);

            CFPoint pp1 = CFPoint(rect.left, rect.bottom);
            CFPoint pp2 = CFPoint(rect.right, rect.bottom);
            CFPoint pp3 = CFPoint(rect.right, rect.top);
            CFPoint pp4 = CFPoint(rect.left, rect.top);

            CFPoint p1 = CoordToScreen(CFPoint(yyy1, xxx1));
            CFPoint p2 = CoordToScreen(CFPoint(yyy2, xxx2));
            CFPoint p3 = CoordToScreen(CFPoint(yyy3, xxx3));
            CFPoint p4 = CoordToScreen(CFPoint(yyy4, xxx4));

            double x1 = std::min({p1.x, p2.x, p3.x, p4.x});
            double x2 = std::max({p1.x, p2.x, p3.x, p4.x});

            double y1 = std::min({p1.y, p2.y, p3.y, p4.y});
            double y2 = std::max({p1.y, p2.y, p3.y, p4.y});

            QTransform tansf;

            QPolygonF r1, r2;

            r1.push_back(QPointF(pp1.x, pp1.y));
            r1.push_back(QPointF(pp2.x, pp2.y));
            r1.push_back(QPointF(pp3.x, pp3.y));
            r1.push_back(QPointF(pp4.x, pp4.y));

            r2.push_back(QPointF(p1.x, p1.y));
            r2.push_back(QPointF(p2.x+1, p2.y));
            r2.push_back(QPointF(p3.x+1, p3.y+1));
            r2.push_back(QPointF(p4.x, p4.y+1));

//            qDebug() << "==";
//            qDebug() << r1 << " " << r2;
//            qDebug() << transf;

            bool ok = QTransform::quadToQuad(r1, r2, tansf);

//            qDebug() << "ok " << ok;

            if (ok) {
                dc->setTransform(tansf);

                int n_download1;

                if (map_typ == ID_WMS_MAP) {
                    std::list<QString> tmpn;

                    if (reread) {
                        int qq;
                        qq = 1;
                    }

                    if (nn == 20 && xx == 748245 && yy == 384318){
                        if (reread) {
                            int qq;
                            qq = 1;
                        }
                        else {
                            int qq;
                            qq = 1;
                        }
                    }

                    int n = get_map_wms(rovno, m_layers, download_manager, map_typ, nn, xx, yy, n_download1, tmpn);

                    if (n || true) {
                        n_download += n_download1;
                        drawPic_wms(dc, rect, tmpn, m_nTransp, 255, xx, yy, nn);
                    }

                }
                else {
                    QString tmpn;
                    
                    if (get_map(download_manager, map_typ, nn, xx, yy, n_download1, tmpn)) {
                        n_download += n_download1;
                        drawPic(dc, rect, tmpn, m_nTransp, 255, xx, yy, nn, map_typ);
                    }
                }

                QTransform tansf0;
                dc->setTransform(tansf0);
            }
        }
    }

    return n_download;
}


void GidWidget::onCtrlF5()  // Перерисовать схему
{
    qDebug() << "onCtrlF5()";

    CFRect r(0, 0, m_wi, m_hi);
    deleteMapAll(r, m_internetMap);
    repaint();
}


void GidWidget::deleteMapAll(CFRect& m_rect, int map_typ)
{
    int n_max = maxNmap(map_typ);

    for (int n = 1; n <= n_max; n++) {
        deleteMap(m_rect, map_typ, n);
    }
}


void GidWidget::deleteMap(CFRect& m_rect, int map_typ, int nn)
{

    double masx = geom.masx;
    double masy = geom.masy;

    geom.masx /= COEF;
    geom.masy /= COEF;

    int n_download = 0;

    CFRect rectf1 = ScreenToCoord(m_rect);
    CFRect rectf;

    double xx1, xx2, yy1, yy2;

    ispr_rectf(rectf1, rectf);

    double n2 = pow(2., nn);

    xx1 = (rectf.left + 180) * n2 / 360;
    xx2 = (rectf.right + 180) * n2 / 360;

    yy1 = (180 + rectf.top) * n2 / 360.;
    yy2 = (180 + rectf.bottom) * n2 / 360.;

    xx2 = fmin(xx2, (1 << nn) - 1);
    yy2 = fmin(yy2, (1 << nn) - 1);

    xx1 = fmax(xx1, 0.);
    yy1 = fmax(yy1, 0.);

    double dh = 720. / pow(2., nn + 1);

    for (int yy = yy1; yy <= yy2; yy ++) {
        for (int xx = xx1; xx <= xx2; xx ++) {
            double fx = coord(xx, nn);
            double fy = coord(yy, nn);
            CFRect rr2 = CFRect(fx, fy, fx + dh, fy + dh);
            CFRect rr;

//            ispr_rectf2(rr2, rr);
            double xxx1, yyy1, xxx2, yyy2, xxx3, yyy3, xxx4, yyy4;
            ispr_rectf2(rr2, rr, xxx1, yyy1, xxx2, yyy2, xxx3, yyy3, xxx4, yyy4);
            CFRect rect = CoordToScreen(rr);

            CFPoint pp1 = CFPoint(rect.left, rect.bottom);
            CFPoint pp2 = CFPoint(rect.right, rect.bottom);
            CFPoint pp3 = CFPoint(rect.right, rect.top);
            CFPoint pp4 = CFPoint(rect.left, rect.top);

            CFPoint p1 = CoordToScreen(CFPoint(yyy1, xxx1));
            CFPoint p2 = CoordToScreen(CFPoint(yyy2, xxx2));
            CFPoint p3 = CoordToScreen(CFPoint(yyy3, xxx3));
            CFPoint p4 = CoordToScreen(CFPoint(yyy4, xxx4));

            double x1 = std::min({p1.x, p2.x, p3.x, p4.x});
            double x2 = std::max({p1.x, p2.x, p3.x, p4.x});

            double y1 = std::min({p1.y, p2.y, p3.y, p4.y});
            double y2 = std::max({p1.y, p2.y, p3.y, p4.y});

            QTransform tansf;

            QPolygonF r1, r2;

            r1.push_back(QPointF(pp1.x, pp1.y));
            r1.push_back(QPointF(pp2.x, pp2.y));
            r1.push_back(QPointF(pp3.x, pp3.y));
            r1.push_back(QPointF(pp4.x, pp4.y));

            r2.push_back(QPointF(p1.x, p1.y));
            r2.push_back(QPointF(p2.x+1, p2.y));
            r2.push_back(QPointF(p3.x+1, p3.y+1));
            r2.push_back(QPointF(p4.x, p4.y+1));

            bool ok = QTransform::quadToQuad(r1, r2, tansf);

            if (ok) {
                if (map_typ == ID_WMS_MAP) {
                    for (auto m_layer: m_layers) {
                        if (x2 < m_layer.wx1 || x1 > m_layer.wx2 || y2 < m_layer.wy1 || y1 > m_layer.wy2) {
                        }
                        else {
                            QString name = m_layer.name + "/" + get_name(nn, xx, yy, map_typ);
                            QString fn = get_path(map_typ) + getWMS_File_Name(rovno) + "/" + name;
                            remove_file(download_manager, fn);
                        }
                    }
                }
                else {
                    QString tmpn;
                    QString name = get_name(nn, xx, yy, map_typ);
                    QString fn = get_path(map_typ) + name;
                    remove_file(download_manager, fn);
                }
            }
        }
    }

    geom.masx = masx;
    geom.masy = masy;
}




//#endif

//#import <msxml3.dll> named_guids
//using namespace MSXML2;

static int init_wms = 0;

void setInit()
{
    init_wms = 0;
}

//QString bstr2a(_bstr_t b);


QString get_wms_xml_name(const QString & r)
{
    m_pathMap = argpath_2();
    return QString("%1/map/WMS_MAP/%2/index.xml").arg(m_pathMap).arg(getWMS_File_Name(r)); //.arg(getWMS_File_Name(r));
}

#include <dialog/MMenuDial.h>

bool get_WMS()
{
#if 0
    return true;
    QString ini = get_wms_xml_name(rovno);
    
    QFile file(ini);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QDomDocument dom;
    dom.setContent(&file);
    file.close();

    QDomNodeList layers = dom.firstChildElement("WMS_Capabilities").firstChildElement("Capability")
            .elementsByTagName("Layer");

    for (int i = 0; i < layers.count(); ++i) {
        QDomNode layer = layers.item(i);
        QDomNode nm = layer.firstChildElement("Name");
        QString name = nm.firstChild().nodeValue();

        if (name == m_layer) {
            QDomNode box = layer.firstChildElement("EX_GeographicBoundingBox");
            if (!box.isNull()) {
                wx1 = box.firstChildElement("westBoundLongitude").firstChild().nodeValue().toDouble();
                wx2 = box.firstChildElement("eastBoundLongitude").firstChild().nodeValue().toDouble();
                wy1 = box.firstChildElement("southBoundLatitude").firstChild().nodeValue().toDouble();
                wy2 = box.firstChildElement("northBoundLatitude").firstChild().nodeValue().toDouble();
                return true;
            }
        }
    }

#endif
    return false;
}

int GidWidget::maxNmap(int typ_map)
{
    if (typ_map == ID_WMS_MAP) {
        return 21;
    }
    else if (typ_map == ID_2GIS_MAP) {
        return 18;
    }
    else if (typ_map == ID_OPENSTREETMAP) {
        return 19;
    }
    else if (typ_map == ID_MAP_SEMEY || typ_map == ID_MAP_SEMEY_TS) {
        return 19;
    }
    else if (typ_map == ID_GOOGLE_MAP) {
        return 21;
    }
    else if (typ_map == ID_MAPTILER_STREETS || typ_map == ID_MAPTILER_TOPO
             || typ_map == ID_MAPTILER_HYBRID
             || typ_map == ID_MAPTILER_OSM) {
        // Проверено запросами: тайлы отдаются вплоть до z=22.
        return 21;
    }
    if (typ_map == ID_YANDEX_MAP || typ_map == ID_YANDEX_SAT) {
        return 17;
    }
    return 18;
}



bool GidWidget::redrawMap(QPainter* dc, CFRect& m_rect, int typ_map, bool is_reread)
{
//    map_typ = typ_map;
    reread = is_reread;

    if (reread) {
        qDebug() << "Reread";
        // Один сброс на весь проход перечитывания: масштабированные
        // плитки ключуются именем файла и размером на экране, адресно
        // их не выбрать, а держать устаревшие нельзя.
        scaled_images.clear();
    }


    if (typ_map == ID_NOMAP) return false;
    if (!isMap(typ_map)) return false;

//    if (view->m_parent_id != 0) return false;

    double masx = geom.masx;
    double masy = geom.masy;

    geom.masx /= COEF;
    geom.masy /= COEF;
    dmas = geom.dmas;


    if (typ_map == ID_WMS_MAP) {
        _width = 512;
        MIN_LEN = 128;
        MIN_LEN = 256;

        _width = 256;
        MIN_LEN = 196.;

        _width = 512;
        _width = 1024;
        MIN_LEN = _width*3./4.;
//        MIN_LEN = _width/2;
        
        
        //        _width = 512;
        //        MIN_LEN = 64;
                //    MIN_LEN = 64;
                //    get_WMS();
    }
    else {
        _width = 256;
        MIN_LEN = 196.;
//        MIN_LEN = 128.;
    }

    double nn = log(360. * dmas / MIN_LEN / (geom.masx * geom.dmas)) / log(2.);
    
    double max_n = maxNmap(typ_map);
    
    nn = max(1., nn);
    nn = min(max_n, nn);

    map_mas = geom.masx * geom.dmas;
/*
    if (typ_map == ID_WMS_MAP && !init_wms) {
        if (!get_WMS()) {
            onWmsCust();
            return false;
        }
        init_wms = 1;
    }
*/

    int n_download = drawMap(dc, m_rect, typ_map, nn);

    geom.masx = masx;
    geom.masy = masy;
    reread = false;

    return n_download > 0;
}

void GidWidget::setMaps(int internetMap)
{
//    m_internetMap = internetMap;

    std::map<QAction*, int> map_map =
    {
    {gidrAction.aNomap               , ID_NOMAP},
    {gidrAction.aGoogleMap           , ID_GOOGLE_MAP},
    {gidrAction.aGoogleSat           , ID_GOOGLE_SAT},
    {gidrAction.aGoogleHybrid        , ID_GOOGLE_HYBRID},
    {gidrAction.aVisicomMap          , ID_VISICOM_MAP},
    {gidrAction.aWmsMap              , ID_WMS_MAP},
    {gidrAction.aMapSemey            , ID_MAP_SEMEY},
    {gidrAction.aMapSemeyTs          , ID_MAP_SEMEY_TS},
    {gidrAction.aOpenstreetmap       , ID_OPENSTREETMAP},
    {gidrAction.a2gisMap             , ID_2GIS_MAP},

    {gidrAction.aEsriSatMap             , ID_ESRI_SATELLITE},

    {gidrAction.aMaptilerStreets        , ID_MAPTILER_STREETS},
    {gidrAction.aMaptilerTopo           , ID_MAPTILER_TOPO},
    {gidrAction.aMaptilerHybrid         , ID_MAPTILER_HYBRID},
    {gidrAction.aMaptilerOsm            , ID_MAPTILER_OSM}
    };

    for (auto &it : map_map) {
        if (it.second == internetMap) {
            it.first->setChecked(true);
            return;
        }
    }
    gidrAction.aNomap->setChecked(true);
}

void GidWidget::onMaps(QAction *action)
{
    m_internetMapAction = action;

/*
ID_YANDEX_MAP,    
ID_YANDEX_SAT,    
ID_YANDEX_PMAP,   
*/

    std::map<QAction*, int> map_map =
    {
    {gidrAction.aNomap               , ID_NOMAP},
    {gidrAction.aGoogleMap           , ID_GOOGLE_MAP},
    {gidrAction.aGoogleSat           , ID_GOOGLE_SAT},
    {gidrAction.aGoogleHybrid        , ID_GOOGLE_HYBRID},
    {gidrAction.aVisicomMap          , ID_VISICOM_MAP},
    {gidrAction.aWmsMap              , ID_WMS_MAP},
    {gidrAction.aMapSemey            , ID_MAP_SEMEY},
    {gidrAction.aMapSemeyTs          , ID_MAP_SEMEY_TS},
    {gidrAction.aOpenstreetmap       , ID_OPENSTREETMAP},
    {gidrAction.a2gisMap             , ID_2GIS_MAP},
    {gidrAction.aEsriSatMap          , ID_ESRI_SATELLITE},

    {gidrAction.aMaptilerStreets     , ID_MAPTILER_STREETS},
    {gidrAction.aMaptilerTopo        , ID_MAPTILER_TOPO},
    {gidrAction.aMaptilerHybrid      , ID_MAPTILER_HYBRID},
    {gidrAction.aMaptilerOsm         , ID_MAPTILER_OSM}

    };

    m_internetMap = ID_NOMAP;

    auto it = map_map.find(action);
    if (it != map_map.end()) {
        m_internetMap = it->second;
    }
    repaint();
}

void mkdir_for_file(const QString & fn);

GidWidget *getView();


bool GidWidget::read_wms_2(const QString & txt)
{
    if (txt.indexOf("http://www.opengis.net/wmts") != -1) {
        qDebug() << "wmts";
        read_wmts_ask(txt);
    }
    else if (txt.indexOf("http://www.opengis.net/wms") != -1) {
        qDebug() << "wms";
        read_wms_ask(txt);
    }
    m_internetMap = ID_WMS_MAP;
    repaint();

    return false;
}

bool GidWidget::read_wms_3(const QString & txt)
{
    if (txt.indexOf("http://www.opengis.net/wmts") != -1) {
        qDebug() << "wmts";
        read_wmts(txt);
    }
    else if (txt.indexOf("http://www.opengis.net/wms") != -1) {
        qDebug() << "wms";
        read_wms(txt);
    }
    return false;
}


bool fun_read_ini2(const QString & txt)
{
    return getView()->read_wms_2(txt);
}

bool fun_read_ini2_error(const QString & txt)
{
    QMessageBox::warning(nullptr, "", txt);
    return true;
}

bool fun_read_ini3(const QString & txt)
{
    return getView()->read_wms_3(txt);
}

bool fun_read_ini3_error(const QString & txt)
{
    QMessageBox::warning(nullptr, "", txt);
    return true;
}


void setView(GidWidget *view);


void GidWidget::onWmsCust() // Настройка
{
    bool ok;
    QSettings settings;

    QString text = settings.value("WMS_host", "").toString();

    QString out = QInputDialog::getText(this,"Адрес WMS-сервера", tr("Введите адрес WMS-сервера"), QLineEdit::Normal, text, &ok);

    if (ok) {
        settings.setValue("WMS_host", rovno);
        qDebug() << rovno;

        rovno = out;
//        QString url = QString("%1?SERVICE=WMS&VERSION=1.3.0&REQUEST=GetCapabilities").arg(rovno);
        QString url = QString("%1?request=GetCapabilities").arg(rovno);
        setView(this);
        download_manager2.doDownload(QUrl::fromEncoded(url.toLocal8Bit()), fun_read_ini2, fun_read_ini2_error);
    }
}
    

void GidWidget::setWMS(const QString & _rovno, const QString & _layer)
{
    if (m_internetMap != ID_WMS_MAP) return;
    
    QSettings settings;

    rovno = settings.value("WMS_host", "").toString();
    QString ini = get_wms_xml_name(rovno);

    m_layers.clear();

    QString url = QString("%1?request=GetCapabilities").arg(rovno);
    setView(this);
    download_manager2.doDownload(QUrl::fromEncoded(url.toLocal8Bit()), fun_read_ini3, fun_read_ini3_error);
}


