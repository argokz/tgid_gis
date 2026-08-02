#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"

#include "gid6Doc.h"
#include "gidrView.h"
#include "win.h"
#include "Podl.h"
#include "Bmp.h"


#define MAX_POINT 10000

CFPoint ScreenToCoord(const CPoint& point, ScrollGeom& geom) {
    return CFPoint(geom.masx * (geom.m_bx + point.x), geom.masy * (geom.m_by + point.y));
}

CFRect ScreenToCoord(const CRect& rect, ScrollGeom& geom)
{
    return CFRect(
        ScreenToCoord(rect.TopLeft(), geom),
        ScreenToCoord(rect.BottomRight(), geom));
}

CPoint CoordToScreen(const CFPoint& point, const ScrollGeom& geom);

void CoordToScreenCl(const CCoordList& cl, std::list< std::vector<CPoint> >& list_cl2, const ScrollGeom& geom);

std::vector<CPoint> clipPolygon(const std::vector<CPoint>& polygon, int screenWidth, int screenHeight);

void draw_layer3(CDC* m_dc, CRect &rect_scr, ScrollGeom & geom, Klassif* kls)
{
    CFRect rectScr = ScreenToCoord(rect_scr, geom);

    std::vector<int> pCounts;
    std::vector<CPoint> points;

    clock_t t1 = clock();

    log1("---------------------------------");
    log1("%s", kls->nazv);


    double width = rect_scr.Width()*geom.masx;
    double height = rect_scr.Height()*geom.masy;

    double wp = geom.masx*3;
    double hp = geom.masy*3;

    int col = kls->coli;
    int colb = kls->colb;

//    if (cl.is_color && colb != 0xFFFFFF) {
//        colb = cl.color;
//    }


    double mas_otn = geom.masx * geom.dmas / 500;

    int markThickness = 0;

//    if (isMark) {
//        markThickness = 4;
//    }


    double h = kls->typ / mas_otn + markThickness;

//    int h = kls_typ / mas_otn + markThickness;
    CBrush brush(rgb_l(colb));
    CPen pen(PS_SOLID, h, rgb_l(col));

//    setline(m_dc, col, kls_typ / mas_otn + markThickness);
    //setfillstyle(m_dc, 0, colb);

    CPen *oldpen = m_dc->SelectObject(&pen);
    CBrush *oldbrush = m_dc->SelectObject(&brush);

    for (int i = 0; i < kls->geo4.size(); i++) {
        CGeoObject *obj = kls->geo4[i];
        CFRect r, rect = obj->rect;

        if (rect.Width() == 0 && rect.Height() == 0) continue;
        if (rect.Width() < wp && rect.Height() < hp) continue;
        
        if (!r.IntersectRect(rect, rectScr)) continue;


        std::list< std::vector<CPoint> > list_cl2;

        CCoordList cl = obj->cl;
        cl.conv();

        CoordToScreenCl(cl, list_cl2, geom);

        for (auto& cl2 : list_cl2) {
            std::vector<CPoint> cl3 = clipPolygon(cl2, rectScr.Width(), rectScr.Height());
            if (cl3.size() >= 3) {

                if (points.size() > MAX_POINT && pCounts.size() > 1) {
                    log1("%s %d %d", kls->nazv, pCounts.size(), points.size());
                    m_dc->PolyPolygon(points.data(), pCounts.data(), pCounts.size());
                    points.clear();
                    pCounts.clear();
                }


                points.insert(points.end(), cl3.begin(), cl3.end());
                pCounts.push_back(cl3.size());
            }
        }
    }


    if (pCounts.size() > 0)  {
        log1("%s %d %d", kls->nazv, pCounts.size(), points.size());
        m_dc->PolyPolygon(points.data(), pCounts.data(), pCounts.size());
    }

    m_dc->SelectObject(oldpen);
    m_dc->SelectObject(oldbrush);

    clock_t t2 = clock();
    log1("%d %s - %g секунд  (1:%.0f)", kls->loc, kls->nazv, (double)(t2-t1) / CLOCKS_PER_SEC, geom.masx*geom.dmas);
}


void draw_layer2(CDC* m_dc, CRect &rect_scr, ScrollGeom & geom, Klassif* kls)
{
    CFRect rectScr = ScreenToCoord(rect_scr, geom);

    std::vector<DWORD> pCounts;
    std::vector<CPoint> points;

    clock_t t1 = clock();

    log1("---------------------------------");
    log1("%s", kls->nazv);


    double width = rect_scr.Width()*geom.masx;
    double height = rect_scr.Height()*geom.masy;

    double wp = geom.masx*3;
    double hp = geom.masy*3;

    int col = kls->coli;

    double mas_otn = geom.masx * geom.dmas / 500;

    int markThickness = 0;

//    if (isMark) {
//        markThickness = 4;
//    }


    double h = kls->typ / mas_otn + markThickness;

    CPen pen(PS_SOLID, h, rgb_l(col));

    CPen *oldpen = m_dc->SelectObject(&pen);

    for (int i = 0; i < kls->geo4.size(); i++) {
        CGeoObject *obj = kls->geo4[i];
        CFRect r, rect = obj->rect;

        if (rect.Width() == 0 && rect.Height() == 0) continue;
        if (rect.Width() < wp && rect.Height() < hp) continue;
        
        if (!r.IntersectRect(rect, rectScr)) continue;


        CCoordList cl = obj->cl;
        cl.conv();

        if (cl.size() < 2) continue;

        if (kls->loc == 3) {
            cl.push_back(cl.front());
        }

        std::list< std::vector<CPoint> > list_cl2;
        CoordToScreenCl(cl, list_cl2, geom);

        for (auto& cl2 : list_cl2) {
            std::vector<CPoint> cl3 = clipPolygon(cl2, rectScr.Width(), rectScr.Height());
            if (cl3.size() >= 3) {

                if (points.size() > MAX_POINT && pCounts.size() > 1) {
                    log1("%s %d %d", kls->nazv, pCounts.size(), points.size());

                    m_dc->PolyPolyline(points.data(), pCounts.data(), pCounts.size());
                    points.clear();
                    pCounts.clear();
                }

                points.insert(points.end(), cl3.begin(), cl3.end());
                pCounts.push_back(cl3.size());
            }
        }
    }

    if (pCounts.size() > 0)  {
        log1("%s %d %d", kls->nazv, pCounts.size(), points.size());
        if (kls->loc == 3) {
            points.push_back(points[0]);
        }

        m_dc->PolyPolyline(points.data(), pCounts.data(), pCounts.size());
    }

    m_dc->SelectObject(oldpen);

    clock_t t2 = clock();
    log1("%d %s - %g секунд  (1:%.0f)", kls->loc, kls->nazv, (double)(t2 - t1) / CLOCKS_PER_SEC, geom.masx * geom.dmas);

}

void MyTextOut(CDC* m_dc, int x, int y, const char* s)
{
    m_dc->TextOut(x, y, s);
}


void DrawLabel3(CDC *m_dc, const CCoordList& cl, const char *text, double x, double y, double alpha, double coef, int loc, const Klassif* kls, ScrollGeom& geom, double mas_otn)
{
  CFRect rect;
  int i, j;

  LOGFONT lf = kls->lf;

  lf.lfEscapement = 1;
  lf.lfOrientation = 1;
  lf.lfEscapement = 0;
  lf.lfOrientation = 0;

  double m_h = (double)m_dc->GetDeviceCaps(LOGPIXELSX) / m_dc->GetDeviceCaps(LOGPIXELSX);
  double lh = lf.lfHeight*m_h*coef/mas_otn;
  
  lf.lfHeight = lh;

  if (text[0]) {
    CFont *oldfont, *font = new CFont();

    if (1/* || size.cx < l*/) {
      m_dc->SetTextAlign(TA_CENTER);
      m_dc->SetBkMode(TRANSPARENT);
      m_dc->SetTextColor(rgb_l(0));

      CPoint pt; // = CoordToScreen(cl.centroid());

      int nn = 0;

      for (i = 0; text[i]; i++) {
        if (text[i] == '\n') nn++; 
      }

      double cosalpha = cos(geom.m_alpha);
      double sinalpha = sin(geom.m_alpha);

      double x0 = x*cosalpha-y*sinalpha;
      double y0 = x*sinalpha+y*cosalpha;
      
      x = x0;
      y = y0;

      alpha += geom.m_alpha*180/M_PI;;

      lf.lfEscapement = -alpha*10;
      lf.lfOrientation = -alpha*10;

//      if (lf.lfEscapement == 0) lf.lfEscapement += 1;
//      if (lf.lfOrientation == 0) lf.lfOrientation += 1;

      font->CreateFontIndirect(&lf);
      oldfont = m_dc->SelectObject(font);

      CSize size = m_dc->GetTextExtent(text, lstrlen(text));

      if (loc == LOC1) {
        pt = CoordToScreen(*cl.begin()+CFPoint(x, y), geom);
        pt.x += -size.cy*sinalpha;
        pt.y += size.cy*cosalpha;
      }
      else {
        pt = CoordToScreen(cl.centroid()+CFPoint(x, y), geom);
      }

      double xxx = 0, yyy = 0;
      double sinp = sin(alpha*M_PI/180.);
      double cosp = cos(alpha*M_PI/180.);
      
      if (nn >= 0) {
        char ss[2560];

        m_dc->SetTextAlign(TA_LEFT);

        i = 0, j = 0; 
        do {
          ss[j++] = text[i];
          if (text[i] == '\n' || text[i] == 0) {
            if (text[i] == '\n') ss[j-1] = 0;
            size = m_dc->GetTextExtent(ss, lstrlen(ss));
            xxx = max(xxx, size.cx);
            yyy += size.cy;
            j = 0;
          } 
          i++;
        } while (text[i-1]);

        CPoint dx = CPoint(xxx*cosp, xxx*sinp);
        CPoint dy = CPoint(yyy*sinp, -yyy*cosp);
        CPoint dx2 = CPoint(xxx*cosp/2, xxx*sinp/2);
        CPoint dy2 = CPoint(yyy*sinp/2, -yyy*cosp/2);
        CPoint dy1 = CPoint(size.cy*sinp, -size.cy*cosp);

        pt = pt-dx2+dy2;

        CPoint pp[5];

        pp[0] = pt-dy;
        pp[1] = pt+dx-dy;
        pp[2] = pt+dx;
        pp[3] = pt;


/**
        if (kls->ramka || isPoint(loc)) {
          setfillstyle(m_dc, 0, m_bk_color);
          if (kls->ramka) {
            setline(m_dc, BLACK, 1.);
          }
          else {
            setline(m_dc, m_bk_color, 1.);
          }
          m_dc->Polygon(pp, 4);
        }
*/

        CPoint pt1 = pt-dy2;

        i = 0, j = 0; 
        do {
          ss[j++] = text[i];
          if (text[i] == '\n' || text[i] == 0) {
            if (text[i] == '\n') ss[j-1] = 0;
            MyTextOut(m_dc, pt.x, pt.y, ss);
            pt = pt-dy1;
            j = 0;
          } 
          i++;
        } while (text[i-1]);
      }
      else {
        MyTextOut(m_dc, pt.x, pt.y-size.cy/2, text);
      } 

      m_dc->SelectObject(oldfont);
      delete font;
    }
  }
}

void draw_layer3_labels(CDC* m_dc, CRect &rect_scr, ScrollGeom & geom, Klassif* kls)
{
    CFRect rectScr = ScreenToCoord(rect_scr, geom);

    clock_t t1 = clock();


    double width = rect_scr.Width()*geom.masx;
    double height = rect_scr.Height()*geom.masy;

    double wp = geom.masx*3;
    double hp = geom.masy*3;

    double mas_otn = geom.masx * geom.dmas / 500;

    LOGFONT lf = kls->lf;

    lf.lfEscapement = 1;
    lf.lfOrientation = 1;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;

    double m_h = (double)m_dc->GetDeviceCaps(LOGPIXELSX) / m_dc->GetDeviceCaps(LOGPIXELSX);


    double coef = 1.;
    double lh = lf.lfHeight*m_h*coef/mas_otn;
    
    lf.lfHeight = lh;

    if (fabs(lh) < MIN_FONT) return;

    log1("---------------------------------");
    log1("Label %s", kls->nazv);


    for (int i = 0; i < kls->geo4.size(); i++) {
        CGeoObject *obj = kls->geo4[i];
        CFRect r, rect = obj->rect;

        if (rect.Width() == 0 && rect.Height() == 0) continue;
        if (rect.Width() < wp && rect.Height() < hp) continue;
        
        if (!r.IntersectRect(rect, rectScr)) continue;

        double x = obj->p_text.x;
        double y = obj->p_text.y;
        double alpha = obj->alpha;
        double coef = obj->coef;
        CString text = obj->text00000;
        CString text_color = obj->text_color;
        int align = obj->align;
        if (coef <= 0.) coef = 1.;

        DrawLabel3(m_dc, obj->cl, text, x, y, alpha, coef, LOC3, kls, geom, mas_otn);
    }

    clock_t t2 = clock();
    log1("Label %d %s - %g секунд  (1:%.0f)", kls->loc, kls->nazv, (double)(t2 - t1) / CLOCKS_PER_SEC, geom.masx * geom.dmas);

}
