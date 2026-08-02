// RastrWnd2.cpp : implementation file
//

#include "stdafx.h"
#include "gid6.h"
#include "RastrWndVect.h"
#include "win.h"
#include "Klassif.h"
#include "kltypVect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRastrWndVect


const int *get_pics_file(int typ, double coef);


static map <int, int*> map_pics;

const int *get_pics_map(int typ)
{
  map <int, int*>::const_iterator it = map_pics.find(typ);

  if (it != map_pics.end()) {
    return it->second;
  }
  return NULL;
}

void init_pics(int n)
{
  const int *pics = get_pics_file(n, 1);
  if (pics) {
    int *pics2 = new int[1024];

    memmove(pics2, pics, 1024*sizeof(int));

    map_pics[n] = pics2;
  }
}

void init_pics()
{
  BOOL bWorking;
  ChDir cd(argpath()+"pics");

  CFileFind finder;

  bWorking = finder.FindFile(_T("*.pic"));
  while (bWorking) {  
    bWorking = finder.FindNextFile();

    CString str = finder.GetFileTitle();

    int n = atoi(str.Mid(2));
    init_pics(n);
/*
    
    const int *pics = get_pics_file(n, 1);
    if (pics) {
      int *pics2 = new int[1024];

      memmove(pics2, pics, 1024*sizeof(int));

      map_pics[n] = pics2;
    }
*/
  } 
}



CRastrWndVect::CRastrWndVect() : CBgi()
{
  m_typ = 1;

  m_regim = RV_INIT;
  m_figura = TIP_LINE;
}

CRastrWndVect::~CRastrWndVect()
{
}


BEGIN_MESSAGE_MAP(CRastrWndVect, CBgi)
  //{{AFX_MSG_MAP(CRastrWndVect)
  ON_WM_PAINT()
  ON_WM_MOUSEMOVE()
  ON_WM_MOUSEACTIVATE()
  ON_WM_LBUTTONDOWN()
  ON_WM_RBUTTONDOWN()
  ON_WM_LBUTTONDBLCLK()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRastrWndVect message handlers

void RectanV(CDC *dc, int x, int y, int col);


static int m_dd = 12;

static int m_xx = 42, m_yy = 48;

int rgb_l(int color, int tr, int bk);
void chXY(double &x, double &y, double x1, double y1, double x2, double y2, double mas, double dx, double dy);


void decode(const int *pics, list <FRAG> &m_frag, int beg)
{
  double dd, dx, dy, x01, y01, y02, x02;
  double xx1, yy1, xx2, yy2, xx3, yy3, xx4, yy4;
  int t, ip, c_color, c_color_bk;
  double xx, yy, r;

  m_frag.clear();

  if (beg) {
    x01 = 1; 
  }
  else {
    x01 = m_dd*m_xx/2+1; 
  }
  
  y01 = m_yy*m_dd/2+1;
  x02 = m_dd*m_xx+1; 
  y02 = m_yy*m_dd/2+1;

  dd = hypot(x02-x01, y02-y01);  if (dd < 1.) return;
  dx = (x02-x01)/dd;
  dy = (y02-y01)/dd;


  CCoordList cl;
 
  ip = 0;

  while (1) {
    t = pics[ip++];

    if (t == 'q') break;

    switch (t)
    {
    case 'f' :
    case 'l' :
      {
        FRAG frag;
        while (true) {
          xx = pics[ip++]; if (xx == 9999) break;
          yy = pics[ip++];
          chXY(xx, yy, x01, y01, x02, y02, 1./m_dd, dx, dy);
          frag.cl.push_back(CFPoint(xx, yy));
        }
        if (t == 'f')
            frag.typ = TIP_POLYGON;
        else
            frag.typ = TIP_LINE;

        frag.color = c_color;
        frag.color_bk = c_color_bk;

        m_frag.push_back(frag);
      }
      break;

    case 'a' :
      xx = pics[ip++];
      yy = pics[ip++];
//      line (m_dc, x1, y1, x + dy*yy + dx*xx, y - dx*yy + dy*xx);
      break;

    case 'b' :
      xx = pics[ip++];
      yy = pics[ip++];

//      line (m_dc, x2, y2, x + dy*yy + dx*xx, y - dx*yy + dy*xx);
      break;

    case 'k' :  // Круг
    case 'r' : //  Окружность
      {
        FRAG frag;
        xx = pics[ip++];
        yy = pics[ip++];
        chXY(xx, yy, x01, y01, x02, y02, 1./m_dd, dx, dy);

        r = pics[ip++];

        frag.cl.push_back(CFPoint(xx, yy));
        frag.cl.push_back(CFPoint(xx, yy+r*m_dd));


        if (t == 'k')
            frag.typ = TIP_CIRCLE;
        else
            frag.typ = TIP_RING;

        frag.color = c_color;
        frag.color_bk = c_color_bk;

        m_frag.push_back(frag);
      }

//      fillellipse(m_dc, xx, yy, r/mas, r/mas);
      break;

    case 'c' :   // Цвет
      c_color = pics[ip++];
      c_color_bk = pics[ip++];
      break;

    case 'd' :   // Дуга
      xx = pics[ip++];
      yy = pics[ip++];
      chXY(xx, yy, x01, y01, x02, y02, 1./m_dd, dx, dy);

      r = pics[ip++]*m_dd;

      xx1 = xx-r;
      yy1 = yy-r;
      xx2 = xx+r;
      yy2 = yy+r;
      xx3 = pics[ip++];
      yy3 = pics[ip++];
      chXY(xx3, yy3, x01, y01, x02, y02, 1./m_dd, dx, dy);

      xx4 = pics[ip++];
      yy4 = pics[ip++];
      chXY(xx4, yy4, x01, y01, x02, y02, 1./m_dd, dx, dy);

      {
        FRAG frag;
        frag.cl.push_back(CFPoint(xx1, yy1));
        frag.cl.push_back(CFPoint(xx2, yy2));
        frag.cl.push_back(CFPoint(xx3, yy3));
        frag.cl.push_back(CFPoint(xx4, yy4));

        frag.typ = TIP_ARC;
        frag.color = c_color;
        frag.color_bk = c_color_bk;
        
        m_frag.push_back(frag);
      
      }

//      m_dc->Arc(xx1, yy1, xx2, yy2, xx3, yy3, xx4, yy4);

      break;
    }
  }
}


static int buf[1024*10];

const int *get_pics_file(int typ, double coef)
{
  CString fn;
  fn.Format("%s/pics/qq%03d.pic", argpath(), typ);

  buf[0] = 'q';

  FILE *f = fopen(fn, "rb");

  if (f) {
    fread(buf, 1024, 1, f);
    fclose(f);
    return (const int *) buf;
  }

  return NULL;
}

void CRastrWndVect::OnInitialUpdate() 
{
  CBgi::OnInitialUpdate();

//  init();
}

const int *get_pics(int typ, double coef);
const int *get_pics_file(int typ, double coef);
const int *get_pics_map(int typ);

int typ_typ(int n);

void CRastrWndVect::clear()
{
  m_frag.clear();
  Invalidate();
  UpdateWindow();
}

void CRastrWndVect::init(int typ)
{
//  int typ1 = typ_typ(typ-1);

  const int *pics = get_pics_map(typ); 
  if (!pics) {
    pics = get_pics(typ, 1); 
//    pics = get_pics(typ_typ(typ-1), 1); 
  }
  if (pics) decode(pics, m_frag, 0);

  m_typ = typ;
}

void CRastrWndVect::restore()
{
  const int *pics = get_pics(m_typ, 1); 
  if (pics) decode(pics, m_frag, 0);
  Invalidate();
}

//void CRastrWndVect::OnPaint() 
void CRastrWndVect::OnDraw(CDC* pDC)
{
//  CPaintDC dc(this); // device context for painting

  CRect rect1;
  pDC->SetBkColor(0xFFFFFF);
  GetClientRect(rect1);
  CBrush brush(0xFFFFFF);
  pDC->FillRect(rect1, &brush);

  CRect rect;
  GetClientRect(rect);

  setline(pDC, 0, 1);

  for ( int x = 0; x <= m_xx; x++ ) {
    for ( int y = 0; y <= m_yy; y++ ) {
      putpixel(pDC, x*m_dd+1, y*m_dd+1, 0);
    }
  }

  setline(pDC, RED, 1);
  line(pDC, 1, m_yy*m_dd/2+1, m_dd*m_xx+1, m_yy*m_dd/2+1);
  line(pDC, m_dd*m_xx/2+1, 1, m_dd*m_xx/2+1, m_yy*m_dd+1);

  int fill[256];

  setfillstyle(pDC, 1, 0);
  setline(pDC, 0, 3);

  list <FRAG>::const_iterator it;


  for (it = m_frag.begin(); it != m_frag.end(); it++) {
    int i;
    CCoordList::const_iterator it2;

    for (i = 0, it2 = it->cl.begin(); it2 != it->cl.end(); it2++) {
      fill[i++] = it2->x;
      fill[i++] = it2->y;
    }

    CFPoint pt1 = it->cl.first();
    CFPoint pt2 = it->cl.second();

    int R = sqrt(pow(pt2.x-pt1.x, 2)+pow(pt2.y-pt1.y, 2));
    R = okr3(R, m_dd, true);

    int c_color;
    int colorF = 0xFFFF;

    switch (it->color) {
    case 'C' :  {
      c_color = rgb_l(color, 30, 0xFFFFFF);  break;
    }

    case 'c' :  c_color = color;  break;
    case 'b' :  c_color = BLACK;  break;
    case 'w' :  c_color = WHITE;  break;
    case 'x' :  setfillstyle(pDC, 1, colorF);  break;
    }
    switch (it->color_bk) {
    case 'C' :  
      {
        setfillstyle(pDC, 1, rgb_l(color, 30, 0xFFFFFF));  
      }
      break;
    case 'c' :  setfillstyle(pDC, 1, color);  break;
    case 'b' :  setfillstyle(pDC, 1, BLACK);  break;
    case 'w' :  setfillstyle(pDC, 1, WHITE);  break;
    case 'x' :  setfillstyle(pDC, 1, colorF); break;
    }

    switch(it->typ) {
    case TIP_LINE:
      drawpoly(pDC, i/2, fill);
      break;
    case TIP_POLYGON:
      fillpoly(pDC, i/2, fill);
      break;
    case TIP_RING:
      ellipse(pDC, pt1.x, pt1.y, R, R);
      break;
    case TIP_CIRCLE:
      fillellipse(pDC, pt1.x, pt1.y, R, R);
      break;
    case TIP_ARC:
      pDC->Arc(fill[0],fill[1],fill[2],fill[3],fill[4],fill[5],fill[6],fill[7]);
      break;
    default:
      break;
    }
  }
}

COLORREF rgb_color(int c);

void CRastrWndVect::OnMouseMove(UINT nFlags, CPoint point) 
{
  CDC *m_dc;

  if ((m_dc = BeginPaint()) == NULL) return;

  XDraw(m_dc, m_pt1, m_pt2);  
  m_pt2 = point;
  XDraw(m_dc, m_pt1, m_pt2);  

  EndPaint(m_dc);

  CBgi::OnMouseMove(nFlags, point);
}

static int x_ARC, y_ARC;
static int R_ARC;

void CRastrWndVect::XDraw(CDC* m_dc, CPoint pt1, CPoint pt2) 
{
  int saveROP;
  setline(m_dc, 0, 1);

  if ((m_figura == TIP_LINE || m_figura == TIP_POLYGON) && m_regim == RV_BEGIN1) {
    xline(m_dc, pt1.x, pt1.y, pt2.x, pt2.y);  
  }
  else if ((m_figura == TIP_RING || m_figura == TIP_CIRCLE)  && m_regim == RV_BEGIN1) {
    int R = sqrt(pow(pt2.x-pt1.x, 2)+pow(pt2.y-pt1.y, 2));

    saveROP = m_dc->SetROP2(R2_NOTXORPEN);
    ellipse(m_dc, pt1.x, pt1.y, R, R);
    m_dc->SetROP2(saveROP);
  }

  else if (m_figura == TIP_ARC && m_regim == RV_BEGIN1) {
    int R = sqrt(pow(pt2.x-pt1.x, 2)+pow(pt2.y-pt1.y, 2));

    saveROP = m_dc->SetROP2(R2_NOTXORPEN);
    ellipse(m_dc, pt1.x, pt1.y, R, R);
    m_dc->SetROP2(saveROP);
  }
  else if (m_figura == TIP_ARC && m_regim == RV_BEGIN2) {
    saveROP = m_dc->SetROP2(R2_NOTXORPEN);
    m_dc->Arc(x_ARC-R_ARC, y_ARC-R_ARC, x_ARC+R_ARC, y_ARC+R_ARC, pt1.x, pt1.y, pt2.x, pt2.y);
    m_dc->SetROP2(saveROP);
  }
}

void CRastrWndVect::delObject(CPoint point)
{
  int fill[256];
  list <FRAG>::iterator it;

  for (it = m_frag.begin(); it != m_frag.end(); it++) {
    int i;
    CCoordList::const_iterator it2;

    for (i = 0, it2 = it->cl.begin(); it2 != it->cl.end(); it2++) {
      fill[i++] = it2->x;
      fill[i++] = it2->y;
    }

    CFPoint pt1 = it->cl.first();
    CFPoint pt2 = it->cl.second();

    int R = hypot(pt2.x-pt1.x, pt2.y-pt1.y);
    R = okr3(R, m_dd, true);

    double h = hypot(pt1.x-point.x, pt1.y-point.y);

    switch(it->typ) {
    case TIP_LINE:
      if (it->cl.find2(CFPoint(point.x, point.y), 3, 2)) {
        m_frag.erase(it);
        goto QUIT;
      }
      break;
    case TIP_POLYGON:
      if (it->cl.find3(CFPoint(point.x, point.y))) {
        m_frag.erase(it);
        goto QUIT;
      }
      break;
    case TIP_RING:
      if (fabs(h-R) < 6) {
        m_frag.erase(it);
        goto QUIT;
      }
      break;
    case TIP_CIRCLE:
      if (h < R) {
        m_frag.erase(it);
        goto QUIT;
      }
      break;
    case TIP_ARC:
      h = hypot((pt1.x+pt2.x)/2-point.x, (pt1.y+pt2.y)/2-point.y);
      if (fabs(h-R) < 6) {
        m_frag.erase(it);
        goto QUIT;
      }
      break;
    default:
      break;
    }
  }

QUIT:
  Invalidate();
}

//void CRastrWndVect::delObject(CPoint point)
//{
//}

void CRastrWndVect::OnLButtonDown(UINT nFlags, CPoint point) 
{
  int x = okr3(point.x, m_dd, true), y = okr3(point.y, m_dd, true);
  CDC *m_dc;

  if (m_figura == TIP_DEL) {
    delObject(point);
    return;
  }

  if ((m_dc = BeginPaint()) == NULL) return;

  XDraw(m_dc, m_pt1, m_pt2);  
  XDraw(m_dc, m_pt1, CPoint(x, y));  
  EndPaint(m_dc);

  if (m_figura == TIP_LINE || m_figura == TIP_POLYGON) {
    m_regim = RV_BEGIN1;
    m_pt1 = m_pt2 = CPoint(x, y);
  }
  else if ((m_figura == TIP_RING || m_figura == TIP_CIRCLE) && m_regim == RV_INIT) {
    m_regim = RV_BEGIN1;
    m_pt1 = m_pt2 = CPoint(x, y);
  }
  else if ((m_figura == TIP_RING || m_figura == TIP_CIRCLE) && m_regim == RV_BEGIN1) {
    m_NP.push_back(CFPoint(x, y));
    m_pt2 = CPoint(x, y);
    createFragm();
    m_regim = RV_INIT;
  }
  else if (m_figura == TIP_ARC  && m_regim == RV_INIT) {
    m_regim = RV_BEGIN1;
    m_pt1 = m_pt2 = CPoint(x, y);
  }
  else if (m_figura == TIP_ARC  && m_regim == RV_BEGIN1) {
    int R = sqrt(pow(x-m_pt1.x, 2)+pow(y-m_pt1.y, 2));

    R_ARC = R;
    x_ARC = m_pt1.x;
    y_ARC = m_pt1.y;
    
    m_NP.push_back(CFPoint(m_pt1.x-R, m_pt1.y-R));
    m_NP.push_back(CFPoint(m_pt1.x+R, m_pt1.y+R));
    m_NP.push_back(CFPoint(x, y));

    if ((m_dc = BeginPaint()) != NULL) {
      XDraw(m_dc, m_pt1, CPoint(x, y));  

      m_regim = RV_BEGIN2;

      m_pt1 = m_pt2 = CPoint(x, y);
      
      XDraw(m_dc, m_pt1, CPoint(x, y));  
      
      EndPaint(m_dc);
    }

    m_pt1 = m_pt2 = CPoint(x, y);
  }
  else if (m_figura == TIP_ARC  && m_regim == RV_BEGIN2) {
    m_NP.push_back(CFPoint(x, y));
    m_pt2 = CPoint(x, y);
    createFragm();
    m_regim = RV_INIT;
  }

  if (m_regim != RV_INIT && m_figura != TIP_ARC) {
    m_NP.push_back(CFPoint(x, y));
    m_pt1 = m_pt2 = CPoint(x, y);
  }

  CBgi::OnLButtonDown(nFlags, point);
}

void CRastrWndVect::createFragm()
{
  if (m_regim != RV_INIT) {
    CDC *m_dc;
    if ((m_dc = BeginPaint()) != NULL) {
      XDraw(m_dc, m_pt1, m_pt2);  
      EndPaint(m_dc);
    }
    if (m_figura == TIP_LINE || m_figura == TIP_POLYGON) {
      m_NP.pop_back();
    }

    FRAG frag;
    frag.typ = m_figura;
    frag.cl = m_NP;
    frag.color_bk = m_is_white ? 'w' : 'c';
    m_frag.push_back(frag);

    m_NP.clear();

    m_regim = RV_INIT;

    Invalidate();
  }
}

void CRastrWndVect::setColor(BOOL is_white) 
{
    m_is_white = is_white;
}


void CRastrWndVect::OnLButtonDblClk(UINT nFlags, CPoint point)
{
  createFragm();
  CBgi::OnLButtonDblClk(nFlags,  point);
}

void CRastrWndVect::OnRButtonDown(UINT nFlags, CPoint point) 
{
  CBgi::OnRButtonDown(nFlags, point);
}

void CRastrWndVect::SetRegim(int regim)
{
  m_figura = regim;
  m_regim = RV_INIT;
} 

#define X1 9998000
#define Y1 9996000

void CRastrWndVect::save()
{
  FILE *f;

  CString fn;

  int buf[1024*10];
  int n = 0;

  fn.Format("%s/pics/qq%03d.pic", argpath(), m_typ);

  f = fopen(fn, "wb");

  if (f) {
    list <FRAG>::const_iterator it;

    for (it = m_frag.begin(); it != m_frag.end(); it++) {
      CCoordList::const_iterator it2;

      buf[n++] = 'c';
      buf[n++] = it->color;
      buf[n++] = it->color_bk;

      switch(it->typ) {
      case TIP_LINE:        buf[n++] = 'l';        break;
      case TIP_POLYGON:     buf[n++] = 'f';        break;
      case TIP_RING:        buf[n++] = 'r';        break;
      case TIP_CIRCLE:      buf[n++] = 'k';        break;
      case TIP_ARC:         buf[n++] = 'd';        break;
      default:              buf[n++] = '?';        break;
      }

      switch(it->typ) {
      case TIP_LINE:
      case TIP_POLYGON:
        for (it2 = it->cl.begin(); it2 != it->cl.end(); it2++) {
          double xx = it2->x;
          double yy = it2->y;

          buf[n++] = (it2->x-m_xx*m_dd/2-1*0)/m_dd+X1;
          buf[n++] = -(it2->y-m_yy*m_dd/2-1)/m_dd+Y1;
        }
        break;
      case TIP_RING:  
      case TIP_CIRCLE:
        {
          CFPoint pt1 = it->cl.first();
          CFPoint pt2 = it->cl.second();
          int R = sqrt(pow(pt2.x-pt1.x, 2)+pow(pt2.y-pt1.y, 2))/m_dd;

          buf[n++] = (pt1.x-m_xx*m_dd/2-1*0)/m_dd+X1;
          buf[n++] = -(pt1.y-m_yy*m_dd/2-1)/m_dd+Y1;
          buf[n++] = R;
//          it2++;
        }
      case TIP_ARC:
        {
          it2 = it->cl.begin();

          CFPoint pt1 = it->cl.first();
          CFPoint pt2 = it->cl.second();
//          int R = sqrt(pow(pt2.x-pt1.x, 2)+pow(pt2.y-pt1.y, 2))/m_dd;
          int R = fabs(pt2.x-pt1.x)/2/m_dd;

          buf[n++] = ((pt1.x+pt2.x)/2-m_xx*m_dd/2-1*0)/m_dd+X1;
          buf[n++] = -((pt1.y+pt2.y)/2-m_yy*m_dd/2-1)/m_dd+Y1;
          buf[n++] = R;

          it2++; it2++;

          for (; it2 != it->cl.end(); it2++) {
            buf[n++] = (it2->x-m_xx*m_dd/2-1*0)/m_dd+X1;
            buf[n++] = -(it2->y-m_yy*m_dd/2-1)/m_dd+Y1;
          }

          it2++;
        }
        
        break;
      }

      if (it->typ == TIP_LINE || it->typ == TIP_POLYGON) buf[n++] = 9999;
    }
//    fprintf(f, "'q'\n");
    buf[n++] = 'q';

    fwrite(buf, sizeof(int), n, f); 
    fclose(f);
  }

void init_pics(int n);

  init_pics(m_typ);
}

int CRastrWndVect::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
  int nResult = CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
  return nResult;

}

void CRastrWndVect::PostNcDestroy()
{
//    delete this;
}


bool LiangBar(int XS1, int YS1, int XS2, int YS2, double& x0, double& y0, double& x1, double& y1);

