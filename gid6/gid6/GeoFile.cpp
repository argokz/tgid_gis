// GeoFile.cpp: implementation of the CGeoFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"
#include "win.h"

#include "gid6.h"

#include "gid6Doc.h"
#include "gidrView.h"
#include "GeoFile.h"

#include <sys/stat.h>
#include <share.h>
#include <math.h>
#include <direct.h>
#include <algorithm>
//#include "TestSheet.h"
//#include "GeoEdit.h"
#include "KlMenu.h"
//#include "EditDlg_shp.h"

#include "ado.h"

#include "electro.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CGeoFile* m_geofile = NULL;


/*
static int n_coord, n_shape, n_point;
static int n_MinGX, n_MinGY, n_MaxGX, n_MaxGY;
static int n_EMinX, n_EMinY, n_EMaxX, n_EMaxY;
*/

CString GetExt(const char* path);
CString GetFName(const char* path);
CString GetTitle(CString path);
CString GetPath(CString path);

//void Init(CTreeCtrl *ctlTree);

const int loc_list[] = { 1, 6, 11, 2, 12, 3, 4, 5 };
const int loc_list_size = sizeof(loc_list) / sizeof(loc_list[0]);

CGeoFile::CGeoFile()
{
    m_fn = "";
    //  m_kls000 = 0;  
    m_nCur = -1;
    m_n_list = 0;
    m_kl_list = new KlList();
    m_shp = NULL;
    is_shpdbf = FALSE;
    is_gidr = FALSE;
    is_odbc = FALSE;
    is_geomedia = FALSE;
    m_name = "";
    m_isOpen = false;
    m_abs_pos = -1;
    m_view = NULL;
    is_exit = FALSE;
    m_is_coord = FALSE;
    m_old_remont = -1;
    m_typop = -1;
    m_n_undo = 0;
}

CGeoFile::~CGeoFile()
{
    //  if (m_isOpen) 
    delete m_kl_list;
    //  if (m_db) delete m_db;
    if (m_shp) delete m_shp;
}

void ShapeToStr(COleVariant& var, CString& str);


#if 0
bool CGeoFile::readGEO() {
    COleVariant varValue;

    int n, nn;
    long nom;

    if (is_shape) return false;
    if (!m_db) return false;

    if (m_n_list == 0) {
        m_frmx1 = LONG_MAX;
        m_frmy1 = LONG_MAX;
        m_frmx2 = LONG_MIN;
        m_frmy2 = LONG_MIN;
    }

    KlList::iterator it = m_kl_list->begin();

    int n_T = 0;

    for (; it != m_kl_list->end(); ++it) {
        Klassif* kl = &it->second;
        if (kl->m_db != m_db) continue;

        kl->m_db = m_db;
        kl->geo4.resize(0);

        if (InitS(kl->nazv)) {
            n_T++;

            if (!m_db->rs->IsEOF()) {
                m_db->rs->MoveFirst();

                nn = m_db->NRecs();
                if (nn) {
                    kl->geo4.resize(nn);
                    n = 0;

                    for (int i = 0; i < kl->geo4.size(); i++) kl->geo4[i] = NULL;

                    while (!m_db->rs->IsEOF()) {
                        m_db->rs->GetFieldValue(0, varValue);  nom = V_I4(&varValue);

                        CFRect rect;
                        m_db->readFRectInt(rect);


                        m_frmx1 = min(m_frmx1, rect.left);
                        m_frmy1 = min(m_frmy1, -rect.bottom);
                        m_frmx2 = max(m_frmx2, rect.right);
                        m_frmy2 = max(m_frmy2, -rect.top);

                        CGeoObject* g = new CGeoObject;

                        g->nom = nom;
                        g->nom2 = nom;
                        g->mark = 0;
                        g->rect = rect;

                        if (n < nn) {
                            kl->geo4[n] = g;
                        }
                        else {
                            kl->geo4.push_back(g);
                        }
                        n++;

                        m_db->rs->MoveNext();
                    }
                    kl->geo4.resize(n);
                }
            }
        }
    }

    return n_T > 0;
}



bool CGeoFile::close()
{
    CString ext = GetExt(m_fn);

    set <CString> set_db = m_kl_list->getFileList();
    set <CString>::const_iterator it = set_db.begin();

    for (; it != set_db.end(); ++it) {
        Klassif* kls = m_kl_list->findKlN(*it);
        if (kls) {
            for (int i = 0; i < kls->geo4.size(); i++) delete kls->geo4[i];
            kls->m_db->closeMDB();
        }
    }

    m_n_list = 0;

    delete m_kl_list;

    m_kl_list = NULL;
    m_kl_list = new KlList();

    return true;
}
#endif



/*

  Внутренняя геобаза

*/

void preobr(const CFPoint* p1, const CFPoint* p2, double* x);

bool CGeoFile::SetKls(const char* tN, const char* qN)
{
    KlList::iterator it = m_kl_list->begin();

    for (; it != m_kl_list->end(); ++it) {
        Klassif* kl = &it->second;
        if (!_strcmpi(kl->nazv, tN) || !_strcmpi(kl->xy_table, tN)) {
            //      m_kls000 = kl;
            return true;
        }
    }
    return false;
}


bool CGeoFile::GoToN(int n)
{
    return false;
}

int CGeoFile::findN(const char* tn, int n) {
    /*
      Klassif *kls = m_kl_list->findKlN(tn);
      if (kls && !kls->xy_table.IsEmpty()) {
        if (InitS(tn)) {
          m_db->GotoKey(n);
          return 1;
        }
      }
    */
    return 0;
}


bool CGeoFile::GetBorder(const Klassif* kls, CFRect& rect1)
{
    rect1 = CFRect(LONG_MAX, LONG_MAX, -LONG_MAX, -LONG_MAX);

    int n = kls->geo4.size();
    for (int i = 0; i < n; i++) {
        CFRect rect = kls->geo4[i]->rect;
        rect1.AddPoint(rect.TopLeft());
        rect1.AddPoint(rect.BottomRight());
    }
    return true;
}

bool CGeoFile::GetBorder(CFRect& rect)
{
    rect.left = m_frmx1;
    rect.top = -m_frmy2;
    rect.right = m_frmx2;
    rect.bottom = -m_frmy1;

    return true;
}


bool CGeoFile::GetBorder2(CFRect& rect)
{
    /*
        rect.left = m_frmx1;
        rect.top = -m_frmy2;
        rect.right = m_frmx2;
        rect.bottom = -m_frmy1;

        return true;
    */
    rect = CFRect(LONG_MAX, LONG_MAX, -LONG_MAX, -LONG_MAX);

    KlList::const_iterator it = m_kl_list->begin();
    for (; it != m_kl_list->end(); ++it) {
        CFRect r;
        const Klassif* kls = &it->second;

        if (kls->ins) {
            GetBorder(kls, r);
            rect.AddPoint(r.TopLeft());
            rect.AddPoint(r.BottomRight());
        }
    }
    return true;
}



bool CGeoFile::GetCoord(const Klassif* kls, int n, CCoordList& cl, double dx, double dy, double coef)
{
    //  if (!m_kls) return false;

    if (isRead(kls, n)) {
        cl = kls->geo4[n - 1]->cl;
        cl.conv();
        m_plus();
        return true;
    }

    d_plus();

    if (GoToN(n)) {
        bool ret = GetCoord(kls, cl, dx, dy, coef);
        kls->geo4[n - 1]->cl = cl;
        kls->geo4[n - 1]->is_read = true;
        CFRect r1 = kls->geo4[n - 1]->rect;
        CFRect r = cl.GetFRect();
        kls->geo4[n - 1]->rect = r;

        cl.conv();

        return ret;
    }

    return false;
}


bool CGeoFile::GetCoord(const Klassif* kls, CCoordList& cl, double dx, double dy, double coef)
{
    cl.clear();

    //  if (m_db) {
    //    m_db->readCoord(cl);
    //  }

    if (m_shp) {
        double x1, y1, x2, y2;
        m_shp->read_coor2(x1, y1, x2, y2, cl);
    }

    if (is_odbc && kls) {
        CCoordList::iterator it;

        for (it = cl.begin(); it != cl.end(); ++it) {
            double x, y;
            x = it->x;
            y = it->y;

            it->x = it->x / kls->IdxGridSize + kls->IdxOriginX;
            it->y = it->y / kls->IdxGridSize - kls->IdxOriginY;
        }
    }

    return true;
}

bool CGeoFile::undo()
{
    AfxMessageBox(_TR("Временно удалено 101"), MB_OK | MB_ICONINFORMATION);
    return true;
}

bool find2(CFPoint p, CFPoint p1, CFPoint p2, double d)
{
    double xmin, ymin, xmax, ymax, x1, y1, x2, y2, xx, yy;

    x1 = p1.x;
    y1 = p1.y;

    x2 = p2.x;
    y2 = p2.y;

    if (x2 > x1) { xmin = x1; xmax = x2; }
    else { xmin = x2; xmax = x1; }
    if (y2 > y1) { ymin = y1; ymax = y2; }
    else { ymin = y2; ymax = y1; }

    if (x1 == x2 && x1 - d < p.x && p.x < x1 + d && ymin < p.y && p.y < ymax) return true;
    if (y1 == y2 && y1 - d < p.y && p.y < y1 + d && xmin < p.x && p.x < xmax) return true;
    if (xmin <= p.x && p.x <= xmax && ymin <= p.y && p.y <= ymax) {
        if (xmax - xmin >= ymax - ymin) {
            yy = y1 + (p.x - x1) * (y2 - y1) / (x2 - x1);
            if (yy - d < p.y && p.y < yy + d) return true;
        }
        else {
            xx = x1 + (p.y - y1) * (x2 - x1) / (y2 - y1);
            if (xx - d < p.x && p.x < xx + d) return true;
        }
    }

    return false;
}

struct QQ {
    long n1, n2, typ;
};

int sfqq(const void* a, const void* b) {
    QQ* a1, * b1;

    a1 = (QQ*)a;
    b1 = (QQ*)b;

    if (a1->n2 > b1->n2) return 1;
    if (a1->n2 < b1->n2) return -1;
    return 0;
}

void CGeoFile::query(const char* sn, const CCoordList& cl0, int& nn, QQQ*& qqq)
{
    /*
      string str;
      int i, j, k, n, l, nom;
      QQ *qq, *qq2;
      CCoordList cl;
      CFRect rect0 = cl0.GetFRect(), r;
      Klassif *kls;

      //QQQ *qqq;
      nn = 0;

      if (!sn) return;

      kls = m_kl_list->findKlN(sn);

      if (!kls) return;

      n = kls->geo4.size();
      if (n == 0) return;

      qq = new QQ[n];

      int n_mark = NMark();

      for ( i = 0; i < n; i++ ) {
    //    if (geo4[sl][i].mark) {
    //      geo4[sl][i].mark = 0;
          qq[i].n1 = i;
          qq[i].n2 = kls->geo4[i]->nom;
    //    }
      }

      qsort((void *) qq, (size_t) n, sizeof(QQ), sfqq);

      k = n;

      n = m_db->NRecs();
      qq2 = new QQ[n];

      qqq = new QQQ[n];

      i = 0;
      COleVariant var;

      int tn;
      int nr = 1;
      nn = 0;

      if (!m_db->rs->IsEOF()) {
        m_db->rs->MoveFirst();

        tn = m_db->FieldNum(_TR("Вид объекта"));

        while ( !m_db->rs->IsEOF() ) {
          m_db->rs->GetFieldValue(0, var);  nom = V_I4(&var);
          if (!cl0.empty()) {
            CFRect rect;
            if (!m_db->readFRect(rect)) {
              m_db->rs->MoveNext();
              nr++;
              continue;
            }
            if (!r.IntersectRect(rect, rect0)) {
              m_db->rs->MoveNext();
              nr++;
              continue;
            }

            GetCoord(cl);

            for (CCoordList::const_iterator it = cl.begin(); it != cl.end(); ++it ) {
              if (cl0.find3(*it)) {
                goto QUIT_LOOP;
              }
            }
            m_db->rs->MoveNext();
            nr++;
            continue;
          }

        QUIT_LOOP:
          qqq[i].n1 = nr;
          qqq[i].n2 = i+n_mark+1;

          qq2[i].n1 = i+n_mark+1;
          qq2[i].n2 = nom;
          qq2[i].typ = -1;
          if (tn >= 0) {
            m_db->rs->GetFieldValue(tn-1, var);
            qq2[i].typ = V_I4(&var);
          }
          i++;
          m_db->rs->MoveNext();
          nr++;
        }
      }
      nn = i;

      qsort((void *) qq2, (size_t) i, sizeof(QQ), sfqq);

      for (j = 0, l = 0; j < i; j++) {
        for (; l < k; l++) {
          if (qq2[j].n2 == qq[l].n2) {
            kls->geo4[qq[l].n1]->mark = 1;
            kls->geo4[qq[l].n1]->nom2 = qq2[j].n1;
            kls->geo4[qq[l].n1]->typ = qq2[j].typ;
            break;
          }
        }
      }

      delete [] qq;
      delete [] qq2;
    //  delete [] qqq;
    */
}

bool CGeoFile::ClearMark()
{
    bool ret = false;

    KlList::iterator it = m_kl_list->begin();

    for (; it != m_kl_list->end(); ++it) {
        Klassif* kl = &it->second;
        for (int i = 0; i < kl->geo4.size(); i++) {
            if (kl->geo4[i]->mark) {
                ret = true;
            }
            kl->geo4[i]->mark = 0;
        }
    }
    return ret;
}

int CGeoFile::findXY3(CFPoint p, double md, int loc, const char* fn)
{
    Klassif* kls = m_kl_list->findKlN(fn);
    //  m_kls000 = kls;

    if (kls && (m_isOpen && m_kl_list) || m_shp) {
        int lc = kls->loc;

        CFRect rect;
        CCoordList cl;
        int i, n;

        if (SetKls(fn)) {
            bool isOpen = false;

            n = kls->NObj();

            for (i = 1; i <= n; i++) {
                GetFRect(kls, i, rect);

                if (IsVisible(kls, i)) {
                    rect.InflateRect(md, md);

                    if (rect.PtInRect(p)) {
                        if (!GetCoord(kls, i, cl)) continue;

                        if (isPoint(GetLoc(kls))) {
                            return GetNom(kls, i);
                        }
                        else {
                            if (lc == 6 && cl.find2(CFPoint(p.x, p.y), md, 2)) {
                                return GetNom(kls, i);
                            }

                            if (isLine(lc) && cl.find2(CFPoint(p.x, p.y), md, GetLoc(kls))) {
                                return GetNom(kls, i);
                            }

                            if (lc == 3 && loc == LOC3 && GetLoc(kls) == 3 && cl.find3(CFPoint(p.x, p.y))) {
                                return GetNom(kls, i);
                            }

                            if (lc == 3 && isLine(loc) && cl.find2(CFPoint(p.x, p.y), md, 2)) {
                                return GetNom(kls, i);
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}


bool CGeoFile::findXY_end_exact(CFPoint p, CString fn, map<CString, set<int> > & set_out )
{
    Klassif* kls = m_kl_list->findKlN(fn);
    if (!kls) return 0;
    if (kls->loc != LOC2) return 0;
    CCoordList cl;

    if (SetKls(fn)) {
        int n = kls->NObj();
        for (int i = 1; i <= n; i++) {
            if (!GetCoord(kls, i, cl)) continue;
            CFPoint p1 = cl.first();
            CFPoint p2 = cl.last();

            double d1 = Length(p, p1);
            double d2 = Length(p, p2);

            if (d1 < 10e-5 || d2 < 10e-5) {
                set_out[fn].insert(i);
            }
        }
    }

    return !set_out.empty();
}


bool CGeoFile::findXY_end_list_exact(CFPoint p, set<CString> &list_fn, map<CString, set<int> > & set_out )
{
    for (auto & it : list_fn) {
        findXY_end_exact(p, it, set_out);
    }
    return !set_out.empty();
}


int CGeoFile::findXY3_bl(CFPoint p, double md, int loc, const char* fn)
{
    Klassif* kls = m_kl_list->findKlN(fn);
    //  m_kls000 = kls;

    double min_len = 1e20;
    int min_nom = 0;

    if (kls && (m_isOpen && m_kl_list) || m_shp) {
        int lc = kls->loc;

        CFRect rect;
        CCoordList cl;
        int i, n;

        if (SetKls(fn)) {
            bool isOpen = false;

            n = kls->NObj();

            for (i = 1; i <= n; i++) {
                GetFRect(kls, i, rect);
                rect.InflateRect(md, md);

                if (IsVisible(kls, i) && rect.PtInRect(p)) {
                    if (!GetCoord(kls, i, cl)) continue;

                    if (isPoint(GetLoc(kls))) {
                        return GetNom(kls, i);
                    }
                    else {
                        if (lc == 6 && cl.find2(CFPoint(p.x, p.y), md, 2)) {
                            return GetNom(kls, i);
                        }

                        if (isLine(lc) && cl.find2(CFPoint(p.x, p.y), md, GetLoc(kls))) {
                            return GetNom(kls, i);
                        }

                        if (lc == 3 && loc == LOC3 && GetLoc(kls) == 3 && cl.find3(CFPoint(p.x, p.y))) {
                            return GetNom(kls, i);
                        }

                        if (lc == 3 && isLine(loc) && cl.find2(CFPoint(p.x, p.y), md, 2)) {
                            return GetNom(kls, i);
                        }
                    }
                }
                if (!GetCoord(kls, i, cl)) continue;

                double len = cl.Length2(p, loc);
                if (len < min_len) {
                    min_len = len;
                    min_nom = GetNom(kls, i);
                }
            }
        }
    }
    return min_nom;
}



#if 0
int CGeoFile::findXY2(CFPoint p, double md, int loc, char* fn)
{
    double len, min_len = 1e30;
    int min_n = -1;
    const Klassif* min_kls = NULL;

    fn[0] = 0;

    if (!m_isOpen || !m_kl_list) return 0;


    if (m_isOpen && m_kl_list) {

        CFRect rect;
        CCoordList cl;
        int i, n;

        for (int lc1 = 0; lc1 < loc_list_size; lc1++) {
            //      int loc_list[] = { 1, 6, 2, 3, 4, 5};
            int lc = loc_list[lc1];

            if (isLine(lc) || lc == 3) {
                if (min_len < md) {
                    //          break;
                }
            }

            min_len = 1e30;

            for (int pr = 10; pr > -1; pr--) {
                KlList::const_iterator it = m_kl_list->begin();
                for (; it != m_kl_list->end(); ++it) {
                    const Klassif* kl = &it->second;

                    if (kl->prio == pr && kl->ins && (kl->loc == lc || (kl->loc == LOC3 && isLine(lc) && loc != LOC6))) {

                        if (SetKls(kl->nazv)) {
                            bool isOpen = false;

                            n = m_kls->NObj();

                            //              for ( i = 1; i <= n; i++ ) {
                            for (i = n; i >= 1; i--) {
                                GetFRect(i, rect);
                                rect.InflateRect(md, md);

                                if (IsVisible(i) && rect.PtInRect(p)) {

                                    if (!GetCoord(i, cl)) continue;

                                    if (isPoint(lc) || isLine(lc) || lc == 3) {
                                        if (isPoint(lc)) {
                                            CFPoint pp = cl.first();
                                            len = Length(p, pp) * 100;
                                        }
                                        else {
                                            len = cl.Length2(p, lc);
                                        }
                                        if (len < min_len) {
                                            min_len = len;
                                            min_n = i;
                                            min_kls = kl;
                                        }
                                    }
                                    if (lc == 3 && loc == LOC3 && min_len > md) {
                                        if (lc == 3 && loc == LOC3 && GetLoc() == 3 && cl.find3(CFPoint(p.x, p.y))) {
                                            min_len = md;
                                            min_n = i;
                                            min_kls = kl;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

QUIT:

    if (min_n >= 0 && min_len <= md) {
        SetKls(min_kls->nazv);
        lstrcpy(fn, min_kls->nazv);

        int n = GetNom(min_n);
        //    sprintf(fn, "%s %d %d %g", min_kls->nazv, min_kls->loc, n, min_len);

        return n;
    }


    return 0;
}

#endif

int CGeoFile::findXY_list(CFPoint p, double md, int loc, set<CString> &list_fn, char* fn)
{
    for (auto & it : list_fn) {
        int n = findXY3(p, md, loc, it);
        if (n != 0) {
            strcpy(fn, it);
            return n;
        }
    }
    return 0;
}



int CGeoFile::findXY2(CFPoint p, double md, int loc, char* fn)
{
    double len, min_len = 1e30;
    int min_n = -1;
    const Klassif* min_kls = NULL;
    fn[0] = 0;

    if (!m_isOpen || !m_kl_list) return 0;

    if (m_isOpen && m_kl_list) {

        CFRect rect;
        CCoordList cl;
        int i, n;

        for (int lc1 = 0; lc1 < loc_list_size; lc1++) {
            //      int loc_list[] = { 1, 6, 2, 3, 4, 5};
            int lc = loc_list[lc1];

            if (isLine(lc) || lc == 3) {
                if (min_len < md) {
                    //          break;
                }
            }

            for (int pr = 10; pr > -1; pr--) {
                KlList::const_iterator it = m_kl_list->begin();
                for (; it != m_kl_list->end(); ++it) {
                    const Klassif* kl = &it->second;

                    if (kl->prio == pr && kl->ins && (kl->loc == lc || (kl->loc == LOC3 && isLine(lc) && loc != LOC6))) {

                        if (SetKls(kl->nazv)) {
                            bool isOpen = false;

                            n = kl->NObj();

                            for (i = 1; i <= n; i++) {
                                GetFRect(kl, i, rect);
                                rect.InflateRect(md, md);

                                if (IsVisible(kl, i) && rect.PtInRect(p)) {
                                    if (!GetCoord(kl, i, cl)) continue;

                                    if (isPoint(lc) || isLine(lc) || lc == 3) {
                                        if (isPoint(lc)) {
                                            CFPoint pp = cl.first();
                                            len = Length(p, pp) * 100;
                                        }
                                        else {
                                            len = cl.Length2(p, lc);
                                        }
                                        if (len < min_len) {
                                            min_len = len;
                                            min_n = i;
                                            min_kls = kl;
                                        }
                                    }
                                    if (lc == 3 && loc == LOC3 && min_len > md) {
                                        if (lc == 3 && loc == LOC3 && GetLoc(kl) == 3 && cl.find3(CFPoint(p.x, p.y))) {
                                            min_len = md;
                                            min_n = i;
                                            min_kls = kl;
                                        }
                                    }
                                }
                            }
                            if (isLine(lc) || lc == 3) {
                                if (min_len < md) {
                                    goto QUIT;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

QUIT:

    if (min_n >= 0 && min_len <= md) {
        SetKls(min_kls->nazv);
        lstrcpy(fn, min_kls->nazv);

        int n = GetNom(min_kls, min_n);
        //    sprintf(fn, "%s %d %d %g", min_kls->nazv.c_str(), min_kls->loc, n, min_len);

        return n;
    }


    return 0;
}





int CGeoFile::findXYmin(CFPoint p, char* fn)
{
    int i, n;
    CFRect rect;
    CCoordList cl;

    double sq_min = 0;
    CString tn_min = "";
    int nom_min = 0;

    KlList::const_iterator it = m_kl_list->begin();
    for (; it != m_kl_list->end(); ++it) {
        const Klassif* kl = &it->second;
        if (kl->ins && kl->loc == LOC3) {
            if (SetKls(kl->nazv)) {
                bool isOpen = false;
                n = kl->NObj();
                for (i = 1; i <= n; i++) {
                    GetFRect(kl, i, rect);

                    if (rect.PtInRect(p)) {
                        if (GetCoord(kl, i, cl)) {
                            if (cl.find3(p)) {
                                double sq = cl.Square();
                                if (sq < sq_min || sq_min == 0) {
                                    tn_min = kl->nazv;
                                    nom_min = GetNom(kl, i);
                                    sq_min = sq;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    lstrcpy(fn, tn_min);
    if (sq_min == 0) return 0;
    return nom_min;
}

int CGeoFile::findXY(CFPoint p, double md, int loc, char* fn)
{
    return findXY2(p, md, loc, fn);
}

int CGeoFile::findXYkls(CFPoint p, double md, int loc, Klassif** kl_out)
{
    if (!m_isOpen || !m_kl_list) return 0;

    CFRect rect;
    CCoordList cl;
    int i, n;

    for (int lc1 = 0; lc1 < loc_list_size; lc1++) {
        //    int loc_list[] = { 1, 6, 2, 3, 4, 5};
        int lc = loc_list[lc1];

        for (int pr = 10; pr > -1; pr--) {
            KlList::iterator it = m_kl_list->begin();
            for (; it != m_kl_list->end(); ++it) {
                Klassif* kl = &it->second;
                if (kl->prio == pr && kl->ins && (kl->loc == lc || (kl->loc == LOC3 && isLine(lc)))) {
                    if (SetKls(kl->nazv)) {
                        bool isOpen = false;
                        n = kl->NObj();

                        for (i = 1; i <= n; i++) {
                            GetFRect(kl, i, rect);
                            rect.InflateRect(md, md);

                            if (IsVisible(kl, i) && rect.PtInRect(p)) {

                                if (!GetCoord(kl, i, cl)) continue;

                                if (GetLoc(kl) == 1 || GetLoc(kl) == 11) {
                                    *kl_out = kl;
                                    return GetNom(kl, i);
                                }
                                else {
                                    if (lc == 6 && cl.find2(CFPoint(p.x, p.y), md, 2)) {
                                        *kl_out = kl;
                                        return GetNom(kl, i);
                                    }
                                    if (isLine(lc) && cl.find2(CFPoint(p.x, p.y), md, GetLoc(kl))) {
                                        *kl_out = kl;
                                        return GetNom(kl, i);
                                    }
                                    if (lc == 3 && loc == LOC3 && GetLoc(kl) == 3 && cl.find3(CFPoint(p.x, p.y))) {
                                        *kl_out = kl;
                                        return GetNom(kl, i);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}



CCoordList CGeoFile::Add(const Klassif* kls)
{
    bool first = true;
    int n1 = 0, n2 = 0;

    for (int i = 0; i < kls->NObj(); i++) {
        if (kls->geo4[i]->mark) {
            kls->geo4[i]->mark = 0;
            if (first) {
                first = false;
                n1 = i + 1;
            }
            else {
                n2 = i + 1;
                break;
            }
        }
    }

    CCoordList cl;

    if (n2 == 0) return cl;

    CCoordList cl1, cl2;

    GoToN(n1);
    GetCoord(kls, cl1);

    GoToN(n2);
    GetCoord(kls, cl2);

    cl.And(cl1, cl2, PBO_UNITE);

    return cl;
}

bool CGeoFile::GetMarkRect(const Klassif* kls, CFRect& rect)
{
    int i;
    rect = CFRect(0, 0, 0, 0);

    for (i = 0; i < kls->NObj(); i++) {
        if (IsMark(kls, i + 1)) {
            rect.UnionRect(&rect, &kls->geo4[i]->rect);
        }
    }
    return true;
}

unsigned int CGeoFile::GetNumList(const Klassif* kls, unsigned int nom)
{
    unsigned int n = kls->NObj();
    for (unsigned int i = 0; i < n; i++) {
        if (kls->geo4[i]->nom == nom) return i + 1;
    }
    return 0;
}

class CGeoObject2
{
public:
    CFRect rect;
    CCoordList cl;
    int nom;
};

class CGeoList2 : public vector<CGeoObject2>
{
};


#include "crack.h"

int CGeoFile::NMark(const char *fn)
{
    Klassif* kls = m_kl_list->findKlN(fn);
    if (kls) return NMark(kls);

    return 0;
}


int CGeoFile::NMark(Klassif* kl)
{
    int n = 0;

    int nn = kl->geo4.size();
    for (int i = 0; i < nn; i++) if (kl->geo4[i]->mark) n++;

    return n;
}


int CGeoFile::NMark(const char *fn, list<int> &list_id)
{
    Klassif* kls = m_kl_list->findKlN(fn);
    if (kls) return NMark(kls, list_id);

    return 0;
}


int CGeoFile::NMark(Klassif* kl, list<int> &list_id)
{
    int n = 0;

    list_id.clear();

    int nn = kl->geo4.size();
    for (int i = 0; i < nn; i++) {
        if (kl->geo4[i]->mark) {
            list_id.push_back(kl->geo4[i]->nom);
            n++;
        }
    }

    return n;
}



int CGeoFile::NMark()
{
    int n, nn, i;

    KlList::const_iterator it = m_kl_list->begin();

    for (n = 0; it != m_kl_list->end(); ++it) {
        const Klassif* kl = &it->second;
        nn = kl->geo4.size();
        for (i = 0; i < nn; i++) if (kl->geo4[i]->mark) n++;
    }
    return n;
}

int CGeoFile::NFlds()
{
    //  if (m_db) return m_db->NFlds();

    if (m_shp) return m_shp->NFlds();

    return 0;

}

#if 0

void CGeoFile::readXYText(double& x, double& y, double& a, double& coef, short& align)
{
    if (m_kls && m_kls->m_db) {
        m_kls->m_db->readXYText(x, y, a, coef, align);
    }
}

void CGeoFile::writeXYText(const char* tN, unsigned int nom, double x, double y, double a, double coef, short align)
{
    if (!InitS(tN)) return;
    //  if (!m_db->GotoKey(nom)) return;

    if (!CGeoFile::GoToID(nom)) return;

    m_db->Edit();
    m_db->writeXYText(x, y, a, coef, align);
    m_db->Update();

    //  reset_geo(m_nCur-1);
    m_kls->geo4[m_nCur - 1]->is_read = false;
    m_kls->geo4[m_nCur - 1]->n_label = -1;
    m_kls->geo4[m_nCur - 1]->n_label_color = -1;


    /*
      m_kls->geo4[m_nCur-1].p_text.x = x;
      m_kls->geo4[m_nCur-1].p_text.y = y;
      m_kls->geo4[m_nCur-1].alpha = a;
      m_kls->geo4[m_nCur-1].coef = coef;
    */
}

#endif

bool CGeoFile::read_label(const Klassif* kls, int n, CString& text, CString& text_color, double& x, double& y, double& alpha, double& coef, short& align)
{
    if (!kls || n < 1) return false;

    x = y = alpha = 0.;
    coef = 1.;
    text = "";
    align = 0;

    /*
      if (!isReadLabel(n)) {
        if (GoToN(n)) {
    //      AfxMessageBox("read_label", MB_OK|MB_ICONINFORMATION);
          bool ret = read_label(text, text_color, x, y, alpha, coef, align);

          kls->geo4[n-1]->p_text.x = x;
          kls->geo4[n-1]->p_text.y = y;
          kls->geo4[n-1]->alpha = alpha;
          kls->geo4[n-1]->coef = coef;
          kls->geo4[n-1]->n_label = kls->n_label;
          kls->geo4[n-1]->n_label_color = kls->n_label_color;
          kls->geo4[n-1]->text00000 = text;
          kls->geo4[n-1]->text_color = text_color;
          kls->geo4[n-1]->align = align;

          return ret;
        }
      }
      else {
    */
    x = kls->geo4[n - 1]->p_text.x;
    y = kls->geo4[n - 1]->p_text.y;
    alpha = kls->geo4[n - 1]->alpha;
    coef = kls->geo4[n - 1]->coef;
    text = kls->geo4[n - 1]->text00000;
    text_color = kls->geo4[n - 1]->text_color;
    align = kls->geo4[n - 1]->align;
    //  }
    return true;
}

bool CGeoFile::read_label(const Klassif* kls, CString& text, CString& text_color, double& x, double& y, double& alpha, double& coef, short& align)
{
#if 0
    x = y = alpha = 0;
    coef = 1;
    text = "";
    text_color = "";

    if (kls && kls->m_db && (kls->n_label >= 0 || kls->n_label_color) && m_nCur >= 0) {
        kls->m_db->readXYText(x, y, alpha, coef, align);

        if (kls->n_label == 9999) {
            if (n_teh_1 == -1) return false;
            if (n_teh_2 == -1) return false;
            if (n_teh_3 == -1) return false;
            if (n_teh_5 == -1) return false;
            if (n_teh_6 == -1) return false;
            if (n_teh_7 == -1) return false;
            if (n_teh_8 == -1) return false;

            CString s1 = kls->m_db->read1(n_teh_1); //  Заказчик  
            CString s2 = kls->m_db->read1(n_teh_2); //  Объект    
            CString s3 = kls->m_db->read1(n_teh_3); //  Qгвс      
            CString s4 = kls->m_db->read1(n_teh_4); //  Qобщ      
            CString s5 = kls->m_db->read1(n_teh_5); //  База      
            CString s6 = kls->m_db->read1(n_teh_6); //  Qот       
            CString s7 = kls->m_db->read1(n_teh_7); //  Выдача_ТУ 
            CString s8 = kls->m_db->read1(n_teh_8); //  Qвен      
            CString s;

            text.Format(_TR("База %s"), s5);

            if (!s1.IsEmpty()) { s.Format("\n%s", s1); text += s; }
            if (!s2.IsEmpty()) { s.Format("\n%s", s2); text += s; }
            if (!s6.IsEmpty()) { s.Format("\nQот=%s", s6); text += s; }
            if (!s3.IsEmpty()) { s.Format("\nQгвс=%s", s3); text += s; }
            if (!s8.IsEmpty()) { s.Format("\nQвен=%s", s8); text += s; }
            if (!s4.IsEmpty()) { s.Format("\nQобщ=%s", s4); text += s; }
            if (!s7.IsEmpty()) { s.Format("\n%s", s7); text += s; }
        }
        else {
            if (kls->n_label >= 0) {
                text = kls->m_db->read1(kls->n_label);
            }
            if (kls->n_label_color >= 0) {
                text_color = kls->m_db->read1(kls->n_label_color);
            }
        }

        return true;
    }
    if (m_shp && kls->n_label >= 0) {
        text = m_shp->readStr(kls->n_label);
        return true;
    }
#endif
    return false;
}


bool CGeoFile::read_label_MI(const Klassif* kls, CString& text, double& x, double& y, double& alpha, double& coef, short& align)
{
#if 0

    x = y = alpha = 0;
    coef = 1;
    text = "";

    if (kls && kls->m_db && kls->n_text_string >= 0 && kls->n_text_angle >= 0 && kls->n_text_size >= 0 && m_nCur >= 0) {
        alpha = kls->m_db->read_double(kls->n_text_angle);
        coef = kls->m_db->read_double(kls->n_text_size); // coef /= 100.;
        text = kls->m_db->read1(kls->n_text_string);
        return true;
    }
#endif
    return false;
}


CString CGeoFile::read_textstring()
{
#if 0
    CString str;
    if (m_kls && m_kls->m_db && m_kls->n_text_string >= 0) {
        return m_kls->m_db->read1(m_kls->n_text_string);
    }
    if (m_shp && m_kls->n_text_string >= 0) {
        return m_shp->readStr(m_kls->n_text_string);
    }
#endif
    return CString("");
}

double CGeoFile::read_angle()
{
#if 0
    CString str;
    if (m_kls && m_kls->m_db && m_kls->n_text_angle >= 0) {
        return m_kls->m_db->read_double(m_kls->n_text_angle);
    }
    if (m_shp && m_kls->n_text_angle >= 0) {
        return atof(m_shp->readStr(m_kls->n_text_angle));
    }
#endif
    return 0.;
}

double CGeoFile::read_size()
{
#if 0
    CString str;
    if (m_kls && m_kls->m_db && m_kls->n_text_size >= 0) {
        return m_kls->m_db->read_double(m_kls->n_text_size);
    }
    if (m_shp && m_kls->n_text_size >= 0) {
        return atof(m_shp->readStr(m_kls->n_text_size));
    }
#endif
    return 1.;
}

int CGeoFile::createObjZd(const char* tN, const CCoordList& cl,
    const char* street, const char* nomer, int et, const char* naimen
)
{

#if 0
    int n1;

    if (!InitS(tN)) return 0;
    m_db->AddNew();



    CCoordList cl2 = cl;
    cl2.y_minus();
    write_cl(tN, cl2);
    //  m_db->writeCurTime();

    CString str;

    //        m_db->rs->SetFieldValue("geodz", COleVariant(h_tr(pt.x, pt.y)));
    m_db->rs->SetFieldValue("h", COleVariant(et * 3.));
    m_db->rs->SetFieldValue("kol_etag", COleVariant((long)et));
    m_db->rs->SetFieldValue("street", CCrack::Var(CString(street), dbText));
    m_db->rs->SetFieldValue("house_numer", CCrack::Var(CString(nomer), dbText));

    str = naimen;
    str = str.Left(20);
    if (!str.IsEmpty()) m_db->rs->SetFieldValue("name_building", CCrack::Var(str, dbText));

    m_db->Update();

    m_db->rs->SetBookmark(m_db->rs->GetLastModifiedBookmark());

    m_db->read_first(n1);

    CGeoObject* g = new CGeoObject;

    g->nom = n1;
    g->nom2 = n1;
    g->mark = 0;
    g->rect = cl2.GetFRect();
    m_kls->geo4.push_back(g);


    return m_kls->geo4.size();
#endif 
    return 0;

}

static CString m_tn = "";
static CString m_pic;

BOOL CGeoFile::isNext()
{
    return m_tn != "";
}

CFPoint CGeoFile::find(const char* tn, const char* coordn)
{
    CFPoint pt(0, 0);

#if 0
    //  if (!m_db) return pt;

    m_tn = tn;
    m_tn1 = tn;
    m_coordn = coordn;

    m_db->openTable(tn);
    m_db->setGeometry(coordn);

    CGeoEdit utd(AfxGetMainWnd(), m_db, tn);
    int ret = utd.DoModal();

    if (ret == IDOK) {
        int i = 1;

        m_pic = utd.m_pic;
        if (utd.m_pic != "") i = m_db->rs->FindFirst(utd.m_pic);

        if (!i || m_db->NRecs() == 0) {
            AfxMessageBox(_TR("Не найдено"), MB_OK | MB_ICONINFORMATION);
            m_abs_pos = -1;
        }
        else {
            m_abs_pos = m_db->rs->GetAbsolutePosition();

            m_bookmark = m_db->rs->GetBookmark();
            if (m_db->read_first(i)) {
                findN(tn, i);

                CCoordList cl;
                GetCoord(cl);
                pt = cl.centroid();
            }
        }
    }
    m_db->closeTable();

#endif

    return pt;
}





CFPoint CGeoFile::findNext()
{
    CFPoint pt(0, 0);
#if 0
    if (m_tn1 == "") return pt;
    if (m_abs_pos == -1) return pt;
    if (!m_db) return pt;

    m_db->openTable(m_tn1);
    m_db->setGeometry(m_coordn);

    m_db->rs->SetAbsolutePosition(m_abs_pos);

    int i = 1;
    if (m_pic != "") i = m_db->rs->FindNext(m_pic);
    else {
        m_db->rs->MoveNext();
    }

    if (!i || m_db->NRecs() == 0) {
        AfxMessageBox(_TR("Не найдено"), MB_OK | MB_ICONINFORMATION);
        m_abs_pos = -1;
    }
    else {
        //    m_bookmark = m_db->rs->GetBookmark( );
        m_abs_pos = m_db->rs->GetAbsolutePosition();
        if (m_db->read_first(i)) {
            findN(m_tn1, i);

            CCoordList cl;
            GetCoord(cl);
            pt = cl.centroid();
        }
    }
    m_db->closeTable();
#endif
    return pt;
}


//#include "Nagr.h"
//#include "Nagr2.h"
//#include "TU.h"
//#include "TU2.h"

int editAlmaGeo(CWnd* parent, const char* fn, const char* tn, long kk, int ro);


//#include "DialogNew2.h"
//#include "FormDialog.h"

/*

void createElem(CDbFile *dbf, int n25, const char *elem, int kk)
{
  long lFindType = AFX_DAO_FIRST;

  if (dbf->openTable(elem)) {
    for (int i = 1; i <= n25; i++) {

      CString str;
      str.Format("[Дефект]=%d AND [Элемент]=%d", kk, i);

      int ret = 0;

      try {
        ret = dbf->rs->Find(lFindType, str);
      }
      catch  (CDaoException *e) {
      }

      if (ret == 0) {
        dbf->AddNew();
        dbf->write(_TR("Дефект"), kk);
        dbf->write(_TR("Элемент"), i);
        dbf->Update();
      }

      lFindType = AFX_DAO_NEXT;
    }

    dbf->closeTable();
  }
}


void createP(CDbFile *dbf, const char *tn, const char *tnP, int kk)
{
  if (!strcmp(tn, _TR("Шурф"))) {
    long lFindType = AFX_DAO_FIRST;

    if (dbf->openTable(tnP)) {
      CString str;
      str.Format("[Шурф]=%d", kk);

      int ret = 0;

      try {
        ret = dbf->rs->Find(lFindType, str);
      }
      catch  (CDaoException *e) {
      }

      if (ret == 0) {
        dbf->AddNew();
        dbf->write(_TR("Шурф"), kk);
        dbf->Update();
      }
      dbf->closeTable();
    }
  }
}

*/

int editAvaria(CWnd* parent, const char* fn, const char* tn, long kk, int ro, const char* avar_el, const char* avar_tr)
{
    AfxMessageBox(_TR("Временно удалено 102"), MB_OK | MB_ICONINFORMATION);
    return 0;
}


int editOprDefect(CWnd* parent, const char* fn, const char* tn, long kk, int ro);
int editKapital(CGidrView* view, const char* fn, const char* table, long kk, int ro, const char* cap);
int editOpressovka(CGidrView* view, const char* fn, const char* table, long kk, int ro, const char* cap);
int editKorroz(CWnd* parent, const char* fn, const char* tn, long kk, int ro);

int editDefectMin(CWnd* parent, const char* fn, const char* tn, long kk, int ro);
int editWithDoc(CWnd* parent, const char* fn, const char* tn, const char* elem, long kk, int ro);

int CGeoFile::findXY(CFPoint pt, double md, char* tn)
{
    int n;

    n = findXY(pt, md, 3, tn);
    if (n == 0) n = findXY(pt, md, 2, tn);

    return n;
}

/*
int CGeoFile::edit(CScroll *scr, CFPoint pt, int md)
{
  char tn[256];
  int n;

  n = findXY(pt, md, 3, tn);
  if (n == 0) n = findXY(pt, md, 2, tn);

  if (n) return edit(scr, tn, n);

  return 0;
}
*/


char* writeCl(const CCoordList& cl, int typ, long& ind);


static long max_n = 0;;

#if 0
void writeCl(int ns, CDaoRecordset& rs, const CCoordList& cl, int loc)
{
    int typ;

    if (loc == LOC3) {
        typ = 5;
    }
    else if (loc == LOC2 || loc == LOC12) {
        typ = 3;
    }
    else if (loc == LOC6) {
        typ = 3;
    }
    else if (loc == LOC1 || loc == LOC11) {
        typ = 1;
    }


    long ind;

    char* pb = writeCl(cl, typ, ind);
    if (!pb) return;

    COleSafeArray sa;
    sa.CreateOneDim(VT_UI1, ind, pb);

    long r1, r2;

    DWORD dim = sa.GetDim();
    DWORD es = sa.GetElemSize();

    sa.GetLBound(1, &r1);
    sa.GetUBound(1, &r2);

    if (r2 > max_n) {
        max_n = r2;
    }


    try {
        rs.SetFieldValue(ns, sa);
    }
    catch (CDaoException* e) {
        AfxMessageBox(e->m_pErrorInfo->m_strDescription, MB_OK | MB_ICONINFORMATION);
        e->Delete();
    }
    catch (...) {
        AfxMessageBox("??", MB_OK | MB_ICONINFORMATION);
    }

    double x1, x2, y1, y2;

    CFRect r = cl.GetFRect();

    x1 = 0.01 * r.left;
    y1 = -0.01 * r.bottom;
    x2 = 0.01 * r.right;
    y2 = -0.01 * r.top;

    COleVariant vt;
    CString str;

    rs.GetFieldValue(0, vt);

    delete[] pb;
}
*/

#if 0

void CGeoFile::write_cl(const char* tn, const CCoordList & cl)
{
    CCoordList cl1 = cl;
    cl1.conv_obr();

    if (n_coord > 0 || n_shape > 0) {
        if (n_coord > 0) {
            CString str;
            cl1.saveStrMinus(str);
            m_db->write(n_coord, str);
        }
        else if (n_shape > 0) {
            Klassif* kls = m_kl_list->findKlN(tn);
            writeCl(n_shape - 1, *m_db->rs, cl1, kls->loc);
        }


        CFRect rect = cl1.GetFRect();
        m_db->write_rect(rect.left, -rect.bottom, rect.right, -rect.top);
        //    m_db->write_rect(rect.left, rect.top, rect.right, rect.bottom);

        m_frmx1 = min(m_frmx1, rect.left);
        m_frmy1 = min(m_frmy1, -rect.bottom);
        m_frmx2 = max(m_frmx2, rect.right);
        m_frmy2 = max(m_frmy2, -rect.top);
    }
    else if (is_geomedia)
    {
        Klassif* kls = m_kl_list->findKlN(tn);
        if (kls) {
            m_db->setGeometry(kls->coord_name);
            m_db->writeCoord(cl, kls->loc);
        }
    }
}

/*

int CGeoFile::updateObj(const char *tN, unsigned int nom, const CCoordList &NP)
{

/*
  if (!InitS(tN)) return 0;
  if (!m_db->GotoKey(nom)) return 0;

//  move_obj.kls = m_kls;
//  move_obj.cl =


  m_db->Edit();
  write_cl(tN, NP);
  m_db->Update();

  int i, n = NObj();

  for ( i = 0; i < n; i++ ) {
    if (m_kls->geo4[i]->nom == nom) {
      m_kls->geo4[i]->rect = NP.GetFRect();
      m_kls->geo4[i]->is_read = false;
      break;
    }
  }
/
  return 1;
}
*/


void CGeoFile::write(const char* tn, int nn, const char* fn, double val)
{
    if (!InitS(tn)) return;

    Klassif* kls = m_kl_list->findKlN(tn);

    if (!kls) return;
    int n = kls->geo4[nn - 1]->nom;

    if (m_db->GotoKey(n)) {
        m_db->Edit();
        m_db->write(fn, val);
        m_db->Update();
    }
#endif
}

#endif
#if 0

void CGeoFile::write(const char* tn, int nn, const char* fn, int val)
{
    if (!InitS(tn)) return;

    Klassif* kls = m_kl_list->findKlN(tn);

    if (!kls) return;
    int n = kls->geo4[nn - 1]->nom;

    if (m_db->GotoKey(n)) {
        m_db->Edit();
        m_db->write(fn, val);
        m_db->Update();
    }
}


void CGeoFile::write(const char* tn, int nn, const char* fn, const char* val)
{
    if (!InitS(tn)) return;

    Klassif* kls = m_kl_list->findKlN(tn);

    if (!kls) return;
    int n = kls->geo4[nn - 1]->nom;

    if (m_db->GotoKey(n)) {
        m_db->Edit();
        m_db->write(fn, val);
        m_db->Update();
    }
}


void CGeoFile::writeNode(const char* tn, int nn, CNode* nn1, CNode* nn2)
{
    if (!InitS(tn)) return;

    Klassif* kls = m_kl_list->findKlN(tn);

    if (!kls) return;

    //  int n = kls->GetNom(nn);

    int n = kls->geo4[nn - 1]->nom;

    if (m_db->GotoKey(n)) {

        //    CString s1 = nn1->typPS != -1 ? nn1->namePS : nn1->node_name.name;
        //    CString s2 = nn2->typPS != -1 ? nn2->namePS : nn2->node_name.name;

        m_db->Edit();
        m_db->write("kod1", nn1->node_name.kod);
        m_db->write("kod2", nn1->node_name.kod);

        m_db->write("uzel1", nn1->node_name.name);
        m_db->write("uzel2", nn2->node_name.name);

        CString sn1 = PSFullName(nn1);
        CString sn2 = PSFullName(nn2);

        m_db->write(_TR("Наименование начального узла трубы"), sn1);
        m_db->write(_TR("Наименование конечного узла трубы"), sn2);
        //    m_db->write(_TR("Наименование узла камеры/павильона"), sn0);

        m_db->Update();
    }
}
#endif

int isElPoint(const char* tn1)
{
    CString tn = tn1;
    tn.MakeLower();

    //  if (tn == EL_IST) return 1;
    //  if (tn == EL_PRIEM) return 2;

    //  if (tn == EL_LINE) return 7;
    //  if (tn == transl("гильза эс")) return 3;
    //  if (tn == transl("кабельный канал эс")) return 4;
    if (tn == transl("муфта")) return 5;
    if (tn == transl("опора эс")) return 6;

    return 0;
}

int isElSq(const char* tn1)
{
    CString tn = tn1;
    tn.MakeLower();

    if (tn == transl("гильза эс")) return 3;
    if (tn == transl("кабельный канал эс")) return 4;

    return 0;
}




int isElEnd(const char* tn1)
{
    CString tn = tn1;
    tn.MakeLower();

    if (tn == EL_IST) return 1;
    if (tn == EL_PRIEM) return 2;
    return 0;
}

void getProject(CCoordList& cl, CFPoint point, CCoordList& m_NP, int loc);

int CGeoFile::createObjElPoint(const char* tN, const CCoordList& cl, map<CString, COleVariant>& m_map_geo_value)
{
    //  AfxMessageBox(_TR("Временно удалено Не найдена Линия электропередач"), MB_OK|MB_ICONINFORMATION);
    //  return 0;

    CString sl = EL_LINE;

    CFPoint pt = cl.first();
    Klassif* kls = m_kl_list->findKlN(sl);
    if (!kls) return 0;

    Klassif* klsN = m_kl_list->findKlN(tN);
    if (!klsN) return 0;

    int n = findXY3(pt, D5 * 100, 1, sl);

    if (!n && klsN->loc == LOC3) {
        n = findXY3(pt, D5 * 100 * 10, 1, sl);
    }


    if (n) {
        int idLEP = kls->GetNom2(n);

        if (idLEP >= 1) {
            CAdoFile* ado = kls->m_ado;

            CString kr = "";

            CCoordList clL = kls->geo4[idLEP - 1]->cl;
            CFPoint p1 = clL.first();
            CFPoint p2 = clL.last();
            CCoordList cl2;

            int be = isElEnd(tN);

            if (be) {
                double d1 = Length(p1, pt);
                double d2 = Length(p2, pt);

                if (d1 < d2) {
                    pt = p1;
                }
                else {
                    pt = p2;
                }
                if (d1 > D5 && d2 > D5) {
                    AfxMessageBox("Установите этот объекты возле конца Линии электропередач", MB_OK | MB_ICONINFORMATION);
                    return 0;
                }
                cl2.push_back(pt);
            }
            else if (isElPoint(tN)) {
                getProject(clL, pt, cl2, klsN->loc);
            }
            else {
                cl2 = cl;
            }
            int nn = createObj2(tN, cl2, m_map_geo_value);
            if (nn) {
                int id = klsN->geo4[nn - 1]->nom;
                ado->Edit(tN, id);
                kr.Format("%d", n);
                ado->write(transl("Наименование ЛЭП"), kr);
                ado->Update();

                /*

                                if (be == 1) {
                                    ado->Edit(sl, kls->geo4[idLEP - 1]->nom);
                                    ado->write("naimenovanie_istochnika", id);
                                    ado->Update();
                                }
                                else if (be == 2) {
                                    ado->Edit(sl, kls->geo4[idLEP - 1]->nom);
                                    ado->write("naimenovanie_priemnika", id);
                                    ado->Update();
                                }
                */
            }
            return nn;
        }
    }
    else {
        AfxMessageBox("Не найдена Линия электропередач", MB_OK | MB_ICONINFORMATION);
    }
    return 0;
}

CString getKvartal(CFPoint pt)
{
    CString kv = "";


#if 0
    int n = m_geofile->findXY3(pt, 0, 3, _TR("Кварталы_работы_мастеров"));

    if (n > 0) {
        if (m_geofile->m_ado->openTableS(_TR("Кварталы_работы_мастеров"))) {
            if (m_geofile->m_ado->GotoKey(n)) {
                kv = m_geofile->m_ado->read(_TR("НОМЕР_КВАРТАЛА"));
                return kv;
            }
            m_geofile->m_ado->closeTable();
        }
    }

    n = m_geofile->findXY3(pt, 0, 3, _TR("Квартал"));

    if (n > 0) {
        if (m_geofile->m_ado->openTable(_TR("Квартал"))) {
            if (m_geofile->m_ado->GotoKey(n)) {
                kv = m_geofile->m_ado->read("KOD_KVARTAL");
                return kv;
            }
            m_geofile->m_ado->closeTable();
        }
    }
#endif

    return "";
}



int CGeoFile::createObjPassportTP(const char* tN, const CCoordList& cl)
{
    return 0;

    CFPoint pt = cl.first();

    CString kv = getKvartal(pt);

    int n = m_geofile->findXY3(pt, 0, 3, _TR("Кварталы_работы_мастеров"));

#if 0
    if (n > 0) {
        if (m_geofile->m_ado->openTableS(_TR("Кварталы_работы_мастеров"))) {
            if (m_geofile->m_ado->GotoKey(n)) {
                kv = m_geofile->m_ado->read(_TR("НОМЕР_КВАРТАЛА"));
            }
            m_geofile->m_ado->closeTable();
        }
    }


    //#if 0

    if (!InitS(tN)) return 0;

    m_db->AddNew();

    int n1;
    CCoordList cl2 = cl;
    //  cl2.y_minus();
    write_cl(tN, cl2);
    m_db->write(_TR("Квартал"), kv);

    m_db->Update();

    m_db->rs->SetBookmark(m_db->rs->GetLastModifiedBookmark());

    m_db->read_first(n1);

    if (n_shape != -1) {
        if (m_db->openTable(m_kls->xy_table)) {

            CFRect rect = cl.GetFRect();

            double gs = m_kls->IdxGridSize;

            double x1 = rect.left / gs / 100;
            double x2 = rect.right / gs / 100;
            double y1 = -rect.bottom / gs / 100;
            double y2 = -rect.top / gs / 100;

            try {
                m_db->rs->AddNew();
                m_db->rs->SetFieldValue(0, COleVariant((long)n1));
                m_db->rs->SetFieldValue(1, COleVariant((long)x1));
                m_db->rs->SetFieldValue(2, COleVariant((long)y1));
                m_db->rs->SetFieldValue(3, COleVariant((long)x2));
                m_db->rs->SetFieldValue(4, COleVariant((long)y2));

                m_db->rs->Update();
            }
            catch (CDaoException* e) {
                e->Delete();
            }

            m_db->closeTable();
        }
    }

    CGeoObject* g = new CGeoObject;

    g->nom = n1;
    g->nom2 = n1;
    g->mark = 0;
    g->rect = cl.GetFRect();
    m_kls->geo4.push_back(g);

    return m_kls->geo4.size();
#endif

    return 0;
}

int CGeoFile::createObj(const char* tN, CCoordList& cl, map<CString, COleVariant>& m_map_geo_value)
{
    int ret = 0;

    if (isElPoint(tN) || isElSq(tN)) {
        ret = createObjElPoint(tN, cl, m_map_geo_value);
    }
    else if (!strcmp(tN, STR_PASPORT_OBJ)) {
        ret = createObjPassportTP(tN, cl);
    }
    else {
        ret = createObj2(tN, cl, m_map_geo_value);

        CString tn = tN;
        tn.MakeLower();

        if (tn == EL_LINE) {
            CFPoint pt1 = cl.first();
            CFPoint pt2 = cl.last();

            CString tnp1 = EL_IST;
            int n1 = m_geofile->findXY3(pt1, 0, 1, tnp1);
            if (n1 == 0) n1 = m_geofile->findXY3(pt2, 0, 1, tnp1);

            CString tnp2 = EL_PRIEM;
            int n2 = m_geofile->findXY3(pt1, 0, 1, tnp2);
            if (n2 == 0) n2 = m_geofile->findXY3(pt2, 0, 1, tnp2);


            if (n1 || n2) {
                Klassif* kls = m_kl_list->findKlN(tn);
                if (kls) {
                    kls->m_ado->Edit(tn, kls->geo4[ret - 1]->nom);
                    if (n1) kls->m_ado->write("naimenovanie_istochnika", n1);
                    if (n2) kls->m_ado->write("naimenovanie_priemnika", n2);
                    kls->m_ado->Update();
                }
            }
        }
    }

    refreshRemont();
    refreshElectro();

    return ret;
}

double LengthPL(double x0, double y0, double x1, double y1, double x2, double y2);

CNode* find_next(CGraph2* graph, CNode* n1, CNode* n2, CNode* n0);


#include "excel.h"

void createRemontExcel(const char* name, CExcel& ex, CDialogNew2* dlg)
{
    AfxMessageBox(_TR("Временно удалено 103"), MB_OK | MB_ICONINFORMATION);
}

CNode* find_next(CGraph2* graph, CNode* n1, CNode* n2, CNode* n0);

int CGeoFile::createZhurnal(const char* tN, const CCoordList& cl, CLINE2* l)
{
    AfxMessageBox(_TR("Временно удалено 104"), MB_OK | MB_ICONINFORMATION);
    return 0;
}



CFPoint CGeoFile::find()
{
    CKlMenu kl(AfxGetMainWnd(), this, _TR("Список видов объектов"));

    if (kl.DoModal() == IDOK) {
        //    m_db = kl.m_kls->m_db;
        return find(kl.m_kls->nazv, kl.m_kls->coord_name);
    }

    return CFPoint(0, 0);
}

void CGeoFile::reset_kl(const Klassif* kls)
{
    if (kls) {
        int i, n = kls->NObj();

        for (i = 0; i < n; i++) {
            //      kls->geo4[i].is_node = false;
            //      kls->geo4[i].is_line = false;
        }
    }
}

/*
int CGeoFile::GetNom2(int n)
{
  int nnn = m_kls->geo4.size();
  for (int i = 0; i < nnn; i++) {
    int nn = m_kls->geo4[i]->nom;

    if (nn == n) {
      return i+1;
    }
  }
  return 0;
}
*/


CFPoint CGeoFile::findPoint(CFPoint p)
{
    CFPoint point(0, 0);

    if (!m_isOpen || !m_kl_list) return 0;

    CFRect rect;
    CCoordList cl;
    int i, n;

    int md = 1000;

    double min_len = 1e10;

    for (int lc1 = 0; lc1 < loc_list_size; lc1++) {
        //    int loc_list[] = { 1, 6, 2, 3, 4, 5};
        int lc = loc_list[lc1];

        for (int pr = 10; pr > -1; pr--) {
            KlList::iterator it = m_kl_list->begin();
            for (; it != m_kl_list->end(); ++it) {
                Klassif* kl = &it->second;
                if (kl->prio == pr && kl->ins && (kl->loc == lc || (kl->loc == LOC3 && isLine(lc)))) {
                    //          if (InitS(kl->nazv)) {

                    if (SetKls(kl->nazv)) {
                        bool isOpen = false;

                        n = kl->NObj();

                        for (i = 1; i <= n; i++) {
                            GetFRect(kl, i, rect);
                            rect.InflateRect(md, md);

                            if (rect.PtInRect(p)) {
                                if (!GetCoord(kl, i, cl)) continue;

                                if (isPoint(lc)) {
                                    CFPoint p1 = cl.first();

                                    double len = Length(p1, p);
                                    if (len < min_len) {
                                        min_len = len;
                                        point = p1;
                                    }
                                }
                                else {
                                    CCoordList::const_iterator it = cl.begin();

                                    for (; it != cl.end(); ++it) {
                                        CFPoint p1 = *it;

                                        double len = Length(p1, p);
                                        if (len < min_len) {
                                            min_len = len;
                                            point = p1;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return point;
}




#if 0

#include "PtGraph.h"


void CGeoFile::findCl(const CFPoint& p1, const CFPoint& p2, const CCoordList& cl1, CCoordList& cl2)
{
    CFPoint point(0, 0);

    if (!m_isOpen || !m_kl_list) return;

    CFRect rect, rr;
    CCoordList cl;
    int i, n;

    int md = 100;

    CFRect rect1 = cl1.GetFRect();

    rect1.AddPoint(p1);
    rect1.AddPoint(p2);


    rect.InflateRect(md, md);


    double min_len = 1e10;

    int lc = 2;

    int qq = 0, q2 = 0;

    CPtGraph graph;

    for (int pr = 10; pr > -1; pr--) {
        KlList::iterator it = m_kl_list->begin();
        for (; it != m_kl_list->end(); ++it) {
            const Klassif* kl = &it->second;
            if (kl->prio == pr && kl->ins && (kl->loc == lc || (kl->loc == LOC3 && isLine(lc)))) {

                if (SetKls(kl->nazv)) {
                    bool isOpen = false;
                    n = NObj();

                    for (i = 1; i <= n; i++) {
                        GetFRect(i, rect);
                        rect.InflateRect(md, md);

                        if (rr.IntersectRect(rect, rect1)) {
                            if (!GetCoord(i, cl)) continue;

                            qq++;

                            CCoordList::const_iterator it = cl.begin();

                            CFPoint c1, c2;

                            int frst = 1;

                            for (; it != cl.end(); ++it) {
                                q2++;
                                c2 = *it;

                                if (fabs(c2.y + 189747) < 1) {
                                    int iii;
                                    iii = 0;
                                }
                                graph.find_ins(c2);

                                if (!frst) {
                                    graph.insert_line(c1, c2);
                                }

                                c1 = c2;
                                frst = 0;
                            }
                        }
                    }
                }
            }
        }
    }

    CPtNode* n1 = graph.find(p1);
    CPtNode* n2 = graph.find(p2);

    //  CPtNode *n;

    if (n1 && n2) {
        graph.reset();
        graph.WS1(n1, n2);
        graph.WG1(n1, n2, cl2);
    }

    CString str;

    str.Format("qq = %d q2 = %d", qq, q2);
    //  AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);

}

#endif



//void CreateIndex(CDaoTableDef &td, const char *fn);
//void CreateField(CDaoTableDef &td, const char *fn, short nType, long lSize, long lAttributes = 0 );


#include "geomenu.h"

int CGeoFile::findAllXY(CGeoMenu* menu, CView* view, CFPoint p, double md/*, char *tn, int &num*/)
{
    if (!m_isOpen || !m_kl_list) return 0;

    CFRect rect;
    CCoordList cl;
    int i, n;
    bool isOpen = false;
    int ret = 0;

    const Klassif* kl_out;

    for (int lc1 = 0; lc1 < loc_list_size; lc1++) {
        int lc = loc_list[lc1];

        for (int pr = 10; pr > -1; pr--) {
            KlList::iterator it = m_kl_list->begin();
            for (; it != m_kl_list->end(); ++it) {
                const Klassif* kl = &it->second;
                if (kl->prio == pr && kl->ins && (kl->loc == lc)) {

                    kl_out = kl;

                    if (SetKls(kl->nazv)) {
                        bool isOpen = false;

                        n = kl->NObj();

                        for (i = 1; i <= n; i++) {
                            GetFRect(kl, i, rect);
                            rect.InflateRect(md, md);

                            if (rect.PtInRect(p)) {
                                GetCoord(kl, i, cl);

                                kl_out = NULL;

                                if (GetLoc(kl) == LOC1 || GetLoc(kl) == LOC11/* || GetLoc(kl) == LOC6*/) {
                                    kl_out = kl;
                                }
                                else {
                                    if (lc == LOC6 && Length(p, cl.first()) * 100 < md) {
                                        kl_out = kl;
                                    }
                                    else if (isLine(lc) && cl.find2(CFPoint(p.x, p.y), md, GetLoc(kl))) {
                                        kl_out = kl;
                                    }
                                    else if (lc == LOC3 && cl.find3(CFPoint(p.x, p.y))) {
                                        kl_out = kl;
                                    }
                                }
                                if (kl_out) {
                                    menu->Add(kl_out, GetNom(kl_out, i));
                                    ret = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return ret;
}

void CGeoFile::no_view_object(CScroll* scr, CDC* dc_old)
{
    if (dc_old) scr->CloseImage(dc_old);
}

CDC* CGeoFile::view_object(CScroll* scr, const char* tn, int n)
{
    CDC* dc2 = NULL;
    int n1 = n;
    int ret = 0;

    Klassif* kls = m_kl_list->findKlN(tn);
    if (kls) {
        n = kls->GetNom2(n);

        if (n == 0) return 0;

        dc2 = scr->SaveImage();

        CDC* m_dc;
        if ((m_dc = scr->BeginPaint()) != NULL) {
            //      m_kls000 = kls;
            DrawObj(m_dc, scr, kls, n, true, true, true, false, kls->no_mas);
            scr->EndPaint(m_dc);
        }
    }

    return dc2;
}

void CGeoFile::zad()
{
}

bool CGeoFile::isRead(const Klassif* kls, int n) {
    return kls && kls->geo4[n - 1]->is_read;
}

bool CGeoFile::isReadLabel(const Klassif* kls, int n)
{
    if (!kls) return false;

    if (kls->n_label != -1 && kls->n_label != kls->geo4[n - 1]->n_label) {
        //    AfxMessageBox("!!!", MB_OK|MB_ICONINFORMATION);
        return false;
    }

    int nl = kls->geo4[n - 1]->n_label;

    return nl == kls->n_label;
}

char* PS_table[] =
{
  "Опора",
  "Дренажный кран",
  "Воздушник",
  "Запорная арматура",
  "Компенсатор",
  "Дренажный трубопровод",
  "Перемычки",
  "Угол поворота трубопровода",
  "Колодцы",
STR_REMONT_DEFECT,
STR_REMONT_SHURF,
STR_REMONT_OSMOTR,
STR_REMONT_KORROZ,
STR_REMONT_ISPYT
};

int PS_table_size = sizeof(PS_table) / sizeof(PS_table[0]);

extern CGeoFile* m_geofile;

bool isGeoPS()
{
    Klassif* kls = NULL;

    if (!m_geofile) return false;

    for (int i = 0; i < PS_table_size; i++) {
        kls = m_geofile->m_kl_list->findKlN(PS_table[i]);

        if (!kls) return false;
    }

    return true;
}

bool CGeoFile::setMark(CCoordList& cl)
{
    Klassif* kls = m_kl_list->findKlN(transl("Здания ТУ"));
    if (!kls) {
        AfxMessageBox(_TR("Не подключен файл Технических условий"), MB_OK | MB_ICONINFORMATION);
        return false;
    }

    return setMark(kls, cl);
}

bool CGeoFile::setMark(Klassif* kls, CCoordList& cl)
{
    CFRect rect = cl.GetFRect();

    int n = kls->geo4.size();

    for (int i = 0; i < n; i++) {
        CFRect r = kls->geo4[i]->rect;

        if (r.left > rect.right) continue;
        if (r.right < rect.left) continue;
        if (r.top > rect.bottom) continue;
        if (r.bottom < rect.top) continue;

        CCoordList cl0 = kls->geo4[i]->cl;

        if (cl0.peres(cl)) {
            kls->geo4[i]->mark = 1;
        }
    }
    return true;
}

CFPoint CGeoFile::toGeo(CFPoint pt)
{
    if (!m_is_coord) return pt;

    double xx = pt.x;
    double yy = pt.y;
    if (xx != 0 && yy != 0) {
        pt.y = m_geo_x[0] * xx + m_geo_x[1] * yy + m_geo_x[2];
        pt.x = m_geo_x[3] * xx + m_geo_x[4] * yy + m_geo_x[5];

        pt.x = pt.x * 100;
        pt.y = -pt.y * 100;
    }
    return pt;
}

void clearMark(Klassif* kls)
{
}

bool CGeoFile::TrioMove2(CFPoint* m_trio_point)
{
    AfxMessageBox(_TR("Временно удалено TrioMove2"), MB_OK | MB_ICONINFORMATION);
    return true;
}

int editKorroz(CWnd* parent, const char* fn, const char* tn, long kk, int ro)
{
    AfxMessageBox(_TR("Временно удалено 105"), MB_OK | MB_ICONINFORMATION);
    return 0;
}

int CGeoFile::createText(CFPoint pt1, CFPoint pt2, const char* text)
{
    CString tN = "podpis";

    Klassif* kls = m_geofile->m_kl_list->findKlN(tN);
    if (!kls) return 0;

    CCoordList cl2;

    cl2.push_back(pt1);
    cl2.push_back(pt2);

    map<CString, COleVariant> geo_value;
    geo_value["text"] = CString(text);

    return createObj(tN, cl2, geo_value);


#if 0

    if (!InitS(tN)) return 0;

    m_db->AddNew();

    int n1;
    CCoordList cl2;

    cl2.push_back(pt1);
    cl2.push_back(pt2);

    write_cl(tN, cl2);

    m_db->write("Text", text);

    m_db->Update();

    m_db->rs->SetBookmark(m_db->rs->GetLastModifiedBookmark());

    m_db->read_first(n1);

    if (n_shape != -1) {
        if (m_db->openTable(m_kls->xy_table)) {

            CFRect rect = cl2.GetFRect();

            double gs = m_kls->IdxGridSize;

            double x1 = rect.left / gs / 100;
            double x2 = rect.right / gs / 100;
            double y1 = -rect.bottom / gs / 100;
            double y2 = -rect.top / gs / 100;

            try {
                m_db->rs->AddNew();
                m_db->rs->SetFieldValue(0, COleVariant((long)n1));
                m_db->rs->SetFieldValue(1, COleVariant((long)x1));
                m_db->rs->SetFieldValue(2, COleVariant((long)y1));
                m_db->rs->SetFieldValue(3, COleVariant((long)x2));
                m_db->rs->SetFieldValue(4, COleVariant((long)y2));

                m_db->rs->Update();
            }
            catch (CDaoException* e) {
                e->Delete();
            }

            m_db->closeTable();
        }
    }

    CGeoObject* g = new CGeoObject;

    g->nom = n1;
    g->nom2 = n1;
    g->mark = 0;
    g->rect = cl2.GetFRect();
    m_kls->geo4.push_back(g);

    return m_kls->geo4.size();
#endif
    return 0;
}

bool CGeoFile::GetFRect(const Klassif* kls, int n, CFRect& rect)
{
    //  rect = kls->geo4[n-1].rect;
    rect = conv(kls->geo4[n - 1]->rect);
    return true;
};

int loc2typ2(int loc)
{
    int typ = 4;

    if (loc == LOC3) {
        typ = 4;
    }
    else if (loc == LOC2 || loc == LOC12) {
        typ = 3;
    }
    else if (loc == LOC1) {
        typ = 1;
    }
    return typ;
}



CString getRusName(const char* tn)
{
    Klassif* kls = m_geofile->m_kl_list->findKlN(tn);

    if (kls) {
        return kls->RusName();
    }

    return tn;
}



bool CGeoFile::SetFRect(const Klassif* kls, int n, const CFRect& rect) {
    if (n > 1) kls->geo4[n - 1]->rect = rect;
    return true;
}

int CGeoFile::IsMark(const Klassif* kls, int n)
{
    return kls->geo4[n - 1]->mark;
}

int CGeoFile::IsVisible(const Klassif* kls, int n)
{
    return kls->geo4[n - 1]->is_visible;
}


bool CGeoFile::GetFRect(const Klassif* kls, CFRect& rect) {
    return GetFRect(kls, m_nCur, rect);
}

bool CGeoFile::SetFRect(const Klassif* kls, const CFRect& rect)
{
    return SetFRect(kls, m_nCur, rect);
}


int CGeoFile::GetNom(const Klassif* kls, int n) {
    return kls->geo4[n - 1]->nom;
}

void CGeoFile::SetMark(Klassif* kls, int n, bool mark /* = true */) {
    kls->geo4[n - 1]->mark = mark;
}

void CGeoFile::CGeoFile::SetNom2(Klassif* kls, int n, int nn)
{
    kls->geo4[n - 1]->nom2 = nn;
}

void CGeoFile::RevMark(Klassif* kls, int n) {
    kls->geo4[n - 1]->mark = !kls->geo4[n - 1]->mark;
}

#include "ado.h"


void CGeoFile::setVisible(const Klassif* kls, const char* _q)
{
    CString q = _q;

    if (!kls) return;
    int i, n = kls->geo4.size();

    for (i = 0; i < n; i++) {
        kls->geo4[i]->is_visible = false;
    }

    CAdoFile* ado = kls->m_ado;


    void split_sql_expressions(CString text, std::list<CString> &list_q);

    std::list<CString> list_q;

    split_sql_expressions(q, list_q);

    if (list_q.size() > 1) {
        q = list_q.back();
        list_q.pop_back();

        for (auto& q1 : list_q) {
            ado->Execute(q1);
        }
    }

    CString qq;

    qq.Format("SELECT id FROM (%s\n) _TT11TT11", q);

    set<int> set_id;

    if (ado->openTable0(qq)) {
        while (!ado->isEOF()) {
            long id = ado->read_long("id");
            set_id.insert(id);

            ado->MoveNext();
        }
        ado->closeTable();
    }

    i = 0;

    for (auto it : set_id) {
        long id = it;

        for (; i < n && kls->geo4[i]->nom < id; i++) {
        }
        if (kls->geo4[i]->nom == id) {
            kls->geo4[i]->is_visible = true;
        }
    }
}

#if 0

// Выключить объекты, которые не находятся на выбранных участках

void CGeoFile::setVisible2(const Klassif* kls, const char* q)
{
    if (!kls) return;
    int i, n = kls->geo4.size();

//    for (i = 0; i < n; i++) {
//        kls->geo4[i]->is_visible = false;
//    }

    CAdoFile* ado = kls->m_ado;

    CString qq;

    //    qq.Format("SELECT id FROM (%s\n) _TT11TT11 ORDER BY id", q);
    qq.Format("SELECT id FROM (%s\n) _TT11TT11", q);

    set<int> set_id;

    if (ado->openTable0(qq)) {
        while (!ado->isEOF()) {
            long id = ado->read_long("id");
            set_id.insert(id);
            ado->MoveNext();
        }
        ado->closeTable();
    }

    i = 0;

    for (auto it : set_id) {
        long id = it;

        for (; i < n && kls->geo4[i]->nom < id; i++) {
            kls->geo4[i]->is_visible = false;
        }
        if (kls->geo4[i]->nom == id) {
            kls->geo4[i]->is_visible = true;
        }
    }
    for (;kls->geo4[i]->nom < id; i++) {
        kls->geo4[i]->is_visible = false;
    }
}

#endif


void CGeoFile::setVisibleColor(const Klassif* kls, const char* q)
{
    if (!kls) return;
    int i, n = kls->geo4.size();

    for (i = 0; i < n; i++) {
        kls->geo4[i]->is_visible = false;
    }

    CAdoFile* ado = kls->m_ado;

    CString qq;

    qq.Format("SELECT id, __color FROM (%s\n) _TT11TT11", q);

    map<int, int> map_id;

    if (ado->openTable0(qq)) {
        while (!ado->isEOF()) {
            long id = ado->read_long("id");
            long color = ado->read_long("__color");
            map_id[id] = color;

            ado->MoveNext();
        }
        ado->closeTable();
    }
    else {
        for (i = 0; i < n; i++) {
            kls->geo4[i]->is_visible = true;
        }
        return;
    }

    i = 0;

    for (auto it : map_id) {
        long id = it.first;

        for (; i < n && kls->geo4[i]->nom < id; i++) {
        }
        if (kls->geo4[i]->nom == id) {
            kls->geo4[i]->is_visible = true;
            kls->geo4[i]->cl.is_color = true;
            //            kls->geo4[i]->cl.color = it.second;
            kls->geo4[i]->cl.color = it.second;
        }
    }
}




void CGeoFile::setVisible(const char* tn, const char* q)
{
    Klassif* kls = m_kl_list->findKlN(tn);
    if (kls) {
        setVisible(kls, q);
    }
}

void CGeoFile::setVisibleAll(const Klassif* kls)
{
    if (!kls) return;
    int i, n = kls->geo4.size();

    for (i = 0; i < n; i++) {
        kls->geo4[i]->is_visible = true;
    }
}

void CGeoFile::setVisibleAllNo(const Klassif* kls)
{
    if (!kls) return;
    int i, n = kls->geo4.size();

    for (i = 0; i < n; i++) {
        kls->geo4[i]->is_visible = false;
    }
}



void CGeoFile::setVisibleAll(const char* tn)
{
    Klassif* kls = m_kl_list->findKlN(tn);
    if (kls) {
        setVisibleAll(kls);
    }
}

void CGeoFile::setVisibleAllNo(const char* tn)
{
    Klassif* kls = m_kl_list->findKlN(tn);
    if (kls) {
        setVisibleAllNo(kls);
    }
}


CString getTableDefect(int typ);


void CGeoFile::setVisibleAllRemonts()
{
    setVisibleAll("defect");
    setVisibleAll("remont");
    setVisibleAll("diag");

    //    setVisibleAll(REM_INDICATOR);


    /*
        setVisibleAll(getTableDefect(ID_VIEW_TOOLBAR_ELECTRO   ));
        setVisibleAll(getTableDefect(ID_VIEW_TOOLBAR_DEFECT    ));
        setVisibleAll(getTableDefect(ID_VIEW_TOOLBAR_SHURF     ));
        setVisibleAll(getTableDefect(TIP_DEFECT2               ));
        setVisibleAll(getTableDefect(TIP_SHURF2                ));
        setVisibleAll(getTableDefect(TIP_KORROZIA              ));
        setVisibleAll(getTableDefect(ID_VIEW_TOOLBAR_OPRESSOVKA));
        setVisibleAll(getTableDefect(ID_VIEW_TOOLBAR_KAPREMONT ));
        setVisibleAll(getTableDefect(ID_VIEW_TOOLBAR_CONTROL_TU));

        setVisibleAll(getTableDefect(ID_VIEW_TOOLBAR_REMONT ));
    */

}


