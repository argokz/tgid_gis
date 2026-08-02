#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"
#include "mmenu.h"
#include "win.h"
#include "excel.h"
#include "ps_alma.h"
#include "stdio.h"

#if 0

int excel(const char *path);


COleVariant getGal()
{
  VARIANT parm;

  wchar_t ss[2];
  ss[0] = 0x2713;
  ss[1] = 0;

  parm.vt = VT_BSTR;
  parm.bstrVal = ::SysAllocString(ss);

  //COleVariant v(parm);

  return parm;
}


//bool isTableNoEmpty(CDbFile &dbf, const char *q);
CString getTyp2(const char *name);

//void read_pav(CDbFile *dbf, const char *q, map<CString, CString> &map_val);
CString get_V(map<CString, CString>& map_RE, CString id);
int jpeg_decode(const char *filename, char *buf);
int jpeg_decode_tr(const char *filename, char *buf, int tr, int m_bk_color);


int test_one_file2(const char *inname);
int png_header(const char *inname, int *nWidth, int *nHeight);
int png_decode8(const char *filename, char *buf);


int jpeg_header(const char *filename, int *width, int *height);

void SetHeaderMap(LPBITMAPINFOHEADER p_bminfo, int w, int h, int size);

void pic_obr(char *buf, int width, int height)
{
  int i, k;

  for (i = 0; i < height/2; i++) {
    for (k = 0; k < width; k++) {

      int c = buf[i*width+k];
      buf[i*width+k] = buf[(height-i-1)*width+k];;
      buf[(height-i-1)*width+k] = c;
    }
  }
}

int jpeg_decode8(const char *filename, char *buf);


//int extern "C" test_one_file2(const char *inname);
//int extern "C" png_header(const char *inname, int *nWidth, int *nHeight);
//int extern "C" png_decode8(const char *filename, char *buf);


bool CopyPng(const char *fn);


bool CopyJpg(const char *fn)
{
  CString ext = GetExt(fn);

  CString exl = ext;
  exl.MakeLower();

  if (exl == "png") {
    return CopyPng(fn);
  }
  
  int width, height;

  if (!jpeg_header(fn, &width, &height)) return false;

  int iSize = sizeof(BITMAPINFOHEADER) ;

  width = (width+7)/8*8;

  BITMAPINFOHEADER header;
  header.biWidth = width;
  header.biHeight = -height;
  header.biSizeImage = 0; //3 * width * height;
  header.biSize = sizeof(BITMAPINFOHEADER);
  header.biPlanes = 1;
  header.biBitCount =  3 * 8;         // RGB
  header.biCompression = BI_RGB;
  header.biXPelsPerMeter = 0;
  header.biYPelsPerMeter = 0;
  header.biClrUsed = 0;
  header.biClrImportant = 0;

  if (OpenClipboard(AfxGetMainWnd()->m_hWnd)) {
    EmptyClipboard();
  
    // Generate handle
    HANDLE handle = (HANDLE)::GlobalAlloc (GHND, sizeof(BITMAPINFOHEADER) + 3 * width * height*2);
    if (handle != NULL)
    {
      // Lock handle
      char *pData = (char *)::GlobalLock((HGLOBAL)handle);
      // Copy header and data
      memcpy(pData, &header, sizeof(BITMAPINFOHEADER));
  //    memcpy(pData + sizeof(BITMAPINFOHEADER), m_pPixelData, 3 * width * height);
  //    jpeg_decode_tr(fn, pData + sizeof(BITMAPINFOHEADER), 255, 0xFFFFFF);
      jpeg_decode8(fn, pData + sizeof(BITMAPINFOHEADER));

  //    pic_obr((char *) pData + sizeof(BITMAPINFOHEADER), width*3, height);

    ::GlobalUnlock((HGLOBAL)handle);
      
      HANDLE h = SetClipboardData(CF_DIB, handle);

      if (h == NULL) {
        DWORD dwError = GetLastError();

        char * lpMsgBuf;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf,  0, NULL );
        AfxMessageBox(lpMsgBuf, MB_OK|MB_ICONINFORMATION);
        LocalFree( lpMsgBuf );
      }
      GlobalFree(handle);
    }
    CloseClipboard();
    return true;
  }
  return false;
}






















bool CopyPng(const char *fn)
{
  int width, height;

  if (!png_header(fn, &width, &height)) return false;


  int iSize = sizeof(BITMAPINFOHEADER) ;

  width = (width+7)/8*8;

  BITMAPINFOHEADER header;
  header.biWidth = width;
  header.biHeight = -height;
  header.biSizeImage = 0; //3 * width * height;
  header.biSize = sizeof(BITMAPINFOHEADER);
  header.biPlanes = 1;
  header.biBitCount =  3 * 8;         // RGB
  header.biCompression = BI_RGB;
  header.biXPelsPerMeter = 0;
  header.biYPelsPerMeter = 0;
  header.biClrUsed = 0;
  header.biClrImportant = 0;

  if (OpenClipboard(AfxGetMainWnd()->m_hWnd)) {
    EmptyClipboard();
  
    // Generate handle
    HANDLE handle = (HANDLE)::GlobalAlloc (GHND, sizeof(BITMAPINFOHEADER) + 3 * width * height*2);
    if (handle != NULL)
    {
      // Lock handle
      char *pData = (char *)::GlobalLock((HGLOBAL)handle);
      // Copy header and data
      memcpy(pData, &header, sizeof(BITMAPINFOHEADER));
  //    memcpy(pData + sizeof(BITMAPINFOHEADER), m_pPixelData, 3 * width * height);
  //    jpeg_decode_tr(fn, pData + sizeof(BITMAPINFOHEADER), 255, 0xFFFFFF);
      png_decode8(fn, pData + sizeof(BITMAPINFOHEADER));

  //    pic_obr((char *) pData + sizeof(BITMAPINFOHEADER), width*3, height);

    ::GlobalUnlock((HGLOBAL)handle);
      
      HANDLE h = SetClipboardData(CF_DIB, handle);

      if (h == NULL) {
        DWORD dwError = GetLastError();

        char * lpMsgBuf;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf,  0, NULL );
        AfxMessageBox(lpMsgBuf, MB_OK|MB_ICONINFORMATION);
        LocalFree( lpMsgBuf );
      }
      GlobalFree(handle);
    }
    CloseClipboard();
    return true;
  }
  return false;
}




#define BI_JPEG 4
#define BI_PNG 5


bool CopyJpg2(const char *fn)
{
  int width, height;
  if (!jpeg_header(fn, &width, &height)) return false;
  int iSize = sizeof(BITMAPINFOHEADER) ;

  fpos_t lengthActual = 0, length = 0;

  FILE *f = fopen(fn, "rb");

  if (!f) return false;

  // Retrive file size
  fseek(f, 0, SEEK_END);
  fgetpos(f, &lengthActual);
  fseek(f, 0, SEEK_SET);

  BITMAPINFOHEADER header;
  header.biSize = sizeof(BITMAPINFOHEADER);
  header.biWidth = width;
  header.biHeight = height;
  header.biPlanes = 1;
  header.biBitCount = 0;         // RGB
  header.biCompression = BI_JPEG;
  header.biSizeImage = lengthActual;
  header.biXPelsPerMeter = 0;
  header.biYPelsPerMeter = 0;
  header.biClrUsed = 0;
  header.biClrImportant = 0;

  if (OpenClipboard(AfxGetMainWnd()->m_hWnd)) {
    if (EmptyClipboard()) {
      // Generate handle
      HANDLE handle = (HANDLE)::GlobalAlloc (GHND, sizeof(BITMAPINFOHEADER) + lengthActual);
      if (handle != NULL)
      {
        char *pData = (char *)::GlobalLock((HGLOBAL)handle);

        memcpy(pData, &header, sizeof(BITMAPINFOHEADER));
        length = fread(pData + sizeof(BITMAPINFOHEADER), 1, lengthActual, f);

        ::GlobalUnlock((HGLOBAL)handle);
        HANDLE h = SetClipboardData(CF_DIB, handle);

        if (h == NULL) {
          DWORD dwError = GetLastError();

          char * lpMsgBuf;
          FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf,  0, NULL );
          AfxMessageBox(lpMsgBuf, MB_OK|MB_ICONINFORMATION);
          LocalFree( lpMsgBuf );
        }
      }
      GlobalFree(handle);
    }
    CloseClipboard();
  }
  fclose (f);
  
  return true;
}


void setGal(CExcel &ex, const char *kod, const char *a, ...)
{
  va_list ls;
  va_start( ls, a );     /* Initialize variable arguments. */

  const char *a1, *a2;

  a1 = a;
  if (a1) a2 = va_arg( ls, const char *);;

  while (a1) {
    if (!strcmp(a1, kod)) {
      ex.set_var(a2, getGal());
      break;
    }
    a1 = va_arg( ls, const char *);
    if (a1) a2 = va_arg( ls, const char *);
  }
  
  va_end(ls); 
}



void CGidrView::pasport_US(CNode2 *node)
{


    map<CString, CString> map_val1;
    map<CString, CString> map_val2;


    long kk = node->id;
//    CString table = nodef[node->typ].table;
//    CString fn = node->file->name;
    CString gn = "";



    CString q, q1, q2;
    map<CString, CString> map_val;

    q1.Format("SELECT * FROM [%s]", table);
    CString pav = _TR("Тепловая камера");

    CAdoFile *ado = m_cxema->m_ado;

    if (ado->openTable(q1)) {
        if (dbf->GotoKey(kk)) {
            CString typ;

            if (!strcmp(table, _TR("Насосная станция")) || !strcmp(table, _TR("Источник тепла"))) {
                typ = table;
            }
            else {
                typ = dbf->read("name_typ");
            }

            pav = getTyp2(typ);

            if (typ == _TR("ВВ"))             pav = _TR("Ввод в здание");
            else if (typ == _TR("ИС"))             pav = _TR("Источник тепла");
            else if (typ == _TR("НУ"))             pav = _TR("Надземный узел");
            else if (typ == _TR("НС"))             pav = _TR("Насосная станция");
            //              else if (typ == _TR("Неизвестно")) pav =     _TR("Неустановленный узел")   ;
            else if (typ == _TR("ПВ"))             pav = _TR("Павильон");
            else if (typ == _TR("КУ"))             pav = _TR("Подземный узел");
            else if (typ == _TR("ТК"))             pav = _TR("Тепловая камера");
            else if (typ == _TR("РП"))             pav = _TR("ТРП");
            else if (typ == "??")             pav = _TR("Неустановленный узел");


            dbf->closeTable();
        }


        q1.Format("SELECT * FROM [%s] WHERE nomer=%d", table, kk);
        q2.Format("SELECT * FROM [%s] WHERE kod='%s' AND uzel='%s'", pav, node->node_name.kod, node->node_name.name);


        read_pav(dbf, q1, map_val1);
        read_pav(dbf, q2, map_val2);
    }



    Cgid6App* pApp = (Cgid6App*)AfxGetApp();

    CString uzel = get_V(map_val1, "uzel");
    CString kol_luk = get_V(map_val2, _TR("Количество люков"));
    CString typ_luk = get_V(map_val2, _TR("Тип люков"));
    CString ves_luk = get_V(map_val2, _TR("Вес люков"));
    CString kol_les = get_V(map_val2, _TR("Количество лесниц"));
    if (kol_les == "") kol_les = "0";

    CString sost_pl = get_V(map_val2, _TR("Состояние плит перекрытия"));
    CString sost_le = get_V(map_val2, _TR("Состояние лестниц"));
    CString priyam = get_V(map_val2, _TR("Наличие приямка"));
    CString dno = get_V(map_val2, _TR("Дно камеры"));

    CString street = get_V(map_val1, "street");
    CString dom = get_V(map_val1, "home_numer");
    CString file = get_V(map_val1, _TR("Схема"));
    CString path = pApp->GetProfileString(szSection, "PATH_CXEMA", "");  // Правильно 2017.11.07
    if (path != "" && path[path.GetLength() - 1] != '\\') path += "\\";



    CExcel ex;

    CString p = "birka.xlsx";
    CString xn1, xn2;

    xn1.Format("%sExcel\\xls\\%s", argpath(), p);
    xn2.Format("%s\\%s", getenv("TMP"), p);

    CopyFile(xn1, xn2, 0);

    if (!ex.IsOpen()) {
        ex.open(xn2);
        ex.set_str("D2", node->node_name.kod);
        ex.set_str("C3", node->node_name.name);


        int kk = 14;
        for (CLINE2 *ll = node->lines; ll; ll = next(ll), kk++) {
            if (bline(ll)->nomP != -1) {
                CString s1, s2;
                s1.Format("E%d", kk);
                s2.Format("%g", bline(ll)->pod.diam_usl);
                ex.set_str(CRange(kk, 5), s2);
            }
        }

        //    ex.set_str("B14", get_V(map_val2, _TR("Внутренняя длина камеры, мм")));


        setGal(ex, kol_luk, "1", "H5", "2", "K5", "3", "M5", "4", "P5", "6", "R5", NULL);
        setGal(ex, ves_luk, "Т", "D7", "Л", "H7", NULL);
        setGal(ex, typ_luk, "О", "K7", "К", "M7", NULL);

        ex.set_str("J21", get_V(map_val2, _TR("Внутренняя длина камеры, мм")));
        ex.set_str("M21", get_V(map_val2, _TR("Внутренняя ширина камеры, мм")));
        ex.set_str("Q21", get_V(map_val2, _TR("Внутренняя высота камеры, мм")));

        setGal(ex, kol_les, "0", "H23", "1", "K23", "2", "M23", "3", "P23", "4", "R23", NULL);
        setGal(ex, sost_pl, "А", "H26", "З", "K26", NULL);
        setGal(ex, sost_le, "З", "E28", " ", "K28", NULL);
        setGal(ex, priyam, "TRUE", "E30", "FALSE", "H30", NULL);
        setGal(ex, dno, "З", "B32", " ", "E32", NULL);

        ex.set_str("H10", get_V(map_val1, "GPS координаты"));
        ex.set_str("K3", street + " " + dom);


        int m_uchM = -1;
        int m_uchR = -1;

        for (ll = node->lines; ll; ll = next(ll)) {
            CLine *l = bline(ll);
            m_uchM = l->m_uchM;
            m_uchR = l->m_uchR;
        }

        {
            map<int, UMS>::const_iterator it = ps_alma.map_UMS.find(m_uchM);

            if (it != ps_alma.map_UMS.end()) {
                UMS rs = it->second;

                NACH *nach = get_Vmap(ps_alma.map_NACH, rs.nach);
                CString *re = get_Vmap(ps_alma.map_RE, 0 /*rs.re*/);

                CString s = "";
                s.Format("%s %s %s", *re, rs.uch, nach->name);

                ex.set_str("M2", *re);
                ex.set_str("Q2", rs.uch);
            }
        }
        {
            map<int, URS>::const_iterator it = ps_alma.map_URS.find(m_uchR);

            if (it != ps_alma.map_URS.end()) {
                URS rs = it->second;

                CString *re = get_Vmap(ps_alma.map_RE, 0 /* rs.re*/);

                ex.set_str("M2", *re);
                ex.set_str("Q2", rs.name);
            }
        }


        if (CopyJpg(path + file)) {
            ex.copy("A1", 2);
        }

        HWND wnd = ::FindWindowEx(0, 0, "XLMAIN", 0);

        if (wnd) {
            ::ShowWindow(wnd, SW_MINIMIZE);
            ::ShowWindow(wnd, SW_RESTORE);
            //        ::SetFocus(wnd);
        }
    }
}

void read_pav(CDbFile *dbf, const char *tn, int n, map<CString, CString> &map_val);
const char *get_idrp(string name);


bool read_geo(const char *tn, int n1, map<CString, CString>& map_val)
{
    if (n1 > 0) {
        CDbFile *dbf = m_geofile->m_db;
        if (dbf->openTable(tn)) {
            if (dbf->GotoKey(n1)) {
                read_pav(dbf, tn, n1, map_val);
                dbf->closeTable();
                return true;
            }
        }
    }
    return false;
}


bool read_geo(const char *tn, CFPoint p, map<CString, CString>& map_val)
{
    int n1 = m_geofile->findXY3(p, 1, 3, tn);

    if (n1 > 0) {
        return read_geo(tn, n1, map_val);
    }
    return false;
}

/*

void CGidrView::pasport_PR(CNode *node)
{
CString q, q1, q2;

map<CString, CString> map_val1;
map<CString, CString> map_val_g;

long kk = node->nomP >= 0 ? node->nomP : node->nomO;
CString table = nodef[node->typ].table;
CString fn = node->file->name;
CString gn = "";

q1.Format("SELECT * FROM [%s]", table);

read_pav(node->file->m_dbf, q1, map_val1);
read_geo(_TR("Здания Карта"), node->coord, map_val_g);

CString p = "birka2.xlsx";
CString xn1, xn2;

xn1.Format("%sExcel\\xls\\%s", argpath(), p);
xn2.Format("%s\\%s", getenv("TMP"), p);

CopyFile(xn1, xn2, 0);

const RasCx *rs = m_cxema->getPC(node->node_name.kod);
if (!rs) return;


CExcel ex;
if (!ex.IsOpen()) {
ex.open(0);
ex.set_str("A1", _TR("Паспорт"));
ex.set_typ("C1", dbText);
ex.set_str("C1", rs->kod_rs);

map<int, URS>::const_iterator it = ps_alma.map_URS.find(rs->uchastok);
if (it !=  ps_alma.map_URS.end()) {
URS urs = it->second;
CString *re = get_Vmap(ps_alma.map_RE, urs.re);
ex.set_str("E1", *re);
ex.set_str("G1", urs.name);
}

ex.set_str("A4", _TR("Адрес объекта"));
ex.set_str("E4", get_V(map_val_g, "ID_ADR_MAS")+" "+get_V(map_val_g, "STREET_NAM")+" "+get_V(map_val_g, "NUMBER_1"));

ex.set_str("A5", _TR("Назначение"));
ex.set_str("E5", get_V(map_val_g, "NAME"));

ex.set_str("A6", _TR("Этажность"));
ex.set_str("E6", get_V(map_val_g, "FLOOR"));

ex.set_str("A7", _TR("Материал"));
ex.set_str("E7", get_V(map_val_g, "MATERIAL"));

ex.set_str("A8", _TR("Год постройки"));
ex.set_str("E8", get_V(map_val_g, "YEAR_OF_FO"));

ex.set_str("A9", _TR("Общая площадь"));
ex.set_str("E9", get_V(map_val_g, "OBSCH_AREA"));

ex.set_str("A10", _TR("Принадлежность"));
ex.set_str("E10", get_V(map_val_g, "KSK"));



double gv_pr = atof(get_V(map_val1, "gvpr"));
double gv_sm = atof(get_V(map_val1, "gvsm"));
double gv_ps = atof(get_V(map_val1, "gvps"));
double gv_pw = atof(get_V(map_val1, "gvpw"));
double gv_op = atof(get_V(map_val1, "gvop"));
double gv_oo = atof(get_V(map_val1, "gvoo"));


ex.set_str("A11", _TR("Система ГВС"));

if (gv_pr+gv_sm+gv_ps+gv_pw+gv_op+gv_oo == 0) {
ex.set_str("E11", _TR("Отсутствует"));
}
if (gv_op+gv_oo > 0) {
ex.set_str("E11", _TR("Открытая"));
}
if (gv_pr+gv_sm+gv_ps+gv_pw > 0) {
ex.set_str("G11", _TR("Закрытая"));
}

double otoplz = atof(get_V(map_val1, "otoplz"));
double otopln = atof(get_V(map_val1, "otopln"));

ex.set_str("A12", _TR("Система отопления"));
if (otoplz+otopln == 0) {
ex.set_str("E12", _TR("Отсутствует"));
}
if (otoplz > 0) {
ex.set_str("E12", _TR("Зависимая"));
}
if (gv_pr+gv_sm+gv_ps+gv_pw > 0) {
ex.set_str("E12", _TR("Независимая"));
}

ex.set_str("A13", _TR("Система присоединения"));

CString cxema = get_V(map_val1, "cxema");

extern int nn_a14, nn_otopln;
const char *buf = get_idrp(string(cxema));

if (buf && nn_a14 >= 0 && (bool)buf[nn_a14]) {
ex.set_str("E13", _TR("Элеватор"));
}
}
}
*/

void CGidrView::pasport_VV(CFPoint pt)
{
    CString zd = _TR("Здания Карта");
    Klassif *kls = m_geofile->m_kl_list->findKlN(transl(zd));

    if (!kls) {
        AfxMessageBox(_TR("Нет зданий"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    CCoordList cl;

    map<CString, CString> map_val1;
    map<CString, CString> map_val_vv;
    map<CString, CString> map_val_zd;

    read_geo(zd, pt, map_val_zd);

    double md = D5 * masx;

    //  int nVV = m_geofile->findXY3(pt, md, 1, vv);

    vector <CNode *> v_node;

    //  if (nVV) {
    //    read_geo(vv, nVV, map_val_vv);

    int nZD = m_geofile->findXY3(pt, 1, 3, zd);
    if (!nZD) {
        nZD = m_geofile->findXY3(pt, md, 2, zd);
    }

    if (!nZD) {
        return;
    }


    if (nZD) {
        m_geofile->m_kls = kls;

        int nom = kls->GetNom2(nZD);
        if (m_geofile->GetCoord(nom, cl)) {
            CFRect rect = cl.GetFRect();
            CMapGraph::iterator p = m_cxema->m_graph->map_node.begin();

            while (p != m_cxema->m_graph->map_node.end()) {
                CNode *node = &(*p);

                if (node->typ == TIP_PR && rect.PtInRect(node->coord)) {
                    if (cl.find3(node->coord)) {
                        v_node.push_back(node);
                    }
                }
                p++;
            }
        }
    }



    CNode *node = NULL;

    if (v_node.size() > 0) {
        node = v_node[0];
    }



    //AfxMessageBox(_TR("Еще не готово"), MB_OK|MB_ICONINFORMATION);
    //  pasport_VV(pt);

    const RasCx *rs = NULL;
    CString q, q1, q2;

    CNode *nodeR = 0, *nodeM = 0;

    if (node) {

        CNode *node1 = node;
        while (node1) {
            CNode *nn = node1;
            node1 = NULL;

            for (CLINE2 *l = nn->lines; l; l = next(l)) {
                CLine *ll = bline(l);
                double naprP = ll->pod.q*napr(l);

                if (ll->nomP != -1 && naprP < 0) {
                    node1 = other(l);
                    if (node1->name_typ_alma == TIP2_PV || node->name_typ_alma == TIP2_TK) if (!nodeR) nodeR = node1;
                    nodeM = node1;
                    break;
                }
            }
        }



        long kk = node->nomP >= 0 ? node->nomP : node->nomO;
        CString table = nodef[node->typ].table;
        CString fn = node->file->name;
        CString gn = "";

        q1.Format("SELECT * FROM [%s]", table);

        read_pav(node->file->m_dbf, q1, map_val1);
        rs = m_cxema->getPC(node->node_name.kod);
    }


    CString p = "birka2.xlsx";
    CString xn1, xn2;

    xn1.Format("%sExcel\\xls\\%s", argpath(), p);
    xn2.Format("%s\\%s", getenv("TMP"), p);

    CopyFile(xn1, xn2, 0);

    CExcel ex;
    if (!ex.IsOpen()) {
        ex.open(xn2);

        //ex.set_str("A1", _TR("Паспорт"));
        ex.set_typ("C1", dbText);
        if (rs) {
            //ex.set_str("C1", rs->kod_rs);
            map<int, URS>::const_iterator it = ps_alma.map_URS.find(rs->uchastok);
            if (it != ps_alma.map_URS.end()) {
                URS urs = it->second;
                CString *re = get_Vmap(ps_alma.map_RE, 0 /* urs.re*/);
                ex.set_str("F1", *re);
                ex.set_str("J1", urs.name);
            }
        }

        if (nodeM) {
            ex.set_str("E3", nodeM->node_name.name);
        }
        if (nodeR) {
            ex.set_str("H3", nodeR->node_name.name);
        }

        //ex.set_str("A4", _TR("Адрес объекта"));

        CString mkr = get_V(map_val_zd, "ID_ADR_MAS");
        if (mkr != "") mkr = _TR("Микрорайон ") + mkr + " ";
        CString dom = get_V(map_val_zd, "NUMBER_1");
        if (dom != "") dom = _TR("дом ") + dom + " ";

        ex.set_str("D7", mkr + get_V(map_val_zd, "STREET_NAM") + " " + dom);

        //ex.set_str("A5", _TR("Назначение"));
        ex.set_str("E9", get_V(map_val_zd, "NAME"));

        ex.set_str("F15", get_V(map_val_zd, "MATERIAL"));


        //ex.set_str("A6", _TR("Этажность"));
        ex.set_str("D13", get_V(map_val_zd, "FLOOR"));
        ex.set_str("H13", get_V(map_val_zd, "QUANTITY_1"));  // Количество подъездов

        CString zhil = get_V(map_val_zd, "ZHIL_ZASTR");  // Жилая застройка

        if (zhil == _TR("Да - Жилая застройка")) {
            ex.set_var("B11", getGal());
        }
        else {
            ex.set_var("F11", getGal());
        }

        //ex.set_str("A7", _TR("Материал"));
        //ex.set_str("E7", get_V(map_val_zd, "MATERIAL"));

        //ex.set_str("A8", _TR("Год постройки"));
        ex.set_str("E17", get_V(map_val_zd, "YEAR_OF_FO"));

        //ex.set_str("A9", _TR("Общая площадь"));
        ex.set_str("I17", get_V(map_val_zd, "OBSCH_AREA"));

        //ex.set_str("A9", _TR("Жилая площадь"));
        ex.set_str("M17", get_V(map_val_zd, "ZHIL_AREA"));
        ex.set_str("F19", get_V(map_val_zd, "KSK"));


        //ex.set_str("A10", _TR("Принадлежность"));
        //ex.set_str("E10", get_V(map_val_zd, "KSK"));


        double gv_pr = atof(get_V(map_val1, "gvpr"));
        double gv_sm = atof(get_V(map_val1, "gvsm"));
        double gv_ps = atof(get_V(map_val1, "gvps"));
        double gv_pw = atof(get_V(map_val1, "gvpw"));
        double gv_op = atof(get_V(map_val1, "gvop"));
        double gv_oo = atof(get_V(map_val1, "gvoo"));


        ex.set_var("E24", COleVariant((long)v_node.size()));


        //ex.set_str("A11", _TR("Система ГВС"));

        if (gv_pr + gv_sm + gv_ps + gv_pw + gv_op + gv_oo == 0) {
            ex.set_var("F26", getGal());
        }
        if (gv_op + gv_oo > 0) {
            ex.set_var("H26", getGal());
        }
        if (gv_pr + gv_sm + gv_ps + gv_pw > 0) {
            ex.set_var("J26", getGal());
        }

        double otoplz = atof(get_V(map_val1, "otoplz"));
        double otopln = atof(get_V(map_val1, "otopln"));

        //ex.set_str("A12", _TR("Система отопления"));
        if (otoplz + otopln == 0) {
            ex.set_var("F28", getGal());
        }
        if (otoplz > 0) {
            ex.set_var("H28", getGal());
        }
        if (gv_pr + gv_sm + gv_ps + gv_pw > 0) {
            ex.set_var("J26", getGal());
        }

        CString pr_avtomat = get_V(map_val1, "pr_avtomat");

        setGal(ex, pr_avtomat, "А", "D42", "Н", "D44", "", "D44", "Ч", _TR("Н42"), NULL);


        //ex.set_str("A13", _TR("Система присоединения"));

        CString cxema = get_V(map_val1, "cxema");

        extern int nn_a14, nn_otopln;
        const char *buf = get_idrp(string(cxema));

        if (buf && nn_a14 >= 0 && (bool)buf[nn_a14]) {
            //ex.set_str("E13", _TR("Элеватор"));
        }


        if (node) {
            if (node->isEl) {
                ex.set_var("F30", getGal());
            }
            if (node->isNez) {
                ex.set_var("J30", getGal());
            }
        }



        for (int i = 0; i < v_node.size(); i++) {
            map<CString, CString> map_val2;
            map<CString, CString> map_val_d;
            CNode *n = v_node[i];
            read_pav(node->file->m_dbf, nodef[node->typ].table, n->nomP, map_val2);

            CString q;
            q.Format("SELECT * FROM [DR_OUT] WHERE kod='%s' AND uzel='%s'", n->node_name.kod, n->node_name.name);

            read_pav(node->file->m_dbf, q, map_val_d);


            ex.set_str(CRange(49 + 9 * i, 6), n->node_name.name);

            CString pr_uchet = get_V(map_val2, "pr_uchet");

            if (pr_uchet == "У") {
                ex.set_var(CRange(49 + 9 * i, 8), getGal());
            }

            CString b1 = get_V(map_val2, _TR("Признак работы прибора учета"));

            if (b1 == "TRUE") {
                ex.set_var(CRange(49 + 9 * i, 10), getGal());
            }

            CString b2 = get_V(map_val2, _TR("Сохранность пломбы прибора учета"));

            if (b2 == "TRUE") {
                ex.set_var(CRange(51 + 9 * i, 8), getGal());
            }

            ex.set_str(CRange(51 + 9 * i, 11), get_V(map_val2, _TR("Номер пломбы прибора учета")));

            CString b7 = get_V(map_val_d, "b7");

            if (atof(b7) != 0) {
                ex.set_str(CRange(54 + 9 * i, 1), _TR("Диаметр элеватора"));
                ex.set_str(CRange(54 + 9 * i, 6), b7);
            }

        }
    }
}

void CGidrView::OnPopupPasVV()
{
    pasport_VV(m_menu_pt);
}

#endif