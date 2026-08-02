#include "stdafx.h"
#include "gid6.h"

#include "gid6Doc.h"
#include "gidrView.h"

#include "geofile.h"

#include "win.h"

#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CGeoFile::DrawLabel1_MI(CDC *m_dc, CScroll *scr, const CCoordList& cl, Klassif* kls)
{
  char s[256];

  if (scr->geom.masx*scr->geom.dmas > 2000) return;

  if (kls->n_text_string < 0) return;

  CSize size;


  LOGFONT lf = kls->lf;

  lf.lfEscapement = 1;
  lf.lfOrientation = 1;

//  CClientDC screenDC(NULL);
//  int dpi = min(DPIPR, m_dc->GetDeviceCaps(LOGPIXELSX));
//  lf.lfHeight = lf.lfHeight*dpi/screenDC.GetDeviceCaps(LOGPIXELSX)/scr->mas_otn_geotext;
//  lf.lfHeight = lf.lfHeight*scr->m_h/scr->mas_otn;

  double sz = read_size()*1000;
  lf.lfHeight = sz*scr->m_h/scr->mas_otn;


  if (abs(lf.lfHeight) > 2) {
    strcpy(s, read_textstring());
    if (s[0]) {

      double angle = read_angle();

      if (angle != 0) {
        lf.lfEscapement = angle*10;
        lf.lfOrientation = angle*10;
      }

      double sz = read_size();

      int nl  = 1;
      for (int j = 0; s[j]; j++) {
        if (s[j] == '\n') nl++;
      }

      if (sz != 0) {
        //CClientDC screenDC(NULL);
        //double dm = ((double)screenDC.GetDeviceCaps(LOGPIXELSX))/m_dc->GetDeviceCaps(LOGPIXELSX);
//        lf.lfHeight = sz*dpi/scr->masx/nl*dm;
        lf.lfHeight = sz*scr->m_h/scr->mas_otn;
      }

      if (lf.lfHeight > 2) {

        CFont *oldfont, *font = new CFont();

        font->DeleteObject();
        font->CreateFontIndirect(&lf);
        oldfont = m_dc->SelectObject(font);

        size = m_dc->GetTextExtent(s, lstrlen(s));

        m_dc->SetTextAlign(TA_LEFT|TA_BOTTOM);
        m_dc->SetBkMode(TRANSPARENT);
        m_dc->SetTextColor(0);
        CPoint pt = scr->CoordToScreen(cl.centroid());
        if (nl == 1) {
          m_dc->TextOut(pt.x, pt.y, s);
        }
        else {
          for (int j = 0; s[j]; j++) {
            if (s[j] == '\n') s[j] = 0;
          }
          m_dc->TextOut(pt.x, pt.y, s);
        }
        m_dc->SelectObject(oldfont);
        delete font;
      }
    }
  }
}

void CScroll::DrawLabel3(CDC *m_dc, const CCoordList& cl, const char *text, double x, double y, double alpha, double coef, int loc, const Klassif* kls)
{
  CFRect rect;
  int i, j;

  LOGFONT lf = kls->lf;

  lf.lfEscapement = 1;
  lf.lfOrientation = 1;
  lf.lfEscapement = 0;
  lf.lfOrientation = 0;

//  CClientDC screenDC(NULL);
//  int dpi = min(DPIPR, m_dc->GetDeviceCaps(LOGPIXELSX));
//  double lh = lf.lfHeight*dpi*coef/screenDC.GetDeviceCaps(LOGPIXELSX)/mas_otn_geotext;
  double lh = lf.lfHeight*m_h*coef/mas_otn;
  
  lf.lfHeight = lh;

  if (text[0] && fabs(lh) > MIN_FONT) {
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
        pt = CoordToScreen(*cl.begin()+CFPoint(x, y));
        pt.x += -size.cy*sinalpha;
        pt.y += size.cy*cosalpha;
      }
      else {
        pt = CoordToScreen(cl.centroid()+CFPoint(x, y));
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

/*
        if (kls->ramka) {
          if (loc == LOC1) {
            m_dc->Polygon(pp, 4);
          }
          else {
            pp[4] = pp[0]; 
            m_dc->Polyline(pp, 5);
          }
        }
*/
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



void CScroll::DrawLabel3_MI(CDC *m_dc, const CCoordList& cl, const char *text, double x, double y, double alpha, double coef, int loc, const Klassif *kls)
{
  CFRect rect;
  int i;

  LOGFONT lf = kls->lf;

  lf.lfEscapement = 1;
  lf.lfOrientation = 1;
  lf.lfEscapement = 0;
  lf.lfOrientation = 0;

//  CClientDC screenDC(NULL);
//  int dpi = min(DPIPR, m_dc->GetDeviceCaps(LOGPIXELSX));
//  double lh = lf.lfHeight*dpi*coef/screenDC.GetDeviceCaps(LOGPIXELSX)/mas_otn_geotext;
  double lh = lf.lfHeight*m_h*coef/mas_otn;
  
  lf.lfHeight = lh;

  if (text[0] && fabs(lh) > MIN_FONT) {
    CFont *oldfont, *font = new CFont();

    if (1/* || size.cx < l*/) {
      m_dc->SetTextAlign(TA_LEFT|TA_BOTTOM);
      m_dc->SetBkMode(TRANSPARENT);
      m_dc->SetTextColor(rgb_l(0));

      CPoint pt; // = CoordToScreen(cl.centroid());

      int nn = 0;

      for (i = 0; text[i]; i++) {
        if (text[i] == '\n') nn++; 
      }

      double cosalpha = cos(geom.m_alpha);
      double sinalpha = sin(geom.m_alpha);


      alpha += geom.m_alpha*180/M_PI;;

      lf.lfEscapement = alpha*10;
      lf.lfOrientation = alpha*10;


      font->CreateFontIndirect(&lf);
      oldfont = m_dc->SelectObject(font);

      CSize size = m_dc->GetTextExtent(text, lstrlen(text));

      if (loc == LOC1) {
        pt = CoordToScreen(*cl.begin());
      }
      else {
        pt = CoordToScreen(cl.centroid());
      }


      double xxx = 0, yyy = 0;
      
      MyTextOut(m_dc, pt.x, pt.y, text);

      m_dc->SelectObject(oldfont);
      delete font;
    }
  }
}





CSize getSize(CDC *m_dc, const char *text, int &n)
{
  char ss[1024];
  int i = 0, j = 0;
  CSize size(0, 0);

  n = 0;

  do {
    ss[j++] = text[i];
    ss[j] = 0;
    if (text[i] == '\n') {
      ss[j] = 0;
    }
    if (text[i] == 0 || text[i] == '\n') {
      CSize sz = m_dc->GetTextExtent(ss);
      size.cx = max(size.cx, sz.cx);
      if (text[i] == '\n' || ss[0]) size.cy += sz.cy;
      n ++;
      j = 0;
    }
    i++;
  } while (text[i-1] != 0);

  return size;
}

#ifdef DEMO_DNEPR 


// Днепропетровск

void CScroll::DrawLabel2(CDC *m_dc, const CCoordList& cl, const char *text, short align, double x, double y, double alpha, double coef, const Klassif *kls)
{
  CFRect rect;
  double x1, y1, x2, y2;
  int u;
  CFPoint p1, p2;

  p1 = cl.first();
  p2 = cl.last();

  cl.napr(x1, y1, x2, y2);

  double dx = (x2-x1)/hypot(x2-x1, y2-y1);
  double dy = (y2-y1)/hypot(x2-x1, y2-y1);

  CPoint pt = CoordToScreen(CFPoint((x1+x2)/2, (y1+y2)/2));

  double uu = atan2(y2-y1, x2-x1);

  u = -uu*1800./M_PI;

  LOGFONT lf = kls->lf;
//  COLORREF color = kls->color;
  COLORREF color = 0;

//  setcolor(0);

  CClientDC screenDC(NULL);

  CSize size0 = m_dc->GetTextExtent(text, lstrlen(text));

  lf.lfEscapement = u;
  lf.lfOrientation = u;
  lf.lfHeight = lf.lfHeight*m_h/mas_otn;

//  if (abs(lf.lfHeight) > 2) {
  if (abs(lf.lfHeight) > 1) {

    if (1/* || size.cx < hypot(x2-x1, y2-y1)/masx*2.*/) {
      CFont *oldfont, *font = new CFont();

      lf.lfEscapement = 0;
      lf.lfOrientation = 0;

      font->DeleteObject();
      font->CreateFontIndirect(&lf);
      oldfont = m_dc->SelectObject(font);

      int n = 0;

      CSize size = getSize(m_dc, text, n);

      pt.x -= dx*size.cx/2;
      pt.y -= dy*size.cx/2;

      if (n > 1) {
        pt.x -= dy*size.cy/2;
        pt.y -= dx*size.cy/2;
      }


      CPoint pt0 = pt;

      if (kls->ramka) {
        double cx = size.cx+1./mas_otn;
        double cy = size.cy+1./mas_otn;

        CPoint dpx = CPoint(cx*dx, cx*dy);
        CPoint dpy = CPoint(-cy*dy, cy*dx);

        CPoint pp[5];

        pp[0] = pt0;
        pp[1] = pt0+dpx;
        pp[2] = pt0+dpx+dpy;
        pp[3] = pt0+dpy;
        pp[4] = pt0;

        m_dc->Polygon(pp, 5);
      }

      lf.lfEscapement = u;
      lf.lfOrientation = u;

      font->DeleteObject();
      font->CreateFontIndirect(&lf);

      oldfont = m_dc->SelectObject(font);
      
//      m_dc->SetTextAlign(TA_CENTER | TA_TOP);
//      m_dc->SetTextAlign(TA_LEFT | TA_TOP);

      m_dc->SetTextAlign(TA_CENTER | TA_BOTTOM);

      m_dc->SetBkMode(TRANSPARENT);

      char ss[1024];
      int i = 0, j = 0;

      do {
        ss[j++] = text[i];
        ss[j] = 0;
        if (text[i] == '\n') {
          ss[j] = 0;
        }
        if (text[i] == 0 || text[i] == '\n') {
          m_dc->TextOut(pt.x, pt.y, ss);
          pt.x = pt.x+dy*lf.lfHeight;
          pt.y = pt.y-dx*lf.lfHeight;
          j = 0;
        }
        i++;
      } while (text[i-1] != 0);

      m_dc->SelectObject(oldfont);
      delete font;
    }
  }
}



#else


void CScroll::DrawLabel2(CDC *m_dc, const CCoordList& cl, const char *text, short align, double x, double y, double alpha, double coef, const Klassif *kls)
{
  CFRect rect;
  double x1, y1, x2, y2;
  int u;
  CFPoint p1, p2;

  p1 = cl.first();
  p2 = cl.last();

  cl.napr(x1, y1, x2, y2);

  double dx = (x2-x1)/hypot(x2-x1, y2-y1);
  double dy = (y2-y1)/hypot(x2-x1, y2-y1);

  CPoint pt = CoordToScreen(CFPoint((x1+x2)/2, (y1+y2)/2));

  double uu = atan2(y2-y1, x2-x1);

  u = -uu*1800./M_PI;

  LOGFONT lf = kls->lf;
//  COLORREF color = kls->color;
  COLORREF color = 0;

//  setcolor(0);

  CClientDC screenDC(NULL);

  CSize size0 = m_dc->GetTextExtent(text, lstrlen(text));

  lf.lfEscapement = u;
  lf.lfOrientation = u;
  lf.lfHeight = lf.lfHeight*m_h/mas_otn;

//  if (abs(lf.lfHeight) > 2) {
  if (abs(lf.lfHeight) > 1) {

    if (1/* || size.cx < hypot(x2-x1, y2-y1)/masx*2.*/) {
      CFont *oldfont, *font = new CFont();

      lf.lfEscapement = 0;
      lf.lfOrientation = 0;

      font->DeleteObject();
      font->CreateFontIndirect(&lf);
      oldfont = m_dc->SelectObject(font);

      int n = 0;

      CSize size = getSize(m_dc, text, n);

      pt.x -= dx*size.cx/2;
      pt.y -= dy*size.cx/2;

      if (n > 1) {
        pt.x -= dy*size.cy/2;
        pt.y -= dx*size.cy/2;
      }


      CPoint pt0 = pt;

      if (kls->ramka) {
        double cx = size.cx+1./mas_otn;
        double cy = size.cy+1./mas_otn;

        CPoint dpx = CPoint(cx*dx, cx*dy);
        CPoint dpy = CPoint(-cy*dy, cy*dx);

        CPoint pp[5];

        pp[0] = pt0;
        pp[1] = pt0+dpx;
        pp[2] = pt0+dpx+dpy;
        pp[3] = pt0+dpy;
        pp[4] = pt0;

        m_dc->Polygon(pp, 5);
      }

      lf.lfEscapement = u;
      lf.lfOrientation = u;

      font->DeleteObject();
      font->CreateFontIndirect(&lf);

      oldfont = m_dc->SelectObject(font);
      
      m_dc->SetTextAlign(TA_CENTER | TA_TOP);
      m_dc->SetTextAlign(TA_LEFT | TA_TOP);

      m_dc->SetBkMode(TRANSPARENT);

      char ss[1024];
      int i = 0, j = 0;

      do {
        ss[j++] = text[i];
        ss[j] = 0;
        if (text[i] == '\n') {
          ss[j] = 0;
        }
        if (text[i] == 0 || text[i] == '\n') {
          m_dc->TextOut(pt.x, pt.y, ss);
          pt.x = pt.x+dy*lf.lfHeight;
          pt.y = pt.y-dx*lf.lfHeight;
          j = 0;
        }
        i++;
      } while (text[i-1] != 0);

      m_dc->SelectObject(oldfont);
      delete font;
    }
  }
}

#endif


void CScroll::DrawLabelMulty1(CDC *m_dc, const CCoordList& cl, const char *text, int v)
{
  CFRect rect;

  double len = cl.Length();

  double h = fabs(m_lf_mag.lfHeight);

//  double dlen = len/10;
//  double dlen = 100;
  double dlen = h*5;

  if (len < h) return;

  if (dlen > len) {
    dlen = len;
  }

  double plen = (len-floor(len/dlen)*dlen)/2;

  if (plen < dlen/2) plen = dlen/2;

//  plen = 0;

  CCoordList::const_iterator it; 
  it = cl.begin();

  const CFPoint *p1 = &*it;

  ++it;

  for (  ; it != cl.end(); it++) {
    const CFPoint *p2 = &*it;

    double l = Length(*p1, *p2);

    if (!isBreakCoord(p1->x) && !isBreakCoord(p2->x)) {
        while (l > plen) {
          CFPoint p;

          p = *p1 + CFPoint((p2->x-p1->x)*plen/l,(p2->y-p1->y)*plen/l);

          plen += dlen;

          CFPoint pp1, pp2;

          if (p2->x > p1->x) {
            pp1 = p - CFPoint((p2->x-p1->x),(p2->y-p1->y));
            pp2 = p + CFPoint((p2->x-p1->x),(p2->y-p1->y));
          }
          else {
            pp1 = p + CFPoint((p2->x-p1->x),(p2->y-p1->y));
            pp2 = p - CFPoint((p2->x-p1->x),(p2->y-p1->y));
          }
          DrawLabelMulty2(m_dc, pp1, pp2, text, v);
        }
    }

    plen -= l;
    p1 = p2;
  }
}




void CScroll::DrawLabelMulty(CDC *m_dc, const CCoordList& cl, const char *text, int v)
{
  CCoordList cl2;

  CCoordList::const_iterator it; 

  for ( it = cl.begin(); it != cl.end(); it++) {
    const CFPoint *p = &*it;
    if (!isBreakCoord(it->x)) {
      cl2.push_back(*p);
    }
    else {
      DrawLabelMulty1(m_dc, cl2, text, v);
      cl2.clear();
    }
  }
  DrawLabelMulty1(m_dc, cl2, text, v);
}

#if 0

void CScroll::DrawLabel2(CDC *m_dc, const CCoordList& cl, const char *text)
{
  CFRect rect;
//  double x1, y1, x2, y2;
//  int u;

  double len = cl.Length();

  double h = fabs(m_lf_mag.lfHeight);

//  double dlen = len/10;
//  double dlen = 100;
  double dlen = h*5;

  if (len < h) return;

  if (dlen > len) {
    dlen = len;
  }


  double plen = (len-floor(len/dlen)*dlen)/2;

  if (plen < dlen/2) plen = dlen/2;


//  plen = 0;

  CCoordList::const_iterator it; 
  it = cl.begin();

  const CFPoint *p1 = &*it;

  ++it;

  for (  ; it != cl.end(); it++) {
    const CFPoint *p2 = &*it;

    if (isBreakCoord(it->x)) {
      while (it != cl.end() && isBreakCoord(it->x)) {
        it++; 
        p2 = &*it;
      }
      it++;

      p1 = p2;

      p2 = &*it;
      while (it != cl.end() && isBreakCoord(it->x)) {
        it++; 
        p2 = &*it;
      }
      if (it != cl.end()) {
        break;
      }
    }




    double l = Length(*p1, *p2);

    while (l > plen) {
      CFPoint p;

      p = *p1 + CFPoint((p2->x-p1->x)*plen/l,(p2->y-p1->y)*plen/l);

      plen += dlen;

      CFPoint pp1, pp2;


      if (p2->x > p1->x) {
        pp1 = p - CFPoint((p2->x-p1->x),(p2->y-p1->y));
        pp2 = p + CFPoint((p2->x-p1->x),(p2->y-p1->y));
      }
      else {
        pp1 = p + CFPoint((p2->x-p1->x),(p2->y-p1->y));
        pp2 = p - CFPoint((p2->x-p1->x),(p2->y-p1->y));
      }

      DrawLabel2(m_dc, pp1, pp2, text);

/* *
      CPoint pp = CoordToScreen(p);
      setcolor(m_dc, 4);
      line(m_dc, pp.x-5, pp.y-5, pp.x+5, pp.y+5);
      line(m_dc, pp.x+5, pp.y-5, pp.x-5, pp.y+5);
/* */

    }
    plen -= l;

    p1 = p2;
  }
}



#endif



void CScroll::DrawLabelMulty2(CDC *m_dc, CFPoint p1, CFPoint p2, const char *text, int vv)
{

  double x1 = p1.x;
  double y1 = p1.y;
  double x2 = p2.x;
  double y2 = p2.y;

  double dx = (x2-x1)/hypot(x2-x1, y2-y1);
  double dy = (y2-y1)/hypot(x2-x1, y2-y1);

  CPoint pt = CoordToScreen(CFPoint((x1+x2)/2, (y1+y2)/2));
  CPoint pt0 = pt;

  pt.x = pt.x+dy*2/mas_otn;
  pt.y = pt.y-dx*2/mas_otn;


  double uu = atan2(y2-y1, x2-x1);

  double u = -uu*1800./M_PI;


  LOGFONT lf;

  memcpy(&lf, &m_lf_mag, sizeof(LOGFONT));
  
  COLORREF color = m_color_mag;

  CClientDC screenDC(NULL);

  CSize size = m_dc->GetTextExtent(text, lstrlen(text));

  lf.lfEscapement = u;
  lf.lfOrientation = u;

//  lf.lfHeight = 10;

  double dm = m_dc->GetDeviceCaps(LOGPIXELSX)/2.54;
  double mas_otn = geom.masx*dm/(500);

  lf.lfHeight = lf.lfHeight*m_h/mas_otn;

  double w = Length(p1, p2)*100;
  double w2 = hypot(x1-x2, y1-y2);

  w /= geom.masx;
  w2 /= geom.masx;

  if (w < abs(lf.lfHeight)) {
//  return;
  }

  if (abs(lf.lfHeight) > 2) {
    if (1) {
      CFont *oldfont, *font = new CFont();
      lf.lfEscapement = u;
      lf.lfOrientation = u;

//      font->DeleteObject();
      font->CreateFontIndirect(&lf);

      oldfont = m_dc->SelectObject(font);
      
      m_dc->SetTextAlign(TA_CENTER | TA_BASELINE);


      m_dc->SetBkMode(TRANSPARENT);

      m_dc->SetTextColor(rgb_l(0));

      char ss[1024];
      int i = 0, j = 0;

 if (!vv) {
      pt.x = pt.x+dy*lf.lfHeight*1.2;
      pt.y = pt.y-dx*lf.lfHeight*1.2;
  }
  else {
      pt.x = pt.x-dy*lf.lfHeight*0.2;
      pt.y = pt.y+dx*lf.lfHeight*0.2;
  }


      do {
        ss[j++] = text[i];
        ss[j] = 0;
        if (text[i] == '\n') {
          ss[j] = 0;
        }
        if (text[i] == 0 || text[i] == '\n') {
          m_dc->TextOut(pt.x, pt.y, ss);

          pt.x = pt.x+dy*lf.lfHeight;
          pt.y = pt.y-dx*lf.lfHeight;

          j = 0;
        }
        i++;
      } while (text[i-1] != 0);

      m_dc->SelectObject(oldfont);
      delete font;
    }
  }
}

int width_txt(CDC *m_dc, const char *text, LOGFONT &lf)
{
  CFont *oldfont, font;
  font.CreateFontIndirect(&lf);
  oldfont = m_dc->SelectObject(&font);
  CSize size = m_dc->GetTextExtent(text, lstrlen(text));
  m_dc->SelectObject(oldfont);

  return size.cx;
}

/// Для Караганды


void CScroll::DrawLabel22(CDC *m_dc, const CCoordList& cl, const char *text, short align, double x, double y, double alpha, double coef, const Klassif *kls)
{
  CFRect rect;
  double x1, y1, x2, y2;
  int u;
  CFPoint p1, p2;

  p1 = cl.first();
  p2 = cl.last();
       
  x1 = p1.x;
  y1 = p1.y;
  x2 = p2.x;
  y2 = p2.y;

  double h = hypot(x2-x1, y2-y1);

  if (h/ geom.masx < 2*strlen(text)) return;

  double dx = (x2-x1)/h;
  double dy = (y2-y1)/h;

  CPoint pt = CoordToScreen(CFPoint((x1+x2)/2, (y1+y2)/2));

  CPoint pt0 = pt;

  pt.x = pt.x+dy*2/mas_otn;
  pt.y = pt.y-dx*2/mas_otn;

  double uu = atan2(y2-y1, x2-x1);

  u = -uu*1800./M_PI;

  LOGFONT lf = kls->lf;

//  lf.lfHeight = lf.lfHeight;

  int w = width_txt(m_dc, text, lf);

  CSize size = m_dc->GetTextExtent(text, lstrlen(text));

  lf.lfEscapement = u;
  lf.lfOrientation = u;
  
  lf.lfHeight = lf.lfHeight*h/(w* geom.masx);

  if (abs(lf.lfHeight) > 2) {
//    if (1/* || size.cx < hypot(x2-x1, y2-y1)/masx*2.*/) {
      CFont *oldfont, *font = new CFont();

      lf.lfEscapement = u;
      lf.lfOrientation = u;

      font->DeleteObject();
      font->CreateFontIndirect(&lf);

      oldfont = m_dc->SelectObject(font);
      
      m_dc->SetTextAlign(TA_CENTER | TA_BASELINE);
      m_dc->SetBkMode(TRANSPARENT);
      m_dc->SetTextColor(rgb_l(0));

      char ss[1024];
      int i = 0, j = 0;

      do {
        ss[j++] = text[i];
        ss[j] = 0;
        if (text[i] == '\n') {
          ss[j] = 0;
        }
        if (text[i] == 0 || text[i] == '\n') {
          m_dc->TextOut(pt.x, pt.y, ss);

          pt.x = pt.x+dy*lf.lfHeight;
          pt.y = pt.y-dx*lf.lfHeight;

          j = 0;
        }
        i++;
      } while (text[i-1] != 0);

      m_dc->SelectObject(oldfont);
      delete font;
//    }
  }
}



void CScroll::DrawLabelMultyZoom(CDC *m_dc, CFPoint p1, CFPoint p2, const char *text, int vv)
{

  double x1 = p1.x;
  double y1 = p1.y;
  double x2 = p2.x;
  double y2 = p2.y;

  double dx = (x2-x1)/hypot(x2-x1, y2-y1);
  double dy = (y2-y1)/hypot(x2-x1, y2-y1);

  CPoint pt = CoordToScreen(CFPoint((x1+x2)/2, (y1+y2)/2));
  CPoint pt0 = pt;

  pt.x = pt.x+dy*2/mas_otn;
  pt.y = pt.y-dx*2/mas_otn;


  double uu = atan2(y2-y1, x2-x1);

  double u = -uu*1800./M_PI;


  LOGFONT lf;

  memcpy(&lf, &m_lf_mag, sizeof(LOGFONT));
  
  COLORREF color = m_color_mag;

  CClientDC screenDC(NULL);

  lf.lfHeight = 10;

  int wt = width_txt(m_dc, text, lf);

  if (wt <= 0) return;

  CSize size = m_dc->GetTextExtent(text, lstrlen(text));

  lf.lfEscapement = u;
  lf.lfOrientation = u;

  double w = Length(p1, p2)*100/ geom.masx;

  lf.lfHeight = lf.lfHeight * w/wt ;


  if (w < abs(lf.lfHeight)) {
//  return;
  }

  if (abs(lf.lfHeight) > 2) {
    if (1) {
      CFont *oldfont, *font = new CFont();
      lf.lfEscapement = u;
      lf.lfOrientation = u;

//      font->DeleteObject();
      font->CreateFontIndirect(&lf);

      oldfont = m_dc->SelectObject(font);
      
      m_dc->SetTextAlign(TA_CENTER | TA_BASELINE);


      m_dc->SetBkMode(TRANSPARENT);

      m_dc->SetTextColor(rgb_l(0));

      char ss[1024];
      int i = 0, j = 0;

 if (!vv) {
      pt.x = pt.x+dy*lf.lfHeight*1.2;
      pt.y = pt.y-dx*lf.lfHeight*1.2;
  }
  else {
      pt.x = pt.x-dy*lf.lfHeight*0.2;
      pt.y = pt.y+dx*lf.lfHeight*0.2;
  }


      do {
        ss[j++] = text[i];
        ss[j] = 0;
        if (text[i] == '\n') {
          ss[j] = 0;
        }
        if (text[i] == 0 || text[i] == '\n') {
          m_dc->TextOut(pt.x, pt.y, ss);

          pt.x = pt.x+dy*lf.lfHeight;
          pt.y = pt.y-dx*lf.lfHeight;

          j = 0;
        }
        i++;
      } while (text[i-1] != 0);

      m_dc->SelectObject(oldfont);
      delete font;
    }
  }
}
