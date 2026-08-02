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

static double wx1 = 0, wx2 = 0, wy1 = 0, wy2 = 0;
extern CString m_pathMap;

static CString m_wms = "";

static int _width = 256;
//static double MIN_LEN = 196.;

static double MIN_LEN = 128.;

//#define  MIN_LEN 196.


CFPoint get_WMS_XY()
{
  return CFPoint((wx1+wx2)/2*10000000., -lat2y((wy1+wy2)/2)*10000000.);
}


bool isJpeg(int typ) 
{
//  return typ == ID_YANDEX_SAT || typ == ID_GOOGLE_SAT;
  return typ == ID_YANDEX_SAT || typ == ID_GOOGLE_SAT || typ == ID_GOOGLE_HYBRID || typ ==  ID_WMS_MAP;
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

  return false;
}



//
// http://habrahabr.ru/qa/2612/
// http://www.geofaq.ru/forum/index.php?action=vthread&forum=2&topic=7&page=5#msg1152
// Вобщем долго разбираясь в исходниках скриптов яндекса, читая этот форум написал таки адекватные функции для работы с привязкой яндекс-карт. Функции на 99% взяты из скрипта яндекса, поэтому гарантированно рабочие. Итак:

double boundaryRestrict(double f, double e, double d) {
  return max(min(f, d), e);
}


CFPoint geoToMercator(CFPoint g) 
{
  double d = g.x * M_PI / 180, m = g.y * M_PI / 180, l = 6378137, k = 0.0818191908426, f = k * sin(m);
  double h = tan(M_PI / 4 + m / 2), j = pow(tan(M_PI / 4 + asin(f) / 2), k), i = h / j;
  // return new CFPoint(floor(l * d), floor(l * log(i)));
  return CFPoint(l * d, l * log(i));
}

CFPoint mercatorToGeo(CFPoint e) {
  double j = M_PI, f = j / 2, i = 6378137, n = 0.003356551468879694, k = 0.00000657187271079536, h = 1.764564338702e-8, m = 5.328478445e-11;
  double g = f - 2 * atan(1 / exp(e.y / i));
  double l = g + n * sin(2 * g) + k * sin(4 * g) + h * sin(6 * g) + m * sin(8 * g);
  double d = e.x / i;
  return CFPoint(d * 180 / M_PI, l * 180 / M_PI);
}

CFPoint mercatorToTiles(CFPoint e) {
  double d = floor((20037508.342789 + e.x) * 53.5865938), f = floor((20037508.342789 - e.y) * 53.5865938);
  d = boundaryRestrict(d, 0, 2147483647);
  f = boundaryRestrict(f, 0, 2147483647);
  return CFPoint(d, f);
}


CFPoint tileToMercator(CPoint d) {
  return CFPoint(floor(d.x / 53.5865938 - 20037508.342789), floor(20037508.342789 - d.y / 53.5865938));
}

int toScale(int i) {
  return 23 - i;
}


CFPoint tileCoordinatesToPixels(CFPoint i, int h) {
  double g = pow(2, toScale(h));
  return CFPoint((int) i.x / g, (int) i.y / g);
}



CPoint getTile(CFPoint h, int i) {
  long e = 8;
  long j = toScale(i), g = (long) h.x >> j, f = (long) h.y >> j;
  return CPoint(g >> e, f >> e);
}

CPoint getPxCoordFromTileCoord(CFPoint h, int i) {
  long j = toScale(i), g = (long) h.x >> j, f = (long) h.y >> j;
  return CPoint(g, f);
}

CPoint getTileCoordFromPixCoord(CPoint h, int i) {
  long j = toScale(i), g = h.x << j, f = h.y << j;
  return CPoint(g, f);
}



void log(const char *podp, const char *s);


//static TCHAR BASED_CODE szSection[] = _T("Settings");


//#define COEF 100000
#define COEF 10000000

static int typ = ID_NOMAP;
static double map_mas = 1;
static CSemaphore c_s ;
static int reread = FALSE;
static int dmas = 0;

//extern CMapsThread *m_pMapsThread = 0;



unsigned int nazv(double x, int n)
{
  unsigned int xx;
  x = (x+180.)/180;

  int j = 0;

  xx = 0;

  for (int i = 0; i < n-1; i++) {
    xx *= 2;
    xx += (x > 1);
    x *= 2; if (x > 2.) x -= 2.;
  }
  return xx;
}

double coord(unsigned int xx, int n)
{
  return -180.+(360.*xx)/pow(2., n);
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

CString rovno = "http://62.80.170.250:6080/arcgis/services/Rovno/Rovno/MapServer/WmsServer";
CString kiev = "http://62.80.170.250:6080/arcgis/services/Kiev/Borispol/MapServer/WmsServer";


void conv(unsigned int xx, unsigned int yy, char *s, int n)
{
  unsigned int x = 0x80000 >> (4+17-n);
  char c;

  s[0] = 't';

  int j = 0;

  for (j = 1; x; x >>= 1) {
         if ( (xx&x) == 0 && (yy&x)) c = 'q';
    else if ( (xx&x) && (yy&x)) c = 'r';
    else if ( (xx&x) == 0 && (yy&x) == 0 ) c = 't';
    else if ( (xx&x) && (yy&x) == 0) c = 's';

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

  switch (typ) {
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
        int yy2 = (1 << nn)-1;
        str.Format("http://tms%d.visicom.ua/2.0.0/planet3/base/%d/%d/%d.png", ii+1, nn, xx, yy2-yy); 
      }
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


        x1 = (xx*360./n2)-180;
        y1 = ((yy+1)*360./n2)-180;

        y1 = -y1;

        double dx = 360./n2;

        x2 = x1 + dx;
        y2 = y1 + dx;

        y1 = y2lat(y1);
        y2 = y2lat(y2);


//        m2d(xx2*1e6, yy2*1e6, x2, y2);

//        CFPoint p = mercatorToGeo(CFPoint(xx1, yy1));

        if (x2 < wx1 || x1 > wx2 || y2 < wy1 || y1 > wy2) {
          str = "";
        }
        else {
          str.Format("%s?request=GetMap&service=WMS&version=1.3.0&layers=1&styles=default&crs=EPSG:4326&bbox=%lf,%lf,%lf,%lf&width=%d&height=%d&format=image/%s", rovno, y1, x1, y2, x2, _width, _width, isJpeg(typ) ? "jpeg" : "png");

//          CString layer = "cite%3Azdaniya";
          CString layer = "cite%3AastanaG";

          str.Format("%s?request=GetMap&service=WMS&version=1.3.0&layers=%s&crs=EPSG:4326&bbox=%lf,%lf,%lf,%lf&width=%d&height=%d&format=image/%s", rovno, layer, y1, x1, y2, x2, _width, _width, isJpeg(typ) ? "jpeg" : "png");
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
      str.Format("http://vec0%d.maps.yandex.net/tiles?l=map&v=2.4.2&hl=ru&x=%d&y=%d&z=%d", ii+1, xx, yy, nn); 
      str.Format("http://vec0%d.maps.yandex.net/tiles?l=map&x=%d&y=%d&z=%d", ii+1, xx, yy, nn); 
      break;

    case ID_YANDEX_SAT:
      str.Format("http://sat0%d.maps.yandex.net/tiles?l=sat&v=1.21.0&hl=ru&x=%d&y=%d&z=%d", ii+1, xx, yy, nn); 
      str.Format("http://sat0%d.maps.yandex.net/tiles?l=sat&x=%d&y=%d&z=%d", ii+1, xx, yy, nn); 
      break;

    case ID_YANDEX_PMAP:
      str.Format("http://0%d.pvec.maps.yandex.net/tiles?l=pmap&x=%d&y=%d&z=%d", ii+1, xx, yy, nn); 
      break;

    case ID_OPENSTREETMAP:
      str.Format("http://%c.tile.openstreetmap.org/%d/%d/%d.png", abcd[ii], xx, yy, nn);
      str.Format("http://tah.openstreetmap.org/Tiles/tile/%d/%d/%d.png", nn, xx, yy);
      str.Format("http://%c.tile.openstreetmap.org/%d/%d/%d.png", abcd[ii], nn, xx, yy);

//      str.Format("http://tile3.opencyclemap.org/landscape/%d/%d/%d.png", /*abcd[ii], */ nn, xx, yy);


      break;
  }

  ii = (ii+1)%3;

  return str;
};


CString get_name(int nn, int xx, int yy) 
{
  CString str;
  if (isJpeg(typ)) {
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

  switch (typ) {
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
    case ID_WMS_MAP:
      path0 = "WMS_MAP\\";
      path0 += m_wms;

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


CString fn(const char *ffn) 
{
  CString str;
  CString path = get_path() ;

  str.Format("%s%s", path, ffn);
  return str;
}

struct DOWNL {
  DOWNL(const char *_fn, const char *_url, unsigned long _xx, unsigned long _yy, int _nn) : 
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
void drawJPG(CDC *dc, CRect &rect, const char *fn, int tr, int bk_color);
void drawPNG(CDC *dc, CRect &rect, const char *fn, int tr, int m_bk_color);

void drawPic(CDC *dc, CRect &rect, const char *fn, int tr, int bk_color)
{
  if (isJpeg(typ)) {
    drawJPG(dc, rect, fn, tr, bk_color);
  }
  else {
    drawPNG(dc, rect, fn, tr, bk_color);
  }
}


void mkdir_for_file(const char *fn)
{
  char s[256];

  for (int i = strlen(fn); i >= 0; i--) {
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


bool get_map(int nn, int xx, int yy, CString & fn, bool is_read = false, bool is_stack = false)
{
  CString path, name, url, tmpn;

  name = get_name(nn, xx, yy);
  fn = get_path()+name;
  url = get_url(nn, xx, yy);

  if (url == "") return false;

  if (is_read) {
    tmpn.Format("%s\\tempfileformap.tmp", getenv("TEMP"));

    int ret = URLDownloadToFile(0, url, tmpn, 0, 0);
    if (ret == S_OK) {
      if (IsFile(tmpn)) {
        mkdir_for_file(fn);
//        MoveFileEx(tmpn, fn, MOVEFILE_REPLACE_EXISTING);
  
        ::CopyFile(tmpn, fn, false);
        
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

double lat2y(double y)
{
  double lat = y/180.*M_PI;
  return log(tan(M_PI/4+lat/2))*180/M_PI;
}

double y2lat(double B)
{
  double lat = B/180.*M_PI;
  return (atan(exp(lat))*360/M_PI - 90);
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
  p_bminfo->biSizeImage = w*h;
  
  p_bminfo->biSizeImage = size;
  p_bminfo->biXPelsPerMeter = 0;
  p_bminfo->biYPelsPerMeter = 0;
  p_bminfo->biClrUsed = 0;
  p_bminfo->biClrImportant = 0;

//  p_bminfo->biCompression = BI_RGB;
//  p_bminfo->biSizeImage = 0;
//  p_bminfo->biBitCount = 8;
}


void ErrorMsg(const char *s, int code);
int jpeg_decode(const char *filename, char *buf);
int jpeg_decode_tr(const char *filename, char *buf, int tr, int m_bk_color);

int get_tr();
int get_bk_color() ;


int reget (const char *pszUrl, const char *fn, DWORD off, DWORD size);

static int m_nTransp;
static int m_bk_color;


void drawJPG(CDC *dc, CRect &rect, const char *fn, int tr, int bk_color)
{
  HANDLE hndl;
  LPVOID buf;

  hndl = ::GlobalAlloc(GMEM_MOVEABLE, _width*_width*3);
  buf = (LPBITMAPINFO)::GlobalLock(hndl);

  BITMAPINFO BitsInfo;
  memset(&BitsInfo, 0, sizeof(BitsInfo));
  SetHeaderMap(&BitsInfo.bmiHeader, _width, _width, 0);

  dc->SetStretchBltMode(STRETCH_DELETESCANS);

  jpeg_decode_tr(fn, (char *) buf, tr, bk_color);

  int qq = StretchDIBits(
    dc->m_hDC, rect.left, rect.top, 
    rect.right-rect.left+1, rect.bottom-rect.top,
    0, 0, _width, _width, 
    buf, &BitsInfo, DIB_RGB_COLORS, SRCCOPY);

  GlobalUnlock(hndl);
  GlobalFree(hndl);
}


#define PNG_NO_EXTERN 1

#include "png/png.h"


BOOL LoadPngFile (PTSTR pstrFileName, png_byte **ppbImageData,
                   int *piWidth, int *piHeight, int *piChannels, png_color *pBkgColor);


int rgb_l(int color, int tr, int bk_color);


void drawPNG(CDC *dc, CRect &rect, const char *fn, int tr, int m_bk_color)
{
//  HANDLE hndl;
//  LPVOID buf;

  FILE *f;

//  f = fopen("C:/Documents and Settings/User/Application Data/Sirius/tgid/map/OPENSTREETMAP/15/22876/12051.png" , "rb");
  f = fopen(fn, "rb");

  if (!f) return;

  BITMAPINFO BitsInfo;
  memset(&BitsInfo, 0, sizeof(BitsInfo));
  SetHeaderMap(&BitsInfo.bmiHeader, _width, _width, 0);
//  SetPalette((RGBQUAD*) ((LPSTR)buf+sizeof(BITMAPINFOHEADER)), _width);

  dc->SetStretchBltMode(STRETCH_DELETESCANS);

// проверяем сигнатуру файла (первые number байт)
  
  const int number = 8;

  png_byte sig[number] = {0};

  fread(sig, sizeof(png_byte), number, f);
  
  if ( !png_check_sig(sig, number) ) { 
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

  png_set_expand(png_ptr);
//  png_set_gray_to_rgb(png_ptr);


//  hndl = ::GlobalAlloc(GMEM_MOVEABLE, _width*_width*3);
//  buf = (LPBITMAPINFO)::GlobalLock(hndl);

  unsigned char *buf;

  buf = new unsigned char[_width*_width*3*2];
  unsigned char **b;

  b = new unsigned char*[_width];

  int i;

  for (i = 0; i < _width; i++) {
//    b[i] = &((unsigned char*)buf)[(255-i)*_width];
    b[i] = &((unsigned char*)buf)[i*_width*3];
  }

  png_read_image(png_ptr, b);

  fclose(f);

  char *cbuf = (char *)buf;

  for (i = 0; i < _width*_width; i++) {
    int r, g, b;
    r = cbuf[i*3];
    g = cbuf[i*3+1];
    b = cbuf[i*3+2];

      unsigned char im1[4];
    
      im1[0] = r;
      im1[1] = g;
      im1[2] = b;
      
      long *ii = (long*) &im1;
      ii[0] = rgb_l(ii[0], tr, m_bk_color);

      r = im1[0];
      g = im1[1];
      b = im1[2];

    cbuf[i*3+2] = r;
    cbuf[i*3+1] = g;
    cbuf[i*3] = b;
  }


  int qq = StretchDIBits(
    dc->m_hDC, rect.left, rect.top, 
    rect.right-rect.left+1, rect.bottom-rect.top,
    0, 0, _width, _width, 
    buf, &BitsInfo, DIB_RGB_COLORS, SRCCOPY);

  delete [] b;
  delete [] buf;

//  GlobalUnlock(hndl);
//  GlobalFree(hndl);
}

void drawMap(CGidrView *view, CDC *dc, CRect &m_rect, int nn, bool is_stack)
{
  CString str;
  CFRect rectf = view->ScreenToCoord(m_rect);

  long xx1, xx2, yy1, yy2, xx, yy;

  xx1 = nazv(rectf.left, nn);
  xx2 = nazv(rectf.right, nn);
  yy2 = nazv(-rectf.top, nn);
  yy1 = nazv(-rectf.bottom, nn);

  double n2 = pow(2.,nn);

  xx1 = (rectf.left+180)*n2/360;
  xx2 = (rectf.right+180)*n2/360;

  yy1 = (180+rectf.top)*n2/360.;
  yy2 = (180+rectf.bottom)*n2/360.;

  xx2 = min(xx2, (1 << nn)-1 );
  yy2 = min(yy2, (1 << nn)-1 );

  xx1 = max(xx1, 0);
  yy1 = max(yy1, 0);


  view->setcolor(dc, 0xFFFF00);

  double dh = 720./pow(2., nn+1);

//  if (yy2-yy1 < 50 && xx2 - xx1 < 50)
  for (yy = yy1; yy <= yy2; yy++ ) {
    for (xx = xx1; xx <= xx2; xx++ ) {

      double fx = coord(xx, nn);
      double fy = coord(yy, nn);
      CFRect rr = CFRect(fx, fy+dh, fx+dh, fy);
      CRect rect = view->CoordToScreen(rr);

      CString tmpn;
      if (get_map(nn, xx, yy, tmpn, false, is_stack)) {
//      if (get_map(nn, xx, yy, tmpn, true, is_stack)) {
        drawPic(dc, rect, tmpn, view->m_nTransp, view->m_bk_color);

        bool m_isSetka = false;
        if (m_isSetka) view->rectangle(dc, rect.left, rect.top, rect.right, rect.bottom);
      }
    }
  }
}

#import <msxml3.dll> named_guids
using namespace MSXML2;

static int init_wms = 0;

void setInit()
{
  init_wms = 0;
}

CString bstr2a(_bstr_t b);


bool get_WMS()
{
  if (init_wms) return false;
  
  MSXML2::IXMLDOMDocumentPtr m_plDomDocument;


  HRESULT hr = m_plDomDocument.CreateInstance(MSXML2::CLSID_DOMDocument);
  if (FAILED(hr))
  {
     _com_error er(hr);
     return false;
  }


  CString tmpn, url;
  init_wms = 1;


  CString r = rovno; // = "http://62.80.170.250:6080/arcgis/services/Rovno/Rovno/MapServer/WmsServer";

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


  typ = ID_WMS_MAP;

  CString ini = get_path()+r+".xml";


  if (!IsFile(ini))  {
    url.Format("%s?SERVICE=WMS&VERSION=1.3.0&REQUEST=GetCapabilities", rovno);
    tmpn.Format("%s\\tempfileformap.tmp", getenv("TEMP"));
    int ret = URLDownloadToFile(0, url, tmpn, 0, 0);
    if (ret != S_OK) return false;

//    MoveFileEx(tmpn, ini, MOVEFILE_REPLACE_EXISTING);
    ::CopyFile(tmpn, ini, false);
  }

  if (!IsFile(ini)) return false;


  _bstr_t bstrFileName;
  bstrFileName = ini;


  variant_t vResult;
  vResult = m_plDomDocument->load(bstrFileName);

  
  if (((bool)vResult) == TRUE) // success!
  {
  MSXML2::IXMLDOMNodeListPtr list = m_plDomDocument->getElementsByTagName("EX_GeographicBoundingBox");

    long l;
    list->get_length(&l);

  MSXML2::IXMLDOMNode *listItem;

    hr = list->get_item(1L, &listItem);

//    listItem = list->Getitem(0);

  MSXML2::IXMLDOMNodePtr pChild = listItem->firstChild;
    double x[10];

    for (int i = 0; NULL != pChild && i < 5;  pChild = pChild->nextSibling, i++) {
      BSTR bstr;
      pChild->get_text(&bstr);
      CString str = bstr2a(bstr);

      x[i] = atof(str);
    }

    wx1 = x[0], wx2 = x[1], wy1 = x[2], wy2 = x[3];
    return true;

  }

  return false;


}


//void CMapsView::redrawPaint()
void redrawMap(CMapsThread *m_pMapsThread, CGidrView *view, CDC *dc, CRect &m_rect, int typ_map, int is_reread)
{
  typ = typ_map;
  reread = is_reread;
  
  if (!view || !::IsWindow(view->m_hWnd)) return;

  if (typ_map == ID_NOMAP) return;
  if (!isMap(typ_map)) return;

  if (view->m_parent_id != 0) return;

  double mas = view->masx;
  
  view->masx /= COEF;
  view->masy /= COEF;

  CString str;

  CClientDC screenDC(NULL);
  dmas = screenDC.GetDeviceCaps(LOGPIXELSX)/2.54;
//  dmas = view->dmas;

//  double nn1 = log(360./20000/view->masx)/log(2.);
  double nn = log(360.*dmas/MIN_LEN/(view->masx*view->dmas))/log(2.);

//  str.Format("%g", nn);
//  Status(2, str);

  char buf[256];
  nn = atof(_gcvt( nn, 7, buf));

//  nn1 = max(1., nn1);
  nn = max(1., nn);

  if (typ_map == ID_WMS_MAP) {
    _width = 512;
    MIN_LEN = 128;

    _width = 512;
    MIN_LEN = 64;


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
  else {
    nn = min(nn, 18);
  }




  if (typ_map == ID_YANDEX_MAP || typ_map == ID_YANDEX_SAT) {
    nn = min(nn, 17);
  }


  if (map_mas != view->masx*view->dmas) {

     CSingleLock lock(&c_s);
     lock.Lock(1000);

     if (lock.IsLocked()) {
       stack_download.clear();
       lock.Unlock();
     }
  }

  map_mas = view->masx*view->dmas;

  if (typ_map == ID_WMS_MAP && !init_wms) {
    get_WMS();
    init_wms = 1;
  }


  drawMap(view, dc, m_rect, nn, true);

//  for (int i = nn-1; i > 8; i--) drawMap(view, dc, m_rect, i, true);


  view->masx = view->masy = mas;
  if (m_pMapsThread) m_pMapsThread->Tic();
  reread = FALSE;
}


IMPLEMENT_DYNCREATE(CMapsThread, CWinThread)


CMapsThread::CMapsThread()
{
  m_isExit = 0;
  m_isOff = 0;
  threadno = 0;
}


CMapsThread::CMapsThread(void *t)
{
  m_isExit = 0;
  m_isOff = 0;
  threadno = 0;

  threadno = t;
}


CMapsThread::~CMapsThread()
{
}


BOOL CMapsThread::InitInstance()
{

  while (TRUE) {
    if (m_isExit) break;

    ::WaitForSingleObject( m_event.m_hObject, INFINITE );

    m_event.ResetEvent();

    if (m_isExit) break;

    CGidrView *view = (CGidrView *) threadno;
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

    while (!m_isExit) {
      int stack_size = 0;

      DOWNL downl;

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
      if(!(bIsConnection && (dwConnectionState & INTERNET_CONNECTION_LAN || 
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

        double mas = view->masx/COEF;
    
        mas = map_mas;

        double nn0 = log(360.*dmas/MIN_LEN/mas)/log(2.);

        if (nn <= nn0) {
          double dh = 720/pow(2., nn+1);
          double fx = coord(downl.xx, nn);
          double fy = coord(downl.yy, nn);
          CFRect rr = CFRect(fx, fy+dh, fx+dh, fy);
          rr.left*=COEF; rr.right*=COEF; rr.top*=COEF; rr.bottom*=COEF;

          int isInternet = 1;

//          if (isInternet && isMap(typ_map)) {
          if (isInternet) {
#if 1
            ::Status(1, str);
#endif
            if (view->m_parent_id == 0) {

            if (get_map(nn, downl.xx, downl.yy, tmpn, true)) {
              CRect rect = view->CoordToScreen(rr);

              CClientDC dc(view);
              drawPic(&dc, rect, fn, view->m_nTransp, view->m_bk_color);
//              view->rectangle(&dc, rect.left, rect.top, rect.right, rect.bottom);
            }
#if 1
           ::Status(1, str + " +");
#endif
            }
          }
        }
      }
    }
  }

  m_isOff = 1;

  return ExitInstance();
}

int CMapsThread::ExitInstance()
{

  ExitThread(0);

  return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CMapsThread, CWinThread)
  //{{AFX_MSG_MAP(CMapsThread)
    // NOTE - the ClassWizard will add and remove mapping macros here.
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CMapsThread::Tic()
{
  m_event.SetEvent();
}

void CMapsThread::Exit(CEvent *event2) 
{
  m_isExit = 1; 
  Tic();
//  m_event2 = event2;
}

BOOL CMapsThread::isOff() 
{ 
  return m_isOff; 
}
